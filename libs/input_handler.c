#include "input_handler.h"
#include "board_painter.h"
#include "chess_defs.h"
#include "chess_logic.h"
#include "instant_input.h"
#include "kbmacros.h"
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

// ######################### END OF MOVEMENT FUNCS #########################

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
		selected_piece = tc_index_square(&(si -> board),
										 &(si -> current_square),
										 &empty_square_flag);
		selected_square = si -> current_square;
		
		if (empty_square_flag)
			return;

		select_state = PIECE_SELECTED;
	}
	else
	{
		tc_piece_tp_unsafe(&(si -> board),
						   selected_piece,
						   &(si -> current_square));
		
		tc_empty_square(selected_square, si -> player_color);
		
		select_state = NOTHING_SELECTED;
	}
}

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
	case KB_ENTER:
		tc_select_square(si);
		break;
	case KB_ESC:
		return 0;
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
