//////////////////////////////////////////////////////////////////////////
// Funktionen die sowohl von den Faehren als auch von den Booten
// benutzt werden:

// Konstanten
Const STATE_STATIONARY   := 0;
Const STATE_MOVING       := 1;
Const STATE_DRIFTING     := 2;
Const DELAY_DRIFTING     := 15000;

Const PROP_DRYDROCK      := "AlreadyBuild";

// Hier wird jedem erstmal ein wenig Schaden zugefuegt, sollte zum Beispiel
// das Schiff einmal auf Grund laufen oder irgendwo anrempeln
Function SmackEveryone(boat) // {{{
  ForEach mob in (boat.mobiles)
    If (!mob.dead)
      ApplyDamagePergon(mob, RandomInt(10));
      PerformAction(mob, ANIM_HIT);
      PlaySoundEffect(mob, SFX_110);
    EndIf
  EndForEach
EndFunction // }}}

//  Spielt ein wenig Sound ab, so wird es multimedial *g*
Function PlayBoatSounds(boat) // {{{
  If (RandomInt(2))
    PlaySoundEffect(boat, SFX_13);
  Else
    PlaySoundEffect(boat, SFX_14);
  EndIf
EndFunction // }}}

//  Loescht das MultiItem Boot oder gibt eine Fehlermeldung zurueck
Function DestroyBoat(who, boat) // {{{
  Var res := DestroyMulti(boat);
  If (res)
    return 1;
  EndIf
  SendSysMessagePergon(
      who, "Loeschen nicht erfolgreich: " + res.errortext,
      "Can't delete: " + res.errortext
  );
  return 0;
EndFunction // }}}

// GetPortGold - Hoehe der Hafenkasse des Ortes abfragen
Function GetPortGold(town) // {{{
  Var i;
  // speichert die Goldbetrage aller Staedte global
  Var PortGoldProp   := {};
  // beinhaltet den Goldbetrag einer Stadt;
  // Syntax: {'Stadtname', 'Hafenkassengold'}
  Var TownGoldProp   := {"", 0};

  PortGoldProp := GetGlobalProperty("PortGold");

  If (!PortGoldProp)
    Return(0);
  Else
    For (i := 1; i <= len(PortGoldProp); i += 1)
      If (PortGoldProp[i][1] == town)
        // Eintrag mit korrekter Stadt gefunden
        TownGoldProp := PortGoldProp[i];
      EndIf
    EndFor
  EndIf
  Return(TownGoldProp[2]);
EndFunction // }}}

// SetPortGold - Gold der Hafenkasse hinzufuegen
Function SetPortGold(town, amount) // {{{
  Var i, goldupdate := 0;
  // speichert die Goldbetrage aller Staedte global
  Var PortGoldProp     := {};
  Var PortGoldProp_new := {};

  Var old_critical:=Is_Critical();
  set_critical(1); // Sonst wohl Probs bei gleichzeitigem Aufruf
  PortGoldProp := GetGlobalProperty("PortGold");

  If (!PortGoldProp)
    PortGoldProp := {};
  EndIf

  // Globales CProp-Array durchgehen und nach Stadteintrag suchen, alten
  // Goldbetrag durch neuen ersetzen; gleichzeitig Array neu aufbauen
  For (i := 1; i <= len(PortGoldProp); i += 1)
    If (PortGoldProp[i][1] == town)
      // Eintrag mit korrekter Stadt gefunden: Goldbetrag aendern
      PortGoldProp_new.append({PortGoldProp[i][1], amount});
      // Indiz, dass ein Stadteintrag gefunden und geupdatet wurde
      goldupdate := 1;
    Else
      // keine korrekte Stadt: alten Eintrag uebernehmen
      PortGoldProp_new.append(PortGoldProp[i]);
    EndIf
  EndFor

  If (!goldupdate)
    // korrekte Stadt war nicht in Liste enthalten, also neuen Eintrag anbauen
    PortGoldProp_new.append({town, amount});
  EndIf

  SetGlobalProperty("PortGold", PortGoldProp_new);
  set_critical(old_critical);
EndFunction // }}}

