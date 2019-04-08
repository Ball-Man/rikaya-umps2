#ifndef PHASE1_LANG_E_
#define PHASE1_LANG_E_

#include <lang.h>

/* Sets the specified amount of memory starting from the given pointer to the given value
 * NOTE: the behaviour might not be what you expect for values different than 0.
 */
extern void *memset(void *s, int c, size_t n);

#endif
