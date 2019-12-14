///////////////////////////////////////////////////////////////////////////
// Traps.inc - by Fraggulus
//
// Zentrale Regulierung der Effekte
//

///////////////////////////////////////////////////////////////////////////
// Modifiziert:
// $Log: not supported by cvs2svn $
// Revision 1.19  2008/08/09 16:15:45  mehdorn
// - Giftfallen repariert
// - bei Detonation/Entschaerfen einer magischen Falle wird Caster-Name nun
//   wieder entfernt
// - magisches Entschärfen gefixt
//
// 19.01.2006 Turley:
// kleiner Tippfehler
//
// 19.01.2006 Fraggulus:
// in UnTrapSuccess() fehlenden Check auf herkoemmliche (Truhen-)Fallen
// eingebaut
//
// 27.02.2004 Zork:
// Funktion CheckTraps
//
// 17.01.2004 Peter -pJOTTR- Straube:
// gebaut?

include "include/gumps";
use cfgfile;
use polsys;
use uo;


///////////////////////////
// Constants and variables
///////////////////////////
Const TrapGumpScreenWidth    := 800;  // assumed For centering the gump
Const TrapGumpScreenHeight   := 600;
Const MAX_TRAPS_LANDFALLEN   := 20;    // maximale Anzahl der Fallen
Const MAX_TRAPS_SEEFALLEN    := 5;
Const MAX_TRAPS_KISTENFALLEN := 20;
Const MAX_TRAPS_LOCHFALLEN   := 10;
Const MAX_TRAPS_GRUBEN       := 2;

// Deklaration der Fallentypen aequivalent zur traps.cfg!
Const TRAP_TYPE_LANDFALLEN := 1;
Const TRAP_TYPE_SEEFALLEN := 2;
Const TRAP_TYPE_KISTENFALLEN := 3;
Const TRAP_TYPE_LOCHFALLEN := 4;
Const TRAP_TYPE_GRUBEN := 5;

var TrapGumpWidth;
var TrapGumpHeight;
var MAXTRAPS := Dictionary{
    TRAP_TYPE_LANDFALLEN->   MAX_TRAPS_LANDFALLEN,
    TRAP_TYPE_SEEFALLEN->    MAX_TRAPS_SEEFALLEN,
    TRAP_TYPE_KISTENFALLEN-> MAX_TRAPS_KISTENFALLEN,
    TRAP_TYPE_LOCHFALLEN->   MAX_TRAPS_LOCHFALLEN,
    TRAP_TYPE_GRUBEN->       MAX_TRAPS_GRUBEN
};

Function PlayTrapEffect(trap)
  var itemdesc := ReadConfigFile("itemdesc");
  var effect := itemdesc[trap.objtype].effect;
  var effectspeed := itemdesc[trap.objtype].effectspeed;
  var effectlen := itemdesc[trap.objtype].effectlength;
  var dieroll := itemdesc[trap.objtype].damage;

  If (!effectlen)
    effectlen := 0xf;
  EndIf

  If (!effectspeed)
    effectspeed := 0xa;
  EndIf

  If (effect)
    PlayStationaryEffect(trap.x, trap.y, trap.z, effect, effectspeed, effectlen,0,trap.realm);
  EndIf

  return (dieroll);
EndFunction

