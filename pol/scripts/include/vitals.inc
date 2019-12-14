///////////////////////////////////////////////////////////////////////////
// Vitals -- alles zu Vitals, Stats&Co.
//
//
// Shinigami

// TODO:
// - Was passiert eigentlich, wenn ich GetAttribute&Co.
//   mit einer Zahl aufrufe? bzw. mit einem falschen string?

// Attribut-Manipulation
//  GetDexPergon              - Liefert die Geschicklichkeit             / 0 .. 9000
//  GetIntPergon              - Liefert die Intelligenz                  / 0 .. 9000
//  GetStrPergon              - Liefert die Staerke                      / 0 .. 9000
//  SetDexPergon              - Setzt die Geschicklichkeit               / 0,0 .. 6000,0 (Player 0,0 .. 209,0)
//  SetIntPergon              - Setzt die Intelligenz                    / 0,0 .. 6000,0 (Player 0,0 .. 209,0)
//  SetStrPergon              - Setzt die Staerke                        / 0,0 .. 6000,0 (Player 0,0 .. 209,0)
//  SetAttributPergon         - Setzt Geschicklichkeit, Intelligenz oder Staerke
//  GetDexBasePergon          - Liefert die Geschicklichkeit (ohne Mod)  / 0 .. 6000
//  GetIntBasePergon          - Liefert die Intelligenz (ohne Mod)       / 0 .. 6000
//  GetStrBasePergon          - Liefert die Staerke (ohne Mod)           / 0 .. 6000
//  GetDexModPergon           - Liefert den Geschicklichkeit-Modifikator / CInt(-3000,0 .. 3000,0)
//  GetIntModPergon           - Liefert den Intelligenz-Modifikator      / CInt(-3000,0 .. 3000,0)
//  GetStrModPergon           - Liefert den Staerke-Modifikator          / CInt(-3000,0 .. 3000,0)
//  SetDexModPergon           - Setzt den Geschicklichkeit-Modifikator   / -3000,0 .. 3000,0
//  SetIntModPergon           - Setzt den Intelligenz-Modifikator        / -3000,0 .. 3000,0
//  SetStrModPergon           - Setzt den Staerke-Modifikator            / -3000,0 .. 3000,0
//  SetAttributModPergon      - Setzt Geschicklichkeit-, Intelligenz- oder Staerke-Modifikator
//  AwardRawDexPergon         - Erhoeht die Raw Geschicklichkeit         / -0x7fffffff .. 0x7fffffff
//  AwardRawIntPergon         - Erhoeht die Raw Intelligenz              / -0x7fffffff .. 0x7fffffff
//  AwardRawStrPergon         - Erhoeht die Raw Staerke                  / -0x7fffffff .. 0x7fffffff
//  AwardRawAttributPergon    - Erhoeht die Raw Geschicklichkeit, Intelligenz oder Staerke
//  CheckAttributLimitsAndCut - Prueft die AttributLimits und schnippelt oder erhoeht ggf.
//  AlterDexModPergon         - Ändert den Geschicklichkeit-Modifikator   / -3000,0 .. 3000,0
//  AlterIntModPergon         - Ändert den Intelligenz-Modifikator        / -3000,0 .. 3000,0
//  AlterStrModPergon         - Ändert den Staerke-Modifikator            / -3000,0 .. 3000,0
//  SetAttributModPergon      - Setzt Geschicklichkeit-, Intelligenz- oder Staerke-Modifikator
//
// Vital-Manipulation
//  GetHPPergon               - Liefert die HitPoints                    / CInt(1,00 .. MaximumValue) (max. 100000,00)
//  GetManaPergon             - Liefert die Mana                         / CInt(1,00 .. MaximumValue) (max. 100000,00)
//  GetStaminaPergon          - Liefert die Stamina                      / CInt(1,00 .. MaximumValue) (max. 100000,00)
//  SetHPPergon               - Setzt die HitPoints                      / 1,00 .. MaximumValue (max. 100000,00)
//  SetManaPergon             - Setzt die Mana                           / 1,00 .. MaximumValue (max. 100000,00)
//  SetStaminaPergon          - Setzt die Stamina                        / 1,00 .. MaximumValue (max. 100000,00)
//  GetMaxHPPergon            - Liefert die maximalen HitPoints          / CInt(1,00 .. 100000,00)
//  GetMaxManaPergon          - Liefert die maximale Mana                / CInt(1,00 .. 100000,00)
//  GetMaxStaminaPergon       - Liefert die maximale Stamina             / CInt(1,00 .. 100000,00)
//  GetHPRegenRatePergon      - Liefert die HitPoints-Regenerationsrate  / -30000 .. 30000 [0,01 Punkte pro Minute]
//  GetManaRegenRatePergon    - Liefert die Mana-Regenerationsrate       / -30000 .. 30000 [0,01 Punkte pro Minute]
//  GetStaminaRegenRatePergon - Liefert die Stamina-Regenerationsrate    / -30000 .. 30000 [0,01 Punkte pro Minute]
//  OverrideNPCMaxVitals      - Nutzt andere Max-Werte als in der NPCDesc vorgegeben
//  ApplyDamagePergon         - Fuegt dem Opfer Schaden zu (Ruestungen werden beruecksichtigt)
//  ApplyRawDamagePergon      - Fuegt dem Opfer Schaden zu (Ruestungen werden ignoriert)
//  ApplyRangeDamagePergon    - Alle Opfer in range um pos werden in Abhaengigkeit von Distanz geschaedigt
//  LogLastDamage             - Loggt Informationen ueber den letzen Schaden
//  LogLastHit                - Loggt Informationen ueber den letzen Angriff
//
// Initialisierung
//  RestoreModPergon - Stellt diverse Modifikatoren wieder her (z.B. nach Server-Restart)
//
// Sonstiges
//  ResNow       - Spieler wiederbeleben
//  ResPenalties - Vital-/Statabzüge beim Wiederbeleben

///////////////////////////////////////////////////////////////////////////
// Benutzte CProps
//  CInt(St_*) - Raw Attribute

use attributes;
use uo;
include "include/debug";
include ":sqlmanager:sqlmanager";

// Konstanten (Attribute & Vitals)
Const ATTRIBUTEID_DEXTERITY             := "Dexterity";
Const ATTRIBUTEID_INTELLIGENCE          := "Intelligence";
Const ATTRIBUTEID_STRENGTH              := "Strength";
Const ATTRIBUTEID_DEXTERITY_DESC        := "Geschicklichkeit";
Const ATTRIBUTEID_INTELLIGENCE_DESC     := "Intelligenz";
Const ATTRIBUTEID_STRENGTH_DESC         := "Staerke";
Const ATTRIBUTEID_DEXTERITY_DESC_ENG    := "dexterity";
Const ATTRIBUTEID_INTELLIGENCE_DESC_ENG := "intelligence";
Const ATTRIBUTEID_STRENGTH_DESC_ENG     := "strength";

Const ATTRIBUTEID_MOD := "_Mod";
Const ATTRIBUTEWERTKENNUNG := "St_";

