//////////////////////////////////////////////////////////////////////////
//
//  Phygon's Manufacture Menu v1.0 (2003/11/14)
//
//  Author:    Phygon
//
//   Modifications:
//     23.11.2003 Phygon: Das ganze bisschen beschleunigt
//     2006/06/03 Shinigami: MGMenuEntrys um Auswahl Tile/Gump erweitert
//     14.09.2006 Turley: gump.inc Funktionen ersetzt durch schnelle Arraybefehle
//                        defaultitem-check erweitert um .desc (Ring==Verziehrter Ring beim Objtype ->doppelte Auswahl)
//
//////////////////////////////////////////////////////////////////////////

//////////////
// Functions
//////////////
//
// MGCreateMenuFromCFG( who, file, menu, tool, skillid, skillmod)		// reads CFG and creates menu
// SortBySkill(list)
// MGSetDefault( tool, menu, res)
// MGGetDefault( tool, menu)
//
// MGInitGump( width, height, defaultamount, title)						// Initializes gump, displays title and some buttons
// MGMenuLines( captions, defaultPage, calcSize)						// Creates menu lines
// MGMenuEntrys( content, cols, rows, defaultitem, defaultpage, startY, useTilePic)	// Creates items with description & checkbox
// MGSelect( who, menuinfo)												// Sends gump to player & returns objtype, desc, amount
//
// MGTextSplit( gumptext, width)        								// Returns array of text lines not exceeding <width>
//
//////////////////////////////////////////////


///////////////////////////
// Constants and variables
///////////////////////////

Const MGumpScreenWidth := 800;				// assumed for centering the gump
Const MGumpScreenHeight := 600;

Const MGumpUseTilePic := 1;
Const MGumpUseGumpPic := 0;

Var MGumpWidth;
Var MGumpHeight;

Var MGlayout;
Var MGdata;
Const OKAY:=10;
Const CANCEL:=11;
Const AMOUNT:=12;
Const START:=100;

//////////////////////////////////////////////////////
// Loads CFG and creates menu.
//////////////////////////////////////////////////////
// params:
// who: person to get skill values from
// file: CFG fiel in package format
// menu: menu entry name (in CFG file)
// tool: tool from witch loads defaults from
// skillid: default skillID (used, if no skill is given in CFG file)
// skillmod: skill modification for first skill, i.e.: (material.difficulty/100)-tool.quality;
// return: menu structure. Display menu by MGSelect( who, menu);
//
// menu entry in CFG:
// Menu carpentry
// {
// 	Title	What to manufacture?
// 	Size	6 3				// x y, size of gump is calculated with this numbers
// 	Entries	6 3				// x y, how many cols/rows to cast into given gump size
// 	TypeMin	1				// 'type' limiters (Item types that are displayed in this menu)
// 	TypeMax	7
// 	Type	Shirts			// menu names for type = TypeMin..TypeMax
// 	Type	Trousers
// 	Type	Hats
// 	Type	Everything else
// }

