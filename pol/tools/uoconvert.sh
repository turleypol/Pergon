#!/bin/sh

if ! test -f pol; then
    echo "Error: must be run in pol directory!" >&2
    exit 1
fi

if ! test -f uoconvert.cfg; then
    echo "Notice: No configuration file 'uoconvert.cfg' found" >&2
fi

tools/uoconvert map     realm=britannia \
    mapid=0 usedif=0 width=6144 height=4096
tools/uoconvert statics realm=britannia
tools/uoconvert maptile realm=britannia

tools/uoconvert map     realm=ilshenar \
    mapid=2 usedif=0 width=2304 height=1600
tools/uoconvert statics realm=ilshenar
tools/uoconvert maptile realm=ilshenar

tools/uoconvert map     realm=malas \
    mapid=3 usedif=0 width=2560 height=2048
tools/uoconvert statics realm=malas
tools/uoconvert maptile realm=malas

tools/uoconvert map     realm=tokuno \
    mapid=4 usedif=0 width=1448 height=1448
tools/uoconvert statics realm=tokuno
tools/uoconvert maptile realm=tokuno

#tools/uoconvert map     realm=termur \
#    mapid=5 usedif=0 width=1280 height=4096
#tools/uoconvert statics realm=termur
#tools/uoconvert maptile realm=termur

#for REALM in britannia ilshenar malas tokuno termur; do
#    mv "realm/$REALM/"* "../realm/$REALM/"
#    rmdir "realm/$REALM"
#done
#rmdir realm

tools/uoconvert multis
mv multis.cfg config

tools/uoconvert tiles
mv tiles.cfg config

tools/uoconvert landtiles
mv landtiles.cfg config
