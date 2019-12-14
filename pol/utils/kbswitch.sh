#!/bin/sh

OLD='pkg/std/cooking'
NEW='pkg/skills/kochenbraten'

function disenable {
    FILE="$1"
    ENAB="$2"
    sed -i -re 's,^(Enabled).*,\1 '"$ENAB"',' "$FILE"
}

if test -z "$1" -o "$1" != 'new'; then
    echo "Enabling old cooking system"
    disenable "$OLD/pkg.cfg" 1
    disenable "$NEW/pkg.cfg" 0
    scripts/ecompile "$OLD/cooking.src"
else
    echo "Enabling new cooking system"
    disenable "$OLD/pkg.cfg" 0
    disenable "$NEW/pkg.cfg" 1
    scripts/ecompile "$NEW/cooking.src"
fi
