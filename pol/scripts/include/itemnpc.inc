///////////////////////////////////////////////////////////////////////////
// Item & NPC Include - Alles allgemeine zur Veraenderung von Items & NPCs
//
// Author: Shinigami

///////////////////////////////////////////////////////////////////////////
// Item-Manipulation
//   CreateItemAtLocationPergon       - Erzeugt das Item an der Postion
//   CreateItemInBackpackPergon       - Erzeugt das Item im Backpack
//   CreateSingleItemInBackpackPergon - Erzeugt einzelnes Item im Backpack
//   CreateItemInContainerPergon      - Erzeugt das Item im Container
//   CreateItemInInventoryPergon      - Erzeugt das Item im Container
//   CreateItemInBackpackStackable    - Erzeugt Items im Backpack, wenn
//                                      moeglich gestackt
//   MoveStackingItemToContainer      - Erzeugt Item im Container neu,
//                                      killt Original
//   SearchForExistingItemStack       - Sucht nach einem vorhandenen Item-Stack
//   DestroyItemPergon                - Zerstoert das Item, verhindert es
//                                      jedoch beim Backpack
//
// Item-Benutzung
//   AccessiblePergon         - Kann der Gegenstand benutzt werden?
//   FindItemInContainerTree  - Sucht ein Item rekursiv im Container
//   FindItemsInContainerTree - Sucht alle Items rekursiv im Container
//   GetTopContainer          - Ermittelt Obercontainer, falls Item verpackt
//   ToolEquipped             - Prueft, ob Tool in Hand ist
//   UpdateBlackList          - aktualisiert Blacklist von .grab/.resource
//
// Werkzeug-Manipulation
//   CheckWholePropertyConfig - Werkzeug-Eigenschaften aller Items ueberpruefen
//   GetPropertyItem          - Werkzeug-Eigenschaften lesen
//   SetPropertyItem          - Werkzeug-Eigenschaften setzen
//   CheckPropertyItem        - Werkzeug-Eigenschaften ueberpruefen
//   GetPropertyPrice         - Verkaufspreis ermitteln
//   SetWerkzeugSchaden       - Werkzeug beschaedigen
//   CheckWerkzeugSchaden     - Werkzeugbeschaedigung pruefen und Werkzeug
//                              ggf. zerstoeren
//
// Sonstiges
//   EntferneA    - Schnippelt das bloeder "a"/"an" ab
//   Endung       - Waehlt die richtige Endung
//   DescEinzahl  - Liefert von der ItemDesc.desc die Einzahl-Variante
//   DescMehrzahl - Liefert von der ItemDesc.desc die Mehrzahl-Variante
//
//////////////////
// Umbauarbeiten
//////////////////
//
// Faellt raus:
//   CreateItemAtLocation_  -> CreateItemAtLocationPergon
//   CreateItemInBackpack_  -> CreateItemInBackpackPergon
//   CreateItemInContainer_ -> CreateItemInContainerPergon
//   CreateItemInInventory_ -> CreateItemInInventoryPergon

use cfgfile;
use os;
use polsys;
use uo;

include ":housing:house";
include "include/bank";
include "include/client";

// Zum Beispiel fuer Tueren in Spielerhaeusern ...
Const IGNORE_LOCKEDDOWN := 1;
Const CHECK_LOCKEDDOWN  := 0;

// Liste nicht mitzunehmender ObjTypes
Const PROP_GRAB_BLACKLIST := "grabblacklist";
// fuer UpdateBlackList bei .grab/.resource
Const BLACKLIST_ADD    := 0;
Const BLACKLIST_REMOVE := 1;
Const BLACKLIST_TOGGLE := 2;

Const NEWBIE_MATERIAL:= // Welche Materialien duerfer Newbies benutzen?
  " Eisen Molybdaen Mangan Iridium Tenerum Indium Cerium"+                         // Metalle
  " Prodymium Lanthan Neodymium Wolfram Promethium Samarium"+
  " Leder Hasenleder Schlangenhaut Wolfsleder Darkwolfleder Gremlinleder"+         // Leder
  " Lamaleder Baerenleder Grizzlyleder Pantherleder Echsenleder"+
  " Feuerechsenleder Ostardleder Waldostardleder Raubostardleder Trollleder"+
  " Mischholz Schratholz Walnussholz Apfelholz Birnenholz Weidenholz"+             // Holz
  " Zypressenholz Eichenholz Pfirsichholz Zedernholz Tannenholz Erlenholz"+
  " Knochen Schaedel Trollschaedel Trollknochen Schlangenschuppen Echsenschuppen"+ // Knochen & Schuppen
  " Leinen Schafwolle Baumwolle Hanf"+                                             // Stoffe
  " Bernstein Zitrin Turmalin Rubin Amethyst Smaragd Saphir"+                      // Edelsteine
  " Glas Obsidian ";                                                               // Sonstiges

///////////////////////////
//                       //
//   Item-Manipulation   //
//                       //
///////////////////////////

/////////////////////////////////////////////////////////////////
// CreateItemAtLocationPergon - Erzeugt das Item an der Postion
/////////////////////////////////////////////////////////////////

Function CreateItemAtLocationPergon(x, y, z, itemdef, amount, realm:=_DEFAULT_REALM)
  If (TypeOfInt(itemdef)==OT_STRUCT) // Via GetItemDescriptor aufgerufen oder Fehler?
    If (itemdef<>error) // Ist es denn auch was Vernuenftiges?
      If (!itemdef.desc)
        SysLog("WARNUNG: CreateItemAtLocationPergon: "+Lower(Hex(itemdef.objtype))+" besitzt keine 'Desc'!");
      EndIf

      Return (CreateItemAtLocation(x, y, z, itemdef, amount, realm));
    Else
      SysLog("FEHLER: CreateItemAtLocationPergon: Fehlerhafter Aufruf = "+itemdef.errortext+"!");
    EndIf
  Else
    Var itemdesc:=ReadConfigFile("::itemdesc");
    If (itemdesc)
      If (TypeOfInt(itemdef)==OT_STRING) // Kein numerischer ObjType?
        itemdef:=GetObjtypeByName(itemdef);
      EndIf

      If (itemdesc[itemdef])
        If (!itemdesc[itemdef].desc)
          SysLog("WARNUNG: CreateItemAtLocationPergon: "+Lower(Hex(itemdef))+" besitzt keine 'Desc'!");
        EndIf

        Return (CreateItemAtLocation(x, y, z, itemdef, amount, realm));
      Else
        If (itemdef<0x4000)
         // SysLog("WARNUNG: CreateItemAtLocationPergon: "+itemdef+" ist nicht definiert!");
          Return (CreateItemAtLocation(x, y, z, itemdef, amount, realm));
        Else
          SysLog("FEHLER: CreateItemAtLocationPergon: "+itemdef+" ist nicht definiert!");
        EndIf
      EndIf
    Else
      SysLog("FEHLER: CreateItemAtLocationPergon: Kann itemdesc.cfg nicht oeffnen!");
    EndIf
  EndIf
