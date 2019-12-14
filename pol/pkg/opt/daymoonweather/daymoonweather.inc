///////////////////////////////////////////////////////////////////////////
// DayMoonWeather - include (Koordiniert die Helligkeit)
//
// based on DayNight Pkg (1999/06/02)
// Author: Racalac
//
// Author: Shinigami

///////////////////////////////////////////////////////////////////////////
// Licht
//   SetDayMoonWeatherLight      - Setzt die Helligkeit fuer den Hintergrund gemaess Tageszustand
//   SetLightLevelPergon         - Setzt die Helligkeit fuer einen Character
//   SetDayLight                 - Setzt das Tageslicht
//   SetMoonLight                - Setzt die Mondhelligkeit
//   SetWeatherLight             - Setzt die Wolkendecke
//   IsNight                     - Ist es dunkel?
//   SetDayLightLevel            - Setzt die Helligkeit fuer den Hintergrund
//   GetLightLevel               - Liefert das Lichtlevel
//
// Wetter & Saison
//   SetWeatherLevelPergon       - Setzt das Wetter
//   DisableWeatherLevelPergon   - Setzt das Wetter zurueck
//   SetSeasonPergon             - Setzt die Saison
//   SetSeasonSpecialPergon      - Setzt die Saison inkl. Texte
//   SetSeasonSpecialLogonPergon - Setzt die Saison inkl. Texte nach einem Logon
//   DisableSeasonPergon         - Setzt die Saison zurueck
//
// Sonstiges
//   CheckGlobalProperty         - Holt und setzt 'ne GlobalProperty

include "include/packets";
include "include/place";

Const MINDAYLIGHT   :=  0; // Minimum
Const DAYLIGHT      :=  0; // Normal
Const NIGHTLIGHT    := 24; // Normal
Const MAXNIGHTLIGHT := 26; // Maximum

Const WEATHER_STARTS_TO_RAIN := 0;
Const WEATHER_FIERCE_STORM   := 1;
Const WEATHER_BEGINS_SNOW    := 2;
Const WEATHER_STORM_BREWING  := 3;

Const SEASON_SPRING     := 0; // Trammel
Const SEASON_SUMMER     := 1; // Old Brittania
Const SEASON_FALL       := 2; // Fall
Const SEASON_WINTER     := 3; // Screwed attempt by OSI For snow.
Const SEASON_DESOLATION := 4; // Felucca - no Leaves on Trees, Gravestones everywhere...

// Wie lange ist welches Lichtlevel fuer den Character gueltig?
Const LIGHTLEVEL_PROP := "lightlevel";

// Pruefen, ob die Region Einschraenkungen hat?
Const CHECK_NOLIGHT := 1;

///////////////
//           //
//   Licht   //
//           //
///////////////

////////////////////////////////////////////////////////////////////////////////////////////
// SetDayMoonWeatherLight - Setzt die Helligkeit fuer den Hintergrund gemaess Tageszustand
////////////////////////////////////////////////////////////////////////////////////////////

Function SetDayMoonWeatherLight(level := -1)
  SetDayLightLevel(GetLightLevel(0, level));
EndFunction

////////////////////////////////////////////////////////////////////
// SetLightLevelPergon - Setzt die Helligkeit fuer einen Character
////////////////////////////////////////////////////////////////////

