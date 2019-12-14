//Function GetArgument( text , argNbr )
//    var argArray := SplitWords( text );
//    If ( argNbr >= 1 AND argNbr <= len( argArray ) )
//        return argArray[ argNbr ];
//    Else
//        return "";
//    EndIf
//
//EndFunction

Function IsLocationAccessible(character, x, y, z) // {{{
    var xd := character.x - x;
    If (xd < -1 or xd > 1)
        return 0;
    EndIf

    var yd := character.y - y;
    If (yd < -1 or yd > 1)
        return 0;
    EndIf

    var zd := character.z - z;
    If (zd < -10 or zd > 10)
        return 0;
    EndIf

    return CheckLosAt(character, x, y, z);
EndFunction // }}}

// prüft, ob Item in angegebenen Toplevel-Containern ist
// liefert true/error
Function InContainer(item, top_containers) // {{{
    // Container-Serials ermitteln
    var top_serials := array{};
    ForEach cont in (top_containers)
        top_serials.append(cont.serial);
        SleepMS(2);
    EndForEach

    // Top-Level-Container des Items suchen
    var is_in := item.container;
    While (is_in.container)
        is_in := is_in.container;
        SleepMS(2);
    EndWhile

    // Test, ob in einem Toplevel-Container
    If (is_in.serial in top_serials)
        return 1;
    EndIf

    // Fehlermeldung bauen
    var fail := error;
    fail.errortext := "That is not your item!";
    return fail;
EndFunction // }}}

// prüft, ob Items des Typs prinzipiell stacken
// liefert true/false oder error bei Fehler
Function Stackable(item) // {{{
    // falls es ein Stapel ist, wird es schon stacken
    If (item.amount>1)
      return 1;
    EndIf

    // ohne genauen Test davon ausgehen, dass es eh nicht stackt
    If (
      item.isa(POLCLASS_MOBILE) or
      item.isa(POLCLASS_LOCKABLE) or
      item.isa(POLCLASS_MULTI)
    )
      return 0;
    EndIf

    // im Zweifel wird es im Knast ausprobiert
    var desc := GetItemDescriptor(item.objtype);
    // damit es nicht so auffaellt
    desc.invisible := 1;

    // mal gucken, ob wir einen Stack erzeugen können
    var test :=
        CreateItemAtLocation(5280, 1190, 0, desc, 2, REALM_BRITANNIA);
    If (test)
        // Stapel war erzeugbar, beseitigen und gut
        DestroyItem(test);
        return 1;
    EndIf

    If (test.errortext == "That item is not stackable. Create one at a time.")
        // oder eben nicht gestapelt
        return 0;
    EndIf

    // sonstige Fehler durchreichen
    return test;
EndFunction // }}}