// Setzt die Propertys des Bootes sowohl scriptintern als auch als CProp,
// um Inkonstistenzen zu vermeiden
Function SetShipStats(stat, value) // {{{
  If (stat == "hp")
    hp := value;
    SetObjProperty(boat, "hp", value);
  ElseIf (stat == "maxhp")
    maxhp := value;
    SetObjProperty(boat, "maxhp", value);
  ElseIf (stat == "state")
    state := value;
    SetObjProperty(boat, "state", value);
  ElseIf (stat == "BoatMobiles")
//    BoatMobiles := value; // wird nicht als interne globale Variable verwendet
    SetObjProperty(boat, "BoatMobiles", value);
  ElseIf (stat == "Material")
//    Material := value; // wird nicht als interne globale Variable verwendet
    SetObjProperty(boat, "Material", value);
  ElseIf (stat == "PortOfRegistry")
//    PortOfRegistry := value; // wird nicht als interne globale Variable verwendet
    SetObjProperty(boat, "PortOfRegistry", value);
  ElseIf (stat == "anchor")
//    anchor := value; // wird nicht als interne globale Variable verwendet
    SetObjProperty(boat, "anchor", value);
  ElseIf (stat == "ist_faehre")
//    ist_faehre := value; // wird nicht als interne globale Variable verwendet
    SetObjProperty(boat, "ist_faehre", value);
  ElseIf (stat == "shiptype")
//    shiptype := value; // wird nicht als interne globale Variable verwendet
    SetObjProperty(boat, "shiptype", value);
  ElseIf (stat == "Zaehler")
    route_index := value;
    SetObjProperty(boat, "Zaehler", value);
  Else
    Syslog("WARNUNG: Unbekannte stat-Variable '" + stat + "' mit Wert '" + value + "' erhalten.");
  EndIf
EndFunction // }}}

// BoatReg - Registriert alle Chars an Deck um vollen ZugrIff auf
// OfflineChars zu haben
Function BoatReg(boat) // {{{
  Var boatmobiles := {}, boatmobilesold := {}, mob;
  boatmobilesold  := GetObjProperty(boat, "BoatMobiles");

  If (boatmobilesold)
    ForEach mobile in boatmobilesold
      mob := SystemFindObjectBySerial( mobile,SYSFIND_SEARCH_OFFLINE_MOBILES);
      If ((mob.multi.serial == boat.serial) and (!(mob.serial in boatmobiles)))
        boatmobiles.append(mob.serial);
      EndIf
    EndForEach
  EndIf
  ForEach mobile in (boat.mobiles)
    If (!(mobile.serial in boatmobiles))
      boatmobiles.append(mobile.serial);
    EndIf
  EndForEach
  SetObjProperty(boat, "BoatMobiles", boatmobiles);
  // SetShipStats("BoatMobiles", boatmobiles);
EndFunction // }}}

