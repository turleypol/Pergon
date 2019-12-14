/////////////////////////////////////////////////////////////////////
// 20.12.2006 Fox -- Townguard spezIfisches für Main_AI
//
// 15.05.07   Fox: im Nahkampf wurden die Events nicht gespeichert
/////////////////////////////////////////////////////////////////////

///////////////
// Konstanten
///////////////

Var IGNORE_ANIMAL_SCRIPT:={"enticedanimal", "tamed", ":tierzucht:w_domest"};
Var REAKTIONS_ZEIT:=1000; //1500; //Zeitverzögerung zwischen Sichtung und Angriff
CONST GUARDBOW:=0x6c18;
CONST ARROW:=0xf3f;
CONST UOBJ_CITYCASHBOX:=0x669c;
CONST TOWNGUARDBILL:=30000;

CONST MAX_PATHLENGTH:=100; //Schritte die ein Guard sich maximal vom Startpunkt entfernt
CONST MAX_FOLLOW_STEPS:=30; //Schritte die ein Guard maximal folgt
CONST FAR_STEPS_DIST:=4; //Schritte innerhalb denen sich der Abstand verringern muss damit nicht in den Fernkampfmode gegangen wird
CONST FAR_STEPS:=5; //die Schritte die mindestens gemacht sein müssen bevor in den Fernkampfmode gegangen wird

Const MUTE_TIME:=180;

Const LOOK_PLUS:=5; //Bei Folgemodus und Stop etwas weiter schauen

/////////////////////////////////////////////////
// InitGuard - Namens- und Ausrüstungserstellung
/////////////////////////////////////////////////

Function InitGuard()
  TestMessage("InitGuard", 1);
  Var npcname:=Me.name;
  If (Me.name["<random>"])
    npcname:=RandomName(Me)+NPCTitle(Me); //NPCTitle setzt auch _rich und _poor für Equipt <- Bedeutung für Stadtwache?
    SetName(Me, npcname);
  EndIf

  Var pos:=Find(npcname, ",", 1);
  If (pos)
    npcname:=npcname[1, pos-1];
  EndIf
  SetObjProperty(Me, "myname", npcname);

  Var equip:=GetObjProperty(Me, "Equipt"); // Wache mit Klamotten ausstatten? Orks sind Nahkämpfer, brauchen z.B. keine Ausstattung?
  If (equip)
    TestMessage("InitGuard: Equippen", 2);
    Var equip2cfg:=ReadConfigFile("::equip2");
    If (equip2cfg)
      //Über Spawnnet die Rune finden damit die Spawnposition und damit wiederum den Placename (z.B. Vesper)
      Var spawnnet:=GetObjProperty(Me, "spawnnet"); // Pruefen, ob die Wache durch eine SpawnRune erzeugt wurde
      If (!spawnnet)
        Var countdown:=10;
        While (countdown And (!spawnnet))
          SleepMS(100);countdown-=1;
          spawnnet:=GetObjProperty(Me, "spawnnet");
        EndWhile
      EndIf

      Var guardkoords:=Me; // PlaceName entweder anhand der eigenen Position oder der SpawnRune bestimmen
      If (spawnnet)
        Var spawnrune:=SystemFindObjectBySerial(CInt(spawnnet[1])); // SpawnRune suchen
        If (spawnrune)
          guardkoords:=spawnrune;
        EndIf
      EndIf
      Var guardplace:=PlaceName(guardkoords);

      If (!Me.getprop("npcstarted")) //Ausrüstung nur beim create ermitteln
        Var mount:="";
        Var place:=lower(params[STARTZONE].name);
        If ((!RandomInt(4))&&!((place=="gefaengnis")||(place=="cove")||(place=="nirdana"))) // Guard steigt ggf. auf Pferdchen auf
          Me.setprop("beritten",1);
          mount:="Horse";
        EndIf

        If (equip2cfg[equip+guardplace+mount]) // Existiert dieses Template?
          equip+=guardplace+mount;
        ElseIf (equip2cfg[equip+guardplace]) // Oder das?
          equip+=guardplace;
        ElseIf (equip2cfg[equip+mount]) // Oder zumindest dies?
          equip+=mount;
        EndIf
      EndIf

      SetObjProperty(Me, "Equipt", equip); // Mal speichern für spaetere Analysen...
      Habille(Me, equip);                  // Die Wache einkleiden..., alte Ausrüstung im Backpack wird entfernt

      //Die Pferde zufällig auswählen deshalb nicht per Mount Eintrag in der equip2.cfg
      If (Me.getprop("beritten")) // Nun aber auch aufsteigen...
        Var horses:={"horse", "horse2", "horse3", "horse4", "horse_m"};

        Var horse:=CreateNPCFromTemplate(horses.randomentry(), Me.x, Me.y, Me.z,0,Me.realm);
        If (horse)
          PetMount(horse, Me, Me);
          Me.run_speed:=250;
        EndIf
      EndIf

      equip2cfg:=0;  //cfg Referenz löschen
    Else
      SysLog("FEHLER: Konnte die Datei equip2.cfg nicht oeffnen!");
      LogToFile("::log/ailog.log", "[guards.inc] [" + GetDateTimeStr() + "] FEHLER: Konnte die Datei equip2.cfg nicht oeffnen!");
    EndIf
  EndIf

  SleepMS(50);
  //Jetzt unterschied ob Serverneustart oder Create
  If (!Me.getprop("npcstarted"))
    //Wo gehoere ich hin
    TestMessage("InitGuard: Create", 2);
    Var place:=lower(params[STARTZONE].name);
    If (SubstractBill(place)) //Abziehen der Erstellungssumme aus der Stadtkasse
      Me.setprop("npcstarted",1);
      Me.setprop("bezahlt",1);
    ElseIf (
      (place=="gefaengnis") || (place=="goetterinsel") ||
      (place=="jhe'lom sued") || (place=="jhe'lom") ||
      (place=="nirdana")
    )
      Me.setprop("npcstarted",1);
    Else
      killmobilesilent(Me,"Stadt "+place+" besitzt kein Gold mehr");
    EndIf
  EndIf

  If (Me.getprop("bezahlt"))
    TestMessage("InitGuard: Wache ist bezahlt", 2);
    //Hier jetzt noch Zusatzausruestung hinzufuegen (Pfeile und Bogen)
    //Zweitwaffe gibt es nur für zahlende Städte
    If (!Me.backpack)
      //Neues Backpack anlegen
      Var newbackpack:=CreateItemAtLocationPergon(Me.x, Me.y, Me.z, 0xe75, 1,Me.realm);
      If (!EquipItem(Me, newbackpack))
        SysLog("Stadtgardist: "+Lower(Hex(Me.serial))+" mag kein Backpack");
        LogToFile("::log/ailog.log", "[guards.inc] [" + GetDateTimeStr() + "] FEHLER: Stadtgardist: "+Lower(Hex(Me.serial))+" mag kein Backpack");
      EndIf
    EndIf
    CreateItemInBackpack(Me,GUARDBOW,1); //was stellt sicher das der nicht bei Laggs gelootet werden kann bevor die Leiche gelöscht wird? !!!
    CreateItemInBackpack(Me,ARROW,1000);
  EndIf
