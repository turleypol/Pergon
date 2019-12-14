// $Log: not supported by cvs2svn $
// Revision 1.4  2008/07/07 14:05:07  mehdorn
// sollte nun wieder gehen, wenn Gefaess in Landschaft steht
//
// Revision 1.3  2008/07/06 22:09:04  mehdorn
// noch etwas veraendert, damit Fuellroutine selbst die Gefaesse wechselt
//     -> Verkuerzt Zeit, in der leeres Fass mit "Voll"-Grafik existiert,
//        gerade beim Brunnen
//
// Revision 1.2  2008/07/06 13:45:28  mehdorn
// Aufteilung fuer Brunnen erweitert
//

use cfgfile;
use os;
use uo;
include "include/modifyskill";

Const WASSERSTAND  := "wasser";
Const FUELLSTAND   := "filled";
Const USE_SKILL    := 1;
Const NO_USE_SKILL := 0;
Const UACTION_CHOP := 0x0D;

// ist das Gefaess geeignet zur Wasseraufnahme (leer, Behaelter ...)
Function GefaessGeeignet(char, Gefaess) // {{{
  If ((!AccessiblePergon(char, Gefaess)) or (Distance(char, Gefaess) > 1))
    SendSysMessagePergon(char,
        "Ihr könnt das Gefäss nicht benutzen.",
        "You're not able to use that."
    );
    return;
  EndIf

  var cfg_item := ReadConfigFile("filled_water");
  If (!cfg_item)
    SendSysMessagePergon(char,
        "Ausführung nicht möglich. Interner Fehler.",
        "Internal server error. Abort."
    );
    syslog("FEHLER: Konnte die filled_water.cfg nicht oeffnen!");
    return cfg_item;
  EndIf

  var item := cfg_item[Gefaess.objtype];
  If (!item)
    SendSysMessagePergon(char,
        "Damit könnt Ihr nichts anfangen!",
        "You don't know what to do with that!"
    );
    return item;
  EndIf

  If (Gefaess.isa(POLCLASS_CONTAINER))
    If (GetObjProperty(Gefaess, "lockid"))
      SendSysMessagePergon(char,
        "Das ausgewählte Gefäss hat einen Verschlussmechanismus und "+
        "kann daher nicht als Wassergefäss benutzt werden!",
        "Target vessel is provided with a lock and cannot be used!"
      );
      var ret := error;
      ret.errortext := "Vessel is lockable and cannot be used";
      return ret;
    EndIf
    ForEach item_in_container in EnumerateItemsInContainer(Gefaess)
      If (item_in_container)
        SendSysMessagePergon(char,
            "Es befindet sich noch etwas in dem Gefäss!",
            "Target vessel is not empty!"
        );
        var ret := error;
        ret.errortext := "Vessel not empty";
        return ret;
      EndIf
    EndForEach
  EndIf

  return item;
EndFunction // }}}

