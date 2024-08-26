#!/bin/bash

set -xe

CFLAGS="-Wall -Wextra"
LIBS="main.c libs/instant_input.c"

gcc $CFLAGS -o termchess $LIBS
