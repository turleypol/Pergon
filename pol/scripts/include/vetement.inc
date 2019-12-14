///////////////////////////////////////////////////////////////////////////
// Vetement Include - Zufaelliges Einkleider eines NPC
//
// Author: Shinigami

// TODO:
//  - Armor etc... mal in die pkg/std/combat/itemdesc.cfg reinschauen

include "include/client";

//////////////////////////////////
// EquipNPC - Kleide den NPC ein
//////////////////////////////////
Function EquipNPC(npc, stripfirst := 1, equip_nonhumans := 0)
  Var equipt := GetObjProperty(npc, "Equipt");
  If (!equipt)
    syslog(
      "WARNUNG: NPC '"+npc.npctemplate+
      "' sollte eingekleidet werden, besitzt aber keine CProp 'Equipt'!"
    );
    return 0;
  EndIf

  If (
    // Nur Menschen/Elfen lassen sich ueberhaupt sinnvoll einkleiden
    (npc.objtype in array{
      CID_HUMAN_MALE, CID_HUMAN_FEMALE, CID_ELF_MALE, CID_ELF_FEMALE
    }) or
    // um Elementare mit Laterne auszustatten etc.
    equip_nonhumans
  )
    return Habille(npc, equipt, stripfirst);
  Else
    syslog(
      "WARNUNG: NPC '"+npc.npctemplate+
      "' ist nicht menschlich, besitzt aber eine Prop 'equipt'!"
    );
    return 0;
  EndIf
EndFunction

/////////////////////////////////
// Habille - Kleide den NPC ein
/////////////////////////////////
Function Habille(npc, equiptype, stripfirst := 1)
  If (equiptype == error)
    // Gibt nichts zu equippen
    return 0;
  EndIf

  var allok := 1;

  Var prio:=Set_Priority(255);

  Var data:=struct;
  data.+color:=ReadConfigFile("::colors");
  data.+cloths:=ReadConfigFile("::cloths");
  data.+equip:=ReadConfigFile("::equip2");

  If (data.color And data.cloths And data.equip)
    If (stripfirst)
      ForEach equipt in ListEquippedItems(npc) // NPC ausziehen
        DestroyItem(equipt);
      EndForEach
    EndIf

    Var group:=data.equip[equiptype]; // Nach der Equip-Definition suchen
    If (!group)
      group := data.equip["Clothes"];
      SysLog(
        "WARNUNG: '"+equiptype+"' wurde nicht in equip2.cfg gefunden "+
        "fuer "+CharInfoStr(npc)
      );
      allok := 0;
    EndIf

    If (group) // Gemaess der Definition anziehen
      Var strg;
      If (npc.gender)
        strg:="W";
      Else
        strg:="M";
      EndIf

      ForEach type in {"Equip", "Equip"+strg, "Armor", "Armor"+strg, "Weapon", "Weapon"+strg}
        ForEach elem in GetConfigStringArray(group, type)
          var res := EquipGroupOrItem(npc, StripComment(elem), data);
          If (!res)
            allok := 0;
          EndIf
        EndForEach
      EndForEach

      Var mounttype:=GetConfigString(group, "Mount");
      If (mounttype)
        mounttype:=SplitWords(StripComment(mounttype));

        Var mount:= CreateItemAtLocationPergon(
          npc.x, npc.y, npc.z, 0xf021, 1,npc.realm
        );
        If (mount)
          mount.graphic:=CInt(mounttype[1]);
          mount.color:=CInt(mounttype[2]);
          mount.newbie:=1;

          mount.movable:=1; // Irgendwie is das bzgl. Injection besser
          EquipItem(npc, mount);
          mount.movable:=0;
        Else
          allok := 0;
        EndIf
      EndIf
    EndIf
  EndIf

  UnloadConfigFile("::equip2");
  UnloadConfigFile("::cloths");
  UnloadConfigFile("::colors");

  Set_Priority(prio);
  return allok;
EndFunction

///////////////////////////////////////////////////////////////
// EquipGroupOrItem - Kleide gemaess Konfigstring oder Gruppe
///////////////////////////////////////////////////////////////

