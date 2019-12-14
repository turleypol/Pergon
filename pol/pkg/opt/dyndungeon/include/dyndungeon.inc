include "include/varutil";

Enum GlobaleCprops
  CPROPDUNGEONID:="DynamicDungeonCounter",   //Letzte verwendete DungeonID
  CPROPDUNGEONS :="DynamicDungeonDictionary",//Zuordnung DungeonID -> {Array mit PlayerSerials, Array benutzte FeldID, PID des Managers}
  CPROPSTATISTIK:="DynamicDungeonStatistik"  //Statistikspeicher
EndEnum

Enum TeleporterCProps
  CPTELE_FIELDID :="fieldid",
  CPTELE_TELETO  :="teleto",
  CPTELE_TELEFROM:="telefrom",
  CPTELE_EXIT    :="exit",
  CPTELE_START   :="start",
  CPTELE_PARTY   :="party"
EndEnum

Enum MobCProps
  CPMOB_CONTROL:="DungeonControlPID",
  CPMOB_BOSS   :="DungeonBoss",
  CPMOB_COORDS :="DungeonCoord"
EndEnum

Enum ItemCProps
  CPITEM_DIFFICULTY:="DungeonDifficulty",
  CPITEM_STATE     :="DungeonState"
EndEnum

Enum FieldType
  BOSSGANG:=1,
  BOSSSACKGASSE,
  BOSSTSTUECK,
  GANG,
  SACKGASSE,
  TSTUECK
EndEnum

Enum DungeonLenght
  SMALL:=5,
  MID  :=10,
  LARGE:=20
EndEnum

Enum DungeonStrength
  NOVICE:="Novice",
  KORY  :="Kory",
  LEGEND:="Legend"
EndEnum

CONST MAX_CREATE_TRIES:=20;

Function AmountToDiff(amount)
  If (4 == amount)
      amount := 1+RandomInt(3);
  EndIf

  var diff := NOVICE;
  Case (amount)
      1: diff := NOVICE;
      2: diff := KORY;
      3: diff := LEGEND;
  EndCase

  return diff;
EndFunction

Function DiffToAmount(difficulty)
  var amount := 1;

  Case (difficulty)
    NOVICE: amount := 1;
    KORY:   amount := 2;
    LEGEND: amount := 3;
  EndCase

  return amount;
EndFunction

Function DungeonChestReward(item, who, difficulty := 0, factor := 1)
    If (!difficulty)
        difficulty := item.getprop(CPITEM_DIFFICULTY);
    EndIf
    Set_Script_Option(SCRIPTOPT_NO_INTERRUPT, 1);
    var state := item.getprop(CPITEM_STATE);
    If (error == state)
        state := dictionary;
    EndIf

    If (state[who.serial])
        SendSysMessagePergon(who,
            "Du hast deine Belohnung schon.",
            "You are already rewarded."
        );
        return;
    EndIf
    state[who.serial] := 1;
    item.setprop(CPITEM_STATE, state);
    Set_Script_Option(SCRIPTOPT_NO_INTERRUPT, 0);

    // amount based on difficulty
    var amount := difficulty * factor;

    CreateItemInBackpackPergon(who, "DynRewardCoin", amount);
    SendSysMessagePergon(who,
        "Ihr steckt eine kleine Belohnung in Euren Rucksack.",
        "You put a little reward into your backpack."
    );
EndFunction

