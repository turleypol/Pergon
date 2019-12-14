/////////////////
// Bibliotheken
/////////////////
use math;
include "include/itemnpc";
include "include/mobile";
include "include/poison";

// Konstanten

// wenn mehr als MOBILES_MAX in MOBILES_RANGE um das Nest/
// schwangeres Tier herumstehen, gibt es keine neuen Tiere
Const MOBILES_RANGE := 5;
Const MOBILES_MAX   := 10;

///////////////////////////////////////////////////////////////////////////
// KeepDistanceToOpponent - Entferne mich/ Laufe zum ...
///////////////////////////////////////////////////////////////////////////
Function KeepDistanceToOpponent(me, opponent, mindist, maxdist, Debug) // {{{
  Var timeout:=ReadGameClock()+Distance(me, opponent)*2;
  Var abstand:=Distance(me, opponent);
  Var noaction:=1;
  While (opponent And (!opponent.hidden) And (!opponent.concealed) And (!Events_Waiting()) And
        (timeout>ReadGameClock()) And (noaction))
    noaction:=0;
    if ((abstand< mindist) and (mindist))
      TurnAwayFrom( opponent );
      WalkAwayFrom(opponent);
      SleepMS(100);
      If (Debug)
        SysLogFile(me.name+" ["+Lower(Hex(me.serial))+"] ich will wegrennen von "+opponent.name);
      EndIf
      noaction:=1;
    endif

    if (( abstand > maxdist ) and (maxdist))
      Walktoward(opponent);
      SleepMS(100);
      If (Debug)
        SysLogFile(me.name+" ["+Lower(Hex(me.serial))+"] ich bin zu weit weg von "+opponent.name);
      EndIf
      noaction:=1;
    endif

    abstand:=Distance(me, opponent);
  EndWhile
EndFunction // }}}

///////////////////////////////////////////////////////////////////////////
// CheckDrinkingHungryLevelAnimal - Ueberprueft das DrinkHungryLevel bei Tieren
///////////////////////////////////////////////////////////////////////////
Function CheckDrinkingHungryLevelAnimal(chr, level_drink) // {{{

  Case (CInt(level_drink))
    1:  KillMobile(chr, "animal-hungrydrinking");
    2:  ApplyRawDamagePergon(chr, 64);
    3:  ApplyRawDamagePergon(chr, 32);
    4:  ApplyRawDamagePergon(chr, 16);
  EndCase

EndFunction // }}}

///////////////////////////////////////////////////////////////////////////
// TurnDrinkingLevelAnimal - pro Turn -1 level_drink
//                         -  delay_drink vom animal-script
///////////////////////////////////////////////////////////////////////////
Function TurnDrinkingLevelAnimal(chr) // {{{
  If ((chr.cmdlevel < CMDLEVEL_SEER) And (!chr.dead))
    Var level_drink:=CDbl(GetObjProperty(chr, PROP_THIRST));
    If (level_drink)
      level_drink:=Max(1.0,level_drink-1.0);
      SetObjProperty(chr, PROP_THIRST, level_drink);
      TestMessage("Level_Drink "+level_drink, 1);
      CheckDrinkingHungryLevelAnimal(chr, level_drink);
    Else
      SetObjProperty(chr, PROP_THIRST, 9.0);
    EndIf
  EndIf
EndFunction // }}}

///////////////////////////////////////////////////////////////////////////
// CheckHungryLevelAnimal - pro Turn -1 hungrylevel
//                        - delay_eat kommt direct vom animal-script
///////////////////////////////////////////////////////////////////////////
Function TurnHungryLevelAnimal(chr) // {{{
  If ((chr.cmdlevel < CMDLEVEL_SEER) And (!chr.dead))
    Var hungrylevel:=CDbl(GetObjProperty(chr, PROP_HUNGER));
    If (hungrylevel)
      hungrylevel:=Max(1.0,hungrylevel-1.0);
      SetObjProperty(chr, PROP_HUNGER, hungrylevel);
      TestMessage("Hungrylevel "+hungrylevel, 1);
      CheckDrinkingHungryLevelAnimal(chr, hungrylevel);
    Else
      SetObjProperty(chr, PROP_HUNGER, 9.0);
    EndIf
  EndIf
