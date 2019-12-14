///////////////////////////////////////////////////////////////////////////
// Inscription include - Inscription Skill For all Books

// Modifications:
// 1999/04/15 ENS:              Reformatted
// 2000/10/11 Fraggulus:        Anpassung von ObjTypes an das Nekromantenscript
// 2002/01/20 Bihinos:          Komplettueberarbeitung und Anpassung an neue
//                              Magie
// 2002/01/20 Shinigami:        ReWrite
// 2002/02/17 Shinigami:        Umstellung SpellContainer -> SpellsMask
// 2002/05/07 Shinigami:        Mehrfachnutzung verhindern
// 2002/05/08 Shinigami:        include umgestrickt und Tinte&Feder eingebaut
// 2003/03/08 Shinigami:        Reihenfolge der Circle umgedreht, weil
//                              sinnvoller
// 2003/12/06 Phygon:           Neues Menü-Gump für die Spellauswahl
// 2004/02/29 Fraggulus:        Reserve/ReleaseItem() fuer die leeren Rollen
// 19.07.2004 Fox:              Abgeschriebene Sprueche bekommen CProp(copied)
// 2004/08/02 Shinigami:        CProp(copied) wurde irrtuemlich als String
//                              angelegt
// 2004/08/01 Bihinos:          Inscription erstellt jetzt deaktivierte
//                              Zauberrollen anstatt Spruchrollen /
//                              Zweisprachig
// 2005-10-09 Seppel:           Artefaktherstellung angefangen
// 2005-10-10 Seppel:           Artefaktherstellung abgeschlossen
// 2005-18-12 Seppel:           beim Herstellen von Artefakten kann auch
//                              besseres Artefakt enstehen
// 2006/06/03 Shinigami:        neue Magie-Symbole entfernt (stattdessen auf
//                              Gumps umgeschwenkt)
// 2008-04-02 mehdorn:          Doppelten Abbruch-Text bei Abbruch der Auswahl
//                              in ArtefactMenu gefixt
// 2008-04-24 mehdorn:          noch eine doppelte Abbruchmeldung gefunden
// $Log: not supported by cvs2svn $
// Revision 1.21  2010/07/15 21:47:38  Mentholia
// Vereinheitlichung der Abbruchtexte
//
// Revision 1.20  2010/03/01 19:09:05  turley
// magiekram auch in ne datafile verschoben, todo sinn und unsinn der einzelnen rausfinden um bissle zu optimieren
//
// Revision 1.19  2010/03/01 18:36:12  turley
// kleiner bug bei skillwin
// sinnlose speicherung der artefaktdaten entfernt
//
// Revision 1.18  2008/10/22 15:30:30  turley
// Bissle mit FindSubstance rumgespielt
//
// Revision 1.17  2008/09/27 21:29:01  mehdorn
// - auslogsicherer
//
// Revision 1.16  2008/09/04 19:45:45  mehdorn
// - in Housing mal Konstanten fuer viel Props eingefuehrt
// - entsprechend in anderen Scripten verwendet
//

/////////////////
// Bibliotheken&Includes
/////////////////
include "artefactbook";
include "include/manufacturegump";
use polsys;

