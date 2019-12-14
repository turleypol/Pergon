///////////////////////////////////////////////////////////////////////////
// hidecommon
//
// gemeinsame Konstanten und Funktionen fuer hidewall (Item und Textcmd)

include "include/eventid";

// Props an der Wand
Const REALWALL := "wallitem";
Const WALLPID  := "#PID";

// Props am nutzenden GM
Const PREFERRED_WALLS := "#hidewallgroup";

// ObjType (sollte zu Itemdesc passen)
Const UOBJ_HIDEWALL    := 0x5fe0;

// Aktionskonstanten (im Event benutzt)
Const ACTION_WALLUP    := 1;
Const ACTION_WALLDOWN  := 2;
Const ACTION_WALLVIS   := 3;
Const ACTION_WALLINVIS := 4;
Const ACTION_COLOR     := 5;
Const ACTION_GRAPHIC   := 6;

// Touchplate-Daten
Const PROP_TP_CONFIG   := "touchconfig";

// bewegte Wand der Hidewall finden
Function FindRealWall(wall) // {{{
        var realwall := wall.getprop(REALWALL);
        If (realwall)
            realwall := SystemFindObjectBySerial(realwall);
        EndIf
        return realwall;
EndFunction // }}}

// Events an Waende schicken
Function WallSignal(who, action, param := "") // {{{
    var walls := who.getprop(PREFERRED_WALLS);
    If (!walls)
        If (who.isa(POLCLASS_MOBILE))
            SendSysMessagePergon(who,
                "Ihr muesst erst eine Wand(-gruppe) auswaehlen!",
                "You must select a wall (group) first!"
            );
            return;
        EndIf
    EndIf

    var event := struct{
        type   := EVID_MASTER_RECALL,
        action := action
    };
    If (param != "")
        event["param"] := param;
    EndIf

    var count     := 0;
    var countsucc := 0;
    ForEach wall in (walls)
        SleepMs(2);
        count += 1;
        var item := SystemFindObjectBySerial(wall);
        If (!item);
            continue;
        EndIf
        var process := GetProcess(item.getprop(WALLPID));
        If (!process)
            continue;
        EndIf
        If (process.sendevent(event))
            countsucc += 1;
        EndIf
    EndForEach

    If (who.isa(POLCLASS_MOBILE))
        SendSysMessagePergon(who,
            countsucc+" von "+count+" Waenden erfolgreich informiert.",
            countsucc+" of "+count+" walls informed successfully."
        );
    EndIf
EndFunction // }}}