EndFunction // }}}

////////////////////////////////////////////////////////////////////////////
// GetPossibleItems - Liefert eine Liste der Items, die das Tierchen sieht
////////////////////////////////////////////////////////////////////////////
Function GetPossibleItems(me, senserange) // {{{
  If (me.multi) // Steh ich in einem Spielerhaus?
    Var range:=me.multi.footprint;
    Return (ListObjectsInBox(range.xmin, range.ymin, me.z-19, range.xmax, range.ymax, me.z+19,me.realm));
  Else
    Return (ListItemsNearLocation(me.x, me.y, me.z, senserange,me.realm));
  EndIf
EndFunction // }}}

Function DrinkSound(me, char) // {{{
    var drink_snd := array{0x4cd, 0x4ce};
    PlaySoundEffect(me, drink_snd.randomentry());
    If (char)
        SayPergon(char, "*schluerf*");
    EndIf
    SleepMS(1500);
EndFunction // }}}

///////////////////////////////////////////////////////////////////////////
// Sucht nach Wassergefaessen kl. Wasserfass,  gr. Wasserfass,  WasserTrog
// SetObjProperty(me, PROP_THIRST,level_drink);
///////////////////////////////////////////////////////////////////////////
function Suche_nach_Wasser(me, char, level_drink, senserange, Debug) // {{{
  If (me.multi)
    Suche_nach_WasserMulti(me, char, level_drink, Debug);
    return;
  EndIf
  Var cfg:=ReadConfigFile("::itemdesc");
  Var ErreichVersuche;
  Var FoundWasser:=0;
  Var Pot;
  Var filled;
  Var filled_transfer;

  If (Debug)
    SysLogFile(me.name+" ["+Lower(Hex(me.serial))+"] ich suche nach Wasser ...");
  EndIf

  foreach item in GetPossibleItems(me, senserange)
    if ( ((item.objtype == 0x0e7b) or (item.objtype == 0x154d ) or (item.objtype == 0x7044 ) or (item.objtype==0x7048 )) and (!FoundWasser) )
      //    kl. Wasserfass                     gr. Wasserfass                       WasserTrog
      ErreichVersuche := 1;
      If (Debug)
        SysLogFile(me.name+" ["+Lower(Hex(me.serial))+"] Wassergefaess gefunden ...");
      EndIf

      While ((!CloseDistance(item)) && (ErreichVersuche<=20)) // Huhn auf gleiche Pos, mit max. 20 Versuchen
        SleepMS(125);
        ErreichVersuche+=1;
      EndWhile

      if ( (Distance(me, item)<2) and (!GetObjProperty(item, "inuse")) )
        Pot := item; SetObjProperty(pot, "inuse",1);
        If (Debug)
          SysLogFile(me.name+" ["+Lower(Hex(me.serial))+"] Wasser ist in der Naehe "+item.x+" "+item.y);
        EndIf
        FoundWasser:=1;
      endif
    endif
    SleepMS(2);
  endforeach

  if (FoundWasser)
    filled:=CDbl(GetObjProperty(pot, "filled"));
    var gift     := GetPoisonLevel(pot);
    var poisoner := pot.getprop(EV_POISONER);

    If (Debug)
      print("["+Lower(Hex(me.serial))+"] level_drink: "+level_drink);
      print("["+Lower(Hex(me.serial))+"] filled: "+filled);
    endif

    filled_transfer:=Min(15.0-level_drink,filled);// wenn mehr wasser gebraucht wird, als im gefaess ist ...
    filled-=filled_transfer;
    If (Debug)
      print("["+Lower(Hex(me.serial))+"] filled_transfer: "+filled_transfer);
    endif
    if (filled>0.4)
      SetObjProperty(pot, "filled", filled); SetObjProperty(pot, "inuse",0);
    else // wenn das Gefaess nach dem trinken leeer ist
      SetObjProperty(pot, "filled", 0);
      Var Gefaess_cfg:=cfg[pot.objtype];
      if (gefaess_cfg.targetitem)
        Var werkzeug:=GetObjProperty(pot, "Werkzeug");
        Var new_Gefaess:=CreateItemAtLocationPergon(pot.x, pot.y, pot.z, Gefaess_cfg.targetitem, 1,pot.realm);
        destroyitem(pot);
        SetObjProperty(new_Gefaess, "Werkzeug",werkzeug);
        IncRevision(new_Gefaess);
      else
        print("["+Lower(Hex(me.serial))+"] Gefaess "+pot.objtype+" hat kein Targetitem in der cfg !");
      endif
    endif


    level_drink+=filled_transfer;
    SetObjProperty(me, PROP_THIRST, level_drink);

    If (gift)
      SayPergon(char, "*Gift!*");
      If (poisoner)
        me.setprop(EV_POISONER, poisoner);
      EndIf
      SetPoisonLevel(me, gift);
      return;
    EndIf

    DrinkSound(me, char);

    If (Debug)
      print("["+Lower(Hex(me.serial))+"] filled_transfer: "+filled_transfer);
      print("["+Lower(Hex(me.serial))+"] level_drink: "+GetObjProperty(me, PROP_THIRST));
    endif
  endif

