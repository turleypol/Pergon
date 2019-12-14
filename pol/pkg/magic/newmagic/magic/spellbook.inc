///////////////////////////////////////////////////////////////////////////
// SpellBook Include v1.01 (2001/03/14)
//
// Author: Shinigami

// ToDo:
// - Sounds fuer Buch oeffnen/schliessen raussuchen
// - Rotes Band (verweist auf letzten benutzten Spell)
// - Flag, welches sagt, ob das Buch offen ist (ok, gibs schon), damit das
//   Buch nach dem wiedereinloggen automatisch angezeigt wird (minimiert...)
// - CheckForSpellInBook - das geht garantiert ueber die ID viel bequemer...

///////////////////////////////////////////////////////////////////////////
// Zauberbuch
//   UseSpellBook              - Doppelklick auf das Spellbook
//   ShowSpellBook             - Zeigt das Spellbook an
//
// SpellPanel
//   TestDemoSpellPanel        - Erzeugt und Testet ein Panel zur Demonstration
//   ShowSpellPanels           - Zeigt alle Spellpanels an
//   ShowSpellPanel            - Zeigt ein Spellpanel an
//   ChangeSpellPanelSize      - Groesse des SpellPanels und damit der Matrix aendern
//
// Initialisierung
//   InitSpellBook             - Initialisiert das Spellbook
//   InitMagierKlasse          - Magierklasse initialisieren
//   GetWholeSpellConfig       - Ermittelt die statischen Daten aller Spells
//   CheckSpellID              - Ueberprueft die tatsaechliche SpellID
//
// Sonstiges
//   CreateSpellBookInBackpack - Erzeugt das Zauberbuch im Backpack inkl. Fingerabdruck
//   PersonalizeSpellBook      - Zauberbuch mit Fingerabdruck versehen
//   CreateSpellInBook         - Erzeugt den Spruch im Zauberbuch
//   CheckForSpellInBook       - Ist ein Spruch im Zauberbuch eingetragen?
//   GetSpellIDfromObjType     - Ermittelt die SpellID zum Item
//   CheckSpellBookInBackpack  - Befindet sich das Zauberbuch im Backpack oder in der rechten Hand?
//   SearchSpellBookInBackpack - Zauberbuch im Backpack oder in der rechten Hand suchen
//   GetSpellsMask             - Holt die SpellsMask des Buches
//   ConvertSpellsInBook       - Konvertiert die Sprueche zur Spellsmask
//

///////////////////////////////////////////////////////////////////////////
// Benutzte CProp's - SpellBook
//   CInt(#Open)       - Ist das Buch geoeffnet?
//   CInt(#ErrorClock) - Wann wird "#Open" nach Fehlversuch entfernt?
//   CInt(Besitzer)    - Fingerabdruck (Serial des Besitzers)
//   Array(SpellsMask) - Spellmaske (Welcher Spell ist im Buch?)
//
// Benutzte CProp's - SpellPanel
//   Array(Data) - {X-Position, Y-Position, Sichtbar?, SpellMatrix}
//                   SpellMatrix = Zeilen{ Spalten{ SpellID }}

/////////////////////////////////////////////////////////////////////////////
// Ermittelte Datenmenge (2001/03/14)
//   Layout = 1578..19879 / Data = 373..1966

/////////////////
// Bibliotheken
/////////////////

Use uo;
Use os;
Use cfgfile;

/////////////
// Includes
/////////////

Include "spelldefs";
Include ":newspells:magicpergon";
Include ":newinscription:inscription";
Include "include/gumputil";
Include "include/string";
Include "include/objtype";
Include "include/pergonutil";
Include "include/modifyskill";

///////////////
// Konstanten
///////////////

Const SPELLID_KORREKTUR:=400;        // (Temporaer) Korrekturwert fuer gemeinsame SpellID

Const GUMP_HIDDENBTN_LEFT:= 2184;    // 8 Buttons fuer Wechsel vom Indexeintrag zur Spellseite (links)
Const GUMP_HIDDENBTN_RIGHT:=2192;    // 8 Buttons fuer Wechsel vom Indexeintrag zur Spellseite (rechts)
Const GUMP_EMPTY_EDIT:=     2152;    // Symbol fuer einen Leerplatz beim Editieren des Panels

Const DEFAULT_PANEL_X:=        30;   // Initialwerte eines neuen Panels
Const DEFAULT_PANEL_Y:=        30;
Const DEFAULT_PANEL_SPALTEN:=   6;
Const DEFAULT_PANEL_ZEILEN:=    2;
Const PANEL_GUMP_WIDTH:=       44;   // Ausmass eines Spellsymbols auf dem Panel
Const PANEL_GUMP_HEIGHT:=      44;

Const BTTN_GROSSES_BUCH:= 1;         // (Rueckgabewert) Wechsel aufs grosse Buch
Const BTTN_KLEINES_BUCH:= 2;         // (Rueckgabewert) Wechsel aufs kleine Buch
Const BTTN_EDIT:=         3;         // (Rueckgabewert) SpellPanel editieren
Const BTTN_X_POS:=        4;         // (Rueckgabewert) SpellPanel (Edit) X-Position
Const BTTN_Y_POS:=        5;         // (Rueckgabewert) SpellPanel (Edit) Y-Position
Const BTTN_ZEILEN:=       6;         // (Rueckgabewert) SpellPanel (Edit) Zeilen
Const BTTN_SPALTEN:=      7;         // (Rueckgabewert) SpellPanel (Edit) Spalten
Const BTTN_NEW_SPELLS:=   8;         // (Rueckgabewert) SpellPanel (Edit) Neue Spell-Grafiken anzeigen?
Const BTTN_TRANSPARENT:=  9;         // (Rueckgabewert) SpellPanel (Edit) Panel transparent anzeigen?
Const BTTN_SPELLS:=      10;         // (Rueckgabewert) Spellaufruf

Const BASE_X:=117;                   // Position des Buches
Const BASE_Y:=116;

///////////////
// Gump-Daten
///////////////
//
// 2220      - Zauberbuch (gross, aufgeklappt)
// 2225-2232 - Index-Shortcuts (die Ziffern im Kreis)
// 2234      - Zauberbuch (klein)
// 2235      - Seitenwechsel zurueck
// 2236      - Seitenwechsel vor
// 2240-2303 - Spellsymbole (klein)
// 2183      - Button zum Schliessen des grossen Buches                (neu)
// 2184-2199 - Button fuer den Wechsel vom Indexeintrag zur Spellseite (neu)
//
//   Spheremaker -> Copy_Gump_2220 dann Photoshop -> Paste dann MulView import
//       0, 99 -  28,120 -> 0x878 (2183)
//      61, 54 - 185, 67 -> 0x888 (2184)
//      61, 69 - 185, 82 -> 0x889 (2185)
//      61, 84 - 185, 97 -> 0x88a (2186)
//      61, 99 - 185,112 -> 0x88b (2187)
//      61,114 - 185,127 -> 0x88c (2188)
//      61,129 - 185,142 -> 0x88d (2189)
//      61,144 - 185,157 -> 0x88e (2190)
//      61,159 - 185,172 -> 0x88f (2191)
//     224, 54 - 348, 67 -> 0x890 (2192)
//     224, 69 - 348, 82 -> 0x891 (2193)
//     224, 84 - 348, 97 -> 0x892 (2194)
//     224, 99 - 348,112 -> 0x893 (2195)
//     224,114 - 348,127 -> 0x894 (2196)
//     224,129 - 348,142 -> 0x895 (2197)
//     224,144 - 348,157 -> 0x896 (2198)
//     224,159 - 348,172 -> 0x897 (2199)

// Schreibsysmbol - 0xfc1


////////////////////
//                //
//   Zauberbuch   //
//                //
////////////////////

/////////////////////////////////////////////////
// UseSpellBook - Doppelklick auf das Spellbook
/////////////////////////////////////////////////

