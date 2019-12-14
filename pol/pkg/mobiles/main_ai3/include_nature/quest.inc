/////////////////////////////////////////////////////////////////////
// 26.03.2006  Questis sind NPCs für Autoquests
// 08.09.06 Fox: - eventtimer durch eventwaiter ersetzt
// 05.04.07 Fox: - StepSleep Änderung nachgezogen, nen Sleep gegen Runaway eingefügt
/////////////////////////////////////////////////////////////////////

///////////////
// Konstanten
///////////////

Include "include/clock";// Für Its_Sleeptime()

Const MAX_CREATE_TRYES:=4;

Function OnIdleQuesti()
  If (Me.warmode)
    Var opp := SystemFindObjectBySerial(GetObjProperty(Me,PROP_OPPONENT));
    If (opp)
      //dann kaempfe ich weiter mit dem aktuellen opponent
      //PrintTextAbovePergon(Me,Me,"kampf " + opp.name);
      FightQuesti(opp, params[RANGEFOLLOW]);
    Else
      //mein aktueller gegner ist weg -> da mach ich wieder auf friedlich
      //PrintTextAbovePergon(Me,Me,"opp weg");
      MakeMePeaceful(params[RANGESENSE]);
    EndIf
  Else
    If ( GetObjProperty(Me,PROP_STAY) )
      //nichts tun, nichtmal umherlaufen
    Else
      If (wandersteps > 0) //wandersteps=0 ist Tiefschlaf
        If (Its_Sleeptime())
          Me.hidden:=1; //verstecken, weil Schlafenszeit
          wandersteps:=0;
        Else
          WanderPergon();
          wandersteps-=1;
        EndIf
      EndIf
    EndIf
  EndIf
EndFunction

Function OnStartQuesti()
  //verankern
  DropAnchor();
EndFunction

Function FightQuesti( byref opponent, byref range )

  wandersteps:=params[IDLESTEPS]; //nach nem Kampf Rumwandern

  //Var evfight:={};
  //evfight.+type:=EVID_STILLFIGHT; <--- wird gar nicht benutzt bisher
  //evfight.+source:=opponent;
  Var timeout_:=ReadGameClock()+2;
  Var ret_wert:=0;
  Var event_merker:=array; //merkt sich die events die während des wartens ausgelesen wurden um sie wieder einzuspeisen
  Var ev;

  If (!Me.warmode)
    SetOpponent( ChangeTarget(opponent) );
    SetObjProperty(Me,PROP_OPPONENT, opponent.serial); //aktuellen opponent merken
    SetWarmode(1);
  EndIf

  DisableEvents(SYSEVENT_ENTEREDAREA); //waehrend ich kaempfe interessiert mich das nicht
  Var last_instr:=Me.process.instr_cycles;
  While ((ReadGameClock()<timeout_)||(event_merker.size() == 0)) //nur wenn die Zeit rum ist und eine Event aufgelaufen ist beenden
    TestMessage("round_fight instr: " + CStr(Me.process.instr_cycles-last_instr), 2);
    last_instr:=Me.process.instr_cycles;
    If (!CheckLineOfSight(me,opponent))
      SetWarmode(0);
      ChangeTarget(opponent);
      SetOpponent(opponent);
      SetObjProperty(Me,PROP_OPPONENT, opponent.serial); //aktuellen opponent merken
      SetWarmode(1);
    EndIf

    If ( (opponent) && (Distance( Me, opponent ) <= range) && !(opponent.hidden) && !(opponent.concealed) && !(opponent.dead))
      TestMessage("------------------", 2);
      If ((Distance( Me, opponent ) < 2 )&&(abs(Me.z-opponent.z) < 15)) //falls ich nicht schon vor dem gegner stehe, renn ich hin
        TestMessage("nahkampf",3);
        EraseObjProperty(Me, PROP_BLOCKED);
        EraseObjProperty(Me, PROP_PATHFIND);
        //Warten auf Aktion
        EnableEvents( SYSEVENT_OPPONENT_MOVED ); //dann muss man nicht ständig prüfen ob sich der Gegner bewegt
        SleepMs(100);
        ev:=Wait_for_EventLogging(5); //wenn nix los ist, der Gegner still steht warum hetzen, also besser ausruhen
        DisableEvents( SYSEVENT_OPPONENT_MOVED );
        If (ev)
          If (ev.type == EVID_CAST_WAIT)
            TestEvents("MagicFight Event",1,ev);
            MagicFight(opponent);
          ElseIf (ev.type == EVID_CHANGE_TARGET)
            SetWarmode(0);
            ChangeTarget(opponent);
            SetOpponent(opponent);
            SetObjProperty(Me,PROP_OPPONENT, opponent.serial); //aktuellen opponent merken
            SetWarmode(1);
          ElseIf (ev.type!=SYSEVENT_OPPONENT_MOVED)
            TestEvents("fightquesti", 3, ev);
            event_merker.append(ev);
          EndIf
        EndIf
      Else
        TestMessage("hinrennen",3);
        MainRunToward(opponent);
        ev:=Wait_for_EventLogging(1);
        If (ev.type == EVID_CAST_WAIT)
          MagicFight(opponent);
        ElseIf (ev.type == EVID_CHANGE_TARGET)
          SetWarmode(0);
          ChangeTarget(opponent);
          SetOpponent(opponent);
          SetObjProperty(Me,PROP_OPPONENT, opponent.serial); //aktuellen opponent merken
          SetWarmode(1);
        ElseIf (ev.type!=SYSEVENT_OPPONENT_MOVED)
          TestEvents("fightquesti", 3, ev);
          event_merker.append(ev);
        EndIf
      EndIf
      ret_wert:=1;
    Else
      //der gegner ist weg -> aufhoeren zu kaempfen
      MakeMePeaceful(params[RANGESENSE]);
      EraseObjProperty(Me, PROP_PATHFIND);
      EraseObjProperty(Me, PROP_BLOCKED);
      SleepMs(30);
      ret_wert:=0;
      Break;
    EndIf
  EndWhile

  //Events einspeisen
  TestMessage("Events einspeisen: "+event_merker.size(),3);
  ForEach ev in event_merker
    SendEvent(Me,ev);
  EndForEach
  Return ret_wert;
