///////////////////////////////////////////////////////////////////////////
// NPC-Include - haufenweise Funktionen rund um NPCs und Co.
//
// started by kiesow in dec 1999

///////////////////////////////////////////////////////////////////////////
// NPC-Kram
//  KillNPC
//    - Toetet den Player/NPC an Ort und Stelle
//  KillNPCSilent
//    - Toetet den NPC durch Verschieben zum Vernichter im Knast
//  Salute
//    - zum Ziel drehen und gruessen, dann zurueckdrehen
//  OpenDistance
//    - vor Gegner wegrennen
//  OpenDistance_
//    - anders vor Gegner wegrennen
//  NpcTitle
//    - Titel vergeben, passend umziehen und Script aendern
//  SayTownQuest
//    - questspezifische Antworten geben
//  UnComment
//    - "//"-Kommentare von String abschneiden
//  Run
//    - Wegrennen bei Angriff
//  CreateNpcFromTemplatePergon
//    - erzeugt NPC mit pergonspezizischen Tricks, z. B. zufaelliges Geschlecht
//  ParseTemplateName
//    - splittet .npctemplate in pkg und templatename auf
//  GetNPCConfig
//    - holt ein Struct mit NPC-Einstellungen zum Template
//  IsValidNPCGraphic
//    - prueft, ob sichere Grafiknummer uebergeben wurde

// wandert nach speech.inc
//  Answering
//  AnswerFromFile
//  GetDummyAnswer
//  CleanText

use cfgfile;
use datafile;
use file;
use os;
use util;
use vitals;
include "include/clock";
include "include/logutil";
include "include/server";
include "include/vitals";

Const KILLNPC_CORPSE   := 1;
Const KILLNPC_NOCORPSE := 0;

Function KillMobile(mobile, reason)
  KillNPC(mobile, reason);
EndFunction
// KillNPC - Toetet den Player/NPC
Function KillNPC(mobile, reason, corpse := KILLNPC_CORPSE) // {{{
  mobile.disable("invul");
  RevokePrivilege(mobile, "invul");
  var value := GetMaxHPPergon(mobile)+3;

  If (corpse == KILLNPC_NOCORPSE)
    // NW-Ecke GreenAcres (damit lostandfound nicht immer anspringt)
    MoveObjectToLocation(
      mobile, 5376, 1081, 0, REALM_DEATH, MOVEOBJECT_FORCELOCATION
    );
    SetObjProperty(mobile, "guardkill", 1);

  // Da brauchen wir keine Leiche...
  ElseIf (mobile.x == 5376 and mobile.y == 1081)
    SetObjProperty(mobile, "guardkill", 1);
  EndIf

  LogLastDamage(mobile, "Kill ["+reason+"]", value);
  ApplyRawDamage(mobile, value);
EndFunction // }}}

Function KillMobileSilent(mobile, reason)
  KillNPCSilent(mobile, reason);
EndFunction
// KillMobileSilent - Verschiebt einen NPC zum Vernichter in den Knast
Function KillNPCSilent(mobile, reason) // {{{
  If (mobile.isa(POLCLASS_MOBILE) and !mobile.isa(POLCLASS_NPC))
    syslog(
      "WARNUNG: KillNPCSilent wurde auf Spieler "+CharInfoStr(mobile)+
      "aufgerufen!"
    );
  EndIf
  mobile.disable("invul");
  RevokePrivilege(mobile, "invul");
  MoveObjectToLocation(
    mobile, 5275, 1185, -5, REALM_BRITANNIA, MOVEOBJECT_FORCELOCATION
  );
  LogLastDamage(mobile, "KillSilent ["+reason+"]", 0);
EndFunction // }}}

// beschriftet einen NPC mit Erzeuger und Erzeugungszeit
Function SignNPCCreation(who, npc) // {{{
  If (
    // nur fuer NPCs gedacht
    !npc.isa(POLCLASS_NPC) or
    // sollte typischerweise ein Staffler sein
    !who.isa(POLCLASS_MOBILE)
  )
    return;
  EndIf

  var signature := struct;
  signature.serial := who.serial;
  signature.acct   := who.acctname;
  signature.time   := ReadGameClock();

  SetObjProperty(npc, "CreatedBy", signature);
