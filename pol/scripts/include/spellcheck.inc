Include "include/thaumaturgy";
Include "include/modifyskill";
Include "include/objtype";
Include "include/packets";
Include ":newspells:resistance";
Include ":newspells:magicpergon";
Include ":combatabilities:combatabilities";

///////////////
// Konstanten
///////////////

Const PID_SPELLCHECK:= "#SpellCheck";
Const EVENTQUEUESIZE_SPELLCHECK:=500; // Die Initialisierung verlangt diese Groesse

Const DEBUG_EVENTS_SPELLCHECK:=1;

Function CheckSpell(byref who, byref event, server_restart:=0)            // checking sustained spells on mobiles
  var spelltag;

  Case (event.spell)
    "spell_dazzle":
      spelltag:= GetObjProperty(who, "spell_dazzle");
      If ((spelltag) && (event.uniqueid==spelltag[3]))           // check "Dazzle"
        If (!server_restart) // Der Server hat den Wert schon zerstoert...
          AlterIntModPergon(who,spelltag[2]);  // remove spelleffect
        EndIf
        ClearCastPowerProperty(who,"Magie","","",1);
        BuffIcons(who, event.spell, 0, 1);
        EraseObjProperty(who, "spell_dazzle");                  // remove spelltag
        Return(1);
      EndIf

    "spell_clumsy":
      spelltag:= GetObjProperty(who, "spell_clumsy");
      If ((spelltag) && (event.uniqueid==spelltag[3]))           // check "Clumsy"
        If (!server_restart) // Der Server hat den Wert schon zerstoert...
          AlterDexModPergon(who,spelltag[2]);  // remove spelleffect
        EndIf
        BuffIcons(who, event.spell, 0, 1);
        EraseObjProperty(who, "spell_clumsy");                  // remove spelltag
        Return(1);
      EndIf

    "spell_weaken":
      spelltag:= GetObjProperty(who, "spell_weaken");
      If ((spelltag) && (event.uniqueid==spelltag[3]))           // check "Weaken"
        If (!server_restart) // Der Server hat den Wert schon zerstoert...
          AlterStrModPergon(who, spelltag[2]);   // remove spelleffect
        EndIf
        BuffIcons(who, event.spell, 0, 1);
        EraseObjProperty(who, "spell_weaken");                  // remove spelltag
        Return(1);
      EndIf

    "spell_cunning":
      spelltag:= GetObjProperty(who, "spell_cunning");
      If ((spelltag) && (event.uniqueid==spelltag[3]))           // check "Cunning"
        If (!server_restart) // Der Server hat den Wert schon zerstoert...
          AlterIntModPergon(who, -spelltag[2]);   // remove spelleffect
        EndIf
        ClearCastPowerProperty(who,"Magie","","",0);
        BuffIcons(who, event.spell, 0, 1);
        EraseObjProperty(who, "spell_cunning");               // remove spelltag
        RefreshSpellPowerGump(who);
        Return(1);
      EndIf

    "spell_agility":
      spelltag:= GetObjProperty(who, "spell_agility");
      If ((spelltag) && (event.uniqueid==spelltag[3]))           // check "Agility"
        If (!server_restart) // Der Server hat den Wert schon zerstoert...
          AlterDexModPergon(who, -spelltag[2]);   // remove spelleffect
        EndIf
        BuffIcons(who, event.spell, 0, 1);
        EraseObjProperty(who, "spell_agility");               // remove spelltag
        Return(1);
      EndIf

    "spell_strength":
      spelltag:= GetObjProperty(who, "spell_strength");
      If ((spelltag) && (event.uniqueid==spelltag[3]))         // check "Strength"
        If (!server_restart) // Der Server hat den Wert schon zerstoert...
          AlterStrModPergon(who, -spelltag[2]);   // remove spelleffect
        EndIf
        BuffIcons(who, event.spell, 0, 1);
        EraseObjProperty(who, "spell_strength");                // remove spelltag
        Return(1);
      EndIf

    "spell_protect":
      spelltag:= GetObjProperty(who, "spell_protect");
      If ((spelltag) && (event.uniqueid==spelltag[3]))           // check "Protection"
        If (!server_restart) // Der Server hat den Wert schon zerstoert...
          who.ar_mod:= who.ar_mod-spelltag[2];                  // remove spelleffect
        EndIf
        BuffIcons(who, event.spell, 0, 1);
        EraseObjProperty(who, "spell_protect");               // remove spelltag
        Return(1);
      EndIf

    "spell_invis":
      If (!server_restart)
        spelltag:= GetObjProperty(who, "spell_invis");
        If ((spelltag) && (event.uniqueid==spelltag[2]))           // check "Invisibility"
          If (who.hidden)                                 // check If who is still hidden
            who.hidden:= 0;                             // remove spelleffect
            who.stealthsteps:= 0;
          EndIf
          EraseObjProperty(who, "spell_invis");               // remove spelltag
        EndIf
      EndIf

    "spell_incognito":
      If (!server_restart)
        spelltag:= GetObjProperty(who, "spell_incognito");
        // check "Incognito"
        If ((spelltag) && (event.uniqueid==spelltag[3]))
          PlaySoundEffect(who, SFX_SPELL_INCOGNITO);
          // remove spelleffect
          SetNamePergon(who, spelltag[2], "Spell Incognito - Reset");
          BuffIcons(who, event.spell, 0, 1);
          // remove spelltag
          EraseObjProperty(who, "spell_incognito");
          If (who.connected)
            PrintTextAbovePrivatePergon(who,
              "Euer Name lautet wieder " + who.name,
              "Your are called " + who.name + " again", who
            );
          EndIf
          // Zur Sicherheit nochmal Schickenlassen wegen Ausblendung
          // der Infos bei Tooltips
          IncRevision(who);
        EndIf
      EndIf

    "spell_poly":
      If (!server_restart)
        spelltag:= GetObjProperty(who, "spell_poly");
        If ((spelltag) && (event.uniqueid==spelltag[3]))             // check "Polymorph"
          PlaySoundEffect(who, SFX_SPELL_POLYMORPH);
          who.graphic:= spelltag[2];                          // remove spelleffect
          who.color := spelltag[4];                           // restore original color-setting // Taya 28.02.2008 
          BuffIcons(who, event.spell, 0, 1);
          EraseObjProperty(who, "spell_poly");                  // remove spelltag
        EndIf
      EndIf

    "spell_aura":
      If (!server_restart)
        spelltag:= GetObjProperty(who, "spell_aura");
        If ((spelltag) && (event.uniqueid==spelltag[4]))             // check Aura Spells
          Set_Critical(1);
          Case (spelltag[2])                                         // remove spelleffect
            "fire":  SetObjProperty(who, "resist_water", CInt(GetObjProperty(who, "resist_water")) + spelltag[3]);
            "water": SetObjProperty(who, "resist_fire", CInt(GetObjProperty(who, "resist_fire")) + spelltag[3]);
            "air":   SetObjProperty(who, "resist_earth", CInt(GetObjProperty(who, "resist_earth")) + spelltag[3]);
            "earth": SetObjProperty(who, "resist_air", CInt(GetObjProperty(who, "resist_air")) + spelltag[3]);
          EndCase
          Set_Critical(0);
          BuffIcons(who, event.spell, 0, 1);
          If (spelltag[5]);
          	GetProcess(spelltag[5]).sendevent(1);
          EndIf
          EraseObjProperty(who, "spell_aura");                       // remove spelltag
          If (!who.isa(POLCLASS_NPC))
            SendStatus(who);
          EndIf
        EndIf
      EndIf

    "spell_resist":
      If (!server_restart)
        spelltag:= GetObjProperty(who, "spell_resist");
        If ((spelltag) && (event.uniqueid==spelltag[6]))           // check Resistance Spells
          PlaySoundEffect(who, SFX_1F2);
          Case (spelltag[2])                              // remove spelleffect
            "fire":   Set_Critical(1);
                      SetObjProperty(who, "resist_fire", CInt(GetObjProperty(who, "resist_fire")) - spelltag[3]);
                      SetObjProperty(who, "resist_water", CInt(GetObjProperty(who, "resist_water")) + CInt(spelltag[3] / 2));
                      Set_Critical(0);
                      ClearCastPowerProperty(who,"Magie","","Fire",0);
                      ClearCastPowerProperty(who,"Magie","","Water",1);
                      RefreshSpellPowerGump(who);
                      If (who.connected)
                        PrintTextAbovePrivatePergon(who, "Ihr verliert Eure Resistenz gegen Feuer",
                                                         "You lose your resistance against fire", who);
                      EndIf
            "water":  Set_Critical(1);
                      SetObjProperty(who, "resist_water", CInt(GetObjProperty(who, "resist_water")) - spelltag[3]);
                      SetObjProperty(who, "resist_fire", CInt(GetObjProperty(who, "resist_fire")) + CInt(spelltag[3] / 2));
                      Set_Critical(0);
                      ClearCastPowerProperty(who,"Magie","","Water",0);
                      ClearCastPowerProperty(who,"Magie","","Fire",1);
                      RefreshSpellPowerGump(who);
                      If (who.connected)
                        PrintTextAbovePrivatePergon(who, "Ihr verliert Eure Resistenz gegen Wasser",
                                                         "You lose your resistance against water", who);
                      EndIf
            "air":    Set_Critical(1);
                      SetObjProperty(who, "resist_air", CInt(GetObjProperty(who, "resist_air")) - spelltag[3]);
                      SetObjProperty(who, "resist_earth", CInt(GetObjProperty(who, "resist_earth")) + CInt(spelltag[3] / 2));
                      Set_Critical(0);
                      ClearCastPowerProperty(who,"Magie","","Air",0);
                      ClearCastPowerProperty(who,"Magie","","Earth",1);
                      RefreshSpellPowerGump(who);
                      If (who.connected)
                        PrintTextAbovePrivatePergon(who, "Ihr verliert Eure Resistenz gegen Luft",
                                                         "You lose your resistance against air", who);
                      EndIf
            "earth":  Set_Critical(1);
                      SetObjProperty(who, "resist_earth", CInt(GetObjProperty(who, "resist_earth")) - spelltag[3]);
                      SetObjProperty(who, "resist_air", CInt(GetObjProperty(who, "resist_air")) + CInt(spelltag[3] / 2));
                      Set_Critical(0);
                      ClearCastPowerProperty(who,"Magie","","Earth",0);
                      ClearCastPowerProperty(who,"Magie","","Air",1);
                      RefreshSpellPowerGump(who);
                      If (who.connected)
                        PrintTextAbovePrivatePergon(who, "Ihr verliert Eure Resistenz gegen Erde",
                                                         "You lose your resistance against earth", who);
                      EndIf
            "poison": Set_Critical(1);
                      SetObjProperty(who, "resist_poison", CInt(GetObjProperty(who, "resist_poison")) - spelltag[3]);
                      Set_Critical(0);
                      ClearCastPowerProperty(who,"Magie","","Poison",0);
                      RefreshSpellPowerGump(who);
                      If (who.connected)
                        PrintTextAbovePrivatePergon(who, "Ihr verliert Eure Resistenz gegen Gifte",
                                                         "You lose your resistance against poison", who);
                      EndIf
          "elements": Set_Critical(1);
                      SetObjProperty(who, "resist_fire", CInt(GetObjProperty(who, "resist_fire")) - spelltag[3]);
                      SetObjProperty(who, "resist_water", CInt(GetObjProperty(who, "resist_water")) - spelltag[3]);
                      SetObjProperty(who, "resist_air", CInt(GetObjProperty(who, "resist_air")) - spelltag[3]);
                      SetObjProperty(who, "resist_earth", CInt(GetObjProperty(who, "resist_earth")) - spelltag[3]);
                      ClearCastPowerProperty(who,"Magie","","Fire",0);
                      ClearCastPowerProperty(who,"Magie","","Water",0);
                      ClearCastPowerProperty(who,"Magie","","Air",0);
                      ClearCastPowerProperty(who,"Magie","","Earth",0);
                      RefreshSpellPowerGump(who);
                      If (spelltag[4] == "immune")
                        SetObjProperty(who, "resist_poison", CInt(GetObjProperty(who, "resist_poison")) - spelltag[3]);
                        ClearCastPowerProperty(who,"Magie","","Poison",0);
                        Set_Critical(0);
                        If (who.connected)
                          PrintTextAbovePrivatePergon(who, "Ihr verliert Eure Unempfindlichkeit gegen alle Elemente",
                                                           "You lose your immunity against all elements", who);
                        EndIf
                      Else
                        Set_Critical(0);
                        If (who.connected)
                          PrintTextAbovePrivatePergon(who, "Ihr verliert Eure Resistenz gegen die Elemente",
                                                           "You lose your resistance against all elements", who);
                        EndIf
                      EndIf
          EndCase
          BuffIcons(who, event.spell, 0, 1);
          EraseObjProperty(who, "spell_resist");                 // remove spelltag
          If (!who.isA(POLCLASS_NPC))
            SendStatus(who); // Update Statfenster
          EndIf
        EndIf
      EndIf

    "spell_reflect":
      If (!server_restart)
        spelltag:= GetObjProperty(who, "spell_reflect");
        If ((spelltag) && (event.uniqueid==spelltag[3]))           // check Reflection spells
          If (spelltag[1] > 0)        // check If spell decayed
            PlaySoundEffect(who, SFX_F6);
            BuffIcons(who, event.spell, 0, 1);
            EraseObjProperty(who, "spell_reflect");               // remove spelltag
          EndIf
        EndIf
      EndIf

    "spell_bless":
      spelltag:= GetObjProperty(who, "spell_bless");
      If ((spelltag) && (event.uniqueid==spelltag[5]))           // check Bless Spells
        If (!server_restart) // Der Server hat den Wert schon zerstoert...
          AlterIntModPergon(who, -spelltag[2]);   // remove spelleffect
          AlterDexModPergon(who, -spelltag[2]);
          AlterStrModPergon(who, -spelltag[2]);
        EndIf
        If (spelltag[3] == "holy warrior")                    // check If blessing has been holy
          If (who.connected)
            PlaySoundEffect(who, SFX_F6);
            SendSysMessagePergon(who, "Der Wahn verlaesst Euch und Ihr werdet wieder normal",
                                      "The zeal leaves you and you become normal again");
          EndIf
          who.color:= spelltag[4];                        // restore original color
        EndIf
        If (spelltag[6]!="chaossphere")
          ClearCastPowerProperty(who,"Magie","Cleric","",0);
          ClearCastPowerProperty(who,"Magie","","Bless",0);
          ClearCastPowerProperty(who,"Magie","","Common",0);
        EndIf
        BuffIcons(who, event.spell, 0, 1);
        EraseObjProperty(who, "spell_bless");                 // remove spelltag
        RefreshSpellPowerGump(who);
        Return(1);
      EndIf

    "spell_curse":
      spelltag:= GetObjProperty(who, "spell_curse");
      If ((spelltag) && (event.uniqueid==spelltag[6]))           // check Curse Spells
        If (spelltag[3] == "curse")                                    // remove spelleffect
          If (!server_restart) // Der Server hat den Wert schon zerstoert...
            AlterIntModPergon(who, spelltag[2]);
            AlterDexModPergon(who, spelltag[2]);
            AlterStrModPergon(who, spelltag[2]);
          EndIf
          BuffIcons(who, event.spell, 0, 1);
          EraseObjProperty(who, "spell_curse");                 // remove spelltag
          ClearCastPowerProperty(who,"Magie","","",1);
          RefreshSpellPowerGump(who);
          Return(1);
        ElseIf (spelltag[3] == "dead knight")                 // check If curse has been deadly
          If (!server_restart)
            who.disable("invul");                                       // make spelltarget vulnerable again
            RevokePrivilege(who, "invul");
            IncRevision(who);
            If ((who.connected) and (!who.dead))
              SendSysMessagePergon(who, "Am Ende erliegt Ihr Eurer Sucht nach dem Tode",
                                        "In the end you succumb to death");
            EndIf
            who.color:= spelltag[4];                        // restore original color
            If (!who.dead)
               KillMobile(who, "necro-deadknight");                              // kill spelltarget
            EndIf
            BuffIcons(who, event.spell, 0, 1);
            EraseObjProperty(who, "spell_curse");                 // remove spelltag
          EndIf
        EndIf
      EndIf

    "spell_fog":
      If (!server_restart)
        spelltag:= GetObjProperty(who, "spell_fog");
        If ((spelltag) && (event.uniqueid==spelltag[3]))             // check "Fog"
          //PlaySoundEffect(who, SFX_SPELL_NIGHT_SIGHT);                   // remove spelleffect
          //PrintTextAbovePrivatePergon(who, "Der Nebel um Euch lichtet sich", "The fog around you cleares", who);
          who.hidden:=0;
          EraseObjProperty(who, "spell_fog");                   // remove spelltag
        EndIf
      EndIf

    "spell_blind":
      If (!server_restart)
        spelltag:= GetObjProperty(who, "spell_blind");
        If ((spelltag) && (event.uniqueid==spelltag[3]))           // check "Blind"
        	BuffIcons(who, event.spell, 0, 1);
          EraseObjProperty(who, "spell_blind");                 // remove spelltag
        EndIf                                         // (scripts/misc/blinded does the rest)
      EndIf

    "spell_paralyze":
      If (!server_restart)
        spelltag:= GetObjProperty(who, "spell_paralyze");
        If ((spelltag) && (event.uniqueid==spelltag[5]))         // check Paralyze spells
          who.paralyzed:= 0;                              // remove spelleffect
          If ((!GetObjProperty(who, "frozen"))&&(!GetObjProperty(who,"spell_statue"))) // unfreeze mobile,
            who.frozen:= 0;                             // only If no GM-freeze happened
          EndIf
          BuffIcons(who, event.spell, 0, 1);
          EraseObjProperty(who, "spell_paralyze");              // remove spelltag
          IncRevision(who);
