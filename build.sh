#!/bin/bash

set -xe

CFLAGS="-Wall -Wextra -g3 -ggdb3"
LIBS="main.c libs/instant_input.c libs/move_getter.c libs/board_painter.c libs/board_logic.c libs/panic.c libs/input_handler.c libs/piece_movement.c"

gcc $CFLAGS -o termchess $LIBS
