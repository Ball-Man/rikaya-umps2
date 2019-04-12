#include <lang.e>

/* Sets the specified amount of memory starting from the given pointer to the given value */
extern void *memset(void *s, int c, size_t n)
{
  uint8_t* p = s;
  while (n--)
    *p++ = (uint8_t)c;
  return s;
}

/* Copies the specified amount of memory from a source point to a destination point */
extern void *memcpy(void *s, void *d, size_t n)
{
  uint8_t* p = s;
  uint8_t* q = d;
  while (n--)
    *q++ = (uint8_t)*p++;
  return d;
}