EndFunction

//////////////////////////////////////////////////////////////
// CreateItemInBackpackPergon - Erzeugt das Item im Backpack
//////////////////////////////////////////////////////////////

Function CreateItemInBackpackPergon(of_character, itemdef, amount:=1, color:=-1)
  Var obj;
  If (TypeOfInt(itemdef)<>OT_STRUCT) // Nur mittels ObjType aufgerufen?
    If (TypeOfInt(itemdef)==OT_STRING) // Kein numerischer ObjType?
      itemdef:=GetObjtypeByName(itemdef);
    EndIf
    obj:=itemdef;
    itemdef:=GetItemDescriptor(itemdef);
  EndIf

  If (itemdef<>error)  // Ist es denn auch was Vernuenftiges?
    If (color<>-1)  // Erstmal die neue und alte Methode trennen... bloede Seiteneffekte
      itemdef.color:=color;
    EndIf
    If (!itemdef.desc)
      SysLog("WARNUNG: CreateItemInBackpackPergon: "+Lower(Hex(itemdef.objtype))+" besitzt keine 'Desc'!");
    EndIf

    Var item:=CreateItemInBackpack(of_character, itemdef, amount);
    If (!item)
      item:=CreateItemAtLocation(of_character.x, of_character.y, of_character.z, itemdef, amount, of_character.realm);
      If (item)
        SendSysMessagePergon(of_character, "Euer Backpack ist voll!");
      EndIf
    EndIf

    Return (item);
  ElseIf (itemdef.errortext["Itemdesc.cfg entry for objtype"])
    If (obj>=0x4000)
      SysLog("FEHLER: CreateItemInBackpackPergon: "+obj+" ist nicht definiert!");
    Else
      Var item;

      If (color<>-1) // Erstmal die neue und alte Methode trennen... bloede Seiteneffekte
        item:=SearchForExistingItemStack(of_character.backpack, obj, color);
        If (item) // ...hab was gefunden
          Var oldamount:=item.amount;
          AddAmount(item, amount);

          If (item.amount<>oldamount+amount) // Hat er auch fleiﬂig gestackt?
            SubtractAmount(item, item.amount-oldamount); // ... wieder nach unten korrigieren und neuen Stack erzeugen
          Else // ...jo, alles da
            amount:=0;
          EndIf
        EndIf

        If (amount) // 'n bisschen was fehlt noch...
          item := CreateItemAtLocation(5376, 1081, 0, obj, amount);
          If (item)
            item.color:=color;
            item.movable:=1;
            If (!MoveItemToContainer(item, of_character.backpack))
              MoveObjectToLocation(item, of_character.x, of_character.y, of_character.z, of_character.realm);
              SendSysMessagePergon(of_character, "Euer Backpack ist voll!");
            EndIf
          EndIf
        EndIf
      Else
        item:=CreateItemInBackpack(of_character, obj, amount);
        If (!item)
          item:=CreateItemAtLocation(of_character.x, of_character.y, of_character.z, obj, amount, of_character.realm);
          If (item)
            SendSysMessagePergon(of_character, "Euer Backpack ist voll!");
          EndIf
        EndIf
      EndIf

      Return (item);
    EndIf

  Else
    SysLog("FEHLER: CreateItemInBackpackPergon: Fehlerhafter Aufruf = "+itemdef.errortext+"!");
  EndIf
EndFunction

//////////////////////////////////////////////////////////////////////////
// CreateSingleItemInBackpackPergon - Erzeugt einzelnes Item im Backpack
//////////////////////////////////////////////////////////////////////////

Function CreateSingleItemInBackpackPergon(of_character, itemdesc, amount:=1, color:=-1)
  Var itemdef:=itemdesc;

  If (TypeOfInt(itemdef)<>OT_STRUCT) // Nur mittels ObjType aufgerufen?
    If (TypeOfInt(itemdef)==OT_STRING) // Kein numerischer ObjType?
      itemdef:=GetObjtypeByName(itemdef);
    EndIf

    itemdef:=GetItemDescriptor(itemdef);
  EndIf

  If (itemdef<>error) // Ist es denn auch was Vernuenftiges?
    If (!itemdef.desc)
      SysLog("WARNUNG: CreateSingleItemInBackpackPergon: "+Lower(Hex(itemdef.objtype))+" besitzt keine 'Desc'!");
    EndIf

    If (color<>-1) // Farbe aendern, wenn gewuenscht
      itemdef.color:=color;
    EndIf

    itemdef.cprops.insert("#forcesingleitem", 1); // Damit wird der Stack getrennt

    Var item:=CreateItemInBackpack(of_character, itemdef, amount);
    If (!item)
      item:=CreateItemAtLocation(of_character.x, of_character.y, of_character.z, itemdef, amount, of_character.realm);
      If (item)
        SendSysMessagePergon(of_character, "Euer Backpack ist voll!");
      EndIf
    EndIf

    If (item)
      EraseObjProperty(item, "#forcesingleitem");
    EndIf

    Return (item);
  Else
    SysLog("FEHLER: CreateSingleItemInBackpackPergon: Fehlerhafter Aufruf = "+itemdef.errortext+" ["+itemdesc+"] !");
  EndIf
EndFunction

////////////////////////////////////////////////////////////////
// CreateItemInContainerPergon - Erzeugt das Item im Container
////////////////////////////////////////////////////////////////

Function CreateItemInContainerPergon(container, itemdef, amount:=1)
  If (TypeOfInt(itemdef)==OT_STRUCT) // Via GetItemDescriptor aufgerufen oder Fehler?
    If (itemdef<>error) // Ist es denn auch was Vernuenftiges?
      If (!itemdef.desc)
        SysLog("WARNUNG: CreateItemInContainerPergon: "+Lower(Hex(itemdef.objtype))+" besitzt keine 'Desc'!");
      EndIf

      Return (CreateItemInContainer(container, itemdef, amount));
    Else
      SysLog("FEHLER: CreateItemInContainerPergon: Fehlerhafter Aufruf = "+itemdef.errortext+"!");
    EndIf
  Else
    Var itemdesc:=ReadConfigFile("::itemdesc");
    If (itemdesc)
      If (TypeOfInt(itemdef)==OT_STRING) // Kein numerischer ObjType?
        itemdef:=GetObjtypeByName(itemdef);
      EndIf

      If (itemdesc[itemdef])
        If (!itemdesc[itemdef].desc)
          SysLog("WARNUNG: CreateItemInContainerPergon: "+Lower(Hex(itemdef))+" besitzt keine 'Desc'!");
        EndIf

        Return (CreateItemInContainer(container, itemdef, amount));
      Else
        If (itemdef<0x4000)
         // SysLog("WARNUNG: CreateItemInContainerPergon: "+itemdef+" ist nicht definiert!");
          Return (CreateItemInContainer(container, itemdef, amount));
        Else
          SysLog("FEHLER: CreateItemInContainerPergon: "+itemdef+" ist nicht definiert!");
        EndIf
      EndIf
    Else
      SysLog("FEHLER: CreateItemInContainerPergon: Kann itemdesc.cfg nicht oeffnen!");
    EndIf
  EndIf
