#ifndef INSTANT_INPUT_H
#define INSTANT_INPUT_H

/*
  Call this at the start of your program to make the terminal an
  instant-input terminal
*/
void
ii_init(void);

/*
  Gets the next character in the program. Crashes the program if
  ii_init has not been called.
 */
char
ii_next_char(void);

/*
  Undo ii_init. This turns the terminal from an instant-input
  terminal to a regular terminal that requires newlines for inputs
  to be read. This doesn't need to be called, as ii_init ensures
  that termmode_load_original runs at exit (atexit function) but
  this may be called regardless.
 */
void
termmode_load_original(void);

#endif
