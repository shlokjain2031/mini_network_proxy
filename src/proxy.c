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
#include <pthread.h>

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

void *client_handler(void *arg) {
    const int client_fd = *(int *)arg;
    free(arg);

    // Get the thread ID
    const pthread_t tid = pthread_self();

    // Create a unique greeting message
    char buffer[100];
    snprintf(buffer, sizeof(buffer), "Hello from proxy server! (Thread ID: %lu)\n", (unsigned long)tid);

    const ssize_t sent = send(client_fd, buffer, strlen(buffer), 0);
    if (sent == -1) perror("send");
    else printf("Sent greeting from thread %lu to client FD %d\n", (unsigned long)tid, client_fd);

    close(client_fd);
    return NULL;
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

    while (1) {
        struct sockaddr_storage client_addr;
        socklen_t client_len = sizeof(client_addr);
        const int client_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &client_len);

        if (client_fd == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                usleep(10000);  // Sleep 10ms to avoid busy loop
                continue;
            }
            perror("accept");
            continue;
        }

        printf("Accepted new client: FD %d\n", client_fd);

        int *pclient_fd = malloc(sizeof(int));
        if (!pclient_fd) {
            perror("malloc"); close(client_fd);
            continue;
        }
        *pclient_fd = client_fd;

        pthread_t tid;
        if (pthread_create(&tid, NULL, client_handler, pclient_fd) != 0) {
            perror("pthread_create");
            close(client_fd);
            free(pclient_fd);
            continue;
        }
        pthread_detach(tid);  // No need to join
    }

    close(listen_fd);
}