EndFunction

////////////////////////////////////////////////////////////////
// CreateItemInInventoryPergon - Erzeugt das Item im Container
////////////////////////////////////////////////////////////////

Function CreateItemInInventoryPergon(container, itemdef, amount:=1)
  If (TypeOfInt(itemdef)==OT_STRUCT) // Via GetItemDescriptor aufgerufen oder Fehler?
    If (itemdef<>error) // Ist es denn auch was Vernuenftiges?
      If (!itemdef.desc)
        SysLog("WARNUNG: CreateItemInInventoryPergon: "+Lower(Hex(itemdef.objtype))+" besitzt keine 'Desc'!");
      EndIf

      Return (CreateItemInInventory(container, itemdef, amount));
    Else
      SysLog("FEHLER: CreateItemInInventoryPergon: Fehlerhafter Aufruf = "+itemdef.errortext+"!");
    EndIf
  Else
    Var itemdesc:=ReadConfigFile("::itemdesc");
    If (itemdesc)
      If (TypeOfInt(itemdef)==OT_STRING) // Kein numerischer ObjType?
        itemdef:=GetObjtypeByName(itemdef);
      EndIf

      If (itemdesc[itemdef])
        If (!itemdesc[itemdef].desc)
          SysLog("WARNUNG: CreateItemInInventoryPergon: "+Lower(Hex(itemdef))+" besitzt keine 'Desc'!");
        EndIf

        Return (CreateItemInInventory(container, itemdef, amount));
      Else
        If (itemdef<0x4000)
         // SysLog("WARNUNG: CreateItemInInventoryPergon: "+itemdef+" ist nicht definiert!");
          Return (CreateItemInInventory(container, itemdef, amount));
        Else
          SysLog("FEHLER: CreateItemInInventoryPergon: "+itemdef+" ist nicht definiert!");
        EndIf
      EndIf
    Else
      SysLog("FEHLER: CreateItemInInventoryPergon: Kann itemdesc.cfg nicht oeffnen!");
    EndIf
  EndIf
EndFunction

// erzeugt neues Item mit kopierten Props/Members im Zielcontainer, damit
// es stacken kˆnnte, und beseitigt altes Item
Function MoveStackingItemToContainer(item, container);
  ForEach object in (FindSubstance(container,item.objtype,1,0,FINDSUBSTANCE_FIND_ALL+FINDSUBSTANCE_ROOT_ONLY))
    // IsStackable() f¸hrt selber gleich am Anfang Check auf Objtype
    // und .stackable durch
    // GewichtsCheck und AmountCheck werden f¸r param0 ausgef¸hrt Reihenfolge!
    If (IsStackable(object,item))
      If (AddAmount(object,item.amount))
        DestroyItem(item);
        Return(object);
      EndIf
    EndIf
    SleepMS(2);
  EndForEach
  var res:=MoveItemToContainer(item, container);
  If (res)
    Return(item);
  Else
    Return(res);
  EndIf
EndFunction

/////////////////////////////////////////////////////////////////////////
// SearchForExistingItemStack - Sucht nach einem vorhandenen Item-Stack
/////////////////////////////////////////////////////////////////////////

Function SearchForExistingItemStack(container, objtype, color)
  // hmm... w‰re es nich schneller, in der cfg nach dem flag zu guggen?
  Var testitem:=CreateItemAtLocation(0, 0, 0, objtype, 2); // Kann nur erzeugt werden, wenn stackable (die 2)
  If (testitem)
    testitem.color:=color;
    If ((!testitem.isa(POLCLASS_WEAPON)) And (!testitem.isa(POLCLASS_ARMOR)))
    	ForEach item in FindSubstance(container,objtype,1,0,FINDSUBSTANCE_FIND_ALL+FINDSUBSTANCE_ROOT_ONLY)
    	  If (IsStackable(item,testitem))
    	    DestroyItem(testitem);
    	    Return (item);
    	  EndIf
    	EndForEach
    Else
      SysLog("WARNUNG: Waffen und Ruestungen koennen nicht gestackt werden!");
    EndIf

    DestroyItem(testitem);
  EndIf

  Return (0);
EndFunction

///////////////////////////////////////////////////////////////////////////////
// DestroyItemPergon - Zerstoert das Item, verhindert es jedoch beim Backpack
///////////////////////////////////////////////////////////////////////////////

// hmm, vielleicht waere es ueber ein DestroyScript doch besser... dann einfach die Items in ein neues Backpack zu verschieben

Function DestroyItemPergon(item)
  If (GetObjProperty(item, "#backpack")) // Ist es ein Spielerbackpack?
    Var player:=SystemFindObjectBySerial(GetObjProperty(item, "#backpack"), SYSFIND_SEARCH_OFFLINE_MOBILES);
    If (player)
      If (item.objtype<>0xe75) // Hmm, ist kein normales Backpack... Warum?
        SysLog("FEHLER: Der Spieler "+player.name+" ["+player.acctname+
          "] hat aber ein komisches Backpack (ObjType "+Lower(Hex(item.objtype))+" / Serial "+Lower(Hex(item.serial))+") !");
      EndIf

      If (!player.backpack) // Er hat gerade kein Backpack...
        EquipItem(player, item); // ...jetzt hat er es wieder

        SendSysMessagePergon(player, "Ihr legt euren Rucksack wieder an.", "");
      Else // ...er hat schon wieder ein neues... hmm
        If (MoveItemToContainer(item, player.backpack)) // Mal in sein aktuelles Backpack verschieben...
          SendSysMessagePergon(player, "Ihr verschiebt euren alten Rucksack in euren neuen.", "");
        Else // ...ging nicht
          MoveObjectToLocation(item, player.x, player.y, player.z, player.realm);

          SendSysMessagePergon(player, "Ihr legt euren alten Rucksack zu euren Fuessen.", "");
        EndIf
      EndIf

      SysLog("FEHLER: Das Backpack des Spielers "+player.name+" ["+player.acctname+"] sollte geloescht werden !");
    Else
      DestroyItem(item);
    EndIf
  Else
    DestroyItem(item);
  EndIf
EndFunction

