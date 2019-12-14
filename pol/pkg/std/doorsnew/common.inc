include "include/objtype";
include "include/storage";

// Allocate a lock ID
// Locks are allocated a new lockid when they are created or changed.
// If a lock's lockid is changed, all old keys will no longer work.
Function AllocLockId() // {{{

  var crit := Is_Critical();
  Set_Critical(1);

  var lockid := CInt(GetGlobalProperty("nextlockid"));
  If (!lockid)
    lockid := 1;
  EndIf
  SetGlobalProperty("nextlockid", lockid + 1);

  Set_Critical(crit);

  return lockid;
EndFunction // }}}

// CheckForKey - Ist ein passender Schluessel vorhanden?
Function HasKeyFor(who, thing) // {{{
  If (!thing.locked)
    // wird manchmal beim GM-Aufschliessen vergessen zu loeschen
    thing.eraseprop("gmlocked");
    return 1;
  EndIf

  // Spieler brauchen einen Schluessel, CMDLevel nicht
  If (who.cmdlevel >= CMDLEVEL_SEER)
    PrintTextAbovePrivatePergon(thing,
      "Fuer CMDLevel wird kurz geoeffnet ...", "", who
    );
    return 1;
  EndIf

  If (thing.getprop("gmlocked"))
    PrintTextAbovePrivatePergon(thing,
      "Die Goetter verbieten den Zutritt.", "", who
    );
    return 0;
  EndIf

  var lockid := thing.getprop("lockid");
  If (!lockid)
    // kein Schloss dran, aber gelockt -> wird nix
    return 0;
  EndIf

  // Backpack nach Schluessel durchsuchen
  If (FindKeyIDs(who, lockid))
    PrintTextAbovePrivatePergon(thing,
      "Ihr oeffnet kurz, schliesst aber sofort wieder ab.", "", who
    );
    return 1;
  EndIf

  // alles fehlgeschlagen
  return 0;
EndFunction // }}}

// liefert ein Array aller Key-IDs von who oder
// sucht mit optionalem Parameter nach bestimmter Key-ID
Function FindKeyIDs(who, findkeyid := "") // {{{
  var allkeys := {};

  // alle Schluessel durchgehen
  var keytype;
  For (keytype := UOBJ_KEY_START; keytype <= UOBJ_KEY_END; keytype += 1)
    ForEach key in (FindSubstance(
        who.backpack, keytype, 1, 0, FINDSUBSTANCE_FIND_ALL
    ))
      If (key.objtype == UOBJ_KEY_RING)
        // Spezialfall Schluesselring
        ForEach key in (EnumerateItemsInContainer(GetBoxInStorageArea(
            // Container in Storage-Area untersuchen
            STORAGE_KEYRINGS, ST_PREF_KEYRINGS+Hex(key.serial))
        ))
          var lockid := CInt(key.getprop("lockid"));
          If (lockid)
            // kein Schluesselrohling
            allkeys.append(lockid);

            If (findkeyid == lockid)
              // Key-ID gefunden, fertig
              return 1;
            EndIf
          EndIf
          SleepMS(2);
        EndForEach

      Else
        // normaler Schluessel
        var lockid := CInt(key.getprop("lockid"));
        If (lockid)
          // kein Schluesselrohling
          allkeys.append(lockid);

          If (findkeyid == lockid)
            // Key-ID gefunden, fertig
            return 1;
          EndIf
        EndIf
      EndIf
    EndForEach
    SleepMS(2);
  EndFor

  If (TypeOfInt(findkeyid) == OT_LONG)
    // Key-ID-Modus und Key-ID nicht gefunden
    return 0;
  EndIf

  // Liste zurueckgeben im Array-Modus
  return allkeys;
EndFunction // }}}

// ist das Objekt ein Schluessel
Function IsAKey(obj) // {{{
  If (obj.objtype == UOBJ_KEY_RING)
    // Schluesselringe liegen im folgenden Bereich,
    // sind aber keine Schluessel -> daher vorher abfangen
    return 0;
  EndIf

  If (obj.objtype >= UOBJ_KEY_START and obj.objtype <= UOBJ_KEY_END)
    return 1;
  EndIf

  return 0;
EndFunction // }}}

// vim: sw=2 sts=2
