
Function InitNPC_Common()
  If (Me.name["<random>"])
    SetName(Me, RandomName(Me));
  EndIf
  If (!Me.getprop("questequipt")) // Wurden schon speziell Angezogen
    Habille(Me, GetObjProperty(Me, "Equipt")); //austatten nach CProp "Equipt"
    EquipMe(); //zusätzliches ausstatten nach configeintraegen und setzen ar_mod
  EndIf
EndFunction

Function IdleTime_Common()
  Var warten, ev;
  //Idlezeit bestimmen
  TestMessage("wandersteps: "+wandersteps,3);
  If ((params[IDLESPEED] > 0)&&(  wandersteps > 0))
    //wandern, nach Aktion
    warten:=RandomInt(100)/params[IDLESPEED];
    If (warten < 1)
      warten:=1;
    EndIf
  Else
    warten:=100; //Schlafmodus nur sehr selten wandern
  EndIf
  TestMessage("Warten "+warten,3);
  //Für Funktionen die über Idle laufen, also keine Events nutzen (Kampf, Master Folgen, eca.)
  //Nach einer Event getriggerten Aktion sollen die schnell fortgesetzt werden können
  If (fastround > 0)
    //weiter ohne Event (Achtung: starke Prozlast bei Dauernutzung)
    SleepMs(100);
    ev:=Wait_for_EventLogging(0);
    If (!(ev))
      fastround-=1;
    EndIf
  Else
    TestMessage("Idlespeed/Wartezeit: " + params[IDLESPEED]+"/"+warten,2);
    ev:=Wait_for_EventLogging(warten);
  EndIf
  Return(ev);
EndFunction

Function Event_DoubleClicked_Common(byref ev)
  If(GetObjProperty(Me,PROP_MASTER))
    If(ev.source.serial == GetObjProperty(Me,PROP_MASTER)) //nur
      //Event_DblClick(me, master);
    EndIf
  EndIf
  Return(1);
EndFunction

Function Event_ItemGiven_Common(byref ev)
  If(GetObjProperty(Me,PROP_MASTER))
    If(ev.source.serial == GetObjProperty(Me,PROP_MASTER)) //nur
      //EatIt(ev);
    EndIf
  EndIf
  Return(1);
EndFunction

Function Event_Speech_Common(byref ev)
  If(GetObjProperty(Me,PROP_MASTER))
    If(ev.source.serial == GetObjProperty(Me,PROP_MASTER)) //nur wenns der Master war interessierts ueberhaupt
      //angreIfen
      If( (ev.text[Lower(Me.name) + " kill"]) || (ev.text == "all kill") )
        If (Can_Speech_Kill())
          Var opp:=Target(Me.master, TGTOPT_HARMFUL+TGTOPT_CHECK_LOS);
          If(opp)
            If( opp != Me.master)
              Var newev := struct;
              newev.+type;
              newev.+source;
              newev.+master;
              newev.source := opp;
              newev.type := SYSEVENT_ENGAGED;
              newev.master := GetObjProperty(Me,PROP_MASTER);
              SendEvent(Me, newev);
            Else
              SendSysMessagePergon(Me.master,Me.name + " schaut Euch mit grossen Augen fragend an...");
            EndIf
          EndIf
        EndIf
      ElseIf( (ev.text[Lower(Me.name) + " guard me"]) || (ev.text == "all guard me") )
        If (Can_Speech_Guard())
          Var newev := struct;
          SetObjProperty(Me,PROP_GUARD,GetObjProperty(Me,PROP_MASTER));
          SetObjProperty(Me,PROP_FOLLOWMASTER, GetObjProperty(Me,PROP_MASTER));
          newev.+type:=EVID_LOOKAROUND;
          newev.+source:=Me.master;
          SendEvent(Me,newev);
          SendSysMessagePergon(Me.master,Me.name + " wird Euch beschuetzen.");
        EndIf
      ElseIf( (ev.text[Lower(Me.name) + " stop"]) || (ev.text == "all stop") )
        SendSysMessagePergon(Me.master,Me.name + " wird aufhoeren Euch zu beschuetzen zu kaempfen oder Euch hinterherzulaufen.");
        EraseObjProperty(Me,PROP_GUARD);
        EraseObjProperty(Me,PROP_FOLLOWMASTER);
        EraseObjProperty(Me,PROP_STAY);
        MakeMePeaceful(params[RANGESENSE]);
      ElseIf( (ev.text[Lower(Me.name) + " follow me"]) || (ev.text == "all follow me") )
        SendSysMessagePergon(Me.master,Me.name + " wird Euch hinterherlaufen.");
        SetObjProperty(Me,PROP_FOLLOWMASTER, GetObjProperty(Me,PROP_MASTER));
        EraseObjProperty(Me,PROP_STAY); //natuerlich bleibt es nicht gleichzeitig stehen
        EraseObjProperty(Me,PROP_GUARD); //guard modus wieder wegnehmen
      ElseIf( (ev.text[Lower(Me.name) + " stay"]) || (ev.text == "all stay") )
        SendSysMessagePergon(Me.master,Me.name + " wird hier stehenbleiben.");
        EraseObjProperty(Me,PROP_GUARD);
        EraseObjProperty(Me,PROP_FOLLOWMASTER);
        SetObjProperty(Me,PROP_STAY,GetObjProperty(Me,PROP_MASTER));
      EndIf
    EndIf
  EndIf
  Return(1);
