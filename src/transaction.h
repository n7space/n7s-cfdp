#ifndef TRANSACTION_H
#define TRANSACTION_H

static const char TEMP_FILE_NAME[] = ".temp_received_file";

struct cfdp_core;
struct filestore_cfg;

struct transaction {
	struct cfdp_core *kernel;
	struct filestore_cfg *filestore;
	uint64_t source_entity_id;
	uint64_t seq_number;
	uint64_t destination_entity_id;
	char source_filename[MAX_FILE_NAME_SIZE];
	char destination_filename[MAX_FILE_NAME_SIZE];

	uint32_t file_size;
	uint32_t file_position;
};

void transaction_open_temp_file(struct transaction *transaction);
void transaction_store_data_in_temp_file(struct transaction *transaction, const cfdpFileDataPDU *file_data);
uint32_t transaction_get_temp_file_checksum(struct transaction *transaction);
void transaction_copy_temp_file_to_dest_file(struct transaction *transaction);
void transaction_close_temp_file(struct transaction *transaction);

void transaction_open_file(struct transaction *transaction);
void transaction_send_file_data(struct transaction *transaction);
bool transaction_is_file_send_complete(struct transaction *transaction);
void transaction_close_file(struct transaction *transaction);

uint32_t transaction_get_file_size(struct transaction *transaction);
uint32_t transaction_get_file_checksum(struct transaction *transaction);
bool transaction_get_file_segment(struct transaction *transaction, char *data,
				  uint32_t length);
bool transaction_is_file_transfer_in_progress(struct transaction *transaction);


#endif