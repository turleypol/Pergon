///////////////////////////////////////////////////////////////////////////////////////////
//
//    Artefaktbuch Include
//
//      2005-10-23  -  Seppel
//      2005-28-12  -  Seppel - Rollen mehrfach eintragbar
//
///////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
//
//  Funktionen
//
//  CreateNewBookInBackpack       - Neues Buch im Backpack erzeugen mit serial
//  CreateNewBlankBookInBackpack  - Neues Buch im Backpack erzeugen ohne serial
//  PersonalizeArtefactBook       - Serial an Buch vermerken
//  AddArtefaktToBook             - Artefakt ins Buch eintragen
//  CheckForArtefactInBook        - Ist Artefakt schon im Buch?
//  UseArtefactBook               - Buch öffnen (Rezepte ansehen)
//  GetArtefactMask               - Artefaktmaske zurückgeben
//  InitializeArtefactMask        - Initialisiert Artefaktmaske mit Nullen
//  IsItemArtefactBook            - Überprüft ob Item Artefaktbuch ist
//  GetArtefactIDFromScroll       - liefert ArtefaktID zum Gegenstand
//  GetIndexEntryFromArtefactID   - AfIndex Eintrag von ArtefactID holen
//  GetRandomArtefactIDFromCircle - liefert zufällige ArtefaktID aus Zirkel
//  InitGlobalArtefactProps       - Initialisiert die Golbale Prop AfIndex(enthält infos über Artefakte)
//
///////////////////////////////////////////////////////////////////////////////////

/////////////////
// Bibliotheken
/////////////////

Use uo;
Use os;
Use cfgfile;

//////////////////////////
//  Includes
//////////////////////////

Include "include/pergonutil";
Include "include/objtype";
Include "include/modifyskill";
Include ":newmagic:spelldefs";
Include "artefactdefs";


//////////////////////////////////////////////////////////////////////////////////
//  CreateNewBookInBackpack   -   Neues Buch im Backpack erzeugen (mit serial)
//////////////////////////////////////////////////////////////////////////////////

Function CreateNewBookInBackpack(who, bookobjtype)
  Var book:= CreateNewBlankBookInBackpack(who, bookobjtype);
  PersonalizeArtefactBook(who, book);
  If (book)
    Return(book);
  EndIf
  Return 0;
EndFunction

//////////////////////////////////////////////////////////////////////////////////
//  CreateNewBlankBookInBackpack   -   Neues Buch im Backpack erzeugen (ohne serial)
//////////////////////////////////////////////////////////////////////////////////

Function CreateNewBlankBookInBackpack(who, bookobjtype)
  Var artefactBooks:= {UOBJ_ARTEFACTBOOK_MAGE, UOBJ_ARTEFACTBOOK_CLERIC, UOBJ_ARTEFACTBOOK_NECRO};
  If ((bookobjtype in artefactBooks) && who)
    Var book:= CreateItemInBackpackPergon(who, bookobjtype);
    InitializeArtefactMask(book);
    Return(book);
  EndIf
  Return 0;
EndFunction

//////////////////////////////////////////////////////////////////////////////////
//  PersonalizeArtefactBook   -   serial am Buch vermerken
//////////////////////////////////////////////////////////////////////////////////

Function PersonalizeArtefactBook(who, book)
  If(book && who)
    SetObjProperty(book, "owner", who.serial);
    Return(book);
  EndIf
  Return 0;
EndFunction

//////////////////////////////////////////////////////////////////////////////////
//  UseArtefactBook   -   Doppelklick aufs Artefaktbuch
//////////////////////////////////////////////////////////////////////////////////

Function UseArtefactBook(who, book)
  If(GetObjProperty(book, "owner"))
    SendSysMessagePergon(who, "Ich bin ein Artefaktbuch und gehoere " + SystemFindObjectBySerial(GetObjProperty(book, "owner")).name);
  Else
    SendSysMessagePergon(who, "Ich bin ein Artefaktbuch und gehoere niemanden");
  EndIf
  SendSysMessagePergon(who, "Hier gibts spaeter mal ne Uebersicht ueber alle enthaltenen Artefakte und vielleicht sogar nen Spellpanel :)");
  // später mal Übersicht über alle eingetragenen Rezpte mit Reagenzien und so
EndFunction

//////////////////////////////////////////////////////////////////////////////////
//  AddArtefactToBook   -   Rezept in Buch eintragen
//////////////////////////////////////////////////////////////////////////////////

