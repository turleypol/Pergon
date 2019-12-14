/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//   NPCCAST für die neuen Magiesprüche
//
//     Author: Hotny
//
//  Änderungen:
//    08.09.06 Fox: eventtimer durch eventwaiter ersetzt
/////////////////////////////////////////////////////////////////////////////////////////////////////////

Include ":newspells:magicpergon";
Include "include/npccast";

Const DEFAULT_DELAY:=5;
Const SUMMON_SKEL_DELAY:=5;


Enum CommonSpells
  SPELL_COMMON_CLUMSY_DELAY     := 3,
  SPELL_COMMON_DAZZLE_DELAY     := 3,
  SPELL_COMMON_MAGICARROW_DELAY := 0,
  SPELL_COMMON_MANADRAIN_DELAY  := 5,
  SPELL_COMMON_REFLECTION_DELAY := 1,
  SPELL_COMMON_REVEAL_DELAY     := 5,
  SPELL_COMMON_SLOW_DELAY       := 5,
  SPELL_COMMON_TELEPORT_DELAY   := 8,
  SPELL_COMMON_WALL_DELAY       := 5,
  SPELL_COMMON_WEAKEN_DELAY     := 3
EndEnum

Enum ClericSpells
  SPELL_CLERIC_ARCHCURE_DELAY      := 5,
  SPELL_CLERIC_ARCHHEAL_DELAY      := 5,
  SPELL_CLERIC_CURE_DELAY          := 1,
  SPELL_CLERIC_DISPELFIELD_DELAY   := 1,
  SPELL_CLERIC_EXORCISM_DELAY      := 5,
  SPELL_CLERIC_FOG_DELAY           := 5,
  SPELL_CLERIC_GREATERHEAL_DELAY   := 2,
  SPELL_CLERIC_HEAL_DELAY          := 2,
  SPELL_CLERIC_HOLYMIRROR_DELAY    := 14,
  SPELL_CLERIC_HOLYSPIRIT_DELAY    := 30,
  SPELL_CLERIC_HOLYSTRIKE_DELAY    := 5,
  SPELL_CLERIC_MEDITATION_DELAY    := 10,
  SPELL_CLERIC_MINDBLAST_DELAY     := 14,
  SPELL_CLERIC_PARALYZEFIELD_DELAY := 8,
  SPELL_CLERIC_PARALYZE_DELAY      := 20,
  SPELL_CLERIC_SACRIFICE_DELAY     := 15,
  SPELL_CLERIC_SUMMONANGEL_DELAY   := 12
EndEnum

Enum MageSpells
  SPELL_MAGE_AIRELEMENTAL_DELAY   := 12,
  SPELL_MAGE_BLIZZARD_DELAY       := 8,
  SPELL_MAGE_CHAINLIGHTNING_DELAY := 20,
  SPELL_MAGE_CHARYBDIS_DELAY      := 20,
  SPELL_MAGE_EARTHELEMENTAL_DELAY := 12,
  SPELL_MAGE_EARTHQUAKE_DELAY     := 20,
  SPELL_MAGE_ENERGYBOLT_DELAY     := 5,
  SPELL_MAGE_FIREAURA_DELAY       := 10,
  SPELL_MAGE_FIREBALL_DELAY       := 5,
  SPELL_MAGE_FIREELEMENTAL_DELAY  := 12,
  SPELL_MAGE_FLAMESTRIKE_DELAY    := 8,
  SPELL_MAGE_FROSTSHOCK_DELAY     := 8,
  SPELL_MAGE_ICEAURA_DELAY        := 10,
  SPELL_MAGE_ICEBOLT_DELAY        := 5,
  SPELL_MAGE_LIGHTNINGAURA_DELAY  := 10,
  SPELL_MAGE_LIGHTNINGFIELD_DELAY := 8,
  SPELL_MAGE_LIGHTNING_DELAY      := 5,
  SPELL_MAGE_METEOR_DELAY         := 12,
  SPELL_MAGE_STONEAURA_DELAY      := 10,
  SPELL_MAGE_STONEHAIL_DELAY      := 15,
  SPELL_MAGE_STONESPLINTER_DELAY  := 10,
  SPELL_MAGE_VULCANO_DELAY        := 25,
  SPELL_MAGE_WATERELEMENTAL_DELAY := 12
EndEnum

