///////////////////////////////////////////////////////////////////////////
// server.inc - Konstanten, die serverseitig benutzt werden

// Workarounds
// Konstanten für .setfacing (frozen ignorieren oder nicht)
Const FACING_NORMAL := 0x0;
Const FACING_FORCE  := 0x1;
// Workarounds Ende

// Altersgrenzen von Accounts/Charactern {{{
// Wo ist die Grenze zum antiken Character/Account?
Const ANCIENT_TIME    := 3600*24*360; // (12 Monate)
// Wo ist die Grenze zwischen 'aktivem' und 'inaktivem' Character/Account?
Const ACTIVE_TIME     := 3600*24*60; // (2 Monate)
// Wie lange gilt ein Account als neu?
Const ACCT_NEW_TIME   := 3600*24*31; // 1 Monat
// Wann wird ein Name automatisch erlaubt
Const AUTO_CHECK_TIME := 3600*24*14; // 14 Tage
// }}}

// Properties an Accounts {{{
// eMail-Adresse zur Kommunikation (String)
Const PLAYEREMAIL       := "eMail";

// Charanlegung
// naechsten Char mit diesem Cmdlevel anlegen
Const ACCT_NEWCHAR_CMDLVL := "CharKennCMDLevel";
// Charname beim Anlegen
Const ACCT_NEWCHAR_NAME := "NewCharName";
// Chars im Account sind geblockt (alte Anlegesperre)
Const ACCT_NEWCHAR_BLOCK  := "NoCharKenn";

// Zeiten (ReadGameClock())
// wann wurde Account zuletzt benutzt?
Const ACCT_LAST_USE    := "lastused";
// Wann wurde der Account erzeugt?
Const ACCT_CREATED     := "created";

// Zaehler fuer fehlgeschlagene Loginversuche
Const LOG_LOGINFAIL     := "FailedLogins";

// sollen Tips angezeigt werden?
Const ACCT_TIP_DISABLED := "TipNo";

// welcher Tip wurde zuletzt angezeigt
Const ACCT_TIP_LAST     := "TipLast";

// Web-Passwort (Passwort extra fuers Webinterface)
Const ACCT_WEBPASS      := "WebPass";
// }}}

// Properties an Spielern {{{
// welche motd wurde zuletzt fuer den Spieler erzeugt
Const PROP_MOTD        := "MsgOfDayID";
// wurde der angeforderte Name schon abgesegnet?
Const PROP_NAME_CHECK     := "name_check";

// Sonderrechte (über .iddqd umschaltbar) (Boolean)
Const I_AM_GOD := "IamGod";
// Enttarnte Staffler in Online-Listen zeigen? (Boolean)
Const PROP_HIDE_STAFF_ONLINE := "#Unconcealed";
// Liste zuletzt besuchter Positionen
Const PROP_LOC_LIST := "goback";

// keine Waffenregelhinweise anzeigen
Const PROP_NO_WEAPON_RULE_NOTICE := "WaffenRegelnMute";

// Erfahrungspunkte
Const EXPERIENCE            := "Experience";
Const EXPERIENCE_RANK       := "ExperienceRank";
Const EXPERIENCE_NO_DISPLAY := "NoExpRank";

// Tooltip des Characters (Gilde, Titel, Cmdlevel) fuer andere verbergen
Const HIDE_CHAR_TOOLTIP := "notooltip";

// Im Ver-/Kaufsmenue keine AoS-Tooltips benutzen (Boolean)
Const NO_AOS_TOOLTIP := "No_AoS_Tooltip";

// Buergermeister wird in .online nicht hervorheben (Boolean)
Const HIDE_BM_ONLINE := "Hide_BM_Online";

