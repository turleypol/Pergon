// $Log: not supported by cvs2svn $
// Revision 1.25  2008/10/22 13:01:59  turley
// Nicht mehr benötigte Funktion entfernt
//
// Revision 1.24  2008/10/13 14:48:56  turley
// Spells werden nicht als String sondern als ID gespeichert
//
// Revision 1.23  2008/08/26 07:39:21  turley
// ungenutzter include entfernt
//
// Revision 1.22  2008/08/14 13:32:03  mehdorn
// - Zwischenstand "Ueberarbeitung von tamed"
//   -> wird nicht aktiv benutzt, manuell bei Tieren setzen
//   -> ersetzt, wenn fertig, dann enticedanimal
//

Include ":newspells:magicpergon";
Include "include/client";
Use cfgfile;
Use os;
Use uo;
Use util;

Function CastSpellAI(caster, cast_on, SpellID, mobtype := 0)
  Case (SpellID)
    999:
      If (!RandomInt(3)) // mal mit ner Wahrscheinlichkeit belegen
        SummonSkel(caster, cast_on);
      EndIf

    default:
      If (TypeOfInt(SpellID)==OT_STRING)
        SpellID:=GetSpellIDAI(SpellID);
      EndIf
      If (!SpellID)
        Return;
      EndIf

      If (!PrepareToCast(caster, cast_on, SpellID, mobtype))
        Return;
      EndIf

      Var parms := array;
      parms[1] := "#MOB";
      parms[2] := caster;
      parms[3] := GetCastPowerPergon(caster, SpellID);
      parms[4] := cast_on;

      Var spells := ReadConfigFile("::spells");        // load spells.cfg
      Start_ScriptPergon(":newspells:"+spells[SpellID].Script, parms);
  EndCase
  Return(1);
EndFunction

Function SummonSkel(me, onwho)
  // Sonst fängt es wie wild an zu laggen...
  If (ListMobilesNearLocation(me.x, me.y, me.z, 5, me.realm).size()<10)
    Case (RandomInt(4))
      0: SayPergon(onwho, "Erwachet meine Kinder! Kommt herbei und nehmt euren Platz ein!");
      1: SayPergon(onwho, "Eilt herbei, mein Gefolge!");
      2: SayPergon(onwho, "Ich rufe die Mächte der Finsternis!");
      3: SayPergon(onwho, "Sterblicher, bald wirst du mein Sklave sein!");
    EndCase

    CreateNpcFromTemplate("skeleton", me.x, me.y -2, me.z, 0,me.realm);
    CreateNpcFromTemplate("zombie", onwho.x, onwho.y - 1, onwho.z, 0,me.realm);
    CreateNpcFromTemplate("skeleton", me.x, me.y + 1, me.z, 0,me.realm);
  Else
    SayPergon(onwho, "Bekämpft sie, meine Kinder!");
  EndIf
EndFunction