//          If (spelltag[2] == "field")                       // check If mobile has been paralyzed
//                                                    // by magic field
//            var free:= 1;
//            SleepMS(50);                                // give mobile a chance to break free
//
//            foreach item in ListItemsAtLocation(who.x, who.y, who.z,who.realm)    // check If mobile still stands on field
//              If ((item.objtype == UOBJ_PARAFIELD_NS) or (item.objtype == UOBJ_PARAFIELD_EW))
//                free:= 0;
//                break;
//              EndIf
//            endforeach
//
//            If (free)                                 // check If mobile is free
//              EraseObjProperty(who, "spell_paralyze");           // remove spelltag
//            Else
//              var dur;                                 // get duration of paralyze according
//                                                // to mobiles magic resistance
//              If (spelltag[4])
//                dur:= CInt(MagicResistance(who, 90, spelltag[4], spelltag[3], 1));
//              Else
//                dur:= spelltag[3];
//              EndIf
//              spelltag[1]:= ReadGameClock() + dur;             // update spelltag
//              spelltag[5]:= ReadMilliSecondClock();
//              PlayObjectCenteredEffect(who, FX_CURSE_EFFECT, 10, 5);
//              SetObjProperty(who, "spell_paralyze", spelltag);
//              who.paralyzed:= 1;
//              who.frozen:= 1;
//              IncRevision(who);
//              SendSpellWaiter(who, "spell_paralyze", dur, spelltag[5], who);
//            EndIf
//          Else
//            EraseObjProperty(who, "spell_paralyze");              // remove spelltag
//          EndIf
        EndIf
      EndIf

    "spell_statue":
      If (!server_restart)
        spelltag:= GetObjProperty(who, "spell_statue");
        If ((spelltag) && (event.uniqueid==spelltag[5]))           // check "Statue"
          who.disable("invul");                           // remove spelleffect
          RevokePrivilege(who, "invul");
          If (!GetObjProperty(who, "frozen"))                   // unfreeze mobile,
            who.frozen:= 0;                             // only If no GM-freeze happened
          EndIf
          IncRevision(who);

          If (who.name["Eine Statue de"])  // Falls nen Incogspell vorher aufhört steht da schon der richtige Name
            who.name:= spelltag[3];                           // reset spelltargets name
          EndIf
          who.color:= spelltag[4];                          // reset spelltargets color

          If (who.connected)
            PlaySoundEffect(who, SFX_22);
            PrintTextAbovePrivatePergon(who, "Ihr spuert, dass Ihr Euch wieder bewegen koennt",
                                             "You feel yourself being able to move again", who);
          EndIf
          BuffIcons(who, event.spell, 0, 1);
          EraseObjProperty(who, "spell_statue");                  // remove spelltag
        EndIf
      Endif

    "spell_spiritspeak":
      If (!server_restart)
        spelltag:= GetObjProperty(who, "spell_spiritspeak");
        If ((spelltag) && (event.uniqueid==spelltag[2]))       // check "Spirit Speak"
          who.disable("hearghosts");                          // remove spelleffect
          who.disable("seeghosts");
          RevokePrivilege(who, "hearghosts");
          RevokePrivilege(who, "seeghosts");
          If (who.connected)
            PlaySoundEffectPrivate(who, SFX_SKILL_SPIRITSPEAK, who);
            PrintTextAbovePrivatePergon(who, "Die Stimmen der Geister verstummen wieder",
                                             "The ghosts' voices fall silent again", who);
          EndIf
          BuffIcons(who, event.spell, 0, 1);
          EraseObjProperty(who, "spell_spiritspeak");             // remove spelltag
        EndIf
      EndIf

    "spell_soulseek":
      If (!server_restart)
        spelltag:= GetObjProperty(who, "spell_soulseek");
        If ((spelltag) && (event.uniqueid==spelltag[2]))         // check "Soul Seek"
          who.disable("seeghosts");                         // remove spelleffect
          RevokePrivilege(who, "seeghosts");
          If (who.connected)
            PlaySoundEffectPrivate(who, SFX_SKILL_SPIRITSPEAK, who);
            PrintTextAbovePrivatePergon(who, "Ihr koennt die Seelen nicht mehr erkennen",
                                             "You cannot sense the souls anymore", who);
          EndIf
          BuffIcons(who, event.spell, 0, 1);
          EraseObjProperty(who, "spell_soulseek");                // remove spelltag
        EndIf
      EndIf

    "spell_doomed":
      If (!server_restart)
        spelltag:= GetObjProperty(who, "spell_doomed");
        If ((spelltag) && (event.uniqueid==spelltag[3]))           // check "Restless Soul"
          If (who.connected)
            PrintTextAbovePrivatePergon(who, "Die Verdammnis wurde von Euch genommen und Ihr seid erloest",
                                             "The damnation has been taken from you and you are released", who);
          EndIf
          BuffIcons(who, event.spell, 0, 1);
          EraseObjProperty(who, "spell_doomed");                  // remove spelltag
        EndIf
      EndIf

    "spell_rush":
      spelltag:= GetObjProperty(who, "spell_rush");
      If ((spelltag) && (event.uniqueid==spelltag[4]))             // check "Blood Rush"
        If (!server_restart) // Der Server hat den Wert schon zerstoert...
          who.ar_mod:= who.ar_mod-spelltag[2];                  // remove spelleffect
        EndIf
        EraseObjProperty(who, "spell_rush");                  // remove spelltag
        Return(1);
      EndIf
      
    "spell_slow":
      If (!server_restart)
        spelltag:=GetObjProperty(who,"spell_slow");
        If ((spelltag) && (event.uniqueid==spelltag[3]))
          If (who.isa(POLCLASS_NPC))
            //Nur NPCs müssen zurückgesetzt werden -> bei Playern ist der Spruch nur Timeout
            who.run_speed:=spelltag[2];
          EndIf
          BuffIcons(who, event.spell, 0, 1);
          EraseObjProperty(who,"spell_slow");
        EndIf
      EndIf
      
    "spell_speed":
      If (!server_restart)
        spelltag:=GetObjProperty(who,"spell_speed");
        If ((spelltag) && (event.uniqueid==spelltag[3]))
          If (who.isa(POLCLASS_NPC))
            //Nur NPCs müssen zurückgesetzt werden -> bei Playern ist der Spruch nur Timeout
            who.run_speed:=spelltag[2];
          EndIf
          BuffIcons(who, event.spell, 0, 1);
          EraseObjProperty(who,"spell_speed");
        EndIf
      EndIf
      
      //////////
    // Alchemie //  
      //////////
    "potion_staerkend":
      spelltag:=GetObjProperty(who, "potion_staerkend");
      If (spelltag &&(event.uniqueid==spelltag[3]))
        If (!server_restart)
          AlterStrModPergon(who, -spelltag[1]);   // remove spelleffect
        EndIf
        BuffIcons(who, event.spell, 0, 1);
        EraseObjProperty(who,"potion_staerkend");
        Return 1;
      EndIf

    "potion_beweglich":
      spelltag:=GetObjProperty(who, "potion_beweglich");
      If (spelltag &&(event.uniqueid==spelltag[3]))
        If (!server_restart)
          AlterDexModPergon(who, -spelltag[1]);   // remove spelleffect
        EndIf
        BuffIcons(who, event.spell, 0, 1);
        EraseObjProperty(who,"potion_beweglich");
        Return 1;
      EndIf
      
    "potion_erhellend":
      spelltag:=GetObjProperty(who, "potion_erhellend");
      If (spelltag &&(event.uniqueid==spelltag[3]))
        If (!server_restart)
          AlterIntModPergon(who, -spelltag[1]);   // remove spelleffect
        EndIf
        ClearCastPowerProperty(who,"Alchemie","","",0);
        RefreshSpellPowerGump(who);
        BuffIcons(who, event.spell, 0, 1);
        EraseObjProperty(who,"potion_erhellend");
        Return 1;
      EndIf

    "potion_feuerresistent":
      If (!server_restart)
        spelltag:=GetObjProperty(who,"potion_feuerresistent");
        If (spelltag && (event.uniqueid==spelltag[3]))
          Set_Critical(1);
          who.setprop("resist_fire",CInt(who.getprop("resist_fire")-spelltag[1]));
          Set_Critical(0);
          BuffIcons(who, event.spell, 0, 1);
          EraseObjProperty(who,"potion_feuerresistent");
          ClearCastPowerProperty(who,"Alchemie","","Fire",0);
          RefreshSpellPowerGump(who);
          If (!who.isA(POLCLASS_NPC))
            SendStatus(who); // Update Statfenster
          EndIf
        EndIf
      EndIf
      
    "potion_erdresistent":
      If (!server_restart)
        spelltag:=GetObjProperty(who,"potion_erdresistent");
        If (spelltag && (event.uniqueid==spelltag[3]))
          Set_Critical(1);
          who.setprop("resist_earth",CInt(who.getprop("resist_earth")-spelltag[1]));
          Set_Critical(0);
          BuffIcons(who, event.spell, 0, 1);
          EraseObjProperty(who,"potion_erdresistent");
          ClearCastPowerProperty(who,"Alchemie","","Earth",0);
          RefreshSpellPowerGump(who);
          If (!who.isA(POLCLASS_NPC))
            SendStatus(who); // Update Statfenster
          EndIf
        EndIf
      EndIf
      
    "potion_wasserresistent":
      If (!server_restart)
        spelltag:=GetObjProperty(who,"potion_wasserresistent");
        If (spelltag && (event.uniqueid==spelltag[3]))
          Set_Critical(1);
          who.setprop("resist_water",CInt(who.getprop("resist_water")-spelltag[1]));
          Set_Critical(0);
          BuffIcons(who, event.spell, 0, 1);
          EraseObjProperty(who,"potion_wasserresistent");
          ClearCastPowerProperty(who,"Alchemie","","Water",0);
          RefreshSpellPowerGump(who);
          If (!who.isA(POLCLASS_NPC))
            SendStatus(who); // Update Statfenster
          EndIf
        EndIf
      EndIf
      
    "potion_luftresistent":
      If (!server_restart)
        spelltag:=GetObjProperty(who,"potion_luftresistent");
        If (spelltag && (event.uniqueid==spelltag[3]))
          Set_Critical(1);
          who.setprop("resist_air",CInt(who.getprop("resist_air")-spelltag[1]));
          Set_Critical(0);
          BuffIcons(who, event.spell, 0, 1);
          EraseObjProperty(who,"potion_luftresistent");
          ClearCastPowerProperty(who,"Alchemie","","Air",0);
          RefreshSpellPowerGump(who);
          If (!who.isA(POLCLASS_NPC))
            SendStatus(who); // Update Statfenster
          EndIf
        EndIf
      EndIf
    
    "potion_giftresistent":
      If (!server_restart)
        spelltag:=GetObjProperty(who,"potion_giftresistent");
        If (spelltag && (event.uniqueid==spelltag[3]))
          Set_Critical(1);
          who.setprop("resist_poison",CInt(who.getprop("resist_poison")-spelltag[1]));
          Set_Critical(0);
          BuffIcons(who, event.spell, 0, 1);
          EraseObjProperty(who,"potion_giftresistent");
          ClearCastPowerProperty(who,"Alchemie","","Poison",0);
          RefreshSpellPowerGump(who);
          If (!who.isA(POLCLASS_NPC))
            SendStatus(who); // Update Statfenster
          EndIf
        EndIf
      EndIf
      
    "potion_versteckend":
      If (!server_restart)
        spelltag:=GetObjProperty(who,"potion_versteckend");
        If (spelltag && (event.uniqueid==spelltag[3]))
          If (who.hidden)
            who.hidden:=0;
          EndIf
          EraseObjProperty(who,"potion_versteckend");
          EraseObjProperty(who,"hiding_diff");
        EndIf
      EndIf
      
      
      //////////
    // Fische   //  
      //////////
      
    "fish_cunning":
      spelltag:= GetObjProperty(who, "fish_cunning");
      If ((spelltag) && (event.uniqueid==spelltag[3]))           // check "Cunning"
        If (!server_restart) // Der Server hat den Wert schon zerstoert...
          AlterIntModPergon(who, -spelltag[1]);   // remove spelleffect
        EndIf
        ClearCastPowerProperty(who,"Fische","","",0);
        RefreshSpellPowerGump(who);
        BuffIcons(who, event.spell, 0, 1);
        EraseObjProperty(who, "fish_cunning");               // remove spelltag
        Return(1);
      EndIf

    "fish_agility":
      spelltag:= GetObjProperty(who, "fish_agility");
      If ((spelltag) && (event.uniqueid==spelltag[3]))           // check "Agility"
        If (!server_restart) // Der Server hat den Wert schon zerstoert...
          AlterDexModPergon(who, -spelltag[1]);   // remove spelleffect
        EndIf
        BuffIcons(who, event.spell, 0, 1);
        EraseObjProperty(who, "fish_agility");               // remove spelltag
        Return(1);
      EndIf

    "fish_strength":
      spelltag:= GetObjProperty(who, "fish_strength");
      If ((spelltag) && (event.uniqueid==spelltag[3]))         // check "Strength"
        If (!server_restart) // Der Server hat den Wert schon zerstoert...
          AlterStrModPergon(who, -spelltag[1]);   // remove spelleffect
        EndIf
        BuffIcons(who, event.spell, 0, 1);
        EraseObjProperty(who, "fish_strength");                // remove spelltag
        Return(1);
      EndIf
      
      //////////
    //   Misc   //  
      //////////
      
    "drinking_int_malus":
      spelltag:= GetObjProperty(who, "drinking_int_malus");
      If ((spelltag) && (event.uniqueid==spelltag[3]))           // check ...
        If (!server_restart) // Der Server hat den Wert schon zerstoert...
          AlterIntModPergon(who, spelltag[2]);   // remove effect
        EndIf
        EraseObjProperty(who, "drinking_int_malus");                  // remove tag
        Return(1);
      EndIf
      
    AB_CPROP: //"WeaponAbility"
      spelltag:= GetObjProperty(who, AB_CPROP);
      If ((spelltag) && (event.uniqueid==spelltag[2]))           // check ...
        Case (spelltag[1])
          AB_DEFENSE_MASTERY:
            If (!server_restart)
              who.ar_mod:=who.ar_mod-spelltag[3];
            EndIf
            If (who.connected)
              SendSysMessagePergon(who,struct{uc_text:=CAscz("Ihr hört auf euch zu Verteidigen!"),lang:="DEU"},
                                       struct{uc_text:=CAscz("You stop to defend!"),lang:="ENU"});
            EndIf
          AB_FEINT:
            If (who.connected)
              SendSysMessagePergon(who,struct{uc_text:=CAscz("Ihr hört auf euch auf einen Feind zu konzentrieren!"),lang:="DEU"},
                                       struct{uc_text:=CAscz("You stop to concentrate on one enemy!"),lang:="ENU"});
            EndIf
          AB_CONFIDENCE:
            If (who.connected)
              SendSysMessagePergon(who,struct{uc_text:=CAscz("Der Rausch verfliegt!"),lang:="DEU"},
                                       struct{uc_text:=CAscz("The flush vanish!"),lang:="ENU"});
            EndIf
          AB_ENEMY_OF_ONE:
            If (who.connected)
              SendSysMessagePergon(who,struct{uc_text:=CAscz("Ihr hört auf euch auf einen Feind zu konzentrieren!"),lang:="DEU"},
                                       struct{uc_text:=CAscz("Yourstop to concentrate on one enemy!"),lang:="ENU"});
            EndIf
          AB_BLADEWEAVE:
            If (!server_restart)
              who.hitchance_mod:=who.hitchance_mod-spelltag[3];
            EndIf
            If (who.connected)
              SendSysMessagePergon(who,struct{uc_text:=CAscz("Ihr verliehrt eure Konzentration für eure Waffe!"),lang:="DEU"},
                                       struct{uc_text:=CAscz("You loose your concentration on your weapon!"),lang:="ENU"});
            EndIf
          AB_MORTAL_STRIKE:
            If (!server_restart)
              who.hitchance_mod:=who.hitchance_mod-spelltag[3];
            EndIf
            If (who.connected)
              SendSysMessagePergon(who,struct{uc_text:=CAscz("Ihr verliehrt eure Konzentration für eure Waffe!"),lang:="DEU"},
                                       struct{uc_text:=CAscz("You loose your concentration on your weapon!"),lang:="ENU"});
            EndIf
          AB_EVASION:
            If (!server_restart)
              who.evasionchance_mod:=who.evasionchance_mod-spelltag[3];
            EndIf
            If (who.connected)
              SendSysMessagePergon(who,struct{uc_text:=CAscz("Ihr fühlt nicht länger als ob ihr jeder Attacke ausweichen könntet!"),lang:="DEU"},
                                       struct{uc_text:=CAscz("You no longer feel that you could evade any attack."),lang:="ENU"});
            EndIf
          AB_MOVING_SHOT:
            RevokePrivilege(who,"firewhilemoving");
            If (who.connected)
              SendSysMessagePergon(who,struct{uc_text:=CAscz("Ihr verliehrt eure Konzentration für eure Waffe!"),lang:="DEU"},
                                       struct{uc_text:=CAscz("You loose your concentration on your weapon!"),lang:="ENU"});
            EndIf
          AB_SURPRISE_ATTACK:
            RevokePrivilege(who,"hiddenattack");
            If (who.connected)
              SendSysMessagePergon(who,struct{uc_text:=CAscz("Ihr verliehrt eure Konzentration!"),lang:="DEU"},
                                       struct{uc_text:=CAscz("You loose your concentration!"),lang:="ENU"});
            EndIf
          default:
            // nur cprop löschen
            If (who.connected)
              SendSysMessagePergon(who,struct{uc_text:=CAscz("Ihr verliehrt eure Konzentration!"),lang:="DEU"},
                                       struct{uc_text:=CAscz("You loose your concentration!"),lang:="ENU"});
            EndIf
        EndCase
        EraseObjProperty(who, AB_CPROP);                  // remove tag
        Return(1);  // Überleben alle keinen Restart, da zukleine Zeit
      EndIf
    "ab_psychic_attack": // Castmalus
      spelltag:= GetObjProperty(who, "ab_psychic_attack");
      If ((spelltag) && (event.uniqueid==spelltag[2]))           // check ...
        ClearCastPowerProperty(who,"Magie","","",1);
        RefreshSpellPowerGump(who);
        EraseObjProperty(who, "ab_psychic_attack");
        Return(1); // Überlebt Restart nicht da zu kurz
      EndIf
    AB_ARMOD:
      spelltag:= GetObjProperty(who, AB_ARMOD);
      If ((spelltag) && (event.uniqueid==spelltag[2]))           // check ...
        If (!server_restart)
          who.ar_mod:=who.ar_mod+spelltag[1];
        EndIf
        EraseObjProperty(who, AB_ARMOD);
        Return(1);
      EndIf 
      
  EndCase
