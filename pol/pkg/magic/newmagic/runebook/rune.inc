///////////////////////////////////////////////////////////////////////////
// Rune - Mark, Recall & Gate v1.0 (2002/02/19)
//
//     based on CustomSpells (1999/11/13)
//       Author: Racalac
//
//     Author: Shinigami

// UOBJ_BLUE_MOONGATE -> Die Teile muessen SaveOnExit=0 sein, sonst
// bleiben die ewig bestehen...

///////////////////////////////////////////////////////////////////////////
// Rune
//   InitRuneMark            - Abnutzungsgrad der Rune initialisieren
//   RenameRune              - Rune umbenennen
//   GetShortRuneDesc        - Kurze Beschreibung der Rune ermitteln
//   BlankRune               - Rune durch Gebrauch abnutzen
//
// Mark
//   MarkRune                - Rune am Standort des Casters setzen
//   IsRuneMarked            - Wurde die Rune bereits markiert?
//
// Recall
//   CastRecallRune          - Teleportiert den Magier zur Runen-markierten Stelle
//   CastRecallLocation      - Teleportiert den Magier zur angegebenen Stelle
//   CastRecall              - Magier teleportieren
//
// Gate
//   CastGateRune            - Erzeugt ein Tor zur Runen-markierten Stelle
//   CastGateLocation        - Erzeugt ein Tor zur angegebenen Stelle
//   CastGate                - Tor erzeugen
//
// Sonstiges
//   CheckAndGetSkillAndBooksForCasting
//                           - Skill und Buecher pruefen und zum Casten raussuchen
//   CheckSkillLevelForSkill - Sucht nach Skill gemaess SkillLevel
//   CheckLocations          - Darf man zwischen Quelle und Ziel teleportieren oder ein Tor errichten?
//   LogonCharFixRune        - Korrigiert ggf. fehlerhafte Einstellungen nach dem Einloggen
//
///////////////////////////////////////////////////////////////////////////
//
// Benutzte CProp's - Spieler
//   CInt(recallgraphic) - urspruengliche Grafik des Spielers
//
// Benutzte CProp's - Rune
//   String(Place) - Bezeichnung (Zielposition)
//   CInt(mark)    - Abnutzungsgrad
//   CInt(x)       - Zielposition X
//   CInt(y)       - Zielposition X
//   CInt(z)       - Zielposition X
//   String(realm) - Zielposition Realm
//
// Benutzte CProp's - Rune
//   Array(homeinfo) - ???

/////////////////
// Bibliotheken
/////////////////
use math;
include ":housing:house";
include "include/server";
include "include/logutil";
include "include/place";
include "include/vitals";

///////////////
// Konstanten
///////////////
Const RUNE_SUCCESS := 1;
Const RUNE_CANCEL_NORUNE := -1;
Const RUNE_CANCEL_NOESCAPE := -2;

Const PROP_RECALL_RESULT  := "#CastRuneResult";
Const PROP_RECALL_RUNNING := "CastRecall";

//////////////
//          //
//   Rune   //
//          //
//////////////

//////////////////////////////////////////////////////////
// InitRuneMark - Abnutzungsgrad der Rune initialisieren
//////////////////////////////////////////////////////////

Function InitRuneMark(caster, rune)
  var mark := CInt(GetIntPergon(caster)*0.06);
  SetObjProperty(rune, "mark", mark);

  return (mark);
EndFunction

/////////////////////////////////
// RenameRune - Rune umbenennen
/////////////////////////////////

Function RenameRune(caster, rune)
  If (IsRuneMarked(rune)) // Ist die Rune markiert?
    var desc := "eine ";

    var mark := CInt(GetObjProperty(rune, "mark")); // Zeigt die Rune bereits Abnutzungserscheinungen?
    If (mark<4)
      If (mark<2)
        desc += "fast verbrauchte ";
      Else
        desc += "benutzte ";
      EndIf
    EndIf

    var place := GetObjProperty(rune, "Place"); // Enthaelt die Rune eine Positionsangabe?
    If (place)
      rune.name := desc+"Rune nach "+place;
    Else
      rune.name := desc+"markierte Rune";
    EndIf
  Else
    SendSysMessagePergon(caster, "Diese Rune ist noch nicht markiert!");
  EndIf
EndFunction

/////////////////////////////////////////////////////////////
// GetShortRuneDesc - Kurze Beschreibung der Rune ermitteln
/////////////////////////////////////////////////////////////