EndFunction // }}}

/////////////////////////////////////////////////////
//  To Salute, a guard will turn toward the salutee,
//  salute, then turn to his original facing.
/////////////////////////////////////////////////////

Function Salute(me, player)
  var old_facing := me.facing;

  TurnToward(player);
  PerformAction(me, UACTION_SALUTE);
  PlaySoundEffect(me, SFX_6F);

  Sleep(2);

  If (old_facing != me.facing)
    Move(old_facing);
  EndIf
EndFunction

///////////////////////////////////////
// OpenDistance - Erhoehe den Abstand
///////////////////////////////////////
Function OpenDistance(opponent)
  If (Distance(me, opponent)>9)
    Return (Distance(me, opponent));
  Else
    RunAwayFrom(opponent);
    Return (0);
  EndIf
EndFunction

Function OpenDistance_(opponent)
  Case (Distance(me, opponent))
    10: Return (1);

    default: RunAwayFrom(opponent);
             Return 0;
  EndCase
EndFunction

/////////////////////////////////////////////////////////////////////////////
// NpcTitle - set a title to a npc
//
// returns: any title
/////////////////////////////////////////////////////////////////////////////
Function NpcTitle(me)
  Var old_title:=SplitWords(me.name);
  Var title:=RandomInt(9);
  Var ttitle:="";

  If (!old_title[2])
    ttitle:=", "+old_title[2]+" "+old_title[3];

    Var rich;
    If (Me.gender==0) // 0 = male, 1 = female
      Case (title)
        0: rich:=0;ttitle:="";
        1: rich:=1;ttitle:=", der Buerger";
        2: rich:=0;ttitle:=", der Rumtreiber";
        3: rich:=0;ttitle:=", der Trinker";
        4: rich:=0;ttitle:=", der Zigeuner";
        5: rich:=1;ttitle:=", der Einwohner";
        6: rich:=0;ttitle:=", der Umherziehende";
        7: rich:=0;ttitle:=", der Vagabund";
        8: rich:=1;ttitle:=", der Arbeiter";
        default: rich:=0;ttitle:="";
      EndCase
    Else
      Case (title)
        0: rich:=0;ttitle:="";
        1: rich:=1;ttitle:=", die Buergerin";
        2: rich:=0;ttitle:=", die Rumtreiberin";
        3: rich:=0;ttitle:=", die Trinkerin";
        4: rich:=0;ttitle:=", die Zigeunerin";
        5: rich:=1;ttitle:=", die Einwohnerin";
        6: rich:=0;ttitle:=", die Umherziehende";
        7: rich:=0;ttitle:=", die Vagabundin";
        8: rich:=1;ttitle:=", die Arbeiterin";
        default: rich:=0;ttitle:="";
      EndCase
    EndIf

    Var Equipt:=GetObjProperty(Me, "Equipt");
    Var Pos:=Find(Equipt, "_", 1);
    If (Pos>0)
      Equipt:=Equipt[1, Pos-1];
    EndIf

    If (rich==1)
      SetObjProperty(Me, "Equipt", Equipt+"Rich");
      Me.script:="noble";
    Else
      SetObjProperty(Me, "Equipt", Equipt+"Poor");
    EndIf
  EndIf

  Return ttitle;
EndFunction