EndFunction

Function CheckWeaponSpell(byref weapon, byref event, server_restart:=0)
  var who:=SystemFindObjectBySerial(event.whoserial,SYSFIND_SEARCH_OFFLINE_MOBILES);
  var spelltag;

  Case (event.spell)
    "spell_damage":
      If (!server_restart)
        spelltag:= GetObjProperty(weapon, "spell_damage");
        If ((spelltag) && (event.uniqueid==spelltag[4]))         // check Elemental Damage Spells
          Case (spelltag[2])                              // remove spelleffect
            "fire":  Set_Critical(1);
                     SetObjProperty(weapon, "damage_fire", CInt(GetObjProperty(weapon, "damage_fire")) - spelltag[3]);
                     Set_Critical(0);
                     If (who.connected)
                       PlaySoundEffect(who, SFX_48);
                       PrintTextAbovePrivatePergon(who, "Die Flammen aus Eurer Waffe erloeschen wieder",
                                                        "The flames from your weapon expire", who);
                     EndIf
                                                       
            "water": Set_Critical(1);
                     SetObjProperty(weapon, "damage_water", CInt(GetObjProperty(weapon, "damage_water")) - spelltag[3]);
                     Set_Critical(0);
                     If (who.connected)
                       PlaySoundEffect(who, SFX_1D6);
                       PrintTextAbovePrivatePergon(who, "Eure Waffe taut wieder auf",
                                                        "Your weapon thaws", who);
                     EndIf
            "air":   Set_Critical(1);
                     SetObjProperty(weapon, "damage_air", CInt(GetObjProperty(weapon, "damage_air")) - spelltag[3]);
                     Set_Critical(0);
                     If (who.connected)
                       PlaySoundEffect(who, SFX_1CA);
                       PrintTextAbovePrivatePergon(who, "Die Blitze aus Eurer Waffe verschwinden wieder",
                                                        "The lightning bolts from your weapon expire", who);
                     EndIf
            "earth": Set_Critical(1);
                     SetObjProperty(weapon, "damage_earth", CInt(GetObjProperty(weapon, "damage_earth")) - spelltag[3]);
                     Set_Critical(0);
                     If (who.connected)
                       PlaySoundEffect(who, SFX_42);
                       PrintTextAbovePrivatePergon(who, "Eure Waffe verliert ihren steinernen Mantel",
                                                        "Your weapon loses its stony cover", who);
                     EndIf
          EndCase
          EraseObjProperty(weapon, "spell_damage");                // remove spelltag
          IncRevision(weapon);
        EndIf
      EndIf

    "spell_bless":
       If (!server_restart)
         spelltag:= GetObjProperty(weapon, event.spell);
         If ((spelltag) && (event.uniqueid==spelltag[5]))          // check Bless spells on Weapon
           If (spelltag[3] == "holy warrior")                   // check If blessing has been holy
             weapon.color:= spelltag[4];                      // restore original color
           EndIf
           EraseObjProperty(weapon, "spell_bless");               // remove spelltag
           CalcItemQualityBonus(weapon);                        // calcs new item bonus
           IncRevision(weapon);
         EndIf
        EndIf

    "spell_curse":
      If (!server_restart)
        spelltag:= GetObjProperty(weapon, "spell_curse");
        If ((spelltag) && (event.uniqueid==spelltag[6]))         // check Curse spells on Weapon
          If ((spelltag[1] > 0) || (server_restart))
            If (spelltag[3] == "dead knight")                    // check If curse has been deadly
              weapon.color:= spelltag[4];                      // restore original color
              EraseObjProperty(weapon, "spell_curse");             // remove spelltag
              CalcItemQualityBonus(weapon);                      // calcs new item bonus
            Else
              EraseObjProperty(weapon, "spell_curse");             // remove spelltag only
            EndIf
            IncRevision(weapon);
          EndIf
        EndIf
      EndIf

    "spell_statue":
      If (!server_restart)
        spelltag:=GetObjProperty(weapon, "spell_statue");        // reset equipments color
        If ((spelltag) && (event.uniqueid==spelltag[5]))
          weapon.color:=spelltag[4];
          EraseObjProperty(weapon, "spell_statue");
          weapon.movable:= 1;                            // make equipment movable
        EndIf
      Endif
      
    "spell_rush":
      If (!server_restart)
        spelltag:= GetObjProperty(weapon, "spell_rush");
        If ((spelltag) && (event.uniqueid==spelltag[4]))             // check "Blood Rush"
          weapon.dmg_mod:= weapon.dmg_mod-spelltag[3];                  // remove spelleffect
          EraseObjProperty(weapon, "spell_rush");                  // remove spelltag
        EndIf
      EndIf

  EndCase
