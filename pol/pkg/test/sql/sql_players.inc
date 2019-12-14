
// Main Status functions
Function logon_player(byref connection, who)
  If (exists_player(connection, who))
    logon_update_player( connection, who);
  Else
    logon_add_player( connection, who);
  EndIf
EndFunction


// gibts den schon?
Function exists_player(byref connection,who)
  Var time := ReadMilliSecondClock();
  Var update_q := "SELECT Serial FROM HP_OnlineCharacters_{} Where Serial={}".format(SERVERNAME, who.serial);
  Var res := mysql_query(connection,update_q);
  SQLDebugLog("Query exist player: {}".format(ReadMilliSecondClock()-time));
  If (res)
    If (mysql_num_rows(res)>0)
      Return(1);
    EndIf
  Else
    SQLDebugLog("FEHLER: sqlmanager exists_player "+res+" "+update_q);
    SysLog("FEHLER: sqlmanager exists_player "+res+" "+update_q);
    mysql_close(connection);
  EndIf
  Return(0);
EndFunction

// Beim logon alles updaten
Function logon_update_player(byref connection,who)
  var PK := 0;
  If (GetObjProperty(who, "permared"))
    PK := 1;
  EndIf
  Var update_q := ("UPDATE HP_OnlineCharacters_{1.server} SET "+
    "Account='{1.account}', Name='{1.name}', Gender={1.gender}, RolePlayer={1.role}, "+
    "Profession='{1.profession}', ProfessionVisible={1.prof_vis}, LiveMapVisible={1.livemap}, Region='{1.region}', "+
    "LogonTime={1.time}, Online={1.online}, CmdLevel={1.cmdlvl}, ConcealLevel={1.concealed}, "+
    "Murderer={1.pk}, LogonIP='{1.ip}', PosX={1.x}, PosY={1.y}, PosZ={1.z}  WHERE Serial={1.serial}").format(struct {
    serial    := who.serial,
    account   := SQLSanitize(who.acctname),
    name      := SQLSanitize(who.name),
    gender    := who.gender,
    role      := wants_rp(who),
    profession:= get_profession(who),
    prof_vis  := show_profession(who),
    livemap   := livemap_visible(who),
    region    := get_region_name(who),
    time      := PolCore().systime,
    online    := 1,
    cmdlvl    := who.cmdlevel,
    concealed := conceal_level(who),
    pk        := PK,
    ip        := who.ip,
    x         := who.x,
    y         := who.y,
    z         := who.z,
    server    := SERVERNAME});
  Var time := ReadMilliSecondClock();
  Var res := mysql_query(connection,update_q);
  SQLDebugLog("Query logon update: {}".format(ReadMilliSecondClock()-time));
  If (res)
    Return(1);
  EndIf
  SQLDebugLog("FEHLER: sqlmanager logon_update_player "+res+" "+update_q);
  SysLog("FEHLER: sqlmanager logon_update_player "+res+" "+update_q);
  mysql_close(connection);
  Return(0);
EndFunction

// noch nicht in der db
Function logon_add_player (byref connection, who)  
  var PK := 0;
  If (GetObjProperty(who, "permared"))
    PK := 1;
  EndIf
  Var update_q := ("INSERT INTO HP_OnlineCharacters_{1.server} (Serial,Account,Name,Gender,RolePlayer,Profession,ProfessionVisible,LiveMapVisible,Region,LogonTime,Online,CmdLevel,ConcealLevel,Murderer,LogonIP,PosX,PosY,PosZ)"+
    " VALUES ({1.serial},'{1.account}','{1.name}',{1.gender},{1.role},'{1.profession}',{1.prof_vis},{1.livemap},'{1.region}',{1.time},{1.online},{1.cmdlvl},{1.concealed},{1.pk},'{1.ip}',{1.x},{1.y},{1.z})").format(struct {
      serial    := who.serial,
      account   := SQLSanitize(who.acctname),
      name      := SQLSanitize(who.name),
      gender    := who.gender,
      role      := wants_rp(who),
      profession:= get_profession(who),
      prof_vis  := show_profession(who),
      livemap   := livemap_visible(who),
      region    := get_region_name(who),
      time      := PolCore().systime,
      online    := 1,
      cmdlvl    := who.cmdlevel,
      concealed := conceal_level(who),
      pk        := PK,
      ip        := who.ip,
      x         := who.x,
      y         := who.y,
      z         := who.z,
      server    := SERVERNAME});
  Var time := ReadMilliSecondClock();
  Var res := mysql_query(connection,update_q);
  SQLDebugLog("Query logon add: {}".format(ReadMilliSecondClock()-time));
  If (res)
    Return(1);
  EndIf
  SQLDebugLog("FEHLER: sqlmanager logon_add_player "+res+" "+update_q);
  SysLog("FEHLER: sqlmanager logon_add_player "+res+" "+update_q);
  mysql_close(connection);
  Return(0);
EndFunction


