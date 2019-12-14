///////////////////////////////////////////////////////////////////////////
// names.inc
//
// This files contain routines to a random name within the names.cfg
// file. Look at that file for the types of names you can request.
//
// Contains other useful name functions, too.
//
// Author: Ignatius 1999/01/??

use cfgfile;
use os;
use uo;
use util;
include ":mail:common";
include ":timer:common";
include "include/logutil";
include "include/pergoninfos";
include "include/server";
include "include/urls";
include "include/varutil";
include ":sqlmanager:sqlmanager";

// Konstanten {{{
// Nametypes
Const BIRD_IDX          := 1;
Const DAEMON_IDX        := 2;
Const DRAGON_IDX        := 3;
//Const EVILMAGE_IDX      := 4;
Const MALE_IDX          := 5;
Const FEMALE_IDX        := 6;
//Const PIRATE_MALE_IDX   := 7;
//Const PIRATE_FEMALE_IDX := 8;
Const LIZARD_IDX        := 9;
Const ORC_IDX           := 10;
Const OGRE_IDX          := 11;
Const RATMAN_IDX        := 12;
Const PROTOCOL_MALE_IDX := 13;
Const PROTOCOL_FEM_IDX  := 14;

Const OBJ_BIRD          := 0x6;
Const OBJ_DAEMON        := 0x9;
Const OBJ_DAEMON_SWORD  := 0xa;
Const OBJ_DRAGON_RED    := 0x3d;
Const OBJ_DRAGON_GREY   := 0xc;
Const OBJ_DRAGON_WYRM   := 0x3b;
Const OBJ_ICEWYRM       := 0xb4;
Const OBJ_ANCIENTWYRM   := 0x2e;
Const OBJ_BALRON        := 0x26;
Const OBJ_ARCANEDEMON   := 0x310;
Const OBJ_HELLGUARDIAN  := 0x66;
//Const OBJ_EVILMAGE      :=
Const OBJ_MALE          := 0x190;
Const OBJ_FEMALE        := 0x191;
//Const OBJ_PIRATE_MALE :=
//Const OBJ_PIRATE_FEMALE :=
Const OBJ_LIZARD        := 0x21;
Const OBJ_LIZARD_MACE   := 0x24;
Const OBJ_LIZARD_SPEAR  := 0x23;

Const OBJ_ORC           := 0x11;
Const OBJ_ORC_CLUB      := 0x29;
Const OBJ_ORC_CAPT      := 0x7;

Const OBJ_OGRE          := 0x1;
Const OBJ_TROLL         := 0x35;

Const OBJ_RATMAN        := 0x2a;
Const OBJ_RATMAN_SWORD  := 0x2d;
Const OBJ_RATMAN_AXE    := 0x2c;
Const OBJ_RATMANLORD    := 0x8f;
Const OBJ_RATMANMM      := 0x8e;

Const OBJ_TERATHEN      := 0x46;
Const OBJ_SANDWORM      := 0xF9;
// }}}

// NameCheck-Funktionen
Const IN_PROGRESS  := 1;
Const NAME_DENIED  := 2;
Const NAME_APPLIED := 3;
Const MAX_NAME_LEN := 20;

// Spieler zu Namenseingabe auffordern
Function AutoAskForPlayerName(who)
  var newname := "";
  Repeat
    // solange fragen, bis es mal passt
    newname := SendTextEntryGump(who,
      "Ihr habt noch keinen gueltigen Namen!",
      TE_CANCEL_DISABLE, TE_STYLE_NORMAL, MAX_NAME_LEN,
      "Neuer Name:"
    );
    SleepMS(2);
  Until (!who.connected || (!ForbiddenName(newname) && newname != ""));

  CancelDisconnect(who);

  If (newname)
    CheckPlayerName(who, newname);
  EndIf
EndFunction

// Prueft, ob Name verboten ist
Const NAME_IS_ALLOWED   := 0;
Const NAME_IS_FORBIDDEN := 1;
Function ForbiddenName(name)
  // Namen in Kleinbuchstaben wandeln und in Bestandteile zerlegen
  name := Lower(StrReplace(name, "-", " "));
  var parts := SplitWords(name);

  // Liste der verbotenen Namen
  var names := GetNameCfgSectionContent("forbidden");

  // TODO: Liste der existierenden Namen

  // Namensbestandteile einzeln pruefen
  ForEach part in (parts)
    // kein SleepMs, da haeufiger von Critical-Scripten aufgerufen
    // und tendenziell nur wenige Namensbestandteile existieren
    If (part in (names))
      return NAME_IS_FORBIDDEN;
    EndIf
  EndForEach

  return NAME_IS_ALLOWED;