EndFunction

Function CheckArmorSpell(byref item, byref event, server_restart:=0)
//  var who:=SystemFindObjectBySerial(event.whoserial);
  var spelltag;

  Case (event.spell)
    "spell_bless":
      If (!server_restart)
        spelltag:= GetObjProperty(item, "spell_bless");
        If ((spelltag) && (event.uniqueid==spelltag[5]))           // check Bless spells on Armor
          If (spelltag[3] == "holy warrior")                    // check If blessing has been holy
            item.color:= spelltag[4];                       // restore original color
          EndIf
          EraseObjProperty(item, "spell_bless");                  // remove spelltag
          CalcItemQualityBonus(item);                       // calcs new item bonus
          IncRevision(item);
        EndIf
      EndIf

    "spell_curse":
      If (!server_restart)
        spelltag:= GetObjProperty(item, "spell_curse");
        If ((spelltag) && (event.uniqueid==spelltag[6]))           // check Curse spells on Armor
          If (spelltag[1] > 0)        // check If spell decayed
            If (spelltag[3] == "dead knight")                   // check If curse has been deadly
              item.color:= spelltag[4];                       // restore original color
            EndIf
            EraseObjProperty(item, "spell_curse");                  // remove spelltag
            IncRevision(item);
           EndIf
        EndIf
      EndIf

    "spell_statue":
      If (!server_restart)
        spelltag:=GetObjProperty(item, "spell_statue");        // reset equipments color
        If ((spelltag) && (event.uniqueid==spelltag[5]))
          item.color:=spelltag[4];
          EraseObjProperty(item, "spell_statue");
          item.movable:= 1;                            // make equipment movable
        EndIf
      Endif

  EndCase
