Enum Buttons
  BTN_ADDBG    := 1000,
  BTN_DELBG    := 1001,
  BTN_PAYTAX   := 1002,
  BTN_DONATE   := 1003,
  BTN_ADDBM    := 1004,
  BTN_ADDFUNCT := 1005,
  BTN_CHAFUNCT := 1006,
  BTN_DELFUNCT := 1007,
  BTN_ADDPASS  := 1008,
  BTN_DELPASS  := 1009
EndEnum

Enum Checkbox
  CB_FUNCTS    := 1009,
  CB_TAX       := 1010,
  CB_PASS      := 1011
EndEnum

Enum TextEntry
  TXT_FUNCT    := 1012,
  TXT_CHOICEA  := 1013,
  TXT_CHOICED  := 1014,
  TXT_CHOICEP  := 1015
EndEnum

Function GetLayoutBg(who, stone, towndat)
  var habitants := towndat.getprop("Einwohner");
  var habitant  := habitants[who.serial];
  
  var ret := {
    "Page 0",
    "NoDispose",
    
    "ResizePic 100 100 9400 300 300",                            //Hintergrund
    "ResizePic 130 110 9400 240 30",                             //�berschrifthintergrund
    "GumpPic 100 140 1418",                                      //Burg
    "GumpPic 105 150 30079",                                     //rechter Winkel - links
    "GumpPic 335 150 30081",                                     //rechter Winkel - rechts
    "TilePic 65 370 5578",                                       //Flagge links
    "HtmlGump 130 117 240 30 0 0 0",                             //"Willkommen in "+Stadtname
    "HtmlGump 130 170 240 60 1 0 0"                              //"Der amtierende B�rgermeister - BMName - hei�t euch herzlich Willkommen"
  };
  
  If (who.getprop("Town") == stone.getprop("Town"))
    var paytaxbutton, paytaxtext;
    If (!habitant.house or habitant.tax.paid)
      paytaxbutton := "";
      paytaxtext   := "";
    Else
      paytaxbutton := "Button 440 265 30008 30009 1 0 "+BTN_PAYTAX;
      paytaxtext   := "Text 460 263 1153 6";
    EndIf
    
    ret += {
      "Page 0",
  
      "Text 140 263 1153 3",                                     //"Steuern"
      "Button 120 265 30008 30009 0 2",                          //Seite 2 - Steuermen�
      "Text 140 348 1153 2",                                     //"B�rgerschaft beenden"
      "Button 120 350 30008 30009 0 3",                          //Seite 3 - B�rgerschaft beenden
    
      "Page 1",
      
      "TilePic 390 370 5579",                                    //Flagge rechts
    
      "Page 2",
      
      "ResizePic 420 100 9400 300 300",
      "ResizePic 450 110 9400 240 30",
      "GumpPic 420 140 1418",                                    //Burg
      "GumpPic 425 150 30079",                                   //rechter Winkel - links
      "GumpPic 655 150 30081",                                   //rechter Winkel - rechts
      "TilePic 710 370 5579",                                    //Flagge rechts
      "HtmlGump 450 117 240 30 4 0 0",                           //"Steuermen�"
      "HtmlGump 450 170 240 60 5 0 0",                           //"Ihr seid im Besitz von - Grundst�cksname - Steuerstatus"
      paytaxtext,                                                //"Steuern zahlen"
      paytaxbutton,                                              //Button - Steuern zahlen
      "Text 460 348 1153 7",                                     //"Gold spenden"
      "Button 440 350 30008 30009 1 0 "+BTN_DONATE,              //Button - Geld spenden
    
      "Page 3",
      
      "ResizePic 420 100 9400 300 300",
      "ResizePic 450 110 9400 240 30",
      "GumpPic 420 140 1418",                                    //Burg
      "GumpPic 425 150 30079",                                   //rechter Winkel - links
      "GumpPic 655 150 30081",                                   //rechter Winkel - rechts
      "TilePic 710 370 5579",                                    //Flagge rechts
      "HtmlGump 450 117 240 30 8 0 0",                           //"B�rgerschaft beenden"
      "HtmlGump 450 170 240 60 9 0 0",                           //"Seid Ihr Euch wirklich sicher, dass Ihr die B�rgerschaft in "+town+" beenden wollt?"
      "Text 460 263 1153 10",                                    //"Best�tigen"
      "Button 440 265 30008 30009 1 0 "+BTN_DELBG                //Button - B�rgerschaft beenden
    };
  ElseIf (!who.getprop("Town"))
    ret += {
      "Page 0",
  
      "Text 140 298 1153 11",                                    //"B�rger der Stadt werden"
      "Button 120 300 30008 30009 1 0 "+BTN_ADDBG,               //Button - B�rger der Stadt werden
      
      "Page 1",
      
      "TilePic 390 370 5579"                                     //Flagge rechts
    };
  Else
    ret += {
      "Page 0",
  
      "HtmlGump 130 290 240 40 12 0 0",                          //"Ihr seid B�rger der Stadt "+Stadtname
      
      "Page 1",
      
      "TilePic 390 370 5579"                                     //Flagge rechts
    };
  EndIf
  
  return ret;
