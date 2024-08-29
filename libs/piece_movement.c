#include "piece_movement.h"
#include "board_logic.h"
#include "chess_defs.h"
#include "panic.h"
#include <stddef.h>
#include <sys/cdefs.h>

#define TC_MOVE_ORDINARY  0
#define TC_MOVE_CAPTURE   (1 << 0)
#define TC_MOVE_ENPASSANT (1 << 1)
#define TC_MOVE_CASTLE    (1 << 2)
#define TC_PROMOTION      (1 << 3)

/*
  0 0 0 0 0 0 0 0
  ~~~~~~~ | | | |
  |       | | | Capture bit flag
  |       | | |
  |       | | En Passant bit flag
  |       | |
  |       | Castle bit flag
  |       |
  |       Promotion bit flag
  |
  If promotion bit flag is raised, these 4 bits store the piece
  that the pawn must promote to.
*/

struct append_valid_mov_args
{
	struct tc_valid_mov* holder;
	const tc_square* to_square;
	char flags;
	int id;
};

/*
  Valid moves are stored in a vector, which should resize.
  Appending to the valid moves should then involve a realloc
  call.
  
  Also, this is a struct because it is my personal philosophy that
  any function that takes in four parameters or more should have its
  own custom struct that holds its arguments. To interface with this:
  
	append_valid_mov((struct append_valid_mov_args) {
			.holder = holder,
			.to_square = square,
			.flags = TC_MOVE_CAPTURE,
			.id = captured_id,
		});
 */
static void
append_valid_mov(struct append_valid_mov_args args)
{
	/* Readability variable */
	struct tc_valid_mov* holder = args.holder;
	
	if (holder -> count + 1 >= holder -> max)
	{
		holder -> max *= 2;
		PN_REALLOC(holder -> array,
				   sizeof(*(holder -> array)) * holder -> max);
	}

	holder -> array[holder -> count].square = *args.to_square;
	holder -> array[holder -> count].flags  = args.flags;
	holder -> array[holder -> count].id     = args.id;
	++(holder -> count);
}

struct append_if_capture_args
{
	const tc_board_state* board;
	const tc_square* square;
	const tc_piece_color opp_color;
	struct tc_valid_mov* return_holder;
};

/*
  Appends to the "valid move" array a move if the piece occupying the
  square matches the opposing color given by args.opp_color. Otherwise,
  the move will not be considered a valid move.This move is really only
  available for pawns---only pawns gain extra moves for captures.
  
  Also, this is a struct because it is my personal philosophy that
  any function that takes in four parameters or more should have its
  own custom struct that holds its arguments. To interface with this:
			
	append_onlyif_capture((struct append_if_capture_args) {
			.board         = board,
			.square        = &capture_square,
			.opp_color     = opp_color,
			.return_holder = return_holder,
		});
 */
static void
append_onlyif_capture(struct append_if_capture_args args)
{
	int empty_square_flag = 0;
	size_t captured_id = tc_square_id(args.board,
									  args.square,
									  &empty_square_flag);
	if (empty_square_flag)
		return;
	
	tc_piece_color captured_color = args.board -> piece_v[captured_id].color;

	if (captured_color == args.opp_color)
		append_valid_mov((struct append_valid_mov_args) {
				.holder = args.return_holder,
				.to_square = args.square,
				.flags = TC_MOVE_CAPTURE,
				.id = captured_id,
			});
}

/*
  +---+---+---+
  | 1 | 2 | 3 |
  +---+---+---+
  |   | p |   |
  +---+---+---+

  1 - if 1 has a piece of opposing color, pawn can move to 1
  2 - if 2 has no pieces, pawn can move to 2
  3 - if 3 has a piece of opposing color, pawn can move to 3
 */
static void
fetch_pawn_moves(const tc_board_state* board,
                   size_t mover_id,
                   struct tc_valid_mov* return_holder)
{
	tc_piece_color mover_color;
	tc_piece_color opp_color;
	tc_square fore_square;
	int errflag;
	
	mover_color = board -> piece_v[mover_id].color;
	opp_color   = tc_enemy_color(mover_color);

	fore_square = board -> piece_v[mover_id].location;
	errflag = 0;
	tc_translate((struct tc_translate_args) {
			.square = &fore_square,
			.drow = tc_forward(mover_color),
			.dcol = 0,
			.errflag = &errflag,
		});
	
	/*
	  After this, fore_square should now be pointing to square 2.
	  This is unless the pawn is on the furthest rank, in which case
	  errflag will be raised. Now this should be impossible:
	  a pawn should stop becoming a pawn by the time it reaches
	  the furthest rank by way of promotion! Regardless, errflag
	  should be checked.
	*/

	if (errflag)
		return;

	errflag = 0;
	(void) tc_square_id(board, &fore_square, &errflag);

