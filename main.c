#include "libs/instant_input.h"
#include "libs/move_printer.h"

int
main(void)
{
	ii_init();
	int parse_error = 0;
	while (!parse_error)
		parse_error = tc_get_n_print_attempt();
	
	return 0;
}
