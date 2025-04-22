#include "filestore.h"

void filestore_create_file(char *filepath) {}

void filestore_delete_file(char *filepath) {}

void filestore_rename_file(char *old_file_name, char *new_file_name) {}

void filestore_append_file(char *target_filepath, char *source_filepath) {}

void filestore_replace_file(char *target_filepath, char *source_filepath) {}

void filestore_list_directory(char *dirpath) {}

void filestore_open(char *filepath) {}

uint32_t filestore_get_file_size(char *filepath) { return 0; }

void filestore_seek(char *filepath, uint32_t offset) {}

uint32_t filestore_tell(char *filepath) { return 0; }

void filestore_read(char *filepath, char *data, uint32_t length) {}

void filestore_write(char *filepath, char *data, uint32_t length) {}

void filestore_close(char *filepath) {}

uint32_t filestore_calculate_checksum(char *filepath) { return 0; }