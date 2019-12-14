///////////////////////////////////////////////////////////////////////
// party.inc - Nützliche Funktionen
//
// Erstellt: 18.12.03
//
// Modified: 01.11.05 Fox: ZugriffErlaubt(who,corpse) hinzugefügt
//           13.11.05 Fox: WasPackAnimal(what) hinzugefügt
//           30.03.07 Fox: Herrenlose Packer konnten nicht gelootet werden

use polsys;
use uo;

//////////////////////////////
// Party Loot
//////////////////////////////

// Darf who von Char gelootet werden?
Function CanLoot(character, who)
  If ((character.party) && (character.party==who.party))
    Return (who.partycanloot);
  EndIf

  return (0);
EndFunction


// IsWasPackAnimal() - War/ist es ein Packtier?
Function IsWasPackAnimal(what)
  var templ;
  If (what.npctemplate)
    templ := what.npctemplate;
  Else
    templ := GetObjProperty(what, "npctemplate");
  EndIf

  If (!templ)
    return 0;
  Else
    templ := Lower(templ);
  EndIf

  If (
    (templ in array{"packhorse", "packllama"}) or
    GetObjProperty(what,"packer")
)
    return 1;
  EndIf

  return 0;
EndFunction

// gibt Zurück ob der Zugriff aufs Loot berechtigt ist
// kommt aus dem alten corpseloot, wird aber jetzt an mehreren Stellen
// benötigt erweitert um Schutz von Göttern und Questchars
Function ZugriffErlaubt(who, corpse)
  If (!who) // Versucht ein Player zu looten? Der Core darf immer.
    return (1);
  EndIf

  If (who.cmdlevel >= CMDLEVEL_SEER)
    return (1);                   // GMs dürfen immer looten
  EndIf

  //NPC's
  If (IsWasPackAnimal(corpse))
    var master := GetObjProperty(corpse, "master");
    If ((!master) || (who.serial == master))
      return (1);                   // Man darf seine eigenen Packer bergen und welche die Niemandem gehören
    Else
      return (0);
    EndIf
  Else
    If (GetObjProperty(corpse, "npctemplate"))      // NPCs, mit Ausnahme von Packern, können immer gelootet werden.
      return (1);
    EndIf
  EndIf

  // Ab hier Player Behandlung:

  //globale Property mit der man den Lootschutz abschalten kann
  If (!GetGlobalProperty("Noloot"))         // Wenn "Noloot" nicht gesetzt, darf jeder alles looten.
    return (1);
  EndIf

  If (who.serial == GetObjProperty(corpse, "myserial"))
    return (1);                   // Man darf seine eigene Leiche bergen...
  EndIf

  var ghost := SystemFindObjectBySerial(GetObjProperty(corpse, "myserial"),SYSFIND_SEARCH_OFFLINE_MOBILES);
  If (ghost)
    If (ghost.cmdlevel >= CMDLEVEL_QUESTCHAR)
      return (0); //dürfen von Playern nicht gelootet werden
    EndIf
    If (ghost.party) // Ist der tote Spieler in einer Party?
      If (CanLoot(who, ghost))            // Ist der tote Spieler in der selben Party wie 'who'?
        return (1);
      EndIf
    Else // Nein, er darf sich nur selber looten
      return (0);
    EndIf
  EndIf

  return (0);                     // Default: Looten verweigern.
EndFunction