EndFunction

////////////////////////////////
// Follow - Folge einem Mobile
////////////////////////////////

Function Follow(byref master)
  // Manchmal kommts vor, dass ein Tier sich selber folgen will -> Abfangen
  TestMessage("Follow: " + master.name, 1);
  If (me == master)
    Return;
  EndIf

  Var d:=0;
  Var close_d:=1;
  Var chaseloop:=0;
  Var imfollow:=1;
  Var timecounter:=0;
  Var town, evil;

  While (master && imfollow)
    If (Events_Waiting() > 0)
      Return;
    EndIf
    If (!master.getprop("master"))
      imfollow:=0;
    EndIf
    d:=Distance(me, master);
    If (d>close_d)
      RunToward(master);

      chaseloop+=1;
      If (chaseloop>100)
        close_d:=2;
      EndIf
    Else
      If (d<close_d)
        close_d:=1;
        chaseloop:=0;
      EndIf
      StepSleep(1, 1);
    EndIf
    //Check wenn Feinde in der Nähe mal profilaktisch mit einer gewissen wahrscheinlichkeit attacken
    If (timecounter < 5)
      //Engaged interesiert mich jetzt nicht
      town:=PlaceTown(Me);
      evil:=PlaceEvil(Me);

      If ((!town) || ((town) && (evil) && (Me.alignment ==ALIGNMENT_GOOD)) || ((town) && (!evil) && (Me.alignment ==ALIGNMENT_EVIL)))
        // Keine Stadt || Böse Stadt + selber Gut || Gute Stadt + selber Böse
        DisableEvents(SYSEVENT_ENGAGED);
        ForEach mobile in ListMobilesNearLocation(Me.x,Me.y,Me.z,15,Me.realm)
          //Dürfen eh nur NPCs
          If ( (mobile.isa(POLCLASS_NPC)) && (!mobile.enabled("invul"))
            && (!mobile.getprop("master")) && (mobile.graphic<>0x190)
            && (mobile.graphic<>0x191) && (!mobile.warmode))
            If (!RandomInt(20)) //20%
              Var ev:=struct;
              ev.+source;
              ev.+type;
              ev.source:=me;
              ev.type:=SYSEVENT_ENGAGED;
              SendEvent(mobile,ev);
            EndIf
          EndIf
          SleepMs(10); //Runaways verhindern bei vielen Mobs in der Gegend
        EndForEach
        SleepMS(5);
        //Jetzt interesiert es mich wieder
        EnableEvents(SYSEVENT_ENGAGED);
      EndIf
      timecounter:=0;
    EndIf
    timecounter+=1;
  EndWhile
EndFunction

Function CreateNemesis()
  SayPergon(Me, "Hilfe Woelfe!");
  For anzahl:=1 To RandomIntMinMax(3,9) //RandomDiceRoll("1d6+2")
    CreateGuardian(Me, "wolf", 4);
    SleepMS(2);
  EndFor
