///////////////////////////////////////////////////////////////////////////
// Include file magicpergon.inc
//
// implements magic related functions For Pergon's magic system

///////////////////////////////////////////////////////////////////////////
// TurnCharTo (who, whom)                 - turns who toward whom
// GetSpellDifficultyPergon (spellid)     - returns required skill For casting
// GetSpellID (magicitem)                 - returns spellid of spell embodied in magicitem
// CheckReagsPergon (who, spellid)        - checks, If required reagents are available
// ConsumeReagsPergon (who, spellid)      - consumes reagents
// CheckManaPergon (who, spellid)         - checks, If required mana is available; returns zero or
//                                          mana actually required
// CheckPartsPergon(who, artefactID)      - sucht nach ArtefactReags im Backpack und gibt Array zurück
// ConsumePartsPergon(who, parts)         - bekommt Array und verbraucht ArtefactReags im Backpack von who
// ConsumeManaPergon (who, spellid, mana) - consumes mana percent of required mana; mana defaults to zero
// CheckKarmaPergon (who, karma)          - checkt ob Karma vorhanden ist
// CheckMagicSkillPergon(who, spellid, skill) - checkt ob erfolgreich gezaubert wird
// PowerWordsPergon (who, spellid, skill) - utters powerwords according to skill used For casting
// CanCastSpell (who, spellid, skill)     - checks If char can cast spell; returns zero or
//                                          mana actually required (due to mana channeling skill)
// CastSpell (who, spellid, skill)        - casts given spell
// GetCastPowerProp(who, prop)            - gibt cProp Wert zurück
// SetCastPowerProp(who, prop, value)     - setzt cProp Wert
// ModifyCastPowerProp(who, prop, value)  - modifiziert cProp Wert
//
// ReflectSpell (caster, caston, moving, fx, speed, loop, explode, soundfx)
//            - checks magic reflection on caston and caster
//              plays soundfx/graphicfx according to reflection
//              returns target of spell
//               + fxtype - type of graphic effect
//                  0: moving effect
//                  1: object centered effect
//                  2: lightningbolt effect
//               + fx - graphic effect of spell
//               + speed, loop, explode - options For fx
//               + soundfx - sound effect of spell

use attributes;
use cfgfile;
use os;
use uo;
include ":runebook:rune";
include "include/modifyskill";
include "include/objtype";
include "include/packets";
include "spellrestrictions";

CONST SUMMON_BY_CLERIC := 1;
CONST SUMMON_BY_ELMAGE := 1;
CONST SUMMON_BY_NECRO  := 9;
// Spezialwert, wenn es nicht klar zuzuordnen ist
CONST SUMMON_BY_UNDEF  := 5;

////////////////////////
// Konstanten (Spells)
////////////////////////
// Amount of Mana, which gets saved If mana channeling was successful
// (-> ConsumeManaPergon())
Const SAVE_MANA_AMNT := 0.4;
Const SPELLID_COMMON_CREATEFOOD := 401; // Essen
Const SPELLID_COMMON_CREATEDRINK := 402; // Trinken
Const SPELLID_COMMON_DAZZLE := 403; // Verwirrung
Const SPELLID_COMMON_CLUMSY := 404; // Missgeschick
Const SPELLID_COMMON_WEAKEN := 405; // Schwaeche
Const SPELLID_COMMON_MAGICARROW := 406; // Magischer Pfeil
Const SPELLID_COMMON_TELEKINESIS := 407; // Telekinese
Const SPELLID_COMMON_RUST := 408; // Zahn der Zeit
Const SPELLID_COMMON_LIGHT := 409; // Licht
Const SPELLID_COMMON_PROTECTION := 410; // Schutz
Const SPELLID_COMMON_MINORHEAL := 411; // Kleine Heilung
Const SPELLID_COMMON_CUNNING := 412; // Scharfsinn
Const SPELLID_COMMON_AGILITY := 413; // Geschicklichkeit
Const SPELLID_COMMON_STRENGTH := 414; // Kraft
Const SPELLID_COMMON_SLOW := 415; // Hinken [noch nicht implementiert]
Const SPELLID_COMMON_SPEED := 416; // Rennen [noch nicht implementiert]
Const SPELLID_COMMON_MAGICLOCK := 417; // Magisches Schloss
Const SPELLID_COMMON_MAGICTRAP := 418; // Magische Falle
Const SPELLID_COMMON_REVEAL := 419; // Enthuellung
Const SPELLID_COMMON_REVIVE := 420; // Reperatur
Const SPELLID_COMMON_MAGICUNLOCK := 421; // Magischer Schluessel
Const SPELLID_COMMON_MAGICUNTRAP := 422; // Entschaerfen
Const SPELLID_COMMON_RECALL := 423; // Zurueckkehren
Const SPELLID_COMMON_MANADRAIN := 424; // Manadieb
Const SPELLID_COMMON_REFLECT := 425; // Magischer Spiegel
Const SPELLID_COMMON_INVISIBILITY := 426; // Unsichtbarkeit
Const SPELLID_COMMON_WALL := 427; // Mauer
Const SPELLID_COMMON_MARK := 428; // Merken
Const SPELLID_COMMON_INCOGNITO := 429; // Inkognito
Const SPELLID_COMMON_TELEPORT := 430; // Teleportation
Const SPELLID_COMMON_GATETRAVEL := 431; // Magisches Tor
Const SPELLID_COMMON_POLYMORPH := 432; // Polymorph

Const SPELLID_MAGE_FIREBALL := 101; // Feuerball
Const SPELLID_MAGE_ICEBOLT := 102; // Eiszapfen
Const SPELLID_MAGE_STONESPLINTER := 103; // Steinsplitter
Const SPELLID_MAGE_ENERGYBOLT := 104; // Stromschlag
Const SPELLID_MAGE_INCINERATE := 105; // Waffe Entflammen
Const SPELLID_MAGE_FREEZE := 106; // Waffe Vereisen
Const SPELLID_MAGE_PETRIFY := 107; // Waffe Versteinern
Const SPELLID_MAGE_CHARGE := 108; // Waffe Aufladen
Const SPELLID_MAGE_FLAMESTRIKE := 109; // Flammenschlag
Const SPELLID_MAGE_FROSTSHOCK := 110; // Frostschock
Const SPELLID_MAGE_METEOR := 111; // Meteor
Const SPELLID_MAGE_LIGHTNING := 112; // Blitz
Const SPELLID_MAGE_RESISTWATER := 113; // Wasserresistenz
Const SPELLID_MAGE_FIREFIELD := 114; // Feuerfeld
Const SPELLID_MAGE_BLIZZARD := 115; // Blizzard
Const SPELLID_MAGE_STONEHAIL := 116; // Steinhagel
Const SPELLID_MAGE_LIGHTNINGFIELD := 117; // Blitzfeld
Const SPELLID_MAGE_RESISTEARTH := 118; // Erdresistenz
Const SPELLID_MAGE_FIREAURA := 119; // Flammenaura
Const SPELLID_MAGE_ICEAURA := 120; // Eisaura
Const SPELLID_MAGE_STONEAURA := 121; // Felsaura
Const SPELLID_MAGE_LIGHTNINGAURA := 122; // Blitzaura
Const SPELLID_MAGE_RESISTFIRE := 123; // Feuerresistenz
Const SPELLID_MAGE_RESISTAIR := 124; // Luftresistenz
Const SPELLID_MAGE_FIREELEMENTAL := 125; // Feuerelementar
Const SPELLID_MAGE_WATERELEMENTAL := 126; // Wasserelementar
Const SPELLID_MAGE_EARTHELEMENTAL := 127; // Erdelementar
Const SPELLID_MAGE_AIRELEMENTAL := 128; // Luftelementar
Const SPELLID_MAGE_VULCANO := 129; // Vulkan
Const SPELLID_MAGE_CHARYBDIS := 130; // Charybdis
Const SPELLID_MAGE_EARTHQUAKE := 131; // Erdbeben
Const SPELLID_MAGE_CHAINLIGHTNING := 132; // Gewitter

