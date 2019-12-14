/**
  * #################
  * #               #
  * # Steuern       #
  * # Taya Jan 2006 #
  * #               #
  * #################
  */

/**
  * Ein Bürger wird als Besitzer eines Grundstückes eingetragen.
  */
function grundbesitzerHinzufuegen(NummerBuerger, NummerGrund, tstone, BM)
    NummerBuerger := CInt(NummerBuerger);
    if (NummerBuerger > 0)
        if (!NummerGrund)
            SendSysMessagePergon(BM, "Bitte eine gueltige Grundstuecksnummer angeben.");
        else
            var BListe := getObjProperty(tstone, CPROP_BUERGERLISTE);
            if (!BListe)
                SendSysMessagePergon(BM, "Es gibt noch keinen Buerger.");
                return;
            endif
            var TempCitizen := BListe[NummerBuerger];
            if (!TempCitizen)
                SendSysMessagePergon(BM, "Der Buerger " + TempCitizen.serial + " wurde nicht in der Liste gefunden.");
                return;
            endif
            if (TempCitizen.Grundbesitz) // Er hat schon ein Grundstück
                SendSysMessagePergon(BM, "Bisheriges Grundstueck '" + TempCitizen.Grundstueck + "' wird entfernt.");
            endif
            TempCitizen.Grundbesitz := true;
            TempCitizen.Grundstueck := Nummergrund;
            TempCitizen.GrundbesitzDatum := GetDateStr();
            BListe[NummerBuerger] := TempCitizen;
            print(BListe);
            setObjProperty(tstone, CPROP_BUERGERLISTE, BListe);
            SendSysMessagePergon(BM, "Buerger " + TempCitizen.name + " hat nun das Grundstueck" + NummerGrund + ".");
        endif
    endif
endfunction

/**
  * Ein Grundbesitzer wird aus der Liste entfernt
  */
function grundbesitzerEntfernen(NummerBesitzer, tstone, BM)
    NummerBesitzer := CInt(NummerBesitzer);
    if (NummerBesitzer > 0)
        var BListe := getObjProperty(tstone, CPROP_BUERGERLISTE);
        var TempCitizen := BListe[NummerBesitzer];
        if (!TempCitizen)
            SendSysMessagePergon(BM, "Der Buerger " + TempCitizen.serial + " wurde nicht in der Liste gefunden.");
            return;
        endif
        TempCitizen.Grundbesitz := false;
        TempCitizen.Grundstueck := "";
        TempCitizen.GrundbesitzDatum := "";
        BListe[NummerBesitzer] := TempCitizen;
        setObjProperty(tstone, CPROP_BUERGERLISTE, BListe);
        SendSysMessagePergon(BM, "Buerger " + TempCitizen.Name + " hat nun kein Grundstueck mehr.");
    endif
endfunction

/**
  * Anhand der Eingabe wird die neue Steuerhöhe festgelegt.
  */
function steuerHoeheFestlegen(SteuerHoehe, tstone, BM)
    SteuerHoehe := CInt(SteuerHoehe);
    if (SteuerHoehe < 1 || SteuerHoehe > 1000000)
        SendSysMessagePergon(BM, "Bitte einen gueltigen Betrag zwischen 1 und 1 Millionen angeben.");
    else // citizen gefunden
        setObjProperty(tstone, CPROP_STEUERBETRAG, SteuerHoehe);
        SendSysMessagePergon(BM, "Die Steuerhoehe ist nun " + SteuerHoehe + ".");
    endif
endfunction

/**
  * Der Bürger kann am Stadtstein die Steuern bezahlen.
  * Dafür muss er natürlich genügend Gold dabei haben.
  */
function steuernBezahlen(who, tstone)
    if (who.cmdlevel >= CMDLEVEL_SEER)
        SendSysMessagePergon(who, "uiuiui Sowas. Ein Gott braucht doch keine Steuern zahlen!");
        return;
    endif
    var SteuerBetrag := getObjProperty(tstone, CPROP_STEUERBETRAG);
    if (!SteuerBetrag)
        SendSysMessagePergon(who, "Der Buergermeister muss erst einen Betrag festlegen.");
    else
        var BListe := getObjProperty(tstone, CPROP_BUERGERLISTE);
        // var keu := CInt(who.serial);
        var Eintrag := BListe[CInt(who.serial)];
        if (Eintrag.SteuernBezahlt == "nein")
            if (who.spendgold(SteuerBetrag))
                Eintrag.SteuernBezahlt := "bezahlt";
                BListe[CInt(who.serial)] := Eintrag;
                setObjProperty(tstone, CPROP_BUERGERLISTE, BListe);
                var SteuerGelder := getObjProperty(tstone, CPROP_STEUERGELDER);
                if (!SteuerGelder)
                    SteuerGelder := 0;
                endif
                SteuerGelder := SteuerGelder + SteuerBetrag;
                setObjProperty(tstone, CPROP_STEUERGELDER, SteuerGelder);
                SendSysMessagePergon(who, "Wir haben den Betrag dankend entgegen genommen.");
            else
                SendSysMessagePergon(who, "Ihr habt leider nicht genuegend Gold dabei.");
            endif
        else
            SendSysMessagePergon(who, "Ihr habt bereits die Steuern fuer dieses Quartal bezahlt.");
        endif
    endif
