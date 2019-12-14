use datafile;

Include "include/server";

Const PERMISSION_HOUSE             := "AllowedToHouse";
Const PERMISSION_JAIL              := "AllowedToJail";
Const PERMISSION_RUNE              := "AllowedToRune";
Const PERMISSION_CHECKIP           := "AllowedToCheckIP";
Const PERMISSION_CHANGECPROPS      := "AllowedToChangeCProps";
Const PERMISSION_ACCESSMAIL        := "AllowedToAccessMail";
Const PERMISSION_ACCESSACCOUNTS    := "AllowedToAccessAccounts";
Const PERMISSION_ACCESSPASSWORDS   := "AllowedToAccessPasswords";
Const PERMISSION_CREATENPC         := "AllowedToCreateNPC";
Const PERMISSION_ACCESSWEB         := "AllowedToAccessWeb";
Const PERMISSION_ACCESSWEBFULL     := "AllowedToAccessWebFull";
Const PERMISSION_CHANGENAMES       := "AllowedToChangeNames";
Const PERMISSION_CHANGESKILLS      := "AllowedToChangeSkills";
Const PERMISSION_CHANGEPERMISSIONS := "AllowedToChangePermissions";

Function InitPermissions(overwrite := 0)
  
  var permdata := OpenDataFile("::Permissions");
  If (!permdata)
    permdata := CreateDataFile("::Permissions", DF_KEYTYPE_STRING);
  EndIf

  ForEach permission in (permissions)
    var perm := permdata.FindElement(permission);
    If (!perm)
      perm := permdata.CreateElement(permission);
    ElseIf (overwrite)
      permdata.DeleteElement(permission);
      perm := permdata.CreateElement(permission);
    Else
      continue;
    EndIf
    Case(permission)
      PERMISSION_HOUSE:
        perm.setprop("minLevel", CMDLEVEL_DEVELOPER);
      PERMISSION_JAIL:
        perm.setprop("minLevel", CMDLEVEL_HIGHGM);
      PERMISSION_RUNE:
        perm.setprop("minLevel", CMDLEVEL_GL);
      PERMISSION_CHECKIP:
        perm.setprop("minLevel", CMDLEVEL_HIGHGM);
      PERMISSION_CHANGECPROPS:
        perm.setprop("minLevel", CMDLEVEL_HIGHGM);
      PERMISSION_ACCESSMAIL:
        perm.setprop("minLevel", CMDLEVEL_DEVELOPER);
      PERMISSION_ACCESSACCOUNTS:
        perm.setprop("minLevel", CMDLEVEL_DEVELOPER);
      PERMISSION_ACCESSPASSWORDS:
        perm.setprop("minLevel", CMDLEVEL_DEVELOPER);
      PERMISSION_CREATENPC:
        perm.setprop("minLevel", CMDLEVEL_HIGHGM);
      PERMISSION_ACCESSWEB:
        perm.setprop("minLevel", CMDLEVEL_SEER);
      PERMISSION_ACCESSWEBFULL:
        var accounts := {
          "turley",
          "ursus"
        };
        perm.setprop("minLevel", CMDLEVEL_DEVELOPER);
        perm.setprop("enabled", accounts);
      PERMISSION_CHANGENAMES:
        perm.setprop("minLevel", CMDLEVEL_GL);
      PERMISSION_CHANGESKILLS:
        perm.setprop("minLevel", CMDLEVEL_DEVELOPER);
      PERMISSION_CHANGEPERMISSIONS:
        perm.setprop("minLevel", CMDLEVEL_DEVELOPER);
    EndCase
  EndForEach
  
  return 1;
EndFunction

// wer darf an Häusern herumspielen?
Function AllowedToHouse(who)
  
  var permdata := OpenDataFile("::Permissions");
  var permission := permdata.FindElement(PERMISSION_HOUSE);
  var minLevel := permission.GetProp("minLevel");
  var acctname := Lower(who.acctname);
  var enabled := permission.GetProp("enabled");
  var disabled := permission.GetProp("disabled");
  
  If (
    (who.cmdlevel >= minLevel and !(acctname in disabled)) or
    acctname in enabled
  )
    return 1;
  EndIf
  
  return 0;
  
EndFunction

// wer darf einknasten/anprangern/steinbrechen etc.
Function AllowedToJail(who)
  
  var permdata := OpenDataFile("::Permissions");
  var permission := permdata.FindElement(PERMISSION_JAIL);
  var minLevel := permission.GetProp("minLevel");
  var acctname := Lower(who.acctname);
  var enabled := permission.GetProp("enabled");
  var disabled := permission.GetProp("disabled");
  
  If (
    (who.cmdlevel >= minLevel and !(acctname in disabled)) or
    acctname in enabled
  )
    return 1;
  EndIf
  
  return 0;
  
EndFunction

