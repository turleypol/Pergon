///////////////////////////////////////////////////////////////////////////////
//
//   Include OnlineSearch - Liefert Informationen ueber eingeloggte Character
//
//     Author: Shinigami
//
//   Modification:
//
//     26.11.2003 Fraggulus: FilterVisibleCharacter() Check auf QuestChar eingebastelt
//
///////////////////////////////////////////////////////////////////////////////
//
// Character-Listen
//   EnumerateVisibleOnlineCharacters       - Listet die sichtbaren eingeloggten Character                -> Liste mit {char.name, char}
//   EnumerateVisibleOnlineCharactersSorted - Liefert die sichtbaren eingeloggten Character, sortiert     -> Liste mit {char.name, char}
//   EnumerateOnlineCharactersCanSeeMe      - Listet eingeloggten Character auf, die mich sehen koennen   -> Liste mit {char.name, char}
//   SearchForOnlineCharacter               - Sucht nach einem eingeloggten Character                     -> char
//   FilterVisibleCharacter                 - Sieht 'who' den Character 'char' ?                          -> 1/0
//   GetCMDLevelTitle                       - Liefert den Titel des CommandLevels                         -> String
//
// Character in Gump eintragen
//   FillInOnlineCharacterInfo              - Character-Daten eingeloggter Spieler ins Gump eintragen     -> Liste mit {char.name, char}
//   FillInHouseFriendCharInfo              - Character-Daten der Freunde eines Hauses ins Gump eintragen -> Liste mit {char.name, char}
//   FillInCharacterInfo                    - Character-Daten ins Gump eintragen

include "include/msgs";   // wegen SendSysMessagePergon
include "include/server"; // wegen CMDLEVEL_*-Konstanten
include "include/string"; // wegen SortMultiArrayByIndexNotCase

///////////////
// Konstanten
///////////////

Const LIST_ME     :=1;              // Ich bin in der Liste enthalten
Const DONT_LIST_ME:=0;              // Ich bin in der Liste nicht enthalten

Const TOTENREICH:="im Reich der Toten";
Const BOOTFAEHRE:="auf einem Boot";
Const KONTINENT :="auf dem Kontinent";

Var playertemplate:=struct;             // Template fuer normale Liste
    playertemplate.+cmdlevel:=CMDLEVEL_PLAYER;
    playertemplate.+serial:=0;

Var admintemplate:=struct;              // Template fuer vollstaendige Liste, abzueglich Jason
    admintemplate.+cmdlevel:=CMDLEVEL_GL;
    admintemplate.+serial:=0;

Var developertemplate:=struct;          // Template fuer vollstaendige Liste
    developertemplate.+cmdlevel:=CMDLEVEL_DEVELOPER;
    developertemplate.+serial:=0;

//////////////////////////
//                      //
//   Character-Listen   //
//                      //
//////////////////////////

////////////////////////////////////////////////////////////////////////////////////
// EnumerateVisibleOnlineCharacters - Listet die sichtbaren eingeloggten Character
////////////////////////////////////////////////////////////////////////////////////

Function EnumerateVisibleOnlineCharacters(who, selber_enthalten, list_unconcealed := 0)
  Var whoserial:=who.serial;
  If (selber_enthalten) // Soll ich selber in der Liste drinbleiben?
    whoserial:=0;       // ...dadurch schlaegt der folgende Check fehlt und ich bleib drin
  EndIf

  Var onlinechars:={}; // Wird gefuellt mit den sichtbaren Charactern

  ForEach char in EnumerateOnlineCharacters()
    // Sieht 'who' den Character 'char' ?
    If (FilterVisibleCharacter(who, whoserial, char, list_unconcealed))
      // Liste gleich fuer die alphabetische Sortierung vorbereiten
      onlinechars.append({char.name, char});
    EndIf
    SleepMS(2);
  EndForEach

  Return (onlinechars);
EndFunction

/////////////////////////////////////////////////////////////////////////////////////////////////////
// EnumerateVisibleOnlineCharactersSorted - Liefert die sichtbaren eingeloggten Character, sortiert
/////////////////////////////////////////////////////////////////////////////////////////////////////

