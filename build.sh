#!/bin/sh
# stupid temporary build script

gcc -o valery-interpreter errors.c $(find ./interpreter -name "*.c" | xargs) -g -DDEBUG
