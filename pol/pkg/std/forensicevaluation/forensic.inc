////////////////////////////////////////////////////////////////////////
//
// 09.11.2003 Fraggulus: create
//
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
//  SetFingerPrint - setzt Fingerabdruecke bzw. Diebstahlsbeweise
//                 - siehe lockpicking.src
//                 - siehe stealing.src
////////////////////////////////////////////////////////////////////////

Function SetFingerPrint(who, tgt)

  // Array {who.serial, Zeitstempel} mit Fingerabdruecken suchen
  Var FPrintArray := {};

  ForEach FPrint in GetObjProperty(tgt, "FingerPrintArray")
    If (CInt(FPrint[1]) <> who.serial)
      FPrintArray.append(FPrint);
    EndIf
  EndForEach

  // neuen Eintrag ans Array haengen...
  FPrintArray.append({who.serial, ReadGameClock()});

  // ... und speichern
  SetObjProperty(tgt, "FingerPrintArray", FPrintArray);

EndFunction

////////////////////////////////////////////////////////////////////////
//  CleanFingerPrintArray - saeubert das Array von zu alten bzw. ungueltigen Eintraegen
////////////////////////////////////////////////////////////////////////

Function CleanFingerPrintArray(ByRef FPArray, DecayTime)

  // Array {who.serial, Zeitstempel} mit Fingerabdruecken suchen
  Var TempFPArray := {};

  ForEach FPrint in FPArray
    If ((CInt(FPrint[2]) >= ReadGameclock() - DecayTime) && (len(FPrint) == 2) && (FPrint[1]) && (FPrint[2]))
      TempFPArray.append(FPrint);
    EndIf
  EndForEach

  // ... und speichern
  FPArray := TempFPArray;

EndFunction