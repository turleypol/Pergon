// Eintrag in cliloc.deu (+3000000)
Enum CLILOC_NR
	CLILOC_LEER := 579,
	CLILOC_LEICHE,                   // Eigene Leiche bergen
  CLILOC_PVENDOR_MSGSEND,          // Playervendor Nachricht hinterlassen
  CLILOC_PVENDOR_MSGRECEIVE,       // Playervendor Nachricht empfangen
  CLILOC_PVENDOR_MSGDELETE,        // Playervendor Nachricht löschen
  CLILOC_VENDOR_BOOK,              // Zauberbuch kaufen
  CLILOC_VENDOR_ARTBOOK,           // Artefaktbuch kaufen
  CLILOC_VENDOR_KARMA,             // Karma
  CLILOC_VENDOR_BUY := 590,        // Kaufen
  CLILOC_VENDOR_SELL,              // Verkaufen
  CLILOC_VENDOR_BANK,              // Bankfach öffnen
  CLILOC_VENDOR_KONTO,             // Kontostand abfragen
  CLILOC_VENDOR_KONTOADD,          // Gold einzahlen
  CLILOC_VENDOR_KONTODEL,          // Gold abbuchen
  CLILOC_VENDOR_CHEQUE,            // Scheck ausstellen
  CLILOC_VENDOR_MONDSTEIN,         // Mondstein kaufen
  CLILOC_VENDOR_SKILL,             // Skilltausch
  CLILOC_VENDOR_FLUCHT,            // Fluchterlaubnis
  CLILOC_VENDOR_TRAIN,             // Trainieren
  CLILOC_VENDOR_MUSIC,             // Barde musizieren lassen
  CLILOC_VENDOR_BEARD,             // Rasur
  CLILOC_VENDOR_HAIR,              // Haarschnitt
  CLILOC_PVENDOR_RENT,             // Händler mieten
  CLILOC_PVENDOR_OPEN,             // Öffnen
  CLILOC_PVENDOR_CLOSE,            // Schließen
  CLILOC_PVENDOR_CLOTHES,          // Kleidung zuweisen
  CLILOC_PVENDOR_STRIP,            // Ausziehen
  CLILOC_PVENDOR_PAYME,            // Gold auszahlen
  CLILOC_PVENDOR_STATUS := 618,    // Status
  CLILOC_PVENDOR_LOHN,             // Zahltag
  CLILOC_PVENDOR_FIRE,             // Kündigen
  CLILOC_VENDOR_HEAL,              // Heilen
  CLILOC_VENDOR_NEWS,              // Nachrichten
  CLILOC_GUARD_HELP,               // um Hilfe rufen
  CLILOC_GUARD_CRIME,              // Verbrechen zeigen
  CLILOC_GUARD_PUNISHMENTS,        // Eigene Vorstrafen
  CLILOC_GUARD_PRISONERS,          // Knastinsassen
  CLILOC_PRISONER_HELP,            // Gefangenem Helfen
  CLILOC_PRISONER_GO,              // Mit dem Gefangenen loslaufen
  CLILOC_PUFF_RELAX,               // Entspannung
  CLILOC_PROTOCOL_OPEN,            // Protokoll öffnen
  CLILOC_PROTOCOL_CLOSE,           // Protokoll schließen
  CLILOC_PROTOCOL_PSTART,          // Pausenstart
  CLILOC_PROTOCOL_PEND,            // Pausenende
  CLILOC_PROTOCOL_MASTERS,         // Meister anzeigen
  CLILOC_PROTOCOL_MADD,            // Meister hinzufügen
  CLILOC_PROTOCOL_MDEL,            // Meister entfernen
  CLILOC_PROTOCOL_RANGE,           // Höhrweite ändern
  CLILOC_NPC_STOP,                 // Halt
  CLILOC_VENDOR_STALL              // Stall
EndEnum

