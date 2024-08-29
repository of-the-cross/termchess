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

#endif
