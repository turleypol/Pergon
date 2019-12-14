///////////////////////////////////////////////////////////////
// Quest Include - Zur Unterstuetzung von Quests auf Pergon
//
// Author: Shinigami

include "include/clock";
include "include/npc";
include "include/varutil";

///////////////////////////////////////////////////////////////////////////
// Questzonen

// Array mit temporaeren Questzonen
Const GPROP_QUESTZONES      := "tempquestzones";
Const GPROP_QUESTZONES_LOCK := "#workingonquestzones";
// Array of Struct{nw = pos, se = pos, endtime = int, opts}
// pos     = Struct{x, y, realm}
// endtime = GameClock-Zeit, wann die Zone Gueltigkeit verliert
// opts    = Array mit Einstellungen fuer die Zone:
//   bei Ablauf der endtime Questitems in Zone loeschen
//   (z. B. Heilertore von .quest heal)
Const QZ_OPT_DESTROY    := 1;
//   bei Ablauf der endtime Quest-NPCs in Zone toeten
//   (z. B. Heiler von .quest heal)
Const QZ_OPT_KILL       := 2;
//   Zone raeumen, d. h. verbliebene Spieler wegteleportieren
//   (auf die Goetterinsel)
Const QZ_OPT_CLEAR      := 4;
//   Noloot auf alle per Textcmd erstellen NPCs
Const QZ_OPT_NOLOOT     := 8;
//   Nocorpse auf alle per Textcmd erstellen NPCs
Const QZ_OPT_NOCORPSE   := 16;
//   Nocorpse auf alle per Textcmd erstellen NPCs
Const QZ_OPT_NOCARVE    := 32;
// Dummy-Option
Const QZ_OPT_DUMMY      := 0;

// bei QuestZoneIsIn Zonenoptionen zurueckgeben oder nicht?
Const QZ_ISIN_NOOPT     := 0;
Const QZ_ISIN_OPT       := 1;

// Fehlerkonstanten
Const QZ_ERR_NOLOCK := "Access conflict on zone property";

// Prop an Staffler, der gerade eine Zone einrichtet
Const PROP_ZONEMAKE   := "#zonemake";

// Items (und NPCs), die fuer Quests gebaut wurden
// (koennen anhand der Marke automatisch entfernt werden)
Const PROP_QUESTBUILT := "questbuilt";


// Questzone innerhalb von pos1, pos2 erstellen, die lifetime Stunden haelt
Function QuestZoneAdd(pos1, pos2, opts := "", lifetime := 0) // {{{
  // erst einmal die Liste aufraeumen
  QuestZoneExpire();

  // opts ist normalerweise ein Array, umwandeln
  If (opts == "")
    opts := array{};
  EndIf

  // ein Tag
  If (!lifetime)
    lifetime := 24 * 3600;
  Else
    lifetime *= 3600;
  EndIf

  // dafuer sorgen, daß pos1 Nordwestecke und pos2 Suedostecke ist
  If (pos1.x > pos2.x)
    SwapValues(pos1.x, pos2.x);
  EndIf
  If (pos1.y > pos2.y)
    SwapValues(pos1.y, pos2.y);
  EndIf

  // vorbereiten
  var zone := struct;
  zone.+nw      := pos1;
  zone.+se      := pos2;
  zone.+endtime := lifetime + ReadGameClock();
  zone.+opts    := opts;

  If (!SetGlobalLock(GPROP_QUESTZONES_LOCK))
    return error{errortext := QZ_ERR_NOLOCK};
  EndIf

  // Zone einfuegen
  var zonelist := GetGlobalProperty(GPROP_QUESTZONES);
  zonelist.append(zone);
  SetGlobalProperty(GPROP_QUESTZONES, zonelist);

  // wieder freigeben
  EraseGlobalProperty(GPROP_QUESTZONES_LOCK);

  return 1;
EndFunction // }}}

