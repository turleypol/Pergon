////////////////////////////////////////////////////////////////////////////
// Drinking- include
//
// Author: Shinigami

// Basislevel "nuechtern" ist 2.0, da bei 0.0 und Addition von Alk < 1 nicht
// korrekt weitergezaehlt wird
Const ALC_LEVEL_SOBER       :=  2.0;
Const ALC_LEVEL_PRIMED      :=  1.0 + ALC_LEVEL_SOBER;
Const ALC_LEVEL_DRUNK       :=  5.0 + ALC_LEVEL_SOBER;
Const ALC_LEVEL_HEAVY_DRUNK := 20.0 + ALC_LEVEL_SOBER;
Const ALC_LEVEL_CRITICAL    := 50.0 + ALC_LEVEL_SOBER;

Const PROP_ALCOHOL          := "level_alcohol";
Const PROP_THIRST           := "level_drink";

// GetDrinkingMsg - Ermittelt die passende Meldung
Function GetDrinkingMsg(level_drink)
  var drinkmsg;

  Case (CInt(level_drink))
    1:  drinkmsg := {"Ihr sterbt fast vor Durst",                            ""};
    2:  drinkmsg := {"Ihr seid am Verdursten!",                              ""};
    3:  drinkmsg := {"Eure Nieren duerstet und Ihr fuehlt Euch schwummrig!", "Your kidneys scream for water, you are feeling queasy."};
    4:  drinkmsg := {"Eure Nieren schmerzen!",                               "Your kidneys are aching!"};
    5:  drinkmsg := {"Ihr seid verdammt durstig!",                           ""};
    6:  drinkmsg := {"Ihr seid durstig.",                                    "You are thirsty."};
    7:  drinkmsg := {"Eure Nieren brauchen Arbeit.",                         "Your kidneys need work."};
    8:  drinkmsg := {"Ihr seid nicht durstig.",                              "You are´nt thirsty."};
    9:  drinkmsg := {"Ihr habt genug getrunken.",                            "You drank enough."};
    10: drinkmsg := {"Ihr habt genug getrunken. Uebertreibt es nicht!",      "You have drunk enough, don't overact!"};
    11: drinkmsg := {"Wenn Ihr weiterso trinkt, werdet Ihr krank.",          "You will get ill, If you drink anymore."};
    12: drinkmsg := {"Ihr habt zuviel getrunken.",                           "You drank too much!"};
    13: drinkmsg := {"Eure Nieren schmerzen vom vielen trinken!",            ""};
    14: drinkmsg := {"Euer Nieren vertragen kein ganzes Fass!",              ""};
    15: drinkmsg := {"Euer Harnfluss ist ungebremst!",                       "Your urinary flow is unstopable!"};
    16: drinkmsg := {"Eure Blase schmerzt unertraeglich!",                   ""};
    17: drinkmsg := {"Ihr habt Euch kurz vor Euren Tod gesoffen!",           ""};
    18: drinkmsg := {"Neuerdings koennt Ihr nicht mehr sinnlos trinken!",    ""};
    19: drinkmsg := {"Ihr koennt Euch keinen Wochenvorrat ansaufen!",        ""};
    20: drinkmsg := {"Der Tod spuelt sich bereits durch Eure Adern!",        ""};
    default: drinkmsg := {"Der Tod ergreift Besitz von Eurem Geist!",        ""};
  EndCase

  return (drinkmsg);
EndFunction

// CheckDrinkingLevel - Ueberprueft das DrinkLevel
Function CheckDrinkingLevel(chr, level_drink)
  var drinkcolor := FONTCOLOR_RED;
  If ((level_drink >= 7) And (level_drink <= 13))
    drinkcolor := FONTCOLOR_GREEN;
  EndIf

  level_drink := CInt(level_drink);

  If ((2 <= level_drink) And (level_drink <= 18))
    Case (level_drink)
      2:  ApplyRawDamagePergon(chr, GetMaxHPPergon(chr)/2);
      3:  ApplyRawDamagePergon(chr, GetMaxHPPergon(chr)/4);
      4:  ApplyRawDamagePergon(chr, GetMaxHPPergon(chr)/8);
      5:  ApplyRawDamagePergon(chr, GetMaxHPPergon(chr)/16);
      6:  ApplyRawDamagePergon(chr, GetMaxHPPergon(chr)/32);
      14: ApplyRawDamagePergon(chr, GetMaxHPPergon(chr)/32);
      15: ApplyRawDamagePergon(chr, GetMaxHPPergon(chr)/16);
      16: ApplyRawDamagePergon(chr, GetMaxHPPergon(chr)/8);
      17: ApplyRawDamagePergon(chr, GetMaxHPPergon(chr)/4);
      18: ApplyRawDamagePergon(chr, GetMaxHPPergon(chr)/2);
    EndCase
  Else
    KillMobile(chr, "drinking");
  EndIf

  return (drinkcolor);
EndFunction