// DryDock setzt das Schiff ins Backpack
Function DryDock(boat, who) // {{{1
  If (
    // falls who (Quelle) nicht dem Eigentuemer und
    (boat.tillerman.getprop("owner") <> who.serial) and
    // auch nicht GM ist
    (who.cmdlevel < CMDLEVEL_HIGHGM)
  )
    // Abbruch
    return;
  EndIf

  If (!SendYesNoGump(who,
      "Trockendock?<br>Verlasst das Boot, bevor Ihr bestaetigt!"
  ))
    // Nur wenn bestaetigt wird, andernfalls wird abgebrochen
    return;
  EndIf

  // GM wird von Bord geschafft,
  // Rest muss Tillerman von draussen steuern
  If (who.cmdlevel >= CMDLEVEL_HIGHGM)
    MoveObjectToLocation(
      who, boat.x+5, boat.y+5, boat.z, boat.realm,
      MOVEOBJECT_FORCELOCATION
    );
  EndIf

  // Schiffstyp wird zurueckgegeben
  var shiptype := GetObjProperty(boat, "shiptype");
  // Erschafft das neue Item im Backpack (Schiff ohne Grafik)
  var newboat := CreateItemInContainerPergon(who.backpack, shiptype, 1);
  If (!newboat)
    // Da war wohl das Backpack voll
    SendSysMessagePergon(who,
      "Euer Rucksack ist voll!", "Your backpack is full!"
    );
    // Abbruch
    return;
  EndIf

  // Grafik fuer das neue Item im Backpack, also ein Spielzeugboot
  newboat.graphic := UOBJ_SHIP_MODEL1;

  // Gibt dem Boot den neuen Namen, bzw. den Namen vom Bootsmann
  If (boat.tillerman.name)
    newboat.name := boat.tillerman.name;
  Else
    newboat.name := "Spielzeugboot";
  EndIf

  // Bewegt alle Items auf dem Boot und setzt sie danach wieder in den
  // Ursprungszustand zurueck
  ForEach item in (boat.items)
    MoveObjectToLocation(
      item, who.x, who.y, who.z, who.realm,
      MOVEOBJECT_FORCELOCATION+MOVEITEM_IGNOREMOVABLE
    );
    SleepMs(5);
  EndForEach

  // Dasselbe fürs Hold
  ForEach item in EnumerateItemsInContainer(boat.hold, ENUMERATE_ROOT_ONLY)
    MoveObjectToLocation(
      item, who.x, who.y, who.z, who.realm,
      MOVEOBJECT_FORCELOCATION+MOVEITEM_IGNOREMOVABLE
    );
    SleepMs(5);
  EndForEach

  // Offline Chars verschieben
  BoatReg(boat);
  TransportChar(boat);

  // Propertys übergeben
  newboat.setprop("lockid",         boat.hold.getprop("lockid"));
  newboat.setprop("maxhp",          boat.getprop("maxhp"));
  newboat.setprop("hp",             boat.getprop("hp"));
  newboat.setprop("Skillreq",       boat.getprop("Skillreq"));
  newboat.setprop("PortOfRegistry", boat.getprop("PortOfRegistry"));
  newboat.setprop("Material",       boat.getprop("Material"));
  newboat.setprop("HoldName",       boat.hold.name);
  newboat.setprop(PROP_DRYDROCK,    1);

  // var key := FindKey(who, boat.portplank);

  // Sollte es nicht klappen ein existierendes Boot zu zerstoeren, wird
  // das Item im Backpack wieder zerstoert
  If (!DestroyBoat(who, boat))
    DestroyItem(newboat);
  // Else
  //   // Sollte aber ein Schluessel existieren wird zumindestens der
  //   // Schluessel im Backpack zerstoert
  //   If (key)
  //     DestroyItem(key);
  //   EndIf
  EndIf
EndFunction // }}}1

// Transportiert einen Player zu einem ortsnahen Beampunkt
// (gedacht fuer das Beamen ans Dock im Hafen)
Function TransportChar(boat, who := 0) // {{{1
  // Suche nach Beampunkt mit kuerzester Distanz
  var configfile := ReadConfigFile(":boat:faehrrouten");
  var element    := FindConfigElem(configfile, "Beampunkte");
  var beampunkte := GetConfigStringArray(element, "Punkt");

  var x, y, z;
  // maximale Kartengroesse als Vorgabedistanz
  var MAX_DISTANCE    := WORLD_X * WORLD_Y;
  var last_distance   := MAX_DISTANCE;
  ForEach punkt in (beampunkte)
    // Distanz zwischen Beampunkt und Boot bestimmen
    var ziel := SplitWords(punkt);
    var pdist := CoordinateDistance(
      boat.x, boat.y, CInt(ziel[1]), CInt(ziel[2])
    );
    If (pdist < last_distance)
      // wenn dichter dran, Werte uebernehmen
      last_distance := pdist;
      x := CInt(ziel[1]);
      y := CInt(ziel[2]);
      z := CInt(ziel[3]);
    EndIf
  EndForEach

  // keinen Punkt gefunden?
  If (last_distance == MAX_DISTANCE)
    SysLog(
      "WARNUNG: In Config 'faehrrouten' fehlt mindestens ein Beampunkt!"
    );
    // Fallback aktivieren
    x := 3011;
    y :=  834;
    z :=   -2;
  EndIf

  // Den Character jetzt noch da hinbewegen und gut ist
  If (who)
    MoveObjectToLocation(
      who, x, y, z, boat.realm, MOVEOBJECT_FORCELOCATION
    );
    return;
  EndIf
  // falls kein Charakter angegeben wurde, Offline-Passagiere wegtun
  If (boat.has_offline_mobiles)
    boat.move_offline_mobiles(x, y, z);
  EndIf
EndFunction // }}}1

// vim: sw=2 sts=2
