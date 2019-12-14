/**
  * #################
  * #               #
  * # Menüstruktur  #
  * # Taya Jan 2006 #
  * #               #
  * #################
  */

//Globale Variablen
var gflayout_anfang;        //layoutarray für Gump(anfang)
var gfdata_anfang;          //dataarray für Gump(anfang)
var gflayout_gott;          //layoutarray für Gump(gott)
var gfdata_gott;            //dataarray für Gump(gott)
var gflayout_bm;            //layoutarray für Gump(BM)
var gfdata_bm;              //dataarray für Gump(BM)
var gflayout_buerger;       //layoutarray für Gump(buerger)
var gfdata_buerger;         //dataarray für Gump(buerger)
var gflayout_antragzurueck; //layoutarray für Gump(antragzurück)
var gfdata_antragzurueck;   //datatarray für Gump(antragzurück)
var gflayout_keinbuerger;   //layoutarray für Gump(keinbuerger)
var gfdata_keinbuerger;     //dataarray für Gump(keinbuerger)
var gflayout_sonstiges;     //layoutarray für Gump(sonstiges)
var gfdata_sonstiges;       //dataarray für Gump(sonstiges)

//##################################################################################################
function GumpPage_SteuernBezahlenLayout(Str)
    return      {
    "resizepic 560 250 9400 300 300",
    "resizepic 590 260 9400 240 30",
    "gumppic 560 290 1418",
    "TilePic 215 520 5578",    //Fahnelinks
    "TilePic 850 520 5579",    //Fahnerechts
    "text 660 267 42 " + Str, //Steuern zahlen Überschrift
    "Button 590 310 30008 30009 1 0 320", //Menü2Button 1
    "text 607 307 33 " + (Str+1), //Menü2Text (Steuern zahlen1)
    "text 607 327 33 " + (Str+2), //Menü2Text (Steuern zahlen2)
    "text 607 347 30 " + (Str+3)  //Menü2Text (Steuern zahlen3)
    };
endfunction
function GumpPage_SteuernBezahlenDataPlus3(item)
    return {
    " Zahlungsmenü",
    "Bezahlen der fälligen",
    "Quartals-Steuern in höhe von",
    GetObjProperty(item, CPROP_STEUERBETRAG)
    };
endfunction



function GumpPage_WaehlenLayout(Str)
    return   {
    "resizepic 560 250 9400 300 300",
    "resizepic 590 260 9400 240 30",
    "gumppic 560 290 1418",
    "TilePic 215 520 5578",    //Fahnelinks
    "TilePic 850 520 5579",    //Fahnerechts
    "text 650 267 42 " + Str,
    "HtmlGump 580 330 260 200 " + (Str+1) + " 1 1 1",
    "text 607 290 33 " + (Str+2),
    "TextEntry 660 290 200 20 30 1 " + (Str+3),
    "Button 590 310 30008 30009 1 0 302", //Menü2Button 1
    "text 607 307 33 " + (Str+4)
    };
endfunction
function GumpPage_WaehlenDataPlus3(kandidatenListeVar)
    return   {
    "   Wahlmenü",
    kandidatenListeVar,
    "Kandidat:",
    "Hier Kandidat eintragen",
    "Wählen"
    };

endfunction



function GumpPage_KandidatWerdenLayout(Str)   //STR = 20
	   return {
	  "resizepic 560 250 9400 300 300",
    "resizepic 590 260 9400 240 30",
    "gumppic 560 290 1418",
    "TilePic 215 520 5578",    //Fahnelinks
    "TilePic 850 520 5579",    //Fahnerechts
    "text 660 267 42 " + Str, //Kandidieren Überschrift
    "Button 590 310 30008 30009 1 0 300", //Menü2Button 1
    "text 607 307 33 " + (Str+1), //Menü2Text (Kandidieren)
    "Button 590 330 30008 30009 1 0 301", //Menü2Button 2
    "text 607 327 33 " + (Str+2) //Menü2Text (Kandidatur zurückziehen)
    };
endfunction
function GumpPage_KandidatWerdenDataPlus2()
    return {
    "Kandidierungsmenü",
    "Selber kandidieren",
    "Kandidatur zurueckziehen"
    };
endfunction



function GumpPage_ErgebnisWahlLayout(Str)    //STR = 23
    return      {
    "resizepic 560 250 9400 300 300",
    "resizepic 590 260 9400 240 30",
    "gumppic 560 290 1418",
    "TilePic 215 520 5578",    //Fahnelinks
    "TilePic 850 520 5579",    //Fahnerechts
    "text 660 267 42 " + Str,
    "HtmlGump 580 300 260 200 " + (Str+1) + " 1 1"
    };
endfunction
function GumpPage_ErgebnisWahlDataPlus1(BMWahlErgebnisListeVar)
    return      {
    " Wahlergebnis",
    BMWahlErgebnisListeVar
    };
endfunction



function GumpPage_EntscheidenLayout(Str)   //STR = 25
	  return {	
	  "resizepic 560 250 9400 300 300",
    "resizepic 590 260 9400 240 30",
    "gumppic 560 290 1418",
    "TilePic 215 520 5578",    //Fahnelinks
    "TilePic 850 520 5579",    //Fahnerechts
    "text 660 267 42 " + Str, //Abstimung Überschrift
    "HtmlGump 580 300 260 200 " + (Str+1) + " 1 1",
    "text 607 507 33 " + (Str+2),// ja
    "Button 590 510 30008 30009 1 0 330", //Menü2Button 1
    "text 797 507 33 " + (Str+3),// nein
    "Button 780 510 30008 30009 1 0 340" //Menü2Button 1
    };
endfunction
function GumpPage_EntscheidenDataPlus3(item)
    return {
    
    "Abstimmungsmenü",
    getObjProperty(item, CPROP_ENTSCHEIDTEXT),
    "Ja",
    "Nein"
    
    };
endfunction



function  GumpPage_ErgEntscheidLayout(Str)   // Str + 29
			  return {
	  "resizepic 560 250 9400 300 300",
    "resizepic 590 260 9400 240 30",
    "gumppic 560 290 1418",
    "TilePic 215 520 5578",    //Fahnelinks
    "TilePic 850 520 5579",    //Fahnerechts
    "text 650 267 42 " + Str ,
    "HtmlGump 580 300 260 200 " + (Str+1) + " 1 1",
    "text 605 507 33 " + (Str+2) 
    };
endfunction
function GumpPage_EntErgData(item)  
    var text := getObjProperty(item, CPROP_ENTSCHEIDTEXT);
    if (!text)
    text := "Es wurde bisher kein Entscheid durchgeführt";
    endif
    var Ergebnis := getObjProperty(item, CPROP_ENTSCHEIDERGEBNIS);
    if (!Ergebnis)
    Ergebnis := {"n/a", "n/a"};
    endif
    var text2;
    if (getObjProperty(item, CPROP_ENTSCHEIDLAEUFT) == 1)
    text2 :=  "Zwischenstand: ";
    else
    text2 := "Endergebnis: ";
    endif
    text2 := text2 + Ergebnis[1] + " (ja) / " + Ergebnis[2] + " (nein)";
    
    return {
		"Entscheidungsergebnis",
		text,
    text2
    
    }; 
endfunction
/////////////////////////////////////////////////////////////////////////////////////////////////
//Gumpdeklaration

/*
################
#              #
#    STEUER    #
#      GP      #
################
*/
function GPAUSWAHL(item)
    gflayout_anfang := {
    "page 0",
    "resizepic 250 250 9400 300 300", //Hintergrund
    "page 1",
    "Button 280 310 30008 30009 1 0 10", //MenüButton 1
    "text 297 307 33 0", // Menütext (GOTT)
    "Button 280 330 30008 30009 1 0 20", //MenüButton 2
    "text 297 327 33 1", // Menütext (BM)
    "Button 280 350 30008 30009 1 0 30", //MenüButton 3
    "text 297 347 33 2", // Menütext (BÜRGER)
    "Button 280 370 30008 30009 1 0 40", //MenüButton 4
    "text 297 367 33 3", // Menütext (ANTRAGZURÜCK)
    "Button 280 390 30008 30009 1 0 50", //MenüButton 5
    "text 297 387 33 4", // Menütext (ANTRAG STELLEN)
    "Button 280 410 30008 30009 1 0 60", //MenüButton 6
    "text 297 407 33 5", // Menütext (SONSTIGES)
    "text 297 287 33 6" // Menütext (Text)
    };

    gfdata_anfang := {
    "GOTT",
    "BM",
    "BÜRGER",
    "ANTRAG ZURÜCK",
    "ANTRAG STELLEN",
    "SONSTIGES",
    "       Bitte Gump wählen!"
    };
    return ( {gflayout_anfang, gfdata_anfang} );
endfunction


