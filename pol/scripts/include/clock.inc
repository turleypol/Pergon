use cfgfile;
use uo;
use util;

// Tagesabschnittslaengen auf Pergon:
// Laenge eines Tages in Minuten
Const PERGON_LENGTH_DAY       := 5*60;  // 5 Stunden
// Anteil der Nacht am Tag
Const PERGON_LENGTH_NIGHT     := 60+40; // 1 Stunde, 40 Minuten
// Anteil der Uebergaenge Tag<->Nacht
Const PERGON_LENGTH_TRANSITON := 30;    // 30 Minuten

var Daylength_ := PERGON_LENGTH_DAY * 60; // in Sekunden

// Zeitangaben mit oder ohne Sekunden liefern
Const WITH_SECONDS := 1;
Const NO_SECONDS   := 0;

// kurze, mittlere oder lange Angaben
Const SHOW_DATE_MEDIUM := 0;
Const SHOW_DATE_FULL   := 1;
Const SHOW_DATE_SHORT  := 2;

// Funktionen fuer den Pergontag
// GetPergonTime - Returns the Time in Seconds
Function GetPergonTime() // {{{
  return CInt(ReadGameClock() % (PERGON_LENGTH_DAY * 60));
EndFunction // }}}

// GetPergonTimeofDay - Returns the Time as a String
Function GetPergonTimeOfDay(tzone := 0) // {{{
  var time := GetPergonTime()*24;

  // Stunden abspalten
  var hours := CInt(time / (PERGON_LENGTH_DAY * 60));
  time %= PERGON_LENGTH_DAY * 60;

  // aus Minuten extrahieren
  var minutes := CInt(time / PERGON_LENGTH_DAY);

  // es gibt theoretisch eine Zeitzone mit 12h Versatz
  If (tzone == 1)
    hours += 12;
  EndIf
  If (hours > 23)
    hours -= 24;
  EndIf

  // ggf. fuehrende Null ergaenzen und fertig
  return AddLeadingZero(hours)+":"+AddLeadingZero(minutes);
EndFunction // }}}

// GetPergonHourOfDay - Returns the Hour as a String
Function GetPergonHourOfDay(tzone := 0) // {{{
  var time := GetPergonTimeOfDay(tzone);
  var hour := SplitWords(time, ":")[1];
  If (hour[1] == "0")
    hour[1] := "";
  EndIf
  return CInt(hour);
EndFunction // }}}

// Funktionen zur Darstellung von PolCore().systime

// GetDateTimeStr - Liefert den aktuellen Tage/Uhrzeit-String
Function GetDateTimeStr(type := SHOW_DATE_MEDIUM, systime := 0) // {{{
  Case (type)
  SHOW_DATE_FULL:
    return StrFormatTime("[%d.%m.%Y %H:%M:%S]", systime);
  SHOW_DATE_SHORT:
    return StrFormatTime("[%d.%m. %H:%M]",      systime);
  default:
  SHOW_DATE_MEDIUM:
    return StrFormatTime("[%d.%m. %H:%M:%S]",   systime);
  EndCase
EndFunction // }}}

// GetDateStr - Liefert den aktuellen Tage-String
Function GetDateStr(systime := 0) // {{{
  return StrFormatTime("%d.%m.%Y", systime);
EndFunction // }}}

// aktuelles Jahr besorgen
Function GetCurrentYear() // {{{
    // %Y  year as a decimal number, e.g. 2017
    return CInt(StrFormatTime("%Y"));
EndFunction // }}}

// aktuellen Monat besorgen
Function GetCurrentMonth() // {{{
    return CInt(Trim(StrFormatTime("%m"), TRIM_LEFT, "0 "));
EndFunction // }}}

// aktuellen Tag im Monat besorgen
Function GetCurrentDay() // {{{
    // %d  The day of the month as a decimal number (range 01 to 31).
    // %e  Like %d, the day of the month as a decimal number, but a
    //     leading zero is replaced by a space.
    // Between the % character and the conversion specifier character,
    // an optional flag and field width may be specified.
    // -   (dash) Do not pad a numeric result string.
    return CInt(Trim(StrFormatTime("%d"), TRIM_LEFT, "0 "));
EndFunction // }}}

// aktuelle Stunde besorgen
Function GetCurrentHour() // {{{
    return CInt(Trim(StrFormatTime("%H"), TRIM_LEFT, "0 "));
EndFunction // }}}

// aktuelle Minute besorgen
Function GetCurrentMinute() // {{{
    return CInt(Trim(StrFormatTime("%M"), TRIM_LEFT, "0 "));
EndFunction // }}}

// nochmal mit ISO-Format
// GetDateStrISO - Liefert Datum und Uhrzeit im ISO-Format
Function GetDateTimeStrISO(systime := 0, no_seconds := 0) // {{{
  return GetDateStrISO(systime)+" "+GetTimeStrISO(systime, no_seconds);
EndFunction // }}}

