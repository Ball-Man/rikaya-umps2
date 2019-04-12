#include <sysbp.e>
#include <sysbp.h>

#include <terminal.e>
#include <umps/libumps.h>
#include <umps/arch.h>

/* Main handler for system calls and breakpoints */
extern void sysbp() {
  state_t *old_area = (state_t *)SYSBP_OAREA;

  term_printf("SYSCALL/BP\nCause: %d\nNumber: %d\n", old_area->cause, old_area->reg_a0);

  old_area->pc_epc += WORD_SIZE;
  LDST(old_area);
}
