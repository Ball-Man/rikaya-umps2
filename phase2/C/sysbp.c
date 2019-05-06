#include <sysbp.e>
#include <sysbp.h>

#include <terminal.e>
#include <umps/libumps.h>
#include <umps/arch.h>
#include <scheduler.e>
#include <pcb.e>

/* Main handler for system calls and breakpoints
 * NOTE: Breakpoints not handled in this phase
 */
extern void sysbp() {
  state_t *old_area = (state_t *)SYSBP_OAREA;

  /* Register a0 tells us which syscall is being called
   * For a complete list, see sysbp.h
   */
  switch (old_area->reg_a0) {
    /* case SYS1: ...
     * case SYS2: ...
     */
  }

  old_area->pc_epc += WORD_SIZE;
  LDST(old_area);
}

/* Handlers for every system call... */
