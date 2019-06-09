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
  /* Update time before returning it */
  cur_proc->p_kernelt_total += TOD_LO - cur_proc->p_kernelt_start;
  cur_proc->p_kernelt_start = TOD_LO;
	
  if (user)
    *user = cur_proc->p_usert_total;

  if (kernel)
    *kernel = cur_proc->p_kernelt_total;

  if (wallclock)
    *wallclock = TOD_LO - cur_proc->p_wallclock_start;
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

  /* Time management */
  new_proc->p_wallclock_start = TOD_LO;

  insertChild(cur_proc, new_proc);
  insertProcQ(&ready_queue, new_proc);
  return 0;
}

/* Terminates the process corresponding to the given pid. if NULL(0), the target is
 * the current executing process.
 * The function is successful only if the target process is a descendant of cur_proc.
 */
extern int Terminate_Process(void **pid) {
  pcb_t *proc = (pid) ? *pid : cur_proc;
  pcb_t *parent = NULL;
  bool allowed = false;

  if (!proc->p_parent)  /* If the current process has no parent(meaning it's the */
    return -1;          /* root process, return an error status. */
  
  /* Check if the target process is a descendant of cur_proc */
  if (proc == cur_proc)
    allowed = true;
  else
    while ((parent = proc->p_parent))
      if (parent == cur_proc)
        allowed = true;

  if (!allowed)   /* If not, return an error */
    return -1;

  parent = proc;
  /* Search for a tutor */
  while (!(parent = parent->p_parent)->tutor);
  /* 'parent' now contains a pointer to a tutor pcb(root process is always tutor) */

  outChild(proc);
  list_splice(&parent->p_child, &proc->p_child);
  freePcb(outProcQ(&ready_queue, proc));

  if (proc == cur_proc) {
    cur_proc = NULL;
    scheduler();
  }

  return 0;
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

    /* Time management */
    cur_proc->p_kernelt_total += TOD_LO - cur_proc->p_kernelt_start;
    cur_proc->p_kernelt_start = 0;

    cur_proc = NULL;

    /* Schedule new process */
    scheduler();
  }
}

/* Waits a clock tick (100 ms) */
extern void Wait_Clock() {
  Passeren(&clock_semaphore); 
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
  cur_proc->tutor = true;
}

/* Assign a superior level handler for a specific exception(type).
 * Should be called maximum once per type. Returns 0 on success, -1 on error. 
 */
HIDDEN int Spec_Passup(int type, state_t *old, state_t *new) {
  if (cur_proc->spec_set[type])
    return -1;
  cur_proc->spec_set[type] = true;  /* Can be called one time per type */
  
  cur_proc->spec_oarea[type] = old;
  cur_proc->spec_narea[type] = new;

  return 0;
}

/* Get current process' pid and its parent's pid */
HIDDEN void Get_pid_ppid(void **pid, void **ppid) {
  if (pid)
    *pid = cur_proc;
  if (ppid)
    *ppid = cur_proc->p_parent;
}

/* Main handler for system calls and breakpoints
 * NOTE: Breakpoints not handled in this phase
 */
extern void sysbp() {
  state_t *old_area = (state_t *)SYSBP_OAREA;

  /* Time management */
  if (cur_proc->p_usert_start) {
    cur_proc->p_usert_total += TOD_LO - cur_proc->p_usert_start;
    cur_proc->p_usert_start = 0;
  }
  cur_proc->p_kernelt_start = TOD_LO;

  /* Distinguish between syscall and bp */

  /* If it's a breakpoint, call superior handler */
  if (((old_area->cause >> 2) & 0x1F) == CAUSE_BP) {
    if (!cur_proc->spec_set[SPEC_TYPE_SYSBP])
      Terminate_Process(0);
    memcpy(old_area, cur_proc->spec_oarea[SPEC_TYPE_SYSBP], sizeof(state_t));
    LDST(cur_proc->spec_narea[SPEC_TYPE_SYSBP]);
  }

  /* If it's a syscall... */
  /* Syscall return management */
  uint32_t ret = 0;

  /* Register a0 tells us which syscall is being called
   * For a complete list, see sysbp.h
   */
  switch (old_area->reg_a0) {
    case GETCPUTIME:
      Get_CPU_Time((uint32_t *)old_area->reg_a1, (uint32_t *)old_area->reg_a2, (uint32_t *)old_area->reg_a3);
      break;

    case CREATEPROCESS:
      ret = Create_Process((state_t *)old_area->reg_a1, (int)old_area->reg_a2, (void **)old_area->reg_a3);
      break;

    case TERMINATEPROCESS:
      ret = Terminate_Process((void **)old_area->reg_a1);

    case PASSEREN:
      Passeren((int *)old_area->reg_a1);
      break;

    case VERHOGEN:
      Verhogen((int *)old_area->reg_a1);
      break;

    case WAITCLOCK:
      Wait_Clock();
      break;

    case WAITIO:
      Do_IO((uint32_t)old_area->reg_a1, (uint32_t *)old_area->reg_a2, (uint8_t)old_area->reg_a3);
      break;

    case SETTUTOR:
      Set_Tutor();
      break;

    case SPECPASSUP:
      Spec_Passup((int)old_area->reg_a1, (state_t *)old_area->reg_a2, (state_t *)old_area->reg_a3);
      break;

    case GETPID:
      Get_pid_ppid((void **)old_area->reg_a1, (void **)old_area->reg_a2);
      break;

    /* Superior level handler */
    default:
      if (!cur_proc->spec_set[SPEC_TYPE_SYSBP])
        Terminate_Process(0);

      memcpy(old_area, cur_proc->spec_oarea[SPEC_TYPE_SYSBP], sizeof(state_t));
      LDST(cur_proc->spec_narea[SPEC_TYPE_SYSBP]);
      break;
  }

  old_area->reg_v0 = ret;
  old_area->pc_epc += WORD_SIZE;

  /* Time management */
  cur_proc->p_kernelt_total += TOD_LO - cur_proc->p_kernelt_start;
  cur_proc->p_kernelt_start = 0;
  cur_proc->p_usert_start = TOD_LO;

  /* Return to execution */
  LDST(old_area);
}
