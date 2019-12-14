///////////////////////////////////////////////////////////////////////////
// Set - Routinen rund um Sets
//
// MakeSet - Baut anhand der Eintraege in sets.cfg die Items zusammen

use cfgfile;
include "include/itemnpc";

Const MAXAREASIZE := 50;

Enum Cprops
  CPROP_INIT  := "bautoolinit",   // CProps zum Finden der Items
  CPROP_ITEMS := "bautoolitems",
  CPROP_COLOR := "bautoolorigcol" // Originalfarbe für Markedit
EndEnum

/////////////////////////////////////////////////////////////////
// MakeSet    - Erzeugt das Set
/////////////////////////////////////////////////////////////////
// loc
//   entweder als Referenz von TargetCoordinates()
//   oder als Struct-Variable der Form
//      Struct{x := target.x, y:= target.y, z:= target.z}
// name
//   Name des Sets laut sets.cfg
// Rueckgabewert:
//   Serial des ersten erschaffenen Set-Items (wichtig fuer DestroySet())
//
// createkind
// - 0= standard wie vorher.
// - 1 = nur Items
// - 2 = nur mobiles
// - 3 = items und mobiles
Function MakeSet(loc, name, itemreturn := 0, createKind := 0) // {{{
    // Zuerst prüfen, ob das gewünschte Set im DataFile ist.
    // Sonst über cfg versuchen.

    var SetsDataFile := OpenDataFile("::sets");
    If (!SetsDataFile)
        // gibt derzeit kein Datafile
        return MakeSetOld(loc, name, itemreturn, createKind);
    EndIf

    var ListeAllerSets := SetsDataFile.FindElement("allsets");
    If (!ListeAllerSets)
        // Datafile ist leer
        return MakeSetOld(loc, name, itemreturn, createKind);
    EndIf

    var MySet := ListeAllerSets.GetProp(name);
    If (!MySet)
        // Set existiert nicht im Datafile
        return MakeSetOld(loc, name, itemreturn, createKind);
    EndIf

    // Das gewünschte Set existiert im DataFile.
    Case (createKind)
    1:
        return CreateStuffDataFile(name, loc, MySet, "item", itemreturn);
    2:
        return CreateStuffDataFile(name, loc, MySet, "mobile", itemreturn);
    3:
        var ret  := CreateStuffDataFile(name, loc, MySet, "item", itemreturn);
        var retm := CreateStuffDataFile(name, loc, MySet, "mobile", itemreturn);
        If (ret > 0 && retm > 0)
            // Es existieren Mobiles und Items.
            var SetItem   := SystemFindObjectBySerial(ret);
            var SetMobile := SystemFindObjectBySerial(retm);
            // Item- mit Mobile-Kette verbinden
            SetObjProperty(SetItem,   "set_pred", SetMobile.serial);
            SetObjProperty(SetMobile, "set_succ", SetItem.serial);
        EndIf
        // das erste mobile der Item-Mobile-Kette zurück geben.
        If (ret[1] <= 1)
            ret := retm;
        Endif
        return ret;
        break;
    EndCase

    // Verhalten wie vor der Änderung von Taya
    var ret := CreateStuffDataFile(name, loc, MySet, "item", itemreturn);
    If (ret[1] <= 1)
        ret := CreateStuffDataFile(name, loc, MySet, "mobile", itemreturn);
    EndIf
    return ret;
EndFunction // }}}

Function MakeSetOld(loc, name, itemreturn := 0, createKind := 0) // {{{
    // Das gewünschte Set im cfg suchen.
    var SetFile := ReadConfigFile("::sets");
    If (!SetFile)
        syslog(
            "FEHLER: Kann die Datei 'sets.cfg' nicht oeffnen:"+
            SetFile.errortext
        );
        return 0;
    EndIf

    var elem := SetFile[name];
    If (!elem)
        syslog(
            "WARNUNG: Das Set '"+name+"' gibt es nicht in der 'sets.cfg'!"
        );
        return 0;
    EndIf

    Case (createKind)
    1 :
        return CreateStuff(SetFile, name, loc, elem, "item", itemreturn);
    2 :
        return CreateStuff(SetFile, name, loc, elem, "mobile", itemreturn);
    3 :
        var ret  := CreateStuff(SetFile, name, loc, elem, "item", itemreturn);
        var retm := CreateStuff(SetFile, name, loc, elem, "mobile", itemreturn);
        If (ret > 0 && retm > 0)
            // es existieren mobiles und items. Item- mit Mobile-Kette verbinden
            var SetItem   := SystemFindObjectBySerial(ret);
            var SetMobile := SystemFindObjectBySerial(retm);
            SetObjProperty(SetItem,   "set_pred", SetMobile.serial);
            SetObjProperty(SetMobile, "set_succ", SetItem.serial);
        EndIf
        // das erste mobile der Item-Mobile-Kette zurück geben.
        If (ret[1] <= 1)
            ret := retm;
        Endif
        return ret;
    EndCase

    // Verhalten wie vor der Änderung von Taya
    var ret := CreateStuff(SetFile, name, loc, elem, "item", itemreturn);
    If (ret[1] <= 1)
        ret := CreateStuff(SetFile, name, loc, elem, "mobile", itemreturn);
    EndIf
    return ret;
