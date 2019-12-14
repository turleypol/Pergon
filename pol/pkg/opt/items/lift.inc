// CProps am Hauptitem
// PID des Lift-Controlscripts
Const PROP_LIFT_PID    := "#PID";
// Serialliste der Komponenten (alles, was mitfaehrt)
Const PROP_LIFT_COMP   := "components";
// Serialliste der Ruf-/Haltesteine auf den Etagen
Const PROP_LIFT_FLOORS := "floors";

// andere CProps
// Verweis auf das Hauptitem (an allen Komponenten)
Const PROP_LIFT_MAIN   := "liftmain";
// letztes Liftitem, auf dem Benutzer stand (an Character)
Const PROP_LIFT_LAST   := "#liftlast";

Const EVTYPE_MODIFY    := "modify";
Const EV_MOD_CALLER    := "call";
Const EV_MOD_COMPONENT := "comp";

Function GetLiftProcess(who, walked)
    var main := SystemFindObjectBySerial(CInt(walked.getprop(PROP_LIFT_MAIN)));
    If (!main)
        syslog(
            "FEHLER: "+ItemInfoStr(walked, COORDS_REALM)+
            " findet Fahrstuhlcontroller nicht"
        );
        SendSysMessagePergon(who,
            "Dieser Fahrstuhl ist derzeit ausser Betrieb.",
            "This elevator is currently out of order."
        );
        return error{errortext := "Lift serial broken"};
    EndIf

    var process := GetProcess(CInt(main.getprop(PROP_LIFT_PID)));
    If (!process)
        syslog(
            "FEHLER: "+ItemInfoStr(walked, COORDS_REALM)+
            " findet Fahrstuhlcontrollerprozess nicht"
        );
        SendSysMessagePergon(who,
            "Dieser Fahrstuhl ist derzeit ausser Betrieb.",
            "This elevator is currently out of order."
        );
        return error{errortext := "Lift process broken"};
    EndIf

    return process;
EndFunction
