include "include/animal";

Function OnIdleMeek()
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
      UpdateHunger(me);
      WanderPergon();
      wandersteps-=1;
    EndIf
  EndIf
EndFunction


Function Flee(byref opponent, byref rangefollow)
  TestMessage("FleeMeek", 1);
  var timeout:=ReadGameClock()+2;

  var evflee:=struct;
  evflee.+type:=EVID_STILLRUN;
  evflee.+source:=opponent;
  evflee.+range:=rangefollow;
  var xold:=Me.x;
  var yold:=Me.y;

  SetWarmode(1); //damit er von seinem anchorpoint wegrennen kann

  wandersteps:=params[IDLESTEPS]; //nach der Flucht Rumwandern

  DisableEvents(SYSEVENT_ENTEREDAREA); //waehrend ich renne interessiert mich das nicht
  while(ReadGameClock()<timeout)
    if( (opponent) && (Distance( Me, opponent ) <= rangefollow)
      && !(opponent.hidden) && !(opponent.concealed)
      && !(opponent.dead))
      RunAwayFrom( opponent );
      PathFindAwayFrom(opponent,xold,yold);
    else
      //der gegner ist zu weit weg -> aufhoeren mit rennen -> umschauen ob nicht noch jemand boeses da ist
      var ev:=struct;
      ev.+type:=EVID_LOOKAROUND;
      ev.+source:=Me;
      SendEvent(Me,ev);
      SleepMs(30);
      return(0);
    EndIf
  EndWhile
  SetWarmode(0);
  SendEvent(Me,evflee);
EndFunction

Function FleeOneStep(byref mob)
  TestMessage("fleeonestep",1);
  RunAwayFrom(mob);
EndFunction

Function OnStartMeek()
  UpdateHunger(me);
  Var ev:=struct;
  ev.+type:=EVID_LOOKAROUND;
  ev.+source:=Me;

  //verankern
  DropAnchor();
  SendEvent(Me,ev);
EndFunction
