#ifndef TC_CHESS_DEFINITIONS_H
#define TC_CHESS_DEFINITIONS_H

#include <stddef.h>
#define TC_COL_SIZE 8

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

#define TC_ROW_SIZE 8

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
	tc_square location;
} tc_piece_inst;
/*
  tc_piece_inst stands for
  terminal chess piece instance
  ~~~~~~~~~~~~~~ ~~~~~ ~~~~~~~~
        |          |       |
        |          |       |
  program name     |       |
                   |       |
               a piece...  |
                           |
                   ...on the board
*/

typedef struct
{
	tc_piece_type piece_moved;
	tc_square to_square;
} tc_move_attempt;

typedef struct
{
	tc_piece_inst piece_from;
	tc_piece_inst piece_to;
} tc_move;

/*
  Stores current position of pieces as well
  as the history of the game. That is to say,
  everything about a game.
 */
typedef struct
{
	size_t history_max;
	size_t history_size;
	tc_move* history_v;

	size_t piece_max;
	size_t piece_size;
	tc_piece_inst* piece_v;
} tc_board_state;

#endif
