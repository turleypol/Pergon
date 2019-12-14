/////////////////////////////////////////////////////////////////////
// Main-Loop fuer alle Main KIs
// Beinhaltet eine Vielzahl von KI Verhalten. Welche genutzt werden,
// für einen NPC, hängt von den Parametern aus der NPCDesc ab.
//
// Basierend auf den Grundlagen die Mai mit der Main_AI geschaffen hat.
// Basierend auf main_ai2 von Lax
//
// Hauptinclude die das Eigentliche Programm beinhaltet
// Sourcen sind aufgeteilt pro Nature um Scriptgrößen zu reduzieren
//
/////////////////////////////////////////////////////////////////////

use npc;
use file;
use cfgfile;
use util;
use uo;
use os;
use polsys;

Include "include/math";
Include "include/eventid";
Include "include/names";
Include "include/npc";
Include "include/animal";
Include "include/mobile";
Include "include/modifyskill";
Include ":newspells:resistance";
Include "include/properties";
Include "include/vetement"; //fuers habille
Include "include/eventwaiter"; // Für Zeitgesteuertes Casten
Include ":main_ai3:include_common/npccast";

//include ":main_ai3:include_common/tame";  // Bis Gruppenverhalten für alle machbar ist
//include ":main_ai3:include_nature/group"; // hier entfernen

Var Me:=Self(); //wird oft benutzt deshalb global
Var params; //enthält alle wichtigen Parameter, sollten überall verfügbar sein deshalb global
Var wandersteps:=0; //weil sonst rennen ja alle wild los bei nem restart oder Questaufbau, Global erstmal aus Faulheit
Var fastround:=0; //Anzahl in Runden in denen die HauptschleIfe beschleunigt durchläuft (um Idle Funktionalität zu beschleunigen) <- Hilfskrücke

Var saywords:=1;  // Sonderwurst für Spell's damit es mit den anderen KI's konform geht

Var MobType; // Was ist es für ein Vieh (Human,Sea,Animal,Monster) diese Unterscheiden sich
             // in der Animationsnummer