Function CreateTrapMenuFromCFG(who, file, menu, skillid, skillmod)

  var itemdesc := ReadConfigFile("::itemdesc");
  var cfg := ReadConfigFile(file);
  If (!cfg)
    syslog("Fehler: Konnte Configfile "+file+" nicht laden.");
    return;
  EndIf

  var menuinfo := cfg[menu];
  If (!menuinfo)
    menuinfo := {};   // default menu entry
    syslog("Warnung: Der Menueeintrag "+menu+" existiert im Configfile "+file+" nicht! Benutze Defaultwerte.");
  EndIf

  var window := menuinfo.size;    // Size 5 3   (space For 5 columbs and 3 rows of icons by default)
  If (window)
    window := splitWords(window);
    window[1] := CInt(window[1]);
    window[2] := CInt(window[2]);
  EndIf
  If (!window || window[1] < 1 || window[2] < 1)
    window := { 5, 3 };
  EndIf
  var size := menuinfo.entries;   // Entries 5 3  (use 5 columbs and 3 rows of icons by default)
  If (size)
    size := splitWords(size);
    size[1] := CInt(size[1]);
    size[2] := CInt(size[2]);
  EndIf
  var entriesPerPage := size[1] * size[2];
  If (!entriesPerPage || entriesPerPage < 1)
    size := { 5, 3 };
    entriesPerPage := size[1] * size[2];
  EndIf
  var typeMax := 0, typeMin := 1;     // limits a menu by type number
  If (menuinfo.typeMax)
    typeMax := CInt(menuinfo.typeMax);
  EndIf
  If (menuinfo.typeMin)
    typeMin := CInt(menuinfo.typeMin);
  EndIf

  var menunames := GetConfigStringArray(menuinfo, "Type");
  var cfgkeys := GetConfigIntKeys(cfg);
  If (!cfgkeys)
    syslog("Fehler: Keine Item-Einträge im Configfile "+file+" gefunden.");
    return;
  EndIf

  var defaultPage := 1;
  var defaultItem := 0;

  var content := array;
  var page;

  var nextPage := 1, pages := dictionary;
  var menus := array;

  // Alle Einträge in die Menüs einsortieren
  var entry, objentry, showItem, type;
  var skill1, skill2, skillid1, skillid2;
  var pagename;
  var obj := struct{ objtype, icon, desc, skill, key };

  ForEach key in cfgkeys
    entry := cfg[key];

    showItem := 1;
    If  (!entry.type)
      type := 1;
    Else
      type := entry.type;
    EndIf

    // Soll der Eintrag überhaupt angezeigt werden?
    If (entry.desc)
      If (type < typeMin || (typeMax && type > typeMax))
        showItem := 0;
      Else

        // defaultitem's werden immer angezeigt, auch wenn zuwenig Skill da ist
        If (entry.defaultitem != 1)
          skill1 := entry.skill;
          If (!skill1)
            skill1 := entry.skill1;
          EndIf
          skill2 := entry.skill2;

          skillid1 := skillid;
          If (entry.attribute1)
            skillid1 := GetSkillName(entry.attribute1);
          EndIf
          If (entry.attribute)
            skillid2 := GetSkillName(entry.attribute);
          EndIf
          If (entry.attribute2)
            skillid2 := GetSkillName(entry.attribute2);
          EndIf

          If (skillid1 && entry.defaultitem != 1)
            If (GetSkillFloatPergon(who, skillid1)<(skill1+skillmod))
              showItem := 0;
            EndIf
          EndIf
          If (skillid2 && entry.defaultitem != 1)
            If (GetSkillFloatPergon(who, skillid2)<(skill2))
              showItem := 0;
            EndIf
          EndIf
        EndIf
      EndIf
    Else
      syslog("Warnung: Item "+key+" im Configfile "+file+" hat keinen desc-Eintrag.");
      showItem := 0;
    EndIf

    If (showItem)
      page := pages[type];
      If (!page)
        pages.insert(type, nextPage);
        pagename := menunames[type-typeMin+1];
        If (!pagename)
          pagename := "Sonstiges";
        EndIf
        menus.insert(nextPage, pagename);
        content.insert(nextPage, array);
        page := nextPage;
        nextPage += 1;
      Else
        If (content[page].size() >= entriesPerPage)
          pages[type] := nextPage;
          pagename := menunames[type-typeMin+1];
          If (!pagename)
            pagename := "Sonstiges";
          EndIf
          menus.insert(nextPage, pagename);
          content.insert(nextPage, array);
          page := nextPage;
          nextPage += 1;
        EndIf
      EndIf

      obj.objtype := entry.objtype;
      obj.icon := entry.icon;
      obj.desc := entry.desc;
      obj.skill := entry.skill;
      If (!obj.skill && entry.skill1)
        obj.skill := entry.skill1;
      EndIf
      obj.key := key;

      If (!obj.objtype) // in some CFGs there are no 'objtype' tags
        obj.objtype := key;
      EndIf

      If (!obj.icon)
        objentry := itemdesc[obj.objtype];
        If (objentry && objentry.graphic)
          obj.icon := objentry.graphic;
        Else
          obj.icon := obj.objtype;
        EndIf
      EndIf

      content[page].append(obj);

      If (obj.objtype == defaultItem)
        defaultPage := page;
      EndIf

      SleepMS(3);
    EndIf
  EndForEach

  // Falls das Menü leer ist, soll CInt(0) zurückgegeben werden.
  If (!content.size())
    return 0;
  EndIf

  // Sortierung nach Skillwert (absteigend)
  ForEach submenu in content
    submenu := SortBySkill(submenu);
  EndForEach
  var width := 60 + window[1]*110;
  var height := 100+ window[2]*100 + TrapMenuLines(menus, 0, width);
  var buttons := TrapInitGump(width, height, menuinfo.title);
  var startY := TrapMenuLines(menus, defaultPage);
  var options := TrapMenuEntrys(content, size[1], size[2], defaultItem, defaultPage, startY);

  return struct{ buttons := buttons, options := options };