Function AddArtefactToBook(who, book, artefactScroll)

  If (!IsItemArtefactBook(book))
    SysLog("FEHLER: AddArtefactToBook falsch aufgerufen (kein Artefaktbuch)!");
    Return 0;
  EndIf

  Var artefactID:= GetArtefactIDFromScroll(artefactScroll);
  If(!artefactID)
    SysLog("FEHLER: GetArtefactIDFromScroll mit falschem objtype aufgerufen!");
    Return 0;
  EndIf

  Var artefactMask:= GetArtefactMask(book);                           // eingetragene Artefakte holen

  Case (book.objtype)                                                 // was fuer ein Buch wars?
    UOBJ_ARTEFACTBOOK_MAGE:
      If((MINMAGEARTEFACTSCROLL <= artefactScroll.objtype) && (artefactScroll.objtype <= MAXMAGEARTEFACTSCROLL))            // ist es auch eine sinnvolle Rolle für das Buch?
         artefactID-= 400;         
         If (artefactMask[artefactID] >= 100)                                                                                 // ist das Artefakt schon zu oft drin?
           SendSysMessagePergon(who, "Ihr koennt das Artefakt nicht noch einmal eintragen!", "You can't inscribe the artefact again");
           Return 2;
         Else
           artefactMask[artefactID] += 1;                                                        // geht nochmal? na dann rein damit!
         EndIf
      Else
        SendSysMessagePergon(who, "In dieses Buch koennt ihr das Artefakt nicht eintragen!", "You can't transfer the artefact into this book!");
        Return 2;
      EndIf
    UOBJ_ARTEFACTBOOK_CLERIC:
      If((MINCLERICARTEFACTSCROLL <= artefactScroll.objtype) && (artefactScroll.objtype <= MAXCLERICARTEFACTSCROLL))
         artefactID-= 500;
         If (artefactMask[artefactID] >= 100)
           SendSysMessagePergon(who, "Ihr koennt das Artefakt nicht nochmal eintragen", "You can't inscribe the artefact again");
           Return 2;
         Else
           artefactMask[artefactID] += 1;
         EndIf
      Else
        SendSysMessagePergon(who, "In dieses Buch koennt ihr das Artefakt nicht eintragen!", "You can't transfer the artefact into this book!");
        Return 2;
      EndIf
    UOBJ_ARTEFACTBOOK_NECRO:
      If((MINNECROARTEFACTSCROLL <= artefactScroll.objtype) && (artefactScroll.objtype <= MAXNECROARTEFACRSCROLL))
         artefactID-= 600;
         If (artefactMask[artefactID] >= 100)
           SendSysMessagePergon(who, "Ihr koennt das Artefakt nicht noch einmal eintragen", "You can't inscribe the artefact again");
           Return 2;
         Else
           artefactMask[artefactID] += 1;
         EndIf
      Else
        SendSysMessagePergon(who, "In dieses Buch koennt ihr das Artefakt nicht eintragen!", "You can't transfer the artefact into this book!");
        Return 2;
      EndIf
    default: SysLog("FEHLER: In AddArtefactToBook ist was schief gelaufen!");       // sollte unmöglich sein (hoffentlich)
      Return 0;
  EndCase

  SetObjProperty(book, "ArtefactMask", artefactMask);                               // und Artefakt endgültig eintragen

  If (artefactMask[artefactID] == 1)
    SendSysMessagePergon(who, "Ihr uebertragt das Artefakt in euer Buch", "You transfer the artefact into your book");
  ElseIf (artefactMask[artefactID] > 1)
    SendSysMessagePergon(who, "Ihr uebertragt das Artefakt ein weiteres mal in euer Buch, seine Macht wird groesser", "You transfer the artefact again into your book, its power increases");
  EndIf
  
  SubtractAmount(artefactScroll, 1);                                                // Rolle wird natürlich zerstört

  Return 1;
EndFunction

//////////////////////////////////////////////////////////////////////////////////
//  GetArtefactMask   -   Artefaktmaske holen uns zurückgeben
//////////////////////////////////////////////////////////////////////////////////

Function GetArtefactMask(book)
  Var artefactMask:= GetObjProperty(book, "ArtefactMask");
  If (!artefactMask)
    SysLog("FEHLER: Artefaktbuch "+Lower(Hex(book.serial))+" hatte keine gueltige ArtefactMask!");
    InitializeArtefactMask(book);
  EndIf
  artefactMask:= GetObjProperty(book, "ArtefactMask");
  return(artefactMask);
EndFunction

//////////////////////////////////////////////////////////////////////////////////
//  InitializeArtefactMask   -   Artefaktmaske mit Nullen initialisieren
//////////////////////////////////////////////////////////////////////////////////