EndFunction

/////////////////////////////////////////////////
// OnStartGuard - Startvorbereitungen
/////////////////////////////////////////////////
Function OnStartGuard()
  //CProps löschen
  EraseObjProperty(Me, PROP_FOLLOW);
  EraseObjProperty(Me, FIGHT_INFO);

  //Notwendigkeit der medata ermitteln !!!
  //medata.+minename:=GetObjProperty(me, "myname");    <- Wird beim Speech Kram genutzt, Alternative: Property nutzen
  //medata.+minelocationinfos:=PlaceInfos(me);
  //medata.+minelocation:=(medata.minelocationinfos).name;
  //medata.+talktome:=0; //sollte jeweils lokal geprüft werden, dürfte eh nur Speechkram betreffen
  //medata.+mute:=0;     //typischer CProp Kandidat
  //medata.+npcpath:=0;

  //verankern
  DropAnchor();
  Var ev:=struct;
  ev.+type:=EVID_LOOKAROUND;
  ev.+source:=Me;
  SendEvent(Me,ev);
EndFunction

/////////////////////////////////////////////////
// OnEnteredAreaEvent - Umgang mit SYSEVENT_ENTEREDAREA
/////////////////////////////////////////////////
Function OnEnteredAreaEvent(byref ev)

  If (!(ev.source.isa(POLCLASS_NPC)))
    wandersteps:=params[IDLESTEPS];//Bei Playern Wandern aktivieren
  EndIf

  If (CheckIgnore(ev.source))
    SayPergon(ev.source, "Ich hab nix gesehen.");
    Return;
  EndIf


  Var lets_fight:=0;
  //Fuer Quest wenn wanted und Questitems die wanted machen
  Case(CInt(GetObjProperty(ev.source, "quest_mostwanted")))
    1:
      If (RandomInt(2))
        lets_fight:=1;
      EndIf
    2:
      lets_fight:=1;
    default:
  endcase
  //Neues Townguardkonzept, Angreifen von Steckbrieflich gesuchten (bountpaper.src)
  Var place:=lower(params[STARTZONE].name);
  Var wanted:=ev.source.getprop("city_wanted");
  If (wanted[place])
    If (wanted[place] > ReadGameClock())
      lets_fight:=1;
    Else
      wanted.erase(place);
    EndIf
  EndIf

  //prüft ob der Gegner ins Angriffsschema passt
  Var opponent;
  If (lets_fight)
    opponent:=CheckOpponent(ev.source, 1);
  Else
    opponent:=CheckOpponent(ev.source, 0);
  EndIf

  If (opponent) //den Gegner angreifen
    NPCSayText(me, ev.source.name+", ich werde euch stellen!", "", ev.source);
    SleepMS(REAKTIONS_ZEIT);//Reaktionszeit zwischen Sichtung und AngrIff
    FightGuard( opponent);
  ElseIf (CheckLineOfSight(Me, ev.source))  //friedlich mit dem Gesichteten interagieren
    If ((params[STARTZONE].evilzone==(ev.source).murderer) And
      ((ev.source).murderer==(ev.source).criminal)         //hm was soll das bringen ???
    ) 
      If (!opponent.isA(POLCLASS_NPC)) //nur Player
        Salute(me, ev.source);
      EndIf
    EndIf

    If (IsNight())
      If (!RandomInt(8))
        NPCSayText(me, Answering(
        "wachen_halt", params[STARTZONE].name, me
        ), "", ev.source);
      EndIf
    EndIf
    
    var pass := ev.source.getprop("pass");
    If ((!(ev.source).murderer) And ((ev.source).criminal) or
      (params[STARTZONE].name == pass.town)
    )
      NPCSayText(me, "Ich behalte Euch genau im Auge!", "", ev.source);
    EndIf
  EndIf
EndFunction

