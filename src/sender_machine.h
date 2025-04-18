#ifndef SENDER_MACHINE_H
#define SENDER_MACHINE_H

#include "stdint.h"
#include "stdbool.h"

#include "constants.h"
#include "event.h"

struct sender_machine
{
    enum MachineState state;
};

void sender_machine_update_state(struct sender_machine *sender_machine, struct event *event);

#endif