Function GetSpellIDAI(byref spellname)
  Var spellid := 0;

  Case (spellname)
    "airelemental"   : spellid := SPELLID_MAGE_AIRELEMENTAL;
    "archcure"       : spellid := SPELLID_CLERIC_ARCHCURE;
    "archcurse"      : spellid := SPELLID_NECRO_ARCHCURSE;     //(Magic)
    "archheal"       : spellid := SPELLID_CLERIC_ARCHHEAL;     //(Magic)
    "archpoison"     : spellid := SPELLID_NECRO_ARCHPOISON;    //(Poison)
    "armageddon"     : spellid := SPELLID_NECRO_ARMAGEDDON;
    "bladespirit"    : spellid := SPELLID_NECRO_BLADESPIRIT;
    "blind"          : spellid := SPELLID_NECRO_BLIND;         //(Magic)
    "blizzard"       : spellid := SPELLID_MAGE_BLIZZARD;       //(Elemental - Water)
    "bonebreaker"    : spellid := SPELLID_NECRO_BONEBREAKER;   //(Magic)
    "chainlightning" : spellid := SPELLID_MAGE_CHAINLIGHTNING; //(Elemental - Air)
    "charybdis"      : spellid := SPELLID_MAGE_CHARYBDIS;      //(Elemental - Water)
    "clumsy"         : spellid := SPELLID_COMMON_CLUMSY;       //(Magic)
    "cure"           : spellid := SPELLID_CLERIC_CURE;
    "curse"          : spellid := SPELLID_NECRO_CURSE;         //(Magic)
    "darkness"       : spellid := SPELLID_NECRO_DARKNESS;         //(Magic)
    "dazzle"         : spellid := SPELLID_COMMON_DAZZLE;       //(Magic)
    "dispelfield"    : spellid := SPELLID_CLERIC_DISPELFIELD;
    "earthelemental" : spellid := SPELLID_MAGE_EARTHELEMENTAL;
    "earthquake"     : spellid := SPELLID_MAGE_EARTHQUAKE;     //(Elemental - Earth)
    "energybolt"     : spellid := SPELLID_MAGE_ENERGYBOLT;     //(Elemental - Air)
    "exorcism"       : spellid := SPELLID_CLERIC_EXORCISM;
    "findcorpse"     : spellid := SPELLID_NECRO_FINDCORPSE;    //(Magic)
    "fireaura"       : spellid := SPELLID_MAGE_FIREAURA;       //(Elemental - Fire)
    "fireball"       : spellid := SPELLID_MAGE_FIREBALL;       //(Elemental - Fire)
    "fireelemental"  : spellid := SPELLID_MAGE_FIREELEMENTAL;
    "firefield"      : spellid := SPELLID_MAGE_FIREFIELD;      //(Elemental - Fire)
    "flamestrike"    : spellid := SPELLID_MAGE_FLAMESTRIKE;    //(Elemental - Fire)
    "fog"            : spellid := SPELLID_CLERIC_FOG;          //(Magic)
    "frostshock"     : spellid := SPELLID_MAGE_FROSTSHOCK;     //(Elemental - Water)
    "greaterheal"    : spellid := SPELLID_CLERIC_GREATERHEAL;
    "hallucination"  : spellid := SPELLID_NECRO_HALLUCINATION;
    "heal"           : spellid := SPELLID_CLERIC_HEAL;
    "holymirror"     : spellid := SPELLID_CLERIC_HOLYMIRROR;
    "holyspirit"     : spellid := SPELLID_CLERIC_HOLYSPIRIT;   //(Magic)
    "holystrike"     : spellid := SPELLID_CLERIC_HOLYSTRIKE;   //(Magic)
    "iceaura"        : spellid := SPELLID_MAGE_ICEAURA;        //(Elemental - Water)
    "icebolt"        : spellid := SPELLID_MAGE_ICEBOLT;        //(Elemental - Water)
    "lethalbreath"   : spellid := SPELLID_NECRO_LETHALBREATH;  //(Magic)
    "lightning"      : spellid := SPELLID_MAGE_LIGHTNING;      //(Elemental - Air)
    "lightningaura"  : spellid := SPELLID_MAGE_LIGHTNINGAURA;  //(Elemental - Air)
    "lightningfield" : spellid := SPELLID_MAGE_LIGHTNINGFIELD; //(Elemental - Air)
    "magicarrow"     : spellid := SPELLID_COMMON_MAGICARROW;   //(Magic)
    "manadrain"      : spellid := SPELLID_COMMON_MANADRAIN;    //(Magic)
    "manavampire"    : spellid := SPELLID_NECRO_MANAVAMPIRE;   //(Magic)
    "meteor"         : spellid := SPELLID_MAGE_METEOR;         //(Elemental - Earth)
    "meditation"     : spellid := SPELLID_CLERIC_MEDITATION;
    "mindblast"      : spellid := SPELLID_CLERIC_MINDBLAST;    //(Magic)
    "paralyze"       : spellid := SPELLID_CLERIC_PARALYZE;     //(Magic)
    "paralyzefield"  : spellid := SPELLID_CLERIC_PARALYZEFIELD;//(Magic)
    "poison"         : spellid := SPELLID_NECRO_POISON;        //(Elemental - Poison)
    "poisoncloud"    : spellid := SPELLID_NECRO_POISONCLOUD;   //(Elemental - Poison)
    "poisonelemental": spellid := SPELLID_NECRO_POISONELEMENTAL;
    "poisonfield"    : spellid := SPELLID_NECRO_POISONFIELD;   //(Elemental - Poison)
    "push"           : spellid := SPELLID_NECRO_PUSH;          //(Magic)
    "reflection"     : spellid := SPELLID_COMMON_REFLECT;
    "reveal"         : spellid := SPELLID_COMMON_REVEAL;       //(Magic)
    "sacrifice"      : spellid := SPELLID_CLERIC_SACRIFICE;    //(Magic)
    "slow"           : spellid := SPELLID_COMMON_SLOW;         //(Magic)
    "statue"         : spellid := SPELLID_NECRO_STATUE;        //(Magic)
    "stoneaura"      : spellid := SPELLID_MAGE_STONEAURA;      //(Elemental - Earth)
    "stonehail"      : spellid := SPELLID_MAGE_STONEHAIL;      //(Elemental - Earth)
    "stonesplinter"  : spellid := SPELLID_MAGE_STONESPLINTER;  //(Elemental -Earth)
    "summondaemon"   : spellid := SPELLID_NECRO_SUMMONDAEMON;
    "summonangel"    : spellid := SPELLID_CLERIC_SUMMONANGEL;
    "teleport"       : spellid := SPELLID_COMMON_TELEPORT;
    "vulcano"        : spellid := SPELLID_MAGE_VULCANO;        //(Elemental - Fire)
    "wall"           : spellid := SPELLID_COMMON_WALL;         //(Magic)
    "waterelemental" : spellid := SPELLID_MAGE_WATERELEMENTAL;
    "weaken"         : spellid := SPELLID_COMMON_WEAKEN;

    "summonskel"     : spellid := 999;
  EndCase

  Return(spellid);
