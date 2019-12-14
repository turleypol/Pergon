Enum Abilities
  AB_ARMOR_IGNORE:=1,
  AB_BLEED_ATTACK,
  AB_DEFENSE_MASTERY,
  AB_FEINT,
  AB_WHIRLWIND,  //5
  AB_PARALYZING_BLOW,
  AB_PSYCHIC_ATTACK,
  AB_SHADOWATTACK,
  AB_CONFIDENCE,
  AB_ENEMY_OF_ONE,//10
  AB_SHADOWJUMP,
  AB_KI_ATTACK,
  AB_BLADEWEAVE,
  AB_DISMOUNT,
  AB_BLOCK,   //15
  AB_MORTAL_STRIKE,
  AB_EVASION,
  AB_MOVING_SHOT,
  AB_SURPRISE_ATTACK,
  AB_DOUBLE_STRIKE //20
EndEnum

Enum CProps
  AB_DELAY:="#CombatAbilityDelay",
  AB_CPROP:="CombatAbility",
  AB_ARMOD:="CombatAbilityArMod"
EndEnum

include "include/spellcheck";
include ":housing:house";

///////////////////////
// HitScriptFunktionen:
///////////////////////
Function PraeAusweichenAttacker(attacker,defender,byref basedmg,byref dealdmg)
  var cprop:=attacker.getprop(AB_CPROP);
  Case (cprop[1])
    AB_ARMOR_IGNORE:
      dealdmg:=basedmg;  // statt rawdamage basedamage nehmen
      attacker.eraseprop(AB_CPROP);
      SendSysMessagePergon(attacker,struct{uc_text:=CAscz("Ihr schlagt durch die Rüstung!"),lang:="DEU"},
                                    struct{uc_text:=CAscz("You hit through the armor"),lang:="ENU"});
      If (!defender.isA(POLCLASS_NPC))
        SendSysMessagePergon(defender,struct{uc_text:=CAscz("Der Schlag ging durch eure Rüstung!"),lang:="DEU"},
                                      struct{uc_text:=CAscz("The hit goes though your armor!"),lang:="ENU"});
      EndIf
    AB_DEFENSE_MASTERY:
      dealdmg-=(50-cprop[3]);  // um 20..50 senken
      dealdmg:=Max(0,dealdmg);
    AB_MORTAL_STRIKE:
      attacker.hitchance_mod:=attacker.hitchance_mod-cprop[3];  // Definitiver Treffer zurücknehmen
      attacker.eraseprop(AB_CPROP);
      SendSysMessagePergon(attacker,struct{uc_text:=CAscz("Ihr schlagt zielsicher zu!"),lang:="DEU"},
                                    struct{uc_text:=CAscz("You hit unerring!"),lang:="ENU"});
      If (!defender.isA(POLCLASS_NPC))
        SendSysMessagePergon(defender,struct{uc_text:=CAscz("Ihr hattet keine Chance auszuweichen!"),lang:="DEU"},
                                      struct{uc_text:=CAscz("You had no chance to evade!"),lang:="ENU"});
      EndIf
    AB_SURPRISE_ATTACK:
      RevokePrivilege(attacker,"hiddenattack");
      attacker.eraseprop(AB_CPROP);
    default:
      Break;
  EndCase
EndFunction

Function PraeAusweichenDefender(attacker,defender,byref dealdmg)
  var cprop:=defender.getprop(AB_CPROP);
  Case (cprop[1])
    AB_FEINT:
      If (attacker.serial==cprop[4]) // richtige Gegner?
        dealdmg-=cprop[3];
        dealdmg:=Max(0,dealdmg);
        defender.eraseprop(AB_CPROP);
        SendSysMessagePergon(defender,struct{uc_text:=CAscz("Ihr führt eine Finte aus!"),lang:="DEU"},
                                      struct{uc_text:=CAscz("You make a feint!"),lang:="ENU"});
        If (!attacker.isA(POLCLASS_NPC))
          SendSysMessagePergon(attacker,struct{uc_text:=CAscz("Euer Gegner führt eine Finte aus!"),lang:="DEU"},
                                        struct{uc_text:=CAscz("Your opponent makes a feint!"),lang:="ENU"});
        EndIf
      EndIf
    default:
      Break;
  EndCase
EndFunction

