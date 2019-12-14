///////////////////////////////////////////////////////////////////////////
// PergonUtil include - Viele kleiner Helferlein
//
// Author: Shinigami

///////////////////////////////////////////////////////////////////////////
// Funktionen
// * IsNumeric                      - Ist der String nummerisch?
// * IsNumericDbl                   - Ist der String nummerisch?
//
// * Start_ScriptPergon             - Ein Script starten und sofort zurueckkehren
// * Run_Script_to_CompletionPergon - Ein Script starten und auf Beendigung warten
//
// * ReservePlayerPergon            - Wie ReserveItem, jedoch ohne Item
// * ReleasePlayerPergon            - Wie ReleaseItem, jedoch ohne Item
//
// * AutoJail                       - Inhaftiert den angegebenen Mobile
// * AutoForgive                    - Entlaesst inhaftierten Mobile
// * SendBuyWindowPergon            - Das Kaufmenue aufrufen...
// * SendSellWindowPergon           - Das Verkaufsmenue aufrufen...
//
// * DeleteCharacterPergon          - Character loeschen
//
// Newbie-Funktionen
// * CheckItemMaterial              - Darf das Item materialtechnisch benutzt werden?
// * IsUsableByNewbie               - Darf das Item aus dem Material von Newbies benutzt werden?
// * CheckEquipmentForNewbieArea    - Waffen und [schwere] Ruestungen des ExNewbies ablegen
// * DeNewbie                       - Newbie-Status wegnehmen
//
// Gump-Funktionen
// * SendYesNoDialog                - Sendet eine Frage und erwartet Ja/Nein als Antwort
// * SendYesNoGump                  - Ja-Nein-Frage in Anlehnung an SendTextEntryGump
//
// * FillInAccountBuchstaben        - Anfangsbuchstaben der Accounts ins Gump eintragen
// * ChooseAccountBuchstabe         - Einen Anfangsbuchstaben auswaehlen
// * FillInAccountNames             - Accounts aller Spieler ins Gump eintragen
// * ChooseAccountName              - Einen Account auswaehlen
// * FillInCharacter                - Character eines Accounts ins Gump eintragen
// * ChooseCharacter                - Einen Character auswaehlen

include ":death:seppel";
include "include/itemnpc";
include "include/mobile";
include "include/names";
include "include/npc";
include "include/server";
include "include/string";
include "include/vetement";
include ":sqlmanager:sqlmanager";

///////////////
// Konstanten
///////////////

Const HOEHE_ACCOUNTS  := 25;  // Wieviel Zeilen passen in den Accounts-Gump?
Const HOEHE_CHARACTER := 7;   // Wieviel Zeilen passen in den Character-Gump?

Const BUTTON_ZURUECK   := 0;   // Zurueck zur Account-Auswahl / Abbruch
Const BUTTON_ACCOUNT   := 10;  // Accountauswahl
Const BUTTON_CHARACTER := 10;  // Characterauswahl

// FillInAccountNames/Buchstaben
Const SHOW_ALL       := 0; // Alle Accounts anzeigen
Const SHOW_BANNED    := 2; // nur gebannte
Const SHOW_NOTBANNED := 1; // nur nichtgebannte
Const SHOW_ENABLED   := 3; // enabled accounts
Const SHOW_DISABLED  := 4; // disabled accounts

////////////////////
//                //
//   Funktionen   //
//                //
////////////////////

///////////////////////////////////////////
// IsNumeric - Ist der String numerisch?
///////////////////////////////////////////

Function IsNumeric(value)
  value := Lower(CStr(value));
  If (value == Lower(CStr(CInt(value))))
    return 1;
  EndIf
  If (value == Lower(CStr(Hex(CInt(value)))))
    return 1;
  EndIf
  return 0;
EndFunction

//////////////////////////////////////////////
// IsNumericDbl - Ist der String numerisch?
//////////////////////////////////////////////

Function IsNumericDbl(value)
  return (CStr(value) == CStr(CDbl(value)));
EndFunction

/////////////////////////////////////////////////////////////////////
// Start_ScriptPergon - Ein Script starten und sofort zurueckkehren
/////////////////////////////////////////////////////////////////////

Function Start_ScriptPergon(script_name, param := 0)
  var scriptobj := Start_Script(script_name, param);
  If (scriptobj.errortext)
    syslog("FEHLER: Start_Script konnte '"+script_name+"' nicht starten ("+scriptobj.errortext+") !");
  EndIf

  return (scriptobj);
EndFunction

//////////////////////////////////////////////////////////////////////////////////
// Run_Script_to_CompletionPergon - Ein Script starten und auf Beendigung warten
//////////////////////////////////////////////////////////////////////////////////

Function Run_Script_to_CompletionPergon(script_name, param := 0)
  var result := Run_Script_to_Completion(script_name, param);
  If (result.errortext)
    syslog("FEHLER: Run_Script_to_Completion konnte '"+script_name+
      "' wahrscheinlich nicht starten ("+result.errortext+") !");
  EndIf

  return (result);
EndFunction

////////////////////////////////////////////////////////////
// ReservePlayerPergon - Wie ReserveItem, jedoch ohne Item
////////////////////////////////////////////////////////////

Function ReservePlayerPergon()
  var axt := CreateItemAtLocationPergon(5889, 513, RandomInt(50), 0xf43, 1);
  If (axt)
    ReserveItem(axt);
    axt.name := "ReservePlayer-Axt";

    return (axt);
  Else
    syslog("WARNUNG: ReserveItemPlayer konnte ReserveItem nicht erzeugen!");
  EndIf
EndFunction

////////////////////////////////////////////////////////////
// ReleasePlayerPergon - Wie ReleaseItem, jedoch ohne Item
////////////////////////////////////////////////////////////

Function ReleasePlayerPergon(resitem, delayms := 0)
  If (delayms>0)
    SleepMS(delayms);
  EndIf

  ReleaseItem(resitem);
  DestroyItem(resitem);
EndFunction