/////////////////////////////////////////////////
// main_ai3 - Beinhaltet die Main-Schleife
/////////////////////////////////////////////////
program main_ai3()
  If (polcore().uptime<180)  // Frischer Restart
    SleepMS(RandomIntMinMax(5000,45000)); // Bissle verteilen (5-45s)
  EndIf
  TestMessage("Start KI Skript main_ai3", 1);
  SetScriptController(me);
  //fuer alle monster
  MobType:=GetMobType(); // Typ rausfinden
  Init();

  Var ev:=struct;

  //bei erstellen oder restarten erstmal warmode:=0, sonst kommt ein gerestartets tier evtl. nicht mehr aus dem warmode
  Set_Event_Queue_Size(30); //mal probehalber eine groessere queue spendieren
  SetWarmode(0);

  Stats_korrigieren(me); //notwendig um Stats über der 210er Grenze aus der NPCDesc umsetzen zu können
  InitNPC();  // Override

  //intern wird idlespeed eh auf 250 gekürzt bei der Berechnung
  //und so wirds für main_ai2 gleich richtig verwendet
  If (Me.run_speed > 250)
    Me.run_speed:=250;
  EndIf

  SleepMs(10); //entschärfen von Runaways


  OnStart(); // Override
  //wandersteps:=params[IDLESTEPS]; //Nur zum testen !!!

  Var last_instr:=Me.process.instr_cycles;

  SleepMs(100); //entschärfen von Runaways

  while(1)
    SleepMS(2);  // RunawayBackup
    TestMessage("round_main instr: " + CStr(Me.process.instr_cycles-last_instr), 1);
    last_instr:=Me.process.instr_cycles;

    //fastround+=1; //nur zum testen, beschleunigt das Ideln !!!

    Before_Events();  // Override (sowas wie schlafensschleife)

    TestMessage("events " + Events_Waiting(), 2);
    If(Me.warmode)
      TestMessage("Kampf Idle",2);
      ev:=Wait_for_EventLogging(1); //im Kampf schnell zur Idle Funktion
    Else
      ev:=IdleTime();  //Override !Return nicht vergessen!
    EndIf

    If ((ev) && ((ev.source.cmdlevel < CMDLEVEL_SEER)
      || (!(ev.type in {SYSEVENT_DOUBLECLICKED,SYSEVENT_ITEM_GIVEN,
                        SYSEVENT_ENGAGED,SYSEVENT_DISENGAGED,
                        SYSEVENT_DAMAGED,SYSEVENT_ENTEREDAREA,
                        SYSEVENT_LEFTAREA}))))

      case(ev.type)
        SYSEVENT_DOUBLECLICKED:
          TestMessage("MainLoop: DOUBLECLICKED", 1);
          Event_DoubleClicked(ev);  //Override

        SYSEVENT_ITEM_GIVEN:
          TestMessage("MainLoop: ITEM_GIVEN", 1);
          Event_ItemGiven(ev);  //Override

        SYSEVENT_SPEECH:
          TestMessage("MainLoop: speech von "+ev.source.name, 1);
          Event_Speech(ev);  //Override

        SYSEVENT_ENGAGED:
          TestMessage("MainLoop: engaged " + ev.source.name, 1);
          Event_Engaged(ev);  //Override

        SYSEVENT_DISENGAGED:
          TestMessage("MainLoop: disengaged " + ev.source.name, 1);
          Event_Disengaged(ev);  //Override

        SYSEVENT_DAMAGED:
          TestMessage("MainLoop: damaged", 1);
          Event_Damaged(ev);  //Override

        SYSEVENT_ENTEREDAREA:
          TestMessage("MainLoop: ENTEREDAREA " + ev.source.name,1);
          Event_EnteredArea(ev);  //Override

        SYSEVENT_LEFTAREA:
          TestMessage("MainLoop: LEFTAREA " + ev.source.name,1);
          Event_LeftArea(ev); // Override

        EVID_STILLRUN:
          TestMessage("MainLoop: stillrun", 1);
          Event_StillRun(ev);

        EVID_LOOKAROUND:
          TestMessage("MainLoop: suchen", 1);
          Event_LookAround(ev);  //Override

        EVID_FOLLOW:
          TestMessage("MainLoop: Follow", 1);
          Event_Follow(ev);  //Override

        EVID_ASKJOIN: //fuer herden/rudeltiere
          TestMessage("MainLoop: askjoin", 1);
          Event_AskJoin(ev);  //Override

        EVID_GRANTJOIN:
          TestMessage("MainLoop: grantjoin", 1);
          Event_GrantJoin(ev);  //Override

        EVID_REFUSEJOIN:
          TestMessage("MainLoop: refusejoin", 1);
          Event_RefuseJoin(ev);  //Override

        EVID_WALK: //für das walkto-command
          Event_Walk(ev);  //Override

        EVID_PEACEMADE:
          TestMessage("MainLoop: friedlich sein", 1);
          Event_PeaceMade(ev);  //Override

        EVID_GUARDS_HELP:
          TestMessage("MainLoop: EVID_GUARDS_HELP", 1);
          Event_Guards_Help(ev);  //Override

        EVID_CAST_WAIT:
          TestMessage("MainLoop: EVID_CAST_WAIT", 1);
          Event_Cast_Wait(ev);  //Override

        EVID_QUEST_WAYPOINT:
          TestMessage("MainLoop: WayPoint reached ",1);
          Event_Quest_Waypoint(ev);  // Override

        EVID_QUEST_SPEECH:
          Event_Quest_Speech(ev);  // Override

        EVID_SELF_KILL:
          Event_Self_Kill(ev); // Override

        EVID_QUEST_NEMESIS:
          Event_Quest_Nemesis(ev); // Override
          
        EVID_REFLECTION:
          Event_Reflection(ev);

        default:
          TestEvents("Unbearbeitbares Event", 1, ev);
      endcase
    Else
      //kein event was nun? gibts doch gar nicht. oder es war ein
      If (ev)
        //event von >= seer
        TestEvents("Commandlevel Event", 1, ev);
      Else
        //kein Event, also ne Idle Runde (wenn wandersteps nicht hochgesetzt ist kann nach einem Event ne längere Zeit vergehen bis in Idle gelaufen wird)
        TestMessage("IdleEcke", 4);
        OnIdle();  //Override

        If (params[IDLESOUND])
          If (!RandomInt(20)) // 5% chance
            PlaySoundEffect(Me, params[IDLESOUND]);
            Case (MobType)
              MOB_HUMAN:    Case (RandomInt(2))
                              0: PerformAction(Me, ANIM_FIDGET_1);
                              1: PerformAction(Me, ANIM_FIDGET_2);
                            EndCase
              MOB_SEA:      PerformAction(Me, ANIM_STAND);
              MOB_ANIMAL:   Case (RandomInt(3))
                              0: PerformAction(Me, ANIM_RUN_ARMED);
                              1: PerformAction(Me, ANIM_ATTACK_1HAND_WIDE);
                              2: PerformAction(Me, ANIM_ATTACK_1HAND_JAB);
                            EndCase
              MOB_MONSTER:  Case (RandomInt(2))
                              0: PerformAction(Me, ANIM_CAST_AREA);
                              1: PerformAction(Me, ANIM_ATTACK_BOW);
                            EndCase
              MOB_UNKNOWN:
            EndCase
          EndIf
        EndIf
      EndIf
    EndIf
  endwhile