Const SPELLID_CLERIC_HEAL := 201; // Heilung
Const SPELLID_CLERIC_CURE := 202; // Gegengift
Const SPELLID_CLERIC_FOG := 203; // Nebel
Const SPELLID_CLERIC_DISPEL := 204; // Bann
Const SPELLID_CLERIC_GREATERHEAL := 205; // Grosse Heilung
Const SPELLID_CLERIC_ARCHCURE := 206; // Erzgegengift
Const SPELLID_CLERIC_UNCURSE := 207; // Gegenfluch
Const SPELLID_CLERIC_CLEANSE := 208; // Augen Oeffnen
Const SPELLID_CLERIC_ARCHHEAL := 209; // Erzheilung
Const SPELLID_CLERIC_BLESS := 210; // Segen
Const SPELLID_CLERIC_BLESSARMOR := 211; // Ruestungssegen
Const SPELLID_CLERIC_BLESSWEAPON := 212; // Waffensegen
Const SPELLID_CLERIC_DISPELFIELD := 213; // Antimagie
Const SPELLID_CLERIC_RESTOREHEALTH := 214; // Gesundheit
Const SPELLID_CLERIC_PARALYZE := 215; // Paralyse
Const SPELLID_CLERIC_ARCHPROTECTION := 216; // Erzschutz
Const SPELLID_CLERIC_EXORCISM := 217; // Exorzismus
Const SPELLID_CLERIC_HOLYSTRIKE := 218; // Opferschlag
Const SPELLID_CLERIC_ARCHBLESS := 219; // Erzsegen
Const SPELLID_CLERIC_MELT := 220; // Schmelzen
Const SPELLID_CLERIC_PARALYZEFIELD := 221; // Paralysefeld
Const SPELLID_CLERIC_RESISTELEMENTS := 222; // Elementarresistenz
Const SPELLID_CLERIC_MEDITATION := 223; // Meditation
Const SPELLID_CLERIC_MINDBLAST := 224; // Geisteskampf
Const SPELLID_CLERIC_SUMMONANGEL := 225; // Rufe Engel
Const SPELLID_CLERIC_RESURRECT := 226; // Wiedererwecken
Const SPELLID_CLERIC_HOLYWARRIOR := 227; // Heiliger Krieger
Const SPELLID_CLERIC_HOLYMIRROR := 228; // Heiliger Spiegel
Const SPELLID_CLERIC_IMMUNE := 229; // Immunitaet
Const SPELLID_CLERIC_SACRIFICE := 230; // Opfer
Const SPELLID_CLERIC_SPIRITSPEAK := 231; // Geistersprache
Const SPELLID_CLERIC_HOLYSPIRIT := 232; // Heiliger Geist

Const SPELLID_NECRO_RAISESKELETON := 301; // Erwecke Skelett
Const SPELLID_NECRO_POISON := 302; // Gift
Const SPELLID_NECRO_CURSE := 303; // Verfluchen
Const SPELLID_NECRO_PUSH := 304; // Wegstossen
Const SPELLID_NECRO_RAISEUNDEAD := 305; // Erwecke Untoten
Const SPELLID_NECRO_ARCHCURSE := 306; // Erzfluch
Const SPELLID_NECRO_CURSEITEM := 307; // Gegenstand Verfluchen
Const SPELLID_NECRO_BLIND := 308; // Erblinden
Const SPELLID_NECRO_SUMMONCREATURE := 309; // Beschwoerung
Const SPELLID_NECRO_ARCHPOISON := 310; // Erzgift
Const SPELLID_NECRO_DARKNESS := 311; // Dunkelheit
Const SPELLID_NECRO_HALLUCINATION := 312; // Halluzination
Const SPELLID_NECRO_CONTROLCREATURE := 313; // Beherrschung
Const SPELLID_NECRO_RAISEGOLEM := 314; // Erwecke Golem
Const SPELLID_NECRO_POISONFIELD := 315; // Giftfeld
Const SPELLID_NECRO_RESISTPOISON := 316; // Giftresistenz
Const SPELLID_NECRO_SOULSENSE := 317; // Seelen Spueren
Const SPELLID_NECRO_BONEBREAKER := 318; // Knochenbrecher
Const SPELLID_NECRO_MANAVAMPIRE := 319; // Manavampir
Const SPELLID_NECRO_POISONCLOUD := 320; // Giftnebel
Const SPELLID_NECRO_BLADESPIRIT := 321; // Klingendaemon
Const SPELLID_NECRO_RELEASE := 322; // Entlassen
Const SPELLID_NECRO_FINDCORPSE := 323; // Leichenschaendung
Const SPELLID_NECRO_BLOODRUSH := 324; // Blutrausch
Const SPELLID_NECRO_SUMMONDAEMON := 325; // Daemonenbeschwoerung
Const SPELLID_NECRO_POISONELEMENTAL := 326; // Giftelementar
Const SPELLID_NECRO_LETHALBREATH := 327; // Hauch des Todes
Const SPELLID_NECRO_STATUE := 328; // Statue
Const SPELLID_NECRO_RESTLESSSOUL := 329; // Verdammung
Const SPELLID_NECRO_DEADKNIGHT := 330; // Todesritter
Const SPELLID_NECRO_OBLITERATEMINION := 331; // Kreaturenopfer
Const SPELLID_NECRO_ARMAGEDDON := 332; // Armageddon


/****************
 * TurnCharTo() *
 ****************/
// +---------------> x
// | +---+---+---+
// | | 7 | 0 | 1 |
// | +---+---+---+
// | | 6 | C | 2 |
// | +---+---+---+
// | | 5 | 4 | 3 |
// | +---+---+---+
// |
// y     facings of Char
Function TurnCharTo (who, whom)
  If ((who.x==whom.x) && (who.y==whom.y))
    return;
  EndIf
  who.setfacing(GetFacing(who.x, who.y, whom.x, whom.y), FACING_FORCE);
EndFunction

/******************************
 * GetCastPowerPergon() *
 ******************************/

Function GetCastPowerPergon (who, spellid)
  var spells := ReadConfigFile("::spells");
  var spellspower := ReadConfigFile(":newspells:spellspower");
  var subClass := Lower(spells[spellid].SubClass); // Subklasse holen
  var class := Lower(spells[spellid].Class);       // Klasse holen

  var basePower := GetIntBasePergon(who); // Grundstärke ist aktuelle Int

  var dtCastPower := who.getprop("MagicCastPower");

  //// Zauberstärkeboni
  // Artefakte (Flakons)
  // Multiplikations mit 10 weil nur die Anzahl der Flakons
  // gespeichert wird, nicht deren Wertigkeit
  var bonusPowerFlask :=
    CInt(dtCastPower["castbonus_flask_of_all"])*10
    + CInt(dtCastPower["castbonus_flask_of_" + class])*10
    + CInt(dtCastPower["castbonus_flask_of_" + subClass])*10
    - CInt(dtCastPower["castmalus_flask_of_all"])*10
    - CInt(dtCastPower["castmalus_flask_of_" + class])*10
    - CInt(dtCastPower["castmalus_flask_of_" + subClass])*10;

  // Artefakte (temporäre Sachen)
  var bonusPowerArtefact :=
    CInt(dtCastPower["castbonus_artefact_all"])
    + CInt(dtCastPower["castbonus_artefact_" + class])
    + CInt(dtCastPower["castbonus_artefact_" + subClass])
    - CInt(dtCastPower["castmalus_artefact_all"])
    - CInt(dtCastPower["castmalus_artefact_" + class])
    - CInt(dtCastPower["castmalus_artefact_" + subClass]);

  // Fische
  var bonusPowerFish :=
    CInt(dtCastPower["castbonus_fish_all"])
    + CInt(dtCastPower["castbonus_fish_" + class])
    + CInt(dtCastPower["castbonus_fish_" + subClass])
    - CInt(dtCastPower["castmalus_fish_all"])
    - CInt(dtCastPower["castmalus_fish_" + class])
    - CInt(dtCastPower["castmalus_fish_" + subClass]);

  // Alchemie
  var bonusPowerAlchemie :=
    CInt(dtCastPower["castbonus_alchemie_all"])
    + CInt(dtCastPower["castbonus_alchemie_" + class])
    + CInt(dtCastPower["castbonus_alchemie_" + subClass])
    - CInt(dtCastPower["castmalus_alchemie_all"])
    - CInt(dtCastPower["castmalus_alchemie_" + class])
    - CInt(dtCastPower["castmalus_alchemie_" + subClass]);

  // Musik
  var bonusPowerMusic :=
    CInt(dtCastPower["castbonus_music_all"])
    + CInt(dtCastPower["castbonus_music_" + class])
    + CInt(dtCastPower["castbonus_music_" + subClass])
    - CInt(dtCastPower["castmalus_music_all"])
    - CInt(dtCastPower["castmalus_music_" + class])
    - CInt(dtCastPower["castmalus_music_" + subClass]);

  // Zauber (z.b. kleiner Intmod, Segen & Co)
  var bonusPowerMagic :=
    CInt(dtCastPower["castbonus_magic_all"])
    + CInt(dtCastPower["castbonus_magic_" + class])
    + CInt(dtCastPower["castbonus_magic_" + subClass])
    - CInt(dtCastPower["castmalus_magic_all"])
    - CInt(dtCastPower["castmalus_magic_" + class])
    - CInt(dtCastPower["castmalus_magic_" + subClass]);

  // Kleidung (Magierhut etc.)
  var bonusPowerClothing :=
    CInt(dtCastPower["castbonus_clothing_all"])
    + CInt(dtCastPower["castbonus_clothing_" + class])
    + CInt(dtCastPower["castbonus_clothing_" + subClass])
    - CInt(dtCastPower["castmalus_clothing_all"])
    - CInt(dtCastPower["castmalus_clothing_" + class])
    - CInt(dtCastPower["castmalus_clothing_" + subClass]);

  // Ausschlusswerte Checken
  If (spellspower[spellid].Magie)
    bonusPowerMagic := 0;
  EndIf
  If (spellspower[spellid].Alchemie)
    bonusPowerAlchemie := 0;
  EndIf
  If (spellspower[spellid].Fische)
    bonusPowerFish := 0;
  EndIf
  If (spellspower[spellid].Musik)
    bonusPowerMusic := 0;
  EndIf
  If (spellspower[spellid].Flakons)
    bonusPowerFlask := 0;
  EndIf
  If (spellspower[spellid].Artefakte)
    bonusPowerArtefact := 0;
  EndIf
  If (spellspower[spellid].Kleidung)
    bonusPowerClothing := 0;
  EndIf

  // Check das die Bonuswerte ihre Maxwerte einhalten
  var bonusPowerArteFlask := Min(100, bonusPowerFlask + bonusPowerArtefact);
  var bonusPowerMusikFish := Min(100, bonusPowerFish + bonusPowerMusic);
  bonusPowerAlchemie      := Min(100, bonusPowerAlchemie);
  bonusPowerMagic         := Min(100, bonusPowerMagic);
  bonusPowerClothing      := Min(100, bonusPowerClothing);

  var CastPowerModifier :=
    0.15 * bonusPowerArteFlask +
    0.15 * bonusPowerMusikFish +
    0.20 * bonusPowerAlchemie +
    0.50 * bonusPowerMagic +
    0.50 * bonusPowerClothing;
  CastPowerModifier /= 100.0;
  basePower *= (CastPowerModifier+1);
  basePower := Min(300, basePower);
  return (CInt(basePower));  // und Wert zurück
