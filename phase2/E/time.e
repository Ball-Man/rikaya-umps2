#ifndef PHASE15_TIME_E_
#define PHASE15_TIME_E_

#include <stdint.h>

/* Get number of microseconds passed */
extern unsigned int get_microseconds();

/* Get the number of cpu cycles corresponding to a given amount of ms */
extern unsigned int get_timer_value(unsigned int ms);

/* Time delay */
extern void delay_ms(unsigned int ms);

/* Set interval timer to the desired amount of cpy cycles */
extern void set_interval_timer(uint32_t cycles);

#endif
