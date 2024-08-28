#ifndef PANIC_H
#define PANIC_H

#include <stddef.h>

/*
  General debugging abort function. It exits while giving
  an error message and tells you what function it exited
  from.
*/
void
pn_panic(const char* errmsg,
         const char func_name[]);

#define PN_PANIC(errmsg) pn_panic(errmsg, __func__)

/*
  panic malloc function
  ALWAYS returns a non-null void pointer. If the pointer is null
  (that is to say, if malloc fails) then the program exits
*/
void*
pn_malloc(size_t size,
          const char func_name[]);

#define PN_MALLOC(size) pn_malloc(size, __func__)

/*
  panic realloc function
  ALWAYS returns a non-null void pointer. If the pointer is null
  (that is to say, if realloc fails) then the program exits
*/
void*
pn_realloc(void* ptr,
           size_t new_size,
           const char func_name[]);

#define PN_REALLOC(size) pn_realloc(size, __func__)

/*
  Panic when some flag is on (or, if some int is any value except 0).
  This is used for custom errnos and error flags.
*/
void
pn_flag(int flag,
        const char* errmsg,
        const char func_name[]);

#define PN_FLAG(flag, errmsg) pn_flag(flag, errmsg, __func__)

#endif