// GetDateStrISO - Liefert Datum im ISO-Format
Function GetDateStrISO(systime := 0) // {{{
  return
    StrFormatTime("%Y", systime)+"-"+
    AddLeadingZero(StrFormatTime("%m", systime))+"-"+
    AddLeadingZero(StrFormatTime("%d", systime));
EndFunction // }}}

// GetTimeStrISO - Liefert den Uhrzeit im ISO-Format
Function GetTimeStrISO(systime := 0, no_seconds := 0) // {{{
  If (no_seconds)
    return StrFormatTime("%H:%M", systime);
  Else
    return StrFormatTime("%H:%M:%S", systime);
  EndIf
EndFunction // }}}

// alter Muell {{{
//Const DateDelta := 719163; // days since 1/1/0001 till 1/1/1970
//
//const SECONDS_IN_MINUTE := 60;
//const HOURS_IN_DAY := 24;
//const MINUTES_IN_HOUR := 60;
//
//const SECONDS_IN_HOUR := SECONDS_IN_MINUTE * MINUTES_IN_HOUR;
//const SECONDS_IN_DAY := SECONDS_IN_HOUR * HOURS_IN_DAY;
//
//const D1 := 365;
//const D4 := D1 * 4 + 1;
//const D100 := D4 * 25 - 1;
//const D400 := D100 * 4 + 1;
//
//function DecodeDate(time, byref year, byref month, byref day)
//  var T := CInt(time / SECONDS_IN_DAY) + DateDelta;
//  if (T <= 0)
//    month := 0;
//    day := 0;
//  else
//    T := T - 1;
//    var Y := 1;
//    while (T >= D400)
//      T := T - D400;
//      Y := Y + 400;
//    endwhile
//    var I, D;
//    DivMod(T, D100, I, D);
//    if (I == 4)
//      I := I - 1;
//      D := D + D100;
//    endif
//    Y := Y + (I * 100);
//    DivMod(D, D4, I, D);
//    Y := Y + (I * 4);
//    DivMod(D, D1, I, D);
//    if (I == 4)
//      I := I - 1;
//      D := D + D1;
//    endif
//    Y := Y + I;
//    var IsLeap := IsLeapYear(Y) + 1;
//    var M := 1;
//    while (1)
//      I := MonthDays[IsLeap][M];
//      if (D < I) break; endif
//      D := D - I;
//      M := M + 1;
//    endwhile
//    month := M;
//    year := Y;
//    day := D + 1;
//  endif
//endfunction
//
//function DecodeTime(time, byref hour, byref minute, byref second)
//  time := CInt(time % SECONDS_IN_DAY); // ingore beyond one day
//  hour := CInt(time / SECONDS_IN_HOUR);
//  second := CInt(time % SECONDS_IN_HOUR);
//  minute := CInt(second / SECONDS_IN_MINUTE);
//  second := CInt(second % SECONDS_IN_MINUTE);
//endfunction
//
///**
// * Does division and returns result and reminder.
// * Used internally. Should be moved to separate Maths.inc.
// *
// */
//function DivMod(dividend, divisor, byref result, byref remainder)
//  result := 0;
//  remainder := 0;
//  if (divisor)
//    result := CInt(dividend / divisor);
//    remainder := CInt(dividend % divisor);
//  endif
//endfunction
//
///**
// * Tells if given year is a leap year.
// *
// */
//function IsLeapYear(year)
//  return ((year % 4) == 0) and (((year % 100) <> 0) or ((year % 400) == 0));
//endfunction
//
//// The MonthDays array can be used to quickly find the number of
////  days in a month:  MonthDays[IsLeapYear(Y) + 1][M]
//
//var MonthDays := array {
//  array { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
//  array { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 } };
// }}}

// Allgemeine Hilfsfunktionen

// HourWithinRange - Is Hour within Range
Function HourWithinRange(hr, hrstart, hrend) // {{{
  hr := CInt(hr);// +24;
  hrstart := CInt(hrstart);// +24;
  hrend := CInt(hrend);// +24;

  If (hrstart>hrend)
    If (((hrstart <= hr) And (hr<24)) OR ((hr<hrend)AND(hr >= 0)))
      return (1);
    EndIf
  Else
    If ((hrstart <= hr) And (hr<hrend))
      return (1);
    EndIf
  EndIf

  return (0);
EndFunction // }}}

// Format_Time - Zeitdauer formatieren
Function Format_Time(time, with_seconds := 0) // {{{
  var days := CInt(time / 86400);
  time %= 86400;
  var hours := CInt(time / 3600);
  time %= 3600;
  var minutes := CInt(time /60);
  time %= 60;
  var seconds := CInt(time);

  // Zeit-Ausgabestring zusammenbasteln
  var result := "";
  If (days)
    result += days + "d, ";
  EndIf

  result += hours + ":" + AddLeadingZero(minutes);

  If (with_seconds)
    result += ":" + AddLeadingZero(seconds);
  EndIf

  return result;
EndFunction // }}}

// Fuehrende Null montieren
Function AddLeadingZero(value) // {{{
    If (Len(value+"") <= 1)
        return "0" + value;
    EndIf
    return value;
EndFunction // }}}
