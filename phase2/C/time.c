#include <time.e>
#include <time.h>

#include <lang.h>

/* Get number of microseconds passed */
extern unsigned int get_microseconds()
{
  return TOD_LO / TIME_SCALE;
}

/* Get the number of cpu cycles corresponding to a given amount of ms */
extern unsigned int get_timer_value(unsigned int ms) {
  return ms * 1000 * TIME_SCALE;
}

/* Time delay */
extern void delay_ms(unsigned int ms)
{
  unsigned int start = get_microseconds();

  while (get_microseconds() - start <= ms * 1000)
    ;
}

/* Set interval timer to the desired amount of cpy cycles */
extern void set_interval_timer(uint32_t cycles) {
  *((uint32_t *)INTERVAL_TIMER) = cycles;
}
