#ifndef TC_CHESS_LOGIC_H
#define TC_CHESS_LOGIC_H

#include "chess_defs.h"
#include <sys/cdefs.h>

/*
  Returns a tc_board_state with 0 moves (no history) with the
  default number of pieces and all the pieces in their default
  starting position
 */
tc_board_state
tc_new_default_board(void);

/*
  Check if two tc_squares contain the same data.
 */
int
tc_square_equals(const tc_square* sqr1, const tc_square* sqr2);

/*
  Find the index of a piece in the board whose location
  matches the square given into the function. If not found,
  the index is set to 0 and the inputted error flag is set
  to one.
 */
size_t
tc_square_id(const tc_board_state* board,
             const tc_square* square,
             int* errflag);

struct tc_translate_args
{
	tc_square* square;
	int drow;
	int dcol;
	int* errflag;
};

/*
  Never use arithmetic to manipulate tc_square datatypes, as it holds
  enums. Instead, use this function. It does whatever arithmetic you
  need to and, if the arithmetic leads to an invalid out of bounds
  square, the errflag is raised.

  Also, this is a struct because it is my personal philosophy that
  any function that takes in four parameters or more should have its
  own custom struct that holds its arguments. To interface with this:

    tc_translate((tc_translate_args) {
            .square = &square,
            .drow = 1, .dcol = 1,
            .errflag = &flag});

 */
void
tc_translate(struct tc_translate_args args);

/*
  Extremely raw function that moves some piece from one square
  to another square. It accesses pieces by index, meaning that
  it WILL get a piece (if it doesn't overflow) and rewrite
  that piece's location.

  Parent function needs to ensure that index is not greater
  than board -> piece_size. If index is greater than piece_max,
  it might crash the program. If index is greater than piece_size,
  it will give nonsense results.

  Ideally, index was taken from tc_index_square, meaning that index
  is ALWAYS a valid.
 */
void
tc_piece_tp_unsafe(const tc_board_state* board,
                   size_t id,
                   const tc_square* to_square);

/*
  Kill a piece given the piece's ID without bounds-checking, assuming
  that the ID given to this function was taken from tc_square_id.
  Also, due to some weird funkiness and optimization, killing a
  piece is actually just a swap and a reduction in the vector size.

  Here is an example of a vector containing our pieces:

  0      1      2      3      4      5      7        8        9
  +-------------------------------------------------------------------+
  | Pawn | Rook | Pawn | Pawn | King | Pawn | Bishop | Knight | Queen |
  +-------------------------------------------------------------------+
                                         |                        |
                                         |                        |
  Supposing we wanted to delete this, ---+                        |
  we just have to swap it out with                                |
  the piece that takes up the last -------------------------------+
  element in the vector.

  
  0      1      2      3      4      5       7        8        9
  +-------------------------------------------------------------------+
  | Pawn | Rook | Pawn | Pawn | King | Queen | Bishop | Knight | Pawn |
  +-------------------------------------------------------------------+
                                         |                         |
                                         +------------+------------+
                                                      |
                                                      |
                   These two switched places  --------+

  After we have done that, we will now reduce the size of the vector.
  Now the vector looks like this:
  
  0      1      2      3      4      5       7        8
  +------------------------------------------------------------+.......
  | Pawn | Rook | Pawn | Pawn | King | Queen | Bishop | Knight | Pawn .
  +------------------------------------------------------------+.......

  What this means is that, after tc_kill_unsafe is called on an ID, some
  random piece on the board will assume the ID of whichever piece was
  killed. This approach has some disadvantages and might make for a
  nasty surprise if you want to do anything like keeping track of your
  pieces, but the upside is that the vector remains contiguous.
*/
void
tc_kill_unsafe(tc_board_state* board, size_t id);

#endif
