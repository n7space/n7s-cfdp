#ifndef RECEIVER_MACHINE_H
#define RECEIVER_MACHINE_H

#include "stdint.h"
#include "stdbool.h"

#include "constants.h"
#include "event.h"

struct receiver_machine
{
    enum MachineState state;
};


void receiver_machine_update_state(struct receiver_machine *receiver_machine, struct event *event);

#endif