EndFunction

Function GetLayoutBM(rights)
  
  var size := 3;
  var page := 2;
  var y    := 265;
  var ret  := {
    "Page 0",
    "NoDispose",
    
    "ResizePic 100 100 9400 300 300",                            //Hintergrund
    "ResizePic 130 110 9400 240 30",                             //�berschrifthintergrund
    "GumpPic 100 140 1418",                                      //Burg
    "GumpPic 105 150 30079",                                     //rechter Winkel - links
    "GumpPic 335 150 30081",                                     //rechter Winkel - rechts
    "TilePic 65 370 5578",                                       //Flagge links
    "Button 105 150 30084 30085 1 0 2",                          //Zur�ck - BgGump �ffnen
    "Tooltip 1011067",                                           //Vorherige Seite
    "HtmlGump 130 117 240 30 0 0 0",                             //B�rgermeistermen�
    "HtmlGump 130 170 240 60 1 0 0",                             //Herzlich Willkommen geehrte/r "+who.name+" Was wollt Ihr machen?"
    
    "Page 1",
    "TilePic 390 370 5579"                                       //Flagge rechts
  };
  If (rights.all or rights.functs)
    ret  += GetLayoutFuncts(page, y, size);
    size += GetDataFuncts().size()+2;
    page += 5;
    y    += 25;
  EndIf
  If (rights.all or rights.tax)
    ret  += GetLayoutTax(page, y, size);
    size += GetDataTax().size();
    page += 2;
    y    += 25;
  EndIf
  If (rights.all or rights.pass)
    ret += GetLayoutPass(page, y, size);
    y += 25;
  EndIf
  
  return ret;
EndFunction

