///////////////////////////////////////////////////////////////////////////
// Msgs include - Alles zum Thema Nachrichten/Mitteilungen
//
// Author: Shinigami

///////////////////////////////////////////////////////////////////////////
//
// Ersetzungen
//   SendSysMessagePergon        - Schickt eine SystemMeldung an den Spieler
//   PrintTextAbovePergon        - Zeigt einen Text ueber dem Objekt an
//   PrintTextAboveLocalizedPergon
//                               - Zeigt passend lokalisierten Text ueber
//                                 dem Objekt an
//   PrintTextAbovePrivatePergon - Zeigt einen Text ueber dem Objekt an, den
//                                 aber nur Character sieht
//   BroadcastPergon             - Schickt eine SystemMeldung an alle Spieler
//   RequestInputPergon          - Texteingabe vom Character holen
//   SayPergon                   - Zeigt einen Text ueber dem NPC an
//
// Steuerung
//   InitLanguage   - Initialisiert die Sprache des Spielers
//   ChangeLanguage - Wechselt die Sprache des Spielers
//   GetLanguage    - Ermittelt die aktuelle Sprache des Spielers
//   ShowLanguage   - Zeigt die aktuelle Sprache des Spielers an
//   GetMessageText - Wählt Text entsprechend der eingestellten Sprache
//
// Logon/Logoff
//   MessageOfTheDay      - Zeigt die Message of the Day an
//   ShutDownMessage      - Zeigt die ShutDown-Message an
//   ShutDownMessageLogon - ShutDown-Message anzeigen, sofern gerade der
//                          CountDown laeuft
//   GetMotDText          - Liefert den Text eines Abschnittes zurueck

// TODO:
//   SendStringAsTipWindow prüfen

/////////////////
// Bibliotheken&Includes
/////////////////

use os;
use unicode;
use uo;

include "include/logutil";

///////////////
// Konstanten
///////////////
Const LANGUAGE_GERMAN  := "Deutsch";  // Nur Deutsch
Const LANGUAGE_ENGLISH := "English";  // Nur Englisch
Const LANGUAGE_BOTH    := -1;         // Sowohl Deutsch, als auch Englisch

// CProp innerhalb des Characters, die die Spracheinstellung aufnimmt
Const LANGUAGE_CPROP := "Language";

//Const TO_TRANSLATE := "--- to translate ---"; // Hinweis
Const TO_TRANSLATE := "";                     // Nervt...

Const NOSYSMSG := "NoSysMsg"; // Sollen die Meldungen ueberhaupt angezeigt werden?

// Farbkonstanten
Const MSG_COLOR_ERROR  :=  37;
Const MSG_COLOR_FAIL   := 332; // Hauptsache anders als Fehlerfarbe
Const MSG_COLOR_NORMAL := 946; // 933
Const MSG_COLOR_SUCC   :=  86; // 468
Const MSG_COLOR_NOTE   := 467; // 468

Const GPROP_SUPPRESSWARNING_NOSPEECHTARGET := "nospeechtargetwarning";

/////////////////////
//                 //
//   Ersetzungen   //
//                 //
/////////////////////

/////////////////////////////////////////////////////////////////////
// SendSysMessagePergon - Schickt eine SystemMeldung an den Spieler
/////////////////////////////////////////////////////////////////////
Function SendSysMessagePergon(who, textger, texteng := "", font := _DEFAULT_TEXT_FONT, color := _DEFAULT_UCCOLOR)
  If (!who)
    If (!GetGlobalProperty(GPROP_SUPPRESSWARNING_NOSPEECHTARGET))
      syslog(
        "FEHLER: Aufruf von SendSysMessagePergon('"+textger+
        "') ist fehlerhaft - 'who' ist ungueltig"
      );
    EndIf
    return;
  EndIf
  If (who.isa(POLCLASS_NPC))  // bei NPC sofort Abbruch
    return;
  EndIf
  If (!who.isa(POLCLASS_MOBILE))
    syslog(
      "FEHLER: Aufruf von SendSysMessagePergon('"+textger+
      "') ist fehlerhaft - kein Mobile ("+TypeOf(who)+")!"
    );
    return;
  EndIf

  // Muessen den identischen Typ aufweisen (UniCode oder ASCII)
  If (TypeOfInt(textger)<>TypeOfInt(texteng))
    syslog(
      "FEHLER: Aufruf von (SendSysMessage|Broadcast)Pergon('"+textger+
      "') ist fehlerhaft! ("+TypeOf(textger)+" vs. "+TypeOf(texteng)+")"
    );
    return;
  EndIf

  If (TypeOfInt(textger) != OT_STRUCT)
    textger := struct{uc_text := CAscz(textger), lang := "DEU"};
    texteng := struct{uc_text := CAscz(texteng), lang := "ENG"};
  EndIf

  // Sollen die Meldungen ueberhaupt angezeigt werden?
  If (!GetObjProperty(who, NOSYSMSG))
    // Ermittelt den Text entsprechend der eingestellten Sprache
    var text := GetMessageText(who, textger, texteng);

    If (text.lang)
      SendSysMessageUC(who, text.uc_text, text.lang, font, color);
    Else
      SendSysMessageUC(who, text.uc_text, text.langcode, font, color);
    EndIf
  EndIf
