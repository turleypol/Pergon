////////////////////////////////////////////////////
//
//   ObjectTypes Include - Object Type Definitions
//
////////////////////////////////////////////////////

///////////////
//           //
//   Magie   //
//           //
///////////////

////////////
// Buecher
////////////

Const UOBJ_SPELLBOOK       :=0x0efa; // Altes Buch gemaess urspruenglichem System
Const UOBJ_NECROBOOK       :=0xb100; // Altes Buch
Const UOBJ_ALCHEMYBOOK     :=0xa100; // Altes Buch

Const UOBJ_SPELLBOOK_MAGE      :=0x6820; // Grimoire
Const UOBJ_SPELLBOOK_CLERIC    :=0x6821; // Liber Castus
Const UOBJ_SPELLBOOK_NECRO     :=0x6822; // paq mu'qaDmey

Const UOBJ_ARTEFACTBOOK_MAGE   :=0xe001; // Elemagartefaktbuch
Const UOBJ_ARTEFACTBOOK_CLERIC :=0xe002; // Priesterartefaktbuch
Const UOBJ_ARTEFACTBOOK_NECRO  :=0xe003; // Necroartefaktbuch

Const UOBJ_SPELLPANEL      :=0x682f; // SpellPanel

Const UOBJ_BEAMBLOCKER     :=0x6831; // Beamblocker

Const UOBJ_RUNE            :=0x1f14; // RecallRune
Const UOBJ_RUNEBOOK        :=0x6100; // Runenbuch

///////////////
// Reagenzien
///////////////

Const UOBJ_ALRAUNE          :=0x0f86;
Const UOBJ_BIMSSTEIN        :=0x0f8b;
Const UOBJ_BLACKROCK        :=0x6830;
Const UOBJ_BLUTEGEL         :=0x0f7c;
Const UOBJ_BLUTFLAKON       :=0x0f7d;
Const UOBJ_BLUTMOOS         :=0x0f7b;
Const UOBJ_DAEMONENKNOCHEN  :=0x0f80;
Const UOBJ_DRACHENBLUT      :=0x0f82;
Const UOBJ_FEUERSTEIN       :=0xf004;
Const UOBJ_FLEDERMAUSFLUEGEL:=0x0f78;
Const UOBJ_GINSENG          :=0x0f85;
Const UOBJ_HUMUS            :=0x0f81;
Const UOBJ_KNOBLAUCH        :=0x0f84;
Const UOBJ_KNOCHEN          :=0x0f7e;
Const UOBJ_MOLCHAUGE        :=0x0f87;
Const UOBJ_MORCHEL          :=0x0f83;
Const UOBJ_OBSIDIAN         :=0x0f89;
Const UOBJ_ROHEISEN         :=0x0f8a;
Const UOBJ_SCHLANGENSCHUPPE :=0x0f8e;
Const UOBJ_SCHWARZE_PERLE   :=0x0f7a;
Const UOBJ_SCHWEFELASCHE    :=0x0f8c;
Const UOBJ_SPINNENSEIDE     :=0x0f8d;
Const UOBJ_TOLLKIRSCHE      :=0x0f88;
Const UOBJ_TORF             :=0x0f79;
Const UOBJ_VULKANASCHE      :=0x0f8f;
Const UOBJ_WURZEL           :=0x0f90;
Const UOBJ_WYRMHERZ         :=0x0f91;



/////////////////////////////////////////////////////////////////////////////
//
// 0x0000 to 0x3FFF - "Standard" objects
//
/////////////////////////////////////////////////////////////////////////////

Const UOBJ_BACKPACK       := 0x0e75;

Const UOBJ_SPAWNRUNE      := 0xa300;

Const UOBJ_FISHSTEAK      := 0x097a;

Const UOBJ_BOX            := 0x09aa;

Const UOBJ_FISH_START     := 0x09cc;
Const UOBJ_FISH_END       := 0x09cf;

Const UOBJ_SFISH_START    := 0x0dd6;
Const UOBJ_SFISH_END      := 0x0dd9;

Const UOBJ_STUMP          := 0xefea;
Const UOBJ_KINDLING       := 0x0de1;
Const UOBJ_CAMPFIRE       := 0x0de3;
Const UOBJ_EMBERS         := 0x0de9;
Const UOBJ_ASHES          := 0x0dea;

Const UOBJ_BANDAGE        := 0x0e21;
Const UOBJ_BLANK_SCROLL   := 0x0e34;

Const UOBJ_BANKBOX        := 0x0e7c;