Function EquipGroupOrItem(npc, elem, ByRef data)
  var allok := 1;
  Var paramsEquip:=SplitWords(elem);
  If (paramsEquip[1]=="Group")
    Var group:=data.equip[paramsEquip[2]];
    If (group)
      Var strg;
      If (npc.gender)
        strg:="W";
      Else
        strg:="M";
      EndIf

      // Gruppen zur Auswahl vorbereiten
      Var groups:={ };
      Var groupsdata:={ };

      ForEach type in {"Group", "Group"+strg}
        ForEach groupelem in GetConfigStringArray(group, type)
          paramsEquip:=SplitWords(StripComment(groupelem));

          Var groupname:=paramsEquip[1];
          If (!(groupname in groups))
            groups.append(groupname);
          EndIf

          paramsEquip.erase(1);
          groupsdata.append({groupname, paramsEquip});
        EndForEach
      EndForEach

      // Gruppe auswaehlen und benutzen
      Var group:=groups.randomentry();

      ForEach groupelem in groupsdata
        If (groupelem[1]==group)
          var res := EquipItemString(npc, groupelem[2], data);
          If (!res)
            allok := 0;
          EndIf
        EndIf
      EndForEach
    Else
      SysLog(
        "WARNUNG: "+CharInfoStr(npc)+" kann '"+LHex(paramsEquip[2])+"' "+
        "nicht in equip2.cfg finden!"
      );
    EndIf
  Else
    var res := EquipItemString(npc, SplitWords(elem), data);
    If (!res)
      allok := 0;
    EndIf
  EndIf
EndFunction

//////////////////////////////////////////////////
// EquipItemString - Kleide gemaess Konfigstring
//////////////////////////////////////////////////

Function EquipItemString(npc, elem, ByRef data)
  If (elem[1]<>"")
    // ObjName ermitteln
    Var objname:=elem[1];
    If (CInt(objname))
      objname:=CInt(objname);
    EndIf

    // Farbe ermitteln
    Var color:=0;
    If (CInt(elem[2]))
      color:=CInt(elem[2]);
    Else
      If (elem[2] And (elem[2]<>"."))
        Var colorgruppe:=data.color[elem[2]];
        If (colorgruppe)
          Var colordata:=GetConfigStringArray(colorgruppe, "Color");
          color:=CInt(StripComment(colordata.randomentry()));
        Else
          SysLog(
            "WARNUNG: '"+npc.name+"' kann '"+elem[2]+
            "' nicht in 'color.cfg' finden!"
          );
          return 0;
        EndIf
      EndIf
    EndIf

    // Chance ermitteln
    Var erzeugen:=1;
    If (CInt(elem[3]))
      If (RandomInt(100)+1>CInt(elem[3]))
        erzeugen:=0;
      EndIf
    EndIf

    // Gegenstand erzeugen
    If (erzeugen)
      Var item;

      // Item nur versuchen zu erzeugen, wenn es ein gueltiges Template oder eine numerische ObjID ist
      If (CInt(objname))
        item:=CreateItemAtLocationPergon(npc.x, npc.y, npc.z, objname, 1,npc.realm);
      ElseIf (GetObjtypeByName(objname))
        item:=CreateItemAtLocationPergon(npc.x, npc.y, npc.z, objname, 1,npc.realm);
      EndIf

      If (!item)
        // Gegenstand aus einer Liste waehlen

        Var itemgruppe:=data.cloths[objname];
        If (itemgruppe)
          Var itemdatag;
          If (npc.gender)
            itemdatag:=GetConfigStringArray(itemgruppe, "ItemW");
          Else
            itemdatag:=GetConfigStringArray(itemgruppe, "ItemM");
          EndIf

          Var itemdata:=GetConfigStringArray(itemgruppe, "Item");
          If (itemdata.size()+itemdatag.size()>0)
            Var nummer:=RandomInt(itemdata.size()+itemdatag.size())+1;
            If (nummer<=itemdata.size())
              objname:=CInt(itemdata[nummer]);
            Else
              objname:=CInt(itemdatag[nummer-itemdata.size()]);
            EndIf

            // Gegenstand erzeugen
            item:=CreateItemAtLocationPergon(npc.x, npc.y, npc.z, objname, 1,npc.realm);
            If (!item)
              SysLog("WARNUNG: '"+npc.name+"' kann '"+objname+"' nicht erzeugen!");
              return 0;
            Endif
          EndIf
        Else
          syslog(
            "HINWEIS: '"+CharInfoStr(npc)+"' kann '"+objname+"' nicht in "+
            "'cloths.cfg' finden oder "+elem+"' enthaelt einen falschen "+
            "ObjType"
          );
          return 0;
        EndIf
      EndIf

      // Gegenstand anziehen
      If (item)
        If (EquipItem(npc, item))
          item.color:=color;
        Else
          DestroyItem(item);
          var logname := objname;
          If (CInt(logname) > 0)
            logname := LHex(CInt(logname));
          EndIf
          SysLog(
            "HINWEIS: "+CharInfoStr(npc)+" kann '"+logname+
            "' nicht anziehen!"
          );
          return 0;
        EndIf
      EndIf
    EndIf // if (erzeugen)
  EndIf
  return 1;
EndFunction

///////////////////////////////////////
// StripComment - Entfernt Kommentare
///////////////////////////////////////

Function StripComment(elem)
  Var pos:=Find(elem, "//", 1);
  If (pos)
    Return (elem[1, pos-1]);
  Else
    Return (elem);
  EndIf
EndFunction

// vim: sw=2 sts=2
