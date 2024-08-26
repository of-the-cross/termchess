#include "libs/board_painter.h"
#include "libs/instant_input.h"
#include "libs/move_printer.h"
#include "libs/term_painter.h"

int
main(void)
{
	ii_init();
	pn_general_setup();

	bp_print_board();
	
	int parse_error = 0;
	while (!parse_error)
	{
		parse_error = tc_get_n_print_attempt();
	}
	
	return 0;
}
