///////////////////////////////////////////////////////////////////////////
// StartEqp include v1.0 (urspruenglicher Autor ist unbekannt)
//
// Author: Shinigami

///////////////////////////////////////////////////////////////////////////
// Ansammlungen von Items erzeugen
//   MakeLoot                - NPC-Corpse oder Container mit Items fuellen                    (NLootGroup.cfg)
//   CreateStartingEquipment - Erzeuge das Equipment nach der Char-Erschaffung                (StartEqp.cfg)
//
// Items gemaess Group-Definition erzeugen
//   CreateFromGroup         - Items gemaess Group-Definition in der ConfigDatei erzeugen
//   CreateFromStackString   - Erzeugt Items als Stacks (sofern der Client es zulaesst)
//   CreateFromItemString    - Erzeugt einzelne [magische] Items (da sie nicht stackbar sind)
//   CreateSingleItem        - Erzeugt ein einzelnes [magisches] Item
//   CreateFromRandomString  - Erzeugt einzelne [magische] Items, zufaellig aus einer Liste
//   CreateFromUniqueString  - Erzeugt ein einzigartiges [magisches] Item
//
// Items "magisch" veraendern
//   MagicAllowedWeaponArmor - Darf die Waffe oder Ruestung "magisch" veraendert werden?
//   ApplyDmgMod             - Gemaess Zufall Veraenderung des Schadens der Waffe
//   ApplyARMod              - Gemaess Zufall Veraenderung der Staerke der Ruestung
//   ApplyQualityMod         - Gemaess Zufall Veraenderung der Qualitaet
//   AddName                 - Name der Waffe oder Ruestung veraendern

use cfgfile;
use os;
use util;
include ":newmagic:spellbook";
include "include/logutil";

Const PROP_NO_START_EQUIP := "nostarteqp";

/////////////////////////////////////////
//   Ansammlungen von Items erzeugen   //
/////////////////////////////////////////

// NPC-Corpse oder Container mit Items fuellen (NLootGroup.cfg)
Function MakeLoot(object, lootgroup) // {{{
  // NPCCorpse -> Param 'lootgroup' wird ignoriert
  //   NPCDesc -> Eintraege 'lootgroup name'
  //              [Eintrag  'magicitemchance chance']
  //     name    = [Zahl oder String] Name der Group in der NLootGroup.cfg
  //     chance  = [Prozentzahl]      Chance zur Erzeugung "magischer" Items (default = 0)
  //
  // Container -> Param 'lootgroup'
  //   lootgroup = [Zahl oder String] Name der Group in der NLootGroup.cfg

  var spawncfg := ReadConfigFile("::nlootgroup");
  If (!spawncfg)
    syslog("FEHLER: Konnte nlootgroup.cfg nicht oeffnen!");
    return;
  EndIf

  var state := struct{
    // Jedes Item bei Zufallsauswahl beliebig oft moeglich
    randomonce := 0,
    // Chance zum Erzeugen "magischer" Items
    magicspawnchance := 0,
    // Liste der Items, die Newbie-Items werden
    newbieitems := array{},
    // Liste der bereits erzeugten Unique-Items
    uniqueitems := array{}
  };

  If (object.isa(POLCLASS_CORPSE)) // Ist es ein toter NPC?
    var npctemplate := GetObjProperty(object, "npctemplate");
    If (!npctemplate)
      syslog(
        "FEHLER: Dem Corpse '"+LHex(object.serial)+
        "' fehlt die Property 'npctemplate' !"
      );
      return;
    EndIf

    // NPCTemplate in der ConfigDatei suchen
    var elem := GetNPCConfig(npctemplate);
    If (!elem)
      syslog(
        "FEHLER: Konnte das NPCTemplate '"+npctemplate+
        "' nicht in der npcdesc.cfg finden!"
      );
      return;
    EndIf

    // Chance zum Erzeugen "magischer" Items holen
    state.magicspawnchance := CInt(GetConfigInt(elem, "magicitemchance"));
    ForEach lootgroup in GetConfigStringArray(elem, "lootgroup")
      // Items, passend zur NPCDesc-Template-LootGroup, erzeugen
      CreateFromGroup(object, lootgroup, spawncfg, state);
    EndForEach

  ElseIf (object.isa(POLCLASS_CONTAINER)) // Ist es ein Container?
    // Items, passend zur uebergebenen LootGroup, erzeugen
    CreateFromGroup(object, lootgroup, spawncfg, state);

  Else
    syslog(
      "FEHLER: Beim Object '"+LHex(object.serial)+
      "' handelt es sich weder um einen Corpse noch um einen Container!"
    );
  EndIf