Function UseSpellBook(who, spellbook)
  Var bookdesc; // Beschreibungen des Buches ermitteln
  Case (spellbook.objtype)
    UOBJ_SPELLBOOK_MAGE:   bookdesc:={{"Das Grimoire", "Dieses Grimoire", "euer Grimoire"},
                                      {"The Grimoire", "This Grimoire", "your Grimoire"}};
    UOBJ_SPELLBOOK_CLERIC: bookdesc:={{"Der Liber Castus", "Dieser Liber Castus", "euren Liber Castus"},
                                      {"The Liber Castus", "This Liber Castus", "your Liber Castus"}};
    UOBJ_SPELLBOOK_NECRO:  bookdesc:={{"Das paq mu'qaDmey", "Dieses paq mu'qaDmey", "euer paq mu'qaDmey"},
                                      {"The paq mu'qaDmey", "This paq mu'qaDmey", "your paq mu'qaDmey"}};
  EndCase

  // Befindet sich das Zauberbuch im Backpack oder in der rechten Hand?
  If (!CheckSpellBookInBackpack(who, spellbook, bookdesc))
    return;
  EndIf

  Var fingerabdruck := GetObjProperty(spellbook, "Besitzer");
  If (!fingerabdruck)
    // Buch noch nicht auf Besitzer festgeschrieben
    // SendSysMessagePergon(who,
    //   bookdesc[1][2]+" wurde fehlerhaft initialisiert. Vernichtet es!"
    // );
    // return;

    // nachholen
    PersonalizeSpellBook(who, spellbook);
    fingerabdruck := who.serial;
  EndIf

  If ((fingerabdruck == who.serial) Or (who.cmdlevel >= CMDLEVEL_SEER))
    // Ist das SpellBook bereits geoeffnet?
    Var open:=GetObjProperty(spellbook, "#Open");
    If (open) // Ja. Falls doch nicht, wird das Flag nach 5 Sekunden entfernt
      Var errorclock:=GetObjProperty(spellbook, "#ErrorClock");
      If (errorclock)
        If (errorclock>ReadGameClock())
          EraseObjProperty(spellbook, "#Open");
          open:=0; // Buch ist wohl doch nicht geoeffnet...
        EndIf
      EndIf
    EndIf

    // Ist das Buch nun geoeffnet?
    If (open) // Ja
      SendSysMessagePergon(who, bookdesc[1][1]+" ist bereits geoeffnet!", bookdesc[2][1]+" has been already opened.");
      SendSysMessagePergon(who, "Falls doch nicht, versucht es erneut...", "If not, try again...");

      SetObjProperty(spellbook, "#ErrorClock", ReadGameClock()+5);
    Else // Nein. Oeffnen...
      EraseObjProperty(spellbook, "#ErrorClock");
      SetObjProperty(spellbook, "#Open", 1);

      SendSysMessagePergon(who, bookdesc[1][1]+" wird geoeffnet.", bookdesc[2][1]+" is opened.");

      Set_Script_Option(SCRIPTOPT_NO_RUNAWAY, 1);
      ShowSpellBook(who, spellbook, bookdesc);

      // Test
      TestDemoSpellPanel(who, spellbook, bookdesc);

      EraseObjProperty(spellbook, "#Open");
    EndIf
  Else
    Var besitzer:=SystemFindObjectBySerial(fingerabdruck, SYSFIND_SEARCH_OFFLINE_MOBILES);
    If (besitzer)
      SendSysMessagePergon(who, bookdesc[1][2]+" gehoert "+besitzer.name+" und nicht Euch!");
      SysLog("WARNUNG: "+who.name+" wollte das Zauberbuch von "+besitzer.name+" ("+
        Lower(Hex(spellbook.serial))+") benutzen!");
    Else
      SendSysMessagePergon(who, bookdesc[1][2]+" hat keinen Besitzer mehr. Vernichtet es!");
      SysLog("WARNUNG: "+who.name+" wollte das Zauberbuch "+Lower(Hex(spellbook.serial))+" benutzen!");
    EndIf
  EndIf
EndFunction

///////////////////////////////////////////
// ShowSpellBook - Zeigt das Spellbook an
///////////////////////////////////////////

Function ShowSpellBook(who, spellbook, bookdesc)
  Var layout_small:={
                     "nodispose",                                    // Grundeinstellung
                     "nomove",
                     "page 0",                                       // Basisseite
                     "button 16 16 2234 2234 1 0 "+BTTN_GROSSES_BUCH //  Verweis auf grosses Buch
                    };
  // Konstanten gemaess Magierklasse verteilen
  Var magierklasse:=0; // Identifikator der globalen Variablen
  Var skillid;         // Skill zum Casten
  Var min_spell;       // Basis-SpellID zum Casten
  Var symbol_spell;    // Positionsangabe des Spell-Gumps
  Case (spellbook.objtype)
    UOBJ_SPELLBOOK_MAGE:   magierklasse:=MAGE_DESC;
                           skillid:=SKILLID_MAGIE;
                           min_spell:=MIN_MAGE_SPELL;
                           If (GetObjProperty(who, CPROP_NEW_SPELLS)) // Neue Spell-Grafiken anzeigen?
                             symbol_spell:=GUMP_MAGE_SPELL;
                           Else
                             symbol_spell:=GUMP_OLD_SPELL;
                           EndIf
    UOBJ_SPELLBOOK_CLERIC: magierklasse:=CLERIC_DESC;
                           skillid:=SKILLID_SEGNEN;
                           min_spell:=MIN_CLERIC_SPELL;
                           If (GetObjProperty(who, CPROP_NEW_SPELLS)) // Neue Spell-Grafiken anzeigen?
                             symbol_spell:=GUMP_CLERIC_SPELL;
                           Else
                             symbol_spell:=GUMP_OLD_SPELL;
                           EndIf
    UOBJ_SPELLBOOK_NECRO:  magierklasse:=NECRO_DESC;
                           skillid:=SKILLID_FLUCHEN;
                           min_spell:=MIN_NECRO_SPELL;
                           If (GetObjProperty(who, CPROP_NEW_SPELLS)) // Neue Spell-Grafiken anzeigen?
                             symbol_spell:=GUMP_NECRO_SPELL;
                           Else
                             symbol_spell:=GUMP_OLD_SPELL;
                           EndIf
  EndCase

  // Spellbook zusammenbauen
  If (magierklasse)
    Var datafile:=OpenDataFile("::spelldata");
    var elem:=datafile.FindElement(magierklasse);
    Var spellsindex:=elem.getprop("SpIndex");     // Spelldaten sortiert nach Circle und Spellname
    Var spellsreindex:=elem.getprop("SpReIndex"); // Umkehrliste (Zuordnung SpellID zu Pos. in Spelldaten)
    Var spellscircle:=elem.getprop("SpCircle");   // Anfang des Circles im Index

    // Endlosschleife, fuer Wechsel zwischen kleinem und grossem Buch
    Var bttn:=BTTN_GROSSES_BUCH;
    While (bttn==BTTN_GROSSES_BUCH)
      Var spellsmask:=GetSpellsMask(spellbook, magierklasse); // Spellmaske (Welcher Spell ist im Buch?)
      Var layout:=elem.getprop("SpLayout"); // Vorgefertigtes Layout
      Var data:=elem.getprop("SpData");     // Vorgefertigte Daten

      Var spellspage:=spellscircle[spellscircle.size()][2]; // Erste Seite der Spells im Buch
      Var datadescpos:=data.size(); // Startposition der Spellnamen in den Gump-Daten
      Var lastpageshown:=0; // Letzte erzeugte Gump-Seite
      Var pageshown; // Ist die Gump-Seite bereits gewechselt, also "sichtbar"?
      Var leftpage;  // Links oder rechts auf der Seite?
      Var pages:=0;  // Anzahl [und Position im Buch] der Spells
      Var bttnpic;   // Hilfsvariable fuer Gump-Button-Definitionen
      Var spellnr;   // Aktueller Spell auf der aktuellen Index-Seite
      Var spellid;   // Aktueller Spell [eines Circles] (Schleifenzaehler)
      Var circle;    // Aktueller Circle (Schleifenzaehler)
      Var page;      // Aktuelle Gump-Seite

      // Buch-Index mit Eintraegen fuellen
      For (circle:=1; circle<spellscircle.size(); circle+=1)
        page:=spellscircle[circle][2];     // Index-Seite fuer Circle ermitteln
        leftpage:=spellscircle[circle][3]; // Links oder rechts auf der Index-Seite?
        pageshown:=0;                      // Seitenwechsel am Anfang eines neuen Circles erzwingen
        spellnr:=0;                        // Neue Seite, also noch nichts eingetragen

        // Buch-Index fuer einzelnen Circle fuellen
        For (spellid:=spellscircle[circle][1]; spellid<spellscircle[circle+1][1]; spellid+=1)
          If (spellsmask[spellid]) // Ist der Spell im Buch enthalten?
            If (!pageshown) // Seitenwechsel einfuegen?
              pageshown:=1;

              If (lastpageshown<>page) // Echter Seitenwechsel noetig?
                lastpageshown:=page;
                layout.append("page "+page);
              EndIf
            EndIf

            // Indexeintrag des Spells einfuegen
            pages:=pages+1;
            If (leftpage) // Links oder rechts?
              bttnpic:=" "+(GUMP_HIDDENBTN_LEFT+spellnr);
              layout.append("button "+(BASE_X+61)+" "+(BASE_Y+54+spellnr*15)+bttnpic+bttnpic+
                " 0 "+(spellspage+CInt((pages-1)/2)+1));
              layout.append("croppedtext "+(BASE_X+61)+" "+(BASE_Y+50+spellnr*15)+" 123 15 0 "+data.size());
            Else
              bttnpic:=" "+(GUMP_HIDDENBTN_RIGHT+spellnr);
              layout.append("button "+(BASE_X+224)+" "+(BASE_Y+54+spellnr*15)+bttnpic+bttnpic+
                " 0 "+(spellspage+CInt((pages-1)/2)+1));
              layout.append("croppedtext "+(BASE_X+224)+" "+(BASE_Y+50+spellnr*15)+" 123 15 0 "+data.size());
            EndIf
            data.append(spellsindex[spellid][1]);

            // Zahl der Sprueche im Index und damit Seiten weiterzaehlen
            spellnr+=1;
            If (spellnr>7) // Seite vollgeschrieben?
              spellnr:=0;

              leftpage:=1-leftpage;
              If (leftpage) // Echter Seitenwechsel noetig?
                page+=1;
                pageshown:=0;
              EndIf
            EndIf
          EndIf
        EndFor
      EndFor

      // Spell-Beschreibungen (echte Spell-Seiten) erzeugen
      If (pages)
        pages:=spellspage+CInt((pages-1)/2)+1; // Gesamtzahl der Gump-Seiten ermitteln
        page:=spellspage;                      // Startseite holen

        If (!pageshown) // Seitenwechsel einfuegen?
          If (lastpageshown<>page) // Echter Seitenwechsel noetig?
            layout.append("page "+page);
          EndIf
        EndIf
        layout.append("button "+(BASE_X+321)+" "+(BASE_Y+8)+" 2236 2236 0 "+(page+1));

        Var spelldata; // Daten zum aktuellen Spell
        Var reagnr;    // Aktuelle Reagenzie (Schleifenzaehler)
        leftpage:=1;     // Neue Gump-Seite, also links gehts los

        // Ausformulierte Seiten der Spells einfuegen
        For (circle:=1; circle<spellscircle.size(); circle:=circle+1)
          bttnpic:=" "+(2224+circle); // Button fuer den Circle

          // Spells fuer einzelnen Circle einfuegen
          For (spellid:=spellscircle[circle][1]; spellid<spellscircle[circle+1][1]; spellid+=1)
            If (spellsmask[spellid]) // Ist der Spell im Buch enthalten?
              spelldata:=spellsindex[spellid]; // Daten zum Spell holen

              // Gump des Spells ermitteln
              bttnpic:=" "+(symbol_spell+spelldata[2]-1);

              // Spellseite zusammenbasteln
              If (leftpage) // Links oder rechts?
                // Seitengrundgeruest anlegen
                page+=1;
                layout.append("page "+page); // Seitenwechsel erzeugen
                layout.append("button "+(BASE_X+50)+" "+(BASE_Y+8)+" 2235 2235 0 "+(page-1));

                // Spellname, Spellsymbol und PowerWords erzeugen
                layout.append("htmlgump "+(BASE_X+85)+" "+(BASE_Y+12)+" 99 35 "+datadescpos+" 0 0");
                layout.append("button "+(BASE_X+61)+" "+(BASE_Y+54)+" "+bttnpic+bttnpic+" 1 0 "+
                  (BTTN_SPELLS+spelldata[2]));
                layout.append("htmlgump "+(BASE_X+109)+" "+(BASE_Y+58)+" 76 35 "+data.size()+" 0 0");
                data.append(spelldata[3]);

                // Reagenzien erzeugen
                For (reagnr:=1; reagnr<=spelldata[5].size(); reagnr+=1)
                  layout.append("text "+(BASE_X+61)+" "+(BASE_Y+95+reagnr*15)+" 0 "+spelldata[5][reagnr]);
                EndFor

                // Verknuepfung zum Index (Circle-Basis-Seite) erzeugen
                bttnpic:=" "+(2224+circle);
                layout.append("button "+(BASE_X+58)+" "+(BASE_Y+175)+bttnpic+bttnpic+" 0 "+
                  spellscircle[circle][2]);
              Else
                // Seitengrundgeruest vervollstaendigen
                If (page<pages) // Gibt es eine nachfolgende Seite?
                  layout.append("button "+(BASE_X+321)+" "+(BASE_Y+8)+" 2236 2236 0 "+(page+1));
                EndIf

                // Spellname, Spellsymbol und PowerWords erzeugen
                layout.append("htmlgump "+(BASE_X+224)+" "+(BASE_Y+12)+" 99 35 "+datadescpos+" 0 0");
                layout.append("button "+(BASE_X+224)+" "+(BASE_Y+54)+" "+bttnpic+bttnpic+" 1 0 "+
                  (BTTN_SPELLS+spelldata[2]));
                layout.append("htmlgump "+(BASE_X+272)+" "+(BASE_Y+58)+" 76 35 "+data.size()+" 0 0");
                data.append(spelldata[3]);

                // Reagenzien erzeugen
                For (reagnr:=1; reagnr<=spelldata[5].size(); reagnr+=1)
                  layout.append("text "+(BASE_X+224)+" "+(BASE_Y+95+reagnr*15)+" 0 "+spelldata[5][reagnr]);
                EndFor

                // Verknuepfung zum Index (Circle-Basis-Seite) erzeugen
                bttnpic:=" "+(2224+circle);
                layout.append("button "+(BASE_X+332)+" "+(BASE_Y+175)+bttnpic+bttnpic+" 0 "+
                  spellscircle[circle][2]);
              EndIf

              datadescpos+=1; // Position des naechsten Spellnamen in den Gump-Daten ermitteln
              leftpage:=1-leftpage;       // Neuer Spell, neue Seite
            EndIf
          EndFor
        EndFor
      EndIf

      // Spellbook aufrufen und Ergebnisse bearbeiten
      bttn:=CInt(SendDialogGump(who, layout, data)[0]);
      If (bttn>BTTN_SPELLS) // Spell casten
        If (CheckSpellBookInBackpack(who, spellbook, bookdesc)) // Befindet sich das Zauberbuch im Backpack oder in der rechten Hand?
          Var spellid:=spellsindex[spellsreindex[bttn-BTTN_SPELLS]][2]; // Daten zum Spell
          If (spellid<=MAX_MAIN_SPELL)
            CastSpell(who, spellid+SPELLID_KORREKTUR, skillid);
          Else
            CastSpell(who, spellid+min_spell-COUNT_MAIN_SPELLS-1, skillid);
          EndIf
        EndIf

        bttn:=CInt(SendDialogGump(who, layout_small, {})[0]); // Buch minimieren
      ElseIf (bttn==BTTN_KLEINES_BUCH)
        bttn:=CInt(SendDialogGump(who, layout_small, {})[0]); // Buch minimieren
      EndIf
    EndWhile
  Else
    SysLog("Spells: Dieses Script ist faelschlicherweise mit dem Item "+Lower(Hex(spellbook.objtype))+" verknuepft!");
  EndIf