Function SetLightLevelPergon(who, level := -1, duration := 0, checknolight := 0)
  If (checknolight)        // Pruefen, ob die Region Einschraenkungen hat?
    If (PlaceNoLight(who)) // Darf man sich hier Licht zaubern?
      SendSysMessagePergon(who, "Die Goetter unterbinden Euren Versuch, diese Region zu erhellen!");
      return;
    EndIf
  EndIf

  If (level == -1)
    // Die Helligkeit soll zurueckgesetzt werden, also das Level bestimmen...
    var lightlevel := GetObjProperty(who, LIGHTLEVEL_PROP);
    If (lightlevel)
      // Ist das alte Lichtlevel noch aktiv?
      If (lightlevel[1]>ReadGameClock())
        // jupp, uebernehmen und Zeit runterrechnen
        level    := lightlevel[2];
        duration := lightlevel[1]-ReadGameClock();
      Else
        // noe, loeschen
        EraseObjProperty(who, LIGHTLEVEL_PROP);
      EndIf
    EndIf

    level := GetLightLevel(who, level);
    // Lichtlevel kurzzeitig auf einen anderen Wert setzen
    If (level<MAXNIGHTLIGHT)
      who.setlightlevel(level+1, 1);
    Else
      who.setlightlevel(level-1, 1);
    EndIf
  Else
    // Die Helligkeit soll gesetzt werden, also das Level speichern ...
    SetObjProperty(who, LIGHTLEVEL_PROP, {duration+ReadGameClock(), level});
    level := GetLightLevel(who, level);
  EndIf

  who.setlightlevel(level, duration);
EndFunction

///////////////////////////////////////
// SetDayLight - Setzt das Tageslicht
///////////////////////////////////////

Function SetDayLight(light)
  SetGlobalProperty("LightDay", light);
  SetDayMoonWeatherLight();
EndFunction

////////////////////////////////////////////
// SetMoonLight - Setzt die Mondhelligkeit
////////////////////////////////////////////

Function SetMoonLight(light)
  SetGlobalProperty("LightMoon", light);
  SetDayMoonWeatherLight();
EndFunction

////////////////////////////////////////////
// SetWeatherLight - Setzt die Wolkendecke
////////////////////////////////////////////

Function SetWeatherLight(light)
  SetGlobalProperty("LightWeather", light);
  SetDayMoonWeatherLight();
EndFunction

/////////////////////////////
// IsNight - Ist es dunkel?
/////////////////////////////

Function IsNight()
  If (GetGlobalProperty("LightDay")>DAYLIGHT+((NIGHTLIGHT-DAYLIGHT)/3)*2)
    return (1);
  Else
    return (0);
  EndIf
EndFunction

/////////////////////////////////////////////////////////////////
// SetDayLightLevel - Setzt die Helligkeit fuer den Hintergrund
/////////////////////////////////////////////////////////////////

Function SetDayLightLevel(level)
  ForEach region in (DayLightZones())
    SetRegionLightLevel(region, level);
  EndForEach
EndFunction

///////////////////////////////////////////
// GetLightLevel - Liefert das Lichtlevel
///////////////////////////////////////////

Function GetLightLevel(who, level)
  var light;

  If (level == -1)
    var daylightlevel := GetGlobalProperty("LightDay")-GetGlobalProperty("LightMoon")+GetGlobalProperty("LightWeather");

    If (who)
      var lightinfo := LightInfos(who);
      If (lightinfo.daylightzone)
        light := daylightlevel;
      Else
        light := lightinfo.lightlevel;
      EndIf
    Else
      light := daylightlevel;
    EndIf
  Else
    light := level;
  EndIf

  If (light<MINDAYLIGHT)
    light := MINDAYLIGHT;
  ElseIf (light>MAXNIGHTLIGHT)
    light := MAXNIGHTLIGHT;
  EndIf

  return (light);
EndFunction

/////////////////////////
//                     //
//   Wetter & Saison   //
//                     //
/////////////////////////

/////////////////////////////////////////////
// SetWeatherLevelPergon - Setzt das Wetter
/////////////////////////////////////////////
// SetRegionWeatherLevel("Britannia_Background", snowrain, level, 0, -1)

Function SetWeatherLevelPergon(type, severity, temperature := 0)
  If (severity>70)
    severity := 70;
  EndIf
  var packet := CreatePacket(0x65, 0x4);
  packet.setint8(1, type);
  packet.setint8(2, severity);
  packet.setint8(3, temperature);

  ForEach player in EnumerateOnlineCharacters()
    If (GetObjProperty(player, "Wetter"))
      packet.sendpacket(player);
    EndIf
  EndForEach
