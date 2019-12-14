//////////////////////////////////////////
//
//   Combat Include v1.0 (2001/11/19)
//
//     Author: Shinigami
//
//   Modifications:
//     2001/11/19 Shinigami: First Write
//
//////////////////////////////////////////
//
// Fight                  - Gegner bekaempfen
// CloseDistanceMobile    - Verringert den Abstand zum Mobile   (erstmal auskommentiert)
// CloseDistanceLocation  - Verringert den Abstand zur Location
// SleepMSClearEventQueue - Warten und Event-Queue leeren
// ClearEventQueue        - Event-Queue leeren
// FleeOutOfSenseRange    - Flucht vor dem Gegener              (erstmal nur aus Grazer uebernommen)

// ToDo:
//   ClearEventQue -> ClearEventQueue

//////////////////////
// Globale Variablen
//////////////////////

Var attack_other:=1;   // Wenn mich ein anderer angreift, haue ich dann zurueck?
Var senserange:=20;
Var attackhitsound:=0;

//////////////////////////////
// Fight - Gegner bekaempfen
//////////////////////////////

//Function Fight(me, opponent)
Function Fight(opponent)
 // If (opponent And (!opponent.cmdlevel)) // Hab ich einen Gegner?
  If (opponent) // Hab ich einen Gegner?
    SetOpponent(opponent);       // Gegner als solchen kennzeichnen

    // ToDo: Folgende Funktionen ueberarbeiten
    Get_Help(opponent);          // Hilfe anfordern (zuerst, damit die Event-Queue geloescht wird)
    Prepare_for_Fight(opponent); // Kampfvorbereitungen durchfuehren

    Var oldpriority:=Set_Priority(50); // Prioritaet hochsetzen

    Var lastx;                         // Verfolgung
    Var lasty;
    Var pathx:=me.x;
    Var pathy:=me.y;

    Var dist:=Distance(me, opponent);  // Abstandsbestimmung
    Var olddist:=-1;

    Var lostimeout:=0;                 // TimeOut, Stopmarke, etc.
    Var stopped:=0;
    Var loops:=0;                      // ToDo: Funktion des Zaehlers ueberdenken

    While (opponent And (!opponent.dead) And (!stopped) And (dist<25))
      loops:=loops+1; // Zaehler, der aus alten Routinen stammt

      SysLog("  CheckLOS = "+CheckLineOfSight(me, opponent));
      SysLog("  Hidden   = "+opponent.hidden);

      If (CheckLineOfSight(me, opponent) And (!opponent.hidden)) // Sehe ich meinen Gegner?
PrintTextAbovePergon(0, me, "Ich sehe meinen Gegner");
        If ((olddist<>dist) Or (dist<=1)) // Hat sich der Abstand zum Gegner veraendert, oder kaempfe ich grad mit ihm?
          lostimeout:=ReadGameClock()+10; // Ja, TimeOut neu setzen
          olddist:=dist;
PrintTextAbovePergon(0, me, "Abstand anners -> neues TimeOut");
        EndIf

PrintTextAbovePergon(0, me, "Event-Schleife Start");
       // Var ev:=Wait_for_EventLogging(CloseDistanceMobile(me, opponent)); // Zum Gegner aufschliessen und auf Events warten
        Var ev:=Wait_for_EventLogging(CloseDistance(opponent)); // Zum Gegner aufschliessen und auf Events warten
        If (ev)
          Repeat // Durch diese Schleife werden Events geloescht, die nicht bearbeitet werden sollen
            If (ev.source=opponent) // Event vom Gegner?
              Case (ev.type)
                SYSEVENT_OPPONENT_MOVED:
PrintTextAbovePergon(0, me, "Gegner hat sich bewegt");
                  lastx:=me.x;lasty:=me.y;
                  If (CoordinateDistanceEuclidean(pathx, pathy, lastx, lasty)>5) // Verfolge ich gerade meinen beweglichen Gegner?
PrintTextAbovePergon(0, me, "Bewegung - neue Pos'n'TimeOut");
                    lostimeout:=ReadGameClock()+10;                    // Ja, TimeOut neu setzen
                    pathx:=lastx;pathy:=lasty;
                  EndIf
                  Break; // Repeat-Schleife verlassen, um zum Gegner aufzuschliessen
              EndCase

              // ToDo: Folgende Funktion und Abfrage ueberarbeiten
              stopped:=Process_Combat_Event(ev, loops); // Sonstige Events/Kampfbegleitende Aktion abarbeiten
              If (stopped) // Abbrechen?
                Break;
              EndIf
            Else // Event von wem anders
              Case (ev.type)
                SYSEVENT_ENGAGED:
                SYSEVENT_DAMAGED:
                  If (!CheckLineOfSight(me, opponent))
                    If (attack_other)                 // Soll ich zurueckhauen?
                      Fight(ev.source);               // Bekaempfe anderen Gegner
                    Else
                      FleeOutOfSenseRange(ev.source); // So schnell wie moeglich weg von hier...
                      stopped:=1;
                    EndIf
                    Break;            // Repeat-Schleife verlassen, um alten Gegner wiederzufinden
                  EndIf
              EndCase
            EndIf
          Until (!(ev:=Wait_for_EventLogging(0)));
        EndIf
PrintTextAbovePergon(0, me, "Event-Schleife Ende");

        If (lostimeout<ReadGameClock()) // Ist mein Gegner unerreichbar?
          stopped:=1;
        EndIf
      Else // Irgendwie sehe ich meinen Gegner nicht (Haeuser, Hoehlen, Baeume, etc.pp.)
