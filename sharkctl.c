//
// Shark control
//
// Patrick Walton <pcwalton@mimiga.net>
//

#include "sharkctl.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SHARK_MSG_ACQUIRE   0x29a
#define SHARK_MSG_RELEASE   0x29b
#define SHARK_MSG_STOP      0x29c
#define SHARK_MSG_START     0x29d

#define RECV_SIZEOF(ty) offsetof(ty, out)

// Private API in libSystem.dylib
extern void bootstrap_look_up(mach_port_t special_port, const char *name,
                              mach_port_t *dest_port);

struct chud_client_acquire_msg {
    mach_msg_header_t hdr;
    uint32_t unk0;          // always 0
    uint32_t unk1;          // always 1
    uint32_t pid;
    uint32_t out[2];
};

struct chud_client_start_msg {
    mach_msg_header_t hdr;
    uint32_t unk0;          // always 1
    uint32_t name0;
    uint32_t arg2;          // always 6
    uint8_t unk1;           // always 0
    uint8_t unk2;           // always 1
    uint8_t unk3;           // uninitialized
    uint8_t unk4;           // always 1
    uint32_t unk5;          // always 0
    uint32_t unk6;          // always 1
    uint32_t name1;         // same as name0
};

struct chud_client_stop_msg {
    mach_msg_header_t hdr;
    uint32_t out[5];
};

struct chud_client_release_msg {
    mach_msg_header_t hdr;
    uint32_t unk0;          // always 0
    uint32_t unk1;          // always 1
    uint32_t pid;
    uint32_t out[2];
};

mach_port_t sharkctl_create_port(void) {
    mach_port_t bootstrap_port, shark_port = 0;
    task_get_special_port(mach_task_self(), TASK_BOOTSTRAP_PORT,
                          &bootstrap_port);
    bootstrap_look_up(bootstrap_port, "CHUD_IPC", &shark_port);
    return shark_port;
}

mach_msg_return_t sharkctl_connect(mach_port_t shark_port) {
    mach_port_t reply_port = mig_get_reply_port();

    struct chud_client_acquire_msg msg;
    msg.hdr.msgh_bits = 0x1513;
    msg.hdr.msgh_size = sizeof(mach_msg_header_t);
    msg.hdr.msgh_remote_port = shark_port;
    msg.hdr.msgh_local_port = reply_port;
    msg.hdr.msgh_reserved = 0;
    msg.hdr.msgh_id = SHARK_MSG_ACQUIRE;
    msg.unk0 = 0;
    msg.unk1 = 1;
    msg.pid = getpid();

    assert(RECV_SIZEOF(struct chud_client_acquire_msg) == 0x24);
    assert(sizeof(msg) == 0x2c);
    mach_msg_return_t result = mach_msg(&msg.hdr, MACH_SEND_MSG | MACH_RCV_MSG,
                                        RECV_SIZEOF(struct
                                            chud_client_acquire_msg),
                                        sizeof(msg), reply_port, 0, 0);

    mig_dealloc_reply_port(reply_port);
    return result;
}

mach_msg_return_t sharkctl_start(mach_port_t shark_port, uint32_t name) {
    mach_port_t reply_port = mig_get_reply_port();

    struct chud_client_start_msg msg;
    msg.hdr.msgh_bits = 0x80001513;
    msg.hdr.msgh_size = sizeof(mach_msg_header_t);
    msg.hdr.msgh_remote_port = shark_port;
    msg.hdr.msgh_local_port = reply_port;
    msg.hdr.msgh_reserved = 0;
    msg.hdr.msgh_id = SHARK_MSG_START;
    msg.unk0 = 1;
    msg.name0 = name;
    msg.arg2 = 6;
    msg.unk1 = 0;
    msg.unk2 = 1;
    msg.unk3 = 0;
    msg.unk4 = 1;
    msg.unk5 = 0;
    msg.unk6 = 1;
    msg.name1 = name;

    assert(sizeof(msg) == 0x34);
    mach_msg_return_t result = mach_msg(&msg.hdr, MACH_SEND_MSG | MACH_RCV_MSG,
                                        sizeof(msg), 0x30, reply_port, 0, 0);

    mig_dealloc_reply_port(reply_port);
    return result;
}

mach_msg_return_t sharkctl_stop(mach_port_t shark_port) {
    mach_port_t reply_port = mig_get_reply_port();

    struct chud_client_stop_msg msg;
    msg.hdr.msgh_bits = 0x1513;
    msg.hdr.msgh_size = sizeof(mach_msg_header_t);
    msg.hdr.msgh_remote_port = shark_port;
    msg.hdr.msgh_local_port = reply_port;
    msg.hdr.msgh_reserved = 0;
    msg.hdr.msgh_id = SHARK_MSG_STOP;

    assert(RECV_SIZEOF(struct chud_client_stop_msg) == 0x18);
    assert(sizeof(msg) == 0x2c);
    mach_msg_return_t result = mach_msg(&msg.hdr, MACH_SEND_MSG | MACH_RCV_MSG,
                                        RECV_SIZEOF(struct
                                            chud_client_stop_msg),
                                        sizeof(msg), reply_port, 0, 0);

    mig_dealloc_reply_port(reply_port);
    return result;
}

mach_msg_return_t sharkctl_disconnect(mach_port_t shark_port) {
    mach_port_t reply_port = mig_get_reply_port();

    struct chud_client_release_msg msg;
    msg.hdr.msgh_bits = 0x1513;
    msg.hdr.msgh_size = sizeof(mach_msg_header_t);
    msg.hdr.msgh_remote_port = shark_port;
    msg.hdr.msgh_local_port = reply_port;
    msg.hdr.msgh_reserved = 0;
    msg.hdr.msgh_id = SHARK_MSG_RELEASE;
    msg.unk0 = 0;
    msg.unk1 = 1;
    msg.pid = getpid();

    assert(RECV_SIZEOF(struct chud_client_release_msg) == 0x24);
    assert(sizeof(msg) == 0x2c);
    mach_msg_return_t result = mach_msg(&msg.hdr, MACH_SEND_MSG | MACH_RCV_MSG,
                                        RECV_SIZEOF(struct
                                            chud_client_release_msg),
                                        sizeof(msg), reply_port, 0, 0);

    mig_dealloc_reply_port(reply_port);
    return result;
}

