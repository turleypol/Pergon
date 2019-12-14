///////////////////////////////////////////////////////////////////////////
// Place
//
// Informationen zu Orten, Behandlung der Sprungliste

// Place
//  PlaceInfos         - In welchem Bereich befindet sich (x, y) ?
//  PlaceRanges        - Wie ist die Region genau aufgebaut?
//  PlaceNames         - Liefert alle Ortsnamen
//  PlaceName          - Wie heisst die Oertlichkeit?
//  PlaceGuarded       - Wird die Zone beschuetzt?
//  PlaceNoCast        - Ist hier Zaubern erlaubt?
//  PlaceNoRune        - Duerfen hier Runen gesetzt werden und darf man
//                       sich hinteleportieren?
//  PlaceNoEscape      - Darf man sich von hier wegteleportieren?
//  PlaceNoHolyWarrior - Darf der Heilige Krieger benutzt werden?
//  PlaceNoHouse       - Ist hier Bauen von PlayerHaeusern erlaubt?
//  PlaceNoLight       - Darf man sich hier Licht zaubern?
//  PlaceTown          - Ist hier eine Siedlung?
//  PlaceEvil          - Ist es eine moerderfreundliche Region?
//  PlaceNewbie        - Ist es eine Newbie-Region?
//
// Light
//  LightInfos    - Wie ist das Lichtlevel bei (x, y) ?
//  DayLightZones - Liefert alle Regionen, die vom Tageslicht beeinflusst
//                  werden
//
// Sprungliste
//  JumpLocationAdd - Position zur Liste hinzufuegen
//  JumpLocationGet - Position aus Liste holen
//
// Realms
//  CheckCoords     - Prueft, ob Koordinaten in Realm liegen


include "include/server";

///////////////////////////////////////////////////////////
// PlaceInfos - In welchem Bereich befindet sich (x, y) ?
///////////////////////////////////////////////////////////
Function PlaceInfos(object, GETALL := 1)
  var result := struct;
  result.+realm;
  result.+region := "Background";
  result.+name := "Sonstwo";
  result.+x1 := 0;
  result.+y1 := 0;
  result.+x2 := 6143;
  result.+y2 := 4095;
  result.+guarded := 0;
  result.+nocast := 0;
  result.+norune := 0;
  result.+noescape := 0;
  result.+noholywarrior := 0;
  result.+nohouse := 0;
  result.+nolight := 0;
  result.+town := 0;
  result.+evilzone := 0;
  result.+newbie := 0;
  result.+noskill := 0;

  var regionsfile := ReadConfigFile("::../regions/regions");
  If (regionsfile)
    var regionname;
    If ((object.isA(POLCLASS_MOBILE)) && (!object.isA(POLCLASS_NPC)))
      regionname := GetRegionName(object);
    Else
      regionname := GetRegionNameAtLocation(object.x, object.y, object.realm);
    EndIf
    var region := regionsfile[regionname];
    If (region)
      If (GETALL)
        var ranges := GetConfigStringArray(region, "Range");
        If (ranges)
          var j := 1;
          While (j <= Len(ranges))
            var range := SplitWords(ranges[j]);
            If ((CInt(range[1]) <= object.x) And (object.x <= CInt(range[3])) And (CInt(range[2]) <= object.y) And (object.y <= CInt(range[4])))
              result.x1 := CInt(range[1]);
              result.y1 := CInt(range[2]);
              result.x2 := CInt(range[3]);
              result.y2 := CInt(range[4]);
              Break;
            EndIf
            j += 1;
          EndWhile
        EndIf
      EndIf
      If ((region.name == "QuestZone") && (GetGlobalProperty("questzonename")))
        result.name := GetGlobalProperty("questzonename");
      Else
        result.name := region.name;
      EndIf
      result.region        := regionname;
      result.realm         := object.realm;
      result.guarded       := CInt(region.Guarded);
      result.nocast        := CInt(region.nocast);        // Benutzt
      result.notame        := CInt(region.notame);
      result.norune        := CInt(region.norune);        // Benutzt
      result.noescape      := CInt(region.noescape);      // Benutzt
      result.noholywarrior := CInt(region.noholywarrior); // Benutzt
      result.nohouse       := CInt(region.nohouse);       // Benutzt
      result.nolight       := CInt(region.nolight);       // Benutzt
      result.town          := CInt(region.town);
      result.evilzone      := CInt(region.evilzone);
      result.newbie        := CInt(region.newbie);
      result.noskill       := CInt(region.noskill);
    EndIf
    //UnloadConfigFile("::../regions/regions");
  Else
    syslog("FEHLER: PlaceInfos: Kann die Datei 'regions/regions.cfg' nicht oeffnen!");
  EndIf
  return (result);