endfunction // }}}

Function Suche_nach_WasserMulti(me, char, level_drink, Debug) // {{{
  Var halfx, halfy,items,part,random,wasser;
  Var filled, filled_transfer;
  Var cfg:=ReadConfigFile("::itemdesc");

  If (Debug)
    SysLogFile(me.name+" ["+Lower(Hex(me.serial))+"] ich suche nach Wasser ...");
  EndIf
  //Wo bin ich und wie sind die Quadranten?
  halfx:=me.multi.footprint.xmin+CInt((me.multi.footprint.xmax-me.multi.footprint.xmin)/2);
  halfy:=me.multi.footprint.ymin+CInt((me.multi.footprint.ymax-me.multi.footprint.ymin)/2);

  If ((me.x <= halfx) && (me.y <= halfy))
    // NordWest
    items:=ListObjectsInBox(me.multi.footprint.xmin, me.multi.footprint.ymin, me.z-19, halfx, halfy, me.z+19,me.realm);
    part:={2,3,4};
  ElseIf ((me.x > halfx) && (me.y <= halfy))
    // NordOst
    items:=ListObjectsInBox(halfx+1, me.multi.footprint.ymin, me.z-19, me.multi.footprint.xmax, halfy, me.z+19,me.realm);
    part:={1,3,4};
  ElseIf ((me.x <= halfx) && (me.y > halfy))
    // SuedWest
    items:=ListObjectsInBox(me.multi.footprint.xmin, halfy+1, me.z-19, halfx, me.multi.footprint.ymax, me.z+19,me.realm);
    part:={1,2,4};
  ElseIf ((me.x > halfx) && (me.y > halfy))
    // SuedOst
    items:=ListObjectsInBox(halfx+1, halfy+1, me.z-19, me.multi.footprint.xmax, me.multi.footprint.ymax, me.z+19,me.realm);
    part:={1,2,3};
  EndIf

  Wasser:=SearchWasser(me,items,Debug,6);
  If (!Wasser)
    Repeat
      // Neuen Quadranten finden
      random:=RandomInt(part.size())+1;
      Case (part[random])
        1:  items:=ListObjectsInBox(me.multi.footprint.xmin, me.multi.footprint.ymin, me.z-19, halfx, halfy, me.z+19,me.realm);
        2:  items:=ListObjectsInBox(halfx+1, me.multi.footprint.ymin, me.z-19, me.multi.footprint.xmax, halfy, me.z+19,me.realm);
        3:  items:=ListObjectsInBox(me.multi.footprint.xmin, halfy+1, me.z-19, halfx, me.multi.footprint.ymax, me.z+19,me.realm);
        4:  items:=ListObjectsInBox(halfx+1, halfy+1, me.z-19, me.multi.footprint.xmax, me.multi.footprint.ymax, me.z+19,me.realm);
      EndCase
      part.erase(random);
      Wasser:=SearchWasser(me,items,Debug,20);
    Until ((Wasser) || (part.size()==0));
  EndIf
  If (Wasser)
    filled:=CDbl(Wasser.getprop("filled"));
    var gift     := GetPoisonLevel(Wasser);
    var poisoner := Wasser.getprop(EV_POISONER);

    If (Debug)
      SysLogFile("["+Lower(Hex(me.serial))+"] level_drink: "+level_drink);
      SysLogFile("["+Lower(Hex(me.serial))+"] filled: "+filled);
    EndIf

    filled_transfer:=Min(15.0-level_drink,filled);// wenn mehr wasser gebraucht wird, als im gefaess ist ...
    filled-=filled_transfer;
    If (Debug)
      SysLogFile("["+Lower(Hex(me.serial))+"] filled_transfer: "+filled_transfer);
    EndIf

    If (filled>0.4)
      Wasser.setprop("filled", filled);
      Wasser.setprop("inuse",0);
      Sleep(2);
      KeepDistanceToOpponent(me, Wasser, 2, 3, Debug); // Und wieder etwas weggehen
    Else // wenn das Gefaess nach dem trinken leeer ist
      Wasser.setprop("filled", 0);
      Var Gefaess_cfg:=cfg[Wasser.objtype];
      If (gefaess_cfg.targetitem)
        Var werkzeug:=Wasser.getprop("Werkzeug");
        Var new_Gefaess:=CreateItemAtLocationPergon(Wasser.x, Wasser.y, Wasser.z, Gefaess_cfg.targetitem, 1,Wasser.realm);
        DestroyItem(Wasser);
        If (werkzeug) // error muß man nicht setzen
          new_Gefaess.setprop("Werkzeug",werkzeug);
        EndIf
        IncRevision(new_Gefaess);
        Sleep(2);
        KeepDistanceToOpponent(me, new_Gefaess, 2, 3, Debug); // Und wieder etwas weggehen
      Else
        SysLog("["+Lower(Hex(me.serial))+"] Gefaess "+Wasser.objtype+" hat kein Targetitem in der cfg !");
      EndIf
    EndIf

    If (gift)
      SayPergon(char, "*Gift!*");
      If (poisoner)
        me.setprop(EV_POISONER, poisoner);
      EndIf
      SetPoisonLevel(me, gift);
      return;
    EndIf

    DrinkSound(me, char);

    level_drink+=filled_transfer;
    me.setprop(PROP_THIRST, level_drink);
    If (Debug)
      SysLogFile("["+Lower(Hex(me.serial))+"] filled_transfer: "+filled_transfer);
      SysLogFile("["+Lower(Hex(me.serial))+"] level_drink: "+GetObjProperty(me, PROP_THIRST));
    EndIf
  EndIf