/////////////////////////////////////////////////
// FollowPlayer - Folgeschleife
/////////////////////////////////////////////////
Function FollowPlayer(byref master_serial)
  //ähnlich wie die Kampfschleife
  //Events sorgen für Verlassen
  //irgendwelche Folgedistanzkriterien sorgen für Verlassen
  //wenn Folgen noch gültig beim Verlassen ein FolgenEvent werfen
  TestMessage("FollowPlayer()", 1);
  Var master:=SystemFindObjectBySerial(CInt(master_serial));
  If (!master)
    //Fehlerfall
    EraseObjProperty(Me, PROP_FOLLOW);
  EndIf

  Var walkback_path:=array;
  Var lastx:=me.x;
  Var lasty:=me.y;

  Var zw:=GetObjProperty(Me,WALKBACK);
  If (zw)
    walkback_path:=zw; //wird auch fortgesetzt wenn vorher ein Follow lief
  Else
    walkback_path:=array;
    walkback_path.append({lastx, lasty});
  EndIf

  Var timeout:=ReadGameClock()+2;
  Var fortsetzen:=1;
  Var event_merker:=array; //merkt sich die events die während des wartens ausgelesen wurden um sie wieder einzuspeisen
  Var ev;

  SetAnchor(me.x, me.y, 0, 0); //Anker deaktivieren, verursacht sonst Probleme weil nicht im Warmode

  Var last_instr:=Me.process.instr_cycles;
  while ((ReadGameClock()<timeout)||(event_merker.size() == 0)) //nur wenn die Zeit rum ist und eine Event aufgelaufen ist beenden
    TestMessage("FollowPlayer: instr: " + CStr(Me.process.instr_cycles-last_instr), 3);
    last_instr:=Me.process.instr_cycles;

    If( (master) && (Distance( Me, master ) <= (params[RANGESENSE] * 2)) && (walkback_path.size()<=MAX_FOLLOW_STEPS) && !(master.hidden) && !(master.concealed) && !(master.dead))
      If((Distance( Me, master ) > 2))
        GoToMob(master, 2);
        //wenn der Guard getrappt ist irgendwas unternehmen?
        If ((me.x!=lastx)||(me.y!=lasty))
          //hat sich bewegt
          TestMessage("FollowPlayer: hat sich bewegt",3);
          lastx:=me.x;lasty:=me.y;
          walkback_path.append({lastx, lasty});
        EndIf
      Else
        StepSleep(5, 0);
      EndIf
    Else
      //der Master ist entschwunden Ende mit Folgen
      TestMessage("FollowPlayer: Master weg", 3);
      EraseObjProperty(Me, PROP_FOLLOW);
      fortsetzen:=0;
      break;
    EndIf
    //Events merken die inzwischen angefallen sind
    //sorgt dafür das nach Ablauf der Mindestzeit die SchleIfe verlassen wird
    //(liegt mehr als 1 vor kann die Reihenfolge verändert werden, das Event-System sollte aber Reihenfolgeunabhängig sein)
    ev:=Wait_for_Event(0); //kein Eventlogging da diese eh wieder eingespeist werden
    If (ev)
      TestEvents("FollowPlayer: ", 1, ev);
      event_merker.append(ev);
    EndIf
  endwhile

  DropAnchor(0); //reaktiviert den Anker
  SetObjProperty(Me, WALKBACK, walkback_path);

  If (fortsetzen)
    SetObjProperty(Me, PROP_FOLLOW, master_serial);
    Var ev:=struct;
    ev.+type:=EVID_FOLLOW;
    ev.+source:=Me;
    ev.+master:=master_serial;
    event_merker.append(ev); //wichtig das es als letztes in der Mainloop ausgewertet wird sonst werden die anderen Events nicht bearbeitet
  EndIf

  //Events einspeisen
  TestMessage("FollowPlayer: Events einspeisen: "+event_merker.size(),3);
  Foreach ev in event_merker
    SendEvent(Me,ev);
  EndForeach

  If (!fortsetzen)
    If (walkback_path.size() > MAX_FOLLOW_STEPS)
      NPCSayText(Me, "Ich darf meinen Wachbereich nicht verlassen.", "", master);
    ElseIf (Distance( Me, master ) > (params[RANGESENSE] * 2))
      NPCSayText(Me, "Ihr seid zu schnell fuer mich!", "", master);
    EndIf
    MakeMePeaceful_Guard();
  EndIf
EndFunction


/////////////////////////////////////////////////////////
// CheckOpponent - Prüft auf potentiellen Gegner
// Parameter: hostiles - 1 Angriffsregeln nicht beachten
//            mobile   - ist mobile Gegner?
/////////////////////////////////////////////////////////

Function CheckOpponent(byref mobile, byref hostiles)
  Var result:=0;

  If (mobile.cmdlevel < CMDLEVEL_SEER)
    SleepMS(RandomInt(20));
    If (hostiles)
      result:=mobile;
      // ElseIf (GetObjProperty(mobile, "guardkill"))
      //   result:=mobile;
    Else
      If (mobile.isa(POLCLASS_NPC))
        If ((GetNPCConfig(mobile.npctemplate).guardignore != 1) && (!(mobile.script in IGNORE_ANIMAL_SCRIPT)) &&
          (CInt(GetObjProperty(mobile, "guardignore")) != 1))
          result:=mobile;
        EndIf
      Else
        If ((mobile.getprop("longmurders")>4) && (!mobile.getprop("permared"))) // Vogelfreier
          result:=mobile;
        ElseIf  (!params[STARTZONE].neutral) // Ref InitParamsSpezial
          If (params[STARTZONE].evilzone)
            If (!mobile.criminal)
              result:=mobile;
            EndIf
          Else
            If (mobile.murderer)
              result:=mobile;
            EndIf
          EndIf
        EndIf
      EndIf
    EndIf
  EndIf
  
  var pass := mobile.getprop("pass");
  If (pass and !mobile.IsA(POLCLASS_NPC) and !hostiles)
    If (params[STARTZONE].name == pass.town)
    	result := 0;
    EndIf
  EndIf
  
  If (result)
    If (params[ANGERSOUND])
      PlaySoundEffect(me, params[ANGERSOUND]);
    EndIf
  EndIf
  Return (result);
EndFunction

/////////////////////////////////////////////////////////
// OnIdleGuard - Aktion wenn kein Event anfällt
/////////////////////////////////////////////////////////
Function OnIdleGuard()
  TestMessage("OnIdleGuard()", 1);
  If(Me.warmode)
    Var opp := SystemFindObjectBySerial(GetObjProperty(Me,PROP_OPPONENT));
    If(opp)
      //dann kaempfe ich weiter mit dem aktuellen opponent
      FightGuard( opp );
    Else
      //mein aktueller gegner ist weg -> da such ich mir wen neues
      Var newev:=struct;
      newev.+type := EVID_LOOKAROUND;
      newev.+source:=Me;
      SendEvent(Me,newev);
    EndIf
  Else
    TestMessage("OnIdleGuard: Wandersteps="+wandersteps, 4);
    If (wandersteps > 0) //wandersteps=0 ist Tiefschlaf
      WanderPergon();
      wandersteps-=1;
    EndIf
    /*If (medata.npcpath)
    If (!medata.talktome)
    NPCWalkPath(me);
    EndIf
    SleepMS(10);
    EndIf

    If (ReadGameClock()>=next_wander)
    next_wander:=ReadGameClock()+10;
    If ((!medata.npcpath) And (!medata.talktome))
    WanderPergon();
    EndIf

    If (GetObjProperty(me, "looter"))
    GrabLoot();
    EndIf
    EndIf*/
  EndIf
