/////////////////////////////////////////////////////////////////
//
//   Class_Ckills Include - Let you choose your primarey Class
//
//     based on Class/Racc System
//       Author: Dream Weaver
//
//       Thanks to Racalac and Sigismund for scripting support.
//
//     Author: Shinigami
//
//   Modifications:
//     2000/04/30 Shinigami: Complete ReWrite
//
/////////////////////////////////////////////////////////////////
//
//   classskill.inc & selectskill.inc might be the same
//
/////////////////////////////////////////////////////////////////

/////////////////
// Bibliotheken
/////////////////

Use uo;
Use os;
Use cfgfile;

///////////////
// Funktionen
///////////////

Function Class_Skills(who)
  Var reskey:=-1;

  Var layout:={
    "page 0",
    "noclose",
    "nodispose",
    "gumppic 0 0 2620",
    "gumppic 20 0 2621",
    "gumppic 290 0 2622",
    "gumppic 0 18 2623",
    "gumppic 22 18 2624",
    "gumppic 288 18 2625",
    "gumppic 0 222 2623",
    "gumppic 22 222 2624",
    "gumppic 288 222 2625",
    "gumppic 0 426 2626",
    "gumppic 20 426 2627",
    "gumppic 290 426 2628",
    "button 215 410 2130 2129 3 0 1",
    "text 20 10 40 0"
  };
  Var data:={"Waehle deinen primaeren Skill aus"};

  Var classfilepath:="classes";
  Var classfile:=ReadConfigFile(classfilepath);
  If (classfile)
    Var hisclass:=Lower(GetObjProperty(who, SKBERUF));
    If (!hisclass)
      hisclass:=KLASSE_BERUF_NIX;
    EndIf

    Var y:=50;
    Var on:=1;
    Var skill;
    ForEach skill in GetConfigStringArray(classfile[hisclass], "Primary")
      data.append(GetSkillInfo(skill).TrainName);
      layout.append("text 30 "+y+" 40 "+Len(data));
      layout.append("radio 290 "+(y+8)+" 2362 2361 "+on+" "+(CInt(skill)+1000));
      y+=20;on:=0;
    EndForEach

    Var res:=SendDialogGump(who, layout, data);
    If (res[0])
      Var key;
      ForEach key in (res.keys)
        If (key>=1000)
          reskey:=key;
        EndIf
      EndForEach
      reskey-=1000;
    Else
      SendSysMessagePergon(who, "Irgendwas ging schief!");
    EndIf
  EndIf
  UnloadConfigFile(classfilepath);
EndFunction