EndFunction // }}}

Function SearchWasser(me, items, Debug, range) // {{{
  Var ErreichVersuche:=1;
  Var path, runpath,j,x,y;
  ForEach item in items
    If ((item.objtype == 0x0e7b) || (item.objtype == 0x154d ) || (item.objtype == 0x7044 ) || (item.objtype==0x7048))
    //    kl. Wasserfass                     gr. Wasserfass                       WasserTrog
      ErreichVersuche := 1;
      If (Debug)
        SysLogFile(me.name+" ["+Lower(Hex(me.serial))+"] Wassergefaess gefunden ...");
      EndIf
      SleepMS(Distance(me,item)*400+RandomInt(1000)); // Gestaffelt abhängig von der Entfernung loslaufen
      ErreichVersuche := 1;

      While ((Distance(me, item)>1) && (ErreichVersuche <=10)) // animal auf gleiche Pos, mit max. 10 Versuchen
        If (!item)
          Break;
        EndIf
        If ((!me.frozen) || (!me.paralyzed))
          x:=item.x;  //x,y etwas korrigieren falls auf das Feld nicht gelaufen werden kann (Tröge)
          y:=item.y;
          If (me.x>item.x)
            x+=1;
          ElseIf (me.x<item.x)
            x-=1;
          EndIf
          If (me.y>item.y)
            y+=1;
          ElseIf (me.y<item.y)
            y-=1;
          EndIf
          path:=FindPath(me.x,me.y,me.z,x,y,item.z,me.realm,0,range);
          range:=6;
          If (path)
            For (j:=1;j<=path.size();j+=1)
              runpath:=RunTowardLocation(path[j].x, path[j].y);
              If (!runpath)
                Sleep(2);
                Break;
              EndIf
            EndFor
          Else
            WanderPergon();
          EndIf
          ErreichVersuche+=1;
          If ( (Distance(me, item)<2) && (!GetObjProperty(item, "inuse")) )
            SetObjProperty(item, "inuse",1);
            If (Debug)
              SysLogFile(me.name+" ["+Lower(Hex(me.serial))+"] Wasser ist in der Naehe "+item.x+" "+item.y);
            EndIf
            Return(item); //Wasser gefunden
          EndIf
        EndIf
        Sleep(2);
      EndWhile
      If ( (Distance(me, item)<2) && (!GetObjProperty(item, "inuse")) )
        SetObjProperty(item, "inuse",1);
        If (Debug)
          SysLogFile(me.name+" ["+Lower(Hex(me.serial))+"] Wasser ist in der Naehe "+item.x+" "+item.y);
        EndIf
        Return(item); //Wasser gefunden
      EndIf
    EndIf
    SleepMS(5);
  EndForEach