Function GetLayoutFuncts(page, y, size)
  return {
    "Page 0",
    
    "Text 140 "+(y-2)+" 1153 "+size,                           //"�mter verwalten"
    "Button 120 "+y+" 30008 30009 0 "+page,                    //Seite 2 - �mterverwaltung
  
    "Page "+page,
  
    "ResizePic 420 100 9400 300 300",
    "ResizePic 450 110 9400 240 30",
    "GumpPic 420 140 1418",                                    //Burg
    "GumpPic 425 150 30079",                                   //rechter Winkel - links
    "GumpPic 655 150 30081",                                   //rechter Winkel - rechts
    "TilePic 710 370 5579",                                    //Flagge rechts
    "HtmlGump 450 117 240 30 "+(size+1)+" 0 0",                //"�mterverwaltung"
    "HtmlGump 450 170 240 60 "+(size+2)+" 0 0",                //"Hier k�nnt Ihr folgendes machen: �mter vergeben, entziehen und erstellen - Was wollt Ihr machen?"
    "Text 460 263 1153 "+(size+3),                             //"Amt vergeben"
    "Button 440 265 30008 30009 0 "+(page+1),                  //Seite 3 - Amtsvergabe
    "Text 460 288 1153 "+(size+4),                             //"Amt �ndern"
    "Button 440 290 30008 30009 0 "+(page+3),                  //Seite 5 - Amt �ndern
    "Text 460 313 1153 "+(size+5),                             //"Amt entziehen"
    "Button 440 315 30008 30009 0 "+(page+4),                  //Seite 6 - Amt entziehen
    
    "Page "+(page+1),
    
    "ResizePic 420 100 9400 300 300",
    "ResizePic 450 110 9400 240 30",
    "GumpPic 420 140 1418",                                    //Burg
    "GumpPic 425 150 30079",                                   //rechter Winkel - links
    "GumpPic 655 150 30081",                                   //rechter Winkel - rechts
    "TilePic 710 370 5579",                                    //Flagge rechts
    "GumpPic 435 260 1141",                                    //Feld f�r Textentry
    "HtmlGump 450 117 240 30 "+(size+6)+" 0 0",                //"Amt erstellen"
    "HtmlGump 450 170 240 60 "+(size+7)+" 0 0",                //"Folgende �mter sind bereits vorhanden:"+�mter
    "TextEntryLimited 448 263 220 20 1153 "+TXT_FUNCT+" "+(size+8)+" 15",       //"Neues Amt"
    "Button 655 373 4005 4005 0 "+(page+2),                    //Seite 4 - B�rgerauswahl
    "Text 460 293 1153 "+(size+9),                             //"�mterverwaltung"
    "Checkbox 440 295 2117 2224 0 "+CB_FUNCTS,                 //Checkbox - �mter
    "Text 460 313 1153 "+(size+10),                            //"Steuerverwaltung"
    "Checkbox 440 315 2117 2224 0 "+CB_TAX,                    //Checkbox - Steuern
    "Text 460 333 1153 "+(size+11),                            //"Passierscheinverwaltung"
    "Checkbox 440 335 2117 2224 0 "+CB_PASS,                   //Checkbox - Passierscheine
    
    "Page "+(page+2),
    
    "ResizePic 420 100 9400 300 300",
    "ResizePic 450 110 9400 240 30",
    "TilePic 710 370 5579",                                    //Flagge rechts
    "Button 655 373 4014 4014 0 "+(page+1),                    //Seite zur�ck - Amt erstellen
    "HtmlGump 450 117 240 30 "+(size+12)+" 0 0",               //"Wer soll das Amt erhalten?"
    "HtmlGump 450 160 250 210 "+(size+14)+" 1 1",              //B�rgerliste
    "GumpPic 450 372 2443",                                    //Feld f�r Textentry
    "TextEntryLimited 458 373 220 21 0 "+TXT_CHOICEA+" 2 3",   //Nummernauswahl
    "Button 555 373 2311 2312 1 0 "+BTN_ADDFUNCT,              //Button - B�rgerauswahl
    
    "Page "+(page+3),
    
    "ResizePic 420 100 9400 300 300",
    "ResizePic 450 110 9400 240 30",
    "GumpPic 420 140 1418",                                    //Burg
    "TilePic 710 370 5579",                                    //Flagge rechts
    "HtmlGump 450 117 240 30 "+(size+13)+" 0 0",               //"Wessen Amt �ndern?"
    "HtmlGump 450 160 250 210 "+(size+15)+" 1 1",              //�mterliste
    "GumpPic 450 372 2443",                                    //Feld f�r Textentry
    "TextEntryLimited 458 373 220 21 0 "+TXT_CHOICED+" 2 3",   //Nummernauswahl
    "Button 555 373 2311 2312 1 0 "+BTN_CHAFUNCT,              //Button - B�rgerauswahl
        
    "Page "+(page+4),
    
    "ResizePic 420 100 9400 300 300",
    "ResizePic 450 110 9400 240 30",
    "TilePic 710 370 5579",                                    //Flagge rechts
    "HtmlGump 450 117 240 30 "+(size+13)+" 0 0",               //"Wessen Amt entziehen?"
    "HtmlGump 450 160 250 210 "+(size+15)+" 1 1",              //�mterliste
    "GumpPic 450 372 2443",                                    //Feld f�r Textentry
    "TextEntryLimited 458 373 220 21 0 "+TXT_CHOICED+" 2 3",   //Nummernauswahl
    "Button 555 373 2311 2312 1 0 "+BTN_DELFUNCT               //Button - B�rgerauswahl
  };
