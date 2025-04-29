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

static void cfdp_core_issue_link_state_procedure(struct cfdp_core *core,
						 uint32_t destination_entity_id,
						 enum EventType event_type)
{
	struct event event;

	if (core->sender[0].transaction.destination_entity_id ==
	    destination_entity_id) {
		event.transaction = core->sender[0].transaction;
		event.type = event_type;
		sender_machine_update_state(&core->sender[0], &event);
	}
}

void cfdp_core_issue_request(struct cfdp_core *core,
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
		receiver_machine_update_state(&core->receiver[0], &event, NULL);
	} else {
		// TODO handle transaction id not present
	}
}

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

// TODO implement indication mechanism
void cfdp_core_transaction_indication(struct cfdp_core *core,
				      struct transaction_id transaction_id)
{
	printf("cfdp transaction indication source_entity_id = %d seq_number = "
	       "%d\n",
	       transaction_id.source_entity_id, transaction_id.seq_number);
}

void cfdp_core_eof_sent_indication(struct cfdp_core *core,
				   struct transaction_id transaction_id)
{
	printf(
	    "cfdp eof sent indication source_entity_id = %d seq_number = %d\n",
	    transaction_id.source_entity_id, transaction_id.seq_number);
}

void cfdp_core_finished_indication(struct cfdp_core *core,
				   struct transaction_id transaction_id)
{
	printf(
	    "cfdp finished indication source_entity_id = %d seq_number = %d\n",
	    transaction_id.source_entity_id, transaction_id.seq_number);
}

void cfdp_core_report_indication(struct cfdp_core *core,
				 struct transaction_id transaction_id)
{
	printf("cfdp report indication source_entity_id = %d seq_number = %d\n",
	       transaction_id.source_entity_id, transaction_id.seq_number);
}

void cfdp_core_eof_received_indication(struct cfdp_core *core,
				       struct transaction_id transaction_id)
{
	printf("cfdp eof received indication source_entity_id = %d seq_number "
	       "= %d\n",
	       transaction_id.source_entity_id, transaction_id.seq_number);
}

void cfdp_core_metadata_received_indication(
    struct cfdp_core *core, struct transaction_id transaction_id)
{
	printf("cfdp metadata received indication source_entity_id = %d "
	       "seq_number = %d\n",
	       transaction_id.source_entity_id, transaction_id.seq_number);
}

void cfdp_core_filesegment_received_indication(
    struct cfdp_core *core, struct transaction_id transaction_id)
{
	printf("cfdp filesegment received indication source_entity_id = %d "
	       "seq_number = %d\n",
	       transaction_id.source_entity_id, transaction_id.seq_number);
}

void cfdp_core_abandoned_indication(struct cfdp_core *core,
				    struct transaction_id transaction_id)
{
	printf(
	    "cfdp abandoned indication source_entity_id = %d seq_number = %d\n",
	    transaction_id.source_entity_id, transaction_id.seq_number);
}

void cfdp_core_suspended_indication(struct cfdp_core *core,
				    struct transaction_id transaction_id)
{
	printf(
	    "cfdp suspended indication source_entity_id = %d seq_number = %d\n",
	    transaction_id.source_entity_id, transaction_id.seq_number);
}

void cfdp_core_resumed_indication(struct cfdp_core *core,
				  struct transaction_id transaction_id)
{
	printf(
	    "cfdp resumed indication source_entity_id = %d seq_number = %d\n",
	    transaction_id.source_entity_id, transaction_id.seq_number);
}

void cfdp_core_fault_indication(struct cfdp_core *core,
				struct transaction_id transaction_id)
{
	printf("cfdp fault indication source_entity_id = %d seq_number = %d\n",
	       transaction_id.source_entity_id, transaction_id.seq_number);
}

void cfdp_core_freeze(struct cfdp_core *core, uint32_t destination_entity_id)
{
	cfdp_core_issue_link_state_procedure(core, destination_entity_id,
					     E40_RECEIVED_FREEZE);
}