EndFunction // }}}

// Erzeuge das Equipment nach der ersten Berufswahl (StartEqp.cfg)
Function CreateStartingEquipment(who, skillids) // {{{
  var spawncfg := ReadConfigFile("::starteqp");
  If (!spawncfg)
    syslog("FEHLER: Konnte starteqp.cfg nicht oeffnen!");
    return;
  EndIf

  var state := struct{
    // Jedes Item bei Zufallsauswahl nur 1x moeglich
    randomonce := 1,
    // Chance zum Erzeugen "magischer" Items
    magicspawnchance := 0,
    // Liste der Items, die Newbie-Items werden
    newbieitems := array{},
    // Liste der bereits erzeugten Unique-Items
    uniqueitems := array{}
  };

  // Equipment, passend zum Skill, erzeugen
  ForEach skillid in (skillids)
    CreateFromGroup(who.backpack, skillid, spawncfg, state);
  EndForEach

  // Standard-Equipment erzeugen
  CreateFromGroup(who.backpack, "All", spawncfg, state);

  // Items zu Newbie-Items machen
  ForEach item in (state.newbieitems)
    item.newbie := 1;
  EndForEach
EndFunction // }}}

/////////////////////////////////////////////////
//   Items gemaess Group-Definition erzeugen   //
/////////////////////////////////////////////////

// Items gemaess Group-Definition in der ConfigDatei erzeugen
Function CreateFromGroup(container, group, byref spawncfg, byref state) // {{{
  // ConfigString = Group groupname
  //   groupname = [Zahl oder String] Name der verschachtelten Gruppe
  // Group in der ConfigDatei suchen
  var elem := FindConfigElem(spawncfg, group);
  If (!elem)
    syslog("FEHLER: Konnte die Group '"+group+"' nicht finden!");
    return;
  EndIf

  ForEach str in GetConfigStringArray(elem, "Group")
    // Items gemaess Group-Definition in der ConfigDatei erzeugen
    CreateFromGroup(container, str, spawncfg, state);
  EndForEach
  SleepMS(2);
  ForEach str in GetConfigStringArray(elem, "Stack")
    // Erzeugt Items als Stacks (sofern der Client es zulaesst)
    CreateFromStackString(container, str, group, state);
  EndForEach
  SleepMS(2);
  ForEach str in GetConfigStringArray(elem, "Item")
    // Erzeugt einzelne [magische] Items (da sie nicht stackbar sind)
    CreateFromItemString(container, str, group, state);
  EndForEach
  SleepMS(2);
  ForEach str in GetConfigStringArray(elem, "Random")
    // Erzeugt einzelne [magische] Items, zufaellig aus einer Liste
    CreateFromRandomString(container, str, spawncfg, state);
  EndForEach
  SleepMS(2);
  ForEach str in GetConfigStringArray(elem, "Unique")
    // Erzeugt ein einzigartiges [magisches] Item
    CreateFromUniqueString(container, str, group, state);
  EndForEach
EndFunction // }}}