Const VITALID_LIFE    := "Life";
Const VITALID_MANA    := "Mana";
Const VITALID_STAMINA := "Stamina";

Const ATTRIBUTELIMITGMRAW_MAX := 0x7fffffff;
Const ATTRIBUTELIMITRAW_MAX   := 33554432;
Const ATTRIBUTELIMITRAW_MIN   := 2048;
Const ATTRIBUTELIMITGM_MAX    := 209;
Const ATTRIBUTELIMIT_MAX      := 150;
Const ATTRIBUTELIMIT_MIN      := 10;
// Welches Attribute soll gekuerzt werden?                   / String
//Const ATTRIBUTECAP := "AttributeCap";

// Konstanten (sonstige)
// Ab wieviel neuen RawPunkten wird ein Anstieg angezeigt?
Const STATADV_STUFE  := 500;
// 20% loss for 20 shortterm murders or more
Const STATLOSS_MAX := 0.20;

///////////////////////////
// Attribut-Manipulation //
///////////////////////////

/////////////////////////////////////////////////////////////////////////////
// AdvanceStatsPergon - Erhoeht die Stats nach einem erfolgreichen SkillCheck
/////////////////////////////////////////////////////////////////////////////
Function AdvanceStatsPergon(mobile, skillid, KorrFaktor := 1.0) // {{{
  If (!PlaceNoSkill(mobile)) // Knastologe bekommt keine Punkte
    var skillconf := GetSkillInfo(skillid);
    If (skillconf.?advdex) // Dexterity
      If (RandomInt(1000)<=skillconf.advdex[1])
        var points := Floor(RandomDiceRoll(skillconf.advdex[2])*KorrFaktor);
        AwardRawDexPergon(mobile, points);

        If (DEBUG_CHECKSKILL)
          If ((mobile.cmdlevel >= CMDLEVEL_SEER) && DEBUG_CHECKSKILLMESSAGE)
            SendSysMessagePergon(mobile, "Dexterity = "+GetDexPergon(mobile)+"/"+
              GetObjProperty(mobile, ATTRIBUTEWERTKENNUNG+ATTRIBUTEID_DEXTERITY)+"/"+points, "", _DEFAULT_TEXT_FONT, 38);
          EndIf
        EndIf
      EndIf
    EndIf
    If (skillconf.?advint) // Intelligence
      If (RandomInt(1000)<=skillconf.advint[1])
        var points := Floor(RandomDiceRoll(skillconf.advint[2])*KorrFaktor);
        AwardRawIntPergon(mobile, points);

        If (DEBUG_CHECKSKILL)
          If ((mobile.cmdlevel >= CMDLEVEL_SEER) && DEBUG_CHECKSKILLMESSAGE)
            SendSysMessagePergon(mobile, "Intelligence = "+GetIntPergon(mobile)+"/"+
              GetObjProperty(mobile, ATTRIBUTEWERTKENNUNG+ATTRIBUTEID_INTELLIGENCE)+"/"+points, "", _DEFAULT_TEXT_FONT, 38);
          EndIf
        EndIf
      EndIf
    EndIf
    If (skillconf.?advstr) // Strength
      If (RandomInt(1000)<=skillconf.advstr[1])
        var points := Floor(RandomDiceRoll(skillconf.advstr[2])*KorrFaktor);
        AwardRawStrPergon(mobile, points);

        If (DEBUG_CHECKSKILL)
          If ((mobile.cmdlevel >= CMDLEVEL_SEER) && DEBUG_CHECKSKILLMESSAGE)
            SendSysMessagePergon(mobile, "Strength = "+GetStrPergon(mobile)+"/"+
              GetObjProperty(mobile, ATTRIBUTEWERTKENNUNG+ATTRIBUTEID_STRENGTH)+"/"+points, "", _DEFAULT_TEXT_FONT, 38);
          EndIf
        EndIf
      EndIf
    EndIf
  EndIf
EndFunction // }}}

/////////////////////////////////////////////////////////////////////////
// Setzt Geschicklichkeit, Intelligenz oder Staerke
/////////////////////////////////////////////////////////////////////////
// value = 0,0..6000,0 (Player 0,0..209,0)
Function SetAttributPergon(mobile, attributeid, value) // {{{
  If (!mobile.isa(POLCLASS_NPC)) // Player
    value := Max(ATTRIBUTELIMIT_MIN, value);
    If (mobile.cmdlevel < CMDLEVEL_SEER)
      value := Min(ATTRIBUTELIMIT_MAX, value);
    Else
      value := Min(ATTRIBUTELIMITGM_MAX, value);
    EndIf

    SetObjProperty(
      mobile, ATTRIBUTEWERTKENNUNG+attributeid,
      BaseSkillToRawSkillPergon(CInt(value*10))
    );
  Else // NPC
    value := Max(0, Min(6000, value));
  EndIf

  If (DEBUG_STATS and DEBUG_RESULTS)
    If ((!mobile.isa(POLCLASS_NPC)) Or DEBUG_NPC)
      syslog("SetAttributPergon("+mobile.name+", "+attributeid+", "+value+")");
    EndIf
  EndIf

  If (SetAttributeBaseValue(mobile, attributeid, CInt(value*10)))
    RecalcVitalsPergon(mobile,attributeid);
    return 1;
  Else
    return 0;
  EndIf
EndFunction // }}}

///////////////////////////////////////////////////////////////////////////
// Ruft RecalcVitals abhängig vom veränderten Attribute/Vital auf
///////////////////////////////////////////////////////////////////////////
Function RecalcVitalsPergon(mobile,vital:=RECALC_VITALS)
  Case (vital)
    RECALC_VITALS:
      RecalcVitals(mobile,RECALC_NO_INTRINSIC_MOD,RECALC_VITALS);
    VITALID_LIFE:
    ATTRIBUTEID_STRENGTH:
      RecalcVitals(mobile,RECALC_NO_INTRINSIC_MOD,VITALID_LIFE);
    VITALID_STAMINA:
    ATTRIBUTEID_DEXTERITY:
      RecalcVitals(mobile,RECALC_NO_INTRINSIC_MOD,VITALID_STAMINA);
    VITALID_MANA:
    ATTRIBUTEID_INTELLIGENCE:
      RecalcVitals(mobile,RECALC_NO_INTRINSIC_MOD,VITALID_MANA);
  EndCase
EndFunction

///////////////////////////////////////////////////////////////////////////
// Setzt Modifikator fuer Geschicklichkeit, Intelligenz oder Staerke
///////////////////////////////////////////////////////////////////////////
// value = -3000,0..3000,0
Function SetAttributModPergon(mobile, attributeid, value) // {{{
  value:=Max(-3000,Min(3000, value));

  If (DEBUG_STATS and DEBUG_RESULTS)
    If ((!mobile.isa(POLCLASS_NPC)) Or DEBUG_NPC)
      syslog(
        "SetAttributModPergon("+mobile.name+", "+attributeid+", "+value+")"
      );
    EndIf
  EndIf

  If (SetAttributeTemporaryMod(mobile, attributeid, CInt(value*10)))
    // Zur Wiederherstellung beim Serverstart
    SetObjProperty(mobile, attributeid+ATTRIBUTEID_MOD, value);
    RecalcVitalsPergon(mobile,attributeid);
    return 1;
  Else
    return 0;
  EndIf