EndFunction // }}}

///////////////////////////////////////////////////////////////////////////
// Sucht die Umgebung um "me" nach Items ab
// item.objtype in UOBJ_TIERFUTTER -> item.eat aus der cfg
// setzt dann SetObjProperty(me, PROP_HUNGER, hungrylevel);
// char bekommt Rohpoints, wenn me hungrylevel>15, SKILLID_TIERZUCHT
///////////////////////////////////////////////////////////////////////////
function Suche_nach_Futter(me, char, TierzuchtSkill, hungrylevel, senserange, Debug) // {{{
    If (me.multi)
        Suche_nach_Futterneu(me, char, TierzuchtSkill, hungrylevel, Debug);
        return;
    EndIf
    Var cfg := ReadConfigFile("::itemdesc");
    Var ErreichVersuche;
    Var FoundNahrung:=0;
    Var Nahrung_cfg;
    Var Nahrung;
    var Tierfutter := GetWhatIWouldEat(me);

    If (Debug)
        SysLogFile(
            me.name+" ["+Lower(Hex(me.serial))+"] ich suche nach Futter ..."
        );
    EndIf

    // ob der char ueberhaupt Tierzuechter ist und genuegend Skill hat
    If (GetSkillPergon(char, SKILLID_TIERZUCHT) < CInt(TierzuchtSkill-10))
        return;
    EndIf

    SayPergon(char, "Futter ja! "+char.name);
    KeepDistanceToOpponent(me, char, 0, 2, Debug);

    ForEach item in GetPossibleItems(me, senserange)
        If (FoundNahrung)
            break;
        EndIf
        If (!(item.objtype in Tierfutter))
            continue;
        EndIf

        ErreichVersuche := 1;
        If (Debug)
            SysLogFile(
                me.name+" ["+Lower(Hex(me.serial))+
                "] Futter ist in der Naehe "+item.x+" "+item.y
            );
        EndIf

        // animal auf gleiche Pos, mit max. 20 Versuchen
        While ((Distance(me, item)>1) && ErreichVersuche <=20)
            runtoward(item);
            ErreichVersuche += 1;

            // noch zu weit weg?
            If (Distance(me, item)>=2)
                continue;
            EndIf

            Nahrung := item;
            Nahrung_cfg := cfg[Nahrung.objtype];
            If (Nahrung_cfg)
                FoundNahrung := 1;
            EndIf
        EndWhile;
    EndForEach

    If (!FoundNahrung)
        ForEach item in GetPossibleItems(me, senserange)
            If (FoundNahrung)
                break;
            EndIf
            If (!(item.objtype in Tierfutter))
                continue;
            EndIf

            ErreichVersuche := 1;
            If (Debug)
                SysLogFile(
                    me.name+" ["+Lower(Hex(me.serial))+
                    "] Futter ist in der Naehe "+item.x+" "+item.y
                );
            EndIf

            // animal auf gleiche Pos, mit max. 20 Versuchen
            While ((!CloseDistance(item)) && (ErreichVersuche<=20))
                SleepMS(125);
                ErreichVersuche += 1;
            EndWhile

            // noch nicht nah genug?
            If (Distance(me, item)>=2)
                continue;
            EndIf

            Nahrung := item;
            Nahrung_cfg := cfg[Nahrung.objtype];
            If (Nahrung_cfg)
                FoundNahrung := 1;
            EndIf
        EndForEach
    EndIf

    If (FoundNahrung)
        var naehrwert := GetFoodSaturation(Nahrung);

        var gift     := GetPoisonLevel(Nahrung);
        var poisoner := Nahrung.getprop(EV_POISONER);
        If (gift)
            SayPergon(char, "*Gift!*");
            SubtractAmount(Nahrung, 1);
            If (poisoner)
                me.setprop(EV_POISONER, poisoner);
            EndIf
            SetPoisonLevel(me, gift);
            return;
        EndIf

        If (char)
            SayPergon(char, "*schmatz*");
        EndIf
        While ((hungrylevel < 15.0)  and (getamount(Nahrung)))
            SubtractAmount(Nahrung, 1);
            hungrylevel += naehrwert;
            PlaySoundEffect(me, SFX_3B + RandomInt(3));
            SleepMS(1000);
        EndWhile
        If (hungrylevel >= 15.0)
            // fuer erfolgreiches Fuettern des Tieres gibt es Rohpoints
            CheckSkillPergon(
                char, SKILLID_TIERZUCHT,
                CInt(TierzuchtSkill), CInt(TierzuchtSkill*20)
            );
        EndIf
        SetObjProperty(me, PROP_HUNGER, hungrylevel);
    EndIf

    If (Debug)
        SysLogFile(
            me.name+" ["+Lower(Hex(me.serial))+"] hungrylevel: "+
            GetObjProperty(me, PROP_HUNGER)
        );
    EndIf
