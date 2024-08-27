#include "libs/board_painter.h"
#include "libs/input_handler.h"
#include "libs/instant_input.h"
#include "libs/term_painter.h"

int
main(void)
{
	ii_init();
	pn_general_setup();
	tc_print_board();

	struct sessioninfo my_session = tc_new_default_session();

	tc_game_loop(&my_session);
	
	return 0;
}
