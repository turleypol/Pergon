///////////////////////////////////////////////////////////////////////////
// PergonStatistik -
// Fuehrt teilweise :) nuetzliche Statistik-Erfassungen durch
//
// Author: Shinigami

///////////////////////////////////////////////////////////////////////////
//   Modifications:

///////////////////////////////////////////////////////////////////////////
// ToDo
// naja, sowas wie, die groesste Labertasche ermitteln, also Woerter zaehlen :)
//
// Statistiken
//   InitStatistik        - Initialisiert dieses Modul
//
//   StatistikLogOnOff    - Statistische Analyse beim Ein-/Ausloggen
//   StatistikListener    - Statistische Analyse fuer das Listener-Script
//   StatistikLogging     - Statistische Analyse fuer das Logging-Script
//   StatistikMsg         - Statistische Analyse fuer das Msg-Script
//   StatistikSayAbove    - Statistische Analyse fuer das SayAbove-Script
//   StatistikSayAbovePvt - Statistische Analyse fuer das SayAbovePvt-Script
//   StatistikPartyFull   - Statistische Analyse fuer das Party-Script (alle Mitglieder)
//   StatistikPartyMember - Statistische Analyse fuer das Party-Script (einzelnes Mitglied)

/////////////////
// Bibliotheken
/////////////////

Use file;

/////////////////////
//                 //
//   Statistiken   //
//                 //
/////////////////////

/////////////////////////////////////////////////////
// InitStatistikPergon - Initialisiert dieses Modul
/////////////////////////////////////////////////////

Function InitStatistik()
  If (GetGlobalProperty(PERGON) Or GetGlobalProperty(BAUSERVER))
    Start_ScriptPergon("::misc/logging"); // Zentrales Logging aktivieren, und #Logging setzen
  EndIf
EndFunction

/////////////////////////////////////////////////////////////////
// StatistikLogOnOff - Statistische Analyse beim Ein-/Ausloggen
/////////////////////////////////////////////////////////////////

Function StatistikLogOnOff(who, text)
  If (GetGlobalProperty("#Statistik")) // Ueberhaupt aktiv (die Prop wird in misc/logging gesetzt) ?
    Var time:=GetDateTimeStr();
    LogToFile("log/Speech/"+who.name+" - "+Lower(Hex(who.serial))+".log", time+text);
  EndIf
EndFunction

//////////////////////////////////////////////////////////////////////
// StatistikListener - Statistische Analyse fuer das Listener-Script
//////////////////////////////////////////////////////////////////////

Function StatistikListener(who, ev)
  If (GetGlobalProperty("#Statistik")) // Ueberhaupt aktiv (die Prop wird in misc/logging gesetzt) ?
    Var speech:=who.name+" ["+who.x+", "+who.y+"] "+ev.text;
    Var time:=GetDateTimeStr();
    SysLog(speech);
    LogToFile("log/Speech/speech.log", time+speech);
    LogToFile("log/Speech/"+who.name+" - "+Lower(Hex(who.serial))+".log", time+speech);
   // LogToFile("log/speech.log", time+speech);
   // LogToFile("log/"+who.name+" - "+Lower(Hex(who.serial))+".log", time+speech);
    QuestCharLog(who, "Listener: "+ev.text);

    Var Spectator:=GetGlobalProperty("#Spectator");
    If (Spectator)
      Var spectatorev:=struct;
      spectatorev.+type:="statistik";
      spectatorev.+value:={who.x, who.y, speech};

      GetProcess(Spectator).sendevent(spectatorev);
    EndIf
  EndIf

  // "böses Wort" dann ab in eine Logfile damit (besseres Aufspüren von freidrehenden Leuten)
  ForEach word in quirolantenwords
  	If (Lower(ev.text)[word])
  		LogToFile("log/Security/quirolanten.log", "["+who.acctname+"] "+who.name+" => "+Upper(word)+": "+ev.text, 1);
  		Break;
  	EndIf
  EndForEach
EndFunction

////////////////////////////////////////////////////////////////////
// StatistikLogging - Statistische Analyse fuer das Logging-Script
////////////////////////////////////////////////////////////////////

Function StatistikLogging(ev)
  If (GetGlobalProperty("#Statistik")) // Ueberhaupt aktiv (die Prop wird in misc/logging gesetzt) ?
    Case (ev.type)
      "tellmember": SysLog(ev.speech); // Party-System - Nachricht an einen bestimmten schicken
                    LogToFile("log/Speech/speech.log", ev.timespeech);
                    LogToFile(ev.sender, ev.timespeech);
                    LogToFile(ev.receiver, ev.timespeech);

      "tellfull":   SysLog(ev.speech); // Party-System - Nachricht an alle schicken
                    LogToFile("log/Speech/speech.log", ev.timespeech);
                   // LogToFile(ev.sender, ev.timespeech); // Steckt in members mit drin
                    ForEach receiver in (ev.receivers)
                      LogToFile(receiver, ev.timespeech);
                    EndForEach

      default:      SysLog("FEHLER: Unbekannter Typ = "+ev.type);
    EndCase
  Else
    SysLog("FEHLER: Unbekannter Typ = "+ev.type);
  EndIf
