/////////////////////////////////////////////////////////////////////
// 20.12.2006 Fox -- Nahkampf-Element-Speier spezifisches für Main_AI
//
/////////////////////////////////////////////////////////////////////

const ELEMENT_FIREBALL  := 0x9000;
const ELEMENT_SNOWBALL  := 0x9001;
const ELEMENT_POISONBALL := 0x9002;
const ELEMENT_WATERBALL := 0x9003;
const ELEMENT_AIRBALL   := 0x9004;
const ELEMENT_EARTHBALL := 0x9005;

Enum Elemente  // Speichersparender als der String
  ELEMENT_FIRE_INT:=1,
  ELEMENT_ICE_INT,
  ELEMENT_POISON_INT,
  ELEMENT_WATER_INT,
  ELEMENT_AIR_INT,
  ELEMENT_EARTH_INT
EndEnum


/////////////////////////////////////////////////
// ThrowElement - Element spucken, Animation und throwelement.src start
/////////////////////////////////////////////////
Function ThrowElement(byref mob,byref elementdamage)
  TestMessage("ThrowElement()",3);
  If(elementdamage > 0) //nur damit werfen wenns auch schaden macht
    case(params[ELEMENT])
      ELEMENT_FIRE_INT:
        PlayMovingEffect(Me, mob, FX_EXPLODE_1, 10, 1, 1);
        Start_ScriptPergon( ":main_ai3:include_nature/throwelement", {mob, Me, ELEMENT_FIRE, elementdamage} ); //vom script spielen lassen, damit es parallel laeuft
      ELEMENT_ICE_INT:
        PlayMovingEffect(Me, mob, 0x3728, 10, 1, 0);
        Start_ScriptPergon( ":main_ai3:include_nature/throwelement", {mob, Me, ELEMENT_ICE, elementdamage} ); //vom script spielen lassen, damit es parallel laeuft
      ELEMENT_POISON_INT:
        PlayMovingEffect(Me, mob, 0x37c4, 10, 1, 0);
        Start_ScriptPergon( ":main_ai3:include_nature/throwelement", {mob, Me, ELEMENT_POISON, elementdamage} ); //vom script spielen lassen, damit es parallel laeuft
      ELEMENT_WATER_INT:
        PlayMovingEffect(Me, mob, 0x3789, 10, 1, 0);
        Start_ScriptPergon( ":main_ai3:include_nature/throwelement", {mob, Me, ELEMENT_WATER, elementdamage} ); //vom script spielen lassen, damit es parallel laeuft
      ELEMENT_AIR_INT:
        PlayMovingEffect(Me, mob, 0x3818, 10, 1, 0);
        Start_ScriptPergon( ":main_ai3:include_nature/throwelement", {mob, Me, ELEMENT_AIR, elementdamage} ); //vom script spielen lassen, damit es parallel laeuft
      ELEMENT_EARTH_INT:
        //PlayMovingEffect(Me, mob, 0x1363, 10, 1, 0);
        PlayMovingEffect(Me, mob, 0x1773, 10, 1, 0);
        Start_ScriptPergon( ":main_ai3:include_nature/throwelement", {mob, Me, ELEMENT_EARTH, elementdamage} ); //vom script spielen lassen, damit es parallel laeuft
      default:
    endcase
  EndIf
  Return(0);
EndFunction

