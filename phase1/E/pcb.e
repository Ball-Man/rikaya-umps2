#ifndef PHASE1_PCB_E_
#define PHASE1_PCB_E_

#include <pcb.h>

/* Initialize list of free pcbs allocating MAX_PROC of them */
extern void initPcbs();

/* Put p in the list of free pcbs */
extern void freePcb(pcb_t *p);

/* Remove and return one pcb from the free list
 * (and initialize it with null values)
 */
extern pcb_t *allocPcb();

/* Initialize the list of pcbs, initializing the list dummy */
extern void mkEmptyProcQ(struct list_head *list_head);

/* Returns true if the list is empty, false otherwise */
extern int emptyProcQ(struct list_head *list_head);

/* Insert the element in the queue, sorting in descending order
 * of the priority of the pcbs
 */
extern void insertProcQ(struct list_head *list_head, pcb_t *p);

/* Returns the first element without removing it. Null if list is empty */
extern pcb_t *headProcQ(struct list_head *list_head);

/* Returns the first element removing it. Null if list is empty */
extern pcb_t *removeProcQ(struct list_head *list_head);

/* Removes the pcb from the queue, Null if absent */
extern pcb_t *outProcQ(struct list_head *list_head, pcb_t *p);

#endif
