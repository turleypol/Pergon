//////////////////////////////////////////////////////
//
//   Alchemie Include
//
//   Modifications:
//     2006/11/30 Hotny:  Erstellt
//
//////////////////////////////////////////////////////

///////////////////////////////////
//
// Globale Konstanten und Variablen
//
///////////////////////////////////

Var fWirkcfg := ReadConfigFile(":alchemie:alchemie");

Var aPositiveWirkungen:={"leuchtend","entgiftend"};
Var aNeutraleWirkungen:={"heilend","erfrischend","astralenergetisch","staerkend","beweglich","erhellend","feuerresistent","wasserresistent","erdresistent","luftresistent","giftresistent"};
Var aNegativeWirkungen:={"explodierend","vergiftend","erblindend"};

Function FindeWirkungsfaktoren(pReags)
  ///////////////////////////////////////
  //
  // Funktionalität: Berechnen aller möglichen Wirkungen mit den entsprechenden Faktoren (inklusive Wichtung)
  //
  // Eingangsparameter: Reags -> Array (Element: Objtype der Reagenzie)
  // Rückgabe im Normalfall: array{Trankgewichtung, dictionary (Schlüssel: Wirkungsbeschreibung, Wert: Wirkungsstärkefaktor)}
  // Rückgabe im Fehlerfall: 0
  /////////////////////////////////////////

  //Jetzt Wirkungsdict berechnen
  Var dcWirkungsdic := dictionary{
                                 "heilend"          ->0.0,
                                 "erfrischend"      ->0.0,
                                 "astralenergetisch"->0.0,
                                 "staerkend"        ->0.0,
                                 "beweglich"        ->0.0,
                                 "erhellend"        ->0.0,
                                 "explodierend"     ->0.0,
                                 "leuchtend"        ->0.0,
                                 "vergiftend"       ->0.0,
                                 "entgiftend"       ->0.0,
                                 "feuerresistent"   ->0.0,
                                 "wasserresistent"  ->0.0,
                                 "erdresistent"     ->0.0,
                                 "luftresistent"    ->0.0,
                                 "giftresistent"    ->0.0,
                                 "versteckend"      ->0.0,
                                 "erblindend"       ->0.0
                                 };
  Var dcGueltigCount := dictionary{
                                 "heilend"          ->0,
                                 "erfrischend"      ->0,
                                 "astralenergetisch"->0,
                                 "staerkend"        ->0,
                                 "beweglich"        ->0,
                                 "erhellend"        ->0,
                                 "explodierend"     ->0,
                                 "leuchtend"        ->0,
                                 "vergiftend"       ->0,
                                 "entgiftend"       ->0,
                                 "feuerresistent"   ->0,
                                 "wasserresistent"  ->0,
                                 "erdresistent"     ->0,
                                 "luftresistent"    ->0,
                                 "giftresistent"    ->0,
                                 "versteckend"      ->0,
                                 "erblindend"       ->0
                                 };

  //jede Reag gewichtet Addieren
  Var sKeys := GetConfigIntKeys(fWirkcfg);
  Var aUsedReag := {};
  Var mehrfachverwendetfaktor:=1.0;
  Var howmuch;
  Var trankgewicht:=0.0;

  ForEach Reagenzie in pReags
    If (Reagenzie in sKeys)
      mehrfachverwendetfaktor:=1.0;
      If (Reagenzie in aUsedReag)
        //wenn diese reag schon im rezept ist, dann die wirkung wichten
        howmuch := CountInArray(aUsedReag,Reagenzie) + 1;
        If (howmuch > 3)
          mehrfachverwendetfaktor:=0.4;
        ElseIf (howmuch == 2)
          mehrfachverwendetfaktor:=0.6;
        EndIf
      EndIf
      AddWirkung(Reagenzie, dcWirkungsdic, mehrfachverwendetfaktor, dcGueltigCount, aUsedReag);
    Else
      //Die Reag ist Unbekannt -> ganze Rezept schlägt fehl
      Return 0;
    EndIf
    //Reag wird schon verwendet
    aUsedReag.append(Reagenzie);
    trankgewicht+=fWirkcfg[Reagenzie].gewicht;
  EndForEach

  //Wir kennen alle Wirkungen, jetzt schauen, ob auch ein korrektes Rezept
  Var isGueltig:=0;
  ForEach element in dcGueltigCount
    If (element >= 3) // 3 verschiedene Reagenzien müssen pro Wirkung genutzt werden damit gültig
      isGueltig:=1;
    Else
      dcWirkungsdic[_element_iter]:=0.0;
    EndIf
  EndForEach
  If (!isGueltig)
    Return 0;
  EndIf
  trankgewicht/=pReags.size();
  Return {trankgewicht,dcWirkungsdic};
