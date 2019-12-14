///////////////////////////////////////////////////////////////////
// musicbard
//
// Bardenlieder spielen und Notenkonvertierung fuer Musikrollen

use cfgfile;
use os;
use uo;
use util;
include ":newspells:magicpergon";
include ":newspells:resistance";
include "include/client";
include "include/itemnpc";
include "include/modifyskill";
include "include/msgs";
include "include/objtype";
include "include/properties";
include "include/server";

// Konstanten {{{
// Noten {{{
Const NOTE_C1  :=  1;
Const NOTE_C1S :=  2;
Const NOTE_D1  :=  3;
Const NOTE_D1S :=  4;
Const NOTE_E1  :=  5;
Const NOTE_F1  :=  6;
Const NOTE_F1S :=  7;
Const NOTE_G1  :=  8;
Const NOTE_G1S :=  9;
Const NOTE_A1  := 10;
Const NOTE_A1S := 11;
Const NOTE_H1  := 12;
Const NOTE_C2  := 13;
Const NOTE_C2S := 14;
Const NOTE_D2  := 15;
Const NOTE_D2S := 16;
Const NOTE_E2  := 17;
Const NOTE_F2  := 18;
Const NOTE_F2S := 19;
Const NOTE_G2  := 20;
Const NOTE_G2S := 21;
Const NOTE_A2  := 22;
Const NOTE_A2S := 23;
Const NOTE_H2  := 24;
Const NOTE_C3  := 25;
Const NOTE_P   := 26;
// }}}

// Pausen {{{
Const LEN_FULL     := 1000*2;
Const LEN_HALF     := 500*2;
Const LEN_3QUARTER := 375*2;
Const LEN_QUARTER  := 250*2;
Const LEN_3EIGHT   := 187*2;
Const LEN_EIGHT    := 125*2;
Const LEN_3SIXTEEN := 3*125;
Const LEN_SIXTEEN  := 125;
// }}}

// Tempi, Instrumente {{{
Const SPEED_SLOW := 2;
Const SPEED_MED  := 1;
Const SPEED_FAST := 1/2;

Const LUTE       := 1;
Const HARP       := 2;
Const STAND_HARP := 3;
Const FLUT       := 4;
// }}}

// diverse relevante Items {{{
Const ITEM_CANDELABRA1  := 0x0a26;
Const ITEM_CANDELABRA2  := 0x0a27;
Const ITEM_CANDELABRA3  := 0x0a28;
Const ITEM_CANDELABRA4  := 0x0b1d;
Const ITEM_CANDELABRA5  := 0x0b1e;
Const ITEM_CANDELABRA6  := 0x0b1f;
Const ITEM_CANDLE1      := 0x0a0f;
Const ITEM_CANDLE2      := 0x0a10;
Const ITEM_CANDLE3      := 0x0a11;
Const ITEM_CANDLE4      := 0x0b1a;
Const ITEM_CANDLE5      := 0x0b1b;
Const ITEM_CANDLE6      := 0x0b1c;
Const ITEM_LANTERN1     := 0x0a22;
Const ITEM_LANTERN2     := 0x0a23;
Const ITEM_LANTERN3     := 0x0a24;
Const ITEM_TORCH1       := 0x0a12;
Const ITEM_TORCH2       := 0x0a13;
Const ITEM_TORCH3       := 0x0a14;
Const ITEM_WALLLANTERN1 := 0x0a20;
Const ITEM_WALLLANTERN2 := 0x0a22;
Const ITEM_WALLLANTERN3 := 0x0a24;
Const ITEM_WALLTORCH1   := 0x0a0c;
Const ITEM_WALLTORCH2   := 0x0a0d;
Const ITEM_WALLTORCH3   := 0x0a0e;
Const ITEM_WALLTORCH4   := 0x0a07;
Const ITEM_WALLTORCH5   := 0x0a08;
Const ITEM_WALLTORCH6   := 0x0a09;

Const ITEM_NARRENKAPPE := 0x171c;
// }}}

// Sonstiges {{{
Const CANDLE_NOK := 1;
Const CANDLE_OK  := 0;

Const PLAY_SUCC := 1;
Const PLAY_FAIL := 0;

Const ABILITYMUL := 1.9;

Const TYPE_NPC := 0;
Const TYPE_PC  := 1;

// Teiler fuer Reg-, Stats- und Res-Gewinne
Const REGDIV   := 10;
Const STATSDIV := 10;
Const RESDIV   := 10;

// Teiler fuer Abstand, in dem Barde noch gehört werden kann
Const DISTDIV  := 10.0;

// alle REGDELAY Sekunden wird regeneriert
Const REGDELAY  := 5;
// alle STATDELAY Sekunden werden die stats ueberprueft
Const STATDELAY := 10;

// die Anzahl der Bardenlieder, wird im musicbook.src gebraucht
Const MAXBARDSONG := 13;

// in Klamotten, die das Zaubern von 5. Zirkeln verhindern,
// kann man auch nicht ordentlich musizieren
Const BLOCKBARDING := 5;

Const EFFECT_STEALTH := 1; // special effect - Schleichen
Const EFFECT_RECALL  := 2; // special effect - Recall

Const RES_FIRE   := "fire";
Const RES_WATER  := "water";
Const RES_EARTH  := "earth";
Const RES_AIR    := "air";
Const RES_POISON := "poison";

Const LIED_ABENTEUER := 9;
// }}}
// }}}

// Quasi-Konstanten
Function GetPosWood() // {{{
  return array{
    ITEM_WALNUSSHOLZ, ITEM_ZYPRESSENHOLZ, ITEM_TROPENHOLZ, ITEM_APFELHOLZ,
    ITEM_BIRNENHOLZ,  ITEM_PFIRSICHHOLZ,  ITEM_ZEDERNHOLZ, ITEM_TANNENHOLZ,
    ITEM_FICHTENHOLZ, ITEM_BUCHENHOLZ,    ITEM_ELFENBEINHOLZ
  };
EndFunction
// Mischholz ist neutral
Function GetNegWood()
  return {
    ITEM_MAHAGONIHOLZ, ITEM_WEIDENHOLZ, ITEM_EICHENHOLZ, ITEM_EDELHOLZ,
    ITEM_SCHRATHOLZ,   ITEM_ERLENHOLZ,  ITEM_EBENHOLZ
  };
EndFunction // }}}

Function GetDanceSteps() // {{{
  return array{
    ANIM_ATTACK_1HAND_DOWN, ANIM_ATTACK_1HAND_JAB, ANIM_ATTACK_1HAND_WIDE,
    ANIM_ATTACK_2HAND_DOWN, ANIM_ATTACK_2HAND_JAB, ANIM_ATTACK_2HAND_WIDE,
    ANIM_ATTACK_BOW,        ANIM_ATTACK_XBOW,
    ANIM_BOW,               ANIM_SALUTE,
    ANIM_CAST_AREA,         ANIM_CAST_DIR,
    // ANIM_EAT,
    // ANIM_FIDGET_1,          ANIM_FIDGET_2,
    ANIM_FIST_ATTACK,       ANIM_HIT,
    // ANIM_RUN_ARMED,         ANIM_RUN_UNARMED,
    ANIM_STAND,             ANIM_STAND_1HAND,      ANIM_STAND_2HAND,
    // ANIM_WALK_ARMED,        ANIM_WALK_UNARMED,     ANIM_WALK_WARMODE,
    ANIM_WAR_TURN
  };
