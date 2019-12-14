///////////////////////////////////////////////////////////////////////////
// TreasureChest include --
// unterschiedliche Funktionen zum Thema Schatzkarten/-truhen

Const UOBJECT_TREASURE_CHEST := 0x0e40;
Const CHEST_CUSTOM_CPROP     := "CustomFilling";

use storage;
include "include/storage";

// CreateTreasureChest - Befuellt die Kiste
Function CreateTreasureChest(
  level, x, y, z, realm := _DEFAULT_REALM, custom := 0
)
  // Kiste vorbereiten
  var chest := CreateItemAtLocationPergon(
    x, y, z, UOBJECT_TREASURE_CHEST, 1, realm
  );
  If (!chest)
    syslog(
      "FEHLER: Schatzkiste konnte nicht bei ["+x+", "+y+", "+z+"; "+
      realm+"] erzeugt werden"
    );
    return;
  EndIf

  SetObjProperty(chest, "level", level);
  chest.movable   := 0;
  chest.usescript := ":treasuremap:treasurechest";
  SetName(chest, "Eine Schatzkiste");

  // Kiste befuellen

  // Sonderfuellung
  If (custom)
    // Kiste mit Fuellung in Storage Area suchen
    var storage   := FindOrCreateStorage(STORAGE_CHEST);
    var container := FindRootItemInStorageArea(storage, custom);
    If (!container)
      syslog(
        "FEHLER: Sonderfuellung "+custom+" fuer "+
        ItemInfoStr(chest, COORDS_REALM)+" ist weg"
      );
      return;
    EndIf

    // Fuellung umpacken
    ForEach item in (EnumerateItemsInContainer(container))
      SleepMs(2);
      MoveItemToContainer(item, chest);
    EndForEach

    // unnuetz gewordenen Container entsorgen
    DestroyRootItemInStorageArea(
      storage, ST_PREF_CHEST+LHex(custom)
    );
    return chest;
  EndIf

  // Standardkiste mit Loot
  var lootgroup := 5;
  Case (level)
    1:  lootgroup := 5;
    2:  lootgroup := 6;
    3:  lootgroup := 7;
    4:  lootgroup := 8;
    5:  lootgroup := 9;
    6:  lootgroup := 156;
    7:  lootgroup := 157;
    8:  lootgroup := 158;
    9:  lootgroup := 159;
    10: lootgroup := 160;
  EndCase

  If ((level == 10) And (RandomInt(100) <= 2)) // 2% Wahrscheinlichkeit
    var item := CreateItemInContainerPergon(chest, 0x14ef, 1);
    If (item)
      item.name      := "Die Kiste scheint leer ...";
      item.usescript := ":treasuremap:treasureletter";
    EndIf

    CreateItemInContainerPergon(chest, 0x3bf0, 1); // Der Zonk
  Else
    MakeLoot(chest, lootgroup);
  EndIf

  return chest;
EndFunction

// SetVisibleMap - Setzt die Gumpdaten fuer die oeffenbare Karte
Function SetVisibleMap(map, x, y)
  SetName(map, "eine Schatzkarte");

  If (x<265)
    map.xwest := 0;
    map.xeast := 2*265; // Verzerrung
  Else
    map.xwest := x-265;
    map.xeast := x+265;
  EndIf

  If (y<260)
    map.ynorth := 0;
    map.ysouth := 2*260; // Verzerrung
  Else
    map.ynorth := y-260;
    map.ysouth := y+260;
  EndIf

  map.gumpwidth := 200;
  map.gumpheight := 200;
EndFunction
