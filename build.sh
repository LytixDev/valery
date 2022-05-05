#!/bin/sh

mkdir -p "$HOME/.local/share/valery/bin"
bin_dir="$HOME/.local/share/valery/bin"

gcc -o bin/which bin/which.c
gcc -o valery valery.c

cp bin/which $bin_dir