Function EnumerateVisibleOnlineCharactersSorted(who, selber_enthalten, list_unconcealed := 0)
  // Liste mit {char.name, char}
  Var onlinechars:=EnumerateVisibleOnlineCharacters(
    who, selber_enthalten, list_unconcealed
  );
  SleepMS(2); // Mal sehen ob ne "Trennung" vor dem Sortieren reicht
  If (onlinechars.size()) // Gibt es Character?
    Return (SortMultiArrayByIndexNotCase(onlinechars, 1));
  Else // Nein, leere Liste liefern
    Return ({});
  EndIf
EndFunction

//////////////////////////////////////////////////////////////////////////////////////////////////
// EnumerateOnlineCharactersCanSeeMe - Listet eingeloggten Character auf, die mich sehen koennen
//////////////////////////////////////////////////////////////////////////////////////////////////

Function EnumerateOnlineCharactersCanSeeMe(who)
  Var onlinechars:={}; // Wird gefuellt mit den Charactern, die mich sehen koennen

  ForEach char in EnumerateOnlineCharacters()
    If (FilterVisibleCharacter(char, char.serial, who)) // Sieht 'char' den Character 'who' ?
      onlinechars.append({char.name, char});            // Liste gleich fuer die alphabetische Sortierung vorbereiten
    EndIf
  EndForEach

  Return (onlinechars);
EndFunction

///////////////////////////////////////////////////////////////////////
// SearchForOnlineCharacter - Sucht nach einem eingeloggten Character
///////////////////////////////////////////////////////////////////////

Function SearchForOnlineCharacter(who, searchname, list_unconcealed := 0)
  searchname:=Lower(" "+searchname+" ");
  Var searchedplayer:={};

  // Liste mit {char.name, char}
  ForEach player in EnumerateVisibleOnlineCharactersSorted(
      who, DONT_LIST_ME, list_unconcealed
  );
    If (Lower(" "+player[1]+" ")[searchname])
      searchedplayer.append(player);
    EndIf
  EndForeach
  SleepMS(2);
  If (searchedplayer.size())     // Hab ich einen Character gefunden?
    If (searchedplayer.size()>1) // Hab ich mehr als einen Character gefunden?
      Var index:=1;

      ForEach player in searchedplayer
        SendSysMessagePergon(who, "["+index+"] - "+player[1]); // Liste der potentiellen Character ausgeben
        index:=index+1;
      EndForEach

      Var number:=CInt(SendTextEntryGump(who, "Welchen Character meint ihr (Nummer) ?", 5));
      If ((1<=number) And (number<=searchedplayer.size()))
        Return (searchedplayer[number][2]);
      EndIf
    Else
      Return (searchedplayer[1][2]); // ...es gibt nur genau einen
    EndIf
  EndIf
EndFunction

////////////////////////////////////////////////////////////////
// FilterVisibleCharacter - Sieht 'who' den Character 'char' ?
////////////////////////////////////////////////////////////////

Function FilterVisibleCharacter(who, whoserial, char, list_unconcealed := 0)

  If (char.serial == whoserial)
    // selber erstmal aus der Liste rausstreichen
    return 0;
  EndIf

  If (who.cmdlevel >= CMDLEVEL_SEER)
    // bin CMDLevel und sehe ...
    If (char.concealed <= who.cmdlevel)
      // alle auf gleichem/kleinerem Concealed-Level
      return 1;
    Else
      // und den Rest nicht
      return 0;
    EndIf
  EndIf

  // Bin Spieler und sehe ...
  If (char.cmdlevel < CMDLEVEL_QUESTCHAR)
    // alles unterhalb von QuestChar
    return 1;
  EndIf

  If (!char.concealed)
    // Char nicht getarnt
    If (list_unconcealed)
      // ungetarnte Leute sollen generell auftauchen
      return 1;
    EndIf

    If (!GetObjProperty(char, PROP_HIDE_STAFF_ONLINE))
      // sonst nur auf expliziten Wunsch
      return 1;
    EndIf
  EndIf

  return 0;
EndFunction

///////////////////////////////////////////////////////////
// GetCMDLevelTitle - Liefert den Titel des CommandLevels
///////////////////////////////////////////////////////////