EndFunction

Function AskForViech(oWho,TrapType)
  //Erstmal Liste aus Config bauen, aller möglichen Tiere
  var oCfgFile := ReadConfigFile(":traps:create");
  var oCfgFileTracking := ReadConfigFile(":tracking:tracking");
  If (!oCfgFile)
    syslog("Fehler: ConfigFile create.cfg in Traps konnte nicht geladen werden");
    return 0;
  EndIf
  If(!oCfgFileTracking)
    syslog("Fehler: ConfigFile Tracking in Traps konnte nicht geladen werden");
    return 0;
  EndIf

  var dPosViechers := dictionary;
  ForEach viech in (oWho.getprop("Traps_Skill").CatchedViechs)
    If (oCfgFile[_viech_iter].Amount < viech)
      //Wasser und Landviechers nach Fallenart filtern
      If (oCfgFile[_viech_iter].type == "land" AND TrapType!=TRAP_TYPE_SEEFALLEN)
          var objtype := GetNPCConfig(oCfgFile[_viech_iter].NpcTemplate).ObjType;
          dPosViechers[_viech_iter] := oCfgFileTracking[objtype].tile;
        ElseIf (oCfgFile[_viech_iter].type == "sea" AND TrapType == TRAP_TYPE_SEEFALLEN)
          var objtype := GetNPCConfig(oCfgFile[_viech_iter].NpcTemplate).ObjType;
          dPosViechers[_viech_iter] := oCfgFileTracking[objtype].tile;
        EndIf
    EndIf
  EndForEach


  var layout := {"nodispose",
               "page 0",
               "resizepic 0 0 2600 620 390",
               "resizepic 50 25 5100 520 25",
               "htmlgump 0 28 620 48 0 0 0",
               "page 1"};
  var data := {"<center>Welches Vieh?</center>"};

  var num := 0;
  var x := 120,y := 90,page := 1;
  ForEach vieh in dPosViechers
    num += 1;
    layout.append("tilepic "+x+" "+y+" "+CInt(vieh));
    layout.append("button "+(x-20)+" "+(y-19)+" 9026 9027 1 0 "+CInt(_vieh_iter));
    x += 90;
    If (num%5 == 0) // Zeilenwechsel
      x := 120;
      y += 100;
    EndIf
    If (num%15 == 0) // Pagewechsel
      page += 1;
      layout.append("button 540 29 9762 9763 0 "+page+" 0");
      layout.append("page "+page);
      layout.append("button 70 29 9766 9767 0 "+(page-1)+" 0");
      x := 120;
      y := 90;
    EndIf
  EndForEach

  var ret := SendDialogGump(oWho,layout,data,50,50);
  If (ret.keys)
    If (dPosViechers[ret.keys[2]]) // Sicherheitscheck ob wirklich vorhanden
      return(ret.keys[2]);
    Else
      SendSysMessagePergon(oWho,"Abbruch","Abort");
      return(0);
    EndIf
  Else
    SendSysMessagePergon(oWho,"Abbruch","Abort");
    return(0);
  EndIf
EndFunction

///////////////////////////////////////////////////////////
// sorts a list of structs by ".skill" (simple BubbleSort)
///////////////////////////////////////////////////////////
// params:
// list of structs with skill member
// return: list, sorted

Function SortBySkill(list)
  var size := list.size();
  var i, modified;

  Repeat
    modified := 0;

    For(i := 1; i < size; i += 1)
      If (list[i].skill < list[i+1].skill)
        modified := 1;

        var obj := list[i];
        list[i] := list[i+1];
        list[i+1] := obj;
      EndIf
    EndFor
  Until (!modified);

  return list;
EndFunction

//////////////////////////////////////////////////////
// Sends gump to person
//////////////////////////////////////////////////////
// params:
// who: player
// menu: struct { buttons, options } returned by CreateTrapsMenuFromCFG
// return: struct { objtype, desc, amount }


