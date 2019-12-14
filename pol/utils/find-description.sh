#!/bin/bash

find . -type f -name itemdesc.cfg -print0 | xargs -0 grep -E $@