EndFunction // }}}

Function GetTones() // {{{
  return array{
    array{ //Laute von C1 bis C3
      0x405, 0x408, 0x40a, 0x40d, 0x40f, 0x411, 0x413, 0x415, 0x417,
      0x3fe, 0x400, 0x402, 0x406, 0x409, 0x40b, 0x40e, 0x410, 0x412,
      0x414, 0x416, 0x418, 0x3ff, 0x401, 0x403, 0x407
    }, array{ //Harfe von C1 bis C3
      0x3d1, 0x3d4, 0x3d6, 0x3d8, 0x3da, 0x3dc, 0x3de, 0x3e0, 0x3e2,
      0x3cb, 0x3cd, 0x3cf, 0x3d2, 0x3d5, 0x3d7, 0x3d9, 0x3db, 0x3dd,
      0x3df, 0x3e1, 0x3e3, 0x3cc, 0x3ce, 0x3d0, 0x3d3
    }, array{ //Standharfe von C1 bis C3
      0x49e, 0x4a1, 0x4a3, 0x4a5, 0x4a7, 0x4a9, 0x4ab, 0x4ad, 0x4af,
      0x498, 0x49a, 0x49c, 0x49f, 0x4a2, 0x4a4, 0x4a6, 0x4a8, 0x4aa,
      0x4ac, 0x4ae, 0x4b0, 0x499, 0x49b, 0x49d, 0x4a0
    }, array{ //Flöte von C1 bis C3
      0x5d7, 0x5da, 0x5dc, 0x5de, 0x5e0, 0x5e2, 0x5e4, 0x5e6, 0x5e8,
      0x5d1, 0x5d3, 0x5d5, 0x5d8, 0x5db, 0x5dd, 0x5df, 0x5e1, 0x5e3,
      0x5e5, 0x5e7, 0x5e9, 0x5d2, 0x5d4, 0x5d6, 0x5d9
    }
  };
EndFunction // }}}

// Resistenzen veraendern und entsprechend Status aktualisieren
Function ModifyResistance(character, resist, value) // {{{
    var resistance := "resist_"+Lower(resist);

    Set_Critical(1);
    SetObjProperty(character, resistance,
        CInt(GetObjProperty(character, resistance)) + value
    );
    Set_Critical(0);

    // Statusfenster aktualisieren
    SendStatus(character);
EndFunction // }}}

Function PlayBardSong(bard, song)
  If (GetObjProperty(bard, PROP_MUSIC_BARDING))
    // der spielt doch schon
    SendSysMessagePergon(bard,
      "Ihr spielt doch schon ein Lied!",
      "You are already playing a song!",
      _DEFAULT_TEXT_FONT, MSG_COLOR_ERROR
    );
    return;
  EndIf

  var skill := GetSkillPergon(bard, SKILLID_MUSIZIEREN);

  If (song.difficulty > (skill + 20)) // Lied zu kompliziert?
    SendSysMessagePergon(bard,
      "Unglaeubig starrt Ihr auf die komplizierten Noten. "+
      "Ihr wagt nicht sie zu spielen.",
      "You stare at the sheet. This song looks very complicated. "+
      "You do not dare to play it.",
      _DEFAULT_TEXT_FONT, MSG_COLOR_ERROR
    );
    return;
  EndIf

  var inst := ChooseInstrument(bard);
  If (!inst)
    // falls kein gültiges Instrument ausgewählt wurde
    return;
  EndIf
    
  var songinfo := array{song.liednr, inst.item.serial};
    
  Detach();
  bard := SystemFindObjectBySerial(bard.serial, SYSFIND_SEARCH_OFFLINE_MOBILES);
  
  var rawpoints := CheckSkillPergon(
                     bard, SKILLID_MUSIZIEREN, song.difficulty, 3*song.difficulty,
                     RAWPOINTS, DONTADVANCESTATS, DONTGIVESKILL
                   );

  If (!rawpoints[1])
    // tja, verspielt ...
    SendSysMessagePergon(bard,
      "Ihr bemueht Euch "+song.name+" zu spielen.",
      "You try to play "+song.name+".",
      _DEFAULT_TEXT_FONT, MSG_COLOR_FAIL
    );

    // vom Script spielen lassen, damit es parallel laeuft
    Start_ScriptPergon("playnotes", {bard, song.noten, inst, PLAY_FAIL, GetPID()});

    // der Barde spielt das Lied mit <liednr>, diese Property wird
    // vom "unbard"-Script gelöscht
    SetObjProperty(bard, PROP_MUSIC_BARDING, songinfo);

    While (
      (GetObjProperty(bard, PROP_MUSIC_BARDING)) and
      (bard.connected)
    )
      // waehrend das Lied spielt, alle REGDELAY Sekunden
      // (geringerer) Stamina-Verlust
      BardLooseStamina(bard, song.difficulty, skill+20);
      Sleep(REGDELAY);
      ShouldEndSong();
    EndWhile
    return;
  EndIf

  If (!CheckImpressivePlay(bard))
    // Bard hat etwas in der Hand oder ist abgelenkt (Warmode):
    // spielen, aber ohne Wirkung

    // vom Script spielen lassen, damit es parallel laeuft
    Start_ScriptPergon("playnotes", {bard, song.noten, inst, PLAY_FAIL, GetPID()});

    // der Barde spielt das Lied mit <liednr>, diese Property wird
    // vom "unbard"-Script gelöscht
    SetObjProperty(bard, PROP_MUSIC_BARDING, songinfo);

    While (
      (GetObjProperty(bard, PROP_MUSIC_BARDING)) and
      (bard.connected)
    )
      // waehrend das Lied spielt, alle REGDELAY Sekunden
      // (geringerer) Stamina-Verlust
      BardLooseStamina(bard, song.difficulty, skill+20);
      Sleep(REGDELAY);
      ShouldEndSong();
    EndWhile
    return;
  EndIf

  SendSysMessagePergon(bard,
    "Ihr spielt ausdrucksstark "+song.name+".",
    "You play "+song.name+" expressive.",
    _DEFAULT_TEXT_FONT, MSG_COLOR_SUCC
  );

  // vom Script spielen lassen, damit es parallel laeuft
  Start_ScriptPergon("playnotes", {bard, song.noten, inst, PLAY_SUCC, GetPID(), rawpoints});

  // der Barde spielt das Lied mit <liednr>, diese Property wird
  // vom "unbard"-Script gelöscht
  SetObjProperty(bard, PROP_MUSIC_BARDING, songinfo);

  // waehrend das Lied spielt, alle REGDELAY Sekunden
  // (geringerer) Stamina-Verlust + Wirkungsentfaltung
  // gemäß der Liedeigenschaften
  While ((GetObjProperty(bard, PROP_MUSIC_BARDING)) and (bard.connected))
    ShouldEndSong();
    BardLooseStamina(bard, song.difficulty, skill);
    If (!CheckImpressivePlay(bard))
      // Barde wurde abgelenkt, sein Spiel hat keine Wirkung mehr;
      // das Lied wird aber komplett gespielt

      // (das -1. lied gibt es nicht, deshalb kann ihm keiner mehr
      // zuhören und es hat auch keine Wirkung)
      SetObjProperty(bard, PROP_MUSIC_BARDING, -1);

    ElseIf (GetObjProperty(bard, PROP_MUSIC_BARDING) != -1)
      // Wirkung entfalten
      EvokeEffects(bard,
        song.liednr, song.effects, song.npceffects, skill, inst.item
      );
    EndIf

    var i;
    For (i := 1; i <= REGDELAY; i += 1)
      Sleep(1);
      DanceAround(bard, skill);
    EndFor
  EndWhile
