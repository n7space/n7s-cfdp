#ifndef RECEIVER_MACHINE_H
#define RECEIVER_MACHINE_H

#include "stdint.h"
#include "stdbool.h"

#include "constants.h"
#include "transaction.h"
#include "transaction_id.h"

struct event;

struct receiver_machine
{
    struct transaction_id transaction_id;
    struct transaction transaction;
    enum MachineState state;
};


void receiver_machine_update_state(struct receiver_machine *receiver_machine, struct event *event);

#endif