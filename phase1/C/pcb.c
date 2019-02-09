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
HIDDEN pcb_t pcb_table[MAX_PROC];

/* Initialize list of free pcbs allocating MAX_PROC of them */
extern void initPcbs() {
  uint8_t i;

  INIT_LIST_HEAD(&pcbfree_h);   /* Initialize sentinel */

  /* Add all the pcbs to the list */
  for (i = 0; i < MAX_PROC; i++)
    freePcb((pcb_table + i));
}

/* Put p in the list of free pcbs */
extern void freePcb(pcb_t *p) {
  list_add(&(p->p_next), &pcbfree_h);
}

/* Remove and return one pcb from the free list */
extern pcb_t *allocPcb() {
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

/* Initialize the list of pcbs, initializing the list dummy */
extern void mkEmptyProcQ(struct list_head *list_head) {
  INIT_LIST_HEAD(list_head);
}

/* Returns true if the list is empty, false otherwise */
extern int emptyProcQ(struct list_head *list_head) {
  return list_empty(list_head);
}

/* Insert the element in the queue */
extern void insertProcQ(struct list_head *list_head, pcb_t *p) {
  struct list_head *pos;
  list_for_each(pos, list_head) {
    if (list_entry(pos, pcb_t, p_next)->priority < p->priority) {
      list_add_tail(&p->p_next, pos);
      return;
    }
  }
  /* Priority less than every other: the if is never triggered; add at the end */
  list_add_tail(&p->p_next, pos);
}

/* Returns the first element without removing it. Null if list is empty */
extern pcb_t *headProcQ(struct list_head *list_head) {
  if (emptyProcQ(list_head))
    return NULL;

  pcb_t *p = list_entry(list_head->next, pcb_t, p_next);
  return p;
}

/* Returns the first element removing it. Null if list is empty */
extern pcb_t *removeProcQ(struct list_head *list_head) {
  pcb_t *p = headProcQ(list_head);

  if (p)
    list_del(&(p->p_next));
  return p;
}

/* Removes the pcb from the queue, Null if absent */
extern pcb_t *outProcQ(struct list_head *list_head, pcb_t *p) {
  struct list_head *pos;

  list_for_each(pos, list_head) {
    if (list_entry(pos, pcb_t, p_next) == p) {
      list_del(pos);
      return list_entry(pos, pcb_t, p_next);
    }
  }

  return NULL;
}