EndFunction

// zeigt einen Text ueber dem Objekt an, aber in der eingestellen Sprache
// der jeweiligen Anwesenden
Function PrintTextAboveLocalizedPergon(above_object, textger, texteng := "", font := _DEFAULT_TEXT_FONT, color := _DEFAULT_TEXT_COLOR)
  ForEach mob in (ListMobilesNearLocationEx(
    above_object.x, above_object.y, LIST_IGNORE_Z, 30,
    LISTEX_FLAG_NORMAL+LISTEX_FLAG_HIDDEN+LISTEX_FLAG_CONCEALED,
    above_object.realm
  ))
    SleepMs(2);
    If (mob.isa(POLCLASS_NPC))
      continue;
    EndIf
    PrintTextAbovePrivatePergon(
      above_object, textger, texteng, mob, font, color
    );
  EndForEach
EndFunction

////////////////////////////////////////////////////////////////
// PrintTextAbovePergon - Zeigt einen Text ueber dem Objekt an
////////////////////////////////////////////////////////////////
Function PrintTextAbovePergon(who, above_object, textger, texteng := "", font := _DEFAULT_TEXT_FONT, color := _DEFAULT_TEXT_COLOR)
  PrintTextAboveLocalizedPergon(above_object, textger, texteng, font, color);
  return;
  who := 0;
EndFunction

///////////////////////////////////////////////////////////////////////////
// PrintTextAbovePrivatePergon - Zeigt einen Text ueber dem Objekt an,
// den aber nur Character sieht
///////////////////////////////////////////////////////////////////////////
Function PrintTextAbovePrivatePergon(above_object, textger, texteng, who, font := _DEFAULT_TEXT_FONT, color := _DEFAULT_TEXT_COLOR)
  If (!who)
    If (!GetGlobalProperty(GPROP_SUPPRESSWARNING_NOSPEECHTARGET))
      syslog(
        "FEHLER: Aufruf von PrintTextAbovePrivatePergon('"+textger+
        "') ist fehlerhaft - 'who' ist ungueltig"
      );
    EndIf
    return;
  EndIf
  If (who.isa(POLCLASS_NPC))  // bei NPC sofort Abbruch
    return;
  EndIf
  If (!who.isa(POLCLASS_MOBILE))
    syslog(
      "FEHLER: Aufruf von PrintTextAbovePrivatePergon('"+textger+
      "') ist fehlerhaft - kein Mobile ("+TypeOf(who)+")!"
    );
    return;
  EndIf

  If (TypeOfInt(textger) != OT_STRUCT)
    textger := struct{uc_text := CAscz(textger), lang := "DEU"};
    texteng := struct{uc_text := CAscz(texteng), lang := "ENG"};
  EndIf

  // Muessen den identischen Typ aufweisen (UniCode oder ASCII)
  If (TypeOfInt(textger)<>TypeOfInt(texteng))
    syslog(
      "FEHLER: Aufruf von PrintTextAbovePrivatePergon ('"+textger+
      "') ist fehlerhaft! ("+TypeOf(textger)+" vs. "+TypeOf(texteng)+")"
    );
    return;
  EndIf

  // Ermittelt den Text entsprechend der eingestellten Sprache
  var text := GetMessageText(who, textger, texteng);

  If (text.lang)
    PrintTextAbovePrivateUC(above_object, text.uc_text, text.lang, who, font, color);
  Else
    PrintTextAbovePrivateUC(above_object, text.uc_text, text.langcode, who, font, color);
  EndIf