// Vorbereitung und Durchfuehrung der Inscription
Function CheckAndUseItemsAndInscribe(who, bookobjtype, bookname) // {{{
  If (!GetObjProperty(who, "#inscription"))                      // Mehrfachnutzung verhindern
    SetObjProperty(who, "#inscription", 1);

    SendSysMessagePergon(who, "Was wollt Ihr benutzen?");

    var item := Target(who);                                       // get item, skill is used with
    If (item)
      If (AccessiblePergon(who, item))
        var artefactBooks := {UOBJ_ARTEFACTBOOK_MAGE, UOBJ_ARTEFACTBOOK_CLERIC, UOBJ_ARTEFACTBOOK_NECRO};
        If (item.objtype == UOBJ_BLANK_SCROLL)                     // If item is blank scroll, skill is used to inscript spell scroll
          var book := SearchSpellBookInBackpack(who, bookobjtype); // Zauberbuch im Backpack oder in der rechten Hand suchen
          If (book)                                              // check If who has Book
            If (AccessiblePergon(who, book))
              If (FindSubstance(who.backpack,0xfc0,1,0)) // Tinte und Feder
                Inscription(who, book, item);                    // call inscription routine from include
              Else
                SendSysMessagePergon(who, "Ihr braucht Tinte und Feder in eurem Rucksack, um schreiben zu koennen!");
              EndIf
            Else
              SendSysMessagePergon(who, "Ihr kommt da nicht ran!");
            EndIf
          Else
            SendSysMessagePergon(who, "Ihr habt "+bookname+", aus dem Ihr abschreiben koenntet!");
          EndIf
        ElseIf(item.objtype in artefactBooks)                    // wenn Artefaktbuch gewählt wurde
          If(GetObjProperty(item, "owner") == who.serial)        // Buch muss natürlich das eigene sein
            Artefaction(who, item);                              // Artefaktherstellungs Routine aufrufen
          Else
            SendSysMessagepergon(who, "Das ist nicht euer Buch!", "This isnt your book!");
          EndIf
        Else
          SendSysMessagePergon(who, "Das koennt ihr nicht benutzen!");
        EndIf
      Else
        SendSysMessagePergon(who, "Ihr kommt da nicht ran!");
      EndIf
    Else
      SendSysMessagePergon(who, "Abbruch", "Abort");
    EndIf

    EraseObjProperty(who, "#inscription");
  Else
    SendSysMessagePergon(who, "Ihr beschreibt bereits Rollen!");
  EndIf
EndFunction // }}}

// Spruchrolle mit Spruch aus Buch beschreiben
Function Inscription(who, book, emptyscrolls) // {{{
  ReserveItem(emptyscrolls);

  var spell_cfg := ReadConfigFile("::spells");

  If (spell_cfg)
    var itemdesc := ReadConfigFile("::itemdesc");
    If (itemdesc)
      // Zu schreibende Cast-Scroll auswaehlen (ItemID)
      var castscroll := ProcessMenu(who, book);
      var castscrollid := castscroll.objtype;
      var number := castscroll.amount;
      If (castscrollid)
        // Ueber die ItemID zur SpellID
        var castscrolldef := itemdesc[castscrollid];
        If (castscrolldef)
          var spellid := castscrolldef.spellid;
          If (spellid)
            // Ueber die SpellID zum benoetigten Inscription Skill
            var spelldef := spell_cfg[spellid];
            If (spelldef)
              var skill := spelldef.skill;
              If (skill)
                var skillid; // Den zu pruefenden Skill zuordnen
                // var color;    // Die Farbe zur inaktiven Zauberrolle zuordnen
                Case (book.objtype)
                  UOBJ_SPELLBOOK_MAGE:   skillid := SKILLID_MAGISCHEGEGENST;
                                         // color := 1815;
                  UOBJ_SPELLBOOK_CLERIC: skillid := SKILLID_GESEGNETEGEGENST;
                                         // color := 1150;
                  UOBJ_SPELLBOOK_NECRO:  skillid := SKILLID_VERFLUCHTEGEGENST;
                                         // color := 2304;
                EndCase

                var akt_x := who.x; // Akt. Koordinaten sichern
                var akt_y := who.y;

                // number wird vom Menü-Gump erzeugt
                //var number := CInt(SendTextEntryGump(who, "Wieviele Rollen wollt Ihr beschreiben? (Max 10)", 5));

                If (1<=number)
                  While (number And (emptyscrolls.amount)) // Noch was schreiben und noch was zum Schreiben da?
                    If (!who.connected)
                      break;
                    EndIf
                    If ((akt_x == who.x) And (akt_y == who.y))   // Hab ich mich wegbewegt?
                      SubtractAmount(emptyscrolls, 1); // Eine Rolle verbraucht
                      number -= 1;                // Einen Versuch "verbraucht"

                      // SkillCheck durchfuehren und bei Erfolg Cast-Scroll im Rucksack erzeugen
                      If (CheckSkillPergon(who, skillid, skill, 2*skill))
                        var castscroll := GetItemDescriptor(CInt(castscrollid));
                        castscroll.cprops.insert("castable", 0);
                        // castscroll.color := color;

                        var scroll := CreateItemInBackpackPergon(who, castscroll, 1);
                        If (scroll)
                          PlaySoundEffect(who, SFX_24A);
                          PlayObjectCenteredEffect(scroll, FX_MARK_EFFECT, 10, 10);
                          SendSysMessagePergon(who, "Ihr beschreibt eine Rolle und legt sie in Euren Rucksack.");
                        Else
                          SendSysMessagePergon(who, "Euer Rucksack ist voll!"); // Dieser Fall tritt nicht auf, da die Rollen auf dem Boden erfolgreich erzeugt werden wenn das Backpack voll ist
                        EndIf

                        Sleep(6); // Etwas warten
                      Else
                        PlaySoundEffect(who, SFX_50);
                        SendSysMessagePergon(who, "Ihr verschreibt Euch und muesst daraufhin die Rolle wegwerfen.");

                        Sleep(4); // Etwas warten
                      EndIf
                    Else
                      SendSysMessagePergon(who, "Ihr verliert Eure Konzentration!");
                      Break;
                    EndIf
                  EndWhile
                Else
                  SendSysMessagePergon(who, "Abbruch", "Abort");
                EndIf

                ReleaseItem(emptyscrolls);
                return;
              Else
                syslog("FEHLER: Spell "+spellid+" besitzt keinen Skill-Parameter!");
              EndIf
            Else
              syslog("FEHLER: Spell "+spellid+" konnte nicht gefunden werden!");
            EndIf
          Else
            syslog("FEHLER: Cast-Scroll "+castscrollid+" besitzt keinen SpellID-Parameter!");
          EndIf
        Else
          syslog("FEHLER: Cast-Scroll "+castscrollid+" konnte nicht gefunden werden!");
        EndIf
      EndIf
    Else
      syslog("FEHLER: Konnte die ItemDesc nicht oeffnen!");
    EndIf
  Else
    syslog("FEHLER: Spells-ConfigFile nicht gefunden!");
  EndIf

  ReleaseItem(emptyscrolls);
  SendSysMessagePergon(who, "Abbruch", "Abort");