EndFunction


/******************************
 * GetSpellDifficultyPergon() *
 ******************************/

Function GetSpellDifficultyPergon (spellid)

  var spells;      // spells.cfg

  spells := ReadConfigFile("::spells");       // load spells.cfg

  If (spells[spellid])                         // return spelldifficulty
    return(CInt(spells[spellid].Skill));
  Else
    return 0;
  EndIf
EndFunction

/****************
 * GetSpellId() *
 ****************/

Function GetSpellId(magicitem)

  var itemdesc := ReadConfigFile(":newmagic:itemdesc");
  If (itemdesc)
    return(itemdesc[magicitem.objtype].spellid);
  Else
    SysLog("GetSpellID: ConfigFile nicht gefunden!");
    return(-1);
  EndIf

EndFunction

/**********************
 * CheckReagsPergon() *
 **********************/

Function CheckReagsPergon (who, spellid)

  If(GetObjProperty(who, I_AM_GOD))              // Götter brauchen keine Reags
    return 1;
  EndIf

  var spells;      // spells.cfg
  var scroll;
  var reagenteng;  // english reagent name
  var reagtype;    // objtype of reagent
  var found;       // If reag is found in backpack
  var pronom;      // german pronom For errortext
  var cfg;         // artefacts.cfg Eintrag oder spells.cfg Eintrag

  If (spellid < 1000)
    spells := ReadConfigFile("::spells");         // load spells.cfg
    cfg := spells[spellid];
  ElseIf ((spellid > 1000) && (spellid < 10000))
    spellid-= 1000;
    spells := ReadConfigFile("::spells");         // load spells.cfg
    cfg := spells[spellid];
    scroll := 1;
  ElseIf(spellid >= 10000)
    spellid -= 10000;             // Wiederherstellen der tatsächlichen ArtefaktID
    spells := ReadConfigFile(":newinscription:artefacts");
    cfg := spells[spellid];
  EndIf

  ForEach reagent in GetConfigStringArray(cfg, "PergonReag")  // search For reags in backpack and check If available
    pronom := "ein ";
    Case (lower(reagent))
      "alraune":           reagtype := UOBJ_ALRAUNE; pronom:= "eine "; reagenteng:="Mandrake Root";
      "bimsstein":         reagtype := UOBJ_BIMSSTEIN; reagenteng:="Pumice";
      "blackrock":         reagtype := UOBJ_BLACKROCK; reagenteng:="Blackrock";
      "blutegel":          reagtype := UOBJ_BLUTEGEL; reagenteng:="Bloodspawn";
      "blutflakon":        reagtype := UOBJ_BLUTFLAKON; reagenteng:="Blood Vial";
      "blutmoos":          reagtype := UOBJ_BLUTMOOS; reagenteng:="Blood Moss";
      "daemonenknochen":   reagtype := UOBJ_DAEMONENKNOCHEN; reagenteng:="Daemon's Bone";
      "drachenblut":       reagtype := UOBJ_DRACHENBLUT; reagenteng:="Dragon's Blood";
      "fledermausfluegel": reagtype := UOBJ_FLEDERMAUSFLUEGEL; reagenteng:="Batwing";
      "feuerstein":        reagtype := UOBJ_FEUERSTEIN; reagenteng:="Flintstone";
      "ginseng":           reagtype := UOBJ_GINSENG; reagenteng:="Ginseng";
      "humus":             reagtype := UOBJ_HUMUS; reagenteng:="Fertile Dirt";
      "knoblauch":         reagtype := UOBJ_KNOBLAUCH; reagenteng:="Garlic";
      "knochen":           reagtype := UOBJ_KNOCHEN; reagenteng:="Bone";
      "molchauge":         reagtype := UOBJ_MOLCHAUGE; reagenteng:="Newteye";
      "morchel":           reagtype := UOBJ_MORCHEL; pronom:= "eine "; reagenteng:="Executioner's Cap";
      "obsidian":          reagtype := UOBJ_OBSIDIAN; reagenteng:="Obsidian";
      "roheisen":          reagtype := UOBJ_ROHEISEN; reagenteng:="Pig Iron";
      "schlangenschuppe":  reagtype := UOBJ_SCHLANGENSCHUPPE; pronom:= "eine "; reagenteng:="Serpent's Scale";
      "schwarze perle":    reagtype := UOBJ_SCHWARZE_PERLE; pronom:= "eine "; reagenteng:="Black Pearl";
      "schwefelasche":     reagtype := UOBJ_SCHWEFELASCHE; pronom:= "eine "; reagenteng:="Sulphurous Ash";
      "spinnenseide":      reagtype := UOBJ_SPINNENSEIDE; pronom:= "eine "; reagenteng:="Spider's Silk";
      "tollkirsche":       reagtype := UOBJ_TOLLKIRSCHE; pronom:= "eine "; reagenteng:="Nightshade";
      "torf":              reagtype := UOBJ_TORF; reagenteng:="Blackmoor";
      "vulkanasche":       reagtype := UOBJ_VULKANASCHE; pronom:= "eine "; reagenteng:="Volcanic Ash";
      "wurzel":            reagtype := UOBJ_WURZEL; pronom:= "eine "; reagenteng:="Dead Wood";
      "wyrmherz":          reagtype := UOBJ_WYRMHERZ; reagenteng:="Wyrm's Heart";
    EndCase

    found := 0;
    If (FindSubstance( who.backpack, reagtype, 1, 0 ))
      found := 1;
    EndIf

    //If (!artefact && !scroll && GetCastScrollInBackPack(who, spellID, skill))    // wenn Zauberrolle im Backpack dann brauchts keine Reags
    //       found := 1;
    //return 1;
    //EndIf

    If (!found)      // no reagent found
      SendSysMessagePergon(who, "Euch fehlt zumindest " + pronom + reagent + " zum Zaubern dieses Spruches",
                                "You need at least one " + reagenteng + " to cast this spell");
      PlaySoundEffect(who, SFX_2C);
      return 0;
    EndIf
  EndForEach

  return 1;

EndFunction

/************************
 * ConsumeReagsPergon() *
 ************************/

