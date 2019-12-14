///////////////////////////////////////////////////////////////////////////
// Animal Include v1.0 (2000/06/17)
//
//     Author: Shinigami

///////////////////////////////////////////////////////////////////////////
//                               Sektionen
// Riding
//   PetMount              - Auf ein Tier aufsteigen
//   MountCheckAge         - Aufsteige-Erlaubnis auf Basis von Alter etc.
//   PetDisMount           - Von einem Tier absteigen
//   PetMasterDeath        - Der Reiter ist gestorben
//   DisMountCritter       - Von einem Tier absteigen (Private)
//   GetMountType          - Liefert das Mount-Item
//   GetPetTemplate        - Liefert das Pet-Template
//
// Stat-Manipulation
//   Stats_korrigieren     - Realisiert MegaMonster
//
// Heaven
//   PetGetHeaven          - Tier aus dem Himmel holen
//   PetSetHeaven          - Tier in den Himmel verschieben
//   PetMasterLogOn        - Spieler loggt sich ein
//   PetMasterLogOff       - Spieler loggt sich aus
//
// Movement
//   WanderPergon          - NPC wandert umher, aber nicht durch Tueren
//   WanderDoorsPergon     - NPC wandert umher
//   DropAnchor            - Setzt den Anker
//   ReleaseAnchor         - Loescht den Anker
//
// HateLoveFrightHunt
//   InitHateLove          - Initialisiert die HateLove-Listen
//   CheckHateLove         - Analysiert das Gegenueber
//
// Corpses
//   GetCorpseConfig       - Liefert die Infos zum Corpse
//
// Parking
//   ParkAnimal            - Tier anbinden und ruhig stellen
//   UnparkAnimal          - Tier wird abbinden
//
// Sonstiges
//   Wait_for_EventLogging - Protokolliert nuetzliche Infos
//   GetWhatIWouldEat      - Findet heraus, was ein Tier fressen mag
//   DraggedItemToAnimal   - Tier fuettern oder Packtier beladen
//   GetFoodSaturation     - Naehrwert eines einzelnen Nahrungsitems ermitteln
//   PetTamedItem          - Liste der Items, die gezaehmten Tieren entsprechen
//   PetEnticedItem        - Liste der Items, die abgerichteten Tieren
//                           entsprechen
//   PetTemplates          - Liste der Tiertemplates, die ver-/gekauft werden
//   Tame                  - Tier abrichten

use cfgfile;
use datafile;
use math;
use os;
use uo;
include ":spawnnet_new:spawnnet";
include "include/logutil";
include "include/mobile";
include "include/poison";
include "include/resources";
include "include/server";
include "include/varutil";

// Konstanten
// ObjectTyp des NPCs {{{
Const ot_drake1          := 0x03c; // Drache (AvatarPatch)
Const ot_unicorn         := 0x07a; // Einhorn                       (LBR)
Const ot_kirin           := 0x084; // Kirin                         (LBR)
Const ot_etheral_llama   := 0x0aa; // Durchsichtiges Lama           (LBR)
Const ot_etheral_ostard  := 0x0ab; // Durchsichtiger Ostard         (LBR)
Const ot_ridgeback       := 0x0bb; // Ridgeback                     (LBR)
Const ot_savage_ridgeback:= 0x0bc; // Wilder Ridgeback              (LBR)
Const ot_horse2nightmare := 0x0c8; // Pferd (hellbraun) & Nightmare
Const ot_horse           := 0x0cc; // Pferd (dunkelbraun)
Const ot_desertostard    := 0x0d2; // Wuestenostard
Const ot_polarbear       := 0x0d5; // Eisbär
Const ot_frenziedostard  := 0x0da; // Aggressiver Ostard
Const ot_forestostard    := 0x0db; // Waldostard
Const ot_llama           := 0x0dc; // Lama
Const ot_horse3          := 0x0e2; // Pferd (leuchtend graun)
Const ot_horse4          := 0x0e4; // Pferd (graubraun)
Const ot_hai_riyo        := 0x0f3; // Hai Riyo
Const ot_chimera         := 0x114; // Chimera
Const ot_cu_sidhe        := 0x115; // Cu Sidhe
Const ot_mondain_steed   := 0x11c; // Schlachtross
Const ot_packhorse       := 0x123; // Packpferd
Const ot_packllama       := 0x124; // Pack-Lama
Const ot_giant_beatle    := 0x317; // Grosser Kaefer                (LBR)
Const ot_skeletal_mount  := 0x319; // Skelett-Stute                 (LBR)
Const ot_swamp_dragon    := 0x31a; // Sumpfdrache                   (LBR)
Const ot_ar_swamp_dragon := 0x31f; // Gepanzerter Sumpfdrache       (LBR)
// }}}

// MountTyp (Item, Layer 25) zum Aufsteigen {{{
Const mt_chimera         :=0x3e90; // Chimera                       (ML)
Const mt_cu_sidhe        :=0x3e91; // Cu Sidhe                      (ML)
Const mt_mondain_steed   :=0x3e92; // Mondain's Steed               (ML)
Const mt_hai_riyo        :=0x3e94; // Hai Riyo                      (SE)
Const mt_horse2nightmare :=0x3e9f; // Pferd (hellbraun) & Nightmare
Const mt_horse3          :=0x3ea0; // Pferd (leuchtend graun)
Const mt_horse4          :=0x3ea1; // Pferd (graubraun)
Const mt_horse           :=0x3ea2; // Pferd (dunkelbraun)
Const mt_desertostard    :=0x3ea3; // Wuestenostard
Const mt_frenziedostard  :=0x3ea4; // Aggressiver Ostard
Const mt_forestostard    :=0x3ea5; // Waldostard
Const mt_llama           :=0x3ea6; // Lama
Const mt_etheral_llama   :=0x3eab; // Durchsichtiges Lama           (LBR)
Const mt_etheral_ostard  :=0x3eac; // Durchsichtiger Ostard         (LBR)
Const mt_kirin           :=0x3ead; // Kirin                         (LBR)
Const mt_unicorn         :=0x3eb4; // Einhorn                       (LBR)
Const mt_ridgeback       :=0x3eb8; // Ridgeback                     (LBR)
Const mt_savage_ridgeback:=0x3eba; // Wilder Ridgeback              (LBR)
Const mt_skeletal_mount  :=0x3ebb; // Skelett-Stute                 (LBR)
Const mt_giant_beatle    :=0x3ebc; // Grosser Kaefer                (LBR)
Const mt_swamp_dragon    :=0x3ebd; // Sumpfdrache                   (LBR)
Const mt_ar_swamp_dragon :=0x3ebe; // Gepanzerter Sumpfdrache       (LBR)
Const mt_polarbear       :=0x3ec5; // Polarbär                      (SE)
Const mt_dragon_avatar   :=0x3f14; // Drache (AvatarPatch)
// }}}

// Lovehate {{{
Const LOVEHATE_IGNORE := 0; // Unbekannt, wird ignoriert
Const LOVEHATE_HATE   := 1; // Hasse ich und versuche es zu toeten
Const LOVEHATE_FRIGHT := 2; // Hab ich Angst vor und versuche wegzurennen
Const LOVEHATE_LOVE   := 3; // Mag ich (z.B. eigene Spezies bei Herdentieren)
Const LOVEHATE_HUNT   := 4; // Wenn ich Hunger hab, fress ich es
Const LOVEHATE_PLAYER := 5; // Spieler
Const LOVEHATE_HUMAN  := 6; // Menschlicher NPC
// }}}

// Sonstiges
Const COLOR_NIGHTMARE := 1109;     // Farbe eines Nightmare
// diverse Herbergen brauchen 25; sicherheitshalber mehr Luft lassen
Const HEAVENRANGE     := 30;       // Bereich fuer LogOn/LogOff
Const PROP_ANCHOR     := "Anchor"; // DropAnchor & Co.
// aktueller Saettigungswert, 15.0 - satt, 0.0 - stirbt vor Hunger
Const PROP_HUNGERCHK  := "hungrylastcheck";


////////////////
//            //
//   Riding   //
//            //
////////////////

