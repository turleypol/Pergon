
Function FollowCharacter(byref character)
  Var timeout:=ReadGameClock()+2; //verhindert das die teuren KI Funktionen umsonst initialisiert werden, bei starker Eventbelastung
  Var event_merker:=array; //merkt sich die events die während des wartens ausgelesen wurden um sie wieder einzuspeisen (ermöglicht SYSEVENT_OPPONENT_MOVED zu nutzen)
  Var ev;

  Var last_instr:=Me.process.instr_cycles;
  // möp das && funzt nicht
  While((ReadGameClock()<timeout)&&(event_merker.size() == 0)) //bei Event das Folgen abbrechen

    TestMessage("round_follow instr: " + CStr(Me.process.instr_cycles-last_instr)+" "+timeout+" "+ReadGameClock(), 2);
    last_instr:=Me.process.instr_cycles;

    If( character.concealed || character.hidden || character.dead)
      //der Master ist ausser Sicht -> auhhoeren mit guard und follow
      EraseObjProperty(Me,PROP_GUARD);
      EraseObjProperty(Me,PROP_FOLLOWMASTER);
      SendSysMessagePergon(Me.master,Me.name + " wird aufhoeren Euch zu beschuetzen oder Euch hinterherzulaufen.");
      break;
    Else
      If ((Distance( Me, character ) < 2 )&&(abs(Me.z-character.z) < 15))
        TestMessage("erreicht",3);
        EraseObjProperty(Me, PROP_BLOCKED);
        EraseObjProperty(Me, PROP_PATHFIND);
        //warten das was passiert, oder der Master sich entfernt
        EnableEvents( SYSEVENT_OPPONENT_MOVED ); //dann muss man nicht ständig prüfen ob sich der Gegner bewegt
        SleepMs(100);
        ev:=Wait_for_Event(5); //wenn nix los ist, der Gegner still steht warum hetzen, also besser ausruhen
        If (ev)
          If (ev.type!=SYSEVENT_OPPONENT_MOVED)
            TestEvents("FollowCharacter", 3, ev);
            event_merker.append(ev);
          EndIf
        EndIf
        DisableEvents( SYSEVENT_OPPONENT_MOVED );
        SetWarmode(0); //weis zwar nicht wofür, aber schaden kanns auch nicht (Fox)
      Else
        SetWarmode(1); //notwendig für die KI Funktionen, weil Master wie Opponent behandelt werden muss? (Fox)
        //todo: eigentlich soll ich erst rennen wenn mein Master zu weit weg ist
        TestMessage("hinrennen",3);
        MainRunToward(character,struct{exist:=0,plan:=0});
        //MainWalkToward(character);
        SetWarmode(0); //sollte man schon machen wenn man nicht in der Kampffunktion landen will
      EndIf
    EndIf
    //Events merken die inzwischen angefallen sind
    //sorgt dafür das nach Ablauf der Mindestzeit die Kampffunktion verlassen wird
    //(liegt mehr als 1 vor kann die Reihenfolge verändert werden, das Event-System sollte aber Reihenfolgeunabhängig sein)
    ev:=Wait_for_Event(0); //kein Eventlogging da diese eh wieder eingespeist werden
    If (ev)
      If (ev.type!=SYSEVENT_OPPONENT_MOVED)
        TestEvents("FollowCharacter", 3, ev);
        event_merker.append(ev);
      EndIf
    EndIf
  endwhile
  //Events einspeisen
  //PrintTextAbovePergon(Me,Me,"Events einspeisen: "+event_merker.size());
  TestMessage("Events einspeisen: "+event_merker.size(),3);
  Foreach ev in event_merker
    SendEvent(Me,ev);
  EndForeach
endFunction



Function WaitForLivingMaster()
  If(!(Me.master.dead))
    //der Meister ist am Leben
    If(Distance(Me,Me.master) <= params[RANGESENSE])
      //der Meister lebt! und er war in meiner Naehe! juhu!
      EraseObjProperty(Me,PROP_MASTERDEAD);
      Return;
    EndIf
  EndIf

  Var endtime := GetObjProperty(Me,PROP_MASTERDEAD);
  If (endtime)
    If (endtime > ReadGameClock())
      //mein Meister kommt und kommt nicht wieder, da folge ich dem Ruf der Wildnis
      Release();
    EndIf
  EndIf
endFunction


// aus enticedanimal.src
// FIXME: vielleicht besser gemeinsames Include benutzen
Function Release()
  If (GetObjProperty(me, "companion"))
    return;
  EndIf

  If (GetObjProperty(me, PROP_SUMMONED))
    PlayStationaryEffect(me.x, me.y, me.z, FX_SMOKE, 0xa, 0xa, 0, me.realm);

    KillNPC(me, "release summoned", KILLNPC_NOCORPSE);

  ElseIf (GetObjProperty(me, "master"))
    //PrintTextAbovePergon(me.master, me, me.name+" laeuft wieder frei herum!");
    SendSysMessagePergon(me.master, me.name + " folgt dem Ruf der Wildnis.");
    UnparkAnimal(me);

    SetObjProperty(me, "ExMaster", GetObjProperty(me, "master"));
    EraseObjProperty(me, "master");
    //Sleep(1);

    me.setmaster(0);

    //me.script:=GetObjProperty(me, "script");
    //EraseObjProperty(me, "script");

    EraseObjProperty(Me,PROP_MASTERDEAD);
    EraseObjProperty(Me,PROP_STAY);
    EraseObjProperty(Me,PROP_FOLLOWMASTER);
    EraseObjProperty(Me,PROP_GUARD);

    //RestartScript(me);
  EndIf
EndFunction
