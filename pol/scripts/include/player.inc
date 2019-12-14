/////////////////////////////////////////////////////////////////////////////
//// player - Verwaltung von Spielen (Accounts Personen zuordnen)
//
//Const PLAYER_DB_FILE := "::players";
//
//Const PDB_PROP_ACCOUNTS := "a";
//Const PDB_PROP_CHARS    := "c";
//
//// erzeugt die Playerdatenbank von Grund auf neu
//Function PDBRecreate() // {{{
//    var db := CreateDataFile(PLAYER_DB_FILE, DF_KEYTYPE_STRING);
//    If (!db)
//        syslog(
//            "FEHLER: Erzeugung von Spielerdatenbank gescheitert: "+
//            db.errortext
//        );
//        return db;
//    EndIf
//
//    // muss nicht leer sein, ggf. ausleeren
//    ForEach key in (db.Keys())
//        SleepMs(2);
//        db.DeleteElement(key);
//    EndForEach
//
//    // neu aufbauen aus allen Accounts
//    ForEach acctname in (ListAccounts())
//        SleepMs(2);
//        var acct := FindAccount(acctname);
//        PDBAddAcctOrChar(acct, db);
//    EndForEach
//
//    // UnloadDataFile(PLAYER_DB_FILE);
//    return 1;
//EndFunction // }}}
//
//// Account (mit allen Charactern) oder Character (einzeln) einfuegen
//Function PDBAddAcctOrChar(acct_or_char, db := 0) // {{{
//    If (acct_or_char.isa(POLCLASS_MOBILE))
//        // da kam ein Char -> eigene Funktion nutzen
//        return PDBAddChar(acct_or_char, db);
//    EndIf
//
//    // Account grob pruefen
//    var acct := acct_or_char;
//    If (TypeOfInt(acct_or_char) == OT_STRING)
//        // bei Accountnamen versuchen wir den Account zu finden
//        acct := FindAccount(acct_or_char);
//        If (!acct)
//            return acct;
//        EndIf
//    EndIf
//
//    // ggf. Datenbank nachladen
//    If (!db)
//        db := PDBOpen();
//        If (!db)
//            return db;
//        EndIf
//    EndIf
//
//    // ggf. bestehenden Eintrag suchen
//    var key   := PDBGetKey(acct);
//    var entry := db.FindElement(key);
//    If (entry)
//        // vorhanden, ggf. aktualisieren
//        var accounts := entry.getprop(PDB_PROP_ACCOUNTS);
//        If (!accounts)
//            // leer, neues Array mit Account anlegen
//            accounts := array{acct.name};
//        Else
//            // da ist was
//            If (!(acct.name in (accounts)))
//                // nur einfuegen wenn nicht vorhanden
//                accounts.append(acct.name);
//            EndIf
//        EndIf
//        // Eintrag sichern
//        entry.setprop(PDB_PROP_ACCOUNTS, accounts);
//    Else
//        // neu anlegen
//        entry := db.CreateElement(key);
//        entry.setprop(PDB_PROP_ACCOUNTS, array{acct.name});
//    EndIf
//
//    // alle Character durchgehen
//    For slot := 1 To 6
//        SleepMs(2);
//        var char := acct.getcharacter(slot);
//        If (char)
//            PDBAddChar(char, db);
//        EndIf
//    EndFor
//
//    return 1;
//EndFunction // }}}
//
//// Character (einzeln) einfuegen (Account wird bei Bedarf eingefuegt)
//Function PDBAddChar(who, db := 0) // {{{
//    // ggf. Datenbank nachladen
//    If (!db)
//        db := PDBOpen();
//        If (!db)
//            return db;
//        EndIf
//    EndIf
//
//    // Eintrag zum Account finden
//    var key := PDBGetKey(who.acct);
//    var entry := db.FindElement(key);
//    If (!entry)
//        // fehlt noch, also erst einmal Account anlegen
//        PDBAddAcctOrChar(who.acct);
//        entry := db.FindElement(key);
//        If (!entry)
//            return entry;
//        EndIf
//    EndIf
//
//    // gefunden, ggf. aktualisieren
//    var chars := entry.getprop(PDB_PROP_CHARS);
//    If (!chars)
//        // leer, neues Array mit Char anlegen
//        chars := array{who.serial};
//    Else
//        // da ist was
//        If (!(who.serial in (chars)))
//            // nur einfuegen wenn nicht vorhanden
//            chars.append(who.serial);
//        EndIf
//    EndIf
//    // Eintrag sichern
//    entry.setprop(PDB_PROP_CHARS, chars);
//
//    return 1;
//EndFunction // }}}
//
//// TODO:
//// loescht Account (und vorher alle verbliebenen Chars darin)
//Function PDBDeleteAccount(who, db := 0);
//    syslog("FEHLER: not implemented");
//    return;
//    who := 1;
//    db  := 1;
//EndFunction
//
//// TODO:
//// loescht einzelnen Char
//Function PDBDeleteChar(who, db := 0);
//    syslog("FEHLER: not implemented");
//    return;
//    who := 1;
//    db  := 1;
//EndFunction
//
//// TODO:
//// ueberprueft die Datenbank auf Korrektheit
//Function PDBValidate()
//    // alle Keys durchgehen
//    // - existieren die Chars/Accounts noch?
//    // - passt Mailadresse in Accounts noch?
//    // -> ggf. Korrektur vornehmen oder Neubau anstossen
//EndFunction
//
//// alle Character/Accounts von Account/Char finden
//Function PDBGetDataOf(acct_or_char, db := 0) // {{{
//    // ggf. Datenbank nachladen
//    If (!db)
//        db := PDBOpen();
//        If (!db)
//            return db;
//        EndIf
//    EndIf
//
//    // Parameter zu Account aufloesen
//    var acct := acct_or_char;
//    If (acct_or_char.isa(POLCLASS_MOBILE))
//        acct := acct_or_char.acct;
//    ElseIf (TypeOfInt(acct_or_char) == OT_STRING)
//        acct := FindAccount(acct_or_char);
//        If (!acct)
//            return acct;
//        EndIf
//    EndIf
//
//    var key    := PDBGetKey(acct);
//    var result := struct;
//    var entry  := db.FindElement(key);
//    var dberr  := error{errortext := "Database inconsistent"};
//    If (!entry)
//        // eigentlich sollte jeder Account eingetragen sein
//        return dberr;
//    EndIf
//    var accts := entry.getprop(PDB_PROP_ACCOUNTS);
//    If (!accts)
//        // prinzipiell muesste der abgefragte Account existieren
//        return dberr;
//    EndIf
//    result.+accounts := accts;
//
//    var chars := entry.getprop(PDB_PROP_CHARS);
//    If (!chars)
//        // Charliste kann leer sein (neuer Account)
//        chars := array{};
//    EndIf
//    result.+characters := chars;
//
//    return result;
//EndFunction // }}}
//
//// gehoeren die Accounts/Spieler zusammen?
//Function PDBIsSamePlayer(acct_or_char1, acct_or_char2) // {{{
//    // zweiten Parameter zu Account aufloesen
//    var second := acct_or_char2;
//    If (acct_or_char2.isa(POLCLASS_MOBILE))
//        second := acct_or_char2.acct;
//    ElseIf (TypeOfInt(acct_or_char2) == OT_STRING)
//        second := FindAccount(acct_or_char2);
//        If (!second)
//            return second;
//        EndIf
//    EndIf
//
//    // Daten vom ersten Parameter holen
//    var first := PDBGetDataOf(acct_or_char1);
//
//    // ist Accountname vom zweiten Parameter in Accountliste vom ersten?
//    If (second.name in (first.accounts))
//        // jawohl, sind der gleiche Spieler
//        return 1;
//    EndIf
//
//    // vermutlich verschiedene Spieler
//    return 0;
//EndFunction // }}}
//
//// interne Funktionen
//// Spielerdatenbank laden
//Function PDBOpen() // {{{
//    var db := OpenDataFile(PLAYER_DB_FILE);
//    If (!db)
//        syslog("FEHLER: Oeffnen der Spielerdatenbank: "+db.errortext);
//    EndIf
//
//    return db;
//EndFunction // }}}
//
//// unter welchem Schluessel kommt der Eintrag in die DB?
//Function PDBGetKey(acct) // {{{
//    // Mailadresse bevorzugen, da die in mehreren Accounts vorkommen kann
//    var mail := acct.getprop(PLAYEREMAIL);
//    If (mail)
//        return Lower(mail);
//    EndIf
//
//    // keine Adresse
//    return acct.name;
//EndFunction // }}}
