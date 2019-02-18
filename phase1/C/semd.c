#include <semd.e>

#include <list.h>
#include <const.h>
#include <pcb.e>

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

/* Internal utility;
 * Remove the semd corresponding to key from the ASL and put into the free list
 * If key isn't found return NULL.
 */
HIDDEN semd_t *_freeSemdKey(int *key) {
  struct semd_t *sem;

  sem = getSemd(key);
  if (!sem)
    return NULL;

  /* Remove from ASL*/
  list_del(&sem->s_next);

  /* Add to the free list */
  list_add(&sem->s_next, &semdfree_h);

  return sem;
}

/* Internal utility;
 * Allocate in the ASL a semd from the free list and set it up with the
 * given key.
 * If no free semd is left, return NULL
 */
HIDDEN semd_t *_allocSemdKey(int *key) {
  struct list_head *alloc_head;
  semd_t *alloc;

  if (list_empty(&semdfree_h))
    return NULL;

  /* Remove from list */
  alloc_head = semdfree_h.next;
  list_del(alloc_head);
  alloc = list_entry(alloc_head, semd_t, s_next);
  
  /* Initialize semd */
  alloc->s_key = key;
  mkEmptyProcQ(&alloc->s_procQ);

  /* Add to ASL */
  list_add(alloc_head, &semd_h);

  return alloc;
}

/* Get the semd corresponding to the key(from the ASL) */ 
extern semd_t *getSemd(int *key) {
  struct list_head *pos;
  semd_t *sem;

  list_for_each(pos, &semd_h) {    /* Search through the list */
    sem = list_entry(pos, semd_t, s_next);
    if (sem->s_key == key)
      return sem;
  }
  return NULL;    /* If key isn't found */
}

/* Insert p in the queue of semd corresponding to the given key */
extern bool insertBlocked(int *key, pcb_t *p) {
  semd_t *sem = getSemd(key);

  if (!sem)
    if (!(sem = _allocSemdKey(key))) /* If can't allocate a new semd */
      return true;

  insertProcQ(&sem->s_procQ, p);
  return false;
}

/* Remove and return the first element in the queue of semd corresponding to key */
extern pcb_t *removeBlocked(int *key) {
  semd_t *sem = getSemd(key);
  pcb_t *p;

  if (!sem)       /* If key isn't found */
    return NULL;

  p = removeProcQ(&sem->s_procQ);
  if (emptyProcQ(&sem->s_procQ))  /* Free semd if no pcb is enqueued */
    _freeSemdKey(key);

  return p;
}

/* Remove p from the semd queue its blocked on */
extern pcb_t *outBlocked(pcb_t *p) {
  semd_t *sem = getSemd(p->p_semKey);

  if (!sem)
    return NULL;

  /* Check if p is found in the given queue */
  return outProcQ(&sem->s_procQ, p);
}

/* Return (without removing) the first pcb blocked in */
extern pcb_t *headBlocked(int *key) {
  semd_t *sem = getSemd(key);

  if (!sem)       /* If key isn't found */
    return NULL;

  return headProcQ(&sem->s_procQ);
}

/* Remove p and the hole tree radicated in p from queues;
 * I'm using the ProcQ functions where possible, but sometimes
 * they're just not right for this work.
 */
extern void outChildBlocked(pcb_t *p) {
  struct list_head q;   /* Scan tree using a queue (BFS)*/
  struct list_head *pos;
  pcb_t *parent;

  mkEmptyProcQ(&q); /* Create queue */

  /* First elements (root) */
  outBlocked(p);
  list_add_tail(&p->p_next, &q);

  /* Scan */
  while (!emptyProcQ(&q)) {
    parent = removeProcQ(&q);

    /* Add children to queue;
     * IMPORTANT: we need to remove the pcb from the semd's queue before
     * adding it to the BFS queue since we're using the same struct member
     * to do it.
     */
    list_for_each(pos, &parent->p_child) {
      outBlocked(list_entry(pos, pcb_t, p_sib));
      list_add_tail(pos, &q);
    }
  }
}
