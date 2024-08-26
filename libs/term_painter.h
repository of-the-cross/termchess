/*
  This file is concerned with ANSI escape sequences to effectively
  paint onto a terminal.
 */
#ifndef TERMPAINTER_H
#define TERMPAINTER_H
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PN_ESC_CHAR ((char) 27)
#define PN_ESC(...) \
	printf("%c", 27);							\
	printf(__VA_ARGS__)

#define PN_CURSOR_TO_HOME     PN_ESC("[H")
#define PN_CURSOR_INVISIBLE   PN_ESC("[?25l")
#define PN_CURSOR_VISIBLE     PN_ESC("[?25h")
#define PN_ALT_BUFFER_ENABLE  PN_ESC("[?1049h")
#define PN_ALT_BUFFER_DISABLE PN_ESC("[?1049l")
#define PN_CURSOR_SAVE        PN_ESC("[s")
#define PN_CURSOR_LOAD        PN_ESC("[u")
#define PN_SCREEN_WIPE        PN_ESC("[2J")

/*
  Switch to alternate buffer, wipe screen, and make cursor invisible.
 */
static inline void
pn_general_init(void)
{
	if (!isatty(STDOUT_FILENO))
	{
		fprintf(stderr, "FATAL ERROR: ");
		fprintf(stderr, "Output is not a terminal.\n");
		exit(-1);
	}
	
	PN_ALT_BUFFER_ENABLE;
	PN_SCREEN_WIPE;
	
	PN_CURSOR_TO_HOME;
	PN_CURSOR_INVISIBLE;
}

/*
  Undo everything dne by pn_general_init function
 */
static inline void
pn_general_cleanup(void)
{
	PN_CURSOR_VISIBLE;
	PN_ALT_BUFFER_DISABLE;
}

/*
  Call this function at the start of the program.
  The atexit call is very important for crashes!
 */
static inline void
pn_general_setup(void)
{
	pn_general_init();
	atexit(pn_general_cleanup);
}

/*
  Move cursor to the given line and column in the terminal.
 */
static inline void
pn_cursor_to(size_t line, size_t column)
{
	PN_ESC("[%lu;%luH", (size_t) line, (size_t) column);
}

#define PN_CURSOR_TO(line, column) PN_ESC("[%lu;%luH", (size_t) line, (size_t) column)

#endif
