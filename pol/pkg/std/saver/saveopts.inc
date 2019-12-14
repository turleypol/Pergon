Const PROP_MINUTES          := "saveinterval";
Const MINUTES_BETWEEN_SAVES := 30;
Const WARN_BEFORE_SECONDS   := 30;

// Giftgruen
Const COLOR_GOOD :=            467;//159;
// Rot
Const COLOR_BAD :=             38;

// globale Property, die waehrend der Sicherung gesetzt ist
Const SAVER_GPROP := "#worldsave";
// globale Property mit letzter Sicherung (Serverzeit, nicht Gamezeit)
Const SAVER_LAST  := "lastworldsave";
// globale Property mit Dauer der letzter Sicherung (auch Serverzeit)
Const SAVER_DUR   := "lastworldsaveduration";

Function WorldsaveRunning()
    If (GetGlobalProperty(SAVER_GPROP))
        return 1;
    Else
        return 0;
    EndIf
EndFunction
