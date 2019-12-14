//**************************************************************************************************
// Include file thaumaturgy.inc
//
//    implements thaumaturgy functions for Pergon's magic system
//
//  InitItemSlots (item, except)              - inititalizes thaumaturgic rune slots in item
//  GetRuneDifficultyPergon (runeid, level)     - returns required skill for casting
//  CheckThaumaReagsPergon (who, runeid, level)   - checks if required reagents are available
//  ConsumeThaumaReagsPergon (who, runeid, level) - consumes reagents
//  RunePowerWordsPergon (who, runeid, level)     - utters powerwords according to level of rune
//  CanCastRune (who, runeid, level)            - checks if char can cast rune
//  CastRune (who, item, rune)                - casts given rune on given item
//  CalcItemQualityBonus (item)             - calcs quality bonus on item (weapon or armor)
//
//
//  2004/02/10 SeBB:      Ersterstellung
//
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
//  Rune types and their slot positions (according to item types):
//
//  [1]  - simple structure with 2 members
//        free      -> free rune slots available
//        max     -> maximum rune slots available (just in case of bugs)
//  [2]  - durability -> bonus ob items max HP                          (all items)
//  [3]  - stability  -> item takes less damage, when used                  (all items)
//  [4]  - quality      -> item has greater quality (summed up into ItemBonus)      (all items)
//  [5]  - fire       -> weapon has fire damage     (CProp attack_fire)       (weapons)
//                -> armor has fire resistance    (CProp resist_fire)       (armor)
//  [6]  - ice        -> weapon has water damage      (CProp attack_water)        (weapons)
//                  armor has water resistance    (CProp resist_water)        (armor)
//  [7]  - stone      -> weapon has earth damage      (CProp attack_earth)        (weapons)
//                  armor has earth resistance    (CProp resist_earth)        (armor)
//  [8]  - lightning  -> weapon has air damage      (CProp attack_air)        (weapons)
//                  armor has air resistance    (CProp resist_air)        (armor)
//  [9]  - poison     -> weapon has poison damage   (CProp attack_poison)     (weapons)
//                  armor has poison resistance (CProp resist_poison)     (armor)
//  [10] - speed      -> weapon has less attack delay                     (weapons)
//       - magic      -> armor has magic resistance   (CProp resist_magic)        (armor)
//  [11] - hit        -> weapon provides greater probability to strike hit        (weapons)
//       - life       -> armor provides more hit points                   (armor)
//       - block      -> shield provides greater probability of blocking          (shield)
//  [12] - pierce     -> weapon provides greater probability to strike piercing hit (weapons)
//       - stamina      -> armor provides more stamina                      (armor)
//       - revenge      -> shield reflects part of absorbed damage back at attacker   (shield)
//  [13] - crush      -> weapon provides greater probability to strike crushing hit (weapons)
//       - mana       -> armor provides more mana                       (armor)
//  [14] - vampire      -> weapon leeches hp according to damage                (weapons)
//       - strength -> armor provides more strength                     (armor)
//  [15] - mana       -> weapon leeches mana according to damage              (weapons)
//       - dexterity  -> armor provides more dexterity                      (armor)
//  [16] - spell      -> staff can be imbued with a specific spell              (staffs)
//       - intellect  -> armor provides more intelligence                   (armor)
//  [17] - magic      -> imbued spell on staff can be casted with more power      (staffs)
//
//
//  Slot entries consist of following members:
//
//    level       -> level of rune (can be 1, 2 or 3)
//    power       -> actual power of effect caused by rune (effect specific)
//    duration    -> gameclock-timestamp when rune will decay next time
//    decay       -> number of times rune has decayed -> modifies power
//
//**************************************************************************************************

use cfgfile;
use math;
use uo;

include "include/objtype";
include "include/modifyskill";

////////////////////////
// Konstanten (Runes) //
////////////////////////

Const MAX_RUNE_LEVEL      := 3;   // maximum rune level

