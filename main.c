#include <stdio.h>
#include "libs/instant_input.h"

int
main(void)
{
	ii_init();
	
	printf("Hello, world!\n");
	printf("Getting next char:\n");
	printf("%c was your input!\n", ii_next_char());
	return 0;
}