// IDconst (wird zur Unterscheidung verwendet, beliebig wählbar Client schickt es grad wieder mit bei Auswahl)
Enum IDconst
  ID_LEICHE:=1,
  ID_PVENDOR_MSGSEND,
  ID_PVENDOR_MSGRECEIVE,
  ID_PVENDOR_MSGDELETE,
  ID_VENDOR_BOOK,
  ID_VENDOR_ARTBOOK,
  ID_VENDOR_KARMA,
  ID_VENDOR_BUY,
  ID_VENDOR_SELL,
  ID_VENDOR_BANK,
  ID_VENDOR_KONTO,
  ID_VENDOR_KONTOADD,
  ID_VENDOR_KONTODEL, 
  ID_VENDOR_CHEQUE,
  ID_VENDOR_MONDSTEIN,
  ID_VENDOR_SKILL,
  ID_VENDOR_FLUCHT,
  ID_VENDOR_TRAIN,
  ID_VENDOR_MUSIC,
  ID_VENDOR_BEARD,
  ID_VENDOR_HAIR,
  ID_PVENDOR_RENT,
  ID_PVENDOR_OPEN,
  ID_PVENDOR_CLOSE,
  ID_PVENDOR_CLOTHES,
  ID_PVENDOR_STRIP,
  ID_PVENDOR_PAYME,
  ID_PVENDOR_STATUS,
  ID_PVENDOR_LOHN,
  ID_PVENDOR_FIRE,
  ID_VENDOR_HEAL,
  ID_VENDOR_NEWS,
  ID_GUARD_HELP,
  ID_GUARD_CRIME,
  ID_GUARD_PUNISHMENTS,
  ID_GUARD_PRISONERS, 
  ID_PRISONER_HELP,
  ID_PRISONER_GO,
  ID_PUFF_RELAX,
  ID_PROTOCOL_OPEN,
  ID_PROTOCOL_CLOSE,
  ID_PROTOCOL_PAUSE,
  ID_PROTOCOL_MASTERS,
  ID_PROTOCOL_MADD,
  ID_PROTOCOL_MDEL,
  ID_PROTOCOL_RANGE,
  ID_NPC_STOP,
  ID_VENDOR_STALL
EndEnum

///////////////////////////////////////////////
//  MenuSelect - Passende Antwort finden..
///////////////////////////////////////////////