EndFunction

/////////////////////////////////////////////////
// FightGuard - Kampfschleife
/////////////////////////////////////////////////

Function FightGuard( byref opponent )
  /*
  - Fernkampf nur im stehen, kein Verfolgungsmodus?
  - Fernkampf wann?
  - min 5 Schritte gelaufen
  - Entfernung zum Gegner kann nicht verringert werden
  - LineOfSight gegeben
  - Wenn die Aktion gegen einen Gegner abgeschlossen ist muss die Verfolgungscprop gelöscht werden
  - FIGHT_INFO
  - follow_steps (werden bei Verfolgungsschritten hochgezählt und bei Dist < 1 auf 0 gesetzt)
  - dist_steps (Anzahl Schritte seit denen keine Distanzverringerung zum Gegner möglich war)
  - path (Weg zum zurückfinden zum Startpunkt)
  - fightmode (0..Nahkampf, 1..Fernkampf)
  - Wenn eine Stadtwache angegriffen wird auch Gegnerwechsel vorsehen bei Damage
  - Stadtwachen hören nicht auf Speech im Kampf (Event deaktivieren)
  - Wenn ein Gegner nicht zu erreichen ist dann diesen auf eine Ignore Liste setzen (1h Dauer).
  Nicht für Damage einsetzen, dann kann ja theoretisch zurück feuern.
  */

  TestMessage("FightGuard()", 1);
  If (!(opponent))
    SetWarMode(0);
    SleepMs(100);
  EndIf

  wandersteps:=params[IDLESTEPS]; //nach nem Kampf Rumwandern
  EraseObjProperty(Me, PROP_FOLLOW); //Folgen abbrechen

  //Prüfen ob es sich um einen Guard im passiven Bogen Modus handelt
  Var far_only:=0;
  If (GetObjProperty(Me, "far_only"))
    far_only:=1;
  EndIf

  Var timeout:=ReadGameClock()+2;
  Var ret_wert:=0;
  Var event_merker:=array; //merkt sich die events die während des wartens ausgelesen wurden um sie wieder einzuspeisen
  Var ev;

  Var lastx:=me.x;
  Var lasty:=me.y;

  Var fight_info:=struct;
  fight_info.+follow_steps;
  fight_info.+dist_steps;
  fight_info.+last_dist;
  fight_info.+fightmode;
  fight_info.+ignore_count;
  Var walkback_path:=array;

  Var zw:=GetObjProperty(Me,WALKBACK);
  If (zw)
    walkback_path:=zw; //wird auch fortgesetzt wenn vorher ein Follow lief
  Else
    walkback_path:=array;
    walkback_path.append({lastx, lasty});
  EndIf

  zw:=GetObjProperty(Me,FIGHT_INFO);
  If (zw && Me.warmode)
    //Kampf fortsetzen
    TestMessage("FightGuard: Kampf fortsetzen", 3);
    fight_info:=zw;
  Else
    //Neuen Kampf beginnen
    TestMessage("FightGuard: Neuer Kampf", 1);
    EraseObjProperty(Me,FIGHT_INFO);
    fight_info.follow_steps:=0;
    fight_info.dist_steps:=0;
    fight_info.last_dist:=Distance(me, opponent);
    fight_info.fightmode:=0;
    fight_info.ignore_count:=0;
    ChangeWeapon(0);
    MakeAlertCall();
  EndIf

  If(!Me.warmode)
    SetOpponent( opponent );
    SetObjProperty(Me,PROP_OPPONENT, opponent.serial); //aktuellen opponent merken
    SetWarmode(1);
  EndIf

  DisableEvents(SYSEVENT_ENTEREDAREA+SYSEVENT_LEFTAREA+SYSEVENT_SPEECH); //waehrend ich kaempfe interessiert mich das nicht

  If (opponent.isa(POLCLASS_NPC)) //sonst könnte man sich damit gegen Looten schützen
    SetObjProperty(opponent, "guardkill", 1);
  EndIf

  Var start_time;
  Var last_instr:=Me.process.instr_cycles;
  Var feedback:=struct;
  Var end_fight:=0;
  while((ReadGameClock()<timeout)||(event_merker.size() == 0)) //nur wenn die Zeit rum ist und eine Event aufgelaufen ist beenden
    TestMessage("FightGuard: round_fight instr: " + CStr(Me.process.instr_cycles-last_instr), 2);
    last_instr:=Me.process.instr_cycles;

    If( (opponent) && (Distance( Me, opponent ) <= params[RANGEFOLLOW]) && !(opponent.hidden) && !(opponent.concealed) && !(opponent.dead) && (walkback_path.size() < MAX_PATHLENGTH) && !(end_fight))
      TestMessage("------------------", 2);
      If ((Distance( Me, opponent ) < 2 )&&(abs(Me.z-opponent.z) < 15)) //falls ich nicht schon vor dem Gegner stehe, renn ich hin
        TestMessage("FightGuard: nahkampf",3);
        ChangeWeapon(0);
        fight_info.follow_steps:=0;
        fight_info.dist_steps:=0;
        fight_info.fightmode:=0;
        EraseObjProperty(Me, PROP_BLOCKED);
        EraseObjProperty(Me, PROP_PATHFIND);
        //Warten auf Aktion
        EnableEvents( SYSEVENT_OPPONENT_MOVED ); //dann muss man nicht ständig prüfen ob sich der Gegner bewegt
        StepSleep(1, 0);
        ev:=Wait_for_Event(5); //wenn nix los ist, der Gegner still steht warum hetzen, also besser ausruhen
        If (ev)
          If (ev.type!=SYSEVENT_OPPONENT_MOVED)
            TestEvents("guards 1", 3, ev);
            event_merker.append(ev);
          EndIf
        EndIf
        DisableEvents( SYSEVENT_OPPONENT_MOVED );
      Else
        If (far_only)
          fight_info.fightmode:=1;
        EndIf
        If (fight_info.fightmode==0)
          TestMessage("FightGuard: hinrennen",3);
          start_time:=ReadMillisecondClock();
          MainRunToward(opponent, feedback); //kehrt nach jedem Schritt zurück <-- Problem eben nicht (an diesen Stellen Abbruchkriterium beachten)
          If (feedback.exist)
            If (feedback.plan==1)
              //Core Pathfind ist fehlgeschlagen, vermutlich ist das Ziel nicht erreichbar
              fight_info.ignore_count:=fight_info.ignore_count+1;
              TestMessage("FightGuard: ignore_count="+fight_info.ignore_count, 2);
              If (fight_info.ignore_count >= 5) //aufgeben und in Zukunft ignorieren
                TestMessage("FightGuard: Opponent Ignorieren", 1);
                Var ignoreList:=GetObjProperty(Me, IGNORE_LIST);
                If (!(ignoreList))
                  ignoreList:=dictionary;
                EndIf
                ignoreList[opponent.serial]:=ReadGameClock()+60*60; //1h
                SetObjProperty(Me, IGNORE_LIST, ignoreList);
                end_fight:=1;
              EndIf
            ElseIf (feedback.plan==2) //Pfad gefunden
              fight_info.ignore_count:=0;
            ElseIf (feedback.plan==3) //ist gefreezt
              fight_info.ignore_count:=fight_info.ignore_count+1;
              TestMessage("FightGuard: ignore_count="+fight_info.ignore_count, 2);
              If (fight_info.ignore_count >= 50) //aufgeben und in Zukunft ignorieren
                TestMessage("FightGuard: Opponent Ignorieren", 1);
                Var ignoreList:=GetObjProperty(Me, IGNORE_LIST);
                If (!(ignoreList))
                  ignoreList:=dictionary;
                EndIf
                ignoreList[opponent.serial]:=ReadGameClock()+60*60; //1h
                SetObjProperty(Me, IGNORE_LIST, ignoreList);
                end_fight:=1;
              EndIf
            EndIf
          EndIf
          fight_info.follow_steps+=Round(CalcTimeSteps(ReadMillisecondClock()-start_time, 1));
          TestMessage("FightGuard: follow_steps="+fight_info.follow_steps, 3);
          If ((me.x!=lastx)||(me.y!=lasty))
            //hat sich bewegt
            TestMessage("FightGuard: hat sich bewegt",3);
            lastx:=me.x;lasty:=me.y;
            walkback_path.append({lastx, lasty});
          EndIf
          If (fight_info.follow_steps >= FAR_STEPS)
            Var dist:=Distance(me, opponent);
            //Abstand muss innerhalb von FAR_STEPS_DIST Schritten verringert werden
            If (dist >= fight_info.last_dist)
              fight_info.dist_steps+=1;
              If (fight_info.dist_steps >= FAR_STEPS_DIST)
                If (CheckLineOfSight(Me, opponent))
                  //Jetzt auf Fernkampf setzen
                  ChangeWeapon(1);
                  fight_info.fightmode:=1;
                EndIf
              EndIf
            Else
              //Distanz verringert
              fight_info.last_dist:=dist;
              fight_info.dist_steps:=0;
            EndIf
          EndIf
        ElseIf (fight_info.fightmode==1)
          //Fernkampf
          //kein Verfolgungsmodus, schiest solange er das Ziel im Blick hat
          If (CheckLineOfSight(Me, opponent))
            ChangeWeapon(1);
            StepSleep(1, 1);
          Else
            fight_info.fightmode:=0; //ohne Sichtlinie wieder Verfolgen
            ChangeWeapon(0);
            fight_info.last_dist:=Distance(me, opponent);
            fight_info.dist_steps:=0;
            SleepMs(100);
          EndIf
        EndIf
      EndIf
      ret_wert:=1;
    Else
      //der Gegner ist weg/nicht erreichbar -> aufhoeren zu kaempfen -> zurück zum Startpunkt rennen
      //Wegekram der mit Gegner zu tun hat löschen
      EraseObjProperty(Me, PROP_PATHFIND);
      EraseObjProperty(Me, PROP_BLOCKED);

      ChangeWeapon(0); //Nahkampfwaffe anlegen

      If (walkback_path.size() >= MAX_PATHLENGTH)
        TestMessage("FightGuard: MAX_PATHLENGTH erreicht", 3);
      EndIf

      If (opponent)
        If (!opponent.isA(POLCLASS_NPC)) //nur Player
          If (opponent.dead)
            NPCSayText(Me, "Die Gerechtigkeit hat gesiegt.", "", opponent);
            LogToFile("::log/ailog.log", "Stadtwache hat "+opponent.name+" besiegt.");
          Else
            If (Distance( Me, opponent ) <= params[RANGEFOLLOW])
              NPCSayText(Me, "Euer Versteck ist gut, aber irgendwann erwische ich Euch schon!", "", opponent);
            Else
              NPCSayText(Me, "Ich werde Euch schon noch erwischen.", "", opponent);
            EndIf
          EndIf
        EndIf
      EndIf

      Var ev:=struct;
      ev.+type:=EVID_PEACEMADE; //heist für Stadtwachen auf den Weg zum Startpunkt machen
      ev.+source:=Me;
      event_merker.append(ev);

      SleepMs(30);
      ret_wert:=0;
      break;
    EndIf
    //Events merken die inzwischen angefallen sind
    //sorgt dafür das nach Ablauf der Mindestzeit die Kampffunktion verlassen wird
    //(liegt mehr als 1 vor kann die Reihenfolge verändert werden, das Event-System sollte aber Reihenfolgeunabhängig sein)
    ev:=Wait_for_Event(0); //kein Eventlogging da diese eh wieder eingespeist werden
    If (ev)
      If (ev.type!=SYSEVENT_OPPONENT_MOVED)
        TestEvents("guards 2", 3, ev);
        event_merker.append(ev);
      EndIf
    EndIf
  endwhile

  SetObjProperty(Me, FIGHT_INFO, fight_info);
  SetObjProperty(Me, WALKBACK, walkback_path);

  //Events einspeisen
  TestMessage("FightGuard: Events einspeisen: "+event_merker.size(),3);
  Foreach ev in event_merker
    SendEvent(Me,ev);
  EndForeach
  Return ret_wert;
