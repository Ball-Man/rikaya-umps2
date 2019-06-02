#include <interrupt.e>
#include <interrupt.h>

#include <terminal.h>
#include <umps/libumps.h>
#include <umps/arch.h>
#include <sysbp.e>
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

/* Internal usage function, returns true(a number != 0) if there's an interrupt pending
 * on the given line from a specific device[0, 7]
 */
HIDDEN bool get_device_pending(uint8_t line, uint8_t device) {
  return (INTER_DEVICES(line) >> device) % 2; 
}

/* Inits the module(sempaphores to 0, mainly) */
extern void interrupt_init() {
  memset(dev_semaphores, 0, sizeof(dev_semaphores));
  memset(term_semaphores, 0, sizeof(term_semaphores));
}

/* Main handler for interrupts */
extern void interrupt() {
  uint8_t i = 0,
          j = 0;
  termreg_t *term;
  uint32_t rc_status,
           tr_status;

  /* Special interrupt lines */
  if (get_line_pending(1)) /* If the local timer interrupt is pending */
    scheduler();
  if (get_line_pending(2)) { /* The interval timer interrupt is pending */
    Veroghen(&clock_semaphore);
    clock_semaphore = 0;  /* Reset semaphore since it's only used for it's queue */
  }
  
  /* I/O devices' interrupt lines */
  for (i = 3; i < 7; i++)    /* Skipping line 7 since it has its own special management */
    if (get_line_pending(i)) {
      for (j = 0; j < 8; j++)
        if (get_device_pending(i, j)) {
          Veroghen(&dev_semaphores[j + (i - 3) * N_DEV_PER_IL]);
          dev_semaphores[j + (i - 3) * N_DEV_PER_IL] = 0;   /* Reset semaphore */
          ((dtpreg_t *)DEV_REG_ADDR(i, j))->command = DEV_ACK;
        }
    }

  /* Check interrupts for the terminal devices */
  if (get_line_pending(7))
    for (i = 0; i < 8; i++)
      if (get_device_pending(7, i)) {
        term = (termreg_t *)DEV_REG_ADDR(7, i);
        tr_status = (term->transm_status) & TERM_STATUS_MASK;
        rc_status = (term->recv_status) & TERM_STATUS_MASK;

        /* Update receive status */
        if (rc_status != TERM_ST_BUSY) {
          Veroghen(&term_semaphores[0][i]);
          term_semaphores[0][i] = 0;
          term->recv_command = DEV_ACK;
        }

        /* Update transmit status */
        if (tr_status != TERM_ST_BUSY) {
          Veroghen(&term_semaphores[1][i]);
          term_semaphores[1][i] = 0;
          term->transm_command = DEV_ACK;
        }
      }
}
