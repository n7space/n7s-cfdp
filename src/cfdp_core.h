#ifndef CFDP_CORE_H
#define CFDP_CORE_H

#include "stdint.h"
#include "stdbool.h"

#include "transport.h"
#include "filestore.h"

typedef struct
{
    uint32_t entity_id;
    filestore *filestore;
    transport *transport;
    bool eof_sent_indication_required;
    bool eof_received_indication_required;
    bool filesegment_received_indication_required;
    bool transaction_finished_indication_required;
    bool suspended_indication_required;
    bool resumed_indication_required;

} cfdp_core;


void cfdp_core_put(cfdp_core *core);

#endif