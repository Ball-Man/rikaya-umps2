#include <tests.e>
#include <terminal.e>
#include <umps/arch.h>
#include <umps/libumps.h>
#include <lang.h>

void main(void)
{
  term_sel(0); /* Select standard output */

  term_printf("This program will execute a buch of reading examples from the tape device.\n\n");

  test_read();

  WAIT();
  *((uint32_t *) MCTL_POWER) = 0x0FF;
  while (1) ;
}
