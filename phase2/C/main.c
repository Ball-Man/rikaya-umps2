#include <umps/arch.h>
#include <umps/types.h>
#include <umps/libumps.h>
/*#include <p2test_rikaya_v0.e>*/
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

int empty = 1;
int full = 0;
int p = 0;

void prod() {
  while (1) {
    SYSCALL(SYS5, (uint32_t)&empty, 0, 0);
    p++;
    term_printf("prod: %d\n", p);
    SYSCALL(SYS4, (uint32_t)&full, 0, 0);
  }
}

void cons() {
  while (1) {
    SYSCALL(SYS5, (uint32_t)&full, 0, 0);
    p--;
    term_printf("cons: %d\n", p);
    SYSCALL(SYS4, (uint32_t)&empty, 0, 0);
  }
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
  scheduler_add((memaddr)prod, 1);
  scheduler_add((memaddr)cons, 1);
  scheduler();

  while (1);
}
