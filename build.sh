#!/bin/bash

set -xe

CFLAGS="-Wall -Wextra -g3 -ggdb3"
LIBS="main.c libs/*.c"

gcc $CFLAGS -o termchess $LIBS
