include "guildutil";

//////////////////////////////
// set_tag - Gildenkürzel ändern
//////////////////////////////
Function set_tag(byref who)
  Var tag;

  tag:=SendTextEntryGump(who, "Wie soll das neue Gildenkuerzel lauten?", TE_CANCEL_ENABLE, TE_STYLE_NORMAL, MAX_GUILD_TAG_SIZE, "Maximal "+MAX_GUILD_TAG_SIZE+", keine Leerzeichen!");
  If (!tag)
    SendSysMessagePergon(who, "Abgebrochen");
    Return;
  Else
    tag:=CStr(tag);
    guild.setprop("guild_tag", tag);
    SendSysMessagePergon(who,"Gildentag: "+tag);
    guildmsg(guild,"Es wurde ein neues Gildenkuerzel festgelegt! Es lautet: "+tag,FONTCOLOR_BROWN);
    ForEach memb in ( guild.members )
      If (memb.title_guild!="")
        memb.title_guild:=tag;
        IncRevision(memb);
        SendSysMessagePergon(memb, "Euer Gildenkuerzel wurde gewechselt!","",_DEFAULT_TEXT_FONT,FONTCOLOR_REDBROWN);
      EndIf
      SleepMS(2);
    EndForEach
  EndIf
EndFunction

//////////////////////////////
// set_name - Neuer Gildenname
//////////////////////////////

Function set_name(byref who, byref stone)
  Var name;

  name:=SendTextEntryGump(who, "Gildenname:", TE_CANCEL_ENABLE, TE_STYLE_NORMAL, MAX_GUILD_NAME_SIZE);
  If (!name)
    SendSysMessagePergon(who, "Abgebrochen");
    Return;
  Else
    name:=CStr(name);
    guild.setprop("name", name);
    stone.name:=name;
    SendSysMessagePergon(who,"Neuer Gildenname: "+name);
    guildmsg(guild,"Es wurde ein neuer Gildenname festgelegt! Er lautet: "+name,FONTCOLOR_BROWN);
  EndIf
EndFunction

//////////////////////////////
// toggle_abbrev - Kürzel anzeigen
//////////////////////////////

Function toggle_abbrev(byref who)
  Var title;
  If (who.title_guild)
    who.title_guild:="";
    IncRevision(who);
    SendSysMessagePergon(who, "Gildenkuerzel ist nun UNSICHTBAR.");
  Else
    title:=guild.getprop("guild_tag");
    If (!title)
      SendSysMessagePergon(who, "Es wurde noch kein Gildenkuerzel gesetzt.");
      Return;
    Else
      who.title_guild:=title;
      IncRevision(who);
      SendSysMessagePergon(who, "Gildenkuerzel ist nun SICHTBAR.");
    EndIf
  EndIf
EndFunction


//////////////////////////////
// - neue Mitglieder aufnehmen
//////////////////////////////

Function invite_mobile(byref who)
  Var trg, oldguild, oldguildname;
  If (!IsGuildLeader(who, guild))
    SendSysMessagePergon(who, "Ihr seid kein Anfuehrer und duerft niemanden aufnehmen!");
    Return;
  EndIf

  SendSysMessagePergon(who, "Wen wollt Ihr in die Gilde aufnehmen?");
  trg:=Target(who);

  If ((!trg) || (!trg.ip) || (trg.dead) || (trg.isa(POLCLASS_NPC)) || (!trg.isa(POLCLASS_MOBILE)) || (trg.serial == who.serial))
    // kein ziel, kein spieler bzw. nicht online oder ziel tot?
    SendSysMessagePergon(who,"Abbruch", "Abort");
    Return;
  EndIf
  If (guild.ismember(trg))
    // ist er schon mitglied DIESER gilde?
    SendSysMessagePergon(who, "Diese Person ist bereits Mitglied der Gilde - habt Ihr getrunken?!");
    Return;
  EndIf

  oldguild:=trg.guild;
  If (oldguild)  // ist er schon mitglied einer ANDEREN gilde?
    oldguildname:=oldguild.getprop("name");
    If (YesNoGumpGuild(trg,"Wollt Ihr aus Eurer jetzigen Gilde austreten?<br>'"+oldguildname+"'"))
      If (RemoveGuildMember(trg, oldguild))
        guildmsg(oldguild,trg.name+" hat die Gilde verlassen!",FONTCOLOR_REDBROWN);
        SendSysMessagePergon(trg,"Ihr seid aus der Gilde '"+oldguildname+"' ausgetreten.","",FONTCOLOR_REDBROWN);
        SendSysMessagePergon(who,trg.name+" ist aus der Gilde '"+oldguildname+"' ausgetreten.","",FONTCOLOR_REDBROWN);
      Else
        SendSysMessagePergon(trg,"Beim Austritt aus der Gilde '"+oldguildname+"' trat ein Fehler auf.");
        SendSysMessagePergon(who,"Bei "+trg.name+"s Austritt aus der Gilde '"+oldguildname+"' trat ein Fehler auf.");
        Return;
      EndIf
    Else
      SendSysMessagePergon(trg, "Ihr habt abgelehnt, aus der Gilde '"+oldguildname+"' auszutreten.");
      SendSysMessagePergon(who, trg.name+" hat abgelehnt, aus der Gilde '"+oldguildname+"' auszutreten.");
      Return;
    EndIf
  EndIf

  If (YesNoGumpGuild(trg,"Wollt Ihr in folgende Gilde aufgenommen werden?<br>'"+guild.getprop("name")+"'"))
    // will er überhaupt?
    If (guild.addmember(trg))
      guildmsg(guild,trg.name+" wurde in die Gilde aufgenommen!",FONTCOLOR_BROWN,trg,"Ihr wurdet in die Gilde '"+guild.getprop( "name" )+"' aufgenommen!");
      trg.title_guild:=guild.getprop("guild_tag");
      IncRevision(trg);
      UpdateMobile(trg);
    Else
      // fehler bei addmember
      SendSysMessagePergon(who, "Bei der Aufnahme trat ein Fehler auf!");
      Return;
    EndIf
  Else
    SendSysMessagePergon(who, trg.name+" hat abgelehnt in die Gilde aufgenommen zu werden." );
    SendSysMessagePergon(trg, "Ihr habt abgelehnt in die Gilde '"+guild.getprop( "name" )+"' aufgenommen zu werden." );
    Return;
  EndIf
