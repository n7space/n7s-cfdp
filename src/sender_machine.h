#ifndef SENDER_MACHINE_H
#define SENDER_MACHINE_H

#include "stdbool.h"
#include "stdint.h"

#include "constants.h"
#include "transaction.h"
#include "transaction_id.h"

struct event;

struct sender_machine {
	struct transaction_id transaction_id;
	struct transaction transaction;
	enum MachineState state;
};

void sender_machine_update_state(struct sender_machine *sender_machine,
				 struct event *event);

#endif