EndFunction

Function GetLayoutTax(page, y, size)
  return {
    "Page 0",
    
    "Text 140 "+(y-2)+" 1153 "+size,                           //"Steuerverwaltung"
    "Button 120 "+y+" 30008 30009 0 "+page,                    //Seitenwechsel - Steuerverwaltung
    
    "Page "+page,
    
    "ResizePic 420 100 9400 300 300",
    "ResizePic 450 110 9400 240 30",
    "GumpPic 420 140 1418",                                    //Burg
    "GumpPic 425 150 30079",                                   //rechter Winkel - links
    "GumpPic 655 150 30081",                                   //rechter Winkel - rechts
    "TilePic 710 370 5579",                                    //Flagge rechts
    "HtmlGump 450 117 240 30 "+(size+1)+" 0 0",                //"Steuerverwaltung"
    "HtmlGump 450 170 240 60 30 0 0",                          //
    "Text 460 293 1153 "+(size+2),                             //"Steuers�umige anzeigen"
    "Button 440 295 30008 30009 0 "+(page+1),                  //Seitenwechsel - Steuers�umige anzeigen
    
    "Page "+(page+1),
    
    "ResizePic 420 100 9400 300 300",
    "ResizePic 450 110 9400 240 30",
    "TilePic 710 370 5579",                                    //Flagge rechts
    "HtmlGump 450 117 240 30 "+(size+3)+" 0 0",                //"Steuers�umige B�rger"
    "HtmlGump 450 160 250 210 28 1 1"                          //Liste der s�umigen B�rger
    };
EndFunction

Function GetLayoutPass(page, y, size);
  return {
    "Page 0",
    
    "Text 140 "+(y-2)+" 1153 "+size,                           //"Passierscheine verwalten"
    "Button 120 "+y+" 30008 30009 0 "+page,                    //Seite 7 - Passierscheine verwalten
    
    "Page "+page,
    
    "ResizePic 420 100 9400 300 300",
    "ResizePic 450 110 9400 240 30",
    "GumpPic 420 140 1418",                                    //Burg
    "GumpPic 425 150 30079",                                   //rechter Winkel - links
    "GumpPic 655 150 30081",                                   //rechter Winkel - rechts
    "TilePic 710 370 5579",                                    //Flagge rechts
    "HtmlGump 450 117 240 30 "+(size+1)+" 0 0",                //"Passierscheine Verwalten"
    "HtmlGump 450 170 240 60 "+(size+2)+" 0 0",                //"Hier k�nnt Ihr folgendes machen: Passierscheine vergeben, entziehen und erstellen - Was wollt Ihr machen?"
    "Text 460 263 1153 "+(size+3),                             //"Passierschein vergeben"
    "Button 440 265 30008 30009 1 0 "+BTN_ADDPASS,             //Button - Passierscheinvergabe
    "Text 460 293 1153 "+(size+4),                             //"Passierschein entziehen"
    "Button 440 295 30008 30009 0 "+(page+1),                  //Seite 8 - Passierschein entziehen
    
    "Page "+(page+1),
    
    "ResizePic 420 100 9400 300 300",
    "ResizePic 450 110 9400 240 30",
    "TilePic 710 370 5579",                                    //Flagge rechts
    "HtmlGump 450 117 240 30 "+(size+5)+" 0 0",                //"Wessen Pass entziehen?"
    "HtmlGump 450 160 250 210 27 1 1",                         //Passierscheinliste
    "GumpPic 450 372 2443",                                    //Feld f�r Textentry
    "TextEntryLimited 458 373 220 21 0 "+TXT_CHOICEP+" 2 2",   //Nummernauswahl
    "Button 555 373 2311 2312 1 0 "+BTN_DELPASS                //Button - Passierschein entziehen
  };
EndFunction

