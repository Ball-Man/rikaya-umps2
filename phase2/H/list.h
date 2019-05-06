#ifndef LINUX_LIST_H_
#define LINUX_LIST_H_

/* This file is from Linux Kernel (include/linux/list.h) 
 * and modified by simply removing hardware prefetching of list items. 
 */

struct list_head {
  struct list_head *next;
  struct list_head *prev;
};

#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define LIST_HEAD(name)\
	struct list_head name = LIST_HEAD_INIT(name)

#define INIT_LIST_HEAD(ptr) do {\
	(ptr)->next = (ptr); (ptr)->prev = (ptr);\
} while (0)

/* Internal utility;
 * Insert a new entry between two known consecutive entries. 
 */
static void __list_add(struct list_head *new, 
	struct list_head *prev, struct list_head *next) {
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}

/* Insert a new entry after the specified head.
 * This is good for implementing stacks.
 */
static void list_add(struct list_head *new, struct list_head *head)
{
  __list_add(new, head, head->next);
}

/* Insert a new entry before the specified head.
 * This is useful for implementing queues.
 */
static void list_add_tail(struct list_head *new, struct list_head *head) {
	__list_add(new, head->prev, head);
}

/* Internal utility;
 * Delete a list entry by making the prev/next entries
 * point to each other.
 */
static void __list_del(struct list_head *prev, struct list_head *next) {
	next->prev = prev;
	prev->next = next;
}

/* Deletes entry from list */
static void list_del(struct list_head *entry) {
	__list_del(entry->prev, entry->next);
	entry->next = (void *) 0;
	entry->prev = (void *) 0;
}

/* Deletes entry from list and reinitialize it */
static void list_del_init(struct list_head *entry) {
  __list_del(entry->prev, entry->next);
  INIT_LIST_HEAD(entry); 
}

/* Delete from one list and add as another's head */
static void list_move(struct list_head *list, struct list_head *head) {
	__list_del(list->prev, list->next);
	list_add(list, head);
}

/* Delete from one list and add as another's tail */
static void list_move_tail(struct list_head *list, struct list_head *head) {
	__list_del(list->prev, list->next);
	list_add_tail(list, head);
}

/* Tests whether a list is empty */
static int list_empty(struct list_head *head) {
	return head->next == head;
}

/* Internal utility */
static void __list_splice(struct list_head *list, struct list_head *head) {
	struct list_head *first = list->next;
	struct list_head *last = list->prev;
	struct list_head *at = head->next;

	first->prev = head;
	head->next = first;

	last->next = at;
	at->prev = last;
}

/* Join two lists */
static void list_splice(struct list_head *list, struct list_head *head) {
  if (!list_empty(list))
		__list_splice(list, head);
}

/* Join two lists and reinitialise the emptied list. */
static void list_splice_init(struct list_head *list,
            struct list_head *head) {
  if (!list_empty(list)) {
		__list_splice(list, head);
		INIT_LIST_HEAD(list);
  }
}

/* Get the struct for this entry */
#define list_entry(ptr, type, member) \
	((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))

/* Iterate over a list */
#define list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); \
		pos = pos->next)

/* Iterate over a list backwards */
#define list_for_each_prev(pos, head) \
	for (pos = (head)->prev; pos != (head); \
		pos = pos->prev)
          
/* Iterate over a list safe against removal of list entry */
#define list_for_each_safe(pos, n, head) \
  for (pos = (head)->next, n = pos->next; pos != (head);\
  	pos = n, n = pos->next)

/* Iterate over list of given type */
#define list_for_each_entry(pos, head, member)\
  for (pos = list_entry((head)->next, typeof(*pos), member);\
		&pos->member != (head);\
		pos = list_entry(pos->member.next, typeof(*pos), member))

/* Iterate over list of given type safe against removal of list entry */
#define list_for_each_entry_safe(pos, n, head, member)\
  for (pos = list_entry((head)->next, typeof(*pos), member),\
  	n = list_entry(pos->member.next, typeof(*pos), member);\
		&pos->member != (head);\
		pos = n, n = list_entry(n->member.next, typeof(*n), member))


#endif
