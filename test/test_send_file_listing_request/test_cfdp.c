#include "cfdp_core.h"
#include "test_filestore.h"
#include "test_transport.h"
#include <stdio.h>
#include <unistd.h>

bool does_file_exist_and_is_not_empty(const char *fname)
{
	FILE *file;
	if ((file = fopen(fname, "r"))) {
		int character = fgetc(file);
		fclose(file);
		return character != EOF;
	}
	return false;
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

void test_timer_restart(const int timeout,
			      		void expired(struct receiver_timer *)){}

void test_timer_stop(){}

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

	cfdp_core_init(&cfd_entity_sender, &filestore, &transport, 6,
		       CHECKSUM_TYPE_MODULAR, 30);
	cfd_entity_sender.cfdp_core_indication_callback = indication_callback;
	cfd_entity_sender.cfdp_core_error_callback = error_callback;
	cfd_entity_sender.receiver[0].timer.timer_restart = test_timer_restart;
	cfd_entity_sender.receiver[0].timer.timer_stop = test_timer_stop;

	test_transport_init_and_bind(&cfd_entity_sender);

	struct message_to_user messages_to_user[MAX_NUMBER_OF_MESSAGES_TO_USER];

	messages_to_user[0].message_to_user_type = DIRECTORY_LISTING_REQUEST;

	strcpy(messages_to_user[0].message_to_user_union.directory_listing_request.directory_name, "../../files");

	strcpy(messages_to_user[0].message_to_user_union.directory_listing_request.directory_file_name,
		"test/test_send_file_listing_request/target/listing_result.txt");

	cfdp_core_put(&cfd_entity_sender, 13, "test/files/small1.txt",
		      "received_small1.txt", 1, messages_to_user);

	while (!cfdp_core_is_done(&cfd_entity_sender)) {
		usleep(1000 * 100);
	}

	sleep(2);
	test_transport_close();

	if (!does_file_exist_and_is_not_empty(
		"test/test_send_file_listing_request/target/listing_result.txt")) {
		return -1;
	}

	return 0;
}