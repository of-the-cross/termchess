#!/bin/bash

set -xe

CFLAGS="-Wall -Wextra"
LIBS="main.c"

gcc $CFLAGS -o termchess $LIBS