/////////////////////////////////////////////////////////////
// CreateItemInContainerStackable - Erzeugt gestackte Items
/////////////////////////////////////////////////////////////
//
//Function CreateItemInContainerStackable(who, objtype, amount)
//  Var result:=CreateItemInContainerPergon(who.backpack, objtype, amount);
//  If (result.errortext)
//    If (result.errortext["not stackable"])
//      For i:=1 To amount
//        result:=CreateItemInContainerPergon(who.backpack, objtype, 1);
//      EndFor
//    EndIf
//  EndIf
//
//  Return (result);
//EndFunction
//
////////////////////////////////////////////////
// CreateItemStacked - Erzeugt gestackte Items
////////////////////////////////////////////////
//
//Function CreateItemStacked(container, objtype, amount)
//  Var result:=CreateItemInInventoryPergon(container, objtype, amount);
//  If (result.errortext)
//    If (result.errortext["not stackable"])
//      For i:=1 To amount
//        result:=CreateItemInInventoryPergon(container, objtype, 1);
//      EndFor
//    EndIf
//  EndIf
//
//  Return (result);
//EndFunction

////////////////////////
//                    //
//   Item-Benutzung   //
//                    //
////////////////////////

///////////////////////////////////////////////////////////
// AccessiblePergon - Kann der Gegenstand benutzt werden?
///////////////////////////////////////////////////////////
Function AccessiblePergon(who, item, lockeddown := CHECK_LOCKEDDOWN)
  If (lockeddown == CHECK_LOCKEDDOWN and item.getprop(PROP_HS_C_FIXED))
    // Festgetackerte Houseitems sind nicht benutzbar ...
    SendSysMessagePergon(who,
      "Ihr muesst den Gegenstand zuerst losmachen!",
      "You've to unlock this item first!"
    );
    return 0;
  ElseIf (CheckItemMaterial(who, item))
    // Darf das Item materialtechnisch benutzt werden?
    return Accessible(who, item);
  // ElseIf (!ReserveItem(item)) // ebenso bereits benutzte Items...
  //   SendSysMessagePergon(who,
  //     "Dieser Gegenstand wird bereits benutzt!", "This item is in use!"
  //   );
  //   return 0;
  EndIf

  return 0; // Wegens CheckItemUsage...
EndFunction

// herausfinden, ob das Object obj dem Spieler who "gehoert"
Function IsMine(who, obj)
  If (who.serial == obj.serial)
    // man kann sich nicht selbst besitzen
    // (bevor sich noch jemand selbst umbenennt)
    return 0;
  EndIf

  obj := GetTopContainer(obj, 1);

  If (
    // Haendler, Packtier oder Packtierleiche bzw. Packtasche
    obj.getprop("master") == who.serial or
    // Zuchttiere oder Leiche davon
    obj.getprop("ExMaster") == who.serial
  )
    return 1;
  EndIf

  If (obj.isa(POLCLASS_ITEM))
    If (obj.serial == who.backpack.serial)
      // direkt dabei (Backpack)
      return 1;
    EndIf

    If (obj.objtype == UOBJ_BLUE_MOONGATE)
      If (obj.getprop("caster") == who.serial)
        // eigenes Tor
        return 1;
      EndIf
    EndIf

    var bank := FindBankBox(who);
    If (bank.serial == obj.serial)
      // in der eigenen Bank
      return 1;
    EndIf

    var house := GetHouseContainingObject(obj);
    If (house)
      If (
        (
          // von Spieler Festgemachtes
          obj.getprop(PROP_HS_C_FIXED) or
          // (im) Hauscontainer
          obj.getprop(PROP_HS_C_SC_BOX)
        ) and
        // im eigenen Haus oder im Haus eines Freundes
        IsHouseOwnerOrFriend(house, who)
      )
        return 1;
      EndIf
    EndIf

  ElseIf (obj.isa(POLCLASS_MOBILE))
    If (obj.serial == who.serial)
      // direkt dabei (Kleidung, Waffen etc.)
      return 1;
    EndIf
  EndIf

  // nicht meins
  return 0;
EndFunction

///////////////////////////////////////////////////////////////////
// FindItemInContainerTree - Sucht ein Item rekursiv im Container
///////////////////////////////////////////////////////////////////

Function FindItemInContainerTree(container, objtype)
	var item:=FindSubstance(container,objtype,1,0);
	If (item[1])
		Return(item[1]);
	EndIf
EndFunction

//////////////////////////////////////////////////////////////////////
// FindItemsInContainerTree - Sucht alle Items rekursiv im Container
//////////////////////////////////////////////////////////////////////

Function FindItemsInContainerTree(container, objtype)
  Return(FindSubstance(container,objtype,1,0,FINDSUBSTANCE_FIND_ALL));
EndFunction

// Ermittelt Obercontainer, falls Item verpackt ist
Function GetTopContainer(obj, return_obj_of_no_container := 0) // {{{
  If (!obj.container and !return_obj_of_no_container)
    var ret := error;
    ret.errortext := "Object not in a container";
    return ret;
  EndIf

  While (obj.container)
    SleepMs(2);
    obj := obj.container;
  EndWhile

  return obj;
EndFunction // }}}

// Prueft, ob Tool in Hand ist, versucht es ggf. zu nehmen
Function ToolEquipped(who, tool, layer := LAYER_HAND2)
    // Werkzeuge hat man typischerweise in der zweiten Hand
    var inhand := GetEquipmentByLayer(who, layer);

    If (inhand and inhand.serial == tool.serial)
        // man hat Werkzeug bereits in der Hand
        return 1;
    EndIf

    If (inhand.errortext == "Nothing equipped on that layer.")
        // Hand frei, in die Hand nehmen
        If (EquipItem(who, tool))
            return 1;
        EndIf
    EndIf

    return 0;
EndFunction

///////////////////////////////////////////////////////
// UpdateBlackList - aktualisiert Blacklist
///////////////////////////////////////////////////////
Function UpdateBlackList(who, objtype, addrm := BLACKLIST_TOGGLE) // {{{
  var blacklist := GetObjProperty(who, PROP_GRAB_BLACKLIST);
  If (TypeOfInt(blacklist) != OT_ARRAY)
    blacklist := array{};
  EndIf

  var in_list := 0;
  If (objtype in (blacklist))
    in_list := 1;
  EndIf

  If (addrm == BLACKLIST_ADD)
    If (!in_list)
      // nur einfuegen, falls noch nicht vorhanden
      blacklist.append(objtype);
    Else
      SendSysMessagePergon(who,
        "Dieses Item wird bereits nicht mehr eingesammelt!"
      );
    EndIf
  ElseIf (addrm == BLACKLIST_TOGGLE and !in_list)
    blacklist.append(objtype);
  ElseIf (
    (addrm == BLACKLIST_REMOVE) or
    (addrm == BLACKLIST_TOGGLE and in_list)
  )
    var newlist := array{};
    ForEach itemtype in (blacklist)
      SleepMS(2);
      If (itemtype != objtype)
        newlist.append(itemtype);
      EndIf
    EndForEach
    blacklist := newlist;
  EndIf

  SetObjProperty(who, PROP_GRAB_BLACKLIST, blacklist);
