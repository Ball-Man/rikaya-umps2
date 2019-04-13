#ifndef PHASE15_SCHEDULER_E_
#define PHASE15_SCHEDULER_E_

#include <lang.h>
#include <list.h>

/* Queue made of maximum 1 element: the pcb of the process currently executing.
 * I'm using a list even if its just one element for a matter of consistency
 * when removing the process tree from the queues. 
 */
extern struct list_head cur_proc;

/* Make empty queues for the scheduler */
extern void scheduler_init();

/* Called every time a context switch is needed (quantum timeout)
 * If a process was running, update its state and insert it back in the queue
 * (This includes resetting its priority)
 * Remove the first process from the queue
 * Age all the remaining processes
 * Set local timer to the correct time slice
 * Log the context switch
 * Transfer control to the process.
 */
extern void scheduler();

/* Add a process to the ready queue with the given priority */
extern bool scheduler_add(memaddr pc, int priority, uint8_t n);

#endif
