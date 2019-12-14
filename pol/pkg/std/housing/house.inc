///////////////////////////////////////////////////////////////////////////
// house
// Haus- und Hausschildfunktionalitaet von Multi- und statischen Haeusern
//
// Author: Shinigami

use cfgfile;
use guilds;
use os;
use uo;
use util;

include ":doorsnew:common";
include ":guildstone:guildutil";
include "include/effects";
include "include/itemnpc";
include "include/logutil";
include "include/msgs";
include "include/names";
include "include/objtype";
include "include/pergonutil";
include "include/permissions";
include "include/place";
include "include/server";
include "include/string";
include "include/sysevent";

// Schildfarben
Const COLOR_PAID              := 63;
Const COLOR_RENTME            := 2121;
Const COLOR_RENTMESOON        := 16;
Const COLOR_UNDERCONSTRUCTION := 55;
Const COLOR_UNPAID            := 40;
Const COLOR_UNPAID_EVICT      := 38;

// maximale Abstand zwischen Hausteilen und Hausschild
// (um diverse Suchen zu begrenzen)
Const MAX_SIGN_RANGE          := 100;

// Liefert vorbelegte Arrays fuer Freunde-Gump
Function GetFriendGump(data_or_layout) // {{{
  If (data_or_layout == "layout")
    return array{
      // Seite 0 (Basisseite)
      "page 0",
      "nodispose",
      "resizepic 10 10 2620 310 590",

      "gumppic 30 40 2621",
      "text 30 19 40 0",
      "text 234 19 40 1"
    };
  EndIf

  return array{"Freund des Hauses", "Entfernen"};
EndFunction // }}}

// Gump-Daten fuer Hausschild
Function GetSignGump(data_or_layout) // {{{
  Case (data_or_layout)
  "layoutowner":
  "layout":
    return array{
      "page 0",
      "gumppic 0 50 2620",
      "gumppic 20 50 2621",
      "gumppic 290 50 2622",
      "gumppic 0 68 2623",
      "gumppic 22 68 2624",
      "gumppic 288 68 2625",
      "gumppic 0 272 2626",
      "gumppic 20 272 2627",
      "gumppic 290 272 2628",
      "gumppic 83 0 100",
      "text 100 29 40 0",
      "text 100 47 40 10",
      "text 20 108 40 1",
      "text 100 108 40 2",
      "text 205 108 40 3",
      "button 55 105 2640 2641 0 1",
      "button 160 105 2640 2641 0 2",
      "button 260 105 2640 2641 0 3",

      "page 1",
      "text 20 150 40 4",
      "text 160 150 68 7",
      "text 20 180 40 12",
      "text 160 180 68 13",
      "text 20 210 40 14",
      "text 160 210 68 15",

      "page 2",
      "text 20 150 40 5",
      "text 30 170 68 16",
      "text 30 190 68 17",
      "text 30 210 68 18",

      "page 3",
      "text 20 147 40 9",       // 33
      "text 20 177 40 11",
      "text 20 207 40 6",               // 35
      "button 260 145 2640 2641 1 3 2", // 36
      "button 260 175 2640 2641 1 3 3",
      "button 260 205 2640 2641 0 4",   // 38

      "page 4",
      "text 65 183 68 8",
      "button 205 175 2640 2641 1 4 1"
    };

  "layoutguest":
    return array{
      "page 0",
      "gumppic 0 50 2620",
      "gumppic 20 50 2621",
      "gumppic 290 50 2622",
      "gumppic 0 68 2623",
      "gumppic 22 68 2624",
      "gumppic 288 68 2625",
      "gumppic 0 272 2626",
      "gumppic 20 272 2627",
      "gumppic 290 272 2628",
      "gumppic 83 0 100",
      "text 100 29 40 0",
      "text 100 47 40 10",
      "text 20 108 40 1",
      "text 100 108 40 2",
      "text 205 108 40 3",
      "button 55 105 2640 2641 0 1",
      "button 160 105 2640 2641 0 2",
      "button 260 105 2640 2641 0 3",

      "page 1",
      "text 20 150 40 4",
      "text 160 150 68 7",
      "text 20 180 40 12",
      "text 160 180 68 13",
      "text 20 210 40 14",
      "text 160 210 68 15",

      "page 2",
      "text 20 150 40 5",
      "text 30 170 68 16",
      "text 30 190 68 17",
      "text 30 210 68 18",

      "page 3",
      "text 20 147 40 9",
      "text 20 177 40 11",
      "text 20 207 40 6"   // 35
      // "button 260 145 2640 2641 1 3 2",
      // "button 260 175 2640 2641 1 3 3",
      // "button 260 205 2640 2641 0 4",

      // "page 4",
      // "text 65 183 68 8",
      // "button 205 175 2640 2641 1 4 1"
    };

  "data":
  default:
    return array{
      "",                                 //  0 - Hausname
      "Info",                             //  1
      "Freunde",                          //  2
      "Optionen",                         //  3
      "Eigentuemer:",                     //  4
      "Benutze die folgenden Kommandos:", //  5
      "Haus abreissen",                   //  6
      "",                                 //  7 - Eigentuemer
      "Seid Ihr SICHER?",                 //  8
      "Haus umbenennen",                  //  9
      "",                                 // 10 - Hausname
      "Schloesser wechseln",              // 11
      "Anzahl Verankerungen:",            // 12
      "",                                 // 13 - "numlockdowns"
      "Anzahl Container:",                // 14
      "",                                 // 15 - "numsecure"
      "Freund einladen",                  // 16
      "Freund ausladen",                  // 17
      "Freunde"                           // 18
    };
  EndCase
EndFunction // }}}

// Das Hausschild benutzen
Function UseHouseSign(who, sign, house) // {{{
  var ownerserial := GetObjProperty(house, PROP_HS_C_OWNER);

  var data := GetSignGump("data");
  // Setzt den Namen des Hauses in das Gump
  SetHouseNameInGump(sign, data);
  // Setzt den Namen des Hausbesitzers in das Gump
  data[8] := GetHouseOwnerName(sign, ownerserial);
  // Liefert Informationen ueber das Haus
  var houseinfo;
  If (IsMultiHouse(house))
    houseinfo := GetHouseInfoMulti(house);
  Else
    houseinfo := GetHouseInfoStatic(house);
  EndIf

  // Falls, aus welchen Gruenden auch immer, die Props nicht gesetzt sind...
  If (GetObjProperty(house, PROP_HS_C_FIX_NUM)==error)
    SetObjProperty(house, PROP_HS_C_FIX_NUM, houseinfo.numlockdowns);
  EndIf
  If (GetObjProperty(house, PROP_HS_C_SC_NUM)==error)
    SetObjProperty(house, PROP_HS_C_SC_NUM, houseinfo.numsecure);
  EndIf

  Case (houseinfo.signtype)
    "GuildHouse":  data[5] := "Gildenmitglied:";
    "PlayerHouse": data[5] := "Eigentuemer:";
    "PlayerRoom":  data[5] := "Mieter:";
    "Rathaus":     data[5] := "Buergermeister:";
  EndCase

  // ... und die Informationen ins Gump eintragen
  data[14] := GetObjProperty(house, PROP_HS_C_FIX_NUM);
  data[16] := GetObjProperty(house, PROP_HS_C_SC_NUM);

  var layoutowner := GetSignGump("layoutowner");
  var layoutguest := GetSignGump("layoutguest");
  // Darf man das Haus zerstoeren?
  If (
    // statische Haeuser darf man nicht abreissen
    !IsMultiHouse(house) or
    // und auch nur ausgewaehlte Personen duerfen
    (!AllowedToHouse(who))
  )
    layoutowner[35] := ""; layoutowner[38] := "";
    layoutguest[35] := "";
  EndIf

  // Darf man das statisches Haus umbenennen?
  If (!IsMultiHouse(house) and !AllowedToHouse(who))
    layoutowner[33] := ""; layoutowner[36]:="";
  EndIf

  // Wer soll was wie sehen, bzw. machen duerfen?
  var layout;
  If (
    // Besitzer
    who.serial == ownerserial Or
    // GL oder ausgewählte Staffler
    who.cmdlevel >= CMDLEVEL_GL Or AllowedToHouse(who)
  )
    layout := layoutowner;
  Else
    layout := layoutguest;
  EndIf

  If (!IsMultiHouse(house))
    layout.append("page 3");
    If (who.cmdlevel >= CMDLEVEL_GL)
      layout.append("text 155 212 68 "+data.size());
      data.append("Reset Koords");
      layout.append("button 260 210 2640 2641 1 0 4");
    Else
      layout.append("");
      layout.append("");
      data.append("");
    EndIf
    If (AllowedToHouse(who))
      layout.append("text 155 247 68 "+data.size());
      data.append("Change Owner");
      layout.append("button 260 245 2640 2641 1 0 5");
    EndIf
  EndIf

  // Gump aufrufen
  Case (SendDialogGump(who, layout, data)[0])
    // Haus des Spielers abreissen (bei StaticSign ausgeblendet...)
    1: DestroyHouse(who, house, houseinfo);
    // Haus des Spielers umbenennen
    2: RenameHouse(who, sign);
    // Schloesser des Hauses austauschen
    3: ChangeHouseLocks(who, house);
    4: ResetKoords(who, sign);
    5: ChangeOwner(who, sign, houseinfo);
  EndCase