EndFunction // }}}

//////////////////////////////////////////////////////////////////////////////////////////
// Ändert Modifikator fuer Geschicklichkeit, Intelligenz oder Staerke
//////////////////////////////////////////////////////////////////////////////////////////
// value = -3000,0..3000,0
Function AlterAttributModPergon(mobile, attributeid, value) // {{{
  value := Max(-3000, Min(3000, value));

  If (DEBUG_STATS and DEBUG_RESULTS)
    If ((!mobile.isa(POLCLASS_NPC)) Or DEBUG_NPC)
      syslog(
        "AlterAttributModPergon("+mobile.name+", "+attributeid+", "+value+")"
      );
    EndIf
  EndIf

  If (AlterAttributeTemporaryMod(mobile, attributeid, CInt(value*10)))
    RecalcVitalsPergon(mobile,attributeid);
    return 1;
  Else
    return 0;
  EndIf
EndFunction // }}}


///////////////////////////////////////////////////////////////
// GetDexBasePergon - Liefert die Geschicklichkeit (ohne Mod)
///////////////////////////////////////////////////////////////
Function GetDexBasePergon(mobile) // result = 0..6000 {{{
  If (DEBUG_STATS and DEBUG_RESULTS)
    If ((!mobile.isa(POLCLASS_NPC)) Or DEBUG_NPC)
      syslog(
        "GetDexBasePergon("+mobile.name+") = "+
        CInt(GetAttributeBaseValue(mobile, ATTRIBUTEID_DEXTERITY))
      );
    EndIf
  EndIf

  return (CInt(GetAttributeBaseValue(mobile, ATTRIBUTEID_DEXTERITY)/10));
EndFunction // }}}

///////////////////////////////////////////////////////////////
// GetDexModPergon - Liefert den Geschicklichkeit-Modifikator
///////////////////////////////////////////////////////////////
Function GetDexModPergon(mobile) // result = CInt(-3000,0..3000,0) {{{
  If (DEBUG_STATS and DEBUG_RESULTS)
    If ((!mobile.isa(POLCLASS_NPC)) Or DEBUG_NPC)
      syslog(
        "GetDexModPergon("+mobile.name+") = "+
        CInt(GetAttributeTemporaryMod(mobile, ATTRIBUTEID_DEXTERITY)/10)
      );
    EndIf
  EndIf

  return (CInt(GetAttributeTemporaryMod(mobile, ATTRIBUTEID_DEXTERITY)/10));
EndFunction // }}}

//////////////////////////////////////////////
// GetDexPergon - Liefert die Geschicklichkeit
//////////////////////////////////////////////
Function GetDexPergon(mobile) // result = 0..9000 {{{
  If (DEBUG_STATS and DEBUG_RESULTS)
    If ((!mobile.isa(POLCLASS_NPC)) Or DEBUG_NPC)
      syslog(
        "GetDexPergon("+mobile.name+") = "+
        GetAttribute(mobile, ATTRIBUTEID_DEXTERITY)
      );
    EndIf
  EndIf

  return (GetAttribute(mobile, ATTRIBUTEID_DEXTERITY));
EndFunction // }}}

//////////////////////////////////////////////
// SetDexPergon - Setzt die Geschicklichkeit
//////////////////////////////////////////////
// value = 0,0..6000,0 (Player 0,0..209,0)
Function SetDexPergon(mobile, value) // {{{
  return (SetAttributPergon(mobile, ATTRIBUTEID_DEXTERITY, value));
EndFunction // }}}

/////////////////////////////////////////////////////////////
// SetDexModPergon - Setzt den Geschicklichkeit-Modifikator
/////////////////////////////////////////////////////////////
Function SetDexModPergon(mobile, value) // value = -3000,0..3000,0 {{{
  return (SetAttributModPergon(mobile, ATTRIBUTEID_DEXTERITY, value));
EndFunction // }}}

/////////////////////////////////////////////////////////////
// AlterDexModPergon - Setzt den Geschicklichkeit-Modifikator
/////////////////////////////////////////////////////////////
Function AlterDexModPergon(mobile, value) // value = -3000,0..3000,0 {{{
  return (AlterAttributModPergon(mobile, ATTRIBUTEID_DEXTERITY, value));
EndFunction // }}}


//////////////////////////////////////////////////////////
// GetIntBasePergon - Liefert die Intelligenz (ohne Mod)
//////////////////////////////////////////////////////////
Function GetIntBasePergon(mobile) // result = 0..6000 {{{
  If (DEBUG_STATS and DEBUG_RESULTS)
    If ((!mobile.isa(POLCLASS_NPC)) Or DEBUG_NPC)
      syslog(
        "GetIntBasePergon("+mobile.name+") = "+
        CInt(GetAttributeBaseValue(mobile, ATTRIBUTEID_INTELLIGENCE))
      );
    EndIf
  EndIf

  return (CInt(GetAttributeBaseValue(mobile, ATTRIBUTEID_INTELLIGENCE)/10));
EndFunction // }}}

//////////////////////////////////////////////////////////
// GetIntModPergon - Liefert den Intelligenz-Modifikator
//////////////////////////////////////////////////////////
Function GetIntModPergon(mobile) // result = CInt(-3000,0..3000,0) {{{
  If (DEBUG_STATS and DEBUG_RESULTS)
    If ((!mobile.isa(POLCLASS_NPC)) Or DEBUG_NPC)
      syslog(
        "GetIntModPergon("+mobile.name+") = "+
        CInt(GetAttributeTemporaryMod(mobile, ATTRIBUTEID_INTELLIGENCE)/10)
      );
    EndIf
  EndIf

  return (CInt(GetAttributeTemporaryMod(mobile, ATTRIBUTEID_INTELLIGENCE)/10));
EndFunction // }}}

///////////////////////////////////////////
// GetIntPergon - Liefert die Intelligenz
///////////////////////////////////////////
Function GetIntPergon(mobile) // result = 0..9000 {{{
  If (DEBUG_STATS and DEBUG_RESULTS)
    If ((!mobile.isa(POLCLASS_NPC)) Or DEBUG_NPC)
      syslog(
        "GetIntPergon("+mobile.name+") = "+
        GetAttribute(mobile, ATTRIBUTEID_INTELLIGENCE)
      );
    EndIf
  EndIf

  return (GetAttribute(mobile, ATTRIBUTEID_INTELLIGENCE));
EndFunction // }}}

/////////////////////////////////////////
// SetIntPergon - Setzt die Intelligenz
/////////////////////////////////////////
// value = 0,0..6000,0 (Player 0,0..209,0)
Function SetIntPergon(mobile, value) // {{{
  return (SetAttributPergon(mobile, ATTRIBUTEID_INTELLIGENCE, value));
EndFunction // }}}

////////////////////////////////////////////////////////
// SetIntModPergon - Setzt den Intelligenz-Modifikator
////////////////////////////////////////////////////////
Function SetIntModPergon(mobile, value) // value = -3000,0..3000,0 {{{
  return (SetAttributModPergon(mobile, ATTRIBUTEID_INTELLIGENCE, value));
