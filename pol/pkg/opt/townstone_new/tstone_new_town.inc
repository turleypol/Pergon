/**
  * ###################
  * #                 #
  * # Stadtfunktionen #
  * # Taya Jan 2006   #
  * #                 #
  * ###################
  */

/**
  * Bürgertum:
  *
  * Wie es aufgenommen wird und wie es abgelegt werden kann.
  *
  * 1. Antrag
  *    Bed:     CPROP_ANTRAG is null
  *           & CPROP_STADT is null
  *    Char:  CPROP_ANTRAG = 1 für wahr
  *           CPROP_ANTRAGSTADT = Stadt
  *    Stone: CPROP_ANTRAGLISTE = +Char
  *
  * 2a) direkter Austritt
  *    Bed:   CPROP_ANTRAG is not null
  *           CPROP_ANTRAG_STADT = Stadt
  *    Char:  del CPROP_ANTRAG
  *           del CPROP_ANTRAGSTADT
  *    Stone: del CPROP_ANTRAGLISTE
  *
  * 2b) Bestätigung durch den BM
  *   i) Ablehnung
  *      siehe 2a)
  *
  *   ii) Annehmen
  *       alles aus 2a)
  *       Char:  CPROP_STADT = Stadt
  *       Stone: CPROP_STADTLISTE = +Char
  *              CPROP_STADTANZAHLBUERGER = +1
  *       Char-Namen ändern
  * 3a) Bürger verlässt die Stadt
  *     Bed:   Char ist kein Grundbesitzer
               Char ist kein Kandidat
  *     Char:  del CPROP_STADT
  *     Stone: CPROP_STADTANZAHL = -1
  *            CPROP_STADTLISTE = -Char
  *     Char-Name ändern
  *
  * 3b) BM schmeisst den Bürger raus
  *     alles aus 3a)
  */

/**
  * Der Char wird gekennzeichnet, dass er in die Stadt aufgenommen werden möchte.
  *
  * citizen : der Char als Objekt
  * tstone  : der Stadtstein als Objekt
  */
function setBuergerAntrag(citizen, tstone)
// Die Bedingung (!CPROP_ANTRAG || !CPROP_STADT) ist schon bei
// der Darstellung des Dialoges geprüft worden.
    SetObjProperty(citizen, CPROP_ANTRAG, true);
    SetObjProperty(citizen, CPROP_ANTRAGSTADT, getObjProperty(tstone, CPROP_STADT));
    // hier muss jetzt noch who in die AntragsListe hinzugefügt werden.
    // speichern von id und name sollte reichen
    var AntragListe := getObjProperty(tstone, CPROP_ANTRAGLISTE);
    if (!AntragListe)
        AntragListe := dictionary;
    endif
    var Eintrag := array;
    Eintrag[1] := citizen.name;
    Eintrag[2] := citizen.acctname;
    AntragListe.insert(citizen.serial, Eintrag);
    setObjProperty(tstone, CPROP_ANTRAGLISTE, AntragListe);
    SendSysMessagePergon(citizen,"Gern haben wir den Antrag entgegengenommen. Der Buergermeister wird sich persoenlich darum kuemmern!");
endfunction

/**
  * Die Kennzeichnung des Chars wird wieder entfernt.
  *
  * citizen : der Char als Objekt
  * tstone  : der Stadtstein als Objekt
  * text    : Dieser Text wird dem citizen zugesandt
  */

function removeBuergerAntrag(citizen, tstone, text := "Wir ihr wuenscht. Antrag storniert.")
    EraseObjProperty(citizen, CPROP_ANTRAG);
    EraseObjProperty(citizen, CPROP_ANTRAGSTADT);
    var AntragListe := dictionary;
    AntragListe := getObjProperty(tstone, CPROP_ANTRAGLISTE);
    AntragListe.erase(citizen.serial);
    setObjProperty(tstone, CPROP_ANTRAGLISTE, AntragListe);
    sendeNachrichtAnSerial(citizen.serial, text);
endfunction

/**
  * Der Bürgermeister läßt den Char nicht Bürger werden.
  *
  * result : Rückgabe des Gump zur Extraktion der Eingaben.
  * tstone : der Stadtstein (Objekt)
  * BM     : Der Bürgermeister (Objekt)
  */
function denyBuergerAntrag(NummerCitizen, tstone, BM)
    NummerCitizen := CInt(NummerCitizen);
    if (NummerCitizen > 0)
        // Serial des Bürgers aus dem Dictionary
        var whoSerial := schluesselAusListe(tstone, CPROP_ANTRAGLISTE, NummerCitizen);
        if (whoSerial > 0)
            // Bürger anhand der Serial
            var citizen := SystemFindObjectBySerial(whoSerial,SYSFIND_SEARCH_OFFLINE_MOBILES);
            SendSysMessagePergon(BM,citizen.name + " wird eine entsprechende Nachricht gesandt");
            removeBuergerAntrag(citizen, tstone, "Der Antrag auf Buergertum wurde agbelehnt!");
        endif
    endif