EndFunction

////////////////////
//                //
//   SpellPanel   //
//                //
////////////////////

////////////////////////////////////////////////////////////////////////
// TestDemoSpellPanel - Erzeugt und Testet ein Panel zur Demonstration
////////////////////////////////////////////////////////////////////////

Function TestDemoSpellPanel(who, spellbook, bookdesc)
  Var spellpanelanzahl:=0;

  ForEach spellpanel in EnumerateItemsInContainer(spellbook)
    If (spellpanel.objtype==UOBJ_SPELLPANEL) // Ist es ein SpellPanel?
      spellpanelanzahl+=1;
    EndIf
  EndForEach

  // Wenn es noch kein Panel gibt, dann ein neues anlegen
  If (!spellpanelanzahl)
    Var matrix:={}; // Matrix der Spells auf dem Panel (Zeile x Spalte)

    // Matrix zur Demonstration fuellen
    For zeile:=1 To DEFAULT_PANEL_ZEILEN
      Var spellzeile:={}; // Eine komplette SpellZeile

      // SpellZeile fuellen
      For spalte:=1 To DEFAULT_PANEL_SPALTEN
        spellzeile.append(0);
      EndFor

      matrix.append(spellzeile);
    EndFor

    // Panel anlegen
    Var spellpanel:=CreateItemInContainerPergon(spellbook, UOBJ_SPELLPANEL);
    If (spellpanel)
      SetObjProperty(spellpanel, "Data", {"Spellpanel", DEFAULT_PANEL_X, DEFAULT_PANEL_Y, 1, 1, matrix});
    EndIf
  EndIf

  // Alle Spellpanels anzeigen
  ShowSpellPanels(who, spellbook, bookdesc);
EndFunction

////////////////////////////////////////////////
// ShowSpellPanels - Zeigt alle Spellpanels an
////////////////////////////////////////////////

Function ShowSpellPanels(who, spellbook, bookdesc)
  ForEach spellpanel in EnumerateItemsInContainer(spellbook)
    If (spellpanel.objtype==UOBJ_SPELLPANEL) // Ist es ein SpellPanel?
      Var data:=GetObjProperty(spellpanel, "Data");
      If (data) // Sind die Einstellungen vorhanden?
        If (data.size()==4) // Alter Stil ohne Name und Transparenz?
          data:={"Spellpanel", data[1], data[2], data[3], 1, data[4]};
          SetObjProperty(spellpanel, "Data", data);
        EndIf

        If (data[4]) // Soll das Panel angezeigt werden?
          // ToDo: 'n extra Script draus machen... so nimmt er immer nur das erste, was er findet
          ShowSpellPanel(who, spellpanel, bookdesc);
        EndIf
      Else
        SysLog("FEHLER: SpellPanel "+Lower(Hex(spellpanel.serial))+" wurde nicht richtig konfiguriert!");
      EndIf
    EndIf
  EndForEach
EndFunction

/////////////////////////////////////////////
// ShowSpellPanel - Zeigt ein Spellpanel an
/////////////////////////////////////////////

