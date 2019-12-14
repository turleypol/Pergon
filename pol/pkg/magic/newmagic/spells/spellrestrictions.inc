//**************************************************************************************************
// Include file spellrestrictions.inc
//
//    implements magic related restrictions
//
//  CanCast (who, spellid)    - checks, if player can cast certain spell
//  TargetSpell (who, spellid)  - lets player target a mobile
//  TargetArea (who, spellid)   - lets player target an area (LOS checked)
//  TargetThing (who, spellid)  - lets player target something
//  obsolete functions    - shouldn't be needed in Pergon
//
//  2002/02/21 Shinigami: NoCast-Region eingebaut
//  2002/02/27 Shinigami: BlocksCastingIfInHand eingebaut und BlockCircle aktiviert
//  2005/03/09 Shinigami: Normale Spieler im Newbie-Area duerfen nur bis zum 4. Zirkel zaubern
//  2005/09/21 Seppel: Paralysiert oder gefreezed kann man nicht mehr zaubern
//  2005/12/18 Seppel: mit LastTarget-Makro kann man nicht mehr Unsichtbare treffen
//**************************************************************************************************

use uo;
use os;
use cfgfile;
use util;

Include "include/npc";
Include "include/quests";

/*************
 * CanCast() *
 *************/

Function CanCast(who, spellid)
  If (!GetObjProperty(who, I_AM_GOD)) // Der SeBB is nur zu faul, sich vernueftig einzukleiden ;o)
    If (PlaceNoCast(who) and QuestZoneIsIn(who) != 1) // Ist das Zaubern in dieser Region erlaubt? Wenn nicht, ist dort keine Quest?
      SendSysMessagePergon(who, "Die Goetter unterbinden in dieser Region Euren Zauber!",
                                "The Gods prevent casting this spell in this region!");
      PlaySoundEffect(who, SFX_2C);
      Return (0);
    EndIf

   // Die Prop wurde nie verwendet, also erstmal auskommentieren
   // If (!GetGlobalProperty("restrictarmor"))
   //   Return (1);
   // EndIf

    Var itemdesccfg:=ReadConfigFile("::itemdesc"); // ItemDesc.cfg oeffnen
    If (itemdesccfg)
      // Gegenstand in der ersten Hand des Spielers ueberpruefen ("BlocksCastingIfInHand")
      Var item:=GetEquipmentByLayer(who, LAYER_HAND1);
      If (item)
        If (itemdesccfg[item.objtype].blockscastingifinhand<>0)
          SendSysMessagePergon(who, "Macht Eure Haende frei zum Zaubern!", "Free your hands to cast!");
          PlaySoundEffect(who, SFX_2C);
          Return (0);
        EndIf
      EndIf

      // Gegenstand in der zweiten Hand des Spielers ueberpruefen ("BlocksCastingIfInHand")
      item:=GetEquipmentByLayer(who, LAYER_HAND2);
      If (item)
        If (itemdesccfg[item.objtype].blockscastingifinhand<>0)
          SendSysMessagePergon(who, "Macht Eure Haende frei zum Zaubern!", "Free your hands to cast!");
          PlaySoundEffect(who, SFX_2C);
          Return (0);
        EndIf
      EndIf

      // Paralyse/Freeze überprüfen
      If (who.paralyzed || who.frozen)
        SendSysMessagePergon(who, "Paralysiert koennt ihr nicht zaubern!", "You can`t cast when you are paralyzed!");
        PlaySoundEffect(who, SFX_2C);
        Return(0);
      EndIf

      Var spellscfg:=ReadConfigFile("::spells"); // Spells.cfg oeffnen
      If (spellscfg)
        Var circle:=spellscfg[spellid].circle;  // Circle zum Spell holen
        Var blockcircle;                        // Circle zum Item

        // Ein normaler Spieler im Newbie-Area oder Newbie/Alt am Goetterpalast, jedoch ausserhalb einer Arena?
        If (((GetObjProperty(who, PLACE_INNEWBIEAREA) And (!GetObjProperty(who, TYPNEWBIE))) Or
           GetObjProperty(who, PLACE_ATGODPALACE)) And (!GetObjProperty(who, ARENABATTLE)))
          If (circle>4)
            SendSysMessagePergon(who, "Die Goetter unterbinden in dieser Region Euren starken Zauber!");
            PlaySoundEffect(who, SFX_2C);
            Return (0);
          EndIf
        EndIf

        // Gegenstaende am Koerper des Spielers ueberpruefen ("BlockCircle")
        ForEach item in ListEquippedItems(who)
          blockcircle:=itemdesccfg[item.objtype].blockcircle;
          If (blockcircle)
            If (circle>=blockcircle)
              SendSysMessagePergon(who, "Eure Ausruestung verhindert das Zaubern!",
                                        "Your equipment prevents spellcasting!");
              PlaySoundEffect(who, SFX_2C);
              Return (0);
            EndIf
          EndIf
        EndForEach
      Else
        SysLog("FEHLER: Konnte die spells.cfg nicht oeffnen!");
      EndIf
    Else
      SysLog("FEHLER: Konnte die itemdesc.cfg nicht oeffnen!");
    EndIf
  EndIf

  Return (1);