Function MGCreateMenuFromCFG( who, file, menu, tool, skillid, skillmod)

	Var itemdesc := ReadConfigFile( "::itemdesc");

	Var cfg := ReadConfigFile( file);
	If (!cfg)
		SysLog("Fehler: Konnte Configfile "+file+" nicht laden.");
		Return;
	EndIf

	Var menuinfo := cfg[menu];
	If (!menuinfo)
		menuinfo := {};		// default menu entry
		SysLog("Warnung: Der Menueeintrag "+menu+" existiert im Configfile "+file+" nicht! Benutze Defaultwerte.");
	EndIf

	Var window := menuinfo.size;		// Size 5 3		(space for 5 columbs and 3 rows of icons by default)
	If (window)
		window := splitWords( window);
		window[1] := CInt(window[1]);
		window[2] := CInt(window[2]);
	EndIf
	If (!window || window[1] < 1 || window[2] < 1)
		window := { 5, 3 };
	EndIf
	Var size := menuinfo.entries;		// Entries 5 3	(use 5 columbs and 3 rows of icons by default)
	If (size)
		size := splitWords( size);
		size[1] := CInt(size[1]);
		size[2] := CInt(size[2]);
	EndIf
	Var entriesPerPage := size[1] * size[2];
	If (!entriesPerPage || entriesPerPage < 1)
		size := { 5, 3 };
		entriesPerPage := size[1] * size[2];
	EndIf
	Var typeMax := 0, typeMin := 1;			// limits a menu by type number
	If (menuinfo.typeMax)
		typeMax := CInt( menuinfo.typeMax);
	EndIf
	If (menuinfo.typeMin)
		typeMin := CInt( menuinfo.typeMin);
	EndIf

	Var menunames := GetConfigStringArray( menuinfo, "Type");
	Var cfgkeys := GetConfigIntKeys( cfg);
	If (!cfgkeys)
		SysLog("Fehler: Keine Item-Einträge im Configfile "+file+" gefunden.");
		Return;
	EndIf

	Var defaultPage := 1;
	Var defaultItem := 0, amount := 1, name:="";
	If (tool)
		Var defaultprop := MGGetDefault( tool, menu);
		If (defaultprop)
			defaultItem := defaultprop.objtype;
			amount := defaultprop.amount;
			name :=defaultprop.name;
		EndIf
	EndIf

	Var content := array;
	Var page;

	Var nextPage := 1, pages := dictionary;
	Var menus := array;

	// Alle Einträge in die Menüs einsortieren
	Var entry, objentry, showItem, type;
	Var skill1, skill2, skillid1, skillid2;
	Var pagename;
	Var obj := struct{ objtype, icon, desc, skill, key };

	Foreach key in cfgkeys
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
						If (GetSkillFloatPergon( who, skillid1)<(skill1+skillmod))
							showItem := 0;
						EndIf
					EndIf
					If (skillid2 && entry.defaultitem != 1)
						If (GetSkillFloatPergon( who, skillid2)<(skill2))
							showItem := 0;
						EndIf
					EndIf
				EndIf
			EndIf
		Else
			SysLog("Warnung: Item "+key+" im Configfile "+file+" hat keinen desc-Eintrag.");
			showItem := 0;
		EndIf

		If (showItem)
			page := pages[type];
			If (!page)
				pages.insert( type, nextPage);
				pagename := menunames[type-typeMin+1];
				If (!pagename)
					pagename := "Sonstiges";
				EndIf
				menus.insert( nextPage, pagename);
				content.insert( nextPage, array);
				page := nextPage;
				nextPage += 1;
			Else
				If (content[page].size() >= entriesPerPage)
					pages[type] := nextPage;
					pagename := menunames[type-typeMin+1];
					If (!pagename)
						pagename := "Sonstiges";
					EndIf
					menus.insert( nextPage, pagename);
					content.insert( nextPage, array);
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

			If (!obj.objtype)	// in some CFGs there are no 'objtype' tags
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

			content[page].append( obj);

			If ((obj.objtype == defaultItem) && (obj.desc==name))
				defaultPage := page;
			EndIf

			
		EndIf
		SleepMs(3);
	EndForeach

	// Falls das Menü leer ist, soll CInt(0) zurückgegeben werden.
	If (!content.size())
		return 0;
	EndIf

	// Sortierung nach Skillwert (absteigend)
	Foreach submenu in content
		submenu := SortBySkill(submenu);
	Endforeach

	Var width  := 60 + window[1]*110;
	Var height := 100+ window[2]*100 + MGMenuLines( menus, 0, width);
	Var buttons := MGInitGump( width, height, amount, menuinfo.title);
	Var startY := MGMenuLines( menus, defaultPage);
	Var options := MGMenuEntrys( content, size[1], size[2], defaultItem, defaultPage, startY, MGumpUseTilePic, name);

	return struct{ buttons := buttons, options := options };
EndFunction

///////////////////////////////////////////////////////////
// sorts a list of structs by ".skill" (simple BubbleSort)
///////////////////////////////////////////////////////////
// params:
// list of structs with skill member
// return: list, sorted

Function SortBySkill(list)
	Var size := list.size();
	Var i, modified;

	Repeat
		modified := 0;

		For(i := 1; i < size; i+=1)
			If (list[i].skill < list[i+1].skill)
				modified := 1;

				Var obj := list[i];
				list[i] := list[i+1];
				list[i+1] := obj;
			EndIf
			SleepMS(2);
		EndFor
	Until (!modified);

	return list;
EndFunction

//////////////////////////////////////////////////////
// Save default objtype/amount to given tool
//////////////////////////////////////////////////////
// params:
// tool, menu name, return-val from MGSelect

Function MGSetDefault( tool, menu, res)
	SetObjProperty( tool, "MenuDef"+menu, { res.objtype, res.amount, res.desc });
EndFunction

//////////////////////////////////////////////////////
// Loads defaults from tool
//////////////////////////////////////////////////////
// params:
// tool, menu name
// return: { objtype, amount }

Function MGGetDefault( tool, menu)
	Var prop := GetObjProperty( tool, "MenuDef"+menu);

	If (prop)
		return struct{ objtype := prop[1], amount := prop[2], name:=prop[3] };
	EndIf

	return struct{ objtype := 0, amount := 1, name:=""};
EndFunction