// PetMount - Auf ein Vieh aufsteigen, zwecks reiten
Function PetMount(pet, master, owner)
  var mounttype := MountCheckAge(pet, master);

  var handled := 0;
  If (!mounttype)
    return handled;
  EndIf

  var dismounted := 1;

  var mount := GetEquipmentByLayer(master, 25);
  If (mount)
    dismounted := PetDisMount(master, owner);
  EndIf

  If (!dismounted)
    return handled;
  EndIf

  MoveObjectToLocation(
    master, pet.x, pet.y, pet.z, pet.realm, MOVEOBJECT_FORCELOCATION
  );
  master.facing := pet.facing;

  var oldcrit:=Is_Critical();
  Set_Critical(1);

  // TODO:
  // - statt kompliziertem Umpacken Tiere nach 5380, 1081 schieben
  //   (reservierte Zone auf GreenAcres), Mountitem nur mit Tier-Serial
  //   versehen
  // - DestroyScript bauen, daß bei Mountzerstoerung das Tier toetet
  // - ControlScript fuer Mounts bauen, dass Tier wiederherstellt, nach
  //   GreenAcres schiebt, alle Mountdaten loescht und die Serial sichert
  // (gemeinsame Funktionalitaet mit stamaster.src)

  mount := CreateItemAtLocationPergon(
    pet.x, pet.y, pet.z, 0xf021, 1, pet.realm
  );
  mount.graphic := mounttype;
  mount.color   := pet.color;

  // Chaossphäre setzt über HK Farbe
  var bless := pet.getprop("spell_bless");
  If (bless)
    If (bless[6] == "chaossphere")
      mount.color := bless[4];
      pet.eraseprop("spell_bless");
    EndIf
  EndIf
  // Grafikänderung von Chaossphäre
  If (pet.getprop("spell_poly"))
    pet.eraseprop("spell_poly");
  EndIf
  // Statuen Farbänderung
  var statue := pet.getprop("spell_statue");
  If (statue)
    mount.color := statue[4];
    pet.eraseprop("spell_statue");
  EndIf
  mount.newbie := 1;
  SetObjProperty(master, "petname",     pet.name);
  SetObjProperty(mount,  "petname",     pet.name);
  SetObjProperty(mount,  "pettemplate", pet.npctemplate);

  // Vitals ...
  var PetVitals   := struct;
  PetVitals.+Hp   := GetHPPergon(pet);
  PetVitals.+Mana := GetManaPergon(pet);
  PetVitals.+Stam := GetStaminaPergon(pet);
  SetObjProperty(mount, "petvitals", PetVitals);

  // ... einige Props ...
  var PetProps     := struct;
  PetProps.+Gender := pet.gender;
  SetObjProperty(mount, "petprops", PetProps);

  // ... und wichtige CProps des Tierchens ...
  var PetCProps := struct;
  var generation := GetObjProperty(pet, "Generation");
  If (generation)
    PetCProps.+Generation  := generation;
  EndIf
  var hunger := GetObjProperty(pet, PROP_HUNGER);
  If (hunger)
    PetCProps.+hungrylevel := hunger;
  EndIf
  var hungcheck := GetObjProperty(pet, PROP_HUNGERCHK);
  If (hungcheck)
    PetCProps.+hungrycheck := hungcheck;
  EndIf
  var colorchange := GetObjProperty(pet, PROP_CHANGE_COLOR);
  If (colorchange)
    PetCProps.+colorchange := colorchange;
  EndIf
  var companion := GetObjProperty(pet, PROP_COMPANION);
  If (companion)
    PetCProps.+companion := companion;
  EndIf
  // ... hier weitere CProps ergaenzen
  // .. poison_level ?
  SetObjProperty(mount, "petcprops", PetCProps);

  // ... sowie Privilegien sichern
  If (pet.privileges().size() >= 1)
    SetObjProperty(mount, "petprivs", pet.privileges());
  EndIf

  mount.movable := 1; // Irgendwie is das bzgl. Injection besser
  EquipItem(master, mount);
  mount.movable := 0;

  // Tierchen unsichtbar entsorgen :)
  KillNPC(pet, "pet-mount", KILLNPC_NOCORPSE);

  Set_Critical(oldcrit);

  handled := 1;

  return handled;
EndFunction

// Reitbarkeitscheck abhaengig vom Char-Alter
// return: Mount-Type, wenn reiten erlaubt, sonst 0
Function MountCheckAge(pet, master)
  var mounttype := GetMountType(pet);
  If (!mounttype)
    return 0;
  EndIf

  // Goetter und NPCs duerfen immer
  If (master.cmdlevel >= CMDLEVEL_QUESTCHAR || master.isA(POLCLASS_NPC))
    return mounttype;
  EndIf

  var guildid := pet.getprop("guildid");
  If (guildid)
    If (guildid != master.guildid)
      SendSysMessagePergon(master, "Ihr besitzt nicht das noetige Wissen dieses Gildenreittier zu reiten.");
      mounttype := 0;
    EndIf
  Else
    var petconfig := GetNPCConfig(pet.npctemplate);
    If (petconfig.noagecheck)
      // speziell markierte Tiere darf jeder reiten
      return mounttype;
    EndIf

    // Wie alt ist der Char?
    var tage := CInt((ReadGameClock() - master.createdat) / 86400);
    // mt_horse, mt_horse2nightmare, mt_horse3, mt_horse4, mt_llama - 0 Tage Anforderung
    If ((pet.npctemplate in {"DesertOstard", "DesertOstard_m", "ForestOstard", "ForestOstard_w"} and
          tage < 90) or
      ((mounttype in {mt_skeletal_mount, mt_giant_beatle} or pet.npctemplate in {"Ridgeback", "SavageRidgeback", "FrenziedOstard", "FrenziedOstard_w"}) and
          tage < 180) or
      (pet.npctemplate in {"LavaRidgeback_m", "LavaRidgeback_w", "IceOstard_m", "IceOstard_w", "LavaOstard_m", "LavaOstard_w"} and
          tage < 270) or
      (mounttype in {mt_mondain_steed, mt_cu_sidhe, mt_polarbear} and
          (tage < 360  or GetObjProperty(master, TYPNEWBIE))) or
      (mounttype in {mt_swamp_dragon, mt_ar_swamp_dragon} and
          (tage < 450 or GetObjProperty(master, TYPNEWBIE))) or
      (mounttype in {mt_hai_riyo, mt_chimera, mt_kirin} and
          (tage < 540 or GetObjProperty(master, TYPNEWBIE))) or
      (mounttype == mt_unicorn and (!master.gender or                 // Nur Frauen duerfen Einhoerner reiten...
          GetObjProperty(master, TYPNEWBIE)))
    )
      // Als Festländer kann man die Tiere reiten, die man Zähmen und Abrichten kann (Bonus für Zähmer)
      If (petconfig.zaehmskill and GetSkillPergon(master, SKILLID_ZAEHMEN) >= petconfig.zaehmskill and
        petconfig.abrichtskill and GetSkillPergon(master, SKILLID_ABRICHTEN) >= petconfig.abrichtskill and
        !GetObjProperty(master, TYPNEWBIE)
      )
        SendSysMessagePergon(master, "Ihr seid wirklich sehr geschickt im Umgang mit diesem Tier.");
      Else
        If (mounttype == mt_unicorn)
          SendSysMessagePergon(master, "Dieses edle Geschoepf akzeptiert Euch nicht als Reiter!");
        Else
          SendSysMessagePergon(master, "Ihr besitzt noch nicht die noetige Erfahrung dieses Tier zu reiten.");
        EndIf
        mounttype := 0;
      EndIf
    EndIf
  EndIf

  return mounttype;
EndFunction

// PetDisMount - Vom reitbaren Untersatz absteigen
Function PetDisMount(master, owner)
  var handled := 0;
  var mount := GetEquipmentByLayer(master, 25);
  If (mount)
    Var critter := GetPetTemplate(mount);
    If (critter <> "")
      handled := DisMountCritter(mount, critter, master, owner);
    Else
      SysLog(
        "WARNUNG: '"+master.name+
        "' sitzt auf irgendwas drauf und kann nicht absteigen!"
      );
    EndIf
  EndIf

  return handled;
EndFunction

