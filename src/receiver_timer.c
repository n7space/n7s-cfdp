#include "cfdp_core.h"
#include "receiver_timer.h"

void receiver_timer_restart(struct receiver_timer *timer)
{
	if (timer->timer_stop != NULL) {
		timer->timer_stop(timer->timer_data);
	}

	if (timer->timer_restart != NULL) {
		timer->timer_restart(timer->timer_data, timer->timeout,
				     receiver_timer_expired);
	}
}

void receiver_timer_stop(struct receiver_timer *timer)
{
	if (timer->timer_stop != NULL) {
		timer->timer_stop(timer->timer_data);
	}
}

void receiver_timer_expired(struct receiver_timer *timer)
{
	cfdp_core_issue_request(timer->core, timer->transaction_id,
				E27_INACTIVITY_TIMEOUT);
}