EndFunction


///////////////////////////////////////////
// leave_guild - Gilde verlassen
///////////////////////////////////////////

Function leave_guild(byref who)
  If (YesNoGumpGuild(who,"Wollt Ihr die folgende Gilde verlassen?<br>'"+guild.getprop("name")+"'"))
    If (RemoveGuildMember(who, guild))
      guildmsg(guild,who.name+" hat gerade die Gilde verlassen!",FONTCOLOR_REDBROWN);
      SendSysMessagePergon(who, "Ihr seid aus der Gilde '"+guild.getprop("name")+"' ausgetreten!","",_DEFAULT_TEXT_FONT,FONTCOLOR_REDBROWN);
    EndIf
  Else
    SendSysMessagePergon(who, "Ihr koennt Euch auch nicht entscheiden, oder?");
  EndIf
EndFunction


///////////////////////////////////////////
// show_charta - Gildenprofil anzeigen
///////////////////////////////////////////

Function show_charta()
  def_sidebground(400);
  layoutback.append("htmlgump 500 75 260 20 "+data.size()+" 0 0");
  data.append("<basefont color=#00000F><center>Gildenprofil</center>");
  layoutback.append("HtmlGump 500 105 260 295 "+data.size()+" 0 1");
  If (guild.getprop("charta"))
    data.append("<basefont color=#00000F>"+guild.getprop("charta"));
  Else
    data.append("");
  EndIf
EndFunction


///////////////////////////////////////////
// edit_charta - Profil editieren
///////////////////////////////////////////

Function edit_charta()
  def_sidebground(150);
  layoutback.append("htmlgump 500 75 260 20 "+data.size()+" 0 0");
  data.append("<center>Gildenprofil editieren</center>");
  layoutback.append("button 600 100 9721 9724 1 0 17");
  layoutback.append("text 635 104 0 "+data.size());
  data.append("Neu");
EndFunction


///////////////////////////////////////////
// charta_new - Text neusetzen
///////////////////////////////////////////

Function charta_new(byref who)
  var cont:="", i,content;
  SendSysMessagePergon(who,"Waehlt das Buch aus in dem das neue Gildenprofil steht.");
  var book:=target(who);
  Var cfg:=ReadConfigFile(":sysbook:itemdesc");
  If (cfg[book.objtype])
    content:=book.getprop("contents");
    For (i:=1;i<=content.size();i+=1)
      cont+=content[i];
      SleepMS(2);
    EndFor
    guild.setprop("charta",cont);
    SendSysMessagepergon(who,"Gildenprofil erfolgreich geaendert.");
  Else
    SendSysMessagePergon(who,"Dies ist kein Buch.");
  EndIf
EndFunction


///////////////////////////////////////////
// set_symbol - Gildensymbol ändern
///////////////////////////////////////////

Function change_symbol()
  def_sidebground(140);
  layoutback.append("htmlgump 500 75 260 20 "+data.size()+" 0 0");
  data.append("<center>Gildensymbol aendern</center>");
  layoutback.append("button 550 130 9706 9707 1 0 16");  // zurück
  layoutback.append("button 700 130 9702 9702 1 0 15");  // vor
EndFunction


///////////////////////////////////////////
// change_symbol - Gildensymbol setzen
///////////////////////////////////////////

