#!/bin/sh

if [ "$(uname)" != "Linux" ]
then
    echo "Not running on gnu/linux. Results my vary. Will probably work for a unix like system with posix compliant c libs."
fi

name="valery"
compile="gcc -o $name -std=c11 valery.c utils/prompt.c utils/load_config.c utils/histfile.c utils/exec.c utils/lexer.c builtin/which.c builtin/cd.c builtin/history.c builtin/builtins.c builtin/help.c"

if [ "$1" = "debug" ]
then
    $compile -g
    echo "compiled executable $name with -g flag"
else
    $compile
    echo "compiled executable $name"
fi
