#ifndef BOARD_PAINTER_H
#define BOARD_PAINTER_H

#include "chess_defs.h"

/*
  Draws the board. This function ought to be only called once
  at the start of the program.
 */
void
tc_print_board(void);

/*
  Paint all the pieces to the terminal given a tc_board_state
  type. tc_print_board must be called before this is called
  or everything will look funky.
 */
void
tc_print_pieces(tc_board_state* board_state,
                tc_color color);

/*
  Print a piece instance with its proper symbol on its
  proper square.
 */
void
tc_print_placed_piece(tc_piece_inst piece,
                      tc_color color);

/*
  Print whitespace over some defined square in the terminal.
 */
void
tc_empty_square(tc_square square,
                tc_color color);

/*
  Move cursor to its equivalent graphical location in the
  terminal.
 */
void
tc_cursor_to_square(tc_square square,
                    tc_color color);

#endif
