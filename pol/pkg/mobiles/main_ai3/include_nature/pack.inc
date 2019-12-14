/////////////////////////////////////////////////////////////////////
// 2004-07-12 Mai -- Rudeltiere jagen gemeinsam
//         folgen einem Leittier
// 2006-08-25 Fox - Rudeltiere benutzen jetzt FightKillerNear für den eigentlich Kampf
//                - LookForNextOpponent wird für Gegnersuche benutzt
/////////////////////////////////////////////////////////////////////

CONST PACK_MAXATTACKER := 3;   //soviele greifen auf einmal einen feind an

Function OnIdlePack()
  TestMessage("OnIdlePack", 2);
//  If( GetObjProperty(Me,PROP_MASTER) )
//    If(GetObjProperty(Me,PROP_MASTERDEAD))
//      WaitForLivingMaster();
//      fastround:=1; //sorgt dafür das das Ideln möglichst schnell wieder angestoßen wird
//      Sleep(1000); //1s, damit die Hauptschleife nicht zu schnell durchlaufen wird
//    Else
//      If( Me.master.dead)
//        SetObjProperty(Me, PROP_MASTERDEAD, ReadGameClock()+600); //die treuezeit in now+treuezeit (in s)
//        fastround:=1; //sorgt dafür das das Ideln möglichst schnell wieder angestoßen wird
//        Sleep(1000); //1s, damit die Hauptschleife nicht zu schnell durchlaufen wird
//      EndIf
//    EndIf
//    wandersteps:=params[IDLESTEPS]; //solange ich nen Master habe, mobil bleiben
//  EndIf

  If(Me.warmode)
    Var opp := SystemFindObjectBySerial(GetObjProperty(Me,PROP_OPPONENT));
    If(opp)
       //dann kaempfe ich weiter mit dem aktuellen opponent
       //PrintTextAbovePergon(Me,Me,"kampf " + opp.name);
       FightKillerNear(opp);
    Else
       //mein aktueller gegner ist weg -> da such ich mir wen neues
       //PrintTextAbovePergon(Me,Me,"opp weg");
       Var newev:=struct;
       newev.+type := EVID_LOOKAROUND;
       newev.+source:=Me;
       SendEvent(Me,newev);
    EndIf
    wandersteps:=params[IDLESTEPS]; //nach nem Kampf wandern
  Else
    If( GetObjProperty(Me,PROP_STAY) )
      //nichts tun, nichtmal umherlaufen
      Sleep(100);
//    ElseIf( GetObjProperty(Me,PROP_FOLLOWMASTER) )
//      //PrintTextAbovePergon(Me,Me,"hinterherlaufen");
//      FollowCharacter(Me.master);
    Else
      OnIdleGroup();
    EndIf
  EndIf
EndFunction

Function OrganizeAttack(byref opponent)
  TestMessage("OrganizeAttack", 2);
  Var members := GetObjProperty( Me, PROP_MEMBERS );
  Var attacker := 0;
  Var ev:=struct;
  ev.+type := SYSEVENT_ENGAGED;
  ev.+source := opponent;
  ev.+range := params[RANGEFOLLOW];
  ev.+leader := Me.serial;
  Var opplist := {};
  Var opp;

  opp := opponent;

  opplist.append(opp); //opplist damit einmal angegriffene ziele nicht doppelt angegrIffen werden
  //alle member durchgehen und angriffsziele vorgeben
  foreach serial in members
    Var member := SystemFindObjectBySerial(serial);
    If(member)
      If(GetObjProperty(member, PROP_FOLLOW) == Me.serial) //laeuft es auch wirklich mir hinterher? (vielleicht hats 2 leader gefragt..)
        If(attacker >= PACK_MAXATTACKER)
          If (opp) // noch da?
            opp := LookForNextOpponent(params[RANGESENSE], 1, opplist, opp.x, opp.y, opp.z);
            If(opp)
              attacker := 0;
              opplist.append(opp);
            EndIf
          EndIf
        EndIf
        ev.source := opp;
        SendEvent(member, ev); //dem member sagen dass es angreifen soll
        attacker += 1;
      EndIf
    EndIf
  endforeach
EndFunction

Function FightPack(byref opponent)
  TestMessage("FightPack", 2);
  Var ev:=struct;
  ev.+type := SYSEVENT_ENGAGED;
  ev.+source := opponent;
  ev.+range := params[RANGEFOLLOW];

  If( GetObjProperty( Me, PROP_LEADER ) )
    SetWarMode(1);
    OrganizeAttack(opponent);
    SetWarMode(0);
    FightKillerNear(opponent);
  ElseIf(GetObjProperty(Me, PROP_FOLLOW)) //Herdentier
    //dem Leittier mitteilen
    Var leader := SystemFindObjectBySerial(GetObjProperty(Me, PROP_FOLLOW));
    If(leader)
      If(!leader.warmode) //nicht den leader nerven, er organisiert doch schon den angriff oder kaempft
        If( opponent.isa(POLCLASS_NPC) )
          SendEvent(leader, ev);
        Else
//          If( !GetObjProperty(leader,PROP_MASTER)) //falls der master zahm ist, kein event schicken um evtl grauwerden des opfers zu vermeiden
            SendEvent(leader, ev);
//          EndIf
        EndIf
      EndIf
      //erstmal angreifen
      FightKillerNear(opponent);
    Else
        //kein leader da? dann erstmal kaempfen
      FightKillerNear(opponent);
    EndIf
  Else //streuner
    FightKillerNear(opponent); //streuner kaempft allein
  EndIf
EndFunction