Function ConsumeReagsPergon (who, spellid)

  If(GetObjProperty(who, I_AM_GOD))              // Götter brauchen keine Reags
    return 1;
  EndIf

  var spells;    // spells.cfg
  var reagtype;  // objtype of reagent
  var cfg;       // artefact.cfg Eintrag oder spells.cfg Eintrag

  If(spellid < 1000)
    spells := ReadConfigFile("::spells");         // load spells.cfg
    cfg := spells[spellid];
  ElseIf(spellid >= 1000)
    spellID -= 10000;               // Wiederherstellung der tatsächlichen ArtefaktID
    spells := ReadConfigFile(":newinscription:artefacts");
    cfg := spells[spellid];
  EndIf

  ForEach reagent in GetConfigStringArray(cfg, "PergonReag")   // search For reags in backpack and consume
    Case (lower(reagent))
      "alraune":           reagtype := UOBJ_ALRAUNE;
      "bimsstein":         reagtype := UOBJ_BIMSSTEIN;
      "blackrock":         reagtype := UOBJ_BLACKROCK;
      "blutegel":          reagtype := UOBJ_BLUTEGEL;
      "blutflakon":        reagtype := UOBJ_BLUTFLAKON;
      "blutmoos":          reagtype := UOBJ_BLUTMOOS;
      "daemonenknochen":   reagtype := UOBJ_DAEMONENKNOCHEN;
      "drachenblut":       reagtype := UOBJ_DRACHENBLUT;
      "feuerstein":        reagtype := UOBJ_FEUERSTEIN;
      "fledermausfluegel": reagtype := UOBJ_FLEDERMAUSFLUEGEL;
      "ginseng":           reagtype := UOBJ_GINSENG;
      "humus":             reagtype := UOBJ_HUMUS;
      "knoblauch":         reagtype := UOBJ_KNOBLAUCH;
      "knochen":           reagtype := UOBJ_KNOCHEN;
      "molchauge":         reagtype := UOBJ_MOLCHAUGE;
      "morchel":           reagtype := UOBJ_MORCHEL;
      "obsidian":          reagtype := UOBJ_OBSIDIAN;
      "roheisen":          reagtype := UOBJ_ROHEISEN;
      "schlangenschuppe":  reagtype := UOBJ_SCHLANGENSCHUPPE;
      "schwarze perle":    reagtype := UOBJ_SCHWARZE_PERLE;
      "schwefelasche":     reagtype := UOBJ_SCHWEFELASCHE;
      "spinnenseide":      reagtype := UOBJ_SPINNENSEIDE;
      "tollkirsche":       reagtype := UOBJ_TOLLKIRSCHE;
      "torf":              reagtype := UOBJ_TORF;
      "vulkanasche":       reagtype := UOBJ_VULKANASCHE;
      "wurzel":            reagtype := UOBJ_WURZEL;
      "wyrmherz":          reagtype := UOBJ_WYRMHERZ;
    EndCase

    var items := FindSubstance( who.backpack, reagtype, 1, 0 );
    If (items)
      SubtractAmount(items[1], 1);
    EndIf
  EndForEach

  return 1;

EndFunction


/********************
 * CheckPartsPergon *
 ********************/

Function CheckPartsPergon (who, artefactID)

  If(GetObjProperty(who, I_AM_GOD))              // Götter brauchen keine Reags
    return 1;
  EndIf

  var itemdesc := ReadConfigFile("::itemdesc");
  var artefacts := ReadConfigFile(":newinscription:artefacts");        // load spells.cfg
  var configParts := GetConfigStringArray(artefacts[artefactID], "PergonPart");

  var parts := {};                 // Liste aller Reags; grösse soviel wie verschiedene Reags
  var partsPart := {};             // Liste einer Reag: grösse soviel wie gebrauchte Stacks im Backpack
  var partsPartPart := {};         // ID eines Reagsstacks mit Anzahl

  ForEach part in configParts   // für jede Reagenzie
    var reag := SplitWords(part)[1];                         // was ist es?
    var amount := CInt(SplitWords(part)[2]);                 // und wieviel?
    var reagObjtype;
    partsPart := {};              // Initialisierung

    Case (lower(reag))           // Liste aller möglichen ArtefaktReags - für alles was nicht drinsteht wird automatisch Drachenblut genutzt!!

      "schaedel":              reagObjtype := UOBJ_SCHAEDEL;
      "trollschaedel":         reagObjtype := UOBJ_TROLLSCHAEDEL;
      "daemonenschaedel":      reagObjtype := UOBJ_DAEMONENSCHAEDEL;
      "drachenschaedel":       reagObjtype := UOBJ_DRACHENSCHAEDEL;
      "zentaurenschaedel":     reagObjtype := UOBJ_ZENTAURENSCHAEDEL;
      "trollknochen":          reagObjtype := UOBJ_TROLLKNOCHEN;
      "drachenknochen":        reagObjtype := UOBJ_DRACHENKNOCHEN;
      "zentaurenknochen":      reagObjtype := UOBJ_ZENTAURENKNOCHEN;
      "beutel":                reagObjtype := UOBJ_BEUTEL;
      "rucksack":              reagObjtype := UOBJ_RUCKSACK;
      "kiste":                 reagObjtype := UOBJ_KISTE;
      "truhe":                 reagObjtype := UOBJ_TRUHE;
      "beutel":                reagObjtype := UOBJ_BEUTEL;
      "papierrolle":           reagObjtype := UOBJ_RAWPAPER;
      "bandage":               reagObjtype := UOBJ_BANDAGE;
      "blutigebandage":        reagObjtype := UOBJ_BLUTIGEBANDAGE;
      "schneeball":            reagObjtype := UOBJ_SCHNEEBALL;
      "honig":                 reagObjtype := UOBJ_HONIG;
      "bienenwachs":           reagObjtype := UOBJ_BIENENWACHS;
      "eierschale":            reagObjtype := UOBJ_EIERSCHALE;
      "fischfilet":            reagObjtype := UOBJ_FISCHFILET;
      "kopf":                  reagObjtype := UOBJ_KOPF;
      "gehirn":                reagObjtype := UOBJ_GEHIRN;
      "bein1":                 reagObjtype := UOBJ_BEIN1;
      "bein2":                 reagObjtype := UOBJ_BEIN2;
      "herz":                  reagObjtype := UOBJ_HERZ;
      "leber":                 reagObjtype := UOBJ_LEBER;
      "recallrune":            reagObjtype := UOBJ_RECALLRUNE;

      "lehm":                  reagObjtype := UOBJ_CLAY;
      "sand":                  reagObjtype := UOBJ_SAND;
      "sandstein":             reagObjtype := UOBJ_SANDSTONE;
      "marmor":                reagObjtype := UOBJ_MARBLESTONE;
      "granit":                reagObjtype := UOBJ_GRANITSTONE;

      "gold":                  reagObjtype := UOBJ_GOLD_COIN;
      "goldbarren":            reagObjtype := UOBJ_GOLD_BARREN;
      "glasbarren":            reagObjtype := UOBJ_GLASS;
      "obsidianbarren":        reagObjtype := UOBJ_OBSIDIAN_BARREN;
      "titanobsidianbarren":   reagObjtype := UOBJ_TITAN_OBSIDIAN_BARREN;

      "hydriumbarren":         reagObjtype := UOBJ_HYDRIUM_BARREN;
      "lavariumbarren":        reagObjtype := UOBJ_LAVARIUM_BARREN;
      "teradiumbarren":        reagObjtype := UOBJ_TERADIUM_BARREN;
      "teratumbarren":         reagObjtype := UOBJ_TERATUM_BARREN;
      "erbiumbarren":          reagObjtype := UOBJ_ERBIUM_BARREN;
      "energiumbarren":        reagObjtype := UOBJ_ENERGIUM_BARREN;

      "ebenholz":              reagObjtype := ITEM_EBENHOLZ;
      "elfenbeinholz":         reagObjtype := ITEM_ELFENBEINHOLZ;

      //      "schwacherheiltrank":    reagObjtype := UOBJ_SCHWACHERHEILTRANK;
      //      "heiltrank":             reagObjtype := UOBJ_HEILTRANK;
      //      "grosserheiltrank":      reagObjtype := UOBJ_GROSSERHEILTRANK;
      //      "schwachesgift":         reagObjtype := UOBJ_SCHWACHESGIFT;
      //      "gift":                  reagObjtype := UOBJ_GIFT;
      //      "starkesgift":           reagObjtype := UOBJ_STARKESGIFT;
      //      "toedlichesgift":        reagObjtype := UOBJ_TOEDLICHESGIFT;
      //      "kleinesprengflasche":   reagObjtype := UOBJ_KLEINESPRENGFLASCHE;
      //      "sprengflasche":         reagObjtype := UOBJ_SPRENGFLASCHE;
      //      "grossesprengflasche":   reagObjtype := UOBJ_GROSSESPRENGFLASCHE;

      "sternsaphir":           reagObjtype := UOBJ_STERNSAPHIR;
      "smaragd":               reagObjtype := UOBJ_SMARAGD;
      "saphir":                reagObjtype := UOBJ_SAPHIR;
      "rubin":                 reagObjtype := UOBJ_RUBIN;
      "zitrin":                reagObjtype := UOBJ_ZITRIN;
      "amethyst":              reagObjtype := UOBJ_AMETHYST;
      "turmalin":              reagObjtype := UOBJ_TURMALIN;
      "bernstein":             reagObjtype := UOBJ_BERNSTEIN;
      "diamant":               reagObjtype := UOBJ_DIAMANT;

      "daemonenblut":          reagObjtype := UOBJ_DAEMONENBLUT;
      "trollblut":             reagObjtype := UOBJ_TROLLBLUT;

      "feenstaub":             reagObjtype := UOBJ_FEENSTAUB;
      "kristallsplitter":      reagObjtype := UOBJ_KRISTALLSPLITTER;
      "succubusfluegel":       reagObjtype := UOBJ_SUCCUBUSFLUEGEL;
      "nagaschuppe":           reagObjtype := UOBJ_NAGASCHUPPE;
      "weihwasser":            reagObjtype := UOBJ_WEIHWASSER;

      "feuerkraut":            reagObjtype := UOBJ_FEUERKRAUT;
      "steinkraut":            reagObjtype := UOBJ_STEINKRAUT;
      "nachtkraut":            reagObjtype := UOBJ_NACHTKRAUT;
      "sumpfkraut":            reagObjtype := UOBJ_SUMPFKRAUT;

      "eisball":               reagObjtype := UOBJ_EISBALL;
      "magischermeteorit":     reagObjtype := UOBJ_MAGISCHERMETEORIT;
      "magischermeteor":       reagObjtype := UOBJ_MAGISCHERMETEOR;
      "magischerkomet":        reagObjtype := UOBJ_MAGISCHERKOMET;
      "magischerasteroid":     reagObjtype := UOBJ_MAGISCHERASTEROID;

      "kraeuterbandage":       reagObjtype := UOBJ_KRAEUTERBANDAGE;
      "sterilebandage":        reagObjtype := UOBJ_STERILEBANDAGE;
      "gesegnetebandage":      reagObjtype := UOBJ_GESEGNETEBANDAGE;
      "heiligebandage":        reagObjtype := UOBJ_HEILIGEBANDAGE;

      "streitkolben":          reagObjtype := UOBJ_STREITKOLBEN;
      "barbarenaxt":           reagObjtype := UOBJ_BARBARENAXT;
      "speer":                 reagObjtype := UOBJ_SPEER;
      "dolch":                 reagObjtype := UOBJ_DOLCH;
      "magierstab":            reagObjtype := UOBJ_MAGIERSTAB;
      "schwert":               reagObjtype := UOBJ_SCHWERT;

      "alraune":               reagObjtype := UOBJ_ALRAUNE;
      "bimsstein":             reagObjtype := UOBJ_BIMSSTEIN;
      "blackrock":             reagObjtype := UOBJ_BLACKROCK;
      "blutegel":              reagObjtype := UOBJ_BLUTEGEL;
      "blutflakon":            reagObjtype := UOBJ_BLUTFLAKON;
      "blutmoos":              reagObjtype := UOBJ_BLUTMOOS;
      "daemonenknochen":       reagObjtype := UOBJ_DAEMONENKNOCHEN;
      "drachenblut":           reagObjtype := UOBJ_DRACHENBLUT;
      "feuerstein":            reagObjtype := UOBJ_FEUERSTEIN;
      "fledermausfluegel":     reagObjtype := UOBJ_FLEDERMAUSFLUEGEL;
      "ginseng":               reagObjtype := UOBJ_GINSENG;
      "humus":                 reagObjtype := UOBJ_HUMUS;
      "knoblauch":             reagObjtype := UOBJ_KNOBLAUCH;
      "knochen":               reagObjtype := UOBJ_KNOCHEN;
      "molchaugen":            reagObjtype := UOBJ_MOLCHAUGE;
      "obsidian":              reagObjtype := UOBJ_OBSIDIAN;
      "roheisen":              reagObjtype := UOBJ_ROHEISEN;
      "schlangenschuppe":      reagObjtype := UOBJ_SCHLANGENSCHUPPE;
      "schwarze_perle":        reagObjtype := UOBJ_SCHWARZE_PERLE;
      "schwefelasche":         reagObjtype := UOBJ_SCHWEFELASCHE;
      "spinnenseide":          reagObjtype := UOBJ_SPINNENSEIDE;
      "tollkirsche":           reagObjtype := UOBJ_TOLLKIRSCHE;
      "torf":                  reagObjtype := UOBJ_TORF;
      "vulkanasche":           reagObjtype := UOBJ_VULKANASCHE;
      "wurzel":                reagObjtype := UOBJ_WURZEL;
      "wyrmherz":              reagObjtype := UOBJ_WYRMHERZ;

      default:    reagObjtype := UOBJ_DRACHENBLUT;           // warum Drachenblut? einfach so!
                  SysLog("FEHLER: In artefacts.cfg ist eine unbekannter ArtefactPart enthalten!");
    EndCase

    reag := PartsEnding(itemdesc[reagObjtype].desc, amount);

    ForEach item in FindSubstance(who.backpack, reagObjtype, 1, 0, FINDSUBSTANCE_FIND_ALL)//EnumerateItemsInContainer(who.backpack) // Backpack durchsuchen
      SleepMS(2);
      var containerleer := 1;
      // Container mit Inhalt darf nicht genutzt werden,
      // da dieser sonst zerstört würde)
      If (
        item.isa(POLCLASS_CONTAINER) and
        EnumerateItemsInContainer(item).size() >= 1
      )
        containerleer := 0;
      EndIf

      var except := 0;
      If (GetObjProperty(item, "status_except"))            // exceptstücke werden nicht genommen
        except := 1;
      EndIf

      If (containerleer && !except)   // gefunden?
        partsPartPart[1] := item;
        // Stack grösser als das was gebraucht wird
        // dann nur soviel merken wie noch gebraucht werden
        partsPartPart[2] := Min(amount, item.amount);

        partsPart.append(partsPartPart);                    // dann merken
        amount-= item.amount;                      // und vom insgesamt gebrauchten abziehen
      EndIf

      If (amount <= 0)          // genug Teile gefunden?
        break;                  // Rucksack nicht weiter nach diesem Teil durchsuchen
      EndIf
    EndForEach

    If (amount > 0)             // wenn nicht genug Teile vorhanden sind abbrechen
      If (amount == 1)
        SendSysMessagepergon(who, "Euch fehlt " + amount + " " +  reag);
      Else
        SendSysMessagepergon(who, "Euch fehlen " + amount + " " +  reag);
      EndIf
      return 0;
    EndIf

    parts.append(partsPart);    // Teile merken
  EndForEach
  return (parts);                    // Liste aller Reags zurückgeben für z.b. ConsumePartsPergon
