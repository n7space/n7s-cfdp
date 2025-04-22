#include "cfdp_core.h"
#include "event.h"
#include <stdio.h>

static bool cfdp_core_is_request_to_sender(struct cfdp_core *core,
					   struct transaction_id transaction_id)
{
	return transaction_id.source_entity_id ==
		   core->sender[0].transaction_id.source_entity_id &&
	       transaction_id.seq_number ==
		   core->sender[0].transaction_id.seq_number;
}

static bool
cfdp_core_is_request_to_receiver(struct cfdp_core *core,
				 struct transaction_id transaction_id)
{
	return transaction_id.source_entity_id ==
		   core->receiver[0].transaction_id.source_entity_id &&
	       transaction_id.seq_number ==
		   core->receiver[0].transaction_id.seq_number;
}

static void cfdp_core_issue_request(struct cfdp_core *core,
				    struct transaction_id transaction_id,
				    enum EventType event_type)
{
	struct event event;

	if (cfdp_core_is_request_to_sender(core, transaction_id)) {
		event.transaction = core->sender[0].transaction;
		event.type = event_type;
		sender_machine_update_state(&core->sender[0], &event);
	} else if (cfdp_core_is_request_to_receiver(core, transaction_id)) {
		event.transaction = core->receiver[0].transaction;
		event.type = event_type;
		receiver_machine_update_state(&core->receiver[0], &event);
	} else {
		// TODO handle transaction id not present
	}
}

// CFDP service requests

struct transaction_id
cfdp_core_put(struct cfdp_core *core, uint32_t destination_entity_id,
	      char source_filename[MAX_FILE_NAME_SIZE],
	      char destination_filename[MAX_FILE_NAME_SIZE])
{
	core->transaction_sequence_number++;

	struct transaction transaction = {
	    .kernel = core,
	    .source_entity_id = core->entity_id,
	    .seq_number = core->transaction_sequence_number,
	    .destination_entity_id = destination_entity_id,
	};

	strncpy(transaction.source_filename, source_filename,
		MAX_FILE_NAME_SIZE);
	transaction.source_filename[MAX_FILE_NAME_SIZE - 1] = '\0';

	strncpy(transaction.destination_filename, destination_filename,
		MAX_FILE_NAME_SIZE);
	transaction.destination_filename[MAX_FILE_NAME_SIZE - 1] = '\0';

	if (core->sender[0].state != COMPLETED) {
		// TODO handle sender busy
	}
	core->sender[0].state = SEND_METADATA;

	struct event event = {.transaction = transaction,
			      .type = E0_ENTERED_STATE};
	sender_machine_update_state(&core->sender[0], &event);

	event.type = E30_RECEIVED_PUT_REQUEST;
	sender_machine_update_state(&core->sender[0], &event);

	struct transaction_id transaction_id = {
	    .source_entity_id = core->entity_id,
	    .seq_number = core->transaction_sequence_number};

	return transaction_id;
}

void cfdp_core_cancel(struct cfdp_core *core,
		      struct transaction_id transaction_id)
{
	cfdp_core_issue_request(core, transaction_id,
				E33_RECEIVED_CANCEL_REQUEST);
}

void cfdp_core_suspend(struct cfdp_core *core,
		       struct transaction_id transaction_id)
{
	cfdp_core_issue_request(core, transaction_id,
				E31_RECEIVED_SUSPEND_REQUEST);
}

void cfdp_core_resume(struct cfdp_core *core,
		      struct transaction_id transaction_id)
{
	cfdp_core_issue_request(core, transaction_id,
				E32_RECEIVED_RESUME_REQUEST);
}

void cfdp_core_report(struct cfdp_core *core,
		      struct transaction_id transaction_id)
{
	cfdp_core_issue_request(core, transaction_id,
				E34_RECEIVED_REPORT_REQUEST);
}