#ifndef CHESS_DEFINITIONS_H
#define CHESS_DEFINITIONS_H

typedef enum
{
	tc_a,
	tc_b,
	tc_c,
	tc_d,
	tc_e,
	tc_f,
	tc_g,
	tc_h,
} tc_col;

typedef enum
{
	tc_1,
	tc_2,
	tc_3,
	tc_4,
	tc_5,
	tc_6,
	tc_7,
	tc_8,
} tc_row;

typedef struct
{
	tc_col col;
	tc_row row;
} tc_square;

typedef enum
{
	tc_pawn,
	tc_bishop,
	tc_knight,
	tc_rook,
	tc_queen,
	tc_king,
} tc_piece_type;

typedef enum
{
	tc_white,
	tc_black,
} tc_piece_color;

typedef struct
{
	tc_piece_type type;
	tc_piece_color color;
} tc_piece;

typedef struct
{
	tc_piece_type piece_moved;
	tc_square to_square;
} tc_move_attempt;

typedef struct
{
	tc_piece piece_moved;
	tc_square from_square;
	tc_square to_square;
} tc_move;

#endif
