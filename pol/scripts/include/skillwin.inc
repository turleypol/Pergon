/////////////////////////////////////////////////////////////////////
//   SkillWin Script - Shows the Skill Window for a Player
//
//     based on Class/Racc System
//       Author: Dream Weaver
//
//       Thanks to Racalac and Sigismund for scripting support.
//
//     Author: Shinigami

include ":drinking:common";
include ":hungrysystem:hungrysystem";
include "include/experience";
include "include/gumputil";
include "include/modifyskill";
include "include/packets";

//////////////////////
// Globale Variablen
//////////////////////

Var layout:={"page 0", "nodispose"};
Var data:={};

Var pagetitles:={};
Var skillsinfo:={};
Var titles:={};
Var statbuttons;

///////////////
// Konstanten
///////////////

Const MAX_LINES:=27;       // Wieveiel Zeilen sollen ueberhaupt angezeigt werden?
Const RESERVED:=3;         // Wieviele Zeilen benoetigen Hunger- und Durstlevel?
Var   statcap_lines:=3;    // Wieviele zusaetzliche Zeilen sind noetig fuers StatCap?
Var   statcap_reserved:=4; // Wieviele Data-Eintraege sind noetig fuers StatCap?
Var   experience_lines:=2; // Wieviel Zeilen für ExperienceBalken

Const BUTTON_STRENGTH    :=1001;
Const BUTTON_DEXTERITY   :=1002;
Const BUTTON_INTELLIGENCE:=1003;

//////////////////
// Hauptfunktion
//////////////////