Function GetShortRuneDesc(runedesc)
  var phrasen := {"eine Rune nach ", "eine markierte Rune", "eine fast verbrauchte Rune nach ",
                "eine fast verbrauchte markierte Rune", "eine benutzte Rune nach ", "eine benutzte markierte Rune"};
  ForEach phrase in phrasen // Nach den Phrasen suchen
    If (runedesc[phrase])
      runedesc[phrase] := ""; // Phrase entfernen
      Break;                // Schleife vorzeitig beenden
    EndIf
  EndForEach

  If (runedesc == "") // Wurde ueberhaupt ein Ort angegeben?
    runedesc := "nicht benanntes Ziel"; // Wohl nicht
  EndIf

  return (runedesc);
EndFunction

/////////////////////////////////////////////
// BlankRune - Rune durch Gebrauch abnutzen
/////////////////////////////////////////////

Function BlankRune(caster, rune, chance)
  var mark := CInt(GetObjProperty(rune, "mark")); // Zeigt die Rune bereits Abnutzungserscheinungen?
  If (mark<1) // Noch nicht...
    mark := InitRuneMark(caster, rune); // Abnutzungsgrad der Rune initialisieren
  EndIf

  If (RandomInt(10)<chance) // Nutzt sich die Rune ab?
    mark -= 1;
    SetObjProperty(rune, "mark", mark);

    If (mark) // Ist die Rune noch zu gebrauchen, oder bereits zu sehr abgenutzt?
      RenameRune(caster, rune);
    Else
      SendSysMessagePergon(caster, "Eure Rune ist zu sehr abgenutzt und wird vernichtet!");
      DestroyItem(rune);
    EndIf
  EndIf
EndFunction

//////////////
//          //
//   Mark   //
//          //
//////////////

///////////////////////////////////////////////////
// MarkRune - Rune am Standort des Casters setzen
///////////////////////////////////////////////////

Function MarkRune(caster, rune)
  // Darf in dieser Region eine Rune gesetzt werden?
  If (PlaceNoRune(caster) and (!GetObjProperty(caster, I_AM_GOD)))
    SendSysMessagePergon(caster,
      "Die Goetter unterbinden Euren Versuch, diese Stelle zu markieren!"
    );
    PlayObjectCenteredEffect(caster, FX_SPELL_FAIL, 5, 15);
    PlaySoundEffect(caster, SFX_2C);
    return 0;
  EndIf

  var house := GetHouseContainingObject(caster);
  If (house and !IsHouseOwnerOrFriend(house, caster))
    SendSysMessagePergon(caster,
      "Ihr seid nicht berechtigt hier eine Rune zu markieren!"
    );
    PlayObjectCenteredEffect(caster, FX_SPELL_FAIL, 5, 15);
    PlaySoundEffect(caster, SFX_2C);
    return;
  EndIf

  SetObjProperty(rune, "x", caster.x); // mark rune
  SetObjProperty(rune, "y", caster.y);
  SetObjProperty(rune, "z", caster.z);
  SetObjProperty(rune, "realm", caster.realm);

  InitRuneMark(caster, rune); // Abnutzungsgrad der Rune initialisieren
  SetName(rune, "eine markierte Rune");

  return 1;
EndFunction

////////////////////////////////////////////////////
// IsRuneMarked - Wurde die Rune bereits markiert?
////////////////////////////////////////////////////

Function IsRuneMarked(rune)
  If (GetObjProperty(rune, "x") == error) // is unmarked rune
    return (0);
  Else
    return (1);
  EndIf
EndFunction

////////////////
//            //
//   Recall   //
//            //
////////////////

/////////////////////////////////////////////////////////////////////////
// CastRecallRune - Teleportiert den Magier zur Runen-markierten Stelle
/////////////////////////////////////////////////////////////////////////
Function CastRecallRune(caster, rune, power)
  var x     := CInt(GetObjProperty(rune, "x"));
  var y     := CInt(GetObjProperty(rune, "y"));
  var z     := CInt(GetObjProperty(rune, "z"));
  var realm := GetObjProperty(rune, "realm");
  If (!realm)
    realm := _DEFAULT_REALM;
    SetObjProperty(rune, "realm", realm);
  EndIf

  // Shinigami: Distanz-Funktionalitaet abgeschaltet bis die Karte
  // Moongategerecht umgebaut wird

  // // calc spell range according to casting power
  // var spells := ReadConfigFile("::spells");
  // var spell  := FindConfigElem(spells, SPELLID_COMMON_RECALL);
  // var range := CInt(power*spell.range/100.0);
  // If (spell.rangemin and spell.rangemin > range)
  //   range := spell.rangemin;
  // ElseIf (spell.rangemax and spell.rangemax < range)
  //   range := spell.rangemax;
  // EndIf
  //
  // // check If caster wants to recall in range
  // If (CoordinateDistance(caster.x, caster.y, x, y) > range)
  //   SendSysMessagePergon(caster,
  //     "Eure Kraefte reichen nicht aus, um dorthin zurueckzukehren",
  //     "You do not have enough power to recall there"
  //   );
  //   return (RUNE_SUCCESS);
  // EndIf

  // Magier teleportieren
  return CastRecall(caster, x, y, z, rune, realm);

  // Compiler beruhigen
  power := power;