EndFunction

// laedt alle Parameter des Bardenliedes
Function LoadBardSong(liednr)
  var song := struct;
  song.+liednr := -1;
  song.+difficulty;
  song.+name;
  song.+effects;
  song.+npceffects;
  song.+noten := array{};

  song.npceffects := struct{
    // health nicht setzen, weil man sonst jemanden "totsingen" kann
    "str", "dex", "int", "armor", "mana", "stamina", "health", "special"
    // Resistenzen fuer NPC werden ignoriert, weil das aus der
    // npcdesc.cfg abgefragt wird
  };
  song.effects := struct{
    "str", "dex", "int", "armor", "mana", "stamina", "health", "special",
    "fire", "water", "earth", "air", "poison"
  };

  // Das könnte hingegen schon schiefgehen ...
  var bardenlieder := ReadConfigFile("bardsong");
  If (!bardenlieder)
    return song;
  EndIf

  var lied := FindConfigElem(bardenlieder, liednr);
  song.liednr     := liednr;
  song.difficulty := GetConfigReal(lied, "difficulty");
  song.name       := GetConfigString(lied, "name");

  ForEach key in (song.effects.keys())
    var dummy := SplitWords(GetConfigString(lied, key));
    song.effects[key] := CDbl(dummy[1]);
    If (song.npceffects.exists(key))
      song.npceffects[key] := CDbl(dummy[2]);
    EndIf
  EndForEach

  ForEach note in (
    GetConfigStringArray(lied, "note")
  )
    note := SplitWords(note);
    var notevalue := ConvertNoteNameToNumber(note[1]);
    var notedauer := ConvertNoteLenToNumber(note[2]);
    song.noten.append({notevalue, notedauer});
  EndForEach

  return song;
EndFunction

// Staminaverlust beim Barden
Function BardLooseStamina(bard, difficulty, skill)
  var loss := 5 + (difficulty - skill)/4;
  var stam := GetStaminaPergon(bard);
  SetStaminaPergon(bard, Max(0, stam - loss));
EndFunction

// zuhörende menschenähnliche Character "tanzen" lassen
Function DanceAround(bard, skill) // {{{
  var dancesteps := GetDanceSteps();
  // alle tanzen synchron
  var action := dancesteps[1+RandomInt(dancesteps.size())];
  // zufällig nach links, rechts oder nicht drehen, und nicht zu oft
  var facingmod := RandomInt(3) - 1;
  If (RandomInt(10) > 3)
    facingmod := 0;
  EndIf

  // nun alle tanzen lassen
  ForEach mob in (ListMobilesNearLocation(
    bard.x, bard.y, bard.z, CInt(skill/DISTDIV), bard.realm
  ))
    SleepMS(2);
    If (
      (mob == bard) or mob.warmode or mob.dead or mob.hidden or
      mob.frozen or mob.paralyzed or mob.getprop(PROP_MUSIC_DANCE) == 0
    )
      // Ausnahmen, die nicht mittanzen
      continue;
    EndIf

    If (mob.graphic in array{
      CID_HUMAN_MALE, CID_HUMAN_FEMALE, CID_ELF_MALE, CID_ELF_FEMALE
    })
      var facing := (8 + mob.facing + facingmod) % 8;
      If (mob.facing <> facing)
        mob.facing := facing;
      EndIf
      PerformAction(mob, action);
    EndIf
  EndForEach
EndFunction // }}}

// Wirkung bei allen Zuhörern auslösen
Function EvokeEffects(bard, bardsong, effects, npceffects, skill, item) // {{{
  ForEach mob in (ListMobilesNearLocation(
    bard.x, bard.y, bard.z, CInt(skill/DISTDIV), bard.realm
  ))
    SleepMS(2);
    If (
      // alle NPCs, ...
      mob.isa(POLCLASS_NPC) and
      // ... die wild sind
      !mob.master
    )
      If (skill <= 80)
        // das gibts nur fuer Berufsbarden
        continue;
      EndIf
      If (mob.alignment == ALIGNMENT_GOOD)
        // nur Neutrale und Boese bekaempfen
        continue;
      EndIf

      If (!hasNPCEffect(npceffects))
        // Lied hat keine Effekte fuer NPCs
        continue;
      EndIf

      var bardprop := GetObjProperty(mob, PROP_MUSIC_LISTEN);
      If (!bardprop)
        // wenn er noch keinem Barden lauscht

        // alle Status-Verschlechterungen wirken lassen
        SetEffectsTemp(
            bard, bardsong, mob, npceffects, TYPE_NPC,
            skill, item
        );
        // Mana, Stamina und Health (negativ) regenerieren
        Regenerate(bard, mob, npceffects, TYPE_NPC, skill, item);
        // rotes Sparkle
        PlayObjectCenteredEffect(mob, 0x374a, 5, 15);
      ElseIf (bardprop[bard.serial])
        // wenn er immer noch demselben Barden lauscht

        // Mana, Stamina und Health (negativ) regenerieren
        Regenerate(bard, mob, npceffects, TYPE_NPC, skill, item);
        // rotes Sparkle
        PlayObjectCenteredEffect(mob, 0x374a, 5, 15);
      Else // wenn er einem weiteren Barden lauscht
        SetEffectsTemp(
            bard, bardsong, mob, npceffects, TYPE_NPC,
            skill, item
        );
        // Mana, Stamina und Health regenerieren
        Regenerate(bard, mob, npceffects, TYPE_NPC, skill, item);
        // Wasseradersparkle
        PlayObjectCenteredEffect(mob, 0x3779, 2, 15);
      EndIf
    ElseIf (
      // kein NPC
      !mob.isa(POLCLASS_NPC) or
      // oder zahmer/abgerichteter NPC
      (mob.isa(POLCLASS_NPC) and mob.master)
    )

      var bardprop := GetObjProperty(mob, PROP_MUSIC_LISTEN);
      If (!bardprop) // wenn er noch keinem Barden lauscht
        // alle Status-Verbesserungen wirken lassen
        SetEffectsTemp(
            bard, bardsong, mob, effects, TYPE_PC,
            skill, item
        );
        // Mana, Stamina und Health regenerieren
        Regenerate(bard, mob, effects, TYPE_PC, skill, item);
        // Wasseradersparkle
        PlayObjectCenteredEffect(mob, 0x3779, 2, 15);
      ElseIf (bardprop[bard.serial]) // wenn er immernoch demselben Barden lauscht
        // Mana, Stamina und Health regenerieren
        Regenerate(bard, mob, effects, TYPE_PC, skill, item);
        // Wasseradersparkle
        PlayObjectCenteredEffect(mob, 0x3779, 2, 15);
      Else // wenn er einem weiteren Barden lauscht
        SetEffectsTemp(
            bard, bardsong, mob, effects, TYPE_PC,
            skill, item
        );
        // Mana, Stamina und Health regenerieren
        Regenerate(bard, mob, effects, TYPE_PC, skill, item);
        // Wasseradersparkle
        PlayObjectCenteredEffect(mob, 0x3779, 2, 15);
      EndIf
    EndIf
    ShouldEndSong();
  EndForEach
