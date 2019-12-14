///////////////////////////////////////////////////////////////////////////
// varutil -- haeufiger gebrauchte Variablenverarbeitungsfunktionen

// Arrayeintraege zu Zahlen machen
Function CIntArray(byref arr) // {{{
  var newarr := array{};

  ForEach elem in (arr)
    SleepMs(2);
    newarr.append(CInt(elem));
  EndForEach

  return newarr;
EndFunction // }}}

// wandelt Strings der Form "0x1234, 0x3434-0x3466, 123, 10-15" in Array
// aus Zahlen um, wobei bei <von>-<bis> alle Zwischenwerte erzeugt werden
Function ResolveRange(unresolved) // {{{
    If (TypeOfInt(unresolved) != OT_STRING)
        return unresolved;
    EndIf

    unresolved:=StrReplace(unresolved," ","");

    var resolved := array{};
    ForEach word in (SplitWords(unresolved, ","))
        If (word["-"])
            var delims := SplitWords(word, "-");
            var elem;
            For (elem := CInt(delims[1]); elem <= CInt(delims[2]); elem += 1)
                resolved.append(elem);
            EndFor
        Else
            resolved.append(CInt(word));
        EndIf
        SleepMS(2);
    EndForEach

    return resolved;
EndFunction // }}}

// Funktion zum klassenbasierten Zerhacken von Strings
// Beispiel: SplitClasses(
//      "Du dumme Sau!^^", array{"ABC...XYZabc...xyz", " !?."}
//      ) wird zu:
//      array{"Du", " ", "dumme", " ", "Sau", "!", "^^"}
//
// Oder doch besser ein Dictionary mit Verweis, welche Klasse aus dem
// Array gepasst hat?
// Function SplitClasses(string, class_array)
//     return;
// EndFunction

// Array zu String umwandeln mit Trenner <delim>
Function Join(delim, arr) // {{{
    If (arr.size() <= 0)
        return "";
    EndIf

    var result := arr[1];

    var i;
    For (i := 2; i <= arr.size(); i += 1)
        result += delim + arr[i];
    EndFor

    return result;
EndFunction // }}}

// entfernt Duplikate aus Array (Hinweis: sortiert Array dabei)
Function Unique(non_unique_arr) // {{{
    var unique_arr := array{};
    If (!non_unique_arr or non_unique_arr.size() == 0)
        return unique_arr;
    EndIf

    non_unique_arr.sort();

    var last := non_unique_arr[1];
    unique_arr.append(last);
    non_unique_arr.erase(1);
    ForEach entry in (non_unique_arr)
        SleepMS(2);
        If (entry != last)
            unique_arr.append(entry);
        EndIf
        last := entry;
    EndForEach

    return unique_arr;
EndFunction // }}}

// tauscht Werte zweier Variablen
Function SwapValues(byref val1, byref val2) // {{{
    // TODO: bei Integer geht es ohne Hilfsvariable
    var help := val1;
    val1 := val2;
    val2 := help;
EndFunction // }}}

// macht aus Optionsstring der Form "foo=bar", "baz", "nobar" ein
// Dict { foo -> bar, baz -> 1, nobar -> 1, bar -> 0 }
Function GetOptions(optstr) // {{{
    var params := SplitWords(optstr);

    var result := dictionary;
    ForEach elem in (params)
        SleepMs(2);
        If (elem["="])
            // komplexe Optionen zerhacken
            var keyvalue := SplitWords(elem, "=");
            result.insert(keyvalue[1], keyvalue[2]);
            continue;
        EndIf

        // Rest normal einfuegen
        result.insert(elem, 1);

        // bei negierten Optionen zusaetzlich Variante ohne "no" einbauen
        If (Find(elem, "no", 0) == 1)
            result.insert(SubStr(elem, 3, Len(elem) - 2), 0);
        EndIf
    EndForEach

    return result;
EndFunction // }}}