// Spieler ist kein Newbie, befindet sich aber in der Region (Boolean)
Const PLACE_INNEWBIEAREA := "InNewbieArea";
// Spieler befindet sich auf der Goetterinsel (Boolean)
Const PLACE_ATGODPALACE  := "AtGodPalace";
// Spieler befindet sich in einem Kampf (Array)
Const ARENABATTLE        := "ArenaBattle";
// war der Spieler vor dem Kampf kriminell?
Const PROP_ARENACRIME := "#ArenaCrimeBefore";
// wurde den Goettern gespendet (Zugang Goetterinsel)?
Const PROP_GODDONATION := "spende_ok";

// Spieler ist neu auf Pergon, auf Jhe'lom (Boolean)
Const TYPNEWBIE := "jhelomnewbie";
// Spieler darf die Fluchtfähre benutzen
Const TYPFLIGHT := "fluchterlaubt";
// Spieler ist PK
Const TYPKILLER := "permared";

// Spieler ist ein Bibliothekar (Boolean)
Const TYPBIBLIOTHEKAR := "Bibliothekar";
// Spieler ist ein QuestCharacter (Boolean)
Const TYPQUESTCHAR    := "QuestChar";
// Spieler ist ein Kassenwart/Kaemmerer etc. (Boolean)
Const TYPKASSENWART   := "Kassenwart";

// Spieler will kein PvP
Const PROP_WANTS_PVP := "pvpwanted";
// Spieler bevorzugt RP
Const PROP_WANTS_RP  := "rpwanted";
// Character will nicht angegriffen werden
Const PROP_NOATTACK  := "Peacemaker";
// Characters moechte magisches Licht benutzen
Const PROP_MAGILITE  := "magilite";
// Character will automatischen Warmode
Const PROP_AUTOWAR   := "autowar";
// Spieler will Beruf in der onlineübersicht
Const PROP_SHOW_PROF  := "showprof_online";
// Spieler will in der LiveMap auftauchen
Const PROP_LIVEMAP := "LiveMapVisible";

// Soundeffekte bei .msg, .letterbox und .page
Const PROP_NOTIFY_MESSAGE := "notify_msg";
Const PROP_NOTIFY_LETTER  := "notify_letter";
Const PROP_NOTIFY_PAGE    := "notify_page";

// Hungerstatus
Const PROP_HUNGER     := "hungrylevel";

// Wer hat zuletzt auf das Opfer eingeschlagen? (Array)
Const LOG_LASTHIT    := "LastHit";
// Wer hat zuletzt das Opfer verletzt? (Array)
Const LOG_LASTDAMAGE := "LastDamage";
// Wer hat wieviel Schaden gemacht? (Dictionary)
Const LOG_DAMAGER    := "#TotalDamager";
// Todeszaehler
Const LOG_DEATHCOUNT  := "DeathCounter";
Const LOG_DEATHCOUNTM := "DeathCounterMonth";

// Strafsystem
// abzusitzende Knastzeit/Prangerzeit/Hoellenzeit
Const PROP_PUNISH_JAILTIME := "jailtime";
// Art der Strafe (Knast, Steinbruch, Pranger, Tod)
Const PROP_PUNISH_TYPE     := "PunishType";
// Position vor Anprangern/Steinbruch fuer Entlassung
Const PROP_PUNISH_OLDPOS   := "PunishPositionBefore";
// Anzahl der zu klopfenden Steine im Steinbruch
Const PROP_PUNISH_STONES   := "PunishStones";

// Zeit bis wieder Skillnutzung moeglich ist (Prop > ReadGameClock())
Const PROP_NEXTSKILLUSE := "#UsesSkill";

// Followmode aktiv
Const PROP_FOLLOWMODE := "#follows";
// .controlnpc
Const PROP_CONTROLNPC := "remotecontrol";
// benutzt magischen Kompass?
Const PROP_COMPASS    := "#magiccompass";

// letzter Einsatz am einarmigen Banditen
Const PROP_BANDIT_AMOUNT := "#Bandit";

// }}}