EndFunction

/////////////////////////////////////////////////////////////////
// BroadcastPergon - Schickt eine SystemMeldung an alle Spieler
/////////////////////////////////////////////////////////////////
Function BroadcastPergon(textger, texteng := "", font := _DEFAULT_TEXT_FONT, color := _DEFAULT_UCCOLOR)
  ForEach player in EnumerateOnlineCharacters()
    // Sollen die Meldungen ueberhaupt angezeigt werden?
    If (!GetObjProperty(player, NOSYSMSG))
      SendSysMessagePergon(player, textger, texteng, font, color);
    EndIf
    SleepMS(2);
  EndForEach
EndFunction

/////////////////////////////////////////////////////////////////
// BroadcastPergonLocal - Schickt eine SystemMeldung an alle Spieler
// innerhalb von <range> um <where>
/////////////////////////////////////////////////////////////////
Function BroadcastPergonLocal(where, range, textger, texteng := "",
  font := _DEFAULT_TEXT_FONT, color := _DEFAULT_UCCOLOR
)
  ForEach player in (ListMobilesNearLocationEx(
      where.x, where.y, LIST_IGNORE_Z, range,
      LISTEX_FLAG_NORMAL+LISTEX_FLAG_HIDDEN+LISTEX_FLAG_GHOST+
      LISTEX_FLAG_CONCEALED, where.realm
  ))
    // Sollen die Meldungen ueberhaupt angezeigt werden?
    If (!GetObjProperty(player, NOSYSMSG))
      SendSysMessagePergon(player, textger, texteng, font, color);
    EndIf
    SleepMS(2);
  EndForEach
EndFunction

/////////////////////////////////////////////////////////
// RequestInputPergon - Texteingabe vom Character holen
/////////////////////////////////////////////////////////
Function RequestInputPergon(who, item, textger, texteng := "")
  // Muessen den identischen Typ aufweisen (UniCode oder ASCII)
  If (TypeOfInt(textger)<>TypeOfInt(texteng))
    syslog(
      "FEHLER: Aufruf von RequestInputPergon ('"+textger+
      "') ist fehlerhaft! ("+TypeOf(textger)+" vs. "+TypeOf(texteng)+")"
    );
    return;
  EndIf
  If (!who.isa(POLCLASS_MOBILE))
    syslog(
      "FEHLER: Aufruf von RequestInputPergon ('"+textger+
      "') ist fehlerhaft - kein Mobile ("+
      TypeOf(who)+" "+LHex(who.serial)+")!"
    );
    return;
  EndIf

  If (TypeOfInt(textger) != OT_STRUCT)
    textger := struct{uc_text := CAscz(textger), lang := "DEU"};
    texteng := struct{uc_text := CAscz(texteng), lang := "ENG"};
  EndIf

  // Ermittelt den Text entsprechend der eingestellten Sprache
  var text := GetMessageText(who, textger, texteng);

  If (text.lang)
    return RequestInputUC(who, item, text.uc_text, text.lang);
  Else
    return RequestInputUC(who, item, text.uc_text, text.langcode);
  EndIf
EndFunction

//////////////////////////////////////////////////
// SayPergon - Zeigt einen Text ueber dem NPC an
//////////////////////////////////////////////////
Function SayPergon(who, textger, texteng := "")
  var npc := Self();
  PrintTextAbovePergon(who, npc, textger, texteng, npc.speech_font, npc.speech_color);
EndFunction

