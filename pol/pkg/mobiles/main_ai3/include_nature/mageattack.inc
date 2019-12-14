Function OnStartMage()
  Var ev:=struct;
  ev.+type:=EVID_LOOKAROUND;
  ev.+source:=Me;

  //verankern
  DropAnchor();
  SendEvent(Me,ev);
EndFunction


Function OnIdleMage(disable_enterevent := 1)
  If(Me.warmode)
    Var timeout:=ReadGameClock()+2; //2s lang wird auch bei Events nicht unterbrochen

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
    //localParams.abstandErhoehen:=3;  // wird nur hier Verwendet deswegen wegen Speicher
    //localParams.abstandVerringern:=params.rangesense/2;  // mal gespart
    localParams.nloRadiusMin:=3;
    localParams.nloRadiusMax:=5;

    Var opponent:=0;
    If (GetObjProperty(Me,PROP_OPPONENT))
      opponent := SystemFindObjectBySerial(GetObjProperty(Me,PROP_OPPONENT));
    EndIf

    If (disable_enterevent)
      // waehrend ich kaempfe interessiert mich das nicht
      DisableEvents(SYSEVENT_ENTEREDAREA);
    EndIf

    Var last_instr:=Me.process.instr_cycles;
    while((ReadGameClock()<timeout)||(event_merker.size() == 0)) //nur wenn die Zeit rum ist und ein Event aufgelaufen ist beenden
      TestMessage("round_fight instr: " + CStr(Me.process.instr_cycles-last_instr), 2);
      last_instr:=Me.process.instr_cycles;
      //SleepMs(500);
      If( (opponent) && (Distance( Me, opponent ) <= params[RANGESENSE]) && !(opponent.hidden) && !(opponent.concealed) && !(opponent.dead))

        If (params[CURRNATURE]==MAGEATTACK)
          If(Distance( Me, opponent ) <= 3) // 3 =localparams.abstandErhoehen
            TestMessage("wegrennen",3);
            MainRunAwayFrom(opponent);
            SleepMs(10); //im Runtoward ist schon ein Sleep in hoehe der run_speed drin!
          ElseIf(!CheckLineOfSight(Me, opponent))
            TestMessage("kann nix sehen",3);
            //pathfind zum gegner hin damit ich ihn wieder sehe
            MainGoToLOS(opponent, localParams);
            //PathfindTowardLOS(opponent,xold,yold); <-- da wird auch nicht mehr gemacht als nochmal LineOfSight Checken
            SleepMs(10);
          ElseIf (Distance( Me, opponent ) > (params[RANGESENSE]/2)) //am Gegner dran bleiben wenn er wegrennt
            TestMessage("hinrennen",3);
            MainRunToward(opponent);
          Else
            TestMessage("fernkampf",3);
            TestMessage("Mana: "+GetManaPergon(me), 8);
            //TestMessage("Warmode: "+Me.warmode,3);
            //TestMessage("Opponent: "+Me.opponent.name,3);
            //TestMessage("LOS: "+CheckLineOfSight(Me, opponent),3);
            //Warten auf Aktion
            EnableEvents( SYSEVENT_OPPONENT_MOVED ); //dann muss man nicht ständig prüfen ob sich der Gegner bewegt
            SleepMs(300);
            ev:=Wait_for_Event(5); //wenn nix los ist, der Gegner still steht warum hetzen, also besser ausruhen
            If (ev)
              If (ev.type!=SYSEVENT_OPPONENT_MOVED)
                TestEvents("mageattack 1", 3, ev);
                event_merker.append(ev);
                If (ev.type == EVID_REFLECTION)
                	timeout := ReadGameClock();
                EndIf
              EndIf
            EndIf
            DisableEvents( SYSEVENT_OPPONENT_MOVED );
          EndIf
        Else  // Nahkampf

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
                TestEvents("mageattack 2", 3, ev);
                event_merker.append(ev);
              EndIf
            EndIf
            DisableEvents( SYSEVENT_OPPONENT_MOVED );
          Else
            TestMessage("hinrennen",3);
            MainRunToward(opponent);
          EndIf
        EndIf

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
        break;
      EndIf
      //Events merken die inzwischen angefallen sind
      //sorgt dafür das nach Ablauf der Mindestzeit die Kampffunktion verlassen wird
      //(liegt mehr als 1 vor kann die Reihenfolge verändert werden, das Event-System sollte aber Reihenfolgeunabhängig sein)
      ev:=Wait_for_Event(0); //kein Eventlogging da diese eh wieder eingespeist werden
      If (ev)
        If (ev.type!=SYSEVENT_OPPONENT_MOVED)
          TestEvents("mageattack 3", 3, ev);
          event_merker.append(ev);
          If (ev.type == EVID_REFLECTION)
            timeout := ReadGameClock();
          EndIf
        EndIf
      EndIf
    endwhile

    //Events einspeisen
    TestMessage("Events einspeisen: "+event_merker.size(),3);
    Foreach ev in event_merker
      SendEvent(Me,ev);
    EndForeach
  Else
    TestMessage("Wandersteps="+wandersteps, 4);
    If (wandersteps > 0) //wandersteps=0 ist Tiefschlaf
      WanderPergon();
      wandersteps-=1;
    EndIf
  EndIf
EndFunction