Function PostAusweichenAttacker(attacker,defender,byref dealdmg)
  var cprop:=attacker.getprop(AB_CPROP);
  Case (cprop[1])
    AB_BLEED_ATTACK:
      Start_ScriptPergon(":combatabilities:bleedattack",{defender,attacker});
      SendSysMessagePergon(attacker,struct{uc_text:=CAscz("Ihr fügt eurem Gegner eine schwere Fleischwunde zu!"),lang:="DEU"},
                                    struct{uc_text:=CAscz("Your target is bleeding!"),lang:="ENU"});
      If (!defender.isA(POLCLASS_NPC))
        SendSysMessagePergon(defender,struct{uc_text:=CAscz("Euch wird eine schwere Fleischwunde zugefügt!"),lang:="DEU"},
                                      struct{uc_text:=CAscz("You are bleeding!"),lang:="ENU"});
      EndIf
      attacker.eraseprop(AB_CPROP);
    AB_WHIRLWIND:
      SendSysMessagePergon(attacker,struct{uc_text:=CAscz("Ihr führt eine Wirbelattacke aus!"),lang:="DEU"},
                                    struct{uc_text:=CAscz("You make a whirlwindattack!"),lang:="ENU"});
      attacker.eraseprop(AB_CPROP);
      MakeWhirlWind(attacker,defender);
    AB_PARALYZING_BLOW:
      If ((!defender.getprop("spell_paralyze")) && (!defender.getprop("spell_statue")))
        defender.paralyzed:= 1;      // paralyze spelltarget
        defender.frozen:= 1;       // freeze spelltarget (necessary 'cos paralyze aint enough)
        IncRevision(defender);

        Var tag:= array, dur:=3;  // Player nur 3s
        If (defender.isA(POLCLASS_NPC))
          dur:=6;
        EndIf
        tag[1]:= ReadGameClock() + dur;
        tag[2]:= 100; // Power für Schmelzen
        tag[5]:= ReadMilliSecondClock();

        SetObjProperty(defender, "spell_paralyze", tag);
        SendSpellWaiter(defender, "spell_paralyze", dur, tag[5], defender);

        SendSysMessagePergon(attacker,struct{uc_text:=CAscz("Ihr überrascht euren Gegner!"),lang:="DEU"},
                                      struct{uc_text:=CAscz("You suprised your opponent!"),lang:="ENU"});
        If (!defender.isA(POLCLASS_NPC))
          SendSysMessagePergon(defender,struct{uc_text:=CAscz("Ihr wurdet überrascht und seit paralysiert!"),lang:="DEU"},
                                        struct{uc_text:=CAscz("You are suprised by this attack and get paralyzed!"),lang:="ENU"});
        EndIf
      EndIf
      attacker.eraseprop(AB_CPROP);
    AB_PSYCHIC_ATTACK:
      If (!defender.getprop("ab_psychic_attack"))
        var tag:={};
        tag[1]:=cprop[3];
        tag[2]:=ReadMilliSecondClock();
        defender.setprop("ab_psychic_attack",tag);
        SetCastPowerProperty(defender,"Magie",cprop[3],"","",1);
        SendSpellWaiter(defender, "ab_psychic_attack", cprop[4], tag[2], defender);
        RefreshSpellPowerGump(defender);
        SendSysMessagePergon(attacker,struct{uc_text:=CAscz("Ihr nehmt eurem Gegner die Konzentration!"),lang:="DEU"},
                                      struct{uc_text:=CAscz("You take your opponent concentration!"),lang:="ENU"});
        If (!defender.isA(POLCLASS_NPC))
          SendSysMessagePergon(defender,struct{uc_text:=CAscz("Ihr wurdet überrascht und verliehrt die Konzentration!"),lang:="DEU"},
                                        struct{uc_text:=CAscz("You are suprised by this attack and loose concentration!"),lang:="ENU"});
        EndIf
      EndIf
      attacker.eraseprop(AB_CPROP);
    AB_SHADOWATTACK:
      SendSysMessagePergon(attacker,struct{uc_text:=CAscz("Ihr schlagt zu und versteckt euch im Schatten!"),lang:="DEU"},
                                    struct{uc_text:=CAscz("You strike and hide in the shadows!"),lang:="ENU"});
      If (!defender.isA(POLCLASS_NPC))
        SendSysMessagePergon(defender,struct{uc_text:=CAscz("Euer Gegner ist in den Schatten verschwunden!"),lang:="DEU"},
                                        struct{uc_text:=CAscz("Your opponent hides in the Shadows!"),lang:="ENU"});
      EndIf
      SetObjProperty(attacker, "hiding_diff", CInt(GetSkillPergon(attacker, SKILLID_VERSTECKEN)*2/5));
      If (attacker.getprop("spell_invis"))
        attacker.eraseprop("spell_invis");
      EndIf
      attacker.setwarmode(0);
      attacker.hidden:=1;
      attacker.stealthsteps:=2;
      attacker.eraseprop(AB_CPROP);
    AB_ENEMY_OF_ONE:
      If (defender.isA(POLCLASS_NPC))
        If (cprop[3])
          If (cprop[3]==GetNPCConfig(defender.npctemplate).species)
            dealdmg*=1.5;
          EndIf
        EndIf
      EndIf
    AB_KI_ATTACK:
      If (CoordinateDistanceEuclidean(attacker.x, attacker.y, cprop[3], cprop[4])>=5)
        dealdmg*=1.5;
        SendSysMessagePergon(attacker,struct{uc_text:=CAscz("Ihr überrennt euren Gegner!"),lang:="DEU"},
                                      struct{uc_text:=CAscz("You overrun your opponent!"),lang:="ENU"});
        If (!defender.isA(POLCLASS_NPC))
          SendSysMessagePergon(defender,struct{uc_text:=CAscz("Ihr wurdet von eurem Gegner überrannt!"),lang:="DEU"},
                                        struct{uc_text:=CAscz("You get overruned by your opponent!"),lang:="ENU"});
        EndIf
      Else
        SendSysMessagePergon(attacker,struct{uc_text:=CAscz("Euch fehlt der nötige Schwung!"),lang:="DEU"},
                                      struct{uc_text:=CAscz("You lack the necessary impetus!"),lang:="ENU"});
      EndIf
      attacker.eraseprop(AB_CPROP);
    AB_DISMOUNT:
      If (GetEquipmentByLayer(defender, 25))
        PetDismount(defender,defender);
        SendSysMessagePergon(attacker,struct{uc_text:=CAscz("Ihr schlagt euren Gegner aus dem Sattel!"),lang:="DEU"},
                                      struct{uc_text:=CAscz("The force of your attack has dislodged them from their mount!"),lang:="ENU"});
        If (!defender.isA(POLCLASS_NPC))
          SendSysMessagePergon(defender,struct{uc_text:=CAscz("Ihr wurdet von eurem Gegner aus dem Sattel geschlagen!"),lang:="DEU"},
                                        struct{uc_text:=CAscz("You fall off your mount!"),lang:="ENU"});
        EndIf
      Else
        SendSysMessagePergon(attacker,struct{uc_text:=CAscz("Eure Gegner hat kein Reittier von dem er Fallen könnte"),lang:="DEU"},
                                      struct{uc_text:=CAscz("This attack only works on mounted targets!"),lang:="ENU"});
      EndIf
      attacker.eraseprop(AB_CPROP);
    AB_DOUBLE_STRIKE:
      // wie im core nur mit festen "waffenspeed" 140 ->mindestens doppeltsoschnell (15000*1000 = s->ms)
      attacker.setswingtimer(15000000/((GetDexPergon(attacker)+100)*140));
      attacker.eraseprop(AB_CPROP);
      dealdmg*=0.8; // nur 80% des Schadens machen
      SendSysMessagePergon(attacker,struct{uc_text:=CAscz("Ihr schlagt blitzschnell zu!"),lang:="DEU"},
                                    struct{uc_text:=CAscz("You attack with lightning speed!"),lang:="ENU"});
      If (!defender.isA(POLCLASS_NPC))
        SendSysMessagePergon(defender,struct{uc_text:=CAscz("Euer Gegner schlägt blitzschnell zu"),lang:="DEU"},
                                      struct{uc_text:=CAscz("Your attacker strikes with lightning speed!"),lang:="ENU"});
      EndIf
    default:
      Break;
  EndCase