///////////////////////////////////////////////////////////////////////////
// AutoJail -
// Setzt angegebenen Mobile in den Knast in zufaellige Zelle
// Parameter "jailtime": Zeit, die im Knast zu verbringen ist
// - mehrfaches AutoJail() erhoeht die Knastzeit um die jeweilige Dauer
// - JailArray ergaenzen und CProp 'jailtime' setzen genuegt
//   (Rest erledigt AutoJailControl-Script)
Const PILLORY_RANGE := 3000;
Function AutoJail(who, jailtime := 0, flags := 0) // {{{
  If (!who.isa(POLCLASS_MOBILE) or who.isa(POLCLASS_NPC))
    return;
  EndIf

  Case (flags.type)
  "Pranger":
    who.setprop(PROP_PUNISH_TYPE, flags.type);
    PutOnPillory(who);

  "Tod":
    who.setprop(PROP_PUNISH_TYPE, flags.type);

    // zur Guillotine laufen
    WalkToGuillotine(who, flags.item);

    // per Guillotine toeten
    Run_Script(
      ":guillotine:headdown", struct{who := who, item := flags.item}
    );
    Sleep(5);

    // TODO: Backpackinhalt irgendwie sichern (Bank? Storage?)

    // in die Hoelle setzen (item (Guillotine) ist Rueckkehrpunkt)
    // man darf nach einer Sekunde wieder heraus, aber das Seppelgate
    // wartet, bis die Straf-CProp vom Jail-Control-Script entfernt wurde
    DescentIntoHell(who, flags.item, 1);

  "Knast":
  default:
    // Eingeknasteten zufaellige Knastzelle verschieben {{{
    var zellen := array{
      struct{x := 5276, y := 1164},
      struct{x := 5286, y := 1164},
      struct{x := 5296, y := 1164},
      struct{x := 5306, y := 1164},
      struct{x := 5309, y := 1186},
      struct{x := 5304, y := 1186}
    };
    var zelle := zellen.randomentry();
    var res := MoveObjectToLocation(
      who, zelle.x, zelle.y, 0, REALM_BRITANNIA, MOVEOBJECT_FORCELOCATION
    );
    If (!res)
      syslog(
        "FEHLER: "+CharInfoStr(who)+
        " konnte nicht in den Knast geworfen werden: "+res.errortext
      );
    EndIf
    // }}}
  EndCase

  // Minimum-Wartezeit setzen {{{
  // Wird beim Einloggen des Chars wieder abgefragt
  // Wenn bereits eine Knastzeit gesetzt war, wird die neue addiert
  jailtime := CInt(jailtime);
  var oldjailtime := CInt(who.getprop(PROP_PUNISH_JAILTIME)/60.0);
  If (oldjailtime > 0)
    jailtime += oldjailtime;
  EndIf
  If (jailtime > 0)
    who.setprop(PROP_PUNISH_JAILTIME, 60*jailtime);
  Else
    syslog(
      "WARNUNG: Ungueltige Jailtime ("+jailtime+") bei "+CharInfoStr(who)
    );
    return;
  EndIf
  // }}}

  // Spieler dem AutoJail-Script bekanntmachen {{{
  // Critical, da mehrere Scripte an der GProp drehen koennen
  var crit := Is_Critical();
  Set_Critical(1);
  var jailarray := GetGlobalProperty("jail");
  If (!jailarray)
    jailarray := array{};
  EndIf

  // Charserial zum Jail-Array hinzufuegen
  If (!(who.serial in jailarray))
    jailarray.append(who.serial);
    SetGlobalProperty("jail", jailarray);
  EndIf
  Set_Critical(crit);
  // }}}
EndFunction // }}}

// Leute an den Pranger tackern
Function PutOnPillory(who) // {{{
  // Character vorbereiten (soll nicht wegrennen koennen)
  who.disable("freemove");
  RevokePrivilege(who, "freemove");
  GrantPrivilege(who, "invul");
  who.enable("invul");
  If (!who.frozen)
    who.frozen := 1;
  EndIf
  IncRevision(who);

  // Pranger suchen {{{
  var allpillories :=
    ListItemsNearLocationOfType(
      who.x, who.y, LIST_IGNORE_Z, PILLORY_RANGE, 0x1286, who.realm
    )+
    ListItemsNearLocationOfType(
      who.x, who.y, LIST_IGNORE_Z, PILLORY_RANGE, 0x1292, who.realm
    );
  If (allpillories.size() <= 0)
    PrintTextAboveLocalizedPergon(
      who, "*keinen Pranger seh*", "*cannot spot pillory*"
    );
    return;
  EndIf

  var nearest;
  If (allpillories.size() == 1)
    // Trivialfall: es gibt nur einen Pranger in der Gegend
    nearest := allpillories[1];
  Else
    // dichtesten Pranger auswaehlen
    var dist := 100000;
    ForEach pillory in (allpillories)
      SleepMS(2);
      var newdist := Distance(who, pillory);
      If (newdist < dist)
        nearest := pillory;
        dist := newdist;
      EndIf
    EndForEach
  EndIf
  // }}}

  // Spieler verschieben, falls gefunden {{{
  // alte Position merken (fuer Freilassen)
  var pos := struct{
    x     := who.x,
    y     := who.y,
    z     := who.z,
    realm := who.realm
  };
  who.setprop(PROP_PUNISH_OLDPOS, pos);

  // passend aufstellen und drehen
  var res := MoveObjectToLocation(
    who, 1+nearest.x, 1+nearest.y, nearest.z, nearest.realm,
    // auf dem Pranger kann man nicht normal laufen
    MOVEOBJECT_FORCELOCATION
  );
  If (nearest.objtype == 0x1286)
    who.setfacing(2, FACING_FORCE);
  ElseIf (nearest.objtype == 0x1292)
    who.setfacing(4, FACING_FORCE);
  EndIf

  If (!res)
    PrintTextAboveLocalizedPergon(who,
      "Anprangern gescheitert: "+res.errortext,
      "Putting on pillory failed: "+res.errortext
    );
  EndIf
  // }}}
EndFunction // }}}

// zur Guillotine laufen
Const FP_IGNORE_NOTHING := 0;
Function WalkToGuillotine(who, guill) // {{{
  var frozen_before := who.frozen;
  who.frozen := 1;

  // wohin muss man laufen, um vor Guillotine zu stehen
  var walkto := struct;
  walkto.x     := guill.x;
  walkto.y     := guill.y;
  walkto.z     := guill.z;
  walkto.realm := guill.realm;
  Case (guill.objtype)
  0x1230: 0xff12:
    walkto.y += 1;
  0x125e: 0xff11:
    walkto.x += 1;
  EndCase

  ForEach pos in (FindPath(
      who.x, who.y, who.z, walkto.x, walkto.y, walkto.z, who.realm,
      FP_IGNORE_NOTHING, 50
  ))
    // passend drehen
    who.setfacing(GetFacing(who.x, who.y, pos.x, pos.y), FACING_FORCE);
    Sleep(1);
    // einen Schritt gehen
    MoveObjectToLocation(
      who, pos.x, pos.y, pos.z, who.realm, MOVEOBJECT_FORCELOCATION
    );
  EndForEach

  who.frozen := frozen_before;
