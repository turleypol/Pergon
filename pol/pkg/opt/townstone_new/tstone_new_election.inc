/**
  * #################
  * #               #
  * # Wahlen        #
  * # Taya Jan 2006 #
  * #               #
  * #################
  */
/**
  * Auf das Ende der diversen Wahlen prüfen
  */
function pruefeEndeWahlen(tstone)
    // Ende der Wahl zum BM
    if (CInt(getObjProperty(tstone, CPROP_BMWAHLLAEUFT)) == 1) // nur interessant wenn BM-Wahl läuft
        //ist die Zeit abgelaufen?
        if (ReadGameclock() > getObjProperty(tstone, CPROP_BMWAHLZEITENDE))
            BMWahlBeende(tstone);
        else
            // hat schon jeder Bürger gewählt?
            var BListe := getObjProperty(tstone, CPROP_BUERGERLISTE);
            var gefunden := false;
            foreach Eintrag in BListe
                if (!Eintrag.Wahl) // jemand hat noch nicht gewählt
                    gefunden := true;
                    break;
                endif
            endforeach
            if (!gefunden)
                BMWahlBeende(tstone);
                print("BM-Wahl wurde beendet.");
            endif
        endif
    endif

    // Ende des Entscheids
    if (CInt(getObjProperty(tstone, CPROP_ENTSCHEIDLAEUFT)) == 1) // nur interessant, wenn Entscheid läuft
        // ist die Zeit abgelaufen?
        if (ReadGameclock() > getObjProperty(tstone, CPROP_ENTSCHEIDZEITENDE))
            EntscheidBeenden(tstone);
        else
            // hat schon jeder Bürger entschieden?
            var BListe := getObjProperty(tstone, CPROP_BUERGERLISTE);
            var gefunden := false;
            foreach Eintrag in BListe
                if (Eintrag.Abstimmung != "entschieden") // jemand hat noch nicht gewählt
                    gefunden := true;
                    break;
                endif
            endforeach
            if (!gefunden)
                EntscheidBeenden(tstone);
                print("Entscheid wurde beendet.");
            endif
        endif
    endif
endfunction

/**
  * #######################
  * #                     #
  * # Funktionen zur Wahl #
  * #                     #
  * #######################
  */

/**
  * Starten der Wahl. Die Bürgerliste wird zurück
  * gesetzt. Das Wahl-Kennzeichen gesetzt und die Wahlliste,
  * sowie die Kandidatenliste gelöscht.
  */
function BMWahlStart(tstone, BM)
    SendSysMessagePergon(BM,"WahlLaeuft = " + getObjProperty(tstone, CPROP_BMWAHLLAEUFT));
    if (CInt(getObjProperty(tstone, CPROP_BMWAHLLAEUFT)) == 1)
        SendSysMessagePergon(BM,"Es laeuft doch bereits eine Wahl!");
    else
        var BListe := getObjProperty(tstone, CPROP_BUERGERLISTE);
        if (BListe.size() > 0)
            setObjProperty(tstone, CPROP_BMWAHLLAEUFT, 1);
            setObjProperty(tstone, CPROP_BMWAHLZEITENDE, ReadGameclock() + ZEIT_BMWAHL);
            eraseObjProperty(tstone, CPROP_BMKANDIDATENLISTE);
            eraseObjProperty(tstone, CPROP_BMWAHLSIEGER);
            eraseObjProperty(tstone, CPROP_BMWAHLLISTE);
            // BürgerListe zurücksetzen
            var BListe := getObjProperty(tstone, CPROP_BUERGERLISTE);
            if (!BListe)
                BListe := dictionary;
            endif
            // Altes Wahlkennzeichen löschen
            foreach name in BListe
                name.Wahl := false;
            endforeach
            setObjProperty(tstone, CPROP_BUERGERLISTE, BListe);
            SendSysMessagePergon(BM,"Die Laenge der Wahl ist " + ZEIT_BMWAHL_TEXT);
        else
            SendSysMessagePergon(BM,"Ohne Buerger ist eine Wahl nicht unbedingt sinnvoll.");
        endif
    endif
