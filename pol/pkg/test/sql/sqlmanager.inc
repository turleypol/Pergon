Use os;

///////////////
// Konstanten
///////////////

Const PID_SQLMANAGER:= "#SQLMANAGER";
Const EVENTQUEUESIZE_SQLMANAGER:=500;

Const DEBUG_EVENTS_SQLMANAGER:=1;

Enum SQLStatus
  SQLStatus_Logon := 0,
  SQLStatus_LogOff,
  SQLStatus_UpdateName,
  SQLStatus_UpdateRegion,
  SQLStatus_Concealed,
  SQLStatus_Shutdown,
  SQLStatus_SetShutdownMsg,
  SQLStatus_UpdateStatus,
  SQLStatus_CreateAccount,
  SQLStatus_NewName,
  SQLStatus_CharDeleted,
  SQLStatus_Trigger_Sync
EndEnum

Function SQLSanitize(str)
  Return(StrReplace(str,"'","''"));
EndFunction

Function SQLDebugLog(text)
  LogToFile("::log/sql.log",StrFormatTime("[%d.%m. %H:%M:%S] ")+text);
EndFunction

// Sender
Function SendSQLManager(object, status, detail := 0)
  Var pid:=GetGlobalProperty(PID_SQLMANAGER);
  If (!pid)
    Return;
  EndIf
  Var waiter:=GetProcess(pid);
  If (!waiter)
    // Kommentarlos schlucken, damit man ihn ohne spam deaktivieren kann
    //SysLog("FEHLER: SQLManager nicht gestartet: "+waiter);
    Return;
  EndIf
  Var event:=struct;
  If (object.serial)
    event.+serial:=object.serial;
  EndIf
  event.+status:=status;
  If (detail)
    event.+detail:=detail;
  EndIf

  While ((waiter.sendevent(event)).errortext=="Event queue is full, discarding event")
    If (Is_Critical())
      Start_ScriptPergon("SendSQLManager", struct{
        obj := object,
        status := status,
        detail := detail
      });
      return;
    EndIf
    SleepMS(RandomInt(100)+2);

    If (DEBUG_EVENTS_SQLMANAGER)
      SysLog("FEHLER: SQLManagerEventQueue voll (EventQueueSize = "+EVENTQUEUESIZE_SQLMANAGER+")!");
    EndIf
  EndWhile
EndFunction