Function set_symbol(byref change);
  Var symbols:={};
  Var symbol:=guild.getprop("guildicon");
  If (!symbol)
    symbol:=5545; // darf jeder
  EndIf
  ForEach gilde in (ListGuilds())  // schon belegte Symbole speichern
    If (gilde.guildid<>guild.guildid)
      If ((gilde.getprop("guildicon")) && (gilde.getprop("guildicon")<>5545))
        symbols.append(gilde.getprop("guildicon"));
      EndIf
    EndIf
    SleepMS(2);
  EndForEach
  Repeat
    symbol+=change;
    If (symbol>5591)
      symbol:=5545;
    ElseIf (symbol<5545)
      symbol:=5591;
    EndIf
    SleepMS(2);
  Until (!(symbol in symbols));
  guild.setprop("guildicon",symbol);
EndFunction


///////////////////////////////////////////
// show_members - Liste der Mitglieder anzeigen
///////////////////////////////////////////

Function show_members()
  Var memsleader:={},mleader:="",memspriv:={},mpriv:="", leader;
  def_sidebground(400);
  layoutback.append("htmlgump 500 75 260 20 "+data.size()+" 0 0");
  data.append("<center>Mitgliedsliste</center>");
  layoutback.append("htmlgump 500 105 260 295 "+data.size()+" 0 1");
  // array mit allen serials der leader
  leader:=guild.getprop(PROP_GUILD_LEADERS);
  ForEach mem in (guild.members)
    If (mem.serial in leader)
      memsleader.append(mem.name);
    Else
      memspriv.append(mem.name);
    EndIf
    SleepMS(2);
  EndForEach
  memsleader:=SortArrayABC(memsleader,1,0);
  memspriv:=SortArrayABC(memspriv,1,0);
  ForEach mem in (memsleader)
    mleader:=mleader+"<basefont color=#1153>"+mem+"<br>";
    SleepMS(2);
  EndForEach
  ForEach mem in (memspriv)
    mpriv:=mpriv+"<basefont color=#48>"+mem+"<br>";
    SleepMS(2);
  EndForEach
  data.append(mleader+"<p>"+mpriv+"</p>");
EndFunction


///////////////////////////////////////////
// edit_members - Mitglieder ändern
// Liste alle Mitglieder mit Auswahl
///////////////////////////////////////////

Function edit_members()
  Var i, j:=1, leader;
  Var memsleader:={},memspriv:={}, dataleader:="", datapriv:="";
  Var mleader:={},mpriv:={};
  def_sidebground(400);
  layoutback.append("htmlgump 500 75 260 20 "+data.size()+" 0 0");
  data.append("<center>Mitgliedsliste</center>");
  layoutback.append("gumppic 505 403 2444");
  layoutback.append("textentry 520 405 30 20 0 51 "+data.size()); //Nr eintragen
  data.append("1");
  layoutback.append("button 570 400 9721 9724 1 0 19");  // Ok button
  layoutback.append("htmlgump 500 105 260 295 "+data.size()+" 0 1");

  // array mit allen serials der leader
  leader:=guild.getprop(PROP_GUILD_LEADERS);
  ForEach mem in (guild.members)
    If (mem.serial in leader)
      memsleader.append({mem.name,mem.serial});
    Else
      memspriv.append({mem.name,mem.serial});
    EndIf
    SleepMS(2);
  EndForEach
  mleader:=SortArrayABC(memsleader,1,1);
  mpriv:=SortArrayABC(memspriv,1,1);
  For (i:=1;i<=mleader.size();i+=1)
    dataleader+="<basefont color=#1153>"+j+" "+mleader[i][1]+"<br>";
    j+=1;
  EndFor
  For (i:=1;i<=mpriv.size();i+=1)
    datapriv+="<basefont color=#48>"+j+" "+mpriv[i][1]+"<br>";
    j+=1;
    SleepMS(2);
  EndFor
  data.append(dataleader+"<p>"+datapriv+"</p>");
  Return(mleader+mpriv);
EndFunction


///////////////////////////////////////////
// edit_member - Mitglied ändern
///////////////////////////////////////////