Function TrapSelect(who, menu)
  If (!menu)
    syslog("Fehler: TrapSelect wurde ohne gültige Menü-Struktur aufgerufen.");
    return;
  EndIf

  var res := GFSendGump(who);
  var buttons := menu.buttons;

  If (!res[buttons.okay])
    return 0;       // return 0 => gump was closed (right-click) or cancelled.
  EndIf

  var obj := struct;

  ForEach item in (menu.options)
    If (res[item.button])
      obj := item;
      break;
    EndIf
  EndForEach

  If (obj && buttons.amount)
    var amount := splitWords(res[buttons.amount]);

    If (amount[2])
      obj.+amount := CInt(amount[2]);
    Else
      obj.+amount := 0;
    EndIf
  EndIf
  return obj;
EndFunction

Function TrapMenuLines(captions, defaultPage := 1, calcSize := 0)
  var page := 0;
  var MenuX;
  var MenuY := 45;

  // in calc-size mode, only the height of the menu is calculated based upon a given gump width
  If (calcSize)
    TrapGumpWidth := calcSize;
  EndIf

  var width, link2page;   //used in the loop

  ForEach menu in captions
    width := GumpTextWidthFast(menu);

    // If current textline is full, continue with a new line
    If (!MenuX || (MenuX + width) > TrapGumpWidth-70)
      MenuX := 40;
      MenuY += 25;

      If (!calcSize)
        GFResizePic(25, MenuY-3, 0x13EC, TrapGumpWidth-50, 25);
      EndIf
    EndIf

    If (!calcSize)
      GFTextLine(MenuX, MenuY, COLOR_DEFAULT, menu);
    EndIf
    MenuX += width;

    If (!calcSize)
      page += 1;
      link2page := page;
      If (link2page == defaultPage)
        link2page := 0;
      EndIf
      GFButtonPage(MenuX, MenuY, 0x1459, 0x1459, link2page+1);
    EndIf

    MenuX += 30;

    SleepMS(3);
  EndForEach

  return MenuY;
EndFunction

//////////////////////////////////////////////////////
// Initializes gump, displayes title and some buttons
//////////////////////////////////////////////////////
// params:
// width, height of the gump
// defaultamount: number that's in the inputbox by default, or -1 to disable amount box
// title of gump
// return: ids of okay, cancel buttons and amount box

Function TrapInitGump(width, height, title := 0)

  TrapGumpWidth := CInt(width);
  TrapGumpHeight := CInt(height);

  var GumpX := (TrapGumpScreenWidth-width)/2;
  var GumpY := (TrapGumpScreenHeight-height)/2;

  If (GumpX < 0)
    GumpX := 0;
  EndIf
  If (GumpY < 0)
    GumpY := 0;
  EndIf

  If (!title)
    title := "Was wollt Ihr herstellen?";
  EndIf

  GFInitGump();
  GFSetRef(GumpX, GumpY);
  GFResizePic(0, 0, 2600, width, height);             // Background

  GFResizePic(50, 25, 0x13EC, width-100, 25);               // Title line graphic
  GFTextLine(GumpTextMid(title, 0, width), 28, COLOR_DEFAULT, title);   // Title text centered

  var okay := GFButtonID(width-200, height-50, 0x850, 0x851, 1);    // Okay button
  var cancel := GFButtonID(width-120, height-50, 0x847, 0x848, 1);    // Cancel button
  var amount := 1;

  return struct{ okay := okay, cancel := cancel, amount := amount };
EndFunction

////////////////////////////////////////////////////////
// Displays menu entrys (icon, description and checkbox
////////////////////////////////////////////////////////
// params:
// content: 2dim array of structures: { objtype, icon, desc }
// cols, rows: how many items should be displayed in given space? either a number, or an array, a number For each page
// defaultItem: objtype of item that should be checked
// defaultPage: must be same value given to MGMenuLines(), or page links are broken
// startY: the value, MGMenuLines() returned
// return: 1dim array of structs: { objtype, desc, button }

