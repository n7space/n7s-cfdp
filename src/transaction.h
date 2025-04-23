#ifndef TRANSACTION_H
#define TRANSACTION_H

struct cfdp_core;
struct filestore_cfg;

struct transaction {
	struct cfdp_core *kernel;
	uint32_t source_entity_id;
	uint32_t seq_number;
	uint32_t destination_entity_id;
	char source_filename[MAX_FILE_NAME_SIZE];
	char destination_filename[MAX_FILE_NAME_SIZE];

	uint32_t file_size;
	uint32_t file_position;
};

uint32_t transaction_get_file_size(struct transaction *transaction, const struct filestore_cfg *filestore);
uint32_t transaction_get_file_checksum(struct transaction *transaction, const struct filestore_cfg *filestore);
bool transaction_get_file_segment(struct transaction *transaction, char *data,
				  uint32_t length, const struct filestore_cfg *filestore);
bool transaction_is_file_send_complete(struct transaction *transaction);

#endif