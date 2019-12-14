
// gibts den schon?
Function exists_server(byref connection)
  Var time := ReadMilliSecondClock();
  Var update_q := "SELECT PolVersion FROM HP_OnlineSystem Where Name='{}'".format(SERVERNAME);
  Var res := mysql_query(connection,update_q);
  SQLDebugLog("Query server exists: {}".format(ReadMilliSecondClock()-time));
  If (res)
    If (mysql_num_rows(res)>0)
      Return(1);
    EndIf
  Else
    SQLDebugLog("FEHLER: sqlmanager server exists "+res+" "+update_q);
    SysLog("FEHLER: sqlmanager server exists "+res+" "+update_q);
    mysql_close(connection);
  EndIf
  Return(0);
EndFunction

Function server_add(byref connection)
  Var update_q := "INSERT INTO HP_OnlineSystem (Name, PolStart,PolVersion,PolAccounts,PolChars,PolShutdownMessage,PolShutdownTime,Time,PolIngameTime,PolNpcs,PolTopLayerItems,PolGMPages,PolBuild,PolCrashTeamInformed)"+
  " VALUES ('{}',0,'',0,0,'',0,0,0,0,0,0,'',0)".format(SERVERNAME);
  Var time := ReadMilliSecondClock();
  Var res := mysql_query(connection,update_q);
  SQLDebugLog("Query update server add: {}".format(ReadMilliSecondClock()-time));
  If (res)
    Return(1);
  EndIf
  SQLDebugLog("FEHLER: sqlmanager server start add "+res+" "+update_q);
  SysLog("FEHLER: sqlmanager server start add "+res+" "+update_q);
  mysql_close(connection);
  Return(0);
EndFunction

Function server_start_update(byref connection)
  // PolStart = Unix pol start
  // PolSoftwareVersion = kennung vom core
  // PolPlayers  = anz spieler start
  // PolChars = aktive chars start
  var statistik := CountActiveCharacters();
  var system:=PolCore();
  If (!exists_server(connection))
    server_add(connection);
  EndIf
  Var update_q := ("UPDATE HP_OnlineSystem SET "+
    "PolStart={1.start}, PolVersion='{1.version}', PolAccounts={1.players}, PolChars={1.chars}, PolShutdownMessage='{1.msg}', PolShutdownTime={1.time}, PolBuild='{1.build}', PolCrashTeamInformed=0, PolAuthToken={1.token}"
    +" WHERE Name='{1.server}'").format(struct {
    start   := system.systime,
    version := SQLSanitize(system.verstr),
    players := statistik[4],
    chars   := statistik[8],
    msg     := "",
    time    := 0,
    build   := SQLSanitize(system.compiledate+ " "+system.compiletime),
    token   := pol_auth_token().current,
    server  := SERVERNAME
   });
  Var time := ReadMilliSecondClock();
  Var res := mysql_query(connection,update_q);
  SQLDebugLog("Query update server start: {}".format(ReadMilliSecondClock()-time));
  If (res)
    Return(1);
  EndIf
  SQLDebugLog("FEHLER: sqlmanager server start update "+res+" "+update_q);
  SysLog("FEHLER: sqlmanager server start update "+res+" "+update_q);
  mysql_close(connection);
  Return(0);
EndFunction

Function update_server_full(byref connection)
  Var system:=PolCore();
  Var update_q := ("UPDATE HP_OnlineSystem SET "+
    "Time={1.time}, PolIngameTime={1.igtime}, PolNpcs={1.npcs}, PolTopLayerItems={1.items}, PolGMPages={1.pages}, PolAuthToken={1.token}"
    +" WHERE Name='{1.server}'").format(struct {
    time   := system.systime,
    igtime := GetPergonTime(),
    npcs   := system.mobilecount,
    items  := system.itemcount,
    pages  := GetGlobalProperty(GM_PAGE_LIST).size(),
    token  := pol_auth_token().current,
    server := SERVERNAME
   });
  Var time := ReadMilliSecondClock();
  Var res := mysql_query(connection,update_q);
  SQLDebugLog("Query update server full: {}".format(ReadMilliSecondClock()-time));
  If (res)
    Return(1);
  EndIf
  SQLDebugLog("FEHLER: sqlmanager server full update "+res+" "+update_q);
  SysLog("FEHLER: sqlmanager server full update "+res+" "+update_q);
  mysql_close(connection);
  Return(0);