endfunction

function BMWahlBeende(tstone)
    var text := "Die Wahl wurde beendet, aber es konnte kein Gewinner ermittelt werden!";
    setObjProperty(tstone, CPROP_BMWAHLLAEUFT, -1);
    // Die Wahl ist gewonnen, wenn es nur einen mit höchster Stimmenzahl gibt
    var KListe := getObjProperty(tstone, CPROP_BMKANDIDATENLISTE);
    var MaxEintrag := array;
    MaxEintrag[1] := "niemand";
    MaxEintrag[2] := 0;
    MaxEintrag[3] := 0;
    var maxAnzahl := 0;
    foreach Eintrag in KListe
        if (CInt(Eintrag[2]) > CInt(MaxEintrag[2]))
            MaxEintrag := Eintrag;
            maxAnzahl := 1;
        elseif (CInt(Eintrag[2]) == CInt(MaxEintrag[2]))
            maxAnzahl += 1;
        endif
    endforeach
    if (maxAnzahl == 1)
        // Dies ist der Gewinner
        setObjProperty(tstone, CPROP_BMWAHLSIEGER, MaxEintrag);
        text := "Die Wahl des Buergermeisters ist sograde beendet worden.";
    endif
    sendeNachrichtAnSerial(getObjProperty(tstone, CPROP_BUERGERMEISTERSERIAL), text);
endfunction

/**
  * Wenn der GL die Wahl unterbricht, werden alle Einträge
  * zurückgesetzt.
  */
function BMWahlStoppen(tstone, god)
    if (CInt(getObjProperty(tstone, CPROP_BMWAHLLAEUFT)) <> 1)
        SendSysMessagePergon(god,"Es laeuft zurzeit keine Wahl.");
    else
        setObjProperty(tstone, CPROP_BMWAHLLAEUFT, false);
        eraseObjProperty(tstone, CPROP_BMKANDIDATENLISTE);
        eraseObjProperty(tstone, CPROP_BMWAHLLISTE);
        var BListe := getObjProperty(tstone, CPROP_BUERGERLISTE);
        foreach Buerger in BListe
            Buerger.Wahl := false;
        endforeach
        setObjProperty(tstone, CPROP_BUERGERLISTE, BListe);
        SendSysMessagePergon(god,"Die Wahlunterlagen wurden vernichtet.");
    endif
endfunction

function BMWahlKandidieren(tstone, citizen)
    if (CInt(getObjProperty(tstone, CPROP_BMWAHLLAEUFT)) == 1)
        var BListe := getObjProperty(tstone, CPROP_BUERGERLISTE);
        if (BListe[citizen.serial])
            var KListe := getObjProperty(tstone, CPROP_BMKANDIDATENLISTE);
            if (!KListe)
                KListe := dictionary;
            endif
            // wenn dieser Char nicht schon in der Kandidatenliste ist hinzufügen
            if (!KListe[citizen.serial])
                var Eintrag := array;
                Eintrag[1] := citizen.name;
                Eintrag[2] := 0; //erhaltene Stimmen
                Eintrag[3] := citizen.serial;
                KListe[citizen.serial] := Eintrag;
                setObjProperty(tstone, CPROP_BMKANDIDATENLISTE, KListe);
                SendSysMessagePergon(citizen,"Ihr seid nun als Kandidat zur Wahl aufgestellt.");
            else
                SendSysMessagePergon(citizen,"Ihr seid doch bereits Kandidat!");
            endif
        else
            SendSysMessagePergon(citizen,"Ihr seid kein Buerger dieser Stadt!");
        endif
    else
        SendSysMessagePergon(citizen,"Zurzeit laeuft keine Wahl.");
    endif
endfunction

