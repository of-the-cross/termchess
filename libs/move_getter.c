#include "move_getter.h"
#include "chess_defs.h"
#include "instant_input.h"
#include <ctype.h>

/*
  Take a char and output a piece type (enum). If this
  fails, set m_errno flag to 1
 */
tc_piece_type
parse_piece_type(char c, int* m_errno)
{
	c = toupper(c);
	switch (c) {
	case 'N':
		return tc_knight;
	case 'B':
		return tc_bishop;
	case 'R':
		return tc_rook;
	case 'Q':
		return tc_queen;
	case 'K':
		return tc_king;
	};

	*m_errno = 1;
	return tc_pawn;
}

/*
  Take a char and output a column (enum). If this
  fails, set m_errno flag to 1
 */
tc_col
parse_col(char c, int* m_errno)
{
	c = toupper(c);
	switch (c) {
	case 'A':
		return tc_a;
	case 'B':
		return tc_b;
	case 'C':
		return tc_c;
	case 'D':
		return tc_d;
	case 'E':
		return tc_e;
	case 'F':
		return tc_f;
	case 'G':
		return tc_g;
	case 'H':
		return tc_h;
	}

	*m_errno = 1;
	return tc_a;
}

/*
  Take a char and output a row (enum). If this
  fails, set m_errno flag to 1
 */
tc_row
parse_row(char c, int* m_errno)
{
	c = toupper(c);
	switch (c) {
	case '1':
		return tc_1;
	case '2':
		return tc_2;
	case '3':
		return tc_3;
	case '4':
		return tc_4;
	case '5':
		return tc_5;
	case '6':
		return tc_6;
	case '7':
		return tc_7;
	case '8':
		return tc_8;
	}

	*m_errno = 1;
	return tc_1;
}

/*
  Using the player's key strokes, determine a plausible move attempt.
  A move usually looks something like Ne6, or knight to e6.
                                      ~~~
                                        |
   +-+-+--------------------------------+
   | | |
   | | +- N -> Knight     -> piece
   | ---- e -> 5th column -> column
   ------ 6 -> 6th row    -> row

   However, with pawns, moves the piece type is implicit.
   A pawn move usually looks something like e4, or pawn to e4.
 */
tc_move_attempt
tc_type_move_attempt(int* m_errno)
{
	tc_move_attempt piece_move;
	int piece_parse_error = 0;
	
	tc_move_attempt pawn_move = {tc_pawn};
	int pawn_parse_error = 0;

	char c;

	c = ii_next_char();
	piece_move.piece_moved = parse_piece_type(c, &piece_parse_error);
	pawn_move.to_square.col = parse_col(c, &pawn_parse_error);

	if (piece_parse_error && pawn_parse_error)
	{
		*m_errno = 1;
		return (tc_move_attempt) {0};
	}
	
	c = ii_next_char();
	piece_move.to_square.col = parse_col(c, &piece_parse_error);
	pawn_move.to_square.row = parse_row(c, &pawn_parse_error);

	if (piece_parse_error)
	{
		if (pawn_parse_error)
		{
			*m_errno = 1;
			return (tc_move_attempt) {0};
		}

		return pawn_move;
	}

	c = ii_next_char();
	piece_move.to_square.row = parse_row(c, &piece_parse_error);

	if (piece_parse_error)
	{
		*m_errno = 1;
		return (tc_move_attempt) {0};
	}

	return piece_move;
}
