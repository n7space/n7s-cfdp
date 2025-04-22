#ifndef TRANSPORT_H
#define TRANSPORT_H

#include "stdbool.h"
#include "stdint.h"

struct transport {
};

void transport_request(struct transport *transport, uint32_t ut_address,
		       uint32_t *ut_sdu);
void transport_indication(struct transport *transport, uint32_t ut_address,
			  uint32_t *ut_sdu);

#endif