void cfdp_core_thaw(struct cfdp_core *core, uint32_t destination_entity_id)
{
	cfdp_core_issue_link_state_procedure(core, destination_entity_id,
					     E41_RECEIVED_THAW);
}

static uint64_t bytes_to_ulong(const byte *data, int size)
{
	uint64_t result;
	for (int i = 0; i < size && i < sizeof(uint64_t); i++) {
		result <<= 8;
		result |= data[i];
	}
	return result;
}

static struct event create_event_for_delivery(struct cfdp_core *core,
					      const cfdpCfdpPDU *pdu)
{
	enum EventType type;

	if (pdu->payload.kind == PayloadData_file_data_PRESENT) {
		type = E11_RECEIVED_FILEDATA;
	} else {
		switch (pdu->payload.u.file_directive.file_directive_pdu.kind) {
		case FileDirectivePDU_eof_pdu_PRESENT:
			if (pdu->payload.u.file_directive.file_directive_pdu.u
				.eof_pdu.condition_code ==
			    cfdpConditionCode_no_error) {
				type = E12_RECEIVED_EOF_NO_ERROR;
			} else {
				type = E13_RECEIVED_EOF_CANCEL;
			}
			break;
		case FileDirectivePDU_finished_pdu_PRESENT:
			if (pdu->payload.u.file_directive.file_directive_pdu.u
				.finished_pdu.condition_code ==
			    cfdpConditionCode_no_error) {
				type = E16_RECEIVED_FINISHED_NO_ERROR;
			} else {
				type = E17_RECEIVED_FINISHED_CANCEL;
			}
			break;
		case FileDirectivePDU_ack_pdu_PRESENT:
			if (pdu->payload.u.file_directive.file_directive_pdu.u
				.ack_pdu.directive_code_of_ack_pdu ==
			    DIRECTIVE_CODE_EOF) {
				type = E14_RECEIVED_ACK_EOF;
			} else if (pdu->payload.u.file_directive
				       .file_directive_pdu.u.ack_pdu
				       .directive_code_of_ack_pdu ==
				   DIRECTIVE_CODE_FINISHED) {
				type = E18_RECEIVED_ACK_FINISHED;
			} else {
				// not implemented
			}
			break;
		case FileDirectivePDU_metadata_pdu_PRESENT:
			type = E10_RECEIVED_METADATA;
			break;
		default:
			// not implemented
			break;
		}
	}

	struct event event;
	event.transaction = core->sender[0].transaction;
	event.type = type;

	return event;
}

static void deliver_pdu_to_sender_machine(struct cfdp_core *core,
					  const cfdpCfdpPDU *pdu)
{
	struct event event = create_event_for_delivery(core, pdu);
	sender_machine_update_state(&core->sender[0], &event);
}

static void deliver_pdu_to_receiver_machine(struct cfdp_core *core,
					    const cfdpCfdpPDU *pdu)
{
	struct event event = create_event_for_delivery(core, pdu);
	receiver_machine_update_state(&core->receiver[0], &event, pdu);
}