EndFunction

//////////////////////////////////////////////////////////////////////////////
// CreateGuardian - Erzeugt die Angreifer (copy aus treasurechest.src)
//////////////////////////////////////////////////////////////////////////////

Function CreateGuardian(byref chest, byref template, byref range)
  Var times:=0; // Wieviele Versuche hab ich schon gebraucht?
  Var creature;
  Var origspawnx;
  Var origspawny;
  Var origspawnz;
  Var spawnx;
  Var spawny;
  Var spawnz;

  // Spawn-Location suchen
  While ((times<>-1) And (times<=MAX_CREATE_TRYES))
    spawnx:=RandomIntMinMax(chest.x-range,chest.x+range+1);
    spawny:=RandomIntMinMax(chest.y-range,chest.y+range+1);
    spawnz:=GetStandingHeight(spawnx, spawny, chest.z,chest.realm);

    If (spawnz==error)
      times+=1;
    Else
      spawnz:=spawnz.z;
      times:=-1;
    EndIf
  EndWhile

  // Keine passende Spawn-Location gefunden
  If (times>MAX_CREATE_TRYES)
    Return;
  EndIf

  times:=0; // Wieviele Versuche hab ich schon gebraucht?

  // NPC erzeugen
  While (times<=MAX_CREATE_TRYES)
    origspawnx:=5280+RandomInt(9);
    origspawny:=1182+RandomInt(5);
    origspawnz:=0;
    SleepMS(10);
    creature:=CreateNpcFromTemplate(template, origspawnx, origspawny, origspawnz,0,chest.realm);
    If (creature)
      times:=-1;
      Var ev:=struct;
      ev.+source;
      ev.+type;
      ev.source:=me;
      ev.type:=SYSEVENT_ENGAGED;
      SendEvent(creature,ev);
      Break;
    Else
      times+=1;
    EndIf
  EndWhile

  // NPC konnte nicht erzeugt werden
  If (times>MAX_CREATE_TRYES)
    Return;
  EndIf

  // NPC an seine Ziellocation verschieben
  MoveObjectToLocation(creature, spawnx, spawny, spawnz, creature.realm, MOVEOBJECT_FORCELOCATION);
EndFunction

//////////////////////////////////////////////////////////////////////////////
// Wegpunkt erreicht. Testen und Messages und sowas
//////////////////////////////////////////////////////////////////////////////

Function WayPointReached(byref p,byref e)
  //wenn der nächste Wegpunkt erreicht ist
  //damit abkürzungen nicht zugelassen sind
  If ((last_waypoint+1)==p)
    last_waypoint += 1;
    Var prop;
    Var quest := questcfg[CInt(GetObjProperty(Me,"quest"))];
    If (!quest)
      syslog("Quest "+ GetObjProperty(Me,"quest") +" doesn't exists");
      Return;
    EndIf

    Var text := GetConfigStringArray(quest,"placetext");
    If (!text)
      syslog("Quest "+ GetObjProperty(Me,"quest") +" doesn't exists");
      Return;
    EndIf

    If (len(text[p])>1)  // Text vorhanden?
      SayPergon(Me,text[p]);
    Else
      SayPergon(Me,"Hier bin ich richtig");
    EndIf

    Var master:=SystemFindObjectBySerial(Me.getprop(PROP_MASTER));
    If (master)
      prop:=master.getprop("quest");
      Var way:=SystemFindObjectBySerial(prop[CStr(GetObjProperty(Me,"quest"))][2][p]);  // Waypoint löschen
      If (way)
        KillMobileSilent(way,"waypoint");
        way.concealed:=0;
      EndIf
    EndIf
    If (e)
      //jetzt die Belohnungen und co rausrücken
      If (master)
        Var time:=timeout(master, me, TIME_COMPLETE, quest);
        If (!time)
          If (Me.getprop("giver"))
            If (SystemFindObjectBySerial(Me.getprop("giver")))
              PrintTextAbovePergon(master,SystemFindObjectBySerial(Me.getprop("giver")),quest.succed);
            EndIf
          Else
            PrintTextAbovePergon(master,Me,quest.succed);
          EndIf
        EndIf
        ForEach waypoint in (prop[CStr(GetObjProperty(Me,"quest"))][2])  //zur Sicherheit nochmal alle Waypoints "löschen"
          waypoint:=SystemFindObjectBySerial(waypoint);
          If (waypoint)
            KillMobileSilent(waypoint,"waypoint");
            waypoint.concealed:=0;
          EndIf
        EndForEach

        prop.erase(CStr(GetObjProperty(Me,"quest")));  // Quest löschen und in completed einfügen
        If (!time)
          If (!prop["completedquests"])
            prop["completedquests"]:={CInt(GetObjProperty(Me,"quest"))};
          Else
            prop["completedquests"].append(CInt(GetObjProperty(Me,"quest")));
          EndIf
        EndIf
        master.setprop("quest",prop);
        PlaySoundEffectPrivate(master,SFX_5B6,master);
        Sleep(3);
        If (!time)
          Var rewards:=GetConfigStringArray(quest,"reward");  // Belohnung
          ForEach reward in (rewards)
            reward:=SplitWords(reward);
            CreateItemInBackpackPergon(master,CInt(reward[1]),CInt(reward[2]));
          EndForEach
          PrintTextAbovePergon(master,Me,"Hier ist eure Belohnung.");
        EndIf
        Me.eraseprop("quest");
        Me.eraseprop(PROP_MASTER);
        Me.eraseprop(PROP_FOLLOWMASTER);

        //Selbstkillevent auslösen
        Var ev:=struct;
        ev.+type;
        ev.+source;

        ev.type := EVID_SELF_KILL;
        ev.source := me;
        SendEventWaiterGeneral(me, SELF_KILL_WAIT, ev);
      EndIf
    EndIf
  EndIf