Function SkillWin(who, observer)
  Set_Script_Option(SCRIPTOPT_NO_RUNAWAY, 1);

  If (GetObjProperty(who, "#NoSkillWin")) // Verhindern, dass das SkillWin direkt nach dem Einloggen angezeigt wird
    EraseObjProperty(who, "#NoSkillWin");
    Return;
  EndIf

  // QuestChars und CMDLeveln keine StatCap-Auswahl anzeigen
  If (who.cmdlevel >= CMDLEVEL_QUESTCHAR)
    statcap_lines:=0;
    statcap_reserved:=0;
    experience_lines:=0;
  EndIf

  skillsinfo:=GetSkillsSorted();
  titles:=GetTitles();

  Var berufskills:={}; // Character nach Skills abgrasen
  Var hauptskills:={};
  Var nebenskills:={};
  Var weitereskills:={};
  Var result;

  If (who.cmdlevel>=CMDLEVEL_SEER) // Bei CMDLevel werden alle Skills angezeigt
    For index:=1 To MAX_SKILLS+1
      berufskills.append(index);
    EndFor

    AddPages("Alle Skills", berufskills.size());                 // SkillWindow-Kopf zusammenbasteln
    AddCommon(who, MAX_LINES-statcap_lines-experience_lines, pagetitles.size());  // Allgemeine Infos einfuegen
    AddStatCap(who, MAX_LINES-statcap_lines-experience_lines, pagetitles.size()); // Auswahloptionen fuers StatCap einfuegen
    AddExperiencePage(who,MAX_LINES-statcap_lines-experience_lines,pagetitles.size());  // Balken der Experience anzeigen

    For index:=1 To pagetitles.size()
      layout.append("button 30 "+((index*15)+39)+" 2118 2117 0 "+index);
      data.append(pagetitles[index]);
    EndFor

    AddSkills(who, 0, MAX_LINES-statcap_lines-experience_lines, berufskills, pagetitles.size()); // SkillWindow-Rumpf zusammenbasteln
  Else // ...bei Spielern nur die, die sie auch haben
    For index:=1 To MAX_SKILLS+1
      Case (GetObjProperty(who, SKILLTYPKENNUNG+skillsinfo[index]))
        SKILLTYP_BERUF:   berufskills.append(index);
        SKILLTYP_HAUPT:   hauptskills.append(index);
        SKILLTYP_NEBEN:   nebenskills.append(index);
        SKILLTYP_WEITERE: weitereskills.append(index);
      EndCase
    EndFor

    Var maxlines:=0; // Hoehe des Gumps setzen

    If (berufskills.size())
      maxlines:=Max(maxlines,(berufskills.size()+1+hauptskills.size()));
    Else
      maxlines:=Max(maxlines, hauptskills.size());
    EndIf
    maxlines:=Max(maxlines, nebenskills.size());
    maxlines:=Max(maxlines, weitereskills.size());

    If (berufskills.size()) // SkillWindow-Kopf zusammenbasteln
      AddPages("Hauptskills", berufskills.size()+1+hauptskills.size());
    Else
      AddPages("Hauptskills", hauptskills.size());
    EndIf
    AddPages("Nebenskills", nebenskills.size());
    AddPages("Weitere Skills", weitereskills.size());

    AddCommon(who, maxlines, pagetitles.size());  // Allgemeine Infos einfuegen
    AddStatCap(who, maxlines, pagetitles.size()); // Auswahloptionen fuers StatCap einfuegen
    AddExperiencePage(who,maxlines,pagetitles.size());  // Balken der Experience anzeigen

    For index:=1 To pagetitles.size()
      layout.append("button 30 "+((index*15)+39)+" 2118 2117 0 "+index);
      data.append(pagetitles[index]);
    EndFor

    Var page:=0; // SkillWindow-Rumpf zusammenbasteln
    If (berufskills.size())
      Var lines:=AddSkills(who, page, MAX_LINES-statcap_lines-experience_lines, berufskills, pagetitles.size());
      If (lines)
        lines+=1;
      EndIf
      AddSkills(who, page, lines, hauptskills, pagetitles.size());
    Else
      AddSkills(who, page, MAX_LINES-statcap_lines-experience_lines, hauptskills, pagetitles.size());
    EndIf
    AddSkills(who, page, MAX_LINES-statcap_lines-experience_lines, nebenskills, pagetitles.size());
    AddSkills(who, page, MAX_LINES-statcap_lines-experience_lines, weitereskills, pagetitles.size());
  EndIf

  Do
    result:=CInt(SendDialogGump(observer, layout, data)[0]); // SkillWindow aufrufen
    If (result>0)
      Detach(); // Hmm, eigentlich nicht noetig, weil der es ja sofort wieder beendet...
      if (who.serial == observer.serial) // damit die Götter da nichts verstellen können
        Case (result)
          BUTTON_STRENGTH:     ChangeStatLock(who,ATTRIBUTEID_STRENGTH);
          BUTTON_DEXTERITY:    ChangeStatLock(who,ATTRIBUTEID_DEXTERITY);
          BUTTON_INTELLIGENCE: ChangeStatLock(who,ATTRIBUTEID_INTELLIGENCE);

          default:             Start_ScriptPergon(GetSkillInfo(skillsinfo[result-1]).Script, who);
        EndCase
      else
        SendSysMessagePergon(observer, "Nanana. Welcher Gott wuerde denn sowas machen?");
      endif
    EndIf

    IF (result == BUTTON_STRENGTH || result == BUTTON_DEXTERITY || result == BUTTON_INTELLIGENCE)
      Case (GetAttributeLock(who,ATTRIBUTEID_STRENGTH))
        ATTRIBUTE_LOCK_UP:     layout[statbuttons]:="button 38 "+layout[statbuttons][11,3]+" 2435 2436 1 0 "+BUTTON_STRENGTH;
        ATTRIBUTE_LOCK_DOWN:   layout[statbuttons]:="button 38 "+layout[statbuttons][11,3]+" 2437 2438 1 0 "+BUTTON_STRENGTH;
        ATTRIBUTE_LOCK_LOCKED: layout[statbuttons]:="button 38 "+layout[statbuttons][11,3]+" 2092 2092 1 0 "+BUTTON_STRENGTH;
      EndCase
      Case (GetAttributeLock(who,ATTRIBUTEID_DEXTERITY))
        ATTRIBUTE_LOCK_UP:     layout[statbuttons+1]:="button 128 "+layout[statbuttons+1][12,3]+" 2435 2436 1 0 "+BUTTON_DEXTERITY;
        ATTRIBUTE_LOCK_DOWN:   layout[statbuttons+1]:="button 128 "+layout[statbuttons+1][12,3]+" 2437 2438 1 0 "+BUTTON_DEXTERITY;
        ATTRIBUTE_LOCK_LOCKED: layout[statbuttons+1]:="button 128 "+layout[statbuttons+1][12,3]+" 2092 2092 1 0 "+BUTTON_DEXTERITY;
      EndCase
      Case (GetAttributeLock(who,ATTRIBUTEID_INTELLIGENCE))
        ATTRIBUTE_LOCK_UP:     layout[statbuttons+2]:="button 258 "+layout[statbuttons+2][12,3]+" 2435 2436 1 0 "+BUTTON_INTELLIGENCE;
        ATTRIBUTE_LOCK_DOWN:   layout[statbuttons+2]:="button 258 "+layout[statbuttons+2][12,3]+" 2437 2438 1 0 "+BUTTON_INTELLIGENCE;
        ATTRIBUTE_LOCK_LOCKED: layout[statbuttons+2]:="button 258 "+layout[statbuttons+2][12,3]+" 2092 2092 1 0 "+BUTTON_INTELLIGENCE;
      EndCase
    EndIF

  DoWhile(result == BUTTON_STRENGTH || result == BUTTON_DEXTERITY || result == BUTTON_INTELLIGENCE);