function kandidatenListe(item)
    return listeErzeugen(item, CPROP_BMKANDIDATENLISTE, 1);
endfunction

function BMWahlKandidaturZurueckziehen(tstone, citizen)
    if (CInt(getObjProperty(tstone, CPROP_BMWAHLLAEUFT)) == 1) // diese Funktion nur während einer laufenden Wahl
        var TListe := getObjProperty(tstone, CPROP_BMKANDIDATENLISTE);
        var Eintrag := TListe[citizen.serial];
        if (!Eintrag)
            SendSysMessagePergon(citizen,"Ihr seid doch gar nicht aufgestellt!");
        else
            if (Eintrag[2]) // schon Stimmen erhalten
                SendSysMessagePergon(citizen,"Ihr habt bereits mind. eine Stimme.");
            else
                TListe.erase(citizen.serial);
                setObjProperty(tstone, CPROP_BMKANDIDATENLISTE, TListe);
                SendSysMessagePergon(citizen,"Kandidatur wurde zurueckgezogen.");
            endif
        endif
    else
        SendSysMessagePergon(citizen,"Es lauft keine Wahl.");
    endif
endfunction


function BMWahlWaehlen(NummerKandidat, tstone, citizen)
    if (getObjProperty(tstone, CPROP_BMWAHLLAEUFT)) // diese Funktion nur während einer laufenden Wahl
        var WListe := getObjProperty(tstone, CPROP_BMWAHLLISTE);
        if (!WLISTE)
            WListe := dictionary;
        endif
        if (!WListe[citizen.acctname]) // nur, wenn mit diesem account noch nicht gewählt wurde
            NummerKandidat := CInt(NummerKandidat);
            if (NummerKandidat > 0)
                var SchluesselKandidat := schluesselAusListe(tstone, CPROP_BMKANDIDATENLISTE, NummerKandidat);
                if (!SchluesselKandidat)
                    SendSysMessagePergon(citizen,"Bitte eine gueltige Nummer fuer einen Kandidaten angeben.");
                else
                    // jetzt die Stimme zählen
                    var TListe := getObjProperty(tstone, CPROP_BMKANDIDATENLISTE);
                    TListe[SchluesselKandidat][2] := CInt(TListe[SchluesselKandidat][2]) + 1;
                    setObjProperty(tstone, CPROP_BMKANDIDATENLISTE, TListe);
                    // nun noch den account zu den bereits gewählten hinzufügen
                    WListe.insert(citizen.acctname, 1);
                    setObjProperty(tstone, CPROP_BMWAHLLISTE, WListe);
                    // zum schluss noch in der buergerliste alle chars mit diesem account kennzeichnen                    
                    var BLIste := getObjProperty(tstone, CPROP_BUERGERLISTE);
                    foreach Buerger in BListe
                        if (citizen.acctname == Buerger.Login)
                            Buerger.Wahl := "gewaehlt"; // in der Bürgerliste als schon gewählt eintragen
//                            SendSysMessagePergon(citizen,"Account " + citizen.acctname + ", char: " + Buerger[1]);
                        endif
                    endforeach
                    setObjProperty(tstone, CPROP_BUERGERLISTE, BListe);
                    SendSysMessagePergon(citizen,"Besten Dank. Ihre Stimme wurde gezaehlt.");
                endif
            else
                SendSysMessagePergon(citizen,"Bitte eine gueltige Nummer eintragen.");
            endif    
        else
            SendSysMessagePergon(citizen,"Ihr habt bereits gewaehlt.");
        endif
    else
        SendSysMessagePergon(citizen,"Zurzeit laeuft keine Wahl.");
    endif
    
    
    
    