// Musik
Const UOBJ_DRUM           := 0x0e9c;
Const UOBJ_TAMB           := 0x0e9d;
Const UOBJ_HARP           := 0x0eb2;
Const UOBJ_LUTE           := 0x0eb3;
Const UOBJ_FLUT           := 0x2805;

Const UOBJ_BLANKMUSICSCROLL := 0x6103;
Const UOBJ_MUSICBOOK        := 0x6101;
Const UOBJ_MUSICSCROLL      := 0x6102;

Const UOBJ_GOLD_COIN      := 0x0eed;

Const UOBJ_POISON_POTION  := 0x0f0a;
Const UOBJ_EMPTY_BOTTLE   := 0x0f0e;

Const UOBJ_GEM_START      := 0x0f0f;
Const UOBJ_GEM_END        := 0x0f30;

Const UOBJ_SHOVEL         := 0x0f39;

Const UOBJ_BLUE_MOONGATE  := 0x0f6c;

Const UOBJ_CLOTH          := 0x0f95;
Const UOBJ_FOLDED_CLOTH   := 0x8005;

Const UOBJ_CLAY           := 0xee19;
Const UOBJ_SNOW			  := 0x0DEF;
Const UOBJ_SAND           := 0xee18;
Const UOBJ_SANDSTONE      := 0x8211;
Const UOBJ_GRANITSTONE    := 0x8212;
Const UOBJ_MARBLESTONE    := 0x8213;
Const UOBJ_GLASS          := 0xee20;
Const UOBJ_LEATHER        := 0x8025;
Const UOBJ_BOARD          := 0x1bd7;
Const UOBJ_STRAW          := 0x1ebd;

Const UOBJ_THREAD_START   := 0x0fa0;
Const UOBJ_THREAD_END     := 0x0fa1;

Const UOBJ_ANVIL1         := 0x0faf;
Const UOBJ_ANVIL2         := 0x0fb0;
Const UOBJ_SMALL_FORGE    := 0x0fb1;  // see also UOBJ_FORGE_START .. UOBJ_FORGE_END

Const UOBJ_KEY_START      := 0x100e;
Const UOBJ_COPPER_KEY     := 0x100e;
Const UOBJ_GOLD_KEY       := 0x100f;
Const UOBJ_IRON_KEY       := 0x1010;
Const UOBJ_KEY_RING       := 0x1011;
Const UOBJ_MAGIC_KEY      := 0x1012;
Const UOBJ_RUSTY_IRON_KEY := 0x1013;
Const UOBJ_KEY_END        := 0x1013;

Const UOBJ_CLOCK          := 0x104c;
Const UOBJ_CLOCK_FRAME    := 0x104e;
Const UOBJ_CLOCK_PARTS    := 0x104f;
Const UOBJ_AXLE_AND_GEARS := 0x1051;
Const UOBJ_GEARS          := 0x1053;
Const UOBJ_HINGE          := 0x1055;
Const UOBJ_SEXTANT        := 0x1057;
Const UOBJ_SEXTANT_PARTS  := 0x1059;
Const UOBJ_AXLE           := 0x105b;
Const UOBJ_SPRINGS        := 0x105d;

Const UOBJ_LOOM_START     := 0x105f;
Const UOBJ_LOOM_END       := 0x1066;

Const UOBJ_DUMMYSWING_NS  := 0x1071;
Const UOBJ_DUMMYFREEZE_NS := 0x1070;
Const UOBJ_DUMMYSWING_EW  := 0x1075;
Const UOBJ_DUMMYFREEZE_EW := 0x1074;

Const UOBJ_WHEEL1         := 0x10a4;
Const UOBJ_WHEEL2         := 0x1019;
Const UOBJ_WHEEL3         := 0x101c;
Const UOBJ_WHEEL4         := 0x1015;

Const UOBJ_BLOOD          := 0x122d;

Const UOBJ_BOW            := 0x13b2;
Const UOBJ_SBOW           := 0x13b1;
Const UOBJ_XBOW           := 0x0f4f;
Const UOBJ_HEAVY_XBOW     := 0x13fd;

Const UOBJ_MAP1           := 0x14eb;
Const UOBJ_MAP2           := 0x14ec;
Const UOBJ_ROLLED_MAP1    := 0x14ed;
Const UOBJ_ROLLED_MAP2    := 0x14ee;

