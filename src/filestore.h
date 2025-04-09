#ifndef FILESTORE_H
#define FILESTORE_H

#include "stdint.h"

typedef struct
{
    char rootpath[32];
} filestore;

void filestore_create_file(filestore *filestore, char *filepath);
void filestore_delete_file(filestore *filestore, char *filepath);
void filestore_rename_file(filestore *filestore, char *filepath_1, char *filepath_2);
void filestore_append_file(filestore *filestore, char *filepath_1, char *filepath_2);
void filestore_replace_file(filestore *filestore, char *filepath_1, char *filepath_2);
void filestore_list_directory(filestore *filestore, char *dirpath);

void filestore_open(filestore *filestore, char *filepath);
uint32_t filestore_get_file_size(filestore *filestore, char *filepath);
void filestore_seek(filestore *filestore, char *filepath, uint32_t offset);
uint32_t filestore_tell(filestore *filestore, char *filepath);
void filestore_read(filestore *filestore, char *filepath, char *data, uint32_t length);
void filestore_write(filestore *filestore, char *filepath, char *data, uint32_t length);
void filestore_close(filestore *filestore, char *filepath);
uint32_t filestore_calculate_checksum(filestore *filestore, char *filepath);

// TODO checksum

#endif