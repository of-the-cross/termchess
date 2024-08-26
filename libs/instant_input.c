/*
     All info about this file can be found in
     https://man7.org/linux/man-pages/man3/termios.3.html

     --------------------------------------------------------------
     
     Raw mode section:
     
     cfmakeraw() sets the terminal to something like the "raw" mode of
     the old Version 7 terminal driver: input is available character
     by character, echoing is disabled, and all special processing of
     terminal input and output characters is disabled.

     Therefore disabling ECHO by way of
     	termios_p -> c_iflag &= ~ECHO
     will ensure that what the user types will not be shown.

     --------------------------------------------------------------
     
    Canonical and noncanonical mode section:
   
    The setting of the ICANON canon flag in c_lflag determines
    whether the terminal is operating in canonical mode (ICANON set)
    or noncanonical mode (ICANON unset).  By default, ICANON is set.
   
     In canonical mode:
   
    •  Input is made available line by line.  An input line is
       available when one of the line delimiters is typed (NL, EOL,
       EOL2; or EOF at the start of line).  Except in the case of
       EOF, the line delimiter is included in the buffer returned by
       read(2).
   
    •  Line editing is enabled (ERASE, KILL; and if the IEXTEN flag
       is set: WERASE, REPRINT, LNEXT).  A read(2) returns at most
       one line of input; if the read(2) requested fewer bytes than
       are available in the current line of input, then only as many
       bytes as requested are read, and the remaining characters will
       be available for a future read(2).
   
    •  The maximum line length is 4096 chars (including the
       terminating newline character); lines longer than 4096 chars
       are truncated.  After 4095 characters, input processing (e.g.,
       ISIG and ECHO* processing) continues, but any input data after
       4095 characters up to (but not including) any terminating
       newline is discarded.  This ensures that the terminal can
       always receive more input until at least one line can be read.
   
    In noncanonical mode input is available immediately (without the
    user having to type a line-delimiter character), no input
    processing is performed, and line editing is disabled.  The read
    buffer will only accept 4095 chars; this provides the necessary
    space for a newline char if the input mode is switched to
    canonical. (...)
   
*/

#include "instant_input.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

struct termios orig_termios;
struct termios used_termios;
int initialized = 0;

/*
  Resets the terminal's state, all its modes, to origin_termios.
  Must be called after termmode_save_original is already called,
  and likely at the end of the program when the program exits.
  with that said, termmode_save_original automatically sets this
  function to be called upon exit, so there is no need to ever
  call this function.
*/
void
termmode_load_original()
{ tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios); }

/*
  Save the terminal's state as it is right now. Should be
  called at the start of the program so that program doesn't
  have lasting effects on a terminal throughout a session.
*/
void
termmode_save_original()
{
  tcgetattr(STDIN_FILENO, &orig_termios);
  used_termios = orig_termios;
  atexit(termmode_load_original);
}

/*
  Activates all the settings set to used_termios to
  the actual terminal. Must be called for changes to set.
*/
void
termmode_set_modes()
{ tcsetattr(STDIN_FILENO, TCSAFLUSH, &used_termios); }

/*
  Disable echo when user types to terminal. Must be set
  with termmode_set_modes() first.
*/
void
termmode_raw()
{ used_termios.c_lflag &= ~ECHO; }

/*
  Enable echo when user types to terminal be set with
  termmode_set_modes() first.
*/
void
termmode_nonraw()
{ used_termios.c_lflag |= ECHO; }

/*
  Waits until return character before parsing input.
  Must be set with termmode_set_modes() first.
*/
void
termmode_canon()
{ used_termios.c_lflag |= ICANON; }

/*
  Parse console input byte-by-byte, not waiting for a return.
  Must be set with termmode_set_modes() first.
*/
void
termmode_noncanon()
{ used_termios.c_lflag &= ~ICANON; }

/*
  Exit program automatically if ii_next_char() is called without
  calling ii_init() first
*/
char
uninitialized_read(void)
{
	fprintf(stderr, "FATAL ERROR: ");
	fprintf(stderr, "Call ii_init() first before calling ii_next_char()\n");
	exit(-1);
}

/*
  If ii_next_char() has been called after ii_init() has also been
  called, then the program is free to call fgetc
*/
char
initialized_read(void)
{
	return fgetc(stdin);
}

/*
  Function which is either an error (uninitialized_read) or is
  an fgetc wrapper (initialized_read)
*/
char (*next_char_func)(void) = &uninitialized_read;

/*
  Wrapper for next_char_func
*/
char
ii_next_char(void)
{
	return (*next_char_func)();
}

/*
  Call this at the start of your program to make the terminal an
  instant-input terminal
*/
void
ii_init(void)
{
  termmode_save_original();
  termmode_raw();
  termmode_noncanon();
  termmode_set_modes();
  
  next_char_func = &initialized_read;
}
