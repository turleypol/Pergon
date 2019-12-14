/////////////////////////////////////////////////////////////////
// Berufe Include - Alles zum Thema Klasse und Berufe
//
// Author: Shinigami

/////////////////////////////////////////////////////////////////
// Initialisierung & Status
//   CheckBerufeConfig      - Prueft das ConfigFile des Berufesystems
//   Update_Title           - Aktualisiert die Bezeichnung des Spielers
//
// Verwaltung
//   KlassenBerufe_einlesen - Klassen und Berufe einlesen
//   KlassenSkills_einlesen - Klassen und Skills einlesen
//   KlasseBeruf_checken    - Klasse und Beruf ueberpruefen

include "include/logutil";

Const FILE_PROFESSIONS := ":attribute:berufe";
Const FILE_CHARSKILLS  := "::charskills";
Const FILE_VORGABEN    := "::skillvorgaben";

//////////////////////////////////
//                              //
//   Initialisierung & Status   //
//                              //
//////////////////////////////////

////////////////////////////////////////////////////////////////
// CheckBerufeConfig - Prueft das ConfigFile des Berufesystems
////////////////////////////////////////////////////////////////
Function CheckBerufeConfig()
  var professions := ReadConfigFile(FILE_PROFESSIONS);
  If (!professions)
    syslog(
      "Skills: Kann '"+FILE_PROFESSIONS+"' nicht lesen: "+
      professions.errortext
    );
    return professions;
  EndIf

  Var klassen := GetConfigStringArray(professions["Klassen"], "Klasse");
  If (klassen.size() == 3) // Konstanten gemaess 'Character Klassen' pruefen
    ForEach klasse in (array{KLASSE_HANDWERKER, KLASSE_KRIEGER, KLASSE_MAGIER})
      If (!klasse in (klassen))
        syslog("FEHLER: Klasse '"+klasse+"' fehlt in "+FILE_PROFESSIONS);
      EndIf
    EndForEach
  Else
    syslog(
      "FEHLER: Anzahl der Klassen falsch in "+FILE_PROFESSIONS+
      " ("+klassen.size()+")"
    );
  EndIf
  klassen.append(KLASSE_UNRELEVANT);

  // Skills der BerufsConfig gegen die Skills der SkillsConfig pruefen
  var skills := dictionary;
  ForEach klasse in klassen
    ForEach skill in GetConfigStringArray(professions[klasse], "Skill")
      var splitted := SplitWords(skill);
      var skillid  := GetSkillName(splitted[1]);
      If (skillid)
        skills[skillid] := 1;
      Else
        syslog(
          "FEHLER: Unbekannter Skill ("+skill+") in "+FILE_PROFESSIONS+
          ", Abschnitt "+klasse
        );
      EndIf
    EndForEach
  EndForEach

  // Skills der Skillsconfig gegen die Skills der Berufsconfig pruefen
  For skillid := 0 to MAX_SKILLS
    If (skills[GetSkillName(skillid)] <> 1)
      syslog(
        "FEHLER: Kein Skill-Name fuer Skill-ID "+skillid+" definiert"
      );
    EndIf
  EndFor

  UnloadConfigFile(FILE_PROFESSIONS);
EndFunction

/////////////////////////////////////////////////////////////
// Update_Title - Aktualisiert die Bezeichnung des Spielers
/////////////////////////////////////////////////////////////

// Function Update_Title(who, skill)
 // If (GetObjProperty(who, "noshow"))
 //   Var title;
 //
 //   Var level:=GetSkillFloatPergon(who, skill);
 //   If (level<30)
 //     title:="";
 //   ElseIf (level<40)
 //     title:="Neuling";
 //   ElseIf (level<50)
 //     title:="Lehrling";
 //   ElseIf (level<60)
 //     title:="Geselle";
 //   ElseIf (level<70)
 //     If (who.gender=0)
 //       title:="Meister";
 //     Else
 //       title:="Meisterin";
 //     EndIf
 //   ElseIf (level<80)
 //     If (who.gender=0)
 //       title:="Experte";
 //     Else
 //       title:="Expertin";
 //     EndIf
 //   ElseIf (level<90)
 //     title:="Adept";
 //   ElseIf (level<100)
 //     If (who.gender=0)
 //       title:="Lehrmeister";
 //     Else
 //       title:="Lehrmeisterin";
 //     EndIf
 //   Else
 //     If (who.gender=0)
 //       title:="Grossmeister";
 //     Else
 //       title:="Grossmeisterin";
 //     EndIf
 //   EndIf
 //
   // Var classfilepath:="classes";
   // Var classfile:=ReadConfigFile(classfilepath);
   // If (classfile)
   //   Var proftitle:=classfile[class].title;
   //   If (proftitle)
   //     SetNamePergon(who, GetObjProperty(who, "myname")+" ,"+title+" "+proftitle);
   //   Else
   //     SetNamePergon(who, GetObjProperty(who, "myname"));
   //   EndIf
   // EndIf
   // UnloadConfigFile(classfilepath);
 // EndIf
//  Return;
// EndFunction

////////////////////
//                //
//   Verwaltung   //
//                //
////////////////////