endfunction

/**
  * In der Bürgerliste werden alle Kennzeichen für
  * gezahlte steuern zurückgesetzt.
  */
function steuerQuartalBeginnen(tstone, BM)
    var BListe := getObjProperty(tstone, CPROP_BUERGERLISTE);
    foreach Eintrag in BListe
        Eintrag.SteuernBezahlt := "nein";
    endforeach
    setObjProperty(tstone, CPROP_BUERGERLISTE, BListe);
    SendSysMessagePergon(BM, "Die Liste der nicht gezahlten Steuern wurde geloescht.");
endfunction

function steuerGelderAbheben(tstone, BM)
    var SteuerGelder := CInt(getObjProperty(tstone, CPROP_STEUERGELDER));
    if (SteuerGelder < 1)
        SendSysMessagePergon(BM, "Zurzeit ist kein Geld in der Steuerkasse.");
    else
        var merker := SteuerGelder;
        var Beutel;
        var i;
        var AnzBeutel := CInt(SteuerGelder / 8940000) + 1;
        var BeutelNr := 1;
        var BeutelCount := "";
        while (SteuerGelder > 0)
            // Eine Beutel im Backpack erzeugen
            Beutel := CreateItemInBackpack(BM, 0x0E76, 1);
            if (AnzBeutel > 1)
                BeutelCount := " / " + BeutelNr + " von " + AnzBeutel;
            endif
            Beutel.name := "Steuergelder der Stadt " + getObjProperty(tstone, CPROP_STADT) + " vom " + GetDateStr() + BeutelCount;
            // es passen genau 149 Stapel Gold in einen Beutel
            for ( i := 1 ; i < 150 ; i += 1)
                // im Beutel dann Goldstapel á 60k erzeugen
                if (SteuerGelder > 60000)
                    CreateItemInContainer(Beutel, 0x0eed, 60000 );
                else
                    CreateItemInContainer(Beutel, 0x0eed, SteuerGelder );
                endif
                SteuerGelder -= 60000;
                if (SteuerGelder < 1)
                    break;
                endif
            endfor
            BeutelNr += 1;
        endwhile
        setObjProperty(tstone, CPROP_STEUERGELDER, 0);
        SendSysMessagePergon(BM, "Ihr solltet nun " + BeutelCount + " Beutel mit " + merker + " Goldstuecken haben.");
    endif
endfunction

function grundbesitzerListe(item)
    var inputDict := dictionary;
    inputDict := getObjProperty(item, CPROP_BUERGERLISTE);
    if (!inputDict)
        return "CPROP nicht vorhanden";
    endif
    var Ausgabe := "";
    foreach name in (inputDict)
        if (name.Grundbesitz)
            Ausgabe += _name_iter +" : " + name.Name + ", (" + name.Login + "), " + CStr(name.Grundbesitz) + ", " + name. Grundstueck + ", " + name.GrundbesitzDatum + "\n";
        endif
    endforeach
    return Ausgabe;
//    var inputDict := dictionary;
//    inputDict := getObjProperty(item, CPROP_GRUNDBESITZERLISTE);
//    if (!inputDict)
//        return "CPROP nicht vorhanden";
//    endif
//    Print(inputDict);
//    var Ausgabe := "";
//    var i := 1;
//    foreach name in (inputDict)
//        // in der ersten Eigenschaft des Eintrags im Dictionary muss der Name sein.
//        Ausgabe += i + " : " + name[1] + ", " + name[2] + ", " + name[3] + "\n";
//        i+=1;
//    endforeach
//    return Ausgabe;
endfunction

function offeneSteuernListe(item)
    var inputDict := dictionary;
    inputDict := getObjProperty(item, CPROP_BUERGERLISTE);
    if (!inputDict)
        return "CPROP nicht vorhanden";
    endif
    var Ausgabe := "";
    foreach name in (inputDict)
        // in der ersten Eigenschaft des Eintrags im Dictionary muss der Name sein.
        if (name["SteuernBezahlt"] == "nein")
            Ausgabe += name["Name"] + ", " + name["SteuernBezahlt"]+ "\n";
        EndIf
    endforeach
    return Ausgabe;
endfunction