EndFunction

Function Event_Engaged_Common(byref ev)
  If ((!ev.source) || (ev.source.serial==Me.serial))
    Return(0);
  EndIf
  If(!ev.source.isa(POLCLASS_NPC)) //keine npcs
    wandersteps:=params[IDLESTEPS];
  EndIf

  Var peace_prop:=GetObjProperty(Me, PROP_PEACEMAKING);
  If (peace_prop)
    TestMessage("MainLoop: peace_prop.done: "+peace_prop.done, 1);
    If ((peace_prop.done == 1) && (peace_prop.timeout) && (peace_prop.timeout > ReadGameClock()))
      //EVID_PEACEMADE von Musik noch nicht verarbeitet, sollte aber vorliegen, ENGAGE verwerfen
      TestMessage("MainLoop: Wegen Musik SYSEVENT_ENGAGED nicht umsetzen.", 1);
      Return(0);
    Else
      EraseObjProperty(Me, PROP_PEACEMAKING);
    EndIf
  EndIf

  If(GetObjProperty(Me,PROP_MASTER))
    If( ev.master == GetObjProperty(Me,PROP_MASTER))
      SetWarmode(0); //um das setzen des neuen vom master gewuenschten Gegners zu ermoeglichen
    EndIf
  Else
    If (NATURE<>NATURE_TOWNGUARD)
      If (ev.source.isa(POLCLASS_NPC))
        If ((!ev.source.master) && (!ev.source.getprop(PROP_MASTER)) && (!ev.source.getprop(PROP_SUMMONED)))
          Return(0);  // Greifen sich nicht gegenseitig an
        EndIf
      EndIf
    EndIf
  EndIf
  Return(1);
EndFunction

Function Event_Disengaged_Common(byref ev)
  Return(0);
  ev:=ev;
EndFunction

