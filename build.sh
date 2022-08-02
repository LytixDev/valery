#!/bin/sh

if [ "$(uname)" != "Linux" ]
then
    echo "Not running on gnu/linux. Results my vary. Will probably work for a unix like system with posix compliant c libs."
fi

name="valery"
include="src/valery.c src/builtins/* src/valery/* src/lib/*"
cflags="-o $name -Wall -Wpedantic -Wextra -Wshadow -std=c99 -I include/"
compile="gcc $cflags $include"

if [ "$1" = "debug" ]
then
    $compile -g
    echo "compiled executable $name with -g flag"
elif [ "$1" = "dev" ]
then
    bear -- $compile
    ctags -R
    echo "generated compilation database for clangd"
elif [ "$1" = "optimized" ]
then
    compile="gcc $cflags -O3 $include"
    $compile
    echo "compiled executable $name with aggressive optmization"
elif [ "$1" = "profile" ]
then
    compile="gcc $cflags -pg -D PROFILE $include"
    $compile
    echo "compiled executable $name for profiling"
else
    $compile
    echo "compiled executable $name"
fi