EndFunction

Function AddWirkung(pReag, byref pWirkdic, pGewicht, byref pGueltigCount, aUsedReag)
  ForEach wirk in pWirkdic
    If (GetConfigReal(fWirkcfg[pReag],_wirk_iter))
      wirk+=GetConfigReal(fWirkcfg[pReag],_wirk_iter)*pGewicht;
      If (!(pReag in aUsedReag)) // Nur wenn noch nicht vorhanden wird Wirkungscounter erhöht
        pGueltigCount[_wirk_iter]+=1;
      EndIf
    EndIf
  EndForEach
EndFunction

Function CountInArray(aArray,item)
  Var i:=0;
  ForEach t in aArray
    If (t==item)
      i+=1;
    EndIf
  EndForEach
  Return i;
EndFunction

Function WirkungsKorrektur(wirkungen,Skill)
  ///////////////////////////////////////
  //
  // Funktionalität: Sortiert Wirkungen nach Größe und schneidet überzählige Wirkungen ab
  //
  // Eingangsparameter: wirkungen -> array{trankgewicht, dictionary (Schlüssel: Wirkungsbeschreibung, Wert: Wirkungsstärkefaktor)}
  //                    Skill     -> GetSkill()
  //
  // Rückgabe im Normalfall: array{trankgewicht, array({Wirkungsbeschreibung,Wirkungsstärkefaktor})}
  // Rückgabe im Fehlerfall: Integer (<0)
  /////////////////////////////////////////
  Var temp:=array;
  Var done;
  ForEach wirk in (wirkungen[2]) // Alle Wirkungen durchgehen und nach Größe Sortieren
    If (wirk)
      done:=0;
      ForEach sorted in temp
        If (wirk>sorted[2])
          temp.insert(_sorted_iter,{_wirk_iter,wirk});
          done:=1;
          Break;
        EndIf
      EndForEach
      If (!done)
        temp.append({_wirk_iter,wirk});
      EndIf
    EndIf
  EndForEach

  // Max Anzahl an Wirkungen Abhängig vom Skill
  If (Skill<50)
    temp.shrink(1);
  ElseIf (Skill<81)
    temp.shrink(2);
  ElseIf (Skill<100)
    temp.shrink(3);
  EndIf
  Return({wirkungen[1],temp});
EndFunction

Function GetDifficulty(wirkungen)
  ///////////////////////////////////////
  //
  // Funktionalität: Berechnen der Gesamtschwierigkeit (Addition aller Wirkungsstärkefaktoren)
  //
  // Eingangsparameter: wirkungen -> array{trankgewicht, array({Wirkungsbeschreibung,Wirkungsstärkefaktor})}
  //
  // Rückgabe im Normalfall: Integer (>=0)
  // Rückgabe im Fehlerfall: Integer (<0)
  /////////////////////////////////////////
  Var dicWirkung := wirkungen[2];
  //Trankgewichtung vorerst deaktivert, da sie sich nur im oberen Bereich und dort stark bemerkbar macht
  //Var Trankgewicht:=wirkungen[1];
  Var Trankgewicht:=1;

  Var WirkungsSumme:=0;
  ForEach wirk in dicWirkung
    WirkungsSumme+=wirk[2];
  EndForEach

  Return CInt((1.4-(2.8/(1.0+Pow(ConstE(),(WirkungsSumme * Trankgewicht/3.0)))))*100);
EndFunction

