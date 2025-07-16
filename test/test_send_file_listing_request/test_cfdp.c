#include "cfdp_core.h"
#include "test_filestore.h"
#include "test_transport.h"
#include <stdio.h>
#include <unistd.h>

#define FILE_BUFFER_SIZE 4096

bool file_exists(const char *fname)
{
	FILE *file;
	if ((file = fopen(fname, "r"))) {
		fclose(file);
		return true;
	}
	return false;
}

bool search_string_in_file(const char *filename, const char *search_str)
{
	FILE *file = fopen(filename, "rb");
	if (file == NULL) {
		perror("Error: Failed to open file\n");
		return -1;
	}

	fseek(file, 0, SEEK_END);
	long filesize = ftell(file);
	rewind(file);

	char buffer[FILE_BUFFER_SIZE];

	fread(buffer, sizeof(char), filesize, file);
	buffer[filesize] = '\0';

	fclose(file);

	return (strstr(buffer, search_str) != NULL) ? true : false;
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

void test_timer_restart(void *user_data, const uint32_t timeout,
			void expired(struct receiver_timer *))
{
}

void test_timer_stop(void *user_data) {}

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
	cfdp_core_register_indication_callback(&cfd_entity_sender, indication_callback);
	cfdp_core_register_error_callback(&cfd_entity_sender, error_callback);

	test_transport_init_and_bind(&cfd_entity_sender);

	struct message_to_user messages_to_user[MAX_NUMBER_OF_MESSAGES_TO_USER];

	messages_to_user[0].message_to_user_type = DIRECTORY_LISTING_REQUEST;

	strcpy(
	    messages_to_user[0]
		.message_to_user_union.directory_listing_request.directory_name,
	    "../../files");

	strcpy(messages_to_user[0]
		   .message_to_user_union.directory_listing_request
		   .directory_file_name,
	       "test/test_send_file_listing_request/target/listing_result.txt");

	cfdp_core_put(&cfd_entity_sender, 13, "", "", 1, messages_to_user);

	while (!cfdp_core_is_done(&cfd_entity_sender)) {
		usleep(1000 * 100);
	}

	sleep(2);
	test_transport_close();

	if (!file_exists("test/test_send_file_listing_request/target/"
			 "listing_result.txt")) {
		return -1;
	}

	if (!search_string_in_file(
		"test/test_send_file_listing_request/target/listing_result.txt",
		"type,path,size,timestamp")) {
		return -1;
	}

	if (!search_string_in_file(
		"test/test_send_file_listing_request/target/listing_result.txt",
		"f,/../../files/small2.txt,23")) {
		return -1;
	}

	return 0;
}