// Erzeugt Items als Stacks (sofern der Core es zulaesst)
Function CreateFromStackString(container, str, group, byref state) // {{{
  // ConfigString = Stack [count := 1] objname [color := 0] [chance := 100]
  //   count   = [Zahl oder DiceRoll] Anzahl der Items im Stack (default = 1)
  //   objname = [Name]               ObjektName
  //   color   = [Zahl]               Farbe                     (default = 0 = Farbe gemaess MUL)
  //   chance  = [Prozentzahl]        Chance zur Erzeugung      (default = 100)
  var params := SplitWords(str); // ConfigString zerlegen
  var count := RandomDiceRoll(params[1]);  // Anzahl der zu erzeugenden Items im Stack
  var color := 0;                // Farbe (0 = Farbe gemaess MUL)
  var chance := 100;             // Chance zur Erzeugung (Prozent)

  If (count != error) // Wurde eine Anzahl uebergeben?
    params[1] := params[2]; // restliche Parameter verschieben
    params[2] := params[3];
    params[3] := params[4];
  Else
    count := 1;
  EndIf

  var objname := params[1]; // ObjName

  If (CInt(params[2])) // Wurde eine Farbe uebergeben?
    color := CInt(params[2]);
  EndIf

  If (CInt(params[3])) // Wurde eine Chance uebergeben?
    chance := CInt(params[3]);
  EndIf

  // Items gemaess Chance erzeugen?
  If (1+RandomInt(100) > CInt(chance))
    // Pech
    return;
  EndIf

  If (!container)
    syslog(
      "WARNUNG: Container nicht mehr vorhanden! (objname: "+objname+
      " count: "+count
    );
    return;
  EndIf

  // Item-Stack erzeugen
  var items := CreateItemInContainerPergon(container, objname, count);
  If (items)
    If (color) // Nur dann Farbe setzen, wenn gewünscht
      items.color := color; // Item-Stack einfaerben
    EndIf
    // merken, um Items spaeter ggf. zu Newbie-Items machen
    state.newbieitems.append(items);
  ElseIf (items.errortext<>"That container is full")
    syslog(
      "FEHLER: Konnte den ConfigString '"+str+"' aus der Group '"+group+
      "' nicht korrekt ausfuehren ("+items+") ["+LHex(container.serial)+"!"
    );
  EndIf
EndFunction // }}}

// Erzeugt einzelne [magische] Items (da sie nicht stackbar sind)
Function CreateFromItemString(container, str, group, byref state) // {{{
  // ConfigString = Item [count := 1] objname [color := 0] [chance := 100]
  //   count   = [Zahl oder DiceRoll] Anzahl der Items     (default = 1)
  //   objname = [Name]               ObjektName
  //   color   = [Zahl]               Farbe                (default = 0 = Farbe gemaess MUL)
  //   chance  = [Prozentzahl]        Chance zur Erzeugung (default = 100)

  var params := SplitWords(str); // ConfigString zerlegen
  var count := RandomDiceRoll(params[1]); // Anzahl der zu erzeugenden Items
  var color := 0;                // Farbe (0 = Farbe gemaess MUL)
  var chance := 100;             // Chance zur Erzeugung (Prozent)

  If (count!=error) // Wurde eine Anzahl uebergeben?
    params[1] := params[2]; // restliche Parameter verschieben
    params[2] := params[3];
    params[3] := params[4];
  Else
    count := 1;
  EndIf
  

  var objname := params[1]; // ObjName

  If (CInt(params[2])) // Wurde eine Farbe uebergeben?
    color := CInt(params[2]);
  EndIf

  If (CInt(params[3])) // Wurde eine Chance uebergeben?
    chance := CInt(params[3]);
  EndIf

  // Items gemaess Chance erzeugen?
  If (RandomInt(100)+1 > CInt(chance))
    // Pech
    return;
  EndIf

  If (!container)
    syslog(
      "WARNUNG: Container nicht mehr vorhanden! (objname: "+objname+
      " count: "+count
    );
    return;
  EndIf

  For index := 1 To count // Einzelne Items erzeugen
    // Erzeugt ein einzelnes [magisches] Item
    var item := CreateSingleItem(container, objname, color, state);
    If (!item)
      If (item.errortext<>"That container is full")
        syslog(
          "FEHLER: Konnte den ConfigString '"+str+"' aus der Group '"+group+
          "' nicht korrekt ausfuehren ("+item+") ["+LHex(container.serial)+"]!"
        );
      EndIf
    EndIf
    SleepMS(2);
  EndFor
