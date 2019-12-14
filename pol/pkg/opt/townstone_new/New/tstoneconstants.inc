Include "include/place";
Include "tstonegump";
Include "include/string";
Include ":housing:house";
Include "include/logutil";

Enum Lists
  LIST_FUNCTS := 900,
  LIST_PASSES := 901
EndEnum

Function InitTStone(stone, tstonedat, who := 0)
  If (!tstonedat)
    tstonedat := CreateDataFile("TStone", DF_KEYTYPE_STRING);
  EndIf
  If (!stone.getprop("Town"))
    If (who)
      SendSysMessagePergon(who,
        "Stadtstein mit .createtstone erstellen!\n"+
        "Abbruch"
      );
    Else
      SysLog(
        "FEHLER: Stadtstein "+ItemInfoStr(stone, COORDS_REALM)+
        " ist keiner Stadt zugewiesen!"
      );
    EndIf
    return 0;
  EndIf
  If (!tstonedat.FindElement(stone.getprop("Town")))
    tstonedat.CreateElement(stone.getprop("Town"));
  EndIf
  
  return 1;
EndFunction

Function InitTown(towndat)
  If (!towndat.getprop("Einwohner"))
    towndat.setprop("Einwohner", dictionary{ });
  EndIf
  If (!towndat.getprop("Steuern"))
    towndat.setprop("Steuern", SteuerStruct());
  EndIf
  If (!towndat.getprop("Passierscheine"))
    towndat.setprop("Passierscheine", array{ });
  EndIf
EndFunction

Function AddBG(who, town, towndat)
  var habitants := towndat.getprop("Einwohner");
  
  habitants.insert(who.serial, EinwohnerStruct(who, towndat));
  towndat.setprop("Einwohner", habitants);
  who.setprop("Town", town);
  who.title_suffix := "Aus "+town;
  SendSysMessagePergon(who, "Ihr seid nun Bürger der Stadt "+town);
EndFunction

Function EraseBG(who, towndat);
  var habitants := towndat.getprop("Einwohner");
  var habitant  := habitants[who.serial];
  
  If (habitant.funct)
    SendSysMessagePergon(who,
      "Ihr könnt Eure Bürgerschaft nicht kündigen, "+
      +"gebt erst Euer Amt als "+habitant.funct+" auf!"
    );
    return;
  EndIf
    
  habitants.erase(who.serial);
  towndat.setprop("Einwohner", habitants);
  who.title_suffix := "";
  SendSysMessagePergon(who, 
    "Ihr seid nicht länger Bürger von "+who.getprop("Town")
  );
  who.eraseprop("Town");
EndFunction

Function AddFunct(who, ret, towndat)
  var habitants := towndat.getprop("Einwohner");
  var habitant  := habitants[who.serial];
  
  If (habitant.rights.functs or habitant.rights.all or
    who.cmdlevel >= CMDLEVEL_HIGHGM
  )
    var newfunct, choice;
    newfunct := ret[TXT_FUNCT];
    choice   := ret[TXT_CHOICEA];
    newfunct[TXT_FUNCT+": "] := "";
    choice[TXT_CHOICEA+": "] := "";
      
    var choiced := GetBgNames(habitants)[1][CInt(choice)][2];
    If (!GetFuncts(habitants, newfunct))
      ForEach habitant in (habitants)
        var bg := SystemFindObjectBySerial(_habitant_iter,
          SYSFIND_SEARCH_OFFLINE_MOBILES
        );
        If (bg.serial == choiced);
          habitant.funct := newfunct;
          If (ret[CB_FUNCTS])
            habitant.rights.+functs := 1;
          EndIf
          If (ret[CB_TAX])
            habitant.rights.+tax := 1;
          EndIf
          If (ret[CB_PASS])
            habitant.rights.+pass := 1;
          EndIf
          towndat.setprop("Einwohner", habitants);
          bg.title_suffix := newfunct+" von "+bg.getprop("Town");
          SendSysMessagePergon(who,
            bg.name+" hat das Amt '"+newfunct+"' erhalten"
          );
          SendTipText(bg,
            "Ihr seid nun "+newfunct+" der Stadt "+bg.getprop("Town")
          );
          return;
        EndIf
      EndForEach
      SendSysMessagePergon(who, "Abbruch", "Abort");
      return;
    Else
      SendSysMessagePergon(who, "Das Amt '"+newfunct+"' ist schon vorhanden!");
      return;
    EndIf
  Else
    SendSysMessagePergon(who, "Du bist ein böser Cheater!");
    return;
  EndIf
EndFunction

