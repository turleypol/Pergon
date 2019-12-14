///////////////////////////////////////////////////////////////////////////
//
//   EventWaiter-Include - Funktionen rund um Event-Verteilung
//
//     Author: Turley
//
//   Modifications
//     07.09.06 Turley - Array Global gemacht, damit man ihn extern anschauen kann
//     08.09.06 Turley - Umbenennung da allgemeiner und zu "Rundpuffer" umgebaut
//     08.09.06 Fox    - source ist nicht zwingend auch das Ziel des Events
//										 - Fehlerlog, nen paar Sicherungen und nen Bug
//     2006/09/16 Shinigami: Array fangen mit 1 an, Events mit 0-Zeit kamen 'ne Stunde zu spaet
//     2006/09/22 Shinigami: Verlagerung in eine Include und erweitertes Logging
//     2006/10/05 Shinigami: Sendefehler an die Waiter werden geloggt
//                           Unversandte Events werden in den naechsten Slot verschoben
//                           Groesse der EventQueue radikal erhoeht
//     06.10.06 Turley - 5 Minuten Slots beim EventWaiterLong
//     2006/10/07 Shinigami: einige Variablennamen an ihre neue Bedeutung angepasst
//     01.11.06 Turley - Sysload/Severity mitloggen
//     03.11.06 Turley - Bissle besser gemacht das Sysload loggen
//     06.12.06 Turley - An Scriptrefs schicken nun auch möglich
//
///////////////////////////////////////////////////////////////////////////
//
// Empfaenger
//   EventWaiter            - Nimmt die Events auf und verschickt sie nach Wartezeit
//
// Sender
//   DoSendEventWaiter      - Schickt Event zum zentralen Script
//   SendEventWaiter        - Schickt Event zum zentralen Script (Ziel=Quelle und ev ohne Nutzlast)
//   SendEventWaiterGeneral - Schickt Event zum zentralen Script (ev mit Nutzlast)

use file;
use os;
use uo;
include "include/clock";

///////////////
// Konstanten
///////////////

Const PID_EVENTWAITERLONG:= "#EventWaiterLong";
Const PID_EVENTWAITERSHORT:="#EventWaiterShort";

Const SHORT_LONG:=3600; // Ab wieviel Sekunden Wartezeit wird auf das langsame Script umgeschaltet?
Const LONG_RANGE:=300;  // Wieviele Sekunden umfasst ein Slot des LongWaiters?

Const EVENTQUEUESIZE:=50000; // Die Initialisierung verlangt diese Groesse

Const DEBUG_EVENTS:=0;

////////////////////
//                //
//   Empfaenger   //
//                //
////////////////////

/////////////////////////////////////////////////////////////////////////
// EventWaiter - Nimmt die Events auf und verschickt sie nach Wartezeit
/////////////////////////////////////////////////////////////////////////