Function ShowSpellPanel(who, spellpanel, bookdesc)
	//ClilocNr der Spells
  Var SpellCliloc:=dictionary{
  "401"->600000,"402"->600001,"403"->600002,"404"->600003,"405"->600004,"406"->600005,"407"->600006,"408"->600007,"409"->600008,"410"->600009,
  "411"->600010,"412"->600011,"413"->600012,"414"->600013,"415"->600014,"416"->600015,"417"->600016,"418"->600017,"419"->600018,"420"->600019,
  "421"->600020,"422"->600021,"423"->600022,"424"->600023,"425"->600024,"426"->600025,"427"->600026,"428"->600027,"429"->600028,"430"->600029,
  "431"->600030,"432"->600031,
  "101"->600032,"102"->600033,"103"->600034,"104"->600035,"105"->600036,"106"->600037,"107"->600038,"108"->600039,"109"->600040,"110"->600041,
  "111"->600042,"112"->600043,"113"->600044,"114"->600045,"115"->600046,"116"->600047,"117"->600048,"118"->600049,"119"->600050,"120"->600051,
  "121"->600052,"122"->600053,"123"->600054,"124"->600055,"125"->600056,"126"->600057,"127"->600058,"128"->600059,"129"->600060,"130"->600061,
  "131"->600062,"132"->600063,
  "201"->600064,"202"->600065,"203"->600066,"204"->600067,"205"->600068,"206"->600069,"207"->600070,"208"->600071,"209"->600072,"210"->600073,
  "211"->600074,"212"->600075,"213"->600076,"214"->600077,"215"->600078,"216"->600079,"217"->600080,"218"->600081,"219"->600082,"220"->600083,
  "221"->600084,"222"->600085,"223"->600086,"224"->600087,"225"->600088,"226"->600089,"227"->600090,"228"->600091,"229"->600092,"230"->600093,
  "231"->600094,"232"->600095,
  "301"->600096,"302"->600097,"303"->600098,"304"->600099,"305"->600100,"306"->600101,"307"->600102,"308"->600103,"309"->600104,"310"->600105,
  "311"->600106,"312"->600107,"313"->600108,"314"->600109,"315"->600110,"316"->600111,"317"->600112,"318"->600113,"319"->600114,"320"->600115,
  "321"->600116,"322"->600117,"323"->600118,"324"->600119,"325"->600120,"326"->600121,"327"->600122,"328"->600123,"329"->600124,"330"->600125,
  "331"->600126,"332"->600127};
  
  Var spellbook:=spellpanel.container; // Zugehoeriges Spellbook
  If (spellbook) // Befindet sich das Panel "im" Spellbook?
    // Konstanten gemaess Magierklasse verteilen
    Var magierklasse:=0; // Identifikator der globalen Variablen
    Var skillid;         // Skill zum Casten
    Var min_spell;       // Basis-SpellID zum Casten
    Var symbol_spell;    // Positionsangabe des Spell-Gumps
    Case (spellbook.objtype)
      UOBJ_SPELLBOOK_MAGE:   magierklasse:=MAGE_DESC;
                             skillid:=SKILLID_MAGIE;
                             min_spell:=MIN_MAGE_SPELL;
                             If (GetObjProperty(who, CPROP_NEW_SPELLS)) // Neue Spell-Grafiken anzeigen?
                               symbol_spell:=GUMP_MAGE_SPELL;
                             Else
                               symbol_spell:=GUMP_OLD_SPELL;
                             EndIf
      UOBJ_SPELLBOOK_CLERIC: magierklasse:=CLERIC_DESC;
                             skillid:=SKILLID_SEGNEN;
                             min_spell:=MIN_CLERIC_SPELL;
                             If (GetObjProperty(who, CPROP_NEW_SPELLS)) // Neue Spell-Grafiken anzeigen?
                               symbol_spell:=GUMP_CLERIC_SPELL;
                             Else
                               symbol_spell:=GUMP_OLD_SPELL;
                             EndIf
      UOBJ_SPELLBOOK_NECRO:  magierklasse:=NECRO_DESC;
                             skillid:=SKILLID_FLUCHEN;
                             min_spell:=MIN_NECRO_SPELL;
                             If (GetObjProperty(who, CPROP_NEW_SPELLS)) // Neue Spell-Grafiken anzeigen?
                               symbol_spell:=GUMP_NECRO_SPELL;
                             Else
                               symbol_spell:=GUMP_OLD_SPELL;
                             EndIf
    EndCase

    // Spellpanel zusammenbauen
    If (magierklasse)
      Var datafile:=OpenDataFile("::spelldata");
      var elem:=datafile.FindElement(magierklasse);
      Var spellsindex:=elem.getprop("SpIndex");     // Spelldaten sortiert nach Circle und Spellname
      Var spellsreindex:=elem.getprop("SpReIndex"); // Umkehrliste (Zuordnung SpellID zu Pos. in Spelldaten)

      // Endlosschleife
      Var edit:=0; // Produktives Panel, oder Panel editieren?
      Var bttn:=BTTN_EDIT;
      While ((bttn==BTTN_EDIT) Or edit)
        // Daten des Panels holen
        Var paneldata:=GetObjProperty(spellpanel, "Data");
        Var panelname:=paneldata[1];   // Name des Panels
        Var x_pos:=paneldata[2];       // X-Position der linken, oberen Ecke
        Var y_pos:=paneldata[3];       // Y-Position der linken, oberen Ecke
        Var transparent:=paneldata[5]; // Panel transparent anzeigen?
        Var matrix:=paneldata[6];      // SpellMatrix
        Var zeilen:=matrix.size();     // Anzahl der SpellZeilen
        Var spalten:=matrix[1].size(); // Anzahl der SpellSpalten

        Var spellsmask:=GetSpellsMask(spellbook, magierklasse); // Spellmaske (Welcher Spell ist im Buch?)
        Var spelldata; // Daten zum aktuellen Spell
        Var bttnpic;   // Hilfsvariable fuer Gump-Button-Definitionen
        Var spellid;   // Aktueller Spell [eines Circles] (Schleifenzaehler)

        // Layout und Data aufbauen
        Var layout:={"nodispose", "nomove", "page 0"};
        Var data:={};

        If (transparent==1) // Panel transparent anzeigen?
          layout.append("resizepic "+x_pos+" "+y_pos+" 5054 "+(spalten*PANEL_GUMP_WIDTH+8)+" "+(zeilen*PANEL_GUMP_HEIGHT+8));
          layout.append("checkertrans "+(x_pos+4)+" "+(y_pos+4)+" "+(spalten*PANEL_GUMP_WIDTH)+" "+(zeilen*PANEL_GUMP_HEIGHT));
        ElseIf (transparent<>2)
          layout.append("resizepic "+x_pos+" "+y_pos+" 5054 "+(spalten*PANEL_GUMP_WIDTH+8)+" "+(zeilen*PANEL_GUMP_HEIGHT+8));
        EndIf

        If (!edit) // Produktives Panel, oder Panel editieren?
          // Clickflaeche des Edit-Buttons
          layout.append("button "+(x_pos+spalten*PANEL_GUMP_WIDTH-19)+" "+
            (y_pos+zeilen*PANEL_GUMP_HEIGHT-19)+" 2703 2703 1 0 "+BTTN_EDIT);
          // Spellpanel zusammenbasteln
          For zeile:=1 To zeilen
            For spalte:=1 To spalten
              spellid:=matrix[zeile][spalte];
              If (spellid)
                If (spellsmask[spellid]) // Ist der Spell im Buch enthalten?
                  spelldata:=spellsindex[spellid][2]; // Daten zum Spell holen

                  // Gump des Spells ermitteln
                  bttnpic:=" "+(symbol_spell+spelldata-1);

                  // Spellsymbol erzeugen
                  layout.append("button "+(x_pos+(spalte-1)*PANEL_GUMP_WIDTH+4)+" "+(y_pos+
                    (zeile-1)*PANEL_GUMP_HEIGHT+4)+" "+bttnpic+bttnpic+" 1 0 "+(BTTN_SPELLS+spelldata));
                  
                  // Tooltip anzeigen
                  Var spellid:=spellsindex[spellsreindex[spelldata]][2]; // Daten zum Spell
                  If (spellid<=MAX_MAIN_SPELL)
                    layout.append("tooltip "+SpellCliloc[CStr(spellid+SPELLID_KORREKTUR)]); //Tooltip anzeigen
                  Else
                    layout.append("tooltip "+SpellCliloc[CStr(spellid+min_spell-COUNT_MAIN_SPELLS-1)]); //Tooltip anzeigen
                  EndIf
                EndIf
              EndIf
            EndFor
          EndFor

          // Optische Flaeche des Edit-Buttons
          layout.append("button "+(x_pos+spalten*PANEL_GUMP_WIDTH-19)+" "+
            (y_pos+zeilen*PANEL_GUMP_HEIGHT-19)+" 2703 2703 1 0 "+BTTN_EDIT);

          // Endlosschleife
          bttn:=BTTN_SPELLS+1;
          While (bttn>BTTN_SPELLS)
            // Spellpanel aufrufen und Ergebnisse bearbeiten
            bttn:=CInt(SendDialogGump(who, layout, data)[0]);
            If (bttn>BTTN_SPELLS) // Spell casten
              If (CheckSpellBookInBackpack(who, spellbook, bookdesc)) // Befindet sich das Zauberbuch im Backpack oder in der rechten Hand?
                Var spellid:=spellsindex[spellsreindex[bttn-BTTN_SPELLS]][2]; // Daten zum Spell
                If (spellid<=MAX_MAIN_SPELL)
                  CastSpell(who, spellid+SPELLID_KORREKTUR, skillid);
                Else
                  CastSpell(who, spellid+min_spell-COUNT_MAIN_SPELLS-1, skillid);
                EndIf
              EndIf
            ElseIf (bttn==BTTN_EDIT)
              edit:=1; // Edit-Modus betreten
            EndIf
          EndWhile
        Else
          // Position des Parameterfeldes bestimmen
          Var basis_x;
          If (x_pos<220)
            basis_x:=x_pos+(spalten*PANEL_GUMP_WIDTH+8)+10;
          Else
            basis_x:=30;
          EndIf

          // Layoutfeld zusammenbasteln
          layout.append("resizepic "+basis_x+" 30 2620 210 172");
          layout.append("checkertrans "+(basis_x+5)+" 35 200 162");
          layout.append("text "+(basis_x+10)+" 40 5 0");
          layout.append("text "+(basis_x+10)+" 68 40 1");
          layout.append("text "+(basis_x+130)+" 68 5 2");
          layout.append("button "+(basis_x+175)+" 65 2640 2641 1 0 "+BTTN_X_POS);
          layout.append("text "+(basis_x+10)+" 88 40 3");
          layout.append("text "+(basis_x+130)+" 88 5 4");
          layout.append("button "+(basis_x+175)+" 85 2640 2641 1 0 "+BTTN_Y_POS);
          layout.append("text "+(basis_x+10)+" 108 40 5");
          layout.append("text "+(basis_x+130)+" 108 5 6");
          layout.append("button "+(basis_x+175)+" 105 2640 2641 1 0 "+BTTN_ZEILEN);
          layout.append("text "+(basis_x+10)+" 128 40 7");
          layout.append("text "+(basis_x+130)+" 128 5 8");
          layout.append("button "+(basis_x+175)+" 125 2640 2641 1 0 "+BTTN_SPALTEN);
          layout.append("text "+(basis_x+10)+" 148 40 9");
          layout.append("text "+(basis_x+130)+" 148 5 10");
          layout.append("button "+(basis_x+175)+" 145 2640 2641 1 0 "+BTTN_NEW_SPELLS);
          layout.append("text "+(basis_x+10)+" 168 40 11");
          layout.append("text "+(basis_x+130)+" 168 5 12");
          layout.append("button "+(basis_x+175)+" 165 2640 2641 1 0 "+BTTN_TRANSPARENT);

          // Datenfeld zusammenbasteln
          data:={"Spellpanel", "X-Position", CStr(x_pos), "Y-Position", CStr(y_pos),
            "Zeilen", CStr(zeilen), "Spalten", CStr(spalten), "Neue Symbole?"};
          If (GetObjProperty(who, CPROP_NEW_SPELLS)) // Neue Spell-Grafiken anzeigen?
            data.append("ja");
          Else
            data.append("nein");
          EndIf
          data.append("Transparent?");
          If (transparent==1) // Panel transparent anzeigen?
            data.append("ja");
          ElseIf (transparent==2)
            data.append("total");
          Else
            data.append("nein");
          EndIf

          // Spellpanel zusammenbasteln
          For zeile:=1 To zeilen
            For spalte:=1 To spalten
              spellid:=matrix[zeile][spalte];
              If (spellid)
                If (spellsmask[spellid]) // Ist der Spell im Buch enthalten?
                  spelldata:=spellsindex[spellid][2]; // Daten zum Spell holen

                  // Gump des Spells ermitteln
                  bttnpic:=" "+(symbol_spell+spelldata-1);

                  // Spellsymbol erzeugen
                  layout.append("button "+(x_pos+(spalte-1)*PANEL_GUMP_WIDTH+4)+" "+(y_pos+
                    (zeile-1)*PANEL_GUMP_HEIGHT+4)+bttnpic+bttnpic+" 1 0 "+(BTTN_SPELLS+spalte+(zeile-1)*spalten));
                EndIf
              Else
                // Spellsymbol erzeugen
                layout.append("button "+(x_pos+(spalte-1)*PANEL_GUMP_WIDTH+12)+" "+(y_pos+
                  (zeile-1)*PANEL_GUMP_HEIGHT+12)+" "+GUMP_EMPTY_EDIT+" "+GUMP_EMPTY_EDIT+" 1 0 "+
                  (BTTN_SPELLS+spalte+(zeile-1)*spalten));
              EndIf
            EndFor
          EndFor

          // Spellpanel aufrufen und Ergebnisse bearbeiten
          bttn:=CInt(SendDialogGump(who, layout, data)[0]);
          If (bttn>BTTN_SPELLS) // Spell casten
            Var zeile:=(bttn-BTTN_SPELLS+spalten-1)/spalten;
            Var spalte:=(bttn-BTTN_SPELLS)-(zeile-1)*spalten;

            // Neuen Spruch fuer das Panel auswaehlen (ItemID)
            //Var panelscrollid:=ProcessMenu(who, spellbook);
            Var panelscrollid:=ProcessMenu(who, spellbook, 0).objtype;
            If (panelscrollid) // Neue Matrix schreiben
              matrix[zeile][spalte]:=GetSpellIDfromObjType(spellbook, panelscrollid);
            Else
              matrix[zeile][spalte]:=0;
            EndIf
            SetObjProperty(spellpanel, "Data", {panelname, x_pos, y_pos, 1, transparent, matrix});
          ElseIf (bttn) // Parameter aendern
            Var description;
            Var maxval;

            If (bttn==BTTN_NEW_SPELLS)
              If (GetObjProperty(who, CPROP_NEW_SPELLS)) // Neue Spell-Grafiken anzeigen?
                SetObjProperty(who, CPROP_NEW_SPELLS, 0);
              Else
                SetObjProperty(who, CPROP_NEW_SPELLS, 1);
              EndIf
            ElseIf (bttn==BTTN_TRANSPARENT)
              If (transparent==1) // Panel transparent anzeigen?
                SetObjProperty(spellpanel, "Data", {panelname, x_pos, y_pos, 1, 2, matrix});
              ElseIf (transparent==2)
                SetObjProperty(spellpanel, "Data", {panelname, x_pos, y_pos, 1, 0, matrix});
              Else
                SetObjProperty(spellpanel, "Data", {panelname, x_pos, y_pos, 1, 1, matrix});
              EndIf
            Else
              Case (bttn)
                BTTN_X_POS:   description:="X-Position";maxval:=2048;
                BTTN_Y_POS:   description:="Y-Position";maxval:=2048;
                BTTN_ZEILEN:  description:="Zeilen";    maxval:=10;
                BTTN_SPALTEN: description:="Spalten";   maxval:=10;
              EndCase

              Var val:=CInt(SendTextEntryGump(who, "Bitte den neuen Wert fuer '"+
                description+"' eingeben!", TE_CANCEL_ENABLE, TE_STYLE_NUMERICAL, maxval));
              If (val)
                Case (bttn)
                  BTTN_X_POS:   SetObjProperty(spellpanel, "Data", {panelname, val, y_pos, 1, transparent, matrix});
                  BTTN_Y_POS:   SetObjProperty(spellpanel, "Data", {panelname, x_pos, val, 1, transparent, matrix});
                  BTTN_ZEILEN:  ChangeSpellPanelSize(spellpanel, panelname, x_pos, y_pos, transparent, val, spalten, matrix);
                  BTTN_SPALTEN: ChangeSpellPanelSize(spellpanel, panelname, x_pos, y_pos, transparent, zeilen, val, matrix);
                EndCase
              Else
                SendSysMessagePergon(who, "Abbruch", "Abort");
              EndIf
            EndIf
          Else
            edit:=0; // Edit-Modus verlassen
            bttn:=BTTN_EDIT;
          EndIf
        EndIf
      EndWhile
    Else
      SysLog("Spells: Dieses Script ist faelschlicherweise mit dem Item "+Lower(Hex(spellbook.objtype))+" verknuepft!");
    EndIf
  Else
    SysLog("Spells: Das Panel "+Lower(Hex(spellpanel.serial))+" befindet sich nicht 'in' einem Spellbook!");
  EndIf
