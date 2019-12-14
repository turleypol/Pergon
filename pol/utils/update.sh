#!/bin/sh

# do clean up before?
if test -n "$1"; then
    echo "Removing old CVS files ..."
    find . -name '.#*' -type f -print0 | xargs --null --no-run-if-empty rm
    if test "$1" = "rebuild"; then
        echo "Removing old compiled files in 5 seconds ..."
        sleep 5
        find . -type f \( -name '*.dep' -o -name '*.ecl' \) -print0 |
            xargs --null --no-run-if-empty rm
    fi
    echo "Done"
fi
nice -16 scripts/ecompile -w -r . -u |
    egrep -v '(is up-to-date|^Writing:)'
