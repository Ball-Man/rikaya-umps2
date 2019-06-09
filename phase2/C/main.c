#include <umps/arch.h>
#include <umps/types.h>
#include <umps/libumps.h>
#include <p2test_rikaya.e>
#include <terminal.e>
#include <scheduler.e>
#include <init.e>
#include <sysbp.e>
#include <interrupt.e>
#include <pcb.e>
#include <semd.e>
#include <sysbp.h>
#include <interrupt.h>
#include <lang.e>
#include <list.h>
#include <const.h>
#include <sysbp.h>

/* Root process */
void system() {
  state_t test_s;

  /* Setup and create test process */
  memset(&test_s, 0, sizeof(test_s));
  test_s.pc_epc = (memaddr)test;
  test_s.reg_sp = RAMTOP - FRAMESIZE * 2;  /* First stack is for the system process */
  test_s.status = ST_PREV_INTERRUPTS | ST_LCL_TIMER | ST_IM_ALL;
  SYSCALL(SETTUTOR, 0, 0, 0);
  SYSCALL(CREATEPROCESS, (int)&test_s, 1, 0);
  
  while (1);
}

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

  /* Init sysbp module */
  sysbp_init();

  /* Init interrupt module */
  interrupt_init();

  /* Init scheduler */
  scheduler_init();
  scheduler_add((memaddr)system, 1);
  scheduler();

  while (1);
}