Function Event_Damaged_Common(byref ev)
  If ((!ev.source) || (ev.source.serial==Me.serial))
    Return(0);
  EndIf
  If(!ev.source.isa(POLCLASS_NPC)) //keine npcs
    wandersteps:=params[IDLESTEPS];
  EndIf

  Var peace_prop:=GetObjProperty(Me, PROP_PEACEMAKING);
  If (peace_prop)
    TestMessage("MainLoop: peace_prop.done: "+peace_prop.done, 1);
    If ((peace_prop.done == 1) && (peace_prop.timeout) && (peace_prop.timeout > ReadGameClock()))
      //EVID_PEACEMADE von Musik noch nicht verarbeitet, sollte aber vorliegen, ENGAGE verwerfen
      TestMessage("MainLoop: Wegen Musik SYSEVENT_DAMAGED nicht umsetzen.", 1);
      Return(0);
    Else
      EraseObjProperty(Me, PROP_PEACEMAKING);
    EndIf
  EndIf
  If (params[SPELLDISPEL])  // Kann ich dispeln?
    If (params[CASTDISPEL]==0)  // Bin noch inaktiv
      If (!CheckLineOfSight(me,ev.source))  // Die Sau ist wech
        If (Distance(me,ev.source)<params[RANGESENSE]) // Aber eigentlich noch in Reichweite
          params[CASTDISPEL]:=1;
        EndIf
      EndIf
    EndIf
  EndIf
  
  If ((!GetObjProperty(Me,PROP_MASTER)) && (NATURE<>NATURE_TOWNGUARD))
    If (ev.source.isa(POLCLASS_NPC))
      If ((!ev.source.master) && (!ev.source.getprop(PROP_MASTER)) && (!ev.source.getprop(PROP_SUMMONED)))
        Return(0);  // Greifen sich nicht gegenseitig an
      EndIf
    EndIf
  EndIf

  Return(1);
EndFunction

Function Event_EnteredArea_Common(byref ev)
  If(!ev.source.isa(POLCLASS_NPC)) //keine npcs
    wandersteps:=params[IDLESTEPS];
  EndIf

  Var peace_prop:=GetObjProperty(Me, PROP_PEACEMAKING);
  If (peace_prop)
    TestMessage("MainLoop: peace_prop.done: "+peace_prop.done, 1);
    If ((peace_prop.done == 1) && (peace_prop.timeout) && (peace_prop.timeout > ReadGameClock()))
      //EVID_PEACEMADE von Musik noch nicht verarbeitet, sollte aber vorliegen, ENGAGE verwerfen
      TestMessage("MainLoop: Wegen Musik SYSEVENT_ENTEREDAREA verschieben.", 1);
      SendEvent(Me,ev);
      Return(0);
    Else
      EraseObjProperty(Me, PROP_PEACEMAKING);
    EndIf
  EndIf

  If( GetObjProperty(Me,PROP_MASTER) ) //nix - bin ja zahm
    Return(0);
  EndIf

  If (!Me.warmode)  // Gegner zum ersten mal entdeckt
    Case (MobType)
      MOB_HUMAN:    PerformAction(Me, UACTION_LOOK_AROUND);
      MOB_SEA:      PerformAction(Me, ANIM_RUN_ARMED);
      MOB_ANIMAL:   PerformAction(Me, ANIM_ATTACK_1HAND_JAB);
      MOB_MONSTER:  PerformAction(Me, ANIM_ATTACK_BOW);
      MOB_UNKNOWN:
    EndCase
  EndIf
  Return(1);
EndFunction

Function Event_LeftArea_Common(byref ev)
  Return;
  ev:=ev;
EndFunction

