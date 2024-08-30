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

struct append_moveset_args
{
    struct tc_moveset* moveset;
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
            .moveset = moveset,
            .to_square = square,
            .flags = TC_MOVE_CAPTURE,
            .id = captured_id,
        });
 */
static void
append_to_moveset(struct append_moveset_args args)
{
    /* Readability variable */
    struct tc_moveset* moveset = args.moveset;
    
    if (moveset -> count + 1 >= moveset -> max)
    {
        moveset -> max *= 2;
        PN_REALLOC(moveset -> array,
                   sizeof(*(moveset -> array)) * moveset -> max);
    }

    moveset -> array[moveset -> count].square = *args.to_square;
    moveset -> array[moveset -> count].flags  = args.flags;
    moveset -> array[moveset -> count].id     = args.id;
    ++(moveset -> count);
}

struct append_onlyif_capture_args
{
    const tc_board_state* board;
    const tc_square* square;
    const tc_color opp_color;
    struct tc_moveset* moveset;
};

/*
  Appends to the _moveset_ vector the square if the piece occupying the
  square matches the opposing color given by args.opp_color. Otherwise,
  the move will not be considered a valid move.This move is really only
  available for pawns---only pawns gain extra moves for captures.
  
  Also, this is a struct because it is my personal philosophy that
  any function that takes in four parameters or more should have its
  own custom struct that holds its arguments. To interface with this:
 */
static void
append_onlyif_capture(struct append_onlyif_capture_args args)
{
    int empty_square_flag = 0;
    size_t captured_id = tc_square_id(args.board,
                                      args.square,
                                      &empty_square_flag);
    if (empty_square_flag)
        return;
    
    tc_color captured_color = args.board -> piece_v[captured_id].color;

    if (captured_color == args.opp_color)
        append_to_moveset((struct append_moveset_args) {
                .moveset = args.moveset,
                .to_square = args.square,
                .flags = TC_MOVE_CAPTURE,
                .id = captured_id,
            });
}

struct append_empty_or_capture_args
{
    const tc_board_state* board;
    const tc_square* square;
    struct tc_moveset* moveset;
    tc_color opp_color;
    int* capture_flag;
    int* blocked_flag;
};

/*
  Appends to the _moveset_ vector the square if the square is empty
  or if the piece is occupied by the square is a piece of the
  opposing color. If the square is empty, no flags are raised.
  If the square is occupied by a piece of the opposing color,
  capture_flag is set to 1. If the square is occupied by a piece
  not of the opposing color, blocked_flag is set to 1.
 */