/*
################
#              #
#  BUERGER GP  #
#              #
################
*/					
function GumpPage_Buerger(item) //(item, GumpPage_KandidatWerdenLayout(), GumpPage_KandidatWerdenDataPlus2(), GumpPage_WaehlenLayout(), GumpPage_WaehlenDataPlus3(kandidatenListeVar), GumpPage_ErgebnisWahlLayout(), GumpPage_ErgebnisWahlDataPlus1(BMWahlErgebnisListeVar), GumpPage_SteuernBezahlenLayout(), GumpPage_SteuernBezahlenDataPlus3(item), GumpPage_ErgEntscheidLayout(), GumpPage_EntErgData(item), GumpPage_EntscheidenLayout(), GumpPage_EntscheidenDataPlus3(item)) //(item, GumpPage_SteuernBezahlenLayoutVar, GumpPage_WaehlenLayoutVar, GumpPage_ErgebnisWahlLayoutVar, GumpPage_SteuernVar, GumpPage_WaehlenDataPlus3Var, GumpPage_ErgebnisWahlDataPlus1Var, GumpPage_ErgEntscheidLayVar, GumpPage_ErgEntscheidDataPlus3Var, GumpPage_EntscheidenLayoutVar, GumpPage_EntscheidenDataPlus3Var, GumpPage_KandidatWerdenLayoutVar, GumpPage_KandidatWerdenDataPlus2Var)
    var stadt := GetObjProperty(item, CPROP_STADT);
    if (!stadt)
        stadt := "CProp " + CPROP_STADT + " setzen";
    else
        stadt := stadt;
    endif
    var BM := GetObjProperty(item, CPROP_BUERGERMEISTER);
    if (!BM)
        BM := "CProp " + CPROP_BUERGERMEISTER + " setzen";
    endif

    gflayout_buerger := {
    "page 0",
    "resizepic 250 250 9400 300 300", //Hintergrund
    "resizepic 280 260 9400 240 30",  //Kästchen für Überschrift
    "page 1",
    //############################################################
    "gumppic 250 290 1418",  //BurgPic
    "gumppic 260 300 30079", //Rahmen
    //"text 310 267 42 0",       //Überschrift (Willkommen...)
    "htmlgump 280 265 240 40 0 0 0",
    "text 285 320 33 1",       //Amtierender BM...
    "text 285 340 33 2",       //...
    "text 285 360 33 3",       //...
    "TilePic 215 520 5578",    //Fahnelinks
    "TilePic 540 520 5579",    //Fahnerechts
    "Button 300 400 4502 4502 0 2 0",   //Weiterbutton zur Menüpage (Page 2)
    "page 2",  //Hauptmenü
    //#############################################################
    "gumppic 250 290 1418",    //BurgPic
    "TilePic 215 520 5578",    //Fahnelinks
    "TilePic 540 520 5579",    //Fahnerechts
    "text 379 267 42 4", //Überschrift (Menü)
    "Button 280 310 30008 30009 0 3 0", //MenüButton 1
    "text 297 307 33 5", // Menütext (Steuernzahlen)
    "Button 280 330 30008 30009 0 4 0", //MenüButton 2
    "text 297 327 33 6", // Menütext (Wählen)
    "Button 280 350 30008 30009 0 5 0", //MenüButton 3
    "text 297 347 33 7", // Menütext (Kandidat werden)
    "Button 280 370 30008 30009 0 6 0", //MenüButton 4
    "text 297 367 33 8", // Menütext (Wahlergebnis)
    "Button 280 390 30008 30009 0 7 0", //MenüButton 5
    "text 297 387 33 9", // Menütext (Abstimmen)
    "Button 280 410 30008 30009 0 8 0", //MenüButton 6
    "text 297 407 33 10", // Menütext (Abstimmungsergebnis)
    "Button 280 430 30008 30009 0 9 0", //MenüButton 7
    "text 297 427 33 11", // Menütext (Stadt verlassen)
    "page 3", //Steuern
    //###############################################################
    "gumppic 250 290 1418",    //BurgPic
    "text 379 267 42 4", //Überschrift (Menü)
    "Button 280 310 30008 30009 0 3 0", //MenüButton 1
    "text 297 307 33 5", // Menütext (Steuernzahlen)
    "Button 280 330 30008 30009 0 4 0", //MenüButton 2
    "text 297 327 33 6", // Menütext (Wählen)
    "Button 280 350 30008 30009 0 5 0", //MenüButton 3
    "text 297 347 33 7", // Menütext (Kandidat werden)
    "Button 280 370 30008 30009 0 6 0", //MenüButton 4
    "text 297 367 33 8", // Menütext (Wahlergebnis)
    "Button 280 390 30008 30009 0 7 0", //MenüButton 5
    "text 297 387 33 9", // Menütext (Abstimmen)
    "Button 280 410 30008 30009 0 8 0", //MenüButton 6
    "text 297 407 33 10", // Menütext (Abstimmungsergebnis)
    "Button 280 430 30008 30009 0 9 0", //MenüButton 7
    "text 297 427 33 11" // Menütext (Stadt verlassen)
    }
    + GumpPage_SteuernBezahlenLayout(12)       //GumpPage_SteuernBezahlenLayoutVar
    +
    {
    "page 4",  //Wählen
    //################################################################
    "gumppic 250 290 1418",    //BurgPic
    "text 379 267 42 4", //Überschrift (Menü)
    "Button 280 310 30008 30009 0 3 0", //MenüButton 1
    "text 297 307 33 5", // Menütext (Steuernzahlen)
    "Button 280 330 30008 30009 0 4 0", //MenüButton 2
    "text 297 327 33 6", // Menütext (Wählen)
    "Button 280 350 30008 30009 0 5 0", //MenüButton 3
    "text 297 347 33 7", // Menütext (Kandidat werden)
    "Button 280 370 30008 30009 0 6 0", //MenüButton 4
    "text 297 367 33 8", // Menütext (Wahlergebnis)
    "Button 280 390 30008 30009 0 7 0", //MenüButton 5
    "text 297 387 33 9", // Menütext (Abstimmen)
    "Button 280 410 30008 30009 0 8 0", //MenüButton 6
    "text 297 407 33 10", // Menütext (Abstimmungsergebnis)
    "Button 280 430 30008 30009 0 9 0", //MenüButton 7
    "text 297 427 33 11" // Menütext (Stadt verlassen)
    }
    + GumpPage_WaehlenLayout(16)     //GumpPage_WaehlenLayoutVar bis STR 20
    +
    {
    "page 5",  //Kandidieren
    //################################################################
    "gumppic 250 290 1418",    //BurgPic

    "text 379 267 42 4", //Überschrift (Menü)
    "Button 280 310 30008 30009 0 3 0", //MenüButton 1
    "text 297 307 33 5", // Menütext (Steuernzahlen)
    "Button 280 330 30008 30009 0 4 0", //MenüButton 2
    "text 297 327 33 6", // Menütext (Wählen)
    "Button 280 350 30008 30009 0 5 0", //MenüButton 3
    "text 297 347 33 7", // Menütext (Kandidat werden)
    "Button 280 370 30008 30009 0 6 0", //MenüButton 4
    "text 297 367 33 8", // Menütext (Wahlergebnis)
    "Button 280 390 30008 30009 0 7 0", //MenüButton 5
    "text 297 387 33 9", // Menütext (Abstimmen)
    "Button 280 410 30008 30009 0 8 0", //MenüButton 6
    "text 297 407 33 10", // Menütext (Abstimmungsergebnis)
    "Button 280 430 30008 30009 0 9 0", //MenüButton 7
    "text 297 427 33 11" // Menütext (Stadt verlassen)
    }
    + GumpPage_KandidatWerdenLayout(21)  //GumpPage_KandidatWerdenLayoutVar bis STR 23
    +
    {
    "page 6", //Wahlergebniss
    //###############################################################
    "gumppic 250 290 1418",    //BurgPic
    "text 379 267 42 4", //Überschrift (Menü)
    "Button 280 310 30008 30009 0 3 0", //MenüButton 1
    "text 297 307 33 5", // Menütext (Steuernzahlen)
    "Button 280 330 30008 30009 0 4 0", //MenüButton 2
    "text 297 327 33 6", // Menütext (Wählen)
    "Button 280 350 30008 30009 0 5 0", //MenüButton 3
    "text 297 347 33 7", // Menütext (Kandidat werden)
    "Button 280 370 30008 30009 0 6 0", //MenüButton 4
    "text 297 367 33 8", // Menütext (Wahlergebnis)
    "Button 280 390 30008 30009 0 7 0", //MenüButton 5
    "text 297 387 33 9", // Menütext (Abstimmen)
    "Button 280 410 30008 30009 0 8 0", //MenüButton 6
    "text 297 407 33 10", // Menütext (Abstimmungsergebnis)
    "Button 280 430 30008 30009 0 9 0", //MenüButton 7
    "text 297 427 33 11" // Menütext (Stadt verlassen)
    }
    + GumpPage_ErgebnisWahlLayout(24)    //GumpPage_ErgebnisWahlLayoutVar bis STR 25
    +
    {
    "page 7", //Abstimmen
    //###############################################################
    "gumppic 250 290 1418",    //BurgPic
    "text 379 267 42 4", //Überschrift (Menü)
    "Button 280 310 30008 30009 0 3 0", //MenüButton 1
    "text 297 307 33 5", // Menütext (Steuernzahlen)
    "Button 280 330 30008 30009 0 4 0", //MenüButton 2
    "text 297 327 33 6", // Menütext (Wählen)
    "Button 280 350 30008 30009 0 5 0", //MenüButton 3
    "text 297 347 33 7", // Menütext (Kandidat werden)
    "Button 280 370 30008 30009 0 6 0", //MenüButton 4
    "text 297 367 33 8", // Menütext (Wahlergebnis)
    "Button 280 390 30008 30009 0 7 0", //MenüButton 5
    "text 297 387 33 9", // Menütext (Abstimmen)
    "Button 280 410 30008 30009 0 8 0", //MenüButton 6
    "text 297 407 33 10", // Menütext (Abstimmungsergebnis)
    "Button 280 430 30008 30009 0 9 0", //MenüButton 7
    "text 297 427 33 11" // Menütext (Stadt verlassen)  
    }
    + GumpPage_EntscheidenLayout(26)   //GumpPage_EntscheidenLayoutVar bis STR 29
    +
    {
    "page 8", //Abstimmungsergebnis
    //###############################################################################
    "gumppic 250 290 1418",    //BurgPic
    "text 379 267 42 4", //Überschrift (Menü)
    "Button 280 310 30008 30009 0 3 0", //MenüButton 1
    "text 297 307 33 5", // Menütext (Steuernzahlen)
    "Button 280 330 30008 30009 0 4 0", //MenüButton 2
    "text 297 327 33 6", // Menütext (Wählen)
    "Button 280 350 30008 30009 0 5 0", //MenüButton 3
    "text 297 347 33 7", // Menütext (Kandidat werden)
    "Button 280 370 30008 30009 0 6 0", //MenüButton 4
    "text 297 367 33 8", // Menütext (Wahlergebnis)
    "Button 280 390 30008 30009 0 7 0", //MenüButton 5
    "text 297 387 33 9", // Menütext (Abstimmen)
    "Button 280 410 30008 30009 0 8 0", //MenüButton 6
    "text 297 407 33 10", // Menütext (Abstimmungsergebnis)
    "Button 280 430 30008 30009 0 9 0", //MenüButton 7
    "text 297 427 33 11" // Menütext (Stadt verlassen)
    }
    + GumpPage_ErgEntscheidLayout(30)   //GumpPage_ErgEntscheidLayVar
    +
    {
    "page 9", //Stadtverlassen
    //###########################################################################
    "gumppic 250 290 1418",    //BurgPic
    "text 379 267 42 4", //Überschrift (Menü)
    "Button 280 310 30008 30009 0 3 0", //MenüButton 1
    "text 297 307 33 5", // Menütext (Steuernzahlen)
    "Button 280 330 30008 30009 0 4 0", //MenüButton 2
    "text 297 327 33 6", // Menütext (Wählen)
    "Button 280 350 30008 30009 0 5 0", //MenüButton 3
    "text 297 347 33 7", // Menütext (Kandidat werden)
    "Button 280 370 30008 30009 0 6 0", //MenüButton 4
    "text 297 367 33 8", // Menütext (Wahlergebnis)
    "Button 280 390 30008 30009 0 7 0", //MenüButton 5
    "text 297 387 33 9", // Menütext (Abstimmen)
    "Button 280 410 30008 30009 0 8 0", //MenüButton 6
    "text 297 407 33 10", // Menütext (Abstimmungsergebnis)
    "Button 280 430 30008 30009 0 9 0", //MenüButton 7
    "text 297 427 33 11", // Menütext (Stadt verlassen)
    "resizepic 560 250 9400 300 300",
    "resizepic 590 260 9400 240 30",
    "gumppic 560 290 1418",
    "TilePic 215 520 5578",    //Fahnelinks
    "TilePic 850 520 5579",    //Fahnerechts
    "text 660 267 42 33", //Stadtverlassen Überschrift
    "Button 590 310 30008 30009 1 0 310", //Menü2Button 1
    "text 607 307 33 34" //Menü2Text (Stadt verlassen)
    };

    gfdata_buerger   := {
    "<center>Willkommen in : " + stadt + "</center>",
    "Der amtierende",
    "Bürgermeister ist : ",
    "" + BM,
    "Menü",
    "Steuern zahlen",
    "Wählen",
    "Kandidieren",
    "Wahlergebnis anzeigen",
    "Abstimmung",
    "Abstimmungsergebnis",
    "Die Stadt verlassen"    
    }
    + GumpPage_SteuernBezahlenDataPlus3(item)                        //GumpPage_SteuernVar
    + GumpPage_WaehlenDataPlus3(kandidatenListe(item))            //GumpPage_WaehlenDataPlus3Var
    + GumpPage_KandidatWerdenDataPlus2()                       //GumpPage_KandidatWerdenDataPlus2Var
    + GumpPage_ErgebnisWahlDataPlus1(BMWahlErgebnisListe(item))   //GumpPage_ErgebnisWahlDataPlus1Var
    + GumpPage_EntscheidenDataPlus3(item)                      //GumpPage_EntscheidenDataPlus3Var
    + GumpPage_EntErgData(item)                                //GumpPage_ErgEntscheidDataPlus3Var
    +
    {   
    "Kündigungsmenü",
    "Stadt verlassen?"
    };
    return ( {gflayout_buerger, gfdata_buerger} );