// Properties an NPCs {{{
// es wurde beschworen
// (Werte siehe pkg/magic/newmagic/spells/magicpergon.inc)
Const PROP_SUMMONED := "summoned";

// Tier bleibt ewig treu und kann nicht uebertragen werden
// ab Wert 99 verhungert es nicht mehr (scripts/include/animal.inc)
// FIXME: mehrere Stufen, weiter Boni
Const PROP_COMPANION := "companion";

// Farbwechsel bei zahmen Tieren fuer Willkommensgeschenk (Zeit in Sekunden)
Const PROP_CHANGE_COLOR := "colorchange";
// }}}

// Properties an Spielern und Items {{{
// Datenlager fuer Zweitcharsupport
Const SELF_SUPPORT1 := "SupportCheck1";
Const SELF_SUPPORT2 := "SupportCheck2";
// }}}

// Properties an Items {{{
// Cprop Struct mit Material/Hersteller für Tooltip
Const TOOLINFO := "ToolInfo";

// welcher Nachrichteneintrag gehoert zum motd-Item?
Const PROP_MOTD_ID := "MsgID";

// Beweiskram
// Wer hat es vergiftet (Name/Serial)
Const EV_POISONER   := "p";

// Wer hat es verkauft (Name/Serial)
Const EV_VENDOR     := "v";

// Wer hat es zerschnippelt (Name/Serial)
Const EV_CHOPPER    := "c";
// }}}

// Globale Property-Konstanten {{{
// Pergon oder BauServer?
Const PERGON    := "Pergon";
Const BAUSERVER := "BauServer";
// Anzeigename (z. B. fuer Webinterface)
Const GPROP_SERVERNAME := "ServerName";

// Anzeigename (z. B. fuer Webinterface)
Const GPROP_DEBUG_ACCOUNT := "DebugAccount";

// Skillcheck für Fähre/Fluchttor deaktivieren?
Const PROP_DISABLE_SKILLCHECK := "newbiegatenoskillcheck";

// aktiver Shutdownvorgang?
Const PROP_SHUTDOWN_RUNNING      := "#ShutDown";

// Waffenregeln aktiv (Wert 111)
Const PROP_WEAPON_RULES_ACTIVE   := "WaffenRegelnAktiviert";

// Pruefung, ob Leute gerade passiv kaempfen (fuer IsFighting)
Const PROP_DEFENSE_IS_FIGHT := "SelfDefenseIsFight";

// Liste der Pages
Const GM_PAGE_LIST     := "gmpages";
// Flag, ob Pages gerade bearbeitet werden
Const GM_PAGE_ACTIVITY := "#gmpages";

// Account- und Charverwaltung
// Zaehlt die Server-Starts
// (nett fuer Statistik, aber auch als langsame logische Uhr)
Const SERVER_STARTUPS  := "StartUps";
// Liste neuer Accounts fuer Account-/Namensteam
// struct{accounts : array of account names, lastupdate : int}
//Const NEW_ACCOUNTS     := "#NewAccounts";

// unbegrenztes Characteranlegen aktivieren (fuer Testserver)
Const CREATE_ALLOW_FREE := "CharCreateFree";
// erlaubt, zwei Character selbst anzulegen (neues Konzept)
Const CREATE_ALLOW_SELF := "CharCreateSelf";
// }}}

// Commandlevel {{{
// Aenderungen an CmdLevel beziehen (mindestens) ein: cmds.cfg,
// setcmdlevel.src, onlinesearch.inc
Const CMDLEVEL_PLAYER    := 0;
Const CMDLEVEL_COUNSELOR := 1;
Const CMDLEVEL_QUESTCHAR := 2;
Const CMDLEVEL_SEER      := 3;
Const CMDLEVEL_GM        := 4;
Const CMDLEVEL_HIGHGM    := 5;
Const CMDLEVEL_GL        := 6;
Const CMDLEVEL_SCRIPTER  := 7;
Const CMDLEVEL_DEVELOPER := 8;
// }}}

