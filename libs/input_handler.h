#ifndef TC_INPUT_HANDLER_H
#define TC_INPUT_HANDLER_H

#include "chess_defs.h"

/*
  This is a massive struct (tc_board_state is a massive datatype)
  so any function that as sessioninfo as a parameter OUGHT to only
  take pointers to the struct!
 */
struct sessioninfo
{
    tc_board_state board;
    tc_color player_color;
    tc_square current_square;
    tc_color current_turn;
};

/*
  Make a session with "default" parameters, which right now
  means the player is white, the cursor is on A1, and the board
  is in its default starting position as is defined in chess.

  This ought to be removed for some more interactive editor for
  sessions where the player can choose options, but that is in
  the future.
 */
struct sessioninfo
tc_new_default_session(void);

/*
  Main game loop that should be called by the main function.
 */
void
tc_game_loop(struct sessioninfo* si);

#endif
