#ifndef TC_CHESS_LOGIC_H
#define TC_CHESS_LOGIC_H

#include "chess_defs.h"

/*
  Returns a tc_board_state with 0 moves (no history) with the
  default number of pieces and all the pieces in their default
  starting position
 */
tc_board_state
tc_new_default_board(void);

#endif
	