EndFunction // }}}

///////////////////////////////////////////////////////////////////////////
// AutoForgive - Entlaesst den Gefangenen
///////////////////////////////////////////////////////////////////////////
Function AutoForgive(who, forgiver := "") // {{{
  If (forgiver)
    forgiver := " (durch "+CharInfoStr(forgiver)+")";
  EndIf

  var type := who.getprop(PROP_PUNISH_TYPE);
  var release := 0;
  If (type == "Steinbruch")
    release := 1;
  EndIf
  who.eraseprop(PROP_PUNISH_TYPE);

  // globales Autojail-Array aufraeumen {{{
  var crit := Is_Critical();
  Set_Critical(1);

  var jailarray := GetGlobalProperty("jail");
  If (!jailarray)
    jailarray := array{};
  EndIf

  ForEach entry in (jailarray)
    If ((CInt(entry) != who.serial) and (entry != who.acctname))
      continue;
    EndIf
    jailarray.erase(_entry_iter);
    SetGlobalProperty("jail", jailarray);
    release := 1;
  EndForEach
  Set_Critical(crit);
  // }}}

  If (release == 0)
    // war gar nicht (mehr) bestraft, Abbruch
    return 0;
  EndIf

  Case (type)
  "Pranger": // {{{
    // Beweglich- und Verwundbarkeit wiederherstellen
    If (who.frozen)
      who.frozen := 0;
    EndIf
    who.disable("invul");
    RevokePrivilege(who, "invul");
    IncRevision(who);

    who.eraseprop(PROP_PUNISH_JAILTIME);

    var pos := who.getprop(PROP_PUNISH_OLDPOS);
    var res := error{
      errortext := "No coordinates before pillory punishment found"
    };
    If (pos)
      res := MoveObjectToLocation(
        who, pos.x, pos.y, pos.z, pos.realm, MOVEOBJECT_FORCELOCATION
      );
    EndIf
    who.eraseprop(PROP_PUNISH_OLDPOS);

    SendSysMessagePergon(who, "Euch wurde vergeben.", "You were forgiven.");
    Sleep(3);

    If (res)
      syslog(
        "STRAFE-ENDE: "+CharInfoStr(who)+" wurde vom Pranger entfernt"+
        forgiver
      );
    Else
      syslog(
        "FEHLER: "+CharInfoStr(who, COORDS_REALM)+
        " konnte nicht vom Pranger entfernt werden: "+res.errortext
      );
      SendSysMessagePergon(who,
        "Ein Fehler ist aufgetreten, der Staff wurde informiert.",
        "An error occurred, the staff has been informed."
      );
    EndIf

    return 1;
    // }}}

  "Tod": // {{{
    // Spieler ueber Strafende informieren
    SendSysMessagePergon(who, "Euch wurde vergeben.", "You were forgiven.");
    Sleep(3);
    SendSysMessagePergon(who,
      "Ihr duerft die Hoelle nun verlassen.", "You may leave hell now."
    );
    return 1;
    // }}}

  "Knast":
    break;

  "Steinbruch": // {{{
    who.eraseprop(PROP_PUNISH_STONES);

    var pos := who.getprop(PROP_PUNISH_OLDPOS);
    var res := error{
      errortext := "No coordinates before stone pit punishment found"
    };
    If (pos)
      res := MoveObjectToLocation(
        who, pos.x, pos.y, pos.z, pos.realm, MOVEOBJECT_FORCELOCATION
      );
    EndIf
    who.eraseprop(PROP_PUNISH_OLDPOS);

    SendSysMessagePergon(who, "Euch wurde vergeben.", "You were forgiven.");
    Sleep(3);

    If (res)
      syslog(
        "STRAFE-ENDE: "+CharInfoStr(who)+" wurde aus dem Steinbruch entlassen"+
        forgiver
      );
    Else
      syslog(
        "FEHLER: "+CharInfoStr(who, COORDS_REALM)+
        " konnte nicht aus dem Steinbruch entlassen werden: "+res.errortext
      );
      SendSysMessagePergon(who,
        "Ein Fehler ist aufgetreten, der Staff wurde informiert.",
        "An error occurred, the staff has been informed."
      );
    EndIf

    return 1;
    // }}}

  default:
    If (!type)
      // kein Typ gesetzt: ist eine normale Knaststrafe
      break;
    EndIf

    syslog("FEHLER: "+CharInfoStr(who)+" hat ungueltigen Straftyp: "+type);
  EndCase

  // normale Knastentlassung (auch als Fallback fuer undefinierte Typen) {{{
  // NPC vorbereiten (steht in Ecke GreenAcres oder wird erzeugt)
  var npc := ListMobilesNearLocation(
    6143, 1279, LIST_IGNORE_Z, 2, REALM_BRITANNIA
  );
  If (npc and npc.size() >= 1)
    npc := npc[1];
  Else
    npc := 0;
  EndIf

  If (!npc or !npc.isa(POLCLASS_NPC))
    // keiner gefunden, neu erzeugen
    // er soll nichts machen, daher ohne Script
    npc := CreateNpcFromTemplate(
      "townguardgodm", 6143, 1279, 0, struct{script := ""}, REALM_BRITANNIA
    );
    // manuell einkleiden und benennen
    Habille(npc, npc.getprop("Equipt"));
    npc.name := RandomName(npc);
  EndIf

  EraseObjProperty(who, PROP_PUNISH_JAILTIME);
  Sleep(1);

  // Player herauslassen
  var oldfrozen := who.frozen;
  who.frozen := 1;
  IncRevision(who);
  MoveObjectToLocation(
    who, 5286, 1174, 0, REALM_BRITANNIA, MOVEOBJECT_FORCELOCATION
  );
  If (who.facing <> 6)
    who.setfacing(6, FACING_FORCE);
  EndIf
  syslog(
    "STRAFE-ENDE: "+CharInfoStr(who)+" wurde aus dem Gefaengnis entlassen"+
    forgiver
  );

  If (!npc)
    // Fallback, falls was schiefgeht
    SendSysMessagePergon(who, "Euch wurde vergeben.", "You were forgiven.");
    Sleep(3);
    syslog("FEHLER: "+npc.errortext);
    who.frozen := oldfrozen;
    IncRevision(who);
  Else
    If (!npc.frozen)
      // sicher ist sicher
      npc.frozen := 1;
    EndIf
    MoveObjectToLocation(
      npc, 5284, 1174, 0, REALM_BRITANNIA, MOVEOBJECT_FORCELOCATION
    );
    If (npc.facing <> 2)
      npc.setfacing(2, FACING_FORCE);
    EndIf
    PrintTextAbovePergon(who, npc, "*hust*", "*cough");
    Sleep(10);
    PrintTextAbovePergon(who, npc,
      "Ihr habt Eure Strafe abgesessen und hoffentlich daraus gelernt.",
      "You have endured your prison sentence and hopefully learned something."
    );
    Sleep(10);
    PrintTextAbovePergon(who, npc,
      "Ich hoffe, dass wir uns nicht so bald wiedersehen.",
      "I look forward to see you not too soon again."
    );
    Sleep(10);
    PrintTextAbovePergon(who, npc,
      "Gehabt Euch wohl!",
      "Farewell!"
    );
    PerformAction(npc, ANIM_SALUTE);
    Sleep(1);
    who.frozen := oldfrozen;
    IncRevision(who);
    Sleep(10);
    // wieder nach GreenAcres schieben
    MoveObjectToLocation(
      npc, 6143, 1279, 0, REALM_BRITANNIA, MOVEOBJECT_FORCELOCATION
    );
  EndIf
  // }}}

  return 1;
