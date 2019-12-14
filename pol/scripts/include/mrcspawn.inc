///////////////////////////////////////////////////////////////////////////
// MRCSpawn Include v1.01 (2000/10/25)
//
// based on MRCSpawn Include
//
// Author: Shinigami
//
// Modifications:
//   2000/10/25 Shinigami: Complete ReWrite
//                         SellLists added
//   2000/12/09 Shinigami: HP/Armor w'modify Price
//   2001/01/07 Shinigami: Minor Bug fixed
//                         MultiLanguage ErrorMessages
//   2001/01/08 Shinigami: All sellable Items -> Movable
//   2001/10/11 Pauker:    Werkzeugverkauf
//   2001/10/30 Shinigami: Werkzeugverkauf gefixt
//   2001/11/14 Shinigami: UnMovable-Fix (in Merchant.inc)
//   2001/12/08 Pauker:    Fuellbare Gefaesse eingebaut
//   2002/04/.. Pauker:    Tierzucht, Property "Generation" am Animal bringt 1..20% zum EK dazu
//   2003/01/13 Shinigami: Anpassung an Pol095 (buyprice:=0 ...)
//   2003/02/02 Shinigami: Quick'n'Dirty-Methode zur Fehlerbeseitigung eingebaut
//
///////////////////////////////////////////////////////////////////////////
//
//   price =  0 - Gegenstand ist un[ver]kaeuflich
//   price >  0 - Preis entspricht genau diesem Wert (1..viel :)
//                groesser 65535 kann nicht verkauft werden (Bug?!?)
//   price = -1 - Preis entspricht dem Wert aus der ItemDesc.cfg
//   price < -1 - ungueltig
//
// ToDo: use stackability to create single items or add to stacks.
//       stacks are always added to, currently.
//       calling CreateItemInContainerPergon 'count' times would do this
//       automatically, actually
//
// this would be nice, and fast, but this doesn't handle multiple stacks.
// in particular, it doesn't handle multiple single nonstackable items.
//   Var itemref:=FindObjtypeInContainer(inventory, objtype);
//   If (itemref)
//     count:=count-itemref.amount;
//   EndIf

////////////
// Library
////////////

Use cfgfile;

/////////////
// Includes
/////////////

Include "include/errormsg";
Include "include/objtype";

///////////////////
// Error-Messages
///////////////////

Const ERRORMSGS_GERMAN:=
  "{}MRCSpawn: Das Item %% konnte nicht erzeugt werden!"+
  "{}MRCSpawn: Items in der Sells-Gruppe '%%' sind fehlerhaft definiert!"+
  "{}MRCSpawn: Kann keine Eintraege des MerchantTypes '%%' finden (Template = '%%') !"+
  "{}MRCSpawn: Kann die Datei 'mrcspawn.cfg' nicht oeffnen !"+
  "{}MRCSpawn: Items in der Gruppe '%%' sind fehlerhaft definiert!";
Const ERRORMSGS_ENGLISH:=
  "{}MRCSpawn: Unable to create Item %% !"+
  "{}MRCSpawn: Items in Sells-Group '%%' are definied in a wrong way!"+
  "{}MRCSpawn: Unable to find MerchantType '%%'!"+
  "{}MRCSpawn: Unable to open 'mrcspawn.cfg'!"+
  "{}MRCSpawn: Items in Group '%%' are definied in a wrong way!";

//////////////
// Constants
//////////////

Const HP_ARMOR      :=  1; // 1=Analyze HP/Armor for Calculating Price
Const NOBUY_ITEM    :=  0; // NPC kauft das Item vom Player nicht an
Const BUY_ITEM      := -2; // NPC kauft das Item vom Player an

Const DEFAULT_PRICE := -1; // Den Preis aus der ItemDesc holen
                           // (oder "4294967295")

//////////////////////////////////////////////////
// RestockInventory - Restock Vendor's Inventory
//////////////////////////////////////////////////

