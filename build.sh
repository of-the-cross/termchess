#!/bin/bash

set -xe

CFLAGS="-Wall -Wextra"
LIBS="main.c libs/instant_input.c libs/move_getter.c"

gcc $CFLAGS -o termchess $LIBS