EndFunction


/*********************
 * ConsumePartsPergon() *
 *********************/

Function ConsumePartsPergon (who, parts)

  If(GetObjProperty(who, I_AM_GOD))              // Götter brauchen keine Reags
    return 1;
  EndIf

  ForEach partsPart in parts
    ForEach partsPartPart in partsPart
      SubtractAmount(partsPartPart[1], partsPartPart[2]);
    EndForEach
  EndForEach
EndFunction



/*********************
 * CheckManaPergon() *
 *********************/

Function CheckManaPergon (who, spellid)

  If(GetObjProperty(who, I_AM_GOD))              // Götter brauchen kein Mana
    return 1;
  EndIf

  var mana;                       // required mana
  var difficulty;                 // spell/artefact difficulty

  If (spellid < 1000)                       // Zauberspruch
    var spells;    // spells.cfg
    spells := ReadConfigFile("::spells");       // load spells.cfg
    mana := CInt(spells[spellid].Mana);        // read required mana
    difficulty := CInt(spells[spellid].skill * 2/3);      // read required skill
  ElseIf(spellID >= 1000)                  // Artefakt
    var artefacts := ReadConfigFile(":newinscription:artefacts");
    mana := GetConfigInt(artefacts[spellid - 10000], "Mana");
    difficulty := CInt(artefacts[spellid-10000].skill * 2/3);
  EndIf

  // Check If caster succesful channels mana
  If (GetSkillPergon(who, SKILLID_SPRUCHZAUBEREI))   // check If caster could channel mana
    // calc reduced mana costs according to casters channeling skill
    var manasaved := CInt(GetSkillPergon(who, SKILLID_SPRUCHZAUBEREI) / 80.0 * SAVE_MANA_AMNT * mana);
    If ((manasaved<1) And (mana>1)) // Bissl was sollten wir schon sparen
      manasaved := 1;
    EndIf
    If (manasaved>=mana) // Naja, zu viel aber auch wieder nich
      manasaved := mana-1;
    EndIf

    If (GetManaPergon(who) < mana - manasaved)      // check If caster has enough mana in Case of channeling
      SendSysMessagePergon(who, "Ihr habt nicht genug Mana fuer diesen Spruch",
                                "You need more Mana to cast this spell");
      PlaySoundEffect(who, SFX_2C);
      return 0;
    Else
      If (CheckSkillPergon(who, SKILLID_SPRUCHZAUBEREI, difficulty, 2*difficulty))   // check If caster channels
        mana-=manasaved;        // reduce mana costs
        SendSysMessagePergon(who, "Ihr spart "+manasaved+" Mana.", "You save "+manasaved+" Mana.");
      EndIf
    EndIf
  EndIf

  If (GetManaPergon(who) < mana)               // check mana
    SendSysMessagePergon(who, "Ihr habt nicht genug Mana fuer diesen Spruch",
                              "You need more Mana to cast this spell");
    PlaySoundEffect(who, SFX_2C);
    return 0;
  EndIf

  return mana;

EndFunction

/***********************
 * ConsumeManaPergon() *
 ***********************/