EndFunction // }}}

////////////////////////////////////////////////////
// SendBuyWindowPergon - Das Kaufmenue aufrufen...
////////////////////////////////////////////////////

Function SendBuyWindowPergon(character, container, vendor, items)
  var flags := VENDOR_SEND_AOS_TOOLTIP;
  If (GetObjProperty(character, NO_AOS_TOOLTIP))
    flags := 0; // Falls der Spieler Probleme mit seinem alten Client hat
  EndIf

  return (SendBuyWindow(character, container, vendor, items, flags));
EndFunction

/////////////////////////////////////////////////////////
// SendSellWindowPergon - Das Verkaufsmenue aufrufen...
/////////////////////////////////////////////////////////

Function SendSellWindowPergon(character, vendor, i1, i2, i3)
  var flags := VENDOR_SEND_AOS_TOOLTIP;
  If (GetObjProperty(character, NO_AOS_TOOLTIP))
    flags := 0; // Falls der Spieler Probleme mit seinem alten Client hat
  EndIf

  return (SendSellWindow(character, vendor, i1, i2, i3, flags));
EndFunction

///////////////////////////////////////////////
// DeleteCharacterPergon - Character loeschen
///////////////////////////////////////////////

Function DeleteCharacterPergon(account, charnr)
  var char := account.getcharacter(charnr);
  If (char)
    Run_Script_to_CompletionPergon("::misc/ondelete", char);
    return (account.deletecharacter(charnr));
  Else
    return (char); // Fehlermeldung durchreichen
  EndIf
EndFunction

Function TimedShutdown(params)
  // Erstmal blocken...
  SetGlobalProperty(PROP_SHUTDOWN_RUNNING, 1);

  var timemul;
  If (params[" "])
    var paramssplit := SplitWords(params);
    ShutDownMessage(params, paramssplit);
    timemul := CInt(paramssplit[1]);
  Else
    ShutDownMessage("", array{});
    timemul := CInt(params);
  EndIf
  
  var countdown_minutes := timemul; // Zeit fuer den Countdown ermitteln
  If (!timemul)
    countdown_minutes := 5;
  EndIf

  // Wann wird der ShutDown voraussichtlich sein?
  var countdown_seconds := countdown_minutes*60; // Gesamtzeit
  var stepdown          := 10;                   // Schrittweite
  SetGlobalProperty(PROP_SHUTDOWN_RUNNING, ReadGameClock()+countdown_seconds);
  syslog("Shutdown in "+countdown_minutes+" Minuten");
  SendSQLManager(0,SQLStatus_SetShutdownMsg);
  
  // countdown-maessig Hinweismeldungen ausgeben
  While (countdown_seconds >= 0)
    var online := EnumerateOnlineCharacters();

    If (online.size() <= 0)
      // alle weg --> sofort abschalten
      break;
    EndIf

    If (0 == countdown_seconds % 60)
      var timeleft := countdown_seconds/60;
      ForEach player in (online)
        SleepMS(2);
        SendSysMessagePergon(player,
          "Noch "+timeleft+" Minuten bis zum Shutdown!",
          "Shutdown in "+timeleft+" minutes!",
          _DEFAULT_TEXT_FONT, 38
        );

        PlaySoundEffectPrivate(player, SFX_THUNDER, player);
        // Damit die CMDLevel nicht von den Spielern gesehen werden ...
        If (player.cmdlevel <= CMDLEVEL_COUNSELOR)
          PlayLightningBoltEffect(player);
        EndIf
      EndForEach
    EndIf

    countdown_seconds -= stepdown;
    Sleep(stepdown);

    // Laeuft der CountDown noch?
    If (!GetGlobalProperty(PROP_SHUTDOWN_RUNNING))
      break;
    EndIf
  EndWhile

  // Laeuft der CountDown noch?
  If (GetGlobalProperty(PROP_SHUTDOWN_RUNNING))
    SendSQLManager(0,SQLStatus_Shutdown);
    Sleep(3); // nochmal kurz pausieren
    ShutDown();
  Else
    BroadCastPergon("Shutdown wurde abgebrochen");
    syslog("Shutdown wurde abgebrochen");
  EndIf
EndFunction

///////////////////////////
//                       //
//   Newbie-Funktionen   //
//                       //
///////////////////////////

////////////////////////////////////////////////////////////////////////
// CheckItemMaterial - Darf das Item materialtechnisch benutzt werden?
////////////////////////////////////////////////////////////////////////

Function CheckItemMaterial(who, item)
  If (GetObjProperty(who, TYPNEWBIE)) // Ein Newbie darf keine hoeherwertigen Gegenstaende benutzen
    var result := IsUsableByNewbie(item);

    Case (result.size()) // Gibs was zu beanstanden?
      1: SendSysMessagePergon(who, "Ihr duerft als Neuling nichts aus "+result[1]+" benutzen!");
         return (0);
      2: SendSysMessagePergon(who, "Ihr duerft als Neuling nichts aus "+result[1]+" und "+result[2]+" benutzen!");
         return (0);

      default: return (1); // Noenoe...
    EndCase
  Else
    return (1);
  EndIf
EndFunction

//////////////////////////////////////////////////////////////////////////////////
// IsUsableByNewbie - Darf das Item aus dem Material von Newbies benutzt werden?
//////////////////////////////////////////////////////////////////////////////////