EndFunction

///////////////////////////////////////////////////////////
// HandleModifyStruct - Zentrale Anlaufstelle für Spelländerungen
//                      um parallele Aufrufe zu syncronisieren
///////////////////////////////////////////////////////////

Function HandleModifyStruct(modifystruct)
  var forwho:=modifystruct.spelltarget;
  If (!forwho)
    Return(0);
  EndIf
  If (forwho.getprop(modifystruct.cprop.name)) // hat er den Spruch schon?
    Return(0);
  EndIf
  ForEach todo in (modifystruct.keys())
    Case (Lower(todo)) // Zur Sicherheit mal lower machen
      "spelltarget": // Wird oben verwendet
        break;
      "int":
        AlterIntModPergon(forwho, modifystruct[todo]);
      "str":
        AlterStrModPergon(forwho, modifystruct[todo]);
      "dex":
        AlterDexModPergon(forwho, modifystruct[todo]);
      "cprop":
        forwho.setprop(modifystruct[todo].name,modifystruct[todo].value);
      "castpower":
        Foreach power in (modifystruct[todo])
          SetCastPowerProperty(power[1],
                               power[2],
                               power[3],
                               power[4],
                               power[5],
                               power[6]);
        EndForEach
        RefreshSpellPowerGump(forwho);
      default:
        Break;
    EndCase
  EndForEach
  Return(1);