EndFunction

Function PostAusweichenDefender(attacker,defender,byref dealdmg)
  var cprop:=defender.getprop(AB_CPROP);
  Case (cprop[1])
    AB_FEINT:
      If (attacker.serial==cprop[4]) // richtige Gegner?
        dealdmg-=cprop[3];
        dealdmg:=Max(0,dealdmg);
      EndIf
    AB_ENEMY_OF_ONE:
      If (cprop[3])
        If (cprop[3]<>GetNPCConfig(defender.npctemplate).species)
          dealdmg*=1.25;
        EndIf
      Else
        cprop[3]:=GetNPCConfig(defender.npctemplate).species;
      EndIf
    AB_CONFIDENCE:
      If (GetStaminaPergon(defender)>0)
        HealDamage(defender,cprop[3]);
        SetStaminaPergon(defender,(GetStaminaPergon(defender)-cprop[3]));
      Else
        SendSysMessagePergon(defender,struct{uc_text:=CAscz("Der Rausch verfliegt!"),lang:="DEU"},
                                      struct{uc_text:=CAscz("The flush vanish!"),lang:="ENU"});
        defender.eraseprop(AB_CPROP);
      EndIf
    AB_BLOCK:
      dealdmg:=0;
      SendSysMessagePergon(defender,struct{uc_text:=CAscz("Ihr blockt den gesamten Schaden!"),lang:="DEU"},
                                    struct{uc_text:=CAscz("You block the damage!"),lang:="ENU"});
      If (!attacker.isA(POLCLASS_NPC))
        SendSysMessagePergon(attacker,struct{uc_text:=CAscz("Ihr könnt den Schildwall nicht durchbrechen!"),lang:="DEU"},
                                      struct{uc_text:=CAscz("You cannot pierce the shieldwall!"),lang:="ENU"});
      EndIf
      defender.eraseprop(AB_CPROP);
    default:
      Break;
  EndCase
EndFunction

///////////////////////////
// Syshook Function
///////////////////////////
Function HitMissDefender(attacker,defender)
  attacker:=attacker;
  var cprop:=defender.getprop(AB_CPROP);
  Case (cprop[1])
    AB_CONFIDENCE:
      If (GetStaminaPergon(defender)>0)
        HealDamage(defender,cprop[3]);
        SetStaminaPergon(defender,(GetStaminaPergon(defender)-cprop[3]));
      Else
        SendSysMessagePergon(defender,struct{uc_text:=CAscz("Der Rausch verfliegt!"),lang:="DEU"},
                                      struct{uc_text:=CAscz("The flush vanish!"),lang:="ENU"});
        defender.eraseprop(AB_CPROP);
      EndIf
    default:
      Break;
  EndCase
EndFunction

//Function HitMissAttacker(attacker,defender)
//  var cprop:=attacker.getprop(AB_CPROP);
//  Case (cprop[1])
//    default:
//      Break;
//  EndCase
//EndFunction

// Wirbelwind attacke ausführen
Function MakeWhirlWind(attacker,defender)
  ForEach mobile in (ListMobilesInLineOfSight( attacker, 1 ))
    If (mobile.serial==attacker.serial) // zur sicherheit extra checken
      continue;
    EndIf
    
    If (mobile.serial<>defender.serial)  // der bekommt schon
      If (!mobile.isA(POLCLASS_NPC))
        SendSysMessagePergon(mobile,struct{uc_text:=CAscz("Ihr werdet durch eine Wirbelattacke getroffen!"),lang:="DEU"},
                                    struct{uc_text:=CAscz("Your are hit bei a Whirlwindattack!"),lang:="ENU"});
      EndIf
      attacker.attack_once(mobile);
      SleepMs(100); // kleine Pause
    EndIf
  EndForEach
EndFunction
///////////////////////////
// Ende Hitscriptfunktionen
///////////////////////////

///////////////////////////////////////////////
// Startet entsprechende Funktionen zum Starten
///////////////////////////////////////////////
Function Start_Ability(who,ability,cfg)
  Case (ability)
    AB_ARMOR_IGNORE:    Start_Armor_Ignore(who,cfg);
    AB_BLEED_ATTACK:    Start_Bleed_Attack(who,cfg);
    AB_DEFENSE_MASTERY: Start_Defense_Mastery(who,cfg);
    AB_FEINT:           Start_Feint(who,cfg);
    AB_WHIRLWIND:       Start_Whirlwind(who,cfg);
    AB_PARALYZING_BLOW: Start_Paralyzing_Blow(who,cfg);
    AB_PSYCHIC_ATTACK:  Start_Psychic_Attack(who,cfg);
    AB_SHADOWATTACK:    Start_ShadowAttack(who,cfg);
    AB_CONFIDENCE:      Start_Confidence(who,cfg);
    AB_ENEMY_OF_ONE:    Start_Enemy_of_One(who,cfg);
    AB_SHADOWJUMP:      Start_Shadowjump(who,cfg);
    AB_KI_ATTACK:       Start_KI_Attack(who,cfg);
    AB_BLADEWEAVE:      Start_BladeWeave(who,cfg);
    AB_DISMOUNT:        Start_Dismount(who,cfg);
    AB_BLOCK:           Start_Block(who,cfg);
    AB_MORTAL_STRIKE:   Start_Mortal_Strike(who,cfg);
    AB_EVASION:         Start_Evasion(who,cfg);
    AB_MOVING_SHOT:     Start_Moving_Shot(who,cfg);
    AB_SURPRISE_ATTACK: Start_Surprise_Attack(who,cfg);
    AB_DOUBLE_STRIKE:   Start_Double_Strike(who,cfg);
    default: SysLog("FEHLER: Unbekannte Ability genutzt: "+ability);
  EndCase