EndFunction // }}}

////////////////////////////////////////////////////////
// AlterIntModPergon - Ändert den Intelligenz-Modifikator
////////////////////////////////////////////////////////
Function AlterIntModPergon(mobile, value) // value = -3000,0..3000,0 {{{
  return (AlterAttributModPergon(mobile, ATTRIBUTEID_INTELLIGENCE, value));
EndFunction // }}}


//////////////////////////////////////////////////////
// GetStrBasePergon - Liefert die Staerke (ohne Mod)
//////////////////////////////////////////////////////
Function GetStrBasePergon(mobile) // result = 0..6000 {{{
  If (DEBUG_STATS and DEBUG_RESULTS)
    If ((!mobile.isa(POLCLASS_NPC)) Or DEBUG_NPC)
      syslog(
        "GetStrBasePergon("+mobile.name+") = "+
        CInt(GetAttributeBaseValue(mobile, ATTRIBUTEID_STRENGTH))
      );
    EndIf
  EndIf

  return (CInt(GetAttributeBaseValue(mobile, ATTRIBUTEID_STRENGTH)/10));
EndFunction // }}}

//////////////////////////////////////////////////////
// GetStrModPergon - Liefert den Staerke-Modifikator
//////////////////////////////////////////////////////
Function GetStrModPergon(mobile) // result = CInt(-3000,0..3000,0) {{{
  If (DEBUG_STATS and DEBUG_RESULTS)
    If ((!mobile.isa(POLCLASS_NPC)) Or DEBUG_NPC)
      syslog(
        "GetStrModPergon("+mobile.name+") = "+
        CInt(GetAttributeTemporaryMod(mobile, ATTRIBUTEID_STRENGTH)/10)
      );
    EndIf
  EndIf

  return (CInt(GetAttributeTemporaryMod(mobile, ATTRIBUTEID_STRENGTH)/10));
EndFunction // }}}

///////////////////////////////////////
// GetStrPergon - Liefert die Staerke
///////////////////////////////////////
Function GetStrPergon(mobile) // result = 0..9000 {{{
  If (DEBUG_STATS and DEBUG_RESULTS)
    If ((!mobile.isa(POLCLASS_NPC)) Or DEBUG_NPC)
      syslog(
        "GetStrPergon("+mobile.name+") = "+
        GetAttribute(mobile, ATTRIBUTEID_STRENGTH)
      );
    EndIf
  EndIf

  return (GetAttribute(mobile, ATTRIBUTEID_STRENGTH));
EndFunction // }}}

/////////////////////////////////////
// SetStrPergon - Setzt die Staerke
/////////////////////////////////////
// value = 0,0..6000,0 (Player 0,0..209,0)
Function SetStrPergon(mobile, value) // {{{
  return (SetAttributPergon(mobile, ATTRIBUTEID_STRENGTH, value));
EndFunction // }}}

////////////////////////////////////////////////////
// SetStrModPergon - Setzt den Staerke-Modifikator
////////////////////////////////////////////////////
Function SetStrModPergon(mobile, value) // value = -3000,0..3000,0 {{{
  return (SetAttributModPergon(mobile, ATTRIBUTEID_STRENGTH, value));
EndFunction // }}}

////////////////////////////////////////////////////
// AlterStrModPergon - Ändert den Staerke-Modifikator
////////////////////////////////////////////////////
Function AlterStrModPergon(mobile, value) // value = -3000,0..3000,0 {{{
  return (AlterAttributModPergon(mobile, ATTRIBUTEID_STRENGTH, value));
EndFunction // }}}

////////////////
// Rohzugriff //
////////////////

////////////////////////////////////////////////////////////////////////////////////////
// AttributeCanLower - kann gekürzt werden? mind. 11 und lockdown status
////////////////////////////////////////////////////////////////////////////////////////
Function AttributeCanLower(mobile, attribute)
  If (GetAttributeLock(mobile,attribute)==ATTRIBUTE_LOCK_DOWN)
    If (CInt(GetAttributeBaseValue(mobile, attribute))>=110)
      Return (1);
    EndIf
  EndIf
  Return (0);
EndFunction

////////////////////////////////////////////////////////////////////////////////////////
// AwardRawAttributPergon - Erhoeht die Raw Geschicklichkeit, Intelligenz oder Staerke
////////////////////////////////////////////////////////////////////////////////////////
// rawpoints = -0x7fffffff..0x7fffffff
Function AwardRawAttributPergon(mobile, attributeid, attributeid_desc, attributeid_desc_eng, rawpoints) // {{{
  If (!mobile.isa(POLCLASS_NPC)) // Player
    Var lockstate:=GetAttributeLock(mobile,attributeid);
    If (lockstate==ATTRIBUTE_LOCK_LOCKED) // Lock wirkt sofort
      Return 0;
    EndIf
    var old_crit := Is_Critical();
    Set_Critical(1);
    var oldrawvalue := GetObjProperty(mobile, ATTRIBUTEWERTKENNUNG+attributeid);
    If (!oldrawvalue)
      var value := Min(2090, GetAttributeBaseValue(mobile, attributeid));

      oldrawvalue := BaseSkillToRawSkillPergon(value);
    EndIf

    var rawvalue := Max(ATTRIBUTELIMITRAW_MIN,oldrawvalue+CInt(rawpoints));
    If (mobile.cmdlevel<CMDLEVEL_SEER)
      rawvalue:=Min(ATTRIBUTELIMITRAW_MAX, rawvalue);
    Else
      rawvalue:=Min(ATTRIBUTELIMITGMRAW_MAX, rawvalue);
    EndIf
    
    //Stat Summe ohne Nachkommastellen sonst addieren die sich mit
    var sum:=CInt(RawSkillToBaseSkillPergon(rawvalue)/10);
    Case (attributeid)
      ATTRIBUTEID_STRENGTH:
        sum+=GetIntBasePergon(mobile)+GetDexBasePergon(mobile);
      ATTRIBUTEID_DEXTERITY:
        sum+=GetIntBasePergon(mobile)+GetStrBasePergon(mobile);
      ATTRIBUTEID_INTELLIGENCE:
        sum+=GetStrBasePergon(mobile)+GetDexBasePergon(mobile);
    EndCase

    If (sum > 300)
      If (lockstate==ATTRIBUTE_LOCK_DOWN) // Down wirkt nur wenn Cap erreicht
        Set_Critical(old_crit);
        Return 0;
      EndIf
      Case (attributeid)
        ATTRIBUTEID_STRENGTH:
          If (AttributeCanLower(mobile,ATTRIBUTEID_DEXTERITY))
            SetDexPergon(mobile, GetDexBasePergon(mobile)-1);
          ElseIf (AttributeCanLower(mobile,ATTRIBUTEID_INTELLIGENCE))
            SetIntPergon(mobile, GetIntBasePergon(mobile)-1);
          Else
            Set_Critical(old_crit);
            Return 0;
          EndIf
        ATTRIBUTEID_DEXTERITY:
          If (AttributeCanLower(mobile,ATTRIBUTEID_INTELLIGENCE))
            SetIntPergon(mobile, GetIntBasePergon(mobile)-1);
          ElseIf (AttributeCanLower(mobile,ATTRIBUTEID_STRENGTH))
            SetStrPergon(mobile, GetStrBasePergon(mobile)-1);
          Else
            Set_Critical(old_crit);
            Return 0;
          EndIf
        ATTRIBUTEID_INTELLIGENCE:
          If (AttributeCanLower(mobile,ATTRIBUTEID_DEXTERITY))
            SetDexPergon(mobile, GetDexBasePergon(mobile)-1);
          ElseIf (AttributeCanLower(mobile,ATTRIBUTEID_STRENGTH))
            SetStrPergon(mobile, GetStrBasePergon(mobile)-1);
          Else
            Set_Critical(old_crit);
            Return 0;
          EndIf
      EndCase
    EndIf
    SetObjProperty(mobile, ATTRIBUTEWERTKENNUNG+attributeid, rawvalue);

    var res:=SetAttributeBaseValue(mobile, attributeid, CInt(RawSkillToBaseSkillPergon(rawvalue)));
    Set_Critical(old_crit);

    // Statanstiegsmeldung anzeigen
    If (CInt(oldrawvalue/STATADV_STUFE)<CInt(rawvalue/STATADV_STUFE))
      SendSysMessagePergon(mobile,
        "Eure "+attributeid_desc+" hat sich verbessert.",
        "Your "+attributeid_desc_eng+" improved.",
        _DEFAULT_TEXT_FONT, FONTCOLOR_GREEN_DARK);
    EndIf

    If (DEBUG_STATS and DEBUG_RESULTS)
      If ((!mobile.isa(POLCLASS_NPC)) Or DEBUG_NPC)
        syslog(
          "AwardRawAttributPergon("+mobile.name+", "+
          attributeid+", "+rawpoints+")"
        );
      EndIf
    EndIf

    If (res)
      RecalcVitalsPergon(mobile,attributeid);
      return 1;
    Else
      return 0;
    EndIf
  Else // NPC
    If (DEBUG_STATS)
      If (((!mobile.isa(POLCLASS_NPC)) Or DEBUG_NPC) And DEBUG_NPCWARNUNG)
        syslog("WARNUNG: NPCs duerfen AwardRawAttributPergon nicht benutzen!");
      EndIf
    EndIf

    return 0;
  EndIf
