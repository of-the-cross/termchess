#include "input_handler.h"
#include "board_painter.h"
#include "chess_defs.h"
#include "board_logic.h"
#include "instant_input.h"
#include "kbmacros.h"
#include "piece_movement.h"
#include <sys/cdefs.h>

/*
  Make a session with "default" parameters, which right now
  means the player is white, the cursor is on A1, and the board
  is in its default starting position as is defined in chess.

  This ought to be removed for some more interactive editor for
  sessions where the player can choose options, but that is in
  the future.
 */
struct sessioninfo
tc_new_default_session(void)
{
    return (struct sessioninfo)
        {
            .board = tc_new_default_board(),
            .player_color = tc_white,
            .current_square = (tc_square) {tc_a, tc_1},
            .current_turn = tc_white,
        };
}

/*
  Updates the terminal. What this means is that it prints
  the pieces and snaps the cursor in the terminal to whichever
  square is being pointed to according to the user's sessioninfo.
 */
static void
tc_repaint(struct sessioninfo* si)
{
    tc_print_pieces(&(si -> board), si -> player_color);
    tc_cursor_to_square(si -> current_square, si -> player_color);
}

/*
  Translate the current_square (remember: that is the selected square,
  the square which our terminal cursor is hovering over) whichever way
  the user wishes. This also keeps into mind the fact that this could
  be different depending on which side of the board the user is in.
 */
static inline void
tc_cursor_translate(struct sessioninfo* si,
                    int drow,
                    int dcol)
{
    tc_translate((struct tc_translate_args) {
            .square  = &(si -> current_square),
            .drow    = drow * tc_forward(si -> player_color),
            .dcol    = dcol * tc_forward(si -> player_color),
            .errflag = NULL,
        });
}

/*
  Big mama of a function. Handles anything and everything to do with
  movement. If a square which contains a piece is selected, and then
  another square is selected, the location (member variable) of that
  piece on that square gets mutated to be that second square.

  Call this function when the user wishes to select a tile.
 */
static void
tc_select_square(struct sessioninfo* si)
{
    enum select_type
        {
            NOTHING_SELECTED,
            PIECE_SELECTED,
        };
    
    static enum select_type select_state = NOTHING_SELECTED;
    static size_t selected_piece;
    static tc_square selected_square;

    if (select_state == NOTHING_SELECTED)
    {
        int empty_square_flag = 0;
        selected_piece = tc_square_id(&(si -> board),
                                      &(si -> current_square),
                                      &empty_square_flag);
        selected_square = si -> current_square;
        
        if (empty_square_flag)
            return;

        if (si -> board.piece_v[selected_piece].color != si -> current_turn)
            return;

        select_state = PIECE_SELECTED;
    }
    else
    {
        struct tc_move_info move_info;
        move_info = tc_evaluate_move(&(si -> board),
                                     selected_piece,
                                     &(si -> current_square));

        if (tc_is_valid_move(move_info))
        {            
            si -> current_turn = tc_enemy_color(si -> current_turn);
        
            tc_piece_tp_unsafe(&(si -> board),
                               selected_piece,
                               &(si -> current_square));
            
            if (move_info.type == v_OPP_CAPTURE)
                tc_kill_unsafe(&(si -> board), move_info.captured_id);
        
            tc_empty_square(selected_square, si -> player_color);
        }
        
        select_state = NOTHING_SELECTED;
    }
}

/*
  Master function for all input handling. Given some char input
  and a struct sessioninfo, it will call the functions aligning
  with that char input, modifying sessioninfo.
 */
static int
handle_input(struct sessioninfo* si, char cinput)
{
    int drow;
    int dcol;
    
    switch (cinput) {
    case 'w':
        drow = 1;
        dcol = 0;
        break;
    case 'W':
        drow = 2;
        dcol = 0;
        break;
    case 'a':
        drow = 0;
        dcol = -1;
        break;
    case 'A':
        drow = 0;
        dcol = -2;
        break;
    case 's':
        drow = -1;
        dcol = 0;
        break;
    case 'S':
        drow = -2;
        dcol = 0;
        break;
    case 'd':
        drow = 0;
        dcol = 1;
        break;
    case 'D':
        drow = 0;
        dcol = 2;
        break;
    case KB_ENTER:
        tc_select_square(si);
        return 1;
    case KB_ESC:
        return 0;
    }
    
    tc_cursor_translate(si, drow, dcol);
    return 1;
}

/*
  Main game loop that should be called by the main function.
  This calls three functions:

  ii_next_char    - Prompts the user for a keystroke
  handle_input    - Modifies game state (si) based on keystroke
  tc_repaint      - Repaints the terminal with the new game state
                    in mind
 */
void
tc_game_loop(struct sessioninfo* si)
{
    do tc_repaint(si);
    while (handle_input(si, ii_next_char()));
}
