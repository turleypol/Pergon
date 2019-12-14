Enum CardColor
  Kreuz:= 1,
  Pik,
  Herz,
  Karo
EndEnum

Var CardColor:={Kreuz,Pik,Herz,Karo};

Enum CardValue
  NoCard := 0,
  Two := 2,
  Three,
  Four,
  Five,
  Six,
  Seven,
  Eight,
  Nine,
  Ten,
  Jester,
  Queen,
  King,
  Ace // = 14
EndEnum

Var CardValue:={NoCard,Two,Three,Four,Five,Six,Seven,Eight,Nine,Ten,Jester,Queen,King,Ace};

Var CardSet:={};  // FreshCardsSet()
Var ActiveCardSet;

Var Card:=Struct{value:=NoCard,color:=Kreuz};

Enum HandType
  Unset:=0,
  HighCard,
  Pair,
  TwoPairs,
  ThreeOfAKind,
  Straight,
  Flush,
  FullHouse,
  FourOfAKind,
  StraightFlush
EndEnum

Enum ShowDownConst
  SHOWNORMAL:=0, // oder normale Runde für Gump
  SHOWALL:=0,  // Fehlende Tablecards zeigen (für internen Gebrauch)
  SHOWHANDS,   // Handcards zeigen
  FOLD         // nix zeigen
EndEnum

Var PokerPlayer:=Struct{Player:=0,
                        Hand:={Card,Card},
                        Chips:=1000,
                        BetToPot:=0,
                        IsOut:=0,
                        Leaved:=0,
                        CanBet:=1,
                        LastAction:="",
                        FinalHand:={Card,Card,Card,Card,Card},
                        FinalHandType:=Unset,
                        GumpPID:=0};

Var Game:=Struct{Pot:=0,
                 Players:={},
                 CurrentPlayer:=1,
                 DealerButton:=1,
                 SmallBlindPlayer:=1,
                 BigBlindPlayer:=1,
                 Blinds:=10,
                 ChipsPerPlayer:=1000,
                 BlindsRaiseTime:=4,
                 BlindsRaise:=10,
                 CurrentStake:=0,
                 Flop:={Card,Card,Card},
                 Turn:={Card},
                 River:={Card},
                 Spectators:={}};

Var Spectator:=Struct{mob:=0,
                      gumpPID:=0};

// Bastelt nen Kartendeck zusammen
// nach Farbe sortiert
Function FreshCardsSet()
  CardSet:={};
  Var i,j,c;
  c:=Card;
  For (i:=1;i<=4;i+=1)
    For (j:=2;j<=14;j+=1)
      c.value:=CardValue[j];
      c.color:=CardColor[i];
      CardSet.append(c);
    EndFor
    SleepMS(2);
  EndFor
EndFunction

Function HandTypeToString(type)
  Return({"High Card",
          "Pair",
          "Two Pairs",
          "Three Of A Kind",
          "Straight",
          "Flush",
          "Full House",
          "Four Of A Kind",
          "Staight Flush"}[type]);
EndFunction

Function CardToString(type)
  Return({"",
          "Two",
          "Three",
          "Four",
          "Five",
          "Six",
          "Seven",
          "Eight",
          "Nine",
          "Ten",
          "Jester",
          "Queen",
          "King",
          "Ace"}[type]);
EndFunction