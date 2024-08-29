#include "panic.h"
#include "instant_input.h"
#include "term_painter.h"
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

/*
  General debugging abort function. It exits while giving
  an error message and tells you what function it exited
  from.
 */
void
pn_panic(const char* errmsg,
         const char func_name[])
{
	TP_SCREEN_WIPE;
	TP_CURSOR_TO_HOME;
	printf("Fatal Error in %s: ", func_name);
	printf("%s\n", errmsg);
	printf("Press any key to exit.\n");
	ii_next_char();
	exit(-1);
}

/*
  panic malloc function
  ALWAYS returns a non-null void pointer. If the pointer is null
  (that is to say, if malloc fails) then the program exits
 */
void*
pn_malloc(size_t size,
          const char func_name[])
{
	void* ptr = malloc(size);
	
	if (ptr == NULL)
		pn_panic("malloc failed.", func_name);
	
	return ptr;
}

/*
  panic realloc function
  ALWAYS returns a non-null void pointer. If the pointer is null
  (that is to say, if realloc fails) then the program exits
*/
void*
pn_realloc(void* ptr,
           size_t new_size,
           const char func_name[])
{
	ptr = realloc(ptr, new_size);
	
	if (ptr == NULL)
		pn_panic("realloc failed.", func_name);
	
	return ptr;
}

/*
  Panic when some flag is on, or is anything except 0.
  This is used for errno's and error flags.
 */
void
pn_flag(int flag,
        const char* errmsg,
        const char func_name[])
{
	if (flag)
		pn_panic(errmsg, func_name);
}