EndFunction

////////////////////////////////////////////////////////////////////////
// CastRecallLocation - Teleportiert den Magier zur angegebenen Stelle
////////////////////////////////////////////////////////////////////////

Function CastRecallLocation(caster, runebook, x, y, z, realm)
  var result := RUNE_SUCCESS;
  If (!realm)
    realm := _DEFAULT_REALM;
  EndIf

  var skillbook := CheckAndGetSkillAndBooksForCasting(caster, runebook); // Skill und Buecher pruefen und zum Casten raussuchen
  If (skillbook) // Verfuegt der Caster ueber magische Faehigkeiten?
    If (PreCastSpell(caster, SPELLID_COMMON_RECALL, skillbook[1])) // Spruch sprechen, ohne das Spruch-Script zu starten
      var spells := ReadConfigFile("::spells");
      var spell := FindConfigElem(spells, SPELLID_COMMON_RECALL);
      var range := spell.range;
      var minrange := spell.rangemin;
      var maxrange := spell.rangemax;
      var power := GetIntPergon(caster);

      range := CInt(power*range/100.0);      // calc spell range according to casting power
      If (minrange and minrange > range)
        range := minrange;
      ElseIf (maxrange and maxrange < range)
        range := maxrange;
      EndIf

      result := CastRecall(caster, x, y, z, 0, realm); // Magier teleportieren

      // Shinigami: Distanz-Funktionalitaet abgeschaltet bis die Karte Moongategerecht umgebaut wird
     // If ((Abs(x - caster.x) > range) || (Abs(y - caster.y) > range))     // check If caster wants to recall in range
     //   SendSysMessagePergon(caster, "Eure Kraefte reichen nicht aus, um dorthin zurueckzukehren",
     //                                "You do not have enough power to recall there");
     // Else
     // EndIf
    EndIf
  EndIf

  return (result);
EndFunction

//////////////////////////////////////
// CastRecall - Magier teleportieren
//////////////////////////////////////
Function CastRecall(caster, to_x, to_y, to_z, rune, to_realm)
  var dest := struct{
    x := to_x, y := to_y, z := to_z, realm := to_realm
  };
  caster := SystemFindObjectBySerial(
    caster.serial, SYSFIND_SEARCH_OFFLINE_MOBILES
  );
  Start_ScriptPergon(
    ":runebook:castrecall",
    struct{caster := caster, dest := dest, rune := rune}
  );
  Sleep(2);
  While (caster.getprop(PROP_RECALL_RUNNING))
    Sleep(1);
    If (!caster.connected)
      // damit beim Abbruch trotzdem Runen abgenutzt werden etc.
      return RUNE_SUCCESS;
    EndIf
  EndWhile

  var ret := caster.getprop(PROP_RECALL_RESULT);
  caster.eraseprop(PROP_RECALL_RESULT);
  If (ret)
    return ret;
  EndIf

  syslog(
    "FEHLER: CastRecallReal hat keinen Status gesetzt bei "+
    CharInfoStr(caster, COORDS_REALM)
  );
  return RUNE_SUCCESS;
EndFunction