endfunction
/*
################
#              #
#  GOTT GP     #
#              #
################
*/
function GumpPage_God(item)
    var stadt := GetObjProperty(item, CPROP_STADT);
    if (!stadt)
        stadt := "CProp " + CPROP_STADT + " setzen";
    else
        stadt := stadt;
    endif
    var BM := GetObjProperty(item, CPROP_BUERGERMEISTER);
    if (!BM)
        BM := "CProp " + CPROP_BUERGERMEISTER + " setzen";
    endif

    gflayout_gott := {
    "page 0",
    "resizepic 250 250 9400 300 300", //Hintergrund
    "resizepic 280 260 9400 240 30",  //Kästchen für Überschrift
    "page 1", //
    //############################################################
    "gumppic 250 290 1418",  //BurgPic
    "gumppic 260 300 30079", //Rahmen
    //"text 310 267 42 0",       //Überschrift (Willkommen...)
    "htmlgump 280 265 240 40 0 0 0",
    "text 285 320 33 1",       //Amtierender BM...
    "text 285 340 33 2",       //...
    "text 285 360 33 3",       //...
    "TilePic 215 520 5578",    //Fahnelinks
    "TilePic 540 520 5579",    //Fahnerechts
    "Button 300 400 4502 4502 0 2 0",   //Weiterbutton zur Menüpage (Page 2)
    "page 2", //HauptMenü
    //#############################################################
    "gumppic 250 290 1418", //BurgPic
    "TilePic 215 520 5578",    //Fahnelinks
    "TilePic 540 520 5579",    //Fahnerechts
    "text 379 267 42 4", //Überschrift (Menü)
    "Button 280 310 30008 30009 0 3 0", //MenüButton 1
    "text 297 307 33 5", //Menütext (Titel verwalten)
    "Button 280 330 30008 30009 0 6 0", //MenüButton 2
    "text 297 327 33 6", //Menütext (BM einrichten)
    "Button 280 350 30008 30009 0 7 0", //MenüButton 3
    "text 297 347 33 7", //Menütext (Stadt einrichten)
    "page 3", //Titel Verwalten SubMenü
    //#############################################################
    "gumppic 250 290 1418", //BurgPic
    "text 379 267 42 4", //Überschrift (Menü)
    "Button 280 310 30008 30009 0 3 0", //MenüButton 1
    "text 297 307 33 5", //Menütext (Titel verwalten)
    "Button 280 330 30008 30009 0 6 0", //MenüButton 2
    "text 297 327 33 6", //Menütext (BM einrichten)
    "Button 280 350 30008 30009 0 7 0", //MenüButton 3
    "text 297 347 33 7", //Menütext (Stadt einrichten)
    "resizepic 560 250 9400 300 300",
    "resizepic 590 260 9400 240 30",
    "gumppic 560 290 1418",
    "TilePic 215 520 5578",    //Fahnelinks
    "TilePic 850 520 5579",    //Fahnerechts
    "text 660 267 42 8", //Titel verwalten Überschrift
    "Button 590 310 30008 30009 0 4 0", //Menü2Button 1
    "text 607 307 33 9", //Menü2Text (Titel hinzufügen)
    "Button 590 330 30008 30009 0 5 0", //Menü2Button 2
    "text 607 327 33 10", //Menü2Text (Titel entfernen)
    "page 4",
    //#############################################################
    "gumppic 250 290 1418", //BurgPic
    "text 379 267 42 4", //Überschrift (Menü)
    "Button 280 310 30008 30009 0 3 0", //MenüButton 1
    "text 297 307 33 5", //Menütext (Titel verwalten)
    "Button 280 330 30008 30009 0 6 0", //MenüButton 2
    "text 297 327 33 6", //Menütext (BM einrichten)
    "Button 280 350 30008 30009 0 7 0", //MenüButton 3
    "text 297 347 33 7", //Menütext (Stadt einrichten)
    "resizepic 560 250 9400 300 300",
    "resizepic 590 260 9400 240 30",
    "gumppic 560 290 1418",
    "TilePic 215 520 5578",    //Fahnelinks
    "TilePic 850 520 5579",    //Fahnerechts
    "text 660 267 42 11", //Titel verwalten Überschrift
    "HtmlGump 580 330 260 200 12 1 1",
    "text 607 287 33 13",
    "TextEntry 640 287 200 20 30 11 14",
    "Button 590 310 30008 30009 1 0 500", //Menü2Button 1
    "text 607 307 33 15", //Menü2Text (Titel hinzufügen)
    "page 5",
    //#############################################################
    "gumppic 250 290 1418", //BurgPic
    "text 379 267 42 4", //Überschrift (Menü)
    "Button 280 310 30008 30009 0 3 0", //MenüButton 1
    "text 297 307 33 5", //Menütext (Titel verwalten)
    "Button 280 330 30008 30009 0 6 0", //MenüButton 2
    "text 297 327 33 6", //Menütext (BM einrichten)
    "Button 280 350 30008 30009 0 7 0", //MenüButton 3
    "text 297 347 33 7", //Menütext (Stadt einrichten)
    "resizepic 560 250 9400 300 300",
    "resizepic 590 260 9400 240 30",
    "gumppic 560 290 1418",
    "TilePic 215 520 5578",    //Fahnelinks
    "TilePic 850 520 5579",    //Fahnerechts
    "text 660 267 42 16", //Titel verwalten Überschrift
    "HtmlGump 580 330 260 200 17 1 1",
    "text 607 287 33 18",
    "TextEntry 640 287 200 20 30 12 19",
    "Button 590 310 30008 30009 1 0 501", //Menü2Button 1
    "text 607 307 33 20", //Menü2Text (Titel entfernen)
    "page 6",
    //#############################################################
    "gumppic 250 290 1418", //BurgPic
    "text 379 267 42 4", //Überschrift (Menü)
    "Button 280 310 30008 30009 0 3 0", //MenüButton 1
    "text 297 307 33 5", //Menütext (Titel verwalten)
    "Button 280 330 30008 30009 0 6 0", //MenüButton 2
    "text 297 327 33 6", //Menütext (BM einrichten)
    "Button 280 350 30008 30009 0 7 0", //MenüButton 3
    "text 297 347 33 7", //Menütext (Stadt einrichten)
    "resizepic 560 250 9400 300 300",
    "resizepic 590 260 9400 240 30",
    "gumppic 560 290 1418",
    "TilePic 215 520 5578",    //Fahnelinks
    "TilePic 850 520 5579",    //Fahnerechts
    "text 660 267 42 21", //Titel verwalten Überschrift
    "Button 590 310 30008 30009 1 0 502", //Menü2Button 1
    "text 610 307 33 22",
    "text 610 347 33 23",
    "text 610 367 33 24",
    "text 610 387 33 25",
    "page 7,",
    //#############################################################
    "gumppic 250 290 1418", //BurgPic
    "text 379 267 42 4", //Überschrift (Menü)
    "Button 280 310 30008 30009 0 3 0", //MenüButton 1
    "text 297 307 33 5", //Menütext (Titel verwalten)
    "Button 280 330 30008 30009 0 6 0", //MenüButton 2
    "text 297 327 33 6", //Menütext (BM einrichten)
    "Button 280 350 30008 30009 0 7 0", //MenüButton 3
    "text 297 347 33 7", //Menütext (Stadt einrichten)
    "resizepic 560 250 9400 300 300",
    "resizepic 590 260 9400 240 30",
    "gumppic 560 290 1418",
    "TilePic 215 520 5578",    //Fahnelinks
    "TilePic 850 520 5579",    //Fahnerechts
    "text 660 267 42 26", //Titel verwalten Überschrift
    "text 607 287 33 27",
    "TextEntry 672 287 200 20 30 13 28",
    "Button 590 310 30008 30009 1 0 503", //Menü1Button 1
    "text 607 307 33 29", //Menü2Text (Stadt einrichten)
    "text 607 327 33 30",
    "text 607 347 33 31",
    "Button 590 390 30008 30009 1 0 504", //Menü1Button 1
    "text 607 387 33 32", //Menü2Text (bmwahl canceln)
    "Button 590 410 30008 30009 1 0 505", //Menü1Button 1
    "text 607 407 33 33", //Menü2Text (srwahl canceln)
    "text 607 427 33 34",
    "text 607 447 33 35",
    "text 607 467 33 36",
    "text 607 487 33 37"
    
    
    };

    gfdata_gott   := {
    "<center>Willkommen in : " + stadt + "</center>",
    "Der amtierende",
    "Bürgermeister ist : ",
    "" + BM,
    "Menü",
    "Titel verwalten",
    "Bürgermeister einrichten",
    "Stadt einrichten",
    "   Titelmenü",
    "Titel hinzufuegen",
    "Titel loeschen",
    "   Titelmenü",
    titelliste(item),
    "Titel:",
    "Hier Titel eintragen",
    "Titel hinzufügen",
    "   Titelmenü",
    titelliste(item),
    "Titel:",
    "Hier Titel eintragen",
    "Titel entfernen",
    "Bürgermeistermenü",
    "Bürgermeister einrichten",
    "Mit dem Target den BM auswählen.",
    "Diese Funktion nur wenn der alte",
    "BM das Amt nicht übergeben kann.",
    "Stadt einrichten",
    "Stadtname:",
    "Hier Stadtnamen eintragen",
    "Stadt einrichten",
    "Nur möglich, wenn kein Bürger",
    "eingetragen ist.",
    "Laufende BM-Wahl abbrechen",
    "Laufenden Entscheid abbrechen",
    "",
    "ACHTUNG: alle Stimmen gehen verloren",
    "",
    ""
    
    
    
    };
    return ( {gflayout_gott, gfdata_gott} );