Function logoff_player(byref connection, who)
  Var update_q := "UPDATE HP_OnlineCharacters_{1.server} SET Online={1.online} WHERE Serial={1.serial}".format(struct {
    serial := who.serial,
    online := 0,
    server := SERVERNAME});
  Var time := ReadMilliSecondClock();
  Var res := mysql_query(connection,update_q);
  SQLDebugLog("Query logoff: {}".format(ReadMilliSecondClock()-time));
  If (res)
    Return(1);
  EndIf
  SQLDebugLog("FEHLER: sqlmanager logoff_player "+res+" "+update_q);
  SysLog("FEHLER: sqlmanager logoff_player "+res+" "+update_q);
  mysql_close(connection);
  Return(0);
EndFunction

Function update_name_player(byref connection,who)
  Var update_q := "UPDATE HP_OnlineCharacters_{1.server} SET Name='{1.name}' WHERE Serial={1.serial}".format(struct {
    serial := who.serial,
    name   := SQLSanitize(who.name),
    server := SERVERNAME});
  Var time := ReadMilliSecondClock();
  Var res := mysql_query(connection,update_q);
  SQLDebugLog("Query update name: {}".format(ReadMilliSecondClock()-time));
  If (res)
    Return(1);
  EndIf
  SQLDebugLog("FEHLER: sqlmanager update_name_player "+res+" "+update_q);
  SysLog("FEHLER: sqlmanager update_name_player "+res+" "+update_q);
  mysql_close(connection);
  Return(0);
EndFunction

Function update_region_player(byref connection,who)
  Var update_q := "UPDATE HP_OnlineCharacters_{1.server} SET Region='{1.region}', PosX={1.x}, PosY={1.y}, PosZ={1.z} WHERE Serial={1.serial}".format(struct {
    serial := who.serial,
    region := get_region_name(who),
    x := who.x, y := who.y, z := who.z,
    server := SERVERNAME});
  Var time := ReadMilliSecondClock();
  Var res := mysql_query(connection,update_q);
  SQLDebugLog("Query update region: {}".format(ReadMilliSecondClock()-time));
  If (res)
    Return(1);
  EndIf
  SQLDebugLog("FEHLER: sqlmanager region_player "+res+" "+update_q);
  SysLog("FEHLER: sqlmanager region_player "+res+" "+update_q);
  mysql_close(connection);
  Return(0);
EndFunction

Function update_concealedlvl_player(byref connection,who)
  Var update_q := "UPDATE HP_OnlineCharacters_{1.server} SET ConcealLevel={1.concealed} WHERE Serial={1.serial}".format(struct {
    serial    := who.serial,
    concealed := conceal_level(who),
    server    := SERVERNAME});
  Var time := ReadMilliSecondClock();
  Var res := mysql_query(connection,update_q);
  SQLDebugLog("Query update concealed: {}".format(ReadMilliSecondClock()-time));
  If (res)
    Return(1);
  EndIf
  SQLDebugLog("FEHLER: sqlmanager concealed_player "+res+" "+update_q);
  SysLog("FEHLER: sqlmanager concealed_player "+res+" "+update_q);
  mysql_close(connection);
  Return(0);
EndFunction

Function update_status_player(byref connection,who)
  Var update_q := "UPDATE HP_OnlineCharacters_{1.server} SET RolePlayer={1.role}, ProfessionVisible={1.prof}, LiveMapVisible={1.livemap} WHERE Serial={1.serial}".format(struct {
    serial := who.serial,
    role   := wants_rp(who),
    prof   := show_profession(who),
    livemap:= livemap_visible(who),
    server := SERVERNAME});
  Var time := ReadMilliSecondClock();
  Var res := mysql_query(connection,update_q);
  SQLDebugLog("Query update concealed: {}".format(ReadMilliSecondClock()-time));
  If (res)
    Return(1);
  EndIf
  SQLDebugLog("FEHLER: sqlmanager concealed_player "+res+" "+update_q);
  SysLog("FEHLER: sqlmanager concealed_player "+res+" "+update_q);
  mysql_close(connection);
  Return(0);
EndFunction

Function get_region_name(who)
  If (who.dead)
    Return(SQLSanitize(TOTENREICH));
//  ElseIf ((who.multi) And (who.multi.isa(POLCLASS_BOAT)))
//    Return(PlaceName(who)+" "+BOOTFAEHRE);
  Else
    Return(SQLSanitize(PlaceName(who)));
  EndIf
EndFunction

Function get_profession(who)
  Var prof := GetObjProperty(who, SKBERUF);
  If (!prof)
    Return(" ");
  EndIf
  Return(SQLSanitize(prof));
EndFunction

Function conceal_level(who)
  If (who.getprop(PROP_HIDE_STAFF_ONLINE))
    Return(who.cmdlevel);
  Else
    Return(who.concealed);
  EndIf
EndFunction

Function wants_rp(who)
  If (who.getprop(PROP_WANTS_RP))
    Return(1);
  Else
    Return(0);
  Endif
EndFunction

Function show_profession(who)
  If (who.getprop(PROP_SHOW_PROF))
    Return(1);
  Else
    Return(0);
  Endif
EndFunction

Function livemap_visible(who)
  var status := who.getprop(PROP_LIVEMAP);
  If (status || status == error)
    Return(1);
  Else
    Return(0);
  EndIf
EndFunction