//    if (NummerKandidat > 0)
//        if (getObjProperty(tstone, CPROP_BMWAHLLAEUFT)) // diese Funktion nur während einer laufenden Wahl
//            var BListe := getObjProperty(tstone, CPROP_BUERGERLISTE);
//            var waehler := BListe[citizen.serial];
//            if (!waehler[5]) // hat noch nicht gewählt
//                var SchluesselKandidat := schluesselAusListe(tstone, CPROP_BMKANDIDATENLISTE, NummerKandidat);
//                if (!SchluesselKandidat)
//                    SendSysMessagePergon(citizen,"Bitte eine gueltige Nummer fuer einen Kandidaten angeben.");
//                else
//                    // jetzt die Stimme zählen
//                    var TListe := getObjProperty(tstone, CPROP_BMKANDIDATENLISTE);
//                    var Eintrag := TListe[SchluesselKandidat];
//                    Eintrag[2] := Eintrag[2] + 1;
//                    TListe[SchluesselKandidat] := Eintrag;
//                    setObjProperty(tstone, CPROP_BMKANDIDATENLISTE, TListe);
//                    // alle chars dieses accounts müssen als "schon gewählt" gekennzeichnet werden.
//                    foreach Buerger in BListe
//                        if (cizizen.acctname = Buerger[2])
//                            Buerger[5] := true; // in der Bürgerliste als schon gewählt eintragen
//                        endif
//                    endforeach
//                    setObjProperty(tstone, CPROP_BUERGERLISTE, BListe);
//                    SendSysMessagePergon(citizen,"Besten Dank. Ihre Stimme wurde gezaehlt.");
//                endif
//            else
//                SendSysMessagePergon(citizen,"Ihr habt eure Stimme bereits abgegeben.");
//            endif
//        else
//            SendSysMessagePergon(citizen,"Es laufen doch gar keine Wahlen.");
//        endif
//    endif
endfunction

function BMWahlErgebnisListe(tstone)
    var inputDict := dictionary;
    inputDict := getObjProperty(tstone, CPROP_BMKANDIDATENLISTE);
    var Ausgabe := "";
    var i := 0;
    foreach name in (inputDict)
        // in der ersten Eigenschaft des Eintrags im Dictionary muss der Name sein.
        Ausgabe := Ausgabe + name[1] + ", " + name[2] + "\n";
        i += 1;
    endforeach
    if (i < 10 && CInt(getObjProperty(tstone, CPROP_BMWAHLLAEUFT)) == 1)
        Ausgabe := "Anzeige erst bei aureichender Teilnahme";
    endif
    return Ausgabe;
endfunction

function BMWahlBeteiligung(tstone)
    var inputDict := dictionary;
    inputDict := getObjProperty(tstone, CPROP_BUERGERLISTE);
    if (!inputDict)
        return "CPROP nicht vorhanden";
    endif
    var Ausgabe := "";
    var gewaehlt;
    foreach name in (inputDict)
        // in der ersten Eigenschaft des Eintrags im Dictionary muss der Name sein.
        if (name["Wahl"])
            gewaehlt := "ja";
        else
            gewaehlt := "nein";
        endif
        Ausgabe += name["Name"] + ", " + gewaehlt + "\n";
    endforeach
    return Ausgabe;
endfunction

