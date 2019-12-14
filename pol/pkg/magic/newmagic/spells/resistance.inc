///////////////////////////////////////////////////////////////////////////
// include, um versch. Scripte, die Elementarangriffe, wie Feuer benutzen,
// zu vereinen

use math;
include "include/modifyskill";

Const ELEMENT_FIRE   := "fire";
Const ELEMENT_ICE    := "ice";
Const ELEMENT_WATER  := "water";
Const ELEMENT_EARTH  := "earth";
Const ELEMENT_AIR    := "air";
Const ELEMENT_POISON := "poison";

Function GetResistance(mobile, resist)
  // Eis = Wasser (kalt)
  If (resist == ELEMENT_ICE)
    resist := ELEMENT_WATER;
  EndIf

  var resistance := "resist_"+Lower(resist);
  var rawresist := CInt(GetObjProperty(mobile, resistance));

  If (!rawresist)
    // no resistance modifications on mobile
    rawresist := CDbl(0);
  EndIf

  // NPC?
  If (mobile.isa(POLCLASS_NPC))
    var elem := GetNPCConfig(mobile.npctemplate);
    If (elem)
      // mobile is NPC -> basic resistance from npcdesc.cfg
      var baseresist := GetElemProperty(elem, resistance);
      If (baseresist)
        // check if NPC is immune against element
        If (Lower(baseresist) == "immune")
          return 1;
        EndIf
        // add NPCs basic resistance to current resistance mods
        rawresist += CInt(baseresist);
      EndIf
    EndIf
  EndIf

  // add skill if magic resistance is called
  If (resistance == "resist_magic")
    rawresist += 2 * GetSkillPergon(mobile, SKILLID_MAGIERESISTENZ);
  EndIf

  // Tresholdfunktion
  var effresist := CDbl(2 / (1 + Pow(ConstE(), rawresist / -50.0)) - 1);

  return effresist;
EndFunction

Function SetResistance(mobile, resist, amount)
  var resistance := "resist_"+Lower(resist);

  SetObjProperty(mobile, resistance, CInt(amount));
EndFunction

Function MagicResistance (who, skill, amount, dmg, msg := 1)
  // Spruchdiff auf Magresi runterskalieren
  // 0.75 : Knochenbrecher 80% sollte 50/50 Chance geben bei 60% magresi
  // Skillbarkeit ist noch gewährleistet (hoffentlich)
  // Hauch des Todes geht jetzt auch mit 60%
  skill := CInt(0.75*skill);

  var magres := GetSkillPergon(who, SKILLID_MAGIERESISTENZ);
  If (!magres)
    // char has no MagicResistance -> full damage
    return dmg;
  EndIf

  // get bonus resistance (magic items etc.)
  var resbonus := CInt(GetObjProperty(who, "resist_magic"));
  If (!resbonus)
    resbonus := CDbl(0);
  EndIf

  // check for NPC
  If (who.isa(POLCLASS_NPC))
    var elem := GetNPCConfig(who.npctemplate);
    If (elem)
      // who is npc -> bonus resistance from npcdesc.cfg
      resbonus += GetConfigReal(elem, "resist_magic");
    EndIf
  EndIf

  If (!CheckSkillPergon(who, SKILLID_MAGIERESISTENZ, skill, skill*2))
    // Skillcheck for MR failed
    If (msg)
      SendSysMessagePergon(who,
        "Eure Magieresistenz reicht nicht aus, um Euch vor diesem "+
        "Zauber zu schuetzen",
        "Your Magic Resistance cannot provide you any protection "+
        "against this spell"
      );
    EndIf
    return dmg;
  EndIf

  // calc raw resistance and award bonus
  magres := 2*magres + resbonus;

  // calc effective resistance using Treshold Function
  var resistance := CDbl(2 / (1 + Pow(ConstE(), magres / -50.0)) - 1);

  // calc actual damage alteration
  var alteration := CDbl(1 - resistance * amount / 100.0);

  var msgstringeng;
  var msgstringger;
  // Customizing message
  If (amount == 0)
    msg := 0;
  ElseIf (alteration <= 0)
    alteration := 0;
    msgstringger := "die gesamte";
    msgstringeng := "the whole";
  ElseIf (alteration <= 0.05)
    msgstringger := "nahezu die gesamte";
    msgstringeng := "nearly the whole";
  ElseIf (alteration <= 0.15)
    msgstringger := "sehr viel von der";
    msgstringeng := "very much of the";
  ElseIf (alteration <= 0.25)
    msgstringger := "vieles von der";
    msgstringeng := "much of the";
  ElseIf (alteration <= 0.35)
    msgstringger := "eine Menge der";
    msgstringeng := "a lot of the";
  ElseIf (alteration <= 0.45)
    msgstringger := "mehr als die Haelfte der";
    msgstringeng := "more than half the";
  ElseIf (alteration <= 0.55)
    msgstringger := "ungefaehr die Haelfte der";
    msgstringeng := "about half the";
  ElseIf (alteration <= 0.65)
    msgstringger := "fast die Haelfte der";
    msgstringeng := "nearly half the";
  ElseIf (alteration <= 0.75)
    msgstringger := "etwas von der";
    msgstringeng := "some of the";
  ElseIf (alteration <= 0.85)
    msgstringger := "ein Weniges der";
    msgstringeng := "a bit of the";
  ElseIf (alteration <= 0.95)
    msgstringger := "sehr wenig von der";
    msgstringeng := "a tiny bit of the";
  ElseIf (alteration < 1)
    msgstringger := "nahezu nichts von der";
    msgstringeng := "nearly nothing of the";
  Else
    alteration := 1;
    msgstringger := "absolut nichts von der";
    msgstringeng := "absolutely nothing of the";
  EndIf

  // Sending message
  If (msg)
    SendSysMessagePergon(who,
      "Eure Magieresistenz absorbiert "+msgstringger+" Wirkung des Zaubers",
      "Your Magic Resistance absorbs "+msgstringeng+" spell"
    );
  EndIf

  // altering damage/effect
  return CDbl(dmg) * alteration;
EndFunction

// wendet die Resistenzen aus Array resist auf Schadenswert damage an
Function CalcResistedDamage(who, damage, resist)
  If (!resist)
    // keine Resistenzen erlaubt
    return damage;
  EndIf

  If (TypeOfInt(resist) != OT_ARRAY)
    return error{
      errortext := "CalcResistedDamage only takes 0 or array as resist!"
    };
  EndIf

  ForEach res in (resist)
    // FIXME: hier sollte Mathematik passieren
    syslog(res+" "+CDbl(GetResistance(who, res)));
  EndForEach
EndFunction

Const RESDAM_SHOW   := 1;
Const RESDAM_NOSHOW := 0;
Function ApplyResistedDamagePergon(who, damage, resist := 0, show := RESDAM_SHOW)
  If (resist)
    var newdamage := CalcResistedDamage(who, damage, resist);
    If (newdamage)
      damage := newdamage;
    EndIf
  EndIf
  If (show)
    ShowDamage(victim, who, rawDamage);
  EndIf
  ApplyDamagePergon(who, damage);
EndFunction

Function ApplyResistedRawDamagePergon(who, damage, resist := 0, show := RESDAM_SHOW)
  If (resist)
    var newdamage := CalcResistedDamage(who, damage, resist);
    If (newdamage)
      damage := newdamage;
    EndIf
  EndIf
  If (show)
    ShowDamage(victim, who, rawDamage);
  EndIf
  ApplyRawDamagePergon(who, damage);
EndFunction

// vim: sts=2 sw=2