// alte Questzonen aus Liste entfernen und ggf. aufraeumen
Function QuestZoneExpire() // {{{
  var now     := ReadGameClock();
  var newlist := array{};

  If (!SetGlobalLock(GPROP_QUESTZONES_LOCK))
    return error{errortext := QZ_ERR_NOLOCK};
  EndIf

  var oldlist := GetGlobalProperty(GPROP_QUESTZONES);
  ForEach zone in (oldlist)
    SleepMs(2);
    If (zone.endtime > now)
      // alle nicht abgelaufenen Zonen behalten
      newlist.append(zone);
    Else
      // ggf. Aktionen bei Zerstoerung ausloesen
      // (dauert potentiell, daher nicht critical ausfuehren)
      QuestZoneDestroy(zone);
    EndIf
  EndForEach

  SetGlobalProperty(GPROP_QUESTZONES, newlist);

  // wieder freigeben
  EraseGlobalProperty(GPROP_QUESTZONES_LOCK);
EndFunction // }}}

// eine Zone aufraeumen (wird von QuestZoneExpire benutzt)
Function QuestZoneDestroy(zone) // {{{
  // alle Items und Mobiles in Zone abklappern
  ForEach item in (ListObjectsInBox(
      zone.nw.x, zone.nw.y, -127, zone.se.x, zone.se.y, 127, zone.nw.realm
  ))
    SleepMs(2);
    If (item.isa(POLCLASS_NPC))
      If ((QZ_OPT_KILL in (zone.opts)) and item.getprop(PROP_QUESTBUILT))
        // Quest-NPCs sollen getoetet werden
        KillNPCSilent(item, "Cleaning Questzone");
      EndIf

    ElseIf (item.isa(POLCLASS_ITEM))
      If (QZ_OPT_DESTROY in (zone.opts) and item.getprop(PROP_QUESTBUILT))
        // Quest-Items sollen vernichtet werden
        DestroyItem(item);
      EndIf
    EndIf
  EndForEach

  If (QZ_OPT_CLEAR in (zone.opts))
    // Spieler sollen aus Zone entfernt werden
    // (falls ausgeloggt, koennen sie oben gar nicht erwischt werden;
    // um die Arbeit nicht doppelt zu haben, daher nur einmalig hier)
    ForEach acctname in (ListAccounts())
      var account := FindAccount(acctname);
      If (!account)
        continue;
      EndIf
      // alle Accounts abklappern
      For slot := 1 To 6
        // alle Character darin untersuchen
        SleepMs(2);
        var char := account.getcharacter(slot);
        If (!char)
          // der Slot war wohl leer
          continue;
        EndIf

        If (
          zone.nw.x <= char.x and char.x <= zone.se.x and
          zone.nw.y <= char.y and char.y <= zone.se.y
        )
          // in Zone, herauswerfen auf die Goetterinsel
          MoveObjectToLocation(
            char, 5177, 353, 70, REALM_BRITANNIA, MOVEOBJECT_FORCELOCATION
          );
        EndIf
      EndFor
    EndForEach
  EndIf
EndFunction // }}}

// ist jemand in einer Questzone
Function QuestZoneIsIn(who, return_opts := QZ_ISIN_NOOPT) // {{{
  // hier kein Expire, das dauert zu lange
  var zonelist := GetGlobalProperty(GPROP_QUESTZONES);

  // Dummy-Eintrag, damit dictionary nicht leer ist
  var opts := dictionary{};

  ForEach zone in (zonelist)
    SleepMs(2);
    If (
      zone.nw.x <= who.x and who.x <= zone.se.x and
      zone.nw.y <= who.y and who.y <= zone.se.y
    )
      If (return_opts == QZ_ISIN_NOOPT)
        // ist in einer Zone, fertig
        return 1;
      EndIf

      // Optionen merken
      ForEach opt in (zone.opts)
        opts.insert(opt, 1);
      EndForEach
      // damit immer eine Option gesetzt ist
      opts.insert(QZ_OPT_DUMMY, 1);

      // (Zonen koennen ueberlappen, daher weitersuchen)
    EndIf
  EndForEach

  // Keys zurueckgeben
  // leeres Array -- keine Zone
  // Array mit Elementen, ggf. Dummy -- mindestens eine Zone gefunden
  return opts.keys();