EndFunction // }}}

// Setzt den Namen des Hauses in das Gump
Function SetHouseNameInGump(sign, byref data) // {{{
  If (!sign.name)
    data[1] := "[unbenannt]";
    data[11]:="";
    return;
  EndIf

  var namesplitted := SplitWords(sign.name);
  var namepart := namesplitted[1];
  var index := 2;

  While (
    (Len(namepart+" "+namesplitted[index])<15) and
    (index<=namesplitted.size())
  )
    namepart := namepart+" "+namesplitted[index];
    index+=1;
  EndWhile

  data[1] := namepart;

  If (index<=namesplitted.size())
    namepart := namesplitted[index];
    index+=1;

    While (index<=namesplitted.size())
      namepart := namepart+" "+namesplitted[index];
      index+=1;
    EndWhile

    data[11] := namepart;
  EndIf
EndFunction // }}}

// Ermittelt den echten Namen des Hausbesitzers fuer das Gump
// aktualisiert ausserdem CProp "lastownername" fuer Tooltips)
Function GetHouseOwnerName(sign, ownerserial) // {{{
  var ownername;

  If (ownerserial)
    ownername := GetRealName(ownerserial);
  EndIf

  If (ownername)
    SetObjProperty(sign, PROP_HS_C_OWNERNAME, ownername);
    IncRevision(sign);
  Else
    If (ownerserial)
      ownername := "geloeschter Besitzer";
    Else
      ownername := "fehlender Besitzer";
    EndIf
  EndIf

  return ownername;
EndFunction // }}}

// Haus des Spielers abreissen
Function DestroyHouse(who, house, houseinfo)
  If (!AllowedToHouse(who))
    return;
  EndIf

  If (!CreateItemInBackpackPergon(who, houseinfo.objtype))
    SendSysMessagePergon(who, "Es konnte kein Beleg für das Haus erzeugt werden!");
    return;
  EndIf
  
  // Eigentumsnachweis zerstören
  var deedserial := GetObjProperty(house, "deedserial");
  If (deedserial)
    var deed := SystemFindObjectBySerial(deedserial);
    If (deed)
      DestroyItem(deed);
    EndIf
  EndIf

  CreateItemAtLocationPergon(house.x, house.y, house.z, UOBJ_BOARD,        RandomInt(CInt(houseinfo.bretter/2)), house.realm);
  CreateItemAtLocationPergon(house.x, house.y, house.z, UOBJ_LOGS,         RandomInt(CInt(houseinfo.staemme/2)), house.realm);
  CreateItemAtLocationPergon(house.x, house.y, house.z, UOBJ_SANDSTONE,    RandomInt(CInt(houseinfo.sandstein/2)), house.realm);
  CreateItemAtLocationPergon(house.x, house.y, house.z, UOBJ_GRANITSTONE,  RandomInt(CInt(houseinfo.granit/2)), house.realm);
  CreateItemAtLocationPergon(house.x, house.y, house.z, UOBJ_MARBLESTONE,  RandomInt(CInt(houseinfo.marmor/2)), house.realm);
  CreateItemAtLocationPergon(house.x, house.y, house.z, UOBJ_CLAY,         RandomInt(CInt(houseinfo.lehm/2)), house.realm);
  CreateItemAtLocationPergon(house.x, house.y, house.z, UOBJ_STRAW,        RandomInt(CInt(houseinfo.stroh/2)), house.realm);
  CreateItemAtLocationPergon(house.x, house.y, house.z, UOBJ_FOLDED_CLOTH, RandomInt(CInt(houseinfo.tuecher/2)), house.realm);
  CreateItemAtLocationPergon(house.x, house.y, house.z, UOBJ_EISEN_BARREN, RandomInt(CInt(houseinfo.barren/2)), house.realm);
  CreateItemAtLocationPergon(house.x, house.y, house.z, UOBJ_LEATHER,      RandomInt(CInt(houseinfo.leder/2)), house.realm);

  SysLog(CharInfoStr(who)+" zerstoert "+MultiInfoStr(house, COORDS_REALM));

  DestroyMulti(house);
EndFunction

// Haus des Spielers umbenennen
Function RenameHouse(who, sign) // {{{
  var newname := RequestInputPergon(who, sign, "Wie soll euer Haus heißen (max. 40 Zeichen, max. 10 Wörter)?");
  if (newname)
    newname := CChrZ(newname.uc_text);
  endif
  If (Len(newname) > 40)
    SendSysMessagePergon(who,
      "Der Hausname darf nicht laenger als 40 Zeichen sein!"
    );
  ElseIf (SplitWords(newname).size() > 10)
    SendSysMessagePergon(who,
      "Der Hausname darf max. 10 Woerter beinhalten!"
    );
  Else
    SetName(sign, newname);
  EndIf
EndFunction // }}}

// Schloesser des Hauses austauschen
Function ChangeHouseLocks(who, house) // {{{
  SendSysMessagePergon(who, "Waehlt Euren neuen Hauptschluessel aus!");

  var newkey := Target(who);
  If (!newkey)
    SendSysMessagePergon(who, "Abbruch", "Abort");
    return;
  EndIf

  If (
    // kein Item
    !newkey.isa(POLCLASS_ITEM) or
    // kein Schluessel
    !IsAKey(newkey)
  )
    SendSysMessagePergon(who, "Dies ist kein Schluessel!");
    return;
  EndIf

  // Neue SchluesselID holen
  var lockid := AllocLockId();

  // Hauptschluessel markieren
  SetObjProperty(newkey, "lockid", lockid);
  // Schloesser der Tueren des Hauses austauschen
  If (IsMultiHouse(house))
    ChangeHouseDoorLocksMulti(house, lockid);
  Else
    ChangeHouseDoorLocksStatic(house, lockid);
  EndIf

  SendSysMessagePergon(who, "Die Schloesser wurden gewechselt.");
EndFunction // }}}