Function edit_member(byref who, byref mems, choice)
  Var leaders, mem, status;
  choice[1,4]:="";
  choice:=CInt(choice);

  If ((!choice) || (choice > guild.members.size()))
    SendSysMessagePergon(who, "Ungueltige Wahl");
    edit_members();  // Back to sender
    Return;
  EndIf

  leaders:=guild.getprop(PROP_GUILD_LEADERS);
  If (!leaders)
    leaders:={};
  EndIf
  mem:=SystemFindObjectBySerial(mems[choice][2],SYSFIND_SEARCH_OFFLINE_MOBILES);

  If (!mem)
    SendSysMessagePergon(who,"Es ist ein Fehler aufgetreten!");
    Return;
  EndIf
  If (IsGuildLeader(mem, guild))
    status:="Anfuehrer";
  Else
    status:="Mitglied";
  EndIf
  def_sidebground(400);
  layoutback.append("htmlgump 500 75 260 20 "+data.size()+" 0 0"); // Name
  data.append("<center>"+mem.name+"</center>");
  layoutback.append("htmlgump 500 90 260 20 "+data.size()+" 0 0"); // Status
  data.append("<center>"+status+"</center>");

  layoutback.append("gumppic 490 110 213");
  layoutback.append("gumppictiled 500 110 255 10 214");
  layoutback.append("gumppic 755 110 215");

  layoutback.append("text 500 125 0 "+data.size());  // Status ändern (Anführer/normal)
  data.append("Mitgliedstatus aendern:");
  layoutback.append("text 535 154 0 "+data.size());
  data.append("Anfuehrer");
  If (status=="Anfuehrer")
    layoutback.append("button 500 150 9724 9721 1 0 20");
  Else
    layoutback.append("button 500 150 9721 9724 1 0 20");
  EndIf
  layoutback.append("text 535 184 0 "+data.size());
  data.append("Mitglied");
  If (status=="Mitglied")
    layoutback.append("button 500 180 9724 9721 1 0 21");
  Else
    layoutback.append("button 500 180 9721 9724 1 0 21");
  EndIf


  layoutback.append("gumppic 500 230 1141");         // Titel vergeben
  layoutback.append("htmlgump 500 209 260 20 "+data.size()+" 0 0");
  data.append("<center>Titel vergeben</center>");
  layoutback.append("textentry 515 232 240 20 0 52 "+data.size());
  If (GetObjProperty(mem,"guildtitle"))
    data.append(GetObjProperty(mem,"guildtitle"));
  Else
    data.append("");
  EndIf

  layoutback.append("gumppic 500 280 1142");         // Gildenbezeichnung vergeben (title_race)
  layoutback.append("htmlgump 500 260 260 20 "+data.size()+" 0 0");
  data.append("<center>Gildenbezeichnung vergeben</center>");
  layoutback.append("textentry 515 282 240 20 0 53 "+data.size());
  If (mem.title_race)
    data.append(mem.title_race);
  Else
    data.append("");
  EndIf

  layoutback.append("gumppic 500 330 1142");         // Praefix vergeben
  layoutback.append("htmlgump 500 311 260 20 "+data.size()+" 0 0");
  data.append("<center>Prefix vergeben</center>");
  layoutback.append("textentry 515 332 240 20 0 54 "+data.size());
  If (mem.title_prefix)
    data.append(mem.title_prefix);
  Else
    data.append("");
  EndIf

  layoutback.append("text 535 370 0 "+data.size());  // entfernen
  data.append("Mitglied entfernen");
  layoutback.append("button 500 366 9721 9724 1 0 23");

  layoutback.append("button 590 400 9721 9724 1 0 24");  // Speichern
  layoutback.append("text 625 404 0 "+data.size());
  data.append("Speichern");
  Return(mem); // Zurückschicken damit die SaveFunktion weiß wer
EndFunction


///////////////////////////////////////////
// edit_membersave - Mitglied speichern
///////////////////////////////////////////

