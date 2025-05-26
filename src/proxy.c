#include "/Users/shlokjain/CLionProjects/mini-network-proxy/include/proxy.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>

const int BACKLOG = 10;

void start_tcp_proxy(int port) {
    struct addrinfo hints, *res;

    // Clear addr uninit memory
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    char port_str[6];  // enough to hold port number max 65535 + null terminator
    snprintf(port_str, sizeof(port_str), "%d", port);
    const int status = getaddrinfo(NULL, port_str, &hints, &res);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        exit(1);
    }

    const int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol); // Initiate a stream socket
    if (sockfd == -1) {
        perror("socket");
        exit(1);
    }
    printf("sockfd: %d\n", sockfd);

    if (bind(sockfd, res->ai_addr, res->ai_addrlen) < 0) {
        perror("bind");
        close(sockfd);
        exit(1);
    }

    if ((listen(sockfd, BACKLOG)) == -1) {
        perror("listen");
        close(sockfd);
        exit(1);
    }

    printf("Proxy listening on port %d...\n", port);

    while (1) {
        struct sockaddr_storage client_addr;
        socklen_t client_len = sizeof(client_addr);

        const int client_fd = accept(sockfd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd == -1) {
            perror("accept");
            if (errno == EINTR) {
                // Interrupted by signal, try again
                continue;
            }
            printf("Exiting accept loop due to accept error.\n");
            break;
        }

        printf("Accepted connection, client FD %d\n", client_fd);
        fflush(stdout);  // Ensure it prints immediately

        const char *welcome = "Hello from proxy server!\n";
        send(client_fd, welcome, strlen(welcome), 0);  // ✅ send something

        // handle_connection(client_fd);
        close(client_fd);
    }
    close(sockfd);
}
