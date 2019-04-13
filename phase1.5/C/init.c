#include <init.e>

#include <umps/types.h>
#include <sysbp.h>
#include <scheduler.e>

/* Initialize the given New Area with the given handler*/
extern void init_newarea(memaddr area, memaddr handler) {
  state_t *narea = (state_t *)area;

  narea->pc_epc = (memaddr)handler;
  narea->reg_sp = RAMTOP;
  narea->status = ST_PREV_INTERRUPTS | ST_LCL_TIMER | ST_CP0;
}

/* Initialize the given ready queue with the given processes */
void init_ready(memaddr proc1, memaddr proc2, memaddr proc3) {
  scheduler_add(proc1, 1, 1);
  scheduler_add(proc2, 2, 2);
  scheduler_add(proc3, 3, 3);

  /* Start the scheduler */
  scheduler();
}