Function edit_membersave(byref who, byref mem, byref params)
  Var leaders, i, whotitle, whotitlealt, whorace, whoracealt, whoprefix, whoprefixalt, memname;

  Case (params)
    "Anfuehrer":
        leaders:=guild.getprop(PROP_GUILD_LEADERS);
        If (leaders.size > MAX_GUILD_LEADERS)
          SendSysMessagePergon(who, "Es existieren bereits 5 Anfuehrer.");
        ElseIf (IsGuildLeader(mem, guild))
          SendSysMessagePergon(who, mem.name+" ist bereits ein Anfuehrer.");
        Else
          If (YesNoGumpGuild(who,"Wollt Ihr wirklich "+mem.name+" zu einem Anfuehrer befoerdern?"))
            leaders.append(mem.serial);
            guild.setprop(PROP_GUILD_LEADERS, leaders);
            guildmsg(guild,mem.name+" wurde zu einem Anfuehrer ernannt.",FONTCOLOR_GREEN,mem,"Ihr wurdet zu einem Anfuehrer ernannt.");
          EndIf
        EndIf
    "Mitglied":
        If (!IsGuildLeader(mem, guild))
          SendSysMessagePergon(who,"Eine verwirrende Wahl.");
        Else
          If (YesNoGumpGuild(who,"Wollt Ihr wirklich "+mem.name+" degradieren?"))
            leaders:=guild.getprop(PROP_GUILD_LEADERS);
            For (i:=1; i<=leaders.size(); i+=1)
              If (leaders[i]==mem.serial)
                leaders.erase(i);
                guild.setprop(PROP_GUILD_LEADERS, leaders);
                Break;
              EndIf
              SleepMS(2);
            EndFor
            guildmsg(guild,mem.name+" ist nun kein Anfuehrer mehr.",FONTCOLOR_RED,mem,"Ihr seit nun kein Anfuehrer mehr.");
          EndIf
        EndIf
    "Mitglied entfernen":
        If (YesNoGumpGuild(who,"Wollt Ihr "+mem.name+" entfernen?"))
          If (RemoveGuildMember(mem, guild, who))
            guildmsg(guild,mem.name+" wurde aus der Gilde entfernt.",FONTCOLOR_RED);
            SendSysMessagePergon(mem, "Ihr wurdet aus der Gilde "+guild.getprop("name")+" entfernt.","",_DEFAULT_TEXT_FONT,FONTCOLOR_RED);
          EndIf
        EndIf
    default:  // Gildentitel, Prefix, Gildenbezeichnung setzen
        whotitle:=params[1];
        whotitle[1,4]:="";
        whorace:=params[2];
        whorace[1,4]:="";
        whoprefix:=params[3];
        whoprefix[1,4]:="";
        whotitlealt:=GetObjProperty(mem,"guildtitle");
        whoracealt:=mem.title_race;
        whoprefixalt:=mem.title_prefix;
        If (len(whotitle)<=20)
          If (whotitlealt<>whotitle)
            If (whotitlealt)
              memname:=mem.name;
              If (whotitlealt=="")
                memname:=" "+memname;
              Else
                memname[whotitlealt]:="";
              EndIf
              If (whotitle=="")
                memname[1,1]:="";
              EndIf
              mem.name:=whotitle+memname;
              SetObjProperty(mem,"guildtitle",whotitle);
            Else
              mem.name:=whotitle+" "+mem.name;
              SetObjProperty(mem,"guildtitle",whotitle);
            EndIf
            SendSysMessagePergon(who,"Titel geaendert zu: "+whotitle);
          EndIf
        Else
          SendSysMessagePergon(who,"Titelbezeichnung ist zu lang.");
        EndIf
        If (len(whorace)<=20)
          If (whorace<>whoracealt)
            mem.title_race:=whorace;
            SendSysMessagePergon(who,"Gildenbezeichnung geaendert zu: "+whorace);
          EndIf
        Else
          SendSysMessagePergon(who,"Gildenbezeichung ist zu lang.");
        EndIf
        If (len(whoprefix)<=20)
          If (whoprefix<>whoprefixalt)
            mem.title_prefix:=whoprefix;
            SendSysMessagePergon(who,"Prefix geaendert zu: "+whoprefix);
          EndIf
        Else
          SendSysMessagePergon(who,"Prefix ist zu lang.");
        EndIf
        IncRevision(mem);
  EndCase
EndFunction


///////////////////////////////////////////
// friendly_guildls - Liste der Allierten
///////////////////////////////////////////

Function friendly_guilds()
  var dataally:="", friends:={};
  def_sidebground(400);
  layoutback.append("htmlgump 500 75 260 20 "+data.size()+" 0 0");
  data.append("<center>Freundlich gesinnte Gilden</center>");
  layoutback.append("htmlgump 500 105 260 295 "+data.size()+" 0 1");

  ForEach friend in (guild.allyguilds)
    friends.append({friend.getprop("name"),friend.getprop("guild_tag")});
    SleepMS(2);
  EndForEach
  friends:=SortArrayABC(friends,1,1);
  ForEach friend in (friends)
    dataally+="<basefont color=#1153>["+friend[2]+"] "+friend[1]+"<br>";
    SleepMS(2);
  EndForEach
  data.append(dataally);
EndFunction


///////////////////////////////////////////
// enemy_guilds - Liste der Feinde
///////////////////////////////////////////

Function enemy_guilds()
  var dataenemy:="", enemys:={};
  def_sidebground(400);
  layoutback.append("htmlgump 500 75 260 20 "+data.size()+" 0 0");
  data.append("<center>Feindlich gesinnte Gilden</center>");
  layoutback.append("htmlgump 500 105 260 295 "+data.size()+" 0 1");

  ForEach enemy in (guild.enemyguilds)
    enemys.append({enemy.getprop("name"),enemy.getprop("guild_tag")});
    SleepMS(2);
  EndForEach
  enemys:=SortArrayABC(enemys,1,1);
  ForEach enemy in (enemys)
    dataenemy+="<basefont color=#1153>["+enemy[2]+"] "+enemy[1]+"<br>";
    SleepMS(2);
  EndForEach
  data.append(dataenemy);
EndFunction


///////////////////////////////////////////
// diplomacy - Liste aller Gilden für Diplomatie
///////////////////////////////////////////

