/*
 * timer_d21.h
 *
 * Created: 3/16/2016 3:40:02 PM
 *  Author: manohar.bathula
 */ 


#ifndef TIMER_D21_H_
#define TIMER_D21_H_

#include <stdint.h>

struct Timer {
	uint32_t end_time;
	uint32_t offset;
};

#endif /* TIMER_D21_H_ */