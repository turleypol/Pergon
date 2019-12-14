///////////////////////////////////////////////////////////////////////////
// Poison include - Regelt alles zum Thema Vergiftung
//
// Author: Fraggle & Shinigami

///////////////////////////////////////////////////////////////////////////
// Funktionen zur Vergiftung
//   AddPoisonLevel   - Erhoeht die Vergiftung um einen Wert
//   SetPoisonLevel   - Setzt die Vergiftung auf einen speziellen Wert
//   GetPoisonLevel   - Wie stark ist der Spieler/Gegenstand vergiftet?
//   IsPoisoned       - Ist der Spieler schon/Gegenstand vergiftet?
//
// Funktionen, die NIE direkt genutz werden sollten
//   LoginPoisoning   - Prueft das Vergiftungslevel beim Einloggen und
//                      fuehrt die Vergiftung durch
//   DoPoisoning      - Prueft das Vergiftungslevel und fuehrt die
//                      Vergiftung durch
//   PoisoningProcess - Der eigentliche Vergiftungsprozess (wird per
//                      Start_ScriptPergon von DoPoisoning aufgerufen!)

/////////////
// Includes
/////////////
include ":newspells:resistance";
include "include/client";
include "include/pergonutil";
include "include/packets";

// laeuft gerade eine Vergiftung?
Const PROP_POISONED  := "#poisoning";
// wie hoch ist der Giftlevel (Items/Mobiles)
Const PROP_POIS_LVL  := "poison_level";
// kann man das Item zum Vergiften benutzen
// (ausser Giftpotions, da laeuft es anders)
Const PROP_IS_POISON := "is_poison";

//////////////////////////////////////////////////////////
// AddPoisonLevel - Erhoeht die Vergiftung um einen Wert
//////////////////////////////////////////////////////////
Function AddPoisonLevel(object, addlevel, attacker := 0)
  // Critical Section ist hier ganz entscheidend, da die Vergiftung im
  // Hintergrund parallel laeuft
  var old_critical := Is_Critical();
  Set_Critical(1);

  var level := addlevel;
  var oldlevel := CInt(GetObjProperty(object, PROP_POIS_LVL));
  If (oldlevel)
    level += oldlevel;
  EndIf

  // Entgiftung geht nur bis 0 und kann nicht angesammelt werden
  If (level <= 0)
    EraseObjProperty(object, PROP_POIS_LVL);
  Else
    SetObjProperty(object, PROP_POIS_LVL, level);
  EndIf
  Set_Critical(old_critical);

  If (object.isa(POLCLASS_MOBILE)) // Nur bei Mobiles
    DoPoisoning(object, attacker); // Vergiftungsprozess starten
  EndIf

  return level; // Fuer Entgiftung ganz nuetzlich
EndFunction

////////////////////////////////////////////////////////////////////
// SetPoisonLevel - Setzt die Vergiftung auf einen speziellen Wert
////////////////////////////////////////////////////////////////////
Function SetPoisonLevel(object, level, attacker := 0)
  // Critical Section nuetzt bei Einzelanweisungen nix, deshalb gibts
  // die hier nicht
  If (level)
    SetObjProperty(object, PROP_POIS_LVL, level);
  Else
    EraseObjProperty(object, PROP_POIS_LVL);
  EndIf

  If (object.isa(POLCLASS_MOBILE)) // Nur bei Mobiles
    DoPoisoning(object, attacker); // Vergiftungsprozess starten
  EndIf
EndFunction

/////////////////////////////////////////////////////////////////////
// GetPoisonLevel - Wie stark ist der Spieler/Gegenstand vergiftet?
/////////////////////////////////////////////////////////////////////
Function GetPoisonLevel(object)
  return (CInt(GetObjProperty(object, PROP_POIS_LVL)));
EndFunction

/////////////////////////////////////////////////////////////
// IsPoisoned - Ist der Spieler schon/Gegenstand vergiftet?
/////////////////////////////////////////////////////////////

Function IsPoisoned(object)
  If (CInt(GetObjProperty(object, PROP_POIS_LVL)))
    return 1;
  EndIf
  return 0;
EndFunction

///////////////////////////////////////////////////////////////////////////
// LoginPoisoning - Prueft das Vergiftungslevel beim Einloggen und
// fuehrt die Vergiftung durch
///////////////////////////////////////////////////////////////////////////
Function LoginPoisoning(mobile, attacker := 0)
  // vorhandene Vergiftungen pruefen
  // var zeitpunkt := GetObjProperty(mobile, PROP_POISONED);
  // var giftlevel := GetObjProperty(mobile, PROP_POIS_LVL);
  EraseObjProperty(mobile, PROP_POISONED);

  // If ((zeitpunkt) and (giftlevel))
  //   If ((ReadGameClock() - zeitpunkt) > 10*giftlevel)
  //     // Vergiftung lange her, ggf. Resistenzen abgebaut
  //     return;
  //   EndIf
  // EndIf

  // Vergiftungsprozess starten
  DoPoisoning(mobile, attacker);
EndFunction