Function ConsumeManaPergon (who, spellid, mana := 0)

  If(GetObjProperty(who, I_AM_GOD))              // Götter brauchen kein Mana
    return 1;
  EndIf

  var spells;      // spells.cfg

  If (!mana)                         // read required mana, If none is given
    spells := ReadConfigFile("::spells");      // load spells.cfg
    mana := GetConfigInt(spells[spellid], "Mana");       // read mana
  EndIf

  SetManaPergon(who, GetManaPergon(who)-mana); // consume mana

  return 1;

EndFunction


/**********************
 * CheckKarmaPergon() *
 **********************/
Function CheckKarmaPergon(who, karma)
  If(GetObjProperty(who, I_AM_GOD))
    // Götter brauchen kein Karma
    return 1;
  EndIf

  If (karma == 0)       // Elemags können immer zaubern
    return 1;
  EndIf

  If ((CInt(GetObjProperty(who, "karma")) <> karma))
    PlaySoundEffect(who, SFX_163);
    SendSysMessagePergon(who,
      "Fuer diesen Zauber fehlt Euch das noetige Karma",
      "You lack the necessary Karma For this spell"
    );
    return 0;
  EndIf

  return 1;
EndFunction

/**********************
 * CheckMagicSkillPergon() *
 **********************/

Function CheckMagicSkillPergon(who, spellid, skill)
  var spells := ReadConfigFile("::spells");                         // load spelldata
  var difficulty := CInt(spells[spellid].skill);

  If (!CheckSkillPergon(who, skill, difficulty, 3*difficulty))       // SkillCheck
    var SpellDesc := spells[spellid].Desc;
    SpellDesc["Spruch"] := "Spruches";
    SendSysMessagePergon(who, "Ihr versagt beim Zaubern des " + SpellDesc + "", "You fail to cast");
    PlayObjectCenteredEffect(who, FX_SPELL_FAIL, 5, 15);
    PlaySoundEffect(who, SFX_2C);
    return 0;
  EndIf

  return 1;

EndFunction


/**********************
 * PowerWordsPergon() *
 **********************/

Function PowerWordsPergon (who, spellid, skill)

  var spells;      // spells.cfg
  var class;     // spellclass For different 'speaking styles'

  spells := ReadConfigFile("::spells");       // load spells.cfg

  Case (skill)                       // choose class
    SKILLID_MAGIE:    class := "PowerMage";
    SKILLID_SEGNEN:   class := "PowerCleric";
    SKILLID_FLUCHEN:  class := "PowerNecro";
    default:          return 0;
  EndCase

  If (who.isa(POLCLASS_NPC))
    PrintTextAbove(who, GetConfigString(spells[spellid], class));
  Else
    PrintTextAbovePergon(who, who, GetConfigString(spells[spellid], class), GetConfigString(spells[spellid], class));    // utter words
  EndIf

  return 1;

EndFunction

/******************
 * CanCastSpell() *
 ******************/

Function CanCastSpell (who, spellid, skill)

  If(GetObjProperty(who, I_AM_GOD))              // Götter können alles
    return 1;
  EndIf

  var spell := CInt(spellid);

  If (!CanCast(who, spellid))
    return 0;             // check If equipeditem blocks casting -> spellrestrictions
  EndIf

  Case (skill)             // check If spell belongs to skill used For casting
    SKILLID_MAGIE:    If (!(((400<spell) And (spell<500)) Or ((100<spell) And (spell<200))))
                        SendSysMessagePergon(who, "Als Magier koennt Ihr diesen Zauber nicht sprechen",
                                                  "You cannot cast this spell as a Mage");
                        return 0;
                      EndIf
    SKILLID_SEGNEN:   If (!(((400<spell) And (spell<500)) Or ((200<spell) And (spell<300))))
                        SendSysMessagePergon(who, "Als Priester koennt Ihr diesen Zauber nicht sprechen",
                                                  "You cannot cast this spell as a Cleric");
                        return 0;
                      EndIf
    SKILLID_FLUCHEN:  If (!(((400<spell) And (spell<500)) Or ((300<spell) And (spell<400))))
                        SendSysMessagePergon(who, "Als Necromant koennt Ihr diesen Zauber nicht sprechen",
                                                  "You cannot cast this spell as a Necromancer");
                        return 0;
                      EndIf
    default:          return 0;
  EndCase

  If (GetSkillPergon(who, skill) + 20 < GetSpellDifficultyPergon(spellid)) // check If char has enough skill
    SendSysMessagePergon(who, "Ihr seid nicht begabt genug fuer diesen Spruch",
                              "You need more talent to cast this spell");
    PlaySoundEffect(who, SFX_2C);
    return 0;
  EndIf


  var karma := 0;                              // default: Elemag hat kein Karma
  If ((spell>=300) && (spell<400))            // Necro?
    karma := -1;
  ElseIf ((spell>=200) && (spell<300))        // Priester?
    karma := 1;
  EndIf

  If (!CheckKarmaPergon(who, karma))
    return 0;
  EndIf

  var scroll := GetCastScrollInBackpack(who, spellid, skill);
  var uses   := GetObjProperty(who, "#UsesScroll");

  If (scroll && (!uses || uses < ReadGameClock()))     // aktvierte Zauberrolle in Backpack und delay abgelaufen ?
    return -1;             // dann brauch mer keine Mana & Reagcheck
  EndIf

  If (!CheckReagsPergon(who, spellid))                         // check If enough reags available
    return 0;
  EndIf

  var mana := CheckManaPergon(who, spellid);                        // check If enough mana available
  If (!mana)
    return 0;
  EndIf

  return mana;

EndFunction

/***************
 * CastSpell() *
 ***************/

Function CastSpell(who, spellid, skill, partymagic:=0)
  If (!GetObjProperty(who, "#CastSpell")) // Zaubert er schon? Sprich, Mehrfachaufruf als Bugnutzung
    SetObjProperty(who, "#CastSpell", 1);

    If (PreCastSpell(who, spellid, skill))

      var spells := ReadConfigFile("::spells");        // load spells.cfg

      var params := {};                      // parameters For spell
      params[1] := who;
      params[2] := skill;

      Detach();
      Start_ScriptPergon(":newspells:"+spells[spellid].Script, params); // calling spelleffect

      EraseObjProperty(who, "#CastSpell");
      return (1);
    EndIf

    EraseObjProperty(who, "#CastSpell");
  Else
    SendSysMessagePergon(who, "Ihr gebt euch bereits der Magie hin.");
  EndIf
  If (partymagic) // wenn partymagic client Target reservierung loswerden
    Start_ScriptPergon(":newmagic:cancelpartytarget", {who}); // cancelTarget starten
  EndIf
EndFunction

/******************
 * PreCastSpell() *
 ******************/

Function PreCastSpell(who, spellid, skill)

  var mana := CanCastSpell(who, spellid, skill);               // check If caster is able to cast spell
  If (mana == -1)                                             // aktivierte Zauberrolle im BP & delay ok?

    var scroll := GetCastScrollInBackpack(who, spellid, skill);

    SubtractAmount(scroll, 1);                                 // zerstöre Rolle
    SetObjProperty(who, "#UsesScroll", ReadGameClock() + 5);  // 5 sekunden Rollenbenutzungsdelay
    PowerWordsPergon(who, spellid, skill);                    // utter powerwords
    If (!CheckMagicSkillPergon(who, spellid, skill));         // skillcheck
      return(0);
    EndIf
    return 1;

  ElseIf (mana > 0)                                           // Mana & Reags vorhanden?
    ConsumeReagsPergon(who, spellid);                         // consume reagents
    ConsumeManaPergon(who, spellid, mana);                    // consume mana
    PowerWordsPergon(who, spellid, skill);                    // utter powerwords

    If (!CheckMagicSkillPergon(who, spellid, skill));         // skillcheck
      return(0);
    EndIf

    return (1);
  Else
    return 0;
  EndIf
EndFunction

/*****************************************
 * GetCastScrollInBackpack(who, spellid) *
 *****************************************/

Function GetCastScrollInBackpack(who, spellid, skill)

  var castScrollObjType := 0;
  //var skill := CheckSkillLevelForSkill(who, 0, SKILLTYP_BERUF);             //bester Magieskill?
  //skill := CheckSkillLevelForSkill(who, skill, SKILLTYP_HAUPT);
  //skill := CheckSkillLevelForSkill(who, skill, SKILLTYP_NEBEN);
  //skill := CheckSkillLevelForSkill(who, skill, SKILLTYP_WEITERE);

  //objtype spellid was isses   skill
  //0x6900  401     commonmage    mage
  //0x691F  432     commonmage    mage
  //0x6920  112     mage          mage
  //0x693F  132     mage          mage
  //0x6960  401     commoncleric  cleric
  //0x697F  432     commoncleric  cleric
  //0x6980  201     cleric        cleric
  //0x699F  232     cleric        cleric
  //0x69C0  401     commonnecro   necro
  //0x69DF  432     commonnecro   necro
  //0x69E0  301     necro         necro
  //0x69FF  332     necro         necro


  If ((spellid > 400) && (spellid < 500) && (skill == SKILLID_MAGIE))
    // Elemag-common
    castScrollObjType := spellid - 400 + 26879;
  ElseIf ((spellid > 400) && (spellid < 500) && (skill == SKILLID_SEGNEN))
    // Priester-common
    castScrollObjType := spellid - 400 + 26975;
  ElseIf ((spellid > 400) && (spellid < 500) && (skill == SKILLID_FLUCHEN))
    // Necro-common
    castScrollObjType := spellid - 400 + 27071;
  ElseIf ((spellid > 100) && (spellid < 200))
    // Elemag
    castScrollObjType := spellid - 100 + 26911;
  ElseIf ((spellid > 200) && (spellid < 300))
    // Priester
    castScrollObjType := spellid - 200 + 27007;
  ElseIf ((spellid > 300) && (spellid < 400))
    // Elemag
    castScrollObjType := spellid - 300 + 27103;
  EndIf

  var result:=0;
  ForEach item in FindSubstance(who.backpack, castScrollObjType, 1, 0, FINDSUBSTANCE_FIND_ALL)
    If (item.getprop("castable")==1)  // passt
      result := item;
      Break;
    EndIf
  EndForEach

  return(result);