Function CastRecallReal(caster, dest, rune)
  // Offline-Ref holen, damit man beim Ausloggen nicht sitzen bleibt
  caster := SystemFindObjectBySerial(
    caster.serial, SYSFIND_SEARCH_OFFLINE_MOBILES
  );

  // Wird in der Logon benutzt, falls der Client abschmiert
  SetObjProperty(caster, PROP_RECALL_RUNNING, 1);

  var oldfrozen := caster.frozen;
  caster.frozen := 1; // Caster festsetzen
  IncRevision(caster);
  PlaySoundEffect(caster, SFX_210);

  If (rune)
    // mit 60%iger Wahrscheinlichkeit Rune abnutzen
    BlankRune(caster, rune, 6);
  EndIf

  // Effekt 1 zeigen
  var origin1 := CreateItemAtLocationPergon(
    caster.x, caster.y, caster.z, 0xf000, 1, caster.realm
  );
  origin1.saveonexit:=0;
  var destination1 := CreateItemAtLocationPergon(
    dest.x, dest.y, dest.z, 0xf000, 1, dest.realm
  );
  destination1.saveonexit:=0;
  Sleep(1);

  // Effekt 2 zeigen
  var origin2 := CreateItemAtLocationPergon(
    caster.x, caster.y, caster.z, 0xf001, 1, caster.realm
  );
  origin2.saveonexit:=0;
  var destination2 := CreateItemAtLocationPergon(
    dest.x, dest.y, dest.z, 0xf001, 1, dest.realm
  );
  destination2.saveonexit:=0;
  Sleep(1);

  // Caster verstecken
  caster.hidden := 1;

  // Effekt 3 zeigen
  var origin3 := CreateItemAtLocationPergon(
    caster.x, caster.y, caster.z, 0xf002, 1, caster.realm
  );
  origin3.saveonexit:=0;
  var destination3 := CreateItemAtLocationPergon(
    dest.x, dest.y, dest.z, 0xf002, 1, dest.realm
  );
  destination3.saveonexit:=0;
  Sleep(1);

  // Effekt 4 zeigen
  var origin4 := CreateItemAtLocationPergon(
    caster.x, caster.y, caster.z, 0xf003, 1, caster.realm
  );
  origin4.saveonexit:=0;
  var destination4 := CreateItemAtLocationPergon(
    dest.x, dest.y, dest.z, 0xf003, 1, dest.realm
  );
  destination4.saveonexit:=0;
  // Basiszeit
  Sleep(2);

  // "Strafzeit" fuer kurze Teleports
  SleepMs(
    45*Pow(Max(
      0,
      20 - CoordinateDistance(caster.x, caster.y, dest.x, dest.y)
    ), 2)
  );

  // Ist der Teleport von den Goettern erlaubt?
  var result := RUNE_SUCCESS;
  result := CheckLocations(caster,
    // Dummy-Eintrag
    struct{x := 0, y := 0, z := 0, realm := _DEFAULT_REALM},
    // nur Ziel pruefen
    dest
  );

  If (result == RUNE_SUCCESS)
    // Quellkoordinaten des Teleports ermitteln fuer Checks und
    // falls Fehler auftritt
    var from := struct{
      x := caster.x, y := caster.y, z := caster.z, realm := caster.realm
    };


    PlayStationaryEffect(
      dest.x, dest.y, dest.z, FX_TELE_VANISH, 0xa, 0xa, 0, dest.realm
    );
    Sleep(RandomInt(2)+1); // Vielleicht hilfts ja gegen die Abstuerze?

    // etwas warten abhaengig von Reiseentfernung
    SleepMs(CoordinateDistance(caster.x, caster.y, dest.x, dest.y));

    var transport_success := MoveObjectToLocation(
      caster, dest.x, dest.y, dest.z, dest.realm
    );
    If (!transport_success)
      // Fehler, zurueck zum Start erzwingen
      MoveObjectToLocation(
        caster, from.x, from.y, from.z, from.realm, MOVEOBJECT_FORCELOCATION
      );
      SendSysMessagePergon(caster,
        "Irgendetwas ging schief!", "Something went wrong!"
      );
    ElseIf (caster.multi)
    	// ein zweimaliges Move scheint das Ausblenden der Obergeschosse
      // im Client zu triggern
    	MoveObjectToLocation(
      caster, dest.x, dest.y, dest.z, dest.realm, MOVEOBJECT_FORCELOCATION
    );
    EndIf

    PlayStationaryEffect(
      from.x, from.y, from.z, FX_TELE_VANISH, 0xa, 0xa, 0, from.realm
    );
    Sleep(RandomInt(2)+1); // Vielleicht hilfts ja gegen die Abstuerze?
  EndIf

  // Effekt 1 & 2 ausblenden
  DestroyItem(origin1); DestroyItem(destination1);
  Sleep(1);
  DestroyItem(origin2); DestroyItem(destination2);
  Sleep(1);
  PlaySoundEffect(caster, SFX_SPELL_RECALL);
  // Caster enttarnen
  If (caster.hidden) // Um unnoetige Events zu sparen...
    caster.hidden := 0;
  EndIf

  // Effekt 3 & 4 ausblenden
  DestroyItem(origin3); DestroyItem(destination3);
  Sleep(1);
  DestroyItem(origin4); DestroyItem(destination4);
  Sleep(1);
  caster.frozen := oldfrozen; // Caster freigeben
  IncRevision(caster);

  If (result <> RUNE_SUCCESS)
    // der Teleport wird von den Goettern nicht erlaubt
    Case (result)
    RUNE_CANCEL_NORUNE:
      SendSysMessagePergon(caster,
        "Die Goetter unterbinden Euren Versuch, den Zielort zu erreichen!",
        "The gods stop your attempt to reach your destination!"
      );
    RUNE_CANCEL_NOESCAPE:
      SendSysMessagePergon(caster,
        "Die Goetter unterbinden Euren Versuch, diesen Ort zu verlassen!",
        "The gods stop your attempt to leave this place!"
      );
    EndCase

    PlayObjectCenteredEffect(caster, FX_SPELL_FAIL, 5, 15);
    PlaySoundEffect(caster, SFX_2C);
  EndIf

  SetObjProperty(caster, PROP_RECALL_RESULT, result);
  EraseObjProperty(caster, PROP_RECALL_RUNNING);

  return result;