Function TrapMenuEntrys(content, cols, rows, defaultItem, defaultPage := 1, startY := 45)
  var page := 0;
  var objects := array;

  var obj := struct{ button, objtype, desc, key };

  var spaceX, spaceY;
  var ItemX, ItemY;

  var checked, desclines;   // Variables used in the loop

  ForEach entry in content
    // switch to page# we want to describe
    page += 1;
    If (page == defaultPage)
      GFPage(1);
    Else
      GFPage(page+1);
    EndIf

    // calculate space used by one item
    spaceX := CInt((TrapGumpWidth - 60) / cols[page]);
    spaceY := CInt((TrapGumpHeight - startY - 100) / rows[page]);

    ItemX := 30;
    ItemY := startY + 25;

    ForEach item in entry
      If (ItemX > TrapGumpWidth-spaceX-30)
        ItemX := 30;
        ItemY += spaceY;
      EndIf

      // check button
      If (defaultItem == item.objtype)
        checked := 1;
      Else
        checked := 0;
      EndIf
      obj.button := GFRadioButton(ItemX, ItemY+35, 0xD0, 0xD1, checked);
      obj.objtype := item.objtype;
      obj.desc := item.desc;
      obj.key := item.key;
      objects.append(obj);

      // the item's icon
      GFTilePic(ItemX+20, ItemY+32, item.icon);

      // the item's description
      desclines := TrapTextSplit(item.desc, spaceX);
      If (!desclines[2])
        GFTextLine(ItemX, ItemY+7, COLOR_DEFAULT, desclines[1]);
      Else
        GFTextLine(ItemX, ItemY, COLOR_DEFAULT, desclines[1]);
        GFTextLine(ItemX, ItemY+13, COLOR_DEFAULT, desclines[2]);
        If (desclines[3])
          GFTextLine(ItemX, ItemY+26, COLOR_DEFAULT, desclines[3]);
        EndIf
      EndIf

      ItemX += spaceX;
    EndForEach

    SleepMS(3);
  EndForEach

  return objects;
EndFunction


////////////////////////////////////////////////////////////
// Parts text line into lines of max. length given by width
////////////////////////////////////////////////////////////
// note:
// this Function does not split a word that are exceeding
// the width yet. (see 'todo')

Function TrapTextSplit(text, width)
  // don't cut If text is not too large
  If (CInt(Len(text)*6.5) <= width)   // 6.5 Pixel pro Buchstabe ist Durchschnitt
    return { text };
  EndIf

  var words := splitWords(text);
  var wordlen, lengths := array;

  // note length of each word
  ForEach word in words
    //wordlen := GumpTextWidthFast(word);  // die Funktion ist zu langsam. Die geschätzte Länge reicht.
    wordlen := Len(word)*8;         // etwas höher schätzen schadet hier nicht.
    lengths.append(wordlen);
  EndForEach

  var lines := array;

  var sum := 0, i, starti := 1;
  var w, l;

  // take as many words as fit in a line
  For (i := 1; i <= words.size(); i += 1)
    If (sum + lengths[i] > width)
      If (sum)
        l := "";
        For (w := starti; w < i; w += 1)
          l += words[w] + " ";
        EndFor
        lines.append(l);
        starti := i; sum := 0;
      Else
        // todo: split single word, it's to long For a single line
        sum += lengths[i];
      EndIf
    Else
      sum += lengths[i];
    EndIf
  EndFor

  l := "";
  For (w := starti; w < i; w += 1)
    l += words[w] + " ";
  EndFor
  lines.append(l);

  return lines;
EndFunction

//
//Farbauswahl der fersteckten Fallen
//

