#include "chess_logic.h"
#include "chess_defs.h"
#include "panic.h"

#define TC_BOARD_SIZE (TC_ROW_SIZE * TC_COL_SIZE)

/*
typedef struct
{
	unsigned int historyc;
	tc_move* historyv;
	unsigned int history_count;
	tc_piece_inst* piece_place;
} tc_board_state;
*/

#define INIT_HISTORY_ALLOC 32
#define INIT_PLACEMENT_ALLOC 32

static tc_piece_inst default_placement[TC_BOARD_SIZE] =
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

#define TC_DEFAULT_PIECE_COUNT 32

/*
  Perform a copy on a tc_piece_inst* (array) type given some size.
  This should NOT be called willy nilly. Whichever parent function
  calls this function holds the responsibility of making sure that
  this for loop doesn't overflow.
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

tc_board_state
tc_new_default_board(void)
{
	size_t history_max     = INIT_HISTORY_ALLOC;
	size_t history_size    = 0;
	tc_move* history_v     = pn_malloc(sizeof(tc_move) *
									   INIT_HISTORY_ALLOC);
	
	size_t piece_max       = INIT_PLACEMENT_ALLOC;
	size_t piece_size      = TC_DEFAULT_PIECE_COUNT;
	tc_piece_inst* piece_v = pn_malloc(sizeof(tc_piece_inst)
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