Const UOBJ_HOUSE_DEED1    := 0x14ef; // Eigentumsnachweis
Const UOBJ_SHIP_DEED1     := 0x14f1;
Const UOBJ_SHIP_DEED2     := 0x14f2;
Const UOBJ_SHIP_MODEL1    := 0x14f3;
Const UOBJ_SHIP_MODEL2    := 0x14f4;

Const UOBJ_SHOE_START     := 0x16f8;
Const UOBJ_SHOE_END       := 0x1712;

Const UOBJ_CHEST          := 0x1940;

Const UOBJ_FORGE_START    := 0x197a;  // see also UOBJ_SMALL_FORGE
Const UOBJ_FORGE_END      := 0x19a9;

Const UOBJ_FEATHER        := 0x1bd1;
Const UOBJ_SHAFTS         := 0x1bd4;
Const UOBJ_LOGS           := 0x1bdd;

Const UOBJ_ARM            := 0x1cec;
Const UOBJ_LEG            := 0x1ce2;
Const UOBJ_TRUNK          := 0x1cee;
Const UOBJ_HEART          := 0x1cef;
Const UOBJ_HEAD           := 0x1dae;

Const UOBJ_HEAD_NEW       := 0x1da0;
Const UOBJ_ARM_LEFT       := 0x1da1;
Const UOBJ_ARM_RIGHT      := 0x1da2;
Const UOBJ_LEG_LEFT       := 0x1da3;
Const UOBJ_LEG_RIGHT      := 0x1da4;
Const UOBJ_TORSO          := 0x1d9f;

Const UOBJ_TINKER_TOOLS   := 0x1ebc;
Const UOBJ_BUTCHERKNIFE1  := 0x13f6;
Const UOBJ_BUTCHERKNIFE2  := 0x13f6;
Const UOBJ_BUTCHERKNIFE3  := 0x0ec2;
Const UOBJ_BUTCHERKNIFE4  := 0x0ec3;
Const UOBJ_SKINNINGKNIFE1 := 0x0ec4;
Const UOBJ_SKINNINGKNIFE2 := 0x0ec5;

Const UOBJ_PENINK         := 0x0fc0;
Const UOBJ_RAWPAPER       := 0xaf04;

Const UOBJ_NOLOOTLANTERN  := 0xaf05;

Const UOBJ_CORPSE         := 0x2006;

Const UOBJ_TICKET         := 0x14f0;

Const UOBJ_TOWNSTONE      := 0xa320;
Const UOBJ_GUILDSTONE     := 0xa390;

Const UOBJ_STONEWALL_NS   := 0x6003;
Const UOBJ_STONEWALL_EW   := 0x6004;
Const UOBJ_FIREFIELD_NS   := 0x398c;
Const UOBJ_FIREFIELD_EW   := 0x3996;
Const UOBJ_EFIELD_NS      := 0x3946;
Const UOBJ_EFIELD_EW      := 0x3956;
Const UOBJ_PARAFIELD_NS   := 0x3967;
Const UOBJ_PARAFIELD_EW   := 0x3979;
Const UOBJ_POISONFIELD_NS := 0x3915;
Const UOBJ_POISONFIELD_EW := 0x3921;

Const GID_PLANK_EXTENDED_FACING_WEST   := 0x3ed5;
Const GID_PLANK_RETRACTED_FACING_WEST  := 0x3eb1;
Const GID_PLANK_EXTENDED_FACING_EAST   := 0x3ed4;
Const GID_PLANK_RETRACTED_FACING_EAST  := 0x3eb2;
Const GID_PLANK_EXTENDED_FACING_SOUTH  := 0x3e84;
Const GID_PLANK_RETRACTED_FACING_SOUTH := 0x3e85;
Const GID_PLANK_EXTENDED_FACING_NORTH  := 0x3e89;
Const GID_PLANK_RETRACTED_FACING_NORTH := 0x3e8a;

// ick! these are fine for humans, but not true for items..
// these are more of a CID.
Const UOBJ_HUMAN_MALE   := 0x190;
Const UOBJ_HUMAN_FEMALE := 0x191;
Const UOBJ_ELF_MALE     := 0x25d;
Const UOBJ_ELF_FEMALE   := 0x25e;

/////////////////////////////////////////////////////////////////////////////
//
// 0x4000 to 0x4FFF - Multis
//
/////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////
//
// 0x5000 to 0xeFFF - To Be Determined
//
/////////////////////////////////////////////////////////////////////////////

//
// TODO: reserve some of these ranges for weapons, armor, wands
//       explicitly leave an area for end users to play with
//