EndFunction // }}}

// Erzeugt ein einzelnes [magisches] Item
Function CreateSingleItem(container, objname, color, byref state) // {{{
  // Item erzeugen
  var item := CreateItemInContainerPergon(container, objname);
  If (!item)
    return item;
  EndIf

  If (color) // Nur dann Farbe setzen, wenn gewünscht
    item.color := color; // Item einfaerben
  EndIf
  // merken, um Item spaeter ggf. zu Newbie-Item machen
  state.newbieitems.append(item);

  // "Magische" Items erzeugen?
  If (1+RandomInt(100) > state.magicspawnchance)
    // noe, normales Item
    return item;
  EndIf

  // Darf die Waffe oder Ruestung "magisch" veraendert werden?
  Case (MagicAllowedWeaponArmor(objname))
  1: // Waffe
    Case (RandomInt(2)+1)
    1: // Gemaess Zufaell Veraenderung des Schadens der Waffe
      ApplyDmgMod(item);
    2: // Gemaess Zufaell Veraenderung der Qualitaet
      ApplyQualityMod(item);
    EndCase
    AddName(item); // Name der Waffe oder Ruestung veraendern

  2: // Ruestung
    // nur AR-Mod, da in ApplyARMod() eine zuaetzliche Chance auf QualityMod
    // enthalten ist
    Case (1)
      1: // Gemaess Zufaell Veraenderung der Staerke der Ruestung
        ApplyARMod(item);
        // Name der Waffe oder Ruestung veraendern
        AddName(item);
      // 2: // Gemaess Zufaell Veraenderung der Qualitaet
      //   ApplyQualityMod(item);
      EndCase
  EndCase

  return item;
EndFunction // }}}

// Erzeugt einzelne [magische] Items, zufaellig aus einer Liste
Function CreateFromRandomString(container, str, byref spawncfg, byref state) // {{{
  // ConfigString = Random [count := 1] group [chance := 100]
  //   count   = [Zahl oder DiceRoll] Anzahl der Items     (default = 1)
  //   group   = [Name]               Name der Item-Group in der ConfigDatei
  //   chance  = [Prozentzahl]        Chance zur Erzeugung (default = 100)
  var params := SplitWords(str); // ConfigString zerlegen
  var count := RandomDiceRoll(params[1]);  // Anzahl der zu erzeugenden Items
  var chance := 100;             // Chance zur Erzeugung (Prozent)

  If (count != error) // Wurde eine Anzahl uebergeben?
    params[1] := params[2]; // restliche Parameter verschieben
    params[2] := params[3];
  Else
    count := 1;
  EndIf

  var group := params[1]; // Item-Group

  If (CInt(params[2])) // Wurde eine Chance uebergeben?
    chance := CInt(params[2]);
  EndIf

  // Items gemaess Chance erzeugen?
  If (1+RandomInt(100) > CInt(chance))
    // Pech
    return;
  EndIf

  // Group in der ConfigDatei suchen
  var elem := FindConfigElem(spawncfg, group);
  If (!elem)
    syslog("FEHLER: Konnte die Group '"+group+"' nicht finden!");
    return;
  EndIf

  // Einzelne [magische] Items in der Group suchen
  var items := GetConfigStringArray(elem, "Item");
  If (!items)
    syslog(
      "FEHLER: Konnte in der Group '"+group+"' keinen Eintrag 'Item' finden!"
    );
    return;
  EndIf

  // Jedes Item bei Zufallsauswahl nur 1x waehlbar?
  If (state.randomonce)
    If (count > Len(items))
      // Es koennen nicht mehr verschiedene "Item"s genommen werden, wie da
      // sind ...
      syslog(
        "WARNUNG: In der Group '"+group+"' gibt es nur "+Len(items)+
        " und keine "+count+" Items ("+str+")!");
      count := Len(items);
    EndIf
  EndIf

  While (count) // Einzelne Items erzeugen
    var index := RandomInt(Len(items))+1; // Per Zufall einen Eintrag auswaehlen
    If (items[index]) // Kann der Eintrag [noch] gewaehlt werden?
      // Erzeugt einzelne [magische] Items (da sie nicht stackbar sind)
      CreateFromItemString(container, items[index], group, state);

      // Jedes Item bei Zufallsauswahl nur 1x waehlbar?
      If (state.randomonce)
        // Eintrag kann zukuenftig nicht mehr gewaehlt werden
        // (in _dieser_ Schleife)
        items[index] := 0;
      EndIf

      count -= 1; // Zahl der zu erzeugenden Items reduzieren
    EndIf
  EndWhile
