///////////////////////////////////////////////////////////////////////////
// Jehova Init Include v1.0 (2000/10/14)
//
// Author: Shinigami

include "include/logutil";

/////////////////////////////////////
// Jehova - Bestrafung durchfuehren
/////////////////////////////////////
Function Jehova(opfer, bestrafer)
  If (!opfer)
    return;
  EndIf

  If (bestrafer)
    If (opfer.cmdlevel >= bestrafer.cmdlevel)
      SendSysMessagePergon(bestrafer, "Kommando nicht ausfuehrbar!");
      return;
    EndIf
  EndIf

  If (bestrafer)
    syslog(
      "STRAFE: "+CharInfoStr(opfer, COORDS_REALM)+" wird bestraft von "+
      CharInfoStr(bestrafer, COORDS_REALM)
    );
  Else
    syslog(
      "STRAFE: "+CharInfoStr(opfer, COORDS_REALM)+" wird bestraft vom Censor"
    );
  EndIf

  Var npc;
  Var ev := struct;
  ev.+type     := EVID_ROBOT_INIT;
  ev.+opponent := opfer;

  Var koords := array{};
  Var winkel;
  Var x;
  Var y;

  Var j;
  Var i;
  For (i:=0; i<8; i+=1)
    j:=3;
    While (j)
      winkel := RandomInt(360);
      x := CInt(Floor(4*Sin(winkel)))+opfer.x;
      y := CInt(Floor(4*Cos(winkel)))+opfer.y;

      If (!((x+" "+y) in koords))
        npc := CreateNpcFromTemplate(
          "jehova", x, y, GetWorldHeight(x, y, opfer.realm), 0, opfer.realm
        );
        If (npc)
          j:=1;
        EndIf

        koords.append(x+" "+y);
      EndIf

      j-=1;
    EndWhile

    If (npc)
      SendEvent(npc, ev);
    EndIf
  EndFor
EndFunction