EndFunction // }}}

// Zonenrestriktionen auf NPC/Item anwenden (vorher mit QuestZoneIsIn holen)
Function QuestZoneApplyRestrictions(thing, opts) // {{{
  If (!thing.isa(POLCLASS_NPC) and !thing.isa(POLCLASS_ITEM))
    return error{errortext := "Restrictions only valid for items and NPCs"};
  EndIf

  If (thing.isa(POLCLASS_ITEM))
    // bei Items ist nicht viel zu tun
    If (QZ_OPT_DESTROY in (opts))
      thing.setprop(PROP_QUESTBUILT, 1);
    EndIf
    return 1;
  EndIf

  // bleiben noch die NPCs
  If (QZ_OPT_NOLOOT in (opts))
    thing.setprop("noloot", 1);
  EndIf
  If (QZ_OPT_NOCORPSE in (opts))
    thing.setprop("nocorpse", 1);
  EndIf
  If (QZ_OPT_NOCARVE in (opts))
    thing.setprop("nocarve", 1);
  EndIf
EndFunction // }}}

// Zonenrestriktionen wieder entfernen
Function QuestZoneRevokeRestrictions(thing) // {{{
  thing.eraseprop(PROP_QUESTBUILT);
  thing.eraseprop("noloot");
  thing.eraseprop("nocorpse");
  thing.eraseprop("nocarve");
EndFunction // }}}

// da QuestZoneDestroy nicht critical laufen sollte, mal ein neuer Ansatz
Function SetGlobalLock(prop) // {{{
  var crit := Is_Critical();
  Set_Critical(1);
  var lock := GetGlobalProperty(prop);
  If (!lock)
    SetGlobalProperty(prop, GetPid());
    Set_Critical(crit);
    // erfolgreich gesetzt
    return 1;
  EndIf

  If (lock == GetPid())
    Set_Critical(crit);
    // war schon auf mich gelockt
    return 2;
  EndIf

  // Mist, da war schon wer
  Set_Critical(crit);
  return 0;
EndFunction // }}}

///////////////////////////////////////////////////////////////////////////
// NPCs

// GetStichwortList - Liefert eine Liste von Stichwoertern
Function GetStichwortList(questspeech) // {{{
  Var speechfilepath:=Lower("speech/quests/"+questspeech);
  Var stichwoerter:=struct;

  Var speechfile:=ReadConfigFile(speechfilepath);
  If (speechfile)
    stichwoerter.+woerter:=" ";
    stichwoerter.+liste:={};

    ForEach stichwort in GetConfigStringArray(speechfile["Liste"], "Stichwort")
      stichwort:=SplitWords(stichwort);
      stichwoerter.woerter:=stichwoerter.woerter+stichwort[2]+" ";
      (stichwoerter.liste).append(stichwort);
    EndForEach

    UnloadConfigFile(speechfilepath);
  EndIf

  Return (stichwoerter);
EndFunction // }}}

///////////////////////////////////////////////////////////////////////////
// QuestCharacter

// Loggt die besonderen Aktionen eines QuestCharacters
// (nur, wenn cmdlevel == QUESTCHAR)
Function QuestCharLog(who, text) // {{{
  If (who.cmdlevel <> CMDLEVEL_QUESTCHAR)
    return;
  EndIf

  ForceQuestCharLog(who, text);
EndFunction // }}}

// Loggt die besonderen Aktionen eines QuestCharacters
// (cmdlevel egal)
Function ForceQuestCharLog(who, text) // {{{
  var logtext:=" "+CharInfoStr(who, COORDS_REALM)+": "+text;
  LogToFile("log/questchars.log", GetDateTimeStr()+logtext);
EndFunction // }}}

// vim: sw=2 sts=2
