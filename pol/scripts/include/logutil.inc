///////////////////////////////////////////////////////////////////////////
// Logutil -- nuetzliche Funktionen rund ums Logging
//
// Author: Mehdorn

include "include/itemnpc";
include "include/server";

Const COORDS_NO      := 0;
Const COORDS_NOREALM := 1;
Const COORDS_REALM   := 2;
// gibt logrelevante Informationen ueber Char (Player, NPC) zurueck
Function CharInfoStr(who, coords := 0) // {{{
    // war gar kein Char?
    If (!who.isa(POLCLASS_MOBILE))
        return ItemInfoStr(who, coords);
    EndIf

    var res;
    If (who.isa(POLCLASS_NPC))
        res :=
            "NPC "+who.name+" ("+who.npctemplate+"/"+LHex(who.serial)+")";
    Else
        If (who.cmdlevel < CMDLEVEL_SEER)
            res := "Spieler ";
        Else
            res := "Staffler ";
        EndIf
        res += who.name+" ("+who.acctname+"/"+LHex(who.serial)+")";
    EndIf

    If (coords != COORDS_NO)
        res += " "+CoordStr(who, coords);
    EndIf

    return res;
EndFunction // }}}

// gibt logrelevante Informationen ueber Items (und Uobjects) zurueck
Function ItemInfoStr(item, coords := COORDS_NO) // {{{
    If (!item.isa(POLCLASS_UOBJECT))
        return CommonInfoStr(item);
    EndIf

    If (item.isa(POLCLASS_MOBILE))
        return MultiInfoStr(item, coords);
    EndIf

    var res := "";
    If (item.isa(POLCLASS_ITEM))
        res += item.desc+" ";
    EndIf
    res += "("+LHex(item.objtype)+"/"+LHex(item.serial)+")";
    If (coords != COORDS_NO)
        // die Koordinaten im Gump interessieren weniger
        res += " "+CoordStr(GetTopContainer(item, 1), coords);
    EndIf

    return res;
EndFunction // }}}

// gibt logrelevante Informationen ueber Multis zurueck
Function MultiInfoStr(multi, coords := COORDS_NO) // {{{
    If (!multi.isa(POLCLASS_MULTI))
        return CommonInfoStr(multi);
    EndIf

    var res := "Multi";
    var multiinfo := "("+LHex(multi.objtype)+"/"+LHex(multi.serial)+")";
    If (coords != COORDS_NO)
        multiinfo += " "+CoordStr(multi, coords);
    EndIf
    If (multi.isa(POLCLASS_BOAT))
        res := "Boot "+multiinfo;
        If (multi.getprop("ist_faehre"))
            res := "Faehre "+multiinfo;
            var route := multi.getprop("Routenname");
            If (route)
                res += " auf Route "+route;
            EndIf
        // FIXME: mal mit sinnvollen Bootsbau-Werten fuellen
        // Else
        //     res += " (Privatboot)";
        EndIf
    ElseIf (multi.isa(POLCLASS_HOUSE))
        res := "Haus";
        If (multi.objtype in (array{0x6133, 0x6172}))
            res := "Bauplatzabdeckung";
        EndIf
        res += " "+multiinfo;
        var serial := multi.getprop(PROP_HS_C_OWNER);
        If (serial)
            var who := SystemFindObjectBySerial(
                serial, SYSFIND_SEARCH_OFFLINE_MOBILES
            );
            If (who)
                res += " von "+CharInfoStr(who, COORDS_NO);
            EndIf
        EndIf
    EndIf

    return res;
EndFunction // }}}

// minimale Information(en) fuer Non-Items
Function CommonInfoStr(thing) // {{{
    var type := TypeOf(thing);
    Case (TypeOfInt(thing))
    OT_ARRAY:
    OT_DICTIONARY:
    OT_STRUCT:
        // TODO: Option einbauen, um rekursiv Inhalte aufzulisten
        return type+" ("+thing.size()+")";

    OT_DOUBLE:
    OT_LONG:
    OT_STRING:
        return type+": '"+thing+"'";

    OT_ERROR:
        return type+": '"+thing.errortext+"'";

    OT_ACCOUNTREF:
        return type+" "+thing.name+" (enabled: "+thing.enabled+
            "; banned: "+thing.banned+")";

    OT_GUILDREF:
        return type+" "+LHex(thing.guildid)+" ("+thing.members.size()+
            " members)";

    OT_PACKAGE:
        return type+" "+thing.name;

    OT_PACKET:
        return type+" "+LHex(thing.GetInt8(0))+
            " (size: "+thing.GetSize()+")";

    // TODO: je nach Typ ggf. noch ein paar Informationen ausgeben
    // "ApplicObj" "ApplicPtr" "BoatRef" "BoundingBox" "ConfigElemRef"
    // "ConfigFileRef" "DataElemRef" "DataFileRef" "DebugContext"
    // "MenuRef" "ScriptExRef"

    // haben eigene Funktionen
    OT_ITEMREF:
    OT_MOBILEREF:
    OT_MULTIREF:
    OT_OFFLINEMOBILEREF:
    // hier will man gezielt nicht mehr wissen
    OT_UNINIT:
    OT_UNKNOWN:
    default:
        return "Typ: "+type;
    EndCase
EndFunction // }}}

// gibt Koordinatenstring (wahlweise mit Realm) zurueck
// Achtung: ignoriert COORDS_NO
Function CoordStr(item, coords := COORDS_NOREALM) // {{{
    item := GetTopContainer(item, 1);
    var res := "["+item.x+", "+item.y+", "+item.z;
    If (coords == COORDS_REALM)
        res += "; "+item.realm;
    EndIf
    res += "]";
    return res;
EndFunction // }}}

// Kurzfassung von "Lower(Hex(val))"
Function LHex(val) // {{{
    return Lower(Hex(val));
EndFunction // }}}