EndFunction

/////////////////////////////////
//Einen Schlag Rüstung ignorieren
/////////////////////////////////
Function Start_Armor_Ignore(who,cfg)
  If (CheckAndReduce(who,cfg.hploss,cfg.stamloss,cfg.armod))
    If (MakeSkillCheckWithChance(cfg.chance))
      Var tag:={};
      tag[1]:=AB_ARMOR_IGNORE;
      tag[2]:=ReadMilliSecondClock();
      who.setprop(AB_CPROP,tag);
      SendSpellWaiter(who, AB_CPROP, cfg.time, tag[2], who);
      PlaySoundEffect(who , CInt(cfg.sound));
    Else
      SendSysMessagePergon(who,struct{uc_text:=CAscz("Ihr versagt dabei "+cfg.name+" anzuwenden."),lang:="DEU"},
                               struct{uc_text:=CAscz("You failed to use "+cfg.name+"."),lang:="ENU"});
    EndIf
  EndIf
EndFunction


// Einen Schlag noch weiter Schaden machen (bleedattack.src)
Function Start_Bleed_Attack(who,cfg)
  If (CheckAndReduce(who,cfg.hploss,cfg.stamloss,cfg.armod))
    If (MakeSkillCheckWithChance(cfg.chance))
      Var tag:={};
      tag[1]:=AB_BLEED_ATTACK;
      tag[2]:=ReadMilliSecondClock();
      who.setprop(AB_CPROP,tag);
      SendSpellWaiter(who, AB_CPROP, cfg.time, tag[2], who);
      PlaySoundEffect(who , CInt(cfg.sound));
    Else
      SendSysMessagePergon(who,struct{uc_text:=CAscz("Ihr versagt dabei "+cfg.name+" anzuwenden."),lang:="DEU"},
                               struct{uc_text:=CAscz("You failed to use "+cfg.name+"."),lang:="ENU"});
    EndIf
  EndIf
EndFunction

// Armod kurze Zeit hochsetzen, dafür weniger Schaden
Function Start_Defense_Mastery(who,cfg)
  If (CheckAndReduce(who,cfg.hploss,cfg.stamloss,cfg.armod))
    If (MakeSkillCheckWithChance(cfg.chance))
      Var mod:=CInt(30.0*GetSkillValueforWeapon(who)/130.0); // 0..30
      who.ar_mod:=who.ar_mod+mod;
      Var tag:={};
      tag[1]:=AB_DEFENSE_MASTERY;
      tag[2]:=ReadMilliSecondClock();
      tag[3]:=mod;
      who.setprop(AB_CPROP,tag);
      PlaySoundEffect(who , CInt(cfg.sound));
      SendSpellWaiter(who, AB_CPROP, cfg.time, tag[2], who);

      SendSysMessagePergon(who,struct{uc_text:=CAscz("Ihr geht in Verteidigunsposition!"),lang:="DEU"},
                               struct{uc_text:=CAscz("You get into defenceposition!"),lang:="ENU"});
    Else
      SendSysMessagePergon(who,struct{uc_text:=CAscz("Ihr versagt dabei "+cfg.name+" anzuwenden."),lang:="DEU"},
                               struct{uc_text:=CAscz("You failed to use "+cfg.name+"."),lang:="ENU"});
    EndIf
  EndIf
EndFunction

// Dmg reduzieren für aktuellen Gegner einmal
Function Start_Feint(who,cfg)
  If (CheckAndReduce(who,cfg.hploss,cfg.stamloss,cfg.armod))
    If (MakeSkillCheckWithChance(cfg.chance))
      If (!who.opponent)
        SendSysMessagePergon(who,struct{uc_text:=CAscz("Ihr solltet euch auf einen Gegner konzentrieren."),lang:="DEU"},
                                 struct{uc_text:=CAscz("You should concentrate on one opponent."),lang:="ENU"});
        Return;
      EndIf
      Var mod:=CInt(50.0*GetSkillValueforWeapon(who)/130.0); // 0..50
      Var tag:={};
      tag[1]:=AB_FEINT;
      tag[2]:=ReadMilliSecondClock();
      tag[3]:=mod;
      tag[4]:=who.opponent.serial;
      who.setprop(AB_CPROP,tag);
      PlaySoundEffect(who , CInt(cfg.sound));
      SendSpellWaiter(who, AB_CPROP, cfg.time, tag[2], who);

      SendSysMessagePergon(who,struct{uc_text:=CAscz("Ihr konzentiert euch nun ganz auf euren Gegner"),lang:="DEU"},
                               struct{uc_text:=CAscz("You start to concentrate on your opponent"),lang:="ENU"});
    Else
      SendSysMessagePergon(who,struct{uc_text:=CAscz("Ihr versagt dabei "+cfg.name+" anzuwenden."),lang:="DEU"},
                               struct{uc_text:=CAscz("You failed to use "+cfg.name+"."),lang:="ENU"});
    EndIf
  EndIf
EndFunction

