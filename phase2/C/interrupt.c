#include <interrupt.e>
#include <interrupt.h>

#include <terminal.e>
#include <umps/libumps.h>
#include <lang.e>
#include <const.h>
#include <scheduler.e>

/* Array of semaphores for the I/O queues,
 * See interrupt.e for more info.
 */
int dev_semaphores[N_DEV_PER_IL * (N_EXT_IL - 1)];

/* Array of semaphores for the terminals' I/O queues,
 * See interrupt.e for more info.
 */
int term_semaphores[2][N_DEV_PER_IL];

/* Internal usage function, returns true(a number != 0) if there's an interrupt
 * pending on the given line.
 */
HIDDEN bool get_line_pending(uint8_t line) {
  state_t *old_area = (state_t *)INTERRUPT_OAREA;

  return (old_area->cause >> 8) & (1 << line);  /* Brackets not needed, but help understanding */
}

/* Inits the module(sempaphores to 0, mainly) */
extern void interrupt_init() {
  memset(dev_semaphores, 0, sizeof(dev_semaphores));
  memset(term_semaphores, 0, sizeof(term_semaphores));
}

/* Main handler for interrupts */
extern void interrupt() {
  if (get_line_pending(1)) /* If the local timer interrupt is pending */
    scheduler();
}