EndFunction

// Echten Namen eines Spielers ermitteln, unabhaengig von Inkognito
Function GetRealName(charref_or_serial)
    var char := charref_or_serial;
    If (!charref_or_serial.isa(POLCLASS_MOBILE))
        char := SystemFindObjectBySerial(
            charref_or_serial, SYSFIND_SEARCH_OFFLINE_MOBILES
        );
    EndIf

    If (!char)
        return;
    EndIf

    var charname := GetObjProperty(char, "spell_incognito");
    If (charname)
        return charname[2];
    EndIf
    charname := GetObjProperty(char, "realrname");
    If (charname)
        return charname;
    EndIf

    return char.name;
EndFunction


// NPC-Funktionen
// Einen zufaelligen Namen fuer einen NPC ermitteln
Function RandomName(me)
  If (!me.isa(POLCLASS_MOBILE) or !me.isa(POLCLASS_NPC))
    SysLog("FEHLER: RandomName darf nur fuer NPCs benutzt werden!");
    return "";
  EndIf

  var newname;
  Case (me.graphic)
  OBJ_BIRD:
    newname := AssignName(me, BIRD_IDX);

  OBJ_BALRON:
  OBJ_DAEMON:
  OBJ_HELLGUARDIAN:
  OBJ_ARCANEDEMON:
  OBJ_DAEMON_SWORD:
    newname := AssignName(me, DAEMON_IDX);

  OBJ_DRAGON_RED:
  OBJ_DRAGON_GREY:
  OBJ_ICEWYRM:
  OBJ_ANCIENTWYRM:
  OBJ_DRAGON_WYRM:
  OBJ_SANDWORM:
    newname := AssignName(me, DRAGON_IDX);

  OBJ_MALE:
    newname := AssignName(me, MALE_IDX);

  OBJ_FEMALE:
    newname := AssignName(me, FEMALE_IDX);

  OBJ_LIZARD:
  OBJ_LIZARD_MACE:
  OBJ_LIZARD_SPEAR:
    newname := AssignName(me, LIZARD_IDX);

  OBJ_ORC:
  OBJ_ORC_CLUB:
  OBJ_ORC_CAPT:
    newname := AssignName(me, ORC_IDX);

  OBJ_TROLL:
  OBJ_OGRE:
    newname := AssignName(me, OGRE_IDX);

  OBJ_RATMAN:
  OBJ_RATMAN_SWORD:
  OBJ_RATMANLORD:
  OBJ_RATMANMM:
  OBJ_RATMAN_AXE:
    newname := AssignName(me, RATMAN_IDX);

  OBJ_TERATHEN:
    newname := AssignName(me, ORC_IDX);

  default:
    SysLog(
      "FEHLER: NPC ("+me.npctemplate+
      ") ist in RandomName nicht definiert!"
    );
  EndCase

  If (!newname)
    newname := "Namenlos, der Rumtreiber";
    syslog("FEHLER: "+CharInfoStr(me, COORDS_REALM)+" hat keinen Namen!");
  EndIf

  return newname;
EndFunction

// Einen zufaelligen Namen fuer einen NPC einer bestimmten Gruppe ermitteln
Function AssignName(me, index)
  var names := GetNameCfgSectionContent(index);

  var name  := "Namenlos";
  If (names.size())
    name := names.randomentry();
  EndIf

  var newname := me.name;
  newname["<random>"] := name;
  return newname;
EndFunction

// Entfernt "<random>", erhaelt somit Namenspraefix sowie -anhang
Function PreserveTitle(me)
  var title := me.name;

  title["<random>"] := "";

  return title;
EndFunction

// Entfernt ", der Baecker", ", die Bastlerin" etc.
Function RemoveTitle(charname)
  ForEach title in (array{", der ", ", die ", ", das "})
    charname := charname[1, Find(charname, title, 1) - 1];
  EndForEach

  return charname;
EndFunction


