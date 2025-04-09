#include "transaction.h"
#include "filestore.h"

uint32_t transaction_get_file_size(transaction *transaction)
{
    filestore_open(transaction->kernel->filestore, transaction->source_filename);
    transaction->file_size = filestore_get_file_size(transaction->kernel->filestore, transaction->source_filename);
    filestore_close(transaction->kernel->filestore, transaction->source_filename);

    return transaction->file_size;
}

uint32_t transaction_get_file_checksum(transaction *transaction)
{
    filestore_open(transaction->kernel->filestore, transaction->source_filename);
    uint32_t checksum = filestore_calculate_checksum(transaction->kernel->filestore, transaction->source_filename);
    filestore_close(transaction->kernel->filestore, transaction->source_filename);

    return checksum;
}

bool transaction_get_file_segment(transaction *transaction, char *data, uint32_t length)
{
    if(transaction->file_position > transaction->file_size)
    {
        return false;
    }

    filestore_open(transaction->kernel->filestore, transaction->source_filename);
    filestore_seek(transaction->kernel->filestore, transaction->source_filename, transaction->file_position);
    filestore_read(transaction->kernel->filestore, transaction->source_filename, data, length);
    filestore_close(transaction->kernel->filestore, transaction->source_filename);

    transaction->file_position += length;

    return true;
}

bool transaction_is_file_send_complete(transaction *transaction)
{
    return transaction->file_position >= transaction->file_size;
}