EndFunction

/////////////////////////////////////////////////
// SubstractBill - Stadtkasse belasten
/////////////////////////////////////////////////
Function SubstractBill(byref place, bill := TOWNGUARDBILL)
  //Sollte eigentlich alle erwischen
  Var citycashbox;
  Var stadtkassen := ListItemsNearLocationOfType(2500, 2500, 10, 2500, UOBJ_CITYCASHBOX, REALM_BRITANNIA);
  foreach stadtkasse in stadtkassen
    If (place == lower(PlaceName(stadtkasse)))
      citycashbox := stadtkasse;
      break;
    EndIf
  endforeach
  Var gold := StrSubtraktion(GetObjProperty(citycashbox, "amountgold"), bill);
  If (gold)
    SetObjProperty(citycashbox, "amountgold", gold);
    Return 1;
  Else
    Return 0;
  EndIf
EndFunction

/////////////////////////////////////////////////
// Event_Speech - Verarbeitet alles Gesprochene
/////////////////////////////////////////////////
Function Check_TalkToMe(byref sprecher, byref voll_text, ByRef sayed)
  sayed:=Lower(voll_text);
  Var sayedarray:=SplitWords(sayed);
  //  Var sayedfirst:=sayedarray[1];
  Var talktome:=0;
  Var myname:=Lower(GetObjProperty(me, "myname"));
  If (sayedarray[1][Lower(myname)])//myname Element des ersten Wortes
    SetObjProperty(sprecher, "talkto", myname);  //dadurch fühlt sich die Stadtwache auch später noch angesprochen, wenn kein Name fällt
    talktome:=1;
  Else
    Var mobile:=ListMobilesNearLocationEx(sprecher.x, sprecher.y, sprecher.z, 14, LISTEX_FLAG_NORMAL,sprecher.realm); //Im Umkreis von 14 Feldern nach angesprochenen Suchen
    //nur wenn der Sprecher mit der Stadtwache alleine ist auch ohne Namen/talkto reagieren
    If (Len(mobile)==2)
      If (((mobile[1].serial==me.serial) And (mobile[2].serial==sprecher.serial)) Or
         ((mobile[2].serial==me.serial) And (mobile[1].serial==sprecher.serial)))
        SetObjProperty(sprecher, "talkto", myname);
        talktome:=1;
      EndIf
    EndIf
  EndIf

  SleepMs(100); //sollte dafür sorgen das der alte "talkto" nicht mehr antwortet weil die CProp inzwischen geändert ist

  If (GetObjProperty(sprecher, "talkto")==myname)
    talktome:=1;
  EndIf

  Return talktome;