Function IsUsableByNewbie(item)
  var result := {};

  If ((!item.isa(POLCLASS_ARMOR)) Or (item.ar>4)) // Alles kleiner/gleich 4 sind normale Klamotten
    If (item.getprop(TOOLINFO))
      var name := item.getprop(TOOLINFO).material;

      var material := "";
      var pos := Find(name, " ", 1);
      If (pos)
        material := name[1, pos-1];
      Else
        material := name;
      EndIf

      If (!NEWBIE_MATERIAL[" "+material+" "])
        result.append(material);
      EndIf

      // Material2 suchen und pruefen (gibts nur, wenn auch Material1 existiert)
      pos := Find(name, " und ", 1);
      If (pos)
        material := "";
        name[1, pos+4] := "";

        pos := Find(name, " ", 1);
        If (pos)
          material := name[1, pos-1];
        Else
          material := name;
        EndIf

        If (!NEWBIE_MATERIAL[" "+material+" "])
          result.append(material);
        EndIf
      EndIf
    Else


      var name := item.name;
      If (name[Len(name)] == "%")
        name[Len(name)] := "";
      EndIf

      // Namenszusaetze loeschen
      var pos := Find(name, "[", 1);
      If (pos)
        var endpos := Find(name, "]", pos+1);
        If (endpos)
          name[pos, endpos-pos+1] := "";
        EndIf
      EndIf

      // Material1 suchen und pruefen
      pos := Find(name, " aus ", 1);
      If (pos)
        var material := "";
        name[1, pos+4] := "";

        pos := Find(name, " ", 1);
        If (pos)
          material := name[1, pos-1];
        Else
          material := name;
        EndIf

        If (!NEWBIE_MATERIAL[" "+material+" "])
          result.append(material);
        EndIf

        // Material2 suchen und pruefen (gibts nur, wenn auch Material1 existiert)
        var pos := Find(name, " und ", 1);
        If (pos)
          material := "";
          name[1, pos+4] := "";

          pos := Find(name, " ", 1);
          If (pos)
            material := name[1, pos-1];
          Else
            material := name;
          EndIf

          If (!NEWBIE_MATERIAL[" "+material+" "])
            result.append(material);
          EndIf
        EndIf
      EndIf
    EndIf
  EndIf

  return (result);
EndFunction

////////////////////////////////////////////////////////////////////////////////////////
// CheckEquipmentForNewbieArea - Waffen und [schwere] Ruestungen des ExNewbies ablegen
////////////////////////////////////////////////////////////////////////////////////////

Function CheckEquipmentForNewbieArea(who, move_to_ground := 1)
  var issuccess := 1;

  ForEach item in ListEquippedItems(who)
    // (Toten-)Roben immer erlauben
    If (item.objtype == 0x1f03)
      continue;
    EndIf

    // bestimmte Dinge ablegen
    If (
      // alle Waffen
      item.isa(POLCLASS_WEAPON) or
      // zusaetzlich InkogRobe
      (item.objtype == 0x6601) or
      // alle Kleidung/Ruestung, die zu mehr als 25 AR fuehrt
      // (ar_mod abziehen, damit Segenszauber etc. ueber 25 AR
      // nicht zu Dauernacktheit fuehren)
      (item.isa(POLCLASS_ARMOR) and who.ar-who.ar_mod+item.ar > 25)
    )
      var moveres := MoveItemToContainer(item, who.backpack);
      If (moveres)
        // verschoben und fertig
        continue;
      EndIf

      // auf den Boden legen, falls nicht deaktiviert
      If (move_to_ground)
        MoveObjectToLocation(
          item, who.x, who.y, who.z, who.realm, MOVEOBJECT_FORCELOCATION
        );
        continue;
      EndIf

      // bleibt nur die Moeglichkeit der Fehlermeldung
      SendSysMessagePergon(who,
        "Ihr konntet "+EntferneA(item.desc)+" nicht in Euer Backpack packen!",
        "You weren't able to put "+EntferneA(item.desc)+" into your backpack!"
      );
      syslog(
        "FEHLER: Equiptest bei "+CharInfoStr(who, COORDS_REALM)+" konnte "+
        ItemInfoStr(item)+" nicht ablegen: "+moveres.errortext
      );
      issuccess := 0;
    EndIf
  EndForEach

  // Magisches Inkognito vorzeitig beenden
  var realname := GetObjProperty(who, "spell_incognito");
  If (realname)
    PlaySoundEffect(who, SFX_SPELL_INCOGNITO);
    SetNamePergon(
    );
    EraseObjProperty(who, "spell_incognito");
    PrintTextAbovePrivatePergon(
      who, "Euer Name lautet wieder "+who.name,
      "Your are called "+who.name+" again", who
    );
  EndIf

  return issuccess;
EndFunction

// DeNewbie - Newbie-Status wegnehmen
Function DeNewbie(who)
    // Newbie-Flags entfernen
    EraseObjProperty(who, TYPNEWBIE);
    who.title_prefix := "";
    PrintTextAbovePrivatePergon(
      who, "Euer Neulings-Status wird Euch aberkannt.",
      "Your newbie state is ceased.", who
    );
    If (GetObjProperty(who, "Passierschein"))
      EraseObjProperty(who, "Passierschein");
    EndIf
    If (GetObjProperty(who, TYPFLIGHT))
      EraseObjProperty(who, TYPFLIGHT);
    EndIf

    syslog(
      "HINWEIS: "+CharInfoStr(who, COORDS_REALM)+" ist geflohen."
    );
EndFunction

//////////////////////////////////////////////////
// EnterNewbieArea - Das Newbie-Area betreten...
//////////////////////////////////////////////////

Function EnterNewbieArea(who, message)
  If (!(who.isa(POLCLASS_NPC)))
    If (GetObjProperty(who, PLACE_INNEWBIEAREA) != error) // Ist er angeblich noch in Newbie-Area?
      syslog("HINWEIS: "+who.name+" ("+Lower(Hex(who.serial))+") "+message+", hatte aber noch den Status 'InNewbieArea'!");
    EndIf
  EndIf

  SetObjProperty(who, PLACE_INNEWBIEAREA, 1); // Player/Npc befindet sich jetzt offiziell in Newbie-Area
EndFunction

///////////////////////////////////////////////////
// LeaveNewbieArea - Das Newbie-Area verlassen...
///////////////////////////////////////////////////

