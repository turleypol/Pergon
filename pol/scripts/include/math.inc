// math.inc
//
// Siggy's Improved Math include, v1.2
//
// Questions, comments, flames?   Send to:
// ICQ: 10520050
//
//  19.12.2005 Fraggulus: Remainder() in GetRemainder() umbenannt, da es sonst mit Variablen aus anderen Includes kollidiert
//

Function RandIntMinMax(minval, maxval)
    return (RandomIntMinMax(minval,maxval+1));
EndFunction

Function Modulus(Numerator, Denominator)

// New Mod, to appease Louds, Dundee, and other disgruntled elements :P
// Returns the modulus (remainder) of the fraction:
//
//     Numerator
//    -----------
//    Denominator
  If ((TypeOfInt(Numerator)==OT_LONG) && (TypeOfInt(Denominator)==OT_LONG))
  	return(Numerator%Denominator);
  Else
    return (Numerator - (CInt(Numerator/Denominator) * Denominator));
  EndIf
EndFunction

Function GetRemainder(Numerator, Denominator)

// Old Mod, now called "Remainder" to satisfy some weenies :P  It was designed
// this way to get remainders For non-integers.
// Returns the remainder of the fraction:
//
//     Numerator
//    -----------
//    Denominator
  return (Numerator%Denominator);

EndFunction


Function Trunc(number, places)

// Truncates a dbl to a fixed amount of decimal places.

  var base := Pow(10, places);

  number *= base;
  number := Cint(number);
  number := CDbl(number);
  number /= base;

  return number;

EndFunction


Function Fac(factorial)

// Returns the factorial of a number:
// Fac(10) = 10 * 9 * 8 * 7 * 6 * 5 * 4 * 3 * 2 * 1

  var x := 1;
  While (factorial > 0)
    x *= factorial;
    factorial -= 1;
  EndWhile
  return x;

EndFunction


Function Mean(ArrayX)

// Returns the arithmatic mean (average) of an array of numbers.

  var n, i := 0.000;

  If (ArrayX)
    ForEach n in ArrayX
      i += ArrayX[n];
    EndForEach
  EndIf

  n := CDbl(i / Len(ArrayX));

  return n;

EndFunction


Function StdDev(ArrayX)

// Returns the Standard Deviation of an array of numbers.

  var m, n, i := 0.000;
  var ArrayMean := Mean(ArrayX);
  var ArrayLen := Len(ArrayX);

  If (ArrayX)
    ForEach m in ArrayX
      n := ArrayX[m] - ArrayMean;
      n := Pow(n, 2);
      i += n;
    EndForEach
  EndIf

  i /= (ArrayLen - 1);
  n := Sqr (i);

  return n;

EndFunction


Function ZTest(ArrayX, mu, sigma)

// Returns the test statistic For a ZTest.   This is used to show
// the likelihood that a sample is drawn from the population.  The
// mean and standard deviation For the population must be known.
// The z-statistic is computed as follows:
//
//  z = (mean - mu) / (sigma / (Sqr(n))
//
// mean = the mean of the ArrayX data
// mu = the mean of the entire population
// sigma = the standard deviation of the population
// n = the number of samples in the data

  var i, n;
  var z := 0.000;
  var ArrayMean := Mean(ArrayX);
  var ArrayLen := Len(ArrayX);

  i := ArrayMean - mu;
  n := sigma / Sqr(ArrayLen);

  z := i/n;

  return := z;

EndFunction


Function TTest(ArrayX, mu)

// Returns the test statistic For a TTest.   This is used to show
// the likelihood that a sample is drawn from the population, when
// the mean and standard deviation For the entire population are
// unknown.  The t-statistic is computed as follows:
//
//  t = (mean - mu) / (s / (Sqr(n))
//
// mean = the mean of the ArrayX data
// mu = the estimated mean of the entire population
// s = the standard deviation of the samples
// n = the number of samples in the data


  var i, n, s;
  var t := 0.000;
  var ArrayMean := Mean(ArrayX);
  var ArrayLen := Len(ArrayX);
  var s := StdDev(ArrayX);

  i := ArrayMean - mu;
  n := s / Sqr(ArrayLen);

  t := i/n;

  return := t;

EndFunction

Function BitIn(ArrayX)

// Takes an array of 8 bits and converts them into an integer.
// This is used For the flag support.  IMPORTANT: bit positon is
// *NOT* the same thing as the array position!   The first bit in
// the flag is the 8th bit in the array, and vice versa.  This is
// because it processes the bit values in the following order:
// 128, 64, 32, 16, 8, 4, 2, 1.

  var i := 8;
  var n;
  var bitvalue := 0;

  While (i >= 0)

    n := 8 - i;
    If (ArrayX[i] == 1)
      bitvalue += Pow(2, n);
    EndIf
    i -= 1;

  EndWhile

  return bitvalue;

EndFunction


Function BitOut(x)

// This Function takes an unsigned, short integer (0-255) and returns
// an array containing the individual bit values.   IMPORTANT: the
// first element of the array is NOT the first bit, it is the 8th
// bit, and so on down.   See the desc For BitIn For the details.

  x := Cint(x);
  var loop := 1;
  var bitvalue := 0;
  var ArrayX := {0,0,0,0,0,0,0,0};
  var n;

  While (loop <= 8)

    n := 8 - loop;
    bitvalue := Pow(2, n);
    If ((x - bitvalue) >= 0)
      ArrayX[loop] := 1;
      x -= bitvalue;
    EndIf
    loop += 1;

  EndWhile

  return ArrayX;

