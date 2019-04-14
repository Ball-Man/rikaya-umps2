#include <time.e>
#include <time.h>

#include <lang.h>

/* Get number of microseconds passed */
extern unsigned int get_microseconds()
{
  return TOD_LO / TIME_SCALE;
}

/* Time delay */
extern void delay_ms(unsigned int ms)
{
  unsigned int start = get_microseconds();

  while (get_microseconds() - start <= ms * 1000)
    ;
}
