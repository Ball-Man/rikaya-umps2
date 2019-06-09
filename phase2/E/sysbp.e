#ifndef PHASE2_SYSBP_E_
#define PHASE2_SYSBP_E_

#include <umps/types.h>
#include <pcb.h>

/* Wait_Clock semaphore */
extern int clock_semaphore;

extern state_t *spec_oarea[3];
extern state_t *spec_narea[3];
extern bool spec_set[3];

/* Initialize the sysbp module */
extern void sysbp_init();

/* Main handler for system calls and breakpoints */
extern void sysbp();

/* Terminates given process */
extern int Terminate_Process(void **pid);

/* Virtual Verhogen: returns the freed pcb */
extern pcb_t *vVerhogen(int *semaddr);

/* Virtual Passeren: (eventually) block the given pcb on the semaphore */
extern bool vPasseren(int *semaddr, pcb_t *proc);

/* Verhogen: free a pcb from semaphore */
extern void Verhogen(int *semaddr);

/* Passeren: require a semaphore */
extern void Passeren(int *semaddr);

#endif