EndFunction

// Tiere des Besitzers vor Transport einfrieren/auftauen,
// damit sie bei Zwischenstops nicht in die Region springen
// Const DO_TELE_FREEZE   := 1;
// Const DO_TELE_UNFREEZE := 0;
// Function HandlePets(who, petlist, state)
//   var changedlist := array{};
//   // alle Tier in Verfolgungsbereich ermitteln
//   // (20 Felder, siehe enticedanimal)
//   ForEach mob in (petlist)
//     SleepMs(2);
//     If (mob.getprop("master") <> who.serial)
//       continue;
//     EndIf
//
//     If (mob.frozen <> state)
//       mob.frozen := state;
//       changedlist.append(mob);
//     EndIf
//   EndForEach
//
//   return changedlist;
// EndFunction

///////////////////////////////////////////////////////////////////
// DestroyCastRecallItems - Zerstoert die Recall-Animations-Items
///////////////////////////////////////////////////////////////////
Function DestroyCastRecallItems(items)
  Sleep(60);
  Sleep(6);              // Effekt 1 ausblenden
  DestroyItem(items[1]);
  DestroyItem(items[2]);

  Sleep(1);              // Effekt 2 ausblenden
  DestroyItem(items[3]);
  DestroyItem(items[4]);

  Sleep(1);              // Effekt 3 ausblenden
  DestroyItem(items[5]);
  DestroyItem(items[6]);

  Sleep(1);              // Effekt 4 ausblenden
  DestroyItem(items[7]);
  DestroyItem(items[8]);
EndFunction

//////////////
//          //
//   Gate   //
//          //
//////////////

///////////////////////////////////////////////////////////////
// CastGateRune - Erzeugt ein Tor zur Runen-markierten Stelle
///////////////////////////////////////////////////////////////

Function CastGateRune(caster, rune, power)
  var spells := ReadConfigFile("::spells");
  var spell := FindConfigElem(spells, SPELLID_COMMON_GATETRAVEL);
  var range := spell.range;         // load spelldata
  var minrange := spell.rangemin;
  var maxrange := spell.rangemax;

  var x := CInt(GetObjProperty(rune, "x"));
  var y := CInt(GetObjProperty(rune, "y"));
  var z := CInt(GetObjProperty(rune, "z"));
  var realm := GetObjProperty(rune, "realm");
  If (!realm)
    realm := _DEFAULT_REALM;
  EndIf

  // calc spell range according to casting power
  range := CInt(power*range/100.0);
  If (minrange and minrange > range)
    range := minrange;
  ElseIf (maxrange and maxrange < range)
    range := maxrange;
  EndIf

  // Shinigami: Distanz-Funktionalitaet abgeschaltet bis die Karte
  // Moongategerecht umgebaut wird
  // // check if caster wants to recall in range
  // If ((Abs(x - caster.x) > range) || (Abs(y - caster.y) > range))
  //   SendSysMessagePergon(caster,
  //     "Eure Kraefte reichen nicht aus, um ein Tor dorthin zu oeffnen",
  //     "You do not have enough power to open a gate there"
  //   );
  //   return (RUNE_SUCCESS);
  // Else
  return (CastGate(caster, x, y, z, rune, power, realm)); // Tor erzeugen
  // EndIf
EndFunction

//////////////////////////////////////////////////////////////
// CastGateLocation - Erzeugt ein Tor zur angegebenen Stelle
//////////////////////////////////////////////////////////////

Function CastGateLocation(caster, runebook, x, y, z, realm)
  var result := RUNE_SUCCESS;
  If (!realm)
    realm := _DEFAULT_REALM;
  EndIf

  var skillbook := CheckAndGetSkillAndBooksForCasting(caster, runebook); // Skill und Buecher pruefen und zum Casten raussuchen
  If (skillbook) // Verfuegt der Caster ueber magische Faehigkeiten?
    If (PreCastSpell(caster, SPELLID_COMMON_GATETRAVEL, skillbook[1])) // Spruch sprechen, ohne das Spruch-Script zu starten
      var spells := ReadConfigFile("::spells");
      var spell := FindConfigElem(spells, SPELLID_COMMON_GATETRAVEL);
      var range := spell.range;
      var minrange := spell.rangemin;
      var maxrange := spell.rangemax;
      var power := GetIntPergon(caster);

      range := CInt(power*range/100.0);      // calc spell range according to casting power
      If (minrange and minrange > range)
        range := minrange;
      ElseIf (maxrange and maxrange < range)
        range := maxrange;
      EndIf
      SleepMS(2); // Kleine Pause wegen Runaway

      result := CastGate(caster, x, y, z, 0, power, realm); // Tor erzeugen

      // Shinigami: Distanz-Funktionalitaet abgeschaltet bis die Karte Moongategerecht umgebaut wird
     // If ((Abs(x - caster.x) > range) || (Abs(y - caster.y) > range))     // check If caster wants to recall in range
     //   SendSysMessagePergon(caster, "Eure Kraefte reichen nicht aus, um ein Tor dorthin zu oeffnen",
     //                                "You do not have enough power to open a gate there");
     // Else
     // EndIf
    EndIf
  EndIf

  return (result);