// PetMasterDeath - Der Master ist auf dem reitbaren Untersatz gestorben
Function PetMasterDeath(ghost)
  var handled:=0;

  var mounts:={};
  ForEach item in EnumerateItemsInContainer(ghost.backpack)
    var critter:=GetPetTemplate(item);
    If (critter<>"")
      mounts.append({item, critter});
    EndIf
  EndForEach

  If (mounts.size() > 1)
    SysLog(
      "WARNUNG: PetMasterDeath - Bug bei Corpse "+ghost.name+
      " ("+(mounts.size())+" Mounts)"
    );
  EndIf

  ForEach mount in mounts
    handled +=
      DisMountCritter(mount[1], mount[2], ghost, ghost, DM_TRY_NEAR);
  EndForEach

  return handled;
EndFunction

// DisMountCritter - Vom Tier absteigen
Const DM_EXACT    := 0;
Const DM_TRY_NEAR := 1;
Function DisMountCritter(mount, critter, master, owner, fuzzy := DM_EXACT)
  var handled := 0;
  var oldcrit := Is_Critical();
  var override:=struct;
  override.+CProps:=dictionary;
  Set_Critical(1);
  override.+facing:=master.facing;
  override.+color:=mount.color;
  var statue := mount.getprop("spell_statue");  // Statuen Farbänderung
  If (statue)
    override.color:=statue[4];
  EndIf
  override.+name:=GetObjProperty(mount, "petname");
  EraseObjProperty(owner, "petname");
  If (owner)
    override.+Master := owner.serial;
    override.CProps.insert("master", owner.serial);
    override.+script:="enticedanimal";
  EndIf
  // ... Props ...
  var PetProps := GetObjProperty(mount, "petprops");
  If (PetProps)
    override.+Gender := PetProps.Gender;
  EndIf
  // ... und gemerkte CProps ...
  var PetCProps := GetObjProperty(mount, "petcprops");
  If (PetCProps.Generation)
    override.CProps.insert("Generation", PetCProps.Generation);
  EndIf
  If (PetCProps.hungrylevel)
    override.CProps.insert(PROP_HUNGER, PetCProps.hungrylevel);
  EndIf
  If (PetCProps.hungrycheck)
    override.CProps.insert(PROP_HUNGERCHK, PetCProps.hungrycheck);
  EndIf
  If (PetCProps.companion)
    override.CProps.insert(PROP_COMPANION, PetCProps.companion);
  EndIf
  If (PetCProps.colorchange)
    override.CProps.insert(PROP_CHANGE_COLOR, PetCProps.colorchange);
  EndIf
  // ... hier weitere CProps ergaenzen
  // .. poison_level (inkl. (Re-)Starten des Giftscripts?)

  // ... sowie Privilegien setzen
  var privs := mount.getprop("petprivs");
  override.+Privs:="";
  override.+Settings:="";
  ForEach priv in (privs.keys())
    override.Privs+=priv+" ";
    If (privs[priv] == 1)
      override.Settings+=priv+" ";
    EndIf
  EndForEach

  var pet := CreateNPCFromTemplate(
    critter, master.x, master.y, master.z, override, master.realm
  );
  If (!pet and fuzzy == DM_TRY_NEAR)
    // Erzeugung in der Umgebung versuchen
    var x, y;
    // von Suedosten her beginnen, da dort typischerweise bei Haeusern
    // die Aussentuer ist; d. h. wenn man in Haustuer stirbt/absteigt,
    // ist das Tier draussen
    For (x := master.x + 1; x >= master.x - 1; x -= 1)
      For (y := master.y + 1; y >= master.y - 1; y -= 1)
        // etwas unterhalb anfangen, da dort eine Treppe sein koennte
        // und das Tier nicht im naechsten Geschoss landen soll
        var z := GetStandingHeight(x, y, master.z - 15, master.realm).z;
        pet := CreateNPCFromTemplate(
          critter, x, y, z, override, master.realm
        );
        If (pet)
          break;
        EndIf
      EndFor
      If (pet)
        break;
      EndIf
    EndFor
  EndIf

  If (pet)
    // Vitals ... (bei Neuerstellung wird es auf Max gesetzt)
    var PetVitals := GetObjProperty(mount, "petvitals");
    If (PetVitals)
      SetHPPergon(pet,      PetVitals.Hp);
      SetManaPergon(pet,    PetVitals.Mana);
      SetStaminaPergon(pet, PetVitals.Stam);
    EndIf

    EraseObjProperty(master, "petvitals");
    EraseObjProperty(master, "petcprops");

    DestroyItem(mount);

    handled:=1;
  Else
    SysLog(
      "FEHLER: Erstellen des NPC-Templates '"+critter+
      "' fuer "+CharInfoStr(master, COORDS_REALM)+" gescheitert: "+
      pet.errortext
    );
  EndIf

  Set_Critical(oldcrit);

  return handled;
EndFunction

// GetMountType - Liefert das Mount-Item
Function GetMountType(pet)
  Var mounttype:=0;

  Case (pet.objtype)
    ot_ar_swamp_dragon:  mounttype:=mt_ar_swamp_dragon;
    ot_chimera:          mounttype:=mt_chimera;
    ot_cu_sidhe:         mounttype:=mt_cu_sidhe;
    ot_desertostard:     mounttype:=mt_desertostard;
    ot_drake1:           mounttype:=mt_dragon_avatar;
    ot_etheral_llama:    mounttype:=mt_etheral_llama;
    ot_etheral_ostard:   mounttype:=mt_etheral_ostard;
    ot_forestostard:     mounttype:=mt_forestostard;
    ot_frenziedostard:   mounttype:=mt_frenziedostard;
    ot_giant_beatle:     mounttype:=mt_giant_beatle;
    ot_hai_riyo:         mounttype:=mt_hai_riyo;
    ot_horse2nightmare:  mounttype:=mt_horse2nightmare;
    ot_horse3:           mounttype:=mt_horse3;
    ot_horse4:           mounttype:=mt_horse4;
    ot_horse:            mounttype:=mt_horse;
    ot_kirin:            mounttype:=mt_kirin;
    ot_llama:            mounttype:=mt_llama;
    ot_mondain_steed:    mounttype:=mt_mondain_steed;
    ot_ridgeback:        mounttype:=mt_ridgeback;
    ot_savage_ridgeback: mounttype:=mt_savage_ridgeback;
    ot_skeletal_mount:   mounttype:=mt_skeletal_mount;
    ot_swamp_dragon:     mounttype:=mt_swamp_dragon;
    ot_unicorn:          mounttype:=mt_unicorn;

    // nur spezielle Varianten
    ot_polarbear:        If (pet.npctemplate=="PolarRideable")
                           mounttype:=mt_polarbear;
                         EndIf
  EndCase

  Return (mounttype);
EndFunction

// GetPetTemplate - Liefert das Pet-Template
Function GetPetTemplate(item)
  Var critter:="";

  If (item.objtype==0xf021)
    Var template:=Lower(GetObjProperty(item, "pettemplate"));
    If (template)
      critter:=template;
    Else
      Case (item.graphic)
        mt_ar_swamp_dragon:  critter:="arswampdragon";
        mt_chimera:          critter:="Chimera";
        mt_cu_sidhe:         critter:="CuSidhe";
        mt_desertostard:     critter:="desertostard";
        mt_dragon_avatar:    critter:="drake1";
        mt_etheral_llama:    critter:="horse";           // Check
        mt_etheral_ostard:   critter:="horse";           // Check
        mt_forestostard:     critter:="forestostard";
        mt_frenziedostard:   critter:="frenziedostard";
        mt_giant_beatle:     critter:="giantbeatle";
        mt_hai_riyo:         critter:="HaiRiyo";
        mt_horse:            critter:="horse";
        mt_horse2nightmare:  If (item.color==COLOR_NIGHTMARE)
                               critter:="nightmare";
                             Else
                               critter:="horse2";
                             EndIf
        mt_horse3:           critter:="horse3";
        mt_horse4:           critter:="horse4";
        mt_kirin:            critter:="kirin";
        mt_llama:            critter:="llama";
        mt_mondain_steed:    critter:="MondainSteed";
        mt_polarbear:        critter:="PolarRideable";
        mt_ridgeback:        critter:="ridgeback";
        mt_savage_ridgeback: critter:="savageridgeback";
        mt_skeletal_mount:   critter:="skeletalmount";
        mt_swamp_dragon:     critter:="swampdragon";
        mt_unicorn:          critter:="unicorn";
      EndCase
    EndIf
  EndIf

  Return (critter);
