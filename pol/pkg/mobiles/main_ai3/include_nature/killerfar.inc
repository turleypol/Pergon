////////////////////////////////////////////////////////////////////////////////////////////
//  Killerfar sind Einzelgänger greIfen an, versuchen immer wegzulaufen um in Schussposition
//  zu kommen
//
//  15.05.07    Fox: im Kampf wurden die Events nicht immer gespeichert
////////////////////////////////////////////////////////////////////////////////////////////


Function CheckProjectileWeapon(byref fernKampfModus)
  Var attweapondesc;
  Var equipment;
  Var attweapon;

  equipment:=GetEquipmentByLayer(Me, LAYER_HAND2);
  If(equipment.isa(POLCLASS_WEAPON))
    attweapon:=equipment;
  Else
    equipment:=GetEquipmentByLayer(Me, LAYER_HAND1);
    If(equipment.isa(POLCLASS_WEAPON))
      attweapon:=equipment;
    EndIf
  EndIf

  If(attweapon)
    attweapondesc:=GetItemDescriptor(attweapon.objtype);
    If(attweapondesc) // Auch dies sollte immer funktionieren! ausser er hat keine waffe in der hand...
      If(attweapondesc.projectile) // Ist es eine Projektilwaffe?
        Var projectile:=FindObjTypeInContainer(Me.backpack, attweapondesc.projectiletype);
        If(!projectile) //pfeile alle -> bogen ins backpack
          MoveItemToContainer(attweapon, Me.backpack);
          Return(0);
        EndIf
      Else
        TestMessage("Nahkampfwaffe ist equippt.", 2);
        fernKampfModus:=0;
        Return 0; //dann ist es die Alternativwaffe, ergo Nahkampf
      EndIf
    EndIf
  Else
    //hab keine waffe in der hand
    TestMessage("Alternative Equippen", 2);
    Habille(me, GetObjProperty(me, "EquiptA"), 0); //Ersatzwaffe anziehen
    fernKampfModus:=0; //in Nahkampfmodus gehen
    Return(0);
  EndIf
  Return(1);
EndFunction


Function OnStartKillerfar()
  Var ev:=struct;
  ev.+type:=EVID_LOOKAROUND;
  ev.+source:=Me;
  DropAnchor();
  SendEvent(Me,ev);
EndFunction

Function OnIdleKillerfar()

  If(Me.warmode)
    Var opp := SystemFindObjectBySerial(GetObjProperty(Me,PROP_OPPONENT));
    If(opp)
      //dann kaempfe ich weiter mit dem aktuellen opponent
      //PrintTextAbovePergon(Me,Me,"kampf " + opp.name);
      FightKillerFar(opp);
    Else
      //mein aktueller gegner ist weg -> da such ich mir wen neues
      //PrintTextAbovePergon(Me,Me,"opp weg");
      Var newev:=struct;
      newev.+type := EVID_LOOKAROUND;
      newev.+source:=Me;
      SendEvent(Me,newev);
    EndIf
  Else
    If (wandersteps > 0) //wandersteps=0 ist Tiefschlaf
      WanderPergon();
      wandersteps-=1;
    EndIf
  EndIf

EndFunction