EndFunction

////////////////////////////
// CastGate - Tor erzeugen
////////////////////////////

Function CastGate(caster, to_x, to_y, to_z, rune, power, to_realm)
  // Wird der Aufbau eine Tores von den Goettern erlaubt?
  var result := RUNE_SUCCESS;

  If (rune) // Handelt es sich um eine Rune?
    // mit 80%iger Wahrscheinlichkeit Rune abnutzen
    BlankRune(caster, rune, 8);
  EndIf

  var from := struct{
    x     := caster.x,
    y     := caster.y,
    z     := caster.z,
    realm := caster.realm
  };
  var dest := struct{
    x     := to_x,
    y     := to_y,
    z     := to_z,
    realm := to_realm
  };

  // Effekte anzeigen
  PlaySoundEffect(caster, SFX_SPELL_GATE_TRAVEL);
  PlayStationaryEffect(
    from.x, from.y, from.z, FX_SPARK_EFFECT, 10, 10, 0, from.realm
  );
  PlayStationaryEffect(
    dest.x, dest.y, dest.z, FX_SPARK_EFFECT, 10, 10, 0, dest.realm
  );

  // Darf man zum Ziel ein Tor aufbauen?
  result := CheckLocations(caster, from, dest);

  If (result == RUNE_SUCCESS)
    // Ist der Aufbau eines Tores von den Goettern erlaubt?
    var gatetype := UOBJ_BLUE_MOONGATE;

    // Case (skill)
    //   SKILLID_MAGIE:   gatetype := UOBJ_RED_MOONGATE;
    //   SKILLID_SEGNEN:  gatetype := UOBJ_WHITE_MOONGATE;
    //   SKILLID_FLUCHEN: gatetype := UOBJ_BLACK_MOONGATE;
    //
    //   default: gatetype := UOBJ_BLUE_MOONGATE;
    // EndCase

    // Caster vom Tor wegbewegen
    MoveObjectToLocation(caster, from.x+1, from.y+1, from.z, from.realm);
    // Bissel mehr Effekt
    PlayStationaryEffect(
      from.x, from.y, from.z, FX_SUMMON_GATE, 10, 32, 0, from.realm
    );
    PlayStationaryEffect(
      dest.x, dest.y, dest.z, FX_SUMMON_GATE, 10, 32, 0, dest.realm
    );
    SleepMS(1500);
    // Tore erzeugen
    var gatefrom := CreateItemAtLocationPergon(
      from.x, from.y, from.z, gatetype, 1, from.realm
    );
    var gateto := CreateItemAtLocationPergon(
      dest.x, dest.y, dest.z, gatetype, 1, dest.realm
    );
    // Wurden die Tore angelegt?
    If (gatefrom And gateto And result)
      SysLog(CharInfoStr(caster)+" stellt "+ItemInfoStr(gatefrom, COORDS_REALM)+" nach "+ItemInfoStr(gateto, COORDS_REALM));
      var sleeptime := CInt(power*9);
      // Fuer den Fall, dass der Client abraucht
      var itemsdecayat := ReadGameClock() + sleeptime + 30;

      // Tore sichern
      gatefrom.movable := 0;
      gatefrom.decayat := itemsdecayat;
      gateto.movable := 0;
      gateto.decayat := itemsdecayat;

      // Informationssammlung fuer Zweitcharsupport-Test
      var info  := struct;
      info.addr := caster.ip;
      info.acct := caster.acctname;
      info.id   := caster.serial;
      info.mail := Lower(caster.acct.getprop(PLAYEREMAIL));
      info.name := GetRealName(caster);

      // Tore initialisieren
      SetObjProperty(gatefrom, "GateDestX", gateto.x);
      SetObjProperty(gatefrom, "GateDestY", gateto.y);
      SetObjProperty(gatefrom, "GateDestZ", gateto.z);
      SetObjProperty(gatefrom, "GateDestRealm", gateto.realm);
      SetObjProperty(gatefrom, "GateDestItem", gateto.serial);
      SetObjProperty(gatefrom, "GatePower", power);
      SetObjProperty(gatefrom, "caster", caster.serial);
      SetObjProperty(gatefrom, "castername", caster.name);
      // SetObjProperty(gatefrom, SELF_SUPPORT1, info);
      SetObjProperty(gateto, "GateDestX", gatefrom.x);
      SetObjProperty(gateto, "GateDestY", gatefrom.y);
      SetObjProperty(gateto, "GateDestZ", gatefrom.z);
      SetObjProperty(gateto, "GateDestRealm", gatefrom.realm);
      SetObjProperty(gateto, "GateDestItem", gatefrom.serial);
      SetObjProperty(gateto, "GatePower", power);
      SetObjProperty(gateto, "caster", caster.serial);
      SetObjProperty(gateto, "castername", caster.name);
      // SetObjProperty(gateto, SELF_SUPPORT1, info);

      gatefrom.name := ("Magisches Tor [" + PlaceName(gateto)+ "]");
      gateto.name := ("Magisches Tor [" + PlaceName(gatefrom)+ "]");

      Detach();                                         // Caster kann wieder was machen

      Sleep(sleeptime);                                 // Tore eine gewisse Zeit aufrechterhalten

      // destroy moongates
      If (gatefrom)
        // Effekte anzeigen
        PlayStationaryEffect(
          from.x, from.y, from.z, FX_TELE_VANISH, 10, 10, 0, from.realm
        );
        SysLog(ItemInfoStr(gatefrom, COORDS_REALM)+" hat sich nach "+sleeptime+" Sekunden aufgeloest");
        DestroyItem(gatefrom);
      EndIf
      If (gateto)
        PlayStationaryEffect(
          dest.x, dest.y, dest.z, FX_TELE_VANISH, 10, 10, 0, dest.realm
        );
        SysLog(ItemInfoStr(gateto, COORDS_REALM)+" hat sich nach "+sleeptime+" Sekunden aufgeloest");
        DestroyItem(gateto);
      EndIf

    Else
      DestroyItem(gatefrom); // Eines von beiden koennte ja noch da sein
      DestroyItem(gateto);

      SendSysMessagePergon(caster, "Irgendwas ging schief!");
    EndIf
  Else
    Case (result)
      RUNE_CANCEL_NORUNE:   SendSysMessagePergon(caster, "Die Goetter unterbinden Euren Versuch, ein Tor zum Zielort aufzubauen!");
      RUNE_CANCEL_NOESCAPE: SendSysMessagePergon(caster, "Die Goetter unterbinden Euren Versuch, ein Tor von diesen Ort aufzubauen!");
    EndCase

    PlayObjectCenteredEffect(caster, FX_SPELL_FAIL, 5, 15);
    PlaySoundEffect(caster, SFX_2C);
  EndIf

  return (result);
