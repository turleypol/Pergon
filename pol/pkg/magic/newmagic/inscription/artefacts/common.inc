include "include/logutil";
include "include/msgs";
include "include/server";
use uo;

Const ARTEFACT_DELAY_NO  := 0;
Const ARTEFACT_DELAY     := 1;
Const ARTEFACT_ACTIVATED := "usable";
Const ARTEFACT_ENABLED   := "active";
Const ARTEFACT_COORDS    := "coords";

Function ArtefactUsable(who, item, usedelay := ARTEFACT_DELAY_NO) // {{{
  If (!item.movable)
    SendSysMessagePergon(who,
      "Das koennt Ihr nicht bewegen", "You cannot move this"
    );
    return 0;
  EndIf

  If (who.cmdlevel >= CMDLEVEL_SEER and who.getprop(I_AM_GOD))
    return 1;
  EndIf

  If (!GetObjProperty(item, "usable"))
    SendSysMessagePergon(who,
      "Ihr muesst das Artefakt erst aktivieren (lassen)!",
      "You have to activate the artefact first!"
    );
    return 0;
  EndIf

  // Nekros duerfen keine gesegneten Artefakte nutzen,
  // Priester keine verfluchten Artefakte {{{
  // var skfluch := GetSkillPergon(who, SKILLID_FLUCHEN);
  // var sksegen := GetSkillPergon(who, SKILLID_SEGNEN);
  // var artid   := GetArtefactID(item);
  // If (
  //   // Nekromant mit Priesteritem
  //   (skfluch > 0 and artid >= 200 and artid < 300) or
  //   // Priester mit Nekromantenitem
  //   (sksegen > 0 and artid >= 300)
  // )
  //   PlaySoundEffect(who, 0x526);
  //   PlayStationaryEffect(
  //     who.x, who.y, who.z, FX_FLAMESTRIKE, 10, 10, 0, who.realm
  //   );
  //   SendSysMessagePergon(who,
  //     "Das Artefakte verschwindet vor Euren Augen.",
  //     "The artefact disappears in your hands."
  //   );
  //   SubtractAmount(item, 1);
  //   return 0;
  // EndIf
  // }}}

  If (usedelay == 0)
    return 1;
  EndIf

  // Delay einhalten
  If (GetObjProperty(who, "#UsesSomething") > ReadGameClock())
    SendSysMessagePergon(who,
      "Ihr muesst etwas warten, bevor Ihr wieder Artefakte benutzen koennt",
      "You have to wait before you can use artefacts"
    );
    return 0;
  EndIf

  return 1;
EndFunction // }}}

Function GetArtefactID(item) // {{{
  var id := Lower(Hex(item.objtype));
  id["0xe"] := "";
  id := CInt(id);
  var idprop := GetObjProperty(item, "ArtefactID");
  If (idprop and idprop <> id)
    syslog(
      "WARNUNG: "+ItemInfoStr(item, COORDS_REALM)+
      ": Artefakt-ID passt nicht zu Property"
    );
  EndIf
  return CInt(id);
EndFunction // }}}

Function HighThrow(who, dest, graphic, delay) // {{{
  // getarnte Goetter duerfen direkt aus dem Himmel fallen lassen
  If (who.cmdlevel < CMDLEVEL_SEER and !who.concealed)
    // zunaechst nur hochsteigen
    PlaySoundEffect(who, 0x0222);
    PlayMovingEffectXYZ(
      who.x, who.y, who.z,
      who.x, who.y, who.z + 120, graphic, 5, 0, 0, who.realm
    );
    Sleep(delay);
  EndIf

  // danach herunterfallen
  PlayMovingEffectXYZ(
    dest.x, dest.y, dest.z + 120,
    dest.x, dest.y, dest.z, graphic, 10, 0, 0, dest.realm
  );
  SleepMS(850);
EndFunction // }}}

// vim: sw=2 sts=2
