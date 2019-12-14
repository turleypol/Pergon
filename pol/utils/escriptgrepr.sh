#!/bin/sh

# Aufrufformat: $0 [-<Grep-Parameter>] "Suchbegriff" [Pfad(e)]
unset GREPARGS
if echo "$1" | grep --quiet "^-"; then
    GREPARGS="$1"
    shift
fi
SEARCH="$1"
shift
if test -n "$2"; then
    DIRS="$@"
else
    DIRS="pkg scripts"
fi

find $DIRS -type f '(' -name '*.src' -o -name '*.inc' ')' -print0 |
    xargs -0 grep $GREPARGS "$SEARCH" | sort
