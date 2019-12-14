Enum Colors
  ITEMCOLOR_ALCHY        := "#00A000",  // Grün
  ITEMCOLOR_MAGIC        := "#8000FF",  // Lila
  ITEMCOLOR_MANUFACTURER := "#CBA300",  // Gold
  ITEMCOLOR_MATERIAL     := "#C0C0C0",  // Hellgrau
  ITEMCOLOR_OWNER        := "#C0C0C0",  // Hellgrau
  ITEMCOLOR_POISON       := "#40A040",  // Grün
  ITEMCOLOR_RUNETEXT     := "#800000",  // Rot
  ITEMCOLOR_SONGTYPE     := "#CBA300",  // Gold
  ITEMCOLOR_ZEIT         := "#408080",  // Blaugruen
  ITEMCOLOR_ZUSTAND      := "#FF8000",  // Orange
  MOBILECOLOR_CMDLVL     := "#CBA300",  // Gold
  MOBILECOLOR_EXP        := "#FFA500",  // Orange
  MOBILECOLOR_GUILD      := "#008000",  // Grün
  MOBILECOLOR_MEMBERS    := "#0099FF",  // Hellblau
  MOBILECOLOR_NORMAL     := "#C0C0C0"   // Hellgrau
EndEnum

include ":housing:house";
include ":musicianship:common";

Function SetCliLocWithText(byref packet, cliloc, text)
  packet.SetInt32(packet.GetSize(), cliloc);
  packet.SetInt16(packet.GetSize(), text.Size()*2);
  packet.SetUnicodeStringFlipped(packet.GetSize(), text, 0);
EndFunction

Function SetCliLocWithOutText(byref packet, cliloc)
  packet.SetInt32(packet.GetSize(), cliloc);
  packet.SetInt16(packet.GetSize(), 0);
EndFunction

// Alchy Potion {{{
Function setTooltipAlchyPotion(object, byref packet)
  If (object.getprop("Description"))
    // <BASEFONT COLOR=#00A000>~1_ALCHYPOTION~
    SetCliLocWithText(packet, 1077210, CAscZ(object.getprop("Description")));
  EndIf
EndFunction
// }}}

// Fährkarten/Tillerman {{{
Function setTooltipMap(object, byref packet)
  If (object.getprop("Arrival_Time"))
    // <BASEFONT COLOR=#408080>~1_ARRIVAL_TIME~
    SetCliLocWithText(packet, 1077211, CAscZ(object.getprop("Arrival_Time")));
  EndIf
EndFunction
// }}}

// Farming: Samen {{{
Function setTooltipSeed(object, byref packet)
  Var temp := object.getprop("generation");
  If (!temp)
    temp := 1;
  EndIf
  // <BASEFONT COLOR=#FF8000>Generation ~1_GENERATION~
  SetCliLocWithText(packet, 1077212, CAscZ(temp));
EndFunction
// }}}

// Tierzucht: Eier {{{
Function setTooltipEgg(object, byref packet)
  Var temp := object.getprop("Generation");
  If (!temp)
    temp := 1;
  EndIf
  // <BASEFONT COLOR=#FF8000>Generation ~1_GENERATION~
  SetCliLocWithText(packet, 1077212, CAscZ(temp));
EndFunction
// }}}

// Farming: Eintopfbar {{{
Function setTooltipPot(object, byref packet)
  // <BASEFONT COLOR=#FF8000>Eintopfbar
  SetCliLocWithOutText(packet, 1077213);
  object:=object;
EndFunction
// }}}

// Hausschild {{{
Function setTooltipSign(object, byref packet)
  var res  := object.getprop(PROP_HS_C_OWNERNAME);
  var rent := object.getprop(PROP_HS_C_RENT_TOOLTIP);
  If (res)
    If (rent and rent <> "")
      // <BASEFONT COLOR=#C0C0C0>~1_OWNER~<br>~2_RENT~
      SetCliLocWithText(packet, 1077215,CAscZ(res+"\t"+rent));
    Else
      // <BASEFONT COLOR=#C0C0C0>~1_OWNER~
      SetCliLocWithText(packet, 1077214,CAscZ(res));
    EndIf
  EndIf
EndFunction
// }}}

// Urnen {{{
Function hasTooltipUrn(object)
  If (object.getprop("name"))
    return 1;
  Else
    return 0;
  EndIf
EndFunction

Function setTooltipUrn(object, byref packet)
  If (object.getprop("fill_level"))
    // <BASEFONT COLOR=#C0C0C0>Asche von ~1_NAME~
    SetCliLocWithText(packet, 1077216,CAscZ(object.getprop("name")));
  Else
    // <BASEFONT COLOR=#C0C0C0>Leere Urne von ~1_NAME~
    SetCliLocWithText(packet, 1077217,CAscZ(object.getprop("name")));
  EndIf
EndFunction
// }}}

// Bardenlieder {{{
Function hasTooltipSong(object)
  return 1;
  object := object;
EndFunction

Function setTooltipSong(object, byref packet)
  If (object.getprop(PROP_MUSIC_SCR_IS_COPY))
    // <BASEFONT COLOR=#CBA300>Kopie
    SetCliLocWithOutText(packet, 1077218);
  Else
    // <BASEFONT COLOR=#CBA300>Original
    SetCliLocWithOutText(packet, 1077219);
  EndIf
EndFunction
// }}}

// Spawnrunen {{{
Function hasTooltipRune(object)
  var data := object.getprop("userdata");
  If (data.questnr > 0 or data.note and data.note != "<keiner>")
    return 1;
  EndIf

  return 0;
  object := object;
EndFunction

Function setTooltipRune(object, byref packet)
  var data := object.getprop("userdata");
  var ret := "";

  If (data.questnr > 0)
    ret += "Quest Nr. "+data.questnr;
  EndIf
  If (data.note and data.note != "<keiner>")
    If (ret)
        ret += "<br>";
    EndIf
    ret += data.note;
  EndIf

  // <BASEFONT COLOR=#800000>~1_RUNETEXT~
  SetCliLocWithText(packet, 1077220, CAscZ(ret));
EndFunction
// }}}
