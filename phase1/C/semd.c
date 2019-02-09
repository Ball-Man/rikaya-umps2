#include <semd.e>

#include <list.h>
#include <const.h>

/* Sentinel for the list of free semds */
HIDDEN struct list_head semdfree_h;

/* Sentinel for the list of active semaphores */
HIDDEN struct list_head semd_h;

/* Array containing all the semds.
 * For details, read the description of pcb_table in pcb.c
 */
HIDDEN semd_t semd_table[MAX_PROC];

/* Initialize list of free semds allocating MAX_PROC of them */
extern void initASL() {
  uint8_t i;

  INIT_LIST_HEAD(&semdfree_h);   /* Initialize sentinel */
  INIT_LIST_HEAD(&semd_h);

  /* Add all the semds to the list */
  for (i = 0; i < MAX_PROC; i++)
    list_add(&((semd_table + i)->s_next), &semdfree_h);
}

/* Get the semd corresponding to the key(from the ASL) */ 
extern semd_t *getSemd(int *key) {
  struct list_head *pos;
  semd_t *p;

  list_for_each(pos, &semd_h) {    /* Search through the list */
    p = list_entry(pos, semd_t, s_next);
    if (p->s_key == key)
      return p;
  }
  return NULL;    /* If key isn't found */
}


