#include "/Users/shlokjain/CLionProjects/mini-network-proxy/include/proxy.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <fcntl.h>

#define MAX_CLIENTS  FD_SETSIZE
#define BACKLOG      10
#define GREETING     "Hello from proxy server!\n"

void set_nonblocking(const int fd) {
    const int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl F_GETFL");
        exit(1);
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl F_SETFL O_NONBLOCK");
        exit(1);
    }
}

void start_tcp_proxy(int port) {
    struct addrinfo hints, *res;
    char port_str[6];
    snprintf(port_str, sizeof(port_str), "%d", port);

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    const int status = getaddrinfo(NULL, port_str, &hints, &res);
    if (status != 0) {
        perror("getaddrinfo");
        exit(1);
    }

    const int listen_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (listen_fd == -1) {
        perror("socket");
        exit(1);
    }

    set_nonblocking(listen_fd);

    if (bind(listen_fd, res->ai_addr, res->ai_addrlen) < 0) {
        perror("bind");
        close(listen_fd);
        exit(1);
    }

    if (listen(listen_fd, BACKLOG) < 0) {
        perror("listen");
        close(listen_fd);
        exit(1);
    }

    printf("Non-blocking TCP server listening on port %d\n", port);

    freeaddrinfo(res);

    fd_set master_set, read_set;
    FD_ZERO(&master_set);
    FD_SET(listen_fd, &master_set); // master_set keeps track of all FDs we care about
    int max_fd = listen_fd;

    int client_fds[MAX_CLIENTS]; // keeps track of all currently connected client sockets
    memset(client_fds, -1, sizeof(client_fds));

    while (1) {
        read_set = master_set;  // select() modifies the set, so copy

        const int ready = select(max_fd + 1, &read_set, NULL, NULL, NULL);
        if (ready < 0) {
            if (errno == EINTR) continue;  // interrupted by signal
            perror("select");
            break;
        }

        // Check if new client is connecting
        if (FD_ISSET(listen_fd, &read_set)) {
            while (1) {
                struct sockaddr_storage client_addr;
                socklen_t client_len = sizeof(client_addr);
                int client_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &client_len);

                if (client_fd == -1) {
                    if (errno == EAGAIN || errno == EWOULDBLOCK) break;
                    perror("accept");
                    break;
                }

                printf("Accepted new client: FD %d\n", client_fd);
                set_nonblocking(client_fd);

                // track the new client
                for (int i = 0; i < MAX_CLIENTS; i++) {
                    if (client_fds[i] == -1) {
                        client_fds[i] = client_fd;
                        break;
                    }
                }

                FD_SET(client_fd, &master_set);
                if (client_fd > max_fd) max_fd = client_fd;
            }
        }

        // Check existing clients for readiness
        for (int i = 0; i < MAX_CLIENTS; i++) {
            int fd = client_fds[i];
            if (fd == -1) continue;

            if (FD_ISSET(fd, &read_set)) {
                // Instead of reading, we just send a greeting and close
                ssize_t sent = send(fd, GREETING, strlen(GREETING), 0);
                if (sent == -1) {
                    perror("send");
                } else {
                    printf("Sent greeting to FD %d\n", fd);
                }
                close(fd);
                FD_CLR(fd, &master_set);
                client_fds[i] = -1;
            }
        }
    }

    close(listen_fd);
}
