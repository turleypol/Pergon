///////////////////////////////////////////////////////////////////////////
// Rechnen mit Strings, damit Zahlen >32 Bit moeglich sind
//
// Author: mehdorn

use basic;
use os;

Include "include/pergonutil";

// richtet zwei Stringwerte auf gleiche Laenge aus, indem es von vorn mit
// gegebenenfalls Nullen auffuellt
Function StrAlign(byref str1, byref str2) // {{{
    // erst eventuelle Trenner entfernen
    str1 := StrReplace(str1, ",", "");
    str2 := StrReplace(str2, ",", "");

    While (Len(str1) < Len(str2))
        str1 := "0"+str1;
        SleepMS(2);
    EndWhile
    While (Len(str2) < Len(str1))
        str2 := "0"+str2;
        SleepMS(2);
    EndWhile
EndFunction // }}}

// entfernt fuehrende Nullen
Function StrCutLeadingZeros(str) // {{{
    str := Trim(str, TRIM_LEFT, "0");

    // da wir alle fuehrenden Nullen wegschneiden, kann der String beim
    // effektiven Wert 0 leer werden
    If ((str == "") || (!str))
        str := "0";
    EndIf

    return str;
EndFunction // }}}

// Addiert zwei Stringwerte
Function StrAddition(str, summand) // {{{
    // sicher ist sicher
    str := CStr(str);
    summand := CStr(summand);

    If (summand["-"])
        // bei negativen Summanden nehmen wir Subtraktion
        summand["-"] := "";
        return StrSubtraktion(str, summand);
    EndIf

    StrAlign(str, summand);

    var uebertrag := 0;
    var ergebnis := "";
    var i;
    For (i := Len(str); i > 0; i -= 1)
        var val := CInt(str[i]) + CInt(summand[i]) + uebertrag;
        If (val >= 10)
            uebertrag := 1;
            val -= 10;
        Else
            uebertrag := 0;
        EndIf
        ergebnis := CStr(val)+ergebnis;
        SleepMS(2);
    EndFor
    If (uebertrag > 0)
        ergebnis := CStr(uebertrag)+ergebnis;
    EndIf

    return StrCutLeadingZeros(ergebnis);
EndFunction // }}}

// Subtrahiert Subtrahend von Stringwert
Function StrSubtraktion(str, subtrahend) // {{{
    // sicher ist sicher
    str := CStr(str);
    subtrahend := CStr(subtrahend);

    If (subtrahend["-"])
        // bei negativen Summanden nehmen wir Addition
        subtrahend["-"] := "";
        return StrAddition(str, subtrahend);
    EndIf

    StrAlign(str, subtrahend);

    var uebertrag := 0;
    var ergebnis := "";
    var i;
    For (i := Len(str); i > 0; i -= 1)
        var val := CInt(str[i]) - CInt(subtrahend[i]) - uebertrag;
        If (val < 0)
            uebertrag := 1;
            val += 10;
        Else
            uebertrag := 0;
        EndIf
        ergebnis := CStr(val)+ergebnis;
        SleepMS(2);
    EndFor

    If (uebertrag > 0)
        var msg := error;
        msg.errortext := "Negative result not supported.";
        return msg;
    EndIf

    return StrCutLeadingZeros(ergebnis);
EndFunction // }}}

// alle drei Stellen ein Trennzeichen einbauen
Function StrPrettyPrint(str) // {{{
    // sicher ist sicher
    str := CStr(str);

    var newstr := "";
    var count  := 0;
    var i;
    // von hinten durchgehen
    For (i := Len(str); i > 0; i -= 1)
        // Ziffern uebernehmen und mitzaehlen
        newstr := str[i] + newstr;
        count += 1;
        If (
            // alle drei Stellen ein Trenner
            count % 3 == 0 and
            // ausser ganz vorn
            i > 1
        )
            newstr := "."+newstr;
        EndIf
    EndFor

    return newstr;
EndFunction // }}}

Function StrMultiplikation(str, faktor)
  
  If (!StrSubtraktion(str, faktor))
    var temp := str;
    str := faktor;
    faktor := temp;
  EndIf
  
  str := CStr(str);
  faktor := CInt(faktor);
  var i, newstr := "0";
  For (i := faktor; i > 0; i -= 1)
    newstr := StrAddition(newstr, str);
  EndFor
  
  return newstr;
EndFunction

// pruefen ob der String nur Zahlen enthaellt
Function StrIsNumeric(str)
  var i;
  For (i := 1; i < Len(str); i += 1)
    If (!IsNumeric(str[i]))
      return 0;
    EndIf
  EndFor
  return 1;
EndFunction