Function DelFunct(who, ret, towndat)
  var habitants := towndat.getprop("Einwohner");
  var habitant  := habitants[who.serial];
  
  If (habitant.rights.functs or habitant.rights.all or
    who.cmdlevel >= CMDLEVEL_HIGHGM
  )
    var choice;
    choice := ret[TXT_CHOICED];
    choice[TXT_CHOICED+": "] := "";
      
    var choiced := GetSpecialNames(who, habitants, LIST_FUNCTS)[1][CInt(choice)][2];
    ForEach habitant in (habitants)
      var bg := SystemFindObjectBySerial(_habitant_iter,
        SYSFIND_SEARCH_OFFLINE_MOBILES
      );
      If (bg.serial == choiced)
        SendSysMessagePergon(who,
          bg.name+" wurde das Amt '"+habitant.funct+"' entzogen"
        );
        SendTipText(Bg,
          "Ihr seid nicht länger "+habitant.funct+" der Stadt "+who.getprop("Town")
        );
        habitant.funct  := 0;
        habitant.rights := struct{ };
        towndat.setprop("Einwohner", habitants);
        bg.title_suffix := "Aus "+who.getprop("Town");
        return;
      EndIf
    EndForEach
    SendSysMessagePergon(who, "Abbruch", "Abort");
    return;
  Else
    SendSysMessagePergon(who, "Du bist ein böser Cheater!");
    return;
  EndIf
EndFunction

Function AddBM(who, town, towndat)
  If (who.cmdlevel <= CMDLEVEL_HIGHGM)
    SendSysMessagePergon(who, "Du bist ein böser Cheater!");
    return;
  EndIf
  
  var habitants := towndat.getprop("Einwohner");
  var BM := SystemFindObjectBySerial(GetFuncts(habitants, "BM"),
              SYSFIND_SEARCH_OFFLINE_MOBILES
            ).name;
    
  SendSysMessagePergon(who, "Wählt einen Bürger zum Bürgermeister");
  SendSysMessagePergon(who, "Achtung: Der alte Bürgermeister wird ersetzt!");
  var tgt := Target(who);
    
  If (tgt.IsA(POLCLASS_MOBILE) and !tgt.IsA(POLCLASS_NPC))
    If (tgt.getprop("Town") == town)
      If (BM)
        BM.title_suffix := "Aus "+BM.getprop("Town");
        habitants[BM.serial].funct := 0;
        habitants[BM.serial].rights.-all;
        SendTipText(BM,
           "Ihr wurdet von "+tgt.name+" als Bürgermeister abgelöst"
        );
      EndIf
      habitants[tgt.serial].funct  := "BM";
      habitants[tgt.serial].rights.+all := 1; 
      towndat.setprop("Einwohner", habitants);
      tgt.title_suffix := "BM von "+tgt.getprop("Town");
      SendTipText(tgt, "Ihr seid nun Bürgermeister von "+tgt.getprop("Town"));
    Else
      SendSysMessagePergon(who, "Der Spieler ist kein Bürger der Stadt!");
      return;
    EndIf
  Else
    SendSysMessagePergon(who, "Das ist kein Spieler!");
    return;
  EndIf
EndFunction

Function AddPass(who, town, towndat)
  var habitants := towndat.getprop("Einwohner");
  var habitant  := habitants[who.serial];
  var passes    := towndat.getprop("Passierscheine");
  
  If (habitant.rights.pass or habitant.rights.all or
    who.cmdlevel >= CMDLEVEL_HIGHGM
  )
    var pass;
    var tgt := Target(who);
    If (tgt.IsA(POLCLASS_MOBILE) and !tgt.IsA(POLCLASS_NPC))
      pass := tgt.getprop("pass");
      If (!pass)
        pass := { town };
      Else
        If (!(town in pass))
          pass.append(town);
        Else
          SendSysMessagePergon(who,
            "Dieser Spieler hat bereits einen Passierschein von "+town
          );
          return 0;
        EndIf
      EndIf
    Else
      SendSysMessagePergon(who,
        "Passierscheine können nur an Spieler vergeben werden!"
      );
      return 0;
    EndIf
    tgt.setprop("pass", pass);
    passes.append(tgt.serial);
    towndat.setprop("Passierscheine", passes);
    SendSysMessagePergon(tgt,
      "Ihr habt nun einen Passierschein von "+town
    );
    return;
  Else
    SendSysMessagePergon(who, "Du bist ein böser Cheater!");
    return;
  EndIf
EndFunction

Function DelPass(who, ret, town, towndat)
  var habitants := towndat.getprop("Einwohner");
  var habitant  := habitants[who.serial];
  var passes    := towndat.getprop("Passierscheine");
  
  If (habitant.rights.pass or habitant.rights.all or
    who.cmdlevel >= CMDLEVEL_HIGHGM
  )
    var choice;
    choice := ret[TXT_CHOICEP];
    choice[TXT_CHOICEP+": "] := "";
      
    var choiced := GetSpecialNames(who, passes, LIST_PASSES)[1][CInt(choice)][2];
    ForEach pass in passes
      var passowner := SystemFindObjectBySerial(pass,
        SYSFIND_SEARCH_OFFLINE_MOBILES
      );
      If (passowner.serial == choiced)
        var towns := passowner.getprop("pass");
        If (towns.size() > 1)
          ForEach townpass in towns
            If (townpass == town)
              towns.erase(_townpass_iter);
              passowner.setprop("pass", towns);
              break;
            EndIf
          EndForEach
        Else
          passowner.eraseprop("pass");
        EndIf
        passes.erase(_pass_iter);
        towndat.setprop("Passierscheine", passes);
        SendTipText(passowner, 
          "Der Passierschein für "+town+" wurde euch entzogen!"
        );
        return;
      EndIf
    EndForEach
  Else
    SendSysMessagePergon(who, "Du bist ein böser Cheater!");
    return;
  EndIf