EndFunction // }}}

/////////////////////////////////////////////////////////
// AwardRawDexPergon - Erhoeht die Raw Geschicklichkeit
/////////////////////////////////////////////////////////
// rawpoints = -0x7fffffff..0x7fffffff
Function AwardRawDexPergon(mobile, rawpoints) // {{{
  return (AwardRawAttributPergon(mobile, ATTRIBUTEID_DEXTERITY, ATTRIBUTEID_DEXTERITY_DESC, ATTRIBUTEID_DEXTERITY_DESC_ENG, rawpoints));
EndFunction // }}}

////////////////////////////////////////////////////
// AwardRawIntPergon - Erhoeht die Raw Intelligenz
////////////////////////////////////////////////////
// rawpoints = -0x7fffffff..0x7fffffff
Function AwardRawIntPergon(mobile, rawpoints) // {{{
  return (AwardRawAttributPergon(mobile, ATTRIBUTEID_INTELLIGENCE, ATTRIBUTEID_INTELLIGENCE_DESC, ATTRIBUTEID_INTELLIGENCE_DESC_ENG, rawpoints));
EndFunction // }}}

////////////////////////////////////////////////
// AwardRawStrPergon - Erhoeht die Raw Staerke
////////////////////////////////////////////////
// rawpoints = -0x7fffffff..0x7fffffff
Function AwardRawStrPergon(mobile, rawpoints) // {{{
  return (AwardRawAttributPergon(mobile, ATTRIBUTEID_STRENGTH, ATTRIBUTEID_STRENGTH_DESC, ATTRIBUTEID_STRENGTH_DESC_ENG, rawpoints));
EndFunction // }}}

///////////////////////////////////////////////////////////////////////////////////////////
// CheckAttributLimitsAndCut - Prueft die AttributLimits und schnippelt oder erhoeht ggf.
///////////////////////////////////////////////////////////////////////////////////////////
Function CheckAttributLimitsAndCut(mobile) // {{{
  AwardRawDexPergon(mobile, 0);
  AwardRawIntPergon(mobile, 0);
  AwardRawStrPergon(mobile, 0);
EndFunction // }}}


////////////////////////
// Vital-Manipulation //
////////////////////////

/////////////////////////////////////////////////////
// GetMaxHPPergon - Liefert die maximalen HitPoints
/////////////////////////////////////////////////////
// result = CInt(1,00..100000,00)
Function GetMaxHPPergon(object) // {{{
  If (object.isa(POLCLASS_MOBILE))
    If (DEBUG_VITALS and DEBUG_RESULTS)
      If ((!object.isa(POLCLASS_NPC)) Or DEBUG_NPC)
        syslog("GetMaxHPPergon("+object.name+") = "+CInt(GetVitalMaximumValue(object, VITALID_LIFE)/100));
      EndIf
    EndIf

    return (CInt(GetVitalMaximumValue(object, VITALID_LIFE)/100));
  Else
    If (DEBUG_ITEMS)
      If ((!object.isa(POLCLASS_WEAPON)) And (!object.isa(POLCLASS_ARMOR)))
        syslog(
          "FEHLER: GetMaxHPPergon wurde mit falscher Objekt-Klasse ("+
          TypeOf(object)+") aufgerufen!"
        );
      EndIf
      If (DEBUG_RESULTS)
        syslog("GetMaxHPPergon("+object.name+") = "+object.maxhp);
      EndIf
    EndIf

    return (object.maxhp);
  EndIf
EndFunction // }}}

////////////////////////////////////////
// GetHPPergon - Liefert die HitPoints
////////////////////////////////////////
// result = CInt(1,00 .. MaximumValue) (max. 100000,00)
Function GetHPPergon(object) // {{{
  If (object.isa(POLCLASS_MOBILE))
    If (DEBUG_VITALS and DEBUG_RESULTS)
      If ((!object.isa(POLCLASS_NPC)) Or DEBUG_NPC)
        syslog(
          "GetHPPergon("+object.name+") = "+
          CInt(GetVital(object, VITALID_LIFE)/100)
        );
      EndIf
    EndIf

    return (CInt(GetVital(object, VITALID_LIFE)/100));
  Else
    If (DEBUG_ITEMS)
      If ((!object.isa(POLCLASS_WEAPON)) And (!object.isa(POLCLASS_ARMOR)))
        syslog(
          "FEHLER: GetHPPergon wurde mit falscher Objekt-Klasse ("+
          TypeOf(object)+") aufgerufen!"
        );
      EndIf
      If (DEBUG_RESULTS)
        syslog("GetHPPergon("+object.name+") = "+object.hp);
      EndIf
    EndIf

    return (object.hp);
  EndIf
