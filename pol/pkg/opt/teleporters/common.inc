///////////////////////////////////////////////////////////////////////////
// Include fuer Scripte walkonaction-Scripte

use cfgfile;
use os;
use polsys;
use uo;
include "include/logutil";
include "include/mobile";
include "include/modifyskill"; // wegen SKKLASSE-Konstante
include "include/pergonutil";  // wegen SendYesNoGump


Const CFGFILE_TELE    := ":teleporters:teleport";
Const PROP_WALKSCRIPT := "walkscript";
Const PROP_CMD_AFFECT := "affectcmdlevel";

// fuer blockfrost/checkfrost
Const PROP_FROST_DUNG := "frost_ok";

// fuer telearena/telenomagic/telemagic/telecommonsetup etc.
Const PROP_TELEP_DEST := "dest";
Const ETELEBROKENDEST := "Teleport destination is broken";

// fuer Stepback
Const PROP_STONED     := "#walkonstoned";

// fuer SpecialTeleChecks
Const TELE_ALLOW      := 1;
Const TELE_DENY       := 0;
Const ERR_DE_NONNAKED := "Eure Besitztuemer koennt ihr in dieser Welt "+
    "nicht nutzen, lasst sie hier.";
Const ERR_EN_NONNAKED := "Your possessions are of no use in this world. "+
    "Leave them here.";

// fuer Returngates
// CProp an Tor - was ist das Ziel (serial)
Const DEST_CPROP    := "destination_serial";
// startet das Tor Teleportvorgaenge beim Betreten
Const ONOFF_CPROP   := "enabled";

// Spezialoptions-CProp
Const PROP_GATE_SPECIAL := "options";
// fuer allgemeine Spezialoptionen an Toren (Struct-Keys)
Const GATE_SPC_ASK        := "a"; // Frage vor Teleportation
Const GATE_SPC_NEWBIES    := "j"; // Jhe'lomer verbieten
Const GATE_SPC_NONNEWBIES := "f"; // Festlaender verbieten
Const GATE_SPC_CLASSES    := "c"; // erlaubte Berufsklassen
                                  // Array{KLASSE_KRIEGER, KLASSE_MAGIER} ...
Const GATE_SPC_NOMOUNT    := "m"; // nur ohne Reittier
Const GATE_SPC_NAKED      := "n"; // nur nackt (auch keine Items)
Const GATE_SPC_ENERGY     := "e"; // max. Anzahl der Durchgaenge
Const GATE_SPC_EREGENTIME := "r"; // Regenerationszeit fuer Durchgaenge
                                  // (in Sekunden)
Const GATE_SPC_EDESTRUCT  := "d"; // Zerstoerung statt Regeneration?
Const GATE_SPC_RETURNGATE := "g"; // Tor zur Rückkehr

Const PROP_CURRENT_ENERGY := "#energy";  // aktueller Energiewert
Const PROP_LAST_USE       := "#lastuse"; // letzte Benutzung fuer Regen.

// <who> einen Schritt hinten den I-Stein setzen
Function StepBack(p, steps := 1) // {{{
    var dist := CoordinateDistance(p.last.x, p.last.y, p.item.x, p.item.y);
    If (
        // mehr als ein Schritt zurueck
        (steps > 1) or
        // Realmwechsel
        (p.last.x == 0 and p.last.y == 0 and p.last.z == 0) or
        // oder draufteleportiert
        (dist <> 1) or
        // oder von anderem Stein auf diesen geschubst worden
        (GetObjProperty(p.who, PROP_STONED) != p.item.serial)
    )
        SetObjProperty(p.who, PROP_STONED, p.item.serial);
        // Koordinaten aus Blickrichtung raten
        var xmul := ConvertDirectionToLocX(p.who.facing);
        var ymul := ConvertDirectionToLocY(p.who.facing);
        While (steps > 1)
            SleepMs(2);
            If (MoveObjectToLocation(
                p.who,
                p.item.x - steps*xmul,
                p.item.y - steps*ymul,
                p.item.z,
                p.item.realm
            ))
                break;
            EndIf
            steps -= 1;
        EndWhile

        If (steps == 1)
            MoveObjectToLocation(
                p.who,
                p.item.x - xmul,
                p.item.y - ymul,
                p.item.z,
                p.item.realm
            );
        EndIf
    ElseIf (dist > 0)
        SetObjProperty(p.who, PROP_STONED, p.item.serial);
        // noch im selben Realm, auf vorherige Position setzen
        MoveObjectToLocation(
            p.who,
            p.last.x, p.last.y, p.last.z, p.item.realm
        );
    EndIf

    Sleep(1);
    var oldcrit := Is_Critical();
    Set_Critical(1);
    // falls wir noch der zuletzt benutzte I-Stein sind, Property loeschen
    If (GetObjProperty(p.who, PROP_STONED) == p.item.serial)
        EraseObjProperty(p.who, PROP_STONED);
    EndIf
    Set_Critical(oldcrit);
