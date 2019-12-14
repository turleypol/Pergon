#!/bin/sh

COREGZ=`ls pol-linux-x86*.tar.gz | tail -n 1`
tar -xzf "$COREGZ" pol scripts/ecompile scripts/runecl uoconvert uotool
mv uoconvert uotool tools/