EndFunction


/***************************************************************************
 * ReflectSpell(caster, caston, moving, fx, speed, loop, explode, soundfx) *
 ***************************************************************************/

Function ReflectSpell (caster, caston, byref dmg, fxtype, fx, speed, loop, explode, soundfx)
	
	var subdmg := dmg * 0.05;
	Repeat
		
    // play soundfx to caster of spell
    PlaySoundEffect(caster, soundfx);

    // play graphicfx to(wards) target of spell
    Case (fxtype)
      0:  PlayMovingEffect(caster, caston, fx, speed, loop, explode);
      1:  PlayObjectCenteredEffect(caston, fx, speed, loop);
      2:  PlayLightningBoltEffect(caston);
    EndCase

    // wait a little
    SleepMS(1500);
    // spell target could have died/got disconnected
    var los := CheckLineOfSight(caster, caston);
    If (!caston or caston.hidden or caston.concealed or !los)
      // spelldamage vanishes
      return 0;
    EndIf

    // test if target has magic reflect
    var castonmr := GetObjProperty(caston, "spell_reflect");
    If (!castonmr)
      // target has no magic reflect -> becomes target
      return caston;
    EndIf

    If (castonmr[2] == "single")
      // target has only 'normal' magic reflect -> reset
      EraseObjProperty(caston, "spell_reflect");
      BuffIcons(caston, "mirror_single", 0, 1);
      If (caston.IsA(POLCLASS_NPC))
        SendEvent(caston, struct{type := 0x0057});
      EndIf
    EndIf
    
    dmg -= subdmg;

    SendSysMessagePergon(caston,
      "Ihr wehrt den Spruch ab", "You reflect the spell"
    );
    // info and sound for target
    PlaySoundEffect(caston, SFX_EF);
    
    SleepMS(500);
    // spell source or target could have died/got disconnected
    If ((!caster) or (!caston))
      // spelldamage vanishes
      return 0;
    EndIf
    
    los := CheckLineOfSight(caston, caster);
    If (caster.hidden or caster.concealed or !los)
    	var newmob := NewMobile(caston, caster);
    	If (newmob)
    		caster := newmob;
    	Else
    		return 0;
    	EndIf
    EndIf
    
    // play soundfx to target of spell
    PlaySoundEffect(caston, soundfx);

    // play graphicfx to(wards) caster of spell
    Case (fxtype)
      0: PlayMovingEffect(caston, caster, fx, speed, loop, explode);
      1: PlayObjectCenteredEffect(caster, fx, speed, loop);
      2: PlayLightningBoltEffect(caster);
    EndCase
    
    SleepMS(1500);
    
    los := CheckLineOfSight(caston, caster);
    If (!caster or caster.hidden or caster.concealed or !los)
      return 0;
    EndIf

    // test if caster has magic reflect
    var castermr := GetObjProperty(caster, "spell_reflect");
    If (!castermr)
      // caster has no magic reflect -> becomes target
      return caster;
    EndIf

    If (castermr[2] == "single")
      // caster has only 'normal' magic reflect -> reset
      EraseObjProperty(caster, "spell_reflect");
      BuffIcons(caster, "mirror_single", 0, 1);
      If (caster.IsA(POLCLASS_NPC))
        SendEvent(caster, struct{type := 0x0057});
      EndIf
    EndIf
    
    dmg -= subdmg;

    SendSysMessagePergon(caster,
      "Ihr wehrt den Spruch ab", "You reflect the spell"
    ); 
    PlaySoundEffect(caster, SFX_EF);
    SleepMS(500);
    
    los := CheckLineOfSight(caster, caston);
    If (caston.hidden or caston.concealed or !los)
    	var newmob := NewMobile(caster, caston);
    	If (newmob)
    		caston := newmob;
    	Else
    		return 0;
    	EndIf
    EndIf
    
  Until (dmg <= 1);
  
  //dmg ist 0 -> kein target nötig
  return 0;
EndFunction

Function NewMobile(Mob1, Mob2)
	
  var newmob  := 0;
  var mobiles := ListMobilesInLineOfSight(Mob1, 8);
  If (mobiles)
    If (!Mob1.IsA(POLCLASS_NPC) and !Mob2.IsA(POLCLASS_NPC))
      newmob := mobiles.randomentry();
    ElseIf (Mob1.IsA(POLCLASS_NPC))
      While (mobiles.size())
        var i := RandomInt(mobiles.size())+1;
        If (!mobiles[i].IsA(POLCLASS_NPC))
          newmob := mobiles[i];
          break;
        EndIf
        mobiles.erase(i);
      EndWhile
    Else
      While (mobiles.size())
        var i := RandomInt(mobiles.size())+1;
        If (mobiles[i].IsA(POLCLASS_NPC))
          newmob := mobiles[i];
          break;
        EndIf
        mobiles.erase(i);
      EndWhile
   EndIf
   If (!newmob)
    	return 0;
    Else
     	return newmob;
    EndIf
  Else
    return 0;
  EndIf
  
EndFunction

///////////////////////////////////////////////
// PartsEnding - Plural/Singular Umformung
///////////////////////////////////////////////

Function PartsEnding(text, amount)
  If (amount>1)
    Return(DescMehrzahl(text));
  Else
    Return(DescEinzahl(text));
  EndIf
EndFunction

///////////////////////////////////
//  GetCastPowerProp(who, influenceFrom, magicClass := "", spellClass:="", bMalus:=0)
//    Holt den angegebenen Wert aus der CastPower-Property
//
//    Hinweis: magicClass ODER spellClass -> wenn beides angegeben dann wird nur magicClass genommen
//              wenn beides frei -> dann allgemeiner Bonus
//
//    Parameter: who - CharacterReferenz
//             influenceFrom - "Alchemie, Musik, Fische, Magie, Flakon, Artefakt"
//             magicClass - "Mage, Cleric, Necro"
//             spellClass - Beschreibung ist bitte der SpellConfig entnehmen
//             bMalus - 0=Bonus;1=Malus
//////////////////////////////////
Function GetCastPowerProperty(who, influenceFrom, magicClass := "", spellClass:="", bMalus:=0)

  var dtCastPower := who.getprop("MagicCastPower");
  If (!dtCastPower)
    dtCastPower := dictionary;
    who.setprop("MagicCastPower", dtCastPower);
  EndIf

  var Description := GetCastValueDesc(influenceFrom, bMalus);

  If (Description=="")
    //war irgendwas in der Beschreibung falsch -> Abbruch
    return "FEHLER IN AUFRUF";
  EndIf

  If (magicClass=="" AND spellClass=="")
    Description += "all";
  ElseIf (magicClass<>"")
    Description += magicClass;
  ElseIf (spellClass<>"")
    Description += spellClass;
  Else
    //Hier ist was komplett Schiefgelaufen -> reinlogisch nicht möglich
    Syslog("Anwendungsfehler in MagicInterface");
    return 0;
  EndIf

  return (CInt(who.getprop("MagicCastPower")[Description]));
EndFunction