// Neue Koords für StaticHäuser setzen
Function ResetKoords(who, sign) // {{{
  If (who.cmdlevel < CMDLEVEL_GL)
    return;
  EndIf

  var homeinfo := array{};

  // 1000 Raeume sollten reichen
  var i := 0;
  For (i := 0; i <= 1000; i += 1)
    var coords := {};
    var x1 := 6*i+1;
    var y1 := 6*i+2;
    var z1 := 6*i+3;
    var x2 := 6*i+4;
    var y2 := 6*i+5;
    var z2 := 6*i+6;
    SendSysMessagePergon(who,
      "Waehlt den Bereich, der als Geschoss/Raum Nr."+(i+1)+
      " bewohnbar/benutzbar sein soll! (ESC = Abbruch)"
    );
    Repeat
      // Einlesen der Koords NW-Ecke
      SendSysMessagePergon(who, "Markiere die Nord-West-Ecke");
      coords[1] := TargetCoordinates(who);
      If (!coords[1])
        SendSysMessagePergon(who, "Abbruch", "Abort");
        break;
      EndIf

      // Einlesen der Koords SO-Ecke
      SendSysMessagePergon(who, "Markiere die Sued-Ost-Ecke");
      coords[2] := TargetCoordinates(who);
      If (!coords[2])
        SendSysMessagePergon(who, "Abbruch", "Abort");
        break;
      EndIf

      // Check ob wirklich richtige Ecken
      If (
        (coords[1].x < coords[2].x) and
        (coords[1].y < coords[2].y) and
        (coords[1].z == coords[2].z)
      )

        homeinfo[x1] := coords[1].x;
        homeinfo[y1] := coords[1].y;
        homeinfo[z1] := coords[1].z;
        homeinfo[x2] := coords[2].x;
        homeinfo[y2] := coords[2].y;
        homeinfo[z2] := coords[2].z+19;

        VisualizeRange(coords[1], coords[2]);
      Else
        SendSysMessagePergon(who, "Koordinaten sind falsch gewaehlt!");
        coords[1] := 0;
        coords[2] := 0;
      EndIf

    Until (coords[1] and coords[2]);

    If (!coords[1] or !coords[2])
      // Abbruch, wenn keine Koords gewaehlt,
      // d. h. keine weiteren Zimmer/Etagen gewollt
      break;
    EndIf
  EndFor

  // keine Koordinaten gewaehlt?
  If (homeinfo.size() <= 0)
    return 0;
  EndIf

  SetObjProperty(sign, PROP_HS_S_DIMENSIONS, homeinfo);
  // Controlscript neustarten
  GetProcess(sign.getprop(PROP_HS_S_CTRL_PID)).kill();
  Start_ScriptPergon(":housing:staticsigncontrol", sign);
  SendSysMessagePergon(who, "Neue Koordinaten gesetzt.");
  return 1;
EndFunction // }}}

// Neuen Besitzer für StaticHäuser setzen
Function ChangeOwner(who, sign, houseinfo) // {{{
  // darf nicht, sicherheitshalber nochmal prüfen
  If (!AllowedToHouse(who))
    return;
  EndIf

  SendSysMessagePergon(who,"Wer soll neuer Besitzer werden?");
  var neu := Target(who);
  If (!neu)
    SendSysMessagePergon(who, "Abbruch", "Abort");
  EndIf

  If (neu.isa(POLCLASS_NPC) or !neu.isA(POLCLASS_MOBILE))
    SendSysMessagePergon(who, "Abbruch", "Abort");
    return;
  EndIf

  SetObjProperty(sign, PROP_HS_C_OWNER,   neu.serial);
  SetObjProperty(sign, PROP_HS_C_ACCOUNT, neu.acctname);

  If (neu.cmdlevel >= CMDLEVEL_SEER)
    // Schildfarbe "mietbar"
    sign.color := COLOR_RENTME;
    syslog(ItemInfoStr(sign, COORDS_REALM)+" ist wieder vermietbar");
    // diverse Werte zuruecksetzen
    SetObjProperty(sign, PROP_HS_C_FIX_NUM, houseinfo.numlockdowns);
    SetObjProperty(sign, PROP_HS_C_SC_NUM,  houseinfo.numsecure);
    SetObjProperty(sign, PROP_HS_C_FRIENDS, array{});
    var belehrung :=
      "Verankerungen/Containeranzahl/Freundesliste zurueckgesetzt. "+
      "Prueft, ob das Haus wirklich leer ist!";
    SendSysMessagePergon(who, belehrung);
    If (neu.serial != who.serial)
        SendSysMessagePergon(neu, belehrung);
    EndIf
  Else
    syslog(
      ItemInfoStr(sign, COORDS_REALM)+" wurde von "+CharInfoStr(who)+
      " an "+CharInfoStr(neu)+" vermietet"
    );
  EndIf

  // Controlscript neustarten
  GetProcess(sign.getprop(PROP_HS_S_CTRL_PID)).kill();
  Start_ScriptPergon(":housing:staticsigncontrol", sign);

  SendSysMessagePergon(who, "Neuer Besitzer gesetzt.");
  If (neu.serial != who.serial)
    SendSysMessagePergon(neu, "Das Haus gehoert nun Euch.");
  EndIf

  // aktualisiert "lastownername" am Schild
  GetHouseOwnerName(sign, neu.serial);
EndFunction // }}}

// Liefert Informationen ueber das statische Haus
Function GetHouseInfoStatic(housesign) // {{{
  var deedinfo := struct;
  deedinfo.+signtype := "PlayerRoom";
  deedinfo.+numlockdowns := 0;
  deedinfo.+numsecure := 0;

  var itemdesc := ReadConfigFile(":housing:itemdesc");
  If (!itemdesc)
    syslog("FEHLER: Konnte die :housing:itemdesc.cfg nicht oeffnen!");
    return;
  EndIf

  var housesigndesc := itemdesc[housesign.objtype];
  If (!housesigndesc)
    syslog(
      "FEHLER: Konnte das Hausschild "+LHex(housesign.objtype)+
      " nicht in der :housing:itemdesc.cfg finden!"
    );
    return;
  EndIf

  If (housesigndesc.signtype)
    deedinfo.signtype := housesigndesc.signtype;
  Else
    syslog(
      "WARNUNG: Fehlender 'SignType'-Eintrag im Hausschild '"+
      LHex(deedinfo.objtype)+"'!"
    );
  EndIf

  If (housesigndesc.numlockdowns)
    deedinfo.numlockdowns := housesigndesc.numlockdowns;
  Else
    syslog(
      "WARNUNG: Fehlender 'NumLockDowns'-Eintrag im Hausschild '"+
      LHex(deedinfo.objtype)+"'!"
    );
  EndIf

  If (housesigndesc.numsecure)
    deedinfo.numsecure := housesigndesc.numsecure;
  Else
    syslog("WARNUNG: Fehlender 'NumSecure'-Eintrag im Hausschild '"+
      LHex(deedinfo.objtype)+"'!"
    );
  EndIf

  return (deedinfo);
EndFunction // }}}

