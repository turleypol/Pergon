// verwaltet die Liste der letzten Diebstähle

use os;
use uo;
include "include/logutil";
include "include/varutil";

Const THEFT_LIST_CPROP  := "thefts";
Const EXPIRE_THEFT      := 3600 * 24 * 14; // 14 days
Const EXPIRE_HOUSEBREAK := 3600 * 24 * 31; // 1 month
Const NO_HOUSEBREAK     := 0;
Const HOUSEBREAK        := 1;

Function AddThief(opfer, dieb, item, break_in := NO_HOUSEBREAK)
    var incident := struct;
    // Dieb und Expires sind notwendige Angaben
    incident.+thief := dieb.serial;

    If (break_in == HOUSEBREAK)
        incident.+expires  := ReadGameClock() + EXPIRE_HOUSEBREAK;
        // ggf. vorformatierter logutil-ItemInfoStr (mit Koordinaten)
        If (item)
            incident.+item := item;
        EndIf
    Else
        incident.+expires := ReadGameClock() + EXPIRE_THEFT;
        // bei Taschendiebstahl wird das Item immer notiert
        // (allerdings mehr zur Information fuer GMs,
        // wird nicht ausgewertet)
        incident.+item := ItemInfoStr(item, COORDS_REALM);
    EndIf

    // Diebesliste des Opfers erweitern
    var old_crit := Is_Critical();
    Set_Critical(1);

    var diebe := GetObjProperty(opfer, THEFT_LIST_CPROP);
    If (!diebe)
        // hatten wir noch keine angelegt?
        diebe := array{};
    EndIf
    diebe.append(incident);
    SetObjProperty(opfer, THEFT_LIST_CPROP, diebe);

    Set_Critical(old_crit);

    return;
EndFunction

Function ExpireTheftList(opfer, run_critical := 1)
    // falls man es mal aus start.src laufen lassen will,
    // geht es auch unkritisch
    var old_crit;
    If (run_critical)
        old_crit := Is_Critical();
        Set_Critical(1);
        Set_Script_Option(SCRIPTOPT_NO_RUNAWAY, 1);
    EndIf

    var diebe := GetObjProperty(opfer, THEFT_LIST_CPROP);
    If (!diebe)
        diebe := array{};
    EndIf

    Var i;
    For (i:=1;i<=diebe.size();i+=1)
        If (ReadGameClock() > diebe[i].expires)
            diebe.erase(i);
            i-=1;
        EndIf
        If (!run_critical)
            SleepMS(2);
        EndIf
    EndFor

    SetObjProperty(opfer, THEFT_LIST_CPROP, diebe);

    If (run_critical)
        Set_Critical(old_crit);
        Set_Script_Option(SCRIPTOPT_NO_RUNAWAY, 0);
    EndIf

    return;
EndFunction

Function GetThieves(opfer)
    var thefts := GetObjProperty(opfer, THEFT_LIST_CPROP);
    If (!thefts or thefts.size() == 0)
        return array{};
    EndIf

    var thieves := array{};
    ForEach theft in (thefts)
        thieves.append(theft.thief);
        SleepMS(2);
    EndForEach

    // remove duplicates
    thieves := unique(thieves);

    return thieves;
EndFunction