// 1Schlag greift alle im umkreis zusätzlich an
Function Start_Whirlwind(who,cfg)
  If (CheckAndReduce(who,cfg.hploss,cfg.stamloss,cfg.armod))
    If (MakeSkillCheckWithChance(cfg.chance))
      Var tag:={};
      tag[1]:=AB_WHIRLWIND;
      tag[2]:=ReadMilliSecondClock();
      who.setprop(AB_CPROP,tag);
      SendSpellWaiter(who, AB_CPROP, cfg.time, tag[2], who);
      PlaySoundEffect(who , CInt(cfg.sound));
    Else
      SendSysMessagePergon(who,struct{uc_text:=CAscz("Ihr versagt dabei "+cfg.name+" anzuwenden."),lang:="DEU"},
                               struct{uc_text:=CAscz("You failed to use "+cfg.name+"."),lang:="ENU"});
    EndIf
  EndIf
EndFunction

// 1Schlag Minipara
Function Start_Paralyzing_Blow(who,cfg)
  If (CheckAndReduce(who,cfg.hploss,cfg.stamloss,cfg.armod))
    If (MakeSkillCheckWithChance(cfg.chance))
      Var tag:={};
      tag[1]:=AB_PARALYZING_BLOW;
      tag[2]:=ReadMilliSecondClock();
      who.setprop(AB_CPROP,tag);
      SendSpellWaiter(who, AB_CPROP, cfg.time, tag[2], who);
      PlaySoundEffect(who , CInt(cfg.sound));
    Else
      SendSysMessagePergon(who,struct{uc_text:=CAscz("Ihr versagt dabei "+cfg.name+" anzuwenden."),lang:="DEU"},
                               struct{uc_text:=CAscz("You failed to use "+cfg.name+"."),lang:="ENU"});
    EndIf
  EndIf
EndFunction

// 1Schlag Spellpowersenken
Function Start_Psychic_Attack(who,cfg)
  If (CheckAndReduce(who,cfg.hploss,cfg.stamloss,cfg.armod))
    If (MakeSkillCheckWithChance(cfg.chance))
      Var tag:={};
      tag[1]:=AB_PSYCHIC_ATTACK;
      tag[2]:=ReadMilliSecondClock();
      tag[3]:=CInt(50+50.0*GetSkillValueforWeapon(who)/130.0); // 50..100 spellpower
      tag[4]:=CInt(10+10.0*GetSkillValueforWeapon(who)/130.0); // 10..20s
      who.setprop(AB_CPROP,tag);
      SendSpellWaiter(who, AB_CPROP, cfg.time, tag[2], who);
      PlaySoundEffect(who , CInt(cfg.sound));
    Else
      SendSysMessagePergon(who,struct{uc_text:=CAscz("Ihr versagt dabei "+cfg.name+" anzuwenden."),lang:="DEU"},
                               struct{uc_text:=CAscz("You failed to use "+cfg.name+"."),lang:="ENU"});
    EndIf
  EndIf
EndFunction

// 1Schlag verstecken
Function Start_ShadowAttack(who,cfg)
  If (CheckAndReduce(who,cfg.hploss,cfg.stamloss,cfg.armod))
    If (MakeSkillCheckWithChance(cfg.chance))
      Var tag:={};
      tag[1]:=AB_SHADOWATTACK;
      tag[2]:=ReadMilliSecondClock();
      who.setprop(AB_CPROP,tag);
      SendSpellWaiter(who, AB_CPROP, cfg.time, tag[2], who);
      PlaySoundEffect(who , CInt(cfg.sound));
    Else
      SendSysMessagePergon(who,struct{uc_text:=CAscz("Ihr versagt dabei "+cfg.name+" anzuwenden."),lang:="DEU"},
                               struct{uc_text:=CAscz("You failed to use "+cfg.name+"."),lang:="ENU"});
    EndIf
  EndIf
EndFunction

// kurze Zeit pro Schlag HP gewinnen, stam verliehren
Function Start_Confidence(who,cfg)
  If (CheckAndReduce(who,cfg.hploss,cfg.stamloss,cfg.armod))
    If (MakeSkillCheckWithChance(cfg.chance))
      Var mod:=CInt(5+10.0*GetSkillValueforWeapon(who)/130.0); // 5..15hp/stam
      Var tag:={};
      tag[1]:=AB_CONFIDENCE;
      tag[2]:=ReadMilliSecondClock();
      tag[3]:=mod;
      who.setprop(AB_CPROP,tag);
      PlaySoundEffect(who , CInt(cfg.sound));
      SendSpellWaiter(who, AB_CPROP, cfg.time, tag[2], who);

      SendSysMessagePergon(who,struct{uc_text:=CAscz("Ihr geratet in einen Kampfrausch!"),lang:="DEU"},
                               struct{uc_text:=CAscz("You are getting a warrush!"),lang:="ENU"});
    Else
      SendSysMessagePergon(who,struct{uc_text:=CAscz("Ihr versagt dabei "+cfg.name+" anzuwenden."),lang:="DEU"},
                               struct{uc_text:=CAscz("You failed to use "+cfg.name+"."),lang:="ENU"});
    EndIf
  EndIf
EndFunction

// kurze Zeit erster Gegner der getroffen hat Species mehr Schaden machen,
// andere Species mehr Schaden abbekommen
Function Start_Enemy_of_One(who,cfg)
  If (CheckAndReduce(who,cfg.hploss,cfg.stamloss,cfg.armod))
    If (MakeSkillCheckWithChance(cfg.chance))
      Var tag:={};
      tag[1]:=AB_ENEMY_OF_ONE;
      tag[2]:=ReadMilliSecondClock();
      tag[3]:=0;  // NPC.species eintrag
      who.setprop(AB_CPROP,tag);
      PlaySoundEffect(who , CInt(cfg.sound));
      SendSpellWaiter(who, AB_CPROP, cfg.time, tag[2], who);

      SendSysMessagePergon(who,struct{uc_text:=CAscz("Ihr konzentriert euch auf euren Gegner!"),lang:="DEU"},
                               struct{uc_text:=CAscz("You concentrate on your enemy!"),lang:="ENU"});
    Else
      SendSysMessagePergon(who,struct{uc_text:=CAscz("Ihr versagt dabei "+cfg.name+" anzuwenden."),lang:="DEU"},
                               struct{uc_text:=CAscz("You failed to use "+cfg.name+"."),lang:="ENU"});
    EndIf
  EndIf
