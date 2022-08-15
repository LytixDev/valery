#!/bin/sh
SRCS=$(find . -name "*.c" | grep -v "valery.c\|prompt.c")
CFLAGS="-I include -Wall -Wpedantic -Wextra -Wshadow -std=c99"

gcc -o interpreter-test $CFLAGS $SRCS