EndFunction // }}}

// Artefakt aus Artefaktbuch herstellen
Function Artefaction(who, book) // {{{

  var artefact := ArtefactMenu(who, book);                           // Auswahlgump aufrufen

  If(!artefact)
    SendSysMessagepergon(who, "Abbruch", "Abort");
    return 0;
  EndIf

  var skillID;

  Case(book.objtype)                                                // Skill jeh nach Buch
   UOBJ_ARTEFACTBOOK_MAGE:   skillID := SKILLID_MAGISCHEGEGENST;
   UOBJ_ARTEFACTBOOK_CLERIC: skillID := SKILLID_GESEGNETEGEGENST;
   UOBJ_ARTEFACTBOOK_NECRO:  skillID := SKILLID_VERFLUCHTEGEGENST;
  EndCase

  var x := who.x;
  var y := who.y;

  var itemdesc := ReadConfigFile("::itemdesc");                     // über die itemdesc zur artefactID
  var item := FindConfigElem(itemdesc, artefact.objtype);
  var artefactID := item.artefactID;

  var artefactscfg := ReadConfigFile("artefacts");                  // über die artefacts.cfg zur difficulty
  var artefactcfg := FindConfigElem(artefactscfg, artefactID);
  var difficulty := artefactcfg.skill;

  If (!difficulty)
    syslog("FEHLER: CFG Fehler in Artefaction!");
    return 0;
  EndIf

  var i;
  For (i := 1; i<= artefact.amount; i += 1)
    var parts := CheckPartsPergon(who, artefactID);

    If (!parts || who.x <> x || who.y <> y)                        // abbrechen wenn ArtefactReags alle oder man sich bewegt
      SendSysMessagePergon(who, "Ihr hoert auf Artefakte herzustellen", "You stop creating artefacts");
      return 0;
    EndIf

    If (CheckSkillPergon(who, skillID, difficulty, 5*difficulty))               // Skillcheck & gain

      var artefactMask := GetArtefactMask(book);
      var artefactIDnew := 0;
      var createtartefact;
      var CProps;
      var MaskArtefactID := artefactID % 100;  // erste Stelle abschneiden -> nurnoch Zirkel und Spell
                                             // dann klappt das auch mit Arrayindex
      If (RandomInt(10001) + artefactMask[MaskArtefactID] >= 10000)                 // wird besseres Artefakt erstellt? Chance von 0.01% bei einmal eingetragen und 1% bei 100 mal eingetragen

        var temp := CInt(MaskArtefactID / 10);
        var circle := temp % 10;                                             // Zirkel des aktuellen Artefakts bestimmen

        artefactIDnew := GetRandomArtefactIdFromCircle(skillID, circle + 1);     // zufälluge ArtefaktID aus höherem Zirkel holen

        If (artefactIDnew <> 0)                                                 // Artefact im höheren Zirkel gefunden?
          var artefactcfgNew := FindConfigElem(artefactscfg, artefactIDnew);     // neue Daten holen damit hergestellt werden kann

          CProps := GetConfigStringArray(artefactcfgNew, "CProp");               // CProps aus artefacts.cfg
          var artefactInfosNew := GetCfgEntryFromArtefactID(artefactIDnew);    // objtype des neuen Artefakts holen
          var objtypeNew := artefactInfosNew.item;

          createtartefact := GetItemDescriptor(CInt(objtypeNew));                // Artefakt konfigurieren
          createtartefact.cprops.insert("ArtefactID" , artefactIDnew);          // ArtefaktID merken für Aktivierung

          SendSysMessagepergon(who, "Die Macht des Buches laesst euch ein maechtigeres Artefakt herstellen", "The power of the book creates a more powerfull artefact");
        EndIf

      EndIf

      If (artefactIDnew == 0)
        CProps := GetConfigStringArray(artefactcfg, "CProp");                    // CProps aus artefacts.cfg
        createtartefact := GetItemDescriptor(CInt(artefact.objtype));            // Artefakt konfigurieren
        createtartefact.cprops.insert("ArtefactID" , artefactID);               // ArtefaktID merken für Aktivierung
      EndIf

      createtartefact.cprops.insert("usable", 0);
      createtartefact.cprops.insert("Tooltip", CAscZ("unaktiviert"));

      ForEach CProp in CProps                                                   // CProps aus artefacts.cfg an das Artefakt pappen
        var cPropName := SplitWords(CProp)[1];
        var cPropNumber := CInt(SplitWords(CProp)[2]);
        createtartefact.cprops.insert(cPropName, cPropNumber);
      EndForEach

      CreateItemInBackpackPergon(who, createtartefact, 1);    // und dann createn
      SendSysMessagePergon(who, "Ihr stellt erfolgreich das Artefakt her", "You successfully create the artefact");
      PlaySoundEffect(who, 0x1E0);
    Else
      SendSysMessagePergon(who, "Ihr versagt beim Herstellen des Artefakts", "You fail to create the artefact!");
      PlaySoundEffect(who, 0x1EB);
    EndIf
    PerformAction(who, ANIM_CAST_AREA);

    ConsumePartsPergon(who, parts);                                  // ArtefactReags verbrauchen

    SleepMS(6000);
  EndFor