// wer darf an Runen arbeiten
Function AllowedToRune(who)
  
  var permdata := OpenDataFile("::Permissions");
  var permission := permdata.FindElement(PERMISSION_RUNE);
  var minLevel := permission.GetProp("minLevel");
  var acctname := Lower(who.acctname);
  var enabled := permission.GetProp("enabled");
  var disabled := permission.GetProp("disabled");
  
  If (
    (who.cmdlevel >= minLevel and !(acctname in disabled)) or
    acctname in enabled
  )
    return 1;
  EndIf
  
  return 0;
  
EndFunction

// wer darf Zweitcharsupport pruefen
Function AllowedToCheckIP(who)

  var permdata := OpenDataFile("::Permissions");
  var permission := permdata.FindElement(PERMISSION_CHECKIP);
  var minLevel := permission.GetProp("minLevel");
  var acctname := Lower(who.acctname);
  var enabled := permission.GetProp("enabled");
  var disabled := permission.GetProp("disabled");
  
  If (
    (who.cmdlevel >= minLevel and !(acctname in disabled)) or
    acctname in enabled
  )
    return 1;
  EndIf
  
  return 0;
  
EndFunction

// wer darf CProps (.editcprops et al.) aendern
Function AllowedToChangeCProps(who)
  
  var permdata := OpenDataFile("::Permissions");
  var permission := permdata.FindElement(PERMISSION_CHANGECPROPS);
  var minLevel := permission.GetProp("minLevel");
  var acctname := Lower(who.acctname);
  var enabled := permission.GetProp("enabled");
  var disabled := permission.GetProp("disabled");
  
  If (
    (who.cmdlevel >= minLevel and !(acctname in disabled)) or
    acctname in enabled
  )
    return 1;
  EndIf
  
  return 0;

EndFunction

// wer darf Mailadressen einsehen/aendern
Function AllowedToAccessMail(who)
  
  var permdata := OpenDataFile("::Permissions");
  var permission := permdata.FindElement(PERMISSION_ACCESSMAIL);
  var minLevel := permission.GetProp("minLevel");
  var acctname := Lower(who.acctname);
  var enabled := permission.GetProp("enabled");
  var disabled := permission.GetProp("disabled");
  
  If (
    (who.cmdlevel >= minLevel and !(acctname in disabled)) or
    acctname in enabled
  )
    return 1;
  EndIf
  
  return 0;
  
EndFunction

// wer darf Accounts/Chars aendern/absegnen
Function AllowedToAccessAccounts(acct)

  var permdata := OpenDataFile("::Permissions");
  
  If (TypeOfInt(acct) == OT_STRING)
    acct := FindAccount(acct);
  Else
    acct := acct.acct;
  EndIf
  
  var permission := permdata.FindElement(PERMISSION_ACCESSACCOUNTS);
  var minLevel := permission.GetProp("minLevel");
  var acctname := Lower(acct.name);
  var enabled := permission.GetProp("enabled");
  var disabled := permission.GetProp("disabled");
  
  If (
    (GetMaxAccountCmdlevel(acct) >= minLevel and !(acctname in disabled)) or
    acctname in enabled
  )
    return 1;
  EndIf
  
  return 0;

EndFunction

// wer darf fremde Passwoerter aendern
Function AllowedToAccessPasswords(who)
  
  var permdata := OpenDataFile("::Permissions");
  var permission := permdata.FindElement(PERMISSION_ACCESSPASSWORDS);
  var minLevel := permission.GetProp("minLevel");
  var acctname := Lower(who.acctname);
  var enabled := permission.GetProp("enabled");
  var disabled := permission.GetProp("disabled");
  
  If (
    (who.cmdlevel >= minLevel and !(acctname in disabled)) or
    acctname in enabled
  )
    return 1;
  EndIf
  
  return 0;
  
EndFunction

// wer darf NPCs (in Massen) erzeugen?
Function AllowedToCreateNPC(who)
  
  var permdata := OpenDataFile("::Permissions");
  var permission := permdata.FindElement(PERMISSION_CREATENPC);
  var minLevel := permission.GetProp("minLevel");
  var acctname := Lower(who.acctname);
  var enabled := permission.GetProp("enabled");
  var disabled := permission.GetProp("disabled");
  
  If (
    (who.cmdlevel >= minLevel and !(acctname in disabled)) or
    acctname in enabled
  )
    return 1;
  EndIf
  
  return 0;
  
EndFunction

// Zugriff auf das Webinterface ist etwas komplexer
Function AllowedToAccessWeb(user, pass)

  If (QueryIP() == "127.0.0.1")
    // lokal darf man immer
    return 1;
  EndIf

  If (user == "")
    // kein Nutzername -- gleich abbrechen
    return 0;
  EndIf

  var acct := FindAccount(user);
  If (!acct)
      // Account existiert nicht
      return 0;
  EndIf

  var webpass := acct.getprop(ACCT_WEBPASS);
  If (webpass)
    webpass:=MD5Encrypt(user+webpass);
    // wenn es ein Webpasswort gibt, so wird nur das geprueft
    If (webpass <> pass)
      // Passwort falsch
      return 0;
    EndIf
  Else
    // ansonsten wird das normale Passwort benutzt
    If (acct.usernamepasswordhash <> pass)
      // Passwort falsch
      return 0;
    EndIf
  EndIf
  
  var permdata := OpenDataFile("::Permissions");
  var permission := permdata.FindElement(PERMISSION_ACCESSWEB);
  var minLevel := permission.GetProp("minLevel");
  var acctname := Lower(user);
  var enabled := permission.GetProp("enabled");
  var disabled := permission.GetProp("disabled");
  
  If (
    (GetMaxAccountCmdLevel(acct) >= minLevel and !(acctname in disabled)) or
    acctname in enabled
  )
    return 1;
  EndIf
  
  return 0;
  