EndFunction // }}}

// prueft grob die Gueltigkeit des Teleporterziels
Function GetTeleDest(tele) // {{{
    var dest := GetObjProperty(tele, PROP_TELEP_DEST);
    If (
        // ueberhaupt gesetzt und passender Typ?
        dest == error or TypeOfInt(dest) != OT_STRUCT or
        // gültige Realm
        !Realms(dest.realm) or
        // gültige Koords
        dest.x < 0 or dest.x > Realms(dest.realm).width or
        dest.y < 0 or dest.y > Realms(dest.realm).height or
        dest.z < -128 or dest.z > 127 or
        // wird immer gesetzt, aber nicht immer genutzt
        dest.facing < 0 or dest.facing > 7
    )
        syslog(
            "FEHLER: "+ItemInfoStr(tele, COORDS_REALM)+
            " hat fehlerhafte Zielkoordinaten!"
        );
        var ret := error;
        error.errortext := ETELEBROKENDEST;
        return ret;
    EndIf

    return dest;
EndFunction // }}}

// Sonderrestriktionen, falls bestimmte CProps gesetzt sind
Function SpecialTeleChecks(gate, who) // {{{
    // gucken, ob bereits initialisiert
    var onoff := GetObjProperty(gate, ONOFF_CPROP);
    If (onoff.errortext)
        SetObjProperty(gate, ONOFF_CPROP, 1);
    EndIf
    If (onoff == 0)
    	If (who.cmdlevel < CMDLEVEL_QUESTCHAR and (gate.graphic == 1 or gate.invisible))
    		return;
    	EndIf
        SendSysMessagePergon(who,
            "Dieses Tor ist derzeit ausser Betrieb.",
            "This gate is currently out of order."
        );
        return;
    EndIf

    // Staff darf immer
    If (who.cmdlevel >= CMDLEVEL_QUESTCHAR)
        return TELE_ALLOW;
    EndIf

    // ueberhaupt Spezialeinstellungen vorhanden?
    var special := gate.getprop(PROP_GATE_SPECIAL);

    If (special == error or TypeOfInt(special) != OT_STRUCT)
        // keine Spezialeinstellungen oder falsches Format
        return TELE_ALLOW;
    EndIf

    // Newbies dürfen nicht durch
    If (special[GATE_SPC_NEWBIES])
        If (who.getprop(TYPNEWBIE))
            SendSysMessagePergon(who,
                "Dieses Tor ist nicht fuer Neulinge bestimmt.",
                "This portal is not for newbies."
            );
            return TELE_DENY;
        EndIf
    EndIf

    // Festlandbewohner dürfen nicht durch
    If (special[GATE_SPC_NONNEWBIES])
        If (!who.getprop(TYPNEWBIE))
            SendSysMessagePergon(who,
                "Dieses Tor ist nicht fuer erfahrene Leute bestimmt.",
                "This portal is not for experienced folks."
            );
            return TELE_DENY;
        EndIf
    EndIf

    If (special[GATE_SPC_CLASSES])
        var allowed := who.getprop(SKKLASSE);
        // nur, wenn mind. 1 Klasse erlaubt und Char-Klasse gesetzt
        If (special[GATE_SPC_CLASSES].size() > 0 and allowed)
            // falsche Klasse?
            If (!(allowed in (special[GATE_SPC_CLASSES])))
                SendSysMessagePergon(who,
                    "Eure Klasse verbietet den Durchgang.",
                    "This teleport is not for your class."
                );
                return TELE_DENY;
            EndIf
        EndIf
    EndIf

    // nur ohne Reittier
    If (special[GATE_SPC_NOMOUNT])
        var mount := GetEquipmentByLayer(who, LAYER_MOUNT);
        If (mount)
            SendSysMessagePergon(who,
                "Euer Reittier koennt ihr in dieser Welt "+
                "nicht nutzen, lasst es hier.",
                "Your mount cannot be used in this world. "+
                "Leave it here."
            );
            return TELE_DENY;
        EndIf
    EndIf

    // Spezialteleporter lässt nur ein Kleidungsstück zu und Newbieitems
    If (special[GATE_SPC_NAKED])
        var count := 0;
        ForEach item in (ListEquippedItems(who))
            // Haare etc. ignorieren
            If (item.layer in (
                array{LAYER_HAIR, LAYER_BEARD, LAYER_PACK, LAYER_MOUNT}
            ))
                continue;
            EndIf
            count += 1;
        EndForEach
        If (count > 1)
            SendSysMessagePergon(who, ERR_DE_NONNAKED, ERR_EN_NONNAKED);
            return TELE_DENY;
        EndIf

        ForEach item in (EnumerateItemsInContainer(who.backpack))
            If (
                // Newbie-Items oder ...
                (item.newbie) or
                // Spellpanel
                (item.objtype == 0x682f)
            )
                continue;
            Else
                SendSysMessagePergon(who, ERR_DE_NONNAKED, ERR_EN_NONNAKED);
                return TELE_DENY;
            EndIf
        EndForEach
    EndIf

    // kurz vor dem Ende, um moeglichst wenige unnoetige Rueckfragen
    // auszuloesen
    // Tor mit Rueckfrage (z. B. fuer gefaehrliche Ziele)
    If (special[GATE_SPC_ASK])
        If (!SendYesNoGump(who,
                "Tor wirklich benutzen?<br>"+
                "Vergesst nie: Auf der anderen Seite koennten grosse "+
                "Gefahren auf Euch lauern!"
        ))
            SendSysMessagePergon(who, "Zieht Eurer Wege!", "Godspeed!");
            return TELE_DENY;
        EndIf
    EndIf

    // am Ende, weil es Werte aendert
    If (special[GATE_SPC_ENERGY])
        var currentenergy := CInt(gate.getprop(PROP_CURRENT_ENERGY));
        var lastuse       := CInt(gate.getprop(PROP_LAST_USE));
        // Regenerationszeit abgelaufen?
        If (lastuse + special[GATE_SPC_EREGENTIME] < ReadGameClock())
            // verbrauchte Energie zuruecksetzen
            currentenergy := 0;
        EndIf
        gate.setprop(PROP_LAST_USE, ReadGameClock());

        If (currentenergy >= special[GATE_SPC_ENERGY])
            // Energie verbraucht
            If (special[GATE_SPC_EDESTRUCT])
                // Zerstoerung vorgesehen?
                SendSysMessagePergon(who,
                    "Die Energie des Tores ist erschoepft. Es erlischt.",
                    "You drained the energy of the portal. It disappears."
                );
                PlayStationaryEffect(
                    gate.x, gate.y, gate.z, FX_SMOKE, 0xa, 0xa, 0, gate.realm
                );
                DestroyItem(gate);
                // Teleport wird ggf. noch erlaubt
            Else
                SendSysMessagePergon(who,
                    "Die Energie des Tores ist erschoepft. "+
                    "Wartet, bis es regeneriert ist.",
                    "You drained the energy of the portal. "+
                    "It will take time to regenerate."
                );
                return TELE_DENY;
            EndIf
        Else
            // Energie noch nicht verbraucht -> verbrauchen
            gate.setprop(PROP_CURRENT_ENERGY, 1+currentenergy);
        EndIf
    EndIf

    return TELE_ALLOW;