Function LeaveNewbieArea(who, message)
  If (!(who.isa(POLCLASS_NPC)))
    If (GetObjProperty(who, PLACE_INNEWBIEAREA) == error) // Ist er angeblich garnicht in Newbie-Area?
      syslog("HINWEIS: "+who.name+" ("+Lower(Hex(who.serial))+") "+message+", hatte aber keinen Status 'InNewbieArea'!");
    EndIf
  EndIf

  EraseObjProperty(who, PLACE_INNEWBIEAREA); // Player/Npc hat jetzt offiziell die Newbie-Area verlassen
EndFunction

/////////////////////////
//                     //
//   Gump-Funktionen   //
//                     //
/////////////////////////

/////////////////////////////////////////////////////////////////////////
// SendYesNoDialog - Sendet eine Frage und erwartet Ja/Nein als Antwort
/////////////////////////////////////////////////////////////////////////

Function SendYesNoDialog(who, frage, x, y)
  var layout := {"nodispose", "nomove", "noclose", "page 0"};
  var data := {};

  If (frage.size())
    layout.append("resizepic "+(x+10)+" "+(y+10)+" 2620 400 "+(120+frage.size()*15));

    ForEach zeile in frage
      layout.append("text "+(x+30)+" "+(y+30+data.size()*15)+" 50 "+data.size());
      data.append(zeile);
    EndForEach
  Else
    layout.append("resizepic "+(x+10)+" "+(y+10)+" 2620 400 135");
    layout.append("text "+(x+30)+" "+(y+30)+" 50 0");
    data.append(frage);
  EndIf

  layout.append("button "+(x+110)+" "+(y+60+data.size()*15)+" 1149 1148 1 0 1");
  layout.append("button "+(x+230)+" "+(y+60+data.size()*15)+" 1146 1145 1 0 2");

  If (CInt(SendDialogGump(who, layout, data)[0]) == 1)
    return (1);
  EndIf
EndFunction

// Ja-Nein-Frage in Anlehnung an SendTextEntryGump
Const TE_SCROLLBAR_DISABLE := 0;
Const TE_SCROLLBAR_ENABLE  := 1;
Function SendYesNoGump(
  who, line, x := 0, y := 0,
  cancel := TE_CANCEL_ENABLE, scroll := TE_SCROLLBAR_DISABLE
)
    var layout := {
        "nodispose",
        "page 0",
        "gumppic 0 0 1140",
        "htmlgump 65 55 260 110 0 0 "+scroll,
        "button 100 198 2073 2072 1 0 0", // cancel
        "button 246 198 2076 2075 1 0 1"  // ok
    };
    If (cancel == TE_CANCEL_DISABLE)
        layout.insert(1, "noclose");
    EndIf
    var data := {
        "<basefont color=white><center>"+line+"</center>"
    };

    var result := SendDialogGump(who, layout, data, x, y);
    If (result)
        return 1;
    EndIf
    return 0;
EndFunction

////////////////////////////////////////////////////////////////////////////////
// FillInAccountBuchstaben - Anfangsbuchstaben der Accounts ins Gump eintragen
////////////////////////////////////////////////////////////////////////////////

Function FillInAccountBuchstaben(showopt := SHOW_ALL)
  var layout := {                                       // Gump-Layout
    "page 0",                                         //   Seite 0 (Basisseite)
    "nodispose",
    "resizepic 10 10 2620 310 "+(70+14*20),
    "gumppic 30 40 2621",
    "checkertrans 15 16 300 "+(58+14*20),
    "text 30 19 40 0"
  };
  var data := {"Account-Anfangsbuchstaben auswaehlen"}; // Gump-Data

  var buchstabencount := 0;        // Zahl des einzutragenden Buchstaben
  var msgbutton := BUTTON_ACCOUNT; // Nummer des Buttons
  var pos_x_subpage := 0;          // Akt. Unterseiten-X-Koordinate
  var pos_y := 60;                 // Akt. Y-Koordinate

  var accountarray := {}; // Welche Accountbuchstaben sind belegt (angepasst ob alle, gebannte, ungebannte)
  ForEach acc in ListAccounts()
    If (!(CAsc(Lower(acc[1])) in accountarray))
      Case (showopt)
        SHOW_BANNED:
          If (FindAccount(acc).banned)
            accountarray.append(CAsc(Lower(acc[1])));
          EndIf
        SHOW_NOTBANNED:
          If (!FindAccount(acc).banned)
            accountarray.append(CAsc(Lower(acc[1])));
          EndIf
        SHOW_ENABLED:
          If (FindAccount(acc).enabled)
            accountarray.append(CAsc(Lower(acc[1])));
          EndIf
        SHOW_DISABLED:
          If (!FindAccount(acc).enabled)
            accountarray.append(CAsc(Lower(acc[1])));
          EndIf
        default:
          accountarray.append(CAsc(Lower(acc[1])));
      EndCase
    EndIf
  EndForEach

  For buchstabe := CAsc("a") To CAsc("z") // Alle Anfangsbuchstaben eintragen
    If (buchstabencount == 14) // Unterseite wechseln oder Seitenwechsel-Buttons einfuegen?
      pos_x_subpage := 140; // Unterseite wechseln
      pos_y := 60;
    EndIf

    If (buchstabe in accountarray)  // In der gefilterten Liste vorhanden?
      layout.append("button "+(30+pos_x_subpage)+" "+(pos_y-3)+" 2640 2641 1 0 "+msgbutton);
    EndIf
    layout.append("text "+(65+pos_x_subpage)+" "+pos_y+" 68 "+data.size());
    data.append("'"+Upper(CChr(buchstabe))+"' bzw. '"+CChr(buchstabe)+"'");

    buchstabencount += 1;
    msgbutton += 1;
    pos_y += 20;
  EndFor

  pos_y += 20;
  layout.append("button "+(30+pos_x_subpage)+" "+(pos_y-3)+" 2640 2641 1 0 "+msgbutton);
  layout.append("text "+(65+pos_x_subpage)+" "+pos_y+" 68 "+data.size());
  data.append("Sonstiges");

  return ({layout, data});
EndFunction

////////////////////////////////////////////////////////////////
// ChooseAccountBuchstabe - Einen Anfangsbuchstaben auswaehlen
////////////////////////////////////////////////////////////////

Function ChooseAccountBuchstabe(who, buchstabendata)
  var nummer := SendDialogGump(who, buchstabendata[1], buchstabendata[2])[0]; // Anfangsbuchstaben auswaehlen
  If (nummer)
    If (nummer-BUTTON_ACCOUNT <= CAsc("z")-CAsc("a"))
      return (CChr(nummer-BUTTON_ACCOUNT+CAsc("a")));
    Else
      return (0);
    EndIf
  Else
    SendSysMessagePergon(who, "Fertig");
    return (-1);
  EndIf