////////////////////////////////////////////
// SendMessagePlayer - Interaktiv Nachricht zwischen Spielern schicken
////////////////////////////////////////////
Const IS_THREAT := 1;
Function SendMessagePlayer(who, whom, msg_type := 0)
  var text := RequestInputPergon(who, who.backpack, "Welche Nachricht möchtet Ihr "+whom.name+" schicken?", "Which message do you want to send to "+whom.name+"?");
  If (!text)
    SendSysMessagePergon(who, "Abbruch", "Abort");
    return;
  EndIf

  var asciimsg := CChrZ(text.uc_text);
  var ucmsgde := text;
  var ucmsgen := text;
  ucmsgde.uc_text := CAscZ(who.name+": ")+(text.uc_text);
  ucmsgen.uc_text := CAscZ(who.name+": ")+(text.uc_text);

  If (!whom.connected)
    SendSysMessagePergon(who,
      whom.name+" hat sich leider in der Zwischenzeit ausgeloggt!",
      whom.name+" has logged off!",
      _DEFAULT_TEXT_FONT, FONTCOLOR_REDBROWN
    );
    return;
  EndIf

  If (whom.getprop(PROP_NOTIFY_MESSAGE))
      PlaySoundEffectPrivate(whom, 0x0506, whom);
  EndIf

  If (msg_type != IS_THREAT)
    SendSysMessagePergon(whom, ucmsgde, ucmsgde, _DEFAULT_UCFONT, FONTCOLOR_GREEN);
  Else
    SendSysMessagePergon(whom, ucmsgde, ucmsgen, _DEFAULT_UCFONT, FONTCOLOR_RED);

    // loggen
    LogToFile(
      "log/lootitems.log", GetDateTimeStr()+" "+CharInfoStr(who)+" droht "+
      CharInfoStr(whom)+" mit: "+asciimsg
    );

    // wichtige Daten fuer Drohung sammeln
    var vermerk := struct;
    vermerk.serial := who.serial;
    vermerk.text   := asciimsg;
    vermerk.time   := polcore().systime+7200; // MSZ
    vermerk.timeS  := GetDateTimeStr();

    // Die Drohung für Beweise am Player vermerken
    // Gültigkeit von Drohungen (Drohzeit) wird in chrdeath.src geprüft)
    var threatdict := GetObjProperty(whom, "threats");
    If (!threatdict)
      threatdict := dictionary;
    EndIf
    threatdict[who.serial] := vermerk;
    SetObjProperty(whom, "threats", threatdict);
  EndIf

  ucmsgde.uc_text := CAscZ("Ich an "+whom.name+": ")+(text.uc_text);
  ucmsgen.uc_text := CAscZ("Myself to "+whom.name+": ")+(text.uc_text);
  SendSysMessagePergon(who, ucmsgde, ucmsgen, _DEFAULT_UCFONT, FONTCOLOR_REDBROWN);

  StatistikMsg(who, whom, asciimsg); // Statistische Analyse durchfuehren

  var Spectator := GetGlobalProperty("#Spectator");
  If (Spectator)
    var spectatorev := struct;
    spectatorev.+type  := "msg";
    spectatorev.+value := {
      who.acctname, whom.acctname, who.name+" -> "+whom.name+": "+asciimsg
    };
    GetProcess(Spectator).sendevent(spectatorev);
  EndIf
EndFunction

///////////////////
//               //
//   Steuerung   //
//               //
///////////////////

//////////////////////////////////////////////////////////
// InitLanguage - Initialisiert die Sprache des Spielers
//////////////////////////////////////////////////////////
Function InitLanguage(who)
  If (!who.isa(POLCLASS_MOBILE) or (who.isa(POLCLASS_NPC)))
    // ist kein Spieler
    return;
  EndIf
  If (!GetObjProperty(who, LANGUAGE_CPROP))
    // Noch keine Sprache gesetzt
    syslog("HINWEIS: Setze Sprache fuer "+CharInfoStr(who));
    SetObjProperty(who, LANGUAGE_CPROP, LANGUAGE_GERMAN);
  EndIf

  ShowLanguage(who); // Zeigt die aktuelle Sprache des Spielers an
EndFunction

///////////////////////////////////////////////////////
// ChangeLanguage - Wechselt die Sprache des Spielers
///////////////////////////////////////////////////////
Function ChangeLanguage(who, both := 0)
  If (!both) // Einfache Umschaltung der Sprache
    Case (GetObjProperty(who, LANGUAGE_CPROP))
    LANGUAGE_GERMAN:
      SetObjProperty(who, LANGUAGE_CPROP, LANGUAGE_ENGLISH);
    LANGUAGE_ENGLISH:
      SetObjProperty(who, LANGUAGE_CPROP, LANGUAGE_GERMAN);
    LANGUAGE_BOTH:
      SetObjProperty(who, LANGUAGE_CPROP, LANGUAGE_GERMAN);
    EndCase
  Else // ... lieber alles anzeigen
    SetObjProperty(who, LANGUAGE_CPROP, LANGUAGE_BOTH);
  EndIf

  ShowLanguage(who); // Zeigt die aktuelle Sprache des Spielers an
