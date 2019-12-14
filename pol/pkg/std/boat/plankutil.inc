include "include/client";
include "include/objtype";

Const PROP_FERRY_CONTROLS_PLANK := "boatcontrolled";

Function IsExtended(plank)
  return plank.graphic in {GID_PLANK_EXTENDED_FACING_EAST, GID_PLANK_EXTENDED_FACING_WEST, GID_PLANK_EXTENDED_FACING_NORTH, GID_PLANK_EXTENDED_FACING_SOUTH};
EndFunction

Function IsPlankOccupied(plank)
  return ListMobilesNearLocation(plank.x, plank.y, plank.z, 0,plank.realm).size();
EndFunction

Function Extend(plank)
  If (!IsExtended(plank))
    TogglePlank(plank);
  EndIf
EndFunction

Function Retract(plank)
  If (IsExtended(plank))
    TogglePlank(plank);
  EndIf
EndFunction

Function TogglePlank(plank)
  var partnertype := PlankPartner(plank.graphic);
  If (partnertype)
    PlaySoundEffect(plank, SFX_TOGGLE_PLANK);
    plank.graphic := partnertype;
  EndIf
EndFunction

Function PlankPartner(planktype)
  Case (planktype)
    GID_PLANK_EXTENDED_FACING_EAST:   return GID_PLANK_RETRACTED_FACING_EAST;  // 0x3ed5: return 0x3eb1;
    GID_PLANK_RETRACTED_FACING_EAST:  return GID_PLANK_EXTENDED_FACING_EAST;   // 0x3eb1: return 0x3ed5;
    GID_PLANK_EXTENDED_FACING_WEST:   return GID_PLANK_RETRACTED_FACING_WEST;  // 0x3ed4: return 0x3eb2;
    GID_PLANK_RETRACTED_FACING_WEST:  return GID_PLANK_EXTENDED_FACING_WEST;   // 0x3eb2: return 0x3ed4;
    GID_PLANK_EXTENDED_FACING_NORTH:  return GID_PLANK_RETRACTED_FACING_NORTH; // 0x3e84: return 0x3e85;
    GID_PLANK_RETRACTED_FACING_NORTH: return GID_PLANK_EXTENDED_FACING_NORTH;  // 0x3e85: return 0x3e84;
    GID_PLANK_EXTENDED_FACING_SOUTH:  return GID_PLANK_RETRACTED_FACING_SOUTH; // 0x3e89: return 0x3e8a;
    GID_PLANK_RETRACTED_FACING_SOUTH: return GID_PLANK_EXTENDED_FACING_SOUTH;  // 0x3e8a: return 0x3e89;
    default:
      syslog("Unknown plank type 0x" + Hex(planktype));
      return error;
  EndCase
EndFunction

Function CanWalkOntoPlank(plank)
  var dx;
  var dy;
  Case (plank.graphic)
    GID_PLANK_EXTENDED_FACING_WEST:
    GID_PLANK_RETRACTED_FACING_WEST:    dx := -1;
                                        dy := 0;
    GID_PLANK_EXTENDED_FACING_EAST:
    GID_PLANK_RETRACTED_FACING_EAST:    dx := 1;
                                        dy := 0;

    GID_PLANK_EXTENDED_FACING_NORTH:
    GID_PLANK_RETRACTED_FACING_NORTH:   dx := 0;
                                        dy := -1;
    GID_PLANK_EXTENDED_FACING_SOUTH:
    GID_PLANK_RETRACTED_FACING_SOUTH:   dx := 0;
                                        dy := 1;
  EndCase
  If (GetStandingHeight(plank.x + dx, plank.y + dy, plank.z,plank.realm))
    return 1;
  Else
    return 0;
  EndIf
EndFunction
