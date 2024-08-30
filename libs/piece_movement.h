#ifndef TC_PIECE_MOVEMENT_H
#define TC_PIECE_MOVEMENT_H

#include "chess_defs.h"
#include <stddef.h>

/*
  Struct is used as a de facto return value. In practice, it should be
  a static variable inside of a parent function calling a child function
  which modifies a tc_moveset struct
 */
struct tc_moveset
{
	size_t max;
	size_t count;
	struct
	{
		tc_square square;
		char flags;
		union
		{
			int id;
			int captured_id;
			int castled_id;
		};
	}* array;
};


/*
  prepended v_ means valid and prepended i_ means invalid. Additionally,
  valid moves are assigned positive integers while invalid moves are
  assigned negative integers for quick and easy if blocks.
 */
enum tc_move_type
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
  two things: "type" and an id. Type will contain binary info on
  whether the move is valid or not. Then there is the union which,
  if the move is valid, can contain whether the ID of the piece being
  captured or the ID of the rook if the move is a castle.
 */
struct tc_move_info
{
	enum tc_move_type type;
	union
	{
		int id;
		int captured_id;
		int rook_id;
	};
};

/* See the definition for the enum tc_move_type */
static inline int
tc_is_valid_move(struct tc_move_info move_info)
{
	return move_info.type > 0;
}

/*
  Given a piece (mover_id) and a square (to_square), this function
  will figure out what of a move it is. Whether it's a valid move
  or an invalid move. Whether the move is a capture, and if so,
  what piece is getting captured.

  Think of this function as a way to take an *attempt* to move
  and turn it into information, which we can then use to figure
  out what the game should be doing next.
 */
struct tc_move_info
tc_evaluate_move(const tc_board_state* board,
                 size_t mover_id,
                 const tc_square* to_square);

#endif
