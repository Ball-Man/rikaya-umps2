#include <sysbp.e>
#include <sysbp.h>

#include <terminal.e>
#include <interrupt.e>
#include <umps/libumps.h>
#include <umps/arch.h>
#include <scheduler.e>
#include <lang.e>
#include <time.e>
#include <semd.e>
#include <pcb.e>

/* Wait_Clock semaphore */
int clock_semaphore;

/* Initialize the sysbp module */
extern void sysbp_init() {
  clock_semaphore = 0;
}

/* Handlers for every system call... */
/* Returns User time, Kernel time and Total time */
HIDDEN void Get_CPU_Time(unsigned int *user, unsigned int *kernel, unsigned int *wallclock) {
	/* Kernel and User time stored, the sum is the total time */
	*kernel = cur_proc->p_kernel_time;
	*user = cur_proc->p_user_time;
	*wallclock = cur_proc->p_kernel_time + cur_proc->p_user_time;
}

/* Create a child process for the current process; PC and $SP are in statep, cpid contains
 * the child process' ID;
 * 0 if successfull, -1 otherwise */
HIDDEN int Create_Process(state_t *statep, int priority, void **cpid) {
	pcb_t *new_proc = allocPcb();

  if (cpid)
    *cpid = new_proc;

	if (!new_proc)
	  return -1;
	/* Set priority */
  new_proc->priority = new_proc->original_priority = priority;

	/* Set state */
	memcpy(statep, &new_proc->p_s, sizeof(state_t));

	/* Set start_time */
  /* new_proc->p_start_time = get_microseconds(); */
  insertChild(cur_proc, new_proc);
  insertProcQ(&ready_queue, new_proc);
  return 0;
}

/* Handler for sys3: terminate current process and its tree */
HIDDEN void Terminate_Process() {
  struct list_head q;
  struct list_head *pos;
  pcb_t *parent;

  mkEmptyProcQ(&q);

  /* Add the current executing process to the queue, and remove from execution */
  list_add_tail(&outProcQ(&ready_queue, cur_proc)->p_next, &q);
  cur_proc = NULL;

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

/* Virtual Verhogen: returns the freed pcb */
extern pcb_t *vVerhogen(int *semaddr) {
  pcb_t *process;

  *semaddr += 1;

  if (*semaddr <= 0) {
    /* Wake up one process and reset process priority */
    process = removeBlocked(semaddr);
    process->priority = process->original_priority;
    return process;
  }

  return NULL;
}

/* Verhogen: free a pcb from semaphore */
extern void Verhogen(int *semaddr) {
	pcb_t *process = vVerhogen(semaddr);   /* Use virtual verhogen */

  if (process)
    insertProcQ(&ready_queue, process);
  /* scheduler();     NOTE: we're not giving away our quantum */
}

/* Virtual Passeren: (eventually) block the given pcb on the semaphore */
extern bool vPasseren(int *semaddr, pcb_t* proc) {
  *semaddr -= 1;

  if (*semaddr < 0) {
    /* (eventually) remove proc from the ready queue and push it  */
    outProcQ(&ready_queue, proc);
    insertBlocked(semaddr, proc);
    return true;
  }

  return false;
}

/* Passeren: require a semaphore */
extern void Passeren(int *semaddr) {
  state_t *old_area = (state_t *)SYSBP_OAREA;
  bool blocked = vPasseren(semaddr, cur_proc);  /* Use virtual passeren */

  if (blocked) {
    memcpy(old_area, &cur_proc->p_s, sizeof(state_t));
    cur_proc->p_s.pc_epc += WORD_SIZE;
    cur_proc = NULL;

    /* Schedule new process */
    scheduler();
  }
}

/* Waits a clock tick (100 ms) */
extern void Wait_Clock() {
	Passeren(0);
	delay_ms(100);
	Verhogen(0);
}

/* Activates an I/O operation inside the register field of the indicated device by coping the command parameter;
 * Blocking operation
 * NOTE: transm is used to select between the two subdevices inside the terminal device.
 *       transm = 0 means the transmitting subdevice is selected.
 *       transm = 1 means the receiving subdevice is selected.
 *       any other value will break everything
 */
HIDDEN void Do_IO(unsigned int command, unsigned int *reg, uint8_t transm) {
  cur_proc->io_command = command;
  cur_proc->io_transm = 1 - transm;
  interrupt_io_command(cur_proc, (devreg_t *)reg);
}

/* Current process now takes as children the parentless processes */
HIDDEN void Set_Tutor() {
}

/*  */
HIDDEN int Spec_Passup(int type, state_t *old, state_t *new) {

}

/* Main handler for system calls and breakpoints
 * NOTE: Breakpoints not handled in this phase
 */
extern void sysbp() {
  /* Keeps the time spent in Kernel Mode */
  unsigned int kernel_start_time = get_microseconds();

  state_t *old_area = (state_t *)SYSBP_OAREA;

  uint32_t ret = 0;
  /* Register a0 tells us which syscall is being called
   * For a complete list, see sysbp.h
   */
  switch (old_area->reg_a0) {
    case CREATEPROCESS:
      ret = Create_Process((state_t *)old_area->reg_a1, (int)old_area->reg_a2, (void **)old_area->reg_a3);
      break;

    case PASSEREN:
      Passeren((int *)old_area->reg_a1);
      break;

    case VERHOGEN:
      Verhogen((int *)old_area->reg_a1);
      break;

    case WAITIO:
      Do_IO((uint32_t)old_area->reg_a1, (uint32_t *)old_area->reg_a2, (uint8_t)old_area->reg_a3);
      break;
  }

  old_area->reg_v0 = ret;
  old_area->pc_epc += WORD_SIZE;

  /* Updates process times */
  cur_proc->p_kernel_time += get_microseconds() - kernel_start_time;

  LDST(old_area);
}
