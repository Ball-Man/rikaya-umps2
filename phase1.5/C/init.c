#include <init.e>

#include <umps/types.h>
#include <sysbp.h>

/* Initialize the given New Area with the given handler*/
extern void init_newarea(memaddr area, memaddr handler) {
  state_t *narea = (state_t *)area;

  narea->pc_epc = (memaddr)handler;
  narea->reg_sp = RAMTOP;
  narea->status = ST_PREV_INTERRUPTS | ST_LCL_TIMER | ST_CP0;
}
