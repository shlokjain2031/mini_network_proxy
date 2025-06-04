#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "/Users/shlokjain/CLionProjects/mini-network-proxy/include/proxy.h"

#define DEFAULT_PORT 3001

int main(int argc, char *argv[]) {
    setbuf(stdout, NULL);  // Disable stdout buffering

    int port = DEFAULT_PORT;

    if (argc >= 2) {
        port = atoi(argv[1]);
    }

    printf("Starting proxy on port %d...\n", port);

    start_tcp_proxy(port);

    return 0;
}
