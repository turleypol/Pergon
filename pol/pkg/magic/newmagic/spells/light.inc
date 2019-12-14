/////////////////////////////////////////////////////////////////////
// Magisches Licht erschaffen

Const PROP_WISP := "#wisp";

// Magisches Licht erzeugen (und nach duration entfernen)
Function CreateMagicLight(caster, master, duration) // {{{
  If (GetObjProperty(master, PROP_WISP))
    If (caster == master)
      SendSysMessagePergon(caster,
        "Ihr habt doch schon ein magisches Licht!"
      );
    Else
      SendSysMessagePergon(caster,
        master.name+" hat doch schon ein magisches Licht!"
      );
    EndIf
    return;
  EndIf

  var parms := struct;
  parms["script"] := "enticedanimal";
  parms["master"] := master.serial;
  parms["CProps"] := dictionary;
  parms["CProps"]["autofollowmaster"] := 1;
  parms["CProps"]["master"]           := master.serial;
  parms["CProps"][PROP_SUMMONED]      := SUMMON_BY_UNDEF;

  var wisp := CreateNpcFromTemplate(
    "magilite", master.x, master.y, master.z, parms, master.realm
  );
  If (!wisp)
    SendSysMessagePergon(caster,
      "Ihr koennt kein Magisches Licht herbeirufen, da etwas defekt ist! "+
      "Wendet euch an einen GM."
    );
    SysLog("FEHLER: Magilite nicht erzeugbar: "+wisp.errortext);
    return;
  EndIf

  // nur 1 Wisp pro Person erlauben, damit nicht jemand Wisps als
  // Schild nutzt
  SetObjProperty(master, PROP_WISP, 1);

  SetScriptController(0);
  Detach();
  var dummy := 0;
  While (dummy < duration)
    dummy += 5;
    Sleep(5);
    If (!wisp or wisp.dead)
      // falls das Licht vorher ausgeloescht wurde
      EraseObjProperty(master, PROP_WISP);
      return;
    EndIf
  EndWhile

  If (master.connected)
    SendSysMessagePergon(master, "Das magische Licht geht seiner Wege.");
  EndIf
  EraseObjProperty(master, PROP_WISP);

  KillNPC(wisp, "mage-light");
EndFunction // }}}

// Das Magische Licht benutzen?
Function UseMagilite(caster, caston) // {{{
  If (
    GetGlobalProperty("WispLight") or
    caster.getprop(PROP_MAGILITE) or
    caston.getprop(PROP_MAGILITE)
  )
    return 1;
  EndIf

  return 0;
EndFunction // }}}

// vim: sw=2 sts=2