Enum NecroSpells
  SPELL_NECRO_ARCHCURSE_DELAY       := 10,
  SPELL_NECRO_ARCHPOISON_DELAY      := 20,
  SPELL_NECRO_ARMAGEDDON_DELAY      := 60,
  SPELL_NECRO_BLADESPIRIT_DELAY     := 10,
  SPELL_NECRO_BLIND_DELAY           := 5,
  SPELL_NECRO_BONEBREAKER_DELAY     := 10,
  SPELL_NECRO_CURSE_DELAY           := 7,
  SPELL_NECRO_DARKNESS_DELAY        := 10,
  SPELL_NECRO_FINDCORPSE_DELAY      := 10,
  SPELL_NECRO_HALLUCINATION_DELAY   := 10,
  SPELL_NECRO_LETHALBREATH_DELAY    := 15,
  SPELL_NECRO_MANAVAMPIRE_DELAY     := 10,
  SPELL_NECRO_POISONCLOUD_DELAY     := 10,
  SPELL_NECRO_POISONELEMENTAL_DELAY := 12,
  SPELL_NECRO_POISONFIELD_DELAY     := 7,
  SPELL_NECRO_POISON_DELAY          := 5,
  SPELL_NECRO_PUSH_DELAY            := 7,
  SPELL_NECRO_STATUE_DELAY          := 15,
  SPELL_NECRO_SUMMONDAEMON_DELAY    := 12
EndEnum


Function MagicFight(byref opponent)
  //testen ob opponent überhaupt angegríffen werden darf
  If ((!opponent)||(opponent.hidden)||(opponent.dead)||(opponent.concealed))
    //der opponent ist weg (warum auch immer)
    //casten reseten
    //params.firstcast:=1;
    TestMessage("MagicFight: Opponent verschwunden", 1);
    Return(0);
  EndIf
  Var ev:=struct; // schonmal zusammenbauen
  ev.+type;
  ev.+source;
  ev.type:=EVID_CAST_WAIT;
  ev.source:=opponent;


  If ((Me.paralyzed) || (Me.frozen))
    // Dann halt nich...
    params[FIRSTCAST]:=0;
    SendEventWaiterGeneral(Me, params[CASTDELAY], ev);
    Return(1);
  EndIf

  //Haben wir schon einen Spiegel?
  If (!Me.getprop("spell_reflect"))
    If (params[SPELLREFLECT])
      TestMessage("MagicFight: Magischer Spiegel war weg", 1);
      castspellAI( Me, Me, SPELLID_COMMON_REFLECT, MobType );
    EndIf
  EndIf

  If (params[FIRSTCAST]) // Damit nicht alle Gleichzeitig Zaubern
    params[FIRSTCAST]:=0;
    SendEventWaiterGeneral(Me, RandomInt(params[CASTDELAY]), ev);
    Return(1);
  EndIf

  //So ab jetzt darf er die im Template eingetragenen Sprüche casten
  //mal checken ob er wirklich castet

  //Mal auf 100 normieren und dann Wahrscheinlichkeiten wuerfen
  If (RandomInt(100)>=((GetManaPergon(Me)*100)/GetMaxManaPergon(Me)))
    TestMessage("MagicFight: Habe zuwenig Mana", 1);
    //Hab zu wenig Mana, da Zaubere ich doch eher noch nicht
    //Dann warte ich das definierte Delay
    SendEventWaiterGeneral(Me, params[CASTDELAY], ev);

    Return(2); //spezial Return für nature MagicAttack
  EndIf

  // LineOfSight interessiert nur für Angriff
  //Dispelfield/Heilen/Gegengift oder normal casten
  Var spell_delay:=params[CASTDELAY];
  Var spell:=0;
  If (params[CASTDISPEL]==2)  // gerade gecastet dann einmal aussetzen
    params[CASTDISPEL]:=0;
  ElseIf (params[CASTDISPEL])
    If (RandomInt(100)<params[SPELLDISPEL]) //Chance gepackt?
      spell:=1;
      spell_delay+=CallCast_GetDelay(SPELLID_CLERIC_DISPELFIELD,Me);
      params[CASTDISPEL]:=2; // gerade gecastet
    Else
      params[CASTDISPEL]:=0; // nicht geschafft also kann gleich wieder
    EndIf
  EndIf
  If (!spell)
    If ((Me.getprop("poison_level")) && (params[SPELLCURE]))
      TestMessage("MagicFight: Bin vergiftet", 1);
      spell_delay+=CallCast_GetDelay(params[SPELLCURE].randomentry(),Me);
      spell:=1;
    ElseIf (params[SPELLHEAL])
      If (RandomInt(GetMaxHPPergon(Me))>GetHPPergon(Me))
        TestMessage("MagicFight: Heil mich", 1);
        spell_delay+=CallCast_GetDelay(params[SPELLHEAL].randomentry(),Me);
        spell:=1;
      EndIf
    EndIf
  EndIf

  If (!spell)
    If (CheckLineOfSight(Me, opponent))
      spell_delay+=CallCast_GetDelay(ChooseSpell(),opponent);
    EndIf
  EndIf

  //EventWaiter für den nächsten Cast setzen
  // 50% fest 50% zufall
  SendEventWaiterGeneral(Me, RandomIntMinMax(spell_delay/2,spell_delay), ev);
  Return(1);
