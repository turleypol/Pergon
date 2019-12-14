/////////////////////////////////////////////////////////////////////
// 2004-07-12 Mai -- Gruppendynamik fuer Herden und Packs
//
/////////////////////////////////////////////////////////////////////

CONST MAXGROUPTRAPPEDTIME := 40;

CONST PROP_GROUPTRAPPED  := "#grouptrapped";
CONST PROP_LEADER   := "leader";
CONST PROP_MEMBERS   := "groupmember";

Function OnStartGroup()
  TestMessage("OnStartGroup", 2);
  Var ev:=struct;
  ev.+type:=EVID_LOOKAROUND;
  ev.+source:=Me;
  SendEvent(Me,ev);
  MakeMeWanderer(); //alle properties loeschen
  FormGroup(); //herde bilden
EndFunction

Function FormGroup()
  TestMessage("FormGroup", 2);
  //PrintTextAbovePergon(Me, Me, "suche leader");
  SleepMs(RandomIntMinMax(100,600)); //test (damit nicht alle auf einmal sich gegenseitig folgen und keiner leader ist)
  Var newleader := LookForLeader();

  If(newleader)
    AskJoinHerd(newleader); //herde beitreten wenn moeglich
  Else
    //selber leader werden
    MakeMeLeader();
  EndIf

EndFunction

Function DoIdleMovement()
  TestMessage("DoIdleMovement", 2);
  Case (NATURE)
    NATURE_HERD:
      Case (MobType)
        MOB_HUMAN:    PerformAction(Me, ANIM_EAT);
        MOB_SEA:      PerformAction(Me, ANIM_RUN_ARMED);
        MOB_ANIMAL:   PerformAction(Me, ANIM_RUN_ARMED);
        MOB_MONSTER:  PerformAction(Me, ANIM_WALK_WARMODE);
        MOB_UNKNOWN:
      EndCase
    NATURE_PACK:
      Case (MobType)
        MOB_HUMAN:    PerformAction(Me, ANIM_SALUTE);
        MOB_SEA:      PerformAction(Me, ANIM_STAND_1HAND);
        MOB_ANIMAL:   PerformAction(Me, ANIM_ATTACK_1HAND_JAB);
        MOB_MONSTER:  PerformAction(Me, ANIM_ATTACK_1HAND_JAB);
        MOB_UNKNOWN:
      EndCase
  endcase

  StepSleep(2, 0); //im PerformAction scheint im Gegensatz zum WalkTo kein Sleep drin zu sein
EndFunction

