////////////////////////////////////////////////////////////////////////////
//
//   Hungry - System Include
//
//     Author: Shinigami
//
//   Modifications:
//     2002/07/23 Shinigami: Prozentualer HP-Abzug, um Magier zu "schonen"
//
////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////
// GetHungryMsg - Ermittelt die passende Meldung
//////////////////////////////////////////////////

Function GetHungryMsg(hungrylevel)
  Var hungrymsg;

  Case (CInt(hungrylevel))
    1:  hungrymsg:={"Ihr sterbt vor Hunger!",                            ""};
    2:  hungrymsg:={"Ihr seid am Verhungern!",                           ""};
    3:  hungrymsg:={"Euer Magen knurrt und Ihr fuehlt Euch schwummrig!", ""};
    4:  hungrymsg:={"Euer Magen schmerzt!",                              ""};
    5:  hungrymsg:={"Ihr seid verdammt hungrig!",                        "You are really hungry!"};
    6:  hungrymsg:={"Ihr seid hungrig.",                                 ""};
    7:  hungrymsg:={"Euer Magen knurrt.",                                "Your stomach snarls."};
    8:  hungrymsg:={"Ihr seid nicht hungrig.",                           "You are not hungry."};
    9:  hungrymsg:={"Ihr seid satt.",                                    "You are full."};
    10: hungrymsg:={"Ihr seid satt und habt Euch beinahe ueberfressen.", "You are full and have yourself almost over-eat."};
    11: hungrymsg:={"Wenn Ihr weiterso esst, werdet Ihr krank.",         ""};
    12: hungrymsg:={"Ihr habt Euch ueberfressen!",                       ""};
    13: hungrymsg:={"Ihr bekommt Kraempfe vom vielen Essen!",            "You are getting cramps due to your eating behaviour."};
    14: hungrymsg:={"Euer Magen vertraegt kein ganzes Schwein!",         ""};
    15: hungrymsg:={"Ihr koennt die Schmerzen kaum noch aushalten!",     ""};
    16: hungrymsg:={"Ihr haltet es kaum noch aus vor Uebelkeit!",        ""};
    17: hungrymsg:={"Ihr habt Euch kurz vor Euren Tod gefressen!",       ""};
    18: hungrymsg:={"Neuerdings koennt Ihr nicht mehr sinnlos essen!",   ""};
    19: hungrymsg:={"Ihr koennt Euch keinen Wochenvorrat anfressen!",    ""};
    20: hungrymsg:={"Der Tod macht sich in Euren Gedaermen breit!",      ""};
    default: hungrymsg:={"Der Tod ergreift Besitz von Eurem Koerper!",   ""};
  EndCase

  Return (hungrymsg);
EndFunction

///////////////////////////////////////////////////
// CheckHungryLevel - Ueberprueft das HungryLevel
///////////////////////////////////////////////////

Function CheckHungryLevel(chr, hungrylevel)
  Var hungrycolor:=FONTCOLOR_RED;
  If ((hungrylevel>=7) And (hungrylevel<=13))
    hungrycolor:=FONTCOLOR_GREEN;
  EndIf

  hungrylevel:=CInt(hungrylevel);

  If ((2<=hungrylevel) And (hungrylevel<=18))
    Case (hungrylevel)
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
    KillMobile(chr, "hungry");
  EndIf

  Return (hungrycolor);
EndFunction