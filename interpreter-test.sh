#!/bin/sh
#SRCS=$(find . -name "*.c" | grep -v "valery.c\|prompt.c")
SRCS=$(find src/valery/interpreter -name "*.c" | grep -v "\/lexer.c\|exec")

#CFLAGS="-I include -Wall -Wpedantic -Wextra -Wshadow -std=c99"
CFLAGS="-I include -std=c99 -g"

gcc $CFLAGS $SRCS "src/lib/hashtable.c"
