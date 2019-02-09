#ifndef PHASE1_PCB_E_
#define PHASE1_PCB_E_

#include <pcb.h>

/* Initialize list of free pcbs allocating MAX_PROC of them */
void initPcbs();

/* Put p in the list of free pcbs */
void freePcb(pcb_t *p);

/* Remove and return one pcb from the free list
 * (and initialize it with null values)
 */
pcb_t *allocPcb();

#endif
