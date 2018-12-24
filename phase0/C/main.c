#include <terminal.e>
#include <umps/arch.h>
#include <umps/libumps.h>
#include <lang.h>

void main(void)
{
  term_sel(0);
  term_printf("This is a printf test. I'm gonna print:\nOne: %d,\nTen: %d,\nFourtytwo: %d,\nA string: %s\n", 1, 10, 42, "ligma");

  WAIT();
  *((uint32_t *) MCTL_POWER) = 0x0FF;
  while (1) ;
}