static void handle_pdu_to_new_receiver_machine(struct cfdp_core *core,
					       const cfdpCfdpPDU *pdu)
{
	if (pdu->pdu_header.direction == cfdpDirection_toward_sender) {
		printf("pdu in towards sender in unacknowledged mode not "
		       "supported, pdu droped\n");
		return;
	}

	if (!(pdu->payload.kind == PayloadData_file_directive_PRESENT &&
	      pdu->payload.u.file_directive.file_directive_pdu.kind ==
		  FileDirectivePDU_metadata_pdu_PRESENT) ||
	    !(pdu->payload.kind == PayloadData_file_directive_PRESENT &&
	      pdu->payload.u.file_directive.file_directive_pdu.kind ==
		  FileDirectivePDU_eof_pdu_PRESENT) ||
	    !(pdu->payload.kind == PayloadData_file_data_PRESENT)) {
		printf("unsupported pdu, pdu droped\n");
		return;
	}

	struct transaction transaction;
	transaction.kernel = core;
	transaction.source_entity_id =
	    bytes_to_ulong(pdu->pdu_header.source_entity_id.arr,
			   pdu->pdu_header.source_entity_id.nCount);
	transaction.seq_number =
	    bytes_to_ulong(pdu->pdu_header.transaction_sequence_number.arr,
			   pdu->pdu_header.transaction_sequence_number.nCount);
	transaction.destination_entity_id =
	    bytes_to_ulong(pdu->pdu_header.destination_entity_id.arr,
			   pdu->pdu_header.destination_entity_id.nCount);

	if (pdu->payload.kind == PayloadData_file_directive_PRESENT &&
	    pdu->payload.u.file_directive.file_directive_pdu.kind ==
		FileDirectivePDU_metadata_pdu_PRESENT) {
		strncpy(transaction.source_filename,
			pdu->payload.u.file_directive.file_directive_pdu.u
			    .metadata_pdu.source_file_name.arr,
			MAX_FILE_NAME_SIZE);
		transaction.source_filename[MAX_FILE_NAME_SIZE - 1] = '\0';

		strncpy(transaction.destination_filename,
			pdu->payload.u.file_directive.file_directive_pdu.u
			    .metadata_pdu.destination_file_name.arr,
			MAX_FILE_NAME_SIZE);
		transaction.destination_filename[MAX_FILE_NAME_SIZE - 1] = '\0';
	}

	struct event event = {.transaction = transaction,
			      .type = E0_ENTERED_STATE};
	receiver_machine_update_state(&core->receiver[0], &event, pdu);
	deliver_pdu_to_receiver_machine(core, pdu);
}

void cfdp_core_received_pdu(struct cfdp_core *core, unsigned char *buf,
			    long count)
{
	BitStream bit_stream;
	BitStream_AttachBuffer(&bit_stream, buf, count);

	cfdpCfdpPDU pdu;
	int error_code = 0;
	if (!cfdpCfdpPDU_ACN_Decode(&pdu, &bit_stream, &error_code)) {
		printf("cannot decode CfdpPDU, error code = %d\n", error_code);
		return;
	}

	if (pdu.pdu_header.transmission_mode == TransmissionMode_acknowledged) {
		printf("pdu in acknowledged mode not supported, pdu droped\n");
		return;
	}

	struct transaction_id transaction_id;
	transaction_id.source_entity_id =
	    bytes_to_ulong(pdu.pdu_header.source_entity_id.arr,
			   pdu.pdu_header.source_entity_id.nCount);
	transaction_id.seq_number =
	    bytes_to_ulong(pdu.pdu_header.transaction_sequence_number.arr,
			   pdu.pdu_header.transaction_sequence_number.nCount);

	if (cfdp_core_is_request_to_sender(core, transaction_id)) {
		if (core->sender[0].state != COMPLETED) {
			deliver_pdu_to_sender_machine(core, &pdu);
			return;
		}
	}

	if (cfdp_core_is_request_to_receiver(core, transaction_id)) {
		if (core->sender[0].state != COMPLETED) {
			deliver_pdu_to_receiver_machine(core, &pdu);
			return;
		}
	}

	handle_pdu_to_new_receiver_machine(core, &pdu);
}

void cfdp_core_run_fault_handler(struct cfdp_core *core,
				 const struct transaction_id transaction_id,
				 const enum FaultHandlerAction action)
{
	switch (action) {
	case FAULT_HANDLER_CANCEL: {
		cfdp_core_issue_request(core, transaction_id,
					E33_RECEIVED_CANCEL_REQUEST);
		break;
	}
	case FAULT_HANDLER_SUSPEND: {
		cfdp_core_issue_request(core, transaction_id,
					E31_RECEIVED_SUSPEND_REQUEST);
		break;
	}
	case FAULT_HANDLER_IGNORE: {
		break;
	}
	case FAULT_HANDLER_ABANDON: {
		cfdp_core_issue_request(core, transaction_id,
					E2_ABANDON_TRANSACTION);
		break;
	}
	default: {
		printf("condition code not supportet in fault handler\n");
	}
	}
}