endfunction

/**
  * Der Bürgermeister akzeptiert den Antrag. Der Char wird als Bürger anerkannt.
  *
  * result : Rückgabe des Gump zur Extraktion der Eingaben.
  * tstone : der Stadtstein als Objekt
  * BM     : Der Bürgermeister (Objekt)
  */
function acceptBuergerAntrag(NummerCitizen, tstone, BM)
    NummerCitizen := CInt(NummerCitizen);
    // Serial des Bürgers aus dem Dictionary
    if (NummerCitizen > 0)
        var whoSerial := schluesselAusListe(tstone, CPROP_ANTRAGLISTE, NummerCitizen);
        if (whoSerial > 0)
            // Bürger anhand der Serial
            var citizen := SystemFindObjectBySerial(whoSerial,SYSFIND_SEARCH_OFFLINE_MOBILES);
            if (citizen)
                // Der Antrag muss auf jeden Fall entfernt werden.
                removeBuergerAntrag(citizen, tstone, "Antrag angenommen. Ihr seid nun Buerger in " + getObjProperty(tstone, CPROP_STADT));
                // Den Bürger in die Stadtliste aufnehmen
                setObjProperty(citizen, CPROP_STADT, getObjProperty(tstone, CPROP_STADT));
                var BListe := getObjProperty(tstone, CPROP_BUERGERLISTE);
                if (!BListe)
                    BListe := dictionary;
                endif
                var Eintrag := struct;
                // Dies hier ist auch gleich die Definition der Bürgerliste
                Eintrag.+Name := citizen.name;
                Eintrag.+Login := citizen.acctname;
                Eintrag.+Datum := GetDateStr();
                Eintrag.+SteuernBezahlt := "nein";
                Eintrag.+Wahl := false;
                Eintrag.+Abstimmung := false;
                Eintrag.+WahlRat := false; 
                Eintrag.+Grundbesitz := false;
                Eintrag.+Grundstueck := "";
                Eintrag.+GrundbesitzDatum := "";
                BListe.insert(citizen.serial, Eintrag);
                setObjProperty(tstone, CPROP_BUERGERLISTE, BListe);
                // Name der des Chars erweitern um " aus Stadtname" und Nachricht versenden
                citizen.title_suffix := " aus " + GetObjProperty(tstone, CPROP_STADT);
                SendSysMessagePergon(BM,citizen.name + " wird eine entsprechende Nachricht gesandt.");
            endif
        endif
    endif
endfunction

/**
  * Der Char wird als Bürger aus der Stadt entfernt.
  *
  * result : Rückgabe des Gump zur Extraktion der Eingaben.
  * tstone : der Stadtstein (Objekt)
  * BM     : Der Bürgermeister (Objekt)
  */
function removeBuergerBySerial(NummerCitizen, tstone, BM)
    NummerCitizen := CInt(NummerCitizen);
    if (NummerCitizen > 0)
        // Serial des Bürgers aus dem Dictionary
        var whoSerial := schluesselAusListe(tstone, CPROP_BUERGERLISTE, NummerCitizen);
        if (whoSerial > 0)
        // hier muss noch komtrolliert werden, ob der char in der kadidatenliste ist
            var istKandidat := 0;
            if (CInt(getObjProperty(tstone, CPROP_BMWAHLLAEUFT)) == 1) // diese Funktion nur während einer laufenden Wahl
                var TListe := getObjProperty(tstone, CPROP_BMKANDIDATENLISTE);
                var Eintrag := TListe[whoSerial];
                if (!Eintrag)
                    istKandidat := 0;
                else
                    istKandidat := 1;
                endif
            else
                istKandidat := 0;
            endif
            if (istKandidat == 0)
                // Bürger anhand der Serial, auch wenn offline
                var citizen := SystemFindObjectBySerial(whoSerial,SYSFIND_SEARCH_OFFLINE_MOBILES);
                SendSysMessagePergon(BM,"Entfernen aus den Listen der Stadt ist geschehen. Eine Nachricht wird dem Buerger zugesand.");
                removeBuerger(citizen, tstone, BM, "Ihr wurdet auf Wunsch des Buergermeisters aus den Listen der Stadt " + getObjProperty(tstone, CPROP_STADT) + " geloescht.");
            else
                SendSysMessagePergon(BM,"Nicht solange die Wahl noch läuft!");
            endif
        endif
    endif
endfunction

/**
  * Der Char wird als Bürger aus der Stadt entfernt.
  *
  * citizen : der Char als Objekt
  * tstone  : der Stadtstein als Objekt
  */
