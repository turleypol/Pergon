tools\uoconvert multis
move multis.cfg config

tools\uoconvert tiles
move tiles.cfg config

tools\uoconvert landtiles
move landtiles.cfg config

rem tools\uoconvert map     realm=britannia mapid=0 usedif=0 width=7168 height=4096
tools\uoconvert map     realm=britannia mapid=0 usedif=0 width=6144 height=4096
tools\uoconvert statics realm=britannia
tools\uoconvert maptile realm=britannia

rem tools\uoconvert map     realm=britannia_alt mapid=1 usedif=0 width=7168 height=4096
rem tools\uoconvert map     realm=britannia_alt mapid=1 usedif=0 width=6144 height=4096
rem tools\uoconvert statics realm=britannia_alt
rem tools\uoconvert maptile realm=britannia_alt

tools\uoconvert map     realm=ilshenar mapid=2 usedif=0 width=2304 height=1600
tools\uoconvert statics realm=ilshenar
tools\uoconvert maptile realm=ilshenar

tools\uoconvert map     realm=malas mapid=3 usedif=0 width=2560 height=2048
tools\uoconvert statics realm=malas
tools\uoconvert maptile realm=malas

tools\uoconvert map     realm=tokuno mapid=4 usedif=0 width=1448 height=1448
tools\uoconvert statics realm=tokuno
tools\uoconvert maptile realm=tokuno

rem tools\uoconvert map     realm=termur mapid=5 usedif=0 width=1280 height=4096
rem tools\uoconvert statics realm=termur
rem tools\uoconvert maptile realm=termur
