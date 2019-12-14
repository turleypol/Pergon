///////////////////////////////////////////////////////////////////////////
// cleanup -- Funktionen fuer die Aktion "Pergon putzt"

include "include/itemnpc";
include "include/modifyskill";

// angenommener minimaler Goldwert eines Items
Const MIN_WORTH := 10;
// Zwischenspeicher fuer Belohnungssystem
Const GPROP_TRASHCOUNT := "Pergon_cleanup_count";
Const GPROP_TRASHWORTH := "Pergon_cleanup_worth";

// Anzahl und Wert von Item(-stacks) und Containern ermitteln
Function AmountAndWorth(obj, amount_override := 0) // {{{
    If (!obj)
        return error{errortext := "No object"};
    EndIf

    // Anzahl und Wert des Items selbst
    var res := struct;
    res.+items := 1;
    res.+worth := GetWorth(obj, amount_override);

    // bei einem Container auch den Inhalt auswerten
    If (obj.isa(POLCLASS_CONTAINER))
        ForEach item in (EnumerateItemsInContainer(
                obj, ENUMERATE_IGNORE_LOCKED
        ))
            SleepMs(2);
            res.items += 1;
            res.worth += GetWorth(item, amount_override);
        EndForEach
    EndIf

    return res;
EndFunction // }}}

Function GetWorth(obj, amount_override := 0) // {{{
    If (obj.objtype == UOBJ_GOLD_COIN)
        If (amount_override)
            return amount_override;
        EndIf
        // Goldwert direkt zaehlen (hat keinen Buyprice)
        return CDbl(obj.amount);
    EndIf
    return CDbl(Max(MIN_WORTH, GetPropertyPrice(obj)));
EndFunction // }}}