// Liefert Informationen ueber das Multihaus
Function GetHouseInfoMulti(house) // {{{
  var deedinfo := struct;
  deedinfo.+objtype;
  deedinfo.+signtype     := "PlayerHouse";
  deedinfo.+numlockdowns := 0;
  deedinfo.+numsecure    := 0;
  deedinfo.+bretter      := 0;
  deedinfo.+staemme      := 0;
  deedinfo.+sandstein    := 0;
  deedinfo.+granit       := 0;
  deedinfo.+marmor       := 0;
  deedinfo.+lehm         := 0;
  deedinfo.+stroh        := 0;
  deedinfo.+tuecher      := 0;
  deedinfo.+barren       := 0;
  deedinfo.+leder        := 0;
  deedinfo.+kristall     := 0;

  var itemdesc := ReadConfigFile(":housing:itemdesc");
  If (!itemdesc)
    syslog("FEHLER: Konnte die :housing:itemdesc.cfg nicht oeffnen!");
    return;
  EndIf

  var housedesc := itemdesc[house.objtype];
  If (!housedesc)
    syslog(
      "FEHLER: Konnte das Haus "+LHex(house.objtype)+
      " nicht in der :housing:itemdesc.cfg finden!"
    );
    return;
  EndIf

  deedinfo.objtype := housedesc.deedobjtype;
  If (!deedinfo.objtype)
    SysLog(
      "FEHLER: Konnte zum Haus "+LHex(house.objtype)+
      " keinen Deed-Eintrag in der :housing:itemdesc.cfg finden!"
    );
    return;
  EndIf

  var housedeeddesc := itemdesc[GetObjtypeByName(deedinfo.objtype)];
  If (!housedeeddesc)
    syslog(
      "FEHLER: Konnte den Deed '"+LHex(deedinfo.objtype)+"' zum Haus "+
      LHex(house.objtype)+" nicht in der :housing:itemdesc.cfg finden!"
    );
    return;
  EndIf

  If (housedeeddesc.numlockdowns)
    deedinfo.numlockdowns := housedeeddesc.numlockdowns;
  ElseIf (!IsNumeric(housedeeddesc.numlockdowns))
    syslog(
      "WARNUNG: Fehlender 'NumLockDowns'-Eintrag im HausDeed '"+
      LHex(deedinfo.objtype)+"'!"
    );
  EndIf
  If (housedeeddesc.numsecure)
    deedinfo.numsecure := housedeeddesc.numsecure;
  ElseIf (!IsNumeric(housedeeddesc.numsecure))
    syslog(
      "WARNUNG: Fehlender 'NumSecure'-Eintrag im HausDeed '"+
      LHex(deedinfo.objtype)+"'!"
    );
  EndIf
  If (housedeeddesc.bretter)
    deedinfo.bretter := housedeeddesc.bretter;
  ElseIf (!IsNumeric(housedeeddesc.bretter))
    syslog(
      "WARNUNG: Fehlender 'Bretter'-Eintrag im HausDeed '"+
      LHex(deedinfo.objtype)+"'!"
    );
  EndIf
  If (housedeeddesc.staemme)
    deedinfo.staemme := housedeeddesc.staemme;
  ElseIf (!IsNumeric(housedeeddesc.staemme))
    syslog(
      "WARNUNG: Fehlender 'Staemme'-Eintrag im HausDeed '"+
      LHex(deedinfo.objtype)+"'!"
    );
  EndIf
  If (housedeeddesc.sandstein)
    deedinfo.sandstein := housedeeddesc.sandstein;
  ElseIf (!IsNumeric(housedeeddesc.sandstein))
    syslog(
      "WARNUNG: Fehlender 'Sandstein'-Eintrag im HausDeed '"+
      LHex(deedinfo.objtype)+"'!"
    );
  EndIf
  If (housedeeddesc.granit)
    deedinfo.granit := housedeeddesc.granit;
  ElseIf (!IsNumeric(housedeeddesc.granit))
    syslog(
      "WARNUNG: Fehlender 'Granit'-Eintrag im HausDeed '"+
      LHex(deedinfo.objtype)+"'!"
    );
  EndIf
  If (housedeeddesc.marmor)
    deedinfo.marmor := housedeeddesc.marmor;
  ElseIf (!IsNumeric(housedeeddesc.marmor))
    syslog(
      "WARNUNG: Fehlender 'Marmor'-Eintrag im HausDeed '"+
      LHex(deedinfo.objtype)+"'!"
    );
  EndIf
  If (housedeeddesc.lehm)
    deedinfo.lehm := housedeeddesc.lehm;
  ElseIf (!IsNumeric(housedeeddesc.lehm))
    syslog(
      "WARNUNG: Fehlender 'Lehm'-Eintrag im HausDeed '"+
      LHex(deedinfo.objtype)+"'!"
    );
  EndIf
  If (housedeeddesc.stroh)
    deedinfo.stroh := housedeeddesc.stroh;
  ElseIf (!IsNumeric(housedeeddesc.stroh))
    syslog(
      "WARNUNG: Fehlender 'Stroh'-Eintrag im HausDeed '"+
      LHex(deedinfo.objtype)+"'!"
    );
  EndIf
  If (housedeeddesc.tuecher)
    deedinfo.tuecher := housedeeddesc.tuecher;
  ElseIf (!IsNumeric(housedeeddesc.tuecher))
    syslog(
      "WARNUNG: Fehlender 'Tuecher'-Eintrag im HausDeed '"+
      LHex(deedinfo.objtype)+"'!"
    );
  EndIf
  If (housedeeddesc.barren)
    deedinfo.barren := housedeeddesc.barren;
  ElseIf (!IsNumeric(housedeeddesc.barren))
    syslog(
      "WARNUNG: Fehlender 'Barren'-Eintrag im HausDeed '"+
      LHex(deedinfo.objtype)+"'!"
    );
  EndIf

  return (deedinfo);
EndFunction // }}}

// alle spieler-festgemachten Items im Haus nachzaehlen
Function Inventur(house) // {{{
  // Inventur nur beim Serverstart durchfuehren
  If (polcore().uptime() > 600)
    return;
  EndIf

  var count;
  var scont;

  If (IsMultiHouse(house))
    // Multihaus
    var info := GetHouseInfoMulti(house);
    count    := info.numlockdowns;
    scont    := info.numsecure;

    ForEach item in (ListObjectsInBox(
        house.footprint.xmin, house.footprint.ymin, -127,
        house.footprint.xmax, house.footprint.ymax,  127, house.realm
    ))
      SleepMs(2);
      If (item.getprop(PROP_HS_C_FIXED))
        count -= 1;
        // sollten immer festgemacht sein,
        // aber gibt ab und an Probleme durch GM-Losmachung
        item.movable := 0;
      EndIf
      If (item.getprop(PROP_HS_C_SC_BOX))
        scont -= 1;
      EndIf
    EndForEach

  Else
    // statisches Haus
    var housedim := house.getprop(PROP_HS_S_DIMENSIONS);

    If (!housedim)
      // doch kein statisches Haus
      return;
    EndIf

    var seen := array{};
    var info := GetHouseInfoStatic(house);
    count    := info.numlockdowns;
    scont    := info.numsecure;

    // alle (vielleicht ueberlappenden) Raeume durchgehen
    While (housedim.size() > 0)
      ForEach item in (ListObjectsInBox(
          housedim[1], housedim[2], housedim[3],
          housedim[4], housedim[5], housedim[6], house.realm
      ))
        SleepMs(2);
        If (!(item.serial in seen))
          seen.append(item.serial);
          If (item.getprop(PROP_HS_C_FIXED))
            count -= 1;
            // sollten immer festgemacht sein,
            // aber gibt ab und an Probleme durch GM-Losmachung
            item.movable := 0;
          EndIf
          If (item.getprop(PROP_HS_C_SC_BOX))
            scont -= 1;
          EndIf
        EndIf
      EndForEach

      // Raum erledigt
      housedim.erase(1); housedim.erase(1); housedim.erase(1);
      housedim.erase(1); housedim.erase(1); housedim.erase(1);
    EndWhile
  EndIf

  If (
    count <> house.getprop(PROP_HS_C_FIX_NUM) or
    scont <> house.getprop(PROP_HS_C_SC_NUM)
  )
    var ownerserial := house.getprop(PROP_HS_C_OWNER);
    var owner := "(unbekannt)";
    If (ownerserial)
      owner := SystemFindObjectBySerial(
        ownerserial, SYSFIND_SEARCH_OFFLINE_MOBILES
      );
      If (owner)
        owner := owner.name;
      Else
        owner := "(geloescht)";
      EndIf
    EndIf

    var fixed := CInt(house.getprop(PROP_HS_C_FIX_NUM));
    If (count <> fixed)
      syslog(
        "INVENTUR: Items "+fixed+"/"+count+" (gespeichert/gezaehlt) bei Haus "+
        LHex(house.serial)+" von "+owner
      );
      house.setprop(PROP_HS_C_FIX_NUM, count);
    EndIf

    var cont := CInt(house.getprop(PROP_HS_C_SC_NUM));
    If (scont <> cont)
      syslog(
        "INVENTUR: Container "+cont+"/"+scont+
        " (gespeichert/gezaehlt) bei Haus "+LHex(house.serial)+" von "+owner
      );
      If (GetGlobalProperty(GPROP_HS_C_CONTINV))
        house.setprop(PROP_HS_C_SC_NUM, scont);
      EndIf
    EndIf
  EndIf
EndFunction // }}}

// Schloesser an Tueren des statischen Hauses austauschen
Function ChangeHouseDoorLocksStatic(house, lockid) // {{{
  var houseinfo := GetObjProperty(house, PROP_HS_S_DIMENSIONS);

  ForEach item in (ListObjectsInBox(
    houseinfo[1], houseinfo[2], houseinfo[3],
    houseinfo[4], houseinfo[5], houseinfo[6]
  )) // Alle originalen Tueren im Haus markieren
    If (
      (UOBJECT_DOORS_START <= item.objtype) and
      (item.objtype <= UOBJECT_DOORS_END) and
      // vom Nutzer nachtraeglich eingebaute Tuer wird ignoriert
      !GetObjProperty(item, PROP_HS_C_FIXED)
    )
      SetObjProperty(item, "lockid", lockid);
      item.locked := 1;
    EndIf
  EndForEach
