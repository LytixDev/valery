#!/bin/sh
# stupid temporary build script

gcc -std=c11 -o valery-interpreter $(find ./src -name "*.c" | xargs) -g -DDEBUG -DDEBUG_INTERPRETER -I include
