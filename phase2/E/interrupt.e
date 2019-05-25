#ifndef PHASE15_INTERRUPT_E_
#define PHASE15_INTERRUPT_E_

#include <umps/arch.h>

/* Array of semaphores for the I/O queues
 * Lines: 3, ..., 6
 * Usage: dev_semaphores[device * (line - 3)] is the semaphore for the given device
 * and given line.
 * Note: line 7(terminals) is left behind and will be managed with a proper matrix
 * (since terminals have two internal devices each).
 */
extern int dev_semaphores[N_DEV_PER_IL * (N_EXT_IL - 1)];

/* Array of semaphores for the terminals' I/O queues.
 * Usage: term_semaphores[0|1][device] is the semaphore for the given terminal
 * (0 is for input terminals, 1 is for output ones).
 */
extern int term_semaphores[2][N_DEV_PER_IL];

/* Inits the module(sempaphores to 0, mainly) */
extern void interrupt_init();

/* Main handler for interrupts */
extern void interrupt();

#endif
