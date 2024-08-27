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

/*
  Find the index of a piece in the board whose location
  matches the square given into the function. If not found,
  the index is set to 0 and the inputted error flag is set
  to one.
 */
size_t
tc_index_square(const tc_board_state* board,
				  const tc_square* square,
				  int* errflag);

/*
  Extremely raw function that moves some piece from one square
  to another square. It accesses pieces by index, meaning that
  it WILL get a piece (if it doesn't overflow) and rewrite
  that piece's location. Will only be called in this file.

  Parent function needs to ensure that index is not greater
  than board -> piece_size. If index is greater than piece_max,
  it might crash the program. If index is greater than piece_size,
  it will give nonsense results.

  Ideally, index was taken from tc_index_square, meaning that index
  is ALWAYS a valid.
 */
void
tc_piece_tp_unsafe(const tc_board_state* board,
				   size_t index,
				   const tc_square* to_square);

#endif
	