EndFunction

//////////////////////////////////////////////////////////////
// ShowLanguage - Zeigt die aktuelle Sprache des Spielers an
//////////////////////////////////////////////////////////////
Function ShowLanguage(who)
  Case (GetObjProperty(who, LANGUAGE_CPROP))
  LANGUAGE_GERMAN:
    SendSysMessagePergon(who,
      "Eure Sprache ist Deutsch.",
      "Your language is German.", color := FONTCOLOR_GREEN
    );

  LANGUAGE_ENGLISH:
    SendSysMessagePergon(who,
      "Eure Sprache ist Englisch.",
      "Your language is English.", color := FONTCOLOR_GREEN
    );

  LANGUAGE_BOTH:
    SendSysMessagePergon(who,
      "Eure Sprache ist Deutsch und Englisch.",
      "Your language is German and English.", color := FONTCOLOR_GREEN
    );

  default:
    syslog(
      "FEHLER: Die CProp Language von "+CharInfoStr(who, COORDS_REALM)+
      " enthaelt den Wert '"+GetObjProperty(who, LANGUAGE_CPROP)+"'!"
    );
    InitLanguage(who);
  EndCase
EndFunction

//////////////////////////////////////////////////////////////
// GetLanguage - Ermittelt die aktuelle Sprache des Spielers
//////////////////////////////////////////////////////////////
Function GetLanguage(who)
  If (GetObjProperty(who, LANGUAGE_CPROP))
    return (GetObjProperty(who, LANGUAGE_CPROP));
  Else
    return 0;
  EndIf
EndFunction

///////////////////////////////////////////////////////////////////////////////
// GetMessageText - Ermittelt den Text entsprechend der eingestellten Sprache
///////////////////////////////////////////////////////////////////////////////
Function GetMessageText(who, textger, texteng)
  // var scriptname := GetProcess(GetPID()).name;

  // Hinweismeldung einbetten (default-Param reicht hier nicht)
  If (texteng == "")
    If (TypeOfInt(textger) == OT_STRUCT) // UniCode
      texteng := Struct{uc_text := CAscZ(TO_TRANSLATE), lang := "ENG"};
    Else // ASCII
      texteng := TO_TRANSLATE;
    EndIf
  EndIf

  var language := LANGUAGE_BOTH;
  If (who) // Eine gezielte Meldung an einen Spieler?
    If (!who.isa(POLCLASS_NPC))
      language := GetObjProperty(who, LANGUAGE_CPROP);
    EndIf
  EndIf

  Case (language)
  LANGUAGE_GERMAN:
    return textger;

  LANGUAGE_ENGLISH:
    return texteng;

  LANGUAGE_BOTH:
    If (TypeOfInt(textger) == OT_STRUCT) // UniCode
      If (texteng.uc_text.size()) // Gibts ueberhaupt einen englischen Text?
        return (Struct{uc_text := textger.uc_text+CAscZ(" / ")+texteng.uc_text, lang := "DEU"});
      Else
        return (Struct{uc_text := textger.uc_text, lang := "DEU"});
      EndIf
    Else // ASCII
      If (texteng) // Gibts ueberhaupt einen englischen Text?
        return textger+" / "+texteng;
      Else
        return textger;
      EndIf
    EndIf

  default:
    syslog(
      "FEHLER: Die CProp Language von "+CharInfoStr(who, COORDS_REALM)+
      " enthaelt den Wert '"+GetObjProperty(who, LANGUAGE_CPROP)+"'!"
    );
    InitLanguage(who);
  EndCase
EndFunction


///////////////////////////////////////////////////////////
// Funktionen mit kurzen Namen um Nachrichten mit einheitlichen
// Farbcodes zu senden
// -------------
// serror = rote sysmessage senden (für fehler oder negative einflüsse/nachrichten)
// succ = grüne sysmsg senden (für erfolg oder positive einflüsse/nachrichten)
// sysmsg = graue sysmsg senden (für neutrale/nur informelle nachrichten)
//
// pabovep = printtextaboveprivateuc in grau (s.o.)
// pabovepe = printtextaboveprivateuc in rot (s.o.)
// paboveps = printtextaboveprivateuc in grün (s.o.)
//
// pabove = PrintTextAbovePergon in grau (s.o.)
// pabovee = PrintTextAbovePergon in rot (s.o.)
// paboves = PrintTextAbovePergon in grün (s.o.)
///////////////////////////////////////////////////////////