EndFunction

//////////////////////////////////////////////////////
// PlaceRanges - Wie ist die Region genau aufgebaut?
//////////////////////////////////////////////////////
Function PlaceRanges(object)
  var result := error{errortext := "Region not found"};

  var regionsfile := ReadConfigFile("::../regions/regions");
  If (!regionsfile)
    syslog(
      "FEHLER: PlaceRanges: Kann die Datei 'regions/regions.cfg' "+
      "nicht oeffnen: "+regionsfile.errortext
    );
    return result;
  EndIf

  // var regionsbonusfile := ReadConfigFile("::regions");
  // If (regionsbonusfile)
  var placeinfo := PlaceInfos(object, 0);

  // var region;
  // If (placeinfo.bonusfile)
  //   region := FindConfigElem(regionsbonusfile, placeinfo.region);
  // Else
  var region := FindConfigElem(regionsfile, placeinfo.region);
  // EndIf
  If (!region)
    syslog(
      "FEHLER: PlaceRanges: Die Region '"+placeinfo.region+
      "' ["+placeinfo.bonusfile+"] konnte nicht gefunden werden: "+
      region.errortext
    );
    return result;
  EndIf

  result := GetConfigStringArray(region, "Range");

  //   UnloadConfigFile("::regions");
  // Else
  //   syslog(
  //     "FEHLER: PlaceRanges: Kann die Datei 'config/regions.cfg' "+
  //     "nicht oeffnen!"
  //   );
  // EndIf

  // UnloadConfigFile("::../regions/regions");

  return result;
EndFunction

////////////////////////////////////////
// PlaceNames - Liefert alle Ortsnamen
////////////////////////////////////////
Function PlaceNames(realmname)
  var regionnames := {};
  var regionsfile := ReadConfigFile("::../regions/regions");

  If (regionsfile)
    var regionsbonusfile := ReadConfigFile("::regions");
    If (regionsbonusfile)
      realmname[1] := Upper(realmname[1]);

      var realminfo := FindConfigElem(regionsbonusfile, realmname+"_World");
      If (realminfo)
        var region;

        ForEach regioninfo in GetConfigStringArray(realminfo, "Region")
          regioninfo := SplitWords(regioninfo);
//          If (regioninfo[2] == "Bonus")
//            region := FindConfigElem(regionsbonusfile, regioninfo[1]);
//          Else
            region := FindConfigElem(regionsfile, regioninfo[1]);
//          EndIf

          If (region)
            var name := region.name;
            If (name)
              regionnames.append(name);
            EndIf
          Else
            syslog("FEHLER: PlaceNames: Die Region '"+regioninfo[1]+"' konnte nicht gefunden werden!");
          EndIf
        EndForEach
      EndIf

      UnloadConfigFile("::regions");
    Else
      syslog("FEHLER: PlaceNames: Kann die Datei 'config/regions.cfg' nicht oeffnen!");
    EndIf

    //UnloadConfigFile("::../regions/regions");
  Else
    syslog("FEHLER: PlaceNames: Kann die Datei 'regions/regions.cfg' nicht oeffnen!");
  EndIf

  return (regionnames);
EndFunction

Function GetRegionEntry(object)
  var regionsfile := ReadConfigFile("::../regions/regions");
  If (regionsfile)
    var regionname;
    If ((object.isA(POLCLASS_MOBILE)) && (!object.isA(POLCLASS_NPC)))
      regionname := GetRegionName(object);
    Else
      regionname := GetRegionNameAtLocation(object.x, object.y, object.realm);
    EndIf
    If (regionname)
      Return (regionsfile[regionname]);
    EndIf
  EndIf
  Return 0;
EndFunction

/////////////////////////////////////////////
// PlaceName - Wie heisst die Oertlichkeit?
/////////////////////////////////////////////
Function PlaceName(object)
  Var region := GetRegionEntry(object);
  If (region)
    If ((region.name == "QuestZone") && (GetGlobalProperty("questzonename")))
      Return (GetGlobalProperty("questzonename"));
    Else
      Return (region.name);
    EndIf
  Endif
  Return ("Sonstwo");
EndFunction

/////////////////////////////////////////////
// PlaceGuarded - Wird die Zone beschuetzt?
/////////////////////////////////////////////
Function PlaceGuarded(object)
  return (PlaceInfos(object, 0).guarded);
