#ifndef PHASE15_TIME_E_
#define PHASE15_TIME_E_

/* Get number of microseconds passed */
extern unsigned int get_microseconds();

/* Get the number of cpu cycles corresponding to a given amount of ms */
extern unsigned int get_timer_value(unsigned int ms);

/* Time delay */
extern void delay_ms(unsigned int ms);

#endif
