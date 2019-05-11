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
extern bool emptyProcQ(struct list_head *list_head);

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

/* Returns true if p has no child */
extern bool emptyChild(pcb_t *p);

/* Inserts p as child of prnt */
extern void insertChild(pcb_t *prnt, pcb_t *p);

/* Removes and returns the first child of p. Returns NULL if it has no child */
extern pcb_t *removeChild(pcb_t *p);

/* Removes p from the list of children of its parent,
 * If p has no parent, returns NULL,
 * If p has a parent it returns itself.
 */
extern pcb_t *outChild(pcb_t *p);

/* Gets the pcb unitary offset from the internal pcb table(used to calculate stack pointer) */
extern uint8_t getPcbOffset(pcb_t *p);

#endif
