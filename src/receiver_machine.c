#include "cfdp_core.h"
#include "event.h"
#include "receiver_machine.h"
#include <assert.h>
#include <stdio.h>

void receiver_machine_init(struct receiver_machine *receiver_machine)
{
	receiver_machine->timer.core = receiver_machine->core;
	receiver_machine->timer.transaction_id = receiver_machine->transaction_id;
	receiver_machine->timer.timeout = INACTIVITY_TIMEOUT_IN_SECONDS;

	receiver_machine->condition_code = cfdpConditionCode_no_error;
	receiver_machine->delivery_code = cfdpDeliveryCode_data_incomplete;

	receiver_machine->received_file_size = 0;

	receiver_machine->state = WAIT_FOR_MD;
}

void receiver_machine_close(struct receiver_machine *receiver_machine)
{
	receiver_machine->state = COMPLETED;
	transaction_close_temp_file(&receiver_machine->transaction);
}

void receiver_machine_update_state(struct receiver_machine *receiver_machine,
				   struct event *event, const cfdpCfdpPDU *pdu)
{
	receiver_timer_restart(&receiver_machine->timer);

	if (receiver_machine->state == WAIT_FOR_MD) {
		switch (event->type) {
		case E0_ENTERED_STATE: {
			receiver_machine_init(receiver_machine);
			receiver_timer_restart(&receiver_machine->timer);
			break;
		}
		case E2_ABANDON_TRANSACTION: {
			cfdp_core_abandoned_indication(
			    receiver_machine->core,
			    receiver_machine->transaction_id);
			receiver_machine_close(receiver_machine);
			break;
		}
		case E3_NOTICE_OF_CANCELLATION: {
			cfdp_core_finished_indication(
			    receiver_machine->core,
			    receiver_machine->transaction_id);
			receiver_machine_close(receiver_machine);
			break;
		}
		case E10_RECEIVED_METADATA: {
			cfdp_core_metadata_received_indication(
			    receiver_machine->core,
			    receiver_machine->transaction_id);
			if (transaction_is_file_transfer_in_progress(
				&receiver_machine->transaction)) {
				transaction_open_temp_file(
				    &receiver_machine->transaction);
			}
			receiver_machine->state = WAIT_FOR_EOF;
			break;
		}
		case E12_RECEIVED_EOF_NO_ERROR: {
			cfdp_core_finished_indication(
			    receiver_machine->core,
			    receiver_machine->transaction_id);
			receiver_machine_close(receiver_machine);
			break;
		}
		case E13_RECEIVED_EOF_CANCEL: {
			assert(pdu != NULL);
			receiver_machine->condition_code =
			    pdu->payload.u.file_directive.file_directive_pdu.u
				.eof_pdu.condition_code;

			cfdp_core_finished_indication(
			    receiver_machine->core,
			    receiver_machine->transaction_id);
			receiver_machine_close(receiver_machine);
			break;
		}
		case E27_INACTIVITY_TIMEOUT: {
			cfdp_core_run_fault_handler(
			    receiver_machine->core,
			    receiver_machine->transaction_id,
			    DEFAULT_FAULT_HANDLER_ACTIONS
				[cfdpConditionCode_inactivity_detected]);
			receiver_timer_restart(&receiver_machine->timer);
			break;
		}
		case E33_RECEIVED_CANCEL_REQUEST: {
			receiver_machine->condition_code =
			    cfdpConditionCode_cancel_request_received;
			cfdp_core_issue_request(
			    receiver_machine->core,
			    receiver_machine->transaction_id,
			    E3_NOTICE_OF_CANCELLATION);
			break;
		}
		case E34_RECEIVED_REPORT_REQUEST: {
			cfdp_core_report_indication(
			    receiver_machine->core,
			    receiver_machine->transaction_id);
			break;
		}
		default: {
			printf("Event not support for state WAIT_FOR_MD\n");
		}
		}
	} else if (receiver_machine->state == WAIT_FOR_EOF) {
		switch (event->type) {
		case E2_ABANDON_TRANSACTION: {
			cfdp_core_abandoned_indication(
			    receiver_machine->core,
			    receiver_machine->transaction_id);
			receiver_machine_close(receiver_machine);
			break;
		}
		case E3_NOTICE_OF_CANCELLATION: {
			cfdp_core_finished_indication(
			    receiver_machine->core,
			    receiver_machine->transaction_id);
			receiver_machine_close(receiver_machine);
			break;
		}
		case E11_RECEIVED_FILEDATA: {
			cfdp_core_filesegment_received_indication(
			    receiver_machine->core,
			    receiver_machine->transaction_id);

			if (transaction_is_file_transfer_in_progress(
				&receiver_machine->transaction)) {
				const cfdpFileDataPDU *file_data_pdu =
				    &(pdu->payload.u.file_data.file_data_pdu);
				transaction_store_data_in_temp_file(
				    &receiver_machine->transaction,
				    file_data_pdu);
				if (receiver_machine->received_file_size <
				    file_data_pdu->segment_offset +
					file_data_pdu->file_data.nCount) {
					receiver_machine->received_file_size =
					    file_data_pdu->segment_offset +
					    file_data_pdu->file_data.nCount;
				}
			}
			break;
		}
		case E12_RECEIVED_EOF_NO_ERROR: {
			cfdp_core_eof_received_indication(
			    receiver_machine->core,
			    receiver_machine->transaction_id);

			if (transaction_is_file_transfer_in_progress(
				&receiver_machine->transaction)) {
				if (receiver_machine->received_file_size >
				    pdu->payload.u.file_directive
					.file_directive_pdu.u.eof_pdu
					.file_size) {
					cfdp_core_run_fault_handler(
					    receiver_machine->core,
					    receiver_machine->transaction_id,
					    DEFAULT_FAULT_HANDLER_ACTIONS
						[cfdpConditionCode_file_size_error]);
					break;
				}

				if (transaction_get_temp_file_checksum(
					&receiver_machine->transaction) !=
				    pdu->payload.u.file_directive
					.file_directive_pdu.u.eof_pdu
					.file_checksum) {
					cfdp_core_run_fault_handler(
					    receiver_machine->core,
					    receiver_machine->transaction_id,
					    DEFAULT_FAULT_HANDLER_ACTIONS
						[cfdpConditionCode_file_checksum_failure]);
					break;
				}

				receiver_machine->delivery_code =
				    DeliveryCode_data_complete;
				transaction_copy_temp_file_to_dest_file(
				    &receiver_machine->transaction);
				transaction_close_temp_file(
				    &receiver_machine->transaction);
			}

			cfdp_core_finished_indication(
			    receiver_machine->core,
			    receiver_machine->transaction_id);
			receiver_machine_close(receiver_machine);
			break;
		}
		case E13_RECEIVED_EOF_CANCEL: {
			assert(pdu != NULL);
			receiver_machine->condition_code =
			    pdu->payload.u.file_directive.file_directive_pdu.u
				.eof_pdu.condition_code;

			cfdp_core_finished_indication(
			    receiver_machine->core,
			    receiver_machine->transaction_id);
			receiver_machine_close(receiver_machine);
			break;
		}
		case E27_INACTIVITY_TIMEOUT: {
			cfdp_core_run_fault_handler(
			    receiver_machine->core,
			    receiver_machine->transaction_id,
			    DEFAULT_FAULT_HANDLER_ACTIONS
				[cfdpConditionCode_inactivity_detected]);
			receiver_timer_restart(&receiver_machine->timer);
			break;
		}
		case E33_RECEIVED_CANCEL_REQUEST: {
			receiver_machine->condition_code =
			    cfdpConditionCode_cancel_request_received;
			cfdp_core_issue_request(
			    receiver_machine->core,
			    receiver_machine->transaction_id,
			    E3_NOTICE_OF_CANCELLATION);
			break;
		}
		case E34_RECEIVED_REPORT_REQUEST: {
			cfdp_core_report_indication(
			    receiver_machine->core,
			    receiver_machine->transaction_id);
			break;
		}
		default: {
			printf("Event not support for state WAIT_FOR_EOF\n");
		}
		}
	}
}