PrintTextAbovePergon(0, me, "Wo ist mein Gegner");
        Var nolostimeout:=0; // TimeOut
        dist:=CInt(CoordinateDistanceEuclidean(me.x, me.y, lastx, lasty)); // Abstand bestimmen
        olddist:=-1;
PrintTextAbovePergon(0, me, "Zur letzten Pos laufen");
        Repeat // Zur letzten bekannten Position laufen - Abbruch, wenn Gegner wieder gesichtet
          If (olddist<>dist)                  // Hat sich der Abstand zum Gegner veraendert?
            nolostimeout:=ReadGameClock()+10; // Ja, TimeOut neu setzen
            olddist:=dist;
          EndIf

          CloseDistanceLocation(me, lastx, lasty, dist); // Zur letzten Gegnerposition aufschliessen
          SleepMSClearEventQueue(10);                    // Warten und Events ignorieren
          dist:=CInt(CoordinateDistanceEuclidean(me.x, me.y, lastx, lasty)); // Abstand bestimmen
        Until ((dist<=1) Or (CheckLineOfSight(me, opponent) And (!opponent.hidden)) Or (nolostimeout<ReadGameClock()));
PrintTextAbovePergon(0, me, "Bin gelaufen");

        If (dist<=1) // An der letzten bekannten Position stehen und warten
PrintTextAbovePergon(0, me, "Nu warte ich, ob er auftaucht");
          nolostimeout:=ReadGameClock()+10; // TimeOut setzen
          Repeat                            // Warten - Abbruch, wenn Gegner wieder gesichtet
            SleepMSClearEventQueue(100);    // Warten und Events ignorieren
          Until ((CheckLineOfSight(me, opponent) And (!opponent.hidden)) Or (nolostimeout<ReadGameClock()));

          If (nolostimeout<ReadGameClock()) // Abbruch durch TimeOut?
            stopped:=1;                     // Ja, der Gegner ist entwischt
          EndIf
        Else
          stopped:=1; // Ich kann meinen Gegner nicht erreichen, Abbruch
        EndIf

        olddist:=-1; // Abstandsmesser initialisieren
      EndIf

      // ToDo: Folgende Funktion ueberarbeiten - hier muesste eigentlich noch 'n Abbruch moeglich sein
      In_Combat_Event_Loop(opponent, loops); // Allg. kampfbegleitende Aktion durchfuehren

      dist:=Distance(me, opponent);          // Abstand zu meinem Gegner neu bestimmen
    EndWhile
PrintTextAbovePergon(0, me, "Genuch gekaempft");

    Set_Priority(oldpriority); // Prioritaet zuruecksetzen

    // ToDo: Folgende Funktionen ueberarbeiten
    After_Combat_Event_Loop(opponent, loops); // Kampfnachbereitungen durchfuehren
    Post_Combat();

    // ToDo: Folgendes Anweisungen sind eigentlich schon in Post_Combat vertreten
   // SetWarMode(0); // Der Gegner kann ja noch leben und der Kampf wurde abgebrochen...
   // Sleep(1);      // Verschnaufpause
  Else
    SetWarMode(0); // Ich habe keinen Gegner, oder es ist ein CMDLevel
  EndIf
EndFunction

////////////////////////////////////////////////////////////
// CloseDistanceMobile - Verringert den Abstand zum Mobile
////////////////////////////////////////////////////////////
//
//Function CloseDistanceMobile(me, mobile)
//  Case (Distance(me, mobile))
//    1: // oben, weil am haeufigsten
//    0: TurnToward(mobile);
//       Return (1);
//    2: WalkToward(mobile);
//
//    default: RunToward(mobile);
//  EndCase
//EndFunction

////////////////////////////////////////////////////////////////
// CloseDistanceLocation - Verringert den Abstand zur Location
////////////////////////////////////////////////////////////////

Function CloseDistanceLocation(me, x, y, dist)
  Case (dist)
    1: // oben, weil am haeufigsten
    0: Return (1);
    2: WalkTowardLocation(x, y);

    default: RunTowardLocation(x, y);
  EndCase
EndFunction

///////////////////////////////////////////////////////////
// SleepMSClearEventQueue - Warten und Event-Queue leeren
///////////////////////////////////////////////////////////

Function SleepMSClearEventQueue(time)
  SleepMS(time);
  Repeat Until (!Wait_for_EventLogging(0));
EndFunction

/////////////////////////////////////////
// ClearEventQueue - Event-Queue leeren
/////////////////////////////////////////

Function ClearEventQueue()
  Repeat Until (!Wait_for_EventLogging(0));
EndFunction

/////////////////////////////////////////////////
// FleeOutOfSenseRange - Flucht vor dem Gegener
/////////////////////////////////////////////////

Function FleeOutOfSenseRange(opponent)
  ClearEventQueue();

  Var fleestart:=ReadGameClock()+30;
  Var dist:=Distance(me, opponent);
  While (opponent And (!opponent.dead) And (!opponent.hidden) And (!Events_Waiting()) And
        ((dist<senserange+1) Or CheckLineOfSight(me, opponent)))
    If (fleestart<ReadGameClock())
      fleestart:=ReadGameClock()+30;

      If ((GetMaxHPPergon(me)>=50) And (dist<CInt(senserange/2)) And (dist<8))
        If (attackhitsound)
          PlaySoundEffect(me, attackhitsound);
        EndIf

        Fight(opponent);
        Return;
      EndIf
    EndIf

    RunAwayFrom(opponent);
    SleepMS(200);

    dist:=Distance(me, opponent);
  EndWhile
EndFunction