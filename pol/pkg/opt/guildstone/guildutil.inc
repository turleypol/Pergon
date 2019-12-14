Const PROP_GUILD_LEADERS := "leaders"; // Property mit Anfuehrern an Gilde
Const GoetterGildeID     := 11;        // GuildID der GötterGilde

///////////////////////////////////////////
// IsGuildLeader - Ist Who einer der Leaders?
///////////////////////////////////////////
Function IsGuildLeader(byref who, byref guildid)
  // darf per Gilde oder Gilden-ID aufgerufen werden
  var guild_ := guildid;
  If (TypeOfInt(guild_) != OT_GUILDREF)
    guild_ := FindGuild(guildid);
  EndIf
  If (!guild_)
    // keine Gilde, kann also nicht Mitglied sein
    return 0;
  EndIf

  If (who.cmdlevel >= CMDLEVEL_GL)
    // Staffler ab GM haben immer Leaderstatus
    return 1;
  EndIf

  If (guild_.ismember(who))
    If (who.serial in (guild_.getprop(PROP_GUILD_LEADERS)))
      // Mitglied und in Anfuehrer-Array
      return 1;
    EndIf
  EndIf

  // dann wohl kein Anfuehrer
  return 0;
EndFunction

// who den Gildentitel entziehen
Function ResetGuildTitle(byref who)
  who.title_guild  := "";
  who.title_prefix := "";
  who.title_race   := "";
  var title := GetObjProperty(who,"guildtitle");
  If (title)
    var name        := who.name;
    name[title+" "] := "";
    who.name        := name;
  EndIf
  who.eraseprop("guildtitle");
  who.eraseprop("guild_id");
  who.eraseprop("gc_talk");
  who.eraseprop("gc_hear");
  IncRevision(who);
  UpdateMobile(who);
EndFunction

///////////////////////////////////////////
// RemoveMem - Entfernt Gilden-Mitglied
///////////////////////////////////////////
Function RemoveGuildMember(byref who, byref trgguild, msgwho:=0)
  If (!msgwho)
    msgwho := who;
  EndIf

  If (trgguild.members.size() <= 1)
    // Gilde aufloesen?
    SendSysMessagePergon(msgwho, "Ihr seit das letzte Mitglied!");
    If (!YesNoGumpGuild(msgwho, "Wollt Ihr die Gilde auflösen?"))
      SendSysMessagePergon(msgwho, "Eine weise Entscheidung.");
      return 0;
    EndIf

    trgguild.removemember(who);
    ResetGuildTitle(who);

    ForEach allies in (trgguild.allyguilds)
      trgguild.removeallyguild(allies);
      SleepMS(2);
    EndForEach
    ForEach enemy in (trgguild.enemyguilds)
      trgguild.removeenemyguild(enemy);
      SleepMS(2);
    EndForEach

    var destroy := DestroyGuild(trgguild);
    If (!destroy)
      SendSysMessagePergon(msgwho, "Fehler: "+destroy.errortext);
      return 0;
    EndIf

    SendSysMessagePergon(msgwho, "Gilde aufgeloest!");
    ResetGuildTitle(who);
    return 1;
    // fertig
  EndIf

  // Anfuehrerliste pruefen
  var leaderlist := trgguild.getprop(PROP_GUILD_LEADERS);
  If (leaderlist.size() <= 1 and leaderlist[1] == who.serial)
    SendSysMessagePergon(msgwho,
      "Ihr wolltet gerade den letzte Gildenanfuehrer entfernen. "+
      "Bestimmt erst einen anderen Anfuehrer oder loest die Gilde auf, "+
      "indem Ihr alle Mitglieder und Euch zuletzt entfernt."
    );
    return 0;
  EndIf

  // ggf. aus der Anfuehrerliste entfernen
  // (bei der Gelegenheit gleich mal geloeschte Chars aussortieren)
  var i;
  For (i := leaderlist.size(); i >= 1; i -= 1)
    SleepMS(2);
    If (leaderlist[i] == who.serial)
      // aktuellen Char herauswerfen
      leaderlist.erase(i);
      continue;
    EndIf

    // andere bleiben nur in der Liste, wenn man sie noch finden kann
    var otherleader := SystemFindObjectBySerial(
      leaderlist[i], SYSFIND_SEARCH_OFFLINE_MOBILES
    );
    If (!otherleader)
      leaderlist.erase(i);
      continue;
    EndIf
  EndFor
  trgguild.setprop(PROP_GUILD_LEADERS, leaderlist);

  // aus der Gilde entfernen
  trgguild.removeMember(who);
  ResetGuildTitle(who);

  return 1;
EndFunction

///////////////////////////////////////////
// def_sidebground - Hintergrund für Sidebar
///////////////////////////////////////////
Function def_sidebground(byref y)
  layoutback.append("resizepic 455 50 2600 350 "+y);
  If (y>200)
    layoutback.append("gumppic 773 20 10441");  // rechter Drache
  EndIf
EndFunction

///////////////////////////////////////////
// guildmsg - Verschickt Msg an alle Mitglieder
///////////////////////////////////////////
Function GuildMsg(targuild, msg, color, who1:=0, msg1:=0)
  If (who1)
    SendSysMessagePergon(who1, msg1,"",_DEFAULT_TEXT_FONT,color);
    ForEach memb in (targuild.members)
      If (memb.serial<>who1.serial)
        SendSysMessagePergon(memb, msg,"",_DEFAULT_TEXT_FONT, color);
        SleepMS(2);
      EndIf
    EndForEach
  Else
    ForEach memb in (targuild.members)
      SendSysMessagePergon(memb, msg,"",_DEFAULT_TEXT_FONT, color);
      SleepMS(2);
    EndForEach
  EndIf
EndFunction

///////////////////////////////////////////
// YesNoGumpGuild - ...
///////////////////////////////////////////
Function YesNoGumpGuild(who, text)
  Var yeslayout := {
    "nodispose",
    "page 0",
    "resizepic 50 50 2600 300 250",
    "gumppic 0 17 10400",
    "gumppic 0 265 10402",
    "gumppic 318 100 10411",
    "htmlgump 90 115 220 80 0 0 0",
    "button 120 210 9721 9724 1 0 1",
    "text 155 214 0 1",
    "button 220 210 9721 9724 1 0 0",
    "text 255 214 0 2"
  };
   Var yesdata := {
    "<basefont color=#00000F><center>"+text+"</center>", "Ja", "Nein"
  };
  return (SendDialogGump(who,yeslayout,yesdata,200,200)[0]);
EndFunction