Function MenuSelect(character, packet)
	var clicked, popupevent;

  clicked:=SystemFindObjectBySerial(packet.getint32(5));  // Serial des "Targets"
  If (clicked)
    popupevent:=packet.GetInt16(9);                       // Welche ID?
    Case (popupevent)
      ID_LEICHE:             Start_ScriptPergon("lootcorpse", {character, clicked});                // Eigene Leiche looten, ausgelagert um unnötigen Speicherverbrauch zu vermeiden
      ID_PVENDOR_MSGSEND:    Start_ScriptPergon("vendormsg",  {character, clicked, "send"});        // Nachricht hinterlassen
      ID_PVENDOR_MSGRECEIVE: Start_ScriptPergon("vendormsg",  {character, clicked, "receive"});     // Nachricht abholen
      ID_PVENDOR_MSGDELETE:  Start_ScriptPergon("vendormsg",  {character, clicked, "delete"});      // Nachricht löschen
      ID_VENDOR_BOOK:        EventSenden(character, clicked, "zauberbuch");                         // Zauberbuch kaufen
      ID_VENDOR_ARTBOOK:     EventSenden(character, clicked, "artefaktbuch");                       // Artefaktbuch kaufen
      ID_VENDOR_KARMA:       EventSenden(character, clicked, "karma");                              // Karma
      ID_VENDOR_BUY:         EventSenden(character, clicked, "kaufen");                             // Kaufen
      ID_VENDOR_SELL:        EventSenden(character, clicked, "verkaufen");                          // Verkaufen
      ID_VENDOR_BANK:        EventSenden(character, clicked, "bank");                               // Bankfach
      ID_VENDOR_KONTO:       EventSenden(character, clicked, "konto");                              // Kontostand abfragen
      ID_VENDOR_KONTOADD:    EventSenden(character, clicked, "einzahlen");                          // Gold einzahlen
      ID_VENDOR_KONTODEL:    EventSenden(character, clicked, "abheben");                            // Gold abbuchen
      ID_VENDOR_CHEQUE:      EventSenden(character, clicked, "scheck");                             // Scheck ausstellen
      ID_VENDOR_MONDSTEIN:   EventSenden(character, clicked, "mondstein");                          // Mondstein kaufen
      ID_VENDOR_SKILL:       EventSenden(character, clicked, "skill");                              // Skilltausch
      ID_VENDOR_FLUCHT:      EventSenden(character, clicked, "flucht");                             // Fluchterlaubnis
      ID_VENDOR_TRAIN:       EventSenden(character, clicked, "trainiere");                          // Trainieren
      ID_VENDOR_MUSIC:       EventSenden(character, clicked, "musik");                              // Barde musizieren lassen
      ID_VENDOR_BEARD:       EventSenden(character, clicked, "rasur");                              // Rasur
      ID_VENDOR_HAIR:        EventSenden(character, clicked, "haarschnitt");                        // Haarschnitt
      ID_PVENDOR_RENT:       EventSenden(character, clicked, "mieten");                             // Händler mieten
      ID_PVENDOR_OPEN:       EventSenden(character, clicked, "oeffnen");                            // Öffnen
      ID_PVENDOR_CLOSE:      EventSenden(character, clicked, "schliessen");                         // Schließen
      ID_PVENDOR_CLOTHES:    EventSenden(character, clicked, "kleidung");                           // Kleidung zuweisen
      ID_PVENDOR_STRIP:      EventSenden(character, clicked, "ausziehen");                          // Ausziehen
      ID_PVENDOR_PAYME:      EventSenden(character, clicked, "sammeln");                            // Gold auszahlen
      ID_PVENDOR_STATUS:     EventSenden(character, clicked, "status");                             // Status
      ID_PVENDOR_LOHN:       EventSenden(character, clicked, "lohn");                               // Zahltag
      ID_PVENDOR_FIRE:       EventSenden(character, clicked, "kuendigen");                          // Kündigung
      ID_VENDOR_HEAL:        EventSenden(character, clicked, "heilen");                             // Heilen
      ID_VENDOR_NEWS:        EventSenden(character, clicked, "news");                               // Nachrichten
      ID_GUARD_HELP:         EventSenden(character, clicked, "hilfe");                              // um Hilfe rufen
      ID_GUARD_CRIME:        EventSenden(character, clicked, "verbrechen");                         // Verbrechen zeigen
      ID_GUARD_PUNISHMENTS:  EventSenden(character, clicked, "vorstrafe");                          // Eigene Vorstrafen
      ID_GUARD_PRISONERS:    EventSenden(character, clicked, "knast");                              // Knastinsassen
      ID_PRISONER_HELP:      EventSenden(character, clicked, "hilfe");                              // Gefangenem helfen
      ID_PRISONER_GO:        EventSenden(character, clicked, "los");                                // Mit dem Gefangenem loslaufen
      ID_PUFF_RELAX:         EventSenden(character, clicked, "entspannung");                        // Entspannung
      ID_PROTOCOL_OPEN:      EventSenden(character, clicked, "protokoll oeffnen");                  // Protokoll öffnen
      ID_PROTOCOL_CLOSE:     EventSenden(character, clicked, "protokoll schliessen");               // Protokoll schließen
      ID_PROTOCOL_PAUSE:     EventSenden(character, clicked, "protokollpause");                     // Protokoll pausieren
      ID_PROTOCOL_MASTERS:   EventSenden(character, clicked, "masters");                            // Meister anzeigen
      ID_PROTOCOL_MADD:      EventSenden(character, clicked, "addmaster");                          // Meister hinzufügen
      ID_PROTOCOL_MDEL:      Start_ScriptPergon("protomaster", {character, clicked});               // Meister entfernen
      ID_PROTOCOL_RANGE:     EventSenden(character, clicked, "setrange");                           // Höhrweite ändern
      ID_NPC_STOP:           EventSenden(character, clicked, "halt");                               // Halt
      ID_VENDOR_STALL:       EventSenden(character, clicked, "stall");                              // Stall
    EndCase
  EndIf

EndFunction

///////////////////////////////////////////////
//  EventSenden - Event an den NPC schicken
///////////////////////////////////////////////

Function EventSenden(character, clicked, text)
  var ev:=struct, name;

  name:=GetObjProperty(clicked, "myname");
  If (!name)
    name:="";
  EndIf

  ev.+type:=SYSEVENT_SPEECH;
  ev.+source:=character;
  ev.+text:=name+" "+text;

  SendEvent(clicked, ev);
EndFunction