use uo;
include "include/server";

// Faktoren fuer Heilung, Heilung mit Bonus und Wartezeit
Const SG_FACT_RES   := "seppelgate_f_res";
Const SG_FACT_PLUS  := "seppelgate_f_plus";
Const SG_FACT_WAIT  := "seppelgate_f_wait";
// darf ein Newbie das Tor nutzen?
Const ALLOW_NEWBIE  := "newbies_allowed";
// CProp fuer Tor-Serial bei Rueckkehr aus Hoelle
Const SG_RETURN_PROP := "sg_return_serial";
// CProp, in der Ende der Wartezeit gespeichert ist
Const SG_WAIT_PROP   := "sg_time_wait";

Function DescentIntoHell(who, gate, how_long) // {{{
    // Gate-Serial fuer Rueckkehr sichern
    SetObjProperty(who, SG_RETURN_PROP, gate.serial);

    // Wartezeit setzen
    SetObjProperty(who, SG_WAIT_PROP, ReadGameClock()+how_long);

    SleepMS(100);
    // in die Hoelle schieben; force, falls am Ziel jemand im Weg steht
    MoveObjectToLocation(
        who, 2113, 1004, -28, REALM_ILSHENAR, MOVEOBJECT_FORCELOCATION
    );
EndFunction // }}}