endfunction
/*
################
#              #
#  BM GP       #
#              #
################
*/
function GumpPage_Buergermeister(item)
    // var TESTSTADTRATWAHLSUBABSTIMMEN := 600;
    var stadt := GetObjProperty(item, CPROP_STADT);
    if (!stadt)
        stadt := "CProp " + CPROP_STADT + " setzen";
    else
        stadt := stadt;
    endif
    var BM := GetObjProperty(item, CPROP_BUERGERMEISTER);
    if (!BM)
        BM := "CProp " + CPROP_BUERGERMEISTER + " setzen";
    endif

    gflayout_bm := {
    "page 0",
    "resizepic 250 250 9400 300 300", //Hintergrund
    "resizepic 280 260 9400 240 30",  //Kästchen für Überschrift
    "page 1",
    //############################################################
    "gumppic 250 290 1418",  //BurgPic
    "gumppic 260 300 30079", //Rahmen
    //"text 310 267 42 0",       //Überschrift (Willkommen...)
    "htmlgump 280 265 240 40 0 0 0",
    "text 285 320 33 1",       //Amtierender BM...
    "text 285 340 33 2",       //...
    "text 285 360 33 3",       //...
    "TilePic 215 520 5578",    //Fahnelinks
    "TilePic 540 520 5579",    //Fahnerechts
    "Button 300 400 4502 4502 0 2 0",   //Weiterbutton zur Menüpage (Page 2)
    "page 2", //HauptMenü
    //#############################################################
    "gumppic 250 290 1418",    //BurgPic
    "TilePic 215 520 5578",    //Fahnelinks
    "TilePic 540 520 5579",    //Fahnerechts
    "text 379 267 42 4", //Überschrift (Menü)
    "Button 280 310 30008 30009 0 10 0", //MenüButton 1
    "text 297 307 33 5", // Menütext (Stadtfunktionen)
    "Button 280 330 30008 30009 0 20 0", //MenüButton 2
    "text 297 327 33 6", // Menütext (Steuerfunktionen)
    "Button 280 350 30008 30009 0 30 0", //MenüButton 3
    "text 297 347 33 7", // Menütext (Wahlfunktionen)
    "page 10", //StadtfunktionenMenü
    //##############################################################
    "gumppic 250 290 1418",    //BurgPic
    "text 379 267 42 4", //Überschrift (Menü)
    "Button 280 310 30008 30009 0 10 0", //MenüButton 1
    "text 297 307 33 5", // Menütext (Stadtfunktionen)
    "Button 280 330 30008 30009 0 20 0", //MenüButton 2
    "text 297 327 33 6", // Menütext (Steuerfunktionen)
    "Button 280 350 30008 30009 0 30 0", //MenüButton 3
    "text 297 347 33 7", // Menütext (Wahlfunktionen)
    "resizepic 560 250 9400 300 300",
    "resizepic 590 260 9400 240 30",
    "gumppic 560 290 1418",
    "TilePic 215 520 5578",    //Fahnelinks
    "TilePic 850 520 5579",    //Fahnerechts
    "text 660 267 42 8", //Stadtfunktionen Überschrift
    "Button 590 310 30008 30009 0 100 0", //Menü2Button 1
    "text 607 307 33 9", //Menü2Text (Titel vergeben)
    "Button 590 330 30008 30009 0 101 0", //Menü2Button 2
    "text 607 327 33 10", //Menü2Text (Bürgerschaft vergeben)
    "Button 590 350 30008 30009 0 102 0", //Menü2Button 1
    "text 607 347 33 11", //Menü2Text (Bürgerschaft wegnehmen)
    "Button 590 370 30008 30009 0 103 0", //Menü2Button 2
    "text 607 367 33 12", //Menü2Text (Bürger Liste)
    "page 100", //TitelvergebenSubMenü
    //#############################################################
    "gumppic 250 290 1418",    //BurgPic
    "text 379 267 42 4", //Überschrift (Menü)
    "Button 280 310 30008 30009 0 10 0", //MenüButton 1
    "text 297 307 33 5", // Menütext (Stadtfunktionen)
    "Button 280 330 30008 30009 0 20 0", //MenüButton 2
    "text 297 327 33 6", // Menütext (Steuerfunktionen)
    "Button 280 350 30008 30009 0 30 0", //MenüButton 3
    "text 297 347 33 7", // Menütext (Wahlfunktionen)
    "resizepic 560 250 9400 300 300",
    "resizepic 590 260 9400 240 30",
    "gumppic 560 290 1418",
    "TilePic 215 520 5578",    //Fahnelinks
    "TilePic 850 520 5579",    //Fahnerechts
    "text 660 267 42 13", //Titelvergeben Überschrift
    // "HtmlGump 580 300 260 200 14 1 1",
    "HtmlGump 580 300 260 100 14 1 1",
    "HtmlGump 580 400 260 100 31 1 1",
    "text 587 527 33 15",// ja
    "Button 570 530 30008 30009 1 0 400", //Menü2Button 1
    "text 757 527 33 16",// nein
    "Button 740 530 30008 30009 1 0 413", //Menü2Button 1
    "TextEntry 573 510 200 20 30 2 17", //TextEntryFeld (Titel)
    "TextEntry 743 510 200 20 30 3 18", //TextEntryFeld (CitizenNr)
    "page 101", //BürgerschaftvergebenSub
    //#############################################################
    "gumppic 250 290 1418",    //BurgPic
    "text 379 267 42 4", //Überschrift (Menü)
    "Button 280 310 30008 30009 0 10 0", //MenüButton 1
    "text 297 307 33 5", // Menütext (Stadtfunktionen)
    "Button 280 330 30008 30009 0 20 0", //MenüButton 2
    "text 297 327 33 6", // Menütext (Steuerfunktionen)
    "Button 280 350 30008 30009 0 30 0", //MenüButton 3
    "text 297 347 33 7", // Menütext (Wahlfunktionen)
    "resizepic 560 250 9400 300 300",
    "resizepic 590 260 9400 240 30",
    "gumppic 560 290 1418",
    "TilePic 215 520 5578",    //Fahnelinks
    "TilePic 850 520 5579",    //Fahnerechts
    "text 640 267 42 19", //Bürgerschaftvergeben Überschrift
    "HtmlGump 580 300 260 200 20 1 1",
    "text 607 507 33 21", //Menü2Text (Buergernummer)
    "Textentry 710 507 200 20 30 4 22",
    "Button 590 530 30008 30009 1 0 401", //Menü2Button 2
    "text 607 527 33 23", //Menü2Text (Antrag annehmen)
    "Button 740 530 30008 30009 1 0 402", //Menü2Button 2
    "text 757 527 33 24", //Menü2Text (Antrag ablehnen)
    "page 102", //Bürger rauswerfenSubMenü
    //#############################################################
    "gumppic 250 290 1418",    //BurgPic
    "text 379 267 42 4", //Überschrift (Menü)
    "Button 280 310 30008 30009 0 10 0", //MenüButton 1
    "text 297 307 33 5", // Menütext (Stadtfunktionen)
    "Button 280 330 30008 30009 0 20 0", //MenüButton 2
    "text 297 327 33 6", // Menütext (Steuerfunktionen)
    "Button 280 350 30008 30009 0 30 0", //MenüButton 3
    "text 297 347 33 7", // Menütext (Wahlfunktionen)
    "resizepic 560 250 9400 300 300",
    "resizepic 590 260 9400 240 30",
    "gumppic 560 290 1418",
    "TilePic 215 520 5578",    //Fahnelinks
    "TilePic 850 520 5579",    //Fahnerechts
    "text 640 267 42 25", //Bürger rauswerfen Überschrift
    "HtmlGump 580 300 260 200 26 1 1",
    "text 607 507 33 27",
    "TextEntry 710 507 200 30 30 5 28", //TextEntryFeld (Serial)
    "Button 590 530 30008 30009 1 0 403", //Menü2Button 1
    "text 607 527 33 29", //Menü2Text (Bürger rauswerfen)
    "page 103", //BürgerübersichtSubMenü
    //##############################################################
    "gumppic 250 290 1418",    //BurgPic
    "text 379 267 42 4", //Überschrift (Menü)
    "Button 280 310 30008 30009 0 10 0", //MenüButton 1
    "text 297 307 33 5", // Menütext (Stadtfunktionen)
    "Button 280 330 30008 30009 0 20 0", //MenüButton 2
    "text 297 327 33 6", // Menütext (Steuerfunktionen)
    "Button 280 350 30008 30009 0 30 0", //MenüButton 3
    "text 297 347 33 7", // Menütext (Wahlfunktionen)
    "resizepic 560 250 9400 300 300",
    "resizepic 590 260 9400 240 30",
    "gumppic 560 290 1418",
    "TilePic 215 520 5578",    //Fahnelinks
    "TilePic 850 520 5579",    //Fahnerechts
    "text 660 267 42 30", //Bürgerübersicht Überschrift
    "HtmlGump 580 330 260 200 31 1 1",
    "page 20", //SteuerfunktionenMenü
    //##############################################################
    "gumppic 250 290 1418",    //BurgPic
    "text 379 267 42 4", //Überschrift (Menü)
    "Button 280 310 30008 30009 0 10 0", //MenüButton 1
    "text 297 307 33 5", // Menütext (Stadtfunktionen)
    "Button 280 330 30008 30009 0 20 0", //MenüButton 2
    "text 297 327 33 6", // Menütext (Steuerfunktionen)
    "Button 280 350 30008 30009 0 30 0", //MenüButton 3
    "text 297 347 33 7", // Menütext (Wahlfunktionen)
    "resizepic 560 250 9400 300 300",
    "resizepic 590 260 9400 240 30",
    "gumppic 560 290 1418",
    "TilePic 215 520 5578",    //Fahnelinks
    "TilePic 850 520 5579",    //Fahnerechts
    "text 660 267 42 32", //Steuerfunktionsmenü Überschrift
    "Button 590 310 30008 30009 0 200 0", //Menü2Button 1
    "text 607 307 33 33", //Menü2Text (Grundbesitzer eintragen)
    "Button 590 330 30008 30009 0 201 0", //Menü2Button 2
    "text 607 327 33 34", //Menü2Text (Grundbesitzer austragen)
    "Button 590 350 30008 30009 0 202 0", //Menü2Button 1
    "text 607 347 33 35", //Menü2Text (Grundbesitzerliste)
    "Button 590 370 30008 30009 0 203 0", //Menü2Button 2
    "text 607 367 33 36", //Menü2Text (Steuerhöhe festlegen)
    "Button 590 390 30008 30009 0 204 0", //Menü2Button 1
    "text 607 387 33 37", //Menü2Text (neues Steuerquartal)
    "Button 590 410 30008 30009 0 205 0", //Menü2Button 2
    "text 607 407 33 38", //Menü2Text (Liste nichtbezahlter Steuern)
    "Button 590 430 30008 30009 0 206 0", //Menü2Button 1
    "text 607 427 33 39", //Menü2Text (Steuergelder abheben)
    "Button 590 450 30008 30009 0 207 0", //Menü2Button 2
    "text 607 447 33 40", //Menü2Text (Steuern zahlen)
		"page 200", //Grundbesitzer eintragen SubMenü
		//##############################################################
		"gumppic 250 290 1418", //BurgPic
		"text 379 267 42 4", //Überschrift (Menü)
		"Button 280 310 30008 30009 0 10 0", //MenüButton 1
		"text 297 307 33 5", // Menütext (Stadtfunktionen)
		"Button 280 330 30008 30009 0 20 0", //MenüButton 2
		"text 297 327 33 6", // Menütext (Steuerfunktionen)
		"Button 280 350 30008 30009 0 30 0", //MenüButton 3
		"text 297 347 33 7", // Menütext (Wahlfunktionen)
		"resizepic 560 250 9400 300 300",
		"resizepic 590 260 9400 240 30",
		"gumppic 560 290 1418",
		"TilePic 215 520 5578", //Fahnelinks
		"TilePic 850 520 5579", //Fahnerechts
		"text 640 267 42 41", //Grundbesitzer eintragen Überschrift
		"HtmlGump 580 300 260 200 42 1 1",
		"text 607 507 33 43",
		"TextEntry 660 507 100 30 30 6 44", //TextEntryFeld (Serial)
		"text 703 507 33 116",
		"TextEntry 775 507 100 30 30 7 117", //TextEntryFeld (Grundstuecksbezeichnung)
		"Button 590 530 30008 30009 1 0 404", //Menü2Button 1
		"text 607 527 33 45", //Menü2Text (Bürger rauswerfen)
		"page 201", //Grundbesitzer austragen SubMenü
		//###############################################################
		"gumppic 250 290 1418", //BurgPic
		"text 379 267 42 4", //Überschrift (Menü)
		"Button 280 310 30008 30009 0 10 0", //MenüButton 1
		"text 297 307 33 5", // Menütext (Stadtfunktionen)
		"Button 280 330 30008 30009 0 20 0", //MenüButton 2
		"text 297 327 33 6", // Menütext (Steuerfunktionen)
		"Button 280 350 30008 30009 0 30 0", //MenüButton 3
		"text 297 347 33 7", // Menütext (Wahlfunktionen)
		"resizepic 560 250 9400 300 300",
		"resizepic 590 260 9400 240 30",
		"gumppic 560 290 1418",
		"TilePic 215 520 5578", //Fahnelinks
		"TilePic 850 520 5579", //Fahnerechts
		"text 640 267 42 46", //Grundbesitzer austragen Überschrift
		"HtmlGump 580 300 260 200 47 1 1",
		"text 607 507 33 48",
		"TextEntry 710 507 200 30 30 8 49", //TextEntryFeld (Serial)
		"Button 590 530 30008 30009 1 0 405", //Menü2Button 1
		"text 607 527 33 50", //Menü2Text (Bürger rauswerfen)
    "text 607 527 33 50", //Menü2Text (Bürger rauswerfen)
    "page 202", //GrundbesitzerListe SubMenü
    //################################################################
    "gumppic 250 290 1418",    //BurgPic
    "text 379 267 42 4", //Überschrift (Menü)
    "Button 280 310 30008 30009 0 10 0", //MenüButton 1
    "text 297 307 33 5", // Menütext (Stadtfunktionen)
    "Button 280 330 30008 30009 0 20 0", //MenüButton 2
    "text 297 327 33 6", // Menütext (Steuerfunktionen)
    "Button 280 350 30008 30009 0 30 0", //MenüButton 3
    "text 297 347 33 7", // Menütext (Wahlfunktionen)
    "resizepic 560 250 9400 300 300",
    "resizepic 590 260 9400 240 30",
    "gumppic 560 290 1418",
    "TilePic 215 520 5578",    //Fahnelinks
    "TilePic 850 520 5579",    //Fahnerechts
    "text 640 267 42 51", //Grundbesitzerliste Überschrift
    "HtmlGump 580 330 260 200 52 1 1", //Menü2Text (Grundbesitzerliste)
    "page 203", //Steuerhöhen SUbMenü
    //###############################################################
    "gumppic 250 290 1418",    //BurgPic
    "text 379 267 42 4", //Überschrift (Menü)
    "Button 280 310 30008 30009 0 10 0", //MenüButton 1
    "text 297 307 33 5", // Menütext (Stadtfunktionen)
    "Button 280 330 30008 30009 0 20 0", //MenüButton 2
    "text 297 327 33 6", // Menütext (Steuerfunktionen)
    "Button 280 350 30008 30009 0 30 0", //MenüButton 3
    "text 297 347 33 7", // Menütext (Wahlfunktionen)
    "resizepic 560 250 9400 300 300",
    "resizepic 590 260 9400 240 30",
    "gumppic 560 290 1418",
    "TilePic 215 520 5578",    //Fahnelinks
    "TilePic 850 520 5579",    //Fahnerechts
    "text 660 267 42 53", //Steuerhöhe Überschrift
    "text 567 290 33 54", //Menü2Text (Alte Steuerhoehe)
    "text 567 310 33 55", //Menü2Text (Neue Steuerhoehe TXT)
    "TextEntry 675 310 200 20 30 9 56", //TextEntryFeld (Steuerhöhe)
    "Button 590 335 30008 30009 1 0 406", //Menü2Button 1
    "text 607 332 33 57", //Menü2Text (Steuerhöhe festlegen)
    "page 204", //Neues Quartal SubMenü
    //###############################################################
    "gumppic 250 290 1418",    //BurgPic
    "text 379 267 42 4", //Überschrift (Menü)
    "Button 280 310 30008 30009 0 10 0", //MenüButton 1
    "text 297 307 33 5", // Menütext (Stadtfunktionen)
    "Button 280 330 30008 30009 0 20 0", //MenüButton 2
    "text 297 327 33 6", // Menütext (Steuerfunktionen)
    "Button 280 350 30008 30009 0 30 0", //MenüButton 3
    "text 297 347 33 7", // Menütext (Wahlfunktionen)
    "resizepic 560 250 9400 300 300",
    "resizepic 590 260 9400 240 30",
    "gumppic 560 290 1418",
    "TilePic 215 520 5578",    //Fahnelinks
    "TilePic 850 520 5579",    //Fahnerechts
    "text 660 267 42 58", //Neues Quartal Überschrift
    "Button 590 310 30008 30009 1 0 407", //Menü2Button 1
    "text 607 307 33 59", //Menü2Text (neues quartal)
    "text 607 327 33 60", //Menü2Text (...)
    "page 205", //Steuersäumigen Liste SubMenü
    //################################################################
    "gumppic 250 290 1418",    //BurgPic
    "text 379 267 42 4", //Überschrift (Menü)
    "Button 280 310 30008 30009 0 10 0", //MenüButton 1
    "text 297 307 33 5", // Menütext (Stadtfunktionen)
    "Button 280 330 30008 30009 0 20 0", //MenüButton 2
    "text 297 327 33 6", // Menütext (Steuerfunktionen)
    "Button 280 350 30008 30009 0 30 0", //MenüButton 3
    "text 297 347 33 7", // Menütext (Wahlfunktionen)
    "resizepic 560 250 9400 300 300",
    "resizepic 590 260 9400 240 30",
    "gumppic 560 290 1418",
    "TilePic 215 520 5578",    //Fahnelinks
    "TilePic 850 520 5579",    //Fahnerechts
    "text 660 267 42 61", //Liste der Steuersäumigen Überschrift
    "HtmlGump 580 330 260 200 62 1 1", //Menü2Text (Steuersäumige)
    "text 607 327 33 63", //Menü2Text (...)
    "page 206", //Steuern abheben SubMenü
    //###############################################################
    "gumppic 250 290 1418",    //BurgPic
    "text 379 267 42 4", //Überschrift (Menü)
    "Button 280 310 30008 30009 0 10 0", //MenüButton 1
    "text 297 307 33 5", // Menütext (Stadtfunktionen)
    "Button 280 330 30008 30009 0 20 0", //MenüButton 2
    "text 297 327 33 6", // Menütext (Steuerfunktionen)
    "Button 280 350 30008 30009 0 30 0", //MenüButton 3
    "text 297 347 33 7", // Menütext (Wahlfunktionen)
    "resizepic 560 250 9400 300 300",
    "resizepic 590 260 9400 240 30",
    "gumppic 560 290 1418",
    "TilePic 215 520 5578",    //Fahnelinks
    "TilePic 850 520 5579",    //Fahnerechts
    "text 660 267 42 64", //Steuern abheben Überschrift
    "text 567 290 33 65", //Menü2Text (Steuergelder)
    "text 567 310 30 66", //Menü2Text (vorhanden)
    "Button 590 340 30008 30009 1 0 408", //Menü2Button 1
    "text 607 337 33 67", //Menü2Text (Abheben)
    "page 207", //Steuern zahlen SubMenü
    //###############################################################
    "gumppic 250 290 1418",    //BurgPic
    "text 379 267 42 4", //Überschrift (Menü)
    "Button 280 310 30008 30009 0 10 0", //MenüButton 1
    "text 297 307 33 5", // Menütext (Stadtfunktionen)
    "Button 280 330 30008 30009 0 20 0", //MenüButton 2
    "text 297 327 33 6", // Menütext (Steuerfunktionen)
    "Button 280 350 30008 30009 0 30 0", //MenüButton 3
    "text 297 347 33 7" // Menütext (Wahlfunktionen)
    }
    + GumpPage_SteuernBezahlenLayout(68) // bis Str 71
    +
    {
    "page 30",
    //###############################################################
    "gumppic 250 290 1418",    //BurgPic
    "text 379 267 42 4", //Überschrift (Menü)
    "Button 280 310 30008 30009 0 10 0", //MenüButton 1
    "text 297 307 33 5", // Menütext (Stadtfunktionen)
    "Button 280 330 30008 30009 0 20 0", //MenüButton 2
    "text 297 327 33 6", // Menütext (Steuerfunktionen)
    "Button 280 350 30008 30009 0 30 0", //MenüButton 3
    "text 297 347 33 7", // Menütext (Wahlfunktionen)
    "resizepic 560 250 9400 300 300",
    "resizepic 590 260 9400 240 30",
    "gumppic 560 290 1418",
    "TilePic 215 520 5578",    //Fahnelinks
    "TilePic 850 520 5579",    //Fahnerechts
    "text 660 267 42 72", //WahlfunktionsMenü Überschrift
    "Button 590 310 30008 30009 0 300 0", //Menü2Button 1
    "text 607 307 33 73", //Menü2Text (BMwahl)
    "Button 590 330 30008 30009 0 301 0", //Menü2Button 2
    "text 607 327 33 74", //Menü2Text (Volksentscheid)
    "page 300", //BMWahlSubMenü
    //################################################################
    "gumppic 250 290 1418",    //BurgPic
    "text 379 267 42 4", //Überschrift (Menü)
    "Button 280 310 30008 30009 0 10 0", //MenüButton 1
    "text 297 307 33 5", // Menütext (Stadtfunktionen)
    "Button 280 330 30008 30009 0 20 0", //MenüButton 2
    "text 297 327 33 6", // Menütext (Steuerfunktionen)
    "Button 280 350 30008 30009 0 30 0", //MenüButton 3
    "text 297 347 33 7", // Menütext (Wahlfunktionen)
    "resizepic 560 250 9400 300 300",
    "resizepic 590 260 9400 240 30",
    "gumppic 560 290 1418",
    "TilePic 215 520 5578",    //Fahnelinks
    "TilePic 850 520 5579",    //Fahnerechts
    "text 660 267 42 76", //BMWahlSubMenü Überschrift
    "Button 590 310 30008 30009 0 3000 0", //Menü2Button 1
    "text 607 307 33 77", //Menü2Text (anberaumen)
    "Button 590 330 30008 30009 0 3001 0", //Menü2Button 2
    "text 607 327 33 78", //Menü2Text (Kandidat werden)
    "Button 590 350 30008 30009 0 3002 0", //Menü2Button 1
    "text 607 347 33 79", //Menü2Text (Stimme abgeben)
    "Button 590 370 30008 30009 0 3003 0", //Menü2Button 1
    "text 607 367 33 80", //Menü2Text (Ergebnis)
    "Button 590 390 30008 30009 0 3004 0", //Menü2Button 1
    "text 607 387 33 81", //Menü2Text (Beteiligung)
    "Button 590 410 30008 30009 0 3005 0", //Menü2Button 1
    "text 607 407 33 82", //Menü2Text (Amt übergeben)
    "page 3000", //BMWahlAnberaumen SubSubMenü
    //################################################################
    "gumppic 250 290 1418",    //BurgPic
    "text 379 267 42 4", //Überschrift (Menü)
    "Button 280 310 30008 30009 0 10 0", //MenüButton 1
    "text 297 307 33 5", // Menütext (Stadtfunktionen)
    "Button 280 330 30008 30009 0 20 0", //MenüButton 2
    "text 297 327 33 6", // Menütext (Steuerfunktionen)
    "Button 280 350 30008 30009 0 30 0", //MenüButton 3
    "text 297 347 33 7", // Menütext (Wahlfunktionen)
    "resizepic 560 250 9400 300 300",
    "resizepic 590 260 9400 240 30",
    "gumppic 560 290 1418",
    "TilePic 215 520 5578",    //Fahnelinks
    "TilePic 850 520 5579",    //Fahnerechts
    "text 660 267 42 83", //Anberaumen Überschrift
    "Button 590 310 30008 30009 1 0 409", //Menü2Button 1
    "text 607 307 33 84", //Menü2Text (anberaumen)
    "page 3001", //BMWahlKandidieren SubSubMenü
    //################################################################
    "gumppic 250 290 1418",    //BurgPic
    "text 379 267 42 4", //Überschrift (Menü)
    "Button 280 310 30008 30009 0 10 0", //MenüButton 1
    "text 297 307 33 5", // Menütext (Stadtfunktionen)
    "Button 280 330 30008 30009 0 20 0", //MenüButton 2
    "text 297 327 33 6", // Menütext (Steuerfunktionen)
    "Button 280 350 30008 30009 0 30 0", //MenüButton 3
    "text 297 347 33 7" // Menütext (Wahlfunktionen)
    }
    + GumpPage_KandidatWerdenLayout(85) // bis Str 87
    +
    {
    "page 3002", //BMWahlWählen SubSubMenü
    //################################################################
    "gumppic 250 290 1418",    //BurgPic
    "text 379 267 42 4", //Überschrift (Menü)
    "Button 280 310 30008 30009 0 10 0", //MenüButton 1
    "text 297 307 33 5", // Menütext (Stadtfunktionen)
    "Button 280 330 30008 30009 0 20 0", //MenüButton 2
    "text 297 327 33 6", // Menütext (Steuerfunktionen)
    "Button 280 350 30008 30009 0 30 0", //MenüButton 3
    "text 297 347 33 7" // Menütext (Wahlfunktionen)
    }
    + GumpPage_WaehlenLayout(88) // bis Str 92
    +
    {
    "page 3003", //ErgebnisWahl SubMenü
    //################################################################
    "gumppic 250 290 1418",    //BurgPic
    "text 379 267 42 4", //Überschrift (Menü)
    "Button 280 310 30008 30009 0 10 0", //MenüButton 1
    "text 297 307 33 5", // Menütext (Stadtfunktionen)
    "Button 280 330 30008 30009 0 20 0", //MenüButton 2
    "text 297 327 33 6", // Menütext (Steuerfunktionen)
    "Button 280 350 30008 30009 0 30 0", //MenüButton 3
    "text 297 347 33 7" // Menütext (Wahlfunktionen)
    }
    + GumpPage_ErgebnisWahlLayout(93) // bis Str 94
    +
    {
    "page 3004", //Beteiligung SubMenü
    //################################################################
    "gumppic 250 290 1418",    //BurgPic
    "text 379 267 42 4", //Überschrift (Menü)
    "Button 280 310 30008 30009 0 10 0", //MenüButton 1
    "text 297 307 33 5", // Menütext (Stadtfunktionen)
    "Button 280 330 30008 30009 0 20 0", //MenüButton 2
    "text 297 327 33 6", // Menütext (Steuerfunktionen)
    "Button 280 350 30008 30009 0 30 0", //MenüButton 3
    "text 297 347 33 7", // Menütext (Wahlfunktionen)
    "resizepic 560 250 9400 300 300",
    "resizepic 590 260 9400 240 30",
    "gumppic 560 290 1418",
    "TilePic 215 520 5578",    //Fahnelinks
    "TilePic 850 520 5579",    //Fahnerechts
    "text 660 267 42 95", //Beteiligung Überschrift
    "HtmlGump 580 300 260 200 96 1 1", //Menü2Text (Beteiligung)
    "page 3005", //BMWahlAmtÜbergabe SubSubMenü
    //################################################################
    "gumppic 250 290 1418",    //BurgPic
    "text 379 267 42 4", //Überschrift (Menü)
    "Button 280 310 30008 30009 0 10 0", //MenüButton 1
    "text 297 307 33 5", // Menütext (Stadtfunktionen)
    "Button 280 330 30008 30009 0 20 0", //MenüButton 2
    "text 297 327 33 6", // Menütext (Steuerfunktionen)
    "Button 280 350 30008 30009 0 30 0", //MenüButton 3
    "text 297 347 33 7", // Menütext (Wahlfunktionen)
    "resizepic 560 250 9400 300 300",
    "resizepic 590 260 9400 240 30",
    "gumppic 560 290 1418",
    "TilePic 215 520 5578",    //Fahnelinks
    "TilePic 850 520 5579",    //Fahnerechts
    "text 660 267 42 97", //Amtübergabe Überschrift
    "Button 590 310 30008 30009 1 0 410", //Menü2Button 1
    "text 607 307 33 98", //Menü2Text (Amt übergeben)
    "page 301", //Volkesentscheid SubMenü
    //##################################################################
    "gumppic 250 290 1418",    //BurgPic
    "text 379 267 42 4", //Überschrift (Menü)
    "Button 280 310 30008 30009 0 10 0", //MenüButton 1
    "text 297 307 33 5", // Menütext (Stadtfunktionen)
    "Button 280 330 30008 30009 0 20 0", //MenüButton 2
    "text 297 327 33 6", // Menütext (Steuerfunktionen)
    "Button 280 350 30008 30009 0 30 0", //MenüButton 3
    "text 297 347 33 7", // Menütext (Wahlfunktionen)
    "resizepic 560 250 9400 300 300",
    "resizepic 590 260 9400 240 30",
    "gumppic 560 290 1418",
    "TilePic 215 520 5578",    //Fahnelinks
    "TilePic 850 520 5579",    //Fahnerechts
    "text 660 267 42 99", //VolksentscheidSubMenü Überschrift
    "Button 590 310 30008 30009 0 3010 0", //Menü2Button 1
    "text 607 307 33 100", //Menü2Text (anberaumen)
    "Button 590 330 30008 30009 0 3011 0", //Menü2Button 2
    "text 607 327 33 101", //Menü2Text (Stimme abgeben)
    "Button 590 350 30008 30009 0 3012 0", //Menü2Button 1
    "text 607 347 33 102", //Menü2Text (Ergebnis)
    "Button 590 370 30008 30009 0 3013 0", //Menü2Button 1
    "text 607 367 33 103", //Menü2Text (Beteiligung)
    "page 3010", //Anberaumen SubSubMenü
    //################################################################
    "gumppic 250 290 1418",    //BurgPic
    "text 379 267 42 4", //Überschrift (Menü)
    "Button 280 310 30008 30009 0 10 0", //MenüButton 1
    "text 297 307 33 5", // Menütext (Stadtfunktionen)
    "Button 280 330 30008 30009 0 20 0", //MenüButton 2
    "text 297 327 33 6", // Menütext (Steuerfunktionen)
    "Button 280 350 30008 30009 0 30 0", //MenüButton 3
    "text 297 347 33 7", // Menütext (Wahlfunktionen)
    "resizepic 560 250 9400 300 300",
    "resizepic 590 260 9400 240 30",
    "gumppic 560 290 1418",
    "TilePic 215 520 5578",    //Fahnelinks
    "TilePic 850 520 5579",    //Fahnerechts
    "text 660 267 42 104", //Anberaumen Überschrift
    "resizepic 575 290 9400 270 75",
    "TextEntry 580 295 210 60 30 10 105",
    "Button 590 380 30008 30009 1 0 411", //Menü2Button 1
    "text 607 377 33 106", //Menü2Text (Anberaumen)
    "page 3011", //Abstimmung SubMenü
    //################################################################
    "gumppic 250 290 1418",    //BurgPic
    "text 379 267 42 4", //Überschrift (Menü)
    "Button 280 310 30008 30009 0 10 0", //MenüButton 1
    "text 297 307 33 5", // Menütext (Stadtfunktionen)
    "Button 280 330 30008 30009 0 20 0", //MenüButton 2
    "text 297 327 33 6", // Menütext (Steuerfunktionen)
    "Button 280 350 30008 30009 0 30 0", //MenüButton 3
    "text 297 347 33 7" // Menütext (Wahlfunktionen)
    }
    + GumpPage_EntscheidenLayout(107) // bis str 110
    +
    {  
    "page 3012", //Umfrageergebnis SubMenü
    //################################################################
    "gumppic 250 290 1418",    //BurgPic
    "text 379 267 42 4", //Überschrift (Menü)
    "Button 280 310 30008 30009 0 10 0", //MenüButton 1
    "text 297 307 33 5", // Menütext (Stadtfunktionen)
    "Button 280 330 30008 30009 0 20 0", //MenüButton 2
    "text 297 327 33 6", // Menütext (Steuerfunktionen)
    "Button 280 350 30008 30009 0 30 0", //MenüButton 3
    "text 297 347 33 7" // Menütext (Wahlfunktionen)
    }
    + GumpPage_ErgEntscheidLayout(111) // bis str 113
    +
    {
    "page 3013", //Beteiligung SubMenü
    //################################################################
    "gumppic 250 290 1418",    //BurgPic
    "text 379 267 42 4", //Überschrift (Menü)
    "Button 280 310 30008 30009 0 10 0", //MenüButton 1
    "text 297 307 33 5", // Menütext (Stadtfunktionen)
    "Button 280 330 30008 30009 0 20 0", //MenüButton 2
    "text 297 327 33 6", // Menütext (Steuerfunktionen)
    "Button 280 350 30008 30009 0 30 0", //MenüButton 3
    "text 297 347 33 7", // Menütext (Wahlfunktionen)
    "resizepic 560 250 9400 300 300",
    "resizepic 590 260 9400 240 30",
    "gumppic 560 290 1418",
    "TilePic 215 520 5578",    //Fahnelinks
    "TilePic 850 520 5579",    //Fahnerechts
    "text 660 267 42 114", //Beteiligung Überschrift
    "HtmlGump 580 300 260 200 115 1 1" //Menü2Text (Beteiligung)

    };

    gfdata_bm   := {
    "<center>Willkommen in : " + stadt + "</center>",
    "Der amtierende",
    "Bürgermeister ist : ",
    "" + BM,
    "Menü",
    "Stadtfunktionen",
    "Steuerfunktionen",
    "Wahlfunktionen",
    "Stadtfunktionsmenü",
    "Tiel vergeben",
    "Bürgerschaft vergeben", //10 
    "Bürgerschaft wegnehmen",
    "Liste der Bürger",
    "Titelvergabemenü",
    titelliste(item),
    "Titel vergeben",
    "Titel loeschen",
    "->Titelnummer<-",
    "->Citizennummer<-",
    " Bürgerschaft vergeben", 
    AntragsListe(item), //20
    "Antragsnummer: ",
    "Serial",
    "Antrag annehmen",
    "Antrag ablehnen",
    " Bürger rauswerfen",
    buergerListe(item),
    "Buergernummer: ",
    "Serial",
    "Bürger rauswerfen", 
    "Bürgerübersicht", //30
    buergerListe(item),
    "Steuerfunktionsmenü",
    "Grundbesitzer eintragen",
    "Grundbesitzer austragen",
    "Grundbesitzerliste",
    "Steuerhöhe festlegen",
    "Neues Steuerquartal",
    "Liste noch offene Steuern",
    "Steuergelder abheben",
    "Steuern zahlen", //40
    "Grundbesitzer eintragen",
    buergerListeMitGrundbesitz(item),
    "Besitzer:",
    "Serial",
    "Grundbesitzer eintragen",
    "Grundbesitzer austragen",
    grundbesitzerListe(item),
    "Besitzernummer:",
    "Serial",
    "Grundbesitzer austragen",
    "Grundbesitzerübersicht",
    grundbesitzerListe(item),
    " Steuerhöhe", //50
    "Steuerhöhe momentan: " + getObjProperty(item, CPROP_STEUERBETRAG),
    "Neue Steuerhoehe: ",
    "Steuerhöhe", 
    "Steuerhöhe festlegen",
    " Steuerquartal",
    "Neues Steuerquartal",
    "festlegen",
    " Steuersäumige",
    offeneSteuernListe(item),
    "", //60
    "Steuerauszahlung",
    "Zurzeit vorhandene Steuergelder: ",
    getObjProperty(item, CPROP_STEUERGELDER), //60
    "Steuern abheben"
    }
    + GumpPage_SteuernBezahlenDataPlus3(item)
    +
    {
    "Wahlfunktionsmenü",
    "Wahl des Bürgermeisters",
    "Volksentscheid",
    "",
    "   Wahlmenü", //70
    "Anberaumen",
    "Kandidat werden",
    "Wählen", 
    "Ergebnis",
    "Beteiligung",
    "Amt übergeben",
    "   Anbraumen",
    "Anberaumen"
    }
    + GumpPage_KandidatWerdenDataPlus2()  //82
    + GumpPage_WaehlenDataPlus3(kandidatenListe(item)) //85
    + GumpPage_ErgebnisWahlDataPlus1(BMWahlErgebnisListe(item)) //87
    +
    {
    "Wahlbeteiligung",
    BMWahlBeteiligung(item),
    "Amtübergabe", //90
    " Amt übergeben",
    "Volksentscheidungsmenü",
    "Anberaumen",
    "Stimme abgeben",
    "Ergebnis",
    "Beteiligung", 
    "  Anberaumen",
    "Hier Umfragetext eintragen",
    "Anberaumen mit obigem Text"
    }
    + GumpPage_EntscheidenDataPlus3(item)
    + GumpPage_EntErgData(item)
    +
    {
    "  Beteiligung",
    EntscheidBeteiligung(item),  
    "Bezeichnung:",
    "Name"
    };
    return ( {gflayout_bm, gfdata_bm} );