EndFunction

// Zugriff auf das Webinterface mit vollem Funktionsumfang
Function AllowedToAccessWebFull(user, pass)

  If (QueryIP() == "127.0.0.1")
    // lokal darf man immer
    return 1;
  EndIf

  If (user == "")
    // kein Nutzername -- gleich abbrechen
    return 0;
  EndIf

  var acct := FindAccount(user);
  If (!acct)
      // Account existiert nicht
      return 0;
  EndIf

  var webpass := acct.getprop(ACCT_WEBPASS);
  If (webpass)
    webpass:=MD5Encrypt(user+webpass);
    // wenn es ein Webpasswort gibt, so wird nur das geprueft
    If (webpass <> pass)
      // Passwort falsch
      return 0;
    EndIf
  Else
    // ansonsten wird das normale Passwort benutzt
    If (acct.usernamepasswordhash <> pass)
      // Passwort falsch
      return 0;
    EndIf
  EndIf
  
  var permdata := OpenDataFile("::Permissions");
  var permission := permdata.FindElement(PERMISSION_ACCESSWEBFULL);
  var minLevel := permission.GetProp("minLevel");
  var acctname := Lower(user);
  var enabled := permission.GetProp("enabled");
  var disabled := permission.GetProp("disabled");
  
  If (
    (GetMaxAccountCmdLevel(acct) >= minLevel and !(acctname in disabled)) or
    acctname in enabled
  )
    return 1;
  EndIf
  
  return 0;
  
EndFunction

// wer darf Spielernamen aendern/absegnen
Function AllowedToChangeNames(acct) // {{{
  
  var permdata := OpenDataFile("::Permissions");
  
  If (TypeOfInt(acct) == OT_STRING)
    acct := FindAccount(acct);
  Else
    acct := acct.acct;
  EndIf
  
  var permission := permdata.FindElement(PERMISSION_CHANGENAMES);
  var minLevel := permission.GetProp("minLevel");
  var acctname := Lower(acct.name);
  var enabled := permission.GetProp("enabled");
  var disabled := permission.GetProp("disabled");
  
  If (
    (GetMaxAccountCmdLevel(acct) >= minLevel and !(acctname in disabled)) or
    acctname in enabled
  )
    return 1;
  EndIf
  
  return 0;
    
EndFunction // }}}

// Wer darf Skills aendern?
Function AllowedToChangeSkills(who) // {{{
 
  var permdata := OpenDataFile("::Permissions");
  var permission := permdata.FindElement(PERMISSION_CHANGESKILLS);
  var minLevel := permission.GetProp("minLevel");
  var acctname := Lower(who.acctname);
  var enabled := permission.GetProp("enabled");
  var disabled := permission.GetProp("disabled");
  
  If (
    (who.cmdlevel >= minLevel and !(acctname in disabled)) or
    acctname in enabled
  )
    return 1;
  EndIf
  
  return 0;
  
EndFunction // }}}

Function AllowedToChangePermissions(acct)
  
  var permdata := OpenDataFile("::Permissions");
  var permission := permdata.FindElement(PERMISSION_CHANGEPERMISSIONS);

  If (TypeOfInt(acct) == OT_STRING)
    acct := FindAccount(acct);
  Else
    acct := acct.acct;
  EndIf

  var minLevel := permission.GetProp("minLevel");
  var acctname := Lower(acct.name);
  var enabled := permission.GetProp("enabled");
  var disabled := permission.GetProp("disabled");

  If (
    (GetMaxAccountCmdLevel(acct) >= minLevel and !(acctname in disabled)) or
    acctname in enabled
  )
    return 1;
  EndIf
  
  return 0;
  
EndFunction

// Helfer, wenn man nur Account hat und Cmdlevel braucht
Function GetMaxAccountCmdlevel(acct) // {{{
  var level := -1;
  For slot := 1 To 6
    SleepMs(2);
    var char := acct.getcharacter(slot);
    If (char and char.cmdlevel > level)
      level := char.cmdlevel;
    EndIf
  EndFor

  return level;
EndFunction // }}}

Function GetMaxAccountCmdChar(acct) // {{{
  var level := -1;
  var maxchar := -1;
  For slot := 1 To 6
    SleepMs(2);
    var char := acct.getcharacter(slot);
    If (char and char.cmdlevel > level)
      level := char.cmdlevel;
      maxchar := char;
    EndIf
  EndFor

  return maxchar;
EndFunction