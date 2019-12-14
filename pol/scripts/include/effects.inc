//////////////////////////////////////////////////////////////////////////////
// effects.inc  - Include für optische Effekte und anderes Zeugs :)
//
// ExplodeInCircle(x, y, z, realm, radius,
//   optionale Parameter:
//   explosions, delay, delaystep, array{effects, ..})
// - Explosion von einem Punkt aus
//
// Function ExplodeToDirection(sx, sy, sz, realm, dir,
//   // optional
//   minz := 5, maxz := 25, particles := 50, delay := 100, delaystep := 25,
//   fx := ""
// - Explosionen von einem Punkt in eine Richtung, auf oder absteigend
//
// Bereich innerhalb der beiden Koordinaten mit Stangen darstellen
// Function VisualizeRange(coord1, coord2, eff := 0x3001, time := 200)
// - Markiert fuer <time> einen rechteckigen Bereich mit Item 0x3001
//
// ExplodeInRectangle(x, y, z, fx0, fx1, fx2, fx3, fx4)
// - Explosion in einem Rechteck von Punkt aus (unfertig)
//

include "include/client";
include "include/math";
include "include/mobile";
use math;
use uo;

Function ExplodeInCircle(cx, cy, cz, realm, radius,
  // optional
  explosions := 50, delay := 100, delaystep := 25,
  fx := ""
)
  If (TypeOfInt(fx) != OT_ARRAY or fx.size() <= 0)
    fx := array{FX_EXPLODE_1};
  EndIf

  var i,j := 0;
  For (i := 0; i <= explosions; i += 1)
    j += (2*ConstPi())/20;

    // Polarkoordinaten (gut wa? ;-)
    var x := CInt(cx + radius * Cos(j) + 1);
    var y := CInt(cy + radius * Sin(j) + 1);
    var z := cz + RandomIntMinMax(5, 1+10);

    var eff := fx.randomentry();
    If ((i%delaystep) == 0)
      SleepMS(delay);
    EndIf
    PlayMovingEffectXYZ(cx, cy, cz, x, y, z, eff, 5, 10, 1, realm);
  EndFor
EndFunction

Function ExplodeToDirection(sx, sy, sz, realm, dir,
  // optional
  minz := 5, maxz := 25, particles := 50, delay := 100, delaystep := 25,
  fx := ""
)
  If (TypeOfInt(fx) != OT_ARRAY or fx.size() <= 0)
    fx := array{FX_EXPLODE_1};
  EndIf

  If (maxz < minz)
    var swap := minz;
    minz := maxz;
    maxz := swap;
  EndIf

  // Koordinatenverschiebung abhaengig von Richtung
  var xdev := 0;
  var ydev := 0;
  If (dir > -1)
    xdev := ConvertDirectionToLocX(dir);
    ydev := ConvertDirectionToLocY(dir);
  EndIf

  For i := 0 To particles
    var eff  := fx.randomentry();
    var x    := RandIntMinMax(sx - 1 + xdev, sx + 1 + xdev);
    var y    := RandIntMinMax(sy - 1 + ydev, sy + 1 + ydev);
    var z    := sz + RandomIntMinMax(minz, 1+maxz);

    If ((i%delaystep) == 0)
      SleepMS(delay);
    EndIf
    PlayMovingEffectXYZ(sx, sy, sz, x, y, z, eff, 1, 10, 0, realm);
  EndFor
EndFunction

Function ExplodeInRectangle(cx, cy, cz, realm, range, fx)

  //PlayMovingEffectXYZ(
EndFunction

// Bereich innerhalb der beiden Koordinaten mit Stangen darstellen
Function VisualizeRange(coord1, coord2, eff := 0x3001, time := 200) // {{{
  // im Uhrzeigersinn
  ForEach point in (
    // W->O, Nordseite
    GetCoordsInLine(coord1.x, coord1.y, coord2.x, coord1.y)+
    // N->S, Ostseite
    GetCoordsInLine(coord2.x, coord1.y, coord2.x, coord2.y)+
    // O->W, Suedseite
    GetCoordsInLine(coord2.x, coord2.y, coord1.x, coord2.y)+
    // S->N, Westseite
    GetCoordsInLine(coord1.x, coord2.y, coord1.x, coord1.y)
  )
    // (alle Eckpunkte sind doppelt drin, aber was soll's)
    SleepMs(2);
    If (!point)
      syslog("FEHLER: VisualizeRange (1): "+point.errortext);
      continue;
    EndIf

    var res := PlayStationaryEffect(
      point.x, point.y, coord1.z, eff, 1, time, 0, coord1.realm
    );
    If (!res)
      syslog("FEHLER: VisualizeRange (2): "+res.errortext);
    EndIf
  EndForEach
EndFunction // }}}

// vim: sw=2 sts=2