Const THAUMA_WEAPON_SLOTS := 15;    // Constants describing number of runeslots on items
Const THAUMA_STAFF_SLOTS  := 17;    // note: these are alle possible slots - only a part of them
Const THAUMA_ARMOR_SLOTS  := 16;    //      is set at any moment -> position encodes rune type
Const THAUMA_SHIELD_SLOTS := 12;

Const RUNE_ALL_STATUS      := 1;    // state of runeslots (free/max)
Const RUNE_ALL_DURABILITY  := 2;    // durability rune for all items
Const RUNE_ALL_STABILITY   := 3;    // stability rune for all items
Const RUNE_ALL_QUALITY     := 4;    // quality rune for all items

Const RUNE_WEAPON_FIRE     := 5;    // fire attack rune for all weapons
Const RUNE_WEAPON_ICE      := 6;    // water attack rune for all weapons
Const RUNE_WEAPON_STONE    := 7;    // earth attack rune for all weapons
Const RUNE_WEAPON_LIGHTNING:= 8;    // air attack rune for all weapons
Const RUNE_WEAPON_POISON   := 9;    // poison attack rune for all weapons
Const RUNE_WEAPON_SPEED    := 10; // speed rune for all weapons
Const RUNE_WEAPON_HIT      := 11; // hit rune for all weapons
Const RUNE_WEAPON_PIERCE   := 12; // piercing hit rune for all weapons
Const RUNE_WEAPON_CRUSH    := 13; // crushing blow rune for all weapons
Const RUNE_WEAPON_VAMPIRE  := 14; // vampire rune for all weapons
Const RUNE_WEAPON_MANA     := 15; // mana rune for all weapons

Const RUNE_STAFF_SPELL     := 16; // spell rune for specific staff weapons
Const RUNE_STAFF_MAGIC     := 17; // magic boost rune for specific staff weapons

Const RUNE_ARMOR_FIRE      := 5;    // fire resistance rune for all armor
Const RUNE_ARMOR_WATER     := 6;    // water resistance rune for all armor
Const RUNE_ARMOR_EARTH     := 7;    // earth resistance rune for all armor
Const RUNE_ARMOR_AIR       := 8;    // air resistance rune for all armor
Const RUNE_ARMOR_POISON    := 9;    // poison resistance rune for all armor
Const RUNE_ARMOR_MAGIC     := 10; // magic resistance rune for all armor
Const RUNE_ARMOR_LIFE      := 11; // life donating rune for common armor
Const RUNE_ARMOR_STAMINA   := 12; // stamina donating rune for common armor
Const RUNE_ARMOR_ENERGY    := 13; // mana donating rune for common armor
Const RUNE_ARMOR_STRENGTH  := 14; // strength boosting rune for common armor
Const RUNE_ARMOR_DEXTERITY := 15; // dexterity boosting rune for common armor
Const RUNE_ARMOR_INTELLECT := 16; // intelligence boosting rune for common armor

Const RUNE_SHIELD_BLOCK    := 11; // blocking rune for shields
Const RUNE_SHIELD_REVENGE  := 12; // revenge rune for shields

/*******************
 * InitItemSlots() *
 *******************/

function InitItemSlots (item, except)

   Var itemdesc:= ReadConfigFile("::itemdesc");

   if (itemdesc)

      Var maxslots;

      if (item.isa(POLCLASS_ARMOR))             // check if if item is armor piece
         if (/*item is shield*/)                // check if item is shield
            maxslots:= THAUMA_SHIELD_SLOTS;
         else                                     // has to be common armor piece
            maxslots:= THAUMA_ARMOR_SLOTS;
         endif
      elseif (item.isa(POLCLASS_WEAPON))          // check if item is weapon
         if (/*item is staff*/)               // check if item is magic staff
            maxslots:= THAUMA_STAFF_SLOTS;
         else                           // has to be more common weapon
            maxslots:= THAUMA_WEAPON_SLOTS;
         endif
      else                              // nothing to initialize
         return 0;                        // return error
      endif

      Var slots:= itemdesc[item.objtype].runeslots; // read default slots from itemdesc
      Var runes:= {};
      Var frstslot:= struct;

      if (except)                         // check if item is exceptional
         slots+= 1;                       // increase runeslots by 1
         SetObjProperty(item, "status_except", except);   // remember except status for item bonus
      endif

      frstslot.+free:= slots;                 // initialize 1st slot
      frstslot.+max := slots;                 // (free and max available slots)
      runes[1]:= frstslot;

      Var ind;

      for (ind:= 2; ind <= maxslots; ind+= 1) // initialize itemslots
         runes[ind]:= 0;
      endfor

      SetObjProperty(item, "Runes", runes);       // tag item with runes
      return 1;                         // return success

   else
      SysLog("FEHLER: InitItemSlots(): itemdesc nicht gefunden!");    // throw error if no itemdesc
      return 0;                                     // return error
   endif