Function OnIdleGroup()
  TestMessage("OnIdleGroup: wandersteps="+wandersteps, 2);
  //params.rangesense := AdjustEnterAreaRange(params.rangesense, params.rangesenseorig);
  //DisableEvents(SYSEVENT_ENTEREDAREA);
  //EnableEvents(SYSEVENT_ENTEREDAREA, params.rangesense);
  //PrintTextAbovePergon(Me,Me, "begin idle" );

  If( GetObjProperty( Me, PROP_LEADER ) ) //Leittier
    If (wandersteps > 0) //wandersteps=0 ist Tiefschlaf
      If (!RandomInt(10)) //10% Wkt
        TestMessage("OnIdleGroup: P1", 3);
        DoIdleMovement(); //Gras Futtern und sowas
      Else
        TestMessage("OnIdleGroup: P2", 3);
        WanderPergon(); //immer mal ein paar Schritte machen -> Schlafenszeit weniger
      EndIf
      If (!RandomInt(3)) // 33%
        TestMessage("OnIdleGroup: P3", 3);
        CheckGroup(); //pruefen ob ich in der Richtigen Gruppe bin
      EndIf
      wandersteps-=1;
    Else
      TestMessage("OnIdleGroup: P4", 3);
      CheckGroup(); //pruefen ob ich in der Richtigen Gruppe bin
    EndIf

    //falls meine Herde zu klein ist -> nach neuer Herde Ausschau halten
    If( GetObjProperty(Me,PROP_MEMBERS).size()+1 == 1 )
      If(!RandomInt(3)) //33%
        TestMessage("OnIdleGroup: P5", 3);
        MakeMeWanderer();
      Else
        //noch ein weilchen Leittier bleiben
      EndIf
    ElseIf( GetObjProperty(Me,PROP_MEMBERS).size()+1 <= (params[GROUPSIZE]/2) )
      If(!RandomInt(50)) // 2%
        TestMessage("OnIdleGroup: P6", 3);
        MakeMeWanderer();
      Else
        //noch ein weilchen zusammenbleiben
      EndIf
    EndIf
  ElseIf(GetObjProperty(Me, PROP_FOLLOW)) //Herdentier
    Var leader := SystemFindObjectBySerial(GetObjProperty(Me, PROP_FOLLOW));
    If ((leader) && (leader.script == Me.script))
      Var bBreak:=0;
      If (!RandomInt(20)) //5%
        If (CheckGroupMember(leader, Me)==0)
          TestMessage("OnIdleGroup: P7", 3);
          MakeMeWanderer();
          bBreak:=1;
        EndIf
      EndIf
      If (bBreak==0)
        If ( GetObjProperty( leader, PROP_LEADER ) )
          If (wandersteps > 0) //wandersteps=0 ist Tiefschlaf
            TestMessage("OnIdleGroup: P8", 3);
            FollowLeader( leader);
            wandersteps-=1;
          EndIf
        Else
          //mein Leader ist ein Herdentier! (haben uns wohl beide gleichzeitig um gegenseitigen Beitritt bemueht)
          TestMessage("OnIdleGroup: P9", 3);
          MakeMeWanderer();
        EndIf
      EndIf
    Else
      //nanu? Leader tot? oder weggezaehmt? -> neuen suchen
      TestMessage("OnIdleGroup: P10", 3);
      MakeMeWanderer();
    EndIf
  Else //Streuner
    If (wandersteps > 0) //wandersteps=0 ist Tiefschlaf
      If (!RandomInt(10)) //10% Wkt
        TestMessage("OnIdleGroup: P11", 3);
        DoIdleMovement(); //Gras Futtern und sowas
      Else
        TestMessage("OnIdleGroup: P12", 3);
        WanderPergon(); //immer mal ein paar Schritte machen -> Schlafenszeit weniger
      EndIf
      wandersteps:=wandersteps-1;
      If(!RandomInt(3)) //33%
        TestMessage("OnIdleGroup: P13", 3);
        FormGroup(); //mal wieder nach einer Herde umschauen
      EndIf
    Else
      TestMessage("OnIdleGroup: P14", 3);
      FormGroup(); //mal wieder nach einer Herde umschauen
    EndIf
  EndIf
  //PrintTextAbovePergon(Me,Me, "end idle" );
EndFunction

