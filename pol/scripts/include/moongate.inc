// moongate.inc

Const UOBJECT_PERGON_MOONGATE    := 0x685b;
Const UOBJECT_PERGON_MOONGLITTER := 0x376a;

Function MoonPosition(place := "")
    var pos := dictionary{
        "cove"              -> array{2404, 1046,   5, REALM_BRITANNIA},
        "jhe'lom"           -> array{1499, 3771,   5, REALM_BRITANNIA},
        "labyrinth-dungeon" -> array{5334,  411,   2, REALM_BRITANNIA},
        "minoc"             -> array{2701,  692,   5, REALM_BRITANNIA},
        "nirdana"           -> array{3503,  306,   5, REALM_BRITANNIA},
        "nujelm"            -> array{3670, 1055,   0, REALM_BRITANNIA},
        "vesper"            -> array{3111,  585,  10, REALM_BRITANNIA},
        "yew"               -> array{ 771,  752,   5, REALM_BRITANNIA}
        // "britain"           -> array{1336, 1997,   5, REALM_BRITANNIA},
        // "moonglow"          -> array{4467, 1283,   5, REALM_BRITANNIA},
        // "trinsic"           -> array{1828, 2948, -20, REALM_BRITANNIA},
        // "skara brae"        -> array{ 643, 2067,   5, REALM_BRITANNIA},
        // "magincia"          -> array{3564, 2140,  34, REALM_BRITANNIA},
    };
    If (place)
        // speziell fuer Moongate-Gump
        return pos[place];
    EndIf

    // oder alles fuer das Aufbau-Script
    return pos;
EndFunction

Function MoonSelection()
    return dictionary{
        "vesper" -> struct{
            id:=1,
            button := "button 403  30 "+MoonPics("vesper")+" 1 0",
            tip := "tooltip 1011030"
        },
        "minoc" -> struct{
            id := 2,
            button := "button 428  91 "+MoonPics("minoc")+" 1 0",
            tip := "tooltip 1011031"
        },
        "cove" -> struct{
            id := 3,
            button := "button 253  94 "+MoonPics("cove")+" 1 0",
            tip := "tooltip 1011033"},
        "yew" -> struct{
            id := 4,
            button := "button 276  29 "+MoonPics("yew")+" 1 0",
            tip := "tooltip 1011032"
        },
        "jhe'lom" -> struct{
            id := 5,
            button := "button 339   5 "+MoonPics("jhe'lom")+" 1 0",
            tip := "tooltip 1011343"
        },
        "nirdana" -> struct{
            id := 6,
            button := "button 339 180 "+MoonPics("nirdana")+" 1 0",
            tip := "tooltip 1011028"
        },
        "nujelm" -> struct{
            id := 7,
            button := "button 403 158 "+MoonPics("nujelm")+" 1 0",
            tip := "tooltip 1011034"
        },
        "labyrinth-dungeon" -> struct{
            id := 8,
            button := "button  94   5 "+MoonPics("labyrinth-dungeon")+" 1 0",
            tip := "tooltip 1075159"
        }
    };
EndFunction

Function MoonPics(place := "")
    var pics := dictionary{
        "labyrinth-dungeon" -> 112,
        "minoc"             -> 123,
        "vesper"            -> 124,
        "cove"              -> 125,
        "yew"               -> 126,
        "jhe'lom"           -> 127,
        "nirdana"           -> 128,
        "nujelm"            -> 129
    };
    If (place)
        // speziell fuer Moongate-Gump
        return pics[place]+" "+pics[place];
    EndIf

    // oder alles fuer den Haendler-Gump
    return pics;
EndFunction

Function CreateMoonGates()
    var gates := MoonPosition();
    ForEach gate in (gates.keys())
        SleepMS(2);
        var pos := gates[gate];
        var desc := GetItemDescriptor(UOBJECT_PERGON_MOONGATE);
        desc.decayat :=  0;
        desc.facing  := 29;
        desc.movable :=  0;
        var item := CreateItemAtLocationPergon(
            pos[1], pos[2], pos[3], desc, 1, pos[4]
        );
        If (!item)
            syslog(
                "FEHLER: Moongate "+gate+" konnte nicht erzeugt werden:"+
                item.errortext+"!"
            );
            continue;
        EndIf
        // und noch Geglitzer
        desc.graphic := UOBJECT_PERGON_MOONGLITTER;
        CreateItemAtLocationPergon(
            pos[1], pos[2], pos[3], desc, 1, pos[4]
        );
    EndForEach
EndFunction

// Function DestroyMoonGates(gates)
// just use DestroyTeleporters with type UOBJECT_PERGON_MOONGATE