endprogram

/////////////////////////////////////////////////
// Init - Paramterinitialisierung eca.
/////////////////////////////////////////////////
Function Init()
  //Zum setzen der Castpower
  // spellpower [1] [2] [3]
  // [1] influenceFrom - "Alchemie,Musik,Fische,Magie,Flakon,Artefakt"
  // [2] value
  // [3] malus 1/0
  // [1] alle _all
  // [1] mit Class
  // [1] mit Class und Subclass
  //  spellpower Magie 20 0
  //  spellpower Magie Mage 10 1
  //  spellpower Magie Cleric 10 0
  //  spellpower Magie Necro Poison 10 0

  // Siehe Enum ParamsStruct main_ai.inc
  params:=struct;
  params.insert(AILEVEL,0);
  params.insert(RANGESENSE,0);
  params.insert(RANGEFOLLOW,0);
  params.insert(IDLESPEED,0);
  params.insert(IDLESTEPS,0);
  params.insert(IDLESOUND,0);
  params.insert(ANGERSOUND,0);

  Var mytemplate:=GetNPCConfig(Me.npctemplate);

  params[RANGESENSE]:=mytemplate.rangesense;
  If(!params[RANGESENSE])
    LogToFile("::log/ailog.log", "FEHLER: " + Me.npctemplate + " hat keinen rangesense eintrag!");
    params[RANGESENSE] := RANGE_DEFSENSE;
  EndIf
  params[RANGEFOLLOW]:=mytemplate.rangefollow;
  If(!params[RANGEFOLLOW])
    LogToFile("::log/ailog.log", "FEHLER: " + Me.npctemplate + " hat keinen rangefollow eintrag!");
    params[RANGEFOLLOW] := RANGE_DEFFOLLOW;
  EndIf
  params[IDLESPEED]:=mytemplate.idlespeed;
  If(params[IDLESPEED]<0)
    params[IDLESPEED] := DEFIDLESPEED;
    LogToFile("::log/ailog.log", "WARNUNG: " + Me.npctemplate + " hat keinen idlespeed eintrag!");
  EndIf
  params[IDLESTEPS]:=mytemplate.idlesteps;
  If(params[IDLESTEPS]<0)
    params[IDLESTEPS] := DEFIDLESTEPS;
    LogToFile("::log/ailog.log", "WARNUNG: " + Me.npctemplate + " hat keinen idlesteps eintrag!");
  EndIf

  params[AILEVEL]:=mytemplate.ailevel;
  If(!params[AILEVEL])
    params[AILEVEL] := DEFAILEVEL;
    LogToFile("::log/ailog.log", "WARNUNG: " + Me.npctemplate + " hat keinen ailevel eintrag!");
  EndIf

  params[IDLESOUND]:=mytemplate.idlesound;
  params[ANGERSOUND]:=mytemplate.angersound;
  If(params[IDLESOUND]<0)
    LogToFile("::log/ailog.log", "FEHLER: " + Me.npctemplate + " hat keinen idlesound Eintrag!");
    params[IDLESOUND] := 0;
  EndIf
  If(params[ANGERSOUND]<0)
    LogToFile("::log/ailog.log", "FEHLER: " + Me.npctemplate + " hat keinen angersound Eintrag!");
    params[ANGERSOUND] := 0;
  EndIf
  params[IDLESOUND]:=CInt(params.idlesound);
  params[ANGERSOUND]:=CInt(params.angersound);

  SleepMs(10); //entschärfen von Runaways

  If (CInt(mytemplate.spellcaster))  // Sprüche besorgen
    params.insert(SPELLCASTER,1);
    params.insert(SPELLS,{});
    params.insert(REFLECTCHANCE,0);
    Var temparray:=GetConfigStringArray(mytemplate,"spell");
    Var tempi;
    params[REFLECTCHANCE] := CInt(mytemplate.reflectchance);
    If (!params[REFLECTCHANCE])
    	params[REFLECTCHANCE] := 15;
    EndIf
    ForEach temp in temparray
      tempi:=SplitWords(temp);
      params[SPELLS].append({GetSpellIDAI(tempi[1]),CInt(tempi[2])});
    EndForeach
    If (params[SPELLS]=={})
      LogToFile("::log/ailog.log", "WARNUNG: " + Me.npctemplate +" besitzt keine Spells!");
    EndIf
    params.insert(CASTDELAY,CInt(mytemplate.castdelay));
    If (!params[CASTDELAY])
      LogToFile("::log/ailog.log", "WARNUNG: " + Me.npctemplate +" besitzt keinen CastDelay!");
      params[CASTDELAY]:=DEFCASTDELAY;
    EndIf
    params.insert(FIRSTCAST,1);
    var i,spell;
    For (i:=1;i<=params[SPELLS].size();i+=1)  // nach Reflection/Heilung/Cure/Dispelfield suchen damit es nicht jedesmal gemacht werden muß
      spell:=params[SPELLS][i];
      Case (spell[1])
        SPELLID_COMMON_REFLECT:
          params.insert(SPELLREFLECT,1);  // name muß man nicht speichern
          params[SPELLS].erase(i);
          i-=1;
        SPELLID_CLERIC_MEDITATION:
        SPELLID_CLERIC_HEAL:
        SPELLID_CLERIC_GREATERHEAL:
        SPELLID_CLERIC_ARCHHEAL:
          If (!spell[2]) // Hat ne Wahrscheinlichkeit also nicht extra Speichern
            If (params[SPELLHEAL])
              params[SPELLHEAL].append(spell[1]);
            Else
              params.insert(SPELLHEAL,{spell[1]});
            EndIf
            params[SPELLS].erase(i);
            i-=1;
          EndIf
        SPELLID_CLERIC_CURE:
        SPELLID_CLERIC_ARCHCURE:
          If (params[SPELLCURE])
            params[SPELLCURE].append(spell[1]);
          Else
            params.insert(SPELLCURE,{spell[1]});
          EndIf
          params[SPELLS].erase(i);
          i-=1;
        SPELLID_CLERIC_DISPELFIELD:
          params.insert(SPELLDISPEL,spell[2]);  // Da interessiert nur die Wahrscheinlichkeit
          params[SPELLS].erase(i);
          params.insert(params[CASTDISPEL],0);
          i-=1;
        default:
      EndCase
      SleepMS(2);
    EndFor

    If (!Me.getprop("MagicCastPower")) // nicht vorhanden also aufbauen
      temparray:=GetConfigStringArray(mytemplate,"spellpower");
      If (temparray)
        ForEach power in temparray
          power:=SplitWords(power);
          If (power.size()==3) // Allgemein
            SetCastPowerProperty(Me, power[1], power[2], "", "", CInt(power[3]));
          ElseIf (power.size()==4) // Klasse
            SetCastPowerProperty(Me, power[1], power[3], power[2], "", CInt(power[4]));
          ElseIf (power.size()==5)  // Subklasse dabei
            SetCastPowerProperty(Me, power[1], power[4], "", power[3], CInt(power[5]));
          EndIf
        EndForEach
      EndIf
    EndIf
    If (!mytemplate.saywords)
      saywords:=0;  // sonderwurst, soll nen Texterscheinen beim Casten
    EndIf
  EndIf

  SleepMs(10); //entschärfen von Runaways

  InitParamsSpezial(mytemplate);  // Override

  EnableEvents(SYSEVENT_ENGAGED);
  EnableEvents(SYSEVENT_DAMAGED);
  EnableEvents(SYSEVENT_ENTEREDAREA, params[RANGESENSE]);
