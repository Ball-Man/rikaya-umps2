#ifndef FASE1_SEMD_H_
#define FASE1_SEMD_H_

#include <list.h>

typedef struct semd_t {
  struct list_head s_next;
  int *s_key;
  struct list_head s_procQ;
} semd_t;

#endif