Function ColorOfTraps(file, menu)
  Set_Script_Option(SCRIPTOPT_NO_RUNAWAY, 1);

  var itemdesc := ReadConfigFile("::itemdesc");
  var cfg := ReadConfigFile(file);
  If (!cfg)
    syslog("Fehler: Konnte Configfile "+file+" nicht laden.");
    return;
  EndIf

  var menuinfo := cfg[menu];
  If (!menuinfo)
    menuinfo := {};   // default menu entry
    syslog("Warnung: Der Menueeintrag "+menu+" existiert im Configfile "+file+" nicht! Benutze Defaultwerte.");
  EndIf

  var window := menuinfo.size;    // Size 5 3   (space For 5 columbs and 3 rows of icons by default)
  If (window)
    window := splitWords(window);
    window[1] := CInt(window[1]);
    window[2] := CInt(window[2]);
  EndIf
  If (!window || window[1] < 1 || window[2] < 1)
    window := { 5, 3 };
  EndIf
  var size := menuinfo.entries;   // Entries 5 3  (use 5 columbs and 3 rows of icons by default)
  If (size)
    size := splitWords(size);
    size[1] := CInt(size[1]);
    size[2] := CInt(size[2]);
  EndIf
  var entriesPerPage := size[1] * size[2];
  If (!entriesPerPage || entriesPerPage < 1)
    size := { 5, 3 };
    entriesPerPage := size[1] * size[2];
  EndIf
  var typeMax := 0, typeMin := 1;     // limits a menu by type number
  If (menuinfo.typeMax)
    typeMax := CInt(menuinfo.typeMax);
  EndIf
  If (menuinfo.typeMin)
    typeMin := CInt(menuinfo.typeMin);
  EndIf

  var menunames := GetConfigStringArray(menuinfo, "Type");
  var cfgkeys := GetConfigIntKeys(cfg);
  If (!cfgkeys)
    syslog("Fehler: Keine Item-Einträge im Configfile "+file+" gefunden.");
    return;
  EndIf

  var defaultPage := 1;
  var defaultItem := 0;

  var content := array;
  var page;

  var nextPage := 1, pages := dictionary;
  var menus := array;

  // Alle Einträge in die Menüs einsortieren
  var entry, objentry, showItem, type;
  var pagename;
  var obj := struct{ objtype, icon, desc, skill, key };

  ForEach key in cfgkeys
    entry := cfg[key];

    showItem := 1;
    If  (!entry.type)
      type := 1;
    Else
      type := entry.type;
    EndIf

    // Soll der Eintrag überhaupt angezeigt werden?
    If (entry.desc)
      If (type < typeMin || (typeMax && type > typeMax))
        showItem := 0;
      EndIf
    Else
      syslog("Warnung: Item "+key+" im Configfile "+file+" hat keinen desc-Eintrag.");
      showItem := 0;
    EndIf

    If (showItem)
      page := pages[type];
      If (!page)
        pages.insert(type, nextPage);
        pagename := menunames[type-typeMin+1];
        If (!pagename)
          pagename := "Sonstiges";
        EndIf
        menus.insert(nextPage, pagename);
        content.insert(nextPage, array);
        page := nextPage;
        nextPage += 1;
      Else
        If (content[page].size() >= entriesPerPage)
          pages[type] := nextPage;
          pagename := menunames[type-typeMin+1];
          If (!pagename)
            pagename := "Sonstiges";
          EndIf
          menus.insert(nextPage, pagename);
          content.insert(nextPage, array);
          page := nextPage;
          nextPage += 1;
        EndIf
      EndIf

      obj.objtype := entry.objtype;
      obj.icon := entry.icon;
      obj.desc := entry.desc;
      obj.skill := entry.skill;
      If (!obj.skill && entry.skill1)
        obj.skill := entry.skill1;
      EndIf
      obj.key := key;

      If (!obj.objtype) // in some CFGs there are no 'objtype' tags
        obj.objtype := key;
      EndIf

      If (!obj.icon)
        objentry := itemdesc[obj.objtype];
        If (objentry && objentry.graphic)
          obj.icon := objentry.graphic;
        Else
          obj.icon := obj.objtype;
        EndIf
      EndIf

      content[page].append(obj);

      If (obj.objtype == defaultItem)
        defaultPage := page;
      EndIf

      SleepMS(3);
    EndIf
  EndForEach

  // Falls das Menü leer ist, soll CInt(0) zurückgegeben werden.
  If (!content.size())
    return 0;
  EndIf

  // Sortierung nach Skillwert (absteigend)
  ForEach submenu in content
    submenu := SortBySkill(submenu);
  EndForEach
  var width := 60 + window[1]*110;
  var height := 100+ window[2]*100 + TrapMenuLines(menus, 0, width);
  var buttons := TrapInitGump(width, height, menuinfo.title);
  var startY := TrapMenuLines(menus, defaultPage);
  var options := TrapMenuEntrys(content, size[1], size[2], defaultItem, defaultPage, startY);

  return struct{ buttons := buttons, options := options };
EndFunction

////////////////////////////////////////////////////////////
// Parts text line into lines of max. length given by width
////////////////////////////////////////////////////////////
// note:
// this Function does not split a word that are exceeding
// the width yet. (see 'todo')

Function DamageTimeFormTrap(trapskill, characterskill, characterdex)
  var time;
  return time;
EndFunction

Function DamageFormTrap(trapskill, characterskill, characterdex, targetstr)
  var time;
  return time;
EndFunction


