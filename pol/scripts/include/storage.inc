// Storage-Manipulation
//   FindOrCreateStorage           - Sucht oder erzeugt ein Storage                                     ok
//   GetBoxInStorageArea           - Sucht oder erzeugt das RootItem im Storage (Storage-Area namentlich uebergeben)
//   FindOrCreateRootItemInStorage - Sucht oder erzeugt das RootItem im Storage                         ok
//   InitRootItemInStorage         - Initialisiert das RootItem im Storage (Inhalt komplett loeschen)   ok
//
// Merchant Storage (NPCs)
//   OpenMerchantStorage           - Oeffnet den zentralen Handelsbereich

include "include/logutil";
include "include/objtype";

// Namen diverser Storage Areas
// Bankboxen der Spieler
Const STORAGE_BANK     := "World Bank";
// eigene Fuellungen fuer Schatzkisten
Const STORAGE_CHEST    := "ChestStorage";
// Schluesselringe
Const STORAGE_KEYRINGS := "Tamed Storage";
// Const STORAGE_KEYRINGS := "Keyrings";
// interne Boxen der NPC-Haendler
Const STORAGE_MERCHANT := "Merchant Storage";
// Hausumzug
Const STORAGE_HOUSE    := "Relocation";
// keine Ahnung
Const STORAGE_TESTAREA := "Pergon TestArea";

// Namenspraefix der Kisten in den Areas
// Bankbox eines Spielers
Const ST_PREF_BANK     := "Bankbox of ";
// Zwischenspeicher fuer Inhalt einer selbstbefuellten Schatzkiste
Const ST_PREF_CHEST    := "Chest ";
// Box fuer Inhalt eines Schluesselrings
Const ST_PREF_KEYRINGS := "Bankbox  ";
// Const ST_PREF_KEYRINGS := "Keyring ";


// silberne Metallbox
Const STORAGE_DEFAULTROOTITEM := 0x0e7c;

//////////////////////////////
//   Storage-Manipulation   //
//////////////////////////////

// FindOrCreateStorage - Sucht oder erzeugt ein Storage
Function FindOrCreateStorage(areaname)
  var area := FindStorageArea(areaname);
  If (!area)
    area := CreateStorageArea(areaname);
    If (area)
      syslog("Storage '"+areaname+"' wurde erzeugt.");
    Else
      syslog("FEHLER: Storage '"+areaname+"' konnte nicht erzeugt werden!");
    EndIf
  EndIf

  return area;
EndFunction

// GetBoxInStorageArea - Sucht oder erzeugt das RootItem im Storage
// (Storage-Area namentlich uebergeben)
Function GetBoxInStorageArea(storagename, boxname, type := STORAGE_DEFAULTROOTITEM)
  var area := FindOrCreateStorage(storagename);
  return FindOrCreateRootItemInStorage(area, boxname, type);
EndFunction

// FindOrCreateRootItemInStorage - Sucht oder erzeugt das RootItem im Storage
Function FindOrCreateRootItemInStorage(storage, name, objtype := STORAGE_DEFAULTROOTITEM)
  var item := FindRootItemInStorageArea(storage, name);
  If (!item)
    item := CreateRootItemInStorageArea(storage, name, objtype);
    If (!item)
      syslog(
        "FEHLER: Storage-RootItem "+LHex(objtype)+
        " konnte nicht erzeugt werden!"
      );
    EndIf
  EndIf

  return item;
EndFunction

// Initialisiert das RootItem im Storage (Inhalt komplett loeschen)
Function InitRootItemInStorage(storage, name, objtype := STORAGE_DEFAULTROOTITEM)
  var item := FindRootItemInStorageArea(storage, name);
  If (item and item.objtype == objtype)
    // existiert, Typ passt: ausleeren und fertig
    ForEach thing in EnumerateItemsInContainer(item, ENUMERATE_ROOT_ONLY)
      DestroyItem(thing);
    EndForEach
    return item;

  Else
    // falscher Typ, erstmal loeschen ...
    DestroyRootItemInStorageArea(storage, name);
  EndIf

  // existiert nicht (mehr), neu erzeugen
  If (!item)
    item := CreateRootItemInStorageArea(storage, name, objtype);
    If (!item)
      syslog(
        "FEHLER: Storage-RootItem "+Lower(Hex(objtype))+
        " konnte nicht erzeugt werden!"
      );
    EndIf
  EndIf

  return item;
EndFunction

////////////////////////////
//                        //
//   Test Bankboxen       //
//                        //
////////////////////////////

Function TestOpenWorldBank()
  return (FindOrCreateStorage(STORAGE_BANK));
EndFunction

Function TestOpenBankBox(mobile)
  If (mobile.isa(POLCLASS_MOBILE) And (!mobile.isa(POLCLASS_NPC)))
    return (FindOrCreateRootItemInStorage(OpenWorldBank(), GetBankBoxName(mobile), UOBJ_BANKBOX));
  Else
    syslog("FEHLER: Nur Player duerfen Bankboxen besitzen!");
  EndIf
EndFunction

Function TestDestroyBankBox(mobile)
  If (mobile.isa(POLCLASS_MOBILE) And (!mobile.isa(POLCLASS_NPC)))
    return (DestroyRootItemInStorageArea(OpenWorldBank(), GetBankBoxName(mobile)));
  Else
    syslog("FEHLER: Nur Player besitzen Bankboxen!");
  EndIf
EndFunction

Function BTestGetBankBoxName(mobile)
  return (ST_PREF_BANK+mobile.serial);
EndFunction

/////////////////////////////////
//                             //
//   Merchant Storage (NPCs)   //
//                             //
/////////////////////////////////

///////////////////////////////////////////////////////////////
// OpenMerchantStorage - Oeffnet den zentralen Handelsbereich
///////////////////////////////////////////////////////////////

Function TestOpenMerchantStorage()
  return (FindOrCreateStorage(STORAGE_MERCHANT));
EndFunction
