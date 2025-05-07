#include "cfdp_core.h"
#include "test_filestore.h"
#include "test_transport.h"
#include <stdio.h>
#include <unistd.h>

bool file_exists(const char *fname)
{
	FILE *file;
	if ((file = fopen(fname, "r"))) {
		fclose(file);
		return true;
	}
	return false;
}

int compare_files(const char *file1, const char *file2)
{
	FILE *fp1 = fopen(file1, "rb");
	FILE *fp2 = fopen(file2, "rb");

	if (!fp1 || !fp2) {
		if (fp1)
			fclose(fp1);
		if (fp2)
			fclose(fp2);
		return -1;
	}

	int result = 0;
	int ch1, ch2;

	while (1) {
		ch1 = fgetc(fp1);
		ch2 = fgetc(fp2);

		if (ch1 != ch2) {
			result = 1;
			break;
		}

		if (ch1 == EOF || ch2 == EOF)
			break;
	}

	if (ch1 != ch2)
		result = 1;

	fclose(fp1);
	fclose(fp2);
	return result;
}

int main(int argc, char *argv[])
{

	struct filestore_cfg filestore;
	filestore.filestore_replace_file = test_filestore_copy_file;
	filestore.filestore_get_file_size = test_filestore_get_file_size;
	filestore.filestore_read = test_filestore_read_file;
	filestore.filestore_write = test_filestore_write_to_file;
	filestore.filestore_calculate_checksum =
	    test_filestore_calculate_checksum;

	struct transport transport;
	transport.transport_send_pdu = test_transport_send_pdu;
	transport.transport_is_ready = test_transport_is_ready;

	struct cfdp_core cfd_entity_sender;

	cfdp_core_init(&cfd_entity_sender, &filestore, &transport, 6, 30);
	test_transport_init_and_bind(&cfd_entity_sender);

	cfdp_core_put(&cfd_entity_sender, 13, "test/files/small.txt",
		      "received_small.txt");

	while (!cfdp_core_is_done(&cfd_entity_sender)) {
		usleep(1000 * 100);
	}

	sleep(1);
	test_transport_close();

	if (!file_exists(
		"test/test_send_of_small_file/target/received_small.txt")) {
		return -1;
	}

	if (compare_files(
		"test/files/small.txt",
		"test/test_send_of_small_file/target/received_small.txt") !=
	    0) {
		return -1;
	}

	return 0;
}