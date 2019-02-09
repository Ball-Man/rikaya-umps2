#include <terminal.e>
#include <umps/arch.h>
#include <umps/libumps.h>
#include <lang.h>
#include <pcb.e>

void main(void)
{
  struct pcb_t *proc;

  term_sel(0);
  term_printf("PHASE 1\n");

  /* Tests */
  initPcbs();
  proc = allocPcb();
  term_printf("%s\n", list_empty(&proc->p_next) ? "OK" : "NOP");

  WAIT();
  *((uint32_t *) MCTL_POWER) = 0x0FF;
  while (1) ;
}