/////////////////////////////////////////////////////////
//
//  Funktion: SayTownQuest - Var (Area) Quest File
//
//   Parameter:
//     player       - (eScript) Player Character Object
//     minelocation - Aktueller Aufenthaltsort
//
//    Look at Shinigami's SetQuests - TextCommand
//
/////////////////////////////////////////////////////////
Function SayTownQuest(player, minelocation)
  // "Jhe'lom Nord" -> "JhelomNord" etc.
  minelocation := CleanText(minelocation, array{"'", " "});
  Var QuestFilePath:=Lower("speech/location/"+minelocation+"/quest");
  Var QuestFile;
  If (FileExists("::config/"+QuestFilePath+".cfg"))
    QuestFile:=ReadConfigFile(QuestFilePath);
  Else
    QuestFilePath:="speech/location/sonstwo/quest";
    If (FileExists("::config/"+QuestFilePath+".cfg"))
      QuestFile:=ReadConfigFile(QuestFilePath);
    EndIf
  EndIf

  If (QuestFile)
    Var QuestInfo:=FindConfigElem(QuestFile, "Town");
    If (QuestInfo)
      Var GlobalProperty:=UnComment(GetConfigString(QuestInfo, "GlobalProperty"));
      If (GlobalProperty)
        Var ActiveQuest:=GetGlobalProperty(GlobalProperty);
        If (!ActiveQuest)
          ActiveQuest:="nil";
        EndIf

        If ((ActiveQuest<>"") And (ActiveQuest<>"nil"))
          Var QuestElem:=FindConfigElem(QuestFile, ActiveQuest);
          If (QuestElem)
            Var Chance:=GetConfigInt(QuestElem, "Chance");
            If (!Chance)
              Chance:=100;
            EndIf

            If (RandomInt(100)+1<=Chance)
              TurnToward(player);

              Var Elem:=GetConfigStringArray(QuestElem, "Elem");
              Var ElemL:=GetConfigStringArray(QuestElem, "ElemL");
              If (Len(Elem)+Len(ElemL)>0)
                Var Nummer:=RandomInt(Len(Elem)+Len(ElemL))+1;
                If (Nummer<=Len(Elem))
                  SayPergon(player, UnComment(Elem[Nummer]));
                  Sleep(3);
                Else
                  Var QuestElemLines:=FindConfigElem(QuestFile,
                    UnComment(ElemL[Nummer-Len(Elem)]));
                  If (QuestElemLines)
                    ForEach Line in GetConfigStringArray(QuestElemLines, "Line")
                      SayPergon(player, Line);
                      Sleep(2);
                    EndForeach
                    Sleep(1);
                  EndIf
                EndIf
              EndIf
            EndIf
          EndIf
        EndIf
      EndIf
    EndIf
    UnloadConfigFile(QuestFilePath);
  EndIf
EndFunction

//////////////////////////////////////////////////////
//  Answering - Find a random Answer for the NPC
//
//  Looks through four Files:
//      - Area Profession File
//      - Global Profession File
//      - Area File
//      - Global File
//  Finds answer from the first two files,
//  or on failure from third. On another
//  failure tries the last. If that fails again,
//  uses predefined list of dummy answers.
//////////////////////////////////////////////////////
Function Answering(sayed, minelocation, me, template_override := "")
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
  var sayedwords := SplitWords(CleanText(sayed));
  // foreach word in (sayedwords)
  //   syslog("NPC hoerte: "+word);
  // endforeach

  var answer_arr := array{};

  // "Jhe'lom Nord" -> "JhelomNord" etc.
  minelocation := CleanText(minelocation, array{"'", " "});

  // lokale typspezifische Antworten finden
  var local_type := AnswerFromFile(
    Lower("speech/location/"+minelocation+"/"+npctemplate), sayedwords
  );
  If (local_type)
    answer_arr.append(local_type);
  EndIf

  // globale typspezifische Antworten finden
  var global_type :=
    AnswerFromFile(Lower("speech/location/"+npctemplate), sayedwords);
  If (global_type)
    answer_arr.append(global_type);
  EndIf

  // Antwort auswaehlen
  If (answer_arr.size() > 0)
    var selected := answer_arr[RandomInt(Len(answer_arr))+1];
    return selected;
  EndIf

  // Fallback auf allgemeine lokale Sachen
  var local_all :=
    AnswerFromFile(Lower("speech/location/"+minelocation+"/all"), sayedwords);
  If (local_all)
    return local_all;
  EndIf

  // Fallback auf globale Sachen
  var global_all :=
    AnswerFromFile("speech/global", sayedwords);
  If (global_all)
    return global_all;
  EndIf

  // Fallback, letzter Versuch
  var dummy := GetDummyAnswer();
  Return dummy;
EndFunction

// Zufaellige Antwort aus SpeechFile holen
Function AnswerFromFile(SpeechFilePath, words)
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
EndFunction

