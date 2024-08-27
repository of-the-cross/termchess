#include "input_handler.h"
#include "board_painter.h"
#include "chess_defs.h"
#include "chess_logic.h"
#include "instant_input.h"
#include <ctype.h>

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
			tc_new_default_board(),
			tc_white,
			(tc_square) {tc_a, tc_1},
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

// ######################### MOVEMENT FUNCTIONS #########################

static void
tc_move_up(struct sessioninfo* si)
{
	if (si -> player_color == tc_black)
	{
		if (si -> current_square.row <= 0)
		{
			si -> current_square.row = 0;
			return;
		}
		si -> current_square.row -= 1;
	}
	
	if (si -> current_square.row >= TC_ROW_SIZE - 1)
	{
		si -> current_square.row = TC_ROW_SIZE - 1;
		return;
	}
	si -> current_square.row += 1;
}

static void
tc_move_left(struct sessioninfo* si)
{
	if (si -> player_color == tc_black)
	{	
		if (si -> current_square.col >= TC_COL_SIZE - 1)
		{
			si -> current_square.col = TC_COL_SIZE - 1;
			return;
		}
		si -> current_square.col += 1;
	}
	
	if (si -> current_square.col <= 0)
	{
		si -> current_square.col = 0;
		return;
	}
	si -> current_square.col -= 1;
}

static void
tc_move_down(struct sessioninfo* si)
{
	if (si -> player_color == tc_black)
	{
		if (si -> current_square.row >= TC_ROW_SIZE - 1)
		{
			si -> current_square.row = TC_ROW_SIZE - 1;
			return;
		}
		si -> current_square.row += 1;
	}
	
	if (si -> current_square.row <= 0)
	{
		si -> current_square.row = 0;
		return;
	}
	si -> current_square.row -= 1;
}

static void
tc_move_right(struct sessioninfo* si)
{
	if (si -> player_color == tc_black)
	{
		if (si -> current_square.col <= 0)
		{
			si -> current_square.col = 0;
			return;
		}
		si -> current_square.col -= 1;
	}
	
	if (si -> current_square.col >= TC_COL_SIZE - 1)
	{
		si -> current_square.col = TC_COL_SIZE - 1;
		return;
	}
	si -> current_square.col += 1;
}

// ######################### MOVEMENT FUNCTIONS #########################

/*
  Master function for all input handling. Given some char input
  and a struct sessioninfo, it will call the functions aligning
  with that char input, modifying sessioninfo.

  The function assumes that cinput is already in uppercase, so
  toupper() must be called before any char input is called!
 */
static int
handle_input(struct sessioninfo* si, char cinput)
{
	switch (cinput) {
	case 'W':
		tc_move_up(si);
		break;
	case 'A':
		tc_move_left(si);
		break;
	case 'S':
		tc_move_down(si);
		break;
	case 'D':
		tc_move_right(si);
		break;
	case 27:					// Escape key
		return 0;				// ends the game loop
	}
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
	while (handle_input(si, toupper(ii_next_char())));
}