// allgemeine Hilfsfunktionen
// Abschnitt aus der names.cfg auslesen
Function GetNameCfgSectionContent(section)
  var namescfg := ReadConfigFile("::names");
  If (!namescfg)
    syslog(
      "FEHLER: Konnte Datei names.cfg nicht oeffnen: "+namescfg.errortext
    );
    return namescfg;
  EndIf

  var elem := FindConfigElem(namescfg, section);
  If (!elem)
    syslog(
      "FEHLER: Konnte Abschnitt "+section+" in names.cfg nicht finden: "+
      elem.errortext
    );
    return elem;
  EndIf

  var names := GetConfigStringArray(elem, "Name");
  return names;
EndFunction

// unnoetige Leerzeichen wegpfeffern etc.
Function CleanupName(name)
  // Leerzeichen entfernen
  name := Trim(name);
  While (name["  "])
    name["  "] := " ";
  EndWhile

  // TODO: vor Interpunktion Leerzeichen entfernen, danach garantieren
  return name;
EndFunction

// Setzt den Namen eines Mobiles
// loginfo kann Mobile oder Logstring sein
Function SetNamePergon(obj, newname, loginfo := 0, force := 0)
  var logstr := "";
  If (loginfo.isa(POLCLASS_MOBILE))
    // GM oder so
    logstr += "gesetzt von "+CharInfoStr(loginfo);
  ElseIf (TypeOfInt(loginfo) == OT_STRING)
    logstr += "("+loginfo+")";
  EndIf

  If (!newname) // Moegliche Fehlerquelle abfangen...
    syslog("FEHLER: SetNamePergon wurde kein Name uebergeben "+logstr);
    return;
  EndIf

  // Name grob bereinigen
  newname := CleanupName(newname);

  If (obj.isa(POLCLASS_ITEM))
    // Items werden einfach so umbenannt
    SetName(obj, newname);
    return;
  EndIf

  // wohl ein Mobile, als erstes Zeichen Grossbuchstaben erzwingen
  // (macht der Client zwar selbst, aber im Serverlog sieht es ggf.
  // seltsam aus)
  newname := UcFirst(newname);

  If (obj.isa(POLCLASS_NPC))
    var name := obj.name;
    var shortname := RemoveTitle(name);
    name[shortname] := newname;
    // Den Namen setzen
    SetName(obj, name);
    // bei NPCs den Titel wegfetzen
    // (myname sollte nur den Namen selbst enthalten)
    obj.setProp("myname", RemoveTitle(name));

    // neustarten, damit sie ihre CProp nochmal einlesen
    var result := RestartScript(obj);
    If (!result)
      SysLog("HINWEIS: NPC-Script nicht neugestartet: "+result.errortext);
    EndIf
  ElseIf (loginfo)
    var name := obj.name;
    If (force)
      name := newname;
    Else
      var shortname := RemoveTown(RemoveGuildPrefix(obj));
      name[shortname] := RemoveTown(RemoveGuildPrefix(obj,newname));
    EndIf
    If (logstr)
      // Logging nur fuer Spielerchars
      syslog(CharInfoStr(obj)+" heisst nun "+name+" "+logstr);
    EndIf
    SetName(obj, name);
    obj.setProp("myname", RemoveTown(RemoveGuildPrefix(obj)));
    SendSQLManager(obj, SQLStatus_UpdateName);
  Else
    CheckPlayerName(obj, newname);
  EndIf
EndFunction

Function CheckPlayerName(who, newname)  
  who.setprop(PROP_NAME_CHECK, IN_PROGRESS);
  SendSQLManager(who, SQLStatus_NewName, newname);
  SendStringAsTipWindow(who, "Euer angeforderter Name wird nun vom Namensteam geprueft. Ueber den Status der Pruefung werdet Ihr per E-Mail benachrichtigt.");
EndFunction

// Stadtkennung aus Namen entfernen
Function RemoveTown(charname)
  return charname[1, Find(charname, " aus ", 1) - 1];
EndFunction

// Name ohne Gildenpraefix zurueckgeben
Function RemoveGuildPrefix(who, charname := "")
  If (!charname)
    charname := who.name;
  EndIf

  var prefix := who.getprop("guildtitle");
  If (!prefix)
    // hat keinen Gildentitel
    return charname;
  EndIf

  var pos := Find(charname, prefix, 1);
  If (!pos)
    // Titel im Namen nicht gefunden
    return charname;
  EndIf

  return charname[pos+Len(prefix)+1, Len(charname)];
EndFunction