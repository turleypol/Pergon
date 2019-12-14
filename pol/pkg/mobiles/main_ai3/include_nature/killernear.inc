/////////////////////////////////////////////////////////////////////
// Killernear sind Einzelgänger greIfen an und kämpfen bis zum Schluss
//
// 15.05.07   Fox: im Nahkampf wurden die Events nicht gespeichert
/////////////////////////////////////////////////////////////////////

Function OnIdleKillerNear()

  If(Me.warmode)
    Var opp := SystemFindObjectBySerial(GetObjProperty(Me,PROP_OPPONENT));
    If(opp)
      //dann kaempfe ich weiter mit dem aktuellen opponent
      FightKillerNear(opp);
    Else
      //mein aktueller gegner ist weg -> da such ich mir wen neues
      Var newev:=struct;
      newev.+type := EVID_LOOKAROUND;
      newev.+source:=Me;
      SendEvent(Me,newev);
    EndIf
  Else
    TestMessage("Wandersteps="+wandersteps, 4);
    If (wandersteps > 0) //wandersteps=0 ist Tiefschlaf
      WanderPergon();
      wandersteps-=1;
    EndIf
  EndIf
EndFunction


Function FightKillerNear( byref opponent)
  TestMessage("FightKillerNear()", 2);
  wandersteps:=params[IDLESTEPS]; //nach nem Kampf Rumwandern

  //Var evfight:={};
  //evfight.+type:=EVID_STILLFIGHT; <--- wird gar nicht benutzt bisher
  //evfight.+source:=opponent;
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
  Var last_instr:=Me.process.instr_cycles;
  while((ReadGameClock()<timeout)||(event_merker.size() == 0)) //nur wenn die Zeit rum ist und eine Event aufgelaufen ist beenden
    TestMessage("round_fight instr: " + CStr(Me.process.instr_cycles-last_instr), 2);
    last_instr:=Me.process.instr_cycles;

    If( (opponent) && (Distance( Me, opponent ) <= params[RANGEFOLLOW]) && !(opponent.hidden) && !(opponent.concealed) && !(opponent.dead))
      TestMessage("------------------", 2);
      If ((Distance( Me, opponent ) < 2 )&&(abs(Me.z-opponent.z) < 15)) //falls ich nicht schon vor dem Gegner stehe, renn ich hin
        TestMessage("nahkampf",3);
        EraseObjProperty(Me, PROP_BLOCKED);
        EraseObjProperty(Me, PROP_PATHFIND);
        //Warten auf Aktion
        EnableEvents( SYSEVENT_OPPONENT_MOVED ); //dann muss man nicht ständig prüfen ob sich der Gegner bewegt
        SleepMs(100);
        ev:=Wait_for_Event(5); //wenn nix los ist, der Gegner still steht warum hetzen, also besser ausruhen
        If (ev)
          If (ev.type!=SYSEVENT_OPPONENT_MOVED)
            TestEvents("killernear 1", 3, ev);
            event_merker.append(ev);
          EndIf
        EndIf
        DisableEvents( SYSEVENT_OPPONENT_MOVED );
      Else
        TestMessage("hinrennen",3);
        MainRunToward(opponent); //kehrt nach jedem Schritt zurück <-- Problem eben nicht (an diesen Stellen Abbruchkriterium beachten)
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
        TestEvents("killernear 2", 3, ev);
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

Function OnStartKillernear()
  Var ev:=struct;
  ev.+type:=EVID_LOOKAROUND;
  ev.+source:=Me;

  //verankern
  DropAnchor();
  SendEvent(Me,ev);
EndFunction