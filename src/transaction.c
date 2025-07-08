#include "cfdp_core.h"
#include "filestore.h"
#include "transaction.h"
#include <stdio.h>

void transaction_store_data_to_file(struct transaction *transaction,
				    const cfdpFileDataPDU *file_data_pdu)
{
	transaction->filestore->filestore_write(
	    transaction->destination_filename, file_data_pdu->segment_offset,
	    (const char *)file_data_pdu->file_data.arr,
	    file_data_pdu->file_data.nCount);
}

uint32_t transaction_get_stored_file_checksum(struct transaction *transaction)
{
	if (transaction->core->checksum_type == CHECKSUM_TYPE_NONE) {
		return 0;
	}

	uint32_t checksum =
	    transaction->filestore->filestore_calculate_checksum(
		transaction->destination_filename,
		transaction->core->checksum_type);

	return checksum;
}

void transaction_delete_stored_file(struct transaction *transaction)
{
	transaction->filestore->filestore_delete_file(
	    transaction->destination_filename);
}

bool transaction_is_file_transfer_in_progress(struct transaction *transaction)
{
	return transaction->source_filename[0] != '\0' &&
	       transaction->destination_filename[0] != '\0';
}

bool transaction_is_file_send_complete(struct transaction *transaction)
{
	return transaction->file_position >= transaction->file_size;
}

uint32_t transaction_get_file_size(struct transaction *transaction)
{
	transaction->file_size =
	    transaction->filestore->filestore_get_file_size(
		transaction->source_filename);

	return transaction->file_size;
}

uint32_t transaction_get_file_checksum(struct transaction *transaction)
{
	if (transaction->core->checksum_type == CHECKSUM_TYPE_NONE) {
		return 0;
	}

	uint32_t checksum =
	    transaction->filestore->filestore_calculate_checksum(
		transaction->source_filename, transaction->core->checksum_type);

	return checksum;
}

bool transaction_get_file_segment(struct transaction *transaction,
				  char *out_data, uint32_t *out_length)
{
	if (transaction->file_position >= transaction->file_size) {
		return false;
	}

	*out_length = transaction->file_size - transaction->file_position >
			      FILE_SEGMENT_LEN
			  ? FILE_SEGMENT_LEN
			  : transaction->file_size - transaction->file_position;

	transaction->filestore->filestore_read(transaction->source_filename,
					       transaction->file_position,
					       out_data, *out_length);

	transaction->file_position += *out_length;

	return true;
}

uint32_t transaction_get_messages_to_user_count(struct transaction *transaction)
{
	return transaction->messages_to_user_count;
}

struct message_to_user
transaction_get_message_to_user(struct transaction *transaction, uint32_t index)
{
	return transaction->messages_to_user[index];
}

void transaction_process_messages_to_user(struct transaction *transaction)
{
	for (int i = 0; i < transaction->messages_to_user_count; i++) {

		switch (transaction->messages_to_user[i].message_to_user_type) {
		case DIRECTORY_LISTING_REQUEST: {
			bool result =
			    transaction->filestore
				->filestore_dump_directory_listing_to_file(
				    transaction->messages_to_user[i]
					.message_to_user_union
					.directory_listing_request
					.directory_name,
				    ".listing");

			struct message_to_user
			    messages_to_user[MAX_NUMBER_OF_MESSAGES_TO_USER];

			messages_to_user[0].message_to_user_type =
			    DIRECTORY_LISTING_RESPONSE;
			messages_to_user[0]
			    .message_to_user_union.directory_listing_response
			    .listing_response_code =
			    result ? LISTING_SUCCESSFUL : LISTING_UNSUCCESSFUL;
			strcpy(messages_to_user[0]
				   .message_to_user_union
				   .directory_listing_response.directory_name,
			       transaction->messages_to_user[i]
				   .message_to_user_union
				   .directory_listing_request.directory_name);
			strcpy(
			    messages_to_user[0]
				.message_to_user_union
				.directory_listing_response.directory_file_name,
			    transaction->messages_to_user[i]
				.message_to_user_union.directory_listing_request
				.directory_file_name);

			messages_to_user[1].message_to_user_type =
			    ORIGINATING_TRANSACTION_ID;
			messages_to_user[1]
			    .message_to_user_union.originating_transaction_id
			    .source_entity_id = transaction->source_entity_id;
			messages_to_user[1]
			    .message_to_user_union.originating_transaction_id
			    .seq_number = transaction->seq_number;

			cfdp_core_put(
			    transaction->core, transaction->core->entity_id,
			    ".listing",
			    transaction->messages_to_user[i]
				.message_to_user_union.directory_listing_request
				.directory_file_name,
			    2, messages_to_user);
			break;
		}
		case DIRECTORY_LISTING_RESPONSE: {
			if (transaction->messages_to_user[i]
				.message_to_user_union
				.directory_listing_response
				.listing_response_code == LISTING_SUCCESSFUL) {
				struct transaction_id transaction_id;
				transaction_id.source_entity_id =
				    transaction->source_entity_id;
				transaction_id.seq_number =
				    transaction->seq_number;
				cfdp_core_successful_listing_indication(
				    transaction->core, transaction_id);
			} else if (transaction->messages_to_user[i]
				       .message_to_user_union
				       .directory_listing_response
				       .listing_response_code ==
				   LISTING_UNSUCCESSFUL) {
				struct transaction_id transaction_id;
				transaction_id.source_entity_id =
				    transaction->source_entity_id;
				transaction_id.seq_number =
				    transaction->seq_number;
				cfdp_core_unsuccessful_listing_indication(
				    transaction->core, transaction_id);
			}
			break;
		}
		case ORIGINATING_TRANSACTION_ID: {
			// NOOP
			break;
		}
		default: {
			if (transaction->core->cfdp_core_error_callback !=
			    NULL) {
				transaction->core->cfdp_core_error_callback(
				    transaction->core, UNSUPPORTED_ACTION, 0);
			}
		}
		}
	}
}
