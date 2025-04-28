#include "sender_machine.h"
#include "dataview.h"
#include "cfdp_core.h"
#include "event.h"

#include <string.h>
#include <stdio.h>

static void ulong_to_bytes(uint64_t data, byte *result, int size)
{
	size = 0;
	uint64_t eval_data = data;

	while(eval_data != 0){
		size++;
		eval_data >>= 8;
	}

	for (size_t i = 0; i < size; i++) {
        result[size - 1 - i] = (uint8_t)(data & 0xFF);
        data >>= 8;
    }
}

void sender_machine_init(struct sender_machine *sender_machine){
	sender_machine->condition_code = cfdpConditionCode_no_error;
	sender_machine->is_frozen = false;
	sender_machine->is_suspended = false;

	sender_machine->state = SEND_METADATA;
}

void sender_machine_close(struct sender_machine *sender_machine){
	sender_machine->state = COMPLETED;
	transaction_close_file(&sender_machine->transaction);
}

void sender_machine_send_metadata(struct sender_machine *sender_machine){
	cfdpCfdpPDU pdu;
	cfdpPDUHeader header;
	cfdpMetadataPDU metadata_pdu;

	header.direction = cfdpDirection_toward_receiver;
	header.transmission_mode = cfdpTransmissionMode_unacknowledged;
	header.crc_flag = cfdpCRCFlag_crc_not_present;
	header.large_file_flag = 0;
	header.segmentation_control = cfdpSegmentationControl_record_boundries_not_preserved;
	header.segment_metadata_flag = cfdpSegmentMetadataFlag_flag_not_present;
	
	ulong_to_bytes(sender_machine->transaction.source_entity_id, header.source_entity_id.arr, header.source_entity_id.nCount);
	ulong_to_bytes(sender_machine->transaction.destination_entity_id, header.destination_entity_id.arr, header.destination_entity_id.nCount);
	ulong_to_bytes(sender_machine->transaction.seq_number, header.transaction_sequence_number.arr, header.transaction_sequence_number.nCount);

	metadata_pdu.closure_requested = ClosureRequested_requested;
	metadata_pdu.checksum_type = 0;
	metadata_pdu.file_size = transaction_get_file_size(&sender_machine->transaction);

	strncpy(metadata_pdu.source_file_name.arr, sender_machine->transaction.source_filename,
		MAX_FILE_NAME_SIZE);
	metadata_pdu.source_file_name.arr[MAX_FILE_NAME_SIZE - 1] = '\0';
	metadata_pdu.source_file_name.nCount = strlen(metadata_pdu.source_file_name.arr);

	strncpy(metadata_pdu.destination_file_name.arr, sender_machine->transaction.destination_filename,
		MAX_FILE_NAME_SIZE);
	metadata_pdu.destination_file_name.arr[MAX_FILE_NAME_SIZE - 1] = '\0';
	metadata_pdu.destination_file_name.nCount = strlen(metadata_pdu.destination_file_name.arr);

	pdu.pdu_header = header;
	pdu.payload.kind = PayloadData_file_directive_PRESENT;
	pdu.payload.u.file_directive.file_directive_pdu.kind = FileDirectivePDU_metadata_pdu_PRESENT;
	pdu.payload.u.file_directive.file_directive_pdu.u.metadata_pdu = metadata_pdu;

	unsigned char buf[cfdpCfdpPDU_REQUIRED_BITS_FOR_ACN_ENCODING];
	long size = cfdpCfdpPDU_REQUIRED_BITS_FOR_ACN_ENCODING;
	BitStream bit_stream;
	BitStream_AttachBuffer(&bit_stream, buf, size);
	int error_code;

	if (!cfdpCfdpPDU_ACN_Encode(&pdu, &bit_stream, &error_code, true)){
		printf(
	    "cannot encode cfdp pdu error_code = %d\n",
	    error_code);
		return;
	}

	sender_machine->core->transport->transport_send_pdu(sender_machine->transaction.destination_entity_id, bit_stream.buf , bit_stream.currentByte + 1);
}

void sender_machine_send_eof(struct sender_machine *sender_machine){
	cfdpCfdpPDU pdu;
	cfdpPDUHeader header;
	cfdpEofPDU eof_pdu;

	header.direction = cfdpDirection_toward_receiver;
	header.transmission_mode = cfdpTransmissionMode_unacknowledged;
	header.crc_flag = cfdpCRCFlag_crc_not_present;
	header.large_file_flag = 0;
	header.segmentation_control = cfdpSegmentationControl_record_boundries_not_preserved;
	header.segment_metadata_flag = cfdpSegmentMetadataFlag_flag_not_present;
	
	ulong_to_bytes(sender_machine->transaction.source_entity_id, header.source_entity_id.arr, header.source_entity_id.nCount);
	ulong_to_bytes(sender_machine->transaction.destination_entity_id, header.destination_entity_id.arr, header.destination_entity_id.nCount);
	ulong_to_bytes(sender_machine->transaction.seq_number, header.transaction_sequence_number.arr, header.transaction_sequence_number.nCount);

	eof_pdu.condition_code = sender_machine->condition_code;
	eof_pdu.file_checksum = transaction_get_file_checksum(&sender_machine->transaction);
	eof_pdu.file_size = transaction_get_file_size(&sender_machine->transaction);

	pdu.pdu_header = header;
	pdu.payload.kind = PayloadData_file_directive_PRESENT;
	pdu.payload.u.file_directive.file_directive_pdu.kind = FileDirectivePDU_eof_pdu_PRESENT;
	pdu.payload.u.file_directive.file_directive_pdu.u.eof_pdu = eof_pdu;

	unsigned char buf[cfdpCfdpPDU_REQUIRED_BITS_FOR_ACN_ENCODING];
	long size = cfdpCfdpPDU_REQUIRED_BITS_FOR_ACN_ENCODING;
	BitStream bit_stream;
	BitStream_AttachBuffer(&bit_stream, buf, size);
	int error_code;

	if (!cfdpCfdpPDU_ACN_Encode(&pdu, &bit_stream, &error_code, true)){
		printf(
	    "cannot encode cfdp pdu error_code = %d\n",
	    error_code);
		return;
	}

	sender_machine->core->transport->transport_send_pdu(sender_machine->transaction.destination_entity_id, bit_stream.buf , bit_stream.currentByte + 1);
}

