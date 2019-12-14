Function OnIdleImmobile()
  If(Me.warmode)
    Var opp := SystemFindObjectBySerial(GetObjProperty(Me,PROP_OPPONENT));
    If(opp)
      //dann kaempfe ich weiter mit dem aktuellen opponent
      FightImmobile(opp);
    Else
      //mein aktueller gegner ist weg -> da such ich mir wen neues
      Var newev:=struct;
      newev.+type := EVID_LOOKAROUND;
      newev.+source:=Me;
      SendEvent(Me,newev);
    EndIf
  EndIf
EndFunction


Function FightImmobile( byref opponent)
  TestMessage("FightImmobile()", 2);

  Var timeout:=ReadGameClock()+5;
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
      //Warten auf Aktion
      EnableEvents( SYSEVENT_OPPONENT_MOVED ); //dann muss man nicht ständig prüfen ob sich der Gegner bewegt
      SleepMs(100);
      ev:=Wait_for_Event(5); //wenn nix los ist, der Gegner still steht warum hetzen, also besser ausruhen
      If (ev)
        If (ev.type!=SYSEVENT_OPPONENT_MOVED)
          TestEvents("Immobile 1", 3, ev);
          event_merker.append(ev);
        EndIf
      EndIf
      DisableEvents( SYSEVENT_OPPONENT_MOVED );
      ret_wert:=1;
    Else
      //der gegner ist weg -> aufhoeren zu kaempfen -> nach neuem gegner umschauen
      Var ev:=struct;
      TestMessage("lookaround",3);
      ev.+type:=EVID_LOOKAROUND;
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
        TestEvents("Immobile 2", 3, ev);
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

Function OnStartImmobile()
  Var ev:=struct;
  ev.+type:=EVID_LOOKAROUND;
  ev.+source:=Me;

  //verankern
  DropAnchor();
  SendEvent(Me,ev);
EndFunction