Function GetCMDLevelTitle(who)
  Case (who)
    CMDLEVEL_PLAYER:    Return ("Spieler");
    CMDLEVEL_COUNSELOR: Return ("Counselor");
    CMDLEVEL_QUESTCHAR: Return ("QuestChar");
    CMDLEVEL_SEER:      Return ("Seer");
    CMDLEVEL_GM:        Return ("GM");
    CMDLEVEL_HIGHGM:    Return ("HighGM");
    CMDLEVEL_GL:        Return ("GameLeader");
    CMDLEVEL_SCRIPTER:  Return ("Scripter");
    CMDLEVEL_DEVELOPER: Return ("Developer");

    default:            Return ("[Undefiniert]");
  EndCase
EndFunction

/////////////////////////////////////
//                                 //
//   Character in Gump eintragen   //
//                                 //
/////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// FillInOnlineCharacterInfo
// Character-Daten eingeloggter Spieler ins Gump eintragen
///////////////////////////////////////////////////////////////////////////
Function FillInOnlineCharacterInfo(
    byref layout, byref data, who, selber_enthalten
)
  // Liste mit {char.name, char}
  Var playerinfo := EnumerateVisibleOnlineCharactersSorted(
    who, selber_enthalten
  );
  If (playerinfo.size()) // Gibt es Character?
    // Character-Daten ins Gump eintragen
    FillInCharacterInfo(layout, data, playerinfo, 1, 1);
    // Liste zur Auswertung des Klickes im Gump zurueckgeben
    return (playerinfo);
  EndIf
EndFunction

////////////////////////////////////////////////////////////////////////////////////////////
// FillInHouseFriendCharInfo - Character-Daten der Freunde eines Hauses ins Gump eintragen
////////////////////////////////////////////////////////////////////////////////////////////

Function FillInHouseFriendCharInfo(byref layout, byref data, house, button := 0)
  // Liste der Freunde holen
  Var friendsref := GetAndCheckHausFreunde(house, 1)[2];
  If (friendsref.size()) // Gibt es Freunde?
    Var friendsinfo:={};

    // Liste fuer die alphabetische Sortierung zusammenstellen
    ForEach friend in friendsref
      friendsinfo.append({friend.name, friend});
    EndForEach
    SleepMS(2);
    friendsinfo:=SortMultiArrayByIndexNotCase(friendsinfo, 1); // ...sortieren

    // Character-Daten ins Gump eintragen
    FillInCharacterInfo(layout, data, friendsinfo, 0, button);
    // Liste zur Auswertung des Klickes im Gump zurueckgeben
    return (friendsinfo);
  EndIf
EndFunction

/////////////////////////////////////////////////////////////
// FillInCharacterInfo - Character-Daten ins Gump eintragen
/////////////////////////////////////////////////////////////
Function FillInCharacterInfo(byref layout, byref data, playerinfo, showcmdlevel, showbuttons)
  Var pagecount:=1;    // Akt. Seitenzahl
  Var playercount:=0;  // Zahl des einzutragenden Players
  Var nextpageswap:=0;
  Var msgbutton:=1;
  Var pos_y;

  ForEach player in playerinfo // Alle Character eintragen
    SleepMS(2);
    If (playercount==nextpageswap) // Seitenwechsel-Buttons einfuegen
      layout.append("page "+pagecount);
      If (playercount>0)
        layout.append("button 140 19 2650 2651 0 "+(pagecount-1));
      EndIf
      If (playerinfo.size()>playercount+26)
        layout.append("button 120 19 2648 2647 0 "+(pagecount+1));
      EndIf

      pagecount+=1;
      nextpageswap+=26;
      pos_y:=60;
    EndIf

    var color := " 68 ";
    If (showcmdlevel) // CMDLevel-Chars farblich anders anzeigen?
      If (player[2].cmdlevel >= CMDLEVEL_SEER)
        color := " 38 ";
      ElseIf (player[2].murderer)
        color := " 66 ";
      ElseIf (player[2].cmdlevel == CMDLEVEL_COUNSELOR)
        color := " 46 ";
      Else
        color := " 68 ";
      EndIf
    EndIf
    layout.append("text 30 "+CStr(pos_y+5)+color+data.size());
    data.append(player[1]);

    If (showbuttons) // Buttons anzeigen?
      layout.append("gumppic 30 "+CStr(22+pos_y)+" 96");
      layout.append("gumppic 93 "+CStr(22+pos_y)+" 96");
      layout.append("button 272 "+CStr(pos_y-2)+" 2640 2641 1 0 "+msgbutton);
    EndIf

    playercount+=1;
    msgbutton+=1;
    pos_y+=20;
  EndForEach
EndFunction