EndFunction // }}}

/////////////////////////////////////////////////////////////////
// DestroySet - Zerstoert ein Set des angegebenen Namens,  erstes Item
// die firstserial traegt
/////////////////////////////////////////////////////////////////
// set_name
//   Name des Sets laut sets.cfg
// firstserial
//   Serial des ersten Set-Items (siehe MakeSet())
// Rueckgabewert:
//   Anzahl der insgesamt zerstoerten Items
Function DestroySet(firstserial) // {{{
  firstserial := CInt(firstserial);

  If (firstserial <= 0)
    return 0;
  EndIf

  // Item anhand der Serial suchen
  var SetItem := SystemFindObjectBySerial(
    firstserial, SYSFIND_SEARCH_OFFLINE_MOBILES
  );

  return DestroyStuff(SetItem);
EndFunction // }}}

/////////////////////////////////////////////////////////////////
// ChangePropertySet - Legt an oder Verändert eine Property aller Teile eines Sets gleichzeitig. Achtung - das kann einige Zeit in Anspruch nehmen!
/////////////////////////////////////////////////////////////////
// set_name
//   Name des Sets laut sets.cfg
// firstserial
//   Serial des ersten Set-Items (siehe MakeSet())
// propname:
//   Name der zu ändernden (oder neuanzulegenden) Property
// propvalue:
//   Wert der zu ändernden (oder neuanzulegenden) Property
Function ChangePropertySet(set_name, firstserial, propname, propvalue) // {{{
  firstserial := CInt(firstserial);

  If (!set_name || firstserial <= 0)
    return(0);
  EndIf

  // Item anhand der Serial suchen
  var SetItem := SystemFindObjectBySerial(firstserial, SYSFIND_SEARCH_OFFLINE_MOBILES);
  If (SetItem)
    var SetFile := ReadConfigFile("::sets");

    If (SetFile)
      var elem := SetFile[set_name];
      If (elem)
        return(ChangePropStuff(SetFile, elem, {SetItem.x, SetItem.y, SetItem.z,SetItem.realm}, propname, propvalue));
      Else
        SysLog("WARNUNG: Das Set '"+set_name+"' gibt es nicht in der 'sets.cfg'!");
        return(0);
      EndIf
    Else
      SysLog("FEHLER: Kann die Datei 'sets.cfg' nicht oeffnen !");
      return(0);
    EndIf

  Else
    return(0);
  EndIf
EndFunction // }}}