function removeBuerger(citizen, tstone, BM := 0, text := "Wir Ihr wuenscht. Ihr seid icht länger Buerger dieser Stadt")
    // je nach Aufruf geht die Nachricht an den char oder den BM.
    var NachrichtText := "Dieser Buerger hat doch noch ein Grundstueck!";
    var BMNachricht := 1;
    if (!BM)
        BM := citizen;
        NachrichtText := "Ihr besitzt noch ein Grundstueck. Dies bitte erst mit dem Buergermeister klaeren.";
        BMNachricht := 0;
    endif
// hier muss noch komtrolliert werden, ob der char in der kadidatenliste ist
    var istKandidat := 0;
    if (CInt(getObjProperty(tstone, CPROP_BMWAHLLAEUFT)) == 1) // diese Funktion nur während einer laufenden Wahl
        var TListe := getObjProperty(tstone, CPROP_BMKANDIDATENLISTE);
        var Eintrag := TListe[citizen.serial];
        if (!Eintrag)
            istKandidat := 0;
        else
            istKandidat := 1;
        endif
    else
        istKandidat := 0;
    endif
    if (istKandidat == 0)
        var GListe := getObjProperty(tstone, CPROP_GRUNDBESITZERLISTE);
        // nur löschen, wenn er kein Grundstück hat.
        if (GListe[citizen.serial])
            SendSysMessagePergon(BM,NachrichtText);
            return;
        endif
        // char aus der Liste der Bürger entfernen
        var BListe := GetObjProperty(tstone, CPROP_BUERGERLISTE);
        BListe.erase(citizen.serial);
        setObjProperty(tstone, CPROP_BUERGERLISTE, BListe);
        // Namen ändern und CProp löschen
        citizen.title_suffix := "";
        EraseObjProperty(citizen, CPROP_STADT);
        sendeNachrichtAnSerial(citizen.serial, text);
        if (BMNachricht)
            SendSysMessagePergon(BM,citizen.name + " wird eine entsprechende Nachricht gesandt");
        endif
    endif
endfunction

function buergerListe(item)
    return listeErzeugen(item, CPROP_BUERGERLISTE, "Name");
endfunction

function buergerListeMitDatum(item)
    var inputDict := dictionary;
    inputDict := getObjProperty(item, CPROP_BUERGERLISTE);
    if (!inputDict)
        return "CPROP nicht vorhanden";
    endif
    var Ausgabe := "";
    var i := 1;
    foreach name in (inputDict)
        // in der ersten Eigenschaft des Eintrags im Dictionary muss der Name sein.
        Ausgabe += i +" : " + name["Name"] + ", (" + name["Login"] + "), " + name["Datum"] + "\n";
        i+=1;
    endforeach
    return Ausgabe;
endfunction

function buergerListeMitGrundbesitz(item)
    var inputDict := dictionary;
    inputDict := getObjProperty(item, CPROP_BUERGERLISTE);
    if (!inputDict)
        return "CPROP nicht vorhanden";
    endif
    var Ausgabe := "";
    var Grundbesitz;
    foreach name in (inputDict)
        if (name.Grundbesitz)
            Grundbesitz := "ja";
        else
            Grundbesitz := "nein";
        endif
        Ausgabe += _name_iter +" : " + name.Name + ", (" + name.Login + "), " + Grundbesitz + ", " + name. Grundstueck + ", " + name.GrundbesitzDatum + "\n";
    endforeach
    return Ausgabe;
endfunction

function titelListe(item)
    var TListe := getObjProperty(item, CPROP_TITELLISTE);
    if (!TListe)
        return "--> kein Titel";
    endif
    var Ausgabe := "";
    var i;
    for ( i:=1; i<=Tliste.Size(); i+=1 )
        Ausgabe += "\n" + i + " "+ Tliste[i];
    endfor
    return Ausgabe;
endfunction


function AntragsListe(item)
    return listeErzeugen(item, CPROP_ANTRAGLISTE, 1);
endfunction

