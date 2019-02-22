#include <lang.e>

/* Sets the specified amount of memory starting from the given pointer to the given value */
extern void *memset(void *s, int c, size_t n)
{
  uint8_t* p = s;
  while (n--)
    *p++ = (uint8_t)c;
  return s;
}
