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

void indication_callback(struct cfdp_core *core,
			 const enum IndicationType indication_type,
			 const struct transaction_id transaction_id)
{
	printf("cfdp indication type=%d source_entity_id = %d seq_number = "
	       "%d\n",
	       indication_type, transaction_id.source_entity_id,
	       transaction_id.seq_number);
}

void error_callback(struct cfdp_core *core, const enum ErrorType error_type,
		    const uint32_t error_code)
{
	printf("cfdp error type=%d error_code = %d\n", error_type, error_code);
}

bool test_timer_restart(void *user_data, const uint32_t timeout,
			void expired(struct receiver_timer *))
{
	return true;
}

bool test_timer_stop(void *user_data) { return true; }

int main(int argc, char *argv[])
{
	CFDP_DATA_BUFFER(cfdp_data_buffer);

	struct filestore_cfg filestore;
	filestore.filestore_data = NULL;
	filestore.filestore_delete_file = test_delete_file;
	filestore.filestore_get_file_size = test_filestore_get_file_size;
	filestore.filestore_read = test_filestore_read_file;
	filestore.filestore_write = test_filestore_write_to_file;

	struct transport transport;
	transport.transport_data = NULL;
	transport.transport_send_pdu = test_transport_send_pdu;
	transport.transport_is_ready = test_transport_is_ready;

	struct receiver_timer timer;
	timer.timer_data = NULL;
	timer.timer_restart = test_timer_restart;
	timer.timer_stop = test_timer_stop;

	struct cfdp_core cfd_entity_sender;

	cfdp_core_init(&cfd_entity_sender, &filestore, &transport, 6,
		       CHECKSUM_TYPE_MODULAR, &timer, 30, cfdp_data_buffer);
	cfdp_core_register_indication_callback(&cfd_entity_sender,
					       indication_callback);
	cfdp_core_register_error_callback(&cfd_entity_sender, error_callback);

	test_transport_init_and_bind(&cfd_entity_sender);

	cfdp_core_put(&cfd_entity_sender, 13, "test/files/small1.txt",
		      "received_small1.txt", 0, NULL);

	while (!cfdp_core_is_done(&cfd_entity_sender)) {
		usleep(1000 * 100);
	}

	sleep(1);
	test_transport_close();

	if (!file_exists(
		"test/test_send_small_file/target/received_small1.txt")) {
		return -1;
	}

	if (compare_files(
		"test/files/small1.txt",
		"test/test_send_small_file/target/received_small1.txt") != 0) {
		return -1;
	}

	return 0;
}