EndFunction // }}}

// herzustellendes Artfeact auswaehlen (neues Menügump)
Function ArtefactMenu(who, artefactBook, chooseAmount := 1) // {{{

  var magierklasse;
  var iDoffset;
  Case (artefactBook.objtype)
    UOBJ_ARTEFACTBOOK_MAGE:     magierklasse := MAGE_DESC;iDoffset:= MAGEIDOFFSET;

    UOBJ_ARTEFACTBOOK_CLERIC:   magierklasse := CLERIC_DESC;iDoffset:= CLERICIDOFFSET;

    UOBJ_ARTEFACTBOOK_NECRO:    magierklasse := NECRO_DESC;iDoffset:= NECROIDOFFSET;
  EndCase

  If(!magierklasse)
    syslog("FEHLER: falscher Aufruf von ArtefactMenu!");
    return 0;
  EndIf

  var artefactMask := GetArtefactMask(artefactBook);                    // Artefaktmaske (Welcher Spell ist im Buch?)
  Var artefacts:= ReadConfigFile(":newinscription:artefacts");

  var circles := {};   // Gibt es in dem Circle einen Spell?
  var artefactdata;  // Daten zum aktuellen Spell
  var anyArtefact;   // Gibt es in irgendein Circle einen Spell?
  var artefactID;    // Aktuelles Artefakt [eines Circles] (Schleifenzaehler)
  var circle;        // Aktueller Circle (Schleifenzaehler)

  // Fuer welche Circle existieren Spells?
  For (circle := 1; circle <= 8; circle += 1)
    For (artefactID := circle * 10 + 1; artefactID < circle * 10 + 8; artefactID += 1)
      If (artefactMask[artefactID])                     // Ist der Spell im Buch enthalten?
        circles[circle] := 1;                             // Dieser Circle enthaelt einen Spell
        anyArtefact := 1;                                 // Irgendein Circle enthaelt einen Spell
      EndIf
    EndFor
  EndFor

  If (!anyArtefact)
    SendSysMessagepergon(who, "Euer Artefaktbuch ist leer!", "Your artefactbook is empty!");
    return 0;
  EndIf

  var title := "Wählt das Artefakt, das Ihr herstellen wollt.";

  var size := { 4, 2 };         // Platz für 8 Einträge pro Zirkel
  var window := { 4.2, 1.6 };   // Größe etwas anpassen

  var defaultItem := 0;
  var defaultPage := 1;
  var amount := 1;
  var name := "";

  // Die Default-Daten aus dem Artefaktbuch holen

  var defaultprop := MGGetDefault(artefactBook, "Artefaction");
  If (defaultprop)
     defaultItem := defaultprop.objtype;
     amount := defaultprop.amount;
     name := defaultprop.name;
  EndIf

  // Wenn die Anzahl unerheblich ist... (z.B. SpellPanel)
  If (!chooseamount)
    amount := -1;
  EndIf

  var menus := array, content := array;

  // die Menüs aufbauen
  For (circle := 1; circle <= 8; circle += 1)
    If (circles[circle])
      // Name des Menüs
      menus.append(MakeRoman(circle)+". Zirkel");
      var submenu := array;
      // Alle Zauber eines Zirkels (die im Buch stehen) anzeigen
      For (artefactID := circle * 10 + 1; artefactID <= circle * 10  + 8; artefactID += 1)
        If (artefactMask[artefactID])                   // Ist der Spell im Buch enthalten?
          artefactData := artefacts[artefactID+iDoffset];    // Daten zum Spell holen

          If (artefactData.item == defaultItem)     // Wurde der Spruch letztes Mal schon geschrieben?
             defaultPage := menus.size();         // Dann muss dieser Zirkel als erstes angezeigt werden
          EndIf

          // Daten jedes Spells: Name, Icon und Objtype der Spruchrolle
          submenu.append(struct{ desc := artefactData.name, icon := artefactdata.graphic, objtype := artefactdata.item});
        EndIf
      EndFor
      content.append(submenu);
    EndIf
  EndFor

  // Menü in Gump umsetzen
  var width := 60 + window[1]*110;
  var height := 100+ window[2]*100 + MGMenuLines(menus, 0, width);
  var buttons := MGInitGump(width, height, amount, title);
  var startY := MGMenuLines(menus, defaultPage);
  var options := MGMenuEntrys(content, size[1], size[2], defaultItem, defaultPage, startY, MGumpUseTilePic,name);
  var menu := struct{ buttons := buttons, options := options };

  // Menü anzeigen
  var Auswahl_item := MGSelect(who, menu);
  If (!Auswahl_item)
    // weil wir nichts returnen, sagt der Aufrufer Artefaction Abbruch
    // SendSysMessagePergon(who, "Abbruch", "Abort");
    return;
  EndIf

  // Wenn die Anzahl wichtig ist... (z.B. Inscription)
  If (chooseamount)
    If (!Auswahl_item.amount || Auswahl_item.amount < 0)
      SendSysMessagePergon(who, "Abbruch", "Abort");
      return;
    EndIf

    If (Auswahl_item.amount > 10)
      Auswahl_item.amount := 10;
    EndIf
  EndIf

  MGSetDefault(artefactbook, "Artefaction", Auswahl_item);

  return Auswahl_item;