EndFunction

////////////////////////////////////////////////////////////////////////////////
// ChangeSpellPanelSize - Groesse des SpellPanels und damit der Matrix aendern
////////////////////////////////////////////////////////////////////////////////

Function ChangeSpellPanelSize(spellpanel, panelname, x_pos, y_pos, transparent, zeilen_new, spalten_new, matrix_old)
  Var matrix:={};
  Var spellid;

  // Neue Matrix initialisieren
  For zeile:=1 To zeilen_new
    Var spellzeile:={}; // Eine komplette SpellZeile

    // SpellZeile fuellen
    For spalte:=1 To spalten_new
      spellid:=matrix_old[zeile][spalte];
      If (spellid)
        spellzeile.append(spellid); // Wert aus der alten Matrix uebernehmen
      Else
        spellzeile.append(0);
      EndIf
    EndFor

    matrix.append(spellzeile);
  EndFor

  SetObjProperty(spellpanel, "Data", {panelname, x_pos, y_pos, 1, transparent, matrix});
EndFunction

/////////////////////////
//                     //
//   Initialisierung   //
//                     //
/////////////////////////

////////////////////////////////////////////////
// InitSpellBook - Initialisiert das Spellbook
////////////////////////////////////////////////
//TODO: Ist das wirklich alles nˆtig?
Function InitSpellBook()
  BroadcastPergon("Magic-Pergon Spellbook wird initialisiert...");
  SysLog("Magic-Pergon Spellbook wird initialisiert...");

  // Magierklassen initialisieren
  If (InitMagierKlasse(MAGE_DESC) And InitMagierKlasse(CLERIC_DESC) And
     InitMagierKlasse(NECRO_DESC))
    BroadcastPergon("Magic-Pergon Spellbook wurde initialisiert.");
    SysLog("Magic-Pergon Spellbook wurde initialisiert.");
  Else
    BroadcastPergon("Magic-Pergon Spellbook konnte nicht initialisiert werden!");
    SysLog("Magic-Pergon Spellbook konnte nicht initialisiert werden!");
  EndIf
EndFunction

///////////////////////////////////////////////////
// InitMagierKlasse - Magierklasse initialisieren
///////////////////////////////////////////////////

