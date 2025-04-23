#include "cfdp_core.h"
#include "filestore.h"
#include "transaction.h"

uint32_t transaction_get_file_size(struct transaction *transaction, const struct filestore *filestore)
{
	filestore->filestore_open(transaction->source_filename);
	transaction->file_size =
	    filestore->filestore_get_file_size(transaction->source_filename);
	filestore->filestore_close(transaction->source_filename);

	return transaction->file_size;
}

uint32_t transaction_get_file_checksum(struct transaction *transaction, const struct filestore *filestore)
{
	filestore->filestore_open(transaction->source_filename);
	uint32_t checksum =
	    filestore->filestore_calculate_checksum(transaction->source_filename);
	filestore->filestore_close(transaction->source_filename);

	return checksum;
}

bool transaction_get_file_segment(struct transaction *transaction, char *data,
				  uint32_t length, const struct filestore *filestore)
{
	if (transaction->file_position > transaction->file_size) {
		return false;
	}

	filestore->filestore_open(transaction->source_filename);
	filestore->filestore_seek(transaction->source_filename,
		       transaction->file_position);
	filestore->filestore_read(transaction->source_filename, data, length);
	filestore->filestore_close(transaction->source_filename);

	transaction->file_position += length;

	return true;
}

bool transaction_is_file_send_complete(struct transaction *transaction)
{
	return transaction->file_position >= transaction->file_size;
}