// Zufaellige Dummy-Antwort
Function GetDummyAnswer()
  Case (RandomInt(11))
    0:  return "Dazu weiss ich leider nichts.";
    1:  return "Davon habe ich keine Ahnung.";
    2:  return "Lasst mich in Ruhe.";
    3:  return "Habe gerade keine Zeit.";
    4:  return "Hmmm ... aha";
    5:  return "Jaja, es ist gerade "+GetPergonTimeOfDay();
    6:  return "Wie meint Ihr?";
    7:  return "Das verstehe ich nicht.";
    8:  return "Davon weiss ich nichts.";
    9: return "Ich weiss nichts darueber.";
    10: return "Wenn Ihr meint.";
  EndCase
EndFunction

// Entfernt Satzzeichen und so Kram
Function CleanText(sometext, what := "")
  If (what == "")
    what := array{".", ",", "?", "!"};
  EndIf
  sometext:=CStr(sometext);
  If (Len(sometext)>0)
    Foreach char in (what)
      sometext:=StrReplace(sometext,char,"");
      SleepMS(2);
    EndForEach
  EndIf

  return sometext;
EndFunction

////////////////////////////////////////////
// Entfernt den Kommentar aus einem String
////////////////////////////////////////////
Function UnComment(ConfigString)
  If (ConfigString)
    Var Pos:=Find(ConfigString, "//", 1);
    If (Pos<>0)
      ConfigString:=ConfigString[1, Pos-1];

      If (Len(ConfigString)>0)
        ConfigString:=Trim(ConfigString,TRIM_RIGHT);
      EndIf
    EndIf
  EndIf

  Return (ConfigString);
EndFunction

// Function TimeZone(x, y)
//  var timezonecfg := ReadConfigFile("timezone");
//  If(!timezonecfg)
//    syslog("cant find timezone.cfg" + timezonecfg.errortext);
//    return;
//  EndIf
//  var timezoneelem := FindConfigElem(timezonecfg , 1);
//    If(!timezoneelem)
//      syslog("bad error in timezone.cfg");
//    Else
//      var timezonearea := GetConfigString(timezoneelem , "range");
//      var areasplit := SplitWords(timezonearea);
//      If(x >= cInt(areasplit[1]) and x <= cInt(areasplit[3]) and y >= cInt(areasplit[2]) and y <= cInt(areasplit[4]))
//        unloadconfigfile("timezone");
// 2 ???       return(2);
//      Else
//        timezoneelem := FindConfigElem(timezonecfg, 2);
//        If (!timezoneelem)
//          syslog ("bad error in timezone.cfg");
//        Else
//          timezonearea := GetConfigString(timezoneelem , "range");
//          areasplit := SplitWords(timezonearea);
//          If(x >= cInt(areasplit[1]) and x <= cInt(areasplit[3]) and y >= cInt(areasplit[2]) and y <= cInt(areasplit[4]))
//            unloadconfigfile("timezone");
// 1 ???           return(1);
//        EndIf
//      EndIf
//      EndIf
//    EndIf
// EndFunction

Function Run(opponent, Range)
    TurnAwayFrom(opponent);

    DisableEvents(SYSEVENT_ENTEREDAREA + SYSEVENT_LEFTAREA);
    SetOpponent(opponent);

    Var waittime;
    Var tobreak:=0;

    while ((opponent) && (!opponent.dead) && (!tobreak))
        SleepMS(2);
        waittime := OpenDistance( opponent ) * 15;
        If (waittime>750) // mehr als 50 Felder ->auch Abbruch
          Break;
        EndIf
        Var ev := Wait_for_EventLogging( waittime );
        // print( "event type: " + ev.type );
        case (ev.type)
//            SYSEVENT_SPEECH:

            SYSEVENT_DISENGAGED:
                if (ev.source.serial == opponent.serial)
                    SayPergon(ev.source, "Lasset uns Frieden halten!" );
                    tobreak:=1;
                endif

            SYSEVENT_ENGAGED:
            SYSEVENT_DAMAGED:
                if (ev.source)
                    if (ev.source.serial != opponent.serial)
                        SayPergon(ev.source, "Ihr auch noch?" );
                    else
                        if (ev.type == "damage")
                          SayPergon(ev.source, "Aaaaaargh, das tut doch weh. Hilfe!!!");
                            //print( "I just took " + ev.damage + " points of damage." );
                        endif
                    endif
                endif

