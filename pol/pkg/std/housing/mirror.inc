/*
 *  Mirror.inc
 *  gemeinsame Funktionen und Konstanten fuer Mirror-Scripte
 */

include "house";

Const PROP_MIRROR_WHAT := "mirror_item";
Const ESIGNNOTFOUND    := "Sign to mirror not found";
Const ESIGNNOTEXIST    := "Sign ceased to exist";

// holt sich Farbe, Beschreibung und Sichbarkeit von
// einem anderen Schild und stellt sie dar
Function UpdateSign(mirror)
  // Serial besorgen
  var sign := GetObjProperty(mirror, PROP_MIRROR_WHAT);
  If (!sign)
    EraseObjProperty(mirror, PROP_MIRROR_WHAT);
    var ret := error;
    ret.errortext := ESIGNNOTFOUND;
    return ret;
  EndIf
  
  // Schild zur Serial ermitteln
  sign := SystemFindObjectBySerial(sign);
  If (!sign)
    If (mirror.color != COLOR_RENTMESOON)
      mirror.color := COLOR_RENTMESOON;
    EndIf
    
    var ret := error;
    ret.errortext := ESIGNNOTEXIST;
    return ret;
  EndIf

  // Namen, Unsichtbarkeit und Farbe holen
  If ((sign.name) and mirror.name != sign.name)
    SetName(mirror, sign.name);
  EndIf
  If (mirror.invisible != sign.invisible)
    mirror.invisible := sign.invisible;
  EndIf
  If (mirror.color != sign.color)
    mirror.color := sign.color;
  EndIf

  // bei Prop-Aenderungen tut sich eventuell was an den Tooltips
  var changes := 0;
  // ausgewaehlte CProps kopieren, so vorhanden
  ForEach prop in (array{
    // Account und Besitzer
    PROP_HS_C_ACCOUNT, PROP_HS_C_OWNER,
    // Anzeigename
    PROP_HS_C_OWNERNAME,
    // falls Schild zu Multihaus gehoert, Verweis auf Multihaus
    PROP_HS_M_HOUSE,
    // Mietkram
    PROP_HS_C_RENT_TOOLTIP
    })
    SleepMS(2);
    var transfer := GetObjProperty(sign, prop);
    If (!transfer)
      var oldval := GetObjProperty(mirror, prop);
      If ((!oldval) or (oldval != transfer))
        SetObjProperty(mirror, prop, transfer);
        changes := 1;
      EndIf
    EndIf
  EndForEach

  If (changes)
    IncRevision(mirror);
  EndIf
EndFunction
