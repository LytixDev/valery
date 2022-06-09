#!/bin/sh

mkdir -p "$HOME/.local/share/valery/bin"
bin_dir="$HOME/.local/share/valery/bin"

gcc -o bin/which bin/which.c
gcc -o valery -std=c11 valery.c utils/prompt.c utils/load_config.c utils/histfile.c

cp bin/which $bin_dir
