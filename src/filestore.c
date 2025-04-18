#include "filestore.h"

void filestore_create_file(struct filestore *filestore, char *filepath)
{

}

void filestore_delete_file(struct filestore *filestore, char *filepath)
{

}

void filestore_rename_file(struct filestore *filestore, char *filepath_1, char *filepath_2)
{

}

void filestore_append_file(struct filestore *filestore, char *filepath_1, char *filepath_2)
{

}

void filestore_replace_file(struct filestore *filestore, char *filepath_1, char *filepath_2)
{

}

void filestore_list_directory(struct filestore *filestore, char *dirpath)
{
    
}

void filestore_open(struct filestore *filestore, char *filepath)
{

}

uint32_t filestore_get_file_size(struct filestore *filestore, char *filepath)
{
    return 0;
}

void filestore_seek(struct filestore *filestore, char *filepath, uint32_t offset)
{

}

uint32_t filestore_tell(struct filestore *filestore, char *filepath)
{
    return 0;
}

void filestore_read(struct filestore *filestore, char *filepath, char *data, uint32_t length)
{

}

void filestore_write(struct filestore *filestore, char *filepath, char *data, uint32_t length)
{

}

void filestore_close(struct filestore *filestore, char *filepath)
{

}

uint32_t filestore_calculate_checksum(struct filestore *filestore, char *filepath)
{
    return 0;
}