EndFunction

///////////////////////////////////////////////////////////////////
// FillInAccountNames - Accounts aller Spieler ins Gump eintragen
///////////////////////////////////////////////////////////////////

Function FillInAccountNames(buchstabe, showopt := SHOW_ALL)
  var layout := {                                         // Gump-Layout
    "page 0",                                           //   Seite 0 (Basisseite)
    "nodispose",
    "resizepic 10 10 2620 310 "+(70+HOEHE_ACCOUNTS*20),
    "gumppic 30 40 2621",
    "checkertrans 15 16 300 "+(58+HOEHE_ACCOUNTS*20)
  };
  var data := {};                                         // Gump-Data

  var accountcount := 0;           // Zahl des einzutragenden Accounts
  var pagecount := 1;              // Akt. Seitenzahl
  var nextpageswap := 0;           // Wann erfolgt ein Seitenwechsel?
  var nextsubpageswap := 0;        // Wann erfolgt ein Unterseitenwechsel?
  var msgbutton := BUTTON_ACCOUNT; // Nummer des Buttons
  var pos_x_subpage := -1;         // Akt. Unterseiten-X-Koordinate
  var pos_y;                     // Akt. Y-Koordinate

  var accountnames := {}; // Accountnamen, alphabetisch sortiert
  ForEach accountname in ListAccounts()
    If (buchstabe) // Einen gezielten Buchstaben, oder den Rest?
      If (Lower(accountname[1]) == buchstabe) // Nur die Accounts mit dem Anfangsbuchstaben
        Case (showopt)  // Welche Anzeigen?
          SHOW_BANNED:
            If (FindAccount(accountname).banned)
              accountnames.append({accountname, accountname});
            EndIf
          SHOW_NOTBANNED:
            If (!FindAccount(accountname).banned)
              accountnames.append({accountname, accountname});
            EndIf
          SHOW_ENABLED:
            If (FindAccount(accountname).enabled)
              accountnames.append({accountname, accountname});
            EndIf
          SHOW_DISABLED:
            If (!FindAccount(accountname).enabled)
              accountnames.append({accountname, accountname});
            EndIf
          default:
            accountnames.append({accountname, accountname});
        EndCase
      EndIf
    Else
      If ((Lower(accountname[1])<"a") Or ("z"<Lower(accountname[1]))) // Nur die Accounts ohne die Anfangsbuchstaben
        Case (showopt)  // Welche Anzeigen?
          SHOW_BANNED:
            If (FindAccount(accountname).banned)
              accountnames.append({accountname, accountname});
            EndIf
          SHOW_NOTBANNED:
            If (!FindAccount(accountname).banned)
              accountnames.append({accountname, accountname});
            EndIf
          SHOW_ENABLED:
            If (FindAccount(accountname).enabled)
              accountnames.append({accountname, accountname});
            EndIf
          SHOW_DISABLED:
            If (!FindAccount(accountname).enabled)
              accountnames.append({accountname, accountname});
            EndIf
          default:
            accountnames.append({accountname, accountname});
        EndCase
      EndIf
    EndIf
  EndForEach
  accountnames := SortMultiArrayByIndexNotCase(accountnames, 1);

  ForEach accountname in accountnames  // Alle Accounts eintragen
    If (accountcount == nextsubpageswap) // Unterseite wechseln oder Seitenwechsel-Buttons einfuegen?
      If (accountcount == nextpageswap)  // Seitenwechsel-Buttons einfuegen?
        layout.append("page "+pagecount);
        layout.append("text 30 19 40 "+data.size());

        If (accountcount>0)
          layout.append("button 283 19 2650 2651 0 "+(pagecount-1));
        EndIf
        If (accountnames.size()>accountcount+HOEHE_ACCOUNTS*2)
          layout.append("button 263 19 2648 2647 0 "+(pagecount+1));
          data.append("Account auswaehlen ("+(accountcount+1)+" bis "+(accountcount+HOEHE_ACCOUNTS*2)+")");
        Else
          data.append("Account auswaehlen ("+(accountcount+1)+" bis "+accountnames.size()+")");
        EndIf

        pagecount := pagecount+1;
        nextpageswap := nextpageswap+HOEHE_ACCOUNTS*2;
      EndIf

      If (pos_x_subpage) // Unterseite wechseln
        pos_x_subpage := 0;
      Else
        pos_x_subpage := 140;
      EndIf

      nextsubpageswap := nextsubpageswap+HOEHE_ACCOUNTS;
      pos_y := 60;
    EndIf

    layout.append("button "+(30+pos_x_subpage)+" "+(pos_y-3)+" 2640 2641 1 0 "+msgbutton);
    layout.append("text "+(65+pos_x_subpage)+" "+pos_y+" 68 "+data.size());
    data.append(accountname[2]);

    accountcount += 1;
    msgbutton += 1;
    pos_y += 20;
  EndForEach

  return ({layout, data, accountnames});
EndFunction

/////////////////////////////////////////////////
// ChooseAccountName - Einen Account auswaehlen
/////////////////////////////////////////////////

Function ChooseAccountName(who, accountdata)
  var nummer := SendDialogGump(who, accountdata[1], accountdata[2])[0]; // Account auswaehlen
  If (nummer)
    return (accountdata[3][nummer-BUTTON_ACCOUNT+1][2]);
  Else
    SendSysMessagePergon(who, "Zurueck");
  EndIf
EndFunction

//////////////////////////////////////////////////////////////////
// FillInCharacter - Character eines Accounts ins Gump eintragen
//////////////////////////////////////////////////////////////////

