//
// Shark control test program
//
// Patrick Walton <pcwalton@mimiga.net>
//

#include "sharkctl.h"
#include <stdio.h>

int fact(int n) {
    if (n == 1)
        return 1;
    return n + fact(n - 1);
}

int main() {
    mach_port_t shark_port = sharkctl_create_port();
    printf("port: %x\n", shark_port);
    mach_msg_return_t result = sharkctl_connect(shark_port);
    printf("connect: %x\n", result);
    result = sharkctl_start(shark_port, 0xdeadbeef);
    printf("start: %x\n", result);

    int i;
    for (i = 0; i < 100000; i++)
        fact(1000);

    result = sharkctl_stop(shark_port);
    printf("stop: %x\n", result);
    result = sharkctl_disconnect(shark_port);
    printf("disconnect: %x\n", result);

    return 0;
}

