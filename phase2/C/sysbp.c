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
  /* Keeps the time spent in Kernel Mode */
  unsigned int kernel_start_time = get_microseconds();
	
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
  
  /* Updates process times */
  cur_proc->p_kernel_time += get_microseconds() - kernel_start_time;
  
  LDST(old_area);
}

/* Handlers for every system call... */
/* Returns User time, Kernel time and Total time */
extern void Get_CPU_Time (unsigned int *user, unsigned int *kernel, unsigned int *wallclock) {
	/* Kernel and User time stored, the sum is the total time */
	*kernel = cur_proc->p_kernel_time;
	*user = cur_proc->p_user_time;
	*wallclock = cur_proc->p_kernel_time + cur_proc->p_user_time;
}

/* Create a child process for the current process; PC and $SP are in statep, cpid contains
 * the child process' ID;
 * 0 if successfull, -1 otherwise */
extern int Create_Process (state_t *statep, int priority, void **cpid) {
	pcb_t *new_proc = allocPcb();
	if (!new_proc)
	  return -1;
	/* Set priority */
    new_proc->priority = new_proc->original_priority = priority; 
	
	/* Set state */
	new_proc->p_s = statep;
	
	/* Set start_time */
    new_proc->p_start_time = get_microseconds();
    
    if (!insertChild(cur_proc, new_proc))
      return -1;
    return 0;
}

/* Handler for sys3: terminate current process and its tree */
extern void Terminate_Process() {
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

/*  */
extern void Veroghen (int *semaddr) {
        semaddr +=1;
	pcb_t *process = removeBlocked(semaddr);
	
	if(process != NULL){
		process->p_semKey = NULL;
		insertProcQ(&ready_queue, process);
	}
	scheduler();
}

/*  */
extern void Passeren (int *semaddr) {
        if (semaddr>0)
        {
         semaddr -= 1;
	 insertBlocked(semaddr, cur_proc);
	 cur_proc->p_semKey = semaddr;
	 cur_proc = NULL;
	 scheduler();
        }
}

/* Waits a clock tick (100 ms) */
extern void Wait_Clock () {
	Passeren(0);
	delay_ms(100);
	Veroghen(0);
}

/* Activates an I/O operation inside the register field of the indicated device by coping the command parameter;
 * Blocking operation */
extern int Do_IO (unsigned int command, unsigned int *register) {
	
}

/* Current process now takes as children the parentless processes */
extern void Set_Tutor () {
	cur_tutor = cur_proc;
}

/*  */
extern int Spec_Passup (int type, state_t *old, state_t *new) {
	
}
