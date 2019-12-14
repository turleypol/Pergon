// ab wievielen Morden wird man vogelfrei
Const MAX_MURDERS            := 5;
// wann verfallen die Zaehler?
Const LONG_COUNT_DECAY_TIME  := 144000; // 40 hours
Const SHORT_COUNT_DECAY_TIME := 28800;  //  8 hours

include "include/place";

// Dialog, um Moerder zu melden, anzeigen; markierte Moerder aktualisieren
Function SendReportGump(who) // {{{
  // Basis-Checks {{{
  If (who.reportables.size() <= 0)
    // gibt gar nichts zu melden
    return;
  EndIf

  // Darf man ueberhaupt?
  If (
    // ist PK oder
    who.getprop(TYPKILLER) or
    // vogelfrei
    who.getprop("longmurders") >= MAX_MURDERS
  )
    // sind gesetzlos und duerfen daher nicht flaggen
    ClearReportables(who);
    return;
  EndIf

  var punish := who.getprop(PROP_PUNISH_TYPE);
  If (punish and "Tod" == punish)
      // bei Todesstrafe ausfuehrenden GM nicht anschwaerzen
      return;
  EndIf

  If (PlaceName(who)["Nirdana"])
    // im Stadtgebiet von Nirdana sind Mord und Totschlag erlaubt
    return;
  EndIf
  // }}}

  // Gump-Daten {{{
  var layout := {
    "page 0",
    "nodispose",
    "gumppic 0 0 206",
    "gumppic 43 0 201",
    "gumppic 460 0 207",
    "gumppic 0 43 202",
    "gumppic 43 43 200",
    "gumppic 170 43 200",
    "gumppic 295 43 200",
    "gumppic 335 43 200",
    "gumppic 43 170 200",
    "gumppic 170 170 200",
    "gumppic 295 170 200",
    "gumppic 335 170 200",
    "gumppic 43 220 200",
    "gumppic 170 220 200",
    "gumppic 295 220 200",
    "gumppic 335 220 200",
    "gumppic 460 43 203",
    "gumppic 0 340 204",
    "gumppic 43 340 233",
    "gumppic 460 340 205",
    "button 380 345 249 248 1 0 0",
    "text 50 20 40 0",
    "text 50 50 40 1",
    "text 50 70 40 2",
    "text 50 90 40 3",
    "page 1"
  };

  var data := {
    "Nun ist die Zeit der Vergeltung gekommen!",
    "Markiert keinen oder mehrere Player, um sie als Moerder zu melden",
    "und drueckt OKAY. Solltet ihr ein Item in der Hand halten, legt",
    "es zuvor in euer Backpack."
  };
  // }}}

  // Liste potentieller Moerder aufbauen (max. 20 - nicht ganz sauber) {{{
  var akt_x       := 40;
  var akt_y       := 100;
  var buttonid    := 10;
  var donekillers := array{};
  ForEach reportable in (who.reportables)
    If (reportable.killer.serial in (donekillers))
      // keine Doppeleintragungen erlauben
      continue;
    EndIf

    If (buttonid == 21)
      akt_x := 300;
      akt_y := 80;
    EndIf

    donekillers.append(reportable.killer.serial);
    data.append(reportable.killer.name);
    layout.append("checkbox "+akt_x+" "+(akt_y+20)+" 210 211 0 "+buttonid);
    layout.append("text "+(akt_x+20)+" "+(akt_y+20)+" 40 "+(data.size()-1));

    akt_y += 20;
    buttonid += 1;
  EndForEach
  // }}}

  // Gump aufrufen und gedrueckte CheckBoxen abarbeiten {{{
  ForEach buttonid in (SendDialogGump(who, layout, data).keys)
    If (buttonid >= 10) // IDs der CheckBoxen
      // Character als Moerder melden
      var char := who.reportables[buttonid-9];
      If (char)
        SendSysMessagePergon(char.killer,
          "Ihr wurdet als Moerder gemeldet.", "", _DEFAULT_TEXT_FONT, 38
        );
        AdjustMurderCounts(char.killer);
      EndIf
    EndIf
  EndForEach
  // }}}

  ClearReportables(who);
EndFunction // }}}

// Moerderliste bereinigen
Function ClearReportables(who) // {{{
  ForEach reportable in (who.reportables)
    who.removereportable(reportable.serial, reportable.gameclock);
  EndForEach
EndFunction // }}}

// Moerderzaehler und Timeouts der Person aktualisieren
// TODO: ping pong counter
Function AdjustMurderCounts(who) // {{{
  var shortcount := who.getprop("shortmurders");
  If (!shortcount)
    shortcount := 0;
  EndIf
  shortcount += 1;
  who.setprop("shortmurders", shortcount);

  var longcount := who.getprop("longmurders");
  If (!longcount)
    longcount := 0;
  EndIf
  longcount += 1;
  who.setprop("longmurders", longcount);

  If (longcount >= MAX_MURDERS)
    who.setmurderer(1);
    SendSysMessagePergon(who,
      "Ihr seid nun vogelfrei!", "", _DEFAULT_TEXT_FONT, 38
    );
  EndIf

  var onlinetime := who.getprop("onlinetimer");
  who.setprop("decaylongcountat",  onlinetime+LONG_COUNT_DECAY_TIME);
  who.setprop("decayshortcountat", onlinetime+SHORT_COUNT_DECAY_TIME);
EndFunction // }}}

// Items aus Leiche verschieben, ausser Haare und Baerte
// Entspricht der MoveItems-Fkt. in Sebbs Magiescripten
Function MoveCorpseItems(oldcont, newcont) // {{{
  var ignorelist := {
    // Mensch
    0x2004, 0x203b, 0x203c, 0x203d, 0x2044, 0x2045, 0x2046, 0x2047,
    0x2048, 0x2049, 0x204a,
    // Elf männlich extra Haare
    0x2fbf, 0x2fc0, 0x2fc1, 0x2fc2, 0x2fcd, 0x2fce, 0x2fcf, 0x2fd1,
    // Elf weiblich extra Haare
    0x2fcc, 0x2fd0,
    // Bärte
    0x2005, 0x203e, 0x203f, 0x2040, 0x2041, 0x204b, 0x204c, 0x204d,
    // Mountitem
    0xf021
  };

  ForEach item in EnumerateItemsInContainer(oldcont, ENUMERATE_ROOT_ONLY)
    If (item.objtype in ignorelist)
      // Haare, Bart, etc. ausnehmen
      continue;
    EndIf
    If (item.movable)
      // Item bewegen
      MoveItemToContainer(item, newcont, item.x, item.y);
    Else
      // sonst Status aendern, bewegen und wieder setzen
      item.movable := 1;
      MoveItemToContainer(item, newcont, item.x, item.y);
      item.movable := 0;
    EndIf
  EndForEach
EndFunction // }}}

// vim: sw=2 sts=2
