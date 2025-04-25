#include "cfdp_core.h"
#include "filestore.h"
#include "transaction.h"

void transaction_open_temp_file(struct transaction *transaction){
	transaction->filestore->filestore_open(TEMP_FILE_NAME);
}

void transaction_store_data_in_temp_file(struct transaction *transaction, const cfdpFileDataPDU *file_data_pdu){
	transaction->filestore->filestore_seek(TEMP_FILE_NAME, file_data_pdu->segment_offset);
	transaction->filestore->filestore_write(TEMP_FILE_NAME, file_data_pdu->file_data.arr, file_data_pdu->file_data.nCount);
}

uint32_t transaction_get_temp_file_checksum(struct transaction *transaction)
{
	return transaction->filestore->filestore_calculate_checksum(TEMP_FILE_NAME);
}

void transaction_copy_temp_file_to_dest_file(struct transaction *transaction){
	
}

void transaction_close_temp_file(struct transaction *transaction){
	transaction->filestore->filestore_close(TEMP_FILE_NAME);
}

void transaction_store_file_data(struct transaction *transaction){

}

uint32_t transaction_get_file_size(struct transaction *transaction)
{
	transaction->filestore->filestore_open(transaction->source_filename);
	transaction->file_size =
	    transaction->filestore->filestore_get_file_size(transaction->source_filename);
	transaction->filestore->filestore_close(transaction->source_filename);

	return transaction->file_size;
}

uint32_t transaction_get_file_checksum(struct transaction *transaction)
{
	transaction->filestore->filestore_open(transaction->source_filename);
	uint32_t checksum = transaction->filestore->filestore_calculate_checksum(
	    transaction->source_filename);
	transaction->filestore->filestore_close(transaction->source_filename);

	return checksum;
}

bool transaction_get_file_segment(struct transaction *transaction, char *data,
				  uint32_t length)
{
	if (transaction->file_position > transaction->file_size) {
		return false;
	}

	transaction->filestore->filestore_open(transaction->source_filename);
	transaction->filestore->filestore_seek(transaction->source_filename,
				  transaction->file_position);
	transaction->filestore->filestore_read(transaction->source_filename, data, length);
	transaction->filestore->filestore_close(transaction->source_filename);

	transaction->file_position += length;

	return true;
}

bool transaction_is_file_transfer_in_progress(struct transaction *transaction)
{
	return transaction->source_filename[0] != '\0' && transaction->destination_filename[0] != '\0';
}

bool transaction_is_file_send_complete(struct transaction *transaction)
{
	return transaction->file_position >= transaction->file_size;
}