void sender_machine_update_state(struct sender_machine *sender_machine,
				 struct event *event)
{
	if(sender_machine->state == SEND_METADATA)
	{
		switch(event->type)
		{
			case E0_ENTERED_STATE:
			{
				sender_machine_init(sender_machine);
				break;
			}
			case E30_RECEIVED_PUT_REQUEST:
			{
				cfdp_core_transaction_indication(sender_machine->core, sender_machine->transaction_id);
				sender_machine_send_metadata(sender_machine);
				if(transaction_is_file_transfer_in_progress(&sender_machine->transaction)){
					sender_machine->state = SEND_FILE;
					cfdp_core_issue_request(sender_machine->core, sender_machine->transaction_id, E0_ENTERED_STATE);
				}
				else {
					sender_machine_send_eof(sender_machine);
					cfdp_core_eof_sent_indication(sender_machine->core, sender_machine->transaction_id);
					cfdp_core_finished_indication(sender_machine->core, sender_machine->transaction_id);
				}
				break;
			}
			default:
			{
				printf("Event not support for state SEND_METADATA\n");
			}
		}
	} else if (sender_machine->state == WAIT_FOR_EOF){
		switch(event->type)
		{
			case E0_ENTERED_STATE:
			{
				transaction_open_file(&sender_machine->transaction);
				cfdp_core_issue_request(sender_machine->core, sender_machine->transaction_id, E1_SEND_FILE_DATA);
				break;
			}
			case E1_SEND_FILE_DATA:
			{
				if(sender_machine->is_suspended || sender_machine->is_frozen)
					break;

				if(sender_machine->core->transport->transport_is_ready())
				{
					transaction_send_file_data(&sender_machine->transaction);
					if(transaction_is_file_send_complete(&sender_machine->transaction))
					{
						sender_machine_send_eof(sender_machine);
						cfdp_core_eof_sent_indication(sender_machine->core, sender_machine->transaction_id);
						cfdp_core_finished_indication(sender_machine->core, sender_machine->transaction_id);
						sender_machine_close(sender_machine);
						return;
					}
				}
				cfdp_core_issue_request(sender_machine->core, sender_machine->transaction_id, E1_SEND_FILE_DATA);
				
				break;
			}
			case E2_ABANDON_TRANSACTION:
			{
				cfdp_core_abandoned_indication(sender_machine->core, sender_machine->transaction_id);
				sender_machine_close(sender_machine);
				break;
			}
			case E3_NOTICE_OF_CANCELLATION:
			{
				sender_machine_send_eof(sender_machine);
				cfdp_core_finished_indication(sender_machine->core, sender_machine->transaction_id);
				sender_machine_close(sender_machine);
				break;
			}
			case E4_NOTICE_OF_SUSPENSION:
			{
				if(!sender_machine->is_suspended){
					cfdp_core_suspended_indication(sender_machine->core, sender_machine->transaction_id);
					sender_machine->is_suspended = true;
				}
				break;
			}
			case E31_RECEIVED_SUSPEND_REQUEST:
			{
				cfdp_core_issue_request(sender_machine->core, sender_machine->transaction_id, E4_NOTICE_OF_SUSPENSION);
				break;
			}
			case E32_RECEIVED_RESUME_REQUEST:
			{
				if(sender_machine->is_suspended){
					cfdp_core_resumed_indication(sender_machine->core, sender_machine->transaction_id);
					sender_machine->is_suspended = false;
				}
				cfdp_core_issue_request(sender_machine->core, sender_machine->transaction_id, E1_SEND_FILE_DATA);
				break;
			}
			case E33_RECEIVED_CANCEL_REQUEST:
			{
				sender_machine->condition_code = cfdpConditionCode_cancel_request_received;
				cfdp_core_issue_request(sender_machine->core, sender_machine->transaction_id, E3_NOTICE_OF_CANCELLATION);
				break;
			}
			case E34_RECEIVED_REPORT_REQUEST:
			{
				cfdp_core_report_indication(sender_machine->core, sender_machine->transaction_id);
				break;
			}
			case E40_RECEIVED_FREEZE:
			{
				sender_machine->is_frozen = true;
				break;
			}
			case E41_RECEIVED_THAW:
			{
				if(sender_machine->is_frozen){
					sender_machine->is_frozen = false;
					cfdp_core_issue_request(sender_machine->core, sender_machine->transaction_id, E1_SEND_FILE_DATA);
				}
				break;
			}
			default:
			{
				printf("Event not support for state WAIT_FOR_EOF\n");
			}
		}
	}
}