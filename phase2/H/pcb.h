#ifndef PHASE1_PCB_H_
#define PHASE1_PCB_H_

#include <umps/types.h>
#include <list.h>
#include <lang.h>

/* Process Control Block */
typedef struct pcb_t {
  struct list_head p_next;
  struct pcb_t* p_parent;
  struct list_head p_child;
  struct list_head p_sib;

  /* Scheduling and semaphores */
  int priority;
  int original_priority;
  state_t p_s;
  int *p_semKey;

  /* Time */
  unsigned int p_kernel_time;
  unsigned int p_user_time;

  /* I/O management */
  uint32_t io_command;
  uint8_t io_transm;
  uint32_t io_status;
} pcb_t;

#endif