EndFunction // }}}

///////////////////////////
//                       //
//   Stat-Manipulation   //
//                       //
///////////////////////////

// Stats_korrigieren - gemaess NPCDesc die Stats veraendern
Function Stats_korrigieren(me)
  var int;
  var str;
  var dex;
  var mana;
  var hp;
  var stam;

  If (me.getprop("custom_template"))
    Var datafile:=OpenDataFile("::customnpcs");
    Var custom_template:=GetObjProperty(me, "custom_template");
    If (datafile && custom_template)
      var elem:=datafile.FindElement(custom_template);
      If (elem)
        var data:=elem.getprop("data");
        int:= data.int;
        str:= data.str;
        dex:= data.dex;
        mana:=data.maxmana;
        hp:=  data.maxhp;
        stam:=data.maxstam;
      EndIf
    EndIf
    custom_template:=0;
  Else
    Var template:=GetNPCConfig(me.npctemplate);
    int:=CInt(template.int);
    str:=CInt(template.str);
    dex:=CInt(template.dex);
    mana:=CInt(template.mana);
    hp:=CInt(template.hp);
    stam:=CInt(template.stam);
  EndIf

  If (str)
    If (str<=210)
      SetStrPergon(me, str);
      SetStrModPergon(me, 0);
    Else
      SetStrPergon(me, 210);
      SetStrModPergon(me, str-210);
    EndIf
  EndIf

  If (dex)
    If (dex<=210)
      SetDexPergon(me, dex);
      SetDexModPergon(me, 0);
    Else
      SetDexPergon(me, 210);
      SetDexModPergon(me, dex-210);
    EndIf
  EndIf

  If (int)
    If (int<=210)
      SetIntPergon(me, int);
      SetIntModPergon(me, 0);
    Else
      SetIntPergon(me, 210);
      SetIntModPergon(me, int-210);
    EndIf
  EndIf

  If (hp)
    SetHPPergon(me, hp);
  EndIf

  If (mana)
    SetManaPergon(me, mana);
  EndIf

  If (stam)
    SetStaminaPergon(me, stam);
  EndIf
EndFunction

////////////////
//            //
//   Heaven   //
//            //
////////////////

// PetGetHeaven - Tier aus dem Himmel holen
Function PetGetHeaven(master, petserial, originallocation:=1)
  Var pet:=SystemFindObjectBySerial(petserial);
  If (pet)
    Var verschoben;
    If (originallocation)
      verschoben:=MoveObjectToLocation(pet, pet.x, pet.y, GetObjProperty(pet, "Heaven_Z"), pet.realm, MOVEOBJECT_FORCELOCATION);
    Else
      verschoben:=MoveObjectToLocation(pet, master.x, master.y, master.z, master.realm, MOVEOBJECT_FORCELOCATION);
    EndIf

    If (verschoben)
      EraseObjProperty(pet, "Heaven_Z");
      pet.script:=GetObjProperty(pet, "Heaven_OldScript");
      EraseObjProperty(pet, "Heaven_OldScript");

      SetObjProperty(pet, "master", master.serial);
      // im Himmel werden sie quasi nicht hungrig
      SetObjProperty(pet, PROP_HUNGERCHK, ReadGameClock());

      If (pet.hidden) // Um unnoetige Events zu sparen...
        pet.hidden:=0;
      EndIf
      RestartScript(pet);
    Else
      SysLog(
        "Heaven: "+petserial+" von '"+master.name+
        "' liess sich nicht verschieben!"
      );
    EndIf
  Else
    SysLog("Heaven: "+petserial+" von '"+master.name+"' ist verschwunden!");
  EndIf
EndFunction

// PetSetHeaven - Tier in den Himmel verschieben
Function PetSetHeaven(master, pet)
  Var old_z:=pet.z;

  If (MoveObjectToLocation(
      pet, pet.x, pet.y, 127, pet.realm, MOVEOBJECT_FORCELOCATION
  ))
    SetObjProperty(pet, "Heaven_Z", old_z);
    SetObjProperty(pet, "Heaven_OldScript", pet.script);
    pet.script := "heaven";
    pet.hidden := 1;
    RestartScript(pet);

    Return (1);
  Else
    SysLog(
      "Heaven: "+pet.serial+" von '"+master.name+
      "' liess sich nicht verschieben!"
    );
  EndIf
EndFunction // }}}

// PetMasterLogOn - Spieler loggt sich ein
Function PetMasterLogOn(master)
  Var heaven:=GetObjProperty(master, "heaven");
  If (heaven)
    ForEach petserial in heaven
      PetGetHeaven(master, petserial, 1);
    EndForEach
  EndIf

  EraseObjProperty(master, "heaven"); // Sicher ist sicher...
EndFunction

// PetMasterLogOff - Spieler loggt sich aus
Function PetMasterLogOff(master)
  PetMasterLogOn(master);

  var pets   := ListMobilesNearLocationEx(
    master.x, master.y, LIST_IGNORE_Z, HEAVENRANGE,
    LISTEX_FLAG_NORMAL, master.realm
  );

  var heaven := array{};
  ForEach pet in (pets)
    If (pet.isa(POLCLASS_NPC))
      If (
        (pet.script in {
            "calmanimal",
            "enticedanimal",
            "tamed",
            "tamedanimal"
        }) And (!GetObjProperty(pet, PROP_SUMMONED)) And (pet.master==master)
      )
        If (PetSetHeaven(master, pet))
          heaven.append(pet.serial);
        EndIf
      EndIf
    EndIf
  EndForEach

  SetObjProperty(master, "heaven", heaven);
EndFunction

//    npc.color               rw
//    npc.graphic             rw
//    npc.name                rw
//
//    npc._dexterity          ro
//    npc._dexterity_mod      rw
//    npc._intelligence       ro
//    npc._intelligence_mod   rw
//    npc._strength           ro
//    npc._strength_mod       rw
//
//    npc._hp                 rw
//    npc._hp_regen_rate      rw
//    npc._mana               rw
//    npc._mana_regen_rate    rw
//    npc._stamina            rw
//    npc._stamina_regen_rate rw
//
//    npc.ar_mod              rw
//
//    npc.frozen              rw
//    npc.paralyzed           rw
//    npc.poisoned            rw
//
// 2 routinen schreiben: savenpc & restore npc (fuer riding & stamaster)

//////////////////
//              //
//   Movement   //
//              //
//////////////////

// WanderPergon - Laesst einen NPC wandern, aber nicht durch Tueren
Function WanderPergon()
  var mob := Self();

  var door := ListItemsNearLocationWithFlag(
    mob.x, mob.y, mob.z, 2, TILEDATA_FLAG_DOOR, mob.realm
  )[1];
  If (door)
    WalkAwayFrom(door);
    // im Wasser gibt es keine Tueren, daher ist return ok
    return;
  EndIf

  // Flugtiere sollten Wasser vermeiden
  If (mob.movemode["A"])
    var dx := ConvertDirectionToLocX(mob.facing);
    var dy := ConvertDirectionToLocY(mob.facing);
    If (
      // direkt vor mir ist Wasser ...
      IsWater(mob.x + dx, mob.y + dy, mob.realm) and
      // ... und weiter weg ist auch Wasser
      IsWater(mob.x + 10*dx, mob.y + 10*dy, mob.realm)
    )
      // wahrscheinlich recht breiter Fluss oder Meer, Suche nach Land
      mob.facing := RandomInt(8);
      dx := ConvertDirectionToLocX(mob.facing);
      dy := ConvertDirectionToLocY(mob.facing);
      If (!IsWater(mob.x + 10*dx, mob.y + 10*dy, mob.realm))
        // da ist Festland, gezielt hinlaufen
        WalkTowardLocation(mob.x+dx, mob.y+dy);
        // damit waeren wir gewandert
        return;
      EndIf
    EndIf
  EndIf

  Wander();

  If (!(mob.movemode["A"]))
    // kein Flugtier -> ertrinkt nicht
    return;
  EndIf

  // Ertrinken, wenn im Wasser
  If (IsWater(mob.x, mob.y, mob.realm, mob.z, 1))
    // ist im Wasser
    var bath_time := GetObjProperty(mob, "water_stay_time");
    If (!bath_time)
      // erst ins Wasser geraten, Zeit merken
      SetObjProperty(mob, "water_stay_time", ReadGameClock());
    ElseIf (ReadGameClock() > bath_time+900)
      // zu lange im Wasser (15 Minuten), Ertrinken
      PlaySoundEffect(mob, SFX_DROWN);
      PlayStationaryEffect(mob.x, mob.y, mob.z, 0x34d1, 5, 0, 0, mob.realm);

      // falls es eine Rune gibt, dorthin zurueckkehren
      If (ReturnToSpawnRune(mob))
        EraseObjProperty(mob, "water_stay_time");
        return;
      EndIf
      // sonst und bei Fehlern zum Vernichter
      KillMobileSilent(mob, "drowned");
    EndIf
  Else
    // nicht (mehr) im Wasser
    EraseObjProperty(mob, "water_stay_time");
  EndIf
