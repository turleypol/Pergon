
Const PROP_STATUS    := "proto_status";
Const TIME_NOW       := 0;
Const ENAMEAMBIGUOUS := "Name is ambiguous";
Const ENOTFOUND      := "Name not found";

Function IsAPlayer(who) // {{{
    If ((who.isa(POLCLASS_NPC)) or (!who.isa(POLCLASS_MOBILE)))
        return 0;
    EndIf
    return 1;
EndFunction // }}}

Function ProtocolName() // {{{
    return
        "::log/protokoll-"+
        StrFormatTime("%Y-%m-%d-%H-%M")+
        "--"+
        Lower(CleanText(PlaceName(Self()), array{"'", " "}))+
        ".log";
EndFunction // }}}

Function LogEnterLeave(byref me, who, type, byref status) // {{{
    var dir := "gekommen";
    If (type == SYSEVENT_LEFTAREA)
        dir := "gegangen";
    EndIf

    LogToFile(status.logname, TimeNow()+GetRealName(who)+" ist "+dir);

    If (!IsAPlayer(who))
        // bei NPCs sind wir nicht so gruendlich
        return;
    EndIf

    var players := array{};
    ForEach mob in (ListMobilesNearLocation(
            me.x, me.y, LIST_IGNORE_Z, status.range, me.realm
    ))
        SleepMs(2);
        If (IsAPlayer(mob))
            players.append(mob.name);
        EndIf
    EndForEach

    LogToFile(status.logname, TimeNow()+"Anwesende: "+join(", ", players));
EndFunction // }}}

Function AutoCloseProtocol(byref status) // {{{
    var me := Self();

    var found := 0;
    ForEach mob in (ListMobilesNearLocation(
            me.x, me.y, LIST_IGNORE_Z, status.range, me.realm
    ))
        SleepMs(2);
        If (IsAPlayer(mob))
            found += 1;
        EndIf
    EndForEach

    // keiner mehr da und Server laeuft schon eine Weile
    // (Protokoll soll bei Serverneustarts waehrend einer
    // Verhandlung nicht abgebrochen werden, aber kurz nach
    // Serverstart ist vermutlich noch niemand eingeloggt)
    If ((found <= 0) and (polcore().uptime > 3600))
        status.proto := 0;
        status.pause := 0;
        SetObjProperty(me, PROP_STATUS, status);
    EndIf
EndFunction // }}}

Function CanCommand(byref status, who) // {{{
    // auf Wunsch von Murphy von Seer heruntergesetzt
    If (who.cmdlevel >= CMDLEVEL_COUNSELOR)
        return 2;
    EndIf

    If (who.serial in (status.masters))
        return 1;
    EndIf

    return 0;
EndFunction // }}}

Function MasterAdd(byref status, add) // {{{
    
    // hinzufuegen, falls nicht schon in Liste
    If (!(add.serial in (status.masters)))
      status.masters.append(add.serial);
    EndIf

    return add.name;
EndFunction // }}}

Function MasterDel(byref status, name) // {{{
    var masters := dictionary{};
    // alle Meister durchgehen
    ForEach serial in (status.masters)
        SleepMs(2);
        var handle := SystemFindObjectBySerial(
            serial, SYSFIND_SEARCH_OFFLINE_MOBILES
        );
        If (
            handle and (
                // alle mit passenden Namen finden
                Lower(handle.name)[name] or
                Lower(GetRealName(handle.name))[name]
            )
        )
            // samt Index merken
            masters.insert(_serial_iter, handle.name);
        EndIf
    EndForEach

    // Fehler abfangen
    If (masters.size() > 1)
        var ret := error;
        ret.errortext := ENAMEAMBIGUOUS;
        return ret;
    EndIf
    If (masters.size() <= 0)
        var ret := error;
        ret.errortext := ENOTFOUND;
        return ret;
    EndIf

    // ggf. Index loeschen
    var key := masters.keys()[1];
    status.masters.erase(key);
    return masters[key];
EndFunction // }}}

Function MasterList(byref status) // {{{
    var masters := array{};
    // alle Meister heraussuchen
    ForEach serial in (status.masters)
        SleepMs(2);
        var handle := SystemFindObjectBySerial(
            serial, SYSFIND_SEARCH_OFFLINE_MOBILES
        );
        If (handle)
            masters.append(handle.name);
        EndIf
    EndForEach

    // mit Komma getrennt zurueckgeben
    return join(", ", masters);
EndFunction // }}}

Function TimeNow(suffix := " ") // {{{
    return StrFormatTime("%H:%M:%S")+suffix;
EndFunction // }}}