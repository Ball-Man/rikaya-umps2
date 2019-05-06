#include <umps/arch.h>
#include <umps/types.h>
#include <umps/libumps.h>
#include <terminal.e>
#include <scheduler.e>
#include <init.e>
#include <sysbp.e>
#include <interrupt.e>
#include <pcb.e>
#include <semd.e>
#include <sysbp.h>
#include <interrupt.h>
#include <lang.h>
#include <list.h>
#include <const.h>
#include <sysbp.h>

/* Entry point */
void main() {
  /* Init pcbs */
  initPcbs();
  initASL();

  /* DEBUG */
  term_sel(0);

  /* Init New Areas */
  init_newarea(SYSBP_NAREA, (memaddr)sysbp);
  init_newarea(INTERRUPT_NAREA, (memaddr)interrupt);
  
  /* Init scheduler */
  scheduler_init();

  term_printf("INIT OK\n");
  while (1);
}
