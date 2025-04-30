#include <stdio.h>
#include "cfdp_core.h"
#include "test_filestore.h"


int main(int argc, char *argv[]) {

    struct filestore_cfg filestore;
    filestore.filestore_replace_file = test_filestore_copy_file;
    filestore.filestore_get_file_size = test_filestore_get_file_size;
    filestore.filestore_read = test_filestore_read_file;
    filestore.filestore_write = test_filestore_write_to_file;
    filestore.filestore_calculate_checksum = test_filestore_calculate_checksum;

    struct cfdp_core cfd_entity_sender;
    cfd_entity_sender.entity_id = 6;
    cfd_entity_sender.filestore = &filestore;

    return 0;
}