EndFunction // }}}

// Konfiguriert I-Stein, Auswahl als ConfigElemRef oder Int uebergeben
Function BasicSetup(item, choice) // {{{
    // ggf. Konfiguration lesen
    If (TypeOfInt(choice) != OT_CONFIGELEMREF)
        var cfgfile := ReadConfigFile(CFGFILE_TELE);
        If (cfgfile == error)
            return cfgfile;
        EndIf
        choice := FindConfigElem(cfgfile, choice);
        If (choice == error)
            return cfgfile;
        EndIf
    EndIf

    // Pflichtwerte
    If (GetConfigInt(choice, "Visible"))
        item.invisible := 0;
    Else
        item.invisible := 1;
    EndIf

    var desc := GetConfigString(choice, "Desc");
    If (desc)
        If (item.invisible)
            SetName(item, desc+" (unsichtbar)");
        Else
            SetName(item, desc);
        EndIf
    EndIf

    var walkon := GetConfigString(choice, "WalkOnScript");
    If (walkon == error)
        syslog("FEHLER: 'WalkOnScript' nicht definiert!");
        return walkon;
    EndIf
    SetObjProperty(item, PROP_WALKSCRIPT, walkon);

    // optionale Einstellungen
    If (GetConfigInt(choice, "AffectCmdLevel"))
        SetObjProperty(item, PROP_CMD_AFFECT, 1);
    Else
        SetObjProperty(item, PROP_CMD_AFFECT, 0);
    EndIf

    var color := GetConfigInt(choice, "Color");
    If (color != error)
        item.color := color;
    Else
        item.color := GetItemDescriptor(item.objtype).color;
    EndIf

    var graphic := GetConfigInt(choice, "Graphic");
    If (graphic != error)
        item.graphic := graphic;
    Else
        item.graphic := GetItemDescriptor(item.objtype).graphic;
    EndIf

    var script := GetConfigString(choice, "Script");
    If (script != error)
        item.usescript := ":teleporters:"+script;
    Else
        item.usescript := "";
    EndIf

    return walkon;
EndFunction // }}}
