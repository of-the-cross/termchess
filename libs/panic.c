#include "panic.h"
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

/*
  panic malloc function
  ALWAYS returns a non-null void pointer. If the pointer is null
  (that is to say, if malloc fails) then the program exits
 */
void*
pn_malloc(size_t size)
{
	void* ptr = malloc(size);
	if (ptr == NULL)
	{
		fprintf(stderr, "FATAL ERROR: ");
		fprintf(stderr, "malloc failed.\n");
		exit(1);
	}
	return ptr;
}
