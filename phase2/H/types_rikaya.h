#ifndef _TYPES11_H
#define _TYPES11_H

#include <umps/types.h>
#include "list.h"
#include "const.h"

typedef unsigned int memaddr;

/* Semaphore Descriptor (SEMD) data structure */
typedef struct semd_t {
	struct list_head	s_next;
	
	/* Semaphore key */
	int					*s_key;
	
	/* Queue of PCBs blocked on the semaphore */
	struct list_head	s_procQ;
} semd_t;

typedef struct semdev {
	 semd_t disk[DEV_PER_INT];
	 semd_t tape[DEV_PER_INT];
	 semd_t network[DEV_PER_INT];
	 semd_t printer[DEV_PER_INT];
	 semd_t terminalR[DEV_PER_INT];
	 semd_t terminalT[DEV_PER_INT];
} semdev;

#endif