/////////////////////////////////////////////////
// FightOpponentsFar - Elementball spucken auf Entfernung
/////////////////////////////////////////////////
Function FightOpponentsFar(byref range)
  TestMessage("FightOpponentsFar()",2);
  Var opppos;
  Var i:=1;

  //If( (GetObjProperty(Me,PROP_MASTER)) && (GetObjProperty(Me,PROP_GUARD)) ) //zahme Elementuser im guardmodus bekaempfen die feinde des master
  If(GetObjProperty(Me,PROP_MASTER)) //zahme Elementuser im guardmodus bekaempfen die feinde des master
    //ne Prop und dann stehts doch in ner Eigenschaft
    foreach mob in ListHostiles( Me.master, Distance(Me.master,Me) + range, LH_FLAG_LOS ) //alle Feinde des Masters - von mir aus Range weit weg
      If( (mob.cmdlevel < CMDLEVEL_SEER) )
        opppos := FindOpponentDirection(mob);
        //me.facing ist 7 im norden, findopponentdirection hat 0 im norden, deshalb 0,1,2 statt -1,0,1
        If((opppos == Me.facing) || (opppos == (Me.facing+1)&7) || (opppos == (Me.facing+2)&7))
          If(RandomInt(10)<7) // 70%
            If ((CheckLineOfSight(Me,mob)) && (Distance(Me,mob)>1))
              ThrowElement(mob,params[ELEMENTDMGFAR]);
              i+=1;
            EndIf
          EndIf
          If(i>params[ELEMENTAMOUNT]) //nur elementamount viele nacheinander hauen
            Return;
          EndIf
        EndIf
      EndIf
    endforeach
  Else //wildes Tier
    foreach mob in ListMobilesInLineOfSight( Me, range )
      //TestMessage("mob "+mob.name,1);
      If(!(mob==Me)&&ValidEnemy(mob))
        opppos := FindOpponentDirection(mob);
        //TestMessage("opppos/mypos "+opppos+"/"+mypos+"/"+(mypos+1)%8+"/"+(mypos+2)%8,1);
        If((opppos == Me.facing) || (opppos == (Me.facing+1)&7) || (opppos == (Me.facing+2)&7))
          If((RandomInt(10)<7) && (CheckLineOfSight(Me,mob)) && (Distance(Me,mob)>1))
            ThrowElement(mob,params[ELEMENTDMGFAR]);
            i+=1;
          EndIf
          If(i>params[ELEMENTAMOUNT]) //nur elementamount viele nacheinander hauen
            Return;
          EndIf
        EndIf
      EndIf
    endforeach
  EndIf
  Return;
EndFunction

/////////////////////////////////////////////////
// FightOpponentsNear - Elementflamme fauchen im Nahbereich
/////////////////////////////////////////////////
Function FightOpponentsNear(byref range)
  TestMessage("FightOpponentsNear()",2);
  Var opppos;

  // If( (GetObjProperty(Me,PROP_MASTER)) && (GetObjProperty(Me,PROP_GUARD)) ) //zahme Elementuser im guardmodus bekaempfen die feinde des master
  If(GetObjProperty(Me,PROP_MASTER)) //zahme Elementuser im guardmodus bekaempfen die feinde des master
    foreach mob in ListHostiles( Me.master, Distance(Me.master,Me) + range, LH_FLAG_LOS )
      If((mob.cmdlevel < CMDLEVEL_SEER) )
        opppos := FindOpponentDirection(mob);
        //me.facing ist 7 im norden, findopponentdirection hat 0 im norden, deshalb 0,1,2 statt -1,0,1
        If((opppos == Me.facing) || (opppos == (Me.facing+1)&7) || (opppos == (Me.facing+2)&7))
          ThrowElement(mob,params[ELEMENTDMGNEAR]);
        EndIf
      EndIf
    endforeach
  Else //wildes tier
    foreach mob in ListMobilesInLineOfSight( Me, range )
      If(ValidEnemy(mob))
        opppos := FindOpponentDirection(mob);
        If((opppos == Me.facing) || (opppos == (Me.facing+1)&7) || (opppos == (Me.facing+2)&7))
          ThrowElement(mob,params[ELEMENTDMGNEAR]);
        EndIf
      EndIf
    endforeach
  EndIf
  Return;
EndFunction

