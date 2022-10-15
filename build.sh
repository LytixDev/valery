#!/bin/sh
# stupid temporary build script

gcc -o valery-interpreter errors.c $(find ./src -name "*.c" | xargs) -g -DDEBUG -I include
