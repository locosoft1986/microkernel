#ifndef TIMER_H
#define TIMER_H

#include "stdint.h"

/* Install the timer handler && setup the system clock */
void init_timer();

/* Set the timer phase in hz */
void timer_phase(uint32_t frequency);

/* Loop until the given time has been reached */
void timer_uwait(uint32_t ticks);

/* A macro to wait the given time in seconds */
#define timer_wait(x)	timer_uwait(x*1000)

#endif