EndFunction // }}}

///////////////////////////////////////////////////////////////////////////
// Sucht die Umgebung um "me" nach Items ab
// item.objtype in UOBJ_TIERFUTTER -> item.eat aus der cfg
// setzt dann SetObjProperty(me, PROP_HUNGER, hungrylevel);
// char bekommt Rohpoints, wenn me hungrylevel>15, SKILLID_TIERZUCHT
///////////////////////////////////////////////////////////////////////////
Function Suche_nach_Futterneu(me, char, TierzuchtSkill, hungrylevel, Debug) // {{{
    var halfx, halfy, items, part, random;
    var cfg := ReadConfigFile("::itemdesc");
    var Nahrung;

    If (Debug)
      SysLogFile(
          me.name+" ["+Lower(Hex(me.serial))+"] ich suche nach Futter ..."
      );
    EndIf

    // ob der char ueberhaupt Tierzuechter ist und genuegend Skill hat
    If (GetSkillPergon(char, SKILLID_TIERZUCHT) < CInt(TierzuchtSkill-10))
      return;
    EndIf

    SayPergon(char, "Futter Ja! "+char.name);
    halfx := me.multi.footprint.xmin +
      CInt((me.multi.footprint.xmax - me.multi.footprint.xmin)/2);
    halfy := me.multi.footprint.ymin +
      CInt((me.multi.footprint.ymax - me.multi.footprint.ymin)/2);

    If ((me.x <= halfx) && (me.y <= halfy))
      // Nordwest
      items := ListObjectsInBox(
        me.multi.footprint.xmin, me.multi.footprint.ymin, me.z-19,
        halfx, halfy, me.z+19, me.realm
      );
      part := {2, 3, 4};
    ElseIf ((me.x > halfx) && (me.y <= halfy))
      // Nordost
      items := ListObjectsInBox(
        halfx+1, me.multi.footprint.ymin, me.z-19,
        me.multi.footprint.xmax, halfy, me.z+19, me.realm
      );
      part := {1, 3, 4};
    ElseIf ((me.x <= halfx) && (me.y > halfy))
      // Suedwest
      items := ListObjectsInBox(
        me.multi.footprint.xmin, halfy+1, me.z-19,
        halfx, me.multi.footprint.ymax, me.z+19, me.realm
      );
      part := {1, 2, 4};
    ElseIf ((me.x > halfx) && (me.y > halfy))
      // Suedost
      items := ListObjectsInBox(
        halfx+1, halfy+1, me.z-19,
        me.multi.footprint.xmax, me.multi.footprint.ymax, me.z+19,
        me.realm
      );
      part:={1, 2, 3};
    EndIf

    Nahrung := SearchFutter(me, items, cfg, Debug, 6);
    If (!Nahrung)
      Repeat
        // Neuen Quadranten finden
        random := RandomInt(part.size()) + 1;
        Case (part[random])
        1:
          items := ListObjectsInBox(
            me.multi.footprint.xmin, me.multi.footprint.ymin, me.z-19,
            halfx, halfy, me.z+19, me.realm
          );
        2:
          items := ListObjectsInBox(
            halfx+1, me.multi.footprint.ymin, me.z-19,
            me.multi.footprint.xmax, halfy, me.z+19, me.realm
          );
        3:
          items := ListObjectsInBox(
            me.multi.footprint.xmin, halfy+1, me.z-19,
            halfx, me.multi.footprint.ymax, me.z+19, me.realm
          );
        4:
          items := ListObjectsInBox(
            halfx+1, halfy+1, me.z-19,
            me.multi.footprint.xmax, me.multi.footprint.ymax, me.z+19,
            me.realm
          );
        EndCase
        part.erase(random);
        Nahrung := SearchFutter(me, items, cfg, Debug, 20);
      Until ((Nahrung) || (part.size()==0));
    EndIf

    If (Nahrung)
      var naehrwert := GetFoodSaturation(Nahrung);

      var gift     := GetPoisonLevel(Nahrung);
      var poisoner := Nahrung.getprop(EV_POISONER);
      If (gift)
        SayPergon(char, "*Gift!*");
        SubtractAmount(Nahrung, 1);
        If (poisoner)
          me.setprop(EV_POISONER, poisoner);
        EndIf
        SetPoisonLevel(me, gift);
        return;
      EndIf

      If (char)
        SayPergon(char, "*schmatz*");
      EndIf
      While ((hungrylevel < 15.0) && (GetAmount(Nahrung)))
        SubtractAmount(Nahrung, 1);
        hungrylevel += naehrwert;
        PlaySoundEffect(me, SFX_3B+RandomInt(3));
        SleepMs(1000);
      EndWhile
      If (hungrylevel >= 15.0)
        // fuer erfolgreiches Fuettern des Tieres gibt es Rohpoints
        CheckSkillPergon(
          char, SKILLID_TIERZUCHT,
          CInt(TierzuchtSkill), CInt(TierzuchtSkill*20)
        );
      EndIf
      SetObjProperty(me, PROP_HUNGER, hungrylevel);
      // Und wieder etwas weggehen
      KeepDistanceToOpponent(me, Nahrung, 2, 3, Debug);
    EndIf

    If (Debug)
      SysLogFile(
        me.name+" ["+Lower(Hex(me.serial))+"] hungrylevel: "+
        GetObjProperty(me, PROP_HUNGER)
      );
    EndIf