Function CreateStuff(config, set_name, loc, elem, stufftype, itemreturn) // {{{
    var name := GetConfigString(elem, "desc");
    var FirstItemSerial := 0;
    var items := {};

    var lastobject := 0;
    ForEach item in GetConfigStringArray(elem, stufftype)
        SleepMS(2);

        var stuffelem;
        var definition := SplitWords(item);
        If (definition[1] == "#")
            stuffelem := config[definition[2]];
        Else
            stuffelem := struct;
            If (stufftype == "item")
                stuffelem.+objtype := CInt(definition[1]);
            ElseIf (stufftype == "mobile")
                stuffelem.+objtype := definition[1];
            EndIf
            stuffelem.+x := CInt(definition[2]);
            stuffelem.+y := CInt(definition[3]);
            stuffelem.+z := CInt(definition[4]);

            If (definition[5])
                stuffelem.+color := CInt(definition[5]);
            Else
                stuffelem.+color := 0;
            EndIf
        EndIf

        If (!stuffelem.objtype)
            continue;
        EndIf

        var props := struct;
        If (stufftype == "item")
            // Descriptor vorbereiten bei Items
            props := GetItemDescriptor(CInt(stuffelem.objtype));
            If (!props)
                props := struct;
                props["ObjClass"] := "item";
                props["ObjType"]  := CInt(stuffelem.objtype);
            EndIf
        EndIf
        props["color"] := stuffelem.color;
        If (!props.exists["CProps"])
            props["CProps"] := dictionary{};
        EndIf
        // Namen des Sets an jedem Objekt festhalten
        props.CProps.insert("set_name", set_name);
        // mit Vorgaenger zu Liste verketten
        If (lastobject)
            props.CProps.insert("set_pred", lastobject.serial);
        EndIf

        var object;
        If (stufftype == "item")
            object := CreateItemAtLocation(
                loc.x+stuffelem.x, loc.y+stuffelem.y, loc.z+stuffelem.z,
                props, 1, loc.realm
            );
            object.decayat := 0;
            object.movable := 0;
            If (itemreturn)
                items.append(object);
            EndIf

            If (FirstItemSerial == 0)
                FirstItemSerial := object.serial;
            EndIf

            ForEach cprop in GetConfigStringArray(elem, "cprop")
                var prop := SplitWords(cprop);
                If (""+prop[2] == ""+CInt(prop[2]))
                prop[2] := CInt(prop[2]);
                EndIf

                SetObjProperty(object, prop[1], prop[2]);
            EndForEach

            If (name)
                object.name := name;
            EndIf
        ElseIf (stufftype == "mobile")
            object := CreateNpcFromTemplate(
                stuffelem.objtype,
                loc.x+stuffelem.x, loc.y+stuffelem.y, loc.z+stuffelem.z,
                props, loc.realm
            );
            If (!object)
                syslog(
                    "FEHLER: Mobile-Template '"+stuffelem.objtype+"' im Set '"+
                    set_name+"' nicht gefunden."
                );
            EndIf
        EndIf

        // Verkettung von Vorgaenger zu aktuellem Objekt herstellen
        If (lastobject)
            SetObjProperty(lastobject, "set_succ", object.serial);
        EndIf

        lastobject := object;
    EndForEach

    If (stufftype == "item" && FirstItemSerial > 0)
        If (itemreturn)
            return {FirstItemSerial, items};
        Else
            return FirstItemSerial;
        EndIf
    Else
        return lastobject.serial;
    EndIf
EndFunction // }}}

Function CreateStuffDataFile(set_name, loc, elemts, stufftype, itemreturn) // {{{
    var FirstItemSerial := 0;
    var returnItems := {};
    var lastobject := 0;

    ForEach item in (elemts)
        SleepMS(2);
        // nur die gewünschten Objekte erzeugen
        If (item.kind <> stufftype)
            continue;
        EndIf

        var x := loc.x+item.x;
        var y := loc.y+item.y;
        var z := loc.z+item.z;
        var props := struct;
        If (item.kind == "item")
            // Descriptor vorbereiten bei Items
            props := GetItemDescriptor(CInt(item.type));
            If (!props)
                props := struct{};
                props["ObjClass"] := "item";
                props["ObjType"]  := CInt(item.type);
            EndIf
        EndIf
        props["color"]   := item.color;
        props["graphic"] := item.graphic;
        If (!props.exists["CProps"])
            props["CProps"] := dictionary{};
        EndIf
        // alle CProps anhängen
        ForEach cprop in (item.cprops.keys())
            var value := item.cprops[cprop];
            if (value)
                props.CProps.insert(cprop, value);
            EndIf
        EndForEach

        // Namen des Sets an jedem Objekt festhalten
        props.CProps.insert("set_name", set_name);
        // mit Vorgaenger zu Liste verketten
        If (lastobject)
            props.CProps.insert("set_pred", lastobject.serial);
        EndIf

        var object := 0;
        If (item.kind == "item")
            object := CreateItemAtLocation(x, y, z, props, 1, loc.realm);
            object.decayat := 0;
            object.movable := 0;
            object.name    := item.name;
            If (itemreturn)
                returnItems.append(object);
            EndIf
            If (FirstItemSerial == 0)
                FirstItemSerial := object.serial;
            EndIf
        Else
            // Ein Mobile erzeugen
            props.+facing := RandomInt(8);
            props.+name    := item.name;
            object := CreateNpcFromTemplatePergon(
                item.type, x, y, z, 2, props, 1, loc.realm
            );
            If (!object)
                syslog(
                    "FEHLER: Mobile-Template '"+item.type+"' im Set '"+
                    set_name+"' nicht gefunden."
                );
                continue;
            EndIf
        EndIf

        // Verkettung von Vorgaenger zum Objekt
        If (lastobject)
            SetObjProperty(lastobject, "set_succ", object.serial);
        EndIf
        lastobject := object;
    EndForEach

    If (stufftype == "item")
        // Items sollten erzeugt werden.
        if (FirstItemSerial > 0)
            // ist wirklich ein Item erzeugt worden?
            If (itemreturn)
                // Keine Ahnung wofür das hier gebraucht wird.
                return {FirstItemSerial, returnItems};
            Else
                // Damit die Items später hinter die Mobile Schlange gestellt werden kann.
                return FirstItemSerial;
            EndIf
        Else
            // Wenn nicht, dann -1 zurück geben.
            return -1;
        EndIf
    Else
        // Damit die Mobiles vor die Items Schlange gestellt werden kann.
        return lastobject.serial;
    EndIf
