#include <stdio.h>
#include <unistd.h>
#include "cfdp_core.h"
#include "test_filestore.h"
#include "test_transport.h"

bool file_exists(const char *fname)
{
    FILE *file;
    if ((file = fopen(fname, "r")))
    {
        fclose(file);
        return true;
    }
    return false;
}


int main(int argc, char *argv[]) {

    struct filestore_cfg filestore;
    filestore.filestore_replace_file = test_filestore_copy_file;
    filestore.filestore_get_file_size = test_filestore_get_file_size;
    filestore.filestore_read = test_filestore_read_file;
    filestore.filestore_write = test_filestore_write_to_file;
    filestore.filestore_calculate_checksum = test_filestore_calculate_checksum;

    struct transport transport;
    transport.transport_send_pdu = test_transport_send_pdu;
    transport.transport_is_ready = test_transport_is_ready;

    struct cfdp_core cfd_entity_sender;

    cfdp_core_init(&cfd_entity_sender, &filestore, &transport);
    test_transport_init_and_bind(&cfd_entity_sender);

    cfdp_core_put(&cfd_entity_sender, 13, "../test/files/local/small.txt", "received_small.txt");

    while(!cfdp_core_is_done(&cfd_entity_sender)){
        usleep(1000 * 100);
    }

    if(!file_exists("test/files/remote/received_small.txt"))
        return -1;

    return 0;
}