EndFunction

//////////////////////////////////////////////////////////////////////////////
// Change Target Funktionen
//////////////////////////////////////////////////////////////////////////////

Function ChangeTarget(byref opponent)
  Var cfg:=GetNPCConfig(Me.npctemplate);
  //Nächstes TargetChangeEvent erzeugen
  Var ev:=struct;
  ev.+type;
  ev.type:=EVID_CHANGE_TARGET;
  SendEventWaiterGeneral(Me, cfg.changetargetdelay, ev);

  Case (cfg.changetarget)
    "easy":
      If (!RandomInt(10)) //10%
        Return opponent;
      EndIf
    "middle":
      If (!RandomInt(2)) //50%
        Return opponent;
      EndIf
    "hard":
      If (RandomInt(10)<9) //90%
        opponent:=ChangeToWeakest(opponent,params[RANGESENSE]);
        Return opponent;
      EndIf
    default:
      Return opponent;
  EndCase
EndFunction

Function ChangeToWeakest(byref opponent,byref viewrange);
  Var pulk:=ListMobilesNearLocation(opponent.x,opponent.y,opponent.z,viewrange,opponent.realm);
  Var newopponent:=opponent;
  ForEach player in pulk
    If (CInt(GetHPPergon(player))<Cint(GetHPPergon(newopponent)))
      If ((player.serial <> me.serial)&&(!player.isa(POLCLASS_NPC))&&(CheckLineOfSight(me,player)))
        newopponent:=player;
      EndIf
    EndIf
  EndForEach
  Return newopponent;
EndFunction

//////////////////////////////////////////////////////////////////////////////
// Bei versteckten Angriffen (Magier hinter mauern und co)
//////////////////////////////////////////////////////////////////////////////

Function AgainstHidden()
  //Speziell QuestMage

  Var delay:=-1;
  If ((me.getprop("poison_level")) && (params[SPELLCURE]))
    //Ups ich bin vergiftet, dann mal schauen ob ich mich entgiften kann
    delay:=CallCast_GetDelay(params[SPELLCURE],me);
  EndIf
  //Ich bekomm noch schaden und bin nicht vergIftet
  If ((antimagie)&&(delay==-1))
    If (params.exists(SPELLDISPEL))
      delay:=CallCast_GetDelay(SPELLID_CLERIC_DISPELFIELD,me);
      antimagie:=0;
    EndIf
  EndIf
  //Es gibt noch versteckte
  Var victims := ListMobilesNearLocationEx(me.x, me.y, me.z, params[RANGESENSE], LISTEX_FLAG_NORMAL + LISTEX_FLAG_HIDDEN,me.realm);
  Var leben_Welche:=0;
  ForEach victim in victims
    If (!victim.dead)
      leben_Welche:=1;
      Break;
    EndIf
  EndForEach
  If ((victims)&&(delay==-1)&&(leben_Welche))
    Var choice:={};
    ForEach spell in (params[SPELLS])
      Case (spell[1])
        SPELLID_NECRO_POISONCLOUD:
          choice.append(spell[1]);
        SPELLID_NECRO_ARMAGEDDON:
          choice.append(spell[1]);
        SPELLID_CLERIC_HOLYSPIRIT:
          choice.append(spell[1]);
        SPELLID_COMMON_REVEAL:
          choice.append(spell[1]);
        SPELLID_MAGE_BLIZZARD:
          choice.append(spell[1]);
        SPELLID_MAGE_CHAINLIGHTNING:
          choice.append(spell[1]);
        SPELLID_MAGE_EARTHQUAKE:
          choice.append(spell[1]);
        SPELLID_MAGE_FIREAURA:
          choice.append(spell[1]);
        SPELLID_MAGE_ICEAURA:
          choice.append(spell[1]);
        SPELLID_MAGE_LIGHTNINGAURA:
          choice.append(spell[1]);
        SPELLID_MAGE_STONEAURA:
          choice.append(spell[1]);
        SPELLID_MAGE_STONEHAIL:
          choice.append(spell[1]);
        SPELLID_MAGE_VULCANO:
          choice.append(spell[1]);
        SPELLID_NECRO_ARCHCURSE:
          choice.append(spell[1]);
        SPELLID_NECRO_ARCHPOISON:
          choice.append(spell[1]);
      EndCase
    EndForEach
    Var spell := choice.randomentry();
    TestMessage("AgainstHidden "+spell, 1);
    delay:=CallCast_GetDelay(spell,me);
  EndIf
  If (delay==-1)
    //Hat alles nicht geholfen und es ist auch keiner in der Nähe
    WanderPergon();
    Antimagie:=1;
  EndIf
  Return delay;
