#include <scheduler.e>
#include <scheduler.h>

#include <umps/libumps.h>
#include <umps/arch.h>
#include <lang.e>
#include <log.e>
#include <time.e>
#include <interrupt.h>
#include <pcb.e>
#include <terminal.e>
#include <const.h>

/* List of processes waiting. Its currently HIDDEN since in this phase there's no
 * need for it to be extern. Maybe this will change in the next phase
 */
HIDDEN struct list_head ready_queue;

/* Pcb of the process currently in execution */
struct list_head cur_proc;

/* Make empty queues for the scheduler */
extern void scheduler_init() {
  mkEmptyProcQ(&ready_queue);
  mkEmptyProcQ(&cur_proc);
}

/* Called every time a context switch is needed (quantum timeout) */
extern void scheduler() {
  pcb_t *current,
        *next;
  struct list_head *pos;
  if ((current = removeProcQ(&cur_proc))) {  /* If a process was being executed */
    /* Save state, reset priority and insert in the queue */
    memcpy((state_t *)INTERRUPT_OAREA, &current->p_s, sizeof(state_t));
    current->priority = current->original_priority;

    insertProcQ(&ready_queue, current);
  }

  next = removeProcQ(&ready_queue);

  /* Age all the other processes */
  list_for_each(pos, &ready_queue)
    list_entry(pos, pcb_t, p_next)->priority++;

  setTIMER(get_timer_value(TIMESLICE_MS));

  /* Log for tests */
  log_process_order(next->original_priority);

  /* Context switch */
  insertProcQ(&cur_proc, next);
  LDST(&next->p_s);
}

/* Add a process to the ready queue with the given priority */
extern bool scheduler_add(memaddr pc, int priority, uint8_t n) {
  pcb_t *new_proc = allocPcb();

  if (!new_proc)  /* If MAXPROC processes are already running */
    return false;

  /* Set priority */
  new_proc->priority = new_proc->original_priority = priority;

  /* Set state */
  new_proc->p_s.pc_epc = pc;
  new_proc->p_s.reg_sp = RAMTOP - FRAMESIZE * n;
  new_proc->p_s.status = ST_PREV_INTERRUPTS | ST_LCL_TIMER | (ST_IM_ALL - 0x8000);
  /* 0x8000 is the mask bit for terminal device interrupts. I'm masking it since
   * it's triggered by the terminal operations executed by the test functions.
   * Complete support for device interrupts is coming in the next phase.
   */

  insertProcQ(&ready_queue, new_proc);

  return true;
}
