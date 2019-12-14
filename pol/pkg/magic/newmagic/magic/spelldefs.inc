///////////////////////////////////////////////////////////////////////////////////////////////
//
//   SpellDefs Include
//
//     Author: Shinigami
//
//   Modifications:
//     2001/12/27 Shinigami: SpellBook-Konstanten -> ObjType.inc
//     2006/06/03 Shinigami: neue Magie-Symbole entfernt (stattdessen auf Gumps umgeschwenkt)
//
///////////////////////////////////////////////////////////////////////////////////////////////

///////////////
// Konstanten
///////////////

Const MAGE_DESC:=  "Mage";           // Magierklassen-[Kurz]Bezeichnung
Const CLERIC_DESC:="Cleric";
Const NECRO_DESC:= "Necro";

Const MIN_MAGE_SPELL_ITEM:=  0x6700; // Inscription-Scrolls gemaess Magier-Klasse (ItemID)
Const MAX_MAGE_SPELL_ITEM:=  0x673f;
Const MIN_CLERIC_SPELL_ITEM:=0x6760;
Const MAX_CLERIC_SPELL_ITEM:=0x679f;
Const MIN_NECRO_SPELL_ITEM:= 0x67c0;
Const MAX_NECRO_SPELL_ITEM:= 0x67ff;

Const MIN_MAGE_CAST_ITEM:=   0x6900; // Cast-Scrolls gemaess Magier-Klasse        (ItemID)
Const MAX_MAGE_CAST_ITEM:=   0x693f;
Const MIN_CLERIC_CAST_ITEM:= 0x6960;
Const MAX_CLERIC_CAST_ITEM:= 0x699f;
Const MIN_NECRO_CAST_ITEM:=  0x69c0;
Const MAX_NECRO_CAST_ITEM:=  0x69ff;

Const MIN_SPELL_ITEM:=MIN_MAGE_SPELL_ITEM;  // Inscription-/Cast-Scrolls unabhaengig von Magier-Klasse (ItemID)
Const MAX_SPELL_ITEM:=MAX_NECRO_SPELL_ITEM;
Const MIN_CAST_ITEM:= MIN_MAGE_CAST_ITEM;
Const MAX_CAST_ITEM:= MAX_NECRO_CAST_ITEM;

Const CPROP_NEW_SPELLS:="NewSpells"; // Neue Spell-Grafiken anzeigen?

Const GUMP_MAGE_CIRCLE:=     0x3040; // Circle-Symbole gemaess Magier-Klasse (klein)
Const GUMP_CLERIC_CIRCLE:=   0x3048;
Const GUMP_NECRO_CIRCLE:=    0x3050;

Const GUMP_MAGE_SPELL:=      0x3000; // Spellsymbole gemaess Magier-Klasse (klein)
Const GUMP_CLERIC_SPELL:=    0x3060;
Const GUMP_NECRO_SPELL:=     0x30c0;
Const GUMP_OLD_SPELL:=        0x8C0;

Const SYMBOL_MAGE_CIRCLE:=   0x3a40; // [unbenutzt] Circle-Symbole gemaess Magier-Klasse     (ItemID)
Const SYMBOL_CLERIC_CIRCLE:= 0x3a48; // [unbenutzt]
Const SYMBOL_NECRO_CIRCLE:=  0x3a50; // [unbenutzt]
Const SYMBOL_OLD_CIRCLE:=    0x20c0;

Const SYMBOL_MAGE_SPELL:=    0x3a00; // [unbenutzt] Spell-Symbole gemaess Magier-Klasse      (ItemID)
Const SYMBOL_CLERIC_SPELL:=  0x3a60; // [unbenutzt]
Const SYMBOL_NECRO_SPELL:=   0x3ac0; // [unbenutzt]
Const SYMBOL_OLD_SPELL:=     0x2080;

Const MIN_MAIN_SPELL:=            1; // Spellnummern gemaess Magier-Klasse       (SpellID)
Const MAX_MAIN_SPELL:=           32;
Const MIN_MAGE_SPELL:=          101;
Const MAX_MAGE_SPELL:=          132;
Const MIN_CLERIC_SPELL:=        201;
Const MAX_CLERIC_SPELL:=        232;
Const MIN_NECRO_SPELL:=         301;
Const MAX_NECRO_SPELL:=         332;

Const COUNT_MAIN_SPELLS:=MAX_MAIN_SPELL-MIN_MAIN_SPELL+1;