EndFunction

Function IsWater(x, y, realm, z:=0, checkz:=0) // {{{
  var onwater := GetStandingLayers(x, y, MAPDATA_FLAG_MOVESEA, realm);
  If (onwater and onwater.size() >= 1)
    If (checkz)
      Foreach water in onwater
        If (water.z==z)
          return 1;
        EndIf
      EndForEach
    Else
      return 1;
    EndIf
  EndIf

  var wateritems := ListItemsNearLocationWithFlag(
    x, y, LIST_IGNORE_Z, 1, TILEDATA_FLAG_WET, realm
  );
  If (wateritems and wateritems.size() >= 1)
    If (checkz)
      Foreach water in wateritems
        If (water.z==z)
          return 1;
        EndIf
      EndForEach
    Else
      return 1;
    EndIf
  EndIf

  var cfgfile := ReadConfigFile("::tiles");
  If (cfgfile == error)
    syslog("FEHLER: Laden von Tiledata gescheitert: "+cfgfile.errortext);
    return cfgfile;
  EndIf

  ForEach static in (ListStaticsAtLocation(x, y, LIST_IGNORE_Z, 0, realm))
    SleepMs(2);
    var elem := FindConfigElem(cfgfile, static.objtype);
    If (elem == error)
      /*syslog(
        "FEHLER: ["+x+", "+y+"; "+realm+"] hat Typ "+
        static.objtype+" nicht gefunden: "+elem.errortext
      );*/
      continue;
    EndIf

    var layer := GetConfigInt(elem, "MoveSea");
    If (layer == 1)
      If (checkz)
        If (static.z==z)
          return 1;
        EndIf
      Else
        return 1;
      EndIf
    EndIf
  EndForEach

  return 0;
EndFunction
// }}}

// WanderDoorsPergon - Laesst einen NPC wandern
Function WanderDoorsPergon() // {{{
  // Var me:=Self();

  Wander();

  // Var walking:=1;
  // Var anchor:=GetObjProperty(me, PROP_ANCHOR);
  // If (anchor.size()==4)
  //   If ((me.x-anchor[1]>anchor[3]) Or (me.y-anchor[2]>anchor[3]))
  //     If (RandomInt(100)<anchor[4])
  //       WalkTowardLocation(anchor[1], anchor[2]);
  //       walking:=0;
  //     EndIf
  //   EndIf
  // EndIf
  //
  // If (walking)
  //   Case (RandomInt(8))
  //     0: 1: 2: 3: 4: 5: Move(me.facing);
  //     6:                Move(me.facing-1);
  //     7:                Move(me.facing+1);
  //   EndCase
  // EndIf
EndFunction
// }}}

// DropAnchor - Setzt den Anker
// Benutzt:
//   NPCDesc Template.dstart - AnkerBereich
//     (0   - deaktiviert Anker) - rechteckiger Bereich und kein Pygaros!
//   NPCDesc Template.psub   - GrenzenChance
//     (100 - akzeptiert Grenze)
//     (0   - ignoriert  Grenze)
//   NPC Object.anchor - Array(Anchor.x, Anchor.y, dstart, psub)
//
// Parameter:
//   Integer(overwrite) = 0 -> Benutzt den alten Anker, auch nach Server-Restart
//                        1 -> Setzt einen neuen Anker
//   Array(overwrite)       -> Setzt die Werte direkt
Function DropAnchor(overwrite := 0)
  If (TypeOfInt(overwrite) == OT_ARRAY) // Ankerwerte direkt setzen
    SetObjProperty(overwrite[1], PROP_ANCHOR, overwrite[2]);
    SetAnchor(
      overwrite[2][1], overwrite[2][2], overwrite[2][3], overwrite[2][4]
    );
  Else

    Var anchor := GetObjProperty(me, PROP_ANCHOR);
    If ((!anchor) or overwrite)
      // Wir haben noch keinen Anker definiert, oder aber ueberschreiben
      Var dstart := GetNPCConfig(me.npctemplate).dstart;
      If (dstart)
        Var psub := GetNPCConfig(me.npctemplate).psub;
        If (!psub)
          psub := 100; // Grenze akzeptieren
        EndIf

        SetObjProperty(me, PROP_ANCHOR, {me.x, me.y, dstart, psub});
        SetAnchor(me.x, me.y, dstart, psub);
      Else
        ReleaseAnchor(me);
      EndIf
    Else // Wir haben bereits einen Anker definiert
      // Anker reaktivieren
      SetAnchor(anchor[1], anchor[2], anchor[3], anchor[4]);
    EndIf
  EndIf
EndFunction

//////////////////////////////////////
// ReleaseAnchor - Loescht den Anker
//////////////////////////////////////
Function ReleaseAnchor(npc)
  EraseObjProperty(npc, PROP_ANCHOR);
  SetAnchor(npc.x, npc.y, 0, 0);
EndFunction

////////////////////////////
//                        //
//   HateLoveFrightHunt   //
//                        //
////////////////////////////

// CheckHateLove - Gegenueber analysieren
// 0 - Ignorieren = Laesst sich nirgends einordnen
// 1 - Hate       = Hasse ich [und sollte versuchen, es zu toeten]
// 2 - Fright     = Habe ich Angst vor und versuche wegzurennen
// 3 - Love       = Mag ich (z.B. eigene Spezies bei Herdentieren)
// 4 - Hunt       = Wenn ich Hunger hab fress ich das
// 5 - Player     = Spieler
// 6 - Human NPC  = Menschlicher NPC
Function CheckHateLove(me, opponent, flags := "")
  // Ist der Parameter gueltig?
  If (!opponent or !opponent.isa(POLCLASS_MOBILE))
    return LOVEHATE_IGNORE;
  EndIf

  // Handelt es sich nicht um einen GM?
  If (opponent.cmdlevel > CMDLEVEL_SEER or opponent.hidden)
    return LOVEHATE_IGNORE;
  EndIf

  // Ist es ein NPC?
  If (!opponent.isa(POLCLASS_NPC))
    // Spieler
    return LOVEHATE_PLAYER;
  EndIf

  Var hatelovefile:=ReadConfigFile("::hatelove");
  If (!hatelovefile)
    syslog("FEHLER: Laden von hatelove gescheitert: "+hatelovefile.errortext);
    return LOVEHATE_IGNORE;
  EndIf

  // Flags umwandeln in Array
  If (TypeOfInt(flags) == OT_STRING)
    flags := array{};
  EndIf

  var element := hatelovefile[Lower(me.npctemplate)];
  If (element)
    var def := GetConfigString(element,Lower(opponent.npctemplate));
    If (def)
      Case (Lower(def))
      "hate": // Hasse ich [und sollte versuchen, es zu toeten]
        If ("nohate" in (flags))
          // darf nicht hassen
          break;
        EndIf
        return LOVEHATE_HATE;

      "fright": // Habe ich Angst vor und versuche wegzurennen
        return LOVEHATE_FRIGHT;

      "love": // Mag ich (z.B. eigene Spezies bei Herdentieren)
        return LOVEHATE_LOVE;

      "hunt": // Wenn ich Hunger hab fress ich das
        If ("nohunt" in (flags))
          // darf nicht jagen hassen
          break;
        EndIf
        return LOVEHATE_HUNT;

      default:
        break;
      EndCase
    EndIf
  EndIf

  If (IsElfOrHuman(opponent))
    // Menschlicher NPC
    return LOVEHATE_HUMAN;
  EndIf

  return 0;
