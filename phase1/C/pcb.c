#include <pcb.e>

#include <lang.h>
#include <const.h>

/* Sentinel for the list of free pcbs */
HIDDEN struct list_head pcbfree_h;

/* Array containing all the pcbs. We can't
 * allocate things on the heap since we are
 * working with extra low level stuff, so we
 * need to allocate all the pcbs statically 
 */
HIDDEN pcb_t pcbFree_table[MAX_PROC];

/* Initialize list of free pcbs allocating MAX_PROC of them */
void initPcbs() {
  uint8_t i;

  INIT_LIST_HEAD(&pcbfree_h);   /* Initialize sentinel */

  /* Add all the pcbs to the list */
  for (i = 0; i < MAX_PROC; i++)
    list_add(&((pcbFree_table + i)->p_next), &pcbfree_h);
}

/* Put p in the list of free pcbs */
void freePcb(pcb_t *p) {
  list_add(&(p->p_next), &pcbfree_h);
}

/* Remove and return one pcb from the free list */
pcb_t *allocPcb() {
  struct list_head *alloc_head;
  pcb_t *alloc;

  if (list_empty(&pcbfree_h))
    return NULL;

  /* Remove from list */
  alloc_head = pcbfree_h.next;
  list_del(alloc_head);
  alloc = list_entry(alloc_head, pcb_t, p_next);
  
  /* Initialize pcb */
  /* List init probably not needed after all but better safe than sorry,
   * right?
   */
  INIT_LIST_HEAD(&alloc->p_next);
  alloc->p_parent = NULL;
  INIT_LIST_HEAD(&alloc->p_child);
  INIT_LIST_HEAD(&alloc->p_sib);
  alloc->p_s = 0;
  alloc->priority = 0;
  alloc->p_semKey = NULL;

  return alloc;
}