//////////////////////////////////////////////////////
// Initializes gump, displayes title and some buttons
//////////////////////////////////////////////////////
// params:
// width, height of the gump
// defaultamount: number that's in the inputbox by default, or -1 to disable amount box
// title of gump
// return: ids of okay, cancel buttons and amount box

Function MGInitGump(width, height, defaultamount := 1, title := 0)

	MGumpWidth := CInt(width);
	MGumpHeight := CInt(height);

//	Var GumpX := Max(0,(MGumpScreenWidth-width)/2);
//	Var GumpY := Max(0,(MGumpScreenHeight-height)/2);

	If (!title)
		title := "Was wollt Ihr herstellen?";
	EndIf

	MGlayout:={"nodispose", 
	           "page 0",
	           "resizepic 0 0 2600 "+width+" "+height, // Background
	           "resizepic 50 25 5100 "+(width-100)+" 25", // Title line graphic
	           "htmlgump 0 28 "+width+" 28 0 0 0", // Title text centered
	           "button "+(width-200)+" "+(height-50)+" 2128 2129 1 0 "+OKAY, // Okay button
	           "button "+(width-120)+" "+(height-50)+" 2119 2120 1 0 "+CANCEL}; // Cancel button
	         
	         
	MGdata:={"<center>"+title+"</center>"};

	If (CInt(defaultamount) >= 0)
		MGlayout.append("text 55 "+(height-50)+" "+COLOR_DEFAULT+" "+MGdata.size());
		MGdata.append("Anzahl");
		MGlayout.append("gumppic 106 "+(height-52)+" 2443");
		MGlayout.append("textentry 115 "+(height-50)+" 70 15 "+COLOR_DEFAULT+" "+AMOUNT+" "+MGdata.size()); // How many items to build? Default is 1.
		MGdata.append(CInt(defaultamount));
	EndIf

	return struct{ okay := OKAY, cancel := CANCEL, amount := AMOUNT };
EndFunction

////////////////////////////////////////////////////
// Displays menu and calculates Y position of items
////////////////////////////////////////////////////
// params:
// captions: array of strings (menu labels)
// defaultPage: page, that is displayed when gump is started, or 0 if empty page should be displayed first
// calcSize: 0 to display menu, or width of gump, to calculate height of the menu
// return: y-koord of the lower point of the menu

Function MGMenuLines(captions, defaultPage := 1, calcSize := 0)
	Var page := 0;
	Var MenuX;
	Var MenuY := 45;

	// in calc-size mode, only the height of the menu is calculated based upon a given gump width
	If (calcSize)
		MGumpWidth := calcSize;
	EndIf

	Var width, link2page;		//used in the loop

	Foreach menu in captions
		width := GumpTextWidthFast(menu);

		// if current textline is full, continue with a new line
		If (!MenuX || (MenuX + width) > MGumpWidth-70)
			MenuX := 40;
			MenuY += 25;

			If (!calcSize)
				MGlayout.append("resizepic 25 "+(MenuY-3)+" 5100 "+(MGumpWidth-50)+" 25");
			EndIf
		EndIf

		If (!calcSize)
			MGlayout.append("text "+MenuX+" "+MenuY+" "+COLOR_DEFAULT+" "+MGdata.size());
			MGdata.append(menu);
		EndIf
		MenuX += width;

		If (!calcSize)
			page += 1;
			link2page := page;
			If (link2page==defaultPage)
				link2page := 0;
			EndIf
			MGlayout.append("button "+MenuX+" "+MenuY+" 5209 5209 0 "+(link2page+1));
		EndIf

		MenuX += 30;

		SleepMs(3);
	EndForeach

	return MenuY;
EndFunction

////////////////////////////////////////////////////////
// Displays menu entrys (icon, description and checkbox
////////////////////////////////////////////////////////
// params:
// content: 2dim array of structures: { objtype, icon, desc }
// cols, rows: how many items should be displayed in given space? either a number, or an array, a number for each page
// defaultItem: objtype of item that should be checked
// defaultPage: must be same value given to MGMenuLines(), or page links are broken
// startY: the value, MGMenuLines() returned
// useTilePic: use Tile Pic (1) or Gump Pic (0)
// return: 1dim array of structs: { objtype, desc, button }

