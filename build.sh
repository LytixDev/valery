#!/bin/sh

if [ "$(uname)" != "Linux" ]
then
    echo "Not running on gnu/linux. Results my vary. Will probably work for a unix like system with posix compliant c libs."
fi

bin_dir="$HOME/.local/share/valery/bin"
mkdir -p "$bin_dir"

gcc -o valery -std=c11 valery.c utils/prompt.c utils/load_config.c utils/histfile.c utils/exec.c utils/lexer.c builtin/which.c builtin/cd.c builtin/history.c