///////////////////////////////////
//  SetCastPowerProp(who, influenceFrom, influenceValue, magicClass := "", spellClass:="", bMalus:=0)
//
//    Hinweis: magicClass ODER spellClass -> wenn beides angegeben dann wird nur magicClass genommen
//              wenn beides frei -> dann allgemeiner Bonus
//
//    Parameter: who - CharacterReferenz
//             influenceFrom - "Alchemie, Musik, Fische, Magie, Flakon, Artefakt"
//         influenceValue - Einflussfaktor (Integerwert)
//             magicClass - "Mage, Cleric, Necro"
//             spellClass - Beschreibung ist bitte der SpellConfig entnehmen
//             bMalus - 0=Bonus;1=Malus
///////////////////////////////////
Function SetCastPowerProperty(who, influenceFrom, influenceValue, magicClass := "", spellClass:="", bMalus:=0)

  // Wertbeschreibung zusammenbauen
  var Description := GetCastValueDesc(influenceFrom, bMalus);

  If (Description == "")
    //war irgendwas in der Beschreibung falsch -> Abbruch
    return 0;
  EndIf

  If (magicClass=="" AND spellClass=="")
    Description += "all";
  ElseIf (magicClass<>"")
    Description += lower(magicClass);
  ElseIf (spellClass<>"")
    Description += lower(spellClass);
  Else
    //Hier ist was komplett Schiefgelaufen -> reinlogisch nicht möglich
    Syslog("Anwendungsfehler in MagicInterface");
    return 0;
  EndIf
  var oldkrit := Is_Critical();
  Set_Critical(1);
  var dtCastPower := who.getprop("MagicCastPower");
  If (!dtCastPower)
    dtCastPower := dictionary;
  EndIf

  If (CInt(dtCastPower[Description])>0)
    //Hier liegt schon ein Wert drauf -> zwei mal darf nicht beeinflusst werden
    Set_Critical(oldkrit);
    return 1; //-> weil ist ja schon beeinflusst
  EndIf

  dtCastPower[Description] := influenceValue;

  who.setprop("MagicCastPower", dtCastPower);
  Set_Critical(oldkrit);
  return 1;
EndFunction


///////////////////////////////////////////
//  CastPowerPropInUse(who, influenceFrom, magicClass := "", spellClass:="", bMalus:=0)
//    überprüft ob die angegebene Eigenschaft schon gesetzt ist (erneutes setzen nicht möglich)
//
//    Hinweis: magicClass ODER spellClass -> wenn beides angegeben dann wird nur magicClass genommen
//              wenn beides frei -> dann allgemeiner Bonus
//
//    Parameter: who - CharacterReferenz
//             influenceFrom - "Alchemie, Musik, Fische, Magie, Flakon, Artefakt"
//             magicClass - "Mage, Cleric, Necro"
//             spellClass - Beschreibung ist bitte der SpellConfig entnehmen
//             bMalus - 0=Bonus;1=Malus
///////////////////////////////////////////
Function CastPowerPropertyInUse(who, influenceFrom, magicClass := "", spellClass:="", bMalus:=0)

  var dtCastPower := who.getprop("MagicCastPower");
  If (!dtCastPower)
    dtCastPower := dictionary;
  EndIf

  //Wertbeschreibung zusammenbauen
  var Description := GetCastValueDesc(influenceFrom, bMalus);

  If (Description == "")
    //war irgendwas in der Beschreibung falsch -> Abbruch
    return 0;
  EndIf

  If (magicClass=="" AND spellClass=="")
    Description += "all";
  ElseIf (magicClass<>"")
    Description += lower(magicClass);
  ElseIf (spellClass<>"")
    Description += lower(spellClass);
  Else
    //Hier ist was komplett Schiefgelaufen -> reinlogisch nicht möglich
    Syslog("Anwendungsfehler in MagicInterface");
    return 0;
  EndIf

  If (CInt(dtCastPower[Description])>0)
    //Hier liegt schon ein Wert drauf -> zwei mal darf nicht beeinflusst werden
    return 1; //-> weil ist ja schon beeinflusst
  Else
    return 0;
  EndIf
EndFunction

///////////////////////////////////
//  ModifyCastPowerProperty(who, influenceFrom, influenceValue, magicClass := "", spellClass:="", bMalus:=0)
//
//    Hinweis: magicClass ODER spellClass -> wenn beides angegeben dann wird nur magicClass genommen
//              wenn beides frei -> dann allgemeiner Bonus
//        - ist nur für Flakons freigeschaltet
//
//    Parameter: who - CharacterReferenz
//             influenceFrom - "Alchemie, Musik, Fische, Magie, Flakon, Artefakt"
//         influenceValue - Einflussfaktor (Integerwert)
//             magicClass - "Mage, Cleric, Necro"
//             spellClass - Beschreibung ist bitte der SpellConfig entnehmen
//             bMalus - 0=Bonus;1=Malus
///////////////////////////////////
Function ModifyCastPowerProperty(who, influenceFrom, influenceValue, magicClass := "", spellClass:="", bMalus:=0)

  If (!(influenceFrom in array{"Flakon", "Magie"}))
    // Addieren von Effekten nur für Flakons erlaubt
    return 1;
  EndIf

  //Wertbeschreibung zusammenbauen
  var Description := GetCastValueDesc(influenceFrom, bMalus);

  If (Description == "")
    //war irgendwas in der Beschreibung falsch -> Abbruch
    return 0;
  EndIf

  If (magicClass=="" AND spellClass=="")
    Description += "all";
  ElseIf (magicClass<>"")
    Description += lower(magicClass);
  ElseIf (spellClass<>"")
    Description += lower(spellClass);
  Else
    //Hier ist was komplett Schiefgelaufen -> reinlogisch nicht möglich
    Syslog("Anwendungsfehler in MagicInterface");
    return 0;
  EndIf

  var oldkrit := Is_Critical();
  Set_Critical(1);

  var dtCastPower := who.getprop("MagicCastPower");
  If (!dtCastPower)
    dtCastPower := dictionary;
  EndIf

  dtCastPower[Description] := CInt(dtCastPower[Description])+CInt(influenceValue);

  who.setprop("MagicCastPower", dtCastPower);

  Set_Critical(oldkrit);

  return 1;
EndFunction

// Wertbeschreibung zusammenbauen
Function GetCastValueDesc(influenceFrom, bMalus)
  var Description := "cast";

  If (bMalus)
    Description += "malus_";
  Else
    Description += "bonus_";
  EndIf

  Case (influenceFrom)
  "Alchemie":
    Description += "alchemie_";
  "Musik":
    Description += "music_";
  "Fische":
    Description += "fish_";
  "Magie":
    Description += "magic_";
  "Kleidung":
    Description += "clothing_";
  "Flakon":
    Description += "flask_of_";
  "Artefakt":
    Description += "artefact_";
  default:
    return "";
  EndCase

  return Description;
EndFunction

///////////////////////////////////
//  ClearCastPowerProp(who, influenceFrom, influenceValue, magicClass := "", spellClass:="", bMalus:=0)
//
//    Hinweis: magicClass ODER spellClass -> wenn beides angegeben dann wird nur magicClass genommen
//              wenn beides frei -> dann allgemeiner Bonus
//
//    Parameter: who - CharacterReferenz
//             influenceFrom - "Alchemie, Musik, Fische, Magie, Flakon, Artefakt"
//             magicClass - "Mage, Cleric, Necro"
//             spellClass - Beschreibung ist bitte der SpellConfig entnehmen
//             bMalus - 0=Bonus;1=Malus
///////////////////////////////////

Function ClearCastPowerProperty(who, influenceFrom, magicClass := "", spellClass:="", bMalus:=0)

  //Wertbeschreibung zusammenbauen
  var Description := "";
  If (bMalus)
    Case(influenceFrom)
      "Alchemie":
        Description := "castmalus_alchemie_";
      "Musik":
         Description := "castmalus_music_";
      "Fische":
        Description := "castmalus_fish_";
      "Magie":
        Description := "castmalus_magic_";
      "Kleidung":
        Description := "castmalus_clothing_";
      "Flakon":
        Description := "castmalus_flask_of_";
      "Artefakt":
        Description := "castmalus_artefact_";
    EndCase
  Else
    Case(influenceFrom)
      "Alchemie":
        Description := "castbonus_alchemie_";
      "Musik":
        Description := "castbonus_music_";
      "Fische":
        Description := "castbonus_fish_";
      "Magie":
        Description := "castbonus_magic_";
      "Kleidung":
        Description := "castbonus_clothing_";
      "Flakon":
        Description := "castbonus_flask_of_";
      "Artefakt":
        Description := "castbonus_artefact_";
    EndCase
  EndIf

  If (Description=="")
    //war irgendwas in der Beschreibung falsch -> Abbruch
    return 0;
  EndIf

  If (magicClass=="" AND spellClass=="")
    Description += "all";
  ElseIf (magicClass<>"")
    Description += lower(magicClass);
  ElseIf (spellClass<>"")
    Description += lower(spellClass);
  Else
    //Hier ist was komplett Schiefgelaufen -> reinlogisch nicht möglich
    Syslog("Anwendungsfehler in MagicInterface");
    return 0;
  EndIf

  var oldkrit := Is_Critical();
  Set_Critical(1);

  var dtCastPower := who.getprop("MagicCastPower");
  If (!dtCastPower)
    dtCastPower := dictionary;
  EndIf

  dtCastPower.erase(Description);

  who.setprop("MagicCastPower", dtCastPower);

  Set_Critical(oldkrit);

  return 1;
EndFunction

Function RefreshSpellPowerGump(who)
  If (who.getprop("#spellpowergump"))
    var script := GetProcess(who.getprop("#spellpowergump"));
    script.sendevent(struct{type := 1});
  EndIf
EndFunction

// vim: sw=2 sts=2
