/**
  * #################
  * #               #
  * # Konstanten    #
  * # Globale Vars  #
  * # Taya Jan 2006 #
  * #               #
  * #################
  */

// allgemeine Konstanten
    const true := 1;
    const false := 0;

    // schöne Knopfpaare:
    // 0x13ba und 0x13bc
    // 0x482 und 0x47f
    const KNOPF_FREI := 0x482; // 1154
    const KNOPF_GESPERRT := 0x47f; // 1151
    const BALKEN := 9400;

    // Mindesthöhe und Zeilenhöhe im Gump
    const ZEILE := 25;
    // const MINHEIGHT := 240;
    // const FIRSTLINE := 250;

//Const ZEIT_ABSTIMMUNG := 7*12*3600; //6*3600;  // 1/2 Woche
    Const ZEIT_BMWAHL                := 7*24*3600; 
    const ZEIT_BMWAHL_TEXT           := "eine Woche";
    Const ZEIT_ENTSCHEID             := 7*12*3600; 
    const ZEIT_ENTSCHEID_TEXT        := "eine halbe Woche";

// Konstanten für die CProp's
// Stadt
    const CPROP_STADT                := "Stadt"; // siehe auch createstone.src
    const CPROP_ANTRAG               := "Antrag";
    const CPROP_ANTRAGSTADT          := "AntragStadt";
    const CPROP_ANTRAGLISTE          := "AntragListe"; // dictionary
    const CPROP_BUERGERLISTE         := "BuergerListe";
//    const CPROP_EINWOHNERZAHL        := "EinwohnerZahl"; // siehe auch createstone.src
    const CPROP_BUERGERMEISTER       := "Buergermeister";
    const CPROP_BUERGERMEISTERSERIAL := "BuergerMeisterSerial";
    const CPROP_LOGONMESSAGE         := "LogonMessage";
    const CPROP_TITELLISTE           := "TitelListe";
// Steuer
    const CPROP_STEUERBETRAG         := "Steuerbetrag";
    const CPROP_STEUERLISTE          := "Steuerliste";
    const CPROP_GRUNDBESITZERLISTE   := "GrundbesitzerListe";
    const CPROP_STEUERGELDER         := "SteuerGelder";
// Wahl-System
    // Wahl des BM
    const CPROP_BMWAHLLAEUFT         := "WahlLaeuft";
    const CPROP_BMWAHLZEITENDE       := "wahlZeitEnde";
    const CPROP_BMKANDIDATENLISTE    := "KandidatenListe";
    const CPROP_BMWAHLLISTE          := "WahlListe";
    const CPROP_BMWAHLSIEGER         := "WahlSieger";
    // Umfrage
    const CPROP_ENTSCHEIDLAEUFT      := "EntscheidLaeuft";
    const CPROP_ENTSCHEIDZEITENDE    := "EntscheidZeitEnde";
    const CPROP_ENTSCHEIDERGEBNIS    := "EntscheidErgebnis";
    const CPROP_ENTSCHEIDTEXT        := "EntscheidText";
    const CPROP_ENTSCHEIDWAHLLISTE   := "EntscheidWahlListe";


/**
  * Hier doch noch ein paar globale Variablen :)
  */

// Die Texte aus den Menüs müssen noch gefunden werden. Dies ist
// mit diesen Variablen möglich.

// Bereich Stadt

    // BM Titel an Bürger vergeben
    var InputTitelVergebenNr:=struct;
	InputTitelVergebenNr.+dtidx;
	InputTitelVergebenNr.+txtid;
    var InputTitelVergebenName:=struct;
	InputTitelVergebenName.+dtidx;
	InputTitelVergebenName.+txtid;
    // Antrag auf Bürger annehmen oder ablehnen
    var InputAntragNummer:=struct;
	InputAntragNummer.+dtidx;
	InputAntragNummer.+txtid;
    // BM schmeisst einen Bürger raus
    var InputBuergerRaus:=struct;
	InputBuergerRaus.+dtidx;
	InputBuergerRaus.+txtid;

// Bereich Steuern

    // Liste der Grundbesitzer, Bürger
    var InputGrundListeBuergerNr:=struct;
	InputGrundListeBuergerNr.+dtidx;
	InputGrundListeBuergerNr.+txtid;
    // Liste der Grundbesitzer, Grundbesitzer
    var InputGrundListeGrundNr:=struct;
	InputGrundListeGrundNr.+dtidx;
	InputGrundListeGrundNr.+txtid;
    // Liste der Grundbesitzer, Grundbesitzer
    var InputGrundListeBesitzerNr:=struct;
	InputGrundListeBesitzerNr.+dtidx;
	InputGrundListeBesitzerNr.+txtid;
    // Steuerbetrag
    var InputSteuerBetrag:=struct;
	InputSteuerBetrag.+dtidx;
	InputSteuerBetrag.+txtid;

// Bereich Wahl des BM

    // Nummer des zu wählenden Kandidaten
    var InputBMWahlNr:=struct;
	InputBMWahlNr.+dtidx;
	InputBMWahlNr.+txtid;

// Bereich Entscheid

    // Nummer des zu wählenden Kandidaten
    var InputEntscheidText:=struct;
	InputEntscheidText.+dtidx;
	InputEntscheidText.+txtid;

// Bereich God

    // God legt Titel fest
    var InputGodTitelNeu:=struct;
	InputGodTitelNeu.+dtidx;
	InputGodTitelNeu.+txtid;
    var InputGodTitelLoeschen:=struct;
	InputGodTitelLoeschen.+dtidx;
	InputGodTitelLoeschen.+txtid;
    // God legt die Stadt fest
    var InputGodStadtName:=struct;
	InputGodStadtName.+dtidx;
	InputGodStadtName.+txtid;