endfunction

/*****************************
 * GetRuneDifficultyPergon() *
 *****************************/

function GetRuneDifficultyPergon (runeid, level:= 1)

   Var runes:= ReadConfigFile("::runes");         // runes.cfg
   Var rune:= FindConfigElem(runes, runeid);        // rune for runeid from runes.cfg

   if (rune)
      level-= 1;
      return(CInt(GetConfigInt(rune, "Skill") * (1 + level * 0.25)));   // return runedifficulty
   else
      return 0;
   endif

endfunction

/****************************
 * CheckThaumaReagsPergon() *
 ****************************/

function CheckThaumaReagsPergon (who, runeid, level:= 1)

   Var runes:= ReadConfigFile("::runes");         // runes.cfg
   Var rune:= FindConfigElem(runes, runeid);        // rune for runeid from runes.cfg
   Var reagenteng;                        // english reagent name
   Var reagtype;                          // objtypes of reagent
   Var found;                           // number of reags found in backpack

   foreach reagent in GetConfigStringArray(rune, "PergonReag")    // search for reags in backpack and check if available
     reagtype:= {};                               // initialize reagtype array
     case (lower(reagent))                            // gather possible reagtypes into array
       "amethyst":    reagtype[1]:= UOBJ_AMETHYST_1;
                  reagtype[2]:= UOBJ_AMETHYST_2;
                  reagtype[3]:= UOBJ_AMETHYST_3;
                  reagtype[4]:= UOBJ_AMETHYST_4;
                  reagtype[5]:= UOBJ_AMETHYST_5;
                    reagenteng:= "Amethyst";
       "aquamarin":   reagtype[1]:= UOBJ_AQUAMARIN_1;
                  reagtype[2]:= UOBJ_AQUAMARIN_2;
                  reagtype[3]:= UOBJ_AQUAMARIN_3;
                    reagenteng:= "Aquamarine";
       "bernstein":   reagtype[1]:= UOBJ_BERNSTEIN;
                    reagenteng:= "Amber";
       "blackrock":   reagtype[1]:= UOBL_BLACKROCK;
                  reagenteng:= "Blackrock";
       "diamant":     reagtype[1]:= UOBJ_DIAMANT_1;
                  reagtype[2]:= UOBJ_DIAMANT_2;
                  reagtype[3]:= UOBJ_DIAMANT_3;
                  reagtype[4]:= UOBJ_DIAMANT_4;
                    reagenteng:= "Diamond";
       "feuerdiamant":  reagtype[1]:= UOBJ_FEUERDIAMANT;
                    reagenteng:= "Firediamond";
       "granat":      reagtype[1]:= UOBJ_GRANAT_1;
                  reagtype[2]:= UOBJ_GRANAT_2;
                  reagtype[3]:= UOBJ_GRANAT_3;
                  reagtype[4]:= UOBJ_GRANAT_4;
                    reagenteng:= "Garnet";
       "gold":        reagtype[1]:= UOBJ_GOLD_INGOT;
                reagenteng:= "Gold Ingot";
       "rubin":     reagtype[1]:= UOBJ_RUBIN_1;
                  reagtype[2]:= UOBJ_RUBIN_2;
                  reagtype[3]:= UOBJ_RUBIN_3;
                    reagenteng:= "Ruby";
       "saphir":      reagtype[1]:= UOBJ_SAPHIR_1;
                    reagtype[2]:= UOBJ_SAPHIR_2;
                    reagtype[3]:= UOBJ_SAPHIR_3;
                    reagenteng:= "Sapphire";
       "schaedel":    reagtype[1]:= UOBJ_SCHAEDEL;
                    reagenteng:= "Skull";
       "silber":      reagtype[1]:= UOBJ_SILVER_INGOT;
                  reagenteng:= "Silver Ingot";
       "smaragd":     reagtype[1]:= UOBJ_SMARAGD_1;
                  reagtype[2]:= UOBJ_SMARAGD_2;
                    reagenteng:= "Emerald";
       "turmalin":    reagtype[1]:= UOBJ_TURMALIN_1;
                    reagtype[2]:= UOBJ_TURMALIN_2;
                    reagtype[3]:= UOBJ_TURMALIN_3;
                    reagtype[4]:= UOBJ_TURMALIN_4;
                    reagenteng:= "Tourmaline";
       "zitrin":      reagtype[1]:= UOBJ_ZITRIN_1;
                    reagtype[2]:= UOBJ_ZITRIN_2;
                    reagtype[3]:= UOBJ_ZITRIN_3;
                    reagtype[4]:= UOBJ_ZITRIN_4;
                    reagenteng:= "Citrine";
     endcase

     found:= 0;
     foreach item in EnumerateItemsInContainer(who.backpack)    // search for reagents
        if (item.objtype in reagtype)                   // check all possible reagent object types
           found+= item.amount;                 // add amount
        endif
     endforeach

     if (found < level)                             // not enough reagents found
       SendSysMessagePergon(who, "Euch fehlt zumindest ein " + reagent + " zum Zaubern dieser Rune",
                        "You need at least one " + reagenteng + " to imbue this rune");
       PlaySoundEffect(who, SFX_2C);
       return 0;
     endif
   endforeach

   return 1;