EndFunction // }}}

///////////////////////////////
//                           //
//   Werkzeug-Manipulation   //
//                           //
///////////////////////////////

////////////////////////////////////////////////////
// Werkzeug-Eigenschaften aller Items ueberpruefen
////////////////////////////////////////////////////

Function CheckWholePropertyConfig()
  Var itemdesc:=ReadConfigFile("::itemdesc");
  If (itemdesc)
    ForEach objtype in GetConfigIntKeys(itemdesc)
      Var item:=itemdesc[objtype];
      If (item.type)
        If (item.type in {"b", "c", "s", "t"})
          If ((!item.quality) And (item.quality<>0))
            SysLog("FEHLER: Item "+item.desc+" ("+Hex(objtype)+") hat keine Quality!");
          ElseIf (!IsNumericDbl(item.quality))
            SysLog("FEHLER: Item "+item.desc+" ("+Hex(objtype)+") hat ungueltige Quality ["+item.quality+"] !");
          EndIf
          If ((!item.maxhp) And (item.maxhp<>0))
            SysLog("FEHLER: Item "+item.desc+" ("+Hex(objtype)+") hat keine MaxHP!");
          ElseIf (!IsNumeric(item.maxhp))
            SysLog("FEHLER: Item "+item.desc+" ("+Hex(objtype)+") hat ungueltige MaxHP ["+item.maxhp+"] !");
          EndIf
          If ((!item.attribute2) And (item.attribute2<>0))
            SysLog("FEHLER: Item "+item.desc+" ("+Hex(objtype)+") hat keine SkillID!");
          ElseIf (!IsSkillNameDefinied(item.attribute2))
            SysLog("FEHLER: Item "+item.desc+" ("+Hex(objtype)+") hat ungueltige SkillID ["+item.attribute2+"] !");
          EndIf
          If ((!item.skillrepair) And (item.skillrepair<>0))
            SysLog("FEHLER: Item "+item.desc+" ("+Hex(objtype)+") hat keinen SkillRepair!");
          ElseIf (!IsNumeric(item.skillrepair))
            SysLog("FEHLER: Item "+item.desc+" ("+Hex(objtype)+") hat ungueltigen SkillRepair ["+item.skillrepair+"] !");
          EndIf
        ElseIf (!IsNumeric(item.type))
          SysLog("FEHLER: Item "+item.desc+" ("+Hex(objtype)+") hat ungueltigen Type ["+item.type+"] !");
        EndIf
      EndIf
    EndForEach
  Else
    SysLog("FEHLER: CheckWholePropertyConfig: Konnte die '::itemdesc' nicht oeffnen!");
  EndIf
EndFunction

///////////////////////////////////////////////////
// GetPropertyItem - Werkzeug-Eigenschaften lesen
///////////////////////////////////////////////////

Function GetPropertyItem(item)
  Var props:=struct;

  If (item.isa(POLCLASS_WEAPON) Or item.isa(POLCLASS_ARMOR)) // Waffe oder Ruestung
    Var werkzeug:=GetObjProperty(item, "Werkzeug");
    If (werkzeug) // Initialisiert
      props.+quality:=item.quality;
      props.+hp:=item.hp;
      props.+maxhp:=item.maxhp;
      props.+qualityorig:=werkzeug[1];
      props.+maxhporig:=werkzeug[2];
      props.+type:=werkzeug[3];
      props.+skillid:=werkzeug[4];
      props.+skillrepair:=werkzeug[5];
      props.+price:=werkzeug[6];
    Else // NPC
      Var itemdesc:=ReadConfigFile("::itemdesc");
      If (itemdesc)
        Var elem:=itemdesc[item.objtype];
        If (elem)
          props.+quality:=item.quality;
          props.+hp:=item.maxhp;
          props.+maxhp:=item.maxhp;
          props.+qualityorig:=elem.quality;
          props.+maxhporig:=elem.maxhp;
          props.+type:=elem.type;
          props.+skillid:=elem.attribute2;
          props.+skillrepair:=elem.skillrepair;

          If (elem.vendorbuysfor)
            props.+price:=elem.vendorbuysfor;
          Else
            props.+price:=0;
          EndIf
        Else
          SysLog("FEHLER: GetPropertyItem: "+item.objtype+" wurde nicht in der '::itemdesc' gefunden!");
        EndIf
      Else
        SysLog("FEHLER: GetPropertyItem: Konnte die '::itemdesc' nicht oeffnen!");
      EndIf
    EndIf
  ElseIf (item.isa(POLCLASS_ITEM)) // Gegenstand
    Var werkzeug:=GetObjProperty(item, "Werkzeug");
    If (werkzeug) // Initialisiert
      props.+quality:=werkzeug[1];
      props.+hp:=werkzeug[2];
      props.+maxhp:=werkzeug[3];
      props.+qualityorig:=werkzeug[4];
      props.+maxhporig:=werkzeug[5];
      props.+type:=werkzeug[6];
      props.+skillid:=werkzeug[7];
      props.+skillrepair:=werkzeug[8];
      props.+price:=werkzeug[9];
    Else // NPC
      Var itemdesc:=ReadConfigFile("::itemdesc");
      If (itemdesc)
        Var elem:=itemdesc[item.objtype];
        If (elem)
          props.+quality:=elem.quality;
          props.+hp:=elem.maxhp;
          props.+maxhp:=elem.maxhp;
          props.+qualityorig:=elem.quality;
          props.+maxhporig:=elem.maxhp;
          props.+type:=elem.type;
          props.+skillid:=elem.attribute2;
          props.+skillrepair:=elem.skillrepair;
          If (elem.skillrepair And elem.vendorbuysfor) // Nur Werkzeug-Item's haben einen dynamischen Preis
            props.+price:=elem.vendorbuysfor;
          Else
            props.+price:=0;
          EndIf
        Else
          SysLog("FEHLER: GetPropertyItem: "+item.objtype+" wurde nicht in der '::itemdesc' gefunden!");
        EndIf
      Else
        SysLog("FEHLER: GetPropertyItem: Konnte die '::itemdesc' nicht oeffnen!");
      EndIf
    EndIf
  Else
    Return (0);
  EndIf

  If (item.isa(POLCLASS_WEAPON) Or item.isa(POLCLASS_ARMOR) Or GetObjProperty(item, "Werkzeug")) // Waffe oder Ruestung oder Werkzeug
    // Werkzeug-Eigenschaften ueberpruefen (Quick'n'Dirty, da nicht an der Ursache gecheckt...)
    Var check:=CheckPropertyItem(item, props.quality, props.hp, props.maxhp, props.qualityorig,
      props.maxhporig, props.type, props.skillid, props.skillrepair, props.price);
    If (check)
      props.quality:=check[1];
      props.hp:=check[2];
      props.maxhp:=check[3];
      props.qualityorig:=check[4];
      props.maxhporig:=check[5];
      props.type:=check[6];
      props.skillid:=check[7];
      props.skillrepair:=check[8];
      props.price:=check[9];

    EndIf
  EndIf
  Return (props);
