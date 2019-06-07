#include <interrupt.e>
#include <interrupt.h>

#include <terminal.h>
#include <umps/libumps.h>
#include <umps/arch.h>
#include <sysbp.e>
#include <lang.e>
#include <const.h>
#include <scheduler.e>

/* Array of pcbs which have already sent a command and are waiting for a response */
HIDDEN int dev_cur_semaphores[N_DEV_PER_IL * (N_EXT_IL - 1)];

/* Array of pcbs which have already sent a command(to a terminal and are waiting for a response) */
HIDDEN int term_cur_semaphores[2][N_DEV_PER_IL];

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
  /*return (INTER_DEVICES(line) >> device) % 2;*/
  return *((uint32_t *)INTER_DEVICES(line)) << (1 << device);
}

/* Inits the module(sempaphores to 0, mainly) */
extern void interrupt_init() {
  memset(dev_semaphores, 0, sizeof(dev_semaphores));
  memset(term_semaphores, 0, sizeof(term_semaphores));
  memset(dev_cur_semaphores, 0, sizeof(dev_cur_semaphores));
  memset(term_cur_semaphores, 0, sizeof(term_cur_semaphores));
}

/* Asks for an IO operation (proc is the sender of the request, reg is the requested device) */
extern void interrupt_io_command(pcb_t *proc, devreg_t *reg) {
  uint8_t offset = ((memaddr)reg - DEV_LINE_BASE) / DEV_REG_SIZE; /* For "standard" devices */
  uint8_t term_offset = ((memaddr)reg - TERM_LINE) / DEV_REG_SIZE;  /* For terminals */
  int *sem;

  /* Choose which semaphore the process will wait on */
  if ((memaddr)reg < TERM_LINE)   /* If the selected device is not a terminal */
    if (!dev_cur_semaphores[offset])
      sem = &dev_cur_semaphores[offset];
    else
      sem = &dev_semaphores[offset];
  else                            /* If it's a terminal */
    if (!term_cur_semaphores[proc->io_transm][term_offset])
      sem = &term_cur_semaphores[proc->io_transm][term_offset];
    else
      sem = &term_semaphores[proc->io_transm][term_offset];

  if (!*sem)
    *((uint32_t *)reg + 1 + 2 * proc->io_transm) = proc->io_command;
  if (proc == cur_proc)
    Passeren(sem);
  else
    vPasseren(sem, proc);
}

/* Main handler for interrupts */
extern void interrupt() {
  uint8_t i = 0,
          j = 0;
  termreg_t *term;
  dtpreg_t *dev;
  uint32_t rc_status,
           tr_status;
  pcb_t *freed_proc;

  /* Special interrupt lines */
  if (get_line_pending(1)) /* If the local timer interrupt is pending */
    scheduler();
  if (get_line_pending(2)) { /* The interval timer interrupt is pending */
    freed_proc = vVerhogen(&clock_semaphore);
    if (freed_proc)
      insertProcQ(&ready_queue, freed_proc);
  }
  
  /* I/O devices' interrupt lines */
  for (i = 3; i < 7; i++)    /* Skipping line 7 since it has its own special management */
    if (get_line_pending(i)) {
      for (j = 0; j < 8; j++)
        if (get_device_pending(i, j)) {
          dev = (dtpreg_t *)DEV_REG_ADDR(i, j);

          /* Free waiting process */
          if (dev_cur_semaphores[j + (i - 3) * N_DEV_PER_IL]) {
            freed_proc = vVerhogen(&dev_cur_semaphores[j + (i - 3) * N_DEV_PER_IL]);
            freed_proc->io_status = dev->status;
            insertProcQ(&ready_queue, freed_proc);
          }

          dev->command = DEV_CMD_ACK;
          while(dev->status != DEV_ST_READY); /* Wait for ack processing */

          /* Next waiting process */
          if (dev_semaphores[j + (i - 3) * N_DEV_PER_IL]) {
            freed_proc = vVerhogen(&dev_semaphores[j + (i - 3) * N_DEV_PER_IL]);
            interrupt_io_command(freed_proc, (devreg_t *)DEV_REG_ADDR(i, j));
          }

          /* TODO: return status code (field in pcb_t + scheduler management)*/
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
        if (rc_status != TERM_ST_BUSY && rc_status != DEV_ST_READY
          && rc_status != DEV_ST_NOT_INSTALLED) {

          /* Free waiting process */
          if (term_cur_semaphores[0][i]) {
            freed_proc = vVerhogen(&term_cur_semaphores[0][i]);
            freed_proc->io_status = term->recv_status;
            insertProcQ(&ready_queue, freed_proc);
          }

          term->recv_command = DEV_CMD_ACK;
          while((rc_status = (term->recv_status) & TERM_STATUS_MASK) != DEV_ST_READY); /* Wait for ack processing */

          /* Next waiting process */
          if (term_semaphores[0][i]) {
            freed_proc = vVerhogen(&term_semaphores[0][i]);
            interrupt_io_command(freed_proc, (devreg_t *)DEV_REG_ADDR(7, i));
          }
        }

        /* Update transmit status */
        if (tr_status != TERM_ST_BUSY && tr_status != DEV_ST_READY 
          && tr_status != DEV_ST_NOT_INSTALLED) {
          /* Free waiting process */
          if (term_cur_semaphores[1][i]) {
            freed_proc = vVerhogen(&term_cur_semaphores[1][i]);
            freed_proc->p_s.reg_v0 = term->transm_status;
            insertProcQ(&ready_queue, freed_proc);
          }
          term->transm_command = DEV_CMD_ACK;
          while((tr_status = (term->transm_status) & TERM_STATUS_MASK) != DEV_ST_READY); /* Wait for ack processing */

          /* Next waiting process */
          if (term_semaphores[1][i]) {
            freed_proc = vVerhogen(&term_semaphores[1][i]);
            interrupt_io_command(freed_proc, (devreg_t *)DEV_REG_ADDR(7, i));
          }
        }
      }

  state_t *old_area = (state_t *)INTERRUPT_OAREA;
  LDST(old_area);
}
