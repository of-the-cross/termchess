#include "piece_movement.h"
#include "board_logic.h"
#include "chess_defs.h"
#include "panic.h"
#include <stddef.h>
#include <sys/cdefs.h>


//	enum tc_mov_state
//	{
//		v_EMPTY_SQUARE,
//		v_OPP_CAPTURE,
//		i_INVALID_SQUARE,
//		i_OWN_CAPTURE,
//		i_CHECK_NEGLECTED,
//	};

struct __tc_valid_mov
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

#define TC_MOVE_CAPTURE   (1 << 0)
#define TC_MOVE_ENPASSANT (1 << 1)
#define TC_MOVE_CASTLE    (1 << 2)
// #define TC_MOVE_PROMOTION(move) (1 << 3)
// #define TC_PROMOTION_PIECE(move) (move >> 4)

#define TC_MOVE_ORDINARY 0

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

  4 bits

*/

struct append_valid_mov_args
{
	struct __tc_valid_mov* holder;
	const tc_square* to_square;
	char flags;
	int id;
};

static void
__append_valid_mov(struct append_valid_mov_args args)
{
	struct __tc_valid_mov* holder = args.holder;
	
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

static void
append_valid_mov(struct tc_valid_mov* holder,
                 const tc_square* element)
{
	if (holder -> count + 1 >= holder -> max)
	{
		holder -> max *= 2;
		PN_REALLOC(holder -> arr, holder -> max);
	}

	holder -> arr[holder -> count] = *element;
	++(holder -> count);
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
static inline void
fetch_pawn_moves(const tc_board_state* board,
                       const tc_piece_inst* piece,
                       struct tc_valid_mov* return_holder)
{

	/* TODO: REMOVE THIS ASAP */
	//if (piece -> location.row == tc_1)
	//	return;

	tc_piece_color enemy;
	int forward;

	if (piece -> color == tc_white)
	{
		forward = 1;
		enemy = tc_black;
	}
	else
	{
		forward = -1;
		enemy = tc_white;
	}

	tc_square possible_move;
	int empty_square;

	possible_move = piece -> location;
	possible_move.row += forward;

	empty_square = 0;
	(void) tc_square_id(board, &possible_move, &empty_square);

	if (empty_square)
		append_valid_mov(return_holder, &possible_move);

	size_t capture;

	/* Pawns on H file can't possibly capture right */
	if (possible_move.col < tc_h)
	{
		possible_move.col += 1;

		empty_square = 0;
		capture = tc_square_id(board, &possible_move, &empty_square);

		if (!empty_square && board -> piece_v[capture].color == enemy)
			append_valid_mov(return_holder, &possible_move);

		possible_move.col -= 1;
	}

	/* Pawns on A file can't possibly capture left */
	if (possible_move.col > tc_a)
	{
		possible_move.col -= 1;

		empty_square = 0;
		capture = tc_square_id(board, &possible_move, &empty_square);

		if (!empty_square && board -> piece_v[capture].color == enemy)
			append_valid_mov(return_holder, &possible_move);

		possible_move.col += 1;
	}
}

struct append_if_capture_args
{
	const tc_board_state* board;
	const tc_square* square;
	const tc_piece_color opp_color;
	struct __tc_valid_mov* return_holder;
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
		__append_valid_mov((struct append_valid_mov_args) {
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
__fetch_pawn_moves(const tc_board_state* board,
                   size_t mover_id,
                   struct __tc_valid_mov* return_holder)
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
		__append_valid_mov((struct append_valid_mov_args) {
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

static void
fetch_valid_moves(const tc_board_state* board,
                  size_t mover_id,
                  struct __tc_valid_mov* return_holder)
{
	return_holder -> count = 0;
	switch (board -> piece_v[mover_id].type)
	{
	case tc_pawn:
		__fetch_pawn_moves(board, mover_id, return_holder);
		break;
	case tc_bishop:
		__fetch_pawn_moves(board, mover_id, return_holder);
		break;
	case tc_knight:
		__fetch_pawn_moves(board, mover_id, return_holder);
		break;
	case tc_rook:
		__fetch_pawn_moves(board, mover_id, return_holder);
		break;
	case tc_queen:
		__fetch_pawn_moves(board, mover_id, return_holder);
		break;
	case tc_king:
		__fetch_pawn_moves(board, mover_id, return_holder);
		break;
	}
}

static inline int
check_if_valid_move(struct __tc_valid_mov* valid_moves,
					const tc_square* move)
{
	for (size_t i = 0; i < valid_moves -> count; ++i)
	{
        /******************************************************************
         *                                                                *
         *                &(valid_moves -> array[i].square)               *
         *                                                                *
         *  Look at "valid moves", take its "array" member, take its      *
         *  i'th element (which is a struct), take that element's square  *
         *  member, and take that square member's address. We need this   *
         *  address because tc_square_equals takes pointers to squares.   *
         *                                                                *
         ******************************************************************/
		
		if (tc_square_equals(&(valid_moves -> array[i].square), move))
			return i;
	}
	return -1;
}

static inline void
reset_valid_move_holder(struct tc_valid_mov* holder)
{
	static const size_t initial_arr_size = 12;

	if (holder -> arr == NULL)
	{
		holder -> arr = PN_MALLOC(sizeof(tc_square) * initial_arr_size);
		holder -> max = initial_arr_size;
	}

	holder -> count = 0;
}

/*
  struct __tc_valid_mov
{
	size_t max;
	size_t count;
	struct
	{
		tc_square square;
		char flags;
	}* array;
};
 */

static inline void
__reset_valid_move_holder(struct __tc_valid_mov* holder)
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
  When a
 */
struct tc_mov_info
tc_evaluate_move(const tc_board_state* board,
                 size_t mover_id,
                 const tc_square* to_square)
{
	static struct __tc_valid_mov valid_moves = {0};

	__reset_valid_move_holder(&valid_moves);
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