endfunction

/******************************
 * ConsumeThaumaReagsPergon() *
 ******************************/

function ConsumeThaumaReagsPergon (who, runeid, level:= 1)

   Var runes:= ReadConfigFile("::runes");         // runes.cfg
   Var rune:= FindConfigElem(runes, runeid);        // rune for runeid from runes.cfg
   Var reagtype;                          // objtypes of reagent

   foreach reagent in GetConfigStringArray(rune, "PergonReag")    // search for reags in backpack and consume
     reagtype:= {};                               // initialize reagtype array
     case (lower(reagent))                            // gather possible reagtypes into array
       "amethyst":    reagtype[1]:= UOBJ_AMETHYST_1;
                  reagtype[2]:= UOBJ_AMETHYST_2;
                  reagtype[3]:= UOBJ_AMETHYST_3;
                  reagtype[4]:= UOBJ_AMETHYST_4;
                  reagtype[5]:= UOBJ_AMETHYST_5;
       "aquamarin":   reagtype[1]:= UOBJ_AQUAMARIN_1;
                  reagtype[2]:= UOBJ_AQUAMARIN_2;
                  reagtype[3]:= UOBJ_AQUAMARIN_3;
       "bernstein":   reagtype[1]:= UOBJ_BERNSTEIN;
       "blackrock":   reagtype[1]:= UOBL_BLACKROCK;
       "diamant":     reagtype[1]:= UOBJ_DIAMANT_1;
                  reagtype[2]:= UOBJ_DIAMANT_2;
                  reagtype[3]:= UOBJ_DIAMANT_3;
                  reagtype[4]:= UOBJ_DIAMANT_4;
       "feuerdiamant":  reagtype[1]:= UOBJ_FEUERDIAMANT;
       "granat":      reagtype[1]:= UOBJ_GRANAT_1;
                  reagtype[2]:= UOBJ_GRANAT_2;
                  reagtype[3]:= UOBJ_GRANAT_3;
                  reagtype[4]:= UOBJ_GRANAT_4;
       "gold":        reagtype[1]:= UOBJ_GOLD_INGOT;
       "rubin":     reagtype[1]:= UOBJ_RUBIN_1;
                  reagtype[2]:= UOBJ_RUBIN_2;
                  reagtype[3]:= UOBJ_RUBIN_3;
       "saphir":      reagtype[1]:= UOBJ_SAPHIR_1;
                    reagtype[2]:= UOBJ_SAPHIR_2;
                    reagtype[3]:= UOBJ_SAPHIR_3;
       "schaedel":    reagtype[1]:= UOBJ_SCHAEDEL;
       "silber":      reagtype[1]:= UOBJ_SILVER_INGOT;
       "smaragd":     reagtype[1]:= UOBJ_SMARAGD_1;
                  reagtype[2]:= UOBJ_SMARAGD_2;
       "turmalin":    reagtype[1]:= UOBJ_TURMALIN_1;
                    reagtype[2]:= UOBJ_TURMALIN_2;
                    reagtype[3]:= UOBJ_TURMALIN_3;
                    reagtype[4]:= UOBJ_TURMALIN_4;
       "zitrin":      reagtype[1]:= UOBJ_ZITRIN_1;
                    reagtype[2]:= UOBJ_ZITRIN_2;
                    reagtype[3]:= UOBJ_ZITRIN_3;
                    reagtype[4]:= UOBJ_ZITRIN_4;
     endcase

     foreach item in EnumerateItemsInContainer(who.backpack)    // search for reagent
        if (item.objtype in reagtype)                   // check all possible reagent object types
           if (item.amount < level)                     // check if stack is too small
              level-= item.amount;                // decrease demand by stacksize
              DestroyItem(item);                        // 'consume' whole stack
           else
              SubtractAmount(item, level);                // consume demanded number of reagents
              break;
           endif
        endif
     endforeach
   endforeach

   return 1;