/////////////////////////////////////////////////
// FightElementUser - KampfschleIfe, Nahkämpfer
/////////////////////////////////////////////////
Function FightElementUser( byref opponent)
  wandersteps:=params[IDLESTEPS]; //nach nem Kampf Rumwandern

  Var timeout:=ReadGameClock()+2;
  Var ret_wert:=0;
  Var event_merker:=array; //merkt sich die events die während des wartens ausgelesen wurden um sie wieder einzuspeisen
  Var ev;

  If(!Me.warmode)
    SetOpponent( opponent );
    SetObjProperty(Me,PROP_OPPONENT, opponent.serial); //aktuellen opponent merken
    SetWarmode(1);
  EndIf

  DisableEvents(SYSEVENT_ENTEREDAREA); //waehrend ich kaempfe interessiert mich das nicht

  Var start_time, multi, no_element;
  Var last_instr:=Me.process.instr_cycles;
  while((ReadGameClock()<timeout)||(event_merker.size() == 0)) //nur wenn die Zeit rum ist und eine Event aufgelaufen ist beenden
    TestMessage("FightElementUser: instr: " + CStr(Me.process.instr_cycles-last_instr), 2);
    last_instr:=Me.process.instr_cycles;

    If( (opponent) && (Distance( Me, opponent ) <= params[RANGEFOLLOW]) && !(opponent.hidden) && !(opponent.concealed) && !(opponent.dead))
      TestMessage("------------------", 2);
      If ((Distance( Me, opponent ) < 2 )&&(abs(Me.z-opponent.z) < 15)) //falls ich nicht schon vor dem Gegner stehe, renn ich hin
        TestMessage("nahkampf",3);
        EraseObjProperty(Me, PROP_BLOCKED);
        EraseObjProperty(Me, PROP_PATHFIND);
        StepSleep(1,1);
        If (!RandomInt(2)) //Entscheiden ob Elementball oder -fauch
          If(RandomInt(1000) < params[ELEMENTFRQNEAR])
            FightOpponentsNear(1);
          EndIf
        Else
          If(RandomInt(1000)< params[ELEMENTFRQFAR])
            FightOpponentsFar(Round(params[RANGESENSE]+RandomInt(5)+1));
          EndIf
        EndIf
      Else
        TestMessage("hinrennen",3);
        start_time:=ReadMillisecondClock();
        MainRunToward(opponent); //kehrt nach jedem Schritt zurück <-- Problem eben nicht (an diesen Stellen Abbruchkriterium beachten)
        multi:=Round(CalcTimeSteps(ReadMillisecondClock()-start_time, 1));
        If (multi < 1)
          multi:=1;
        EndIf
        //Wktsrechnung statt Mehrfachwiederholung in SchleIfe, Vermeidet übermäßiges Feuern bei Freez eca.
        no_element:=pow((1.0-params[ELEMENTFRQFAR]/1000.0),multi)*1000.0;
        If(RandomInt(1000) < (1000-no_element))
          FightOpponentsFar(params[RANGESENSE]);
        EndIf
      EndIf
      ret_wert:=1;
    Else
      //der gegner ist weg -> aufhoeren zu kaempfen -> nach neuem gegner umschauen
      Var ev:=struct;
      TestMessage("lookaround",3);
      ev.+type:=EVID_LOOKAROUND;
      ev.+source:=Me;
      event_merker.append(ev);

      //Wegekram der mit Gegner zu tun hat löschen, könnte aber Probleme mit Herdentieren geben, noch prüfen
      EraseObjProperty(Me, PROP_PATHFIND);
      EraseObjProperty(Me, PROP_BLOCKED);

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
        TestEvents("killerfar", 3, ev);
        event_merker.append(ev);
      EndIf
    EndIf
  endwhile

  //Events einspeisen
  TestMessage("Events einspeisen: "+event_merker.size(),3);
  Foreach ev in event_merker
    SendEvent(Me,ev);
  EndForeach
  Return ret_wert;
EndFunction

Function OnIdleElementUser()
  If(Me.warmode)
    Var opp := SystemFindObjectBySerial(GetObjProperty(Me,PROP_OPPONENT));
    If(opp)
      //dann kaempfe ich weiter mit dem aktuellen opponent
      FightElementUser( opp);
    Else
      //mein aktueller gegner ist weg -> da such ich mir wen neues
      Var newev:=struct;
      newev.+type := EVID_LOOKAROUND;
      newev.+source:=Me;
      SendEvent(Me,newev);
    EndIf
  Else
    // ZahmeViecher Deaktivert
//    If( GetObjProperty(Me,PROP_FOLLOWMASTER) )
//      FollowCharacter(Me.master,params.rangesense);
//      If( GetObjProperty(Me,PROP_GUARD) )
//        Var newev:=struct;
//        newev.+type := EVID_LOOKAROUND;
//        newev.+source:=Me;
//        SendEvent(Me,newev);
//        SleepMs(30);
//      EndIf
//    Else
      If (wandersteps > 0) //wandersteps=0 ist Tiefschlaf
        WanderPergon(); //immer mal ein paar schritte machen -> schlafenszeit weniger
        wandersteps-=1;
      EndIf
//    EndIf
  EndIf
EndFunction

Function OnStartElementUser()
  Var ev:=struct;
  ev.+type:=EVID_LOOKAROUND;
  ev.+source:=Me;

  //verankern
  DropAnchor();
  SendEvent(Me,ev);
EndFunction