EndFunction

/////////////////
//             //
//   Corpses   //
//             //
/////////////////

// GetCorpseConfig - Liefert die Infos zum Corpse
Function GetCorpseConfig(corpse)
  Var corpsesfile:=ReadConfigFile("::corpses");
  If (corpsesfile)
    Var npctemplate:=Lower(GetObjProperty(corpse, "npctemplate"));

    Var elem:=FindConfigElem(corpsesfile, npctemplate);
    If (!elem)
      elem:=FindConfigElem(corpsesfile, corpse.corpsetype);
    EndIf

    If (elem)
      Var corpseinfo:=struct;
      corpseinfo.+template:=npctemplate;

      corpseinfo.+item:={};
      ForEach item in GetConfigStringArray(elem, "Item")
        item:=SplitWords(item);
        (corpseinfo.item).append({item[1], CInt(item[2])});
      EndForEach
      ForEach item in GetConfigStringArray(elem, "Item_"+corpseinfo.template)
        item:=SplitWords(item);
        (corpseinfo.item).append({item[1], CInt(item[2])});
      EndForEach

      corpseinfo.+lootgroup:=CInt(elem.lootgroup);
      corpseinfo.+destroy:=CInt(elem.destroy);
      corpseinfo.+cut:=CInt(elem.cut);

      Return (corpseinfo);
    EndIf
  Else
    SysLog("Animal: Kann die Datei 'corpses.cfg' nicht oeffnen!");
  EndIf
EndFunction

/////////////////
//             //
//   Parking   //
//             //
/////////////////

// ParkAnimal - Tier anbinden und ruhig stellen
Function ParkAnimal(tier)
  If (GetObjProperty(tier, "RoamsFreeAt"))
    // Akt. RoamFree-Zeit merken
    SetObjProperty(tier, "parking", GetObjProperty(tier, "RoamsFreeAt"));
  Else
    SetObjProperty(tier, "parking", ReadGameClock()+60);
  EndIf

  // Auf eine hohe Zahl setzen, damit Vieh waehrend der Park-Phase nicht
  // abhauen kann. Allerdings nicht ganz Max, da sonst beim fuettern waehrend
  // des Parkens MaxInt ueberlaeuft und das Vieh abhaut.
  SetObjProperty(tier, "RoamsFreeAt", 0x6fffffff);
  // Akt. Zeit merken, um spaeter Standzeit bestimmen zu koennen
  SetObjProperty(tier, "parktime", ReadGameClock());

  tier.warmode := 0;

  tier.frozen := 1;
  IncRevision(tier);
  SendEvent(tier,Struct{type := EVID_PARKING});
EndFunction

// UnparkAnimal - Tier wird abbinden
Function UnparkAnimal(tier)
  If ((GetObjProperty(tier, "RoamsFreeAt") < ReadGameClock()) or (!GetObjProperty(tier, "RoamsFreeAt")))
    // Aus irgendeinem Grund ist die Prop < der aktuellen Zeit,
    // daher neue Zeit + 12 Stunden setzen
    SetObjProperty(tier, "RoamsFreeAt", ReadGameClock()+43200);
  Else
    var parkzeit := ReadGameClock() - CInt(GetObjProperty(tier, "parktime"));
    SetObjProperty(tier, "RoamsFreeAt", CInt(GetObjProperty(tier, "parking")) + parkzeit);
  EndIf

  EraseObjProperty(tier, "parking");
  EraseObjProperty(tier, "parktime");

  tier.frozen := 0;
  IncRevision(tier);
  SendEvent(tier,Struct{type:=EVID_PARKING});
EndFunction

///////////////////
//               //
//   Sonstiges   //
//               //
///////////////////

// Wait_for_EventLogging - Protokolliert nuetzliche Infos
Function Wait_for_EventLogging(num_seconds_timeout)
  Var event := Wait_for_Event(num_seconds_timeout);
  If ((event) && (!(event.type in {EVID_DOMEST_EAT,EVID_DOMEST_DRINK,EVID_DOMEST_TIME,EVID_DOMEST_SOUND,EVID_DOMEST_BABY,EVID_LOOKAROUND,EVID_WALK}))) //eigene Events Interessieren nich
    Var old_critical := Is_Critical();
    Set_Critical(1);

    Var eventspropname := "events_"+GetProcess(GetPID()).name;

    Var events := GetGlobalProperty(eventspropname);
    If (!events)
      // Name der Prop in einer globalen Liste verzeichnen
      Var eventspropnames := GetGlobalProperty("eventpropnames");
      If (eventspropnames)
        eventspropnames.append(eventspropname);
      Else
        eventspropnames := {eventspropname};
      EndIf
      SetGlobalProperty("eventpropnames", eventspropnames);

      events := Dictionary;
    EndIf

    // Events des Scriptes mitzaehlen
    If (events[event.type])
      events[event.type] := events[event.type] + 1;
    Else
      events[event.type] := 1;
    EndIf
    SetGlobalProperty(eventspropname, events);

    Set_Critical(old_critical);
  EndIf

  Return (event);
EndFunction

// GetWhatIWouldEat - Findet heraus, was das Tier so frisst
Function GetWhatIWouldEat(tier)
  // Fallback fuer Tiere ohne Eintrag auf Aas (Leichenteile), rohes und
  // gegartes Fleisch (Rippchen, Huhn, Lamm, Schinken)
  // Rest braucht Eintraege in animals-eat.cfg
  var i_eat := array{
    0x976, 0x977, 0x9b8, 0x9b9, 0x9c0, 0x9c9, 0x9f1, 0x9f2, 0x1044,
    0x1607, 0x1608, 0x1609, 0x160a, 0x1ce2, 0x1ce5, 0x1cec, 0x1ced,
    0x1cee, 0x1cef, 0x1cf0, 0x1d9f, 0x1da0, 0x1da1, 0x1da2, 0x1da3,
    0x1da4
// alter Eintrag:
//    0x09d0, 0x09d1, 0x09d2, 0x0c5c, 0x0c5d, 0x0c64, 0x0c65, 0x0c66,
//    0x0c67, 0x0c6a, 0x0c6b, 0x0c6c, 0x0c6d, 0x0c6e, 0x0c70, 0x0c71,
//    0x0c72, 0x0c73, 0x0c74, 0x0c75, 0x0c77, 0x0c78, 0x0c79, 0x0c7a,
//    0x0c7b, 0x0c7c, 0x0c7f, 0x0c80, 0x0c81, 0x0c82, 0x0d39, 0x0d3a,
//    0x0f36, 0x100c, 0x100d, 0x171d, 0x171e, 0x171f, 0x1720, 0x1721,
//    0x1722, 0x1723, 0x1724, 0x1725, 0x1726, 0x1727, 0x1728, 0x1729,
//    0x172a, 0x172b, 0x172c, 0x172d, 0x1a92, 0x1a93, 0x1a94, 0x1a95,
//    0x1a96, 0x1ebd, 0x1ebe, 0x1ebf
    };
  // pruefen, ob fuer das Tier Nahrung konfiguriert ist
  var what_eats_what := ReadConfigFile("::animals-eat");
  If (what_eats_what)
    var entry := FindConfigElem(what_eats_what, CInt(tier.graphic));
    If (entry)
      i_eat := ResolveRange(GetConfigString(entry, "Food"));
    Else
      syslog(
        "WARNUNG: Kein Eintrag fuer Tier "+tier.graphic+": "+
        entry.errortext
      );
    EndIf
  Else
    syslog("WARNUNG: Fehler bei Futterdatei: "+what_eats_what.errortext);
  EndIf

  return i_eat;
EndFunction

