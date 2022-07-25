#!/bin/sh

if [ "$(uname)" != "Linux" ]
then
    echo "Not running on gnu/linux. Results my vary. Will probably work for a unix like system with posix compliant c libs."
fi

name="valery"
include="valery.c utils/prompt.c utils/load_config.c utils/histfile.c utils/exec.c utils/lexer.c \
         builtins/which.c builtins/cd.c builtins/history.c builtins/builtins.c builtins/help.c"
compile="gcc -o $name -std=c11 $include"

name_test="valery_test"
include_test="tests/memory_leak_test.c utils/load_config.c utils/histfile.c utils/exec.c utils/lexer.c \
         builtins/which.c builtins/cd.c builtins/history.c builtins/builtins.c builtins/help.c"
compile_test="gcc -o $name_test -std=c11 $include_test -g"


if [ "$1" = "debug" ]
then
    $compile -g
    echo "compiled executable $name with -g flag"
elif [ "$1" = "test" ]
then
    $compile_test
    echo "compiled test $name_test"
else
    $compile
    echo "compiled executable $name"
fi
