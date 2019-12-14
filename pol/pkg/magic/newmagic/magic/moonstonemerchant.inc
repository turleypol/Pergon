///////////////////////////////////////////////////////////////////////////
// MoonstoneMerchant Funktionen v1.0 (2005/01/19)
//
// Author: Bihinos

include "::include/moongate";
include ":runebook:rune";

//////////////////////
// Gump-Definitionen
//////////////////////
Function MoonStonePrices()
    return {
        6000,   18000,  30000,  60000,  78000,
        90000, 102000, 120000, 138000, 150000
    };
EndFunction

Function MoonStoneCharges()
    return {
           1,       3,      5,     10,     13,
          15,      17,     20,     23,     25
    };
EndFunction

Const GUMP_LAYOUT := 0;
Const GUMP_DATA   := 1;
Function MoonStoneGump(type := GUMP_LAYOUT)
    If (type == GUMP_LAYOUT)
        return {
            "page 0", // Seite 0 (Basisseite)
            "nodispose",
            "resizepic 0 0 9380 480 250",
            "",
            // Frage
            "text 85 5 66 0",
            "text 60 50 40 1",

            "button 30 90 2640 2641 1 0 1",
            "htmlgump 65 94 100 20 2 0 0",
            "htmlgump 165 94 80 20 12 0 0",
            "button 30 115 2640 2641 1 0 2",
            "htmlgump 65 119 100 20 3 0 0",
            "htmlgump 165 119 80 20 13 0 0",
            "button 30 140 2640 2641 1 0 3",
            "htmlgump 65 144 100 20 4 0 0",
            "htmlgump 165 144 80 20 14 0 0",
            "button 30 165 2640 2641 1 0 4",
            "htmlgump 65 169 100 20 5 0 0",
            "htmlgump 165 169 80 20 15 0 0",
            "button 30 190 2640 2641 1 0 5",
            "htmlgump 65 194 100 20 6 0 0",
            "htmlgump 165 194 80 20 16 0 0",

            "button 255 90 2640 2641 1 0 6",
            "htmlgump 290 94 100 20 7 0 0",
            "htmlgump 375 94 80 20 17 0 0",
            "button 255 115 2640 2641 1 0 7",
            "htmlgump 290 119 100 20 8 0 0",
            "htmlgump 375 119 80 20 18 0 0",
            "button 255 140 2640 2641 1 0 8",
            "htmlgump 290 144 100 20 9 0 0",
            "htmlgump 375 144 80 20 19 0 0",
            "button 255 165 2640 2641 1 0 9",
            "htmlgump 290 169 100 20 10 0 0",
            "htmlgump 375 169 80 20 20 0 0",
            "button 255 190 2640 2641 1 0 10",
            "htmlgump 290 194 100 20 11 0 0",
            "htmlgump 375 194 80 20 21 0 0"
        };
    ElseIf (type == GUMP_DATA)
        var charges := MoonStoneCharges();
        return {
            "Was fuer einen Mondstein moechtet Ihr kaufen?",
            "Mondstein mit:",
            "<p align=left>"+charges[1]+" Ladung</p>",
            "<p align=left>"+charges[2]+" Ladungen</p>",
            "<p align=left>"+charges[3]+" Ladungen</p>",
            "<p align=left>"+charges[4]+" Ladungen</p>",
            "<p align=left>"+charges[5]+" Ladungen</p>",
            "<p align=left>"+charges[6]+" Ladungen</p>",
            "<p align=left>"+charges[7]+" Ladungen</p>",
            "<p align=left>"+charges[8]+" Ladungen</p>",
            "<p align=left>"+charges[9]+" Ladungen</p>",
            "<p align=left>"+charges[10]+" Ladungen</p>"
        };
    EndIf
EndFunction

///////////////////////////////////////////////////////////////
// Speech_Mondstein - Jemand hat zum NPC "mondstein" gesagt
///////////////////////////////////////////////////////////////
Function Speech_Mondstein(me, medata, ev)
    var loc := PlaceName(me);
    If (!MoonPics().exists(Lower(loc)))
        // Bin ich an einem legalen Ort?
        NPCSayTextTraditional(me,
            "Ich habe leider keine Mondsteine fuer "+loc+".", ""
        );
        return;
    EndIf
    var layout := MoonStoneGump(GUMP_LAYOUT);
    layout[4] := "gumppic 390 30 "+MoonPics()[Lower(loc)];

    var data := MoonStoneGump(GUMP_DATA);
    ForEach price in (MoonStonePrices())
        data.append(
            "<basefont color=#404040><p align=right>"+price + " gp</p>"
        );
    EndForEach

    var code := SendDialogGump(ev.source, layout, data, 200, 200)[0];

    var charges := MoonstoneCharges();
    If ((code) && (charges[code]))
        If (!GetPayment(me, ev.source, MoonStonePrices()[code]))
            NPCSayTextTraditional(me, "Hier ist Euer Mondstein.", "");

            var stone := CreateItemInBackpackPergon(
                ev.source, 0x685a, charges[code]
            );
            If (stone)
                SetName(stone, loc+"Mondstein%e");
                SetObjProperty(stone, "location", loc);
            Else
                NPCSayTextTraditional(me, "Da ist irgendwas schiefgegangen.", "");
                syslog(
                    "Warnung: Konnte Moonstone fuer "+loc+" nicht erzeugen"
                );
            EndIf
        EndIf

        return (1); // ignore speechconfig
    EndIf

    return;
    medata := 1;
EndFunction