Function BerechneSkalierteWirkungen(wirkungen,trankdiff,skill,bWaageVorhanden)
  ///////////////////////////////////////
  //
  // Funktionalität: Berechnung der Auswirkungen auf Player/NPCs
  //                 (Funktionsberechnung und Wichtung nach Wirkungsfeld)
  //
  // Eingangsparameter: array{Trankgewichtung -> double,
  //                          wirkungen -> dictionary (Schlüssel: Wirkungsbeschreibung, Wert: Wirkungsstärkefaktor)
  //                         }
  //                    trankdiff: die Schwierigkeit des Trankes (UrForm, der 100 faktor wird hier benutzt)
  //                    skill: der Skill mit dem der Player den Trank braut/brauen will
  //
  // Rückgabe im Normalfall: array (array: Wirkungsbeschreibung, Wirkungsstärke, Wirkungsdauer)
  //
  // Rückgabe im Fehlerfall: 0
  /////////////////////////////////////////
  var dbTrankgewichtung:=wirkungen[1];
  var aWirkungen := wirkungen[2];

  //Hier jetzt die Randomskalierung
  //Falls er den Trank super kann, brauch man nix abziehen
  Var RandomBereich:=CInt(Max(0,trankdiff + 20 - skill));

  ForEach aWirk In aWirkungen
    //Auswirkungswert berechnen
    aWirk[2]:=GetWirkungsWert(dbTrankgewichtung*aWirk[2]);
    //Zufallsbereich hinzurechnen
    aWirk[2]:=aWirk[2] - RandomBereich + RandomInt(RandomBereich);
    aWirk[2]:=CInt(Max(0,aWirk[2]));
    //Dauer berechnen und zum Array hinzufügen
    aWirk.append(FindeWirkungsdauer(aWirk[2],aWirk[1],bWaageVorhanden));
  EndForeach

  //Hier werden die zufälligen Nebenwirkungen ausgewürfelt und hinzugefügt
  var aNebenwirkungen:={};
  //erste Nebenwirkung
  var rndNw:=RandomInt(100);
  If (rndNw < ((trankdiff + 20 - skill)*1.5))
    //zufall liegt zwischen 0-60% je nach Fähigkeit für die erste Nebenwirkung
    rndNw:=Max(10,rndNw); // 10%Mindestwirkung
    If (aWirkungen[1][1] in (aPositiveWirkungen + aNeutraleWirkungen))
      //Eine negative Nebenwirkung würfeln (hier werden auch stärke und co abgefangen, da die Tränke im Brauen selbst nicht negativ werden können
      var aNw := aNegativeWirkungen + aNeutraleWirkungen;
      var sNebenwirkung := aNw[RandomInt(aNw.size())+1];
      If (sNebenwirkung in aNeutraleWirkungen)
        var staerke:= -1 * SkaliereWirkung(sNebenwirkung,(rndNw/100.0)*aWirkungen[1][2]);
        aNebenwirkungen.append({sNebenwirkung,staerke,FindeWirkungsdauer(staerke,sNebenwirkung,bWaageVorhanden)});
      Else
        var staerke:=SkaliereWirkung(sNebenwirkung,(rndNw/100.0)*aWirkungen[1][2]);
        aNebenwirkungen.append({sNebenwirkung,staerke,FindeWirkungsdauer(staerke,sNebenwirkung,bWaageVorhanden)});
      EndIf
    Else
      //Eine positive Nebenwirkung würfeln, da ein negativer Trank
      var aNw:=aPositiveWirkungen + aNeutraleWirkungen;
      var sNebenwirkung:=aNw[RandomInt(aNw.size())+1];
      var staerke:=SkaliereWirkung(sNebenwirkung,((rndNw/100.0)*aWirkungen[1][2]));
      aNebenwirkungen.append({sNebenwirkung,staerke,FindeWirkungsdauer(staerke,sNebenwirkung,bWaageVorhanden)});
    EndIf
  EndIf

  rndNw:=RandomInt(100);
  If (rndNw < ((trankdiff + 20 - skill)*0.75))
    rndNw:=Max(10,rndNw); // 10%Mindestwirkung
    //zufall liegt zwischen 0-30% je nach Fähigkeit für die zweite Nebenwirkung
    If (aWirkungen[1][1] in (aPositiveWirkungen + aNeutraleWirkungen))
      //Eine negative Nebenwirkung würfeln (hier werden auch stärke und co abgefangen, da die Tränke im Brauen selbst nicht negativ werden können
      var aNw := aNegativeWirkungen + aNeutraleWirkungen;
      var sNebenwirkung := aNw[RandomInt(aNw.size())+1];
      If (sNebenwirkung in aNeutraleWirkungen)
        var staerke:= -1 * SkaliereWirkung(sNebenwirkung,((rndNw/100.0)*aWirkungen[1][2]));
        If (staerke<0)
          aNebenwirkungen.append({sNebenwirkung,staerke,FindeWirkungsdauer(staerke,sNebenwirkung,bWaageVorhanden)});
        EndIf
      Else
        var staerke:= SkaliereWirkung(sNebenwirkung,(rndNw/100.0)*aWirkungen[1][2]);
        If (staerke)
          aNebenwirkungen.append({sNebenwirkung,staerke,FindeWirkungsdauer(staerke,sNebenwirkung,bWaageVorhanden)});
        EndIf
      EndIf
    Else
      //Eine positive Nebenwirkung würfeln, da ein negativer Trank
      var aNw:=aPositiveWirkungen + aNeutraleWirkungen;
      var sNebenwirkung:=aNw[RandomInt(aNw.size())+1];
      var staerke:=SkaliereWirkung(sNebenwirkung,(rndNw/100.0)*aWirkungen[1][2]);
      If (staerke)
        aNebenwirkungen.append({sNebenwirkung,staerke,FindeWirkungsdauer(staerke,sNebenwirkung,bWaageVorhanden)});
      EndIf
    EndIf
  Endif

  rndNw:=RandomInt(100);
  //Jetzt die Nebenwirkung die über der Skillbereich hinaus geht
  If (rndNw < ((trankdiff + 30 - skill)*0.5))
    rndNw:=Max(10,rndNw); // 10%Mindestwirkung
    //die Wahrscheinlichkeit für dererlei Wirkung liegt zwischen 0-25% je nach Fähigkeit
    var aNw:={"verdunkelnd","hungrig","durstig"};
    var sNebenwirkung:=aNw[RandomInt(aNw.size())+1];
    var staerke:=SkaliereWirkung(sNebenwirkung,(rndNw/100.0)*aWirkungen[1][2]);
    If (staerke)
      aNebenwirkungen.append({sNebenwirkung,staerke,FindeWirkungsdauer(staerke,sNebenwirkung,bWaageVorhanden)});
    EndIf
  Endif

  var i;
  For(i:=1;i<=aWirkungen.size();i+=1)
    //Jetzt die Wirkung noch Skalieren
    //muss hier gemacht werden, weil die Faktoren in der Reinform für die Nebenwirkungsberechnung gebraucht wird
    var staerke:=SkaliereWirkung(aWirkungen[i][1],aWirkungen[i][2]);
    If (staerke<>0)
      aWirkungen[i][2]:=staerke;
    Else
      aWirkungen.erase(i);
      i-=1;
    EndIf
  EndFor

  Return {aWirkungen,aNebenwirkungen};
