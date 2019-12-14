#!/bin/bash

# Binaries ausfuehrbar machen
for FILE in pol scripts/ecompile scripts/runecl tools/uoconvert; do
    chmod u+x "$FILE"
done

shopt -s nullglob
chmod u+x utils/*.sh
chmod u+x utils/*.pl
