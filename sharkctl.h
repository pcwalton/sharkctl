//
// Shark control
//
// Patrick Walton <pcwalton@mimiga.net>
//

#ifndef SHARKCTL_H
#define SHARKCTL_H

#include <mach/mach.h>
#include <stdint.h>

mach_port_t sharkctl_create_port(void);
mach_msg_return_t sharkctl_connect(mach_port_t shark_port);
mach_msg_return_t sharkctl_start(mach_port_t shark_port, uint32_t name);
mach_msg_return_t sharkctl_stop(mach_port_t shark_port);
mach_msg_return_t sharkctl_disconnect(mach_port_t shark_port);

#endif

