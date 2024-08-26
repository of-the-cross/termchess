#include "panic.h"
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

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