EndFunction

Function SkaliereWirkung(psWirkung,psFaktor)
  Case (psWirkung)
    "heilend":
      Return CInt(psFaktor*1.5);
    "erfrischend":
      Return CInt(psFaktor*1.5);
    "astralenergetisch":
      Return CInt(psFaktor * 1.125);
    "staerkend":
      Return CInt(psFaktor/6.6);
    "beweglich":
      Return CInt(psFaktor/6.6);
    "erhellend":
      Return CInt(psFaktor/6.6);
    "explodierend":
      Return CInt(psFaktor*1.5);
    "leuchtend":
      Return CInt(Min(psFaktor*0.15,24)); //24Maximum
    "vergiftend":
      Return CInt(psFaktor*0.25); // /15);
    "entgiftend":
      Return CInt(psFaktor*0.25); // /15);
    "feuerresistent":
      Return CInt(psFaktor/1.3);
    "wasserresistent":
      Return CInt(psFaktor/1.3);
    "erdresistent":
      Return CInt(psFaktor/1.3);
    "luftresistent":
      Return CInt(psFaktor/1.3);
    "giftresistent":
      Return CInt(psFaktor/1.3);
    "versteckend":
      Return CInt(psFaktor/2.5);
    "erblindend":
      Return CInt(psFaktor/2.5);
    "verdunkelnd":
      Return CInt(Max(1,Min(24,psFaktor*0.15)));  // 1..24
    "hungrig":
      return CInt(Max(1,Min(10,psFaktor/20)));  // 1..10
    "durstig":
      return CInt(Max(1,Min(10,psFaktor/20))); //1..10
    Default:
      Return 0;
  EndCase
