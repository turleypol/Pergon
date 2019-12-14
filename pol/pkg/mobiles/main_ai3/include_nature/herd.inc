/////////////////////////////////////////////////////////////////////
// 2004-02-26 Mai -- Herdentiere sind Fluchtiere
//         sie folgen einem Leittier
/////////////////////////////////////////////////////////////////////

Function OnIdleHerd()
  TestMessage("OnIdleHerd", 2);

  If (GetObjProperty(Me, PROP_MASTER))
    If (GetObjProperty(Me, PROP_MASTERDEAD))
      // sorgt dafür das das Ideln möglichst schnell wieder angestoßen wird
      WaitForLivingMaster();
      fastround := 1;
      // 1s, damit die HauptschleIfe nicht zu schnell durchlaufen wird
      Sleep(1);
    Else
      If (Me.master.dead)
        // neue Treuezeit setzen (in s)
        SetObjProperty(Me, PROP_MASTERDEAD, ReadGameClock()+600);

        // sorgt dafür das das Ideln möglichst schnell wieder angestoßen wird
        fastround := 1;
        // 1s, damit die HauptschleIfe nicht zu schnell durchlaufen wird
        Sleep(1);
      EndIf
    EndIf
    // solange ich nen Master habe, mobil bleiben
    wandersteps := params[IDLESTEPS];
  EndIf

  // Event StillRun übernimmt das friedlich machen
  If (!Me.warmode)
    If (GetObjProperty(Me, PROP_STAY))
      // nichts tun, nichtmal umherlaufen
      Sleep(100);
    ElseIf (GetObjProperty(Me, PROP_FOLLOWMASTER))
      FollowCharacter(Me.master);
    Else
      OnIdleGroup();
    EndIf
  EndIf
EndFunction

Function FleeHerd(byref opponent, byref rangefollow)
  TestMessage("FleePack", 2);
  var timeout := ReadGameClock()+2;

  var evflee := struct;
  evflee.+type   := EVID_STILLRUN;
  evflee.+source := opponent;
  evflee.+range  := rangefollow;
  var xold := Me.x;
  var yold := Me.y;

  // damit er von seinem anchorpoint wegrennen kann
  SetWarmode(1);

  wandersteps := params[IDLESTEPS]; // nach der Flucht Rumwandern

  // waehrend ich renne interessiert mich das nicht
  DisableEvents(SYSEVENT_ENTEREDAREA);
  var leader := 0;
  If (Me.getprop(PROP_FOLLOW))
    leader := SystemFindObjectBySerial(Me.getprop(PROP_FOLLOW));
  EndIf

  If (GetObjProperty(Me, PROP_LEADER)) // Tier ist Anfuehrer
    OrganizeFlee(opponent, rangefollow);
    TestMessage("FleeHerd: Renne weg vor "+CharInfoStr(opponent), 1);
    While (ReadGameClock()<timeout)
      If (
        // gibt Gegner noch
        (opponent) && (opponent.connected) &&
        // ist in der Naehe
        (Distance(Me, opponent) <= rangefollow) &&
        // und nicht weg
        !(opponent.hidden) && !(opponent.concealed) && !(opponent.dead)
      )
        RunAwayFrom(opponent);
        PathFindAwayFrom(opponent, xold, yold);

      Else // der bisherige Gegner ist zu weit weg
        // Umschauen ob nicht noch jemand boeses da ist
        var ev := struct;
        ev.+type := EVID_LOOKAROUND;
        ev.+source := Me;
        SendEvent(Me, ev);
        SleepMS(30);
        return(0);
      EndIf
    EndWhile
    SendEvent(Me, evflee);
    return;

  ElseIf (leader) // Herdentier hat derzeit Anfuehrer
      // dem Leittier mitteilen
      var ev := struct;
      ev.+type := SYSEVENT_ENGAGED;
      ev.+source := opponent;
      ev.+range := rangefollow;
      // nicht den leader nerven, er organisiert doch schon den
      // angriff oder kaempft
      If (!leader.warmode)
        If (opponent.isa(POLCLASS_NPC))
          SendEvent(leader, ev);
        Else
          If (!GetObjProperty(leader, PROP_MASTER))
            // falls der Master zahm ist, kein event schicken
            // um evtl grauwerden des opfers zu vermeiden
            SendEvent(leader, ev);
          EndIf
        EndIf
    EndIf
  EndIf

  // normales Herdentier
  While (ReadGameClock() < timeout)
    SleepMS(2);
    If (
      // gibt Gegner noch
      (opponent) && (opponent.connected) &&
      // ist in der Naehe
      (Distance(Me, opponent) <= rangefollow) &&
      // und nicht weg
      !(opponent.hidden) && !(opponent.concealed) && !(opponent.dead)
    )

      If (leader)
        TestMessage("FleeHerd: Folge Leader "+CharInfoStr(leader), 1);
        GoToMob(leader, -3); // Damit er immer Rennt negative mindist
      Else
        // Leittier ist hinueber
        TestMessage("FleeHerd: Leader tot, Neugruppieren", 1);
        If (Me.getprop(PROP_FOLLOW))
          // neue Herde bilden
          MakeMeWanderer();
          FormGroup();
        EndIf
      EndIf

    Else // der bisherige Gegner ist zu weit weg
      // Umschauen ob nicht noch jemand boeses da ist
      var ev := struct;
      ev.+type := EVID_LOOKAROUND;
      ev.+source := Me;
      SendEvent(Me, ev);
      SleepMS(30);
      return(0);
    EndIf
  EndWhile

  SendEvent(Me, evflee);
EndFunction

Function OrganizeFlee(byref opponent, byref rangefollow)
  TestMessage("OrganizeFlee", 2);
  var members := GetObjProperty(Me, PROP_MEMBERS);
  var ev := struct;
  ev.+type   := SYSEVENT_ENGAGED;
  ev.+source := opponent;
  ev.+range  := rangefollow;
  ev.+leader := Me.serial;

  // alle Member durchgehen und Fluchtziel vorgeben
  ForEach serial in members
    SleepMs(2);
    var member := SystemFindObjectBySerial(serial);
    If (!member)
      // das Mitglied hat es schon zerlegt
      continue;
    EndIf
    // TODO: Check noetig, ob das Member noch zur Gruppe gehoert
    // (weggezaehmt?)

    If (GetObjProperty(member, PROP_FOLLOW) == Me.serial)
    // laeuft es auch wirklich mir hinterher?
    // (vielleicht hats 2 leader gefragt..)
      // dem Member sagen, dass es angegriffen wird
      SendEvent(member, ev);
    EndIf
  EndForEach
EndFunction
