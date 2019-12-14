///////////////////////////////////////////////////////////////////////////
// Auf- und Abbau von Osterdeko, Geschenkverteilungskontrolle

include "include/set";

// zaehlt die abgeholten Geschenke
Const PROP_PRESENT_COUNT := "xmas";
// markiert, ob Geschenk ein Ei oder ein Weihnachtsdings ist
Const PROP_XMAS_EVENT := "xmas";

// Oster- oder Weihnachtsaufbauten?
Const EVENT_XMAS      := 1;
Const EVENT_EASTER    := 2;
Const EVENT_MAY       := 3;

// hier sind die auf-/abzubauenden Sets definiert
Function GetSetsToCreate(event := EVENT_XMAS) // {{{
    Case (event)
    EVENT_EASTER:
        return array{
            // Minoc
            2523,  554, 0, "Osternest",
            // Vesper
            2885,  698, 0, "Osternest",
            // Cove
            2262, 1224, 0, "Osternest",
            // Yew
            542,  996, 0, "Osternest",
            // Jhe'lom
            1381, 3803, 0, "Osternest",
            // Nirdana (hinter Heiler)
            3340,  276, 4, "OsternestNirdana",
            // Nujelm
            // 3713, 1236, 0, "Osternest",
            // Aussenposten
            2099,  782, 0, "Osternest"
        };

    EVENT_XMAS:
        return array{
            // Minoc
            2522,  556, 0, "Weihnacht",
            // Vesper
            2884,  700, 0, "Weihnacht",
            // Cove
            2262, 1224, 0, "Weihnacht",
            // Yew
            542,  998, 0, "Weihnacht",
            // Jhe'lom
            1380, 3805, 0, "Weihnacht",
            // Nirdana (vor dem Tor)
            3341,  355, 4, "Weihnacht",
            // Nujelm
            // 3713, 1237, 0, "Weihnacht",
            // Aussenposten
            2100,  782, 0, "Weihnacht"
        };

    EVENT_MAY:
        return array{
            // Minoc
            2522,  555, 0, "Maibaum",
            // Vesper
            2885,  699, 0, "Maibaum",
            // Cove
            2262, 1224, 0, "Maibaum",
            // Yew
            541,  997, 0, "Maibaum",
            // Jhe'lom
            1381, 3803, 0, "Maibaum",
            // Nirdana (hinter Heiler)
            3328,  221, 0, "MaibaumNirdana",
            // Nujelm?
            // Aussenposten
            2100,  778, 0, "Maibaum"
        };

    EndCase
EndFunction // }}}

// Sets auf bzw. abbauen
Function XmasSetCreate(type := 0) // {{{
    var SetList := GetGlobalProperty("XmasBuilt");
    If (type == 0 and SetList)
        // aktuelle Sets abreissen
        ForEach set in (SetList)
            DestroySet(set);
            SleepMS(5);
        EndForEach
        EraseGlobalProperty("XmasBuilt");
        // Geschenkzaehler braucht jetzt auch niemand mehr
        XmasPresentReset();
        syslog("HINWEIS: Xmas-Elemente wurden zerstoert.");
        return;
    EndIf

    // sonst je nach "type" passendes Set aufbauen
    If (SetList)
        // oh, schon erledigt
        return;
    EndIf

    // Array von Set-Start-Item-Serials
    SetList := array{};

    var SetsToCreate := GetSetsToCreate(type);
    If ((len(SetsToCreate) < 4) or (Modulus(len(SetsToCreate), 4) != 0))
        syslog(
            "FEHLER: var 'SetsToCreate' (" + SetsToCreate +
            ") ist vorhanden, enthaelt aber eine ungenuegende oder "+
            "falsche Anzahl Elemente!"
        );
        return;
    EndIf

    var SetKoords := dictionary{};
    var i;
    For (i := 1; i <= len(SetsToCreate); i += 4)
        SetKoords.x := SetsToCreate[i+0];
        SetKoords.y := SetsToCreate[i+1];
        SetKoords.z := SetsToCreate[i+2];
        SetKoords.realm := _DEFAULT_REALM;
        var SetSerial := MakeSet(SetKoords, SetsToCreate[i+3]);
        If (SetSerial)
            // Aufbau war erfolgreich.. also Serial zwischenspeichern
            SetList.append(SetSerial);
        Else
            syslog(
            ") nicht erzeugen!"
            );
        EndIf
        SleepMS(5);
    EndFor

    If (len(SetList) >= 1)
        SetGlobalProperty("XmasBuilt", SetList);
    Else
        syslog("WARNUNG: Es wurden keine Xmas-Sets erzeugt!");
    EndIf
EndFunction // }}}

// Geschenkezaehler bei allen Spielern loeschen
Function XmasPresentReset() // {{{
    var done := 0;
    ForEach accountname in ListAccounts()
        SleepMs(2);
        var account := FindAccount(accountname);
        If (!account)
            continue;
        EndIf

        For slot := 1 To 6
            SleepMs(2);
            var char := account.getcharacter(slot);
            If (!char)
                continue;
            EndIf

            If (char.getprop(PROP_PRESENT_COUNT) <> error)
                char.eraseprop(PROP_PRESENT_COUNT);
                done += 1;
            EndIf
        EndFor
    EndForEach

    If (done > 0)
        syslog(
            "HINWEIS: Die Geschenkezaehler wurden zurueckgesetzt."
        );
    EndIf
EndFunction // }}}

// Ostersonntag ermitteln, siehe:
// https://de.wikipedia.org/wiki/Gau%C3%9Fsche_Osterformel#Eine_erg.C3.A4nzte_Osterformel
// http://www.ptb.de/cms/ptb/fachabteilungen/abt4/fb-44/ag-441/darstellung-der-gesetzlichen-zeit/wann-ist-ostern.html
// ergibt Ostersonntag bezogen auf Maerz, d.h.:
//  1 - 1. Maerz
// 32 - 1. April (1 Tag nach 31. Maerz)
Function EasterDate(year := 0)
    If (!year)
        year := GetCurrentYear();
    EndIf

    var century   := CInt(year/100);
    // Mondschaltung
    var moon_sw   := 15 + CInt((3*century+3)/4) - CInt((8*century+13)/25);
    // Sonnenschaltung
    var sun_sw    :=  2 - CInt((3*century+3)/4);
    // Mondparameter
    var moon_para := year % 19;
    // erster Vollmond
    var firstfull := (19*moon_para+moon_sw) % 30;
    // Kalenderkorrektur
    var cal_corr  := CInt((firstfull + CInt(moon_para/11)) / 29);
    // "Ostergrenze"
    var border    := 21 + firstfull - cal_corr;
    // erster Sonntag im Maerz
    var mar_sun1  := 7 - (year+CInt(year/4)+sun_sw) % 7;
    // Osterentfernung
    var east_dist := 7 - (border-mar_sun1) % 7;

    // Ostersonntag
    return border + east_dist;
EndFunction