	/*
	  "errflag" is raised here if there is no piece obstructing the
	  square just in front of the pawn---which is to say, square 2:

		+---+
		| 2 | <--- If a piece is NOT here, errflag is raised.
		+---+
		| p | <--- The pawn that wishes to move
		+---+
	  
	 */
	if (errflag)
		append_valid_mov((struct append_valid_mov_args) {
				.holder = return_holder,
				.to_square = &fore_square,
				.flags = TC_MOVE_ORDINARY,
			});

	/*
	  +---+---+---+
	  | 1 |   | 3 | <-- Now checking these two capture squares
	  +---+---+---+
	  |   | p |   | <-- The pawn that wishes to move
	  +---+---+---+
	*/

	tc_square capture_square;
	
	capture_square = fore_square;
	errflag = 0;
	tc_translate((struct tc_translate_args) {
			.square = &capture_square,
			.drow = 0,
			.dcol = 1,
			.errflag = &errflag,
		});

	/* errflag is raised if the pawn is on the furthest column */
	if (!errflag)
		append_onlyif_capture((struct append_if_capture_args) {
				.board = board,
				.square = &capture_square,
				.opp_color = opp_color,
				.return_holder = return_holder,
			});
	
	capture_square = fore_square;
	errflag = 0;
	tc_translate((struct tc_translate_args) {
			.square = &capture_square,
			.drow = 0,
			.dcol = -1,
			.errflag = &errflag,
		});

	/* errflag is raised if the pawn is on the furthest column */
	if (!errflag)
		append_onlyif_capture((struct append_if_capture_args) {
				.board = board,
				.square = &capture_square,
				.opp_color = opp_color,
				.return_holder = return_holder,
			});
}

/*
  Set up a vector of squares where a piece is allowed to go.
  This does not keep chess-checks in mind.

  It does not have a return value. Instead, it will manipulate
  a reference to something that I will be calling a return_holder.
  We should ideally be using the same return_holder throughout the
  entire runtime of the program. The return holder will get bigger
  and bigger as more and more moves are appended to it, after all.
 */
static void
fetch_valid_moves(const tc_board_state* board,
                  size_t mover_id,
                  struct tc_valid_mov* return_holder)
{
	return_holder -> count = 0;
	switch (board -> piece_v[mover_id].type)
	{
	case tc_pawn:
		fetch_pawn_moves(board, mover_id, return_holder);
		break;
	case tc_bishop:
		fetch_pawn_moves(board, mover_id, return_holder);
		break;
	case tc_knight:
		fetch_pawn_moves(board, mover_id, return_holder);
		break;
	case tc_rook:
		fetch_pawn_moves(board, mover_id, return_holder);
		break;
	case tc_queen:
		fetch_pawn_moves(board, mover_id, return_holder);
		break;
	case tc_king:
		fetch_pawn_moves(board, mover_id, return_holder);
		break;
	}
}

/*
  Glorified sequential search. Just checks to see if some
  tc_square move is contained in a tc_square array. tc_valid_mov
  contains the size of the array, so this _really is_ just a search
  function.
 */
static inline int
check_if_valid_move(struct tc_valid_mov* valid_moves,
                    const tc_square* move)
{
	for (size_t i = 0; i < valid_moves -> count; ++i)
		if (tc_square_equals(&(valid_moves -> array[i].square), move))
			return i;	
	return -1;
}

/*
  See the valid_moves field in tc_evaluate_move function.

  valid_moves starts out with all its bits switched off,
  meaning that holder -> array should be NULL. All this
  This function, then, will initialize valid_moves.
 */
static inline void
reset_valid_move_holder(struct tc_valid_mov* holder)
{
	static const size_t initial_array_size = 12;

	if (holder -> array == NULL)
	{
		holder -> max = initial_array_size;
		holder -> array = PN_MALLOC((sizeof(tc_square) + sizeof(char))
									* initial_array_size);
	}
	
	holder -> count = 0;
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
struct tc_mov_info
tc_evaluate_move(const tc_board_state* board,
                 size_t mover_id,
                 const tc_square* to_square)
{
	static struct tc_valid_mov valid_moves = {0};

	reset_valid_move_holder(&valid_moves);
	fetch_valid_moves(board, mover_id, &valid_moves);

	int index = check_if_valid_move(&valid_moves, to_square);
	if (index == -1)
		return (struct tc_mov_info)
			{ .validity = i_INVALID_SQUARE };

	if (valid_moves.array[index].flags & TC_MOVE_CAPTURE)
		return (struct tc_mov_info)
			{
				.validity = v_OPP_CAPTURE,
				.captured_id = valid_moves.array[index].captured_id,
			};

	return (struct tc_mov_info)
		{ .validity = v_EMPTY_SQUARE };
}