endfunction

/**************************
 * RunePowerWordsPergon() *
 **************************/

function RunePowerWordsPergon (who, runeid, level:= 1)

   Var runes:= ReadConfigFile("::runes");         // load runes.cfg
   Var rune:= FindConfigElem(runes, runeid);        // look up rune
   Var words:= GetConfigString(rune, "PowerWords"); // get powerwords

   case (level)                                       // modify powerwords according
      1:  words:= "SanTou " + words;              // to rune level
      2:  words:= "NiTou " + words;
      3:  words:= "ItTou " + words;
   endcase

   PrintTextAbovePergon(who, words);                // utter words

   return 1;

endfunction

/*****************
 * CanCastRune() *
 *****************/

function CanCastRune (who, runeid, level:= 1)

   if (!RuneCastingRestrictions(who))
     return 0;                            // check if equipped item blocks rune casting
   endif

   if (GetSkillPergon(who, SKILLID_VERZAUBERN) + 20 < GetRuneDifficultyPergon(runeid, level))
      SendSysMessagePergon(who, "Ihr seid nicht begabt genug um diese Rune anzuwenden",
                                "You need more talent to use this rune");
      PlaySoundEffect(who, SFX_2C);
      return 0;                         // check if char has enough skill
   endif

   if (!CheckThaumaReagsPergon(who, runeid, level))
      return 0;                         // check if enough reags available
   endif

   return 1;

endfunction

/**************
 * CastRune() *
 **************/

