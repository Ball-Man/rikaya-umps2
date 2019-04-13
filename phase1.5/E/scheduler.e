#ifndef PHASE15_SCHEDULER_E_
#define PHASE15_SCHEDULER_E_

#include <lang.h>

/* Called every time a context switch is needed (quantum timeout)
 * Remove the first process from the queue
 * Age all the remaining processes
 * Set local timer to the correct time slice
 * Transfer control to the process
 */
extern void scheduler();

/* Add a process to the ready queue with the given priority */
extern void scheduler_add(memaddr pc, int priority);

#endif