Function diplomacy()
  Var enemyguilds:={}, allyguilds:={}, neutralguilds:={};
  Var j:=1, i, guildtag;
  Var dataenemy:="", dataally:="", dataneutral:="";

  def_sidebground(400);
  layoutback.append("htmlgump 500 75 260 20 "+data.size()+" 0 0");
  data.append("<center>Diplomatie</center>");
  layoutback.append("gumppic 505 403 2444");
  layoutback.append("textentry 520 405 30 20 0 55 "+data.size()); //Nr eintragen
  data.append("1");
  layoutback.append("button 570 400 9721 9724 1 0 13");  // Ok button
  layoutback.append("htmlgump 500 105 260 295 "+data.size()+" 0 1");

  ForEach gilde in (ListGuilds())  // Gilden sammeln
    If (gilde.members.size()>=1)  // gültige Gilden
      If (gilde.guildid<>guild.guildid) // nicht die eigene
        If (gilde.guildid<>GoetterGildeID) // Göttergilde rausgenommen
          If (guild.isallyguild(gilde))
            If (gilde.getprop("guild_tag"))
              guildtag:=gilde.getprop("guild_tag");
            Else
              guildtag:="";
            EndIf
            allyguilds.append({gilde.getprop("name"),guildtag,gilde.guildid});
          ElseIf (guild.isenemyguild(gilde))
            If (gilde.getprop("guild_tag"))
              guildtag:=gilde.getprop("guild_tag");
            Else
              guildtag:="";
            EndIf
            enemyguilds.append({gilde.getprop("name"),guildtag,gilde.guildid});
          Else
            If (gilde.getprop("guild_tag"))
              guildtag:=gilde.getprop("guild_tag");
            Else
              guildtag:="";
            EndIf
            neutralguilds.append({gilde.getprop("name"),guildtag,gilde.guildid});
          EndIf
        EndIf
      EndIf
      SleepMS(2);
    EndIf
  EndForEach
  allyguilds:=SortArrayABC(allyguilds,1,1);
  enemyguilds:=SortArrayABC(enemyguilds,1,1);
  neutralguilds:=SortArrayABC(neutralguilds,1,1);
  For (i:=1;i<=allyguilds.size();i+=1)
    dataally+="<basefont color=#00CC33>"+j+" "+allyguilds[i][1]+" ["+allyguilds[i][2]+"]<br>";
    j+=1;
  EndFor
  For (i:=1;i<=enemyguilds.size();i+=1)
    dataenemy+="<basefont color=#800000>"+j+" "+enemyguilds[i][1]+" ["+enemyguilds[i][2]+"]<br>";
    j+=1;
  EndFor
  For (i:=1;i<=neutralguilds.size();i+=1)
    dataneutral+="<basefont color=#C0C0C0>"+j+" "+neutralguilds[i][1]+" ["+neutralguilds[i][2]+"]<br>";
    j+=1;
  EndFor
  data.append(dataally+"<p>"+dataenemy+"<p>"+dataneutral);
  Return(allyguilds+enemyguilds+neutralguilds);
EndFunction


///////////////////////////////////////////
// edit_diplomacy - Diplomatieoptionen anzeigen
///////////////////////////////////////////

Function edit_diplomacy(byref who, byref gilden, choice)
  Var gilde, status, angebot:="", angebotfremd:="", tag;

  choice[1,4]:="";
  choice:=CInt(choice);
  If ((!choice) || (choice > gilden.size()))
    SendSysMessagePergon(who, "Ungueltige Wahl");
    diplomacy();  // Back to sender
    Return;
  EndIf

  gilde:=FindGuild(gilden[choice][3]);
  If (!gilde)
    SendSysMessagePergon(who,"Es ist ein Fehler aufgetreten!");
    Return;
  EndIf

  If (guild.isallyguild(gilde))
    status:="<basefont color=#00CC33>Alliert";
  ElseIf (guild.isenemyguild(gilde))
    status:="<basefont color=#800000>Verfeindet";
  Else
    status:="<basefont color=#C0C0C0>Neutral";
  EndIf
  If (gilde.guildid in guild.getprop("wannabe_allies"))
    angebotfremd:="<basefont color=#00CC33>Alliert";
  ElseIf (gilde.guildid in guild.getprop("wannabe_enemies"))
    angebotfremd:="<basefont color=#800000>Verfeindet";
  EndIf
  If (guild.guildid in gilde.getprop("wannabe_allies"))
    angebot:="<basefont color=#00CC33>Alliert";
  ElseIf (guild.guildid in gilde.getprop("wannabe_enemies"))
    angebot:="<basefont color=#800000>Verfeindet";
  EndIf
  tag:=gilde.getprop("guild_tag");
  If (!tag)
    tag:="";
  EndIf
  def_sidebground(400);
  layoutback.append("htmlgump 500 75 260 20 "+data.size()+" 0 0"); // Name
  data.append("<center>"+gilde.getprop("name")+"</center>");
  layoutback.append("htmlgump 500 90 260 20 "+data.size()+" 0 0"); // Status
  data.append("<center>["+tag+"]</center>");
  layoutback.append("htmlgump 500 110 260 20 "+data.size()+" 0 0"); // Status
  data.append("<center>"+status+"</center>");
  layoutback.append("htmlgump 500 130 260 20 "+data.size()+" 0 0"); //Angebote?
  data.append("<center>Eigenes Angebot: "+angebot+"</center>");
  layoutback.append("htmlgump 500 150 260 20 "+data.size()+" 0 0"); //Angebote?
  data.append("<center>Ihr Angebot: "+angebotfremd+"</center>");

  layoutback.append("gumppic 490 170 213");
  layoutback.append("gumppictiled 500 170 255 10 214");
  layoutback.append("gumppic 755 170 215");

  layoutback.append("text 535 198 0 "+data.size());
  data.append("Verbuenden");
  layoutback.append("button 500 194 9721 9724 1 0 25");
  layoutback.append("text 535 238 0 "+data.size());
  data.append("Neutral");
  layoutback.append("button 500 234 9721 9724 1 0 26");
  layoutback.append("text 535 278 0 "+data.size());
  data.append("Krieg erklaeren");
  layoutback.append("button 500 274 9721 9724 1 0 27");

  Return(gilde);  // Speichern für spätere Verwendung