Function EventWaiter(log_uo_time:=0)
  Var events:={};
  Var index:=1;

  Var serial_failed_global:=0;
  Var events_resend_global:=0;
  Var events_send_global:=0;
  Var serial_failed;
  Var events_resend;
  Var events_send;
  Var sendresult;

  Var waittime, ev, slot, nextindex, nextwait, sysload, severity, sysload_iter;
    

  If (DEBUG_EVENTS)
    LogToFile(LOGFILE_IN, "Time;Script;Source;Destination;Index;WaitTime;Slot;SlotSize;Type");
    If (log_uo_time)
      LogToFile(LOGFILE_OUT, "Time;UOTime;Index;SlotSize;Send;ReSend;SerialFailed;SendGlobal;ReSendGlobal;SerialFailedGlobal;SysLoad;Severity");
    Else
      LogToFile(LOGFILE_OUT, "Time;Index;SlotSize;Send;ReSend;SerialFailed;SendGlobal;ReSendGlobal;SerialFailedGlobal");
    EndIf
  EndIf

  Set_Event_Queue_Size(EVENTQUEUESIZE);

  SetGlobalProperty(PID_EVENTWAITER, GetPid());
  events[SIZE]:={}; // Array Size festlegen

  waittime:=ReadGameClock()+WAIT;
  While (1)
  	SleepMS(2); // Kleiner RunawaySchutz wenn massig Events ankommen
    ev:=Wait_For_Event(WAIT);
    If (ev)
      If (ev.erase)
        EraseSerial(events,ev.erase);
      ElseIf (ev.time<SIZE)
        slot:=index+ev.time; // Relativ zur ZeigerPos
        If (slot>SIZE)
          slot-=SIZE;
        EndIf

        If (events[slot])
          events[slot].append(ev);
        Else
          events[slot]:={ev};
        EndIf

        If (DEBUG_EVENTS)
          LogToFile(LOGFILE_IN, ";"+ev.source.script+";"+Lower(Hex(ev.source.serial))+";"+Lower(Hex(ev.destination.serial))+
            ";"+index+";"+ev.time+";"+slot+";"+events[slot].size()+";"+Lower(Hex(ev.type)), 1);
        EndIf
      Else
        LogToFile("log/timer.log", GetDateTimeStr()+" FEHLER: "+ev+" hat zu langes WaitTime ("+ev.time+") !");
      EndIf
    EndIf

    If (waittime<=ReadGameClock()) // Schon Wartezeit vergangen? Also nächsten Event wegschicken
      waittime:=ReadGameClock()+WAIT;

      nextindex:=index+1; // Zeiger
      If (nextindex>SIZE)
        nextindex:=1;
      EndIf

      If (DEBUG_EVENTS)
        serial_failed:=0;
        events_resend:=0;
        events_send:=0;
      EndIf

      nextwait:=50;
      If (DEBUG_EVENTS)
        If ((events[index].size()>1) && (events[index]<>{})) // An welcher Stelle wird die sysload geloggt
     		  sysload_iter:=CInt(events[index].size()/2);
        Else  //Nur ein Event oder gar keins, also gleich loggen
       	  sysload:=polcore().sysload;
          severity:=polcore().sysload_severity;
        EndIf
      EndIf

      ForEach event in (events[index])
        If (nextwait==_event_iter) // Runaway-Schutz
          SleepMS(RandomInt(200)); // Die eigene EventQueue ist gross genug um zu puffern
          nextwait+=50;
        EndIf

        If (DEBUG_EVENTS)
          If (sysload_iter==_event_iter)
        	  sysload:=polcore().sysload;
        	  severity:=polcore().sysload_severity;
          EndIf
        EndIf
        If (((event.destination.isA(POLCLASS_NPC)) && (event.destination.serial)) || (event.destination.pid))
        	If (event.destination.isA(POLCLASS_NPC))
            sendresult:=SendEvent(event.destination, event);
          ElseIf (event.destination.pid)
          	sendresult:=event.destination.sendevent(event);
          EndIf
          If (sendresult)
            If (DEBUG_EVENTS)
              events_send_global+=1;
              events_send+=1;
            EndIf
          ElseIf (sendresult.errortext=="Event queue is full, discarding event")
            events[nextindex].append(event); // Event weiterreichen

            If (DEBUG_EVENTS)
              events_resend_global+=1;
              events_resend+=1;
            EndIf
          Else // NPC zwischendurch gestorben etc.
            If (DEBUG_EVENTS)
              serial_failed_global+=1;
              serial_failed+=1;
            EndIf
          EndIf
        Else
          If (DEBUG_EVENTS)
            serial_failed_global+=1;
            serial_failed+=1;
          EndIf
        EndIf
      EndForEach

      events[index]:={}; // Die Events sind versandt also weg damit aus der Sendeliste

      If (DEBUG_EVENTS)
        If (log_uo_time)
          LogToFile(LOGFILE_OUT, ";"+GetPergonTimeOfDay()+";"+index+";"+(events_send+events_resend+serial_failed)+";"+events_send+";"+
            events_resend+";"+serial_failed+";"+events_send_global+";"+events_resend_global+";"+serial_failed_global+";"+sysload+";"+severity, 1);
        Else
          LogToFile(LOGFILE_OUT, ";"+index+";"+(events_send+events_resend+serial_failed)+";"+events_send+";"+events_resend+
            ";"+serial_failed+";"+events_send_global+";"+events_resend_global+";"+serial_failed_global, 1);
        EndIf
      EndIf

      index:=nextindex;
    EndIf
  EndWhile
EndFunction

Function EraseSerial(byref events,serial)
  Var i;
  ForEach evindex in events
    For (i:=1;i<=evindex.size();i+=1)
      If (evindex[i].destination.isA(POLCLASS_NPC))
        If (evindex[i].destination.serial==serial)
          evindex.erase(i);
          i-=1;
        EndIf
      EndIf
    EndFor
    SleepMS(2);
  EndForEach
EndFunction

////////////////
//            //
//   Sender   //
//            //
////////////////

///////////////////////////////////////////////////////////
// DoSendEventWaiter - Schickt Event zum zentralen Script
///////////////////////////////////////////////////////////

Function DoSendEventWaiter(pid, ev)
  Var waiter:=GetProcess(GetGlobalProperty(pid));
  While (!waiter)
    Sleep(5);
    waiter:=GetProcess(GetGlobalProperty(pid));
  EndWhile

  While ((waiter.sendevent(ev)).errortext=="Event queue is full, discarding event")
    SleepMS(RandomInt(10000));

    If (DEBUG_EVENTS)
      SysLog("FEHLER: EventQueue voll (EventQueueSize = "+EVENTQUEUESIZE+")!");
    EndIf
  EndWhile
EndFunction

////////////////////////////////////////////////////////////////////////////////////////////
// SendEventWaiter - Schickt Event zum zentralen Script (Ziel=Quelle und ev ohne Nutzlast)
////////////////////////////////////////////////////////////////////////////////////////////

Function SendEventWaiter(source, waittime, type)
  If (waittime>=SHORT_LONG) // Ab 1h in das ungenauere langsamere Script
    DoSendEventWaiter(PID_EVENTWAITERLONG, Struct{source:=source, time:=CInt(waittime/LONG_RANGE), type:=type, destination:=source});
  Else
    DoSendEventWaiter(PID_EVENTWAITERSHORT, Struct{source:=source, time:=CInt(waittime), type:=type, destination:=source});
  EndIf
EndFunction

//////////////////////////////////////////////////////////////////////////////////
// SendEventWaiterGeneral - Schickt Event zum zentralen Script (ev mit Nutzlast)
//////////////////////////////////////////////////////////////////////////////////

Function SendEventWaiterGeneral(destination, waittime, ev)
  ev.+destination:=destination;

  If (waittime>=SHORT_LONG) // Ab 1h in das ungenauere langsamere Script
    ev.+time:=CInt(waittime/LONG_RANGE);
    DoSendEventWaiter(PID_EVENTWAITERLONG, ev);
  Else
    ev.+time:=CInt(waittime);
    DoSendEventWaiter(PID_EVENTWAITERSHORT, ev);
  EndIf
EndFunction
