#ifndef TRANSPORT_H
#define TRANSPORT_H

#include "stdint.h"
#include "stdbool.h"

typedef struct
{
} transport;

void transport_request(transport *transport, uint32_t ut_address, uint32_t *ut_sdu);
void transport_indication(transport *transport, uint32_t ut_address, uint32_t *ut_sdu);

#endif