EndFunction

Function ChangeStatLock(who,new)
  var state:=GetAttributeLock(who,new);
  state+=1;
  If (state>ATTRIBUTE_LOCK_LOCKED)
    state:=ATTRIBUTE_LOCK_UP;
  EndIf
  SetAttributeLock(who,new,state);
EndFunction

///////////////////////////////////////////
// AddCommon - Fuegt allgemeine Infos ein
///////////////////////////////////////////

Function AddCommon(who, maxlines, pages)
  layout.append("resizepic 10 10 3500 355 "+(((maxlines+statcap_lines+pages+experience_lines)*15)+141));
  layout.append("gumppic 50 30 2100");

  Var klasse:=GetObjProperty(who, SKKLASSE); // Klasse und Beruf eintragen
  If (!klasse)
    klasse:="";
  EndIf

  Var beruf:=GetObjProperty(who, SKBERUF);
  If ((!beruf) Or (beruf==KLASSE_BERUF_NIX))
    beruf:="";
  EndIf

  layout.append("text 230 26 50 0");
  layout.append("text 230 50 50 1");
  data.append(klasse);
  data.append(beruf);

  Var drinkmsg:=GetDrinkingMsg(CInt(GetObjProperty(who, PROP_THIRST)));
  Var hungrymsg:=GetHungryMsg(CInt(GetObjProperty(who,  PROP_HUNGER)));

  layout.append("htmlgump 30 "+(((maxlines+statcap_lines+pages+experience_lines)*15)+87)+" 315 45 2 1 0"); // Hunger- und Durstlevel eintragen
  data.append(GetMessageText(who, hungrymsg[1], hungrymsg[2])+" "+GetMessageText(who, drinkmsg[1], drinkmsg[2]));
EndFunction

/////////////////////////////////////////////////////////
// AddStatCap - Fuegt Auswahloptionen fuers StatCap ein
/////////////////////////////////////////////////////////

Function AddStatCap(who, maxlines, pages)
  If (statcap_lines) // Ueberhaupt was anzeigen?
    Var y_pos:=((maxlines+pages)*15)+86;
    layout.append("resizepic 30 "+y_pos+" 3000 315 45");
    layout.append("text 34 "+(y_pos+2)+" 0 3");
    data.append("Wie sollen sich die Stats verhalten?");

    y_pos+=22;
    layout.append("text 54 "+y_pos+" 0 4");
    layout.append("text 144 "+y_pos+" 0 5");
    layout.append("text 274 "+y_pos+" 0 6");
    data.append(ATTRIBUTEID_STRENGTH_DESC);
    data.append(ATTRIBUTEID_DEXTERITY_DESC);
    data.append(ATTRIBUTEID_INTELLIGENCE_DESC);

    y_pos+=4; // Die Buttons sitzen ein bisschen tiefer
    Case (GetAttributeLock(who,ATTRIBUTEID_STRENGTH))
      ATTRIBUTE_LOCK_UP:     layout.append("button 38 "+y_pos+" 2435 2436 1 0 "+BUTTON_STRENGTH);
      ATTRIBUTE_LOCK_DOWN:   layout.append("button 38 "+y_pos+" 2437 2438 1 0 "+BUTTON_STRENGTH);
      ATTRIBUTE_LOCK_LOCKED: layout.append("button 38 "+y_pos+" 2092 2092 1 0 "+BUTTON_STRENGTH);
    EndCase
    statbuttons:=layout.size();
    Case (GetAttributeLock(who,ATTRIBUTEID_DEXTERITY))
      ATTRIBUTE_LOCK_UP:     layout.append("button 128 "+y_pos+" 2435 2436 1 0 "+BUTTON_DEXTERITY);
      ATTRIBUTE_LOCK_DOWN:   layout.append("button 128 "+y_pos+" 2437 2438 1 0 "+BUTTON_DEXTERITY);
      ATTRIBUTE_LOCK_LOCKED: layout.append("button 128 "+y_pos+" 2092 2092 1 0 "+BUTTON_DEXTERITY);
    EndCase
    Case (GetAttributeLock(who,ATTRIBUTEID_INTELLIGENCE))
      ATTRIBUTE_LOCK_UP:     layout.append("button 258 "+y_pos+" 2435 2436 1 0 "+BUTTON_INTELLIGENCE);
      ATTRIBUTE_LOCK_DOWN:   layout.append("button 258 "+y_pos+" 2437 2438 1 0 "+BUTTON_INTELLIGENCE);
      ATTRIBUTE_LOCK_LOCKED: layout.append("button 258 "+y_pos+" 2092 2092 1 0 "+BUTTON_INTELLIGENCE);
    EndCase
  EndIf
