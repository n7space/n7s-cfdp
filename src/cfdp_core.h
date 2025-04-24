#ifndef CFDP_CORE_H
#define CFDP_CORE_H

#include "stdbool.h"
#include "stdint.h"
#include "string.h"

#include "constants.h"
#include "filestore.h"
#include "receiver_machine.h"
#include "sender_machine.h"
#include "transaction_id.h"
#include "transport.h"

struct cfdp_core {
	uint32_t entity_id;
	struct filestore *filestore;
	struct transport *transport;
	bool eof_sent_indication_required;
	bool eof_received_indication_required;
	bool filesegment_received_indication_required;
	bool transaction_finished_indication_required;
	bool suspended_indication_required;
	bool resumed_indication_required;

	uint32_t transaction_sequence_number;
	struct sender_machine sender[MAX_NUMBER_OF_SENDER_MACHINES];
	struct receiver_machine receiver[MAX_NUMBER_OF_RECEIVER_MACHINES];
};

// CFDP service requests

struct transaction_id
cfdp_core_put(struct cfdp_core *core, uint32_t destination_entity_id,
	      char source_filename[MAX_FILE_NAME_SIZE],
	      char destination_filename[MAX_FILE_NAME_SIZE]);

void cfdp_core_cancel(struct cfdp_core *core,
		      struct transaction_id transaction_id);

void cfdp_core_suspend(struct cfdp_core *core,
		       struct transaction_id transaction_id);

void cfdp_core_resume(struct cfdp_core *core,
		      struct transaction_id transaction_id);

void cfdp_core_report(struct cfdp_core *core,
		      struct transaction_id transaction_id);

// CFDP indications

void cfdp_core_transaction_indication(struct cfdp_core *core,
				      struct transaction_id transaction_id);

void cfdp_core_eof_sent_indication(struct cfdp_core *core,
				   struct transaction_id transaction_id);

void cfdp_core_finished_indication(struct cfdp_core *core,
				   struct transaction_id transaction_id);

void cfdp_core_report_indication(struct cfdp_core *core,
				 struct transaction_id transaction_id);

void cfdp_core_eof_received_indication(struct cfdp_core *core,
				       struct transaction_id transaction_id);

void cfdp_core_metadata_received_indication(
    struct cfdp_core *core, struct transaction_id transaction_id);

void cfdp_core_filesegment_received_indication(
    struct cfdp_core *core, struct transaction_id transaction_id);

void cfdp_core_abandoned_indication(struct cfdp_core *core,
				    struct transaction_id transaction_id);

void cfdp_core_suspended_indication(struct cfdp_core *core,
				    struct transaction_id transaction_id);

void cfdp_core_resumed_indication(struct cfdp_core *core,
				  struct transaction_id transaction_id);

void cfdp_core_fault_indication(struct cfdp_core *core,
				struct transaction_id transaction_id);

// CFDP link state procedures

void cfdp_core_freeze(struct cfdp_core *core, uint32_t destination_entity_id);

void cfdp_core_thaw(struct cfdp_core *core, uint32_t destination_entity_id);

void cfdp_core_received_pdu(struct cfdp_core *core, unsigned char *buf, long count);

#endif