EndFunction // }}}

// Schloesser der Tueren des Multihauses austauschen
Function ChangeHouseDoorLocksMulti(house, lockid) // {{{
  ForEach item in (house.components)
    If (
      (UOBJECT_DOORS_START <= item.objtype) and
      (item.objtype <= UOBJECT_DOORS_END)
    ) // alle Tueren des Multis
      SetObjProperty(item, "lockid", lockid);
      item.locked := 1;
    EndIf
  EndForEach
EndFunction // }}}

// Auf Kommandos der Spieler warten
Function ControlHouseSign(sign, house, range) // {{{
  // Registriert das Haus im System
  RegisterHouse(house, "#houselist");
  If (!IsMultiHouse(house))
    // nur Schilder statischer Haeuser registrieren
    RegisterHouse(sign, "#signlist");
  EndIf

  // Gildenmeistern als Quasibesitzer registrieren
  RegisterGuildStone(house);

  // ... und auch das Schild bzgl. Sprache
  RegisterForSpeechEvents(sign, range);

  While (sign and house)
    SleepMs(2);
    CheckOwnerStatus(house, sign);
    // Auf ein Ereignis warten...
    var ev := Wait_For_Event(1000);
    If (!ev)
      continue;
    EndIf

    // Befehle werden gesprochen
    If (ev.type == SYSEVENT_SPEECH)
      If (
        // muss im Haus sein
        IsInHouse(house, ev.source) and
        // Pruefung, ob Eigentuemer [oder Freund] jeneswelchen
        IsHouseOwnerOrFriend(house, ev.source)
      )
        Handle_Speech(house, ev);
      EndIf
    EndIf
  EndWhile
EndFunction // }}}

// bei Haeusern mit Gildenstein die Gildenmeister zu Quasibesitzern machen
Function RegisterGuildStone(house) // {{{
  // FIXME: nach einem Update entfernen (2010-03-22)
  house.eraseprop("guildleaders");

  // nach Gildenstein suchen
  var guildid := -1;
  ForEach stone in (ListItemsNearLocationOfType(
      house.x, house.y, LIST_IGNORE_Z,
      MAX_SIGN_RANGE, UOBJ_GUILDSTONE, house.realm
  ))
    If (!IsInHouse(house, stone))
      // nur Steine im Haus interessieren
      continue;
    EndIf

    var stoneid := CInt(stone.getprop("guild_id"));
    If (stoneid > guildid)
      // erster gueltiger Gildenstein im Haus gewinnt
      guildid := stoneid;
      break;
    EndIf
  EndForEach
  If (guildid == -1)
    // offenbar kein Gildenhaus
    return;
  EndIf

  // Gildenleiter herausfinden
  var guild := FindGuild(guildid);
  If (!guild)
    // Gilde gibt es nicht mehr
    return;
  EndIf

  // Gilden-ID fuer spaeter sichern
  house.setprop("#guild_id", guildid);
EndFunction // }}}

// Registriert das Haus(-schild) im System
Function RegisterHouse(item, list) // {{{
  SleepMS(RandomInt(2500)); // bisschen Streuung hilft Lags zu reduzieren

  Set_Critical(1);

  // Liste holen
  var itemlist := GetGlobalProperty(list);
  If (itemlist)
    If (!(item.serial in itemlist))
      itemlist.append(item.serial); // erweitern
    EndIf
  Else
    itemlist := {item.serial};      // neu anlegen
  EndIf
  // speichern
  SetGlobalProperty(list, itemlist);

  Set_Critical(0);
EndFunction // }}}

// Vom Leuten im Haus Gesprochenes auswerten
Function Handle_Speech(house, event) // {{{
  Case (Lower(event.text))
  // Item im inneren eines Hauses fest-/losmachen
  "festmachen":
    ItemFestLosMachen(house, event.source, 1);
  "losmachen":
    ItemFestLosMachen(house, event.source, 0);
  // SecureContainer aufstellen/entfernen
  "container ost":
  "container west":
    SecureContainerAufstellen(house, event.source, 0xe41);
  "container nord":
  "container sued":
    SecureContainerAufstellen(house, event.source, 0xe40);
  "container weg":
    SecureContainerEntfernen(house, event.source);

  // Freundesliste verwalten
  "freund einladen":
    If (IsHouseOwner(house, event.source))
      DialogFreundAufnehmen(house, event.source);
    Else
      SendSysMessagePergon(event.source,
        "Das duerfen nur Hausbesitzer.", "Only house owners can do that."
      );
    EndIf
  "freund ausladen":
    If (IsHouseOwner(house, event.source))
      DialogFreundEntfernen(house, event.source);
    Else
      SendSysMessagePergon(event.source,
        "Das duerfen nur Hausbesitzer.", "Only house owners can do that."
      );
    EndIf
  "freunde":
    DialogFreundeAnzeigen(house, event.source);

  default:
    // Kommando vom Typ "<richtung> <anzahl>"?
    var splittext := SplitWords(event.text);
    If (!splittext.size() == 2)
      return;
    EndIf
    var dist := CInt(splittext[2]);
    If (!IsNumeric(dist) or dist < 1)
      return;
    EndIf

    // Gegenstand im Haus verschieben
    Case (Lower(splittext[1]))
    "nord":
      MoveItemInHouse(house, event.source,      0, -dist,      0);
    "nordz":
      MoveItemInHouse(house, event.source,      0, -dist,      0, 1);
    "sued":
      MoveItemInHouse(house, event.source,      0,  dist,      0);
    "suedz":
      MoveItemInHouse(house, event.source,      0,  dist,      0, 1);
    "ost":
      MoveItemInHouse(house, event.source,  dist,      0,      0);
    "ostz":
      MoveItemInHouse(house, event.source,  dist,      0,      0, 1);
    "west":
      MoveItemInHouse(house, event.source, -dist,      0,      0);
    "westz":
      MoveItemInHouse(house, event.source, -dist,      0,      0, 1);
    "auf":
      MoveItemInHouse(house, event.source,      0,      0,  dist);
    "ab":
      MoveItemInHouse(house, event.source,      0,      0, -dist);
    EndCase
  EndCase
EndFunction // }}}

// Item im inneren eines Hauses fest- oder losmachen
Function ItemFestLosMachen(house, owner, festmachen) // {{{
  SendSysMessagePergon(owner,
    "Waehlt den Gegenstand aus!", "Select an item!"
  );

  var item := Target(owner, TGTOPT_NOCHECK_LOS);
  If (!item.isa(POLCLASS_ITEM))
    SendSysMessagePergon(owner, "Abbruch", "Abort");
    return;
  EndIf

  If (!IsInHouse(house, item))
    SendSysMessagePergon(owner,
      "Das befindet sich nicht innerhalb des Hauses!",
      "That's not in your house!"
    );
    return;
  EndIf

  If (festmachen) // Festmachen
    If (!item.movable)
      SendSysMessagePergon(owner,
        "Der Gegenstand ist bereits verankert!",
        "This item is already anchored."
      );
      return;
    EndIf

    If (!GetObjProperty(house, PROP_HS_C_FIX_NUM))
      SendSysMessagePergon(owner,
        "Es duerfen keine weiteren Gegenstaende verankert werden!",
        "You have reached the maxixum amount of anchors."
      );
      return;
    EndIf

    SetObjProperty(
      house, PROP_HS_C_FIX_NUM, GetObjProperty(house, PROP_HS_C_FIX_NUM) - 1
    );
    item.movable := 0;
    SetObjProperty(item, PROP_HS_C_FIXED, 1);

    SendSysMessagePergon(owner, "Gegenstand verankert.", "Item anchored.");

    syslog(
      CharInfoStr(owner)+" verankert "+ItemInfoStr(item, COORDS_REALM)
    );
  Else // Losmachen
    If (!GetObjProperty(item, PROP_HS_C_FIXED))
      SendSysMessagePergon(owner,
        "Ihr koennt das nicht losmachen!",
        "You cannot detach that."
      );
      return;
    EndIf

    SetObjProperty(
      house, PROP_HS_C_FIX_NUM, GetObjProperty(house, PROP_HS_C_FIX_NUM) + 1
    );
    // Damit es nich sofort verschwindet...
    item.decayat := ReadGameClock()+3600;
    item.movable := 1;
    EraseObjProperty(item, PROP_HS_C_FIXED);

    SendSysMessagePergon(owner, "Gegenstand losgemacht.", "Item detached.");

    syslog(
      CharInfoStr(owner)+" loest "+ItemInfoStr(item, COORDS_REALM)
    );
  EndIf
