///////////////////////////////////////////////////////////////////////////
// importexport - fuer Import und Export von Items

include ":housing:house";
include "include/server";
include "include/storage";
include "include/varutil";

// fuer Statics-Export/-Import
const SUFFIX_STATIC  := "statics";
const SUFFIX_MOVABLE := "items";
// legt Namen fuer Statics-Dateien fest
Function FileStatics(prefix := "") // {{{
    If (prefix)
        prefix += "-";
    EndIf
    return "statics/"+prefix+SUFFIX_STATIC;
EndFunction // }}}

    // legt Namen fuer Datei mit nicht-statischen Items fest
Function FileMovable(prefix := "") // {{{
    If (prefix)
        prefix += "-";
    EndIf
    return "statics/"+prefix+SUFFIX_MOVABLE;
EndFunction // }}}

// fuer Export von Hausinhalten
const PROP_PO_PACKINFO := "PPI";
const PO_AMOUNT        := "a";

// einen Bereich einpacken; bei Haus relativ zu Hauskoordinaten
Function PlayerObjectPackArea(who, pos1, pos2, boxbasename) // {{{
    // Haus-Bezugspunkt holen
    var house := GetHouseContainingObject(who);

    // Bereich sortieren
    If (pos1.x > pos2.x) SwapValues(pos1.x, pos2.x); EndIf
    If (pos1.y > pos2.y) SwapValues(pos1.y, pos2.y); EndIf

    var items := ListObjectsInBox(
        pos1.x, pos1.y, 127, pos2.x, pos2.y, -128, pos1.realm
    );

    PlayerObjectPack(items, boxbasename, house);
EndFunction // }}}

// alles im Umkreis einpacken
Function PlayerObjectPackRange(who, range, boxbasename) // {{{
    var items := ListItemsNearLocation(who.x, who.y, who.z, range, who.realm);
    PlayerObjectPack(items, boxbasename);
EndFunction // }}}

// eigentliche Einpackfunktion
Function PlayerObjectPack(items, boxbasename, relative_to := 0) // {{{
    // Leerzeichen machen das Wiederherstellscript unnoetig kompliziert
    boxbasename := StrReplace(boxbasename, " ", "_");

    // Storagebox besorgen
    var boxnumber := 0;
    var area := FindOrCreateStorage(STORAGE_HOUSE);
    var name := boxbasename+"_"+boxnumber;
    var box := FindOrCreateRootItemInStorage(area, name);

    // alle Items durchgehen
    ForEach item in (items)
        SleepMs(2);
        If (item in (house.components))
            // Multikomponenten nicht einpacken
            continue;
        EndIf

        // GM-festgemachte Sachen behandeln?
        var is_house_container := item.getprop(PROP_HS_C_SC_BOX);
        // If (!item.getprop(PROP_HS_C_FIXED) and !is_house_container)
        //     // mach was
        // EndIf

        If (is_house_container)
            var container_house := item.getprop(PROP_HS_C_SC_HOUSE);
            // durch innere Box ersetzen
            item := SystemFindObjectBySerial(is_house_container);
            If (!item)
                // defekter Hauscontainer?
                continue;
            EndIf
            // diese markieren
            item.setprop(PROP_HS_C_SC_HOUSE, container_house);
        EndIf

        // losmachen
        var movable := item.movable;
        item.movable := 1;

        var info := struct;
        If (!item.container)
            If (!relative_to)
                relative_to := struct{x := 0, y := 0, z := 0};
            EndIf
            // Weltkoordinaten relativ zu rel_coord sichern (damit man
            // es spaeter automatisch in gleichen Haustyp entpacken kann)
            info.+x := item.x - relative_to.x;
            info.+y := item.y - relative_to.y;
            info.+z := item.z - relative_to.z;
            item.setprop(PROP_PO_PACKINFO, info);
        EndIf

        // Items zusammenstauchen
        If (!item.isa(POLCLASS_CONTAINER))
            // kein Container, ggf. Amount reduzieren und fertig
            PlayerObjectReduceAmount(item, info);
        Else
            // bei Containern Inhalt bearbeiten
            ForEach content in (
                EnumerateItemsInContainer(item, ENUMERATE_IGNORE_LOCKED)
            )
                SleepMs(2);
                PlayerObjectReduceAmount(content);
            EndForEach
        EndIf

        // Move in Storage
        var result := MoveItemToContainer(item, box);
        If (result.errortext == "Container is too full to add that")
            // voll, weitere Kiste erzeugen
            var oldbox := box;
            boxnumber += 1;
            name := boxbasename+"_"+boxnumber;
            box  := FindOrCreateRootItemInStorage(area, name);
            // verketten
            oldbox.setprop("next", box.serial);
            box.setprop("last", oldbox.serial);
            result := MoveItemToContainer(item, box);
        EndIf
        If (!result)
            syslog(
                ItemInfoStr(item, COORDS_REALM)+
                " liess sich nicht verpacken: "+result.errortext;
            );
        EndIf

        // los/fest-Status wiederherstellen
        item.movable := movable;
    EndForEach

    // Name der Storage-Kiste zurueckgeben
    return name;