Function RestockInventory(npctemplate, merchanttype, inventory)
  Var mrcspawnfile:=ReadConfigFile("mrcspawn");
  If (mrcspawnfile)
    Var allproductgroups:=FindConfigElem(mrcspawnfile, merchanttype);
    If (allproductgroups)
      Var sellitem;
      Var objtype;
      Var count;
      Var item;
      Var i;

      ForEach productgroup in GetConfigStringArray(allproductgroups, "Sells")
        ForEach itemstr in GetConfigStringArray(FindConfigElem(mrcspawnfile, productgroup), "Item")
          SleepMS(100);

          sellitem:=SplitWords(itemstr);
          objtype:=sellitem[1];
          If (CInt(objtype))
            objtype:=CInt(objtype);
          Else
            objtype:=GetObjtypeByName(objtype);
          EndIf

          If (objtype)
            count:=CInt(sellitem[2]);
            ForEach item in EnumerateItemsInContainer(inventory)
              If (item.objtype==objtype)
                count-=item.amount;
              EndIf
              SleepMS(2);
            EndForEach

            If (count>0)
              For (i:=1; i<=count; i+=1)
                item:=CreateItemInInventoryPergon(inventory, objtype, 1);
                If (!item)
                  PrintErrorMessage(1, {Hex(objtype)});
                EndIf
              EndFor
            EndIf
          Else
            PrintErrorMessage(2, {productgroup});
          EndIf
        EndForEach
        SleepMS(2);
      EndForEach
    Else
      PrintErrorMessage(3, {merchanttype, npctemplate});
    EndIf

    UnloadConfigFile("mrcspawn");
  Else
    PrintErrorMessage(4);
  EndIf
EndFunction

///////////////////////////////////////////////////////////////
// ModifyPCSellList - Mark sellable Items in Players Backpack
///////////////////////////////////////////////////////////////

Function ModifyPCSellList(npctemplate, merchanttype, who)
  Var backpackitems:=EnumerateItemsInContainer(who.backpack,ENUMERATE_ROOT_ONLY);

  Var mrcspawnfile:=ReadConfigFile("mrcspawn");
  If (mrcspawnfile)
    Var allproductgroups:=FindConfigElem(mrcspawnfile, merchanttype);
    If (allproductgroups)
      Var buys:=GetConfigStringArray(allproductgroups, "Buys");
      Var nobuys:=GetConfigStringArray(allproductgroups, "NoBuys");

      Var iteminit:=BUY_ITEM; // Buys everything
      Var itemmark;
      Var productgroups:={ };

      If (buys.size()>=1) // Buys nothing, but defined Items
        iteminit:=NOBUY_ITEM;itemmark:=BUY_ITEM;
        productgroups:=buys;
      ElseIf (nobuys.size()>=1) // Buys everything, but not defined Items
        iteminit:=BUY_ITEM;itemmark:=NOBUY_ITEM;
        productgroups:=nobuys;
      EndIf

      ForEach item in backpackitems
        item.buyprice:=iteminit;
      EndForEach

      Var buynobuyable:={ };
      Var objtype;

      If (productgroups.size())
        ForEach productgroup in productgroups
          ForEach itemstr in GetConfigStringArray(FindConfigElem(mrcspawnfile, productgroup), "Item")
            objtype:=itemstr;
            If (CInt(objtype))
              objtype:=CInt(objtype);
            Else
              objtype:=GetObjtypeByName(objtype);
            EndIf

            If (objtype)
              buynobuyable.append(objtype);
            Else
              PrintErrorMessage(5, {productgroup});
            EndIf
          EndForEach
          SleepMS(2);
        EndForEach

        If (buynobuyable.size())
          ForEach item in backpackitems
            If (item.objtype in buynobuyable)
              item.buyprice:=itemmark;
            EndIf
          EndForEach
        EndIf
      EndIf

      SetItemPrice(who, backpackitems); // Preis der Gegenstaende setzen
    Else // Buys nothing
      ForEach item in backpackitems
        item.buyprice:=NOBUY_ITEM;
      EndForEach

      PrintErrorMessage(3, {merchanttype, npctemplate});
    EndIf

    UnloadConfigFile("mrcspawn");
  Else // Buys nothing
    ForEach item in backpackitems
      item.buyprice:=NOBUY_ITEM;
    EndForEach

    PrintErrorMessage(4);
  EndIf
EndFunction

////////////////////////////////////////////////////
// SetItemPrice - Setzt den Preis der Gegenstaende
////////////////////////////////////////////////////