EndFunction

Function server_set_shutdown_msg(byref connection)
  Var msg  := "";
  Var shutdowntime := 0;
  Var running := GetGlobalProperty(PROP_SHUTDOWN_RUNNING);
  If (running)
    shutdowntime := running - ReadGameClock();
    shutdowntime += PolCore().systime;
    msg := GetGlobalProperty("#ShutDownMessage");
    If (msg)
      msg := SQLSanitize(msg);
    Else
      msg := GetGlobalProperty("#ShutDownMessageFormat");
      If (msg)
        msg := SQLSanitize(GetMotDText(0, msg));
      Else
        msg := "Wir werden alle sterben...";
      Endif
    EndIf
  EndIf
  Var update_q := ("UPDATE HP_OnlineSystem SET PolShutdownMessage='{1.msg}', PolShutdownTime={1.time} WHERE Name='{1.server}'").format(struct {
    msg    := msg,
    time   := shutdowntime,
    server := SERVERNAME
   });
  Var time := ReadMilliSecondClock();
  Var res := mysql_query(connection,update_q);
  SQLDebugLog("Query set shutdown msg: {}".format(ReadMilliSecondClock()-time));
  If (res)
    Return(1);
  EndIf
  SQLDebugLog("FEHLER: sqlmanager set shutdown msg "+res+" "+update_q);
  SysLog("FEHLER: sqlmanager set shutdown msg "+res+" "+update_q);
  mysql_close(connection);
  Return(0);
EndFunction

Function update_server(byref connection)
  Var system:=PolCore();
  Var update_q := ("UPDATE HP_OnlineSystem SET "+
    "Time={1.time}, PolIngameTime={1.igtime}, PolAuthToken={1.token}"
    +" WHERE Name='{1.server}'").format(struct {
    time   := system.systime,
    igtime := GetPergonTime(),
    token  := pol_auth_token().current,
    server := SERVERNAME
   });
  Var time := ReadMilliSecondClock();
  Var res := mysql_query(connection,update_q);
  SQLDebugLog("Query update small server: {}".format(ReadMilliSecondClock()-time));
  If (res)
    Return(1);
  EndIf
  SQLDebugLog("FEHLER: sqlmanager server small update "+res+" "+update_q);
  SysLog("FEHLER: sqlmanager server small update "+res+" "+update_q);
  mysql_close(connection);
  Return(0);
EndFunction

Function pol_auth_token()
  Var newtoken := RandomIntMinMax(-1000000000, 1000000000);
  Var token := GetGlobalProperty("PolAuthToken");
  If (token)
    token.last := token.current;
    token.current := newtoken;
  Else
    token := struct{last := newtoken, current := newtoken};
  EndIf
  SetGlobalProperty("PolAuthToken", token);
  return token;
EndFunction