// Erfolgreiches Entschaerfen einer Falle und evtl deren Vernichtung/Rueckbau
Function UnTrapSuccess(character, trapon)
  var cfg := ReadConfigFile(":traps:traps");
  var trap := FindConfigElem(cfg, GetObjProperty(trapon, "trap"));

  // alles was beim Fallen_legen oder beim verstecktes_finden gesetzt wurde wird geloescht
  // Sprengfallen werden "nur" entschaerft, der Rest geloescht
  If (trap.type == TRAP_TYPE_KISTENFALLEN)
    SendSysMessagePergon(character, "Ihr habt eine " + trap.desc +" erfolgreich entschaerft.");
    EraseTrapCProps(trapon);
  ElseIf (trap.type)
    SendSysMessagePergon(character, "Ihr baut eine " + trap.desc +" ab.");
    DestroyItem(trapon);
  Else
    // herkoemmliche (magische oder Truhen-) Fallen abpruefen
    If (GetObjProperty(trapon, "trap_type"))
      SendSysMessagePergon(character, "Ihr habt eine Falle erfolgreich entschaerft.");
      EraseTrapCProps(trapon);
    Else
      SendSysMessagePergon(character, "Irgendetwas misslingt und es passiert nichts.");
    EndIf
  EndIf
EndFunction

// Erfolgloses Entschaerfen einer Falle
Function UnTrapFail(character)
  SendSysMessagePergon(character, "Ihr schafft es nicht, diese Falle zu entschaerfen.");

  // Anteil der Hitpoints vom Fallensteller werden abgezogen
  ApplyRawDamagePergon(character, RandomInt(CInt(GetMaxHPPergon(character)/5.0)));
EndFunction

Function CreateMagicTrap(character, trapon, spellpower, trap_key, default_usescript := 0)   // konsisten mit BuildContainerTrap() aus
                    // traps.src halten !!!
  If (GetObjProperty(trapon, "trap_strength"))
    UnTrapFail(character);
    EraseTrapCProps (trapon);
  Else
    var cfg := ReadConfigFile(":traps:traps");
    var trap := FindConfigElem(cfg, trap_key);

    var use_script := trap.use_script; // Da fehlt halt was in der Config, wo ich nicht sicher bin, ob gewollt, oder nicht
    If ((!use_script) And default_usescript)
      use_script := default_usescript;
    EndIf

    SetTrapCProps (trapon, 0, character.serial, spellpower, spellpower + 50, ReadGameClock() + spellpower * 360, trap_key, use_script);
    Case (trap_key)
      8:  trapon.setprop("trap_type",1);  // Nadelfalle
      9:  trapon.setprop("trap_type",3);  // Sprengfalle
      10: trapon.setprop("trap_type",2);  // Giftfalle
      default: trapon.setprop("trap_type",1);
    EndCase
    SendSysMessagePergon(character, "Ihr baut eine " + trap.desc + " auf.", "You create a trap.");
  EndIf
EndFunction

Function SetTrapCProps (trapon, buildersdex, buildersserial, buildersskill, trap_strength, expiretime, trap, use_script)
  SetObjProperty(trapon, "buildersdex", buildersdex);
  SetObjProperty(trapon, "buildersserial", buildersserial);
  SetObjProperty(trapon, "buildersskill", buildersskill);
  SetObjProperty(trapon, "trap_strength", trap_strength);
  SetObjProperty(trapon, "trap", trap);
  trapon.decayat := CInt(expiretime);
  If (use_script)
    trapon.usescript := use_script;
  EndIf
EndFunction

Function EraseTrapCProps(trapon)
  EraseObjProperty(trapon, "buildersdex");
  EraseObjProperty(trapon, "buildersserial");
  EraseObjProperty(trapon, "buildersskill");
  EraseObjProperty(trapon, "casted_by"); // magische Kistenfalle
  EraseObjProperty(trapon, "poisonlvl"); // nur Giftfallen
  EraseObjProperty(trapon, "trap");
  EraseObjProperty(trapon, "trap_strength");
  EraseObjProperty(trapon, "trap_type"); // nur bei Kistenfallen
  EraseObjProperty(trapon, "trapped");
  trapon.decayat := 0;
  trapon.usescript := "";
EndFunction

