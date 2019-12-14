/////////////////////////////////////////////////////////////////
//
//  Trackgump Script - Shows the Track Gump
//
//
//     Author: Fox
//
//  Features: - sortiert Mobiles nach Entfernung
//            - sortiert nach Typen, siehe SortArray
//            - zeigt Player mit Blauer, Grauer bzw. Roter Schrift
//  Aenderungen:
//    31.09.2005 Fox: - Elementuser zu den Magischen
//                    - Farbliche Hervorhebung des gewählten Reiters
//    05.01.2007 Turley - MGFindSubstring optimiert
//    12.05.2007 Fox: - Anpassung für Elfen
/////////////////////////////////////////////////////////////////

Use os;
Use uo;
Include "include/gumps";
Include "include/gumputil";
Include "include/msgs";

Var ANZEIGE_ANZAHL := 8;
Var MAX_NAME_WIDTH := 130;
Var MAX_LENGTH := 19;
Var MAX_PAGE := 20; //max Pages pro Reiter, MAX_PAGE*ANZEIGE_ANZAHL ergibt maximal in einem Reiter anzeigbare Anzahl Lebewesen
Var MAX_SORT := 100; //begrenzt Anzahl Mobiles für Entfernungssortierung

Var nummer := 0;
Var ButtonDict := dictionary; //Zuordnung der Buttons zu Ergebnissen
Var HumanList := {0x190, 0x191};
Var FellList := {
  0xc, 0x3b, 0x31f, 0x31a, 0x68, 0x3c, 0x3d, 0xdb, 0xd2, 0xda, 0x96, 0xbc,
  0xbb, 0x35, 0x36, 0x37, 101
};

//Drachen, Ostarde, Seeschlange, Ridgeback, Troll, Zentaure


Var character;

//------------------------------------------------------
Function CreateTrackMenu(who, title, TrackArray, EndZeit, zu_viele)
  character := who;
  MGInitGump();

  Var PlayerArray := array; //Player
  Var GefahrArray := array; //Gefahr
  Var FellArray := array; //Fell
  Var MagischArray := array; //Magisch

  SortArray(TrackArray, PlayerArray, GefahrArray, FellArray, MagischArray);

  CreateTrackPages(title, 0*MAX_PAGE, PlayerArray, 0);
  CreateTrackPages(title, 1*MAX_PAGE, GefahrArray, 1);
  CreateTrackPages(title, 2*MAX_PAGE, FellArray, 2);
  CreateTrackPages(title, 3*MAX_PAGE, MagischArray, 3);
  CreateTrackPages(title, 4*MAX_PAGE, TrackArray, 4);

  //var LaufZeit := ReadGameClock()-StartZeit;
  //SendSysMessagePergon(character, "Laufzeit: " + LaufZeit);

  While (EndZeit > ReadGameClock()) //Warten bis die 6s rum sind, sowohl wegen Gumpaufbau als auch gegen Powerskillen
    Sleep(1);
  EndWhile

  If (zu_viele==1)
    SendSysMessagePergon(character, "Zu viele Spuren ueberlagern sich, ihr koennt nur " + TrackArray.size() + " Kreaturen finden.", "Too many tracks on the ground, you can not identify all.");
  Else
    SendSysMessagePergon(character, "Insgesamt findet Ihr " + TrackArray.size() + " Kreaturen.", "All in all there are " + TrackArray.size() + " creatures.");
  EndIf
  Var choice := GFSendGump( character );
  //SendSysMessagePergon(character, "choice: " + choice);
  Var his_choice := 0;

  ForEach key in ( choice.keys )
    his_choice := key;
  EndForEach

  Var output;
  If (his_choice > 0)
    output := ButtonDict[his_choice];
    If (output != 0)  //nicht Cancel
      output := output.Mobile;
      //SendSysMessagePergon(character, "Mobile: " + output.name);
    EndIf
  Else //falls mit rechtem Mausklick abgebrochen wurde
    output := 0;
  EndIf

  //SendSysMessagePergon(character, "Output: "+ output);

  Return(output);
EndFunction

