#ifndef TEST_TRANSPORT_H
#define TEST_TRANSPORT_H

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define PORT 5111
#define BUFFER_SIZE 1024
const char IP_ADDRESS[] = "127.0.0.1";

int sockfd;

void *test_transport_receiver_thread(void *arg) {
    char buffer[BUFFER_SIZE];
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    while (1) {
        int n = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0,
                         (struct sockaddr *)&client_addr, &addr_len);
        if (n > 0) {
            buffer[n] = '\0';
            printf("[Thread] Received: %s\n", buffer);
        } else {
            perror("recvfrom failed");
            continue;
        }

        struct cfdp_core *core = arg;
        cfdp_core_received_pdu(core, buffer, n);
    }

    return NULL;
}

void test_transport_init_and_bind(struct cfdp_core *core)
{
    struct sockaddr_in server_addr;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(IP_ADDRESS);
    server_addr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    pthread_t thread_id;
    if (pthread_create(&thread_id, NULL, test_transport_receiver_thread, (void *)core) != 0) {
        perror("pthread_create failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
}

void test_transport_send_pdu(const byte pdu[], const int size)
{
    send(sockfd, pdu, size, 0);
}

bool test_transport_is_ready()
{
    return true;
}

#endif