EndFunction

Function Event_Speech_Guard(byref sprecher, byref text)
  //Für konkrete Kommandos
  Var sayed:=Lower(text);
  Var sayedarray:=SplitWords(sayed);
  Var sayedfirst:=sayedarray[1];
  Var myname:=Lower(GetObjProperty(me, "myname"));

  //Namen aus dem Text entfernen
  If (sayedarray[1]==myname)
    If (sayedarray[2])
      sayedfirst:=sayedarray[2];
      sayed:=sayedarray[2];

      Var i;
      For (i:=3; i<=Len(sayedarray); i+=1)
        sayed+=" "+sayedarray[i]; //setzt den Text nach dem Namen wieder zusammen
      EndFor
    Else
      sayed:="hallo";
    EndIf
  EndIf

  SleepMS(5);

  If (!GetObjProperty(me, "mutetime"))
    Case (sayed)
      "zeit": NPCSayText(me, "Es ist "+GetPergonTimeOfDay(), "", sprecher);
      "ort": NPCSayText(me, "Ihr seid in "+params[STARTZONE].name, "", sprecher);
      "name":
        NPCSayText(me, "Gestatten "+myname, "", sprecher);

      "fresse":
      "schnauze":
      "klappe":
      "ruhe":
      "schweig":
        SetObjProperty(me, "mutetime", ReadGameClock()+MUTE_TIME);
        NPCSayText(me, "Ok, ich bin ja schon stumm...", "", sprecher);
      "fehler":
        SysLog("TownGuard: Bin ein Bug #"+me.serial+" '"+myname+"' gefunden von #"+sprecher.serial);
        NPCSayText(me, "Bin als boeser Bug markiert...", "", sprecher);
      default:
        NPCSayText(me, Answering(
                       sayed, params[STARTZONE].name, me
                       ), "", sprecher);
    EndCase
  Else
    If (ReadGameClock()>=GetObjProperty(me, "mutetime"))
      EraseObjProperty(me, "mutetime");
    EndIf
  EndIf
EndFunction

