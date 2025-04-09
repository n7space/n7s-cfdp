#ifndef RECEIVER_MACHINE_H
#define RECEIVER_MACHINE_H

#include "stdint.h"
#include "stdbool.h"

#include "constants.h"

typedef struct
{
    enum MachineState state;

} receiver_machine;


//void receiver_machine_update_state(receiver_machine *receiver_machine, );

#endif