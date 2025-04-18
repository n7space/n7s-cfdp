#ifndef CFDP_CORE_H
#define CFDP_CORE_H

#include "string.h"
#include "stdint.h"
#include "stdbool.h"

#include "constants.h"
#include "transport.h"
#include "transaction.h"
#include "filestore.h"
#include "sender_machine.h"
#include "receiver_machine.h"

struct transaction_id
{
    uint32_t source_entity_id;
    uint32_t seq_number;
};

struct cfdp_core
{
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
    struct sender_machine sender;
    struct receiver_machine receiver;
};


// TODO add fault_handler_overrides, messages_to_user, filestore_requests, checksum_type
struct transaction_id cfdp_core_put(struct cfdp_core *core,
                                    uint32_t destination_entity_id,
                                    char source_filename[MAX_FILE_NAME_SIZE],
                                    char destination_filename[MAX_FILE_NAME_SIZE]);

#endif