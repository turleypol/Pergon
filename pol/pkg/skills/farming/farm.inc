// Liste von Serials der Items, die zum Komposthaufen gehoeren
Const PROP_LIST       := "HeapItems";
// Verweis auf das Controlitem an den anderen Komposthaufenitems
Const PROP_CONTROLLER := "Controller";

Function UseWaterVessel(char, water := 2)
    var cfg := readconfigfile("::itemdesc");
    var gefaessliste := {
        0x0e7b, // kl. Wasserfass
        0x0ffa, // Eimer
        0x154d  // gr. Wasserfass
    };

    var ok := 0;
    // Backpack nach Wasser-Gefaess durchsuchen
    ForEach objtype in (gefaessliste)
        ForEach Gefaess in (FindSubstance(
            char.backpack, objtype, 1, 0, FINDSUBSTANCE_FIND_ALL
        ))
            var filled := GetObjProperty(gefaess, "filled");
            If (filled < water)
                // zu leer, ggf. weitere Gefaesse probieren
                continue;
            EndIf
            If (!ReserveItem(Gefaess))
              continue;
            EndIf

            // 2 Liter fuer den Samen
            filled -= water;
            PlaySoundEffect(char, 0x12);
            Sleep(1);

            If (filled >= 0.2)
                SetObjProperty(Gefaess, "filled", filled);
                ok := 1;
                ReleaseItem(Gefaess);
                break;
            EndIf

            // falls nun so gut wie leer, Gefaess ersetzen
            var gefaess_cfg := cfg[gefaess.objtype];
            If (!gefaess_cfg.targetitem)
                syslog(
                    "WARNUNG: Gefaess "+gefaess.objtype+
                    " hat kein Targetitem in der cfg!"
                );
            EndIf
            ok := 1;
            var werkzeug := GetObjProperty(Gefaess, "Werkzeug");
            DestroyItem(gefaess);
            var new_gefaess := CreateItemInBackpackPergon(
                char, Gefaess_cfg.targetitem, 1
            );
            new_gefaess.movable := 1;
            SetObjProperty(new_Gefaess, "Werkzeug", werkzeug);
            ReleaseItem(Gefaess);
            IncRevision(new_gefaess);
            break;
        EndForEach
        If (ok)
            break;
        EndIf
    EndForEach

    return ok;
EndFunction
