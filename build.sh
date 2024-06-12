#!/bin/bash

CFLAGS="-Wall -Wextra"

gcc bsbs.c -o bsbs $CFLAGS

if [[ $? -eq 0 ]]; then
    ./bsbs
    rm -f bsbs
else
    echo "Compilation failed"
    exit 1
fi