EndFunction // }}}

Function ChangePropStuff(config, elem, origitempos, propname, propvalue) // {{{
  var set_name := GetConfigString(elem, "desc");
  var SetArray := array;
  var itempos := array;
  var ChangeCounter := 0;

  // Array mit allen Setitems aufbauen
  ForEach item in GetConfigStringArray(elem, "item")
    var stuffelem;

    var definition := SplitWords(item);
    If (definition[1] == "#")
      stuffelem := config[definition[2]];
    Else
      stuffelem := struct;
      stuffelem.+objtype := CInt(definition[1]);
      stuffelem.+x := CInt(definition[2]);
      stuffelem.+y := CInt(definition[3]);
      stuffelem.+z := CInt(definition[4]);
    EndIf
    SetArray.append(stuffelem);
  EndForEach

  // x,y,z der einzelnen Eintraege anhand des ersten Eintrags bereinigen/normalisieren (nicht jedes Set faengt auf 0,0,0 an)
  var xmod := SetArray[1].x;
  var ymod := SetArray[1].y;
  var zmod := SetArray[1].z;

  // relative Positionen der Setitems durchgehen und reale Item suchen und zerstoeren
  ForEach arrayelem In SetArray

    // relative Position aus dem SetArray zu itempos addieren
    itempos[1] := origitempos[1] + arrayelem.x - xmod;
    itempos[2] := origitempos[2] + arrayelem.y - ymod;
    itempos[3] := origitempos[3] + arrayelem.z - zmod;

    // an itempos nach Item suchen
    var ItemList := ListItemsAtLocation(itempos[1], itempos[2], itempos[3],origitempos[4]);
    If (Len(ItemList) > 0)
      ForEach itemfound In ItemList
        // checken, ob objtype und desc übereinstimmen
        If (itemfound.objtype == arrayelem.objtype)
          If (set_name)
              If (lower(itemfound.desc) == lower(set_name))
                  SetObjProperty(itemfound, propname, propvalue);
                  ChangeCounter += 1;
                    EndIf
          Else
                  SetObjProperty(itemfound, propname, propvalue);
                ChangeCounter += 1;
          EndIf
        EndIf

      EndForEach
    Else
      // hm, fehlt schon ein Item ->egal
    EndIf

  EndForEach

  return (ChangeCounter);
EndFunction // }}}

Function DestroyStuff(item) // {{{
  // var set_name := GetConfigString(elem, "desc");
  // var SetArray := array;
  // var itempos := array;
  var DestroyCounter := 0;

  // erstmal Vorgaenger und Nachfolger suchen
  var setpred := GetObjProperty(item, "set_pred");
  var setsucc := GetObjProperty(item, "set_succ");

  var destroy := SystemFindObjectBySerial(setpred);
  var destroyed;
  // erstmal Richtung Anfangsitem abbauen
  While (destroy)
    setpred := CInt(GetObjProperty(destroy, "set_pred"));
    If (destroy.isA(POLCLASS_ITEM))
        destroyed := DestroyItem(destroy);
    Else
        SetObjProperty(destroy,  "guardkill", 1);
        destroyed := destroy.kill();
    EndIf
    If (destroyed)
      DestroyCounter += 1;
    EndIf
    If (!setpred)
      break;
    EndIf
    destroy := SystemFindObjectBySerial(setpred);
  EndWhile

  destroy := SystemFindObjectBySerial(setsucc);
  // dann in die andere Richtung
  While (destroy)
    setsucc := CInt(GetObjProperty(destroy, "set_succ"));
    If (destroy.isA(POLCLASS_ITEM))
        destroyed := DestroyItem(destroy);
    Else
        SetObjProperty(destroy,  "guardkill", 1);
        destroyed := destroy.kill();
    EndIf
    DestroyCounter += 1;
    If (!setsucc)
      break;
    EndIf
    destroy := SystemFindObjectBySerial(setsucc);
  EndWhile

  // zuletzt das Item in der Mitte vernichten
  If (item.isA(POLCLASS_ITEM))
      destroyed := DestroyItem(item);
  Else
      SetObjProperty(item,  "guardkill", 1);
      destroyed := item.kill();
  EndIf
  DestroyCounter += 1;

  return DestroyCounter;
