/////////////////////////////////////////
//
//   TextCMD - Inlude v1.0 (2000-07-21)
//
//     Author: Shinigami
//
/////////////////////////////////////////

/////////////////
// Bibliotheken
/////////////////

Use uo;
Use os;
Use cfgfile;

////////////////////////////////////////////////////////////
// CanExecuteCMD - Darf der Spieler das Commando benutzen?
////////////////////////////////////////////////////////////

Function CanExecuteCMD(who, TextCMD)
  Var result:=0;

  Var CMDFilePath:="textcmd";
  Var CMDFile:=ReadConfigFile(CMDFilePath);
  If (CMDFile)
    Var CMDInfo:=FindConfigElem(CMDFile, Hex(who.serial));
    If (!CMDInfo)
      CMDInfo:=FindConfigElem(CMDFile, GetCMDLevelTitle(who.cmdlevel)+"_Level");
    EndIf

    If (CMDInfo)
      ForEach textcmdsublists in GetConfigStringArray(CMDInfo, "Liste")
        Var textcmdsublist:=FindConfigElem(CMDFile, textcmdsublists);
        If (textcmdsublist)
          Case (GetConfigInt(textcmdsublist, TextCMD))
            1: result:=1; // Erlauben
               Break;
            0: Break;     // Verbieten
          EndCase
        EndIf
      EndForEach
    EndIf

    UnloadConfigFile(CMDFilePath);
  Else
    SysLog("CanExecuteCMD: Kann die Datei '"+CMDFilePath+".cfg' nicht oeffnen !");
  EndIf

  If (!result)
    SendSysMessagePergon(who, "Ihr habt nicht das noetige Recht um den Befehl zu nutzen!");
  EndIf

  Return (result);
EndFunction