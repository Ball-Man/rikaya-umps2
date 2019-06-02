#ifndef PHASE2_SYSBP_E_
#define PHASE2_SYSBP_E_

#include <umps/types.h>

/* Wait_Clock semaphore */
extern int clock_semaphore;

/* Initialize the sysbp module */
extern void sysbp_init();

/* Main handler for system calls and breakpoints */
extern void sysbp();

/* Returns User time, Kernel time and Total time */
extern void Get_CPU_Time(unsigned int *user, unsigned int *kernel, unsigned int *wallclock);

/* Create a child process for the current process; PC and $SP are in statep, cpid contains
 * the child process' ID;
 * 0 if successfull, -1 otherwise */
extern int Create_Process(state_t *statep, int priority, void **cpid);

/* Handler for sys3: terminate current process and its tree */
extern void Terminate_Process();

/*  */
extern void Verhogen(int *semaddr);

/*  */
extern void Passeren(int *semaddr);

/* Waits a clock tick (100 ms) */
extern void Wait_Clock();

/* Activates an I/O operation inside the register field of the indicated device by coping the command parameter;
 * Blocking operation */
extern int Do_IO(unsigned int command, unsigned int *reg);

/* Current process now takes as children the parentless processes */
extern void Set_Tutor();

/*  */
extern int Spec_Passup(int type, state_t *old, state_t *new);

#endif