endfunction
/*
################
#              #
#  sontige GP  #
#              #
################
*/
function GumpPage_BuergerAnderswo(who, item)
    var stadt := GetObjProperty(item, CPROP_STADT);
    if (!stadt)
        stadt := "CProp " + CPROP_STADT + " setzen";
    else
        stadt := stadt;
    endif
    var BM := GetObjProperty(item, CPROP_BUERGERMEISTER);
    if (!BM)
        BM := "CProp " + CPROP_BUERGERMEISTER + " setzen";
    endif

    gflayout_sonstiges := {
    "page 0",
    "resizepic 250 250 9400 300 300", //Hintergrund
    "resizepic 280 260 9400 240 30",  //Kästchen für Überschrift
    "page 1",
    //############################################################
    "gumppic 250 290 1418",  //BurgPic
    "gumppic 260 300 30079", //Rahmen
    //"text 310 267 42 0",       //Überschrift (Willkommen...)
    "htmlgump 280 265 240 40 0 0 0",
    "text 285 320 33 1",       //Amtierender BM...
    "text 285 340 33 2",       //...
    "text 285 360 33 3",       //...
    "TilePic 215 520 5578",    //Fahnelinks
    "TilePic 540 520 5579",    //Fahnerechts
    "Button 300 400 4502 4502 0 2 0",   //Weiterbutton zur Menüpage (Page 2)
    "page 2",
    //############################################################
    "gumppic 250 290 1418", //BurgPic
    "TilePic 215 520 5578",    //Fahnelinks
    "TilePic 540 520 5579",    //Fahnerechts
    "text 360 267 42 4", //Kein Bürger...
    "text 285 320 33 5", //...
    "text 285 340 30 6" //...  
    };

    gfdata_sonstiges   := {
    "<center>Willkommen in : " + stadt + "</center>",
    "Der amtierende",
    "Bürgermeister ist : ",
    "" + BM,
    "Kein Zutritt!!!",
    "Ihr seid bereits Bürger in: ",
    GetObjProperty(who, CPROP_STADT)
    };
    return ( {gflayout_sonstiges, gfdata_sonstiges} );
