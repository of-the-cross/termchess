/*
  This is a purely temporary header file for testing. Expect
  this file to disappear very quickly in the following commits.
 */

#ifndef TC_MOVE_PRINTER_H
#define TC_MOVE_PRINTER_H

#include "chess_defs.h"
#include "move_getter.h"
#include <stdio.h>

/*
  Take a move attempt and print that out in the form of
      [PIECE] to [COL][ROW]
	  
  This may look for example like:
      Pawn to e4
 */
static inline void
tc_print_attempt(tc_move_attempt* attempt)
{
	switch (attempt -> piece_moved) {
	case tc_pawn:
		printf("Pawn");
		break;
	case tc_bishop:
		printf("Bishop");
		break;
	case tc_knight:
		printf("Knight");
		break;
	case tc_rook:
		printf("Rook");
		break;
	case tc_queen:
		printf("Queen");
		break;
	case tc_king:
		printf("King");
		break;
	}

	printf(" to ");

	int column = (int) attempt -> to_square.col;
	column += 'a';
	printf("%c", column);

	int row = (int) attempt -> to_square.row;
	row += 1;
	printf("%d", row);
	
	printf("\n");
}

/*
  Prompt the user for a move attempt, and then
  try to print that out
 */
static inline int
tc_get_n_print_attempt(void)
{
	tc_move_attempt move;
	int parse_error = 0;

	move = tc_type_move_attempt(&parse_error);
	if (parse_error)
		return 1;

	tc_print_attempt(&move);

	return 0;
}

#endif