EndFunction

////////////////////////////////////////////
// PlaceNoCast - Ist hier Zaubern erlaubt?
////////////////////////////////////////////
Function PlaceNoCast(object)
  return (PlaceInfos(object, 0).nocast);
EndFunction

////////////////////////////////////////////
// PlaceNoTame - Zaehmen/Abrichten erlaubt?
////////////////////////////////////////////
Function PlaceNoTame(object)
  return PlaceInfos(object, 0).notame;
EndFunction

////////////////////////////////////////////////////////////////////////////////////////
// PlaceNoRune - Duerfen hier Runen gesetzt werden und darf man sich hinteleportieren?
////////////////////////////////////////////////////////////////////////////////////////
Function PlaceNoRune(object)
  // ist Beamblocker in der Naehe, der Hinbeamen, Markieren und Torstellen
  // verhindert?
  var blockers := ListItemsNearLocationOfType(
    object.x, object.y, LIST_IGNORE_Z, 20, UOBJ_BEAMBLOCKER, object.realm
  );
  ForEach blocker in (blockers)
    If (
      CoordinateDistance(object.x, object.y, blocker.x, blocker.y) >
      blocker.getprop("Range")
    )
      // Reichweite des Blockers nicht hoch genug
      continue;
    EndIf
    If (!blocker.getprop("NoRune"))
      // nicht aktiviert
      continue;
    EndIf
    If (blocker.getprop("ThisFloorOnly"))
      // auf bestimmte Etage beschraenkt
      If (blocker.z - 15 > object.z)
        // Objekt ist weiter unten
        continue;
      EndIf
      If (blocker.z + 15 < object.z)
        // Objekt ist weiter oben
        continue;
      EndIf
    EndIf

    return 1;
  EndForEach

  // Boote sind nicht erlaubt
  If ((object.isa(POLCLASS_BOAT)) or (object.multi.isa(POLCLASS_BOAT)))
    return 1;
  EndIf

  return (PlaceInfos(object, 0).norune);
EndFunction

/////////////////////////////////////////////////////////////
// PlaceNoEscape - Darf man sich von hier wegteleportieren?
/////////////////////////////////////////////////////////////
Function PlaceNoEscape(object)
  // ist Beamblocker in der Naehe, der Wegbeamen verhindert?
  var blockers := ListItemsNearLocationOfType(
    object.x, object.y, LIST_IGNORE_Z, 20, UOBJ_BEAMBLOCKER, object.realm
  );
  ForEach blocker in (blockers)
    If (
      CoordinateDistance(object.x, object.y, blocker.x, blocker.y) >
      blocker.getprop("Range")
    )
      // Reichweite des Blockers nicht hoch genug
      continue;
    EndIf
    If (!blocker.getprop("NoEscape"))
      // nicht aktiviert
      continue;
    EndIf
    If (blocker.getprop("ThisFloorOnly"))
      // auf bestimmte Etage beschraenkt
      If (blocker.z - 15 > object.z)
        // Objekt ist weiter unten
        continue;
      EndIf
      If (blocker.z + 15 < object.z)
        // Objekt ist weiter oben
        continue;
      EndIf
    EndIf

    return 1;
  EndForEach

  // Boote sind nicht erlaubt
  If ((object.isa(POLCLASS_BOAT)) or (object.multi.isa(POLCLASS_BOAT)))
    return 1;
  EndIf

  return (PlaceInfos(object, 0).noescape);
EndFunction

//////////////////////////////////////////////////////////////////
// PlaceNoHolyWarrior - Darf der Heilige Krieger benutzt werden?
//////////////////////////////////////////////////////////////////
Function PlaceNoHolyWarrior(object)
  return (PlaceInfos(object, 0).holywarrior);
EndFunction

//////////////////////////////////////////////////////////////
// PlaceNoHouse - Ist hier Bauen von PlayerHaeusern erlaubt?
//////////////////////////////////////////////////////////////
Function PlaceNoHouse(object)
  return (PlaceInfos(object, 0).nohouse);
EndFunction

/////////////////////////////////////////////////////
// PlaceNoLight - Darf man sich hier Licht zaubern?
/////////////////////////////////////////////////////
Function PlaceNoLight(object)
  return (PlaceInfos(object, 0).nolight);
EndFunction

////////////////////////////////////////
// PlaceTown - Ist hier eine Siedlung?
////////////////////////////////////////
Function PlaceTown(object)
  return (PlaceInfos(object, 0).town);
EndFunction

