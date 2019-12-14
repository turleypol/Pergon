#!/bin/bash

if test -n "$1"; then
    utils/update.sh clean
fi
find . -type f -iname '*.inc' -printf "%h\n" |
    sort | uniq | xargs --delimiter "\n" \
        nice -16 scripts/ecompile -w -u -T -ri |
            egrep -v '(is up-to-date|^Writing:)'