Function InitMagierKlasse(magierklasse)
  Var spells:=GetWholeSpellConfig(magierklasse); // Statische Daten aller Spells holen
  If (spells)
    // Gump-Layout definieren
    Var layout:={"nodispose",
      "page 0",
      "gumppic "+BASE_X+" "+BASE_Y+" 2220",
      "button "+BASE_X+" "+(BASE_Y+99)+" 2183 2183 1 0 "+BTTN_KLEINES_BUCH};

    Var spellsdata; // Spelldaten der Spells, getrennt nach Circle
    Var min_spell;  // SpellID-Korrekturwert
    Var data;       // Gump-Data

    // Anzahl der Circle, Circle-Beschreibungen und SpellID-Korrekturwert festlegen
    Case (magierklasse)
      MAGE_DESC:   spellsdata:={{}, {}, {}, {}, {}, {}, {}, {}}; // Nur 8 Circle
                   data:={
                     "INDEX",          // Index-Seite 1..8
                     "Erster Zirkel",
                     "Zweiter Zirkel",
                     "Dritter Zirkel",
                     "Vierter Zirkel",
                     "Fuenfter Zirkel",
                     "Sechster Zirkel",
                     "Siebter Zirkel",
                     "Achter Zirkel"};
                   min_spell:=MIN_MAGE_SPELL;
      CLERIC_DESC: spellsdata:={{}, {}, {}, {}, {}, {}, {}, {}}; // Nur 8 Circle
                   data:={
                     "INDEX",          // Index-Seite 1..8
                     "Erster Zirkel",
                     "Zweiter Zirkel",
                     "Dritter Zirkel",
                     "Vierter Zirkel",
                     "Fuenfter Zirkel",
                     "Sechster Zirkel",
                     "Siebter Zirkel",
                     "Achter Zirkel"};
                   min_spell:=MIN_CLERIC_SPELL;
      NECRO_DESC:  spellsdata:={{}, {}, {}, {}, {}, {}, {}, {}}; // Nur 8 Circle
                   data:={
                     "INDEX",          // Index-Seite 1..8
                     "Erster Zirkel",
                     "Zweiter Zirkel",
                     "Dritter Zirkel",
                     "Vierter Zirkel",
                     "Fuenfter Zirkel",
                     "Sechster Zirkel",
                     "Siebter Zirkel",
                     "Achter Zirkel"};
                   min_spell:=MIN_NECRO_SPELL;
    EndCase

    Var datareagpos:=data.size()-1; // Startposition der Reagenzien in den Gump-Daten
    Var spellsreagents:={}; // Liste der Reagenzien, synchron zu den Gump-Daten
    Var spellsmask:={}; // Spellmaske (Welcher Spell ist im Buch?) im Voraus generieren
    Var circle; // Circle des Spells
    Var class;  // Zuordnung des Spells ("All", "Mage", "Cleric" oder "Necro")
    Var desc;   // Beschreibung des Spells

    // Spelldaten ermitteln und getrennt nach Circle einordnen
    ForEach spell in spells
      // SpellID ermitteln
      Var spellid:=spell.spellid;
      If (spellid>=SPELLID_KORREKTUR)
        spellid:=spellid-SPELLID_KORREKTUR;
      EndIf

      // Spell gemaess Spellnummer den allgemeinen oder speziellen Spells zuordnen
      If (spellid<=MAX_MAIN_SPELL)
        class:="All";
      Else
        class:=magierklasse;
      EndIf

      // Passt der Class-Eintrag zur SpellID?
      If (spell.class==class)
        // Gehoert der Spell einem gueltigen Circle an?
        circle:=CInt(spell.circle);
        If ((1<=circle) And (circle<=spellsdata.size()))
          // Ist die Spell-Beschreibung gueltig?
          desc:=CStr(spell.desc);
          If ((desc[1, 8]=="Spruch "+CChr(34)) And (desc[Len(desc), 1]==CChr(34)))
            // Verweisliste der Reagenzien erstellen
            Var reagents:={};
            ForEach reagent in GetConfigStringArray(spell, "PergonReag")
              If (reagent in spellsreagents)
                // Position der Reagenzie raussuchen
                Var index:=1;
                While (spellsreagents[index]<>reagent)
                  index+=1;
                EndWhile

                reagents.append(index+datareagpos); // Index der Verweisliste hinzufuegen
              Else
                data.append(reagent); // Reagenzie in die Gump-Daten aufnehmen
                spellsreagents.append(reagent);

                reagents.append(spellsreagents.size()+datareagpos); // Index der Verweisliste hinzufuegen
              EndIf
            EndForEach

            // Luecke zwischen speziellem und allgemeinem Spell schliessen
            If (spellid>=min_spell)
              spellid:=spellid-min_spell+COUNT_MAIN_SPELLS+1;
            EndIf

            // Spelldaten, nach Circle sortiert, eintragen
            //   Beschreibung, SpellID, PowerWords, Script, Reagenzienverweis
            spellsdata[circle].append({desc[9, Len(desc)-9], spellid,
              GetConfigString(spell, "Power"+magierklasse), spell.script, reagents});
          Else
            SysLog("Spells: Fehlerhafte 'Desc' in Spell "+spell.spellid+"!");
          EndIf
        Else
          SysLog("Spells: Fehlerhafter 'Circle' in Spell "+spell.spellid+"!");
        EndIf
      Else
        SysLog("Spells: Fehlerhafte 'Class' in Spell "+spell.spellid+"!");
      EndIf

      spellsmask.append(0); // Spellmaske erweitern
    EndForEach

    Var spellscirclebase:={}; // Bei welchem Eintrag faengt welcher Circle an?
    Var spellsindex:={}; // Spelldaten sortiert nach Circle und Spellname
    Var base:=1; // Bei welchem Eintrag fing der aktuelle Circle an?

    // Spelldaten innerhalb der Circle [nicht] sortieren und Gesamtliste bilden
    For (circle:=1; circle<=spellsdata.size(); circle+=1)
      // Spelldaten innerhalb des Circle [nicht] sortieren
      ForEach spell in (spellsdata[circle]) // SortMultiArrayByIndexNotCase(spellsdata[circle], 1)
        spellsindex.append(spell);
      EndForEach

      spellscirclebase.append(base); // Position des ersten Spells des Circles speichern
      base:=base+spellsdata[circle].size();
    EndFor
    spellscirclebase.append(base); // Position hinter der des letzten Spells speichern

    // Umkehrtliste zur Gesamtliste bilden
    Var spellsreindex:={}; // Umkehrliste (Zuordnung SpellID zu Pos. in Spelldaten)
    Var spellid; // Aktuelle SpellNummer (Schleifenzaehler)
    For (spellid:=1; spellid<=spellsindex.size(); spellid+=1)
      spellsreindex[spellsindex[spellid][2]]:=spellid;
    EndFor

    Var spellcirclepages:={}; // Anzahl der Index-Seiten je Circle
    Var spellscircle:={}; // Anfang des Circles im Index
    Var leftpage:=1; // Links oder rechts auf der Seite?
    Var circlepage;  // Aktuelle Seite des Circles (Schleifenzaehler)
    Var page:=0;     // Aktuelle Index-Seite im Gump

    // Verteilung der Circle im Buch-Index ermitteln
    For (circle:=1; circle<spellscirclebase.size(); circle+=1)
      // Anzahl der Index-Seiten je Circle ermitteln
      spellcirclepages.append(CInt((spellscirclebase[circle+1]-spellscirclebase[circle]-1)/8)+1);

      // Anfang des Circles im Index speichern
      //   Position des ersten Spells des Circles, Gump-Startseite, links oder rechts
      If (leftpage) // Links oder rechts?
        spellscircle.append({spellscirclebase[circle], page+1, 1});
      Else
        spellscircle.append({spellscirclebase[circle], page, 0});
      EndIf

      // Index-Seiten "durchzaehlen"
      For (circlepage:=1; circlepage<=spellcirclepages[circle]; circlepage+=1)
        If (leftpage) // Echter Seitenwechsel noetig?
          page+=1;
        EndIf

        leftpage:=1-leftpage;
      EndFor
    EndFor
    // Position hinter der des letzten Spells und Seite hinter der letzten Index-Seite speichern
    spellscircle.append({spellscirclebase[spellscirclebase.size()], page});

    leftpage:=1; // Links oder rechts auf der Seite?   (Wert initialisieren)
    page:=0;     // Aktuelle Index-Seite im Gump       (Wert initialisieren)

    // Alle Index-Seiten zusammenbauen
    For (circle:=1; circle<spellscirclebase.size(); circle+=1)
      For (circlepage:=1; circlepage<=spellcirclepages[circle]; circlepage+=1)
        If (leftpage) // Links oder rechts?
          // Seitengrundgeruest anlegen
          page+=1;
          layout.append("page "+page);

          // Circle-"ShortButtons" an der Buchunterseite im Index erzeugen
          layout.append("button "+(BASE_X+58)+" "+(BASE_Y+175)+" 2225 2225 0 "+spellscircle[1][2]);
          layout.append("button "+(BASE_X+93)+" "+(BASE_Y+175)+" 2226 2226 0 "+spellscircle[2][2]);
          layout.append("button "+(BASE_X+130)+" "+(BASE_Y+175)+" 2227 2227 0 "+spellscircle[3][2]);
          layout.append("button "+(BASE_X+164)+" "+(BASE_Y+175)+" 2228 2228 0 "+spellscircle[4][2]);
          layout.append("button "+(BASE_X+227)+" "+(BASE_Y+175)+" 2229 2229 0 "+spellscircle[5][2]);
          layout.append("button "+(BASE_X+260)+" "+(BASE_Y+175)+" 2230 2230 0 "+spellscircle[6][2]);
          layout.append("button "+(BASE_X+297)+" "+(BASE_Y+175)+" 2231 2231 0 "+spellscircle[7][2]);
          layout.append("button "+(BASE_X+332)+" "+(BASE_Y+175)+" 2232 2232 0 "+spellscircle[8][2]);

          // Button zum Wechsel auf die Vorseite erzeugen
          If (!((circle==1) And (circlepage==1)) )
            layout.append("button "+(BASE_X+50)+" "+(BASE_Y+8)+" 2235 2235 0 "+(page-1));
          EndIf

          // Seitentitel und Circle-Beschreibungen erzeugen
          layout.append("text "+GumpTextMid(data[1], BASE_X+50, 151)+" "+(BASE_Y+12)+" 1111 0");
          layout.append("text "+(BASE_X+61)+" "+(BASE_Y+31)+" 2100 "+circle);
        Else
          // Button zum Wechsel auf die Folgeseite erzeugen
          If (!((circle==spellscirclebase.size()-1) And (circlepage==spellcirclepages[circle])))
            layout.append("button "+(BASE_X+321)+" "+(BASE_Y+8)+" 2236 2236 0 "+(page+1));
          EndIf

          // Seitentitel und Circle-Beschreibungen erzeugen
          layout.append("text "+GumpTextMid(data[1], BASE_X+204, 152)+" "+(BASE_Y+12)+" 1111 0");
          layout.append("text "+(BASE_X+224)+" "+(BASE_Y+31)+" 2100 "+circle);
        EndIf

        leftpage:=1-leftpage;
      EndFor
    EndFor

    // Alle voraus"berechneten" Daten speichern
    Var datafile:=OpenDataFile("::spelldata");
    var elem:=datafile.CreateElement(magierklasse);
    elem.setprop("SpIndex",spellsindex);     // Spelldaten sortiert nach Circle und Spellname
    elem.setprop("SpReIndex",spellsreindex); // Umkehrliste (Zuordnung SpellID zu Pos. in Spelldaten)
    elem.setprop("SpCircle",spellscircle);   // Anfang des Circles im Index
    elem.setprop("SpMask",spellsmask);       // Spellmaske (Welcher Spell ist im Buch?)
    elem.setprop("SpLayout",layout);         // Vorgefertigtes Gump-Layout
    elem.setprop("SpData",data);             // Vorgefertigte Gump-Daten
    Return (1);
  Else
    SysLog("  Klasse '"+magierklasse+"' konnte nicht initialisiert werden!");
  EndIf