Function serror(who, txt)
  return sendsysmessageUC(who, CAscZ(txt), "DEU", 3, MSG_COLOR_ERROR);  // 40 / 2118
EndFunction

Function succ(who, txt)
  return sendsysmessageUC(who, CAscZ(txt), "DEU", 3, MSG_COLOR_SUCC);   // 75
EndFunction

Function sysmsg(who, txt)
  return sendsysmessageUC(who, CAscZ(txt), "DEU", 3, MSG_COLOR_NORMAL);  // 954
EndFunction

Function pabovep(who, txt, what)
  return PrintTextAbovePrivateUC(who, CAscZ(txt), "DEU", what, 3, MSG_COLOR_NORMAL);
EndFunction

Function pabovepe(who, txt, towhom)
  return PrintTextAbovePrivateUC(who, CAscZ(txt), "DEU", towhom, 3, MSG_COLOR_ERROR);
EndFunction

Function paboveps(who, txt, towhom)
  return PrintTextAbovePrivateUC(who, CAscZ(txt), "DEU", towhom, 3, MSG_COLOR_SUCC);
EndFunction

Function pabove(who, txt)
  return PrintTextAboveUC(who, CAscZ(txt), "DEU", 3, MSG_COLOR_NORMAL);
EndFunction

Function pabovee(who, txt)
  return PrintTextAboveUC(who, CAscZ(txt), "DEU", 3, MSG_COLOR_ERROR);
EndFunction

Function paboves(who, txt)
  return PrintTextAboveUC(who, CAscZ(txt), "DEU", 3, MSG_COLOR_SUCC);
EndFunction

//////////////////////////////////////////////////////
// MessageOfTheDay - Zeigt die Message of the Day an
//////////////////////////////////////////////////////
Function MessageOfTheDay(who)
  var motdfile := ReadConfigFile("speech/messageoftheday");
  If (!motdfile)
    syslog(
      "HINWEIS: Keine 'MotD'-Datei gefunden: "+motdfile.errortext
    );
    return;
  EndIf

  var msgid := GetConfigMaxIntKey(motdfile);
  If (
    // gibt offenbar noch keine Nachrichten
    !msgid or
    // nur einmal pro Char erzeugen
    msgid == GetObjProperty(who, PROP_MOTD)
  )
    // unload, damit man im laufenden Betrieb leicht Nachrichten einbauen kann
    UnloadConfigFile("speech/messageoftheday");
    return;
  EndIf

  // Nachricht besorgen
  var message := FindConfigElem(motdfile, msgid);

  // laenger als ca. 2 Monate her?
  var nowtime := PolCore().systime;
  var msgtime := GetConfigInt(message, "UnixTime");
  If (msgtime + (70 * 24 * 3600) < nowtime)
    // zu alt, Nachricht ignorieren, nicht wieder probieren
    SetObjProperty(who, PROP_MOTD, msgid);

    UnloadConfigFile("speech/messageoftheday");
    return;
  EndIf

  // Nachrichtenzettel erzeugen
  var title := GetConfigString(message, "Title");
  message := CreateItemInBackpackPergon(who, 0xfff3);
  SetObjProperty(message, PROP_MOTD_ID, msgid);
  If (title)
    SetName(message, "Neuigkeit: "+title);
  EndIf
  // Erstellung merken
  SetObjProperty(who, PROP_MOTD, msgid);
  // angucken lassen
  UseItem(message, who);

  UnloadConfigFile("speech/messageoftheday");
EndFunction

