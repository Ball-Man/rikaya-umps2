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

  switch (old_area->reg_a0) {
    case SYS3:
      sys3();
      break;
  }

  old_area->pc_epc += WORD_SIZE;
  LDST(old_area);
}

/* Handler for sys3: terminate current process and its tree */
extern void sys3() {
  struct list_head q;
  struct list_head *pos;
  pcb_t *parent;

  mkEmptyProcQ(&q);

  list_add_tail(&outProcQ(&ready_queue, cur_proc)->p_next, &q);

  while ((parent = removeProcQ(&q))) {
    /* Check if the children queue is empty or not defined */
    if (!emptyProcQ(&parent->p_child) && parent->p_child.next)
      /* Cycle through the children and remove from them from the ready queue */
      list_for_each(pos, &parent->p_child)
        list_add_tail(
          &outProcQ(&ready_queue, list_entry(pos, pcb_t, p_sib))->p_next, &q);
  }

  /* At this point all the terminated processes may be freed with freePcb.
   * However, this function is going to work very differently in the next phase
   * so it probably doesn't matter too much.
   */

  /* Run the scheduler to carry on with the execution */
  scheduler();
}
