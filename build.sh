#!/bin/sh

if [ "$(uname)" != "Linux" ]
then
    echo "Not running on gnu/linux. Results my vary. Will probably work for a unix like system with posix compliant c libs."
fi

name="valery"
include="valery.c utils/prompt.c utils/load_config.c utils/histfile.c utils/exec.c utils/lexer.c \
         builtins/which.c builtins/cd.c builtins/history.c builtins/builtins.c builtins/help.c"
compile="gcc -o $name -std=c11 $include"

if [ "$1" = "debug" ]
then
    $compile -g
    echo "compiled executable $name with -g flag"
else
    $compile
    echo "compiled executable $name"
fi