//------------------------------------------------------
Function CreateTrackPages(title, startPage, ReiterArray, Typ)
  //Erstellt die Pages die zu einem Reiter gehören
  //Jedem Reiter können maximal 100 Pages unterstehen

  Var GWidth := 620;
  Var GHeight := 360;

  Var size := ReiterArray.size();
  Var scrollPose := 0; //Position im Array
  Var parts := CINT(size/ANZEIGE_ANZAHL);
  If (size%ANZEIGE_ANZAHL > 0)
    parts+=1; //Für die Unvollständige Seite
  EndIf

  If (parts < 1)
    parts := 1; //damit auch eine leere Seite angezeigt werden kann
  EndIf

  If (parts > MAX_PAGE)
    parts := MAX_PAGE;
  EndIf

  Var maxScroll, minScroll;
  Var PageCount;
  Var slideX;
  Var slideY;
  Var i;
  For (PageCount := 1+startPage;PageCount<=parts+startPage;PageCount:=PageCount+1)
    GFPage(PageCount);
    MGInitPage(GWidth,GHeight,title, Typ);

    maxScroll := (scrollPose+1)*ANZEIGE_ANZAHL;
    If (maxScroll > size) //Keine "AnzeigeAnzahl" Items mehr übrig
      maxScroll := size;
    EndIf
    minScroll := scrollPose*ANZEIGE_ANZAHL+1;

    If (scrollPose < (parts-1))
      GFButtonPage(570, 130, 0x26AF, 0x26B0, PageCount+1); //Button nach rechts Scrollen
    EndIf
    If (scrollPose > 0)
      GFButtonPage(30, 130, 0x26B5, 0x26B6, PageCount-1);
    EndIf

    slideX := 0;
    slideY := 0;
    Var button_counter := 0; //um festzustellen ab wann die untere Reihe Buttons(für Auswahl) beginnt
    For (i := scrollPose*ANZEIGE_ANZAHL+1; i<=maxScroll; i+=1)
      button_counter := button_counter+1;
      ButtonDict.insert(GFButtonID(50+slideX, 80+slideY, 0x13C2, 0xE14, 1), ReiterArray[i]);
      //SendSysMessagePergon(character, "Name: " + ReiterArray[i].Mobile.name + "C_M: " + ReiterArray[i].Mobile.criminal + " " + ReiterArray[i].Mobile.murderer);

      Var textColor := 398;
      If ((ReiterArray[i].Mobile.criminal == 1) || (ReiterArray[i].Mobile.alignment==ALIGNMENT_NEUTRAL))
        textColor := COLOR_GRAY;
      EndIf
      If ((ReiterArray[i].Mobile.murderer == 1) || (ReiterArray[i].Mobile.alignment==ALIGNMENT_EVIL))
        textColor := 136;
      EndIf
      MGTrackName(ReiterArray[i].Mobile.name, 50+slideX, 90+slideY, textColor);
      GFTilePic(90+slideX,120+slideY, ReiterArray[i].Type);
      slideX+=MAX_NAME_WIDTH;
      If (button_counter==4)
        slideX := 0;
        slideY := 100;
      EndIf
    EndFor
    scrollPose += 1;
    SleepMS(10);
  EndFor
EndFunction

//------------------------------------------------------
Function MGInitGump()
  Var GumpX := 20;
  Var GumpY := 20;

  If (GumpX < 0)
    GumpX := 0;
  EndIf
  If (GumpY < 0)
    GumpY := 0;
  EndIf

  GFInitGump();
  GFSetRef(GumpX, GumpY); //setzt (0,0) Punkt
EndFunction
//------------------------------------------------------
Function MGInitPage(width, height, title, Typ)
  GFResizePic(0, 0, 2600, width, height);             // Background

  GFResizePic(50, 25, 0x13EC, width-100, 25);               // Title line graphic
  GFTextLine(GumpTextMid(title, 0, width), 28, COLOR_DEFAULT, title);   // Title text centered

  GFResizePic(50, 55, 0x13EC, width-100, 25); //Hintergrund Auswahlleiste

  Var captions := {"Player","Stark","gute Felle","Magisch","Alles"};
  Var MenuX := 50+30;
  Var MenuY := 55+4;
  Var BPage := 1;
  Var TextWidth := 0;
  Var Count := 0;

  ForEach menu in captions
    TextWidth := GumpTextWidthFast(menu);

    If (Count==Typ)
      GFTextLine(MenuX, MenuY, COLOR_GRAY, menu);
    Else
      GFTextLine(MenuX, MenuY, COLOR_DEFAULT, menu);
    EndIf

    MenuX += TextWidth;

    GFButtonPage(MenuX, MenuY, 0x1459, 0x1459, BPage); //Click auf diesen Reiter bringt einen zur Page BPage
    MenuX += 30;
    BPage+=MAX_PAGE;
    Count+=1;
    SleepMS(10);
  EndForEach

  // Cancel button
  Var cancel := GFButtonID( width-120, height-50, 0x847, 0x848, 1);
  ButtonDict.insert(cancel, 0);