Function GetLayoutGM()  
  return {    
    "Page 0",
    "NoDispose",
    
    "ResizePic 100 100 9400 300 300",                            //Hintergrund
    "ResizePic 130 110 9400 240 30",                             //�berschrifthintergrund
    "GumpPic 100 140 1418",                                      //Burg
    "GumpPic 105 150 30079",                                     //rechter Winkel - links
    "GumpPic 335 150 30081",                                     //rechter Winkel - rechts
    "TilePic 65 370 5578",                                       //Flagge links
    "Button 105 150 30084 30085 1 0 1",                          //Seite 2 - BM-Men�
    "Tooltip 1011067",                                           //Vorherige Seite
    "TilePic 390 370 5579",                                      //Flagge rechts
    "HtmlGump 130 117 240 30 0 0 0",                             //"GM-Men�"
    "HtmlGump 130 170 240 60 1 0 0",                             //"Herzlich willkommen erhabener "+GMName+" Was wollt ihr machen?"
    "Text 140 263 1153 2",                                       //"Einen B�rgermeister bestimmen"
    "Button 120 265 30008 30009 1 0 "+BTN_ADDBM                  //Button - B�rgermeister bestimmen
  };
EndFunction 

Function GetDataBg(who, town, towndat)
  var ret;
  var habitants := towndat.getprop("Einwohner");
  var habitant  := habitants[who.serial];
  var BM := SystemFindObjectBySerial(GetFuncts(habitants, "BM"),
              SYSFIND_SEARCH_OFFLINE_MOBILES
            ).name;
            
  If (!BM)
    ret := {
      "<CENTER><H3>Willkommen in "+town+"</H3></CENTER>",        // 0
      "<BASEFONT color="+HTML_WHITE+"><CENTER>"+                 // \
        "Zur Zeit gibt es keinen amtierenden B�rgermeister!"+    //  1
      "</CENTER></BASEFONT>"                                     // /
    };
  Else
    ret := {
      "<CENTER><H3>Willkommen in "+town+"</H3></CENTER>",        // 0
      "<BASEFONT color="+HTML_WHITE+"><CENTER>"+                 // \
        "Der amtierende B�rgermeister<br\>"+                     //  \     
        BM+"<br\>"+                                              //   1
        "hei�t Euch herzlich Willkommen"+                        //  /
      "</CENTER></BASEFONT>"                                     // /
    };
  EndIf
  
  var housename;
  var nohouse;
  var tax;
  
  If (habitant.house)
    housename := habitant.house;
    nohouse   := "";
    If (habitant.tax.paid)
      tax := "Eure Steuern sind bereits entrichtet";
    Else
      tax := "Eure Steuern sind noch offen";
    EndIf
  Else
    housename := "";
    nohouse   := "keinem Grundst�ck";
    tax       := "Ihr m�sst keine Steuern zahlen!";
  EndIf
  
  ret += {
    "B�rgerschaft beenden",                                      // 2
    "Steuern",                                                   // 3
    "<CENTER><H3>Steuermen�</H3></CENTER>",                      // 4
    "<BASEFONT color="+HTML_WHITE+"><CENTER>"+                   // \
      "Ihr seid im Besitz von<br\>"+                             //  \     
      nohouse+housename+"<br\>"+                                 //   5
      tax+                                                       //  /
    "</CENTER></BASEFONT>",                                      // /
    "Steuern zahlen",                                            // 6
    "Gold spenden",                                              // 7
    "<CENTER><H3>B�rgerschaft beenden</H3></CENTER>",            // 8
    "<BASEFONT color="+HTML_WHITE+"><CENTER>"+                   //  \
      "Seid Ihr Euch wirlklich sicher, "+                        //   9     
      "dass Ihr die B�rgerschaft in "+town+" beenden wollt?"+    //  / 
    "</CENTER></BASEFONT>",                                      // /
    "Best�tigen",                                                // 10
    "B�rger der Stadt werden",                                   // 11
    "<BASEFONT color="+HTML_RED+"><CENTER><H3>"+                 // \
      "Ihr seid B�rger der Stadt "+who.getprop("Town")+          //  12
    "</H3></CENTER></BASEFONT>"                                  // /
  };
  
  return ret;
EndFunction