EndFunction // }}}

// Zu schreibenden Spruch auswaehlen (neues Menügump)
Function ProcessMenu(who, spellbook, chooseamount := 1) // {{{
  // Konstanten gemaess Magierklasse verteilen
  var magierklasse := 0; // Identifikator der globalen Variablen
  var min_spell_item;  // Minimale ItemID der Spezial-Spells
  var symbol_circle;   // Positionsangabe der Circle-Symbol-Items
  var symbol_spell;    // Positionsangabe des Spell-Symbol-Items
  Case (spellbook.objtype)
    UOBJ_SPELLBOOK_MAGE:   magierklasse := MAGE_DESC;
                           min_spell_item := MIN_MAGE_CAST_ITEM;
                           If (GetObjProperty(who, CPROP_NEW_SPELLS)) // Neue Spell-Grafiken anzeigen?
                             symbol_circle := GUMP_MAGE_CIRCLE-1;
                             symbol_spell := GUMP_MAGE_SPELL-1;
                           Else
                             symbol_circle := SYMBOL_OLD_CIRCLE-1;
                             symbol_spell := SYMBOL_OLD_SPELL-1;
                           EndIf
    UOBJ_SPELLBOOK_CLERIC: magierklasse := CLERIC_DESC;
                           min_spell_item := MIN_CLERIC_CAST_ITEM;
                           If (GetObjProperty(who, CPROP_NEW_SPELLS)) // Neue Spell-Grafiken anzeigen?
                             symbol_circle := GUMP_CLERIC_CIRCLE-1;
                             symbol_spell := GUMP_CLERIC_SPELL-1;
                           Else
                             symbol_circle := SYMBOL_OLD_CIRCLE-1;
                             symbol_spell := SYMBOL_OLD_SPELL-1;
                           EndIf
    UOBJ_SPELLBOOK_NECRO:  magierklasse := NECRO_DESC;
                           min_spell_item := MIN_NECRO_CAST_ITEM;
                           If (GetObjProperty(who, CPROP_NEW_SPELLS)) // Neue Spell-Grafiken anzeigen?
                             symbol_circle := GUMP_NECRO_CIRCLE-1;
                             symbol_spell := GUMP_NECRO_SPELL-1;
                           Else
                             symbol_circle := SYMBOL_OLD_CIRCLE-1;
                             symbol_spell := SYMBOL_OLD_SPELL-1;
                           EndIf
  EndCase

  // Auswahlmenue zusammenbauen
  If (magierklasse)
    var spellsmask := GetSpellsMask(spellbook, magierklasse);       // Spellmaske (Welcher Spell ist im Buch?)
    Var datafile:=OpenDataFile("::spelldata");
    var elem:=datafile.FindElement(magierklasse);
    var spellsindex := elem.getprop("SpIndex");   // Spelldaten sortiert nach Circle und Spellname
    var spellscircle := elem.getprop("SpCircle"); // Anfang des Circles im Index

    var circles := {}; // Gibt es in dem Circle einen Spell?
    var spelldata;   // Daten zum aktuellen Spell
    var anyspell;    // Gibt es in irgendein Circle einen Spell?
    var spellid;     // Aktueller Spell [eines Circles] (Schleifenzaehler)
    var circle;      // Aktueller Circle (Schleifenzaehler)

    // Fuer welche Circle existieren Spells?
    For (circle := 1; circle<spellscircle.size(); circle += 1)
      For (spellid := spellscircle[circle][1]; spellid<spellscircle[circle+1][1]; spellid := spellid+1)
        If (spellsmask[spellid])  // Ist der Spell im Buch enthalten?
          circles[circle] := 1;     // Dieser Circle enthaelt einen Spell
          anyspell := 1;            // Irgendein Circle enthaelt einen Spell
        EndIf
      EndFor
    EndFor

    // Auswahlmenue erzeugen (neues MenüGump)
    If (anyspell)
      var title := "Wählt den Spruch, den Ihr niederschreiben wollt.";

      var size := { 4, 2 };       // Platz für 8 Einträge pro Zirkel
      var window := { 4.2, 1.6 }; // Größe etwas anpassen

      var defaultItem := 0;
      var defaultPage := 1;
      var amount := 1;
      var name := "";

      // Die Default-Daten aus dem Zauberbuch holen
      If (spellbook)
        var defaultprop := MGGetDefault(spellbook, "Inscription");
        If (defaultprop)
          defaultItem := defaultprop.objtype;
          amount := defaultprop.amount;
          name := defaultprop.name;
        EndIf
      EndIf

      // Wenn die Anzahl unerheblich ist... (z.B. SpellPanel)
      If (!chooseamount)
        amount := -1;
      EndIf

      var menus := array, content := array;

      // die Menüs aufbauen
      For (circle := 1; circle<spellscircle.size(); circle += 1)
        If (circles[circle])

          // Name des Menüs
          menus.append(MakeRoman(circle)+". Zirkel");
          var submenu := array;

          // Alle Zauber eines Zirkels (die im Buch stehen) anzeigen
          For (spellid := spellscircle[circle][1]; spellid<spellscircle[circle+1][1]; spellid += 1)
            If (spellsmask[spellid]) // Ist der Spell im Buch enthalten?
              spelldata := spellsindex[spellid]; // Daten zum Spell holen

              If (spelldata[2]+min_spell_item-1 == defaultItem) // Wurde der Spruch letztes Mal schon geschrieben?
                defaultPage := menus.size(); // Dann muss dieser Zirkel als erstes angezeigt werden
              EndIf

              // Daten jedes Spells: Name, Icon und Objtype der Spruchrolle
              submenu.append(struct{ desc := spelldata[1], icon := spelldata[2]+symbol_spell, objtype := spelldata[2]+min_spell_item-1 });
            EndIf
          EndFor

          content.append(submenu);
        EndIf
      EndFor

      // Menü in Gump umsetzen
      var width := 60 + window[1]*110;
      var height := 100+ window[2]*100 + MGMenuLines(menus, 0, width);
      var buttons := MGInitGump(width, height, amount, title);
      var startY := MGMenuLines(menus, defaultPage);
      var options;
      If (GetObjProperty(who, CPROP_NEW_SPELLS)) // Neue Spell-Grafiken anzeigen?
        options := MGMenuEntrys(content, size[1], size[2], defaultItem, defaultPage, startY, MGumpUseGumpPic, name);
      Else
        options := MGMenuEntrys(content, size[1], size[2], defaultItem, defaultPage, startY, MGumpUseTilePic, name);
      EndIf
      var menu := struct{ buttons := buttons, options := options };

      // Menü anzeigen
      var Auswahl_item := MGSelect(who, menu);
      If (!Auswahl_item)
        // SendSysMessagePergon(who, "Abbruch", "Abort");
        return;
      EndIf

      // Wenn die Anzahl wichtig ist... (z.B. Inscription)
      If (chooseamount)
        If (!Auswahl_item.amount || Auswahl_item.amount < 0)
          SendSysMessagePergon(who, "Abbruch", "Abort");
          return;
        EndIf

        If (Auswahl_item.amount > 10)
          Auswahl_item.amount := 10;
        EndIf
      EndIf

      MGSetDefault(spellbook, "Inscription", Auswahl_item);

      return Auswahl_item;
    Else
        SendSysMessagePergon(who, "Euer Buch ist leer!");
    EndIf
  Else
    syslog("Spells: Dieses Script ist faelschlicherweise mit dem Item "+Lower(Hex(spellbook.objtype))+" verknuepft!");
  EndIf