EndFunction

/////////////////////////////////////////////////////////
// DisableWeatherLevelPergon - Setzt das Wetter zurueck
/////////////////////////////////////////////////////////

Function DisableWeatherLevelPergon(who := 0)
  If (who)
    SendPacket(who, "65FF0000");
  Else
    ForEach player in EnumerateOnlineCharacters()
      If (GetObjProperty(player, "Wetter"))
        SendPacket(player, "65FF0000");
      EndIf
    EndForEach
  EndIf
EndFunction

///////////////////////////////////////
// SetSeasonPergon - Setzt die Saison
///////////////////////////////////////

Function SetSeasonPergon(season, who := 0)
	var packet := CreatePacket(0xBC, 0x3);
	packet.setint8(1, season);
	packet.setint8(2, 0x1);

  If (who)
    packet.sendpacket(who);
    SetLightLevelPergon(who);
  Else
    ForEach player in EnumerateOnlineCharacters()
      If (GetObjProperty(player, "Wetter"))
      	packet.sendpacket(player);
        SetLightLevelPergon(player);
      EndIf
    EndForEach
  EndIf
EndFunction

//////////////////////////////////////////////////////////
// SetSeasonSpecialPergon - Setzt die Saison inkl. Texte
//////////////////////////////////////////////////////////

Function SetSeasonSpecialPergon(season, who := 0)
  var seasontext, seasontext_e;
  var seasoncode;

  Case (season)
    0: seasontext := "Die Waerme des Sommers erfuellt das Land...";
       seasontext_e := "The warmth of the summer flows over the land...";
       seasoncode := SEASON_SUMMER;
    1: seasontext := "Die Blaetter fallen, der Herbst beginnt...";
       seasontext_e := "The leafs fall, autumn is coming...";
       seasoncode := SEASON_FALL;
    2: seasontext := "Die Kaelte des Winters ergreift das Land...";
       seasontext_e := "the cold of the winter raises over the land...";
       seasoncode := SEASON_WINTER;
    3: seasontext := "Das Eis schmilzt, der Fruehling beginnt...";
       seasontext_e := "The ice is melting, spring is coming...";
       seasoncode := SEASON_SPRING;
  EndCase

  If (who)
    SendSysMessagePergon(who, seasontext, seasontext_e);
  Else
    BroadcastPergon(seasontext, seasontext_e);
  EndIf
  SetSeasonPergon(seasoncode, who);
EndFunction

////////////////////////////////////////////////////////////////////////////////
// SetSeasonSpecialLogonPergon - Setzt die Saison inkl. Texte nach einem Logon
////////////////////////////////////////////////////////////////////////////////

Function SetSeasonSpecialLogonPergon(who)
  SetSeasonSpecialPergon(GetGlobalProperty("Season"), who);
EndFunction

///////////////////////////////////////////////////
// DisableSeasonPergon - Setzt die Saison zurueck
///////////////////////////////////////////////////

Function DisableSeasonPergon(who := 0)
  If (who)
    SendPacket(who, "BC0101");
    SetLightLevelPergon(who);
  Else
    ForEach player in EnumerateOnlineCharacters()
      If (GetObjProperty(player, "Wetter"))
        SendPacket(player, "BC0101");
        SetLightLevelPergon(player);
      EndIf
    EndForEach
  EndIf
EndFunction

///////////////////
//               //
//   Sonstiges   //
//               //
///////////////////

////////////////////////////////////////////////////////////
// CheckGlobalProperty - Holt und setzt 'ne GlobalProperty
////////////////////////////////////////////////////////////

Function CheckGlobalProperty(property, defaultvalue := 0)
  var value := GetGlobalProperty(property);
  If (!value)
    value := defaultvalue;
    SetGlobalProperty(property, value);
  EndIf

  return (value);
EndFunction