Function titelVergeben(NummerTitel, NummerCitizen, tstone, BM)
    NummerCitizen := CInt(NummerCitizen);
    NummerTitel := CInt(NummerTitel);
    if (NummerCitizen < 1 || NummerTitel < 1)
        SendSysMessagePergon(BM, "Bitte nur gueltige Zahlen eintragen.");
    else
        var SchluesselCitizen := schluesselAusListe(tstone, CPROP_BUERGERLISTE, NummerCitizen);
        print(SchluesselCitizen);
        if (!SchluesselCitizen)
            SendSysMessagePergon(BM, "Buerger mit dieser Nummer nicht gefunden.");
        else // citizen gefunden
            var TArray := getObjProperty(tstone, CPROP_TITELLISTE);
            if (NummerTitel > TArray.size())
                SendSysMessagePergon(BM, "Titel mit dieser Nummer nicht gefunden.");
            else // Titel gefunden
                var citizen := SystemFindObjectBySerial(SchluesselCitizen,SYSFIND_SEARCH_OFFLINE_MOBILES);
                var BListe := getObjProperty(tstone, CPROP_BUERGERLISTE);
                print(BListe);
                var Temp := BListe[SchluesselCitizen];
                var BuergerName := Temp.Name;
                var Titel := " (" + TArray[NummerTitel] + ")";
                if (!BuergerName[Titel])
                    // Name um dem Titel erweitern
                    BuergerName := BuergerName + Titel;
                    // Den BM fragen ob das so in Ordnung ist
                    var Antwort := YesNo(BM, "Ist das so korrekt: '" + BuergerName + "'?");
                    if (Antwort) // == 1
                        Temp.Name := BuergerName;
                        BListe[SchluesselCitizen] := Temp;
                        setObjProperty(tstone, CPROP_BUERGERLISTE, BListe);
                        citizen.title_suffix := " " + TArray[NummerTitel] + citizen.title_suffix;
                        SendSysMessagePergon(BM, "Buerger " + BuergerName + " wird eine entsprechende Nachricht gesandt.");
                        sendeNachrichtAnSerial(SchluesselCitizen, "Ihr habt nun das Amt " + Titel + " in der Stadt " + getObjProperty(tstone, CPROP_STADT));
                    endif
                else
                    SendSysMessagePergon(BM, "Der Buerger " + BuergerName + " ist bereits "+ Titel + ".");
                endif
            endif
        endif
    endif
endfunction

function alleTitelWegnehmen(NummerCitizen, tstone, BM)
    NummerCitizen := CInt(NummerCitizen);
    // NummerTitel := CInt(NummerTitel);
    if (NummerCitizen < 1)
        SendSysMessagePergon(BM, "Bitte nur gueltige Zahlen eintragen.");
    else
        var SchluesselCitizen := schluesselAusListe(tstone, CPROP_BUERGERLISTE, NummerCitizen);
        if (!SchluesselCitizen)
            SendSysMessagePergon(BM, "Buerger mit dieser Nummer nicht gefunden.");
        else // citizen gefunden
            var citizen := SystemFindObjectBySerial(SchluesselCitizen,SYSFIND_SEARCH_OFFLINE_MOBILES);
            // title_suffix löschen
            var BListe := getObjProperty(tstone, CPROP_BUERGERLISTE);
            var Temp := BListe[SchluesselCitizen];
            Temp.Name := citizen.name;
            var Antwort := YesNo(BM, "Ist das so korrekt: '" + Temp.Name + "'?");
            if (Antwort) // == 1
                Bliste[SchluesselCitizen] := Temp;
                setObjProperty(tstone, CPROP_BUERGERLISTE, BListe);
                citizen.title_suffix := "";
                SendSysMessagePergon(BM, "Buerger " + Citizen.name + " wird eine entsprechende Nachricht gesandt.");
                sendeNachrichtAnSerial(SchluesselCitizen, "Euer Amt in der Stadt " + getObjProperty(tstone, CPROP_STADT) + " ist euch entzogen worden.");
            endif
        endif
    endif
endfunction

function titelHinzufuegen(Titel, tstone, who)
    SendSysMessagePergon(who, "Funktion: titelHinzufuegen.");
    if (!Titel.errortext)
        var TListe := getObjProperty(tstone, CPROP_TITELLISTE);
        if (!TListe)
            TListe := array;
        endif
        var i;
        var gefunden := false;
        for ( i:=1; i<=TListe.Size(); i:=i+1 )
            if ( TListe[i] == Titel )
                gefunden := true;
                break;
            endif
        endfor
        if (!gefunden and Titel != "Hier Titel eintragen")
            TListe.append(Titel);
            print(TListe);
            setObjProperty(tstone, CPROP_TITELLISTE, TListe);
            SendSysMessagePergon(who, "Neuer Titel '" + Titel + "' hinzugefuegt.");
        else
            SendSysMessagePergon(who, "Bitte einen gueltigen Titel angeben.");
        endif
    endif
endfunction

function titelEntfernen(NummerTitel, tstone, who)
    NummerTitel := CInt(NummerTitel);
    var TListe := getObjProperty(tstone, CPROP_TITELLISTE);
    if (!TListe)
        return;
    endif
    if (NummerTitel > 0 && NummerTitel <= TListe.size())
        var zuLoeschenderTitel := TListe[NummerTitel];
        TListe.erase(NummerTitel);
        setObjProperty(tstone, CPROP_TITELLISTE, TListe);
        SendSysMessagePergon(who, "Titel '" + zuLoeschenderTitel + "' wird entfernt.");
    endif
endfunction