Function GetDataBM(who := 0, towndat := 0)
  var gender, ret;
  var habitants := towndat.getprop("Einwohner");
  var passes    := towndat.getprop("Passierscheine");
  var functs    := CStr(GetFuncts(habitants));
  functs        := functs[2, len(functs)-2];
  
  If (who.gender)
    gender := "";
  Else
    gender := "r";
  EndIf
  
  ret := {
    "<CENTER><H3>B�rgermeistermen�</H3></CENTER>",               // 0
    "<BASEFONT color="+HTML_WHITE+"><CENTER>"+                   // \
      "Herzlich Willkommen verehrte"+gender+"<br\>"+             //  \
      who.name+"<br\>"+                                          //   1
      "Was wollt Ihr machen?"+                                   //  /
    "</CENTER></BASEFONT>",                                      // /
    "1"                                                          // 2
  };
  
  ret += GetDataFuncts(functs);
  ret += GetBgNames(habitants)[2];
  ret += GetSpecialNames(who, habitants, LIST_FUNCTS)[2];
  ret += GetDataTax();
  ret += GetDataPass();
  return ret;
EndFunction

Function GetDataFuncts(functs := 0)
  var ret := {
    "�mter verwalten",                                           // 0
    "<CENTER><H3>�mterverwaltung</H3></CENTER>",                 // 1
    "<BASEFONT color="+HTML_WHITE+"><CENTER>"+                   // \
      "Hier k�nnt Ihr folgendes machen:<br\>"+                   //  \
      "�mter vergeben und entziehen<br\>"+                       //   2
      "Was wollt Ihr machen?"+                                   //  /
    "</CENTER></BASEFONT>",                                      // /
    "Amt vergeben",                                              // 3
    "Amt �ndern",                                                // 4
    "Amt entziehen",                                             // 5
    "<CENTER><H3>Amt erstellen</H3></CENTER>",                   // 6
    "<BASEFONT color="+HTML_WHITE+"><CENTER>"+                   //  \
      "Folgende �mter sind bereits vorhanden:<br\>"+             //   7
      functs+                                                    //  /
    "</CENTER></BASEFONT>",                                      // /
    "Neues Amt",                                                 // 8
    "�mterverwaltung",                                           // 9
    "Steuerverwaltung",                                          //10
    "Passierscheinverwaltung",                                   //11
    "<CENTER><H3>Wer soll das Amt erhalten?</H3></CENTER>",      //12
    "<CENTER><H3>Wessen Amt entziehen?</H3></CENTER>"            //13
  };
  
  return ret;
EndFunction

Function GetDataTax()
  return {
    "Steuers�umige anzeigen",                                    // 0
    "<CENTER><H3>Steuers�umige B�rger</H3></CENTER>"             // 1
  };
EndFunction

Function GetDataPass()
  return {
    "Passierscheine verwalten",                                  // 0
    "<CENTER><H3>Passierscheinverwaltung</H3></CENTER>",         // 1
    "<BASEFONT color="+HTML_WHITE+"><CENTER>"+                   // \
      "Hier k�nnt Ihr folgendes machen:<br\>"+                   //  \
      "Passierscheine vergeben und entziehen<br\>"+              //   2
      "Was wollt Ihr machen?"+                                   //  /
    "</CENTER></BASEFONT>",                                      // /
    "Passierschein vergeben",                                    // 3
    "Passierschein entziehen",                                   // 4
    "<CENTER><H3>Wessen Pass entziehen?</H3></CENTER>",          // 5
    "Passierscheine verwalten"                                   // 6
  };
EndFunction

Function GetDataGM(who)
  var gender;
  
  If (who.gender)
    gender := "";
  Else
    gender := "r";
  EndIf
  
  return {
    "<CENTER><H3>GM-Men�</H3></CENTER>",                         // 0
    "<BASEFONT color="+HTML_WHITE+"><CENTER>"+                   // \
      "Herzlich Willkommen erhabene"+gender+"<br\>"+             //  \
      who.name+"<br\>"+                                          //   1
      "Was wollt Ihr machen?"+                                   //  /
    "</CENTER></BASEFONT>",                                      // /
    "Einen B�rgermeister bestimmen"                              // 2
  };
EndFunction