EndFunction

Function CallCast_GetDelay(byref spell,byref opponent)
  TestMessage("MagicFight: Zaubere "+spell, 1);
  Var delay:=0;
  If (castspellAI( Me, opponent, spell,MobType ))
    Case (spell)
      999:                           delay := SUMMON_SKEL_DELAY;
      SPELLID_MAGE_AIRELEMENTAL:     delay := SPELL_MAGE_AIRELEMENTAL_DELAY;
      SPELLID_CLERIC_ARCHCURE:       delay := SPELL_CLERIC_ARCHCURE_DELAY;
      SPELLID_NECRO_ARCHCURSE:       delay := SPELL_NECRO_ARCHCURSE_DELAY;
      SPELLID_CLERIC_ARCHHEAL:       delay := SPELL_CLERIC_ARCHHEAL_DELAY;
      SPELLID_NECRO_ARCHPOISON:      delay := SPELL_NECRO_ARCHPOISON_DELAY;
      SPELLID_NECRO_ARMAGEDDON:      delay := SPELL_NECRO_ARMAGEDDON_DELAY;
      SPELLID_NECRO_BLADESPIRIT:     delay := SPELL_NECRO_BLADESPIRIT_DELAY;
      SPELLID_NECRO_BLIND:           delay := SPELL_NECRO_BLIND_DELAY;
      SPELLID_MAGE_BLIZZARD:         delay := SPELL_MAGE_BLIZZARD_DELAY;
      SPELLID_NECRO_BONEBREAKER:     delay := SPELL_NECRO_BONEBREAKER_DELAY;
      SPELLID_MAGE_CHAINLIGHTNING:   delay := SPELL_MAGE_CHAINLIGHTNING_DELAY;
      SPELLID_MAGE_CHARYBDIS:        delay := SPELL_MAGE_CHARYBDIS_DELAY;
      SPELLID_COMMON_CLUMSY:         delay := SPELL_COMMON_CLUMSY_DELAY;
      SPELLID_CLERIC_CURE:           delay := SPELL_CLERIC_CURE_DELAY;
      SPELLID_NECRO_CURSE:           delay := SPELL_NECRO_CURSE_DELAY;
      SPELLID_NECRO_DARKNESS:        delay := SPELL_NECRO_DARKNESS_DELAY;
      SPELLID_COMMON_DAZZLE:         delay := SPELL_COMMON_DAZZLE_DELAY;
      SPELLID_CLERIC_DISPELFIELD:    delay := SPELL_CLERIC_DISPELFIELD_DELAY;
      SPELLID_MAGE_EARTHELEMENTAL:   delay := SPELL_MAGE_EARTHELEMENTAL_DELAY;
      SPELLID_MAGE_EARTHQUAKE:       delay := SPELL_MAGE_EARTHQUAKE_DELAY;
      SPELLID_MAGE_ENERGYBOLT:       delay := SPELL_MAGE_ENERGYBOLT_DELAY;
      SPELLID_CLERIC_EXORCISM:       delay := SPELL_CLERIC_EXORCISM_DELAY;
      SPELLID_NECRO_FINDCORPSE:      delay := SPELL_NECRO_FINDCORPSE_DELAY;
      SPELLID_MAGE_FIREAURA:         delay := SPELL_MAGE_FIREAURA_DELAY;
      SPELLID_MAGE_FIREBALL:         delay := SPELL_MAGE_FIREBALL_DELAY;
      SPELLID_MAGE_FIREELEMENTAL:    delay := SPELL_MAGE_FIREELEMENTAL_DELAY;
      SPELLID_MAGE_FLAMESTRIKE:      delay := SPELL_MAGE_FLAMESTRIKE_DELAY;
      SPELLID_CLERIC_FOG:            delay := SPELL_CLERIC_FOG_DELAY;
      SPELLID_MAGE_FROSTSHOCK:       delay := SPELL_MAGE_FROSTSHOCK_DELAY;
      SPELLID_CLERIC_GREATERHEAL:    delay := SPELL_CLERIC_GREATERHEAL_DELAY;
      SPELLID_NECRO_HALLUCINATION:   delay := SPELL_NECRO_HALLUCINATION_DELAY;
      SPELLID_CLERIC_HEAL:           delay := SPELL_CLERIC_HEAL_DELAY;
      SPELLID_CLERIC_HOLYMIRROR:     delay := SPELL_CLERIC_HOLYMIRROR_DELAY;
      SPELLID_CLERIC_HOLYSPIRIT:     delay := SPELL_CLERIC_HOLYSPIRIT_DELAY;
      SPELLID_CLERIC_HOLYSTRIKE:     delay := SPELL_CLERIC_HOLYSTRIKE_DELAY;
      SPELLID_MAGE_ICEAURA:          delay := SPELL_MAGE_ICEAURA_DELAY;
      SPELLID_MAGE_ICEBOLT:          delay := SPELL_MAGE_ICEBOLT_DELAY;
      SPELLID_NECRO_LETHALBREATH:    delay := SPELL_NECRO_LETHALBREATH_DELAY;
      SPELLID_MAGE_LIGHTNING:        delay := SPELL_MAGE_LIGHTNING_DELAY;
      SPELLID_MAGE_LIGHTNINGAURA:    delay := SPELL_MAGE_LIGHTNINGAURA_DELAY;
      SPELLID_MAGE_LIGHTNINGFIELD:   delay := SPELL_MAGE_LIGHTNINGFIELD_DELAY;
      SPELLID_COMMON_MAGICARROW:     delay := SPELL_COMMON_MAGICARROW_DELAY;
      SPELLID_COMMON_MANADRAIN:      delay := SPELL_COMMON_MANADRAIN_DELAY;
      SPELLID_NECRO_MANAVAMPIRE:     delay := SPELL_NECRO_MANAVAMPIRE_DELAY;
      SPELLID_MAGE_METEOR:           delay := SPELL_MAGE_METEOR_DELAY;
      SPELLID_CLERIC_MEDITATION:     delay := SPELL_CLERIC_MEDITATION_DELAY;
      SPELLID_CLERIC_MINDBLAST:      delay := SPELL_CLERIC_MINDBLAST_DELAY;
      SPELLID_CLERIC_PARALYZE:       delay := SPELL_CLERIC_PARALYZE_DELAY;
      SPELLID_CLERIC_PARALYZEFIELD:  delay := SPELL_CLERIC_PARALYZEFIELD_DELAY;
      SPELLID_NECRO_POISON:          delay := SPELL_NECRO_POISON_DELAY;
      SPELLID_NECRO_POISONCLOUD:     delay := SPELL_NECRO_POISONCLOUD_DELAY;
      SPELLID_NECRO_POISONELEMENTAL: delay := SPELL_NECRO_POISONELEMENTAL_DELAY;
      SPELLID_NECRO_POISONFIELD:     delay := SPELL_NECRO_POISONFIELD_DELAY;
      SPELLID_NECRO_PUSH:            delay := SPELL_NECRO_PUSH_DELAY;
      SPELLID_COMMON_REFLECT:        delay := SPELL_COMMON_REFLECTION_DELAY;
      SPELLID_COMMON_REVEAL:         delay := SPELL_COMMON_REVEAL_DELAY;
      SPELLID_CLERIC_SACRIFICE:      delay := SPELL_CLERIC_SACRIFICE_DELAY;
      SPELLID_COMMON_SLOW:           delay := SPELL_COMMON_SLOW_DELAY;
      SPELLID_NECRO_STATUE:          delay := SPELL_NECRO_STATUE_DELAY;
      SPELLID_MAGE_STONEAURA:        delay := SPELL_MAGE_STONEAURA_DELAY;
      SPELLID_MAGE_STONEHAIL:        delay := SPELL_MAGE_STONEHAIL_DELAY;
      SPELLID_MAGE_STONESPLINTER:    delay := SPELL_MAGE_STONESPLINTER_DELAY;
      SPELLID_CLERIC_SUMMONANGEL:    delay := SPELL_CLERIC_SUMMONANGEL_DELAY;
      SPELLID_NECRO_SUMMONDAEMON:    delay := SPELL_NECRO_SUMMONDAEMON_DELAY;
      SPELLID_COMMON_TELEPORT:       delay := SPELL_COMMON_TELEPORT_DELAY;
      SPELLID_MAGE_VULCANO:          delay := SPELL_MAGE_VULCANO_DELAY;
      SPELLID_COMMON_WALL:           delay := SPELL_COMMON_WALL_DELAY;
      SPELLID_MAGE_WATERELEMENTAL:   delay := SPELL_MAGE_WATERELEMENTAL_DELAY;
      SPELLID_COMMON_WEAKEN:         delay := SPELL_COMMON_WEAKEN_DELAY;
      default:
        delay:=DEFAULT_DELAY;
    EndCase
  EndIf
  Return(delay);
EndFunction

Function ChooseSpell()
  Var luck:=RandomIntMinMax(1,101); //bestimmt wegen der Wahrscheinlichkeit
  Var reachedluck:=0;
  Var i:=1;
  Var spell:={};
  While ((reachedluck<luck)&&(spell))
    spell:=params[SPELLS][i];
    reachedluck+=spell[2];
    i+=1;
  EndWhile
  If (spell)
    Return(spell[1]);
  Else
    Return(0);
  EndIf
EndFunction