EndFunction // }}}

// Altes Auswahlmenü
Function ProcessMenuOldStyle(who, spellbook) // {{{
  // Konstanten gemaess Magierklasse verteilen
  var magierklasse := 0; // Identifikator der globalen Variablen
  var min_spell_item;  // Minimale ItemID der Spezial-Spells
  var symbol_circle;   // Positionsangabe der Circle-Symbol-Items
  var symbol_spell;    // Positionsangabe des Spell-Symbol-Items
  Case (spellbook.objtype)
    UOBJ_SPELLBOOK_MAGE:   magierklasse := MAGE_DESC;
                           min_spell_item := MIN_MAGE_CAST_ITEM;
                           If (GetObjProperty(who, CPROP_NEW_SPELLS)) // Neue Spell-Grafiken anzeigen?
                             symbol_circle := SYMBOL_MAGE_CIRCLE-1;
                             symbol_spell := SYMBOL_MAGE_SPELL-1;
                           Else
                             symbol_circle := SYMBOL_OLD_CIRCLE-1;
                             symbol_spell := SYMBOL_OLD_SPELL-1;
                           EndIf
    UOBJ_SPELLBOOK_CLERIC: magierklasse := CLERIC_DESC;
                           min_spell_item := MIN_CLERIC_CAST_ITEM;
                           If (GetObjProperty(who, CPROP_NEW_SPELLS)) // Neue Spell-Grafiken anzeigen?
                             symbol_circle := SYMBOL_CLERIC_CIRCLE-1;
                             symbol_spell := SYMBOL_CLERIC_SPELL-1;
                           Else
                             symbol_circle := SYMBOL_OLD_CIRCLE-1;
                             symbol_spell := SYMBOL_OLD_SPELL-1;
                           EndIf
    UOBJ_SPELLBOOK_NECRO:  magierklasse := NECRO_DESC;
                           min_spell_item := MIN_NECRO_CAST_ITEM;
                           If (GetObjProperty(who, CPROP_NEW_SPELLS)) // Neue Spell-Grafiken anzeigen?
                             symbol_circle := SYMBOL_NECRO_CIRCLE-1;
                             symbol_spell := SYMBOL_NECRO_SPELL-1;
                           Else
                             symbol_circle := SYMBOL_OLD_CIRCLE-1;
                             symbol_spell := SYMBOL_OLD_SPELL-1;
                           EndIf
  EndCase

  // Auswahlmenue zusammenbauen
  If (magierklasse)
    var spellsmask := GetSpellsMask(spellbook, magierklasse);       // Spellmaske (Welcher Spell ist im Buch?)
    Var datafile:=OpenDataFile("::spelldata");
    var elem:=datafile.FindElement(magierklasse);
    var spellsindex := elem.getprop("SpIndex");   // Spelldaten sortiert nach Circle und Spellname
    var spellscircle := elem.getprop("SpCircle"); // Anfang des Circles im Index

    var circles := {}; // Gibt es in dem Circle einen Spell?
    var spelldata;   // Daten zum aktuellen Spell
    var anyspell;    // Gibt es in irgendein Circle einen Spell?
    var spellid;     // Aktueller Spell [eines Circles] (Schleifenzaehler)
    var circle;      // Aktueller Circle (Schleifenzaehler)

    // Fuer welche Circle existieren Spells?
    For (circle := 1; circle<spellscircle.size(); circle += 1)
      For (spellid := spellscircle[circle][1]; spellid<spellscircle[circle+1][1]; spellid += 1)
        If (spellsmask[spellid]) // Ist der Spell im Buch enthalten?
          circles[circle] := 1; // Dieser Circle enthaelt einen Spell
          anyspell := 1;        // Irgendein Circle enthaelt einen Spell
        EndIf
      EndFor
    EndFor

    // Circle-Auswahlmenue erzeugen
    If (anyspell)
      var menu := CreateMenu("Waehlt einen Zirkel!");

      For (circle := spellscircle.size()-1; circle>0; circle -= 1)
        If (circles[circle])
          AddMenuItem(menu, circle+symbol_circle, MakeRoman(circle)+". Zirkel");
        EndIf
      EndFor

      // Circle auswaehlen
      circle := SelectMenuItem2(who, menu);
      If (circle)
        circle := CInt(circle.objtype-symbol_circle);

        // Spell-Auswahlmenue erzeugen
        menu := CreateMenu("Waehlt einen Spruch!");

        // Spells fuer einzelnen Circle einfuegen
        For (spellid := spellscircle[circle][1]; spellid<spellscircle[circle+1][1]; spellid += 1)
          If (spellsmask[spellid]) // Ist der Spell im Buch enthalten?
            spelldata := spellsindex[spellid]; // Daten zum Spell holen
            AddMenuItem(menu, spelldata[2]+symbol_spell, spelldata[1]);
          EndIf
        EndFor

        // Spell auswaehlen
        spellid := SelectMenuItem2(who, menu);
        If (spellid)
          return (CInt(spellid.objtype-symbol_spell)+min_spell_item-1);
        EndIf
      EndIf
    Else
      SendSysMessagePergon(who, "Euer Buch ist leer!");
    EndIf
  Else
    syslog("Spells: Dieses Script ist faelschlicherweise mit dem Item "+Lower(Hex(spellbook.objtype))+" verknuepft!");
  EndIf
EndFunction // }}}

// MakeRoman - Roemische Zirkelnummern liefern
Function MakeRoman(str) // {{{
  Case (str)
    1: return("I");
    2: return("II");
    3: return("III");
    4: return("IV");
    5: return("V");
    6: return("VI");
    7: return("VII");
    8: return("VIII");
  EndCase
EndFunction // }}}
