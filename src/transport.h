#ifndef TRANSPORT_H
#define TRANSPORT_H

#include "stdbool.h"
#include "stdint.h"

struct transport {
	void (*transport_send_pdu)(const uint64_t destination_entity_id, const byte pdu[], const int size);
	bool (*transport_is_ready)();
};

#endif