Function InitializeArtefactMask(book)
  If (!IsItemArtefactBook(book));
    SysLog("FEHLER: InitializeArtefactMask mit nicht-Artefaktbuch aufgerufen!");
    Return 0;
  EndIf  

  Var artefactMask:= {};
  Var i;
  For(i:= 1 ; i<= 88; i+= 1)                    // alles auf 0 setzen
    artefactMask[i]:= 0;
  EndFor

  SetObjProperty(book, "ArtefactMask" , artefactMask);

  return 1;
EndFunction

//////////////////////////////////////////////////////////////////////////////////
//  IsItemArtefactBook   -   Überprüfen ob Item Artefaktbuch ist
//////////////////////////////////////////////////////////////////////////////////

Function IsItemArtefactBook(item)
  Var ARTEFACTBOOKS:= {UOBJ_ARTEFACTBOOK_MAGE, UOBJ_ARTEFACTBOOK_CLERIC, UOBJ_ARTEFACTBOOK_NECRO};

  If (item.objtype in ARTEFACTBOOKS)
    Return 1;
  EndIf

  Return 0;
EndFunction

//////////////////////////////////////////////////////////////////////////////////
//  GetArtefactIDFromScroll   -   ArtefaktID von Rollen holen
//////////////////////////////////////////////////////////////////////////////////

Function GetArtefactIDFromScroll(scroll)

  Var artefacts:= ReadConfigFile("::itemdesc");
  Var artefact:= FindConfigElem(artefacts, scroll.objtype);
  Var ID:= artefact.artefactID;
  
  If (!ID)
    SysLog("FEHLER: Da hat eine Artefaktrolle keinen ArtefactID-Eintrag!");
    Return 0;
  EndIf

  Return(ID);
EndFunction

//////////////////////////////////////////////////////////////////////////////////
//  GetIndexEntryFromArtefactID  -   AfIndex Eintrag von ArtefactID holen
//////////////////////////////////////////////////////////////////////////////////

Function GetCfgEntryFromArtefactID(artefactID)

  If ((artefactID < MINMAGEARTEFACTID) || (artefactID > MAXNECROARTEFACTID))
    SysLog("FEHLER: GetCfgEntryFromArtefactID mit falscher ArtefaktID aufgerufen");
    Return(0);
  EndIf
  
  Var artefacts:= ReadConfigFile(":newinscription:artefacts");
  If (artefacts[artefactID])
    Return(artefacts[artefactID]);
  Else
    SysLog("FEHLER: GetCfgEntryFromArtefactID kein cfg Eintrag:"+artefactID);
    Return(0);
  EndIf
  Return(0);
EndFunction

//////////////////////////////////////////////////////////////////////////////////
// GetRandomArtefactIDFromCircle  - liefert zufällige ArtefaktID aus Zirkel
//////////////////////////////////////////////////////////////////////////////////

Function GetRandomArtefactIDFromCircle(skillID, circle)
  
  Var magierklasse;
  Var IDoffset;
  
  Case (skillID)
    SKILLID_MAGISCHEGEGENST:   magierklasse:= MAGE_DESC;   IDoffset:= MAGEIDOFFSET;
    SKILLID_GESEGNETEGEGENST:  magierklasse:= CLERIC_DESC; IDoffset:= CLERICIDOFFSET;
    SKILLID_VERFLUCHTEGEGENST: magierklasse:= NECRO_DESC;  IDoffset:= NECROIDOFFSET;      
  EndCase
  Var artefacts:= ReadConfigFile(":newinscription:artefacts");
  
  Var arrayPosBegin:= IDoffset+circle*10 + 1;                                // Array Bereich berechnen
  Var arrayPosEnd:= IDoffset+circle*10 + 9;
    
  Var i;
  Var j:= 0;
  For (i:= arrayPosBegin; i<= arrayPosEnd ;i+= 1)           // Array Bereich durchsuchen
    If (artefacts[i])
      If (artefacts[i].name=="nix") // wenn nicht definiert suche abbrechen
        break;
      Else
        j+=1; // ansonsten ok, und weitergucken
      EndIf
    Else
      break;
    EndIf
  EndFor
  
  If (j == 0)                                  // gar kein Artefakt in dem Zirkel vorhanden?
    Return(0);   
  EndIf  
 
  Var artefactID:= IDoffset + circle * 10 + RandomInt(j) + 1;   // ArtefaktID berechnen  
  Return(artefactID);                                           // und zurückgeben
  
EndFunction