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
  bool tutor;

  /* Scheduling and semaphores */
  int priority;
  int original_priority;
  state_t p_s;
  int *p_semKey;

  /* Time */
  uint32_t p_wallclock_start;
  uint32_t p_usert_start;
  uint32_t p_usert_total;
  uint32_t p_kernelt_start;
  uint32_t p_kernelt_total;

  /* I/O management */
  uint32_t io_command;
  uint8_t io_transm;

  /* Trap vectors */
  state_t *spec_oarea[3];
  state_t *spec_narea[3];
  bool spec_set[3];
} pcb_t;

#endif