///////////////////////////////////////////////////////////////////////////
// DoPoisoning - Prueft Vergiftungslevel und fuehrt Vergiftung durch
///////////////////////////////////////////////////////////////////////////
Function DoPoisoning(mobile, attacker)
  // Laeuft der Vergiftungsprozess bereits? (doppelte Abfrage, sicher
  // ist sicher)
  If (!GetObjProperty(mobile, PROP_POISONED))
    var params := {};
    params[1] := mobile;
    params[2] := attacker;
    // Vergiftungsprozess starten
    Start_ScriptPergon("::misc/poisoning", params);
  EndIf
EndFunction

///////////////////////////////////////////////////////////////////////////
// PoisoningProcess - Der eigentliche Vergiftungsprozess (wird per
// Start_ScriptPergon von DoPoisoning aufgerufen!)
///////////////////////////////////////////////////////////////////////////
Function PoisoningProcess(mobile, attacker)
	var rep := DAMAGE_USE_REPSYS;
	var controller := SetScriptController(0);
	If (attacker == 0)
		attacker := mobile;
		rep      := DAMAGE_NO_REPSYS;
	ElseIf (controller == 0)
		rep      := DAMAGE_NO_REPSYS;
	EndIf
	SetScriptController(attacker);
	
  // Laeuft der Vergiftungsprozess bereits? (doppelte Abfrage, sicher
  // ist sicher)
  If (GetObjProperty(mobile, PROP_POISONED))
    return;
  EndIf
  SetObjProperty(mobile, PROP_POISONED, 1); // ReadGameClock());

  If (!GetObjProperty(mobile, PROP_POIS_LVL))
    // ist gar nicht (mehr) vergiftet
    mobile.poisoned := 0;
    IncRevision(mobile);
    EraseObjProperty(mobile, EV_POISONER);
    EraseObjProperty(mobile, PROP_POISONED);
    return;
  EndIf

  var level  := 1; // Dummywert, wird in Schleife critical gelesen
  While (level >= 1)
    // Vergiftungslevel ermitteln {{{
    // Critical Section ist hier ganz entscheidend, da die Vergiftung im
    // Hintergrund parallel laeuft
    var old_critical := Is_Critical();
    Set_Critical(1);

    level := CInt(GetObjProperty(mobile, PROP_POIS_LVL));
    If (level >= 1)
      mobile.poisoned := 1;
      IncRevision(mobile);
      SetObjProperty(mobile, PROP_POIS_LVL, level-1);
    Else
      mobile.poisoned := 0;
      IncRevision(mobile);
      EraseObjProperty(mobile, PROP_POIS_LVL);
    EndIf
    Set_Critical(old_critical);

    If (level <= 0)
      level := 0;
    EndIf
    // }}}

    // Gitfresi des Opfers
    var resist := 1 - CDbl(GetResistance(mobile, "poison"));

    // Schadensformel/-staerke
    var giftschaden := CInt(Max(0, (
      (
        0.2/(Pow(ConstE(), level / -3.0) + 1) - 0.09
      )*100) * resist
    ));

    If (giftschaden <= 0)
      // kein Giftschaden (Resistenz) -> Vergiftung beenden
      Set_Critical(1);
      EraseObjProperty(mobile, EV_POISONER);
      EraseObjProperty(mobile, PROP_POISONED);
      EraseObjProperty(mobile, PROP_POIS_LVL);
      Set_Critical(old_critical);
      mobile.poisoned := 0;
      IncRevision(mobile);
      return;
    EndIf

    // tot oder weg (NPC-Tod)
    If ((!mobile) or (mobile.dead))
      break;
    EndIf
    If (!mobile.isa(POLCLASS_NPC)) // Player
      If (!mobile.ip) // Ist der Spieler noch eingeloggt?
        break;
      EndIf

      var gifttext := "";
      If (level==30)
        gifttext := "Ihr spuert die toedliche Wirkung des Giftes!";
      ElseIf (level==20)
        gifttext := "Das Gift brennt sich durch Eure Adern!";
      ElseIf (level==15)
        gifttext := "Das Gift bringt Euch um!";
      ElseIf (level==10)
        gifttext := "Ihr spuert das Gift durch Eure Adern rinnen!";
      ElseIf (level==5)
        gifttext := "Die Wirkung des Giftes laesst langsam nach!";
      ElseIf (level==1)
        gifttext := "Das Gift ist kaum mehr zu spueren!";
      EndIf
      If (gifttext != "")
        PrintTextAbovePergon(mobile, mobile, gifttext);
      EndIf
    EndIf

    // Vergiftung durchfuehren
    PlaySoundEffect(mobile, SFX_SPELL_POISON);
    // Geht durch die Ruestung durch -> Raw
    ApplyRawDamagePergon(mobile, giftschaden, rep);

    // Wenn das Viech tot ist, brauchen wir nicht warten
    // tot oder weg (NPC-Tod)
    If ((!mobile) or (mobile.dead))
      break;
    EndIf
    Sleep(1);
  EndWhile

  // noch nicht tot, aber noch vergiftet (z. B. ausgeloggter Player)?
  If (!mobile.isa(POLCLASS_NPC) and !mobile.dead and (level >= 1))
    return;
  EndIf

  If (mobile)  // noch existent?
    EraseObjProperty(mobile, EV_POISONER);
    EraseObjProperty(mobile, PROP_POISONED);
    EraseObjProperty(mobile, PROP_POIS_LVL);
  EndIf
EndFunction

// vim: sts=2 sw=2