///////////////////////////////////////////////////////
// PlaceEvil - Ist es eine moerderfreundliche Region?
///////////////////////////////////////////////////////
Function PlaceEvil(object)
  return (PlaceInfos(object, 0).evilzone);
EndFunction

/////////////////////////////////////////////
// PlaceNewbie - Ist es eine Newbie-Region?
/////////////////////////////////////////////
Function PlaceNewbie(object)
  return (PlaceInfos(object, 0).newbie);
EndFunction

/////////////////////////////////////////////
// PlaceNoSkill - Darf man Rawpoints bekommen
/////////////////////////////////////////////
Function PlaceNoSkill(object)
  return (PlaceInfos(object, 0).noskill);
EndFunction

///////////
// Light //
///////////

/////////////////////////////////////////////////////
// LightInfos - Wie ist das Lichtlevel bei (x, y) ?
/////////////////////////////////////////////////////
Function LightInfos(object)
  var result := struct;
  result.+realm;
  result.+region := "Background";
  result.+name := "Sonstwo";
  result.+x1 := 0;
  result.+y1 := 0;
  result.+x2 := 6143;
  result.+y2 := 4095;
  result.+lightlevel := 0;
  result.+daylightzone := 0;

  var lightfile := ReadConfigFile("::../regions/light");
  If (lightfile)
    var regionsbonusfile := ReadConfigFile("::regions");
    If (regionsbonusfile)
      var x := object.x;
      var y := object.y;
      var realmname := object.realm;
      result.realm := realmname;
      realmname[1] := Upper(realmname[1]);

      var realminfo := FindConfigElem(regionsbonusfile, realmname+"_Light");
      If (realminfo)
        var regionen := GetConfigStringArray(realminfo, "Region");
        If (regionen)
          var i := 1;
          While (i <= Len(regionen))
            var region := FindConfigElem(lightfile, regionen[i]);
            If (region)
              var ranges := GetConfigStringArray(region, "Range");
              If (ranges)
                var j := 1;
                While (j <= Len(ranges))
                  var range := SplitWords(ranges[j]);
                  If ((CInt(range[1]) <= x) And (x <= CInt(range[3])) And (CInt(range[2]) <= y) And (y <= CInt(range[4])))
                    result.region := regionen[i];
                    result.name := region.name;
                    result.x1 := CInt(range[1]);
                    result.y1 := CInt(range[2]);
                    result.x2 := CInt(range[3]);
                    result.y2 := CInt(range[4]);
                    result.lightlevel := region.lightlevel;
                    result.daylightzone := region.daylightzone;
                    j := Len(ranges);i := Len(regionen);
                  EndIf
                  j += 1;
                EndWhile
              EndIf
            Else
              syslog("FEHLER: LightInfos: Die Region '"+regionen[i]+"' konnte nicht gefunden werden!");
            EndIf
            i += 1;
          EndWhile
        EndIf
      EndIf

      UnloadConfigFile("::regions");
    Else
      syslog("FEHLER: LightInfos: Kann die Datei 'config/regions.cfg' nicht oeffnen!");
    EndIf

    UnloadConfigFile("::../regions/light");
  Else
    syslog("FEHLER: LightInfos: Kann die Datei 'regions/light.cfg' nicht oeffnen!");
  EndIf

  return (result);
EndFunction

///////////////////////////////////////////////////////////////////////////
// DayLightZones - Liefert alle Regionen, die vom Tageslicht beeinflusst werden
///////////////////////////////////////////////////////////////////////////
Function DayLightZones()
  var zones := {};

  var lightfile := ReadConfigFile("::../regions/light");
  If (lightfile)
    var regionsbonusfile := ReadConfigFile("::regions");
    If (regionsbonusfile)
      ForEach realmname in {REALM_BRITANNIA, REALM_ILSHENAR, REALM_MALAS, REALM_TOKUNO}
        realmname[1] := Upper(realmname[1]);

        var realminfo := FindConfigElem(regionsbonusfile, realmname+"_Light");
        If (realminfo)
          ForEach regioninfo in GetConfigStringArray(realminfo, "Region")
            var region := FindConfigElem(lightfile, regioninfo);
            If (region)
              If (region.daylightzone)
                zones.append(regioninfo);
              EndIf
            Else
              syslog("FEHLER: LightInfos: Die Region '"+regioninfo+"' konnte nicht gefunden werden!");
            EndIf
          EndForEach
        EndIf
      EndForEach

      UnloadConfigFile("::regions");
    Else
      syslog("FEHLER: LightInfos: Kann die Datei 'config/regions.cfg' nicht oeffnen!");
    EndIf

    UnloadConfigFile("::../regions/light");
  Else
    syslog("FEHLER: LightInfos: Kann die Datei 'regions/light.cfg' nicht oeffnen!");
  EndIf

  return (zones);