/////////////////////////////////////////////////////////
// KlassenBerufe_einlesen - Klassen und Berufe einlesen
/////////////////////////////////////////////////////////
Function KlassenBerufe_einlesen()
  var professions := ReadConfigFile(FILE_PROFESSIONS);
  If (!professions)
    syslog(
      "Skills: Kann '"+FILE_PROFESSIONS+"' nicht lesen: "+
      professions.errortext
    );
    return professions;
  EndIf

  var klassenberufe := dictionary;
  ForEach klasse in GetConfigStringArray(professions["Klassen"], "Klasse")
    var berufe := " ";
    ForEach beruf in GetConfigStringArray(professions[klasse], "Beruf")
      berufe += SplitWords(beruf)[2]+" ";
    EndForEach

    klassenberufe[klasse] := berufe;
  EndForEach

  UnloadConfigFile(FILE_PROFESSIONS);

  return klassenberufe;
EndFunction

/////////////////////////////////////////////////////////
// KlassenSkills_einlesen - Klassen und Skills einlesen
/////////////////////////////////////////////////////////
Function KlassenSkills_einlesen()
  var professions := ReadConfigFile(FILE_PROFESSIONS);
  If (!professions)
    syslog(
      "Skills: Kann '"+FILE_PROFESSIONS+"' nicht lesen: "+
      professions.errortext
    );
    return professions;
  EndIf

  var klassenskills := struct;
  klassenskills.+skills  := dictionary;

  klassenskills.+klassen :=
    GetConfigStringArray(professions["Klassen"], "Klasse")+
    array{KLASSE_UNRELEVANT};

  ForEach klasse in (klassenskills.klassen)
    ForEach skill in GetConfigStringArray(professions[klasse], "Skill")
      var splitted := SplitWords(skill);
      var skillid  := GetSkillName(splitted[1]);
      If (skillid)
        klassenskills.skills[skillid] := {CInt(splitted[2]), klasse};
      EndIf
    EndForEach
  EndForEach

  return klassenskills;
EndFunction

Function Berufskills_einlesen()
  var professions := ReadConfigFile(FILE_PROFESSIONS);
  If (!professions)
    syslog(
      "Skills: Kann '"+FILE_PROFESSIONS+"' nicht lesen: "+
      professions.errortext
    );
    return professions;
  EndIf

  var berufskills := dictionary;
  var klassen := GetConfigStringArray(professions["Klassen"], "Klasse");

  ForEach klasse in klassen
    ForEach beruf in GetConfigStringArray(professions[klasse], "Beruf")
      var splitted := SplitWords(beruf);
      berufskills[splitted[2]] := array{
        GetSkillName(splitted[3]), GetSkillName(splitted[4])
      };
    EndForEach
  EndForEach

  return berufskills;
EndFunction

Function Skillvorgabe_einlesen(beruf)
	var vorgaben := ReadConfigFile(FILE_VORGABEN);
	If (!vorgaben)
    syslog(
      "Skills: Kann '"+FILE_VORGABEN+"' nicht lesen: "+
      vorgaben.errortext
    );
    return vorgaben;
  EndIf

  var skillvorgabe := dictionary;
  skillvorgabe["hauptskills"]   := GetConfigStringArray(vorgaben[beruf], "HauptSkill");
  skillvorgabe["nebenskills"]   := GetConfigStringArray(vorgaben[beruf], "NebenSkill");
  skillvorgabe["weitereskills"] := GetConfigStringArray(vorgaben[beruf], "WeitereSkill");
  
  ForEach skilltype in {"hauptskills", "nebenskills", "weitereskills"}
  	ForEach skillid in (skillvorgabe[skilltype])
  		skillid := GetSkillName(skillid);
  	EndForEach
  EndForEach
  
  return skillvorgabe;
EndFunction

////////////////////////////////////////////////////////
// KlasseBeruf_checken - Klasse und Beruf ueberpruefen
////////////////////////////////////////////////////////
Function KlasseBeruf_checken(who, klassenberufe, logging := 1)
  var klasseberuf := struct;
  klasseberuf.+klasse := who.getprop(SKKLASSE);
  klasseberuf.+beruf  := who.getprop(SKBERUF);

  var char_age := ReadGameClock() - who.createdat;
  If (!klasseberuf.klasse)
    // Alter pruefen und erst nach zwei Wochen warnen
    If (char_age > 3600 * 24 * 14)
      syslog("HINWEIS: "+CharInfoStr(who)+" hat noch keine Klasse");
    EndIf
    return;
  EndIf
  If (!klasseberuf.beruf)
    If (char_age > 3600 * 24 * 14)
      syslog("HINWEIS: "+CharInfoStr(who)+" hat noch keinen Beruf");
    EndIf
    return;
  EndIf

  If (!(klasseberuf.klasse in (klassenberufe.keys())))
    If (logging)
      syslog(
        "WARNUNG: "+CharInfoStr(who)+" hat ungueltige Klasse '"+
        klasseberuf.klasse+"'"
      );
    EndIf
    return;
  EndIf

  If (
    klasseberuf.beruf == KLASSE_BERUF_NIX or
    klassenberufe[klasseberuf.klasse][" "+klasseberuf.beruf+" "]
  )
    // Beruf bekannt
    return klasseberuf;
  EndIf

  If (logging)
    syslog(
      "WARNUNG: '"+CharInfoStr(who)+" hat ungueltigen Beruf '"+
      klasseberuf.beruf+"'"
    );
  EndIf
EndFunction
