/*
  This here is how the board should look empty:
  +---+---+---+---+---+---+---+---+
  |   |   |   |   |   |   |   |   |
  +---+---+---+---+---+---+---+---+
  |   |   |   |   |   |   |   |   |
  +---+---+---+---+---+---+---+---+
  |   |   |   |   |   |   |   |   |
  +---+---+---+---+---+---+---+---+
  |   |   |   |   |   |   |   |   |
  +---+---+---+---+---+---+---+---+
  |   |   |   |   |   |   |   |   |
  +---+---+---+---+---+---+---+---+
  |   |   |   |   |   |   |   |   |
  +---+---+---+---+---+---+---+---+
  |   |   |   |   |   |   |   |   |
  +---+---+---+---+---+---+---+---+
  |   |   |   |   |   |   |   |   |
  +---+---+---+---+---+---+---+---+

  And here is how it should look filled:
  +---+---+---+---+---+---+---+---+
  | R | N | B | Q | K | B | N | R |
  +---+---+---+---+---+---+---+---+
  | p | p | p | p | p | p | p | p |
  +---+---+---+---+---+---+---+---+
  |   |   |   |   |   |   |   |   |
  +---+---+---+---+---+---+---+---+
  |   |   |   |   |   |   |   |   |
  +---+---+---+---+---+---+---+---+
  |   |   |   |   |   |   |   |   |
  +---+---+---+---+---+---+---+---+
  |   |   |   |   |   |   |   |   |
  +---+---+---+---+---+---+---+---+
  | p | p | p | p | p | p | p | p |
  +---+---+---+---+---+---+---+---+
  | R | N | B | Q | K | B | N | R |
  +---+---+---+---+---+---+---+---+
  
*/
#include "board_painter.h"
#include "chess_defs.h"
#include "term_painter.h"
#include <stdio.h>

void
print_horizontal_line(void)
{
    puts("+---+---+---+---+---+---+---+---+");
}

void
print_free_spaces(void)
{
    puts("|   |   |   |   |   |   |   |   |");
}

/*
  Draws the board. This function ought to be only called once
  at the start of the program.
 */
void
tc_print_board(void)
{
    print_horizontal_line();
    for (int i = 0; i < 8; ++i)
    {
        print_free_spaces();
        print_horizontal_line();
    }
    printf("\nHit ESC key to exit.");
}

/*
  Prints the emoji associated to whichever piece is
  inputted into this function.
*/
void
print_raw_piece(tc_piece_inst piece)
{
#define EMPTY_PIECE (tc_color) (tc_black)
    switch (piece.type) {
    case tc_pawn:
        if (piece.color == EMPTY_PIECE)
            printf("♙");
        else
            printf("♟");
        break;
    case tc_bishop:
        if (piece.color == EMPTY_PIECE)
            printf("♗");
        else
            printf("♝");
        break;
    case tc_knight:
        if (piece.color == EMPTY_PIECE)
            printf("♘");
        else
            printf("♞");
        break;
    case tc_rook:
        if (piece.color == EMPTY_PIECE)
            printf("♖");
        else
            printf("♜");
        break;
    case tc_queen:
        if (piece.color == EMPTY_PIECE)
            printf("♕");
        else
            printf("♛");
        break;
    case tc_king:
        if (piece.color == EMPTY_PIECE)
            printf("♔");
        else
            printf("♚");
        break;
    }
}

/*
  Convert a chess row managed by game logic to a
  terminal row managed by graphics.
 */
size_t
chessrow_to_termrow(size_t row,
                    tc_color color)
{
    if (color == tc_white)
        row = TC_ROW_SIZE - row - 1;
    row *= 2;
    row += 2;
    return row;
}

/*
  Convert a chess column managed by game logic to a
  terminal column managed by graphics.
 */
size_t chesscol_to_termcol(size_t col,
                           tc_color color)
{
    if (color == tc_black)
        col = TC_COL_SIZE - col - 1;
    
    col *= 4;
    col += 3;
    return col;
}

/*
  Given some square (as is understood by game logic),
  move the cursor to its graphical position in the terminal.
 */
void
tc_cursor_to_square(tc_square square,
                    tc_color color)
{
    size_t row = chessrow_to_termrow(square.row, color);
    size_t col = chesscol_to_termcol(square.col, color);
    tp_cursor_to(row, col);
}

/*
  Print a piece instance with its proper symbol on its
  proper square. This uses the gamelogic-to-terminal-graphics
  process discussed above.
 */
void
tc_print_placed_piece(tc_piece_inst piece,
                      tc_color color)
{
    tc_cursor_to_square(piece.location, color);
    print_raw_piece(piece);
}

/*
  Print whitespace over some defined square in the terminal.
  This uses the gamelogic-to-terminal-graphics process
  discussed above.
 */
void
tc_empty_square(tc_square square,
                tc_color color)
{
    tc_cursor_to_square(square, color);
    putchar(' ');
}

/*
  Paint all the pieces to the terminal given a tc_board_state
  type. tc_print_board must be called before this is called
  or everything will look funky.
 */
void
tc_print_pieces(tc_board_state* board_state,
                tc_color color)
{
    size_t piece_count = board_state -> piece_size;

    for (size_t i = 0; i < piece_count; ++i)
        tc_print_placed_piece(board_state -> piece_v[i], color);
}