EndFunction // }}}

// prueft, ob ein Effekt vorgesehen ist
// 0 - kein Effekt; 1 - Effekt
Function hasNPCEffect(eff) // {{{
  ForEach key in (eff.keys())
    SleepMS(2);
    If (eff[key])
      return 1;
    EndIf
  EndForEach
  return 0;
EndFunction // }}}

// regenerierende Wirkung fuer Stamina, HP und Mana
Function Regenerate(bard, who, effects, efftype, skill, item)
  ForEach mem in (array{"health", "mana", "stamina"})
    SleepMS(2);
    var add :=
      CalcEffect(bard, effects[mem], efftype, skill, REGDIV, item);

    var curr, maxv;
    Set_Critical(1);
    Case (mem)
    "health":
      curr := GetHPPergon(who);
      maxv := GetMaxHPPergon(who);
      SetHPPergon(who, CInt(Min(curr+add, maxv)));
    "mana":
      curr := GetManaPergon(who);
      maxv := GetMaxManaPergon(who);
      SetManaPergon(who, CInt(Min(curr+add, maxv)));
    "stamina":
      If (who.serial == bard.serial)
          // da ein barde ja stamina verliert, sollte er
          // sie sich nicht selber wieder setzen koennen
          add := 0;
      EndIf
      curr := GetStaminaPergon(who);
      maxv := GetMaxStaminaPergon(who);
      SetStaminaPergon(who, CInt(Min(curr+add, maxv)));
    EndCase
    Set_Critical(0);
  EndForEach
EndFunction

// Staerke des Liedeffekts abhängig von Instrument berechnen
Function CalcEffect(bard, effectvalue, efftype, skill, div, item)
  var props := GetPropertyItem(item);

  If (!props)
    // NPC-Musikinstrument
    return ApplyFoolsCapBonus(bard, CInt(effectvalue*(skill/div)));
  EndIf

  If (props.hp <= (props.maxhporig/2))
    // Instrument ist verstimmt -> Effekt schwaechen
    var weak := Max(0, (skill/div) - (props.qualityorig/props.quality));
    var result := CInt(effectvalue*weak);
    // syslog("bad " + result);
    return ApplyFoolsCapBonus(bard, result);
  EndIf

  var result := 0;
  // Instrument ist noch halbwegs gestimmt -> Effekt verstaerken
  // manche Holzarten verstärken positiven, manche negativen Effekt
  If (
    (CInt(GetObjProperty(item, "material")) in GetPosWood()) and
    (efftype == TYPE_PC)
  )
    // positiven Effekt staerken
    result := CInt(
        effectvalue * (((skill*props.quality)/(8.0*div)) + (skill/div))
    );
  ElseIf (
    (CInt(GetObjProperty(item, "material")) in GetNegWood()) and
    (efftype == TYPE_NPC)
  )
      // negativen Effekt staerken
    result := CInt(
        effectvalue * (((skill*props.quality)/(8.0*div)) + (skill/div))
    );
  Else
    result := CInt(effectvalue * (skill/div));
  EndIf

  return ApplyFoolsCapBonus(bard, result);
EndFunction

// Narrenkappenbonus berechnen
Function ApplyFoolsCapBonus(bard, result)
  var cap := GetEquipmentByLayer(bard, LAYER_HELMET); 
  If (cap.objtype == ITEM_NARRENKAPPE)
    // ein ordentlicher Barde hat eine Narrenkappe auf
    var qual := GetPropertyItem(cap).quality;
    If (result > 0)
      result += CInt(qual);
    ElseIf (result < 0)
      result -= CInt(qual);
    EndIf
    // result == 0 -> bei einer derart kaputten Harfe hilft auch
    // keine Narrenkappe mehr
  EndIf

  return result;
EndFunction