// DraggedItemToAnimal - "Verwertet" den uebergebenen Gegenstand
Function DraggedItemToAnimal(tier, ev)
  // mit Nahrung gefuettert?
  var i_eat := GetWhatIWouldEat(tier);

  // falls das passende dabei war, fuettern
  If (ev.item.graphic in (i_eat))
    var gift := GetPoisonLevel(ev.item);
    If (gift)
      var poisoner := ev.item.getprop(EV_POISONER);
      If (poisoner)
        tier.setprop(EV_POISONER, poisoner);
      EndIf

      SubtractAmount(ev.item, 1);
      SetPoisonLevel(tier, gift);
      If (ev.item)
        MoveObjectToLocation(
          ev.item, tier.x, tier.y, tier.z, tier.realm
        );
        PrintTextAbovePrivatePergon(
          ev.item, "*plumps*", "*thud*", ev.source
        );
        return;
      EndIf
    EndIf

    // Naehrwert auf Hungerlevel addieren
    var saturation := GetFoodSaturation(ev.item);
    var hunger     := tier.getprop(PROP_HUNGER);
    If (hunger == error)
      // frisch erzeugt?
      hunger := 9.0;
    EndIf
    var subtract  := 1;
    var hungerold := hunger;
    hunger := Min(15.0, hunger + saturation * ev.item.amount);
    tier.setprop(PROP_HUNGER, CDBl(hunger));

    If (hunger >= 14.0)
      PrintTextAbovePrivatePergon(tier, "*voll*", "*full*", ev.source);
    Else
      PrintTextAbovePrivatePergon(tier, "*mampf*", "*yum*", ev.source);
    EndIf
    PlaySoundEffect(tier, SFX_3B+RandomInt(3));
    If (hunger - hungerold)
      subtract := CInt((hunger - hungerold) / saturation);
      If (!subtract)
        subtract := 1;
      EndIf
    EndIf
    SubtractAmount(ev.item, subtract);
    If (ev.item)
      If (!MoveItemToContainer(ev.item, ev.source.backpack))
        MoveObjectToLocation(
          ev.item, tier.x, tier.y, tier.z, tier.realm
        );
        PrintTextAbovePrivatePergon(
          ev.item, "*plumps*", "*thud*", ev.source
        );
      EndIf
    EndIf

    var losreissen := GetObjProperty(tier, "RoamsFreeAt");
    If (losreissen)
      SetObjProperty(
        tier, "RoamsFreeAt", losreissen + CInt(TREUEZEIT/36.0)
      );
    Else
      SetObjProperty(tier, "RoamsFreeAt", ReadGameClock() + TREUEZEIT);
    EndIf
    return;
  EndIf

  // Packtier? (0x123 - Packpferd, 0x124 - Packlama)
  If (tier.graphic in array{0x123, 0x124})
    // wenn es noch nie beladen wurde, hat es keinen Rucksack
    If (!tier.backpack)
      // Rucksack erzeugen und dranpappen
      var newbackpack := CreateItemAtLocationPergon(
        tier.x, tier.y, tier.z, 0xe75, 1, tier.realm
      );
      If (!EquipItem(tier, newbackpack))
        SysLog("Animal: "+Lower(Hex(tier.serial))+" mag kein Backpack");
      EndIf
    EndIf

    // hat es jetzt einen Rucksack?
    If (tier.backpack)
      // versuchen zu beladen
      MoveItemToContainer(ev.item, tier.backpack);
      PlaySoundEffect(tier, SFX_49);
      return;
    EndIf
  EndIf

  SendSysMessagePergon(ev.source, "Euch faellt etwas zu Boden ...");
  MoveObjectToLocation(ev.item, tier.x, tier.y, tier.z, tier.realm);
EndFunction

// Naehrwert eines einzelnen Nahrungsitems ermitteln
// (bei Stacks Naehrwert aus Funktion * food.amount manuell berechnen!)
Function GetFoodSaturation(food)
  var cfg      := ReadConfigFile("::itemdesc");
  var fooddata := cfg[food.objtype];
  var eat      := fooddata.eat;
  If (!eat)
    eat := 1.0;
    syslog("HINWEIS: "+ItemInfoStr(food)+" hat keinen Naehrwert");
  EndIf

  return eat;
EndFunction

// Hungerwert aktualisieren, ggf. verhungern
Const HUNGRY_INTERVAL_ANIMAL := 24*3600;
Function UpdateHunger(animal)
  var hungry    := animal.getprop(PROP_HUNGER);
  var lastcheck := animal.getprop(PROP_HUNGERCHK);
  If (hungry == error or lastcheck == error)
    // bisher nicht initialisiert, mittleren Wert setzen
    // (ca. zwei Monate weiterleben ohne Futter)
    animal.setprop(PROP_HUNGER, 9.0);
    animal.setprop(PROP_HUNGERCHK, ReadGameClock());
    return;
  EndIf

  var hasmaster := animal.getprop("master");
  If (hasmaster and hungry < 1.0)
    PrintTextAboveLocalizedPergon(animal, "*magenknurr*", "*collywobble*");
  EndIf

  // einmal alle 24 Stunden Hungerlevel aktualisieren
  If (lastcheck + HUNGRY_INTERVAL_ANIMAL >= ReadGameClock())
    // letzter Check noch nicht so lange her
    return;
  EndIf

  // haelt etwas laenger als drei Monate bei vollem Futter
  // (0.15 bei Aufruf alle 24h, bei seltenerem Aufruf angemessen erhoehen)
  hungry -= 0.15;
  // Multiplizieren mit Zeitraum unterlassen, killt sonst ewig nicht
  // gefuetterte Tiere beim Absteigen (zwar korrekt, aber unerwuenscht)
  // * ((ReadGameClock() - lastcheck)/CDbl(HUNGRY_INTERVAL_ANIMAL));
  animal.setprop(PROP_HUNGER, hungry);
  animal.setprop(PROP_HUNGERCHK, ReadGameClock());

  If (hungry > 0.0)
    // noch nicht verhungert
    return;
  EndIf

  var companion := CInt(animal.getprop(PROP_COMPANION));
  If (companion)
    animal.setprop(PROP_HUNGER, 9.0);
    If (companion >= 99)
      // Cmdlevel koennen so ihre Tiere hungerresistent machen
      return;
    EndIf
    // ein Zaehmlevel verloren
    animal.setprop(PROP_COMPANION, companion - 1);
  EndIf

  // verhungern
  PerformAction(animal, ANIM_DIE_BACK);
  If (ReturnToSpawnRune(animal))
    // Viech springt zurueck zu Rune, um Tod+Respawn zu sparen
    // (quasi wieder neu, daher Futter wieder auffuellen)
    animal.setprop(PROP_HUNGER, 9.0);
    return;
  Else
    // hat keine Rune mehr oder Stelle ist blockiert etc.
    // als Fallback normal sterben
    If (animal.script["grazer_new"] or animal.script["main_ai3:meek"])
      // erst einmal nur fuer ausgewaehlte, ungezaehmte Tierchen
      KillNPC(animal, "hungry", KILLNPC_CORPSE);
    EndIf
    syslog("HUNGER: "+CharInfoStr(animal)+" waere jetzt verhungert");
    PrintTextAboveLocalizedPergon(animal, "*verhunger*", "*famished*");
    animal.setprop(PROP_HUNGER, 1.0);
  EndIf
EndFunction