EndFunction

Function GetWirkungsWert(x)
  return (15*sqrt(x)+10*x);
EndFunction

Function FindeFarbe(sHauptWirkung)
  ///////////////////////////////////////
  //
  // Funktionalität: Hauptwirkung filtern und den Farbwert zurückgeben
  //
  // Eingangsparameter: String mit der Bezeichnung der Hauptwirkung
  // Rückgabe im Normalfall: Integer (Farbcode)
  // Rückgabe im Fehlerfall: Integer (0)
  /////////////////////////////////////////
  Case (sHauptWirkung)
    "heilend":           Return(1359);
    "erfrischend":       Return(324);
    "astralenergetisch": Return(1283);
    "staerkend":         Return(1737);
    "beweglich":         Return(414);
    "erhellend":         Return(10);
    "explodierend":      Return(1289);
    "leuchtend":         Return(2962);
    "vergiftend":        Return(62);
    "entgiftend":        Return(1742);
    "feuerresistent":    Return(38);
    "wasserresistent":   Return(101);
    "erdresistent":      Return(1133);
    "luftresistent":     Return(1419);
    "giftresistent":     Return(58);
    "versteckend":       Return(931);
    "erblindend":        Return(0);
    default:             Return(0);
  EndCase
EndFunction

Function FindeWirkungsdauer(dbWirkungsStaerke,sWirkung,bWaageVorhanden)
  dbWirkungsStaerke:=Abs(dbWirkungsStaerke);
  
  //Rückgabe sind Sekundenwerte
  Case (sWirkung)
    "heilend":
      If (bWaageVorhanden)
        Return 0;
      Else
        Return 30;
      EndIf
    "erfrischend":
      If (bWaageVorhanden)
        Return 0;
      Else
        Return 30;
      EndIf
    "astralenergetisch":
      If (bWaageVorhanden)
        Return 0;
      Else
        Return 30;
      EndIf
    "staerkend":
      Return CInt(dbWirkungsStaerke*10);
    "beweglich":
      Return CInt(dbWirkungsStaerke*10);
    "erhellend":
      Return CInt(dbWirkungsStaerke*10);
    "explodierend":
      Return 0;
    "leuchtend":
      Return CInt(dbWirkungsStaerke*16);
    "vergiftend":
      Return 0;
    "entgiftend":
      Return 0;
    "feuerresistent":
      Return CInt(dbWirkungsStaerke*9);
    "wasserresistent":
      Return CInt(dbWirkungsStaerke*9);
    "erdresistent":
      Return CInt(dbWirkungsStaerke*9);
    "luftresistent":
      Return CInt(dbWirkungsStaerke*9);
    "giftresistent":
      Return CInt(dbWirkungsStaerke*9);
    "versteckend":
      If (bWaageVorhanden)
        Return 0;
      Else
        Return CInt(dbWirkungsStaerke*3*2.5); //weil 2.5 ist der Faktor für die Korrektur der Diff beim entdecken
      EndIf
    "verdunkelnd":
      Return CInt(dbWirkungsStaerke*16);
    "hungrig":
      If (bWaageVorhanden)
        Return 0;
      Else
        Return 10;
      EndIf
    "durstig":
      If (bWaageVorhanden)
        Return 0;
      Else
        Return 10;
      EndIf
    "erblindend":
      Return 0;
    Default:
      Return 0;
  EndCase
EndFunction