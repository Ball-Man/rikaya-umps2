#ifndef PHASE15_INIT_E_
#define PHASE15_INIT_E_

#include <const.h>
#include <lang.h>
#include <list.h>

/* Initialize the given New Area */
void init_newarea(memaddr area, memaddr handler);

/* Initialize the given ready queue with the given processes */
void init_ready(struct list_head *queue, memaddr proc1, memaddr proc2, memaddr proc3);

#endif
