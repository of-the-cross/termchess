/*
  This file is concerned with ANSI escape sequences to effectively
  paint onto a terminal.
 */
#ifndef PAINTER_H
#define PAINTER_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define ESC_CHAR ((char) 27)
#define ESC(s) printf("%c%s", 27, s)

#define PN_CURSOR_TO_HOME     ESC("[H")
#define PN_CURSOR_INVISIBLE   ESC("[?25l")
#define PN_CURSOR_VISIBLE     ESC("[?25h")
#define PN_ALT_BUFFER_ENABLE  ESC("[?1049h")
#define PN_ALT_BUFFER_DISABLE ESC("[?1049l")
#define PN_CURSOR_SAVE        ESC("[s")
#define PN_CURSOR_LOAD        ESC("[u")
#define PN_SCREEN_WIPE        ESC("[2J")

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

#endif