Function SetEffectsTemp(bard, bardsong, who, effects, efftype, skill, item)
  // Im Prinzip aehnlich wie "DoTempMod" aus statmod.h, aber es werden
  // nicht die Magisch-beeinflusst-Properties gesetzt, daher kann
  // trotzdem zusaetzlich gezaubert werden, aber mehrere Barden wirken
  // nicht kumulativ
  // TODO: ob das gut ist??? da kann zeuchs wie + 40 staerke rauskommen

  var effvalues := struct;

  var bardprop := GetObjProperty(who, PROP_MUSIC_LISTEN);
  If (!bardprop)
    bardprop := dictionary;
  EndIf
  bardprop.insert(bard.serial, effvalues);

  // die jeweiligen Effekte setzen
  If (effects.str)
    effvalues.+str;
    effvalues.str := CalcEffect(bard, effects.str, efftype, skill, STATSDIV, item);
    If (bardprop.size() == 1)
      AlterStrModPergon(who, effvalues.str);
    Else
      var temp := 0;
      ForEach serial in (bardprop.keys())
        If (bardprop[serial].str > temp)
          temp := bardprop[serial].str;
        EndIf
      EndForEach
      If (effvalues.str > temp)
        AlterStrModPergon(who, effvalues.str - temp);
      EndIf
    EndIf
  EndIf
  If (effects.dex)
    effvalues.+dex;
    effvalues.dex := CalcEffect(bard, effects.dex, efftype, skill, STATSDIV, item);
    If (bardprop.size() == 1)
      AlterDexModPergon(who, effvalues.dex);
    Else
      var temp := 0;
      ForEach serial in (bardprop.keys())
        If (bardprop[serial].dex > temp)
          temp := bardprop[serial].dex;
        EndIf
      EndForEach
      If (effvalues.dex > temp)
        AlterDexModPergon(who, effvalues.dex - temp);
      EndIf
    EndIf
  EndIf
  If (effects.int)
    effvalues.+int;
    effvalues.int := CalcEffect(bard, effects.int, efftype, skill, STATSDIV, item);
    If (bardprop.size() == 1)
      AlterIntModPergon(who, effvalues.int);
      SetCastPowerProperty(who, "Musik", effvalues.int, "", "", 0);
      RefreshSpellPowerGump(who);
    Else
      var temp := 0;
      ForEach serial in (bardprop.keys())
        If (bardprop[serial].int > temp)
          temp := bardprop[serial].int;
        EndIf
      EndForEach
      If (effvalues.int > temp)
        AlterIntModPergon(who, effvalues.int - temp);
        SetCastPowerProperty(who, "Musik", effvalues.int - temp, "", "", 0);
        RefreshSpellPowerGump(who);
      EndIf
    EndIf
  EndIf
  If (effects.armor)
    effvalues.+armor;
    effvalues.armor := CalcEffect(bard, effects.armor, efftype, skill, STATSDIV, item);
    If (bardprop.size() == 1)
      who.ar_mod := who.ar_mod + effvalues.armor;
    Else
      var temp := 0;
      ForEach serial in (bardprop.keys())
        If (bardprop[serial].armor > temp)
          temp := bardprop[serial].armor;
        EndIf
      EndForEach
      If (effvalues.armor > temp)
        who.ar_mod := who.ar_mod + effvalues.armor - temp;
      EndIf
    EndIf
  EndIf
  If (effects.fire)
    effvalues.+fire;
    effvalues.fire := CalcEffect(bard, effects.fire, efftype, skill, RESDIV, item);
    If (bardprop.size() == 1)
      ModifyResistance(who, RES_FIRE, effvalues.fire);
      SetObjProperty(who, "music_fire", effvalues.fire);
      SetCastPowerProperty(who, "Musik", CInt(effvalues.fire/2), "", "Fire", 0);
      PrintResistanceText(who, PROP_FIRE, "Feuer");
      RefreshSpellPowerGump(who);
    Else
      var temp := 0;
      ForEach serial in (bardprop.keys())
        If (bardprop[serial].fire > temp)
          temp := bardprop[serial].fire;
        EndIf
      EndForEach
      If (effvalues.fire > temp)
        ModifyResistance(who, RES_FIRE, effvalues.fire - temp);
        SetObjProperty(who, "music_fire", effvalues.fire - temp);
        SetCastPowerProperty(who, "Musik", CInt((effvalues.fire - temp)/2), "", "Fire", 0);
        PrintResistanceText(who, PROP_FIRE, "Feuer");
        RefreshSpellPowerGump(who);
      EndIf
    EndIf
  EndIf
  If (effects.water)
    effvalues.+water;
    effvalues.water := CalcEffect(bard, effects.water, efftype, skill, RESDIV, item);
    If (bardprop.size() == 1)
      ModifyResistance(who, RES_WATER, effvalues.water);
      SetObjProperty(who, "music_water", effvalues.water);
      SetCastPowerProperty(who, "Musik", CInt(effvalues.water/2), "", "Water", 0);
      PrintResistanceText(who, PROP_WATER, "Wasser");
      RefreshSpellPowerGump(who);
    Else
      var temp := 0;
      ForEach serial in (bardprop.keys())
        If (bardprop[serial].water > temp)
          temp := bardprop[serial].water;
        EndIf
      EndForEach
      If (effvalues.water > temp)
        ModifyResistance(who, RES_WATER, effvalues.water - temp);
        SetObjProperty(who, "music_water", effvalues.water - temp);
        SetCastPowerProperty(who, "Musik", CInt((effvalues.water - temp)/2), "", "Water", 0);
        PrintResistanceText(who, PROP_WATER, "Wasser");
        RefreshSpellPowerGump(who);
      EndIf
    EndIf
  EndIf
  If (effects.earth)
    effvalues.+earth;
    effvalues.earth := CalcEffect(bard, effects.earth, efftype, skill, RESDIV, item);
    If (bardprop.size() == 1)
      ModifyResistance(who, RES_EARTH, effvalues.earth);
      SetObjProperty(who, "music_earth", effvalues.earth);
      SetCastPowerProperty(who, "Musik", CInt(effvalues.earth/2), "", "Earth", 0);
      PrintResistanceText(who, PROP_EARTH, "Erde");
      RefreshSpellPowerGump(who);
    Else
      var temp := 0;
      ForEach serial in (bardprop.keys())
        If (bardprop[serial].earth > temp)
          temp := bardprop[serial].earth;
        EndIf
      EndForEach
      If (effvalues.earth > temp)
        ModifyResistance(who, RES_EARTH, effvalues.earth - temp);
        SetObjProperty(who, "music_earth", effvalues.earth - temp);
        SetCastPowerProperty(who, "Musik", CInt((effvalues.earth - temp)/2), "", "Earth", 0);
        PrintResistanceText(who, PROP_EARTH, "Erde");
        RefreshSpellPowerGump(who);
      EndIf
    EndIf
  EndIf
  If (effects.air) //Blitzresistenz
    effvalues.+air;
    effvalues.air := CalcEffect(bard, effects.air, efftype, skill, RESDIV, item);
    If (bardprop.size() == 1)
      ModifyResistance(who, RES_AIR, effvalues.air);
      SetObjProperty(who, "music_air", effvalues.air);
      SetCastPowerProperty(who, "Musik", CInt(effvalues.air/2), "", "Air", 0);
      PrintResistanceText(who, PROP_AIR, "Luft");
      RefreshSpellPowerGump(who);
    Else
      var temp := 0;
      ForEach serial in (bardprop.keys())
        If (bardprop[serial].air > temp)
          temp := bardprop[serial].air;
        EndIf
      EndForEach
      If (effvalues.air > temp)
        ModifyResistance(who, RES_AIR, effvalues.air - temp);
        SetObjProperty(who, "music_air", effvalues.air - temp);
        SetCastPowerProperty(who, "Musik", CInt((effvalues.air - temp)/2), "", "Air", 0);
        PrintResistanceText(who, PROP_AIR, "Luft");
        RefreshSpellPowerGump(who);
      EndIf
    EndIf
  EndIf
  If (effects.poison)
    effvalues.+poison;
    effvalues.poison := CalcEffect(bard, effects.poison, efftype, skill, RESDIV, item);
    If (bardprop.size() == 1)
      ModifyResistance(who, RES_POISON, effvalues.poison);
      SetObjProperty(who, "music_poison", effvalues.poison);
      SetCastPowerProperty(who, "Musik", CInt(effvalues.poison/2), "", "Poison", 0);
      PrintResistanceText(who, PROP_POISON, "Gift");
      RefreshSpellPowerGump(who);
    Else
      var temp := 0;
      ForEach serial in (bardprop.keys())
        If (bardprop[serial].poison > temp)
          temp := bardprop[serial].poison;
        EndIf
      EndForEach
      If (effvalues.poison > temp)
        ModifyResistance(who, RES_POISON, effvalues.poison - temp);
        SetObjProperty(who, "music_poison", effvalues.poison - temp);
        SetCastPowerProperty(who, "Musik", CInt((effvalues.poison - temp)/2), "", "Poison", 0);
        PrintResistanceText(who, PROP_POISON, "Gift");
        RefreshSpellPowerGump(who);
      EndIf
    EndIf
  EndIf
  Case (CInt(effects.special))
  EFFECT_STEALTH: // Pink Panther -> Schleichen
    MakeSpecialEffectStealth(who, bard, skill);
  EFFECT_RECALL: // Abenteuerlust -> Recall
    If ((who == bard) and (skill >= 100))
     SendSysMessagePergon(who,
      "Eure Mitstreiter folgen Euch bis ans Ende der Welt.", "", 
      _DEFAULT_TEXT_FONT, MSG_COLOR_NOTE
     );
    ElseIf (who == bard)
     SendSysMessagePergon(who,
      "Ihr koennt den Nutzen nur erahnen.", "",
      _DEFAULT_TEXT_FONT, MSG_COLOR_NOTE
     );
    EndIf
  EndCase
  
  bardprop[bard.serial] := effvalues;
  SetObjProperty(who, PROP_MUSIC_LISTEN, bardprop);

  // die PROP_MUSIC_LISTEN-Property löschen und Stat-Verbesserungen
  // wieder rueckgaengig machen fuer den jeweils betroffenen Mobile
  var params := struct{
    charserial := who.serial,
    bardserial := bard.serial,
    skill      := skill,
    song       := bardsong,
    effects    := effects,
    values     := effvalues,
    inst       := item
  };
  Start_ScriptPergon("unbard", params);
