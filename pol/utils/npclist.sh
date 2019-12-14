#!/bin/sh

expand config/npcdesc.cfg |
    grep -iE "^ *\<(NPCTemplate|Name)\>" |
    sed -re 's,.+Name +(.*),Name \1\n,' > npclist.txt
