#ifndef FILESTORE_H
#define FILESTORE_H

#include "stdint.h"

struct filestore_cfg {

    void (*filestore_create_file)(char *filepath);
    void (*filestore_delete_file)(char *filepath);
    void (*filestore_rename_file)(char *old_file_name, char *new_file_name);
    void (*filestore_append_file)(char *target_filepath, char *source_filepath);
    void (*filestore_replace_file)(char *target_filepath, char *source_filepath);
    void (*filestore_list_directory)(char *dirpath);

    void (*filestore_open)(char *filepath);
    uint32_t (*filestore_get_file_size)(char *filepath);
    void (*filestore_seek)(char *filepath, uint32_t offset);
    uint32_t (*filestore_tell)(char *filepath);
    void (*filestore_read)(char *filepath, char *data, uint32_t length);
    void (*filestore_write)(char *filepath, char *data, uint32_t length);
    void (*filestore_close)(char *filepath);
    uint32_t (*filestore_calculate_checksum)(char *filepath);
};



// TODO checksum

#endif