Function FollowLeader(byref leader)
  TestMessage("FollowLeader: Start", 2);
  Var oldposx:= Me.x;
  Var oldposy:= Me.y;
  Var membermin:=GetNearestMember(leader); //das herdentier welches den geringsten abstand zu mir hat


  Var timeout:=ReadGameClock()+2;
  Var ret_wert:=0;
  Var event_merker:=array; //merkt sich die events die während des wartens ausgelesen wurden um sie wieder einzuspeisen
  Var ev;

  Var max_idle_steps:=5; //Anzahl Idleaktionen mit denen man in der Funktion Follow Leader bleibt
                         //- Lastersparnis weil steps nur hochgezählt wird wenn sich der Mob nicht mehr bewegt
                         //- gibt dem Leader Zeit sich zu bewegen
                         //-sorgt dafür das die Gruppencheck Funktionen in OnIdleGroup bei rumstehen genutzt werden
  Var max_follow_steps:=2; //Faktor den ein Follower mehr laufen darf, als der Leader an wandersteps verbraucht
                         //Notwendig um Herdenformation zu erreichen und das keine Follower vom Leader abgehängt werden
                         //weniger ist möglich mehr nicht (Abbruch durch Event möglich und gewollt)
  Var idle_steps:=0;
  Var follow_steps:=0;

  SetAnchor(me.x, me.y, 0, 0); //Anker deaktivieren, verursacht sonst Probleme weil nicht im Warmode

  Var last_instr:=Me.process.instr_cycles;
  while ((ReadGameClock()<timeout)||((event_merker.size() == 0)&&(idle_steps < max_idle_steps))) //nur wenn die Zeit rum ist und eine Event aufgelaufen ist beenden
    TestMessage("FollowLeader instr: " + CStr(Me.process.instr_cycles-last_instr), 3);
    last_instr:=Me.process.instr_cycles;

    membermin:=GetNearestMember(leader);
    If (membermin)
      If (Distance( Me, leader ) <= CInt(max(params[GROUPDIST],2)))
        //ok nah genug dran
        If(!RandomInt(5)) //20%chance
          DoIdleMovement();
        EndIf
        StepSleep(1, 0);
      ElseIf ((Distance( Me, membermin ) <= CInt(max(params[GROUPDIST],2))) && (Distance(membermin,leader) <= CInt(max(params[GROUPDIST],2)) ) )
        //wir sind nah genug am leader
        If(!RandomInt(5)) // 20%chance
          DoIdleMovement();
        EndIf
        StepSleep(1, 0);
      ElseIf( Distance( Me, leader ) > (params[GROUPSIZE]) ) //zum Leader laufen
        GoToMob(leader,params[GROUPSIZE]);
      Else //zur Herdenposition laufen
        GoToHerdPos(leader,membermin);
      EndIf
    Else //ich bin das einzige Herdentier
       If((Distance( Me, leader ) > params[GROUPDIST]))
          GoToMob(leader,params[GROUPSIZE]);
       Else
          If(!RandomInt(5)) // 20%chance
            DoIdleMovement();
          EndIf
          StepSleep(1, 0);
       EndIf
    EndIf
    SleepMs(10); //Hier gabs ein Runaway, eine der Subfunktion muss Fehlerhaft sofort zurück kommen !!!
    If( (oldposx == Me.x) && (oldposy == Me.y) ) //für den Fall dass das Tier getrapped ist
      idle_steps+=1;
      TestMessage("FollowLeader hold: steps= " + CStr(idle_steps), 3);
      PlayTime(leader,oldposx,oldposy); //Zufällige Bewegungen vom Leader weg um zu verhindern das die Herde zu dicht zusammen kommt und den Leader blockiert
    Else
      TestMessage("FollowLeader run", 3);
      idle_steps:=0;
      EraseObjProperty(Me,PROP_GROUPTRAPPED);
      oldposx:= Me.x;
      oldposy:= Me.y;
    EndIf
    If (follow_steps < max_follow_steps)
      follow_steps+=1;
    Else
      follow_steps:=0;
      wandersteps-=1;
      TestMessage("FollowLeader wandersteps: " + CStr(wandersteps), 3);
      If (wandersteps <= 0)
        Return; //sonst bleibt er bis zum nächsten Event am Rennen
      EndIf
    EndIf
    //Events merken die inzwischen angefallen sind
    //sorgt dafür das nach Ablauf der Mindestzeit die SchleIfe verlassen wird
    //(liegt mehr als 1 vor kann die Reihenfolge verändert werden, das Event-System sollte aber Reihenfolgeunabhängig sein)
    ev:=Wait_for_Event(0); //kein Eventlogging da diese eh wieder eingespeist werden
    If (ev)
      TestEvents("FollowLeader: ev=", 3, ev);
      event_merker.append(ev);
    EndIf
  endwhile

  DropAnchor(0); //reaktiviert den Anker

  //Events einspeisen
  TestMessage("FollowLeader: Events einspeisen: "+event_merker.size(),3);
  Foreach ev in event_merker
    SendEvent(Me,ev);
  EndForeach
  Return ret_wert;
EndFunction

Function CheckGroupType(byref mob)
  If (params[GROUP]) //falls ich keinen gruppeneintrag habe, kann ich nicht die richtige gruppe finden
    If(params[GROUP] == GetNPCConfig(mob.npctemplate).group)
      Return(1);
    Else
      Return(0);
    EndIf
  Else
    Return(0);
  EndIf
EndFunction