//            SYSEVENT_OPPONENT_MOVED:
//
//
//            SYSEVENT_ENTEREDAREA:
//            SYSEVENT_LEFTAREA:
                // ignore both of these
        endcase
    endwhile

    EnableEvents(SYSEVENT_ENTEREDAREA+SYSEVENT_LEFTAREA, Range);

    SetWarMode(0);
EndFunction


/////////////////////////////////////////////////////////////
// CreateNpcFromTemplatePergon - beruecksichtigt GenderWarp
/////////////////////////////////////////////////////////////
Function CreateNpcFromTemplatePergon(template, x, y, z, warp := 0, override_properties := 0, use_custom_templates:=0, realm:=_DEFAULT_REALM)
// warp == 0  - normales CreateNpcFromTemplate
// warp == 1  - Gender vom NPC wird auf 1 gesetzt
// warp == 2  - Gender vom NPC wird zufaellig auf 0 oder 1 gesetzt

  If (use_custom_templates)
    Var datafile:=OpenDataFile("::customnpcs");
    If (datafile)
      var elem:=datafile.FindElement(template);
      If (elem)
        var data:=elem.getprop("data");
        Var orig_template:= GetNPCConfig(data.origtemplate);
        If (orig_template)
          If (!override_properties)
            override_properties:=struct;
          EndIf
          If (!override_properties.exists("CProps"))
            override_properties.+CProps:=dictionary;
          EndIf
          override_properties.+ar:=data.ar;
          override_properties.+color:=data.color;
          override_properties.+truecolor:=data.color;
          override_properties.+graphic:=CInt(data.graphic);
          override_properties.+trueobjtype:=CInt(data.graphic);
          override_properties.+name:=data.npcname;
          If (data.movemode)
            override_properties.+movemode:=data.movemode;
          EndIf
          override_properties.CProps.insert("myname",data.npcname);// Ersatz für SetMobileNamePergon
          override_properties.CProps.insert("custom_template",template);
          If (data.nocarve)
              override_properties.CProps.insert("nocarve",data.nocarve);
          EndIf
          If (data.nocorpse)
            override_properties.CProps.insert("nocorpse",data.nocorpse);
          EndIf
          If (data.noloot)
            override_properties.CProps.insert("noloot",data.noloot);
          EndIf
          override_properties.+RunSpeed:=CInt(data.runspeed);

          override_properties.+Strength:=data.str;
          override_properties.+Intelligence:=data.int;
          override_properties.+Dexterity:=data.dex;
          override_properties.CProps.insert("OverrideHits",data.maxhp);
          override_properties.CProps.insert("OverrideMana",data.maxmana);
          override_properties.CProps.insert("OverrideStamina",data.maxstam);
          template:=data.origtemplate; // für create umbiegen
        EndIf
      EndIf
    EndIf
  EndIf

  If (warp)
    If (warp==1)
      If (!override_properties.exists("Gender"))
        override_properties.+Gender;
      EndIf
      override_properties.Gender:=warp;
    Else  //==2
      Var GWarp := GetNPCConfig(template).genderwarp;

      If (GWarp)
        If (!override_properties.exists("Gender"))
          override_properties.+Gender;
        EndIf
        override_properties.Gender := RandomInt(2);
      EndIf
    EndIf
  EndIf

  Return(CreateNpcFromTemplate(template, x, y, z, override_properties, realm));
EndFunction

//////////////////////////////////////////////////////////////////////////
// ParseTemplateName - Splittet .npctemplate in pkg und templatename auf
//////////////////////////////////////////////////////////////////////////
Function ParseTemplateName(template_name)
  If ( template_name.IsA(POLCLASS_NPC) )
    template_name := template_name.npctemplate;
  EndIf

  var colon_a := Find(template_name, ":", 1);
  var colon_b := Find(template_name, ":", 2);

  var parsed := struct{"package", "template"};

  If ( !colon_a || !colon_b )
    // Keine : pkg Definition also /config nehmen
    parsed.package := "";
    parsed.template := template_name;
  Else
    parsed.package := template_name[colon_a+1, colon_b-2];
    parsed.template := template_name[colon_b+1, Len(template_name)];
  EndIf

  Return (parsed);
EndFunction

