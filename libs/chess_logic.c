#include "chess_logic.h"
#include "chess_defs.h"
#include "panic.h"
#include <string.h>

#define INIT_HISTORY_ALLOC 32
#define INIT_PLACEMENT_ALLOC 32

#define TC_DEFAULT_PIECE_COUNT 32

/* This is the default starting position in chess games */
static tc_piece_inst default_placement[TC_DEFAULT_PIECE_COUNT] =
{
	/* White */
	
	{ tc_rook   , tc_white, {tc_a, tc_1}},
	{ tc_knight , tc_white, {tc_b, tc_1}},
	{ tc_bishop , tc_white, {tc_c, tc_1}},
	{ tc_queen  , tc_white, {tc_d, tc_1}},
	{ tc_king   , tc_white, {tc_e, tc_1}},
	{ tc_bishop , tc_white, {tc_f, tc_1}},
	{ tc_knight , tc_white, {tc_g, tc_1}},
	{ tc_rook   , tc_white, {tc_h, tc_1}},
	
	{ tc_pawn, tc_white, {tc_a, tc_2}},
	{ tc_pawn, tc_white, {tc_b, tc_2}},
	{ tc_pawn, tc_white, {tc_c, tc_2}},
	{ tc_pawn, tc_white, {tc_d, tc_2}},
	{ tc_pawn, tc_white, {tc_e, tc_2}},
	{ tc_pawn, tc_white, {tc_f, tc_2}},
	{ tc_pawn, tc_white, {tc_g, tc_2}},
	{ tc_pawn, tc_white, {tc_h, tc_2}},

	/* Black */
	
	{ tc_rook   , tc_black, {tc_a, tc_8}},
	{ tc_knight , tc_black, {tc_b, tc_8}},
	{ tc_bishop , tc_black, {tc_c, tc_8}},
	{ tc_queen  , tc_black, {tc_d, tc_8}},
	{ tc_king   , tc_black, {tc_e, tc_8}},
	{ tc_bishop , tc_black, {tc_f, tc_8}},
	{ tc_knight , tc_black, {tc_g, tc_8}},
	{ tc_rook   , tc_black, {tc_h, tc_8}},
	
	{ tc_pawn, tc_black, {tc_a, tc_7}},
	{ tc_pawn, tc_black, {tc_b, tc_7}},
	{ tc_pawn, tc_black, {tc_c, tc_7}},
	{ tc_pawn, tc_black, {tc_d, tc_7}},
	{ tc_pawn, tc_black, {tc_e, tc_7}},
	{ tc_pawn, tc_black, {tc_f, tc_7}},
	{ tc_pawn, tc_black, {tc_g, tc_7}},
	{ tc_pawn, tc_black, {tc_h, tc_7}},
};


/*
  Perform a copy on a tc_piece_inst* (array) type given some size.
  This should NOT be called willy nilly. Whichever parent function
  calls this function holds the responsibility of making sure that
  this for loop doesn't overflow!
 */
void
pvcpy_unsafe(const tc_piece_inst* from,
			 tc_piece_inst* to,
			 size_t size)
{
	for (size_t i = 0; i < size; ++i)
	{
		to[i] = from[i];
	}
}

/*
  Returns a tc_board_state with 0 moves (no history) with the
  default number of pieces and all the pieces in their default
  starting position
 */
tc_board_state
tc_new_default_board(void)
{
	size_t history_max     = INIT_HISTORY_ALLOC;
	size_t history_size    = 0;
	tc_move* history_v     = PN_MALLOC(sizeof(tc_move) *
									   INIT_HISTORY_ALLOC);
	
	size_t piece_max       = INIT_PLACEMENT_ALLOC;
	size_t piece_size      = TC_DEFAULT_PIECE_COUNT;
	tc_piece_inst* piece_v = PN_MALLOC(sizeof(tc_piece_inst)
									   * INIT_PLACEMENT_ALLOC);

	pvcpy_unsafe(default_placement, piece_v, TC_DEFAULT_PIECE_COUNT);
	
	return (tc_board_state)
		{
			history_max,
			history_size,
			history_v,

			piece_max,
			piece_size,
			piece_v,
	  };
}

/*
  Check if two tc_squares point to the same square.
 */
int
tc_square_equals(const tc_square* sqr1, const tc_square* sqr2)
{
	if (memcmp(sqr1, sqr2, sizeof(tc_square)) == 0)
		return 1;
	else
		return 0;
}

/*
  Find the index of a piece in the board whose location
  matches the square given into the function. If not found,
  the index is set to 0 and the inputted error flag is set
  to one.
 */
size_t
tc_index_square(const tc_board_state* board,
					 const tc_square* square,
					 int* errflag)
{
	tc_square* current_square;
	for (size_t i = 0; i < board -> piece_size; ++i)
	{
		current_square = &(board -> piece_v[i].location);
		if (tc_square_equals(current_square, square))
			return i;
	}
	*errflag = 1;
	return 0;
}


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
				   const tc_square* to_square)
{
	board -> piece_v[index].location = *to_square;
}