////////////////////////////////////////////////////
// ShutDownMessage - Zeigt die ShutDown-Message an
////////////////////////////////////////////////////
Function ShutDownMessage(params, paramssplit)
  // Uebergebene Message benutzen, oder aus dem Configfile auslesen?
  If (paramssplit.size()>1)
    params := params-(paramssplit[1]+" ")+
      " (Hinweis: Wer sich nicht rechtzeitig ausloggt und dadurch "+
      "den Verlust einiger Gegenstaende erleidet, hat dies selbst "+
      "zu verantworten.)";
    SetGlobalProperty("#ShutDownMessage", params);
    syslog("SHUTDOWN-Msg: "+params);

    ForEach onlinechr in EnumerateOnlineCharacters()
      SendStringAsTipWindow(onlinechr, params);
    EndForEach
  Else
    var motdfile := ReadConfigFile("speech/messageoftheday");
    If (!motdfile)
      syslog(
        "HINWEIS: Keine 'MotD'-Datei bei Shutdown gefunden: "+
        motdfile.errortext
      );
      return;
    EndIf

    var message := FindConfigElem(motdfile, "Message");
    If (message)
      SetGlobalProperty("#ShutDownMessageFormat", message);

      ForEach onlinechr in EnumerateOnlineCharacters()
        SendStringAsTipWindow(onlinechr, GetMotDText(onlinechr, message));
      EndForEach
    EndIf
    // UnloadConfigFile(MoDFilePath);
  EndIf
EndFunction

///////////////////////////////////////////////////////////////////////////
// ShutDownMessageLogon - ShutDown-Message anzeigen, sofern gerade der
// CountDown laeuft
///////////////////////////////////////////////////////////////////////////
Function ShutDownMessageLogon(who)
  var shutdowntime := GetGlobalProperty(PROP_SHUTDOWN_RUNNING);
  If (shutdowntime)
    var time := shutdowntime - ReadGameClock();
    var minutes := CInt(time/60);
    var seconds := time-minutes*60;

    var params := "Hinweis: In "+minutes+" Minuten und "+seconds+
      " Sekunden wird der Server heruntergefahren.";
    var message := GetGlobalProperty("#ShutDownMessage");
    If (message)
      params += " Folgender Grund liegt vor: '"+message+"'.";
    Else
      message := GetGlobalProperty("#ShutDownMessageFormat");
      If (message)
        params += " Folgender Grund liegt vor: '"+
          GetMotDText(who, message)+"'.";
      EndIf
    EndIf

    SendStringAsTipWindow(who, params);
  EndIf
EndFunction

////////////////////////////////////////////////////////////
// GetMotDText - Liefert den Text eines Abschnittes zurueck
////////////////////////////////////////////////////////////
Function GetMotDText(who, abschnitt)
  var text := GetConfigStringArray(abschnitt, "Text");
  If (!text.size())
    If (who.gender)
      text := GetConfigStringArray(abschnitt, "TextW");
    Else
      text := GetConfigStringArray(abschnitt, "TextM");
    EndIf
  EndIf

  var abschnitttext := "";
  ForEach textline in text
    If ("." == textline)
      // Zeilen mit nur einem Punkt werden zum Absatz
      abschnitttext += CChr(0x0d)+CChr(0x0d);
    Else
      abschnitttext += textline+" ";
    EndIf
  EndForEach

  return (abschnitttext);
EndFunction

// ggf. Text an eine schon vorhandene LogonMessage hängen
Function UpdateLogonMessage(who, message)
  var oldMessage := GetObjProperty(who, "LogonMessage");
  If (oldMessage)
    message := oldMessage+CChr(0x0d)+CChr(0x0d)+message;
  EndIf
  SetObjProperty(who, "LogonMessage", message);
EndFunction

// Nachricht ggf. an zwei Character schicken
Function SendSysMessageTwice(who1, who2, message1, message2 := "")
  If (TypeOfInt(message1) == OT_STRUCT)
    SendSysMessagePergon(who1, message1.ger, message1.eng);
  Else
    SendSysMessagePergon(who1, message1);
  EndIf
  If (who2.serial != who1.serial)
    If (message2 != "")
      If (TypeOfInt(message2) == OT_STRUCT)
        SendSysMessagePergon(who2, message2.ger, message2.eng);
      Else
        SendSysMessagePergon(who2, message2);
      EndIf
    Else
      If (TypeOfInt(message1) == OT_STRUCT)
        SendSysMessagePergon(who2, message1.ger, message1.eng);
      Else
        SendSysMessagePergon(who2, message1);
      EndIf
    EndIf
  EndIf
EndFunction