Function LookForLeader( )
 TestMessage("LookForLeader", 2);
 foreach mob in ListMobilesInLineOfSight( Me, params[RANGESENSE] )
  If( CheckGroupType(mob) ) //wenn es jemand vom selben typ ist
    //SysLog("selbes viech gefunden");
    If( (GetObjProperty(mob, PROP_LEADER)) && (GetObjProperty(mob, PROP_MEMBERS).size()+1 < params[GROUPSIZE]) )
        //PrintTextAbovePergon(Me, Me, "leader gefunden");
      Return mob;
    EndIf
  EndIf
  SleepMs(10); //gegen Runaways bei vielen Mobs
 endforeach
 //SysLog("keinen gefunden");
 Return 0;
EndFunction

Function JoinGroup(byref leader)
  TestMessage("JoinGroup", 2);
  Var anchor := GetObjProperty(leader,PROP_ANCHOR);
//  Var ankerparams:=array;

  SetObjProperty(Me, PROP_FOLLOW, leader.serial);
  EraseObjProperty( Me, PROP_LEADER );
  EraseObjProperty( Me, PROP_MEMBERS );

  //Me.name := "Herdentier" + hex(Me.serial);

  //ankerparams[1] := Me;    //warum auch immer Jason... wer setzt schonmal den anker von jemand anderem?
  //ankerparams[2] := anchor;
  //DropAnchor(ankerparams); //den gleichen Anker wie das Leittier benutzen <- Problem überschreibt den eigenen Anker
  If (anchor)
    SetAnchor(anchor[1], anchor[2], anchor[3], anchor[4]); //den gleichen Anker wie das Leittier benutzen
  EndIf

  /*If(GetObjProperty(leader,PROP_MASTER))
     If(params.nature == NATURE_PACK)
       MakeMeWanderer(params); //rudeltiere schliessen sich keinem zahmen rudel an
      //(um probleme zu vermeiden, dass master evtl grau wird falls neues member angrIff auf master ausloest)
      //<-- wird jetzt bei FightPackMember direkt verhindert
     EndIf
  EndIf*/


  If(GetObjProperty(Me,PROP_MASTER))
    If( !GetObjProperty(leader,PROP_MASTER))
      //ich bin zwar zahm, aber mein leader nicht
      If(!RandomInt(3))// 33%chance
      //PrintTextAbovePergon(Me,Me, "join2" );
        Release(); //dem ruf der Wildnis folgen und mich der wilden Herde anschliessen
      Else
        MakeMeWanderer(); //mit der wilden herde geh ich nicht...
      EndIf
    EndIf
  EndIf
EndFunction

Function AskJoinHerd(byref leader)
  TestMessage("AskJoinHerd", 2);
  Var ev:=struct;
  ev.+type:=EVID_ASKJOIN;
  ev.+source:=Me;

 SendEvent(leader, ev);
EndFunction

Function MakeMeLeader()
  TestMessage("MakeMeLeader", 2);
  SetObjProperty( Me, PROP_LEADER, Me.serial );
  EraseObjProperty( Me, PROP_FOLLOW );
  SetObjProperty( Me, PROP_MEMBERS, {} ); //hab noch keine member

  //Me.name := "Leittier" + hex(Me.serial);
  params[RANGESENSE] := params[RANGESENSELEADER];
  params[RANGESENSEORIG] := params[RANGESENSELEADER];

  //PrintTextAbovePergon(Me,Me, "leader " + params.rangesense );
  DropAnchor(); //aktiviert einen vorhandenen Anker (PROP_ANCHOR) bzw setzt einen neuen
EndFunction

Function MakeMeWanderer()
  TestMessage("MakeMeWanderer", 2);
  EraseObjProperty( Me, PROP_LEADER );
  EraseObjProperty( Me, PROP_FOLLOW );
  EraseObjProperty( Me, PROP_MEMBERS );
  EraseObjProperty( Me, PROP_GROUPTRAPPED );

  //Me.name := "Streuner" + hex(Me.serial);
  params[RANGESENSE] := params[RANGESENSELEADER] - RANGE_LEADER;
  params[RANGESENSEORIG] := params[RANGESENSELEADER] - RANGE_LEADER;

  DropAnchor(); //aktiviert einen vorhandenen Anker (PROP_ANCHOR) bzw setzt einen neuen

  //wandersteps:=params[IDLESTEPS]; //Schritte fürs wandern setzen <- wird beim zufälligen Auflösen zu kleiner Herden angeworfen