EndFunction // }}}

// Erzeugt ein einzigartiges [magisches] Item
Function CreateFromUniqueString(container, str, group, byref state) // {{{
  // ConfigString = Unique objname [color := 0] [chance := 100]
  //   objname = [Name]        ObjektName
  //   color   = [Zahl]        Farbe                (default = 0 = Farbe gemaess MUL)
  //   chance  = [Prozentzahl] Chance zur Erzeugung (default = 100)
  var params := SplitWords(str); // ConfigString zerlegen
  var color := 0;                // Farbe (0 = Farbe gemaess MUL)
  var chance := 100;             // Chance zur Erzeugung (Prozent)

  var objname := params[1];                 // ObjName
  var objtype := GetObjtypeByName(objname); // ObjType

  If (CInt(params[2])) // Wurde eine Farbe uebergeben?
    color := CInt(params[2]);
  EndIf

  If (CInt(params[3])) // Wurde eine Chance uebergeben?
    chance := CInt(params[3]);
  EndIf

  // Items gemaess Chance erzeugen?
  If (1+RandomInt(100) > CInt(chance))
    return;
  EndIf

  // Wurde dieses Item bereits erzeugt?
  If (objtype in (state.uniqueitems))
    // ja, Abbruch
    return;
  EndIf

  If (!container)
    syslog(
      "WARNUNG: Container nicht mehr vorhanden! (objname: "+objname+
      " color: "+color
    );
    return;
  EndIf

  // Erzeugt ein einzelnes [magisches] Item
  var item := CreateSingleItem(container, objname, color, state);
  If (!item)
    If (item.errortext <> "That container is full")
      // nicht ueber volle Ziele meckern
      return;
    EndIf
    syslog(
      "FEHLER: Konnte den ConfigString '"+str+"' aus der Group '"+group+
      "' nicht korrekt ausfuehren ("+item+") ["+LHex(container.serial)+"!"
    );
    return;
  EndIf

  state.uniqueitems.append(objtype);
EndFunction // }}}

////////////////////////////////////
//   Items "magisch" veraendern   //
////////////////////////////////////

// Darf die Waffe oder Ruestung "magisch" veraendert werden?
Function MagicAllowedWeaponArmor(objname) // {{{
  var magicweapscfg := ReadConfigFile(":combat:magicweaps");
  If (!magicweapscfg)
    syslog("Konnte die Datei :combat:magicweaps.cfg nicht oeffnen!");
    return magicweapscfg;
  EndIf

  var magicarmorcfg := ReadConfigFile(":combat:magicarmor");
  If (!magicarmorcfg)
    syslog("Konnte die Datei :combat:magicarmor.cfg nicht oeffnen!");
    return magicarmorcfg;
  EndIf

  // Auflistung der Waffen suchen
  var weapons := FindConfigElem(magicweapscfg, "1");
  If (!weapons)
    syslog(
      "FEHLER: Konnte die Group '1' nicht in der :combat:magicweaps finden!"
    );
    return weapons;
  EndIf

  // Darf die Waffe "magisch" veraendert werden?
  If (GetConfigInt(weapons, objname))
    return 1; // Waffe
  EndIf

  // Auflistung der Ruestungen suchen
  var armors := FindConfigElem(magicarmorcfg, "1");
  If (!armors)
    syslog(
      "FEHLER: Konnte die Group '1' nicht in der :combat:magicarmor finden!"
    );
    return armors;
  EndIf

  // Darf die Ruestung "magisch" veraendert werden?
  If (GetConfigInt(armors, objname))
    return 2; // Ruestung
  EndIf