Function FillInCharacter(accountname)
  var layout := {                                                           // Gump-Layout
    "page 0",                                                             //   Seite 0 (Basisseite)
    "nodispose",
    "noclose",
    "resizepic 10 10 2620 310 "+(70+HOEHE_CHARACTER*20),
    "gumppic 30 40 2621",
    "checkertrans 15 16 300 "+(58+HOEHE_CHARACTER*20),
    "text 30 19 40 0",
    "button 30 "+(38+HOEHE_CHARACTER*20)+" 2121 2120 1 0 "+BUTTON_ZURUECK
  };
  var data := {                                                             // Gump-Data
    "Character von '"+accountname+"' auswaehlen"
  };

  var characters := {}; // Character des Accounts, alphabetisch sortiert

  var account := FindAccount(accountname);
  If (account)
    var charactercount := 0;           // Zahl des einzutragenden Characters
    var msgbutton := BUTTON_CHARACTER; // Nummer des Buttons
    var pos_y := 60;                   // Akt. Y-Koordinate

    For charnr := 1 To 5
      var char := account.getcharacter(charnr);
      If (char)
        characters.append({char.name, char});
      EndIf
    EndFor
    characters := SortMultiArrayByIndexNotCase(characters, 1);

    If (characters.size()) // Gibt es Character in diesem Account?
      ForEach character in characters // Alle Character eintragen
        layout.append("button 30 "+(pos_y-3)+" 2640 2641 1 0 "+msgbutton);
        If (character[2].cmdlevel >= CMDLEVEL_SEER) // CMDLevel-Chars farblich anders anzeigen
          layout.append("text 65 "+pos_y+" 38 "+data.size());
        Else
          layout.append("text 65 "+pos_y+" 68 "+data.size());
        EndIf
        data.append(character[2].name);

        charactercount += 1;
        msgbutton += 1;
        pos_y += 20;
      EndForEach
    Else // Nein...
      layout.append("text 30 "+pos_y+" 68 "+data.size());
      data.append("Keine Character vorhanden");
    EndIf
  Else
    syslog("'"+accountname+"' gibt es nicht !");
  EndIf

  return ({layout, data, characters});
EndFunction

/////////////////////////////////////////////////
// ChooseCharacter - Einen Character auswaehlen
/////////////////////////////////////////////////

Function ChooseCharacter(who, characterdata)
  var nummer := SendDialogGump(who, characterdata[1], characterdata[2])[0]; // Character auswaehlen
  If (nummer)
    return (characterdata[3][nummer-BUTTON_CHARACTER+1][2]);
  EndIf
EndFunction

/////////////////////////////////////////////////
// SoundEmote - Bestimmte Emotes machen Sound
/////////////////////////////////////////////////
Function SoundEmote(who, text)
  var EmoteSounds := dictionary{
    CStr(CAscz("*ah*"))       -> {0x41A, 0x30B}, // _ah
    CStr(CAscz("*ahha*"))     -> {0x41B, 0x30C}, // _ahha
    CStr(CAscz("*applaus*"))  -> {0x41C, 0x30D}, // _applaud
    CStr(CAscz("*schneutz*")) -> {0x41D, 0x30E}, // _blownose
    CStr(CAscz("*rülps*"))    -> {0x41E, 0x30F}, // _burp
    CStr(CAscz("*ruelps*"))   -> {0x41E, 0x30F},
    CStr(CAscz("*freu*"))     -> {0x41F, 0x310}, // _cheer
    // _clear_throad, _cough, _couchbs
    CStr(CAscz("*hust*"))     -> {{0x420, 0x311}, {0x421, 0x312}, {0x422, 0x313}},
    CStr(CAscz("*heul*"))     -> {0x423, 0x314}, // _cry
    CStr(CAscz("*furz*"))     -> {0x429, 0x319}, // _fart
    CStr(CAscz("*erschreck*"))-> {0x42A, 0x31A}, // _gasp
    CStr(CAscz("*kicher*"))   -> {0x42B, 0x31B}, // _giggle
    CStr(CAscz("*hehe*"))     -> {0x42B, 0x31B},
    CStr(CAscz("*hihi*"))     -> {0x42B, 0x31B},
    CStr(CAscz("*gagger*"))   -> {0x42B, 0x31B},
    CStr(CAscz("*stöhn*"))    -> {0x42C, 0x31C}, // _groan
    CStr(CAscz("*stoehn*"))   -> {0x42C, 0x31C},
    CStr(CAscz("*knurr*"))    -> {0x42D, 0x31D}, // _growl
    CStr(CAscz("*hey*"))      -> {0x42E, 0x31E}, // _hey
    CStr(CAscz("*hick*"))     -> {0x42F, 0x31F}, // _hiccup
    CStr(CAscz("*hu?*"))      -> {0x430, 0x320}, // _huh
    CStr(CAscz("*hä?*"))      -> {0x430, 0x320},
    CStr(CAscz("*hae?*"))     -> {0x430, 0x320},
    CStr(CAscz("*küss*"))     -> {0x431, 0x321}, // _kiss
    CStr(CAscz("*kuess*"))    -> {0x431, 0x321},
    CStr(CAscz("*kiss*"))     -> {0x431, 0x321},
    CStr(CAscz("*lach*"))     -> {0x432, 0x322}, // _laugh
    CStr(CAscz("*no*"))       -> {0x433, 0x323}, // _no
    CStr(CAscz("*aua*"))      -> {0x434, 0x324}, // _oh
    CStr(CAscz("*oooh*"))     -> {0x43E, 0x32C}, // _oooh
    CStr(CAscz("*ups*"))      -> {0x43F, 0x32D}, // _oops
    CStr(CAscz("*kotz*"))     -> {0x440, 0x32E}, // _puke
    // _scream, _yell
    CStr(CAscz("*schrei*"))   -> {{0x441, 0x32F}, {0x44B, 0x339}},
    CStr(CAscz("*pst*"))      -> {0x442, 0x330}, // _shush
    CStr(CAscz("*seufz*"))    -> {0x443, 0x331}, // _sigh
    CStr(CAscz("*hum*"))      -> {0x443, 0x331},
    CStr(CAscz("*nies*"))     -> {0x444, 0x332}, // _sneeze
    CStr(CAscz("*schnief*"))  -> {0x445, 0x333}, // _sniff
    CStr(CAscz("*schnarch*")) -> {0x446, 0x334}, // _snore
    CStr(CAscz("*spuck*"))    -> {0x447, 0x335}, // _spit
    CStr(CAscz("*pfeif*"))    -> {0x448, 0x336}, // _whistle
    CStr(CAscz("*flöt*"))     -> {0x448, 0x336},
    CStr(CAscz("*floet*"))    -> {0x448, 0x336},
    CStr(CAscz("*gaehn*"))    -> {0x449, 0x337}, // _yawn
    CStr(CAscz("*gähn*"))     -> {0x449, 0x337},
    CStr(CAscz("*juhu*"))     -> {0x44A, 0x338}  // _yea
  };
  // Ist zwar eigentlich doppelt, da uc_text schon Ascii ist, aber nötig
  // wegen Groß/Kleinschreibung
  var sound := EmoteSounds[CStr(CAscz(text))];
  If (sound)
    If (TypeOfInt(sound[1]) == OT_ARRAY)  // mehrere zur Auswahl?
      PlaySoundEffect(who, sound.randomentry()[who.gender+1]);
    Else
      PlaySoundEffect(who, sound[who.gender+1]);
    EndIf
  EndIf
EndFunction

// vim: sw=2 sts=2