EndFunction

////////////////////////////////////////////////////
// SetPropertyItem - Werkzeug-Eigenschaften setzen
////////////////////////////////////////////////////

Function SetPropertyItem(item, quality, hp, maxhp, qualityorig, maxhporig, type, skillid, skillrepair, price)
  // Format der HitPoints ueberpruefen und ggf. korrigieren
  If (CInt(hp)<>CDbl(hp))
    SysLog("WARNUNG: Item "+item.desc+" ("+Hex(item.objtype)+"/"+Hex(item.serial)+") hatte Gleitkomma HP!");
    hp:=CInt(hp);
  EndIf
  If (CInt(maxhp)<>CDbl(maxhp))
    SysLog("WARNUNG: Item "+item.desc+" ("+Hex(item.objtype)+"/"+Hex(item.serial)+") hatte Gleitkomma MaxHP!");
    maxhp:=CInt(maxhp);
  EndIf
  If (CInt(maxhporig)<>CDbl(maxhporig))
    SysLog("WARNUNG: Item "+item.desc+" ("+Hex(item.objtype)+"/"+Hex(item.serial)+") hatte Gleitkomma MaxHPOrig!");
    maxhporig:=CInt(maxhporig);
  EndIf

  // Werkzeug-Eigenschaften ueberpruefen und setzen (Quick'n'Dirty, da nicht an der Ursache gecheckt...)
  Var check:=CheckPropertyItem(item, quality, hp, maxhp, qualityorig, maxhporig, type, skillid, skillrepair, price);
  If (check)
    If (item.isa(POLCLASS_WEAPON) Or item.isa(POLCLASS_ARMOR)) // Waffe oder Ruestung
      item.quality:=check[1];
      item.hp:=check[2];
      SetObjProperty(item, "Werkzeug", {check[4], check[5], check[6], check[7], check[8], check[9]});
    ElseIf (item.isa(POLCLASS_ITEM)) // Gegenstand
      SetObjProperty(item, "Werkzeug", {check[1], check[2], check[3], check[4], check[5], check[6], check[7], check[8], check[9]});
    EndIf
    IncRevision(item);
    Return (1);
  EndIf
EndFunction

////////////////////////////////////////////////////////////
// CheckPropertyItem - Werkzeug-Eigenschaften ueberpruefen
////////////////////////////////////////////////////////////

Function CheckPropertyItem(item, quality, hp, maxhp, qualityorig, maxhporig, type, skillid, skillrepair, price)
  If (item.isa(POLCLASS_WEAPON) Or item.isa(POLCLASS_ARMOR) Or item.isa(POLCLASS_ITEM))
    If (!(type in {"b", "c", "s", "t"}))
      SysLog("FEHLER: Item "+item.desc+" ("+Hex(item.objtype)+"/"+Hex(item.serial)+") hat ungueltigen Type ["+type+"] !");
    Else
      Var fehlerfrei:=1;

      If (!quality)
        SysLog("FEHLER: Item "+item.desc+" ("+Hex(item.objtype)+"/"+Hex(item.serial)+") hat keine Quality!");
        quality:=1; // Quick'n'Dirty, da nicht an der Ursache gecheckt...
       // fehlerfrei:=0;
      ElseIf (!IsNumericDbl(quality))
        SysLog("FEHLER: Item "+item.desc+" ("+Hex(item.objtype)+"/"+Hex(item.serial)+") hat ungueltige Quality ["+quality+"] !");
        fehlerfrei:=0;
      EndIf

      If (!hp)
//        SysLog("FEHLER: Item "+item.desc+" ("+Hex(item.objtype)+"/"+Hex(item.serial)+") hat keine HP!");
        hp:=10; // Quick'n'Dirty, da nicht an der Ursache gecheckt...
       // fehlerfrei:=0;
      ElseIf (!IsNumeric(hp))
        SysLog("FEHLER: Item "+item.desc+" ("+Hex(item.objtype)+"/"+Hex(item.serial)+") hat ungueltige HP ["+hp+"] !");
        fehlerfrei:=0;
      EndIf

      If (!maxhp)
//        SysLog("FEHLER: Item "+item.desc+" ("+Hex(item.objtype)+"/"+Hex(item.serial)+") hat keine MaxHP!");
        maxhp:=10; // Quick'n'Dirty, da nicht an der Ursache gecheckt...
       // fehlerfrei:=0;
      ElseIf (!IsNumeric(maxhp))
        SysLog("FEHLER: Item "+item.desc+" ("+Hex(item.objtype)+"/"+Hex(item.serial)+") hat ungueltige MaxHP ["+maxhp+"] !");
        fehlerfrei:=0;
      EndIf

      If (!qualityorig)
        SysLog("FEHLER: Item "+item.desc+" ("+Hex(item.objtype)+"/"+Hex(item.serial)+") hat keine QualityOrig!");
        qualityorig:=1; // Quick'n'Dirty, da nicht an der Ursache gecheckt...
       // fehlerfrei:=0;
      ElseIf (!IsNumericDbl(qualityorig))
        SysLog("FEHLER: Item "+item.desc+" ("+Hex(item.objtype)+"/"+Hex(item.serial)+") hat ungueltige QualityOrig ["+qualityorig+"] !");
        fehlerfrei:=0;
      EndIf

      If (!maxhporig)