EndFunction

/////////////////////////////////////////////////////////
// AddExperiencePage - Fuegt Experience Balken ein
/////////////////////////////////////////////////////////

Function AddExperiencePage(who, maxlines, pages)
  If (experience_lines) // Ueberhaupt was anzeigen?
    Var y_pos:=((maxlines+pages+statcap_lines)*15)+88;
    layout.append("resizepic 30 "+y_pos+" 3000 315 25");
    y_pos+=7;
    layout.append("gumppic 40 "+y_pos+" 50 hue=910");  // Hintergrundbalken
    layout.append("gumppic 184 "+y_pos+" 50 hue=910");

    var DiffExpOfRank;
    var exp:=CInt(GetObjProperty(who, EXPERIENCE));
    var rank:=GetRank(who);
    If (rank==0)
        DiffExpOfRank:=0;
    Else
      DiffExpOfRank:=Pow(2,rank-1)*2000; // Wieviel exp entspricht der Rank
    EndIf
    exp-=DiffExpOfRank;
    DiffExpOfRank:=Pow(2,rank)*2000-DiffExpOfRank; // Wieviel bis zum nächsten Rank
    var pixel:=288.0/DiffExpOfRank;
    layout.append("gumppictiled 40 "+y_pos+" "+(CInt(pixel*exp))+" 11 50");
  EndIf
EndFunction

//////////////////////////////////
// AddPages - Erzeugt die Seiten
//////////////////////////////////

Function AddPages(title, lines)
  Var pages:=CInt(lines/(MAX_LINES-statcap_lines-experience_lines));
  If (pages*(MAX_LINES-statcap_lines-experience_lines)<lines)
    pages+=1;
  EndIf

  If (pages<=1)
    pagetitles.append(title);
  Else
    For page:=1 To pages
      pagetitles.append(title+" "+page);
    EndFor
  EndIf
EndFunction

///////////////////////////////////////////
// AddSkills - Erzeugt die Skilleintraege
///////////////////////////////////////////

Function AddSkills(who, ByRef page, lines, skills, pages)
  Var skillinfo;
  Var titleid;
  Var y_pos;

  If (skills.size()) // Skills anzeigen
    For index:=1 To skills.size()
      If (lines==MAX_LINES-statcap_lines-experience_lines)
        lines:=0;
        AddSkillsPage(page);
      EndIf

      skillinfo:=GetSkillInfo(skillsinfo[skills[index]]);

      titleid:=GetTitleIDSkill(who, skillinfo.name);
      y_pos:=((lines+pages)*15)+65;
      If (skillinfo.Script)
        layout.append("button 30 "+(y_pos+4)+" 2118 2117 1 0 "+(skills[index]+1));
      EndIf

      layout.append("text 50 "+y_pos+" 0 "+data.size());
      data.append(skillinfo.TrainName);

      If (skillinfo.ID<>SKILLID_BASTELN)
        layout.append("text "+titles[2][titleid]+" "+y_pos+" 0 "+data.size());
        If (who.getprop("rpwanted"))
          data.append(titles[1][titleid]);
        Else
          data.append("("+RawSkillToBaseSkillPergon(GetRawSkillPergon(who, skillinfo.ID))/10.0+"%)");
        EndIf        
      EndIf

      lines+=1;
    EndFor
  Else // ...es gibt hier keine Skills
    If (lines==MAX_LINES-statcap_lines-experience_lines)
      lines:=0;
      AddSkillsPage(page);
    EndIf
  EndIf

  Return (lines);
EndFunction

////////////////////////////////////////////////////////////
// AddSkillsPage - Erzeugt den Seitenkopf einer Skillseite
////////////////////////////////////////////////////////////

Function AddSkillsPage(ByRef page)
  page+=1;
  layout.append("page "+page);

  For pagetitle:=1 To pagetitles.size()
    If (pagetitle==page)
      layout.append("text 50 "+((pagetitle*15)+35)+" 40 "+(pagetitle+RESERVED+statcap_reserved-1));
    Else
      layout.append("text 50 "+((pagetitle*15)+35)+" 0 "+(pagetitle+RESERVED+statcap_reserved-1));
    EndIf
  EndFor
EndFunction