EndFunction // }}}

//////////////////////////////////////
// SetHPPergon - Setzt die HitPoints
//////////////////////////////////////
// value = 1,00 .. MaximumValue (max. 100000,00)
Function SetHPPergon(object, value) // {{{
  value := Max(0, Min(GetMaxHPPergon(object), value));

  If (value < GetHPPergon(object)) // ihm sollen HPs abgezogen werden ...
    LogLastDamage(object, "SetHP", value);
  EndIf

  If (object.isa(POLCLASS_MOBILE))
    If (DEBUG_VITALS and DEBUG_RESULTS)
      If ((!object.isa(POLCLASS_NPC)) Or DEBUG_NPC)
        syslog("SetHPPergon("+object.name+", "+value+")");
      EndIf
    EndIf

    return (SetVital(object, VITALID_LIFE, CInt(value*100)));
  Else
    If (DEBUG_ITEMS)
      If ((!object.isa(POLCLASS_WEAPON)) And (!object.isa(POLCLASS_ARMOR)))
        syslog(
          "FEHLER: SetHPPergon wurde mit falscher Objekt-Klasse ("+
          TypeOf(object)+") aufgerufen!"
        );
      EndIf
      If (DEBUG_RESULTS)
        syslog("SetHPPergon("+object.name+", "+value+")");
      EndIf
    EndIf

    object.hp := value;
    return 1;
  EndIf
EndFunction // }}}


/////////////////////////////////////////////////
// GetMaxManaPergon - Liefert die maximale Mana
/////////////////////////////////////////////////
// result = CInt(1,00..100000,00)
Function GetMaxManaPergon(mobile) // {{{
  If (DEBUG_VITALS and DEBUG_RESULTS)
    If ((!mobile.isa(POLCLASS_NPC)) Or DEBUG_NPC)
      syslog(
        "GetMaxManaPergon("+mobile.name+") = "+
        CInt(GetVitalMaximumValue(mobile, VITALID_MANA)/100)
      );
    EndIf
  EndIf

  return (CInt(GetVitalMaximumValue(mobile, VITALID_MANA)/100));
EndFunction // }}}

/////////////////////////////////////
// GetManaPergon - Liefert die Mana
/////////////////////////////////////
// result = CInt(1,00 .. MaximumValue) (max. 100000,00)
Function GetManaPergon(mobile) // {{{
  If (DEBUG_VITALS and DEBUG_RESULTS)
    If ((!mobile.isa(POLCLASS_NPC)) Or DEBUG_NPC)
      syslog(
        "GetManaPergon("+mobile.name+") = "+
        CInt(GetVital(mobile, VITALID_MANA)/100)
      );
    EndIf
  EndIf

  return (CInt(GetVital(mobile, VITALID_MANA)/100));
EndFunction // }}}

///////////////////////////////////
// SetManaPergon - Setzt die Mana
///////////////////////////////////
// value = 1,00 .. MaximumValue (max. 100000,00)
Function SetManaPergon(mobile, value) // {{{
  value := Max(0, Min(GetMaxManaPergon(mobile), value));

  If (DEBUG_VITALS and DEBUG_RESULTS)
    If ((!mobile.isa(POLCLASS_NPC)) Or DEBUG_NPC)
      syslog("SetManaPergon("+mobile.name+", "+value+")");
    EndIf
  EndIf

  return (SetVital(mobile, VITALID_MANA, CInt(value*100)));
EndFunction // }}}


///////////////////////////////////////////////////////
// GetMaxStaminaPergon - Liefert die maximale Stamina
///////////////////////////////////////////////////////
// result = CInt(1,00..100000,00)
Function GetMaxStaminaPergon(mobile) // {{{
  If (DEBUG_VITALS and DEBUG_RESULTS)
    If ((!mobile.isa(POLCLASS_NPC)) Or DEBUG_NPC)
      syslog(
        "GetMaxStaminaPergon("+mobile.name+") = "+
        CInt(GetVitalMaximumValue(mobile, VITALID_STAMINA)/100)
      );
    EndIf
  EndIf

  return (CInt(GetVitalMaximumValue(mobile, VITALID_STAMINA)/100));
EndFunction // }}}

///////////////////////////////////////////
// GetStaminaPergon - Liefert die Stamina
///////////////////////////////////////////
// result = CInt(1,00 .. MaximumValue) (max. 100000,00)
Function GetStaminaPergon(mobile) // {{{
  If (DEBUG_VITALS and DEBUG_RESULTS)
    If ((!mobile.isa(POLCLASS_NPC)) Or DEBUG_NPC)
      syslog(
        "GetStaminaPergon("+mobile.name+") = "+
        CInt(GetVital(mobile, VITALID_STAMINA)/100)
      );
    EndIf
  EndIf

  return (CInt(GetVital(mobile, VITALID_STAMINA)/100));
EndFunction // }}}

/////////////////////////////////////////
// SetStaminaPergon - Setzt die Stamina
/////////////////////////////////////////
// value = 1,00 .. MaximumValue (max. 100000,00)
Function SetStaminaPergon(mobile, value) // {{{
  value:=Max(0, Min(GetMaxStaminaPergon(mobile), value));

  If (DEBUG_VITALS and DEBUG_RESULTS)
    If ((!mobile.isa(POLCLASS_NPC)) Or DEBUG_NPC)
      syslog("SetStaminaPergon("+mobile.name+", "+value+")");
    EndIf
  EndIf

  return (SetVital(mobile, VITALID_STAMINA, CInt(value*100)));
EndFunction // }}}


///////////////////////////////////////////////////////////////////
// GetHPRegenRatePergon - Liefert die HitPoints-Regenerationsrate
///////////////////////////////////////////////////////////////////
// result = -30000..30000 [0,01 Punkte pro Minute]
Function GetHPRegenRatePergon(mobile) // {{{
  If (DEBUG_VITALS and DEBUG_RESULTS)
    If ((!mobile.isa(POLCLASS_NPC)) Or DEBUG_NPC)
      syslog(
        "GetHPRegenRatePergon("+mobile.name+") = "+
        GetVitalRegenRate(mobile, VITALID_LIFE)
      );
    EndIf
  EndIf

  return (GetVitalRegenRate(mobile, VITALID_LIFE));
EndFunction // }}}

////////////////////////////////////////////////////////////////
// GetManaRegenRatePergon - Liefert die Mana-Regenerationsrate
////////////////////////////////////////////////////////////////
// result = -30000..30000 [0,01 Punkte pro Minute]
Function GetManaRegenRatePergon(mobile) // {{{
  If (DEBUG_VITALS and DEBUG_RESULTS)
    If ((!mobile.isa(POLCLASS_NPC)) Or DEBUG_NPC)
      syslog(
        "GetManaRegenRatePergon("+mobile.name+") = "+
        GetVitalRegenRate(mobile, VITALID_MANA)
      );
    EndIf
  EndIf

  return (GetVitalRegenRate(mobile, VITALID_MANA));
EndFunction // }}}