EndFunction

// Versteckter Teleport
Function Start_Shadowjump(who,cfg)
  If (CheckAndReduce(who,cfg.hploss,cfg.stamloss,cfg.armod))
    If (!who.hidden)
      SendSysMessagePergon(who,struct{uc_text:=CAscz("Ihr müsst versteckt sein."),lang:="DEU"},
                               struct{uc_text:=CAscz("You have to be hidden."),lang:="ENU"});
      Return;
    EndIf
    If (who.multi)
      SendSysMessagePergon(who,struct{uc_text:=CAscz("Die Götter unterbinden Euren Versuch, dorthin zu teleportieren!"),lang:="DEU"},
                               struct{uc_text:=CAscz("The Gods do not let you teleport there!"),lang:="ENU"});
      Return;
    EndIf
    If (MakeSkillCheckWithChance(cfg.chance))
      var oldx:=who.x,oldy:=who.y;
      var where:=TargetCoordinates(who);
      If ((oldx<>who.x) || (oldy<>who.y))
        SendSysMessagePergon(who,struct{uc_text:=CAscz("Ihr verliehrt die Konzentration."),lang:="DEU"},
                                 struct{uc_text:=CAscz("You lose the concentration."),lang:="ENU"});
        Return;
      EndIf
      If (!where)
        SendSysMessagePergon(who,struct{uc_text:=CAscz("Abbruch"),lang:="DEU"},
                                 struct{uc_text:=CAscz("Abort"),lang:="ENU"});
        Return;
      EndIf
      If (!CheckLosAt(who, where.x, where.y, where.z))
        SendSysMessagePergon(who,struct{uc_text:=CAscz("Ihr könnt das nicht sehen."),lang:="DEU"},
                                 struct{uc_text:=CAscz("You cannot see that."),lang:="ENU"});
        Return;
      EndIf
      If (CoordinateDistance(who.x,who.y, where.x, where.y)>10)
        SendSysMessagePergon(who,struct{uc_text:=CAscz("Dies ist zu weit weg."),lang:="DEU"},
                                 struct{uc_text:=CAscz("This is to far away."),lang:="ENU"});
        Return;
      EndIf
      var house := GetHouseContainingObject(where);
      If (house and !IsHouseOwnerOrFriend(house, who))
        SendSysMessagePergon(who,struct{uc_text:=CAscz("Die Götter unterbinden Euren Versuch, dorthin zu teleportieren!"),lang:="DEU"},
                                 struct{uc_text:=CAscz("The Gods do not let you teleport there!"),lang:="ENU"});
        Return;
      EndIf
      If (GetMapInfo(where.x, where.y,where.realm).landtile in {0xA8, 0xA9, 0x64, 0xAA, 0xAB}) //wasser
        SendSysMessagePergon(who,struct{uc_text:=CAscz("Dorthin könnt Ihr nicht teleportieren!"),lang:="DEU"},
                                 struct{uc_text:=CAscz("You cannot teleport there!"),lang:="ENU"});
        Return;
      EndIf

      SleepMS(500);
      If (!CheckSkillPergon(who,SKILLID_VERSTECKEN,-1,0))
        who.hidden:=0;
        SendSysMessagePergon(who,struct{uc_text:=CAscz("Ihr versagt dabei euch zu verstecken."),lang:="DEU"},
                                 struct{uc_text:=CAscz("You failed to hide."),lang:="ENU"});
      Else
        PlaySoundEffect(who , CInt(cfg.sound));
        MoveObjectToLocation(who, where.x, where.y, where.z, who.realm);
      EndIf
    Else
      SendSysMessagePergon(who,struct{uc_text:=CAscz("Ihr versagt dabei "+cfg.name+" anzuwenden."),lang:="DEU"},
                               struct{uc_text:=CAscz("You failed to use "+cfg.name+"."),lang:="ENU"});
    EndIf
  EndIf
EndFunction

// kurz Zeit jeweiter die Strecke ist die man zurückgelegt hat
// um so größer ist der dmgmod
Function Start_KI_Attack(who,cfg)
  If (CheckAndReduce(who,cfg.hploss,cfg.stamloss,cfg.armod))
    If (who.weapon)
      If (who.weapon.attribute==SKILLID_BOGENSCHIESSEN)
        SendSysMessagePergon(who,struct{uc_text:=CAscz("Ein Sturmangriff klappt nicht mit einer Fernwaffe!"),lang:="DEU"},
                                 struct{uc_text:=CAscz("Assault dont work with rangedweapons!"),lang:="ENU"});
        Return;
      EndIf
    EndIf
    If (MakeSkillCheckWithChance(cfg.chance))
      Var tag:={};
      tag[1]:=AB_KI_ATTACK;
      tag[2]:=ReadMilliSecondClock();
      tag[3]:=who.x;
      tag[4]:=who.y;
      who.setprop(AB_CPROP,tag);
      PlaySoundEffect(who , CInt(cfg.sound));
      SendSpellWaiter(who, AB_CPROP, cfg.time, tag[2], who);

      SendSysMessagePergon(who,struct{uc_text:=CAscz("Ihr startet einen Sturmangriff!"),lang:="DEU"},
                               struct{uc_text:=CAscz("You start a assault!"),lang:="ENU"});
    Else
      SendSysMessagePergon(who,struct{uc_text:=CAscz("Ihr versagt dabei "+cfg.name+" anzuwenden."),lang:="DEU"},
                               struct{uc_text:=CAscz("You failed to use "+cfg.name+"."),lang:="ENU"});
    EndIf
  EndIf