// recommendation: start these out somewhere like 0x6300, and reserve
// enough space for N (Say, 64) colors of ore and ingots
// they don't all have to be defined yet, just reserved.

//Const UOBJ_IRON_ORE         := 0x6009;
//Const UOBJ_BRONZE_ORE       := 0x600a;
//Const UOBJ_DULLCOPPER_ORE   := 0x600b;
//Const UOBJ_COPPER_ORE       := 0x600c;
//Const UOBJ_ONYX_ORE         := 0x600d;
//Const UOBJ_PYRITE_ORE       := 0x600e;
//Const UOBJ_MALACHITE_ORE    := 0x600f;
//Const UOBJ_AZURITE_ORE      := 0x6010;
Const UOBJ_EISEN_ERZ    :=0x6009;
Const UOBJ_MOLYBDAEN_ERZ:=0x600a;
Const UOBJ_MANGAN_ERZ   :=0x600b;
Const UOBJ_IRIDIUM_ERZ  :=0x600c;
Const UOBJ_TENERUM_ERZ  :=0x600d;
Const UOBJ_INDIUM_ERZ   :=0x600e;
Const UOBJ_LANTHAN_ERZ  :=0x600f;
Const UOBJ_WOLFRAM_ERZ  :=0x6010;
Const UOBJ_PLATIN_ERZ   :=0x6011;
Const UOBJ_CHROM_ERZ    :=0x6012;
Const UOBJ_TITANIUM_ERZ :=0x6013;

//Const UOBJ_BARREN_START     := 0x6011;
//Const UOBJ_IRON_INGOT       := 0x6011;
//Const UOBJ_BRONZE_INGOT     := 0x6012;
//Const UOBJ_DULLCOPPER_INGOT := 0x6013;
//Const UOBJ_COPPER_INGOT     := 0x6014;
//Const UOBJ_ONYX_INGOT       := 0x6015;
//Const UOBJ_PYRITE_INGOT     := 0x6016;
//Const UOBJ_MALACHITE_INGOT  := 0x6017;
//Const UOBJ_AZURITE_INGOT    := 0x6018;
//Const UOBJ_BARREN_END       := 0x6018;
Const UOBJ_BARREN_START     := 0x6300;
Const UOBJ_EISEN_BARREN     := 0x6300;
Const UOBJ_MOLYBDAEN_BARREN := 0x6301;
Const UOBJ_MANGAN_BARREN    := 0x6302;
Const UOBJ_IRIDIUM_BARREN   := 0x6303;
Const UOBJ_TENERUM_BARREN   := 0x6304;
Const UOBJ_INDIUM_BARREN    := 0x6305;
Const UOBJ_LANTHAN_BARREN   := 0x6306;
Const UOBJ_WOLFRAM_BARREN   := 0x6307;
Const UOBJ_PLATIN_BARREN    := 0x6308;
Const UOBJ_CHROM_BARREN     := 0x6309;
Const UOBJ_TITANIUM_BARREN  := 0x630a;
Const UOBJ_CERIUM_BARREN    := 0x630b;
Const UOBJ_PRODYMIUM_BARREN := 0x630c;
Const UOBJ_NEODYMIUM_BARREN := 0x630d;
Const UOBJ_PROMETHIUM_BARREN:= 0x630e;
Const UOBJ_SAMARIUM_BARREN  := 0x630f;
Const UOBJ_ROPIUM_BARREN    := 0x6310;
Const UOBJ_GADONIUM_BARREN  := 0x6311;
Const UOBJ_TERBIUM_BARREN   := 0x6312;
Const UOBJ_HOLMIUM_BARREN   := 0x6313;
Const UOBJ_ERBIUM_BARREN    := 0x6314;
Const UOBJ_SOLARUM_BARREN   := 0x6315;
Const UOBJ_HYDRIUM_BARREN   := 0x6316;
Const UOBJ_LAVARIUM_BARREN  := 0x6317;
Const UOBJ_TERADIUM_BARREN  := 0x6318;
Const UOBJ_TERATUM_BARREN   := 0x6319;
Const UOBJ_ENERGIUM_BARREN  := 0x631a;
Const UOBJ_BARREN_END       := 0x631a;

Const UOBJ_GOLD_BARREN      := 0x1be9;
Const UOBJ_OBSIDIAN_BARREN  := 0xff98;
Const UOBJ_TITAN_OBSIDIAN_BARREN := 0xff99;

