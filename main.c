#include "libs/board_painter.h" // tc_print_board
#include "libs/input_handler.h" // tc_new_default_session, tc_game_loop
#include "libs/instant_input.h" // ii_init
#include "libs/term_painter.h"  // tp_general_setup

int
main(void)
{
	ii_init();
	tp_general_setup();
	tc_print_board();

	struct sessioninfo my_session = tc_new_default_session();

	tc_game_loop(&my_session);
	
	return 0;
}
