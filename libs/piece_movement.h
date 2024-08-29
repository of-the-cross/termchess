#ifndef TC_PIECE_MOVEMENT_H
#define TC_PIECE_MOVEMENT_H

#include "chess_defs.h"
#include <stddef.h>

/*
  Struct is used as a de facto return value. In practice, it should be
  a static variable inside of a parent function calling a child function
  which modifies a tc_valid_mov struct
 */
struct tc_valid_mov
{
	size_t max;
	size_t count;
	tc_square* arr;
};

/*
  prepended v_ means valid and prepended i_ means invalid. Additionally,
  valid moves are assigned positive integers while invalid moves are
  assigned negative integers for quick and easy if blocks.
 */
enum tc_mov_state
{
	v_EMPTY_SQUARE    = 1,
	v_OPP_CAPTURE     = 2,
	v_CASTLE          = 3,
	
	i_INVALID_SQUARE  = -1,
	i_OWN_CAPTURE     = -2,
	i_CHECK_NEGLECTED = -3,
};

/*
  Information about a move. For the program to run, we need to know
  two things: "validity" and an id. Validity will contain binary info
  on whether the move is valid or not. Then there is the union which,
  if the move is valid, can contain whether the ID of the piece being
  captured or the ID of the rook if the move is a castle.
 */
struct tc_mov_info
{
	enum tc_mov_state validity;
	union
	{
		int id;
		int captured_id;
		int rook_id;
	};
};

static inline int
tc_is_valid_move(struct tc_mov_info move_info)
{
	return move_info.validity > 0;
}

// #define TC_IF_VALID_MOVE(s) if ((enum tc_mov_state) s > 0)

struct tc_mov_info
tc_evaluate_move(const tc_board_state* board,
                 size_t mover_id,
                 const tc_square* to_square);
/*
struct tc_valid_mov
tc_all_valid_squares(const tc_board_state* board,
					 const tc_piece_inst* piece);
*/

#endif