CONST ITEM_WALNUSSHOLZ   := 0x7012;
CONST ITEM_ZYPRESSENHOLZ := 0x7017;
CONST ITEM_TROPENHOLZ    := 0x7019;
CONST ITEM_APFELHOLZ 	   := 0x7013;
CONST ITEM_BIRNENHOLZ 	 := 0x7014;
CONST ITEM_PFIRSICHHOLZ  := 0x701a;
CONST ITEM_ZEDERNHOLZ 	 := 0x701b;
CONST ITEM_TANNENHOLZ 	 := 0x701c;
CONST ITEM_FICHTENHOLZ 	 := 0x701e;
CONST ITEM_BUCHENHOLZ 	 := 0x701f;
CONST ITEM_ELFENBEINHOLZ := 0x7021;

CONST ITEM_MAHAGONIHOLZ	 := 0x7010;
CONST ITEM_WEIDENHOLZ 	 := 0x7016;
CONST ITEM_EICHENHOLZ 	 := 0x7018;
CONST ITEM_EDELHOLZ 	   := 0x7015;
CONST ITEM_SCHRATHOLZ 	 := 0x7011;
CONST ITEM_ERLENHOLZ 	   := 0x701d;
CONST ITEM_EBENHOLZ 	   := 0x7020;

//
// Ship Components
//

Const EXTOBJ_TILLERMAN       := 0xf010;
Const EXTOBJ_PORT_PLANK      := 0xf011;
Const EXTOBJ_STARBOARD_PLANK := 0xf012;
Const EXTOBJ_HOLD            := 0xf013;

//
// Felle
//

Const UOBJ_NORMALFELL         := 0x11f9;
Const UOBJ_HASENFELL          := 0x8010;
Const UOBJ_SCHLANGENHAUT      := 0x8011;
Const UOBJ_WOLFSFELL          := 0x8012;
Const UOBJ_DARKWOLFFELL       := 0x8013;
Const UOBJ_GREMLINFELL        := 0x8014;
Const UOBJ_LAMAFELL           := 0x8015;
Const UOBJ_BAERENFELL         := 0x8016;
Const UOBJ_GRIZZLYFELL        := 0x8017;
Const UOBJ_PANTHERFELL        := 0x8018;
Const UOBJ_ECHSENHAUT         := 0x8019;
Const UOBJ_FEUERECHSENHAUT    := 0x801a;
Const UOBJ_OSTARDFELL         := 0x801b;
Const UOBJ_WALDOSTARDFELL     := 0x801c;
Const UOBJ_RAUBOSTARDFELL     := 0x801d;
Const UOBJ_TROLLFELL          := 0x8020;
Const UOBJ_AZURFELL           := 0x8021;
Const UOBJ_GROSSSCHLANGENFELL := 0x8022;
Const UOBJ_DRACHENFELL        := 0x8023;
Const UOBJ_OLD_DRAGONFELL     := 0x8024;
Const UOBJ_ZENTAURENFELL1     := 0x8300;
Const UOBJ_ZENTAURENFELL2     := 0x8301;
Const UOBJ_ZENTAURENFELL3     := 0x8302;
Const UOBJ_ZENTAURENFELL4     := 0x8303;

Const UOBJ_SCHAEDEL               :=0x1ae2;
Const UOBJ_DAEMONENSCHAEDEL       :=0x6320;
Const UOBJ_TROLLSCHAEDEL          :=0x6321;
Const UOBJ_TROLLKNOCHEN           :=0x6322;
Const UOBJ_ECHSENSCHUPPE          :=0x6323;
Const UOBJ_DRACHENSCHUPPE         :=0x6324;
Const UOBJ_DRACHENSCHUPPEGRUEN    :=0x6325;
Const UOBJ_DRACHENSCHUPPEBLAU     :=0x6326;
Const UOBJ_ECHSENSCHUPPEBRONZE    :=0x6327;
Const UOBJ_DRACHENKNOCHEN         :=0x6328;
Const UOBJ_DRACHENSCHAEDEL        :=0x6329;
Const UOBJ_DRACHENSCHUPPEEISBLAU  :=0x6330;
Const UOBJ_DRACHENSCHUPPESILBER   :=0x6331;
Const UOBJ_ECHSENSCHUPPEGOLD      :=0x6332;
Const UOBJ_ZENTAURENKNOCHEN       :=0x6333;
Const UOBJ_ZENTAURENSCHAEDEL      :=0x6334;


/////////////////////////////////////////////////////
//  ArtefaktReags/Teile
/////////////////////////////////////