// Liste der Items, die gezaehmten Tieren entsprechen
Function PetTamedItem()
  return {
    0x211c, 0x8164, 0x2125, 0x8166, 0x211b, 0x8168, 0x20d1, 0x816a, 0x816c,
    0x816e, 0x2101, 0x8170, 0x20e6, 0x8172, 0x2108, 0x8174, 0x2103, 0x8138,
    0x20ef, 0x20f0, 0x20d4, 0x8176, 0x2121, 0x2124, 0x211f, 0x2120, 0x8178,
    0x817a, 0x8128, 0x8129, 0x817c, 0x817e, 0x8180, 0x8182, 0x8184, 0x20f6,
    0x812a, 0x2137, 0x8186, 0x2135, 0x8188, 0x2136, 0x818a, 0x818c, 0x818e,
    0x8190, 0x8192, 0x8194, 0x8196, 0x8198, 0x819a, 0x20cf, 0x819e, 0x2118,
    0x81a0, 0x81a2, 0x81a4, 0x2122, 0x81a6, 0x81a8, 0x81aa, 0x2126, 0x2127,
    0x211a, 0x81ac, 0x81ae, 0x81b0, 0x211d, 0x81b2, 0x81b4, 0x2123, 0x81b6,
    0x81b8, 0x81ba, 0x81bc, 0x81be, 0x81c0, 0x81c2, 0x81c4, 0x81c6, 0x81c8,
    0x81ca, 0x81cc, 0x81ce, 0x81d0, 0x81d2, 0x81d4, 0x81d6, 0x20da, 0x81d8,
    0x81da, 0x81dc, 0x81de, 0x81e0, 0x81e2, 0x81e4, 0x81e6, 0x81e8, 0x211e,
    0x81ea, 0x81ec, 0x2119, 0x81ee, 0x81f0, 0x81f2, 0x81f4, 0x81f6, 0x81f8,
    0x81fa, 0x81fc, 0x81fe, 0x8207, 0x8209, 0x820B, 0x820D, 0x8214
  };
EndFunction

// Liste der Items, die abgerichteten Tieren entsprechen
Function PetEnticedItem()
  return {
    0x811c, 0x8165, 0x8125, 0x8167, 0x811b, 0x8169, 0x80d1, 0x816b, 0x816d,
    0x816f, 0x8101, 0x8171, 0x80e6, 0x8173, 0x8108, 0x8175, 0x8103, 0x8139,
    0x80ef, 0x80f0, 0x80d4, 0x8177, 0x8121, 0x8124, 0x811f, 0x8120, 0x8179,
    0x817b, 0x812b, 0x812c, 0x817d, 0x817f, 0x8181, 0x8183, 0x8185, 0x80f6,
    0x812d, 0x8137, 0x8187, 0x8135, 0x8189, 0x8136, 0x818b, 0x818d, 0x818f,
    0x8191, 0x8193, 0x8195, 0x8197, 0x8199, 0x819b, 0x80cf, 0x819f, 0x8118,
    0x81a1, 0x81a3, 0x81a5, 0x8122, 0x81a7, 0x81a9, 0x81ab, 0x8126, 0x8127,
    0x811a, 0x81ad, 0x81af, 0x81b1, 0x811d, 0x81b3, 0x81b5, 0x8123, 0x81b7,
    0x81b9, 0x81bb, 0x81bd, 0x81bf, 0x81c1, 0x81c3, 0x81c5, 0x81c7, 0x81c9,
    0x81cb, 0x81cd, 0x81cf, 0x81d1, 0x81d3, 0x81d5, 0x81d7, 0x80da, 0x81d9,
    0x81db, 0x81dd, 0x81df, 0x81e1, 0x81e3, 0x81e5, 0x81e7, 0x81e9, 0x811e,
    0x81eb, 0x81ed, 0x8119, 0x81ef, 0x81f1, 0x81f3, 0x81f5, 0x81f7, 0x81f9,
    0x81fb, 0x81fd, 0x81ff, 0x8208, 0x820A, 0x820C, 0x820E, 0x8215
  };
EndFunction

// Liste der Tiertemplates, die ver-/gekauft werden
Function PetTemplates()
  return {
    // Hund Huendin Haesin Hase Katze Kater Huhn
    "Dog", "Dog_w", "Rabbit", "Rabbit_m", "Cat", "Cat_m", "Chicken",
    // Hahn Affe Äffin Eber Sau Schaf Schafbock Ziege
    "Cock", "Ape", "Ape_w", "Pig", "Pig_w", "Sheep", "Sheep_m", "Goat",
    // Ziegenbock Kuh Kuh Bulle Bulle Hirschkuh Hirsch Stute (braun)
    "Goat_m", "Cow", "Cow2", "Bull", "Bull2", "Deer", "Gheart", "Horse",
    // Pferd (hellbraun) Pferd (grau) Pferd (falb) Pferd (rotbraun)
    // Pferd (kaffeebraun) Hengst (braun)
    "Horse2", "Horse3", "Horse4", "Horse5", "Horse6", "Horse_m",
    // Hengst (dunkelbraun) Hengst (schwarz) Hengst (falb) Hengst (weiss)
    // Gorilla
    "Horse_m2", "Horse_m3", "Horse_m4", "Horse_m5", "Gorilla",
    // Gorillaweibchen Lama Lamahengst Waldostard Waldostardin
    "Gorilla_w", "Llama", "Llama_m", "ForestOstard", "ForestOstard_w",
    // Wuestenostardin Wuestenostard Raubostard
    "DesertOstard", "DesertOstard_m", "FrenziedOstard",
    // Raubostardin Hoehlenostardin Hoehlenostard Eisostard
    "FrenziedOstard_w", "CaveOstard", "CaveOstard_m", "IceOstard_m",
    // Eisostardin Lavaostard Lavaostardin Lavaridgeback
    "IceOstard_w", "LavaOstard_m", "LavaOstard_w", "LavaRidgeback_m",
    // Lavaridgeback Braunbaer Braunbaerin Schwarzbaer Schwarzbaerin
    "LavaRidgeback_w", "Bear", "Bear_w", "BlackBear", "BlackBear_w",
    // Eisbaer Eisbaerin Wolf Woelfin Hoehlenwolf Hoehlenwoelfin
    "Polar", "Polar_w", "Wolf", "Wolf_w", "DireWolf", "DireWolf_w",
    // Packpferd Packlama Vogel Papagei (gruen) Papagei (rot)
    "PackHorse", "PackLlama", "Bird", "Parrot_Green", "Parrot_Red",
    // Papagei (gelb) Adler Rabe Waldhuehnchen Ratte Riesenkroete
    "Parrot_Yellow", "Eagle", "Raven", "Vip", "Rat", "Giant_Toad",
    // Ochsenfrosch Wildschwein Wildhund Wildkatze Riesige Ratte
    // Riesenratte
    "BullFrog", "Hildisvini", "RabidDog", "RabidCat", "GiantRat", "GRat",
    // Schlange Riesenschlange Riesenskorpion Felsenscorpion Riesenspinne
    "Snake", "Giant_Serpent", "Scorp", "RockScorpion", "Spider",
    // Felsenspinne Flammenspinne Phasenspinne Eistarantel
    "RockSpider", "FlamingSpider", "PhaseSpider", "FrostSpider",
    // Frostphasenspinne Alligator Felsenechse Feuerechse
    "IcePhaseSpider", "Alligator", "RockLizard", "FireLizard",
    // Lavaechse Wuestenechse Surat Gremlin ein wirklich grosser Kaefer
    "LavaLizard", "SandLizard", "Mongbat", "Gremlin", "GiantBeatle",
    // Ridgeback Wilder Ridgeback Grizzly Frostwolf Dunkelwolf
    "Ridgeback", "SavageRidgeback", "Grizzly", "FrostWolf", "DarkWolf",
    // Panther Skelettstute Imp Hoellenkatze Hoellenhund
    "Panther", "SkeletalMount", "Imp", "HellCat", "HellHound",
    // Nachtmahr Succubus Grisu, der Sumpfdrache Smaug, der Sumpfdrache
    // Kirin
    "NightMare", "Succubus", "ARSwampDragon", "SwampDragon", "Kirin",
    // Alter Eisbaer Schlachtross Greif Lupus Malus Chimera
    "PolarRideable", "MondainSteed", "HaiRiyo", "CuSidhe", "Chimera"
  };
EndFunction

// Tier wird abgerichtet und ist dem Abrichter treu
Function Tame(who, animal)
  animal.eraseprop("RoamsFreeAt");
  animal.eraseprop("TameTimer");

  // Restriktionen der Questzone aufheben
  QuestZoneRevokeRestrictions(animal);

  // Kommentar aus enticement.src uebernommen:
  // Folgende Zeile faellt zukuenftig raus!!!
  animal.setprop("master", who.serial);
  // und das stammt aus dem Textcmd .tame
  animal.setmaster(who);

  // Script fuer "abgerichtetes" Tier aktivieren
  animal.script := "enticedanimal";

  // und (neu-)starten
  RestartScript(animal);
EndFunction

// vim: sts=2 sw=2
