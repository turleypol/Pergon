#!/bin/sh

unset FILES
for FILE in \
    pol.log \
    log/pol.log \
    cvs-stderr.log \
    cvs-stdout.log \
    map-stderr.log \
    map-stdout.log \
    pergon-stderr.log \
    pergon-stdout.log \
; do
    if test -r "$FILE"; then
        FILES="$FILES $FILE"
    fi
done

grep -hiE '(FEHLER|WARNUNG|error|warning)' $FILES | sort