EndFunction

// kurze Zeit hitchance+
Function Start_BladeWeave(who,cfg)
  If (CheckAndReduce(who,cfg.hploss,cfg.stamloss,cfg.armod))
    If (MakeSkillCheckWithChance(cfg.chance))
      Var tag:={};
      tag[1]:=AB_BLADEWEAVE;
      tag[2]:=ReadMilliSecondClock();
      tag[3]:=RandomIntMinMax(40,101); // 4-10% hitchancemod
      who.hitchance_mod:=who.hitchance_mod+tag[3];
      who.setprop(AB_CPROP,tag);
      PlaySoundEffect(who , CInt(cfg.sound));
      SendSpellWaiter(who, AB_CPROP, cfg.time, tag[2], who);

      SendSysMessagePergon(who,struct{uc_text:=CAscz("Ihr konzentriert euch auf eure Waffe!"),lang:="DEU"},
                               struct{uc_text:=CAscz("You start to concentrate on your weapon!"),lang:="ENU"});
    Else
      SendSysMessagePergon(who,struct{uc_text:=CAscz("Ihr versagt dabei "+cfg.name+" anzuwenden."),lang:="DEU"},
                               struct{uc_text:=CAscz("You failed to use "+cfg.name+"."),lang:="ENU"});
    EndIf
  EndIf
EndFunction

// 1Schlag Absteigen lassen
Function Start_Dismount(who,cfg)
  If (CheckAndReduce(who,cfg.hploss,cfg.stamloss,cfg.armod))
    If (MakeSkillCheckWithChance(cfg.chance))
      Var tag:={};
      tag[1]:=AB_DISMOUNT;
      tag[2]:=ReadMilliSecondClock();
      who.setprop(AB_CPROP,tag);
      SendSpellWaiter(who, AB_CPROP, cfg.time, tag[2], who);
      PlaySoundEffect(who , CInt(cfg.sound));
    Else
      SendSysMessagePergon(who,struct{uc_text:=CAscz("Ihr versagt dabei "+cfg.name+" anzuwenden."),lang:="DEU"},
                               struct{uc_text:=CAscz("You failed to use "+cfg.name+"."),lang:="ENU"});
    EndIf
  EndIf
EndFunction

// 1Schlag komplett blocken
Function Start_Block(who,cfg)
  If (CheckAndReduce(who,cfg.hploss,cfg.stamloss,cfg.armod))
    If (MakeSkillCheckWithChance(cfg.chance))
      Var tag:={};
      tag[1]:=AB_BLOCK;
      tag[2]:=ReadMilliSecondClock();
      who.setprop(AB_CPROP,tag);
      SendSpellWaiter(who, AB_CPROP, cfg.time, tag[2], who);
      PlaySoundEffect(who , CInt(cfg.sound));
    Else
      SendSysMessagePergon(who,struct{uc_text:=CAscz("Ihr versagt dabei "+cfg.name+" anzuwenden."),lang:="DEU"},
                               struct{uc_text:=CAscz("You failed to use "+cfg.name+"."),lang:="ENU"});
    EndIf
  EndIf
EndFunction

// Ein definitiver Treffer
Function Start_Mortal_Strike(who,cfg)
  If (CheckAndReduce(who,cfg.hploss,cfg.stamloss,cfg.armod))
    If (MakeSkillCheckWithChance(cfg.chance))
      Var tag:={};
      tag[1]:=AB_MORTAL_STRIKE;
      tag[2]:=ReadMilliSecondClock();
      tag[3]:=2000; // hitchance_mod
      who.hitchance_mod:=who.hitchance_mod+tag[3];
      who.setprop(AB_CPROP,tag);
      SendSpellWaiter(who, AB_CPROP, cfg.time, tag[2], who);
      PlaySoundEffect(who , CInt(cfg.sound));
    Else
      SendSysMessagePergon(who,struct{uc_text:=CAscz("Ihr versagt dabei "+cfg.name+" anzuwenden."),lang:="DEU"},
                               struct{uc_text:=CAscz("You failed to use "+cfg.name+"."),lang:="ENU"});
    EndIf
  EndIf
EndFunction

// eine Zeit lang evasionchance+
Function Start_Evasion(who,cfg)
  If (CheckAndReduce(who,cfg.hploss,cfg.stamloss,cfg.armod))
    If (MakeSkillCheckWithChance(cfg.chance))
      Var tag:={};
      tag[1]:=AB_EVASION;
      tag[2]:=ReadMilliSecondClock();
      tag[3]:=RandomIntMinMax(40,101); // 4-10% evasionchancemod
      who.evasionchance_mod:=who.evasionchance_mod+tag[3];
      who.setprop(AB_CPROP,tag);
      SendSpellWaiter(who, AB_CPROP, cfg.time, tag[2], who);
      PlaySoundEffect(who , CInt(cfg.sound));
    Else
      SendSysMessagePergon(who,struct{uc_text:=CAscz("Ihr versagt dabei "+cfg.name+" anzuwenden."),lang:="DEU"},
                               struct{uc_text:=CAscz("You failed to use "+cfg.name+"."),lang:="ENU"});
    EndIf
  EndIf
EndFunction