endfunction
/*
################
#              #
# kein Buerger #
#      GP      #
################
*/
function GumpPage_KeinBuerger(item)
    var stadt := GetObjProperty(item, CPROP_STADT);
    if (!stadt)
        stadt := "CProp " + CPROP_STADT + " setzen";
    else
        stadt := stadt;
    endif
    var BM := GetObjProperty(item, CPROP_BUERGERMEISTER);
    if (!BM)
        BM := "CProp " + CPROP_BUERGERMEISTER + " setzen";
    endif

    gflayout_keinbuerger := {
    "page 0",
    "resizepic 250 250 9400 300 300", //Hintergrund
    "resizepic 280 260 9400 240 30",  //Kästchen für Überschrift
    "page 1",
    //############################################################
    "gumppic 250 290 1418",  //BurgPic
    "gumppic 260 300 30079", //Rahmen
    //"text 310 267 42 0",       //Überschrift (Willkommen...)
    "htmlgump 280 265 240 40 0 0 0",
    "text 285 320 33 1",       //Amtierender BM...
    "text 285 340 33 2",       //...
    "text 285 360 33 3",       //...
    "TilePic 215 520 5578",    //Fahnelinks
    "TilePic 540 520 5579",    //Fahnerechts
    "Button 300 400 4502 4502 0 2 0",   //Weiterbutton zur Menüpage (Page 2)
    "page 2",
    //#############################################################
    "gumppic 250 290 1418",
    "TilePic 215 520 5578",    //Fahnelinks
    "TilePic 540 520 5579",    //Fahnerechts
    "text 379 267 42 4", //Menü Überschrift
    "Button 280 310 30008 30009 0 3 0", //Menübutton 1
    "text 297 307 33 5",  //Antrag stellen...
    "text 297 327 33 6",  //...
    "page 3",
    //##############################################################
    "gumppic 250 290 1418",
    "text 360 267 42 7", //Menü Überschrift
    "Button 280 310 30008 30009 0 3 0",
    "text 297 307 33 8",
    "text 297 327 33 9",
    "resizepic 560 250 9400 300 300",
    "resizepic 590 260 9400 240 30",
    "text 660 267 42 10", //Antrag bestätigen Überschrift
    "gumppic 560 290 1418",
    "TilePic 215 520 5578",    //Fahnelinks
    "TilePic 850 520 5579",    //Fahnerechts
    "Button 590 310 30008 30009 1 0 100", //Menü2Button 1
    "text 607 307 33 11" //Menü2Text (Antrag bestätigen)
    };

    gfdata_keinbuerger   := {
    "<center>Willkommen in : " + stadt + "</center>",
    "Der amtierende",
    "Bürgermeister ist : ",
    "" + BM,
    "Menü",
    "Antrag zur Aufnahme",
    "stellen",
    "Menü",
    "Antrag zur Aufnahme",
    "stellen",
    " Antrags Menü",
    "Antrag bestätigen"
    };
    return ( {gflayout_keinbuerger, gfdata_keinbuerger} );
