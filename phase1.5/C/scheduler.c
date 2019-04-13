#include <scheduler.e>

#include <umps/libumps.h>
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
  term_printf("SCHEDULER\n");
  WAIT();
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
  new_proc->p_s.status = ST_PREV_INTERRUPTS | ST_LCL_TIMER | ST_IM_ALL;

  insertProcQ(&ready_queue, new_proc);

  return true;
}
