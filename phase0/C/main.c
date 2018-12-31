#include <tests.e>
#include <terminal.e>
#include <umps/arch.h>
#include <umps/libumps.h>
#include <lang.h>

void main(void)
{
  term_sel(0); /* Select standard output */

  term_printf("This program will execute a buch of reading examples from the tape device.\n\n");
  
  term_printf("*** First test:\n");
  test_read();

  term_printf("*** Second test:\n");
  test_index();  

  term_printf("\nFor additional info on what these tests are actually doing, read the docs.\n");

  WAIT();
  *((uint32_t *) MCTL_POWER) = 0x0FF;
  while (1) ;
}