Function server_create_account(byref connection)
  Var time := ReadMilliSecondClock();
  Var res := mysql_query(connection,"SELECT ID,Name,PergonPasswordPlain,Mail FROM HP_AccountCreation_{} WHERE (IngameCreated=0 AND IngameErrorMessage='' AND MailConfirmed=1)".format(SERVERNAME));
  SQLDebugLog("Query pending accounts: {}".format(ReadMilliSecondClock()-time));
  If (res)
    For i:=1 To (mysql_num_rows(res))
      var rows := mysql_fetch_row(res);
      If (rows[2] && rows[3])
        Var create_res := CreateAccount(rows[2], rows[3], 1);
        create_res.setprop(PLAYEREMAIL, rows[4]);
        
        If (!create_res)
          Syslog("FEHLER: konnte account nicht erstellen "+create_res.errortext);
          Var update_q := ("UPDATE HP_AccountCreation_{1.server} SET "+
                          "IngameErrorMessage='{1.text}'"
                          +" WHERE ID={1.id}").format(struct {
                          text   := SQLSanitize(create_res.errortext),
                          id     := rows[1],
                          server := SERVERNAME
                         });
          mysql_query(connection,update_q);
        Else
          Syslog("NEUER ACCOUNT: "+rows[2]);
          Var update_q := ("UPDATE HP_AccountCreation_{1.server} SET "+
                          "IngameCreated=1"
                          +" WHERE ID={1.id}").format(struct {
                          id     := rows[1],
                          server := SERVERNAME
                         });
          mysql_query(connection,update_q);
        EndIf
      EndIf
    EndFor
  Else
    SQLDebugLog("FEHLER: sqlmanager server_create_account "+res);
    SysLog("FEHLER: sqlmanager server_create_account "+res);
    mysql_close(connection);
  EndIf
EndFunction

////////////////////////////////////////////////////////////////
// NAMENSTABELLE

Function exists_naming(byref connection, serial)
  Var time := ReadMilliSecondClock();
  Var update_q := "SELECT Serial FROM HP_CharNaming_{} Where Serial={}".format(SERVERNAME, serial);
  Var res := mysql_query(connection,update_q);
  SQLDebugLog("Query exists_naming: {}".format(ReadMilliSecondClock()-time));
  If (res)
    If (mysql_num_rows(res)>0)
      Return(1);
    EndIf
  Else
    SQLDebugLog("FEHLER: sqlmanager exists_naming "+res+" "+update_q);
    SysLog("FEHLER: sqlmanager exists_naming "+res+" "+update_q);
    mysql_close(connection);
  EndIf
  Return(0);
EndFunction

Function add_new_name(byref connection, who, newname)
  Var update_q := ("INSERT INTO HP_CharNaming_{1.server} (Serial,Name,NameChangesHistory,Account,Reason1,Reason2,Reason3)"+
    " VALUES ({1.serial},'{1.name}','{1.reason}','{1.account}','{1.reason}','{1.reason}','{1.reason}')").format(struct {
      serial    := who.serial,
      account   := SQLSanitize(who.acctname),
      name      := SQLSanitize(newname),      
      reason    := "",
      server    := SERVERNAME});
  Var time := ReadMilliSecondClock();
  Var res := mysql_query(connection,update_q);
  SQLDebugLog("Query name add: {}".format(ReadMilliSecondClock()-time));
  If (res)
    Return(1);
  EndIf
  SQLDebugLog("FEHLER: sqlmanager add_new_name "+res+" "+update_q);
  SysLog("FEHLER: sqlmanager add_new_name "+res+" "+update_q);
  mysql_close(connection);
  Return(0);
EndFunction

Function update_name(byref connection, who, newname)
  Var update_q := ("UPDATE HP_CharNaming_{1.server} SET "+
  "Serial={1.serial}, Name='{1.name}', Account='{1.account}',"+
  "Judge1={1.judge}, Judge2={1.judge}, Judge3={1.judge},"+
  "Decision1={1.decision}, Decision2={1.decision}, Decision3={1.decision}, NameConfirmed={1.confirmed}, "+
  "Reason1='{1.reason}', Reason2='{1.reason}', Reason3='{1.reason}',"+
  "OwnerInformed={1.informed}, TeamInformed={1.informed}, NameApplied={1.applied}"+
  " WHERE Serial={1.serial}").format(struct {
      serial    := who.serial,
      account   := SQLSanitize(who.acctname),
      name      := SQLSanitize(newname),
      judge     := -1,
      decision  := -1,
      reason    := "",
      informed  := 0,
      applied   := 0,
      confirmed := -1,
      server    := SERVERNAME});
  Var time := ReadMilliSecondClock();
  Var res := mysql_query(connection,update_q);
  SQLDebugLog("Query name update: {}".format(ReadMilliSecondClock()-time));
  If (res)
    Return(1);
  EndIf
  SQLDebugLog("FEHLER: sqlmanager update_name "+res+" "+update_q);
  SysLog("FEHLER: sqlmanager update_name "+res+" "+update_q);
  mysql_close(connection);
  Return(0);