EndFunction

Function CheckClothing(bard)
  var itemdesc := ReadConfigFile("::itemdesc");
  If (!itemdesc)
    syslog(
      "FEHLER: Konnte ::itemdesc nicht lesen; "+itemdesc.errortext
    );
    return 2;
  EndIf

  ForEach item in ListEquippedItems(bard)
    var block := itemdesc[item.objtype].blockcircle;
    If ((block) && (BLOCKBARDING >= block))
      // traegt etwas Dickeres als eine beschlagene Lederruestung
      return 0;
    EndIf
  EndForEach

  return 1;
EndFunction

Function CheckImpressivePlay(who)
  If (
    GetEquipmentByLayer(who, LAYER_HAND1) or
    GetEquipmentByLayer(who, LAYER_HAND2)
  )
    SendSysMessagePergon(who,
      "Ihr widmet Euch nicht mit beiden Haenden dem Musikinstrument. "+
      "Ihr koennt nicht ausdrucksstark genug spielen.",
      "You need both hand for your instrument. "+
      "Therefore you cannot play expressive.",
      _DEFAULT_TEXT_FONT, MSG_COLOR_FAIL
    );
    return 0;
  ElseIf (who.warmode)
    SendSysMessagePergon(who,
      "Eure Gedanken kreisen nur um den Kampf. "+
      "Ihr koennt nicht ausdrucksstark genug spielen.",
      "Your thoughts are still in the fight. "+
      "Therefore you cannot play expressive.",
      _DEFAULT_TEXT_FONT, MSG_COLOR_FAIL
    );
    return 0;
  ElseIf (!CheckClothing(who))
    SendSysMessagePergon(who,
      "Eure schweren Gewaender sind Euch hinderlich. "+
      "Ihr koennt nicht ausdrucksstark genug spielen.",
      "You thick clothing cumbers you. "+
      "Therefore you cannot play expressive.",
      _DEFAULT_TEXT_FONT, MSG_COLOR_FAIL
    );
    return 0;
  EndIf

  return 1;
EndFunction

// guckt nach Laterne in der Hand von <who>,
// Kerzen koennen wahlweise ignoriert werden
Function CheckLightInHand(who, nocandle)
  var inhand := GetEquipmentByLayer(who, LAYER_HAND2);

  var gLight := array{
    ITEM_CANDLE1,  ITEM_CANDLE2,  ITEM_CANDLE3,
    ITEM_LANTERN1, ITEM_LANTERN2, ITEM_LANTERN3,
    ITEM_TORCH1,   ITEM_TORCH2,   ITEM_TORCH3
  };

  If (inhand in gLight)
    If ((inhand.facing == 2) and (nocandle == CANDLE_OK))
      // kleine Kerze in der Hand ist ok
      return 1;
    EndIf

    // sonstiges Licht in der Hand
    return 0;
  EndIf

  // kein Licht dabei
  return 1;
EndFunction

// guckt nach Lampen - 1 - keine Lampen da; 0 - Lampen in der Nähe
Function CheckLightNear(who)
  // nach Laternen suchen
  ForEach item in (ListItemsNearLocationWithFlag(
    who.x, who.y, who.z, 3, TILEDATA_FLAG_LIGHTSOURCE, who.realm
  ))
    If (item.facing != 2)
      // eine leuchtende Lichtquelle
      // (facing 2 ist nur so eine funzel)
      return 0;
    EndIf
  EndForEach

  // nach Wisps und Leuten mit Laternen in der Hand suchen
  ForEach mob in (
    ListMobilesNearLocation(who.x, who.y, who.z, 3, who.realm)
  )
    If (Lower(mob.npctemplate) == "wisp")
      // aha ein wisp in der naehe!
      return 0;
    ElseIf (!who.isa(POLCLASS_NPC))
      If (!CheckLightInHand(mob, CANDLE_NOK))
        //der player hat doch eine Laterne in der Hand!
        return 0;
      EndIf
    EndIf
  EndForEach

  return 1;
EndFunction

// Leute tarnen/schleichen lassen, wenn dunkel genug
Function MakeSpecialEffectStealth(who, bard, skill) // {{{
  var daytime := GetPergonTimeOfDay();
  If (daytime >= "06:00" and daytime <= "19:59")
    // tagsueber nichts machen, ausser in dauerdunklen Gegenden
    var level := PlaceLightLevel(who);
    If (level == error)
      // kein Dauerlicht
      return;
    EndIf
    If (level < 15)
      // Region ist dauerhell, nicht dauerdunkel
      return;
    EndIf
  EndIf

  If (!CheckLightInHand(who, CANDLE_OK))
    SendSysMessagePergon(who,
      "Ihr wollt Euch verstecken, doch es ist zu hell!",
      "You try to hide, but it is not dark enough."
    );
    return;
  EndIf

  If (!CheckLightNear(who))
    SendSysMessagePergon(who,
        "Ihr wollt Euch verstecken, doch es ist zu hell in der Umgebung!",
        "You try to hide, but it is too bright in your surroundings."
    );
    return;
  EndIf

  SetObjProperty(who, PROP_MUSIC_STEALTH, 1);
  who.hidden := 1;
  var steps := (GetDexPergon(who)/50.0) + (skill/50.0);
  If (who.serial == bard.serial)
    who.stealthsteps := CInt(steps);
  Else
    // die Zuhoerer koennen nicht so weit schleichen
    who.stealthsteps := CInt(steps - Distance(who, bard));
  EndIf
  SendSysMessagePergon(who,
    "Ihr fuehlt Euch geschmeidig wie ein Panther. "+
    "Ungesehen schleicht Ihr umher.",
    "You feel sleek like a panther. You unseenly sneak around.",
    _DEFAULT_TEXT_FONT, MSG_COLOR_NOTE
  );
EndFunction

// herausfinden, ob es eine Dunkelzone ist
// 0 - GreenAcres, 20 - Nirdana
Function PlaceLightLevel(where) // {{{
  var regionfile := ReadConfigFile("::../regions/light");
  var regionname;
  If ((where.isa(POLCLASS_MOBILE)) && (!where.isa(POLCLASS_NPC)))
    regionname := GetRegionName(where);
  Else
    regionname := GetRegionNameAtLocation(where.x, where.y, where.realm);
  EndIf
  var region := regionfile[regionname];
  If (region == error)
    return region;
  EndIf

  var lightlevel   := CInt(region.lightlevel);
  var daylightzone := CInt(region.daylightzone);
  If (daylightzone)
    // TODO: da kann man die Helligkeit nur ueber die Tageszeit schaetzen
    return error{errortext := "Is a daylight region"};
  EndIf

  return lightlevel;
EndFunction // }}}

