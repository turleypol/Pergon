///////////////////////////////////////////////////////////////////////////
// allgemeines Include fuer Spiele

Const PROP_CONCEAL_CONFIG := "config";

Function GetConcealConfig(item);
        var config := item.getprop(PROP_CONCEAL_CONFIG);
        If (!config)
            config := struct{
                interval := 10,
                range    := 10
            };
        EndIf

        return config;
EndFunction

// Capture the flag
// PID vom ControlScript
Const PROP_CAPTURE_FLAG_PID := "#captureflag";
// laeuft gerade eine Runde
Const PROP_CAPTURE_RUNNING  := "#capturerun";
// in welchem Team ist der Spieler
Const PROP_CAPTURE_TEAM     := "#captureteam";

// Flaggenfarben
Const FLAG_COLOR_BLUE :=  3;
Const FLAG_COLOR_RED  := 37;

// Flaggenfarbe aus (NPC-)Namen bestimmen
Function FlagNameToColor(name) // {{{
    name := Lower(name);
    If (name["blue"])
        return FLAG_COLOR_BLUE;
    EndIf
    return FLAG_COLOR_RED;
EndFunction // }}}

// aus Farbnummer einen Farbnamen machen (fuer Meldungen)
Function FlagColorToName(color) // {{{
    If (color == FLAG_COLOR_BLUE)
        return "Blau";
    EndIf

    return "Rot";
EndFunction // }}}

// Controlscript informieren
Function FlagEvent(byref ctl, byref me, byref type, byref mobile, extra := 0) // {{{
    var event := struct;
    event.+source := me;
    event.+type   := type;
    event.+mobile := mobile;
    If (extra)
        event.+extra := extra;
    EndIf
    ctl.sendevent(event);
EndFunction // }}}

// fuer Crystalball, Tarot und Runecup
// By Fraggulus - 27.08.2002
// ueberprueft die Gueltigkeit des Zieles
Function CheckFortuneTarget(who, ball) // {{{
  SendSysMessagePergon(who,
    "Wem wollt Ihr die Zukunft vorhersagen?",
    "Whom do you wanna tell the fortune?"
  );
  var fortune_tgt := Target(who);

  If (!fortune_tgt)
    SendSysMessagePergon(who, "Abbruch", "Abort");
    return;
  ElseIf (fortune_tgt == who)
    SendSysMessagePergon(who,
      "Ihr koennt nicht in Eure eigene Zukunft blicken!",
      "You cannot see your own future!"
    );
    return;
  ElseIf (distance(fortune_tgt, ball) > 2)
    SendSysMessagePergon(who,
      fortune_tgt.name+
      " ist zu weit entfernt, um das Ritual durchfuehren zu koennen!",
      fortune_tgt.name+" is too far away For telling him his future!"
    );
    return;
  ElseIf (distance(who, ball) > 2)
    SendSysMessagePergon(who,
      "Ihr seid nicht nah genug, um das Ritual durchfuehren zu koennen!",
      "You're not close enough to the crystal ball to see the future!"
    );
    return;
  ElseIf (fortune_tgt.isa(POLCLASS_NPC))
    SendSysMessagePergon(who,
      fortune_tgt.name+"s Zukunft bleibt Euch verborgen!",
      fortune_tgt.name+"'s future won't reveal!"
    );
    return;
  ElseIf (fortune_tgt.isa(POLCLASS_ITEM))
    SendSysMessagePergon(who,
      fortune_tgt.desc+" hat keine wissenswerte Zukunft.",
      fortune_tgt.desc+" has no worth knowing future."
    );
    return;
  Else
    Detach();
    return (fortune_tgt);
  EndIf

  return (fortune_tgt);
EndFunction // }}}