/////////////////////////////////////////////////
// OnSpeechEvent - Umgang mit SYSEVENT_SPEECH
/////////////////////////////////////////////////
Function OnSpeechEvent(byref ev)
  If (!((ev.source).serial))
    Return;
  EndIf
  Var los:=CheckLineOfSight(me, ev.source);
  If (los)
    TurnToward(ev.source);
  EndIf

  Var text:="";
  Var talkme:=Check_TalkToMe(ev.source, ev.text, text);
  If (talkme && los)
    //Test auf Wortteil
    //Problem, Hierarchie oder mehrere Events gleichzeitig auslösen?
    If (text["guards"] || text["wache"] || text["hilfe"])
      SayPergon(ev.source, "Wo ist die Gefahr?");
      Var ev:=struct;
      ev.+type:=EVID_LOOKAROUND;
      ev.+source:=Me;
      ev.+help:=ev.source;
      SendEvent(Me,ev);
    ElseIf (text["verbrechen"])
      Var master_serial:=CInt((ev.source).serial);
      Var folgen:=1;
      //prüfen ob Sprecher schon jemanden im Schlepp hat
      ForEach mobile in ListMobilesNearLocationEx(ev.source.x, ev.source.y, ev.source.z, 15, LISTEX_FLAG_NORMAL+LISTEX_FLAG_HIDDEN,ev.source.realm)
        If (GetObjProperty(mobile, PROP_FOLLOW)==master_serial)
          If (!(mobile==Me))
            SayPergon(ev.source, "Ihr habt schon genug Hilfe!");
          EndIf
          folgen:=0;
          break;
        EndIf
        SleepMs(10); //Runaways verhindern bei vielen Mobs in der Gegend
      EndForEach
      If (folgen)
        SayPergon(ev.source, "Führt mich zum Ort des Verbrechens!");
        SetObjProperty(Me, PROP_FOLLOW, master_serial);
        LogToFile("::log/guards.log", "Folge Buerger: "+Lower(Hex(me.serial))+" "+
        ev.source.name+" "+master_serial+" "+ReadGameClock());
        Var ev:=struct;
        ev.+type:=EVID_FOLLOW;
        ev.+source:=Me;
        ev.+master:=master_serial;
        SendEvent(Me,ev);
      EndIf
    ElseIf (text["stop"]||text["halt"])
      /*unterbricht folgen, wie das dem Eventbasiertem System unterjubeln?
      <- geht nur über ne stop CProp am Guard
      <- dann sollte es beim Folgen auch eine geben  #Follow
      <- EVID_FOLLOW ist nur mit CProp gültig*/
      Var follow:=GetObjProperty(Me, PROP_FOLLOW);
      If (follow)
        If ((ev.source).serial==follow)
          SayPergon(ev.source, "Dann wollen wir mal sehen...");
          EraseObjProperty(Me, PROP_FOLLOW);
          Var ev:=struct;
          ev.+type:=EVID_LOOKAROUND;
          ev.+source:=Me;
          ev.+range:=params[RANGESENSE]+LOOK_PLUS;
          SendEvent(Me,ev);
        Else
          SayPergon(ev.source, "Unterlasst diese Scherze!");
        EndIf
      Else
        SayPergon(ev.source, "Unterlasst diese Scherze!");
      EndIf
    ElseIf (text["arrestant"] || text["gefangen"] || text["knast"])
      // Abfrage der gejailten Accounts
      Var jailarray:=GetGlobalProperty("jail");
      If (jailarray.size())
        PrintTextAbovePergon(ev.source, me, "Derzeit sind folgende Personen arrestiert:");
        Sleep(4);
        Var prisoner;
        ForEach arrestantserial in jailarray
          prisoner := SystemFindObjectBySerial(CInt(arrestantserial), SYSFIND_SEARCH_OFFLINE_MOBILES);
          PrintTextAbovePergon(ev.source, me, prisoner.name);
        EndForEach
        Sleep(1);
        PrintTextAbovePergon(ev.source, me, "<Ende der Liste>");
      Else
        PrintTextAbovePergon(ev.source, me, "Derzeit sind keine Personen arrestiert.");
      EndIf
    ElseIf (text["vorstrafe"])
      // Abfrage des eigenen Strafregisters // fuer Newbies und CmdLevel kostenlos
      Sleep(RandomInt(3));
      If (!GetObjProperty(ev.source, "#GetPunishments"))
        SetObjProperty(ev.source, "#GetPunishments", 1); // verhindert (zusammen mit dem Sleep()) zeitgleiches Bearbeiten der Anfrage von mehreren Stadtwachen
        Var bezahlt := 0;
        Var Beitrag := 1000;
        If (GetObjProperty(ev.source, TYPNEWBIE) || (ev.source.cmdlevel >= CMDLEVEL_SEER))
          Start_ScriptPergon("::textcmd/3_seer/punish", {"#SCRIPT", ev.source});
        Else
          If (ev.source.gold>=Beitrag)
            ev.source.spendgold(Beitrag);
            bezahlt := 1;
          Else
            bezahlt := 0;
          EndIf

          If (bezahlt)
            SayPergon(ev.source, "Danke fuer Euren kleinen finanziellen Beitrag!");
            SubstractBill(lower(params[STARTZONE].name), -Beitrag);
            Start_ScriptPergon("::textcmd/3_seer/punish", {"#SCRIPT", ev.source});
          Else
            SayPergon(ev.source, "Bringt bitte erst " + Beitrag + " Gold als Gebühr mit!");
            SayPergon(ev.source, "Vorher gibt es keine Informationen!");
          EndIf
        EndIf
        EraseObjProperty(ev.source, "#GetPunishments");
      EndIf
    Else
      If (len(text) >= 1)
        If (!(text[1] == "*" && text[len(text)] =="*"))  // auf Emotes nicht reagieren
          Event_Speech_Guard(ev.source, text);
        EndIf
      EndIf
    EndIf
  EndIf
EndFunction

//////////////////////////////////////////////////
// NPCSayText - Stellt die Sprache des NPC's dar
//////////////////////////////////////////////////

Function NPCSayText(byref npc, byref text, byref texteng,byref pc)
  Var shown:=0;
  If (pc.isA(POLCLASS_NPC)) //nichts zu NPCs sagen
    Return;
  EndIf
  ForEach mobile in ListMobilesNearLocationEx(npc.x, npc.y, npc.z, 6, LISTEX_FLAG_NORMAL+LISTEX_FLAG_HIDDEN,npc.realm)
    PrintTextAbovePrivatePergon(npc, text, texteng, mobile);
    If (mobile.serial==pc.serial)
      shown:=1;
      break;
    EndIf
    SleepMs(10); //Runaways verhindern bei vielen Mobs inder Gegend
  EndForEach

  If (!shown)
    PrintTextAbovePrivatePergon(npc, text, texteng, pc);
  EndIf
EndFunction

//////////////////////////////////////////////////
// MakeMePeaceful_Townguard - Herstellen des friedlichen Stadtwachenzustands
//////////////////////////////////////////////////

Function MakeMePeaceful_Guard()
  TestMessage("MakeMePeaceful_Guard()", 1);
  EnableEvents(SYSEVENT_SPEECH, params[RANGESENSE]);
  EnableEvents(SYSEVENT_DISENGAGED);

  EraseObjProperty(Me,FIGHT_INFO);

  Var walkback_path:=array;
  walkback_path:=GetObjProperty(Me, WALKBACK);
  //TestMessage("MakeMePeaceful_Guard: walkback_path="+walkback_path, 1);
  If (walkback_path)
    RunBackPath(walkback_path); //reagiert solange nicht auf Events
    Var ev:=struct;
    ev.+type:=EVID_LOOKAROUND;
    ev.+source:=Me;
    SendEvent(Me,ev); //da auf dem Rückweg Events gelöscht werden könnte nen Gegner am Rückkehrpunkt stehen und nicht detektiert werden
  EndIf
  EraseObjProperty(Me,WALKBACK);

  If (Me.opponent)
    EraseObjProperty(Me.opponent, "guardkill");
  EndIf

  MakeMePeaceful(params[RANGESENSE]);