EndFunction

Function update_name_table(byref connection, who, newname)
  If (exists_naming(connection, who.serial))
    update_name(connection, who, newname);
  Else
    add_new_name(connection, who, newname);
  EndIf
EndFunction

Function server_set_non_applied_names(byref connection)
  Var time := ReadMilliSecondClock();
  Var res := mysql_query(connection,"SELECT Serial,Name,NameConfirmed,CharDeleted FROM HP_CharNaming_{} WHERE NameApplied=0 AND NameConfirmed in (0,1)".format(SERVERNAME));
  SQLDebugLog("Query non applied names: {}".format(ReadMilliSecondClock()-time));
  If (res)
    For i:=1 To (mysql_num_rows(res))
      var rows := mysql_fetch_row(res);
      If (rows[1] && rows[2])
        var serial:=rows[1];
        var name:=CleanupName(rows[2]);
        var confirmed:=rows[3];
        var deleted:=rows[4];
        var char := SystemFindObjectBySerial(
          serial, SYSFIND_SEARCH_OFFLINE_MOBILES
        );
        If (char)
          If (confirmed)
            SetNamePergon(char, name, "Namensteam");
            If (char.connected)
              SendStringAsTipWindow(char, "Euer angeforderter Name wurde soeben gesetzt.");
              char.eraseProp(PROP_NAME_CHECK);
            Else
              char.setProp(PROP_NAME_CHECK, NAME_APPLIED);
            EndIf
            syslog(
                "NAMEN: Name '"+name+"' ("+LHex(serial)+") bestaetigt/gesetzt"
            );
            Var update_q := ("UPDATE HP_CharNaming_{1.server} SET "+
              "NameChangesHistory = CASE WHEN NameChangesHistory = '' THEN '{1.name}' ELSE CONCAT(NameChangesHistory, ';', '{1.name}') END, "+
              "NameChangesCounter = NameChangesCounter+1, NameApplied=1 WHERE Serial={1.serial}").format(struct{
                name := name,
                serial := serial,
                server := SERVERNAME
              }
            );
            mysql_query(connection,update_q);
          Else
            char.setProp(PROP_NAME_CHECK, NAME_DENIED);
            syslog(
                "NAMEN: Von "+CharInfoStr(char)+" angeforderter Name '"+name+"' abgelehnt"
            );
            mysql_query(connection,"UPDATE HP_CharNaming_{1.server} SET NameApplied=1 WHERE Serial={1.serial}".format(struct{
              serial := serial,
              server := SERVERNAME
            }));
          EndIf
        ElseIf (!deleted)
          syslog("char serial ("+LHex(serial)+") existiert nicht mehr");
          mysql_query(connection,"UPDATE HP_CharNaming_{} SET CharDeleted=1 WHERE Serial={}".format(SERVERNAME,serial));
        EndIf
      EndIf
    EndFor
  Else
    SQLDebugLog("FEHLER: sqlmanager server_set_non_applied_names "+res);
    SysLog("FEHLER: sqlmanager server_set_non_applied_names "+res);
    mysql_close(connection);
    Return(0);
  EndIf
EndFunction

Function mark_deleted_name_table(byref connection, serial)
  Var time := ReadMilliSecondClock();
  Var res := mysql_query(connection,"UPDATE HP_CharNaming_{} SET CharDeleted=1 WHERE Serial={}".format(SERVERNAME,serial));
  SQLDebugLog("Query mark char deleted: {}".format(ReadMilliSecondClock()-time));
  If (!res)
    SQLDebugLog("FEHLER: sqlmanager mark_deleted_name_table "+res);
    SysLog("FEHLER: sqlmanager mark_deleted_name_table "+res);
    mysql_close(connection);
    Return(0);
  EndIf
EndFunction