// ********************************************************************************
//  Function IsWater
//  This Function checks to see If the targeted square is a water tile
//  It allows For targeting only flat water tiles so that the splash animation
//  can be used effectively.
// ********************************************************************************
Function IsWater(thistile, locinfo)
    If (thistile.objtype)
        If ((thistile.objtype >= 0x1797) and (thistile.objtype <= 0x17B2))
            return 1;
        ElseIf ((thistile.objtype >= 0x346E) and (thistile.objtype <= 0x34D5))
            return 1;
        ElseIf ((thistile.objtype >= 0x3490) and (thistile.objtype <= 0x34AB))
            return 1;
        ElseIf ((thistile.objtype >= 0x34B5) and (thistile.objtype <= 0x34D5))
            return 1;
        EndIf
    Else
        If ((locinfo.landtile >= 0x00A8) and (locinfo.landtile <= 0x00AB))
            return 1;
        ElseIf ((locinfo.landtile >= 0x0136) and (locinfo.landtile <= 0x0137))
            return 1;
        ElseIf ((locinfo.landtile >= 0x3FF0) and (locinfo.landtile <= 0x3FF3))
            return 1;
        EndIf
    EndIf
    return 0;
EndFunction


// Aus der Config der zu erschaffenden Viechers wird eins ausgewaehlt
Function chooseaviech(trapon, file, skill, type, wishVieh)
  var chance := (randomint(CInt(skill)) + 1);
  var entry;
  var cfg := ReadConfigFile(file);
  If (!cfg)
    syslog("Fehler: Konnte Configfile "+file+" nicht laden.");
    return;
  EndIf
  var cfgkeys := GetConfigIntKeys(cfg);
  var posibleviechers := {};
  var choosenviech := 0;
  var createdviech := 0;

  //durchforsten der config bis das tier mit dem hoechsten eintrag gefunden wurde
  ForEach vieh in cfgkeys
    entry := cfg[vieh];
    If(entry.type == type)
      If(entry.skill < chance)
        posibleviechers.Append(entry);
        SleepMS(10);
      EndIf
    EndIf
  EndForEach
  //Print(posibleviechers);
  If (wishVieh > 0)
    wishVieh := cfg[wishVieh];
    //Es gibt ein Wunsch Tier
    var templates := {};
    ForEach vieh in posibleviechers
      templates.Append(vieh.NpcTemplate);
    EndForEach
    If (wishVieh.NpcTemplate in templates)
      //Jetzt wird mit einer Wahrscheinlichkeit von 20% das gewünschte Vieh gefangen
      If(RandomInt(100) < 20)
        choosenviech := wishVieh;
      Else
        return 0;
      EndIf
    Else
      return 0;
    EndIf
  Else
    choosenviech := posibleviechers[RandomInt(posibleviechers.size()) + 1];
    //Print(choosenviech.NpcTemplate);
  EndIf

  createdviech := CreateNPCFromTemplate(CStr(choosenviech.NpcTemplate), trapon.x-1, trapon.y-1, GetMapInfo(trapon.x,trapon.y,trapon.realm).z,0,trapon.realm);

  //damits nicht abhaut oder abgereift
  //createdviech.frozen := 1;
  //IncRevision(createdviech);

  // damit das destroysctipt die viecher sauber identvizieren kann
  SetObjProperty(createdviech,"creatingtrap",trapon.serial);
  SetObjProperty(trapon, "creatednpc",createdviech.serial);
  return choosenviech.ViechID;
EndFunction

Function CheckTraps(Character, TrapSkillProp)
//loeschen von nicht mehr vorhandenen traps
var ActivTraps := TrapSkillProp.ActivTraps;
  ForEach TrapType IN ActivTraps
    var Traps := {};
    ForEach Trap IN TrapType
      var TrapItem := SystemFindObjectBySerial(Trap);
      If((TrapItem) AND (CInt(GetObjProperty(TrapItem, "buildersserial")) == CInt(Character.serial)))      //check ob aufgelistetes Item wirklich zum Char gehoert
        Traps.Append(Trap);
      EndIf
    EndForEach
      ActivTraps[_TrapType_iter] := Traps;
  EndForEach
  TrapSkillProp.ActivTraps := ActivTraps;
  return(TrapSkillProp);
EndFunction

Function GetNameBySerial(Serial)
//Zork 2004-02-28
  var Result := "<unbekannt>";
  var NPC := SystemFindObjectBySerial(Serial,SYSFIND_SEARCH_OFFLINE_MOBILES);
  If (NPC)
    Result := NPC.name;
  EndIf
  return(Result);
EndFunction