EndFunction
//-----------------------------------------------------
Function SortTrackArray(MultiArray)
  // var StartZeit := ReadGameClock();
  Var ArrayLen  := Len(MultiArray);
  If (ArrayLen < 2)
    Return(MultiArray);
  EndIf

  Var i, k, f, s;
  For (i := 1; i < ArrayLen; i += 1)
    For ( k := i+1; k <= ArrayLen; k += 1)
      f := MultiArray[i];
      s := MultiArray[k];
      If ( s.Dist < f.Dist )
        MultiArray[i] := s;
        MultiArray[k] := f;
      EndIf
    EndFor
    SleepMS(2);
  EndFor

  //var LaufZeit := ReadGameClock()-StartZeit;
  //SendSysMessagePergon(character, "Laufzeit sort: " + LaufZeit);
  Return(MultiArray);
EndFunction
//-----------------------------------------------------

Function SortArray(byref TrackArray, byref PlayerArray, byref GefahrArray, byref FellArray, byref MagischArray);
  If (TrackArray.Size() <= MAX_SORT)
    TrackArray := SortTrackArray(TrackArray);
  Else
    SendSysMessagePergon(character, "Die Spuren sind undeutlich, das Alter ist nicht bestimmbar.","You can not identify the distance of the tracks.");
  EndIf

  Var i;
  For (i := 1; i <= TrackArray.Size(); i+=1)
    If (!TrackArray[i].Mobile.isa(POLCLASS_NPC)) // AND (TrackArray[i].Mobile.objtype In HumanList))
      PlayerArray.append(TrackArray[i]);
    EndIf

    If ((GetStrPergon(TrackArray[i].Mobile) >= 400 ))// AND (!(TrackArray[i].Mobile.objtype == 0x124) OR !(TrackArray[i].Mobile.objtype == 0x123))) //kein Packlama und Packpferd ist gefährlic
      GefahrArray.append(TrackArray[i]);
    EndIf

    If (TrackArray[i].Mobile.objtype In FellList)
      FellArray.append(TrackArray[i]);
    EndIf

    If (TrackArray[i].Caster)
      MagischArray.append(TrackArray[i]);
    EndIf
  EndFor
  //SendSysMessagePergon(character, "Fertig in Reiter sortiert");
EndFunction

//-----------------------------------------------------

Function MGTrackName(name, x, y, color)
  Var width;
  Var ausgabe := "";
  Var pos;
  Var dy := 0;
  Var zaehler := 1;

  Repeat
    ausgabe := MGFindSubString(name);
    width := MAX_NAME_WIDTH/22.0*Len(ausgabe);
    pos := MAX_NAME_WIDTH/2-width/2;
    GFTextLine(pos+x, y+dy, color, ausgabe);
    dy+=10;
    zaehler+=1;
  Until ((Len(name) == "") OR (zaehler>2));
EndFunction
//-----------------------------------------------------

Function MGFindSubstring(byref restString)
  // restString wird übergeben als voller String, beinhaltet am Ende
  // das Stringstück das übrig bleibt wenn der Rükgabestring vorn
  // abgeschnitten wurde
  // Rückgabe ist der String der in MAX_LENGTH passt, abgeschnitten
  // bei einem Leerzeichen

  Var fullString := restString;
  Var laenge := Len(fullString);
  Var s;
  Var lenString := "";
  Var stringarray;
  restString := "";

  If (laenge > MAX_LENGTH) //nur wenn der String überhaupt zu Lang ist muss getrennt werden
    stringarray := SplitWords(fullString);
    If (stringarray.size()>1) // Leerzeichen vorhanden
      For (s := 1;s<=stringarray.size();s+=1)
        If ((Len(lenString)+1+Len(stringarray[s]))>MAX_LENGTH) //passt es noch rein?
          Break;
        Else
          lenString+=" "+stringarray[s];
        EndIf
      EndFor
      lenString[1,1] := ""; //führendes Leerzeichen entfernen
      restString := fullString[Len(lenString)+2,laenge]; // was bleibt noch übrig?
    Else
      lenString := fullString[1,MAX_LENGTH];
      If (laenge<=MAX_LENGTH)
        restString := "";
      Else
        restString := fullString[Len(lenString+1),laenge];
      EndIf
    EndIf
  Else
    lenString := fullString;
  EndIf

  Return(lenString);
EndFunction