EndFunction // }}}

// Stellt SecureContainer auf
Function SecureContainerAufstellen(house, owner, objtype) // {{{
  If (!GetObjProperty(house, PROP_HS_C_SC_NUM))
    SendSysMessagePergon(owner,
      "Es duerfen keine weiteren sicheren Container aufgestellt werden!"
    );
    return;
  EndIf

  SendSysMessagePergon(owner,
    "Wo wollt Ihr den sicheren Container aufstellen?"
  );
  var location := TargetCoordinates(owner);
  If (!location)
    SendSysMessagePergon(owner, "Abbruch", "Abort");
    return;
  EndIf
  If (location.item.container)
    SendSysMessagePergon(owner, "Ungueltige Auswahl");
    return;
  EndIf

  If (!IsInHouse(house, location))
    SendSysMessagePergon(owner,
      "Das befindet sich nicht innerhalb des Hauses!"
    );
    return;
  EndIf

  var box := CreateItemAtLocationPergon(
    location.x, location.y, location.z, objtype, 1, location.realm
  );
  If (!box)
    SendSysMessagePergon(owner,
      "Der Container konnte nicht aufgestellt werden!"
    );
    return;
  EndIf

  var innerbox := CreateItemInContainerPergon(box, objtype, 1);
  If (!innerbox)
    SendSysMessagePergon(owner,
      "Der Container konnte nicht aufgestellt werden!"
    );
    DestroyItem(box);
    return;
  EndIf

  SetObjProperty(house, PROP_HS_C_SC_NUM, GetObjProperty(house, PROP_HS_C_SC_NUM)-1);

  box.movable := 0;
  SetObjProperty(box, PROP_HS_C_SC_HOUSE, house.serial);
  SetObjProperty(box, PROP_HS_C_SC_BOX, innerbox.serial);
  box.usescript := ":housing:securecont";

  SendSysMessagePergon(owner, "Sicherer Container wurde gesetzt.");
EndFunction // }}}

// Entfernt SecureContainer
Function SecureContainerEntfernen(house, owner) // {{{
  SendSysMessagePergon(owner,
    "Waehlt den Container aus, den Ihr entfernen wollt!"
  );

  var box := Target(owner, TGTOPT_NOCHECK_LOS);
  If (!box.isa(POLCLASS_CONTAINER))
    SendSysMessagePergon(owner, "Abbruch", "Abort");
    return;
  EndIf

  If (!IsInHouse(house, box))
    SendSysMessagePergon(owner,
      "Das befindet sich nicht innerhalb des Hauses!"
    );
    return;
  EndIf

  var boxserial := GetObjProperty(box, PROP_HS_C_SC_BOX);
  If (!boxserial or GetObjProperty(box, PROP_HS_C_SC_HOUSE) != house.serial)
    SendSysMessagePergon(owner,
      "Dies ist keiner Eurer sicheren Container!"
    );
    return;
  EndIf

  var innerbox := SystemFindObjectBySerial(boxserial);

  // Nur einen leeren Container loeschen lassen...
  If (EnumerateItemsInContainer(innerbox).size() >= 1)
    SendSysMessagePergon(owner,
      "Ihr solltet den Container vorher leerraeumen!"
    );
    return;
  EndIf

  SetObjProperty(house, PROP_HS_C_SC_NUM, GetObjProperty(house, PROP_HS_C_SC_NUM)+1);
  DestroyItem(box);
  SendSysMessagePergon(owner, "Sicherer Container wurde entfernt.");
EndFunction // }}}

// Gegenstand im Haus verschieben
Function MoveItemInHouse(house, owner, modx, mody, modz, retain_z := 0) // {{{
  SendSysMessagePergon(owner, "Waehlt den Gegenstand aus!");

  var item := Target(owner, TGTOPT_NOCHECK_LOS);
  If (!item.isa(POLCLASS_ITEM))
    SendSysMessagePergon(owner, "Abbruch", "Abort");
    return;
  EndIf

  If (!IsInHouse(house, item))
    SendSysMessagePergon(
      owner, "Das befindet sich nicht innerhalb des Hauses!"
    );
    return;
  EndIf

  If (!item.movable)
    SendSysMessagePergon(owner, "Ihr muesst den Gegenstand erst losmachen!");
    return;
  EndIf

  // Prueft und korrigiert (ggf.) die Veraenderung in Z-Richtung
  modz := CheckModZ(item.z, modz);

  If (modx Or mody Or modz) // Ueberhaupt verschieben?
    var oldx := item.x;
    var oldy := item.y;
    var oldz := item.z;
    var oldrealm := item.realm;

// TODO: vielleicht wieder einbauen mit lockeddown-Pruefung?
//    var oldmovable := item.movable;
//    item.movable := 1;

    // Gegenstand verschieben
    If (modz)
      // in der Hoehe
      If (distance(owner, item)>1)
        SendSysMessagePergon(
          owner, "Ihr muesst naeher an den Gegenstand heran."
        );
        return;
      EndIf
      MoveObjectToLocation(
        item, item.x+modx, item.y+mody, item.z+modz, item.realm,
        MOVEOBJECT_FORCELOCATION
      );
    Else
      // in Himmelsrichtungen
      If (retain_z)
        MoveObjectToLocation(
          item, item.x+modx, item.y+mody, item.z, item.realm,
          MOVEOBJECT_FORCELOCATION
        );
      Else
        MoveObjectToLocation(
          item, item.x+modx, item.y+mody, item.z, item.realm,
          MOVEOBJECT_NORMAL
        );
      EndIf
    EndIf

    // falls ausserhalb des Hauses, zurueckholen
    If (!IsInHouse(house, item))
      MoveObjectToLocation(
        item, oldx, oldy, oldz, oldrealm, MOVEOBJECT_FORCELOCATION
      );
    EndIf

    If ((item.x == oldx) && (item.y == oldy) && (item.z == oldz))
      SendSysMessagePergon(
        owner, "Der Gegenstand konnte nicht verschoben werden!"
      );
    Else
      SendSysMessagePergon(owner, "Der Gegenstand wurde verschoben.");
    EndIf
//  item.movable := oldmovable;
  EndIf
EndFunction // }}}

// Ist das Objekt im Haus?
// (struct{x, y, z, realm} ist auch erlaubt)
Function IsInHouse(house, obj) // {{{
  // optional nur Serial uebergeben
  If (TypeOfInt(house) == OT_LONG)
    house := SystemFindObjectBySerial(house);
  EndIf

  var houseserial := house.getprop(PROP_HS_M_HOUSE);
  If (houseserial)
    // da wurde uns ein (Multi-)Schild uebergeben
    house := SystemFindObjectBySerial(houseserial);
  EndIf

  // Multihaus {{{
  If (IsMultiHouse(house))
    // kein echtes Item uebergeben
    If (TypeOfInt(obj) == OT_STRUCT)
      var itemmulti := GetMultiHouseAtPosition(obj);
      If (itemmulti and itemmulti.serial == house.serial)
        return 1;
      EndIf

      return 0;
    EndIf

    // echtes Item uebergeben

    // gar nicht in einem Multi
    If (!obj.multi)
      return 0;
    EndIf

    If (obj.multi.serial == house.serial)
      return 1;
    EndIf

    return 0;
  EndIf
  // }}}

  // statisches Haus {{{
  var housedim := house.getprop(PROP_HS_S_DIMENSIONS);
  If (!housedim)
    return 0;
  EndIf

  For i := 0 to (housedim.size()/6)
    var x1 := 6*i+1;
    var y1 := 6*i+2;
    var z1 := 6*i+3;

    var x2 := 6*i+4;
    var y2 := 6*i+5;
    var z2 := 6*i+6;

    If (
      (housedim[x1] <= obj.x) and (obj.x <= housedim[x2]) and
      (housedim[y1] <= obj.y) and (obj.y <= housedim[y2]) and
      (housedim[z1] <= obj.z) and (obj.z <= housedim[z2]) and
      (house.realm == obj.realm)
    )
      return 1;
    EndIf
    SleepMS(2);
  EndFor
  return 0;
  // }}}