//        SysLog("FEHLER: Item "+item.desc+" ("+Hex(item.objtype)+"/"+Hex(item.serial)+") hat keine MaxHPOrig!");
        maxhporig:=10; // Quick'n'Dirty, da nicht an der Ursache gecheckt...
       // fehlerfrei:=0;
      ElseIf (!IsNumeric(maxhporig))
        SysLog("FEHLER: Item "+item.desc+" ("+Hex(item.objtype)+"/"+Hex(item.serial)+") hat ungueltige MaxHPOrig ["+maxhporig+"] !");
        fehlerfrei:=0;
      EndIf

      If (!skillid)
        SysLog("FEHLER: Item "+item.desc+" ("+Hex(item.objtype)+"/"+Hex(item.serial)+") hat keine SkillID!");
        fehlerfrei:=0;
      ElseIf (!IsSkillNameDefinied(skillid))
        SysLog("FEHLER: Item "+item.desc+" ("+Hex(item.objtype)+"/"+Hex(item.serial)+") hat ungueltige SkillID!");
        fehlerfrei:=0;
      EndIf

      If (!skillrepair)
        SysLog("FEHLER: Item "+item.desc+" ("+Hex(item.objtype)+"/"+Hex(item.serial)+") hat keinen SkillRepair!");
        skillrepair:=200; // Quick'n'Dirty, da nicht an der Ursache gecheckt...
       // fehlerfrei:=0;
      ElseIf (!IsNumericDbl(skillrepair))
        SysLog("FEHLER: Item "+item.desc+" ("+Hex(item.objtype)+"/"+Hex(item.serial)+") hat ungueltigen SkillRepair ["+skillrepair+"] !");
        fehlerfrei:=0;
      EndIf

      If (!price)
        SysLog("FEHLER: Item "+item.desc+" ("+Hex(item.objtype)+"/"+Hex(item.serial)+") hat keinen Price!");
        fehlerfrei:=0;
      ElseIf (!IsNumeric(price))
        SysLog("FEHLER: Item "+item.desc+" ("+Hex(item.objtype)+"/"+Hex(item.serial)+") hat ungueltigen Price ["+price+"] !");
        fehlerfrei:=0;
      EndIf

      If (fehlerfrei) // Die ggf. korrigierten Werte zurueckgeben (Quick'n'Dirty, da nicht an der Ursache gecheckt...)
        Return ({CDbl(quality), CInt(hp), CInt(maxhp), CDbl(qualityorig), CInt(maxhporig), type, skillid, skillrepair, CInt(price)});
      EndIf
    EndIf
  EndIf
EndFunction

///////////////////////////////////////////////
// GetPropertyPrice - Verkaufspreis ermitteln
///////////////////////////////////////////////

Function GetPropertyPrice(item)
  If (item)
    Var props:=GetPropertyItem(item);
    If (props.price)
      Return (CInt(props.price*((props.hp*props.quality)/(props.maxhporig*props.qualityorig))));
    Else
      Var itemdesc:=ReadConfigFile("::itemdesc");
      If (itemdesc)
        Var elem:=itemdesc[item.objtype];
        If (elem)
          If (elem.vendorsellsfor)
            Return (elem.vendorsellsfor);
          EndIf
        Else
          SysLog("FEHLER: GetPropertyPrice: "+item.objtype+" wurde nicht in der '::itemdesc' gefunden!");
        EndIf
      Else
        SysLog("FEHLER: GetPropertyPrice: Konnte die '::itemdesc' nicht oeffnen!");
      EndIf
    EndIf
  Else
    SysLog("WARNUNG: GetPropertyPrice: Uebergebener Gegenstand ist undefiniert!");
  EndIf
EndFunction

///////////////////////////////////////////////
// SetWerkzeugSchaden - Werkzeug beschaedigen
///////////////////////////////////////////////

Function SetWerkzeugSchaden(char, item, text_1, text_2, text_destroyed)
  Var werkzeug:=GetPropertyItem(item);
  If (werkzeug)
    If (RandomInt(100)<5) // Zufaelliges Ereignis mit 5% Chance
      If (text_1)
        SendSysMessagePergon(char, text_1);
      EndIf

      werkzeug.hp:=werkzeug.hp-(RandomInt(8)+2);
      If (werkzeug.hp<2)
        SubtractAmount(item, 1);
        If (item)
          SetPropertyItem(
            item, werkzeug.qualityorig, werkzeug.maxhp, werkzeug.maxhp,
            werkzeug.qualityorig, werkzeug.maxhporig, werkzeug.type,
            werkzeug.skillid, werkzeug.skillrepair, werkzeug.price
          );
        EndIf

        If (text_destroyed)
          SendSysMessagePergon(char, text_destroyed);
        EndIf

        Return (1);
      ElseIf ((werkzeug.hp<=15) And text_2)
        SendSysMessagePergon(char, text_2);
      EndIf

      SetPropertyItem(
        item, werkzeug.quality, werkzeug.hp, werkzeug.maxhp,
        werkzeug.qualityorig, werkzeug.maxhporig, werkzeug.type,
        werkzeug.skillid, werkzeug.skillrepair, werkzeug.price
      );
    ElseIf (RandomInt(100)<5) // Zufaelliges Ereignis mit 5% Chance
      If (text_1)
        SendSysMessagePergon(char, text_1);
      EndIf

      werkzeug.quality:=werkzeug.quality-0.1;
      If (werkzeug.quality<0.1)
        SubtractAmount(item, 1);
        If (item)
          SetPropertyItem(
            item, werkzeug.qualityorig, werkzeug.maxhp, werkzeug.maxhp,
            werkzeug.qualityorig, werkzeug.maxhporig, werkzeug.type,
            werkzeug.skillid, werkzeug.skillrepair, werkzeug.price
          );
        EndIf

        If (text_destroyed)
          SendSysMessagePergon(char, text_destroyed);
        EndIf

        Return (1);
      ElseIf ((werkzeug.quality<=0.4) And text_2)
        SendSysMessagePergon(char, text_2);
      EndIf

      SetPropertyItem(
        item, werkzeug.quality, werkzeug.hp, werkzeug.maxhp,
        werkzeug.qualityorig, werkzeug.maxhporig, werkzeug.type,
        werkzeug.skillid, werkzeug.skillrepair, werkzeug.price
      );
    EndIf
  EndIf
EndFunction

//////////////////////////////////////////////////////////////////////////////////////
// CheckWerkzeugSchaden - Werkzeugbeschaedigung pruefen und Werkzeug ggf. zerstoeren
//////////////////////////////////////////////////////////////////////////////////////

Function CheckWerkzeugSchaden(who, item, waiting:=0)
  If (item.isa(POLCLASS_WEAPON) Or item.isa(POLCLASS_ARMOR))
    Var verbrauch:=100*item.hp/item.maxhp;
    If ((verbrauch<=5) Or (item.hp<1))
      SysLog(EntferneA(item.desc)+" ("+Hex(item.serial)+") ist zu stark abgenutzt und zerfaellt.");
      SendSysMessagePergon(who, EntferneA(item.desc)+" ist zu stark abgenutzt und zerfaellt.");

      If (waiting)
        Start_ScriptPergon("::misc/waitdestroyitem", item); // Wenn der Char offline sein kann (z.B. EquipTest)
      Else
        SubtractAmount(item, 1); // sonst (z.B. im Kampf)
      EndIf

      Return (1);
    ElseIf (verbrauch<=10)
      SendSysMessagePergon(who, EntferneA(item.desc)+" ist nur noch Schrott.");
    ElseIf (verbrauch<=20)
      SendSysMessagePergon(who, EntferneA(item.desc)+" sieht schon ziemlich ramponiert aus.");
    EndIf
  EndIf
EndFunction

///////////////////
//               //
//   Sonstiges   //
//               //
///////////////////