function BMWahlAmtUebergeben(tstone, BM)
    // dies geht nur wenn ein Sieger hinterlegt wurden
    if (CInt(getObjProperty(tstone, CPROP_BMWAHLLAEUFT)) == 1)
        SendSysMessagePergon(BM,"Nicht solange die Wahl noch läuft.");
    else
        if (BM.serial == CInt(getObjProperty(tstone, CPROP_BUERGERMEISTERSERIAL)))
            var Sieger := getObjProperty(tstone, CPROP_BMWAHLSIEGER);
            if (!Sieger)
                SendSysMessagePergon(BM,"Es gibt keinen Sieger.");
            else
                if (Sieger[3] == BM.serial)
                    sendsysmessagepergon(BM, "Und Somit Habt Ihr Das Amt An Euch Selbst Uebergeben");
                else
                    var Sieger := getObjProperty(tstone, CPROP_BMWAHLSIEGER);
                    //SendSysMessagePergon(BM,"Der Sieger war: " + Sieger);
                    var NeuerBM := SystemFindObjectBySerial(Sieger[3],SYSFIND_SEARCH_OFFLINE_MOBILES);
                    if (!NeuerBM)
                        SendSysMessagePergon(BM,"Da stimmt was nicht. Konnte den neuen Buergermeister nicht finden.");
                    else
                        var BListe := getObjProperty(tstone, CPROP_BUERGERLISTE);
                        // alten BM austragen
                        var TempBM := BListe[CInt(BM.serial)];
                        TempBM.Name := BM.name;
                        BListe[CInt(BM.serial)] := TempBM;
                        BM.title_suffix := BM.title_suffix[5, len(BM.title_suffix)];
                        BM.cmdlevel := CMDLEVEL_PLAYER;
                        SendSysMessagePergon(BM, "Ihr seid nicht laenger Buergermeister diese Stadt.");
                        // neuen BM eintragen
                        TempBM := BListe[CInt(NeuerBM.serial)];
                        TempBM.Name := NeuerBM.name + " (BM)";
                        BListe[CInt(NeuerBM.serial)] := TempBM;
                        setObjProperty(tstone, CPROP_BUERGERLISTE, BListe);
                        // alte Ämter weg!
                        NeuerBM.title_suffix := " BM aus " + getObjProperty(tstone, CPROP_STADT);
                        NeuerBM.cmdlevel := CMDLEVEL_COUNSELOR;
                        setObjProperty(tstone, CPROP_BUERGERMEISTER, NeuerBM.name);
                        setObjProperty(tstone, CPROP_BUERGERMEISTERSERIAL, NeuerBM.serial);
                        sendeNachrichtAnSerial(NeuerBM.serial, "Ihr seid nun Buergermeister der Stadt " + getObjProperty(tstone, CPROP_STADT) + ".");
                    endif
                endif
            endif
        else
            SendSysMessagePergon(BM,"Alter Schlingel! Wie kommst du in dieses Menue?");
        endif
    endif
endfunction

/**
  * ############################
  * #                          #
  * # Funktionen zum Entscheid #
  * #                          #
  * ############################
  */

function EntscheidStart(EntscheidText, tstone, BM)
    if (CInt(getObjProperty(tstone, CPROP_ENTSCHEIDLAEUFT)) == 1)
        SendSysMessagePergon(BM, "Es laeuft doch bereits ein Volksentscheid.");
    else
        if (!EntscheidText || EntscheidText == "")
            SendSysMessagePergon(BM, "Ohne Text geht das nicht!");
        else
            setObjProperty(tstone, CPROP_ENTSCHEIDTEXT, EntscheidText);
            setObjProperty(tstone, CPROP_ENTSCHEIDLAEUFT, 1);
            setObjProperty(tstone, CPROP_ENTSCHEIDZEITENDE, ReadGameclock() + ZEIT_ENTSCHEID);
            var BListe := getObjProperty(tstone, CPROP_BUERGERLISTE);
            foreach Eintrag in BListe
                Eintrag.Abstimmung := false;
            endforeach
            setObjProperty(tstone, CPROP_BUERGERLISTE, BListe);
            eraseObjProperty(tstone, CPROP_ENTSCHEIDERGEBNIS);
            eraseObjProperty(tstone, CPROP_ENTSCHEIDWAHLLISTE);
            SendSysMessagePergon(BM,"Die Laenge des Volksentscheids ist " + ZEIT_ENTSCHEID_TEXT);
        endif
    endif
endfunction