EndFunction

//////////////////////////////////////////////////
// MakeMePeaceful_Townguard - Herstellen des friedlichen Stadtwachenzustands
//////////////////////////////////////////////////

Function RunBackPath(byref path)
  TestMessage("RunBackPath()", 1);
  //TestMessage("path.size="+path.size(), 1);
  SetAnchor(me.x, me.y, 0, 0); //Anker deaktivieren, verursacht sonst Probleme weil nicht im Warmode
  If (path.size())
    Var starttime:=ReadGameClock();
    //    Var nextlog:=starttime+30;
    Var timeout:=0;
    //Var stopped:=0;
    Var i:=path.size();
    Var dist:=0;
    Var newx:=0;
    Var newy:=0;
    While (i > 0)
      newx:=path[i][1];
      newy:=path[i][2];

      timeout:=ReadGameClock()+10;
      dist:=CInt(CoordinateDistanceEuclidean(me.x, me.y, newx, newy));
      Var olddist:=1000;
      Repeat
        TestMessage("RunBackPath: "+ starttime+" "+
        me.x+" "+me.y+" "+me.z+" -> "+newx+" "+newy+" - "+dist+" "+olddist+" "+ i + " - " + CInt(timeout-ReadGameClock()), 3);
        If (dist < olddist)
          timeout:=ReadGameClock()+10;
          olddist:=dist;
        EndIf

        If (dist > 0)
          RunTowardLocation(newx, newy);
        EndIf
        SleepMSClear(10);
        //Z wird nicht berücksichtigt, bei Benutzung von Treppen springt man aber auch nicht auf einen Schlag nach oben
        dist:=CInt(CoordinateDistanceEuclidean(me.x, me.y, newx, newy));
      Until ((dist<=0) Or (timeout<ReadGameClock()));
      i-=1;
    EndWhile
    If (dist > 0)
      LogToFile("::log/guards.log", "Ruekkehrpunkt nicht erreicht: "+Lower(Hex(me.serial))+" "+
      me.x+" "+me.y+" "+me.z+" -> "+newx+" "+newy+" - "+dist+" "+path.size());
    EndIf
  EndIf
  DropAnchor(0); //reaktiviert den Anker
EndFunction

//////////////////////////////////////////////////
// MakeMePeaceful_Townguard - Herstellen des friedlichen Stadtwachenzustands
// fern 1 - Anlegen Bogen
// opponent - ist der Gegner ein Player dann schwächere Waffen anlegen
//////////////////////////////////////////////////

Function ChangeWeapon(byref fern)//, byref opponent)
  Var equipment1:=GetEquipmentByLayer(Me, LAYER_HAND1);
  Var equipment2:=GetEquipmentByLayer(Me, LAYER_HAND2);
  //derzeit noch keine Grundlage mehr als 2 Equipments zu verwalten
  //  If (fern)
  //    If (opponent.isa(POLCLASS_NPC))
  //      TestMessage("ChangeWeapon Fern High", 1);
  //      If (equipment1 <> GUARDBOW)
  //        MoveItemToContainer(equipment1,who.backpack);
  //        // Backpack leeren
  //        ForEach item in EnumerateItemsInContainer(Me.backpack)
  //          DestroyItem(item);
  //        EndForEach
  //      EndIf
  //    Else
  //      TestMessage("ChangeWeapon Fern LOW", 1);
  //    EndIf
  //  Else
  //    If (opponent.isa(POLCLASS_NPC))
  //      TestMessage("ChangeWeapon Near High", 1);
  //    Else
  //      TestMessage("ChangeWeapon Near LOW", 1);
  //    EndIf
  //  EndIf

  If (fern)
    If (equipment1.objtype<>GUARDBOW)
      If(equipment1.isa(POLCLASS_WEAPON))
        MoveItemToContainer(equipment1, Me.backpack);
      EndIf
      If((equipment2.isa(POLCLASS_WEAPON))||(equipment2.isa(POLCLASS_ARMOR)))
        MoveItemToContainer(equipment2,Me.backpack);
      EndIf
      Var bow:=FindObjTypeInContainer(Me.Backpack,GUARDBOW);
      EquipItem(Me,bow);
      //für Orkstadtwachen
      If (Me.graphic==0x29)
        Me.graphic:=0x11;
      EndIf
    EndIf
  Else
    If (equipment1.objtype==GUARDBOW)
      MoveItemToContainer(equipment1,Me.backpack);
      foreach item in enumerateitemsincontainer(Me.backpack)
        If (item.isa(POLCLASS_WEAPON))
          If (item.objtype!=GUARDBOW)
            EquipItem(Me,item);
          EndIf
        ElseIf (item.isa(POLCLASS_ARMOR))
          EquipItem(Me,item);
        EndIf
      endforeach
      If (Me.graphic==0x11)
        Me.graphic:=0x29;
      EndIf
    EndIf
  EndIf
EndFunction

/////////////////////////////////////////////////
// SleepMSClear - Wartet und loescht die Events
/////////////////////////////////////////////////

Function SleepMSClear(byref time)
  SleepMS(time);
  Clear_Event_Queue();
EndFunction

/////////////////////////////////////////////////
// MakeAlertCall - Ruft weitere Wachen zu hilfe
/////////////////////////////////////////////////
Function MakeAlertCall()
  SayPergon(Me, "Alarm, Wachen zu mir!");

  Var ev:=struct;
  ev.+type:=EVID_GUARDS_HELP;
  ev.+source:=Me;

  foreach mob in ListMobilesNearLocation( Me.x, Me.y, Me.z, params[RANGEFOLLOW], Me.realm )
    If ((mob.serial<>Me.serial)&&(mob.script["townguard"]))
      SendEvent(mob,ev);
    EndIf
  endforeach
EndFunction
