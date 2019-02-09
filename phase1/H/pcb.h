#ifndef PHASE1_PCB_H_
#define PHASE1_PCB_H_

#include <list.h>
#include <lang.h>

#define state_t uint32_t

/* Process Control Block */
typedef struct pcb_t {
  struct list_head p_next;
  struct pcb_t* p_parent;
  struct list_head p_child;
  struct list_head p_sib;
  state_t p_s;
  int priority;
  int *p_semKey;
} pcb_t;

#endif