Function Spawn(spawnEntry,Difficulty,x:=0,y:=0,z:=0,range:=0)
  ////////////////////////////////////////////////////////////////////////////
  //
  //  Spawnt NPCs
  //
  //  Parameter:  spawnEntry -> CfgString aus dungeons.cfg
  //              Difficulty -> String der Difficulty
  //              x,y,z,range -> falls angegeben nicht Koords aus Cfg nehmen
  //  Returnwert: Referenz des NPC
  //  Returnwert im Fehlerfall: 0
  //
  ////////////////////////////////////////////////////////////////////////////
  //Return ist Referenz zum erzeugten NPC
  Var oGroupsConfig:=ReadConfigFile(":dyndungeon:dungeongroups");
  If (!oGroupsConfig)
    syslog("FEHLER: dungeongroups nicht geladen: "+oGroupsConfig.errortext);
    return 0;
  EndIf

  var cfgSpawnNPC := SplitWords(spawnEntry);
  var npctemplate := GetConfigString(oGroupsConfig[cfgSpawnNPC[4]],Difficulty);
  If (!npctemplate)
    syslog(
      "FEHLER: npcgroup "+cfgSpawnNPC[4]+"/"+Difficulty+" nicht gefunden: "+
      npctemplate.errortext
    );
    return 0;
  EndIf

  npctemplate:=SplitWords(npctemplate);
  npctemplate:=npctemplate.randomentry();

  If (!x)
    x:=CInt(cfgSpawnNPC[1]);
  EndIf
  If (!y)
    y:=CInt(cfgSpawnNPC[2]);
  EndIf
  If (!z)
    z:=CInt(cfgSpawnNPC[3]);
  EndIf
  If (!range)
    range:=CInt(cfgSpawnNPC[5]);
  EndIf

  var sea_npc:=0;
  // Ist es ein See-NPC bzw beides?
  If (GetNPCConfig(npctemplate).movemode["S"])
    sea_npc:=1;
  Else
    sea_npc:=0;
  EndIf

  // Spawnlocation besorgen
  var creature;
  var times:=0;
  var spawnx;
  var spawny;
  var spawnz;
  While (times<=MAX_CREATE_TRIES)
    spawnx:=RandomIntMinMax(x-range,x+range+1);
    spawny:=RandomIntMinMax(y-range,y+range+1);
    spawnz:=GetStandingHeight(spawnx, spawny, z-3,REALM_MALAS);
    If (spawnz==error)
      If (sea_npc) // Ist es ein See-NPC?
        spawnz:=GetStandingLayers(spawnx, spawny, MAPDATA_FLAG_MOVESEA, REALM_MALAS)[1]; // Erstmal schaun ob Wasser da ist
        If (!spawnz)
          times+=1;
        Else
          Break;
        EndIf
      Else
        times+=1;
      EndIf
    Else
      Break;
    EndIf
    SleepMS(2);
  EndWhile
  If (times>MAX_CREATE_TRIES)  // Keine passende Location gefunden
    SysLog(
      "FEHLER: Es konnte keine Location für SpawnNPC "+x+" "+y+" "+z+" "+
      npctemplate+" gefunden werden."
    );
    Return 0;
  EndIf

  // NPC erzeugen
  creature:=CreateNpcFromTemplatePergon(
    ":dyndungeon:"+npctemplate, spawnx, spawny, spawnz.z,
    // gender, props, custom, realm
    2,         0,     0,      REALM_MALAS
  );
  If (!creature)
    SysLog(
      "FEHLER: Es konnte kein NPC für SpawnNPC "+x+" "+y+" "+z+" "+
      npctemplate+" erzeugt werden: "+creature.errortext
    );
    Return 0;
  EndIf
  creature.setprop(CPMOB_COORDS,{spawnx,spawny,spawnz.z});

  var rand:=RandomInt(100);  // "Airmodus"? Ignorieren von mag.Feldern
  Case (Difficulty)
    NOVICE:
      If (!rand)  // 1%
        creature.setprop("ignorefields",1);
      EndIf
    KORY:
      If (rand<35) // 35%
        creature.setprop("ignorefields",1);
      EndIf
    LEGEND:
      If (rand<70) // 70%
        creature.setprop("ignorefields",1);
      EndIf
  EndCase

  Return creature;
EndFunction