EndFunction

////////////////////////////////////////////////////////////
// StatistikMsg - Statistische Analyse fuer das Msg-Script
////////////////////////////////////////////////////////////

Function StatistikMsg(who, whom, asciimessage)
  If (GetGlobalProperty("#Statistik")) // Ueberhaupt aktiv (die Prop wird in misc/logging gesetzt) ?
    Var speech:=who.name+" -> "+whom.name+": "+asciimessage;
    Var time:=GetDateTimeStr();
    SysLog(speech);
    LogToFile("log/Speech/speech.log", time+speech);
    LogToFile("log/Speech/"+who.name+" - "+Lower(Hex(who.serial))+".log", time+speech);
    LogToFile("log/Speech/"+whom.name+" - "+Lower(Hex(whom.serial))+".log", time+speech);
   // LogToFile("log/speech.log", time+speech);
   // LogToFile("log/"+who.name+" - "+Lower(Hex(who.serial))+".log", time+speech);
   // LogToFile("log/"+whom.name+" - "+Lower(Hex(whom.serial))+".log", time+speech);
    QuestCharLog(who, "TextCMD Msg: "+speech);
  EndIf
EndFunction

//////////////////////////////////////////////////////////////////////
// StatistikSayAbove - Statistische Analyse fuer das SayAbove-Script
//////////////////////////////////////////////////////////////////////

Function StatistikSayAbove(who, what, text)
  If (GetGlobalProperty("#Statistik")) // Ueberhaupt aktiv (die Prop wird in misc/logging gesetzt) ?
    Var speech:=who.name+" ueber "+what.name+": "+text;
    Var time:=GetDateTimeStr();
    SysLog(speech);
    LogToFile("log/Speech/speech.log", time+speech);
    LogToFile("log/Speech/"+who.name+" - "+Lower(Hex(who.serial))+".log", time+speech);
   // LogToFile("log/speech.log", time+speech);
   // LogToFile("log/"+who.name+" - "+Lower(Hex(who.serial))+".log", time+speech);
    If (what.isa(POLCLASS_NPC))
      QuestCharLog(who, "TextCMD SayAbove: NPC '"+what.name+"' ["+what.npctemplate+"] ["+what.x+", "+what.y+", "+what.z+"]: "+text);
    Else
      LogToFile("log/Speech/"+what.name+" - "+Lower(Hex(what.serial))+".log", time+speech);
     // LogToFile("log/"+what.name+" - "+Lower(Hex(what.serial))+".log", time+speech);
      QuestCharLog(who, "TextCMD SayAbove: Player '"+what.name+"' ["+what.acctname+"] ["+what.x+", "+what.y+", "+what.z+"]: "+text);
    EndIf
  EndIf
EndFunction

////////////////////////////////////////////////////////////////////////////
// StatistikSayAbovePvt - Statistische Analyse fuer das SayAbovePvt-Script
////////////////////////////////////////////////////////////////////////////

Function StatistikSayAbovePvt(who, what, text)
  If (GetGlobalProperty("#Statistik")) // Ueberhaupt aktiv (die Prop wird in misc/logging gesetzt) ?
    Var speech:=who.name+" ueber "+what.name+": "+text;
    Var time:=GetDateTimeStr();
    SysLog(speech);
    LogToFile("log/Speech/speech.log", time+speech);
    LogToFile("log/Speech/"+who.name+" - "+Lower(Hex(who.serial))+".log", time+speech);
    LogToFile("log/Speech/"+what.name+" - "+Lower(Hex(what.serial))+".log", time+speech);
   // LogToFile("log/speech.log", time+speech);
   // LogToFile("log/"+who.name+" - "+Lower(Hex(who.serial))+".log", time+speech);
   // LogToFile("log/"+what.name+" - "+Lower(Hex(what.serial))+".log", time+speech);
    QuestCharLog(who, "TextCMD SayAbovePvt: Player '"+what.name+"' ["+what.acctname+"] ["+what.x+", "+what.y+", "+what.z+"]: "+text);
  EndIf
EndFunction

//////////////////////////////////////////////////////////////////////////////////////
// StatistikPartyFull - Statistische Analyse fuer das Party-Script (alle Mitglieder)
//////////////////////////////////////////////////////////////////////////////////////