static void
append_empty_or_capture(struct append_empty_or_capture_args args)
{
    int empty_square_flag = 0;
    size_t captured_id = tc_square_id(args.board,
                                      args.square,
                                      &empty_square_flag);
    if (empty_square_flag)
    {
        append_to_moveset((struct append_moveset_args) {
                .moveset = args.moveset,
                .to_square = args.square,
                .flags = TC_MOVE_ORDINARY
            });
        return;
    }
    
    tc_color captured_color = args.board -> piece_v[captured_id].color;

    if (captured_color == args.opp_color)
    {
        append_to_moveset((struct append_moveset_args) {
                .moveset = args.moveset,
                .to_square = args.square,
                .flags = TC_MOVE_CAPTURE,
                .id = captured_id,
            });
        *(args.capture_flag) = 1;
        return;
    }
    
    *(args.blocked_flag) = 1;
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
pawn_moveset(const tc_board_state* board,
             size_t mover_id,
             struct tc_moveset* moveset)
{
    tc_color mover_color;
    tc_color opp_color;
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
        append_to_moveset((struct append_moveset_args) {
                .moveset = moveset,
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
        append_onlyif_capture((struct append_onlyif_capture_args) {
                .board = board,
                .square = &capture_square,
                .opp_color = opp_color,
                .moveset = moveset,
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
        append_onlyif_capture((struct append_onlyif_capture_args) {
                .board = board,
                .square = &capture_square,
                .opp_color = opp_color,
                .moveset = moveset,
            });
}

struct append_linear_args
{
    const tc_board_state* board;
    const tc_square* root_square;
    struct tc_moveset* moveset;
    tc_color opp_color;
    int row_step;
    int col_step;
};

/*
  Append to _moveset_ vector repeatedly in a line (the
  "line" being defined by given "steps") until there is
  a capture, a same-color piece blocking the path, or
  the end of the board is reached. If there is a capture,
  the square to that capture will of course be appended,
  but the loop will stop. Given this:
  
  +---+
  | p | <--- Piece that wishes to move
  +---+
  |   |
  +---+
  |   |
  +---+
  |   |
  +---+
  |   |
  +---+
  | e | <--- Enemy of opposite color
  +---+

  Then every square here will be appended. Like so:
  
  +---+
  | p |
  +---+
  | ! | <--- Appended with TC_MOVE_ORDINARY
  +---+
  | ! | <--- Appended with TC_MOVE_ORDINARY
  +---+
  | ! | <--- Appended with TC_MOVE_ORDINARY
  +---+
  | ! | <--- Appended with TC_MOVE_ORDINARY
  +---+
  | ! | <--- Appended with TC_MOVE_CAPTURE
  +---+

  If the piece on the bottom wasn't the opposing
  color, then every square up until that piece's
  square will be appended. Like so:
  
  +---+
  | p |
  +---+
  | ! | <--- Appended with TC_MOVE_ORDINARY
  +---+
  | ! | <--- Appended with TC_MOVE_ORDINARY
  +---+
  | ! | <--- Appended with TC_MOVE_ORDINARY
  +---+
  | ! | <--- Appended with TC_MOVE_ORDINARY
  +---+
  |   |
  +---+
  
 */
static void
append_linear(struct append_linear_args args)
{
    tc_square square;
    int obstructed_flag = 0;
    int out_of_bounds_flag = 0;
    square = *args.root_square;

    while (!obstructed_flag)
    {
        tc_translate((struct tc_translate_args) {
                .square = &square,
                .drow = args.row_step,
                .dcol = args.col_step,
                .errflag = &out_of_bounds_flag,
            });
        
        if (out_of_bounds_flag)
            return;
        
        append_empty_or_capture((struct append_empty_or_capture_args) {
                .board = args.board,
                .square = &square,
                .moveset = args.moveset,
                .opp_color = args.opp_color,
                .capture_flag = &obstructed_flag,
                .blocked_flag = &obstructed_flag,
            });
    }
}

/*
  +---+---+---+---+---+---+---+
  |   |   |   | p |   <--- Pieces of the same color block the square
  +---+---+---+---+---+--- So this should be an invalid square
  |   |   |   | ! |   |   |   |
  +---+---+---+---+---+---+---+
  |   |   |   | ! |   |   |   |
  +---+---+---+---+---+---+---+
  | ! | ! | ! | R | ! | ! | ! |
  +---+---+---+---+---+---+---+
  |   |   |   | ! |   |   |   |
  +---+---+---+---+---+---+---+
  |   |   |   | ! |   |   |   |
  +---+---+---+---+---+---+---+
  |   |   |   | p |   <-- Pieces of opposite color are fair game
  +---+---+---+---+---+-- So this should be a valid square

  Vertical and horizontal movement, basically.
 */
static void
rook_moveset(const tc_board_state* board,
             size_t mover_id,
             struct tc_moveset* moveset)
{
    tc_square mover_square;
    tc_color opp_color;
    mover_square = board -> piece_v[mover_id].location;
    opp_color = tc_enemy_color(board -> piece_v[mover_id].color);

    append_linear((struct append_linear_args) {
            .board       = board,
            .root_square = &mover_square,
            .moveset     = moveset,
            .opp_color   = opp_color,
            .row_step    = 1,
            .col_step    = 0,
        });
    
    append_linear((struct append_linear_args) {
            .board       = board,
            .root_square = &mover_square,
            .moveset     = moveset,
            .opp_color   = opp_color,
            .row_step    = -1,
            .col_step    = 0,
        });
    
    append_linear((struct append_linear_args) {
            .board       = board,
            .root_square = &mover_square,
            .moveset     = moveset,
            .opp_color   = opp_color,
            .row_step    = 0,
            .col_step    = 1,
        });
    
    append_linear((struct append_linear_args) {
            .board       = board,
            .root_square = &mover_square,
            .moveset     = moveset,
            .opp_color   = opp_color,
            .row_step    = 0,
            .col_step    = -1,
        });
}

/*
  +---+---+---+---+---+---+---+
  | ! |   |   |   |   |   | p | <--- Pieces of the same color block
  +---+---+---+---+---+---+---+      the square so this should be an
  |   | ! |   |   |   | ! |   |      invalid square
  +---+---+---+---+---+---+---+
  |   |   | ! |   | ! |   |   |
  +---+---+---+---+---+---+---+
  |   |   |   | B |   |   |   |
  +---+---+---+---+---+---+---+
  |   |   | ! |   | ! |   |   |
  +---+---+---+---+---+---+---+
  |   | ! |   |   |   | ! |   |      Pieces of opposite color are
  +---+---+---+---+---+---+---+      fair game so this should be a
  | ! |   |   |   |   |   | p | <--- valid square
  +---+---+---+---+---+---+---+

  Diagonal movement, basically.
 */
static void
bishop_moveset(const tc_board_state* board,
               size_t mover_id,
               struct tc_moveset* moveset)
{
    tc_square mover_square;
    tc_color opp_color;
    mover_square = board -> piece_v[mover_id].location;
    opp_color = tc_enemy_color(board -> piece_v[mover_id].color);

    append_linear((struct append_linear_args) {
            .board       = board,
            .root_square = &mover_square,
            .moveset     = moveset,
            .opp_color   = opp_color,
            .row_step    = 1,
            .col_step    = 1,
        });
    
    append_linear((struct append_linear_args) {
            .board       = board,
            .root_square = &mover_square,
            .moveset     = moveset,
            .opp_color   = opp_color,
            .row_step    = -1,
            .col_step    = 1,
        });
    
    append_linear((struct append_linear_args) {
            .board       = board,
            .root_square = &mover_square,
            .moveset     = moveset,
            .opp_color   = opp_color,
            .row_step    = 1,
            .col_step    = -1,
        });
    
    append_linear((struct append_linear_args) {
            .board       = board,
            .root_square = &mover_square,
            .moveset     = moveset,
            .opp_color   = opp_color,
            .row_step    = -1,
            .col_step    = -1,
        });
}

/*
  Set up a vector of squares where a piece is allowed to go.
  This does not keep chess-checks in mind.

  It does not have a return value. Instead, it will manipulate
  a reference to the moveset, which will be its return holder.
  We should ideally be using the same moveset throughout the
  entire runtime of the program. The return moveset will get bigger
  and bigger as more and more moves are appended to it, after all.
 */
static void
fetch_moveset(const tc_board_state* board,
              size_t mover_id,
              struct tc_moveset* moveset)
{
    moveset -> count = 0;
    switch (board -> piece_v[mover_id].type)
    {
    case tc_pawn:
        pawn_moveset(board, mover_id, moveset);
        break;
    case tc_bishop:
        bishop_moveset(board, mover_id, moveset);
        break;
    case tc_knight:
        pawn_moveset(board, mover_id, moveset);
        break;
    case tc_rook:
        rook_moveset(board, mover_id, moveset);
        break;
    case tc_queen:
        rook_moveset(board, mover_id, moveset);
        bishop_moveset(board, mover_id, moveset);
        break;
    case tc_king:
        pawn_moveset(board, mover_id, moveset);
        break;
    }
}

/*
  Glorified sequential search. Just checks to see if some
  tc_square move is contained in a tc_square array. tc_moveset
  contains the size of the array, so this _really is_ just a search
  function.
 */
static inline int
find_move_in_set(struct tc_moveset* moveset,
                 const tc_square* key)
{
    for (size_t i = 0; i < moveset -> count; ++i)
        if (tc_square_equals(&(moveset -> array[i].square), key))
            return i;   
    return -1;
}

/*
  See the valid_moves field in tc_evaluate_move function.

  valid_moves starts out with all its bits switched off,
  meaning that moveset -> array should be NULL. All this
  This function, then, will initialize valid_moves.
 */
static inline void
reset_moveset(struct tc_moveset* moveset)
{
    static const size_t initial_array_size = 12;

    if (moveset -> array == NULL)
    {
        moveset -> max = initial_array_size;
        moveset -> array = PN_MALLOC((sizeof(tc_square) + sizeof(char)) *
                                    initial_array_size);
    }
    
    moveset -> count = 0;
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
struct tc_move_info
tc_evaluate_move(const tc_board_state* board,
                 size_t mover_id,
                 const tc_square* to_square)
{
    static struct tc_moveset moveset = {0};

    reset_moveset(&moveset);
    fetch_moveset(board, mover_id, &moveset);

    int index = find_move_in_set(&moveset, to_square);
    if (index == -1)
        return (struct tc_move_info)
            { .type = i_INVALID_SQUARE };

    if (moveset.array[index].flags & TC_MOVE_CAPTURE)
        return (struct tc_move_info)
            {
                .type = v_OPP_CAPTURE,
                .captured_id = moveset.array[index].captured_id,
            };

    return (struct tc_move_info)
        { .type = v_EMPTY_SQUARE };
}