Function Event_LookAround_Common(byref ev)
  Var peace_prop:=GetObjProperty(Me, PROP_PEACEMAKING);
  If (peace_prop)
    TestMessage("MainLoop: peace_prop.done: "+peace_prop.done, 1);
    If ((peace_prop.done == 1) && (peace_prop.timeout) && (peace_prop.timeout > ReadGameClock()))
      //EVID_PEACEMADE von Musik noch nicht verarbeitet, sollte aber vorliegen, ENGAGE verwerfen
      TestMessage("MainLoop: Wegen Musik SYSEVENT_LOOKAROUND verschieben.", 1);
      SendEvent(Me,ev);
      Return(0);
    Else
      EraseObjProperty(Me, PROP_PEACEMAKING);
    EndIf
  EndIf

  Case (MobType)
    MOB_HUMAN:    PerformAction(Me, UACTION_LOOK_AROUND);
    MOB_SEA:      PerformAction(Me, ANIM_RUN_ARMED);
    MOB_ANIMAL:   PerformAction(Me, ANIM_ATTACK_1HAND_JAB);
    MOB_MONSTER:  PerformAction(Me, ANIM_ATTACK_BOW);
    MOB_UNKNOWN:
  EndCase

  Var newopp:=0;
  Var range:=params[RANGESENSE];

  If (ev.range)
    range:=ev.range;
  EndIf

  If (ev.help)
    newopp := LookForNextHostile( range, ev.help);
    If (!(newopp)&&(NATURE==NATURE_TOWNGUARD)) //Stadtwachen helfen auch wenn der Hilferufer nicht direkt attackiert wird
      newopp := LookForNextOpponent( range );
    EndIf
  Else
    newopp := LookForNextOpponent( range );
  EndIf

  If( newopp )
    If (params[ANGERSOUND])
      PlaySoundEffect(me, params[ANGERSOUND]);
    EndIf
    SetWarmode(0); //warmode kurz verlassen um Setzen eines neuen Gegners zu ermöglichen
    Return(newopp);
  Else
    Return(-1);
  EndIf
EndFunction

Function Event_Follow_Common(byref ev)
  Return;
  ev:=ev;
EndFunction

Function Event_AskJoin_Common(byref ev)
  Var groupsize := GetObjProperty( Me, PROP_MEMBERS ).size()+1;
  If( (GetObjProperty( Me, PROP_LEADER )) && ( groupsize < params[GROUPSIZE]))
    AddMember(ev.source);
  Else
    RefuseMember(ev.source);
  EndIf
EndFunction

Function Event_GrantJoin_Common(byref ev)
  JoinGroup(ev.source); //herde beitreten
EndFunction

Function Event_RefuseJoin_Common(byref ev)
  //erst beim naechsten idle weitersuchen...
  If(!RandomInt(5)) // 20% chance
    MakeMeLeader(); //manchmal leader werden, weil es sonst in der naehe einer herde nur streuner gibt
  EndIf
  Return;
  ev:=ev;
EndFunction

Function Event_Walk_Common(byref ev)
  Var i;
  For(i:=0;i<ev.range[3];i+=1)
    If (!((Me.x==ev.range[1]) && (Me.y==ev.range[2])))
      If( cint(ev.range[4]) == 1)
        RunTowardLocation(ev.range[1], ev.range[2]);
      Else
        WalkTowardLocation(ev.range[1], ev.range[2]);
      EndIf
    EndIf
  EndFor
  Return(1);
EndFunction

Function Event_Peacemade_Common(byref ev)
  //Für Musik
  Var peace_prop:=GetObjProperty(Me, PROP_PEACEMAKING);
  If (peace_prop)
    TestMessage("MainLoop: friedlich sein durch Musik.", 1);
    peace_prop.done:=2;
    SetObjProperty(Me, PROP_PEACEMAKING, peace_prop);
  EndIf
  Return(1);
  ev:=ev;
EndFunction

Function Event_Guards_Help_Common(byref ev)
  Return;
  ev:=ev;
EndFunction

Function Event_Cast_Wait_Common(byref ev)
  If (params.exists(SPELLCASTER))  // Darf er überhaupt?
    If (Me.warmode) // Kämpf ich noch?
      Return(MagicFight(ev.source));
    EndIf
  EndIf
  Return(0);
EndFunction

Function Event_Reflection_Common(byref ev)
  Return;
  ev := ev;
EndFunction

Function ValidEnemy_Override_Common(byref mob)
  If (!(mob.isa(POLCLASS_NPC)) && (mob.cmdlevel < CMDLEVEL_SEER))
    Return(1);
  ElseIf (mob.script in {"enticedanimal","tamed",":main_ai3:quest_escort"} )
    If (mob.npctemplate!="magilite")
      Return(1);
    EndIf
  EndIf
  Return(0);
EndFunction