Function CastRune (who, item, rune)

  Var runeid:=  rune.RuneId;                  // get runeid of rune
  Var runes:=   ReadConfigFile("::runes");        // load runes.cfg
  Var runecfg:= FindConfigElem(runes, runeid);      // look up rune config
  Var slot:= runecfg.propslot;                // get associated rune slot in itemprop
  Var skill:= runecfg.skill;                  // get basic rune skill
  Var oldrune;                            // rune, already casted on item
  Var level:= 1;                          // level of rune - determined by existing runes
  Var decay:= 0;                          // decay of rune - determined by existing runes

  if (item.layer)                         // check if item is equipped
     SendSysMessagePergon(who, "Ihr koennt keine Runen auf Gegenstaende zaubern, die gerade benutzt werden",
                               "You cannot imbue items currently in use");
     PlaySoundEffect(who, SFX_2C);
     return 0;
  endif

  Var itemrunes:= GetObjProperty(item, "Runes");    // get existing runes from item

  if (!itemrunes)                         // no runes on item / item is not initialized
    InitItemSlots(item);                    // initialize item
  else
     freeslots:= itemrunes[RUNE_ALL_STATUS].free;
     if (freeslots > 0)                     // check if free runeslots are available
        SendSysMessagePergon(who, "Auf " + item.desc + " sind keine weiteren freien Runensockel zu finden",
                                  "There are no more free rune sockets on " + item.desc);
        PlaySoundEffect(who, SFX_1EB);
        return 0;
     endif

     oldrune:= itemrunes[slot];               // get runeslot entry of rune
     if (oldrune)                         // check if rune has been casted before
        if (oldrune.level >= MAX_RUNE_LEVEL)        // check if old rune is at maximum level
           SendSysMessagePergon(who, "Ihr koennt diese Rune nicht noch einmal auf " + item.desc + " zaubern",
                               "You cannot imbue " + item.desc + " with this rune anymore");
           PlaySoundEffect(who, SFX_1EB);
           return 0;
        endif

        level:= CInt(oldrune.level + 1);          // increase needed rune level
        decay:= CInt(oldrune.decay);            // backup rune decay
     endif
  endif

  If (PreCastRune(who, rune, runeid))

     Var diff:= GetRuneDifficultyPergon(runeid, level);         // get difficulty of rune at actual level

     if (CheckSkillPergon(who, SKILLID_VERZAUBERN, diff, 3*diff))   // check if caster is successful

        Var maxpower:= runecfg.effectmax;                                 // get rune effect limits
        Var minpower:= runecfg.effectmin;
        Var maxdur:=   runecfg.durationmax;                       // get rune duration limits
        Var mindur:=   runecfg.durationmin;
        Var power:=    CInt(runecfg.effect * GetIntPergon(who) / 100.0);    // get and alter rune effect
        Var dur:=      CInt(runecfg.duration * GetIntPergon(who) / 100.0);    // get and alter rune duration

        if (maxpower && maxpower < power)                   // apply effect limits
           power:= maxpower;
        elseif (minpower && minpower > power)
           power:= minpower;
        endif
        if (maxdur && maxdur < dur)                       // apply duration limits
           dur:= maxdur;
        elseif (mindur && mindur > dur)
           dur:= mindur;
        endif

        PlaySoundEffect(item, SFX_2B);
        PerformAction(who, ANIM_CAST_AREA);
        PlayObjectCenteredEffect(item, FX_BLESS_EFFECT, 5, 20);
        PlaySoundEffect(item, SFX_1F8);

        Var newrune:= struct;                   // new rune

        newrune.+level:=   level;             // save rune properties
        newrune.+power:=   power;
        newrune.+duration:= dur;
        newrune.+decay:=   decay

        itemrunes[slot]:= newrune;                    // apply rune
        itemrunes[RUNE_ALL_STATUS].free:= freeslots - 1;       // decrease free slots

        SetObjProperty(item, "Runes", itemrunes);         // tag item with runes

        return 1;
     endif

     PlaySoundEffect(who, SFX_2C);              // so we have fumbled, haven't we...

  EndIf

EndFunction

/*****************
 * PreCastRune() *
 *****************/

Function PreCastRune (who, rune, runeid)

  If (CanCastRune(who, runeid, level));         // check if caster is able to cast rune
    ConsumeThaumaReagsPergon(who, runeid, level);   // consume reagents
    RunePowerWordsPergon(who, runeid, level);     // utter powerwords
    SubtractAmount(rune, 1);                  // destroy rune
    Return (1);
  EndIf

EndFunction

/*****************************
 * RuneCastingRestrictions() *
 *****************************/