EndFunction

/*****************
 * TargetSpell() *
 *****************/

function TargetSpell (who, TargOpt := TGTOPT_CHECK_LOS)

   Var cx:= who.x;
   Var cy:= who.y;

   Var aim:= Target(who, TargOpt);

   if (!aim.isa(POLCLASS_MOBILE))
      return;
   endif

   if ((cx != who.x) or (cy != who.y))
      SendSysMessagePergon(who, "Ihr verliert die zum Zaubern notwendige Konzentration",
                                "You lose the concentration required to cast spells");
      return 0;
   endif

   If (aim.hidden == 1)       // mit dem LastTarget Makro könnte man ansonsten Unsichtbare treffen
     SendSysMessagePergon(who, "Ihr koennt euer Ziel nicht sehen!", "You can't see your target");
     return 0;
   EndIf

   return aim;
endfunction

/****************
 * TargetArea() *
 ****************/

function TargetArea (who, TargOpt:= TGTOPT_CHECK_LOS)

   If (!(TargOpt in {0, TGTOPT_CHECK_LOS})) // Shinigami
     SysLog("FEHLER! TargetArea wurde mit unzulaessiger TargOpt="+TargOpt+" aufgerufen!");
   EndIf

   Var cx:= who.x;
   Var cy:= who.y;

   Var aim:= TargetCoordinates(who);

   if (!aim)
      return 0;
   endif

   if ((cx != who.x) or (cy != who.y))
      SendSysMessagePergon(who, "Ihr verliert die zum Zaubern notwendige Konzentration",
                                "You lose the concentration required to cast spells");
      return 0;
   endif

   if (TargOpt == TGTOPT_CHECK_LOS)
      if (!CheckLosAt(who, aim.x, aim.y, aim.z))
         SendSysMessagePergon(who, "Ihr koennt das nicht sehen", "You cannot see that");
         return 0;
      endif
   endif

   return aim;
endfunction

/*****************
 * TargetThing() *
 *****************/

function TargetThing (who, TargOpt:= TGTOPT_CHECK_LOS)

   Var cx:= who.x;
   Var cy:= who.y;

   Var aim:= Target(who, TargOpt);

   if (!aim)
      return;
   endif

   if ((cx != who.x) or (cy != who.y))
      SendSysMessagePergon(who, "Ihr verliert die zum Zaubern notwendige Konzentration",
                                "You lose the concentration required to cast spells");
      return 0;
   endif

   If (aim.hidden == 1)       // mit dem LastTarget Makro könnte man ansonsten Unsichtbare treffen
     SendSysMessagePergon(who, "Ihr koennt euer Ziel nicht sehen!", "You can't see your target");
     return 0;
   EndIf

   return aim;
endfunction

/*********************************
 * for Pergon obsolete functions *
 *********************************/

function Reflected(cast_on)

  var castonmr:= GetObjProperty(cast_on, "spell_reflect");
  if (castonmr)
    if (castonmr[2] == "single")            // target has only 'normal' magic reflect -> reset
       EraseObjProperty(cast_on, "spell_reflect");
    endif

    sleepms(500);
    PlaySoundEffect(cast_on, SFX_SPELL_MAGIC_REFLECTION);
    PlayObjectCenteredEffect( cast_on, FX_CURSE_EFFECT, 10,10);
    SendSysMessagePergon(cast_on, "Deine magische Resistenz hat dich bewahrt!");
    return 1;
  endif

  return 0;
endfunction

function Resisted(circle, caster, cast_on, amt)

  Var magery := Cint(GetSkillPergon(caster, SKILLID_MAGERY));
  Var evalint := Cint(GetSkillPergon(caster, SKILLID_EVALINT));
  Var halfamt := amt/2;
  Var rezamt := Cint(( (RandomInt(halfamt)+1) * evalint ) / 100);
  Var diff := (circle * 10);
  Var points := (diff * 10);

  if (CheckSkillPergon(cast_on,SKILLID_MAGICRESISTANCE, diff, points))
    SendSysMessagePergon(cast_on, "Du bist gegen magische Angriffe gut geruestet!");
    PlaySoundEffect(cast_on, SFX_SPELL_WEAKEN);
    PlayObjectCenteredEffect( cast_on, FX_CURSE_EFFECT, 5,5);
    if (evalint > 0)
      amt := rezamt + halfamt;
    else
      amt := halfamt;
    endif
    return amt;
  else
    return amt;
  endif
endfunction

function CalcSpellDamage(circle, caster, cast_on)

  Var die_string := circle+"d8";
  Var dmg := RandomDiceRoll( die_string ) ;
  dmg += Cint( GetSkillPergon(caster, SKILLID_MAGERY)/10);
  if ( dmg > (circle * 8) )
    dmg := circle * 8;
  endif

  return dmg;
endfunction
