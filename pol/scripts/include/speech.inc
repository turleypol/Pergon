///////////////////////////////////////////////////////////////////////////
//  speech.inc
//
//  Author: Dream Weaver 2 June 1999
//
//  Parameters: inspeech: the words spoken to the NPC
//              mood: a string containing an element in speechgroup.cfg
//              templatename: key to the speechgroup.cfg file -- usually the
//              template name, but change For quests

///////////////////////////////////////////////////////////////////////////
// $Log: not supported by cvs2svn $
// Revision 1.5  2008/10/22 12:57:58  turley
// RemovePunctuation() vereinfacht
//
// Revision 1.4  2008/08/22 00:06:18  mehdorn
// - fehlende Variable ergaenzt
//
// Revision 1.3  2008/07/29 03:43:56  mehdorn
// - Speech-Verschiebung von npc.inc nach speech.inc begonnen
// - Process_Text heisst nun ProcessText (Einheitlichkeit)
//   - einige Scripte daher angepasst
//

use cfgfile;

Function ProcessText(inspeech, mood, templatename) // {{{
    var me := Self();
    var words := SplitWords(inspeech);
    var speech_cfg := ReadConfigFile("speechgroup");
    var elem := speech_cfg[templatename];

    ForEach word in words
        var tmpword := GetObjProperty(me, "keyword"+ word);
        If (tmpword)
            return tmpword;
        EndIf
    EndForEach

    ForEach word in words
        var wordgroup := GetConfigInt(elem, mood);
        If (!wordgroup)
            wordgroup := GetConfigInt(elem, "default");
        EndIf
        If (!wordgroup)
            wordgroup := templatename;
        EndIf
        elem := speech_cfg[wordgroup];
        var tmpword := GetConfigString(elem,  word);
        If (tmpword)
            return tmpword;
        EndIf
    EndForEach

    var retval := GetConfigString(elem, "default");
    If ((!retval) or (retval == "5"))
        retval := "";
    EndIf

    return retval;
EndFunction // }}}

// FIXME: haessliches Interface
Function GetAnswerTo(sayed, minelocation, me, template_override := "") // {{{
  Var npctemplate;
  If (template_override == "")
    npctemplate := Lower(me.npctemplate);
  Else
    npctemplate := template_override;
  EndIf

  // ist NPC Mann oder Frau?
  Var lastcharacter := npctemplate[Len(npctemplate)];
  If ((lastcharacter=="m") Or (lastcharacter=="w"))
    // FIXME: unused
    // var sex := lastcharacter;
    npctemplate := npctemplate[1, Len(npctemplate)-1];
  EndIf

  // Text vorbereiten und in Woerter zerteilen
  var sayedwords := SplitWords(RemovePunctuation(sayed));
  // foreach word in (sayedwords)
  //   syslog("NPC hoerte: "+word);
  // endforeach

  var answer_arr := array{};

  // lokale typspezifische Antworten finden
  var local_type := GetAnswerFromFile(
    Lower("speech/location/"+minelocation+"/"+npctemplate), sayedwords
  );
  If (local_type)
    answer_arr.append(local_type);
  EndIf

  // globale typspezifische Antworten finden
  var global_type :=
    GetAnswerFromFile(Lower("speech/location/"+npctemplate), sayedwords);
  If (global_type)
    answer_arr.append(global_type);
  EndIf

  // Antwort auswaehlen
  If (answer_arr.size() > 0)
    var selected := answer_arr[RandomInt(Len(answer_arr))+1];
    return selected;
  EndIf

  // Fallback auf allgemeine lokale Sachen
  var local_all := GetAnswerFromFile(
    Lower("speech/location/"+minelocation+"/all"), sayedwords
  );
  If (local_all)
    return local_all;
  EndIf

  // Fallback auf globale Sachen
  var global_all :=
    GetAnswerFromFile("speech/global", sayedwords);
  If (global_all)
    return global_all;
  EndIf

  // Fallback, letzter Versuch
  var dummy := RandomDummyAnswer(me);
  Return dummy;
EndFunction // }}}

// Zufaellige Antwort aus SpeechFile holen
Function GetAnswerFromFile(SpeechFilePath, words) // {{{
  If (!FileExists("::config/"+SpeechFilePath+".cfg"))
    return;
  EndIf

  Var SpeechFile := ReadConfigFile("::"+SpeechFilePath);
  If (!SpeechFile)
    // syslog(
    //     "WARNUNG: SpeechFile "+SpeechFilePath+" will nicht: "+
    //     SpeechFile.errortext
    // );
    return;
  EndIf

  var Antworten := array;

  Foreach word in (words)
    Var SpeechElem := FindConfigElem(SpeechFile, word);
    If (!SpeechElem)
      continue;
    EndIf
    Antworten := Antworten + GetConfigStringArray(SpeechElem, "Antwort");
  EndForeach

  UnloadConfigFile(SpeechFilePath);

  If (Antworten.size() == 0)
    return;
  EndIf
  // syslog("Ich kenne: "+Antworten);
  var selected := Antworten[RandomInt(Len(Antworten))+1];
  If (selected)
    // syslog("Gewaehlte Antwort: "+selected);
    return selected;
  EndIf
EndFunction // }}}

// Zufaellige Dummy-Antwort
Function RandomDummyAnswer(me) // {{{
  Case (RandomInt(11)+1)
    1:  return "Dazu weiss ich leider nichts.";
    2:  return "Davon habe ich keine Ahnung.";
    3:  return "Lasst mich in Ruhe.";
    4:  return "Habe gerade keine Zeit.";
    5:  return "Hmmm ... aha";
    6:  return "Wie meint Ihr?";
    7:  return "Das verstehe ich nicht.";
    8:  return "Davon weiss ich nichts.";
    9: return "Ich weiss nichts darueber.";
    10: return "Wenn Ihr meint.";
  EndCase
EndFunction // }}}

// Entfernt Satzzeichen und so Kram
Function RemovePunctuation(sometext) // {{{
  sometext:=CStr(sometext);
  If (Len(sometext)>0)
    Foreach char in (array{"^", ".", ",", "?", "!"})
      sometext:=StrReplace(sometext,char,"");
    EndForeach
  EndIf

  return sometext;
EndFunction // }}}
