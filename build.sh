#!/bin/sh

if [ "$(uname)" != "Linux" ]
then
    echo "Not running on gnu/linux. Results my vary. Will probably work for a unix like system with posix compliant c libs."
fi

mkdir -p "$HOME/.local/share/valery/bin"
bin_dir="$HOME/.local/share/valery/bin"

gcc -o bin/which bin/which.c
gcc -o valery -std=c11 valery.c utils/prompt.c utils/load_config.c utils/histfile.c utils/exec.c

cp bin/which $bin_dir