Function SetItemPrice(who, backpackitems)
  If (HP_ARMOR) // Waffen-, Ruestungs- und Werkzeugspezifische Preise berechnen
    Var itemdesc:=ReadConfigFile("::itemdesc");

    ForEach item in backpackitems
      SleepMS(2);
      If (item.buyprice==BUY_ITEM) // Darf das Item verkauft werden, sprich: Ist ein Preis gesetzt?
        Var iteminfo:=itemdesc[item.objtype];

        If (item.isa(POLCLASS_WEAPON)) // Waffe

          Var werkzeug:=GetObjProperty(item, "Werkzeug");
          If (werkzeug) // Hergestellte Waffe
            Var divisor:=werkzeug[2]*werkzeug[1];
            If (divisor) // Sind die Werte falsch gesetzt? (Quick'n'Dirty, da nicht an der Ursache gecheckt...)
              item.buyprice:=CInt((werkzeug[6]*item.hp*item.quality)/divisor);
            Else // ...dann lieber den normalen Preis nehmen
              item.buyprice:=CInt(werkzeug[6]);
            EndIf
          Else // NPC-Waffe
            item.buyprice:=CInt(iteminfo.vendorbuysfor*item.hp/iteminfo.maxhp);
          EndIf

        ElseIf (item.isa(POLCLASS_ARMOR)) // Ruestung

          Var werkzeug:=GetObjProperty(item, "Werkzeug");
          If (werkzeug) // Hergestellte Ruestung
            Var divisor:=werkzeug[2]*werkzeug[1];
            If (divisor) // Sind die Werte falsch gesetzt? (Quick'n'Dirty, da nicht an der Ursache gecheckt...)
              item.buyprice:=CInt((werkzeug[6]*item.hp*item.quality)/divisor);
            Else // ...dann lieber den normalen Preis nehmen
              item.buyprice:=CInt(werkzeug[6]);
            EndIf
          Else // NPC-Ruestung
            item.buyprice:=CInt(iteminfo.vendorbuysfor*item.ar/iteminfo.ar);
          EndIf

        Else // Normaler Gegenstand

          Var werkzeug:=GetObjProperty(item, "Werkzeug");
          If (werkzeug) // Hergestellter Gegenstand
            Var divisor:=werkzeug[5]*werkzeug[4];
            If (divisor) // Sind die Werte falsch gesetzt? (Quick'n'Dirty, da nicht an der Ursache gecheckt...)
              item.buyprice:=CInt((werkzeug[9]*werkzeug[2]*werkzeug[1])/divisor);
            Else // ...dann lieber den normalen Preis nehmen
              item.buyprice:=CInt(werkzeug[9]);
            EndIf
          Else
            If (item.objtype in {UOBJ_ALCHYPOTION,UOBJ_RECIPE}) // Beide haben schon direkt den Preis gesetzt
              item.buyprice:=item.getprop("potion").buyprice;
            Else
              item.buyprice:=DEFAULT_PRICE; // Den Preis aus der ItemDesc holen...
            EndIf
          EndIf

          Var filled:=GetObjProperty(item, "filled");
          If (filled) // ob es ein mit ... gefuelltes Gefaess ist
            If (iteminfo.vendorbuysfilledfor)
              item.buyprice:=CInt(item.buyprice+filled*iteminfo.vendorbuysfilledfor);
            Else
              SysLog("FEHLER: Fuer Gefaess "+Lower(Hex(item.objtype))+" fehlt 'VendorBuysFilledFor'!");
            EndIf
          EndIf

          Var generation:=GetObjProperty(item, "Generation");
          If (generation) // Wenn es ein gezuechtetes Tier ist... 1..20 bringt 1..20% zum EK Preis dazu
            item.buyprice:=CInt(item.buyprice*(1+generation/100));
          EndIf

        EndIf

        If (item.buyprice<-1) // Gab es trotz aller Checks 'ne fehlerhafte Preisbestimmung?
          SysLog("FEHLER: Irgendwas lief bei der Preisbestimmung schief (Item="+item.desc+
            " ["+Lower(Hex(item.objtype))+"] / Werkzeug = "+GetObjProperty(item, "Werkzeug")+") !");
          item.buyprice:=NOBUY_ITEM; // Gegenstand als unverkaeuflich deklarieren
        EndIf
      EndIf
    EndForEach
  EndIf
  return;
  // TODO: Haendlerabhaengige Preise
  // Compiler ruhigstellen
  who := who;
EndFunction