//////////////////////////////////////////////////////////////////////
// GetStaminaRegenRatePergon - Liefert die Stamina-Regenerationsrate
//////////////////////////////////////////////////////////////////////
// result = -30000..30000 [0,01 Punkte pro Minute]
Function GetStaminaRegenRatePergon(mobile) // {{{
  If (DEBUG_VITALS and DEBUG_RESULTS)
    If ((!mobile.isa(POLCLASS_NPC)) Or DEBUG_NPC)
      syslog(
        "GetStaminaRegenRatePergon("+mobile.name+") = "+
        GetVitalRegenRate(mobile, VITALID_STAMINA)
      );
    EndIf
  EndIf

  return (GetVitalRegenRate(mobile, VITALID_STAMINA));
EndFunction // }}}

///////////////////////////////////////////////////////////////////////////
// OverrideNPCMaxVitals - Nutzt andere Max-Werte als in der NPCDesc vorgegeben
///////////////////////////////////////////////////////////////////////////
// 0 -> Loeschen (d.h. NPCDesc-default) / -1 -> ignorieren
Function OverrideNPCMaxVitals(mobile, maxhp, maxmana, maxstamina) // {{{
  If (DEBUG_VITALS)
    If (!mobile.isa(POLCLASS_NPC))
      syslog(
        "FEHLER: OverrideNPCMaxVitals wurde mit falscher Objekt-Klasse ("+
        TypeOf(mobile)+") aufgerufen!"
      );
    EndIf
  EndIf

  If (maxhp == 0)
    EraseObjProperty(mobile, "OverrideHits");
  ElseIf (maxhp<>-1)
    SetObjProperty(mobile, "OverrideHits", CInt(maxhp));
  EndIf

  If (maxmana == 0)
    EraseObjProperty(mobile, "OverrideMana");
  ElseIf (maxmana<>-1)
    SetObjProperty(mobile, "OverrideMana", CInt(maxmana));
  EndIf

  If (maxstamina == 0)
    EraseObjProperty(mobile, "OverrideStamina");
  ElseIf (maxstamina<>-1)
    SetObjProperty(mobile, "OverrideStamina", CInt(maxstamina));
  EndIf

  RecalcVitalsPergon(mobile,RECALC_VITALS);
EndFunction // }}}

///////////////////////////////////////////////////////////////////////////
// Fuegt dem Opfer Schaden zu (Ruestungen werden beruecksichtigt)
///////////////////////////////////////////////////////////////////////////
Function ApplyDamagePergon(mobile, value, userepsys := DAMAGE_USE_REPSYS) // {{{
  If (!mobile.enabled("invul"))
    LogLastDamage(mobile, "Damage", value);
    ApplyDamage(mobile, CInt(value), userepsys);
  EndIf
EndFunction // }}}

///////////////////////////////////////////////////////////////////////////
// Fuegt dem Opfer Schaden zu (Ruestungen werden ignoriert)
///////////////////////////////////////////////////////////////////////////
Function ApplyRawDamagePergon(mobile, value, userepsys := DAMAGE_USE_REPSYS) // {{{
  If (!mobile.enabled("invul"))
    LogLastDamage(mobile, "RawDamage", value);
    ApplyRawDamage(mobile, CInt(value), userepsys);
  EndIf
EndFunction // }}}

//  Alle Opfer in range um pos werden in Abhaengigkeit von Distanz geschaedigt
Const DAMAGE_RAW   := 0;
Const DAMAGE_ARMOR := 1;
Function ApplyRangeDamagePergon(pos, damage, range, type := DAMAGE_RAW, userepsys := DAMAGE_USE_REPSYS)
  ForEach victim in (ListMobilesNearLocationEx(
      pos.x, pos.y, pos.z, range,
      LISTEX_FLAG_NORMAL+LISTEX_FLAG_HIDDEN, pos.realm
  ))
    SleepMs(2);
    If (victim.cmdlevel >= CMDLEVEL_SEER)
      // Staff nicht klatschen
      continue;
    EndIf

    // Schaden abhaengig von Distanz ermitteln
    var dist := CoordinateDistance(pos.x, pos.y, victim.x, victim.y);
    var distdmg;
    If (dist < range/3.0)
      // nah am Zentrum (inneres Drittel des Bereichs)
      distdmg := damage;
    Else
      // abfallende Gerade von (range/3, damage) nach (range, 0)
      // Max-Schutz vor negativem Schaden falls Rundungsfehler
      distdmg := Max(0, CInt((3.0*damage/2.0 * (-1.0*dist/range + 1.0))));
    EndIf

    // Schaden verteilen
    If (distdmg)
      If (type == DAMAGE_RAW)
        ApplyRawDamagePergon(victim, distdmg, userepsys);
      Else
        ApplyDamagePergon(victim, distdmg, userepsys);
      EndIf
    EndIf
  EndForEach
EndFunction

/////////////////////////////////////////////////////////////////
// LogLastDamage - Loggt Informationen ueber den letzen Schaden
/////////////////////////////////////////////////////////////////
Function LogLastDamage(mobile, type, value) // {{{
  var animallog := 0;
  If (mobile.isa(POLCLASS_NPC))
    If (
      // Eigentum oder
      mobile.master or
      // domestiziertes Tierchen
      mobile.script in {":tierzucht:m_domest", ":tierzucht:w_domest"}
    )
      var masterlog := "Domest";
      If (mobile.master)
        masterlog := {mobile.master.name, mobile.master.serial};
      EndIf

      animallog := {
        mobile.npctemplate, masterlog, mobile.x, mobile.y, mobile.z
      };
    EndIf
  EndIf

  // Liefert denjenigen, der fuer den verursachten Schaden verantwortlich ist
  var attacker := SetScriptController(0);
  SetScriptController(attacker);

  var attackerlog := array{"Niemand", 0};
  If (attacker)
    attackerlog := array{attacker.name, attacker.serial};
  EndIf

  SetObjProperty(mobile, LOG_LASTDAMAGE,
    {
      type, attackerlog, animallog, ReadGameClock(),
      {
        GetHPPergon(mobile), -value
      },
      GetProcess(GetPID()).name
    }
  );

  // Wer macht den meisten Schaden?
  // Könnte man zB Lootrechte oder KI Gegnerwechsel dranhängen
  If (mobile.isA(POLCLASS_NPC))
    If (attacker)
      If (attacker.isA(POLCLASS_NPC))
        If (attacker.master)
          attacker := attacker.master;
        Else
          return;
        EndIf
      EndIf
      var oldcrit := Is_Critical();
      Set_Critical(1);
      var logdamager := mobile.getprop(LOG_DAMAGER);
      If (!logdamager)
        logdamager := dictionary;
      EndIf
      If (!logdamager.exists(attacker.serial))
        logdamager[attacker.serial] := struct;
        logdamager[attacker.serial].+dmg := 0;
        logdamager[attacker.serial].+time := 0;
      EndIf
      logdamager[attacker.serial].dmg += value;
      logdamager[attacker.serial].time := ReadGameClock();
      mobile.setprop(LOG_DAMAGER,logdamager);
      Set_Critical(oldcrit);
    EndIf
  EndIf
EndFunction // }}}