Function FightKillerFar( byref opponent )

  wandersteps:=params[IDLESTEPS]; //nach nem Kampf Rumwandern

  Var ret_wert:=0;
  Var timeout:=ReadGameClock()+2; //2s lang wird auch bei Events nicht unterbrochen
  //Var evfight:={};
  //evfight.+type:=EVID_STILLFIGHT;
  //evfight.+source:=opponent;

  Var event_merker:=array; //merkt sich die events die während des wartens ausgelesen wurden um sie wieder einzuspeisen
  Var ev;

  //Variablen für Verhalten festlegen
  Var localParams:=struct;
  localParams.+bevorzugteRichtung; //im UZS bei 1
  //localParams.+abstandErhoehen; //ist er Näher am Gegner wegrennen
  //localParams.+abstandVerringern; //ist er weiter weg vom Gegner hinrennen
  //ohne LineOfSight in einem Kreis um den Gegner laufen
  localParams.+nloRadiusMin; //OpponentRichtung+90°+45°
  localParams.+nloRadiusMax; //OpponentRichtung+90°-45°
  localParams.bevorzugteRichtung:=RandomInt(2);
  //localParams.abstandErhoehen:=3;
  //localParams.abstandVerringern:=12;
  localParams.nloRadiusMin:=3;
  localParams.nloRadiusMax:=5;
  Var fernkampfModus:=1; //1..Fernkampf 0..Nahkampf

  If(!Me.warmode)
    SetOpponent( opponent );
    SetObjProperty(Me,PROP_OPPONENT, opponent.serial); //aktuellen opponent merken um evtl überfluessige still_fights zu skippen
    SetWarmode(1);
  EndIf

  DisableEvents(SYSEVENT_ENTEREDAREA); //waehrend ich kaempfe interessiert mich das nicht

  Var last_instr:=Me.process.instr_cycles;
  while((ReadGameClock()<timeout)||(event_merker.size() == 0)) //nur wenn die Zeit rum ist und eine Event aufgelaufen ist beenden
    TestMessage("round_fight instr: " + CStr(Me.process.instr_cycles-last_instr), 2);
    last_instr:=Me.process.instr_cycles;
    //SleepMs(500);
    If( (opponent) && (Distance( Me, opponent ) <= params[RANGEFOLLOW]) && !(opponent.hidden) && !(opponent.concealed) && !(opponent.dead))
      TestMessage("------------------", 2);
      /*If( Distance( Me, opponent ) > 1 )
      MainRunToward(opponent);
      SleepMs(10);
      Else
      TestMessage("Gegner erreicht.", 1);
      EraseObjProperty(Me, PROP_PATHFIND);
      EraseObjProperty(Me, PROP_BLOCKED);
      SleepMs(2000);
      EndIf*/

      If(fernKampfModus)
        If (CheckProjectileWeapon(fernKampfModus)) //solange ich pfeile habe...
          If(Distance( Me, opponent ) <= 3)//localParams.abstandErhoehen)
            TestMessage("wegrennen",3);
            MainRunAwayFrom(opponent);
            SleepMs(10); //im Runtoward ist schon ein Sleep in hoehe der run_speed drin!
          ElseIf(!CheckLineOfSight(Me, opponent))
            TestMessage("kann nix sehen",3);
            //pathfind zum gegner hin damit ich ihn wieder sehe
            MainGoToLOS(opponent, localParams);
            //PathfindTowardLOS(opponent,xold,yold); <-- da wird auch nicht mehr gemacht als nochmal LineOfSight Checken
            SleepMs(10);
          ElseIf (Distance( Me, opponent ) > 12)//localParams.abstandVerringern) //am Gegner dran bleiben wenn er wegrennt
            TestMessage("hinrennen",3);
            MainRunToward(opponent);
          Else
            TestMessage("fernkampf",3);
            //TestMessage("Warmode: "+Me.warmode,3);
            //TestMessage("Opponent: "+Me.opponent.name,3);
            //TestMessage("LOS: "+CheckLineOfSight(Me, opponent),3);
            //Warten auf Aktion
            EnableEvents( SYSEVENT_OPPONENT_MOVED ); //dann muss man nicht ständig prüfen ob sich der Gegner bewegt
            SleepMs(300);
            ev:=Wait_for_Event(5); //wenn nix los ist, der Gegner still steht warum hetzen, also besser ausruhen
            If (ev)
              If (ev.type!=SYSEVENT_OPPONENT_MOVED)
                TestEvents("killerfar 1", 3, ev);
                event_merker.append(ev);
              EndIf
            EndIf
            DisableEvents( SYSEVENT_OPPONENT_MOVED );
          EndIf
        Else
          TestMessage("round_fight Fehlerfall 1", 2);
        EndIf
      Else //andernfalls versuche ichs mal als nahkaempfer
        If ((Distance( Me, opponent ) < 2 )&&(abs(Me.z-opponent.z) < 15)) //falls ich nicht schon vor dem gegner stehe, renn ich hin
          TestMessage("nahkampf",3);
          EraseObjProperty(Me, PROP_BLOCKED);
          EraseObjProperty(Me, PROP_PATHFIND);
          //Warten auf Aktion
          EnableEvents( SYSEVENT_OPPONENT_MOVED ); //dann muss man nicht ständig prüfen ob sich der Gegner bewegt
          SleepMs(100);
          ev:=Wait_for_Event(5); //wenn nix los ist, der Gegner still steht warum hetzen, also besser ausruhen
          If (ev)
            If (ev.type!=SYSEVENT_OPPONENT_MOVED)
              TestEvents("killerfar 2", 3, ev);
              event_merker.append(ev);
            EndIf
          EndIf
          DisableEvents( SYSEVENT_OPPONENT_MOVED );
        Else
          TestMessage("hinrennen",3);
          MainRunToward(opponent);
        EndIf
      EndIf
      ret_wert:=1;
    Else
      //der gegner ist weg -> aufhoeren zu kaempfen -> nach neuem gegner umschauen
      Var ev:=struct;
      //PrintTextAbovePergon(Me,Me,"lookaround");
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
        TestEvents("killerfar 3", 3, ev);
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

