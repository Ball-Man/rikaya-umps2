#include <interrupt.e>
#include <interrupt.h>

#include <terminal.e>
#include <umps/libumps.h>
#include <umps/arch.h>
#include <lang.h>
#include <const.h>

/* Internal usage function, returns true(a number != 0) if there's an interrupt
 * pending on the given line.
 */
HIDDEN bool get_line(uint8_t line) {
  state_t *old_area = (state_t *)INTERRUPT_OAREA;

  return (old_area->cause >> 8) & (1 << line);  /* Brackets not needed, but help understanding */
}

/* Main handler for interrupts */
extern void interrupt() {
  state_t *old_area = (state_t *)INTERRUPT_OAREA;

  term_printf("INTERRUPT\nCause: %d\n", old_area->cause);

  old_area->pc_epc += WORD_SIZE;
  LDST(old_area);
}
