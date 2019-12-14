use os;

Const PROP_DISCONNECT_PID := "#disconnect";

Function TimedDisconnect(who, time := 300)
    var param := struct{who := who, time := time};
    Start_ScriptPergon(":timer:autodisconnect", param);
EndFunction

Function CancelDisconnect(who)
    var pid := who.getprop(PROP_DISCONNECT_PID);
    If (!pid)
        return;
    EndIf

    var script := GetProcess(pid);
    If (script)
        script.kill();
    EndIf
EndFunction