EndFunction // }}}

// Gemaess Zufaell Veraenderung des Schadens der Waffe
Function ApplyDmgMod(item) // {{{
  var randvalue := RandomInt(100)+1;
  If (randvalue >= 50)
    item.dmg_mod := 1;
    // Fuer den Skill "Gegenstaende identifizieren"
    SetObjProperty(item, "suffix", "der Qualitaet");
  ElseIf (randvalue >= 25)
    item.dmg_mod := 3;
    SetObjProperty(item, "suffix", "der Macht");
  ElseIf (randvalue >= 15)
    item.dmg_mod := 5;
    SetObjProperty(item, "suffix", "der Vernichtung");
  ElseIf (randvalue >= 5)
    item.dmg_mod := 7;
    SetObjProperty(item, "suffix", "der Staerke");
  ElseIf (randvalue >= 1)
    item.dmg_mod := 9;
    SetObjProperty(item, "suffix", "der Zerstoerung");
  EndIf

  If (RandomInt(100)+1 <= 10)
    // Mit 10%iger Chance zusaetzlich Veraenderung der Qualitaet der Waffe
    ApplyQualityMod(item);
  EndIf
EndFunction // }}}

// Gemaess Zufaell Veraenderung der Staerke der Ruestung
Function ApplyARMod(item) // {{{
  var randvalue := RandomInt(100)+1;
  If (randvalue >= 50)
    item.ar_mod := 1;
    // Fuer den Skill "Gegenstaende identifizieren"
    SetObjProperty(item, "suffix", "der Verteidigung");
  ElseIf (randvalue >= 25)
    item.ar_mod := 2;
    SetObjProperty(item, "suffix", "des Schutzes");
  ElseIf (randvalue >= 15)
    item.ar_mod := 4;
    SetObjProperty(item, "suffix", "der Abwehr");
  ElseIf (randvalue >= 5)
    item.ar_mod := 6;
    SetObjProperty(item, "suffix", "der Verstaerkung");
  ElseIf (randvalue >= 1)
    item.ar_mod := 9;
    SetObjProperty(item, "suffix", "der Unverwundbarkeit");
  EndIf

  // Mit 10%iger Chance zusaetzlich Veraenderung der Qualitaet der Ruestung
  If (RandomInt(100)+1 <= 10)
    ApplyQualityMod(item);
  EndIf
EndFunction // }}}

// ApplyQualityMod - Gemaess Zufaell Veraenderung der Qualitaet
Function ApplyQualityMod(item) // {{{
  var randvalue := RandomInt(100)+1;
  If (randvalue >= 50)
    item.quality := 1.3;
    // Fuer den Skill "Gegenstaende identifizieren"
    SetObjProperty(item, "prefix", "[gut]");
  ElseIf (randvalue >= 25)
    item.quality := 1.6;
    SetObjProperty(item, "prefix", "[besser]");
  ElseIf (randvalue >= 15)
    item.quality := 1.9;
    SetObjProperty(item, "prefix", "[haltbar]");
  ElseIf (randvalue >= 5)
    item.quality := 2.2;
    SetObjProperty(item, "prefix", "[stabil]");
  ElseIf (randvalue >= 1)
    item.quality := 2.5;
    SetObjProperty(item, "prefix", "[undurchdringlich]");
  EndIf

  item.hp := item.maxhp; // HitPoints anpassen
  IncRevision(item);
EndFunction // }}}

// AddName - Name der Waffe oder Ruestung veraendern
Function AddName(item) // {{{
  If (SplitWords(item.desc)[1] in {"a", "an", "ein", "eine"})
    SetName(item, "[magisch] "+item.desc[
        Find(item.desc, " ", 1)+1, Len(item.desc)
    ]);
  Else
    SetName(item, "[magisch] "+item.desc);
  EndIf
EndFunction // }}}

// vim: sw=2 sts=2 nowrap
