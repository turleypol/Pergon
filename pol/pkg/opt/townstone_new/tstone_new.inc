/**
  * #########################
  * #                       #
  * # allgemeine Funktionen #
  * # Taya Jan 2006         #
  * #                       #
  * #########################
  */

// Falls who eine Incognito-Robe anhat oder den Incognito-Spruch
// ausgeführt hat, wird "ja" geantwortet. Sonst "nein"
function istInkognito(who)
    if (GetObjProperty(who, "spell_incognito") || GetObjProperty(who, "realrname"))
        return "ja";
    else
        return "nein";
    endif
endfunction

/**
  * Je nach Auswahl des Users wird hier die
  * entsprechende Funktion ermittelt und
  * auch ausgeführt.
  *
  * Return 1 = Dialog erneut aufrufen
  *        0 = sonst
  */
function auswahlDerFunktion(choice, who, item)

// tipp von itchy
/*
if (gumreturn[buttonid])
..
elseif (gumpreturn[andereid])
...
*/

    var result1;
    var result2;

    var his_choice := choice[0];
    // SendSysMessagePergon(who, "r1 , r2: " + result1 + "," +result2 );
    case (his_choice)
        // Stadtfunktionen
        100: 
            setBuergerAntrag(who, item);
            return 0;
        200: 
            removeBuergerAntrag(who, item);
            return 0;
        310: 
            removeBuerger(who, item);
            return 0;
        320: 
            steuernBezahlen(who, item);
            return 0;
        400: 
            result1 := choice[2][3, len(choice[2])]; 
            result2 := choice[3][3, len(choice[3])]; 
            titelVergeben(result1, result2, item, who);
            return 0;
        401: 
            result1 := choice[4][3, len(choice[4])]; 
            acceptBuergerAntrag(result1, item, who);
            return 0;
        402: 
            result1 := choice[4][3, len(choice[4])]; 
            denyBuergerAntrag(result1, item, who);
            return 0;
        403: 
            result1 := choice[5][3, len(choice[5])]; 
            removeBuergerBySerial(result1, item, who);
            return 0;
        413: 
            result1 := choice[3][3, len(choice[3])]; 
            alleTitelWegnehmen(result1, item, who);
            return 0;
        // Steuerfunktionen
        404: 
            result1 := choice[6][3, len(choice[6])]; 
            result2 := choice[7][3, len(choice[7])]; 
            grundbesitzerHinzufuegen(result1, result2, item, who);
            return 0;
        405: 
            result1 := choice[8][3, len(choice[8])]; 
            grundbesitzerEntfernen(result1, item, who);
            return 0;
        406: 
            result1 := choice[9][3, len(choice[9])]; 
            steuerHoeheFestlegen(result1, item, who);
            return 0;
        407: 
            steuerQuartalBeginnen(item, who);
            return 0;
        408: 
            steuerGelderAbheben(item, who);
            return 0;
        // Wahlen
          // Wahl des BM
            409: 
                BMWahlStart(item, who);
                return 0;
            300: 
                BMWahlKandidieren(item, who);
                return 0;
            301: 
                BMWahlKandidaturZurueckziehen(item, who);
                return 0;
            302: 
                result1 := choice[1][3, len(choice[1])]; 
                BMWahlWaehlen(result1, item, who);
                return 0;
          410: 
                BMWahlAmtUebergeben(item, who);
                return 0;
          // Entscheid
          411: 
                result1 := choice[10][5, len(choice[10])]; 
                EntscheidStart(result1, item, who);
                return 0;
          330: 
                // SendSysMessagePergon(who,"Knopf JA gedrückt.");
                EntscheidJaNein(item, who, "JA"); // mit ja gestimmt
                return 0;
          340: 
                // SendSysMessagePergon(who,"Knopf NEIN gedrückt.");
                EntscheidJaNein(item, who, "NEIN");// mit nein gestimmt
                return 0;
          // Wahl des Stadtrats
          412: return 0;
            // StadtratWahlStart(item, who);
        // GM Funktionen
        500:
            result1 := choice[11][5, len(choice[11])]; 
            titelHinzufuegen(result1, item, who);
            return 0;
        501: 
            result2 := choice[12][5, len(choice[12])]; 
            titelEntfernen(result2, item, who);
            return 0;
        502: 
            setzeBM(who, item);
            return 0;
        503: 
            // Name der Stadt setzen. Neue Funktion. Nur wenn kein Bürger eingetragen.
            result1 := choice[13][5, len(choice[13])]; 
            setzeStadtName(result1, who, item);
            return 0;
        504: 
            // laufende Wahl des BM unterbrechen.
            BMWahlStoppen(item, who);
            return 0;
        505:
            EntscheidStoppen(item,who);
            return 0;
    default:
        SendSysMessagePergon(who, "Auf Bald.");
    endcase
endfunction

/**
  * Hier kann der Name der Stadt gesetzt, bzw.
  * geändert werden. Bei einer Änderung darf 
  * es allerdings keinen Bürger geben.
  */
function setzeStadtName(StadtName, god, item)
    var BListe := getObjProperty(item, CPROP_BUERGERLISTE);
    if (StadtName != "Hier Stadtnamen eintragen")
        if (!BListe || BListe.size() < 1)
            setObjProperty(item, CPROP_STADT, StadtName);
            setObjProperty(item, CPROP_ENTSCHEIDTEXT, " ");
            setObjProperty(item, CPROP_STEUERBETRAG, 60000);
        else
            SendSysMessagePergon(god, "Es scheinen noch Buerger in der Stadt zu leben.");
        endif
    endif