Function SpawnSpecial(Special,Difficulty)
  ////////////////////////////////////////////////////////////////////////////////////////////
  //
  //  Spawnt Items
  //
  //  Parameter:  Special -> CfgString aus dungeons.cfg
  //              Difficulty -> String der Difficulty
  //  Returnwert: Referenz des Items
  //  Returnwert im Fehlerfall: 0
  //
  //////////////////////////////////////////////////////////////////////////////////////////////
  Var oGroupsConfig:=ReadConfigFile(":dyndungeon:dungeongroups");
  var cfgSpecial:=SplitWords(Special);
  var objtype:=SplitWords(GetConfigString(oGroupsConfig[cfgSpecial[4]],Difficulty));

  //Wahrscheinlichkeit ausschneiden
  var procent:=CInt(objtype[1]);
  objtype.erase(1);
  If (procent>RandomInt(100))
    //Den gültigen ObjType auswählen
    var lootgroup:=objtype[1];
    objtype.erase(1);
    objtype:=CInt(objtype.randomentry());
    var x,y,z,range;
    range:=CInt(cfgSpecial[6]);
    cfgSpecial[1]:=CInt(cfgSpecial[1]);  //Koords
    cfgSpecial[2]:=CInt(cfgSpecial[2]);
    cfgSpecial[3]:=CInt(cfgSpecial[3]);
    var times:=0;
    While (times<=MAX_CREATE_TRIES)
      x:=RandomIntMinMax(cfgSpecial[1]-range,cfgSpecial[1]+range+1);
      y:=RandomIntMinMax(cfgSpecial[2]-range,cfgSpecial[2]+range+1);
      z:=GetStandingHeight(x,y,cfgSpecial[3]-3,REALM_MALAS);
      If (z==error)
        times+=1;
      Else
        Break;
      EndIf
    EndWhile
    var object:=CreateItemAtLocation(x,y,z.z,objtype,1,REALM_MALAS);
    If (object)
      If (object.isa(POLCLASS_CONTAINER))
        MakeLoot(object,lootgroup);
      Else
        object.setprop(CPITEM_DIFFICULTY, DiffToAmount(Difficulty));
      EndIf
      Return(object);
    EndIf
  EndIf
  Return(0);
EndFunction

Function FindBossAfterRestart(DungeonX1,DungeonY1,DungeonX2,DungeonY2)
  ForEach mob in (ListObjectsInBox(DungeonX1,DungeonY1,-127,DungeonX2,DungeonY2,128,REALM_MALAS))
    SleepMS(2);
    If (!mob.isA(POLCLASS_NPC))
      Continue;
    EndIf
    If (mob.getprop(CPMOB_BOSS))
      Return(mob);
    EndIf
  EndForEach
  Return(0);
EndFunction

Function FindMobsAfterRestart(DungeonX1,DungeonY1,DungeonX2,DungeonY2, byref Mobs)
  ForEach mob in (ListObjectsInBox(DungeonX1,DungeonY1,-127,DungeonX2,DungeonY2,128,REALM_MALAS))
    SleepMS(2);
    If (!mob.isA(POLCLASS_NPC))
      continue;
    EndIf
    If (mob.getprop(CPMOB_BOSS))
      continue;  // Boss wollen wir nicht
    EndIf
    If (mob.getprop(CPMOB_CONTROL))
      mob.setprop(CPMOB_CONTROL,GetPID());
      Mobs.append(mob);
    EndIf
  EndForEach
EndFunction


// Zwingt NPC zum Angriff auf Char
Function ForceOpponentChange(npc, char)
  Var ev := struct;
  ev.+ type;
  ev.+ source;
  ev.type := EVID_PEACEMADE;
  ev.source := char;

  SendEvent(npc, ev);  // erstmal friedlich machen, damit er alten Gegner vergisst
  ev.type := SYSEVENT_ENGAGED;
  SendEvent(npc, ev);  // und dann angreifen lassen
EndFunction