EndFunction


///////////////////////////////////////////
// save_diplo - Abspeichern
///////////////////////////////////////////

Function save_diplo(byref who, byref gilde, param);
  Var wannabe:={}, addally, i, addenemy;
  Case (param)
    "Verbuenden":
        If (guild.isallyguild(gilde))
          SendSysMessagePergon(who,"Ihr fuehlt Euch wohl nicht so gut. Ihr seit bereits Verbuendet.");
          Return;
        EndIf
        wannabe:=guild.getprop("wannabe_allies");
        If (!wannabe)
          wannabe:={};
        EndIf
        If (gilde.guildid in wannabe)  // andere Gilde hat schon angeboten
          If (YesNoGumpGuild(who,"Wollt Ihr ein Buendnis mit '"+gilde.getprop("name")+"' eingehen?"))
            guild.removeenemyguild(gilde);
            addally:=guild.addallyguild(gilde);
            If (!addally)
              SendSysMessagePergon(who,"Es ist ein Fehler aufgetreten! "+addally);
              Return;
            EndIf
            guildmsg(guild,"Es wurde ein Buendniss mit '"+gilde.getprop("name")+"' eingegangen.",FONTCOLOR_BROWN);
            guildmsg(gilde,"Es wurde ein Buendniss mit '"+guild.getprop("name")+"' eingegangen.",FONTCOLOR_BROWN);
            For (i:=1;i<=wannabe.size();i+=1)
              If (wannabe[i]==gilde.guildid)
                wannabe.erase(i);
                guild.setprop("wannabe_allies",wannabe);
                Break;
              EndIf
            EndFor
          Else
            SendSysMessagePergon(who,"Wie Ihr wollt.");
          EndIf
        ElseIf (guild.guildid in gilde.getprop("wannabe_allies"))  // Schonmal gemacht?
          SendSysMessagePergon(who,"Ihr habt dieser Gilde bereits ein Angebot gemacht");
        Else  // Bündnis anbieten
          wannabe:=gilde.getprop("wannabe_allies");
          If (!wannabe)
            wannabe:={};
          EndIf
          wannabe.append(guild.guildid);
          gilde.setprop("wannabe_allies",wannabe);
          gilde.getprop("wannabe_allies");
          guildmsg(guild,"Ihr bietet der Gilde '"+gilde.getprop("name")+"' ein Buendnis an.",FONTCOLOR_BROWN);
          guildmsg(gilde,guild.getprop("name")+" bietet Euch ein Buendnis an.",FONTCOLOR_BROWN);
        EndIf


    "Neutral":
        // Allieangebote loswerden
        wannabe:=guild.getprop("wannabe_allies");
        If (!wannabe)
          wannabe:={};
        EndIf
        If (gilde.guildid in wannabe)
          For (i:=1;i<=wannabe.size();i+=1)
            If (wannabe[i]==gilde.guildid)
              wannabe.erase(i);
              guild.setprop("wannabe_allies",wannabe);
              guildmsg(guild,"Es wurde das Buendnissangebot von '"+gilde.getprop("name")+"' abgelehnt.",FONTCOLOR_BROWN);
              guildmsg(gilde,"Es wurde das Buendnisangebot mit '"+guild.getprop("name")+"' abgelehnt.",FONTCOLOR_BROWN);
              Break;
            EndIf
          EndFor
        EndIf
        wannabe:=gilde.getprop("wannabe_allies");
        If (!wannabe)
          wannabe:={};
        EndIf
        If (guild.guildid in wannabe)
          For (i:=1;i<=wannabe.size();i+=1)
            If (wannabe[i]==guild.guildid)
              wannabe.erase(i);
              gilde.setprop("wannabe_allies",wannabe);
              guildmsg(guild,"Es wurde das Buendnissangebot mit '"+gilde.getprop("name")+"' zurueckgezogen.",FONTCOLOR_BROWN);
              guildmsg(gilde,"Es wurde das Buendnisangebot von '"+guild.getprop("name")+"' zurueckgezogen.",FONTCOLOR_BROWN);
              Break;
            EndIf
          EndFor
        EndIf
        // Kriegsangebote loswerden
        wannabe:=guild.getprop("wannabe_enemies");
        If (!wannabe)
          wannabe:={};
        EndIf
        If (gilde.guildid in wannabe)
          For (i:=1;i<=wannabe.size();i+=1)
            If (wannabe[i]==gilde.guildid)
              wannabe.erase(i);
              guild.setprop("wannabe_enemies",wannabe);
              guildmsg(guild,"Es wurde das Kriegsangebot von '"+gilde.getprop("name")+"' abgelehnt.",FONTCOLOR_BROWN);
              guildmsg(gilde,"Es wurde das Kriegsangebot mit '"+guild.getprop("name")+"' abgelehnt.",FONTCOLOR_BROWN);
              Break;
            EndIf
          EndFor
        EndIf
        wannabe:=gilde.getprop("wannabe_enemies");
        If (!wannabe)
          wannabe:={};
        EndIf
        If (guild.guildid in wannabe)
          For (i:=1;i<=wannabe.size();i+=1)
            If (wannabe[i]==guild.guildid)
              wannabe.erase(i);
              gilde.setprop("wannabe_enemies",wannabe);
              guildmsg(guild,"Es wurde das Kriegsangebot mit '"+gilde.getprop("name")+"' zurueckgezogen.",FONTCOLOR_BROWN);
              guildmsg(gilde,"Es wurde das Kriegsangebot von '"+guild.getprop("name")+"' zurueckgezogen.",FONTCOLOR_BROWN);
              Break;
            EndIf
          EndFor
        EndIf
        If (guild.isallyguild(gilde))
          If (YesNoGumpGuild(who,"Wollt Ihr das Buendnis mit '"+gilde.getprop("name")+"' aufheben?"))
            guild.removeallyguild(gilde);
            guildmsg(guild,"Das Buendnis mit '"+gilde.getprop("name")+"' wurde beendet.",FONTCOLOR_BROWN);
            guildmsg(gilde,"Das Buendnis mit '"+guild.getprop("name")+"' wurde beendet.",FONTCOLOR_BROWN);
          Else
            SendSysMessagePergon(who,"Wie Ihr wollt.");
          EndIf
        ElseIf (guild.isenemyguild(gilde))
          If (YesNoGumpGuild(who,"Wollt Ihr den Krieg mit '"+gilde.getprop("name")+"' beenden?"))
            guild.removeenemyguild(gilde);
            guildmsg(guild,"Der Krieg mit '"+gilde.getprop("name")+"' wurde beendet.",FONTCOLOR_BROWN);
            guildmsg(gilde,"Der Krieg mit '"+guild.getprop("name")+"' wurde beendet.",FONTCOLOR_BROWN);
          Else
            SendSysMessagePergon(who,"Wie Ihr wollt.");
          EndIf
        EndIf


    "Krieg":
        If (guild.isenemyguild(gilde))
          SendSysMessagePergon(who,"Ihr fuehlt Euch wohl nicht so gut. Ihr seid bereits im Krieg.");
          Return;
        EndIf

        wannabe:=guild.getprop("wannabe_enemies");
        If (!wannabe)
          wannabe:={};
        EndIf
        If (gilde.guildid in wannabe)  // andere Gilde hat schon angeboten
          If (YesNoGumpGuild(who,"Wollt Ihr den Krieg mit '"+gilde.getprop("name")+"' beginnen?"))
            guild.removeallyguild(gilde);
            addenemy:=guild.addenemyguild(gilde);
            If (!addenemy)
              SendSysMessagePergon(who,"Es ist ein Fehler aufgetreten! "+addenemy);
              Return;
            EndIf
            guildmsg(guild,"Es herrscht Krieg mit '"+gilde.getprop("name")+"'!",FONTCOLOR_BROWN);
            guildmsg(gilde,"Es herrscht Krieg mit '"+guild.getprop("name")+"'!",FONTCOLOR_BROWN);
            For (i:=1;i<=wannabe.size();i+=1)
              If (wannabe[i]==gilde.guildid)
                wannabe.erase(i);
                guild.setprop("wannabe_enemies",wannabe);
                Break;
              EndIf
            EndFor
          Else
            SendSysMessagePergon(who,"Wie Ihr wollt.");
          EndIf
        ElseIf (guild.guildid in gilde.getprop("wannabe_enemies"))  // Schonmal gemacht?
          SendSysMessagePergon(who,"Ihr habt dieser Gilde bereits ein Angebot gemacht");
        Else  // Krieg "anbieten"
          wannabe:=gilde.getprop("wannabe_enemies");
          If (!wannabe)
            wannabe:={};
          EndIf
          wannabe.append(guild.guildid);
          gilde.setprop("wannabe_enemies",wannabe);
          guildmsg(guild,"Ihr bietet der Gilde '"+gilde.getprop("name")+"' einen Krieg an.",FONTCOLOR_BROWN);
          guildmsg(gilde,guild.getprop("name")+" bietet Euch einen Krieg an.",FONTCOLOR_BROWN);
        EndIf
  EndCase
EndFunction
