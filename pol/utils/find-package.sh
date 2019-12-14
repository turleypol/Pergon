#!/bin/bash

find . -type f -name pkg.cfg -print0 | xargs -0 grep -l $@