endfunction
/**
  * Abfragen des neuen BM durch Target und
  * setzen der nötigen angaben
  */
function setzeBM(god, tstone)
    var BM := target(god);
    if ((BM.isa(POLCLASS_MOBILE)) && (!BM.isa(POLCLASS_NPC)))
        // noch prüfen, ob dieser Char Bürger ist
        var BListe := getObjProperty(tstone, CPROP_BUERGERLISTE);
        var vorhanden := BListe[BM.serial];
        if (vorhanden)
            var BListe := getObjProperty(tstone, CPROP_BUERGERLISTE);
            var Eintrag := array;
            //alten Bürgermeister entfernen
            var BMSerialAlt := getObjProperty(tstone, CPROP_BUERGERMEISTERSERIAL);
            var citizen := SystemFindObjectBySerial(BMSerialAlt, SYSFIND_SEARCH_OFFLINE_MOBILES);
            if (citizen)
                Var stelle := find(citizen.title_suffix, "BM", 1);
                if (!stelle)
                    SendSysMessagePergon(god, "Den Namen des alten Buergermeisters in der Schneiderpuppe pruefen.");
                else
                    Eintrag := Bliste[citizen.serial];
                    Eintrag[1] := citizen.name;
                    BListe[citizen] := Eintrag;
                    citizen.title_suffix := citizen.title_suffix[5, len(citizen.title_suffix)];
                    citizen.cmdlevel := CMDLEVEL_PLAYER;
                    SendSysMessagePergon(god, "Dem alten Buergermeister wird eine entsprechende Nachricht gesandt");
                    sendeNachrichtAnSerial(citizen.serial, "Euer Amt als Buergermeister in der Stadt " + getObjProperty(tstone, CPROP_STADT) + " ist euch entzogen worden.");
                endif
            else
                SendSysMessagePergon(god, "Der alte Buergermeister wurde nicht gefunden.");
            endif
            Eintrag := Bliste[citizen.serial];
            Eintrag[1] := citizen.name + "(BM)";
            BListe[citizen] := Eintrag;
            setObjProperty(tstone, CPROP_BUERGERLISTE, BListe);
            BM.title_suffix := " BM " + BM.title_suffix;
            setObjProperty(tstone, CPROP_BUERGERMEISTER, BM.name);
            setObjProperty(tstone, CPROP_BUERGERMEISTERSERIAL, BM.serial);
            BM.cmdlevel := CMDLEVEL_COUNSELOR;
            SendSysMessagePergon(god, "Der neue Buergermeister wurde gesetzt!");
            sendeNachrichtAnSerial(BM.serial, "Ihr seid nun Buergermeister der Stadt " + getObjProperty(tstone, CPROP_STADT) + ".");
        else
            SendSysMessagePergon(god, "Dieser Character ist kein Buerger.");
        endif
    endif
endfunction

/**
  * Erzeugt aus einem Dictionary eine nummerierte
  * Liste zur Auswahl durch den User.
  */
function listeErzeugen(item, meinDict, Bezeichnung)
    var inputDict := dictionary;
    inputDict := getObjProperty(item, meinDict);
    var Ausgabe := "";
    var i := 1;
    foreach Eintrag in (inputDict)
        // in der ersten Eigenschaft des Eintrags im Dictionary muss der Name sein.
        Ausgabe += i +"  " + Eintrag[Bezeichnung] + "\n";
        i+=1;
    endforeach
    return Ausgabe;
endfunction

/**
  * Erzeugt aus einem Array eine nummerierte
  * Liste zur Auswahl durch den User.
  */
function listeErzeugenAusArray(meinArray)
    var Ausgabe := "";
    var i := 1;
    foreach name in (meinArray)
        // in der ersten Eigenschaft des Eintrags im Dictionary muss der Name sein.
        Ausgabe += _name_iter +"  " + name + "\n";
    endforeach
    return Ausgabe;
endfunction

/**
  * Holt den passenden Eintrag zur Liste aus der
  * Funktion listeErzeugen.
  */
function schluesselAusListe(item, cprop, nummer)
    var Liste := getObjProperty(item, cprop);
    // das ist von Turley. Geht das überhaupt?
//    if (Liste.keys()[nummer])
//        return(Liste[Liste.keys()[nummer]]);
//    else
//        return(0);
//    endif
    // so wars alt 
    var SchluesselListe := {};
    SchluesselListe := Liste.keys();
    var Schluessel := 0;
    foreach schl in (SchluesselListe)
        if (_schl_iter == nummer)
            Schluessel := schl;
            break;
        endif
    endforeach
    return Schluessel; 
endfunction

/**
  * Nachricht an einen Char versenden. Je nachdem
  * ob er on ist. Die Serial wird verwendet, um festzustellen
  * ob der Char online ist.
  */
function sendeNachrichtAnSerial(SchluesselCitizen, text)
    var citizen;
    citizen := SystemFindObjectBySerial(SchluesselCitizen);
    if (!citizen)
        citizen := SystemFindObjectBySerial(SchluesselCitizen,SYSFIND_SEARCH_OFFLINE_MOBILES);
        if (!citizen)
            // Char wurde nicht gefunden
        else
            // Char gefunden, ist aber wohl offline. LogonMessage hinterlegen.
            var Nachricht := getObjProperty(citizen, CPROP_LOGONMESSAGE);
            if (!Nachricht)
                Nachricht := text;
            else
                Nachricht += " ---- " + text;
            endif
            setObjProperty(citizen, CPROP_LOGONMESSAGE, Nachricht);
        endif
    else // online
        SendStringAsTipWindow(citizen, text);
    endif
endfunction