Function MagicAttackFight(byref opponent)
  wandersteps:=params[IDLESTEPS]; //nach nem Kampf Rumwandern
  If (!Me.warmode)
    SetOpponent( opponent );
    SetObjProperty(Me,PROP_OPPONENT, opponent.serial); //aktuellen opponent merken
    SetWarmode(1);
  EndIf

  If( (!opponent) || (Distance( Me, opponent ) > params[RANGEFOLLOW])
    || (opponent.hidden) || (opponent.concealed) || (opponent.dead))
    // Gegner weg also nach neuen Suchen
    Var ev:=struct;
    TestMessage("lookaround",3);
    ev.+type:=EVID_LOOKAROUND;
    ev.+source:=Me;
    SendEvent(Me,ev);
    Return (2);
  EndIf

  If (MagicFight(opponent)==2) // kein Mana
    // Dann gewisse Chance das er nicht mehr nur rumsteht
    // sondern in den Nahkampf übergeht
    If (params[CURRNATURE]==MAGEATTACK)
      If (RandomInt(2))
        TestMessage("change nature",1);
        Var ev:=struct;
        ev.+type:=EVID_LOOKAROUND;
        ev.+source:=Me;
        params[CURRNATURE]:=NEARATTACK;
        params[CASTDELAY]:=params[CASTDELAY]*2; // Da er weiterhin casten kann Delay erhöhen (hat eh kaum Mana)
        SendEvent(Me,ev); // Schnapp sie dir
      EndIf
    EndIf
  EndIf
EndFunction

Function DoHealing()
  TestMessage("helfe Freunde", 4);
  ForEach myfriend in (ListMobilesNearLocation(Me.x, Me.y, Me.z, params[RANGESENSE],Me.realm))
    If (myfriend.isa(POLCLASS_NPC))
      If (myfriend.serial<>Me.serial)
        If (Healerstuff(myfriend))
          Return(1);
        EndIf
      EndIf
    EndIf
    SleepMS(2);
  EndForEach
  //Konnte keinem Helfen also Schaden machen
  Return(0);
EndFunction

function HealerStuff(byref mobile)
  TestMessage("Helfe "+Hex(mobile.serial),4);
  If (Lower(mobile.npctemplate)["townguard"] || (mobile.script in {"enticedanimal", "tamed"}))
    Return(0);
  EndIf
  var result:=MagicHelp(mobile);
  If (result==2) // Mana alle
    // Dann gewisse Chance das er nicht mehr nur rumsteht
    // sondern in den Nahkampf übergeht
    If (params[CURRNATURE]==MAGEATTACK)
      If (!RandomInt(3)) // 33% Chance
        TestMessage("change nature",1);
        Var ev:=struct;
        ev.+type:=EVID_LOOKAROUND;
        ev.+source:=Me;
        params[CURRNATURE]:=NEARATTACK;
        params[CASTDELAY]:=params[CASTDELAY]*2; // Da er weiterhin casten kann Delay erhöhen (hat eh kaum Mana)
        SendEvent(Me,ev); // Schnapp sie dir
      EndIf
    EndIf
    Return(1);
  ElseIf (result==1)
    Return(1);
  Else
    Return(0);
  EndIf
endfunction

Function MagicHelp(byref friend)
  //testen ob freund überhaupt geholfen werden darf
  If ((!friend)||(friend.hidden)||(friend.dead)||(friend.concealed))
    //der freund ist weg (warum auch immer)
    TestMessage("MagicFight: Freund verschwunden", 1);
    Return (0);
  EndIf
  Var ev:=struct; // schonmal zusammenbauen
  ev.+type;
  ev.+source;
  ev.type:=EVID_CAST_WAIT;
  ev.source:=Me;

  If ((Me.paralyzed) || (Me.frozen))
    // Dann halt nich...
    SendEventWaiterGeneral(Me, params[CASTDELAY], ev);
    Return(1);
  EndIf

  //So ab jetzt darf er die im Template eingetragenen Sprüche casten
  //mal checken ob er wirklich castet

  //Mal auf 100 normieren und dann Wahrscheinlichkeiten wuerfen
  Var chance:=(GetManaPergon(Me)*100)/GetMaxManaPergon(Me);
  If (RandomInt(100)>=chance)
    TestMessage("MagicFight: Habe zuwenig Mana", 1);
    //Hab zu wenig Mana, da Zaubere ich doch eher noch nicht
    //Dann warte ich das definierte Delay
    SendEventWaiterGeneral(Me, params[CASTDELAY], ev);

    Return(2); //spezial Return für nature MagicAttack
  EndIf

  Var spell_delay:=0;
  Var cast:=0;  // Hab ich auch gecastet?
  If ((friend.getprop("poison_level")) && (params[SPELLCURE]))
    TestMessage("MagicFight: Freund vergiftet", 1);
    spell_delay:=CallCast_GetDelay(params[SPELLCURE].randomentry(),friend);
    cast:=1;
  ElseIf ((RandomInt(GetMaxHPPergon(friend))>GetHPPergon(friend)) && (params[SPELLHEAL]))
    TestMessage("MagicFight: Heil Freund", 1);
    spell_delay:=CallCast_GetDelay(params[SPELLHEAL].randomentry(),friend);
    cast:=1;
  EndIf

  //EventWaiter für den nächsten Cast setzen
  If (cast)
    spell_delay+=params[CASTDELAY];
    spell_delay/=2;  // 50% fest 50% zufall
    SendEventWaiterGeneral(Me, RandomIntMinMax(spell_delay,2*spell_delay), ev);
    Return(1);
  Else
    Return(0);
  EndIf
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