// fuer Wasserkanne/Wassereimer ...
// Rueckgabewerte: error bei Fehler, -1 bei vollem Gefaess, Restmenge sonst
Function FuelleWasserGefaess(char, byref Gefaess, myst, skill := USE_SKILL) // {{{
  var item := GefaessGeeignet(char, Gefaess);
  If (!item)
    // irgendein Fehler unterwegs
    return item;
  EndIf

  // bei Bedarf leeres durch volles Gefaess ersetzen
  If (item.targetitem)
    Gefaess := ErsetzeGefaess(char, Gefaess, item.targetitem);
    If (!Gefaess)
      return Gefaess;
    EndIf
  EndIf

  ReserveItem(Gefaess);

  var fuellstand := GetObjProperty(Gefaess, FUELLSTAND);
  If (fuellstand.errortext)
    fuellstand := 0;
  EndIf
  var liter_transfer := CalcTransfer(char, Gefaess, skill);

  // print("fuellstand: "+fuellstand);
  // print("liter_transfer: "+liter_transfer);
  // print("filled_Skill_Max: "+filled_Skill_Max);
  // print("liter_transfer: "+liter_transfer);
  If (liter_transfer <= 0.1)
    SendSysMessagePergon(
      char, "Das Gefäss ist schon voll!", "This vessel is already filled!"
    );
    return -1;
  EndIf

  // die Wassermenge wird ausgelesen
  var wasser := GetObjProperty(myst, WASSERSTAND);
  // maximal soviel Wasser wie in Quelle ist schoepfbar
  liter_transfer := Min(wasser, liter_transfer);
  If (wasser - liter_transfer < 0.5)
    SetObjProperty(myst, WASSERSTAND, 0);
    PlaySoundEffect(char, 0x12);
    PerformAction(char, UACTION_CHOP);
    Sleep(2);
    SetObjProperty(Gefaess, FUELLSTAND, fuellstand+liter_transfer);
    SendSysMessagePergon(char,
      "Das Gefäss wurde mit "+liter_transfer+" Liter Wasser gefüllt.",
      "The vessel was filled with "+liter_transfer+" liters of water."
    );
    return 0;
  Else
    // die Wassermenge an der Wasserader wird gesetzt
    SetObjProperty(myst, WASSERSTAND, wasser - liter_transfer);
    PlaySoundEffect(char, 0x12);
    PerformAction(char, UACTION_CHOP);
    Sleep(2);
    SetObjProperty(Gefaess, FUELLSTAND, fuellstand+liter_transfer);
    If (fuellstand+liter_transfer >= GetCapacity(Gefaess))
      // Gefaess voll
      SendSysMessagePergon(char,
        "Das Gefaess ist nun voll.", "The vessel is now full."
      );
      return -1;
    EndIf
    SendSysMessagePergon(char,
      "Das Gefäss wurde mit "+liter_transfer+" Liter Wasser gefüllt.",
      "The vessel was filled with "+liter_transfer+" liters of water."
    );

    return wasser-liter_transfer;
  EndIf
EndFunction // }}}

// fuer Wasserkanne/Wassereimer ...
Function ErsetzeGefaess(char, Gefaess, item_new) // {{{
  If (!item_new in array{0x1f9d, 0x0ffa, 0x0e7b, 0x154d})
    return;
  EndIf

  ReserveItem(Gefaess);
  var fuellstand := GetObjProperty(Gefaess, FUELLSTAND);
  var werkzeug := GetObjProperty(Gefaess, "Werkzeug");
  // Container merken
  var cont := Gefaess.container;
  DestroyItem(Gefaess);

  var neu;
  If (cont)
    neu := CreateItemInContainerPergon(cont, item_new, 1);
  Else
    neu := CreateItemAtLocationPergon(
      char.x, char.y, char.z, item_new, 1, char.realm
    );
  EndIf

  // hergestelltes Gefaess,
  // Eigenschaften an das neue Gefaess weiterreichen
  If (werkzeug)
    SetObjProperty(neu, "Werkzeug", werkzeug);
    IncRevision(neu);
  EndIf
  SetObjProperty(neu, FUELLSTAND, fuellstand);

  return neu;
EndFunction // }}}

// wahlweise skillabhängig Transfermenge ermitteln
Function CalcTransfer(char, gefaess, skill) // {{{
  var kapazitaet_max := GetCapacity(gefaess);
  If (!kapazitaet_max)
    return;
  EndIf

  var fuellstand := GetObjProperty(gefaess, FUELLSTAND);

  If (skill == NO_USE_SKILL)
    // ohne Skill immer das beste herausholen
    return kapazitaet_max - fuellstand;
  EndIf

  //(10 bis 101 Liter)
  var skill_max := 10+CInt(0.7*GetSkillPergon(char, SKILLID_WILDNISKUNDE));
  If ((skill_max>kapazitaet_max) || (skill_max+fuellstand>kapazitaet_max))
    return kapazitaet_max - fuellstand;
  Else
    return skill_max;
  EndIf
EndFunction // }}}

// ermittelt maximalen Fuellstand des Gefaesses
Function GetCapacity(item) // {{{
  var itemcfg := ReadConfigFile("::itemdesc");
  If (!itemcfg)
    syslog("FEHLER: Konnte die ::itemdesc.cfg nicht oeffnen");
    return itemcfg;
  EndIf

  var item_cfg := itemcfg[item.objtype];
  If (!item_cfg)
    syslog("FEHLER: Konnte "+item+" in der ::itemdesc.cfg nicht finden.");
    return item_cfg;
  EndIf

  return item_cfg.filledmax;
EndFunction // }}}

// vim: sw=2 sts=2