// NPC-Alignment {{{
Const ALIGNMENT_GOOD    := 2;
Const ALIGNMENT_NEUTRAL := 0;
Const ALIGNMENT_EVIL    := 1;
// }}}

// Housing {{{
// gemeinsamen Konstanten fuer statische und Multihaeuser
Const PROP_HS_C_ACCOUNT        := "owneracct";
Const PROP_HS_C_OWNER          := "ownerserial";
Const PROP_HS_C_OWNERNAME      := "lastownername";
Const PROP_HS_C_FRIENDS        := "friends";
// Festgemachtes
Const PROP_HS_C_FIXED          := "lockeddown";
Const PROP_HS_C_FIX_NUM        := "numlockdowns";
// SC = secure container
Const PROP_HS_C_SC_NUM         := "numsecure";
Const PROP_HS_C_SC_BOX         := "boxserial";
Const PROP_HS_C_SC_HOUSE       := "houseserial";
// Props fuer Mietanzeige
Const PROP_HS_C_RENT_TOPAY     := "rent_topay_count";
Const PROP_HS_C_RENT_TOOLTIP   := "rent_tooltip";

// Props fuer statische Haeuser
Const PROP_HS_S_CTRL_PID       := "#PID";
Const PROP_HS_S_DIMENSIONS     := "houseinfo";

// Props fuer Multihaeuser
// Verweis auf Haus an Multikomponente
Const PROP_HS_M_HOUSE          := "house_serial";

// Globalprop, wenn gesetzt, wird Containerinventur ausgefuehrt
Const GPROP_HS_C_CONTINV       := "container_inv";
// }}}

// Musik {{{
Const PROP_MUSIC_BARDING      := "#barding";
Const PROP_MUSIC_DANCE        := "DanceToBard";
Const PROP_MUSIC_LISTEN       := "#listentobarding";
Const PROP_MUSIC_STEALTH      := "#listenstealth";
Const PROP_MUSIC_SCR_SONGNAME := "musicname";
Const PROP_MUSIC_SCR_NOTES    := "noteslist";
Const PROP_MUSIC_SCR_IS_COPY  := "copied";
// }}}

Const REALM_DEATH := "deaths_domain";

Const GLOBAL_POL_CFG_STRUCT   := "#PolCfgStruct";
Const GLOBAL_SSOPT_CFG_STRUCT := "#ServSpecOptCfgStruct";

Function ReadPolCFG()
  Var lines:=ReadFile("pol.cfg");
  Var pol:=struct;
  ParseCfg(lines,pol);
  SetGlobalProperty(GLOBAL_POL_CFG_STRUCT,pol);
EndFunction

Function ReadSSOptCFG()
  Var lines:=ReadFile("config/servspecopt.cfg");
  Var ssopt:=struct;
  ParseCfg(lines,ssopt);
  SetGlobalProperty(GLOBAL_SSOPT_CFG_STRUCT,ssopt);
EndFunction

Function ParseCfg(lines, byref cfgstruct)
  Foreach line in lines
    line:=Trim(line);
    If (line[1] in {"#","/"})
      continue;
    EndIf
    var pos:=Find(line,"=",1);
    If (!pos)
      pos:=Find(line,CChr(32),1); //leerzeichen
      If (!pos)
      	pos:=Find(line,CChr(9),1); //tab
      	If (!pos)
          continue;
        EndIf
      EndIf
    EndIf
    var value:=Trim(line[pos+1,len(line)-pos]);
    var tvalue := Lower(CStr(value)); //Numeric check
    If ((tvalue == Lower(CStr(CInt(tvalue)))) || (tvalue == Lower(CStr(Hex(CInt(tvalue))))))
      value:=CInt(value);
    EndIf
    cfgstruct.insert(Trim(line[1,pos-1]),value);
  EndForEach
EndFunction