endfunction
/*
################
#              #
# Antrag zurück#
#     GP       #
################
*/
function GumpPage_BuergerAntrag(item)
    var stadt := GetObjProperty(item, CPROP_STADT);
    if (!stadt)
        stadt := "CProp " + CPROP_STADT + " setzen";
    else
        stadt := stadt;
    endif
    var BM := GetObjProperty(item, CPROP_BUERGERMEISTER);
    if (!BM)
        BM := "CProp " + CPROP_BUERGERMEISTER + " setzen";
    endif

    gflayout_antragzurueck := {
    "page 0",
    "resizepic 250 250 9400 300 300", //Hintergrund
    "resizepic 280 260 9400 240 30",  //Kästchen für Überschrift
    "page 1",
    //############################################################
    "gumppic 250 290 1418",  //BurgPic
    "gumppic 260 300 30079", //Rahmen
    "htmlgump 280 265 240 40 0 0 0",
    //"text 310 267 42 0",       //Überschrift (Willkommen...)
    "text 285 320 33 1",       //Amtierender BM...
    "text 285 340 33 2",       //...
    "text 285 360 33 3",       //...
    "TilePic 215 520 5578",    //Fahnelinks
    "TilePic 540 520 5579",    //Fahnerechts
    "Button 300 400 4502 4502 0 2 0",   //Weiterbutton zur Menüpage (Page 2)
    "page 2",
    //#############################################################
    "gumppic 250 290 1418",
    "TilePic 215 520 5578",    //Fahnelinks
    "TilePic 540 520 5579",    //Fahnerechts
    "text 379 267 42 4", //Menü Überschrift
    "Button 280 310 30008 30009 0 3 0",
    "text 297 307 33 5",
    "text 297 327 33 6",
    "page 3",
    //##############################################################
    "gumppic 250 290 1418",
    "text 360 267 42 7", //Menü Überschrift
    "Button 280 310 30008 30009 0 3 0",
    "text 297 307 33 8",
    "text 297 327 33 9",
    "resizepic 560 250 9400 300 300",
    "resizepic 590 260 9400 240 30",
    "text 660 267 42 10", //Antrag bestätigen Überschrift
    "gumppic 560 290 1418",
    "TilePic 215 520 5578",    //Fahnelinks
    "TilePic 850 520 5579",    //Fahnerechts
    "Button 590 310 30008 30009 1 0 200",
    "text 607 307 33 11"
    };

    gfdata_antragzurueck   := {
    "<center>Willkommen in : " + stadt + "</center>",
    "Der amtierende",
    "Bürgermeister ist : ",
    "" + BM,
    "Menü",
    "Wollt ihr den",
    "Antrag zurückziehen",
    "Menü",
    "Wollt ihr den",
    "Antrag zurückziehen",
    " Antrags Menü",
    "Anullierung bestätigen"
    };
    return ( {gflayout_antragzurueck, gfdata_antragzurueck} );
endfunction