EndFunction

///////////////////
//               //
//   Sonstiges   //
//               //
///////////////////

/////////////////////////////////////////////////////////////////////////////////////////////
// CheckAndGetSkillAndBooksForCasting - Skill und Buecher pruefen und zum Casten raussuchen
/////////////////////////////////////////////////////////////////////////////////////////////

Function CheckAndGetSkillAndBooksForCasting(who, runebook)
  var basiscontainer := runebook.container; // Runenbuch muss sich im Backpack befinden
  While (basiscontainer.container)
    basiscontainer := basiscontainer.container;
  EndWhile

  If (basiscontainer.serial == who.serial) // Befindet sich das Runenbuch im Backpack?
    If (AccessiblePergon(who, runebook))
      var scroll, skill;
      var SkillID := { SKILLID_MAGIE, SKILLID_SEGNEN, SKILLID_FLUCHEN };
      For i := 1 To 3
        scroll := GetCastScrollInBackpack(who, SPELLID_COMMON_RECALL, SkillID[i]);
        If ((scroll != 0) and 
        	(HasPlayerSkill(who, SkillID[i]) or who.cmdlevel >= CMDLEVEL_SEER) and 
        	(GetObjProperty(who, "#UsesScroll") < ReadGameClock())
        )
          skill := SkillID[i];
          break;
        EndIf
      EndFor
      If (!skill)
        skill := CheckSkillLevelForSkill(who, 0, SKILLTYP_BERUF);
        skill := CheckSkillLevelForSkill(who, skill, SKILLTYP_HAUPT);
        skill := CheckSkillLevelForSkill(who, skill, SKILLTYP_NEBEN);
        skill := CheckSkillLevelForSkill(who, skill, SKILLTYP_WEITERE);
        If (who.cmdlevel >= CMDLEVEL_SEER)
      		skill := SKILLID_MAGIE;
      	EndIf
      EndIf

      If (skill)
        var spellbookdesc;
        var spellbook;

        Case (skill)
          SKILLID_MAGIE:   spellbook := SearchSpellBookInBackpack(who, UOBJ_SPELLBOOK_MAGE);
                           spellbookdesc := {"euer Grimoire", "kein Grimoire"};
          SKILLID_SEGNEN:  spellbook := SearchSpellBookInBackpack(who, UOBJ_SPELLBOOK_CLERIC);
                           spellbookdesc := {"euren Liber Castus", "keinen Liber Castus"};
          SKILLID_FLUCHEN: spellbook := SearchSpellBookInBackpack(who, UOBJ_SPELLBOOK_NECRO);
                           spellbookdesc := {"euer paq mu'qaDmey", "kein paq mu'qaDmey"};
        EndCase

        If (spellbook) // Zauberbuch gefunden?
          If (AccessiblePergon(who, spellbook))
            return ({skill, spellbook});
          Else
            SendSysMessagePergon(who, "Ihr kommt nicht an "+spellbookdesc[1]+" heran!");
          EndIf
        Else
          SendSysMessagePergon(who, "Ihr habt "+spellbookdesc[2]+" in eurem Backpack!");
        EndIf
      Else
        SendSysMessagePergon(who, "Ihr wisst nichts mit magischen Dingen anzufangen!");
      EndIf
    Else
      SendSysMessagePergon(who, "Ihr kommt nicht an das Runenbuch heran!");
    EndIf
  Else
    SendSysMessagePergon(who, "Das Runenbuch muss sich im Backpack befinden!");
  EndIf