EndFunction // }}}

// Spieleritems in Landschaft wiederherstellen
// FIXME: Hauscontainer, relativ vs. automatisch plazieren
Function PlayerObjectUnpack(item, dest, relative_to := 0, force := MOVEOBJECT_NORMAL) // {{{
    var info := item.getprop(PROP_PO_PACKINFO);
    If (info == error)
        // absichtlich nur Error, damit ggf. auch leere Props geloescht werden
        return;
    EndIf

    // erst Amount korrigieren, dann verschieben, damit Clients in
    // Umgebung der Wiederherstellung weniger Daten empfangen müssen
    PlayerObjectRestoreAmount(item);

    // wieder auf die Welt holen
    If (info.x and info.y)
        If (relative_to)
            // bei relativer Wiederherstellung im Haus ist force Pflicht,
            // sonst fallen Items ggf. zu Boden
            force := force ^ MOVEOBJECT_FORCELOCATION;
            info.x += dest.x; info.y += dest.y; info.z += dest.z;
        Else
            info.x := dest.x; info.y := dest.y; info.z := dest.z;
        EndIf
        // Bewegbarkeit immer ignorieren
        force := force ^ MOVEITEM_IGNOREMOVABLE;
        MoveObjectToLocation(item, info.x, info.y, info.z, dest.realm, force);
    EndIf

    // Info wegwerfen
    item.eraseprop(info);

    If (!item.isa(POLCLASS_CONTAINER))
        // kein Container, fertig
        return;
    EndIf

    // bei Containern Inhalt bearbeiten
    ForEach content in (
        EnumerateItemsInContainer(item, ENUMERATE_IGNORE_LOCKED)
    )
        SleepMs(2);
        PlayerObjectRestoreAmount(content);
    EndForEach
EndFunction // }}}

// bei Items mit amount>1 diesen wegsichern und reduzieren
// (dann passt mehr davon in eine Kiste in StorageArea)
Function PlayerObjectReduceAmount(item, info := 0) // {{{
    If (item.amount <= 1)
        // Amount ist schon minimal
        return;
    EndIf

    If (!info)
        info := struct;
    EndIf
    // alte Menge merken und auf 1 reduzieren
    var orig_amount := item.amount;
    If (SubtractAmount(item, orig_amount - 1))
        // bei Erfolg Daten fuer spaeteres Entpacken merken
        info[PO_AMOUNT] := orig_amount;
        item.setprop(PROP_PO_PACKINFO, info);
    EndIf
EndFunction // }}}

// Items mit reduziertem Amount wiederherstellen
Function PlayerObjectRestoreAmount(item) // {{{
    var info := item.getprop(PROP_PO_PACKINFO);
    If (!info)
        return;
    EndIf

    var orig_amount := CInt(info[PO_AMOUNT]);
    If (orig_amount)
        // keine Fehlerbehandlung, Wahrscheinlichkeit fuer in_use ist gering
        AddAmount(item, orig_amount - 1);
        If (info.x)
            // da sind noch Koordinaten gesichert, nur Amount entfernen
            info.erase(PO_AMOUNT);
            info.setprop(PO_AMOUNT, info);
        Else
            // keine Koordinaten, Prop entsorgen
            info.eraseprop(PO_AMOUNT);
        EndIf
    EndIf
EndFunction // }}}