EndFunction

/////////////////////////////////////////////////
// LookForNextOpponent - sucht nach einem Gegner
/////////////////////////////////////////////////
Function LookForNextOpponent( range, groupmode:=0, opplist:={}, x:=0, y:=0, z:=0)
  Var opponentList:=array;
  If( GetObjProperty(Me,PROP_MASTER) )
    If( GetObjProperty(Me,PROP_GUARD) ) //zahme viecher im guard/kill-modus greIfen an was dem Meister feindlich gesinnt ist
      foreach mob in ListHostiles( Me.master, range, LH_FLAG_LOS )
        If( (mob.cmdlevel < CMDLEVEL_SEER) )
          opponentList.append(mob);
        EndIf
        SleepMs(5); //bei vielen Mobs gibt es sonst Probleme
      endforeach
    EndIf
  Else //wilde Viecher
    If (groupmode)
      //Suche um eine Position herum, wird bei Pack genutzt um die effektive Kampfreichweite zu erhöhen, Dominoeffekt
      foreach mob in ListMobilesNearLocation( x, y, z, range,Me.realm ) //spaeter auf hate-love achten
        If (ValidEnemy(mob))
          TestMessage("MainLoop: Add Gegner(1): "+mob.name, 1);
          If(!(mob in opplist))
            opponentList.append(mob);
          EndIf
        EndIf
        SleepMs(5); //bei vielen Mobs gibt es sonst Probleme
      endforeach
    Else
      //Suche primär nach Sichtlinie, alternativ ohne
      foreach mob in ListMobilesInLineOfSight( Me, range ) //spaeter auf hate-love achten
        TestMessage("MainLoop: Add Gegner(2)?: "+mob.name, 1);
        If(ValidEnemy(mob))
          TestMessage("MainLoop: Add Gegner LOS: "+mob.name, 1);
          If(!(mob in opplist))
            opponentList.append(mob);
          EndIf
        EndIf
        SleepMs(10); //bei vielen Mobs gibt es sonst Probleme
      endforeach
      If (!opponentList.size())
        //auch auf Gegner hinter Deckung achten, bei Enter Area reagiert der NPC ja auch unabhängig von LOS
        foreach mob in ListMobilesNearLocation( Me.x, Me.y, Me.z, range,Me.realm ) //spaeter auf hate-love achten
          If(ValidEnemy(mob))
            TestMessage("MainLoop: Add Gegner(3): "+mob.name, 1);
            If(!(mob in opplist))
              opponentList.append(mob);
            EndIf
          EndIf
          SleepMs(5); //bei vielen Mobs gibt es sonst Probleme
        endforeach
      EndIf
    EndIf
  EndIf

  TestMessage("MainLoop: Anzahl Gegner: "+CInt(opponentList.size()), 2);
  If ( opponentList.size() > 0 )
    Var opponent:=opponentList.randomentry();
    TestMessage("MainLoop: Neuer Gegner gefunden: "+opponent.name, 1);
    Return opponent;
  Else
    TestMessage("MainLoop: Kein Gegner gefunden.", 1);
    Return 0;
  EndIf
