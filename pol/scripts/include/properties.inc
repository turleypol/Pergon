CONST PROP_MODSPEED := "#speed";

CONST PROP_POISON := "poison";
CONST PROP_FIRE   := "fire";
CONST PROP_EARTH  := "earth";
CONST PROP_WATER  := "water";
CONST PROP_AIR    := "air";

CONST PROP_PEACEMAKING    := "peacemaking";

Function GetObjPropOrDefault(obj, prop, def) // {{{
    // liefert Property, falls gefunden, sonst default
    var res := GetObjProperty(obj, prop);
    If (res.errortext)
        return def;
    EndIf

    return res;
EndFunction // }}}

Function GetGlobalPropOrDefault(prop, def) // {{{
    // liefert Property, falls gefunden, sonst default
    var res := GetGlobalProperty(prop);
    If (res.errortext)
        return def;
    EndIf

    return res;
EndFunction // }}}

// passt nicht exakt, aber halbwegs
Function IsValidPrivilege(priv) // {{{
    // Kleine Whitelist, damit sich die Leute nicht zumüllen mit
    // nichtexistenten Privs
    If (priv in (array{
                "all",
                "attackhidden",
                "canbeheardasghost",
                "clotheany",
                "dblclickany",
                "firewhilemoving",
                "freemove",
                "hearghosts",
                "hiddenattack",
                "ignoredoors",
                "invul",
                "losany",
                "moveany",
                "moveanydist",
                "plogany",
                "renameany",
                "runwhilestealth",
                "seeghosts",
                "seehidden",
                "seeinvisitems",
                "speedhack"
    }))
        return 1;
    EndIf

    return error{errortext := "Unbekanntes Privileg "+priv};
EndFunction // }}}
