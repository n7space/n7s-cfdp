#include "cfdp_core.h"
#include <stdio.h>

struct transaction_id cfdp_core_put(struct cfdp_core *core,
                                    uint32_t destination_entity_id,
                                    char source_filename[MAX_FILE_NAME_SIZE],
                                    char destination_filename[MAX_FILE_NAME_SIZE])
{
    core->transaction_sequence_number++;

    struct transaction transaction = 
    {
        .kernel = core,
        .source_entity_id = core->entity_id,
        .seq_number = core->transaction_sequence_number,
        .destination_entity_id = destination_entity_id,
    };

    strncpy(transaction.source_filename, source_filename, MAX_FILE_NAME_SIZE);
    transaction.source_filename[MAX_FILE_NAME_SIZE - 1] = '\0';

    strncpy(transaction.destination_filename, destination_filename, MAX_FILE_NAME_SIZE);
    transaction.destination_filename[MAX_FILE_NAME_SIZE - 1] = '\0';

    if(core->sender.state != COMPLETED)
    {
        // TODO handle sender busy
    }
    core->sender.state = SEND_METADATA;

    struct event event =
    {
        .transaction = &transaction,
        .type = E0_ENTERED_STATE
    };
    sender_machine_update_state(&core->sender, &event);

    event.type = E30_RECEIVED_PUT_REQUEST;
    sender_machine_update_state(&core->sender, &event);

    struct transaction_id transaction_id =
    {
        .source_entity_id = core->entity_id,
        .seq_number = core->transaction_sequence_number
    };

    return transaction_id;
}