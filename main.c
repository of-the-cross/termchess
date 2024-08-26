#include "libs/board_painter.h"
#include "libs/instant_input.h"
#include "libs/move_printer.h"
#include "libs/term_painter.h"
#include "libs/chess_defs.h"
#include "libs/chess_logic.h"

int
main(void)
{
	ii_init();
	pn_general_setup();

	tc_print_board();

	tc_board_state my_board = tc_new_default_board();
	tc_print_pieces(&my_board);
	
	int parse_error = 0;
	while (!parse_error)
	{
		parse_error = tc_get_n_print_attempt();
	}
	
	return 0;
}