////////////////////////////////////////////////////////////////////////////
// GetNPCConfig - liest passende npcdesc aus und liefert Template Eintrag
////////////////////////////////////////////////////////////////////////////
Function GetNPCConfig(template)
  If (!template.package)
    template := ParseTemplateName(template);
  EndIf

  var npc_cfg := ReadConfigFile(":"+template.package+":npcdesc");
  If ( npc_cfg.errortext )
    Return (error{"errortext":="Error::GetNPCConfig() - Could not open :"+template.package+":npcdesc ->"+npc_cfg.errortext});
  EndIf

  var cfg_elem := npc_cfg[template.template];
  If ( cfg_elem.errortext )
    Return (error{"errortext":="Error::GetNPCConfig() - Could not find elem ["+template.template+"] ->"+cfg_elem.errortext});
  EndIf

  Return (cfg_elem);
EndFunction

// bestimmte Grafiken bringen den Client zum Absturz
Function IsValidNPCGraphic(graphic)
  If (!CInt(graphic) or CInt(graphic)<0)
    // "0" and negative numbers and non-numbers are not valid by default
    var res := error;
    res.errortext := "Not a valid graphic number";
    return res;
  EndIf

  var cfgfile := ReadConfigFile("::npcgraphics");
  If (!cfgfile)
    // fuer brauchbare Fehlermeldungen ohne Aufwand nehme man die
    // Standard-Routinen
    return cfgfile;
  EndIf

  var key := "npc"+CInt(graphic)+"graph";
  var elem := FindConfigElem(cfgfile, key);
  If (!elem)
    return elem;
  EndIf

  var res := GetConfigInt(elem, "allowed");
  If (!res)
    return res;
  EndIf

  // true und eventuell irgendwie hilfreich
  return CInt(graphic);
EndFunction

// automatisch Gespraechspartner waehlen
Function AutoTalkTo(byref me, byref medata, who := 0)
  If (medata.talktome)
    // keine laufenden Gespraeche abbrechen
    // syslog(me.name+" quatscht schon mit "+medata.talktome.name);
    If (medata.talktome.getprop("talkto") <> medata.minename)
      // redet inzwischen mit einem anderen
      // syslog(me.name+" stellt Gespraech ein mit "+medata.talktome.name);
      medata.talktome := 0;
    EndIf
    return;
  EndIf

  // etwas verteilen, damit nicht alle gleichzeitig suchen
  // (will es nicht critical machen)
  SleepMs(RandomInt(500));

  // damit Leute bei allen interessanten Events erfasst werden
  var range := 1 + Max(array{
      CInt(meprototype.threshold_enter),
      CInt(meprototype.threshold_leave),
      CInt(meprototype.threshold_speech)
  });

  // meine Umgebung absuchen, wer da so rumrennt
  ForEach mob in (ListMobilesNearLocationEx(
      me.x, me.y, me.z, range, LISTEX_FLAG_NORMAL, me.realm
  ))
    If (mob.isa(POLCLASS_NPC))
      // mit NPCs rede ich nicht
      continue;
    EndIf

    If (mob.getprop("talkto"))
      // nicht ins Gespraech einmischen
      // syslog(mob.name+" redet schon");
      continue;
    EndIf

    // ist ein Player
    If (who and mob <> who)
      // soll jemand bestimmten ansprechen, hab aber jemand anderen
      // syslog(mob.name+" ist der Falsche");
      continue;
    EndIf

    // der Gespraechskandidat ist derzeit in der Naehe
    // syslog(me.name+" waehlt "+mob.name);
    SetObjProperty(who, "talkto", medata.minename);
    medata.talktome := who;
    break;
  EndForEach

  // syslog(me.name+" hat niemanden zum Quatschen");
EndFunction

// Stellt die Sprache des NPCs dar
// (fuer alte NPCs auf Basis merchant.inc/nonmerchant.inc)
Function NPCSayTextTraditional(npc, text, texteng, range := 0)
  If (!range)
    range := 2*meprototype.threshold_speech;
  EndIf
  ForEach mobile in (ListMobilesNearLocationEx(
      npc.x, npc.y, npc.z, range,
      LISTEX_FLAG_NORMAL+LISTEX_FLAG_HIDDEN, npc.realm
  ))
    SleepMS(2);
    PrintTextAbovePrivatePergon(npc, text, texteng, mobile);
  EndForEach
EndFunction

// vim: sw=2 sts=2