Function RuneCastingRestrictions (who)

  If (who.cmdlevel < 2)                 // Der SeBB is nur zu faul, sich vernueftig einzukleiden ;o) - jopp ;)
    If (PlaceNoCast(who))               // Ist das Zaubern in dieser Region erlaubt?
      SendSysMessagePergon(who, "Die Goetter unterbinden Runenzauberei in dieser Region!",
                                "The Gods prevent casting runes in this region!");
      PlaySoundEffect(who, SFX_2C);
      Return (0);
    EndIf

    Var itemdesccfg:= ReadConfigFile("::itemdesc"); // ItemDesc.cfg oeffnen
    Var item;

    If (itemdesccfg)

      // Gegenstand in der ersten Hand des Spielers ueberpruefen ("BlocksCastingIfInHand")
      item:= GetEquipmentByLayer(who, LAYER_HAND1);
      If (item)
        If (itemdesccfg[item.objtype].blockscastingifinhand <> 0)
          SendSysMessagePergon(who, "Macht Eure Haende frei zum Zaubern!", "Free your hands to cast!");
          PlaySoundEffect(who, SFX_2C);
          Return (0);
        EndIf
      EndIf

      // Gegenstand in der zweiten Hand des Spielers ueberpruefen ("BlocksCastingIfInHand")
      item:= GetEquipmentByLayer(who, LAYER_HAND2);
      If (item)
        If (itemdesccfg[item.objtype].blockscastingifinhand <> 0)
          SendSysMessagePergon(who, "Macht Eure Haende frei zum Zaubern!", "Free your hands to cast!");
          PlaySoundEffect(who, SFX_2C);
          Return (0);
        EndIf
      EndIf

      // Gegenstaende am Koerper des Spielers ueberpruefen ("BlockCircle")
      ForEach item in ListEquippedItems(who)
        blockcircle:= itemdesccfg[item.objtype].blockcircle;
        If (blockcircle)
          SendSysMessagePergon(who, "Eure Ausruestung verhindert das Zaubern!",
                                    "Your equipment prevents spellcasting!");
          PlaySoundEffect(who, SFX_2C);
          Return (0);
        EndIf
      EndForEach

    Else
      SysLog("FEHLER: Konnte die itemdesc.cfg nicht oeffnen!");
    EndIf
  EndIf

  Return (1);

EndFunction

/**************************
 * CalcItemQualityBonus() *
 **************************/

function CalcItemQualityBonus (item)
   // Var excepttag:= GetObjProperty(item, "status_except");        // get exceptional status
   Var runetag:=   GetObjProperty(item, "Runes")[RUNE_ALL_QUALITY]; // get quality rune
   Var blesstag:=  GetObjProperty(item, "spell_bless");         // get blessing spell
   Var cursetag:=  GetObjProperty(item, "spell_curse");              // get curse spell
   Var except:= 0;                                  // initialize boni
   Var rune:=   0;
   Var bless:=  0;
   Var curse:=  0;

   //if (excepttag)                         // check if item is exceptional
   //   except:= excepttag;                   // get exceptional bonus
   //endif
   if (runetag)                         // check if item has a quality rune
      rune:= runetag.power;                 // get rune bonus
   endif
   if (blesstag)                          // check if item has been blessed
      bless:= blesstag[2];                    // get bless bonus
   endif
   if (cursetag and cursetag[3] == "dead knight"      // check if item is a deadly cursed weapon
                and item.isa(POLCLASS_WEAPON))
      curse:= cursetag[5];                    // get curse bonus
   endif

   Var new:= sqrt(except*except + rune*rune + bless*bless);       // calc basic new bonus

   if (item.isa(POLCLASS_WEAPON))             // check if item is weapon
      item.dmg_mod:= CInt(new + curse);         // set weapon quality bonus
      return 1;                         // return success
   elseif (item.isa(POLCLASS_ARMOR))            // check if item is armor
                                // WTF?
      item.ar_mod:= CInt(item.ar * new / 100.0);    // set armor quality bonus
      return 1;                         // return success
   else                               // item cannot get quality bonus
      return 0;                         // return error
   endif

endfunction