EndFunction // }}}

Function SearchFutter(me, byref items, cfg, Debug, range) // {{{
  var ErreichVersuche, path, runpath, Nahrung, Nahrung_cfg, essen := array, trog := 0;
  var Tierfutter := GetWhatIWouldEat(me);

  ForEach item in (items)
    SleepMS(2);
    If (!(item.objtype in Tierfutter))
      // ungeniessbare Sachen ignorieren
      If (item.objtype == 0x704a)
        //Futtertrog
        var introg := EnumerateItemsInContainer(item, ENUMERATE_ROOT_ONLY);
        ForEach itemintrog in (introg)
          If (!(itemintrog.objtype in Tierfutter))
            continue;
          Else
            essen.append(itemintrog);
          EndIf
        EndForEach
        If (essen.size());
          trog := item;
        Else
          continue;
        EndIf
      Else
        continue;
      EndIf
    EndIf

    // Gestaffelt abhängig von der Entfernung loslaufen
    SleepMS(Distance(me, item) * 400 + RandomInt(1000));

    ErreichVersuche := 1;
    If (Debug)
      SysLogFile(
        me.name+" ["+Lower(Hex(me.serial))+
        "] Futter ist in der Naehe "+item.x+" "+item.y
      );
    EndIf

    // animal auf gleiche Pos, mit max. 5 Versuchen
    While ((Distance(me, item) > 1) && ErreichVersuche <= 5)
      If (!item)
        Break;
      EndIf
      Sleep(2);
      If (!item)
        Break;
      EndIf
      ErreichVersuche+=1;
      If ((me.frozen) || (me.paralyzed))
        continue;
      EndIf
      
      var x := item.x;  //x,y etwas korrigieren falls auf das Feld nicht gelaufen werden kann (Tröge)
      var y := item.y;
      If (trog)
        If (me.x > item.x)
          x += 1;
        ElseIf (me.x < item.x)
          x -= 1;
        EndIf
        If (me.y > item.y)
          y += 1;
        ElseIf (me.y < item.y)
          y -= 1;
        EndIf
      EndIf

      path := FindPath(
        me.x, me.y, me.z, x, y, item.z, me.realm, 0, range
      );
      range := 6;
      If (path)
        var i;
        For (i := 1; i <= path.size(); i += 1)
          runpath := RunTowardLocation(path[i].x, path[i].y);
          If (!runpath)
            Break;
          EndIf
        EndFor
      Else
        WanderPergon();
      EndIf

      If (Distance(me, item) >= 2)
        continue;
      EndIf

      If (trog)
        Nahrung := essen.randomentry();
      Else
        Nahrung := item;
      EndIf
      
      Nahrung_cfg := cfg[Nahrung.objtype];
      If (Nahrung_cfg.eat)
        return Nahrung;
      EndIf
    EndWhile

    If (Distance(me, item) < 2)
      If (trog)
        Nahrung := essen.randomentry();
      Else
        Nahrung := item;
      EndIf
      Nahrung_cfg := cfg[Nahrung.objtype];
      If (Nahrung_cfg.eat)
        return Nahrung;
      EndIf
    EndIf
  EndForEach

  return 0;
EndFunction // }}}

///////////////////////////////////////////
// CloseDistance - Verringere den Abstand
///////////////////////////////////////////
Function CloseDistance(opponent) // {{{
  //Case (Distance(me, opponent))
  //  1: // oben, weil am haeufigsten
  //  0: TurnToward(opponent);
  //     Return (1);
  // 2: WalkToward(opponent);
  //     Return (0);
  //
  //  default: RunToward(opponent);
  //           Return (0);
  //EndCase

    Case (Distance(me, opponent))
    1: // oben, weil am haeufigsten
    0: TurnToward(opponent);
       Return (1);
    2: If (!WalkToward(opponent))
         Return (2);
       Else
         Return (0);
       EndIf

    default: If (!RunToward(opponent))
               Return (1);
             Else
               Return (0);
             EndIf
  EndCase
EndFunction // }}}

// vim: sw=2 sts=2