// Double gezielt in Dezimalschreibweise wandeln (String)
Function PrettyPrintDouble(dbl) // {{{
    dbl := CStr(dbl);

    If (!(dbl["+"]))
        return dbl;
    EndIf

    // abgetrennte Kommastellen separieren
    var parts := SplitWords(dbl, "e+");
    var zeros := CInt(parts[2]);
    dbl       := parts[1];

    If (!(dbl["."]))
        // Ganzzahl, einfach hinreichend viele Nullen anhaengen
        For i := 1 To zeros
            dbl += "0";
        EndFor
        return dbl;
    EndIf

    parts := SplitWords(dbl, ".");
    // Punkt entfernen
    var missingzeros := zeros - Len(parts[2]);
    If (missingzeros < 0)
        // ich zweifele, dass es je auftritt
        syslog(
            "WARNUNG: "+dbl+" mit "+zeros+" Exponentialstellen hat "+
            missingzeros+" fehlende Nullen"
        );
    EndIf
    dbl["."] := "";
    For i := 1 To missingzeros
        // nur fehlende Nullen ergaenzen
        dbl += "0";
    EndFor

    return dbl;
EndFunction // }}}

// schoenere Zahlen durch passende Praefixe (Einheit selbst anhaengen)
Function PrettyPrintPrefix(dbl, unit := "", base := 1000, fracs := 1) // {{{
    var binar := "";
    If (CInt(base) == 1024)
        binar := "i";
    EndIf
    var neg := "";
    If (dbl < 0)
        neg := "-";
        dbl := -1 * dbl;
    EndIf

    var org := dbl;
    base    := CDbl(base);

    // na gut, nicht schoen, aber zumindest selten
    ForEach prefix in (array{"", "k", "M", "G", "T", "P", "E", "Z", "Y"})
        If (dbl >= base)
            dbl := dbl / base;
        Else
            // grob auf passende Nachkommastellen runden
            dbl := CInt(dbl * Pow(10, fracs)) / Pow(10, fracs);

            If (prefix == "")
                // kein Prefix, direkt ausgeben
                If (unit == "")
                    // keine Einheit, kein Leerzeichen
                    return neg+CStr(dbl);
                EndIf
                // Einheit mit Leerzeichen abtrennen
                return neg+CStr(dbl)+" "+unit;
            EndIf

            // Prefix und ggf. Binaerkennung anhaengen
            return neg+CStr(dbl)+" "+prefix+binar+unit;
        EndIf
    EndForEach

    // viel zu gross (eher unwahrscheinlich)
    return org;
EndFunction // }}}

// billiger, selbstgebauter ternaerer "Operator"
Function Tern(bool, iftrue, iffalse) // {{{
    If (bool)
        return iftrue;
    EndIf

    return iffalse;
EndFunction // }}}

// erzeugt einen zufaelligen Code, der von dont_accept abweicht (wenn gesetzt)
Const RANDOM_CODE_LENGTH := 8;
Function GenRandomCode(dont_accept := 0) // {{{
    // folgende Zeichen werden fuer den Code benutzt
    // alphanumerisch, ausser o, O, 0 und i, I, l wegen Verwechselung
    var chars := {
        "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F",
        "G", "H", "J", "K", "L", "M", "N", "P", "Q", "R", "S", "T", "U", "V",
        "W", "X", "Y", "Z", "a", "b", "c", "d", "e", "f", "g", "h", "j", "k",
        "m", "n", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z"
    };

    var code := "";
    While (Len(code) < RANDOM_CODE_LENGTH)
        code += chars.randomentry();
        If (dont_accept and code == dont_accept)
            code := "";
        EndIf
    EndWhile

    return code;
EndFunction // }}}

// macht aus String <str> ein Stringarray mit maximaler Zeilenlaenge <wrap>
Function TextWrap(str, wrap := 60) // {{{
    var lines := array{};
    var line  := "";
    ForEach word in (SplitWords(str))
        SleepMs(2);
        If (Len(line+word+" ") <= wrap)
            // immer anhaengen, solange es passt
            line += word+" ";
        Else
            // passt nicht mehr, neue Zeile anfangen
            lines.append(Trim(line));
            line := word+" ";
        EndIf
    EndForEach
    // noch was uebrig?
    If (line)
        lines.append(Trim(line));
    EndIf

    return lines;
EndFunction // }}}

// das erste Zeichen des Strings zum Grossbuchstaben machen
Function UCFirst(str)
    str[1] := Upper(str[1]);
    return str;
EndFunction