//////////////////////////////////////////////////////////////
// LogLastHit - Loggt Informationen ueber den letzen Angriff
//////////////////////////////////////////////////////////////
Function LogLastHit(defender, attacker, weapon) // {{{
  var lasthitlog := 0;
  If (defender.isa(POLCLASS_NPC))
    If (
      // Eigentum oder
      defender.master or
      // domestiziertes Tierchen
      defender.script in {":tierzucht:m_domest", ":tierzucht:w_domest"}
    )
      lasthitlog := 1;
    EndIf
  Else
    lasthitlog := 1;
  EndIf

  If (lasthitlog)
    SetObjProperty(
      defender,
      LOG_LASTHIT,
      {attacker.name, attacker.serial, ReadGameClock(), weapon.desc}
    );
  EndIf
EndFunction // }}}

//////////////////////////////////////////////////////////////
// GetMostRecentTotalDamager - Liefert Mobileref der größten Damager	
//////////////////////////////////////////////////////////////
Function GetMostRecentTotalDamager(mobile, top := 1, decay := 120) // {{{
  var logdamager;
  If (mobile.isA(POLCLASS_MOBILE)) 
    logdamager := mobile.getprop(LOG_DAMAGER);
  Else
    logdamager := mobile;
  EndIf
  var i;
 
  If (logdamager)
    For (i := 1;i<=logdamager.keys().size();i += 1)
      If (ReadGameClock()>logdamager[logdamager.keys()[i]].time+decay)
        logdamager.erase(logdamager.keys()[i]);
        i -= 1;
      EndIf
    EndFor
  EndIf
  
  var damager := 0;
  If (logdamager)  // Noch was zu tun?
    var total := -1;
    If (top == 1)  // Die einfache Variante
      For (i := 1;i<=logdamager.keys().size();i += 1)
        If (logdamager[logdamager.keys()[i]].dmg>total)
          If (SystemFindObjectBySerial(logdamager.keys()[i])) // auch online
            damager := logdamager.keys()[i];
            total := logdamager[logdamager.keys()[i]].dmg;
          EndIf
        EndIf
      EndFor
      If (damager)
        damager := SystemFindObjectBySerial(damager);
      EndIf
    Else
      damager := {0};
      total := {0};
      var j;
      For (i := 1;i<=logdamager.keys().size();i += 1)
        For (j := 1;j<=total.size();j += 1)
          If (logdamager[logdamager.keys()[i]].dmg>total[j])
            If (SystemFindObjectBySerial(logdamager.keys()[i])) // auch online
              damager.insert(j,logdamager.keys()[i]);
              total.insert(j,logdamager[logdamager.keys()[i]].dmg);
              damager.shrink(top);  // wieder auf Maxanz begrenzen
              total.shrink(top);
              Break;
            EndIf
          EndIf
          SleepMS(2);
        EndFor
      EndFor
      For (i := 1;i<=damager.size();i += 1)
        If (damager[i])
          damager[i] := SystemFindObjectBySerial(damager[i]);
        Else
          damager.shrink(i-1);
          Break;
        EndIf
      EndFor
    EndIf
  EndIf
  return(damager);
EndFunction // }}}


/////////////////////
// Initialisierung //
/////////////////////

////////////////////////////////////////////////////////////////////////////////////////////
// RestoreModPergon - Stellt diverse Modifikatoren wieder her (z.B. nach'm Server-Restart)
////////////////////////////////////////////////////////////////////////////////////////////
// Erstmal noch nicht benutzen, so lange die Mods noch mal so und so behandelt werden
Function RestoreModPergon(mobile) // {{{
  var dex_mod := GetObjProperty(
    mobile, ATTRIBUTEID_DEXTERITY+ATTRIBUTEID_MOD
  );
  If (dex_mod)
    SetDexModPergon(mobile, dex_mod);
  EndIf

  var int_mod := GetObjProperty(
    mobile, ATTRIBUTEID_INTELLIGENCE+ATTRIBUTEID_MOD
  );
  If (int_mod)
    SetDexModPergon(mobile, int_mod);
  EndIf

  var str_mod := GetObjProperty(
    mobile, ATTRIBUTEID_STRENGTH+ATTRIBUTEID_MOD
  );
  If (str_mod)
    SetDexModPergon(mobile, str_mod);
  EndIf
EndFunction // }}}


///////////////
// Sonstiges //
///////////////

// belebt Ziel nach Rueckfrage wieder
Function ResNow(who) // {{{
  var layout := {
    "page 0",
    "nodispose",
    "noclose",
    "gumppic 300 230 2070",
    "button 330 305 2073 2072 1 0 0",
    "button 400 305 2076 2075 1 0 1",
    "text 332 264 40 0"
  };

  var data := {"Jetzt resurrecten?"};

  // sicherstellen das während der Gumpauswahl der Betroffene
  // nicht die Position wechselt
  var x := who.x;
  var y := who.y;

  var ret := SendDialogGump(who, layout, data);
  If (!ret)
    return 0;
  EndIf

  If ((x == who.x) and (y == who.y))
    return (ret[0]);
  Else
    SendSysMessagePergon(who,
      "Ihr haettet euch nicht bewegen sollen, "+
      "der Zauber verliert seine Wirkung.",
      "You should not have moved. The magic drains its effect."
    );
  EndIf
EndFunction // }}}

// Wiederbeleben und Roben markieren, damit man die Kistenzumueller erwischt
Function ResurrectPergon(who) // {{{
  Resurrect(who);
  SendSQLManager(who, SQLStatus_UpdateRegion);
  var gown := GetEquipmentByLayer(who, LAYER_ROBE);
  If (gown.objtype == 0x1f03)
    // ist sehr wahrscheinlich eine Totenrobe
    gown.setprop("owner", who.serial);
  EndIf
EndFunction // }}}

// Stat-/Vitalabzuege beim Wiederbeleben
Function ResPenalties(who) // {{{
  return;
  MurdererStatLoss(who);
  // Compilerwarnung unterdruecken
  who := who;
EndFunction // }}}

// Hilfsfunktion fuer ResPenalties
Function MurdererStatLoss(who) // {{{
  var shortc := GetObjProperty(who, "shortmurders");
  If (!shortc)
    return;
  EndIf

  If (shortc <=4 )
    return;
  EndIf

  // is murderer
  var multiplier := Min(STATLOSS_MAX,CInt(shortc)/100.0);
  multiplier := 1.0-multiplier;

  SetStrPergon(who, CInt(GetStrPergon(who)*multiplier));
  SetDexPergon(who, CInt(GetDexPergon(who)*multiplier));
  SetIntPergon(who, CInt(GetIntPergon(who)*multiplier));

  SetHPPergon(who,      GetMaxHPPergon(who));
  SetStaminaPergon(who, GetMaxStaminaPergon(who));
  SetManaPergon(who,    GetMaxManaPergon(who));

  SendSysMessagePergon(who,
    "Euer Koerper zahlt den schmerzhaften Preis fuer Eure Suenden.",
    "Thy body pays the price for your sins."
  );
EndFunction // }}}

// vim: sw=2 sts=2