EndFunction // }}}

// War nur Testweise.
// Aufgrund der großen Anzahl macht eine Ausgabe aber keinen Sinn.
// Nicht weiter entwickelt.
Function getSets()
    var SetFile := ReadConfigFile("::sets");
    var ret;
    var ConfigKeys := GetConfigStringKeys(SetFile);
    print(ConfigKeys);

    ret := "Hier gehts los";
    print("Anzahl Elemente in SetFile: " + ConfigKeys.Size());
    ForEach elem in ( ConfigKeys )
        print(elem);
        ret := ret + elem;
    EndForEach
    return ret;
EndFunction

// RecoverInitItem - bei nichtexistentem Init-Item Wiederherstellungsversuch
Function RecoverInitItem(one_item) // {{{
  // Serial des alten, weggekommenen Init-Items
  var serial := one_item.getprop(CPROP_INIT);

  // Liste gefundener Items, die ebenfalls die
  // alte Serial als Init-Item eingetragen haben
  var items := array{};

  // Region nach Items durchsuchen ...
  ForEach item in (ListItemsNearLocation(
      one_item.x, one_item.y, LIST_IGNORE_Z, 2*MAXAREASIZE, one_item.realm
  ))
    SleepMs(2);
    // ... die ebenfalls die alte Serial eingetragen haben
    If (item.getprop(CPROP_INIT) == serial)
      // in Listen packen und ...
      items.append(item);
    EndIf
  EndForEach

  // Markierung erneuern
  BautoolMark(items);

  // zuletzt Liste zurückgeben
  return items;
EndFunction // }}}

// Item-Liste per Bautool-Markierung verknuepfen: // {{{
// - Init-Item bekommt Liste aller Item-Serials verpasst
// - alle Items bekommt Verweis auf Init-Item-Serial
Function BautoolMark(items, init_item := 0)
  // Serial-Liste aufbauen
  var itemserials := array{};
  ForEach item in items
    itemserials.append(item.serial);
    SleepMS(2);
  EndForEach

  If (init_item)
    var pos := init_item in itemserials;
    If (pos)
      // gewähltes Item an erste Stelle tauschen
      itemserials[pos] := itemserials[1];
      itemserials[1]   := init_item.serial;
    Else
      init_item := 0;
    EndIf
  EndIf
  If (!init_item)
    // erstbestes Item benutzen
    init_item := items[1];
  EndIf

  ForEach item in items
    item.setprop(CPROP_INIT, init_item.serial);
    item.eraseprop(CPROP_ITEMS);
    SleepMS(2);
  EndForEach
  init_item.setprop(CPROP_ITEMS, itemserials);
EndFunction // }}}

Function BautoolSelect(item_or_serial)
  var items := array{};

  var item := item_or_serial;
  If (!item_or_serial.isa(POLCLASS_ITEM))
      item := SystemFindObjectBySerial(item_or_serial);
  EndIf

  var init_item := SystemFindObjectBySerial(CInt(item.getprop(CPROP_INIT)));
  If (!init_item)
    // nicht gefunden? Reparaturversuch:
    items := RecoverInitItem(item);
    return items;
  EndIf

  var itemserials  := init_item.getprop(CPROP_ITEMS);
  var update_marks := 0;
  ForEach item in itemserials
    SleepMS(2);
    var obj := SystemFindObjectBySerial(item);
    If (obj)
      // gehört es noch der selben Gruppe an
      If (CInt(obj.getprop(CPROP_INIT)) == CInt(init_item.serial))
        items.append(obj);
      EndIf
    Else
      update_marks := 1;
    EndIf
  EndForEach

  If (update_marks)
    BautoolMark(items, init_item);
  EndIf

  return items;
EndFunction