function EntscheidStoppen(tstone, god)
    if (CInt(getObjProperty(tstone, CPROP_ENTSCHEIDLAEUFT)) <> 1)
        SendSysMessagePergon(god,"Es laeuft zurzeit kein Entscheid.");
    else
        eraseObjProperty(tstone, CPROP_ENTSCHEIDTEXT);
        setObjProperty(tstone, CPROP_ENTSCHEIDLAEUFT, 0);
        var BListe := getObjProperty(tstone, CPROP_BUERGERLISTE);
        foreach Buerger in BListe
            Buerger.Abstimmung := false;
        endforeach
        setObjProperty(tstone, CPROP_BUERGERLISTE, BListe);
        SendSysMessagePergon(god,"Die Entscheidunterlagen wurden vernichtet.");
    endif
endfunction

function EntscheidBeenden(tstone)
    setObjProperty(tstone, CPROP_ENTSCHEIDLAEUFT, -1);
    sendeNachrichtAnSerial(getObjProperty(tstone, CPROP_BUERGERMEISTERSERIAL), "Der Entscheid wurde beendet.");
endfunction


function EntscheidJaNein(tstone, citizen, JaNein)
    if (citizen.cmdlevel >= CMDLEVEL_SEER)
        SendSysMessagePergon(citizen,"Na sowas. Welcher Gott wuerde schon entscheiden wollen?");
        return;
    endif
    if (CInt(getObjProperty(tstone, CPROP_ENTSCHEIDLAEUFT)) != 1)
        SendSysMessagePergon(citizen,"Es lauft zurzeit kein Volksentscheid.");
    else
        var WListe := getObjProperty(tstone, CPROP_ENTSCHEIDWAHLLISTE);
        if (!WListe)
            WListe := dictionary;
        EndIf
        if (CInt(WListe[citizen.acctname]) == 1)
            SendSysMessagePergon(citizen,"Ihr habt bereits entschieden.");
        else
            var EntscheidErgebnis := getObjProperty(tstone, CPROP_ENTSCHEIDERGEBNIS);
            if (!EntscheidErgebnis)
                EntscheidErgebnis := array;
                EntscheidErgebnis[1] := 0;
                EntscheidErgebnis[2] := 0;
            endif
            if (JaNein == "JA")
                EntscheidErgebnis[1] += 1;
            else
                EntscheidErgebnis[2] += 1;
            endif
            setObjProperty(tstone, CPROP_ENTSCHEIDERGEBNIS, EntscheidErgebnis);
            WListe.insert(citizen.acctname, 1);
            setObjProperty(tstone, CPROP_ENTSCHEIDWAHLLISTE, WListe);
            var BListe := getObjProperty(tstone, CPROP_BUERGERLISTE);
            foreach Buerger in BListe
                if (citizen.acctname == Buerger.Login)
                    Buerger.Abstimmung := "entschieden"; // in der Bürgerliste als schon entschieden eintragen
                    break;
                endif
            endforeach
            setObjProperty(tstone, CPROP_BUERGERLISTE, BListe);
            SendSysMessagePergon(citizen,"Eure Stimme wurde mit " + JaNein + " aufgenommen.");
        endif
    endif
endfunction

function EntscheidBeteiligung(tstone)
    var inputDict := dictionary;
    inputDict := getObjProperty(tstone, CPROP_BUERGERLISTE);
    if (!inputDict)
        return "CPROP nicht vorhanden";
    endif
    var Ausgabe := "";
    var gewaehlt;
    foreach name in (inputDict)
        // in der ersten Eigenschaft des Eintrags im Dictionary muss der Name sein.
        if (name["Abstimmung"])
            gewaehlt := "ja";
        else
            gewaehlt := "nein";
        endif
        Ausgabe += name["Name"] + ", " + gewaehlt + "\n";
    endforeach
    return Ausgabe;
endfunction

/**
  * #####################################
  * #                                   #
  * # Funktionen zur Wahl des Stadtrats #
  * #                                   #
  * #####################################
  */

function StadtratWahlStart(tstone, BM)
endfunction

function StadtratBeteiligung(tstone)
    return "noch nicht implementiert";
endfunction
