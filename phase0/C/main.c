#include <terminal.e>
#include <umps/arch.h>
#include <umps/libumps.h>
#include <lang.h>

void main(void)
{
  term_sel(0);
  term_printstr("Cristian e' un cattivo ragazzo. Alessandro anche peggio\n");    

  WAIT();
  *((uint32_t *) MCTL_POWER) = 0x0FF;
  while (1) ;
}
