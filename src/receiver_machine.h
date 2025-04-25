#ifndef RECEIVER_MACHINE_H
#define RECEIVER_MACHINE_H

#include "stdbool.h"
#include "stdint.h"

#include "constants.h"
#include "transaction.h"
#include "transaction_id.h"
#include "dataview.h"

struct event;

struct receiver_machine {
	struct cfdp_core *core;
	struct transaction_id transaction_id;
	struct transaction transaction;

	cfdpConditionCode condition_code;
	cfdpDeliveryCode delivery_code;
	cfdpFileStatus file_status;
	bool is_frozen;
	bool metadata_received;
	bool is_suspended;
	
	uint32_t received_file_size;
	bool is_file_open;

	enum MachineState state;
};

void receiver_machine_init(struct receiver_machine *receiver_machine);
void receiver_machine_close(struct receiver_machine *receiver_machine);

void receiver_machine_update_state(struct receiver_machine *receiver_machine,
				   struct event *event, const cfdpCfdpPDU *pdu);

#endif