// Schießen in Bewegung (Bogen) für eine Zeit
Function Start_Moving_Shot(who,cfg)
  If (CheckAndReduce(who,cfg.hploss,cfg.stamloss,cfg.armod))
    If (who.weapon)
      If (who.weapon.attribute<>SKILLID_BOGENSCHIESSEN)
        SendSysMessagePergon(who,struct{uc_text:=CAscz(cfg.name+" klappt nicht mit einer Nahkampfwaffe!"),lang:="DEU"},
                                 struct{uc_text:=CAscz("Moving Shot dont work with shortrangeweapons!"),lang:="ENU"});
        Return;
      EndIf
    Else
      SendSysMessagePergon(who,struct{uc_text:=CAscz(cfg.name+" klappt nicht mit einer Nahkampfwaffe!"),lang:="DEU"},
                               struct{uc_text:=CAscz("Moving Shot dont work with shortrangeweapons!"),lang:="ENU"});
      Return;
    EndIf
    If (MakeSkillCheckWithChance(cfg.chance))
      // Wenn es sich um einen Buguser handelt, dann darf er die Fähigkeit erst einmal nicht nutzen
      If (!GetObjProperty(who, "nomovingshotwhilebugusing"))
        Var tag:={};
        tag[1]:=AB_MOVING_SHOT;
        tag[2]:=ReadMilliSecondClock();
        GrantPrivilege(who,"firewhilemoving");
        who.enable("firewhilemoving");
        who.setprop(AB_CPROP,tag);
        SendSpellWaiter(who, AB_CPROP, cfg.time, tag[2], who);
        PlaySoundEffect(who , CInt(cfg.sound));
      EndIf
    Else
      SendSysMessagePergon(who,struct{uc_text:=CAscz("Ihr versagt dabei "+cfg.name+" anzuwenden."),lang:="DEU"},
                               struct{uc_text:=CAscz("You failed to use "+cfg.name+"."),lang:="ENU"});
    EndIf
  EndIf
EndFunction

// 1Schlag versteckt attakieren
Function Start_Surprise_Attack(who,cfg)
  If (CheckAndReduce(who,cfg.hploss,cfg.stamloss,cfg.armod))
    If (MakeSkillCheckWithChance(cfg.chance))
      Var tag:={};
      tag[1]:=AB_SURPRISE_ATTACK;
      tag[2]:=ReadMilliSecondClock();
      GrantPrivilege(who,"hiddenattack");
      who.enable("hiddenattack");
      who.setprop(AB_CPROP,tag);
      SendSpellWaiter(who, AB_CPROP, cfg.time, tag[2], who);
      PlaySoundEffect(who , CInt(cfg.sound));
    Else
      SendSysMessagePergon(who,struct{uc_text:=CAscz("Ihr versagt dabei "+cfg.name+" anzuwenden."),lang:="DEU"},
                               struct{uc_text:=CAscz("You failed to use "+cfg.name+"."),lang:="ENU"});
    EndIf
  EndIf
EndFunction

Function Start_Double_Strike(who,cfg)
  If (CheckAndReduce(who,cfg.hploss,cfg.stamloss,cfg.armod))
    If (MakeSkillCheckWithChance(cfg.chance))
      Var tag:={};
      tag[1]:=AB_DOUBLE_STRIKE;
      tag[2]:=ReadMilliSecondClock();
      who.setprop(AB_CPROP,tag);
      SendSpellWaiter(who, AB_CPROP, cfg.time, tag[2], who);
      PlaySoundEffect(who , CInt(cfg.sound));
    Else
      SendSysMessagePergon(who,struct{uc_text:=CAscz("Ihr versagt dabei "+cfg.name+" anzuwenden."),lang:="DEU"},
                               struct{uc_text:=CAscz("You failed to use "+cfg.name+"."),lang:="ENU"});
    EndIf
  EndIf
EndFunction


// hp,stam,armod reduzieren
Function CheckAndReduce(who,hploss,stamloss,armod)
  If (hploss)
    If (hploss>(GetHPPergon(who)+1))
      SendSysMessagePergon(who,struct{uc_text:=CAscz("Ihr besitzt nicht genug Leben."),lang:="DEU"},
                               struct{uc_text:=CAscz("You have to less live."),lang:="ENU"});
      Return(0);
    Else
      ApplyRawDamagePergon(who,hploss);
    EndIf
  EndIf
  If (stamloss)
    If (stamloss>(GetStaminaPergon(who)+1))
      SendSysMessagePergon(who,struct{uc_text:=CAscz("Ihr seit zu erschöpft."),lang:="DEU"},
                               struct{uc_text:=CAscz("You are to exhausted."),lang:="ENU"});
      Return(0);
    Else
      SetStaminaPergon(who,(GetStaminaPergon(who)-stamloss));
    EndIf
  EndIf
  If (armod)
    armod:=SplitWords(armod);
    armod[1]:=CInt(armod[1]);
    armod[2]:=CInt(armod[2]);
    var oldcrit:=Is_Critical();
    var tag:={};
    Set_Critical(1);
    If (who.getprop(AB_ARMOD)) // noch vorhanden
      who.ar_mod:=who.ar_mod-armod[1]+who.getprop(AB_ARMOD)[1];
    Else
      who.ar_mod:=who.ar_mod-armod[1];
    EndIf
    tag[1]:=armod[1];
    tag[2]:=ReadMilliSecondClock();
    who.setprop(AB_ARMOD,tag);
    Set_Critical(oldcrit);
    SendSpellWaiter(who, AB_ARMOD, armod[2], tag[2], who);
  EndIf
  Return(1);
EndFunction

// Wiegroß ist der Skill der Waffenfähigkeit
Function GetSkillValueforWeapon(who)
  Var skill;
  If (who.weapon)
    skill:=who.weapon.attribute;
  Else
    skill:=SKILLID_FAUSTKAMPF;
  EndIf
  Return(GetSkillPergon(who,skill));
EndFunction

// Da eh nur relativ kein richtiger Skillcheck
// somit ist Fähigkeit aktivieren während man
// versteckt ist auch möglich
Function MakeSkillCheckWithChance(chance)
  If (RandomFloat(1.0)<=chance)
    Return(1);
  EndIf
  Return(0);
EndFunction