EndFunction

Function PrepareToCast(caster, cast_on, spellid, mobtype := 0)
  If (CheckLineOfSight(caster, cast_on))
    If (saywords)
      If (spellid < 200)
        PowerWordsPergon(caster,spellid,SKILLID_MAGIE);
      ElseIf (spellid < 300)
        PowerWordsPergon(caster,spellid,SKILLID_SEGNEN);
      ElseIf (spellid < 400)
        PowerWordsPergon(caster,spellid,SKILLID_FLUCHEN);
      Else
        If (GetSkillPergon(caster, SKILLID_MAGIE))
          PowerWordsPergon(caster,spellid,SKILLID_MAGIE);
        ElseIf (GetSkillPergon(caster, SKILLID_SEGNEN))
          PowerWordsPergon(caster,spellid,SKILLID_SEGNEN);
        Else
          PowerWordsPergon(caster,spellid,SKILLID_FLUCHEN);
        EndIf
      EndIf
      Case (mobtype)
        1:  PerformAction(caster, ANIM_CAST_DIR);     // MOB_HUMAN
        3:  PerformAction(caster, ANIM_FIDGET_1);     // MOB_ANIMAL
        4:  PerformAction(caster, ANIM_WALK_WARMODE); // MOB_MONSTER
        0:  // MOB_UNKNOWN
        2:  // MOB_SEA
      EndCase
      Sleep(1);
    EndIf

    If (spellid < 200)
      If (!CheckMagicSkillPergon(caster, spellid, SKILLID_MAGIE))
        PlaySoundEffect(caster, 0x005d);
        PlayObjectCenteredEffect(caster, 0x3735, 0x0a, 0x1e);
        Return(0);
      EndIf
    ElseIf (spellid < 300)
      If (!CheckMagicSkillPergon(caster, spellid, SKILLID_SEGNEN))
        PlaySoundEffect(caster, 0x005d);
        PlayObjectCenteredEffect(caster, 0x3735, 0x0a, 0x1e);
        Return(0);
      EndIf
    ElseIf (spellid < 400)
      If (!CheckMagicSkillPergon(caster, spellid, SKILLID_FLUCHEN))
        PlaySoundEffect(caster, 0x005d);
        PlayObjectCenteredEffect(caster, 0x3735, 0x0a, 0x1e);
        Return(0);
      EndIf
    Else
      If (GetSkillPergon(caster, SKILLID_MAGIE))
        If (!CheckMagicSkillPergon(caster, spellid, SKILLID_MAGIE))
          PlaySoundEffect(caster, 0x005d);
          PlayObjectCenteredEffect(caster, 0x3735, 0x0a, 0x1e);
          Return(0);
        EndIf
      ElseIf (GetSkillPergon(caster, SKILLID_SEGNEN))
        If (!CheckMagicSkillPergon(caster, spellid, SKILLID_SEGNEN))
          PlaySoundEffect(caster, 0x005d);
          PlayObjectCenteredEffect(caster, 0x3735, 0x0a, 0x1e);
          Return(0);
        EndIf
      Else
        If (!CheckMagicSkillPergon(caster, spellid, SKILLID_FLUCHEN))
          PlaySoundEffect(caster, 0x005d);
          PlayObjectCenteredEffect(caster, 0x3735, 0x0a, 0x1e);
          Return(0);
        EndIf
      EndIf
    EndIf

    If (!ConsumeManaPergon(caster, spellid))
      Return(0);
    EndIf

    Return(1);
  EndIf
EndFunction