EndFunction // }}}

// Gibt Haus zurueck, in dem sich das Objekt befindet, oder Error
Function GetHouseContainingObject(obj) // {{{
  // Multis abhandeln (geht schneller als statische Haeuser abklappern)
  var onmultihouse := GetMultiHouseAtPosition(obj);
  If (onmultihouse)
    return onmultihouse;
  EndIf

  // statische Haeuser durchprobieren
  ForEach serial in GetGlobalProperty("#signlist")
    SleepMs(2);
    var sign := SystemFindObjectBySerial(serial);
    // existiert nicht mehr oder ist nicht statisch
    If (!sign or !sign.getprop(PROP_HS_S_DIMENSIONS))
        continue;
    EndIf

    // Hack zum Beschleunigen
    // (unter der Annahme, dass das Schild nah am Haus ist,
    // und es keine derart riesigen Haeuser gibt)
    If (CoordinateDistance(obj.x, obj.y, sign.x, sign.y) > MAX_SIGN_RANGE)
        continue;
    EndIf

    If (IsInHouse(sign, obj))
        // Haus gefunden
        return sign;
    EndIf
  EndForEach

  // nichts gefunden
  var fail := error;
  fail.errortext := "Not in a house";
  return fail;
EndFunction // }}}

// ermittelt Multi an pos := struct{x, y, z, realm}
// (darf auch mit Objekten in der Welt aufgerufen werden)
Function GetMultiHouseAtPosition(pos) // {{{
  // bissel sinnlos, aber der Vollstaendigkeit halber
  If (IsMultiHouse(pos))
    return pos;
  EndIf

  // Items koennen verpackt sein
  If (pos.isa(POLCLASS_ITEM))
    While (pos.container)
      pos := pos.container;
    EndWhile
  EndIf

  // oberstes Item bzw. Mobiles/UObjects haben ggf. multi-Member
  If (pos.multi and IsMultiHouse(pos.multi))
    return pos.multi;
  EndIf

  // gar kein Item uebergeben
  If (TypeOfInt(pos) == OT_STRUCT)
    ForEach multi in (ListMultisInBox(
      pos.x, pos.y, -127, pos.x, pos.y, pos.z, pos.realm
    ))
      If (IsMultiHouse(multi))
        return multi;
      EndIf
    EndForEach
  EndIf

  var fail := error;
  fail.errortext := "No multi house at position";
  return fail;
EndFunction // }}}

// ist es ein richtiges Spielerhaus?
Function IsMultiHouse(multi) // {{{
  If (multi.isa(POLCLASS_HOUSE) and multi.getprop(PROP_HS_C_OWNER))
    return 1;
  EndIf

  return 0;
EndFunction // }}}

// Hausschild des Multihauses finden
Function GetMultiHouseSign(house) // {{{
  ForEach part in (house.components)
    SleepMs(2);
    If (part.getprop(PROP_HS_C_OWNERNAME))
      return part;
    EndIf
  EndForEach

  return error{errortext := "No house sign found"};
EndFunction // }}}

// Ist der Character Besitzer oder Hausfreund?
Function IsHouseOwnerOrFriend(house, who) // {{{
  If (!house)
    return 0;
  EndIf

  // um Freundeslisten zu pruefen etc.
  If (AllowedToHouse(who) or who.cmdlevel >= CMDLEVEL_GL)
    return 1;
  EndIf

  // entweder mit Objektreferenz oder mit Serial aufrufbar
  If (TypeOfInt(house) == OT_LONG)
    house := SystemFindObjectBySerial(house);
  EndIf

  // Eigentuemer und Freunde duerfen es benutzen
  If (
    IsHouseFriend(house, who) or
    IsHouseOwner(house, who)
  )
    return 1;
  EndIf

  return 0;
EndFunction // }}}

// Ist der Character Besitzer des Hauses?
Function IsHouseOwner(house, who) // {{{
  If (!house)
    return 0;
  EndIf

  // entweder mit Objektreferenz oder mit Serial aufrufbar
  If (TypeOfInt(house) == OT_LONG)
    house := SystemFindObjectBySerial(house);
  EndIf

  var serial := GetObjProperty(house, PROP_HS_C_OWNER);
  If (!serial)
    // Haus hat keinen Besitzer
    return 0;
  EndIf

  // Eigentuemer
  If (who.serial == serial)
    return 1;
  EndIf

  // bei Gildenhaeusern die Gildenleiter zulassen
  var guildid := house.getprop("#guild_id");
  If (guildid)
    If (IsGuildLeader(who, guildid))
      return 1;
    EndIf
  EndIf

  // anderer Character des Accounts
  var owner := SystemFindObjectBySerial(
    serial, SYSFIND_SEARCH_OFFLINE_MOBILES
  );
  If (
    // keine Seer++ aufnehmen
    who.cmdlevel < CMDLEVEL_SEER and
    // aber sonstige Leute im Account
    owner.acct.name == who.acct.name and
    owner.acct.UsernamePasswordHash == who.acct.UsernamePasswordHash
  )
    var friends := GetObjProperty(house, PROP_HS_C_FRIENDS);
    If (!(who.serial in friends))
      HausFreundAufnehmen(0, house, who);
    EndIf
    return 1;
  EndIf

  return 0;
EndFunction // }}}

// Ist der Character in Liste der Freunde enthalten?
Function IsHouseFriend(house, friend) // {{{
  If (!house)
    return 0;
  EndIf

  // entweder mit Objektreferenz oder mit Serial aufrufbar
  If (TypeOfInt(house) == OT_LONG)
    house := SystemFindObjectBySerial(house);
  EndIf

  // Items und NPCs sind aus Prinzip keine Freunde
  If (!friend.isa(POLCLASS_MOBILE) or friend.isa(POLCLASS_NPC))
    return 0;
  EndIf

  // Ist es ein Freund?
  If (friend.serial in GetObjProperty(house, PROP_HS_C_FRIENDS))
    return 1;
  EndIf

  return 0;
EndFunction // }}}

// Spieler auswaehlen, um ihn in Liste der Freunde aufzunehmen
Function DialogFreundAufnehmen(house, owner) // {{{
  SendSysMessagePergon(owner, "Waehlt Euren Freund fuer das Haus!");

  var friend := Target(owner);
  If ((friend.isa(POLCLASS_MOBILE)) And (!friend.isa(POLCLASS_NPC)))
    If (friend.serial<>owner.serial)
      HausFreundAufnehmen(owner, house, friend);
    Else
      SendSysMessagePergon(owner, "Seid Ihr schizophren?");
    EndIf
  Else
    SendSysMessagePergon(owner, "Abbruch", "Abort");
  EndIf
EndFunction // }}}