// abhaengig von Staerke der Resistenz passende Text an Player senden
Function PrintResistanceText(who, resistance, resname) // {{{
    var res := GetResistance(who, resistance);
    var text_de := "";
    var text_en := "";

    If (res<0.2)
        text_de := "Ihr seid nun ein wenig widerstandsfaehiger gegenueber "+
            resname+".";
        text_en := "You can now resist "+resname+" a little.";
    ElseIf (res<0.4)
        text_de := "Ihr seid nun widerstandsfaehiger gegenueber "+resname+".";
        text_en := "You can now resist some amount of "+resname+".";
    ElseIf (res<0.6)
        text_de := "Ihr seid nun viel widerstandsfaehiger gegenueber "+
            resname+".";
        text_en := "You can now resist some bigger amount of "+resname+".";
    ElseIf (res<0.8)
        text_de := "Ihr seid nun sehr viel widerstandsfaehiger gegenueber "+
            resname+".";
        text_en := "You can now resist some really big amount of "+
            resname+".";
    Else
        text_de := "Ihr fuerchtet "+resname+" nicht mehr!";
        text_en := "You don't fear "+resname+" any longer!";
    EndIf

    SendSysMessagePergon(
        who, text_de, text_en, _DEFAULT_TEXT_FONT, MSG_COLOR_NOTE
    );
EndFunction // }}}

// (eigene) Melodie abspielen
Function PlayMelody(bard, musicname, noten) // {{{
  If (!musicname)
    musicname := "unbekannt";
  EndIf

  If (GetObjProperty(bard, PROP_MUSIC_BARDING))
    SendSysMessagePergon(bard,
        "Ihr spielt doch schon ein Lied!",
        "You are already playing a song!",
        _DEFAULT_TEXT_FONT, MSG_COLOR_ERROR
    );
    return;
  EndIf

  var inst := ChooseInstrument(bard);
  If (!inst)
    // kein gültiges Instrument ausgewählt
    return;
  EndIf

  SetObjProperty(bard, PROP_MUSIC_BARDING, -1);
  If (CheckSkillPergon(
      bard, SKILLID_MUSIZIEREN, 20, GetSkillDefaultPoints(SKILLID_MUSIZIEREN)
  ))
    SendSysMessagePergon(bard,
      "Ihr spielt gedankenversunken "+musicname+".",
      "You musingly play "+musicname+".",
      _DEFAULT_TEXT_FONT, MSG_COLOR_SUCC
    );
    PlaySucc(bard, noten, inst);
  Else
    SendSysMessagePergon(bard,
      "Ihr versucht "+musicname+" zu spielen.",
      "You try to play "+musicname+".",
      _DEFAULT_TEXT_FONT, MSG_COLOR_FAIL
    );
    PlayFail(bard, noten, inst);
  EndIf
  EraseObjProperty(bard, PROP_MUSIC_BARDING);
EndFunction // }}}

// darf man das Instrument spielen? (Qualitaet gegen Skill aufwiegen)
Function CheckAbilityForInstrument(bard, inst) // {{{
  var skill  := GetSkillPergon(bard, SKILLID_MUSIZIEREN);

  If ((skill <= 80) and (inst.nr == FLUT))
     SendSysMessagePergon(bard,
       "Eine Floete? Wie spielt man darauf?"+
       "Ihr findet noch keine Antwort auf diese Frage.",
       "A Flute? How do you play on it?"+
       "You can't find an answer.",
       _DEFAULT_TEXT_FONT, MSG_COLOR_FAIL
     );
     return 0;
  EndIf

  var props := GetPropertyItem(inst.item);
  // syslog(props);
  If (!props)
    return inst;
  EndIf

  If ((props.qualityorig*10*ABILITYMUL) < (skill + 40))
    return inst;
  EndIf

  If ((props.qualityorig*10*ABILITYMUL) < (skill + 45))
    SendSysMessagePergon(bard,
      "Voller Ehrfurcht betrachtet Ihr das herrliche Instrument. "+
      "Ihr zupft vorsichtig ein wenig an den Saiten.",
      "You look at the awesome instrument. "+
      "Cautiously, you twitch its strings.",
      _DEFAULT_TEXT_FONT, MSG_COLOR_FAIL
    );
    PlayFail(bard, {
        {NOTE_D1, LEN_FULL}, {NOTE_H1, LEN_FULL}, {NOTE_D2, LEN_FULL},
        {NOTE_D1, LEN_FULL}, {NOTE_H1, LEN_FULL}, {NOTE_D2, LEN_FULL}
      }, inst
    );
    return 0;
  EndIf

  SendSysMessagePergon(bard,
    "Voller Ehrfurcht betrachtet Ihr das herrliche Instrument. "+
    "Ihr wagt nicht darauf zu spielen.",
    "You look at the awesome instrument. "+
    "You do not dare to play it.",
    _DEFAULT_TEXT_FONT, MSG_COLOR_ERROR
  );
  return 0;
EndFunction // }}}

// Instrument waehlen und ueberpruefen
Function ChooseInstrument(bard) // {{{
  var inst := struct;
  inst.+nr;
  inst.+item;

  SendSysMessagePergon(bard, "Musikinstrument waehlen.", "Select instrument");
  var instrument := Target(bard);
  If (!instrument)
    SendSysMessagePergon(bard, "Abbruch", "Abort");
    return 0;
  ElseIf (instrument.objtype == UOBJ_DRUM || instrument.objtype == UOBJ_TAMB)
    SendSysMessagePergon(bard,
      "Das ist ein Rhythmusinstrument!", "That's a rhythm instrument!",
      _DEFAULT_TEXT_FONT, MSG_COLOR_ERROR
    );
    return 0;
  ElseIf (Distance(bard, instrument) >= 2)
    SendSysMessagePergon(bard,
      "So lange Arme habt Ihr nicht!", "Your arms are too short!",
      _DEFAULT_TEXT_FONT, MSG_COLOR_ERROR
    );
    return 0;
  ElseIf (!CheckLineOfSight(bard, instrument))
    SendSysMessagePergon(bard,
      "Ihr koennt das Instrument doch gar nicht sehen!",
      "You cannot see your instrument!",
      _DEFAULT_TEXT_FONT, MSG_COLOR_ERROR
    );
    return 0;
  ElseIf (instrument.objtype == UOBJ_LUTE)
    inst.nr   := LUTE;
    inst.item := instrument;
  ElseIf (instrument.objtype == UOBJ_HARP)
    inst.nr   := HARP;
    inst.item := instrument;
  ElseIf (instrument.objtype == 0x0eb1)
    inst.nr   := STAND_HARP;
    inst.item := instrument;
  ElseIf (instrument.objtype == UOBJ_FLUT)
    inst.nr   := FLUT;
    inst.item := instrument;
  Else
    SendSysMessagePergon(bard,
      "Ihr wisst nicht, wie Ihr darauf spielen sollt.",
      "You don't know how to play on that.",
      _DEFAULT_TEXT_FONT, MSG_COLOR_ERROR
    );
    return 0;
  EndIf

  // nicht jeder kann gleich auf allem spielen
  // (beste "NegWood"-Harfe bei 102, beste "PosWood"-Harfe bei 78)
  return (CheckAbilityForInstrument(bard, inst));
EndFunction // }}}

