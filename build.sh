#!/bin/sh

if [ "$(uname)" != "Linux" ]
then
    echo "Not running on gnu/linux. Results my vary. Will probably work for a unix like system with posix compliant c libs."
fi

name="valery"
include="src/valery.c src/builtins/* src/valery/*"
compile="gcc -o $name -std=c11 -I include/ $include"

if [ "$1" = "debug" ]
then
    $compile -g
    echo "compiled executable $name with -g flag"
elif [ "$1" = "dev" ]
then
    bear -- $compile
    ctags -R
    echo "generated compilation database for clangd"
else
    $compile
    echo "compiled executable $name"
fi