///////////////////////////////////////////////////
// EntferneA - Schnippelt das bloeder "a"/"an" ab
///////////////////////////////////////////////////

Function EntferneA(desc)
  If (desc[1, 2]=="a ")
    desc[1, 2]:="";
  ElseIf (desc[1, 3]=="an ")
    desc[1, 3]:="";
  EndIf

  Return (desc);
EndFunction

////////////////////////////////////////
// Endung - Waehlt die richtige Endung
////////////////////////////////////////

Function Endung(menge, einzahl, mehrzahl)
  If (menge==1)
    Return (menge+" "+einzahl);
  Else
    Return (menge+" "+mehrzahl);
  EndIf
EndFunction

/////////////////////////////////////////////////////////////////////
// DescEinzahl - Liefert von der ItemDesc.desc die Einzahl-Variante
/////////////////////////////////////////////////////////////////////

Function DescEinzahl(desc)
  var pos  := Find(desc, "%", 1);
  var ret;

  // erstes '%' suchen
  If (pos)
    // bis zum ersten '%' alles kopieren
    ret := desc[1, pos - 1];

    // bis zum ersten '%' (inkl. '%') alles loeschen
    desc[1, pos] := "";

    // '/' suchen und alles danach uebernehmen
    pos := Find(desc, "/", 1);
    If (pos)
      ret += desc[pos + 1, len(desc)];
      // eventuelles zweites '%' loeschen
      ret["%"] := "";
    Else
      // kein '/' gefunden, also schauen, ob ein zweites '%' vorhanden ist (z.B. 0xF3B, Pferdea%e%pfel)
      pos := Find(desc, "%", 1);
      // zweites '%' gefunden, also alles bis dahin loeschen, Rest uebernehmen
      If (pos)
        desc[1, pos] := "";
        ret += desc;
        Return(ret);
      // kein zweites '%' gefunden, also fertig
      Else
        Return(ret);
      EndIf
    EndIf

    Return (ret);
  // kein '%' vorhanden
  Else
    Return (desc);
  EndIf
EndFunction

/////////////////////////////////////////////////////////////////////
// DescMehrzahl - Liefert von der ItemDesc.desc die Mehrzahl-Variante
/////////////////////////////////////////////////////////////////////

Function DescMehrzahl(desc)
  var pos  := Find(desc, "%", 1);
  var ret;

  // erstes '%' suchen
  If (pos)
    // bis zum ersten '%' alles kopieren
    ret := desc[1, pos - 1];

    // bis zum ersten '%' (inkl. '%') alles loeschen
    desc[1, pos] := "";

    // '/' suchen und alles davor uebernehmen
    pos := Find(desc, "/", 1);
    If (pos)
      ret += desc[1, pos - 1];

      // bis '/' alles loeschen
      desc[1, pos] := "";

      // zweites '%' suchen und alles bis dahin loeschen, Rest uebernehmen
      pos := Find(desc, "%", 1);
      If (pos)
        desc[1, pos] := "";
        ret += desc;
        return ret;
      // kein zweites '%' gefunden; merkwuerdig, aber ok
      Else
        return ret;
      EndIf
    // kein '/' gefunden, also schauen, ob ein zweites '%' vorhanden ist (z.B. 0xF3B, Pferdea%e%pfel)
    Else
      pos := Find(desc, "%", 1);
      // zweites '%' gefunden, also '%' loeschen und Rest uebernehmen
      If (pos)
        desc["%"] := "";
        ret += desc;
        return ret;
      // kein zweites '%' gefunden, also Rest uebernehmen und fertig
      Else
        ret += desc;
        return ret;
      EndIf
    EndIf

  // kein '%' vorhanden
  Else
    return desc+"s";
  EndIf
EndFunction
// Nachbau der Corefunktion
// DescEinzahl/DescMehrzahl stimmte in allen getesteten F‰llen
// Aber falls doch mal was nicht passt zu Dokuzwecken:
//Function FormatDescription(name,Plural:=0)
//  var i;
//  Var desc:="";
//  Var phase:=0;
//  For(i:=1;i<=Len(name);i+=1)
//    If (phase==0)
//      If (name[i]=="%")
//        phase:=1;
//      Else
//        desc+=name[i];
//      Endif
//    ElseIf (phase==1)
//      If (name[i]=="%")
//        phase:=3;
//      ElseIf (name[i]=="/")
//        phase:=2;
//      ElseIf (Plural)
//        desc+=name[i];
//      EndIf
//    ElseIf (phase==2)
//      If (name[i]=="%")
//        phase:=3;
//      ElseIf (!Plural)
//        desc+=name[i];
//      EndIf
//    Else //phase 3
//      desc+=name[i];
//    EndIf
//  EndFor
//  Return(desc);
//EndFunction


/////////////////////////////////////////////////////////////////////
// GetTileDataInfo - Liefert tiledata.mul Eintrag als struct
// TileID<0x4000 landtiles TileID>0x3FFF items
/////////////////////////////////////////////////////////////////////

Function GetTileDataInfo(TileID)
  Var polcfg:=GetGlobalProperty(GLOBAL_POL_CFG_STRUCT);
  Var tiledata:=OpenBinaryFile( polcfg.UoDataFileRoot+"tiledata.mul", OPENMODE_IN, LITTLE_ENDIAN );
  If (!tiledata)
    Return(error);
  EndIf
  Var pos;
  If (TileID<0x4000) // landtiles
    pos:=(((TileID >> 5) + 1) * 4) + (TileID * 26);
  Else
    Var tmpid:=TileID-0x4000;
    tiledata.seek(0x68800,SEEKDIR_BEG);
    pos:=(((tmpid >> 5) + 1) * 4) + (tmpid * 37);
  EndIf
  tiledata.seek(pos,SEEKDIR_CUR);
  Var tile:=struct;
  tile.+ID        :=TileID;
  tile.+flags     :=tiledata.getint32(); //uo::TILEDATA_FLAG_*
  If (TileID<0x4000)
    tile.+texid   :=tiledata.getint16();
  Else
    tile.+weight  :=tiledata.getint8();
    tile.+quality :=tiledata.getint8(); // wearable items: Layer
    tile.+unk1    :=tiledata.getint16();
    tile.+unk2    :=tiledata.getint8();
    tile.+quantity:=tiledata.getint8();
    tile.+anim    :=tiledata.getint16();
    tile.+unk3    :=tiledata.getint8();
    tile.+hue     :=tiledata.getint8();
    tile.+stackoff:=tiledata.getint8();
    tile.+value   :=tiledata.getint8();
    tile.+height  :=tiledata.getint8(); // if Flag Bridge height/2
  EndIf
  tile.+name      :=tiledata.getstring(20);
  tiledata.close();
  Return(tile);
EndFunction

// vim: sw=2 sts=2