EndFunction


////////////////
//            //
//   Sender   //
//            //
////////////////

///////////////////////////////////////////////////////////
// DoSendEventWaiter - Schickt Event zum zentralen Script
///////////////////////////////////////////////////////////

Function SendSpellWaiter(object, spell, time, id, who,modifystruct:=0)
  var waiter:=GetProcess(GetGlobalProperty(PID_SPELLCHECK));
  While (!waiter)
    Sleep(5);
    waiter:=GetProcess(GetGlobalProperty(PID_SPELLCHECK));
  EndWhile
  var event:=struct;
  event.+time:=CInt(time+ReadGameClock());
  event.+whoserial:=who.serial;
  event.+spell:=spell;
  event.+serial:=object.serial;
  event.+uniqueid:=id;
  event.+modifystruct:=modifystruct;
	
  If ((object.getprop(SKKLASSE) != KLASSE_MAGIER) && (object.cmdlevel < CMDLEVEL_SEER))
	  time := 0;
  EndIf
  BuffIcons(object, spell, time);

  While ((waiter.sendevent(event)).errortext=="Event queue is full, discarding event")
    SleepMS(RandomInt(100)+2);

    If (DEBUG_EVENTS_SPELLCHECK)
      SysLog("FEHLER: SpellEventQueue voll (EventQueueSize = "+EVENTQUEUESIZE_SPELLCHECK+")!");
    EndIf
  EndWhile
EndFunction