EndFunction


Function BitAnd(x, y)

// Performs a bitwise logical conjunction For two unsigned, short
// integers (0-255).  If both values For a position are set, then
// that position in the result is set.   If either are reset, then
// the position in the result is reset.

  var ArrayX := BitOut(x);
  var ArrayY := BitOut(y);
  var z := 0;
  var loop := 1;
  var n;

  While (loop <= 8)

    n := loop - 1;
    If ((ArrayX[loop] == 1) AND (ArrayY[loop] == 1))
      z += Pow(2, n);
    EndIf
    loop += 1;

  EndWhile

  return z;

EndFunction


Function BitOr(x, y)

// Performs a bitwise logical disjunction For two unsigned, short
// integers (0-255).  If either value (or both values) For a
// position are set, then that position in the result is set.   If
// both are reset, then the position in the result is reset.

  var ArrayX := BitOut(x);
  var ArrayY := BitOut(y);
  var z := 0;
  var loop := 1;
  locan n;

  While (loop <= 8)

    n := loop - 1;
    If ((ArrayX[loop] == 1) or (ArrayY[loop] == 1))
      z += Pow(2, n);
    EndIf
    loop += 1;

  EndWhile

  return z;

EndFunction


Function BitXor(x, y)

// Performs a bitwise logical exclusion For two unsigned, short
// integers (0-255).  If one and ONLY one value For a position is
// set, then that position in the result is set.   If both are
// either set or reset, then the position in the result is reset.

  var ArrayX := BitOut(x);
  var ArrayY := BitOut(y);
  var z := 0;
  var loop := 1;
  var n;

  While (loop <= 8)

    n := loop - 1;
    If (ArrayX[loop] - ArrayY[loop] <> 0)
      z += Pow(2, n);
    EndIf
    loop += 1;

  EndWhile

  return z;

EndFunction


Function BitEqv(x, y)

// Performs a bitwise logical equivalence For two unsigned, short
// integers (0-255).  If one and ONLY one value For a position is
// set, then that position in the result is reset.   If both are
// either set or reset, then the position in the result is set.

  var ArrayX := BitOut(x);
  var ArrayY := BitOut(y);
  var z := 0;
  var loop := 1;
  var n;

  While (loop <= 8)

    n := loop - 1;
    If (ArrayX[loop] - ArrayY[loop] == 0)
      z += Pow(2, n);
    EndIf
    loop += 1;

  EndWhile

  return z;

EndFunction


Function BitImp(x, y)

// Performs a bitwise logical implication For two unsigned, short
// integers (0-255).  If the a position in the first integer is set,
// and the corresponding position For the second integer is reset,
// then that position in the result is reset.  All other combinations
// will result in the value being set.

  var ArrayX := BitOut(x);
  var ArrayY := BitOut(y);
  var z := 0;
  var loop := 1;
  var n;

  While (loop <= 8)

    n := loop - 1;
    If (ArrayX[loop] - ArrayY[loop] < 1)
      z += Pow(2, n);
    EndIf
    loop += 1;

  EndWhile

  return z;

EndFunction


Function BitNot(x)

// Performs a logical negation on a short, unsigned integer (0-255).
// If a position in the integer is set, then that position in the
// result is reset.  Conversely, If a position in the integer is
// reset, then that position in the result is set.

  var ArrayX := BitOut(x);
  var z := 0;
  var loop := 1;
  var n;

  While (loop <= 8)

    n := loop - 1;
    If (ArrayX[loop] == 0)
      ArrayX[loop] := 1;
      z += Pow(2, n);
    Else
      ArrayX[loop] := 0;
    EndIf

    loop += 1;

  EndWhile

  return z;

EndFunction


Function FlagSet(flag, bit)

// This Function will set the <bit> position in the <flag> register.
// Both <flag> and <bit> are integers.

  bit := 9 - bit;

  var FlagArray := BitOut(flag);
  FlagArray[bit] := 1;

  bit := BitIn(FlagArray);

  return bit;

EndFunction


Function FlagReset(flag, bit)

// This Function will reset the <bit> position in the <flag> register.
// Both <flag> and <bit> are integers.

  bit := 9 - bit;

  var FlagArray := BitOut(flag);
  FlagArray[bit] := 0;

  bit := BitIn(FlagArray);

  return bit;

EndFunction


Function FlagCheck(flag, bit)

// This Function will check the flag in the <bit> position of the
// <flag> register.  It will return a 1 If the flag is set, and a 0
// If the flag is reset.  Both <flag> and <bit> are integers.

  var FlagArray := BitOut(flag);
  bit := 9 - bit;

  If (FlagArray[bit] == 1)
    return 1;
  Else
    return 0;
  EndIf

EndFunction

Function Round(zahl)
  If(zahl>0)
    zahl+=0.5;
  Else
    zahl-=0.5;
  EndIf
  return (CInt(Floor(zahl)));
EndFunction

Function RoundRand(zahl)
  Var vorkomma_anteil:=Floor(zahl);
  vorkomma_anteil += ((zahl-vorkomma_anteil) >= RandomFloat(1.0));
  Return CInt(vorkomma_anteil);
EndFunction