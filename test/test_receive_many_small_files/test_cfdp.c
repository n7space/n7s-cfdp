#include "cfdp_core.h"
#include "test_filestore.h"
#include "test_transport.h"
#include <stdio.h>
#include <unistd.h>

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
	filestore.filestore_delete_file = test_delete_file;
	filestore.filestore_get_file_size = test_filestore_get_file_size;
	filestore.filestore_read = test_filestore_read_file;
	filestore.filestore_write = test_filestore_write_to_file;

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

	sleep(9);
	test_transport_close();

	return 0;
}