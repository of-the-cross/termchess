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

#define TP_ESC_CHAR ((char) 27)
#define TP_ESC(...) \
    printf("%c", 27);                           \
    printf(__VA_ARGS__)

#define TP_CURSOR_TO_HOME     TP_ESC("[H")
#define TP_CURSOR_INVISIBLE   TP_ESC("[?25l")
#define TP_CURSOR_VISIBLE     TP_ESC("[?25h")
#define TP_ALT_BUFFER_ENABLE  TP_ESC("[?1049h")
#define TP_ALT_BUFFER_DISABLE TP_ESC("[?1049l")
#define TP_CURSOR_SAVE        TP_ESC("[s")
#define TP_CURSOR_LOAD        TP_ESC("[u")
#define TP_SCREEN_WIPE        TP_ESC("[2J")

/*
  Switch to alternate buffer, wipe screen, and make cursor invisible.
 */
static inline void
tp_general_init(void)
{
    if (!isatty(STDOUT_FILENO))
    {
        fprintf(stderr, "FATAL ERROR: ");
        fprintf(stderr, "Output is not a terminal.\n");
        exit(-1);
    }
    
    TP_ALT_BUFFER_ENABLE;
    TP_SCREEN_WIPE;
    
    TP_CURSOR_TO_HOME;
    //  TP_CURSOR_INVISIBLE;
}

/*
  Undo everything dne by tp_general_init function
 */
static inline void
tp_general_cleanup(void)
{
    TP_CURSOR_VISIBLE;
    TP_ALT_BUFFER_DISABLE;
}

/*
  Call this function at the start of the program.
  The atexit call is very important for crashes!
 */
static inline void
tp_general_setup(void)
{
    tp_general_init();
    atexit(tp_general_cleanup);
}

/*
  Move cursor to the given line and column in the terminal.
 */
static inline void
tp_cursor_to(size_t line, size_t column)
{
    TP_ESC("[%lu;%luH", (size_t) line, (size_t) column);
}

#define TP_CURSOR_TO(line, column) TP_ESC("[%lu;%luH", (size_t) line, (size_t) column)

#endif