EndFunction

Const MAX_LOCATION_COUNT := 5;
Const JUMP_LOCATION_PRESERVE := 0;
Const JUMP_LOCATION_ERASE    := 1;
// Fuegt angegebene oder aktuelle Position in die Sprungliste ein
Function JumpLocationAdd(who, location := 0) // {{{
  If (!location)
    location := who;
  EndIf

  var newloc := struct{
    x     := location.x,
    y     := location.y,
    z     := location.z,
    realm := location.realm
  };

  var crit := Is_Critical();
  Set_Critical(1);

  var list := who.getprop(PROP_LOC_LIST);
  // notfalls erstellen
  If (!list)
    list := array{};
  EndIf
  // gegebenenfalls hinten kuerzen
  If (list.size() > MAX_LOCATION_COUNT)
    list.shrink(MAX_LOCATION_COUNT);
  EndIf

  // immer vorn anfuegen
  list.insert(1, newloc);

  who.setprop(PROP_LOC_LIST, list);

  Set_Critical(crit);
EndFunction // }}}

// holt letzte Position aus der Sprungliste, loescht sie nach Bedarf
Function JumpLocationGet(who, erase := JUMP_LOCATION_ERASE) // {{{
  var crit := Is_Critical();
  If (erase)
    // nur beim Loeschen wird CProp wieder geschrieben
    Set_Critical(1);
  EndIf

  var list := who.getprop(PROP_LOC_LIST);
  If (!list)
    // bisher keine Ziele
    Set_Critical(crit);
    return error{errortext := "Jump list is empty"};
  EndIf

  // ersten Eintrag holen
  var loc := list[1];

  If (erase)
    // ersten Eintrag entfernen
    list.erase(1);
    who.setprop(PROP_LOC_LIST, list);

    Set_Critical(crit);
  EndIf

  return loc;
EndFunction // }}}

// prueft, ob Koordinate(n) im Realm gueltig ist/sind
Function CheckCoords(coord, coord2 := 0, realm := 0) // {{{
  If (TypeOfInt(coord) == OT_ARRAY)
    coord := CoordArrayToStruct(coord, realm);
  EndIf

  If (coord2 and TypeOfInt(coord2) == OT_ARRAY)
    coord2 := CoordArrayToStruct(coord2, realm);
  EndIf

  var checklist := array{coord};
  If (coord2)
    checklist.append(coord2);
  EndIf

  ForEach coords in (checklist)
    SleepMs(2);
    // gueltiges Realm?
    var checkrealm := Realms(coords.realm);
    If (!realm)
      return error{errortext := "Invalid realm"};
    EndIf

    If (
      // gueltige Koords
      coords.x < 0 or coords.x > checkrealm.width or
      coords.y < 0 or coords.y > checkrealm.height or
      coords.z < -128 or coords.z > 127
    )
      return error{errortext := "Invalid coordinates for realm"};
    EndIf
  EndForEach

  If (coord2)
    // falls mit mehreren Koordinaten aufgerufen, diese noch sortieren
    If (coord.x > coord2.x)
      SwapValues(coord.x, coord2.x);
    EndIf
    If (coord.y > coord2.y)
      SwapValues(coord.y, coord2.y);
    EndIf
    If (coord.z > coord2.z)
      SwapValues(coord.z, coord2.z);
    EndIf

    return array{coord, coord2};
  EndIf

  return coord;
EndFunction // }}}

// Hilfsfunktion fuer CheckCoords zur Konvertierung von Variablen
Function CoordArrayToStruct(coord, realm) // {{{
  var newcoord := struct;
  newcoord.x := coord[1];
  newcoord.y := coord[2];
  If (coord.size() == 3)
    newcoord.z := coord[3];
  Else
    newcoord.z := 0;
  EndIf
  newcoord.realm := realm;
  return newcoord;
EndFunction // }}}

// kurze Ortsangabe (benutzt bei Pages)
Function ShortLoc(loc) // {{{
  If (!loc.realm)
    loc["realm"] := loc.r;
  EndIf

  If (loc.realm)
    return loc.x+" "+loc.y+" "+loc.z+" "+loc.realm[1,1];
  Else
    return loc.x+" "+loc.y+" "+loc.z;
  EndIf
EndFunction // }}}

// vim: sw=2 sts=2
