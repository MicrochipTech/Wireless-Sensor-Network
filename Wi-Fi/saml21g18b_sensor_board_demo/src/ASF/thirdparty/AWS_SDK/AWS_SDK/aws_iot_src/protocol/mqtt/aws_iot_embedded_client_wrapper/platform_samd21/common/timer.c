/**
 * @file timer.c
 * @brief D21 implementation of the timer interface.
 */

#include "AWS_SDK/aws_iot_src/protocol/mqtt/aws_iot_embedded_client_wrapper/timer_interface.h"
extern volatile uint32_t ms_ticks;

static uint32_t getTimeInMillis(void)
{
	return ms_ticks;
}

char expired(Timer *timer)
{
	return ((timer->end_time > 0)
	&& ((getTimeInMillis() + timer->offset) > timer->end_time));
}

void countdown_ms(Timer *timer, unsigned int timeout)
{
	uint32_t timems = getTimeInMillis();

	timer->end_time = timems + timeout;

	if (timer->end_time < timems) {
		timer->offset = ~0 - timems + 1;
		timer->end_time += timer->offset;
	}
	else {
		timer->offset = 0;
	}
}

void countdown(Timer *timer, unsigned int timeout)
{
	uint32_t timems = getTimeInMillis();

	timer->end_time = timems + (timeout * 1000);

	if (timer->end_time < timems) {
		timer->offset = ~0 - timems + 1;
		timer->end_time += timer->offset;
	}
	else {
		timer->offset = 0;
	}
}

int left_ms(Timer *timer)
{
	int diff = timer->end_time - (getTimeInMillis() + timer->offset);
	return (diff > 0 ? diff : 0);
}

void InitTimer(Timer *timer)
{
	timer->end_time = 0;
	timer->offset = 0;
}