// Spieler in Liste der Freunde aufnehmen
Function HausFreundAufnehmen(who, house, friend) // {{{
  If (!friend.isa(POLCLASS_MOBILE) or friend.isa(POLCLASS_NPC))
    return;
  EndIf

  // Critical-Section aktivieren
  var old_critical := Is_Critical();
  Set_Critical(1);

  // Liste der Freunde holen
  var friends := GetAndCheckHausFreunde(house, 0)[1];

  If (friend.serial in friends) // Schon aufgenommen?
    If (who)
      SendSysMessagePergon(who,
        friend.name+" befindet sich bereits in der Freundesliste!",
        friend.name+" is already in the friend list!"
      );
    EndIf
  Else
    friends.append(friend.serial);
    SetObjProperty(house, PROP_HS_C_FRIENDS, friends);
    If (who)
      SendSysMessagePergon(who,
        friend.name+" wurde in die Freundesliste aufgenommen.",
        friend.name+" was added to the friend list."
      );
    EndIf
  EndIf

  // Critical-Section deaktivieren
  Set_Critical(old_critical);
EndFunction // }}}

// Spieler aus Liste der Freunde auswaehlen, um ihn zu entfernen
Function DialogFreundEntfernen(house, owner) // {{{
  // Gump-Daten initialisieren
  var layout := GetFriendGump("layout");
  var data   := GetFriendGump("data");

  // Liste mit {char.name, char}
  var friendsinfo := FillInHouseFriendCharInfo(layout, data, house, 1);
  If (!friendsinfo)
    SendSysMessagePergon(owner,
      "Fuer diese Haus sind keine Freunde eingetragen!"
    );
    return;
  EndIf

  var nummer := SendDialogGump(owner, layout, data)[0];
  If (nummer)
    HausFreundEntfernen(owner, house, friendsinfo[nummer][2]);
  Else
    SendSysMessagePergon(owner, "Abbruch", "Abort");
  EndIf
EndFunction // }}}

// Spieler als Liste der Freunde entfernen
// TODO: optimieren
Function HausFreundEntfernen(who, house, friend) // {{{
  var old_critical:=Is_Critical();
  Set_Critical(1); // Critical-Section aktivieren

  // Liste der Freunde holen
  var friends:=GetAndCheckHausFreunde(house, 0)[1];

  var index:=1; // Freund in der Liste suchen
  While ((index<Len(friends)) and (friends[index]<>friend.serial))
    index+=1;
  EndWhile

  If (friends[index]==friend.serial) // Freund gefunden?
    friends.erase(index);
    SetObjProperty(house, PROP_HS_C_FRIENDS, friends);
    SendSysMessagePergon(who,
      friend.name+" wurde aus der Freundesliste entfernt.",
      friend.name+" was removed from the friend list."
    );
  Else
    SendSysMessagePergon(who,
      friend.name+" befindet sich nicht in der Freundesliste!",
      friend.name+" is not in the friend list!"
    );
  EndIf

  Set_Critical(old_critical); // Critical-Section deaktivieren
EndFunction // }}}

// Liste der Freunde anzeigen
Function DialogFreundeAnzeigen(house, owner) // {{{
  // Gump-Daten initialisieren
  var layout := GetFriendGump("layout");
  var data   := GetFriendGump("data");

  // Liste mit {char.name, char}
  If (FillInHouseFriendCharInfo(layout, data, house))
    data[2] := ""; // Dort nichts anzeigen
    SendDialogGump(owner, layout, data);
  Else
    SendSysMessagePergon(owner,
      "Fuer dieses Haus sind keine Freunde eingetragen!"
    );
  EndIf
EndFunction // }}}

// Liste der Freunde ermitteln und pruefen
Function GetAndCheckHausFreunde(house, critical := 1) // {{{
  // bei Bedarf Critical-Section aktivieren
  var old_critical;
  If (critical)
    old_critical := Is_Critical();
    Set_Critical(1);
  EndIf

  var friends    := {}; // Liste der Freunde
  var friendsref := {}; // Liste der Referenzen
  var friend;

  var friendsprop := GetObjProperty(house, PROP_HS_C_FRIENDS);
  If (friendsprop)
    ForEach friendserial in friendsprop
      friend := SystemFindObjectBySerial(
        friendserial, SYSFIND_SEARCH_OFFLINE_MOBILES
      );
      If (friend) // Gibt es den Char noch?
        friends.append(friendserial);
        friendsref.append(friend);
      EndIf
    EndForEach
  EndIf

  // Liste der Freunde sichern
  SetObjProperty(house, PROP_HS_C_FRIENDS, friends);

  If (critical) // Critical-Section deaktivieren
    Set_Critical(old_critical);
  EndIf

  return ({friends, friendsref});
EndFunction // }}}

// fuer Haeuser auf Jhe'lom gucken, ob Owner noch Newbie ist
Function CheckOwnerStatus(house, sign) // {{{
  var serial := house.getprop(PROP_HS_C_OWNER);
  If (!serial)
    return;
  EndIf

  var owner := SystemFindObjectBySerial(
    serial, SYSFIND_SEARCH_OFFLINE_MOBILES
  );
  If (owner.cmdlevel >= CMDLEVEL_SEER)
    // wenn es gerade nicht vermietet ist, nicht umfärben
    return;
  EndIf

  If (
    // geloescht?
    !owner or
    // oder Jhe'lom-Haus und geflohen
    (PlaceName(house)["Jhe'lom"] and !owner.getprop(TYPNEWBIE))
  )
    // Hausschild farblich markieren
    sign.color := COLOR_RENTMESOON;
    return;
  EndIf
EndFunction // }}}

// Mietstatus aller Haeuser aktualisieren
Function CheckRent(thismonth) // {{{
    var houses := GetGlobalProperty("#houselist");
    If (!houses)
        return;
    EndIf

    ForEach serial in (houses)
        SleepMs(5);
        var house := SystemFindObjectBySerial(serial);
        If (!house)
            // wohl abgerissen
            continue;
        EndIf

        // bei statischen Haeusern passt diese Annahme
        var sign := house;
        If (IsMultiHouse(house))
            sign := GetMultiHouseSign(house);
            If (!sign)
                // ohne Schild keine Mietanzeige am Schild
                continue;
            EndIf
        EndIf

        CheckRentSingle(house, sign, thismonth);
    EndForEach
EndFunction // }}}

// am Monats-/Quartalbeginn Farbe wechseln, um Mietstatus anzuzeigen
Function CheckRentSingle(house, sign, thismonth) // {{{
    If (sign.color == 0)
        // keine Farbe zugewiesen -> kein Mietstatus gewuenscht
        ResetRent(house, sign);
        return;
    EndIf

    If (sign.color in (array{
            // unvermietet, da gibt es keine Mietschuld
            COLOR_RENTME, COLOR_RENTMESOON, COLOR_UNDERCONSTRUCTION
    }))
        ResetRent(house, sign);
        return;
    EndIf

    var color := sign.color;
    Case (color)
      COLOR_PAID:
        If (
            // monatliche Miete auf Jhe'lom
            PlaceName(sign)["Jhe'lom"] or
            // sonst quartalsweise
            (thismonth % 3) == 1
        )
            sign.color := COLOR_UNPAID;
            // Zaehler am Haus setzen und Schild beschriften
            SetObjProperty(house, PROP_HS_C_RENT_TOPAY, 1);
            SetObjProperty(sign,  PROP_HS_C_RENT_TOOLTIP, "Miete faellig");
            IncRevision(sign);
        Else
            // da bezahlt ist, Monate zuruecksetzen
            ResetRent(house, sign);
        EndIf
      COLOR_UNPAID:
      COLOR_UNPAID_EVICT:
        sign.color := COLOR_UNPAID_EVICT;
        // Verzugszaehler am Haus erhoehen
        var count := 1 + CInt(GetObjProperty(house, PROP_HS_C_RENT_TOPAY));
        SetObjProperty(house, PROP_HS_C_RENT_TOPAY, count);
        // Schild passend beschriften
        SetObjProperty(sign,  PROP_HS_C_RENT_TOOLTIP,
            (count-1)+" Monat(e) Mietverzug"
        );
        IncRevision(sign);
    EndCase
EndFunction // }}}

// Miet-Tooltips entfernen
Function ResetRent(house, sign) // {{{
    SetObjProperty(house, PROP_HS_C_RENT_TOPAY, 0);
    SetObjProperty(sign,  PROP_HS_C_RENT_TOOLTIP, "");
    IncRevision(sign);
EndFunction // }}}

// vim: sts=2 sw=2