// gut spielen
Function PlaySucc(bard, noten, inst, rawpoints := 0) // {{{
  If (!noten)
    // ohne zu spielen geht auch nichts kaputt
    return;
  EndIf

  var toene := GetTones();
  
  var i, end;
  For (i := 1; i <= len(noten); i += 1)
    
    end := GetObjProperty(inst.item, "endsong");
    If (end or Distance(inst.item, bard) > 2 or !bard.connected or bard.dead)  
    // Lied abgebrochen, Instrument außer Reichweite, Verbindung weg oder tot
      If (end)
        EraseObjProperty(inst.item, "endsong");
      EndIf
      break;
    EndIf

    If ((noten[i][1] == -1) or (noten[i][2] == -1))
      // Note ist ungueltig
      continue;
    EndIf

    If (noten[i][1] != NOTE_P)
      If (toene[inst.nr][ noten[i][1] ])
        PlaySoundEffect(bard, toene[inst.nr][ noten[i][1] ]);
      EndIf
    EndIf
    // immer abwarten, damit niemand durch Wegrennen
    // schneller skillen kann
    SleepMS(noten[i][2]);
  EndFor

  If (i >= len(noten))
    AwardRawPointsPergon(bard, SKILLID_MUSIZIEREN, rawpoints[2]);
    AdvanceStatsPergon(bard, SKILLID_MUSIZIEREN);
  EndIf

  // Standharfen nicht kaputtmachen, weil im placedeed.src nicht darauf
  // geachtet wird, dass auch maxhp gesetzt wird
  If (inst.nr != STAND_HARP)
    SetWerkzeugSchaden(bard, inst.item,
      "Euer Musikinstrument ist leicht verstimmt!",
      "Euer Musikinstrument ist stark verstimmt!",
      "Euer Musikinstrument ist zerbrochen!"
    );
  EndIf
EndFunction // }}}

// schlecht spielen
Function PlayFail(bard, noten, inst) // {{{
  If (!noten)
    // ohne zu spielen geht auch nichts kaputt
    return;
  EndIf

  var toene := GetTones();

  var i, end;
  For (i := 1; i <= (len(noten))/2; i += 1)
    
    end := GetObjProperty(inst.item, "endsong");
    If (end or Distance(inst.item, bard) > 2 or !bard.connected or bard.dead)  
    // Lied abgebrochen, Instrument außer Reichweite, Verbindung weg oder tot
      If (end)
        EraseObjProperty(inst.item, "endsong");
      EndIf
      break;
    EndIf

    If ((noten[i][1] == -1) or (noten[i][2] == -1))
      // Note ist ungueltig
      continue;
    EndIf

    If (noten[i][1] != NOTE_P)
      var note := toene[inst.nr][ noten[(RandomInt(i))+1][1] ];
      If (note)
        PlaySoundEffect(bard, note);
      EndIf
    EndIf
    // immer abwarten, damit niemand durch Wegrennen
    // schneller skillen kann
    SleepMS(noten[i][2]);
  EndFor

  // Standharfen nicht kaputtmachen, weil im placedeed.src nicht darauf
  // geachtet wird, dass auch maxhp gesetzt wird
  If (inst.nr != STAND_HARP)
    SetWerkzeugSchaden(bard, inst.item,
      "Euer Musikinstrument ist leicht verstimmt!",
      "Euer Musikinstrument ist stark verstimmt!",
      "Euer Musikinstrument ist zerbrochen!"
    );
  EndIf
EndFunction // }}}

// Konvertierungsfunktionen
Function ConvertNoteNumberToString(number) // {{{
  Case (number)
    NOTE_C1:  return "c1";
    NOTE_C1S: return "c#1";
    NOTE_D1:  return "d1";
    NOTE_D1S: return "d#1";
    NOTE_E1:  return "e1";
    NOTE_F1:  return "f1";
    NOTE_F1S: return "f#1";
    NOTE_G1:  return "g1";
    NOTE_G1S: return "g#1";
    NOTE_A1:  return "a1";
    NOTE_A1S: return "a#1";
    NOTE_H1:  return "h1";
    NOTE_C2:  return "c2";
    NOTE_C2S: return "c#2";
    NOTE_D2:  return "d2";
    NOTE_D2S: return "d#2";
    NOTE_E2:  return "e2";
    NOTE_F2:  return "f2";
    NOTE_F2S: return "f#2";
    NOTE_G2:  return "g2";
    NOTE_G2S: return "g#2";
    NOTE_A2:  return "a2";
    NOTE_A2S: return "a#2";
    NOTE_H2:  return "h2";
    NOTE_C3:  return "c3";
    NOTE_P:   return "p";
    default:  return "-leer-";
  EndCase
EndFunction // }}}

Function ConvertNoteNameToNumber(name) // {{{
  Case (name)
    "c1":    return NOTE_C1;
    "c#1":   return NOTE_C1S;
    "d1":    return NOTE_D1;
    "d#1":   return NOTE_D1S;
    "e1":    return NOTE_E1;
    "f1":    return NOTE_F1;
    "f#1":   return NOTE_F1S;
    "g1":    return NOTE_G1;
    "g#1":   return NOTE_G1S;
    "a1":    return NOTE_A1;
    "a#1":   return NOTE_A1S;
    "h1":    return NOTE_H1;
    "c2":    return NOTE_C2;
    "c#2":   return NOTE_C2S;
    "d2":    return NOTE_D2;
    "d#2":   return NOTE_D2S;
    "e2":    return NOTE_E2;
    "f2":    return NOTE_F2;
    "f#2":   return NOTE_F2S;
    "g2":    return NOTE_G2;
    "g#2":   return NOTE_G2S;
    "a2":    return NOTE_A2;
    "a#2":   return NOTE_A2S;
    "h2":    return NOTE_H2;
    "c3":    return NOTE_C3;
    "p":     return NOTE_P;
    default: return -1;
  EndCase
EndFunction // }}}

Function ConvertNoteLenToNumber(dauer) // {{{
  Case (dauer)
    "full":     return LEN_FULL;
    "half":     return LEN_HALF;
    "3quarter": return LEN_3QUARTER;
    "quarter":  return LEN_QUARTER;
    "3eight":   return LEN_3EIGHT;
    "eight":    return LEN_EIGHT;
    "g":        return LEN_FULL;
    "h":        return LEN_HALF;
    "v.":       return LEN_3QUARTER;
    "v":        return LEN_QUARTER;
    "a.":       return LEN_3EIGHT;
    "a":        return LEN_EIGHT;
    "s.":       return LEN_3SIXTEEN;
    "s":        return LEN_SIXTEEN;
    default:    return -1;
  EndCase
EndFunction // }}}

Function ConvertNoteLenToString(number) // {{{
    Case (number)
        LEN_FULL:     return "g";
        LEN_HALF:     return "h";
        LEN_3QUARTER: return "v.";
        LEN_QUARTER:  return "v";
        LEN_3EIGHT:   return "a.";
        LEN_EIGHT:    return "a";
        LEN_3SIXTEEN: return "s.";
        LEN_SIXTEEN:  return "s";
        default:      return "-leer-";
    EndCase
EndFunction // }}}

// Hat playnotes nen Event geschickt das es fertig ist?
// kein kill() über playnotes damit das hier nicht mitten im cprop setzen
// verreckt
Function ShouldEndSong()
  var ev := Wait_For_Event(0);
  If (ev)
    If (ev.type == 0x1234)
      Exit;
    EndIf
  EndIf
EndFunction
