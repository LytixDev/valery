#!/bin/sh
# Nicolai Brand (lytix.dev) 2022
# See LICENSE for license info

# Script used to generate compilation info and tags to LSP

make clean 2>/dev/null
bear -- make 2>/dev/null
ctags -R