EndFunction

//////////////////////////////////////////////////////////////////////
// GetWholeSpellConfig - Ermittelt die statischen Daten aller Spells
//////////////////////////////////////////////////////////////////////

Function GetWholeSpellConfig(magierklasse)
  // Spell-Konfigurationen einlesen
  Var spellsfile:=ReadConfigFile("::spells");
  If (spellsfile)
    Var spells:={}; // Statische Daten der Spells
    Var spellid; // Aktueller Spell (Schleifenzaehler)

    // Statische Daten der allgemeinen Spells zusammenstellen
    For (spellid:=MIN_MAIN_SPELL; spellid<=MAX_MAIN_SPELL; spellid+=1)
      spells.append(CheckSpellID(spellsfile[spellid+SPELLID_KORREKTUR], spellid+SPELLID_KORREKTUR));
    EndFor

    // Statische Daten der speziellen Spells zusammenstellen
    Case (magierklasse)
      MAGE_DESC:   For (spellid:=MIN_MAGE_SPELL; spellid<=MAX_MAGE_SPELL; spellid:=spellid+1)
                     spells.append(CheckSpellID(spellsfile[spellid], spellid));
                   EndFor
      CLERIC_DESC: For (spellid:=MIN_CLERIC_SPELL; spellid<=MAX_CLERIC_SPELL; spellid:=spellid+1)
                     spells.append(CheckSpellID(spellsfile[spellid], spellid));
                   EndFor
      NECRO_DESC:  For (spellid:=MIN_NECRO_SPELL; spellid<=MAX_NECRO_SPELL; spellid:=spellid+1)
                     spells.append(CheckSpellID(spellsfile[spellid], spellid));
                   EndFor
    EndCase

    Return (spells);
  Else
    SysLog("Spells: Kann die Datei 'spells.cfg' nicht oeffnen !");
  EndIf
EndFunction

/////////////////////////////////////////////////////////
// CheckSpellID - Ueberprueft die tatsaechliche SpellID
/////////////////////////////////////////////////////////

Function CheckSpellID(spellconfig, spellid)
  If (CInt(spellconfig.spellid)<>spellid)
    SysLog("Spells: Fehlerhafter 'SpellID' in Spell "+spellid+"!");
  EndIf

  Return (spellconfig);
EndFunction

///////////////////
//               //
//   Sonstiges   //
//               //
///////////////////

///////////////////////////////////////////////////////////////////////////////////////
// CreateSpellBookInBackpack - Erzeugt das Zauberbuch im Backpack inkl. Fingerabdruck
///////////////////////////////////////////////////////////////////////////////////////

Function CreateSpellBookInBackpack(who, bookobjtype)
  Var book:=CreateItemInBackpackPergon(who, bookobjtype, 1);
  If (book)
    PersonalizeSpellBook(who, book); // Zauberbuch mit Fingerabdruck versehen

    Return (book);
  EndIf
EndFunction

/////////////////////////////////////////////////////////////////
// PersonalizeSpellBook - Zauberbuch mit Fingerabdruck versehen
/////////////////////////////////////////////////////////////////

Function PersonalizeSpellBook(who, book)
  If (GetObjProperty(book, "Besitzer"))
    // nur einmalig markieren lassen
    return;
  EndIf

  SetObjProperty(book, "Besitzer", who.serial); // Fingerabdruck
  Var datafile:=OpenDataFile("::spelldata");
  var elem;
  Case (book.objtype) // Spellmaske (Welcher Spell ist im Buch?)
    UOBJ_SPELLBOOK_MAGE:   elem:=datafile.FindElement(MAGE_DESC);
                           SetObjProperty(book, "SpellsMask", elem.getprop("SpMask"));
    UOBJ_SPELLBOOK_CLERIC: elem:=datafile.FindElement(CLERIC_DESC);
                           SetObjProperty(book, "SpellsMask", elem.getprop("SpMask"));
    UOBJ_SPELLBOOK_NECRO:  elem:=datafile.FindElement(NECRO_DESC);
                           SetObjProperty(book, "SpellsMask", elem.getprop("SpMask"));
  EndCase
EndFunction

/////////////////////////////////////////////////////////
// CreateSpellInBook - Erzeugt den Spruch im Zauberbuch
/////////////////////////////////////////////////////////

Function CreateSpellInBook(spellbook, scrollobjtype)
  Var magierklasse:=0; // Identifikator der globalen Variablen
  Var min_spell_item;  // Minimale ItemID der Spezial-Spells
  Var max_spell_item;  // Maximale ItemID der Spezial-Spells
  Case (spellbook.objtype)
    UOBJ_SPELLBOOK_MAGE:   magierklasse:=MAGE_DESC;
                           min_spell_item:=MIN_MAGE_SPELL_ITEM;
                           max_spell_item:=MAX_MAGE_SPELL_ITEM;
    UOBJ_SPELLBOOK_CLERIC: magierklasse:=CLERIC_DESC;
                           min_spell_item:=MIN_CLERIC_SPELL_ITEM;
                           max_spell_item:=MAX_CLERIC_SPELL_ITEM;
    UOBJ_SPELLBOOK_NECRO:  magierklasse:=NECRO_DESC;
                           min_spell_item:=MIN_NECRO_SPELL_ITEM;
                           max_spell_item:=MAX_NECRO_SPELL_ITEM;
  EndCase

  // Spruch eintragen
  If (magierklasse)
    Var datafile:=OpenDataFile("::spelldata");
    var elem:=datafile.FindElement(magierklasse);
    Var spellsreindex:=elem.getprop("SpReIndex"); // Umkehrliste (Zuordnung SpellID zu Pos. in Spelldaten)

    Var spellsmask:=GetObjProperty(spellbook, "SpellsMask"); // Spellmaske (Welcher Spell ist im Buch?)
    If (!spellsmask)
      spellsmask:=elem.getprop("SpMask");

      SysLog("FEHLER: Zauberbuch "+Lower(Hex(spellbook.serial))+" hat keine gueltige SpellsMask!");
    EndIf

    // Spruch in Spellmaske eintragen
    If ((min_spell_item<=scrollobjtype) And (scrollobjtype<=max_spell_item))
      spellsmask[spellsreindex[scrollobjtype-min_spell_item+1]]:=1;
    EndIf

    SetObjProperty(spellbook, "SpellsMask", spellsmask);
  Else
    SysLog("Spells: Dieses Script ist faelschlicherweise mit dem Item "+Lower(Hex(spellbook.objtype))+" verknuepft!");
  EndIf
EndFunction

////////////////////////////////////////////////////////////////////
// CheckForSpellInBook - Ist ein Spruch im Zauberbuch eingetragen?
////////////////////////////////////////////////////////////////////
//
// ToDo: das geht garantiert auch ueber die ID viel bequemer... naja

