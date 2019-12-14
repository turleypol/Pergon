//////////////////////////////////////////////////////////////////////////////
// mobile.inc - Allgemeiner Kram fuer Mobiles

//////////////////////////////////////////////////////////////////////////////
// ConvertDirectionToLocX(facing)
// ConvertDirectionToLocY(facing)
// MovementVectorX(facing)
// MovementVectorY(facing)
//   - gibt -1 .. 1 zurueck, je nachdem, in welche Richtung auf der
//     jeweiligen Koordinatenachse ein Mobile mit dem facing gucken
//     wuerde
// Puke(mobile)
//   - laesst <mobile> kotzen (herunterbeugen, Kotze vor <mobile> erzeugen)

Const GENDER_MALE   := 0;
Const GENDER_FEMALE := 1;

use math;
include "include/msgs";

Function ConvertDirectionToLocX(byref dir)
  // dir*0.78 ~ dir*45*pi/180 Genauigkeit reicht für die Anwendung
  // und ist schneller
  return CInt(Sin(dir*0.78)*1.5);
EndFunction

Function ConvertDirectionToLocY(byref dir)
  return CInt(Cos(dir*0.78)*-1.5);
EndFunction

Function MovementVectorX(facing) // {{{
  //      y ^             f    x
  //        |             000  0
  //    +---+---+---+     001  1
  //  1 | 7 | 0 | 1 |     010  1
  //    +---+---+---+     011  1
  //  0 | 6 | X | 2 |     100  0
  //    +---+---+---+-->  101 -1
  // -1 | 5 | 4 | 3 | x   110 -1
  //    +---+---+---+     111 -1
  //     -1   0   1
  If (facing & 3 == 0)     // niedere beiden Bits 0
      return 0;
  ElseIf (facing & 4 == 4) // höchstes Bit 1
      return -1;
  EndIf
  return 1;
EndFunction // }}}

Function MovementVectorY(facing) // {{{
  //      y ^             f+1  y
  //        |             000 -1
  //    +---+---+---+     001 -1
  //  1 | 7 | 0 | 1 |     010 -1
  //    +---+---+---+     011  0
  //  0 | 6 | X | 2 |     100  1
  //    +---+---+---+-->  101  1
  // -1 | 5 | 4 | 3 | x   110  1
  //    +---+---+---+     111  0
  //     -1   0   1
  facing := (facing + 1) & 7;
  If (facing & 3 == 3)     // niedere beiden Bits 1
      return 0;
  ElseIf (facing & 4 == 4) // höchstes Bit 1
      return 1;
  EndIf
  return -1;
EndFunction // }}}

// laesst <who> kotzen
Function Puke(who) // {{{
  var sound_puke  := SFX_PUKE_MALE;
  var sound_cough := SFX_COUGH_MALE;
  If (who.gender == GENDER_FEMALE)
    sound_puke  := SFX_PUKE_FEMALE;
    sound_cough := SFX_COUGH_FEMALE;
  EndIf

  PerformAction(who, ANIM_EAT);
  Sleep(1);

  PerformAction(who, ANIM_BOW);
  PrintTextAbovePergon(who, who, "*wuerg*");
  PlaySoundEffect(who, sound_puke);
  Sleep(2);

  var desc := struct{
    Color    := 56,
    Graphic  := 0x122d,
    Movable  := 1,
    ObjClass := "Item",
    ObjType  := 0x122d
  };
  var item := CreateItemAtLocation(
    who.x+ConvertDirectionToLocX(who.facing),
    who.y+ConvertDirectionToLocY(who.facing),
    who.z, desc, 1, who.realm
  );
  If (item)
    SetName(item, "Kotze");
  EndIf

  PerformAction(who, ANIM_BOW);
  PlaySoundEffect(who, sound_puke);
  Sleep(1);

  PerformAction(who, ANIM_BOW);
  Sleep(1);

  PlaySoundEffect(who, sound_cough);
EndFunction // }}}

// guckt, ob <who> im Kampfmodus ist oder sich gerade verteidigt
Function IsFighting(who) // {{{
  If (who.warmode)
    return 1;
  EndIf

  If (GetGlobalProperty(PROP_DEFENSE_IS_FIGHT))
    If (ListHostiles(who, 1, LH_FLAG_LOS).size () >= 1)
      return 1;
    EndIf
  EndIf

  return 0;
EndFunction // }}}

// abhaengig vom Cmdlevel Privilegien vergeben
Function SetPrivsForCmdlevel(who) // {{{
  If (!who.isa(POLCLASS_MOBILE))
    syslog("FEHLER: SetPrivsForCmdlevel nicht mit Mobile aufgerufen");
    return;
  EndIf

  If (who.isa(POLCLASS_NPC))
    syslog("FEHLER: SetPrivsForCmdlevel ist nicht fuer NPCs gedacht");
  EndIf

  If (who.cmdlevel >= CMDLEVEL_QUESTCHAR)
    // damit er Questregelbrueche sehen kann
    GrantPrivilege(who, "seeghosts");
    GrantPrivilege(who, "seehidden");
  EndIf
  If (who.cmdlevel >= CMDLEVEL_SEER)
    // um Spieler nicht umzurennen, nicht zu sterben etc.
    GrantPrivilege(who, "dblclickany");
    GrantPrivilege(who, "freemove");
    GrantPrivilege(who, "hearghosts");
    GrantPrivilege(who, "invul");
    GrantPrivilege(who, "losany");
  EndIf
  If (who.cmdlevel >= CMDLEVEL_HIGHGM)
    // fuer Runen, Teleporter etc.
    GrantPrivilege(who, "seeinvisitems");
  EndIf
EndFunction // }}}

// TestMessage - DebugMeldung mittels .settalk
Function TestMessage(byref text, level, me := 0) // {{{
  If (!me)
    me := Self();
  EndIf

  var talkme := me.getprop("talkme");
  If (!talkme)
    return;
  EndIf

  If (CInt(talkme.stufe) >= level)
    var who := SystemFindObjectBySerial(talkme.serial);
    If (who.ip)
      SendSysMessagePergon(who, me.name+": "+text);
    Else
      me.eraseprop("talkme");
    EndIf
  EndIf
EndFunction // }}}

// vim: sw=2 sts=2
