#ifndef FILESTORE_H
#define FILESTORE_H

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

#endif