Const UOBJ_BEUTEL          :=0x0e76;
Const UOBJ_RUCKSACK        :=0x0e75;
Const UOBJ_KISTE           :=0x0e80;
Const UOBJ_TRUHE           :=0x0e42;
Const UOBJ_BLUTIGEBANDAGE  :=0x0e20;
Const UOBJ_SCHNEEBALL      :=0x669d;
Const UOBJ_HONIG           :=0x09ec;
Const UOBJ_BIENENWACHS     :=0x1422;
Const UOBJ_EIERSCHALE      :=0x09b4;
COnst UOBJ_KOPF            :=0x1dae;
Const UOBJ_GEHIRN          :=0x1cf0;
Const UOBJ_BEIN1           :=0x1ce2;
Const UOBJ_BEIN2           :=0x1cec;
Const UOBJ_HERZ            :=0x1ced;
Const UOBJ_LEBER           :=0x1cee;
Const UOBJ_RECALLRUNE      :=0x1f14;
Const UOBJ_FISCHFILET      :=0x097a;

Const UOBJ_DAEMONENBLUT    :=0x0e010;
Const UOBJ_TROLLBLUT       :=0x0e011;
Const UOBJ_EINHORNBLUT     :=0x0e012;
Const UOBJ_SUMPFKRAUT      :=0x0e020;
Const UOBJ_STEINKRAUT      :=0x0e021;
Const UOBJ_FEUERKRAUT      :=0x0e022;
Const UOBJ_NACHTKRAUT      :=0x0e023;

Const UOBJ_FEENSTAUB       :=0x0e031;
Const UOBJ_KRISTALLSPLITTER:=0x0e032;
Const UOBJ_SUCCUBUSFLUEGEL :=0x0e033;
Const UOBJ_NAGASCHUPPE     :=0x0e034;
Const UOBJ_WEIHWASSER      :=0x0e035;

Const UOBJ_SCHWACHERHEILTRANK   :=0xdc01;
Const UOBJ_HEILTRANK            :=0xdc02;
Const UOBJ_GROSSERHEILTRANK     :=0xdc03;
Const UOBJ_SCHWACHESGIFT        :=0xdc04;
Const UOBJ_GIFT                 :=0xdc05;
Const UOBJ_STARKESGIFT          :=0xdc06;
Const UOBJ_TOEDLICHESGIFT       :=0xdc17;
Const UOBJ_KLEINESPRENGFLASCHE  :=0xdc08;
Const UOBJ_SPRENGFLASCHE        :=0xdc09;
Const UOBJ_GROSSESPRENGFLASCHE  :=0xdc07;

Const UOBJ_STERNSAPHIR          :=0x0f0f;
Const UOBJ_SMARAGD              :=0x0f10;
Const UOBJ_SAPHIR               :=0x0f11;
Const UOBJ_RUBIN                :=0x0f13;
Const UOBJ_ZITRIN               :=0x0f15;
Const UOBJ_AMETHYST             :=0x0f16;
Const UOBJ_TURMALIN             :=0x0f18;
Const UOBJ_BERNSTEIN            :=0x0f25;
Const UOBJ_DIAMANT              :=0x0f30;

Const UOBJ_EISBALL              :=0xe123;
Const UOBJ_MAGISCHERMETEORIT    :=0xe141;
Const UOBJ_MAGISCHERMETEOR      :=0xe151;
Const UOBJ_MAGISCHERKOMET       :=0xe161;
Const UOBJ_MAGISCHERASTEROID    :=0xe171;

Const UOBJ_KRAEUTERBANDAGE      :=0xe223;
Const UOBJ_STERILEBANDAGE       :=0xe242;
Const UOBJ_GESEGNETEBANDAGE     :=0xe252;
Const UOBJ_HEILIGEBANDAGE       :=0xe272;

Const UOBJ_STREITKOLBEN         :=0x1406;
Const UOBJ_BARBARENAXT          :=0xf4b;
Const UOBJ_SPEER                :=0xf62;
Const UOBJ_DOLCH                :=0xf51;
Const UOBJ_MAGIERSTAB           :=0x0df0;
Const UOBJ_SCHWERT              :=0xf5e;


//Quest
Const QUEST_DUMMY               :=0xff89;

Const UOBJ_SEED                 :=0xff86;

Const UOBJ_ACKER                :=0x63f;

Const UOBJ_ALCHYPOTION          :=0xffa1; // Trank
Const UOBJ_RECIPE               :=0x226b; // Objecttype der Rezeptrollen