Function CheckForSpellInBook(spellbook, scrollobjtype)
  Var magierklasse:=0; // Identifikator der globalen Variablen
  Var min_spell_item;  // Minimale ItemID der Spezial-Spells
  Var max_spell_item;  // Maximale ItemID der Spezial-Spells
  Case (spellbook.objtype)
    UOBJ_SPELLBOOK_MAGE:   magierklasse:=MAGE_DESC;
                           min_spell_item:=MIN_MAGE_SPELL_ITEM;
                           max_spell_item:=MAX_MAGE_SPELL_ITEM;
    UOBJ_SPELLBOOK_CLERIC: magierklasse:=CLERIC_DESC;
                           min_spell_item:=MIN_CLERIC_SPELL_ITEM;
                           max_spell_item:=MAX_CLERIC_SPELL_ITEM;
    UOBJ_SPELLBOOK_NECRO:  magierklasse:=NECRO_DESC;
                           min_spell_item:=MIN_NECRO_SPELL_ITEM;
                           max_spell_item:=MAX_NECRO_SPELL_ITEM;
  EndCase

  // Spruch suchen
  If (magierklasse)
    Var datafile:=OpenDataFile("::spelldata");
    var elem:=datafile.FindElement(magierklasse);
    Var spellsreindex:=elem.getprop("SpReIndex"); // Umkehrliste (Zuordnung SpellID zu Pos. in Spelldaten)

    Var spellsmask:=GetObjProperty(spellbook, "SpellsMask"); // Spellmaske (Welcher Spell ist im Buch?)
    If (!spellsmask)
      spellsmask:=elem.getprop("SpMask");

      SysLog("FEHLER: Zauberbuch "+Lower(Hex(spellbook.serial))+" hat keine gueltige SpellsMask!");
    EndIf

    // Spruch in Spellmaske suchen
    If ((min_spell_item<=scrollobjtype) And (scrollobjtype<=max_spell_item))
      Return (spellsmask[spellsreindex[scrollobjtype-min_spell_item+1]]);
    EndIf
  Else
    SysLog("Spells: Dieses Script ist faelschlicherweise mit dem Item "+Lower(Hex(spellbook.objtype))+" verknuepft!");
  EndIf
EndFunction

///////////////////////////////////////////////////////////
// GetSpellIDfromObjType - Ermittelt die SpellID zum Item
///////////////////////////////////////////////////////////

Function GetSpellIDfromObjType(spellbook, scrollobjtype)
  Var magierklasse:=0; // Identifikator der globalen Variablen
  Var min_spell_item;  // Minimale ItemID der Spezial-Spells
  Var max_spell_item;  // Maximale ItemID der Spezial-Spells

  Case (spellbook.objtype)
    UOBJ_SPELLBOOK_MAGE:   magierklasse:=MAGE_DESC;
                           min_spell_item:=MIN_MAGE_CAST_ITEM;
                           max_spell_item:=MAX_MAGE_CAST_ITEM;
    UOBJ_SPELLBOOK_CLERIC: magierklasse:=CLERIC_DESC;
                           min_spell_item:=MIN_CLERIC_CAST_ITEM;
                           max_spell_item:=MAX_CLERIC_CAST_ITEM;
    UOBJ_SPELLBOOK_NECRO:  magierklasse:=NECRO_DESC;
                           min_spell_item:=MIN_NECRO_CAST_ITEM;
                           max_spell_item:=MAX_NECRO_CAST_ITEM;
  EndCase

  // SpruchID ermitteln
  If (magierklasse)
    Var datafile:=OpenDataFile("::spelldata");
    var elem:=datafile.FindElement(magierklasse);
    Var spellsreindex:=elem.getprop("SpReIndex"); // Umkehrliste (Zuordnung SpellID zu Pos. in Spelldaten)

    // SpruchID ermitteln
    If ((min_spell_item<=scrollobjtype) And (scrollobjtype<=max_spell_item))
      Return (spellsreindex[scrollobjtype-min_spell_item+1]);
    EndIf
  Else
    SysLog("Spells: Dieses Script ist faelschlicherweise mit dem Item "+Lower(Hex(spellbook.objtype))+" verknuepft!");
  EndIf
EndFunction

//////////////////////////////////////////////////////////////////////////////////////////////////
// CheckSpellBookInBackpack - Befindet sich das Zauberbuch im Backpack oder in der rechten Hand?
//////////////////////////////////////////////////////////////////////////////////////////////////

Function CheckSpellBookInBackpack(who, spellbook, bookdesc)
  Var basiscontainer:=spellbook.container; // Zauberbuch muss sich im Backpack oder in der rechten Hand befinden
  While (basiscontainer.container)
    basiscontainer:=basiscontainer.container;
  EndWhile

  If ((basiscontainer.serial==who.serial) Or (GetEquipmentByLayer(who, LAYER_HAND1).serial==spellbook.serial)) // Hab ich das Zauberbuch bei mir?
    If (AccessiblePergon(who, spellbook))
      Return (1); // Japp
    Else
      SendSysMessagePergon(who, "Ihr kommt nicht an "+bookdesc[1][3]+" heran!");
    EndIf
  Else
    SendSysMessagePergon(who, bookdesc[1][1]+" muss sich im Backpack oder in eurer Hand befinden!");
  EndIf
EndFunction

///////////////////////////////////////////////////////////////////////////////////////
// SearchSpellBookInBackpack - Zauberbuch im Backpack oder in der rechten Hand suchen
///////////////////////////////////////////////////////////////////////////////////////

Function SearchSpellBookInBackpack(who, bookobjtype)
  // zunaechst nur bereits personalisierte Buecher suchen
  var book := SearchSpellBookInBackpackReal(who, bookobjtype);
  If (book)
    // Erfolg, geschafft
    return book;
  EndIf

  // ansonsten gucken, ob personalisierbare Buecher da sind
  return SearchSpellBookInBackpackReal(who, bookobjtype, 1);
EndFunction

Function SearchSpellBookInBackpackReal(who, bookobjtype, personalize := 0)
  Var fingerabdruck;

  // rechte Hand ist auch erlaubt
  Var iteminhand := GetEquipmentByLayer(who, LAYER_HAND1);
  If (iteminhand)
    If (iteminhand.objtype==bookobjtype)
      // Darf ich das Zauberbuch benutzen?
      fingerabdruck := GetObjProperty(iteminhand, "Besitzer");

      // ggf. Buch auf Nutzer registrieren
      If (!fingerabdruck and personalize)
        PersonalizeSpellBook(who, iteminhand);
        fingerabdruck := who.serial;
      EndIf

      If (fingerabdruck)
        If ((fingerabdruck == who.serial) Or (who.cmdlevel >= CMDLEVEL_SEER))
          // ein benutzbares Zauberbuch gefunden, fertig
          return iteminhand;
        EndIf
      EndIf
    EndIf
  EndIf

  ForEach book in FindSubstance(who.backpack,bookobjtype,1,0,FINDSUBSTANCE_FIND_ALL)
    // Darf ich das Zauberbuch benutzen?
    fingerabdruck:=GetObjProperty(book, "Besitzer");

    // ggf. Buch auf Nutzer registrieren
    If (!fingerabdruck and personalize)
      PersonalizeSpellBook(who, iteminhand);
      fingerabdruck := who.serial;
    EndIf

    If (fingerabdruck)
      If ((fingerabdruck == who.serial) Or (who.cmdlevel >= CMDLEVEL_SEER))
        // ein benutzbares Zauberbuch gefunden, fertig
        return book;
      EndIf
    EndIf
  EndForEach
EndFunction

///////////////////////////////////////////////////
// GetSpellsMask - Holt die SpellsMask des Buches
///////////////////////////////////////////////////

Function GetSpellsMask(spellbook, magierklasse)
  Var spellsmask:=GetObjProperty(spellbook, "SpellsMask"); // Spellmaske (Welcher Spell ist im Buch?)
  If (!spellsmask)
    Var datafile:=OpenDataFile("::spelldata");
    var elem:=datafile.FindElement(magierklasse);
    spellsmask:=elem.getprop("SpMask");
    SetObjProperty(spellbook, "SpellsMask", spellsmask);

    If (ConvertSpellsInBook(spellbook)) // Altes Buch konvertieren?
      spellsmask:=GetObjProperty(spellbook, "SpellsMask");
    Else
      SysLog("FEHLER: Zauberbuch "+Lower(Hex(spellbook.serial))+" hat keine gueltige SpellsMask!");
    EndIf
  EndIf

  Return (spellsmask);
EndFunction

//////////////////////////////////////////////////////////////////
// ConvertSpellsInBook - Konvertiert die Sprueche zur Spellsmask
//////////////////////////////////////////////////////////////////

Function ConvertSpellsInBook(spellbook)
  Var result:=0;

  ForEach spellitem in EnumerateItemsInContainer(spellbook)
    If (spellitem.objtype<>UOBJ_SPELLPANEL) // Alles was kein Panel ist umwandeln
      CreateSpellInBook(spellbook, spellitem.objtype);
      DestroyItem(spellitem);
      result:=1; // Es wurde konvertiert, also kein Fehler...
    EndIf
  EndForEach

  Return (result);
EndFunction

// scrollbar ran (umschaltbar... also von horizontal nach vertikal) (0xfa ff.)
// auﬂerdem kleine buttons (‰hnlich wie scrollbar), die das teil verkleinern/-groessern (alle 4 richtungen)

// 0x13be            - Panel-Hintergrund
// 0xfa5/6 + 0xfae/f - Umschalten des ScrollBars (horizontal <-> vertikal)
// 0xfb7/8           - "Ok"-Button

// 0xfab/c           anpassen des fensters

// 0xa5c - edit-symbol fuer panel
// 0xa5d - edit-symbol fuer panel (viel besser eigentlich)
// 0xa8f

// 0x1b58 - Panel-Symbole       0x1b97