EndFunction

Function UmHilfeRufen()
  If (hilferuf)
    SayPergon(Me, "Hilfe, helft mir!");
    ForEach mob in ListMobilesNearLocation( Me.x, Me.y, Me.z, 15,Me.realm )
      If (mob.isa(POLCLASS_NPC) && (mob.script["townguard"]))
        Var ev_talk:=struct;
        ev_talk.+type:=SYSEVENT_SPEECH;
        ev_talk.+source:=Me;
        ev_talk.+text:="Hilfe";
        SendEvent(mob, ev_talk);
      EndIf
    EndForEach
    hilferuf:=0;
    //Wann darf er das nächste mal was sagen
    Var ev_speech:=struct;
    ev_speech.+type;
    ev_speech.type:=EVID_QUEST_SPEECH;
    ev_speech.+source;
    ev_speech.source:=Me;
    SendEventWaiterGeneral(Me, SPEECH_WAIT, ev_speech);
  EndIf
EndFunction

Function MakeOneStep()
  //Zwei Schritte wegen Teleporter
  Move(me.facing);
  Move(me.facing);
EndFunction

//Function ITS_SLEEPTIME
//Gibt eins zurueck, wenn es fuer das Tierchen
//Zeit waere zu schlafen (abh von ActiveTime)
Function Its_Sleeptime()
  If ( (!(params[WAKEUPTIME])) || (!(params[SLEEPTIME])))
    Return(0);
  EndIf

  Var currenthour:=GetDayTime();
  Var sleepstart:=params[SLEEPTIME];
  Var sleepstop:=params[WAKEUPTIME];
// Umrechnen wegen Überlauf bei Tagwechsel (2300->0000)
  If (sleepstop<sleepstart)
    sleepstop+=24;
    If (currenthour<(params[WAKEUPTIME]))
      currenthour+=24;
    EndIf
  EndIf
  If ((sleepstart<=currenthour) && (currenthour<=(sleepstop-1)))
    Return(0);
  Else
    //Schlafenszeit
    Return(1);
  EndIf
EndFunction

/////////////////////////////////////
//      G E T   D A Y T I M E      //
/////////////////////////////////////
// benoetigt clock.inc
// Gibt die momentane Tageszeit zurück
// also quasi die laufende Stunde
// 1      0001-0100 Geisterstunde
// 2-5              Nacht
// 6.7    0501-0700 Morgendaemmerung
// 8-12             Vormittag
// 13     1201-1300 Mittagsstunde
// 14-19            Nachmittag
// 20,21  1901-2100 Abenddaemmerung
// 22-24            Nacht
Function GetDaytime()
  Return(CInt(((GetPergonTime()/15)-1)/60)+1);
EndFunction

Function CastReflection()
	If (params[SPELLREFLECT])
	  var chance := RandomInt(100)+1;
	  If (chance <= params[REFLECTCHANCE]) 
	    SleepMS(1000);
      TestMessage("MagicFight: Magischer Spiegel war weg", 1);
      castspellAI( Me, Me, SPELLID_COMMON_REFLECT, MobType );
    EndIf
  EndIf
EndFunction
