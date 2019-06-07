#ifndef PHASE2_SYSBP_E_
#define PHASE2_SYSBP_E_

#include <umps/types.h>
#include <pcb.h>

/* Wait_Clock semaphore */
extern int clock_semaphore;

/* Initialize the sysbp module */
extern void sysbp_init();

/* Main handler for system calls and breakpoints */
extern void sysbp();

/* Virtual Verhogen: returns the freed pcb */
extern pcb_t *vVerhogen(int *semaddr);

/* Virtual Passeren: (eventually) block the given pcb on the semaphore */
extern bool vPasseren(int *semaddr, pcb_t *proc);

/* Verhogen: free a pcb from semaphore */
extern void Verhogen(int *semaddr);

/* Passeren: require a semaphore */
extern void Passeren(int *semaddr);

#endif
