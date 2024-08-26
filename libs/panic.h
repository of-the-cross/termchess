#ifndef PANIC_H
#define PANIC_H
#include <stddef.h>

/*
  panic malloc function
  ALWAYS returns a non-null void pointer. If the pointer is null
  (that is to say, if malloc fails) then the program exits
 */
void*
pn_malloc(size_t size);

#endif