Function StatistikPartyFull(character, members, msg)
  Var Logging:=GetGlobalProperty("#Statistik");
  If (Logging) // Ueberhaupt aktiv (die Prop wird in misc/logging gesetzt) ?
    Var loggingev:=struct;
    loggingev.+type:="tellfull";
    loggingev.+speech:="Party: "+character.name+" -> Alle Mitglieder: "+CChrZ(msg);
    loggingev.+timespeech:=GetDateTimeStr()+" "+loggingev.speech;
   // loggingev.+sender:="log/Speech/"+character.name+" - "+Lower(Hex(character.serial))+".log"; // Steckt in members mit drin
    loggingev.+receivers:={};

    Foreach member in members
      loggingev.receivers.append("log/Speech/"+member.name+" - "+Lower(Hex(member.serial))+".log");
    EndForeach

    GetProcess(Logging).sendevent(loggingev);
  EndIf
EndFunction

///////////////////////////////////////////////////////////////////////////////////////////
// StatistikPartyMember - Statistische Analyse fuer das Party-Script (einzelnes Mitglied)
///////////////////////////////////////////////////////////////////////////////////////////

Function StatistikPartyMember(character, whom, msg)
  Var Logging:=GetGlobalProperty("#Statistik");
  If (Logging) // Ueberhaupt aktiv (die Prop wird in misc/logging gesetzt) ?
	  Var loggingev:=struct;
	  loggingev.+type:="tellmember";
	  loggingev.+speech:="Party: "+character.name+" -> "+whom.name+": "+CChrZ(msg);
	  loggingev.+timespeech:=GetDateTimeStr()+" "+loggingev.speech;
	  loggingev.+sender:="log/Speech/"+character.name+" - "+Lower(Hex(character.serial))+".log";
	  loggingev.+receiver:="log/Speech/"+whom.name+" - "+Lower(Hex(whom.serial))+".log";

	  GetProcess(Logging).sendevent(loggingev);
	EndIf
EndFunction

/////////////////////
//                 //
//   Statistiken   //
//                 //
/////////////////////

/////////////////////////////////////////////////////
// InitStatistikPergon - Initialisiert dieses Modul
/////////////////////////////////////////////////////

//Function InitStatistik()
//  Return (0); // Hmm, noch keine Funktion...
//EndFunction

/////////////////////////////////////////////////////////////////
// StatistikLogOnOff - Statistische Analyse beim Ein-/Ausloggen
/////////////////////////////////////////////////////////////////

//Function StatistikLogOnOff(who, text)
//  Return (0); // Hmm, noch keine Funktion...
//EndFunction

//////////////////////////////////////////////////////////////////////
// StatistikListener - Statistische Analyse fuer das Listener-Script
//////////////////////////////////////////////////////////////////////

//Function StatistikListener(who, ev)
//  Return (0); // Hmm, noch keine Funktion...
//EndFunction

////////////////////////////////////////////////////////////////////
// StatistikLogging - Statistische Analyse fuer das Logging-Script
////////////////////////////////////////////////////////////////////

//Function StatistikLogging(ev)
//  Return (0); // Hmm, noch keine Funktion...
//EndFunction

////////////////////////////////////////////////////////////
// StatistikMsg - Statistische Analyse fuer das Msg-Script
////////////////////////////////////////////////////////////

//Function StatistikMsg(who, whom, asciimessage)
//  Return (0); // Hmm, noch keine Funktion...
//EndFunction

//////////////////////////////////////////////////////////////////////
// StatistikSayAbove - Statistische Analyse fuer das SayAbove-Script
//////////////////////////////////////////////////////////////////////

//Function StatistikSayAbove(who, what, text)
//  Return (0); // Hmm, noch keine Funktion...
//EndFunction

////////////////////////////////////////////////////////////////////////////
// StatistikSayAbovePvt - Statistische Analyse fuer das SayAbovePvt-Script
////////////////////////////////////////////////////////////////////////////

//Function StatistikSayAbovePvt(who, what, text)
//  Return (0); // Hmm, noch keine Funktion...
//EndFunction

//////////////////////////////////////////////////////////////////////////////////////
// StatistikPartyFull - Statistische Analyse fuer das Party-Script (alle Mitglieder)
//////////////////////////////////////////////////////////////////////////////////////

//Function StatistikPartyFull(character, members, msg)
//  Return (0); // Hmm, noch keine Funktion...
//EndFunction

///////////////////////////////////////////////////////////////////////////////////////////
// StatistikPartyMember - Statistische Analyse fuer das Party-Script (einzelnes Mitglied)
///////////////////////////////////////////////////////////////////////////////////////////

//Function StatistikPartyMember(character, whom, msg)
//  Return (0); // Hmm, noch keine Funktion...
//EndFunction