EndFunction

Function EinwohnerStruct(who, towndat)
  var cashbox := towndat.getprop("Stadtkasse");
  var house;
  ForEach houseserial in GetGlobalProperty("#houselist")
    house := SystemFindObjectBySerial(houseserial);
    If (IsHouseOwner(houseserial, who) and 
      PlaceName(house) == PlaceName(who)
    )
      house := GetMultiHouseSign(house).name;
      cashbox["Steuern"].unpaid.append({who.serial, 1});
      towndat.setprop("Stadtkasse", cashbox);
      break;
    EndIf
  EndForEach
  If (TypeOfInt(house) != OT_STRING)
    house := 0;
  EndIf
  return struct{
    funct  := 0,
    rights := struct{ },
    house  := house,
    pass   := 0,
    tax    := struct{ paid := 0, time := 0},
    time   := ReadGameClock(),
    votebm := 0,
    votere := 0
  };
EndFunction

Function SteuerStruct()
    return struct{
      rate     := 60000,
      punish   := 20000,
      puntime  := 604800*4,     //4 Wochen
      unpaid   := array{ }
    };
EndFunction

Function EraseUnpaid(who, towndat)
  var tax := towndat.getprop("Steuern");
  ForEach habitant in (tax.unpaid)
    If (habitant == who.serial)
      tax.unpaid.erase(_habitant_iter);
      towndat.setprop("Steuern", tax);
      return;
    EndIf
  EndForEach
EndFunction

Function GetFuncts(habitants, funct := 0)
  var functs; 
  If (funct)
    ForEach habitant in (habitants.keys())
      If (habitants[habitant].funct == funct)
        return habitant;
      EndIf
    EndForEach
  Else
    functs := array;
    ForEach habitant in (habitants.keys())
      If (habitants[habitant].funct)
        functs.append(habitants[habitant].funct);
      EndIf
    EndForEach
    return functs;
  EndIf
  return 0;
EndFunction

Function GetBgNames(habitants)
  
  var bgnames := array{ };
  var ret     := "";

  ForEach habitant in (habitants)
    If (!habitant.funct)
      bgnames.append({SystemFindObjectBySerial(_habitant_iter,
        SYSFIND_SEARCH_OFFLINE_MOBILES).name, _habitant_iter
      });
    EndIf
  EndForEach
  
  bgnames := SortArrayABC(bgnames, 1, 1);
  If (bgnames.size())
    For i := 1 To bgnames.size()
        ret := ret+i+" "+bgnames[i][1]+"<br/>";
    EndFor
  Else
    ret := "Keine entsprechenden Bürger gefunden";    
  Endif
  return ({bgnames, ret});
EndFunction

Function GetSpecialNames(who, habitants, list := 0)

  var bgnames := array{ };
  var info    := "";
  var ret     := "";
  
  ForEach habitant in habitants
    If (list == LIST_FUNCTS)
      If (habitant.funct and _habitant_iter != who.serial
        and habitant.funct != "BM"
      )
        bgnames.append({SystemFindObjectBySerial(_habitant_iter,
          SYSFIND_SEARCH_OFFLINE_MOBILES).name, _habitant_iter
        });
      EndIf
    ElseIf (list == LIST_PASSES)
      bgnames.append({SystemFindObjectBySerial(habitant,
        SYSFIND_SEARCH_OFFLINE_MOBILES).name, habitant
      });
    EndIf
  EndForEach
  
  bgnames := SortArrayABC(bgnames, 1, 1);
  If (bgnames.size())
    For i := 1 To bgnames.size()
      ForEach habitant in habitants
        var name := SystemFindObjectBySerial(_habitant_iter,
          SYSFIND_SEARCH_OFFLINE_MOBILES
        ).name;
        If (bgnames[i][1] == name)
          If (list == LIST_FUNCTS)
            info := "("+habitant.funct+")";
          EndIf
        EndIf
      EndForEach
      ret := ret+i+" "+bgnames[i][1]+" "+info+"<br\>";
    EndFor
  Else
    If (list == LIST_FUNCTS)
      ret := "Keine entziehbaren Ämter gefunden";  
    ElseIf (list == LIST_PASSES)
      ret := "Keine Passierscheine gefunden";
    EndIf   
  Endif
  return ({bgnames, ret});
EndFunction
  
Function SendTipText(who, text)
  If (who.connected)
    SendStringAsTipWindow(who, text);
  Else
    who.setprop("LogonMessage", text);
  EndIf
EndFunction