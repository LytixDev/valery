#!/bin/sh
# stupid temporary build script

gcc -o valery-interpreter $(find ./src -name "*.c" | xargs) -g -DDEBUG -DDEBUG_INTERPRETER -I include
