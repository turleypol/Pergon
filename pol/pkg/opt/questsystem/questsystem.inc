Enum TimeCheck
  TIME_CHECK:=1,
  TIME_ACCEPT,
  TIME_DECLINE,
  TIME_COMPLETE,
  TIME_REPEATABLE_SET,
  TIME_REPEATABLE_CHECK
EndEnum

/////////////////////////////////////////
// QuestAbbruch - Bricht Quest Nr X ab
/////////////////////////////////////////

Function QuestAbbruch(who,questid,flag,questtitle:="",char:=0,remote:=0)
  Var cprop, npc;

  If (!flag)
  	If (remote)
  		If (!QuestYesNo(char,questtitle))
        SendSysMessagePergon(char,"Abbruch", "Abort");
        Return;
      EndIf
    Else
      If (!QuestYesNo(who,questtitle))
        SendSysMessagePergon(who,"Abbruch", "Abort");
        Return;
      EndIf
    EndIf
  EndIf
  cprop:=who.getprop("quest");
  
  Var questcfg:=ReadConfigFile(":questsystem:quests");
  If (questcfg[questid].mostwanted)  // MostwantedQuest?
  	Var wanted:=who.getprop("quest_mostwanted");
  	If (wanted)
  		If (wanted<1)
  			wanted:=1;
  	 	EndIf
      who.setprop("quest_mostwanted",CInt(wanted)-1);
  	EndIf
  EndIf
  //timeout
  If (questcfg[questid].timeout)
  	Var time:=who.getprop("questtimeout");
  	If (!time)
  		time:=dictionary;
  	EndIf
  	Var questnpc:=ReadConfigFile(":questsystem:questnpc");
  	Var keys:=GetConfigStringKeys(questnpc);
  	ForEach key in (keys)
  		If (questid in (GetConfigStringArray(questnpc[key],"quest")))
  			time[CStr(key)]:=CInt(questcfg[questid].timeoutdecline)*60+ReadGameClock();
  			Break;
  		EndIf
  	EndForEach
    who.setprop("questtimeout",time);
	EndIf

  Case (cprop[CStr(questid)][1])
  	"escort":
  	  ForEach waypoint in (cprop[CStr(questid)][2])
  	  	waypoint := SystemFindObjectBySerial(waypoint);
  	  	If (waypoint)
  	  		If ((waypoint.isA(POLCLASS_MOBILE)) && (waypoint.npctemplate=="questwaypointki"))
  	  		  KillNPC(waypoint, "questsystem", KILLNPC_NOCORPSE);
  	  		EndIf
  	  	EndIf
  	  EndForEach
  	  npc:=SystemFindObjectBySerial(cprop[CStr(questid)][3]);
  	  If (npc)
  	  	If ((npc.isA(POLCLASS_MOBILE)) && (CInt(npc.getprop("quest"))==CInt(questid)))
  	  	  KillNPC(npc, "questsystem", KILLNPC_NOCORPSE);
  	  	EndIf
  	  EndIf
  	"kill":
  	"collectitem":
  	"npcitem":
  	"transport":
  	"carry":
	
  	default:
  EndCase
  cprop.erase(CStr(questid));
  who.setprop("quest",cprop);
  PlaySoundEffectPrivate(who,SFX_5B4,who);
EndFunction

Function QuestYesNo(who,questtitle)
	Var layout:={"nodispose",
	             "page 0",
	             "resizepic 0 0 9270 390 200",       //Hintergrund links
	             "gumppictiled 0 179 108 31 10462",  // unten links
	             "gumppic 108 179 10452",            // mitte
	             "gumppictiled 286 179 108 31 10462",// unten rechts
	             "gumppictiled 0 17 31 162 10464",   // linke seite
	             "gumppic 0 0 10301",                // oben links
	             "gumppictiled 17 0 362 17 10251",   // mitte oben
	             "gumppic 377 0 10303",              // oben rechts
	             "gumppictiled 363 17 31 162 10464", // rechte seite
  	           "htmlgump 48 60 300 60 0 0 0",      // Text
               "Button 97 120 1154 1155 1 0 1",    // OK
               "Button 272 120 1151 1152 1 0 0"};  // Cancel
  Var data:={"<basefont color=#FFFFFF><center>Wollt ihr das Quest '"+questtitle+"' wirklich abbrechen?</center>"};
  Return(SendDialogGump(who,layout,data,50,50).keys[2]);
EndFunction

Function timeout(who, npc, check, quest)
	var succedtime, questnpc;
	Set_Critical(1);
	var cprop:=who.getprop("questtimeout");
	If (!cprop)
		cprop:=dictionary;
	EndIf
	Case (check)
		TIME_CHECK:
    	  If (cprop[CStr(npc.getprop("questnpc"))])
      		If (cprop[CStr(npc.getprop("questnpc"))]>ReadGameClock())
		    		PrintTextAbovePergon(who,npc,"Ihr habt mich enttaeuscht. Lasst ersteinmal Gras ueber die Sache wachsen.");
		    		Set_Critical(0);
		    		Return(1);
		    	EndIf
		    EndIf
		    
		TIME_ACCEPT:
		    If (quest.timeout)
        	succedtime:=SplitWords(quest.timeouttosucced);
          cprop[CStr(npc.getprop("questnpc"))]:=CInt(succedtime[1])*60+ReadGameClock(); 
          who.setprop("questtimeout",cprop);
        EndIf
        
		TIME_DECLINE:
		    If (quest.timeout)
          cprop[CStr(npc.getprop("questnpc"))]:=CInt(quest.timeoutdecline)*60+ReadGameClock();          	
          who.setprop("questtimeout",cprop);
		    EndIf
		    
		TIME_COMPLETE:
        If (quest.timeout)
        	If (npc.getprop("questnpc_"))
        		questnpc:=npc.getprop("questnpc_");
        	Else
        		questnpc:=npc.getprop("questnpc");
        	EndIf
        	If (cprop[CStr(questnpc)])
        		If (ReadGameClock()>cprop[CStr(questnpc)])
        			PrintTextAbovePergon(who,npc,quest.timeouttosuccedtext);
        			succedtime:=SplitWords(quest.timeouttosucced);
        			cprop[CStr(questnpc)]:=CInt(succedtime[2])*60+ReadGameClock();
        			who.setprop("questtimeout",cprop);
        			Set_Critical(0);
        			Return(1);
        		Else
        			cprop.erase(CStr(questnpc));
        			who.setprop("questtimeout",cprop);
        		EndIf
        	EndIf
        EndIf
        
    TIME_REPEATABLE_SET:
        cprop[CStr(npc)]:=CInt(quest.RepeatableTimeout)*60+ReadGameClock();
        who.setprop("questtimeout",cprop);
    
    TIME_REPEATABLE_CHECK:
        If (cprop[CStr(npc)])
          If (ReadGameClock()>cprop[CStr(npc)])
            cprop.erase(CStr(npc));
            who.setprop("questtimeout",cprop);
            Set_Critical(0);
            Return(1);
          EndIf
        Else
          Set_Critical(0);
          Return(1);
        EndIf
  EndCase
  Set_Critical(0);
EndFunction