EndFunction

/////////////////////////////////////////////////
// LookForNextHostile - sucht nach einem der Angreifer des Opfers
/////////////////////////////////////////////////
Function LookForNextHostile( byref range, byref opfer)
  Var opponentList:=array;

  ForEach mobile in ListHostiles(opfer, range, 0)
    If (ValidEnemy(mobile, 1))
      opponentList.append(mobile);
    EndIf
  EndForEach

  TestMessage("LookForNextHostile: Anzahl Gegner: "+CInt(opponentList.size()), 2);
  If ( opponentList.size() > 0 )
    Var opponent:=opponentList.randomentry();
    TestMessage("LookForNextHostile: Neuer Gegner gefunden: "+opponent.name, 1);
    Return opponent;
  Else
    TestMessage("LookForNextHostile: Kein Gegner gefunden.", 1);
    Return 0;
  EndIf
EndFunction

Function ValidEnemy(byref mob, hostile:=0)
  If(!mob.isa(POLCLASS_NPC))
    wandersteps:=params[IDLESTEPS]; //sind Player in der Nähe aktiv bleiben
  EndIf

  Return(ValidEnemy_Override(mob, hostile));  //Override
EndFunction

Function MakeMePeaceful(byref range)
  SetWarmode( 0 );
  SetOpponent( 0 );
  EnableEvents(SYSEVENT_ENTEREDAREA, range); //wieder umgebung wahrnehmen
  DisableEvents( SYSEVENT_OPPONENT_MOVED ); //ohne Gegner nicht von interesse
  //EraseObjProperty(Me,PROP_BLOCKED); //blocked nicht löschen, herdentier kannst sonst bei feind zwischen sich und leittier nicht zum leittier finden
  EraseObjProperty(Me,PROP_PATHFIND);
  EraseObjProperty(Me,PROP_TRAPPED);
  EraseObjProperty(Me,PROP_OPPONENT);
  If (params.exists(SPELLCASTER))
    params[FIRSTCAST]:=1; // friedlich also darf er wieder casten
    If (params[CASTDISPEL])
      params[CASTDISPEL]:=0;
    EndIf
  EndIf