EndFunction

//////////////////////////////////////////////////////////////////
// CheckSkillLevelForSkill - Sucht nach Skill gemaess SkillLevel
//////////////////////////////////////////////////////////////////

Function CheckSkillLevelForSkill(who, skill, skilllevel)

  If (!skill) // Schon einen Skill gefunden?
    If (HasPlayerSkill(who, SKILLID_MAGIE) == skilllevel)
      skill := SKILLID_MAGIE;
    EndIf

    If (!skill) // Schon einen Skill gefunden?
      If (HasPlayerSkill(who, SKILLID_SEGNEN) == skilllevel)
        skill := SKILLID_SEGNEN;
      EndIf

      If (!skill) // Schon einen Skill gefunden?
        If (HasPlayerSkill(who, SKILLID_FLUCHEN) == skilllevel)
          skill := SKILLID_FLUCHEN;
        EndIf
      EndIf
    EndIf
  EndIf

  return (skill);
EndFunction

// Darf man zwischen Quelle und Ziel teleportieren oder ein Tor errichten?
Function CheckLocations(caster, from, dest)
  // Götter können überall teleportieren
  If (GetObjProperty(caster, I_AM_GOD))
    return (RUNE_SUCCESS);
  EndIf

  // Nur Götter dürfen zwischen den Realms wechseln
  If (dest.realm <> caster.realm)
    return(RUNE_CANCEL_NOESCAPE);
  EndIf

  // Darf man sich aus dieser Region herausteleportieren?
  If (PlaceNoEscape(caster))
    return (RUNE_CANCEL_NOESCAPE);
  EndIf

  // bei Dummy ist from.x == 0
  If (from.x)
    // Darf man aus dieser Region heraus ein Tor errichten?
    If (PlaceNoRune(from))
      return (RUNE_CANCEL_NORUNE);
    EndIf
  EndIf

  // Darf man sich in diese Region teleportieren oder ein Tor errichten?
  If (PlaceNoRune(dest))
    return (RUNE_CANCEL_NORUNE);
  Else // Checken das man nicht in fremden Häusern landet
    var house := GetHouseContainingObject(dest);
    If (house and !IsHouseOwnerOrFriend(house, caster));
      return (RUNE_CANCEL_NORUNE);
    EndIf
  EndIf

  return (RUNE_SUCCESS);
EndFunction

////////////////////////////////////////////////////////////////////////////////////
// LogonCharFixRune - Korrigiert ggf. fehlerhafte Einstellungen nach dem Einloggen
////////////////////////////////////////////////////////////////////////////////////

Function LogonCharFixRune(who)
  var oldgraphic := GetObjProperty(who, "recallgraphic");
  If (oldgraphic)
    who.graphic := oldgraphic;
    who.frozen := 0;
    IncRevision(who);
    If (who.hidden) // Um unnoetige Events zu sparen...
      who.hidden := 0;
    EndIf
    EraseObjProperty(who, "recallgraphic");
  EndIf
EndFunction

// vim: sw=2 sts=2