// gibt einen zufaelligen Wahrsager-Text aus
Function TellFortune(who) // {{{
  var text := "Eure Zukunft ist dunkel ... sehr dunkel!";
  Case (RandomInt(28)+1)
  1:
    text := "Ihr solltet Euch ueber Eure Ziele klar werden und diese "+
      "durchsetzen!";
  2:
    text := "Eure Zukunft ist unklar in diesem Moment, fragt spaeter noch "+
      "einmal ...";
  3:
    text := "Schlechtes Omen ... sehr schlechtes Omen. Ich kann nicht "+
      "mehr sagen.";
  4:
    text := "Huetet Euch vor Euren Naechsten, denn sie werden Euch verraten!";
  5:
    text := "Ich rate Euch, Eure Freunde gut zu beobachten, aber Eure "
      +"Feinde umso mehr.";
  6:
    text := "Die Zukunft ist glaenzend, die Zukunft ist, uhm, *hust* orange?";
  7:
    text := "Ihr werden bekommen, was Ihr verdient habt!";
  8:
    text := "Ihr werdet mit einer neuen Liebe sehr gluecklich werden.";
  9:
    text := "Seid nicht zu voreilig, eine Belohnung abzulehnen.";
  10:
    text := "Lasst die GM's in Ruhe, sie sind sehr angenervt von Eurem "+
      "staendigen Gequengle.";
  11:
    text := "Gebt acht! Eure grosse Liebe hat Augen fuer jemand anderen!.";
  12:
    text := "Maechtige Drachen werden durch Eure Klinge fallen.";
  13:
    text := "Ihr seid jemand, der weiss, was er will. Ihr habt ein "+
      "starkes Selbstbewusstsein, das allerdings schon in Eurer "+
      "Kindheit unterdrueckt wurde.";
  14:
    text := "Ihr habt Euch fuer Euer Alter gut gehalten, obwohl Ihr "+
      "schon viel Leid erfahren musstet.";
  15:
    text := "Ihr werdet einem Betrueger auf den Leim gehen und einen "+
      "finanziellen Verlust erleiden.";
  16:
    text := "Ich sehe noch einen Todesfall, der Euch sehr nahe gehen "+
      "wird, aber am Ende wird alles wieder sehr gut fuer Euch ausgehen.";
  17:
    text := "Ich sehe dunkle Waelder und grosse Gefahren. Seht Euch vor!";
  18:
    text := "Eine falsche Antwort auf eine bestimmte Frage koennte Euch "+
      "den Kopf kosten!";
  19:
    text := "Denkt nach, bevor Ihr sprecht. Ihr wisst nie, wer alles zuhoert.";
  20:
    text := "In Eurer Zukunft wird Euch ein schwarzer Ritter begegnen. "+
      "Er bedeutet nichts Gutes!";
  21:
    text := "Euer Charme duerfte seine Wirkung nicht verfehlen. "+
      "Amuesiert Euch und sorgt fuer Abwechslung!";
  22:
    text := "Sucht nach neuen Aufgaben. Ihr habt die Ausdauer diese "+
      "anzugehen und zu erfuellen.";
  23:
    text := "Besonders beruflich koennt Ihr gute Erfolge erzielen. "+
      "Allerdings sind Disziplin und Ausdauer erforderlich.";
  24:
    text := "Eine Woche voller Leidenschaften steht Euch bevor.";
  25:
    text := "Das Leben ist ein staendiges Auf und Ab. Kaempft nicht! "+
      "Nehmt die Dinge so, wie sie kommen.";
  26:
    text := "Immer mit der Ruhe! Mit Eurer Ungeduld koennt Ihr Euch "+
      "die besten Chancen kaputt machen!";
  27:
    text := "Mit Eurer Ueberzeugungskraft setzt Ihr selbst die "+
      "hartnaeckigste Konkurrenz ausser Gefecht!";
  28:
    text := "Es sollte Euch gelingen, Eure Kreativität in die "+
      "richtigen Bahnen zu lenken. Traut Euch etwas zu!";
  EndCase

  PrintTextAboveLocalizedPergon(
    who, text, "", _DEFAULT_TEXT_FONT, FONTCOLOR_RED
  );
EndFunction // }}}