EndFunction

Function TestEvents(byref ort, byref stufe, byref ev)
  Var talkme:=GetObjProperty(Me, "talkme");
  If (talkme)
    If (CInt(talkme.stufe) >= CInt(stufe))
      Var who:=SystemFindObjectBySerial(talkme.serial);
      If (who.ip)
        ort:=me.name+": "+ort;
        If (ev==0)
          SendSysMessagePergon(who,ort +" Event: Kein Event");
        Else
          Case (ev.type)
            SYSEVENT_ENGAGED:
                SendSysMessagePergon(who,ort +" Event: Engaged");
            SYSEVENT_DAMAGED:
                SendSysMessagePergon(who,ort +" Event: Damaged");
            EVID_PEACEMADE:
                SendSysMessagePergon(who,ort +" Event: Peacemade");
            SYSEVENT_DISENGAGED:
                SendSysMessagePergon(who,ort +" Event: Disengaged");
            SYSEVENT_OPPONENT_MOVED:
                SendSysMessagePergon(who,ort +" Event: Opponent moved");
            SYSEVENT_ENTEREDAREA:
                SendSysMessagePergon(who,ort +" Event: Entered Area");
            SYSEVENT_LEFTAREA:
                SendSysMessagePergon(who,ort +" Event: Left Area");
            SYSEVENT_SPEECH:
                SendSysMessagePergon(who,ort +" Event: Speech");
            EVID_LOOKAROUND:
                SendSysMessagePergon(who,ort +" Event: Look around");
            EVID_FOLLOW:
                SendSysMessagePergon(who,ort +" Event: Follow");
            EVID_CAST_WAIT:
                SendSysMessagePergon(who,ort +" Event: Cast Wait");
            default: SendSysMessagePergon(who,ort +" Event: Unbekannt");
          EndCase
        EndIf
      EndIf
    EndIf
  EndIf
EndFunction

// Zu welchem Typus gehört die Grafik?
// Definierte Animationen unterscheiden sich dort
Function GetMobType()
  var cfg:=ReadConfigFile("::animxlate");
  ForEach elem in { {"Human",MOB_HUMAN}, {"Sea", MOB_SEA}, {"Animal",MOB_ANIMAL}, {"Monster",MOB_MONSTER} }
    If (Me.graphic in GetConfigIntArray(cfg[elem[1]],"Graphic"))
      Return elem[2];
    EndIf
  EndForEach
  Return MOB_UNKNOWN;
EndFunction
