#ifndef TEST_TRANSPORT_H
#define TEST_TRANSPORT_H

#include "cfdp_core.h"
#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT 5111
#define RECEIVER_PORT 5222
#define BUFFER_SIZE 8192

void *test_transport_receiver_thread(void *arg);

void test_transport_init_and_bind(struct cfdp_core *core);

void test_transport_close();

void test_transport_send_pdu(void *user_data, const byte pdu[], const int size);

bool test_transport_is_ready(void *user_data);

#endif