#ifndef PHASE1_SEMD_E_
#define PHASE1_SEMD_E_

#include <semd.h>
#include <lang.h>
#include <pcb.h>

/* Initilize the list of free semds with all the semd from the semd_table 
 * Also initalize the ASL as empty.
 */
extern void initASL();

/* Get the semd corresponding to the key(from the ASL) */ 
extern semd_t *getSemd(int *key);

/* Insert p in the queue of the semd corresponding to the key,
 * If key isn't found, a new semd is allocated in the ASL,
 * If the system already reached MAX_PROC semds return true
 * If everything is fine return false(weird but the doc says so).
 */
extern bool insertBlocked(int *key, pcb_t *p);

/* Remove and return the first element in the queue of
 * blocked processes corresponding to key,
 * If key isn't found, return NULL,
 * If the queue is empty after the removal, remove it
 * from ASL and put it back to the free list.
 */
extern pcb_t *removeBlocked(int *key);

/* Remove p from the semd queue its blocked on,
 * If p isn't found in the queue, return NULL
 * If everything is fine, return p.
 */
extern pcb_t *outBlocked(pcb_t *p);

/* Return (without removing) the first pcb blocked
 * in the semd queue corresponding to key.
 * If key isn't found or its queue is empty, return NULL.
 */
extern pcb_t *headBlocked(int *key);

/* Remove p and the hole tree radicated in p
 * from the semd queues they are (eventually) blocked on. 
 */
extern void outChildBlocked(pcb_t *p);

#endif