Function MGMenuEntrys(content, cols, rows, defaultItem, defaultPage := 1, startY := 45, useTilePic := MGumpUseTilePic, name)
	Var page := 0;
	Var objects := array;

	Var obj := struct{ button, objtype, desc, key };

	Var spaceX, spaceY;
	Var ItemX, ItemY;

	Var checked, desclines, i:=0;		// Variables used in the loop

	Foreach entry in content
		// switch to page# we want to describe
		page += 1;
		If (page==defaultPage)
			MGlayout.append("page 1");
		Else
			MGlayout.append("page "+(page+1));
		EndIf

		// calculate space used by one item
		spaceX := CInt((MGumpWidth - 60) / cols[page]);
		spaceY := CInt((MGumpHeight - startY - 100) / rows[page]);

		ItemX := 30;
		ItemY := startY + 25;

		Foreach item in entry
			i+=1;
			If (ItemX > MGumpWidth-spaceX-30)
				ItemX := 30;
				ItemY += spaceY;
			EndIf
			// check button
			If ((defaultItem==item.objtype) && (name==item.desc))
				checked := 1;
			Else
				checked := 0;
			EndIf
			MGlayout.append("radio "+ItemX+" "+(ItemY+35)+" 208 209 "+checked+" "+(START+i));
			obj.button:=START+i;
			obj.objtype := item.objtype;
			obj.desc := item.desc;
			obj.key := item.key;
			objects.append(obj);

			// the item's icon
			If (useTilePic)
			  MGlayout.append("tilepic "+(ItemX+20)+" "+(ItemY+32)+" "+item.icon);
			Else
			  MGlayout.append("gumppic "+(ItemX+20)+" "+(ItemY+32)+" "+item.icon);
			EndIf

			// the item's description
			desclines := MGTextSplit(item.desc, spaceX);
			If (!desclines[2])
				MGlayout.append("text "+ItemX+" "+(ItemY+7)+" "+COLOR_DEFAULT+" "+MGdata.size());
				MGdata.append(desclines[1]);
			Else
				MGlayout.append("text "+ItemX+" "+ItemY+" "+COLOR_DEFAULT+" "+MGdata.size());
				MGdata.append(desclines[1]);
				MGlayout.append("text "+ItemX+" "+(ItemY+13)+" "+COLOR_DEFAULT+" "+MGdata.size());
				MGdata.append(desclines[2]);
				If (desclines[3])
					MGlayout.append("text "+ItemX+" "+(ItemY+26)+" "+COLOR_DEFAULT+" "+MGdata.size());
				  MGdata.append(desclines[3]);
				EndIf
			EndIf

			ItemX += spaceX;
		EndForeach

		SleepMs(3);
	EndForeach

	return objects;
EndFunction

//////////////////////////////////////////////////////
// Sends gump to person
//////////////////////////////////////////////////////
// params:
// who: player
// menu: struct { buttons, options } returned by MGCreateMenuFromCFG
// return: struct { objtype, desc, amount }

Function MGSelect(who, menu)
	If (!menu)
		SysLog("Fehler: MGSelect wurde ohne gültige Menü-Struktur aufgerufen.");
		Return;
	EndIf

	Var res := SendDialogGump(who,MGlayout,MGdata,50,50);

	Var buttons := menu.buttons;

	If (!res[buttons.okay])
		return 0;				// return 0 => gump was closed (right-click) or cancelled.
	EndIf

	Var obj;

	Foreach item in (menu.options)
		If (res[item.button])
			obj := item;
			break;
		EndIf
	EndForeach

	If (obj && buttons.amount)
		Var amount := splitWords( res[buttons.amount]);

		If (amount[2])
			obj.+amount := CInt( amount[2]);
		Else
			obj.+amount := 0;
		EndIf
	EndIf

	return obj;
EndFunction

////////////////////////////////////////////////////////////
// Parts text line into lines of max. length given by width
////////////////////////////////////////////////////////////
// note:
// this function does not split a word that are exceeding
// the width yet. (see 'todo')

Function MGTextSplit( text, width)
	// don't cut if text is not too large
	If (CInt(Len(text)*6.5) <= width)		// 6.5 Pixel pro Buchstabe ist Durchschnitt
		return { text };
	EndIf

	Var words := splitWords( text);
	Var wordlen, lengths := array;

	// note length of each word
	Foreach word in words
		//wordlen := GumpTextWidthFast( word);	// die Funktion ist zu langsam. Die geschätzte Länge reicht.
		wordlen := Len(word)*8;					// etwas höher schätzen schadet hier nicht.
		lengths.append( wordlen);
	EndForeach

	Var lines := array;

	Var sum := 0, i, starti := 1;
	Var w, l;

	// take as many words as fit in a line
	For (i := 1; i <= words.size(); i+=1)
		If (sum + lengths[i] > width)
			If (sum)
				l := "";
				For (w := starti; w < i; w:=w+1)
					l += words[w] + " ";
				EndFor
				lines.append( l);
				starti := i; sum := 0;
			Else
				// todo: split single word, it's to long for a single line
				sum += lengths[i];
			EndIf
		Else
			sum += lengths[i];
		EndIf
	EndFor

	l := "";
	For (w := starti; w < i; w+=1)
		l += words[w] + " ";
	EndFor
	lines.append( l);

	return lines;
EndFunction