EndFunction

Function AddMember(byref member)
  TestMessage("AddMember", 2);
  Var evgrant:=struct;
  Var members:={};

  evgrant.+type:= EVID_GRANTJOIN;
  evgrant.+source:=Me;

  members := GetObjProperty( Me, PROP_MEMBERS );

  //alle member durchgehen um zu prüfen ob er nicht bereits in der Herde sein sollte
  Var bAdd:=1;
  foreach serial in members
    If(serial == member.serial)
      //der gehoert schon zu meiner herde
      bAdd:=0;
      break;
    EndIf
  endforeach

  If (bAdd==1)
    members.append(member.serial);
    SetObjProperty(Me, PROP_MEMBERS, members);
  EndIf
  SendEvent(member, evgrant);
EndFunction

Function RefuseMember(byref member)
  TestMessage("RefuseMember", 2);
  Var evref:=struct;
  evref.+type:= EVID_REFUSEJOIN;
  evref.+source:=Me;

  SendEvent(member, evref);

EndFunction

Function CheckGroup()
  TestMessage("CheckGroup", 2);
  Var members := GetObjProperty( Me, PROP_MEMBERS );
  Var stillmembers:={};

  TestMessage("CheckGroup: vorher="+GetObjProperty( Me, PROP_MEMBERS ).size()+1, 2);
  //alle member durchgehen ob sie auch noch da sind
  Var member;
  foreach serial in members
    member := SystemFindObjectBySerial(serial);
    //TestMessage("CheckGroup: P1="+serial, 2);
    If((member) && (member.script == Me.script))
      //TestMessage("CheckGroup: P2="+serial, 2);
      If(GetObjProperty(member, PROP_FOLLOW) == Me.serial) //laeuft es auch wirklich mir hinterher? (vielleicht hats 2 leader gefragt..)
        //TestMessage("CheckGroup: P3="+serial, 2);
        stillmembers.append(serial);
      EndIf
    EndIf
  endforeach
  TestMessage("CheckGroup: nachher="+stillmembers.size()+1, 2);
  SetObjProperty(Me, PROP_MEMBERS, stillmembers);
EndFunction

Function CheckGroupMember(byref leader, byref tester)
  TestMessage("CheckGroupMember", 2);
  Var members := GetObjProperty( leader, PROP_MEMBERS );

  //alle member durchgehen ob das zu testende dabei ist dabei ist
  Var member;
  foreach serial in members
    member := SystemFindObjectBySerial(serial);
    //TestMessage("CheckGroupMember: P1="+serial, 2);
    If((member) && (member.serial == tester.serial))
      Return 1;
    EndIf
  endforeach
  Return 0;
EndFunction

Function GoToHerdPos(byref leader, byref membermin)
  TestMessage("GoToHerdPos", 2);
  Var posmember := FindOpponentDirection( membermin );
  Var posleader := FindOpponentDirection( leader );
  // TODO: da könnte neg. Rauskommen, mal schaun ob egal oder nicht (wenn egal dann &7)
  Var posmembernew := (posmember - posleader)%8; //position des herdentiers "eingenordet"

   //PrintTextAbovePergon(Me,Me, "hingehen" );
   //PrintTextAbovePergon(Me,Me, "pl" + posleader + " pm" + posmember + " pmn" +posmembernew);
  Var zw;
  If( (posmembernew <= 3) && (posmembernew >= 1) ) //das naechste member steht "oberhalb" von mir wenn das leitier links steht
    zw:=TryStepTowardLocationWalk((posleader-1)&7); //
  ElseIf( (posmembernew <= 7) && (posmembernew >= 5)  )
    zw:=TryStepTowardLocationWalk((posleader+1)&7);
  Else
    If(!RandomInt(2))
      zw:=TryStepTowardLocationWalk((posleader-1)&7);
    Else
      zw:=TryStepTowardLocationWalk((posleader+1)&7);
    EndIf
  EndIf
  If (!zw)
    StepSleep(1, 0);
  EndIf
EndFunction