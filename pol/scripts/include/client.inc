////////////////////////////////////////////////////////////////////////////
//
//  CLIENT.inc - defines client releated constants.
//
//  This include file is designed to hold all the client lowlevel constants.
//  Not all constants might be usable as of now, but in the future, they
//  might be.
//
//  Author: Ignatius 1999/01/20
//
//  Recent Modifications
//      1999/01/25 Ignatius added animations/actions/sounds.
//      1999/01/26 Ignatius moved skillids/constants into this file and
//                 added a lot of more sounds.
//      1999/01/27 Ignatius found out that I got the sounds all wrong
//                 it seems, fixed up a lot of things there. Started
//                 to map out the T2A sounds. Added some creature IDs.
//
//      27.03.2006 Turley - 2 Fontfarben hinzugefügt für Gildenchat
//      29.03.2006 Turley - Und nochmal Farbkram
//      2008-04-12 mehdorn  Feuer und ein paar Actions
//
////////////////////////////////////////////////////////////////////////////

// Client Flags
Enum ClientType
  CLIENTTYPE_4000  := 0x1,   // 4.0.0a (new spellbookcontent packet 0xbf:0x1b)
  CLIENTTYPE_4070  := 0x2,   // 4.0.7a (new damage packet 0x0b instead of 0xbf:0x22)
  CLIENTTYPE_5000  := 0x4,   // 5.0.0a (compressed gumps)
  CLIENTTYPE_5020  := 0x8,   // 5.0.2a (Buff/Debuff 0xdf)
  CLIENTTYPE_6017  := 0x10,  // 6.0.1.7 (Grid locs)
  CLIENTTYPE_60142 := 0x20,  // 6.0.14.2 (feature enable 0xb9 size change)
  CLIENTTYPE_UOKR  := 0x40,
  CLIENTTYPE_7000  := 0x80,  // 7.0.0.0 (Gargoyle race)
  CLIENTTYPE_UOSA  := 0x100 
EndEnum

Enum ClientExpansion
  FLAG_T2A         := 0x00,
  FLAG_RENAISSANCE := 0x01,
  FLAG_THIRD_DAWN  := 0x02,
  FLAG_LBR         := 0x04,
  FLAG_AOS         := 0x08,
  FLAG_SE          := 0x10,
  FLAG_SA          := 0x20,
  FLAG_UO3D        := 0x40, // ?
  FLAG_RESERVED    := 0x80,
  FLAG_3DCLIENT    := 0x100 // 3d Client 
EndEnum

////
// Soundeffects
// The first section is just an enum of existing sounds,
// the second builds on the first ones, but providing more
// logical names.
///

Const SFX_1     :=  0x0001; // forest
Const SFX_2     :=  0x0002; // forest
Const SFX_3     :=  0x0003; // forest
Const SFX_4     :=  0x0004; // jungle
Const SFX_5     :=  0x0005; // jungle
Const SFX_6     :=  0x0006; // jungle
Const SFX_7     :=  0x0007; // swamp
Const SFX_8     :=  0x0008; // swamp
Const SFX_9     :=  0x0009; // forest
Const SFX_A     :=  0x000a; // forest
Const SFX_B     :=  0x000b; // generic
Const SFX_C     :=  0x000c; // generic
Const SFX_D     :=  0x000d; // jungle
Const SFX_E     :=  0x000e; // jungle
Const SFX_F     :=  0x000f; // swamp
Const SFX_10    :=  0x0010; // swamp
Const SFX_11    :=  0x0011; // water
Const SFX_12    :=  0x0012; // water
Const SFX_13    :=  0x0013; // waves
Const SFX_14    :=  0x0014; // waves
Const SFX_15    :=  0x0015; // wind
Const SFX_16    :=  0x0016; // wind
Const SFX_17    :=  0x0017; // wind
Const SFX_18    :=  0x0018; // bird
Const SFX_19    :=  0x0019; // bird
Const SFX_1A    :=  0x001a; // bird
Const SFX_1B    :=  0x001b; // bird
Const SFX_1C    :=  0x001c; // bird
Const SFX_1D    :=  0x001d; // bird
Const SFX_1E    :=  0x001e; // bird
Const SFX_1F    :=  0x001f; // bird
Const SFX_20    :=  0x0020; // bird
Const SFX_21    :=  0x0021; // bubbles
Const SFX_22    :=  0x0022; // bubbles
Const SFX_23    :=  0x0023; // dripping
Const SFX_24    :=  0x0024; // dripping
Const SFX_25    :=  0x0025; // dripping
Const SFX_26    :=  0x0026; // splashing
Const SFX_27    :=  0x0027; // splashing
Const SFX_28    :=  0x0028; // splashing
Const SFX_29    :=  0x0029; // thundr01.wav, thunder
Const SFX_2A    :=  0x002a; // thundr02.wav, thunder
Const SFX_2B    :=  0x002b; // anvil.wav, anvil
Const SFX_2C    :=  0x002c; // bellows.wav, bellows
Const SFX_2D    :=  0x002d; // chstcls3.wav, chest close
Const SFX_2E    :=  0x002e; // chstopn2.wav, chest open
Const SFX_2F    :=  0x002f; // drawercl.wav, drawer close
Const SFX_30    :=  0x0030; // drawerop.wav, drawer open
Const SFX_31    :=  0x0031; // drink01.wav, drink
Const SFX_32    :=  0x0032; // drink05.wav, drink
Const SFX_33    :=  0x0033; // dropgem1.wav, drop gem
Const SFX_34    :=  0x0034; // dropgem2.wav, drop gem
Const SFX_35    :=  0x0035; // dropgem3.wav, drop gem
Const SFX_36    :=  0x0036; // dropmny1.wav, drop money
Const SFX_37    :=  0x0037; // dropmny2.wav, drop money
Const SFX_38    :=  0x0038; // dropmny3.wav, drop money
Const SFX_39    :=  0x0039; // drum01.wav, drum
Const SFX_3A    :=  0x003a; // drum01_b.wav, drum
Const SFX_3B    :=  0x003b; // eat01.wav, eat
Const SFX_3C    :=  0x003c; // eat03.wav, eat
Const SFX_3D    :=  0x003d; // eat06.wav,eat
Const SFX_3E    :=  0x003e; // flute01.wav, flute
Const SFX_3F    :=  0x003f; // glasbrk1.wav, glassbreak
Const SFX_40    :=  0x0040; // glasbrk2.wav, glassbreak
Const SFX_41    :=  0x0041; // glasbrk3.wav, glassbreak
Const SFX_42    :=  0x0042; // glasbrk4.wav, glassbreak
Const SFX_43    :=  0x0043; // hamer01.wav, hammer
Const SFX_44    :=  0x0044; // harp01.wav, harp
Const SFX_45    :=  0x0045; // harp01_b.wav, harp
Const SFX_46    :=  0x0046; // harp02.wav, harp
Const SFX_47    :=  0x0047; // harp02_b.wav, harp
Const SFX_48    :=  0x0048; // igntorch.wav, ignite torch
Const SFX_49    :=  0x0049; // leather1.wav, leather
Const SFX_4A    :=  0x004a; // lever01.wav, lever
Const SFX_4B    :=  0x004b; // lever02.wav, lever
Const SFX_4C    :=  0x004c; // lever05.wav, lever
Const SFX_4D    :=  0x004d; // lute01.wav, lute
Const SFX_4E    :=  0x004e; // lute01_b.wav, lute
Const SFX_4F    :=  0x004f; // pouring.wav, pouring
Const SFX_50    :=  0x0050; // rustle.wav, rustle
Const SFX_51    :=  0x0051; // swallow2.wav, swallow
Const SFX_52    :=  0x0052; // switch01.wav, switch
Const SFX_53    :=  0x0053; // tamborin.wav, tamborine
Const SFX_54    :=  0x0054; // tambrn_b.wav, tamborine
Const SFX_55    :=  0x0055; // torch01.wav, torch
Const SFX_56    :=  0x0056; // turnpage.wav, turn page
Const SFX_57    :=  0x0057; // unsheth1.wav, ?
Const SFX_58    :=  0x0058; // usecloth.wav, use cloth
Const SFX_59    :=  0x0059; // uselethr.wav, use leather
Const SFX_5A    :=  0x005a; // uselthr1.wav, use leather
Const SFX_5B    :=  0x005b; // aligatr1.wav, alligator
Const SFX_5C    :=  0x005c; // aligatr2.wav, alligator
Const SFX_5D    :=  0x005d; // aligatr3.wav, alligator
Const SFX_5E    :=  0x005e; // aligatr4.wav, alligator
Const SFX_5F    :=  0x005f; // aligatr5.wav, alligator
Const SFX_60    :=  0x0060; // bear1.wav, bear
Const SFX_61    :=  0x0061; // bear2.wav, bear
Const SFX_62    :=  0x0062; // bear3.wav, bear
Const SFX_63    :=  0x0063; // bear4.wav, bear
Const SFX_64    :=  0x0064; // bear5.wav, bear
Const SFX_65    :=  0x0065; // bull1.wav, bull
Const SFX_66    :=  0x0066; // bull2.wav, bull
Const SFX_67    :=  0x0067; // bull3.wav, bull
Const SFX_68    :=  0x0068; // bull4.wav, bull
Const SFX_69    :=  0x0069; // bull5.wav, bull
Const SFX_6A    :=  0x006a; // cat1.wav
Const SFX_6B    :=  0x006b; // cat2.wav
Const SFX_6C    :=  0x006c; // cat3.wav
Const SFX_6D    :=  0x006d; // cat4.wav
Const SFX_6E    :=  0x006e; // cat5.wav
Const SFX_6F    :=  0x006f; // chicken1.wav
Const SFX_70    :=  0x0070; // chicken2.wav
Const SFX_71    :=  0x0071; // chicken3.wav
Const SFX_72    :=  0x0072; // chicken4.wav
Const SFX_73    :=  0x0073; // chicken5.wav
Const SFX_74    :=  0x0074; // cougar1.wav
Const SFX_75    :=  0x0075; // cougar2.wav
Const SFX_76    :=  0x0076; // cougar3.wav
Const SFX_77    :=  0x0077; // cougar4.wav
Const SFX_78    :=  0x0078; // cougar5.wav
Const SFX_79    :=  0x0079; // cow1.wav
Const SFX_7A    :=  0x007a; // cow2.wav
Const SFX_7B    :=  0x007b; // cow3.wav
Const SFX_7C    :=  0x007c; // cow4.wav
Const SFX_7D    :=  0x007d; // cow5.wav
Const SFX_7E    :=  0x007e; // crow1.wav
Const SFX_7F    :=  0x007f; // crow2.wav
Const SFX_80    :=  0x0080; // crow3.wav
Const SFX_81    :=  0x0081; // crow4.wav
Const SFX_82    :=  0x0082; // crow5.wav
Const SFX_83    :=  0x0083; // deer3.wav
Const SFX_84    :=  0x0084; // deer4.wav
Const SFX_85    :=  0x0085; // deer5.wav
Const SFX_86    :=  0x0086; // dog1.wav
Const SFX_87    :=  0x0087; // dog2.wav
Const SFX_88    :=  0x0088; // dog3.wav
Const SFX_89    :=  0x0089; // dog4.wav
Const SFX_8A    :=  0x008a; // dog5.wav
Const SFX_8B    :=  0x008b; // dolphin1.wav
Const SFX_8C    :=  0x008c; // dolphin2.wav
Const SFX_8D    :=  0x008d; // dolphin3.wav
Const SFX_8E    :=  0x008e; // dolphin4.wav
Const SFX_8F    :=  0x008f; // dolphin5.wav
Const SFX_90    :=  0x0090; // eagle1.wav
Const SFX_91    :=  0x0091; // eagle2.wav
Const SFX_92    :=  0x0092; // eagle3.wav
Const SFX_93    :=  0x0093; // eagle4.wav
Const SFX_94    :=  0x0094; // eagle5.wav
Const SFX_95    :=  0x0095; // frstbrd1.wav
Const SFX_96    :=  0x0096; // frstbrd2.wav
Const SFX_97    :=  0x0097; // frstbrd3.wav
Const SFX_98    :=  0x0098; // frstbrd4.wav
Const SFX_99    :=  0x0099; // frstbrd5.wav
Const SFX_9A    :=  0x009a; // goat1.wav
Const SFX_9B    :=  0x009b; // goat2.wav
Const SFX_9C    :=  0x009c; // goat3.wav
Const SFX_9D    :=  0x009d; // goat4.wav
Const SFX_9E    :=  0x009e; // goat5.wav
Const SFX_9F    :=  0x009f; // gorilla1.wav
Const SFX_A0    :=  0x00a0; // gorilla2.wav
Const SFX_A1    :=  0x00a1; // gorilla3.wav
Const SFX_A2    :=  0x00a2; // gorilla4.wav
Const SFX_A3    :=  0x00a3; // gorilla5.wav
Const SFX_A4    :=  0x00a4; // grizzly1.wav
Const SFX_A5    :=  0x00a5; // grizzly2.wav
Const SFX_A6    :=  0x00a6; // grizzly3.wav
Const SFX_A7    :=  0x00a7; // grizzly4.wav
Const SFX_A8    :=  0x00a8; // grizzly5.wav
Const SFX_A9    :=  0x00a9; // horse1.wav
Const SFX_AA    :=  0x00aa; // horse2.wav
Const SFX_AB    :=  0x00ab; // horse3.wav
Const SFX_AC    :=  0x00ac; // horse4.wav
Const SFX_AD    :=  0x00ad; // horse5.wav
Const SFX_AE    :=  0x00ae; // jungbrd1.wav
Const SFX_AF    :=  0x00af; // jungbrd2.wav
Const SFX_B0    :=  0x00b0; // jungbrd3.wav
Const SFX_B1    :=  0x00b1; // jungbrd4.wav
Const SFX_B2    :=  0x00b2; // jungbrd5.wav
Const SFX_B3    :=  0x00b3; // lion1.wav
Const SFX_B4    :=  0x00b4; // lion2.wav
Const SFX_B5    :=  0x00b5; // lion3.wav
Const SFX_B6    :=  0x00b6; // lion4.wav
Const SFX_B7    :=  0x00b7; // lion5.wav
Const SFX_B8    :=  0x00b8; // llama3.wav
Const SFX_B9    :=  0x00b9; // llama4.wav
Const SFX_BA    :=  0x00ba; // llama5.wav
Const SFX_BB    :=  0x00bb; // panther1.wav
Const SFX_BC    :=  0x00bc; // panther2.wav
Const SFX_BD    :=  0x00bd; // panther3.wav
Const SFX_BE    :=  0x00be; // panther4.wav
Const SFX_BF    :=  0x00bf; // panther5.wav
Const SFX_C0    :=  0x00c0; // parrot1.wav
Const SFX_C1    :=  0x00c1; // parrot2.wav
Const SFX_C2    :=  0x00c2; // parrot3.wav
Const SFX_C3    :=  0x00c3; // parrot4.wav
Const SFX_C4    :=  0x00c4; // parrot5.wav
Const SFX_C5    :=  0x00c5; // pig1.wav
Const SFX_C6    :=  0x00c6; // pig2.wav
Const SFX_C7    :=  0x00c7; // pig3.wav
Const SFX_C8    :=  0x00c8; // pig4.wav
Const SFX_C9    :=  0x00c9; // pig5.wav
Const SFX_CA    :=  0x00ca; // rabbit3.wav
Const SFX_CB    :=  0x00cb; // rabbit4.wav
Const SFX_CC    :=  0x00cc; // rabbit5.wav
Const SFX_CD    :=  0x00cd; // rat1.wav
Const SFX_CE    :=  0x00ce; // rat2.wav
Const SFX_CF    :=  0x00cf; // rat3.wav
Const SFX_D0    :=  0x00d0; // rat4.wav
Const SFX_D1    :=  0x00d1; // rat5.wav
Const SFX_D2    :=  0x00d2; // raven1.wav
Const SFX_D3    :=  0x00d3; // raven2.wav
Const SFX_D4    :=  0x00d4; // raven3.wav
Const SFX_D5    :=  0x00d5; // raven4.wav
Const SFX_D6    :=  0x00d6; // raven5.wav
Const SFX_D7    :=  0x00d7; // sheep1.wav
Const SFX_D8    :=  0x00d8; // sheep2.wav
Const SFX_D9    :=  0x00d9; // sheep3.wav
Const SFX_DA    :=  0x00da; // sheep4.wav
Const SFX_DB    :=  0x00db; // sheep5.wav
Const SFX_DC    :=  0x00dc; // snake1.wav
Const SFX_DD    :=  0x00dd; // snake2.wav
Const SFX_DE    :=  0x00de; // snake3.wav
Const SFX_DF    :=  0x00df; // snake4.wav
Const SFX_E0    :=  0x00e0; // snake5.wav
Const SFX_E1    :=  0x00e1; // walrus1.wav
Const SFX_E2    :=  0x00e2; // walrus2.wav
Const SFX_E3    :=  0x00e3; // walrus3.wav
Const SFX_E4    :=  0x00e4; // walrus4.wav
Const SFX_E5    :=  0x00e5; // walrus5.wav
Const SFX_E6    :=  0x00e6; // wolf1.wav
Const SFX_E7    :=  0x00e7; // wolf2.wav
Const SFX_E8    :=  0x00e8; // wolf3.wav
Const SFX_E9    :=  0x00e9; // wolf4.wav
Const SFX_EA    :=  0x00ea; // wolf5.wav
Const SFX_EB    :=  0x00eb; // open01a.wav
Const SFX_EC    :=  0x00ec; // open03a.wav
Const SFX_ED    :=  0x00ed; // open04b.wav
Const SFX_EE    :=  0x00ee; // open09.wav
Const SFX_EF    :=  0x00ef; // portclos.wav
Const SFX_F0    :=  0x00f0; // portcul4.wav
Const SFX_F1    :=  0x00f1; // portcul5.wav
Const SFX_F2    :=  0x00f2; // shut03a.wav
Const SFX_F3    :=  0x00f3; // shut07a.wav
Const SFX_F4    :=  0x00f4; // shut14b.wav
Const SFX_F5    :=  0x00f5; // shut17.wav
Const SFX_F6    :=  0x00f6; // sfx01.wav
Const SFX_F7    :=  0x00f7; // sfx02.wav
Const SFX_F8    :=  0x00f8; // sfx04.wav
Const SFX_F9    :=  0x00f9; // sfx05.wav
Const SFX_FA    :=  0x00fa; // sfx06.wav
Const SFX_FB    :=  0x00fb; // sfx08.wav
Const SFX_FC    :=  0x00fc; // sfx10.wav
Const SFX_FD    :=  0x00fd; // sfx11.wav
Const SFX_FE    :=  0x00fe; // sfx12.wav
Const SFX_FF    :=  0x00ff; // sfx13.wav
Const SFX_100   :=  0x0100; // sfx14_l.wav
Const SFX_101   :=  0x0101; // sfx15_l.wav
Const SFX_102   :=  0x0102; // sfx16_l.wav
Const SFX_103   :=  0x0103; // sfx17_l.wav
Const SFX_104   :=  0x0104; // sfx18_l.wav
Const SFX_105   :=  0x0105; // sfx20_l.wav
Const SFX_106   :=  0x0106; // sfx21_l.wav
Const SFX_107   :=  0x0107; // sfx22_l.wav
Const SFX_108   :=  0x0108; // airelem1.wav
Const SFX_109   :=  0x0109; // airelem2.wav
Const SFX_10A   :=  0x010a; // airelem3.wav
Const SFX_10B   :=  0x010b; // airelem4.wav
Const SFX_10C   :=  0x010c; // airelem5.wav
Const SFX_10D   :=  0x010d; // erthel1.wav
Const SFX_10E   :=  0x010e; // erthel2.wav
Const SFX_10F   :=  0x010f; // erthel3.wav
Const SFX_110   :=  0x0110; // erthel4.wav
Const SFX_111   :=  0x0111; // erthel5.wav
Const SFX_112   :=  0x0112; // firelem1.wav
Const SFX_113   :=  0x0113; // firelem2.wav
Const SFX_114   :=  0x0114; // firelem3.wav
Const SFX_115   :=  0x0115; // firelem4.wav
Const SFX_116   :=  0x0116; // firelem5.wav
Const SFX_117   :=  0x0117; // watrel1.wav
Const SFX_118   :=  0x0118; // watrel2.wav
Const SFX_119   :=  0x0119; // watrel3.wav
Const SFX_11A   :=  0x011a; // watrel4.wav
Const SFX_11B   :=  0x011b; // watrel5.wav
Const SFX_11C   :=  0x011c; // exp01.wav
Const SFX_11D   :=  0x011d; // exp04.wav
Const SFX_11E   :=  0x011e; // exp05.wav
Const SFX_11F   :=  0x011f; // exp06.wav
Const SFX_120   :=  0x0120; // bigfoot1.wav
Const SFX_121   :=  0x0121; // bigfoot2.wav
Const SFX_122   :=  0x0122; // feet01a.wav
Const SFX_123   :=  0x0123; // feet01b.wav
Const SFX_124   :=  0x0124; // feet03a.wav
Const SFX_125   :=  0x0125; // feet03b.wav
Const SFX_126   :=  0x0126; // feet05a.wav
Const SFX_127   :=  0x0127; // feet05b.wav
Const SFX_128   :=  0x0128; // feet08a.wav
Const SFX_129   :=  0x0129; // feet08b.wav
Const SFX_12A   :=  0x012a; // feet10a.wav
Const SFX_12B   :=  0x012b; // feet10b.wav
Const SFX_12C   :=  0x012c; // feet12a.wav
Const SFX_12D   :=  0x012d; // feet12b1wav
Const SFX_12E   :=  0x012e; // feet13a.wav
Const SFX_12F   :=  0x012f; // feet13b.wav
Const SFX_130   :=  0x0130; // feet14a.wav
Const SFX_131   :=  0x0131; // feet14b.wav
Const SFX_132   :=  0x0132; // feet15a.wav
Const SFX_133   :=  0x0133; // feet15b.wav
Const SFX_134   :=  0x0134; // feet15c.wav
Const SFX_135   :=  0x0135; // feet15d.wav
Const SFX_136   :=  0x0136; // hit01.wav
Const SFX_137   :=  0x0137; // hit02.wav
Const SFX_138   :=  0x0138; // hit03.wav
Const SFX_139   :=  0x0139; // hit04.wav
Const SFX_13A   :=  0x013a; // hit05.wav
Const SFX_13B   :=  0x013b; // hit06.wav
Const SFX_13C   :=  0x013c; // hit07.wav
Const SFX_13D   :=  0x013d; // hit08.wav
Const SFX_13E   :=  0x013e; // hit09.wav
Const SFX_13F   :=  0x013f; // hit10.wav
Const SFX_140   :=  0x0140; // hit11.wav
Const SFX_141   :=  0x0141; // hit12.wav
Const SFX_142   :=  0x0142; // hit13.wav
Const SFX_143   :=  0x0143; // hit14.wav
Const SFX_144   :=  0x0144; // hit15.wav
Const SFX_145   :=  0x0145; // hit16.wav
Const SFX_146   :=  0x0146; // hit17.wav
Const SFX_147   :=  0x0147; // hit18.wav
Const SFX_148   :=  0x0148; // hit19.wav
Const SFX_149   :=  0x0149; // hit20.wav
Const SFX_14A   :=  0x014a; // hit21.wav
Const SFX_14B   :=  0x014b; // hit22.wav
Const SFX_14C   :=  0x014c; // foomph01.wav
Const SFX_14D   :=  0x014d; // foomph06.wav
Const SFX_14E   :=  0x014e; // foomph07.wav
Const SFX_14F   :=  0x014f; // foomph08.wav
Const SFX_150   :=  0x0150; // foomph09.wav
Const SFX_151   :=  0x0151; // f_die_01.wav
Const SFX_152   :=  0x0152; // f_die_02.wav
Const SFX_153   :=  0x0153; // f_die_03.wav
Const SFX_154   :=  0x0154; // f_die_04.wav
Const SFX_155   :=  0x0155; // moomph01.wav
Const SFX_156   :=  0x0156; // moomph02.wav
Const SFX_157   :=  0x0157; // moomph03.wav
Const SFX_158   :=  0x0158; // moomph04.wav
Const SFX_159   :=  0x0159; // moomph05.wav
Const SFX_15A   :=  0x015a; // moomph13.wav
Const SFX_15B   :=  0x015b; // m_die_01.wav
Const SFX_15C   :=  0x015c; // m_die_02.wav
Const SFX_15D   :=  0x015d; // m_die_03.wav
Const SFX_15E   :=  0x015e; // m_die_04.wav
Const SFX_15F   :=  0x015f; // fyrbal01.wav
Const SFX_160   :=  0x0160; // fyrbal02.wav
Const SFX_161   :=  0x0161; // fyrbal03.wav
Const SFX_162   :=  0x0162; // corpsr1.wav
Const SFX_163   :=  0x0163; // corpsr3.wav
Const SFX_164   :=  0x0164; // corpsr4.wav
Const SFX_165   :=  0x0165; // corpsr5.wav
Const SFX_166   :=  0x0166; // daemon1.wav
Const SFX_167   :=  0x0167; // daemon2.wav
Const SFX_168   :=  0x0168; // daemon3.wav
Const SFX_169   :=  0x0169; // daemon4.wav
Const SFX_16A   :=  0x016a; // daemon5.wav
Const SFX_16B   :=  0x016b; // dragon1.wav
Const SFX_16C   :=  0x016c; // dragon2.wav
Const SFX_16D   :=  0x016d; // dragon3.wav
Const SFX_16E   :=  0x016e; // dragon4.wav
Const SFX_16F   :=  0x016f; // dragon5.wav
Const SFX_170   :=  0x0170; // ettin1.wav
Const SFX_171   :=  0x0171; // ettin2.wav
Const SFX_172   :=  0x0172; // ettin3.wav
Const SFX_173   :=  0x0173; // ettin4.wav
Const SFX_174   :=  0x0174; // ettin5.wav
Const SFX_175   :=  0x0175; // gargyl1.wav
Const SFX_176   :=  0x0176; // gargyl2.wav
Const SFX_177   :=  0x0177; // gargyl3.wav
Const SFX_178   :=  0x0178; // gargyl4.wav
Const SFX_179   :=  0x0179; // gargyl5.wav
Const SFX_17A   :=  0x017a; // gazer1.wav
Const SFX_17B   :=  0x017b; // gazer2.wav
Const SFX_17C   :=  0x017c; // gazer3.wav
Const SFX_17D   :=  0x017d; // gazer4.wav
Const SFX_17E   :=  0x017e; // gazer5.wav
Const SFX_17F   :=  0x017f; // ghost1.wav
Const SFX_180   :=  0x0180; // ghost2.wav
Const SFX_181   :=  0x0181; // ghost3.wav
Const SFX_182   :=  0x0182; // ghost4.wav
Const SFX_183   :=  0x0183; // ghost5.wav
Const SFX_184   :=  0x0184; // gispidr1.wav
Const SFX_185   :=  0x0185; // gispidr2.wav
Const SFX_186   :=  0x0186; // gispidr3.wav
Const SFX_187   :=  0x0187; // gispidr4.wav
Const SFX_188   :=  0x0188; // gispidr5.wav
Const SFX_189   :=  0x0189; // grat1.wav
Const SFX_18A   :=  0x018a; // grat2.wav
Const SFX_18B   :=  0x018b; // grat3.wav
Const SFX_18C   :=  0x018c; // grat4.wav
Const SFX_18D   :=  0x018d; // grat5.wav
Const SFX_18E   :=  0x018e; // gscorp1.wav
Const SFX_18F   :=  0x018f; // gscorp2.wav
Const SFX_190   :=  0x0190; // gscorp3.wav
Const SFX_191   :=  0x0191; // gscorp4.wav
Const SFX_192   :=  0x0192; // gscorp5.wav
Const SFX_193   :=  0x0193; // harpy1.wav
Const SFX_194   :=  0x0194; // harpy2.wav
Const SFX_195   :=  0x0195; // harpy3.wav
Const SFX_196   :=  0x0196; // harpy4.wav
Const SFX_197   :=  0x0197; // harpy5.wav
Const SFX_198   :=  0x0198; // headls1.wav
Const SFX_199   :=  0x0199; // headls2.wav
Const SFX_19A   :=  0x019a; // headls3.wav
Const SFX_19B   :=  0x019b; // headls4.wav
Const SFX_19C   :=  0x019c; // headls5.wav
Const SFX_19D   :=  0x019d; // liche1.wav
Const SFX_19E   :=  0x019e; // liche2.wav
Const SFX_19F   :=  0x019f; // liche3.wav
Const SFX_1A0   :=  0x01a0; // liche4.wav
Const SFX_1A1   :=  0x01a1; // liche5.wav
Const SFX_1A2   :=  0x01a2; // lzrdmn1.wav
Const SFX_1A3   :=  0x01a3; // lzrdmn2.wav
Const SFX_1A4   :=  0x01a4; // lzrdmn3.wav
Const SFX_1A5   :=  0x01a5; // lzrdmn4.wav
Const SFX_1A6   :=  0x01a6; // lzrdmn5.wav
Const SFX_1A7   :=  0x01a7; // mngbat1.wav
Const SFX_1A8   :=  0x01a8; // mngbat2.wav
Const SFX_1A9   :=  0x01a9; // mngbat3.wav
Const SFX_1AA   :=  0x01aa; // mngbat4.wav
Const SFX_1AB   :=  0x01ab; // mngbat5.wav
Const SFX_1AC   :=  0x01ac; // ogre1.wav
Const SFX_1AD   :=  0x01ad; // ogre2.wav
Const SFX_1AE   :=  0x01ae; // ogre3.wav
Const SFX_1AF   :=  0x01af; // ogre4.wav
Const SFX_1B0   :=  0x01b0; // ogre5.wav
Const SFX_1B1   :=  0x01b1; // orc1.wav
Const SFX_1B2   :=  0x01b2; // orc2.wav
Const SFX_1B3   :=  0x01b3; // orc3.wav
Const SFX_1B4   :=  0x01b4; // orc4.wav
Const SFX_1B5   :=  0x01b5; // orc5.wav
Const SFX_1B6   :=  0x01b6; // ratman1.wav
Const SFX_1B7   :=  0x01b7; // ratman2.wav
Const SFX_1B8   :=  0x01b8; // ratman3.wav
Const SFX_1B9   :=  0x01b9; // ratman4.wav
Const SFX_1BA   :=  0x01ba; // ratman5.wav
Const SFX_1BB   :=  0x01bb; // reaper1.wav
Const SFX_1BC   :=  0x01bc; // reaper2.wav
Const SFX_1BD   :=  0x01bd; // reaper3.wav
Const SFX_1BE   :=  0x01be; // reaper4.wav
Const SFX_1BF   :=  0x01bf; // reaper5.wav
Const SFX_1C0   :=  0x01c0; // seaserp1.wav
Const SFX_1C1   :=  0x01c1; // seaserp3.wav
Const SFX_1C2   :=  0x01c2; // seaserp4.wav
Const SFX_1C3   :=  0x01c3; // seaserp5.wav
Const SFX_1C4   :=  0x01c4; // skletn1.wav
Const SFX_1C5   :=  0x01c5; // skletn2.wav
Const SFX_1C6   :=  0x01c6; // skletn3.wav
Const SFX_1C7   :=  0x01c7; // skletn4.wav
Const SFX_1C8   :=  0x01c8; // skletn5.wav
Const SFX_1C9   :=  0x01c9; // slime1.wav
Const SFX_1CA   :=  0x01ca; // slime2.wav
Const SFX_1CB   :=  0x01cb; // slime3.wav
Const SFX_1CC   :=  0x01cc; // slime4.wav
Const SFX_1CD   :=  0x01cd; // slime5.wav
Const SFX_1CE   :=  0x01ce; // troll1.wav
Const SFX_1CF   :=  0x01cf; // troll2.wav
Const SFX_1D0   :=  0x01d0; // troll3.wav
Const SFX_1D1   :=  0x01d1; // troll4.wav
Const SFX_1D2   :=  0x01d2; // troll5.wav
Const SFX_1D3   :=  0x01d3; // wisp1.wav
Const SFX_1D4   :=  0x01d4; // wisp2.wav
Const SFX_1D5   :=  0x01d5; // wisp3.wav
Const SFX_1D6   :=  0x01d6; // wisp4.wav
Const SFX_1D7   :=  0x01d7; // wisp5.wav
Const SFX_1D8   :=  0x01d8; // zombie1.wav
Const SFX_1D9   :=  0x01d9; // zombie2.wav
Const SFX_1DA   :=  0x01da; // zombie3.wav
Const SFX_1DB   :=  0x01db; // zombie4.wav
Const SFX_1DC   :=  0x01dc; // zombie5.wav
Const SFX_1DD   :=  0x01dd; // elec_l.wav
Const SFX_1DE   :=  0x01de; // fire_l.wav
Const SFX_1DF   :=  0x01df; // hiss_l.wav
Const SFX_1E0   :=  0x01e0; // clumsy.wav
Const SFX_1E1   :=  0x01e1; // cure.wav
Const SFX_1E2   :=  0x01e2; // curse.wav
Const SFX_1E3   :=  0x01e3; // c_food.wav
Const SFX_1E4   :=  0x01e4; // c_light.wav
Const SFX_1E5   :=  0x01e5; // feblmind.wav
Const SFX_1E6   :=  0x01e6; // m_arrow.wav
Const SFX_1E7   :=  0x01e7; // weaken.wav
Const SFX_1E8   :=  0x01e8; // agility.wav
Const SFX_1E9   :=  0x01e9; // archcure.wav
Const SFX_1EA   :=  0x01ea; // birdseye.wav
Const SFX_1EB   :=  0x01eb; // bless.wav
Const SFX_1EC   :=  0x01ec; // cunning.wav
Const SFX_1ED   :=  0x01ed; // identify.wav
Const SFX_1EE   :=  0x01ee; // protect.wav
Const SFX_1EF   :=  0x01ef; // strength.wav
Const SFX_1F0   :=  0x01f0; // trap.wav
Const SFX_1F1   :=  0x01f1; // untrap.wav
Const SFX_1F2   :=  0x01f2; // harm.wav
Const SFX_1F3   :=  0x01f3; // heal.wav
Const SFX_1F4   :=  0x01f4; // magic_fyrbal01.
Const SFX_1F5   :=  0x01f5; // m_lock.wav
Const SFX_1F6   :=  0x01f6; // teleknes.wav
Const SFX_1F7   :=  0x01f7; // walstone.wav
Const SFX_1F8   :=  0x01f8; // aprotect.wav
Const SFX_1F9   :=  0x01f9; // manadran.wav
Const SFX_1FA   :=  0x01fa; // manavamp.wav
Const SFX_1FB   :=  0x01fb; // mark.wav
Const SFX_1FC   :=  0x01fc; // masscurs.wav
Const SFX_1FD   :=  0x01fd; // recall.wav
Const SFX_1FE   :=  0x01fe; // reveal.wav
Const SFX_1FF   :=  0x01ff; // teleport.wav
Const SFX_200   :=  0x0200; // unlock.wav
Const SFX_201   :=  0x0201; // bladsprt.wav
Const SFX_202   :=  0x0202; // dispfild.wav
Const SFX_203   :=  0x0203; // gratheal.wav
Const SFX_204   :=  0x0204; // invisibl.wav
Const SFX_205   :=  0x0205; // paralyze.wav
Const SFX_206   :=  0x0206; // poison.wav
Const SFX_207   :=  0x0207; // thundr01.wav
Const SFX_208   :=  0x0208; // explode.wav
Const SFX_209   :=  0x0209; // flamstrk.wav
Const SFX_20A   :=  0x020a; // massdisp.wav
Const SFX_20B   :=  0x020b; // nrgybolt.wav
Const SFX_20C   :=  0x020c; // parafild.wav
Const SFX_20D   :=  0x020d; // posnfild.wav
Const SFX_20E   :=  0x020e; // earthqke.wav
Const SFX_20F   :=  0x020f; // gatetrvl.wav
Const SFX_210   :=  0x0210; // morph.wav
Const SFX_211   :=  0x0211; // nergyfld.wav
Const SFX_212   :=  0x0212; // nrgywind.wav
Const SFX_213   :=  0x0213; // dethvrtx.wav
Const SFX_214   :=  0x0214; // mindblst.wav
Const SFX_215   :=  0x0215; // resurect.wav
Const SFX_216   :=  0x0216; // sumncrtr.wav
Const SFX_217   :=  0x0217; // sumndemn.wav
Const SFX_218   :=  0x0218; // sumnelem.wav
Const SFX_219   :=  0x0219; // blade01.wav
Const SFX_21A   :=  0x021a; // blade02.wav
Const SFX_21B   :=  0x021b; // blade03.wav
Const SFX_21C   :=  0x021c; // blade04.wav
Const SFX_21D   :=  0x021d; // blade05.wav
Const SFX_21E   :=  0x021e; // boulder1.wav
Const SFX_21F   :=  0x021f; // boulder2.wav
Const SFX_220   :=  0x0220; // crumble1.wav
Const SFX_221   :=  0x0221; // crumble2.wav
Const SFX_222   :=  0x0222; // crumble3.wav
Const SFX_223   :=  0x0223; // crumble4.wav
Const SFX_224   :=  0x0224; // dart01.wav
Const SFX_225   :=  0x0225; // dart02.wav
Const SFX_226   :=  0x0226; // flame01.wav
Const SFX_227   :=  0x0227; // flame03.wav
Const SFX_228   :=  0x0228; // flame05.wav
Const SFX_229   :=  0x0229; // mushrm01.wav
Const SFX_22A   :=  0x022a; // mushrm02.wav
Const SFX_22B   :=  0x022b; // mushrm03.wav
Const SFX_22C   :=  0x022c; // spike01.wav
Const SFX_22D   :=  0x022d; // spike02.wav
Const SFX_22E   :=  0x022e; // spike03.wav
Const SFX_22F   :=  0x022f; // spike04.wav
Const SFX_230   :=  0x0230; // vapor01.wav
Const SFX_231   :=  0x0231; // vapor02.wav
Const SFX_232   :=  0x0232; // vapor03.wav
Const SFX_233   :=  0x0233; // axe01.wav
Const SFX_234   :=  0x0234; // blunt01.wav
Const SFX_235   :=  0x0235; // crossbow.wav
Const SFX_236   :=  0x0236; // gilotin1.wav
Const SFX_237   :=  0x0237; // hvyswrd1.wav
Const SFX_238   :=  0x0238; // hvyswrd4.wav
Const SFX_239   :=  0x0239; // swish01.wav
Const SFX_23A   :=  0x023a; // swish02.wav
Const SFX_23B   :=  0x023b; // swish03.wav
Const SFX_23C   :=  0x023c; // sword1.wav
Const SFX_23D   :=  0x023d; // sword7.wav
Const SFX_23E   :=  0x023e; // carpntry.wav
Const SFX_23F   :=  0x023f; // dye.wav
Const SFX_240   :=  0x0240; // fshsplsh.wav
Const SFX_241   :=  0x0241; // liquid.wav
Const SFX_242   :=  0x0242; // lockpick.wav
Const SFX_243   :=  0x0243; // mortar.wav
Const SFX_244   :=  0x0244; // mystery1.wav
Const SFX_245   :=  0x0245; // mystery2.wav
Const SFX_246   :=  0x0246; // mystery3.wav
Const SFX_247   :=  0x0247; // poison.wav
Const SFX_248   :=  0x0248; // powder.wav
Const SFX_249   :=  0x0249; // scissors.wav
Const SFX_24A   :=  0x024a; // scribe.wav
Const SFX_24B   :=  0x024b; // spirtspk.wav
Const SFX_24C   :=  0x024c; // z_horseft2.wav
Const SFX_24D   :=  0x024d; // z_horseft1.wav
Const SFX_24E   :=  0x024e; // Tera.warrior 1
Const SFX_2D7   :=  0x02d7; // drink5.wav
Const SFX_24F   :=  0x02ef; // Tera.warrior 2
Const SFX_2A2   :=  0x02a2; // ???
Const SFX_312   :=  0x0312; // cough (female)
Const SFX_32E   :=  0x032e; // puke (female)
Const SFX_347   :=  0x0347; // fire01.wav
Const SFX_348   :=  0x0348; // fire02.wav
Const SFX_349   :=  0x0349; // fire03.wav
Const SFX_34A   :=  0x034a; // fire04.wav
Const SFX_34B   :=  0x034b; // fire05.wav
Const SFX_421   :=  0x0421; // cough (male)
Const SFX_440   :=  0x0440; // puke (male)
Const SFX_56D   :=  0x056d; // fetidessence_sfxwashit.wav
Const SFX_5AF   :=  0x05af; // crafting_effect7.wav
Const SFX_5B4   :=  0x05b4; // quest_abandon.wav
Const SFX_5B5   :=  0x05b5; // quest_accept.wav
Const SFX_5B6   :=  0x05b6; // quest_complete.wav
Const SFX_5B7   :=  0x05b7; // quest_update.wav
Const SFX_5B8   :=  0x05b8; // satyrpipe_use_poorly.wav
Const SFX_5B9   :=  0x05b9; // satyrpipe_use_well.wav
Const SFX_5C9   :=  0x05c9; // etherealvoyage.wav
Const SFX_5CB   :=  0x05cb; // immolatingweapon.wav

// Fill in the rest of the T2A sounds here.
// * music *
Const SFX_MUSIC_LUTE_GOOD           :=  SFX_4D;
Const SFX_MUSIC_LUTE_FAIL           :=  SFX_4E;
Const SFX_MUSIC_DRUM_GOOD           :=  SFX_39;
Const SFX_MUSIC_DRUM_FAIL           :=  SFX_3A;
Const SFX_MUSIC_HARP_LARGE_GOOD     :=  SFX_44;
Const SFX_MUSIC_HARP_LARGE_FAIL     :=  SFX_45;
Const SFX_MUSIC_HARP_LAP_GOOD       :=  SFX_46;
Const SFX_MUSIC_HARP_LAP_FAIL       :=  SFX_47;
Const SFX_MUSIC_TAMBORINE_GOOD      :=  SFX_53;
Const SFX_MUSIC_TAMBORINE_FAIL      :=  SFX_54;

// Mai-Musik
Const SFX_MUSIC_DRUM_DRAMA          :=  0x2E9;
Const SFX_MUSIC_DRUM_PEACE          :=  0x2EA;
Const SFX_MUSIC_DRUM_BATTLE         :=  0x2E8;
Const SFX_MUSIC_HARP_DRAMA          :=  0x393;
Const SFX_MUSIC_HARP_PEACE          :=  0x394;
Const SFX_MUSIC_HARP_BATTLE         :=  0x392;
Const SFX_MUSIC_LUTE_DRAMA          :=  0x40C;
Const SFX_MUSIC_LUTE_PEACE          :=  0x419;
Const SFX_MUSIC_LUTE_BATTLE         :=  0x404;
Const SFX_MUSIC_TAMB_DRAMA          :=  0x4B7;
Const SFX_MUSIC_TAMB_PEACE          :=  0x4B8;
Const SFX_MUSIC_TAMB_BATTLE         :=  0x4B6;
Const SFX_MUSIC_FLUT_PEACE          :=  0x359;
Const SFX_MUSIC_FLUT_BATTLE         :=  0x5D7;

// * spells & magic sounds *
Const SFX_SPELL_CLUMSY              :=  SFX_1E0;
Const SFX_SPELL_CREATE_FOOD         :=  SFX_1E3;
Const SFX_SPELL_FEEBLEMIND          :=  SFX_1E5;
Const SFX_SPELL_HEAL                :=  SFX_1F3;
Const SFX_SPELL_MAGIC_ARROW         :=  SFX_1E6;
Const SFX_SPELL_NIGHT_SIGHT         :=  SFX_1E4;
Const SFX_SPELL_REACTIVE_ARMOR      :=  SFX_1F3;
Const SFX_SPELL_WEAKEN              :=  SFX_1E7;
Const SFX_SPELL_AGILITY             :=  SFX_1E8;
Const SFX_SPELL_CUNNING             :=  SFX_1EC;
Const SFX_SPELL_CURE                :=  SFX_1E1;
Const SFX_SPELL_HARM                :=  SFX_1F2;
Const SFX_SPELL_MAGIC_TRAP          :=  SFX_1F0;
Const SFX_SPELL_MAGIC_UNTRAP        :=  SFX_1F1;
Const SFX_SPELL_PROTECTION          :=  SFX_1EE;
Const SFX_SPELL_STRENGTH            :=  SFX_1EF;
Const SFX_SPELL_BLESS               :=  SFX_1EB;
Const SFX_SPELL_FIREBALL            :=  SFX_160;
Const SFX_SPELL_MAGIC_LOCK          :=  SFX_1F5;
Const SFX_SPELL_TELEKINESIS         :=  SFX_1F6;
Const SFX_SPELL_TELEPORT            :=  SFX_1FF;
Const SFX_SPELL_POISON              :=  SFX_247;
Const SFX_SPELL_UNLOCK              :=  SFX_200;
Const SFX_SPELL_WALL_OF_STONE       :=  SFX_1F7;
Const SFX_SPELL_ARCH_CURE           :=  SFX_1E9;
Const SFX_SPELL_ARCH_PROTECTION     :=  SFX_1F8;
Const SFX_SPELL_CURSE               :=  SFX_1E2;
Const SFX_SPELL_FIRE_FIELD          :=  SFX_226;
Const SFX_SPELL_GREATER_HEAL        :=  SFX_203;
Const SFX_SPELL_LIGHTNING           :=  SFX_2A;
Const SFX_SPELL_MANA_DRAIN          :=  SFX_1F9;
Const SFX_SPELL_RECALL              :=  SFX_1FD;
Const SFX_SPELL_BLADE_SPIRITS       :=  SFX_213;
Const SFX_SPELL_BLADE_SUMMON        :=  SFX_2A2;
Const SFX_SPELL_ENERGY_FIELD        :=  SFX_211; // used to Say dispel field?
Const SFX_SPELL_INCOGNITO           :=  SFX_1ED;
Const SFX_SPELL_MAGIC_REFLECTION    :=  SFX_1E8;
Const SFX_SPELL_MIND_BLAST          :=  SFX_214;
Const SFX_SPELL_PARALYZE            :=  SFX_205;
Const SFX_SPELL_POISON_FIELD        :=  SFX_20D; // was 227 (??)
Const SFX_SPELL_SUMMON_CREATURE     :=  SFX_216;
Const SFX_SPELL_DISPEL              :=  SFX_202;
Const SFX_SPELL_ENERGY_BOLT         :=  SFX_20B;
Const SFX_SPELL_EXPLOSION           :=  SFX_208;
Const SFX_SPELL_INVISIBILITY        :=  SFX_204;
Const SFX_SPELL_MARK                :=  SFX_1FB;
Const SFX_SPELL_MASS_CURSE          :=  SFX_1FC;
Const SFX_SPELL_PARALYZE_FIELD      :=  SFX_212;
Const SFX_SPELL_REVEAL              :=  SFX_1FE;
Const SFX_SPELL_CHAIN_LIGHTNING     :=  SFX_30;
Const SFX_SPELL_PARA_FIELD          :=  SFX_20C;
Const SFX_SPELL_FLAME_STRIKE        :=  SFX_209;
Const SFX_SPELL_GATE_TRAVEL         :=  SFX_20F;
Const SFX_SPELL_MANA_VAMPIRE        :=  SFX_1FA;
Const SFX_SPELL_MASS_DISPEL         :=  SFX_20A;
Const SFX_SPELL_METEOR_SWARM        :=  SFX_161;
Const SFX_SPELL_POLYMORPH           :=  SFX_20A;
Const SFX_SPELL_EARTHQUAKE          :=  SFX_20E;
Const SFX_SPELL_ENERGY_VORTEX       :=  SFX_213;
Const SFX_SPELL_RESSURECTION        :=  SFX_215;
Const SFX_SPELL_SUMMON_ELEMENTAL    :=  SFX_218;
Const SFX_SPELL_SUMMON_DAEMON       :=  SFX_217;

// * skills *
Const SFX_SKILL_SPIRITSPEAK         :=  SFX_24B;
Const SFX_SKILL_LUMBERJACK          :=  SFX_13F;

// * items *
Const SFX_TRAP_NEEDLE               :=  SFX_225;
Const SFX_TRAP_POISON               :=  SFX_206;
Const SFX_TRAP_EXPLOSION            :=  SFX_208;
Const SFX_ANVIL                     :=  SFX_2B;
Const SFX_PICK                      :=  SFX_13B;
Const SFX_HAMMER                    :=  SFX_43;
Const SFX_GLASSBREAK4               :=  SFX_42;
Const SFX_FRUITDROP                 :=  SFX_122;
Const SFX_FRUITPICKER               :=  SFX_5CB;

Const SFX_HARP_SUCC                 :=  SFX_46;
Const SFX_LUTE_SUCC                 :=  SFX_4D;
Const SFX_DRUM_SUCC                 :=  SFX_39;
Const SFX_TAMB_SUCC                 :=  SFX_53;
Const SFX_FLUT_SUCC                 :=  SFX_5B9;
Const SFX_HARP_FAIL                 :=  SFX_47;
Const SFX_LUTE_FAIL                 :=  SFX_4E;
Const SFX_DRUM_FAIL                 :=  SFX_3A;
Const SFX_TAMB_FAIL                 :=  SFX_54;
Const SFX_FLUT_FAIL                 :=  SFX_5B8;

// * misc *
Const SFX_BELL_RING                 :=  SFX_101;
Const SFX_CHEST_FOUND               :=  SFX_4C;
Const SFX_THUNDER                   :=  SFX_2A;

// * dyndungeon *
Const SFX_DYN_BOSS_COLOR            := SFX_5C9;
Const SFX_DYN_BOSS_INVUL            := SFX_5AF;
Const SFX_DYN_BOSS_VUL              := SFX_56D;
Const SFX_DYN_WALKFIELD             := SFX_4A;

// * doors *
Const SFX_OPEN_WOODEN_DOOR          :=  SFX_EB;
Const SFX_CLOSE_WOODEN_DOOR         :=  SFX_F2;
Const SFX_OPEN_METAL_DOOR           :=  SFX_ED;
Const SFX_CLOSE_METAL_DOOR          :=  SFX_F4;
Const SFX_OPEN_TENT_DOOR            :=  SFX_5A;
Const SFX_CLOSE_TENT_DOOR           :=  SFX_5A;
Const SFX_OPEN_SLIDE_DOOR           :=  SFX_EE;
Const SFX_CLOSE_SLIDE_DOOR          :=  SFX_F5;
Const SFX_TOGGLE_PLANK              :=  SFX_F3;

// * common npc sounds *
Const SFX_DROWN                     :=  SFX_27;
Const SFX_COUGH_FEMALE              :=  SFX_312;
Const SFX_COUGH_MALE                :=  SFX_421;
Const SFX_PUKE_FEMALE               :=  SFX_32E;
Const SFX_PUKE_MALE                 :=  SFX_440;

////////////////////////////////////
//                                //
//   Human Animations & Actions   //
//                                //
////////////////////////////////////

Const ANIM_WALK_UNARMED     := 0x00; // Walk unarmed.
Const ANIM_WALK_ARMED       := 0x01; // Walk armed.
Const ANIM_RUN_UNARMED      := 0x02; // Run unarmed.
Const ANIM_RUN_ARMED        := 0x03; // Run armed.
Const ANIM_STAND            := 0x04; // Stand.
Const ANIM_FIDGET_1         := 0x05; // Twist a little (look around?).
Const ANIM_FIDGET_2         := 0x06; // Look down.
Const ANIM_STAND_1HAND      := 0x07; // Stand 1 handed attack.
Const ANIM_STAND_2HAND      := 0x08; // Stand 2 handed attack.
Const ANIM_ATTACK_1HAND_WIDE:= 0x09; // Attack 1 handed wide (sword/any).
Const ANIM_ATTACK_1HAND_JAB := 0x0a; // Attack 1 handed jab (fencing).
Const ANIM_ATTACK_1HAND_DOWN:= 0x0b; // Attack 1 handed down (mace).
Const ANIM_ATTACK_2HAND_DOWN:= 0x0c; // Attack 2 handed jab (mace).
Const ANIM_ATTACK_2HAND_WIDE:= 0x0d; // Attack 2 handed wide.
Const ANIM_ATTACK_2HAND_JAB := 0x0e; // Attack 2 handed jab (spear).
Const ANIM_WALK_WARMODE     := 0x0f; // Walk in warmode.
Const ANIM_CAST_DIR         := 0x10; // Spellcasting direction.
Const ANIM_CAST_AREA        := 0x11; // Spellcasting area.
Const ANIM_ATTACK_BOW       := 0x12; // Attack bow.
Const ANIM_ATTACK_XBOW      := 0x13; // Attack xbow.
Const ANIM_HIT              := 0x14; // Get hit.
Const ANIM_DIE_BACK         := 0x15; // Die backward.
Const ANIM_DIE_FOR          := 0x16; // Die forward.
Const ANIM_WAR_TURN         := 0x1e; // take cover by turning aside
Const ANIM_FIST_ATTACK      := 0x1f; // attack with fist
Const ANIM_BOW              := 0x20; // Bow.
Const ANIM_SALUTE           := 0x21; // Salute.
Const ANIM_EAT              := 0x22; // Eat.

Const UACTION_LOOK_AROUND   := 0x05; // [gelangweilt] Umherschauen
Const UACTION_NOD           := 0x06; // Nicken
Const UACTION_SMELT         := 0x0b; // Schmelztiegel benutzen
Const UACTION_PICK          := 0x0b; // Hacke/Picke benutzen
Const UACTION_SALUTE        := 0x21; // Salutieren

////
// Graphicaleffects (itemIDs)
///
// * general *
Const FX_SPLASH         :=  0x352d; //
Const FX_EXPLODE_1      :=  0x36cb; //
Const FX_EXPLODE_2      :=  0x36bd; //
Const FX_EXPLODE_3      :=  0x36b0; //
Const FX_FIREBALL       :=  0x36d4; //
Const FX_MAGIC_ARROW    :=  0x36e4; //
Const FX_FIRE_BOLT      :=  0x36f4; // Firesnake. (pagan?)
Const FX_EXPLODE_BALL   :=  0x36fe; // Not used.

Const FX_FLAMESTRIKE    :=  0x3709; //
Const FX_SMOKE          :=  0x3728;
Const FX_TELE_VANISH    :=  0x372a; // Teleport vanish.
Const FX_SPELL_FAIL     :=  0x3735; //
Const FX_BLESS_EFFECT   :=  0x373a; //
Const FX_CURSE_EFFECT   :=  0x374a; //
Const FX_SPARK_EFFECT   :=  0x375a; //
Const FX_HEAL_EFFECT    :=  0x376a; //
Const FX_MARK_EFFECT    :=  0x3779;
Const FX_BLUEMOONSTART  :=  0x3789; // Deathvortex?
Const FX_ENERGY_BOLT    :=  0x379f; //
Const FX_BLADES_EMERGE  :=  0x37a0; // Not used.
Const FX_GLOW           :=  0x37b9; //
Const FX_GLOW_SPIKE     :=  0x37c4; //
Const FX_DEATH_FUNNEL   :=  0x37cc; //
Const FX_BLADES         :=  0x37eb; //
Const FX_STATIC         :=  0x3818; // Not used. (pink static)
Const FX_POISON_F_EW    :=  0x3915; // Poison field.
Const FX_POISON_F_NS    :=  0x3920; // Poison field.
Const FX_ENERGY_F_EW    :=  0x3947; // Energy field.
Const FX_ENERGY_F_NS    :=  0x3956; // Energy field.
Const FX_PARA_F_EW      :=  0x3967; // Paralyze field.
Const FX_PARA_F_NS      :=  0x3979; // Paralyze field.
Const FX_FIRE_F_EW      :=  0x398c; // Fire field.
Const FX_FIRE_F_NS      :=  0x3996; // Fire field.
Const FX_SUMMON_GATE    :=  0x1AF3; //Summon Gate

////
// Layers
///
Const LAYER_NONE        :=  0x00;   // Not used?
Const LAYER_HAND1       :=  0x01;   // One handed weapon.
Const LAYER_HAND2       :=  0x02;   // Two handed weapon, shield or misc.
Const LAYER_SHOES       :=  0x03;   // Shoes.
Const LAYER_PANTS       :=  0x04;   // Pants.
Const LAYER_SHIRT       :=  0x05;   // Shirt.
Const LAYER_HELMET      :=  0x06;   // Helmet or Hat.
Const LAYER_GLOVES      :=  0x07;   // Gloves.
Const LAYER_RING        :=  0x08;   // Ring.
Const LAYER_TALISMAN    :=  0x09;   // Talisman (ML).
Const LAYER_NECK        :=  0x0a;   // Neck.
Const LAYER_HAIR        :=  0x0b;   // Hair
Const LAYER_WAIST       :=  0x0c;   // Waist (half apron).
Const LAYER_CHEST       :=  0x0d;   // Torso (inner) (chest armor).
Const LAYER_WRIST       :=  0x0e;   // Bracelet.
Const LAYER_PACK2       :=  0x0f;   // Unused (backpack, but ord. bp is 0x15).
Const LAYER_BEARD       :=  0x10;   // Facial hair.
Const LAYER_TUNIC       :=  0x11;   // Torso (middle) (tunic, sash etc.).
Const LAYER_EARS        :=  0x12;   // Earrings.
Const LAYER_ARMS        :=  0x13;   // Arms.
Const LAYER_CAPE        :=  0x14;   // Back (cloak) and Quivers (ML).
Const LAYER_PACK        :=  0x15;   // Backpack
Const LAYER_ROBE        :=  0x16;   // Torso (outer) (robe)
Const LAYER_SKIRT       :=  0x17;   // Legs (outer) (skirt/robe).
Const LAYER_LEGS        :=  0x18;   // Legs (inner) (leg armor).
Const LAYER_MOUNT       :=  0x19;   // Mount (horse, ostard etc.).
Const LAYER_VENDORSTOCK :=  0x1a;   // This vendor will sell and restock.
Const LAYER_VENDOREXTRA :=  0x1b;   // This vendor will resell to players but not restock.
Const LAYER_VENDORBUY   :=  0x1c;   // This vendor can buy from players but does not stock.
Const LAYER_BANKBOX     :=  0x1d;   // Contents of bankbox
Const LAYER_TRADE       :=  0x1e;   // Can be multiple of these, do not use directly.

////
// Fonts
///
Const FONT_MAX          :=  0x00;   // Lowest font.
Const FONT_MIN          :=  0x09;   // Highest font.

Const FONT_BOLD         :=  0x00;   // Bold text.
Const FONT_SHADOW       :=  0x01;   // Text with shadow.
Const FONT_BOLD_SHAD    :=  0x02;   // Bold text with shadow.
Const FONT_NORMAL       :=  0x03;   // Normal (default).
Const FONT_GOTHIC       :=  0x04;   // Gothic.
Const FONT_ITALIC       :=  0x05;   // Italic
Const FONT_SMALL_DARK   :=  0x06;   // Small dark letters.
Const FONT_COLOR        :=  0x07;   // Colorful font.
Const FONT_RUNE         :=  0x08;   // Rune font (only capitals).
Const FONT_SMALL_LIGHT  :=  0x09;   // Small light letters.

////
// Creature IDs ( hardcoded graphic IDs )
///
Const CID_ZERO              :=  0x000;  // Not used.
Const CID_OGRE              :=  0x001;  // Ogre.
Const CID_ETTIN             :=  0x002;  // Ettin.
Const CID_ZOMBIE            :=  0x003;  // Zombie.
Const CID_GARGOYLE          :=  0x004;  // Gargoyle.
Const CID_EAGLE             :=  0x005;  // Eagle.
Const CID_BIRD              :=  0x006;  // Bird.
Const CID_ORC_LORD          :=  0x007;  // Orc Lord.
Const CID_CORPSER           :=  0x008;  // Corpser.
Const CID_DAEMON            :=  0x009;  // Daemon.
Const CID_DAEMON_SWORD      :=  0x00a;  // Daemon with sword.
Const CID_DRAGON_GREY       :=  0x00c;  // Dragon grey.
Const CID_AIR_ELEMENTAL     :=  0x00d;  // Air Elemental.
Const CID_EARTH_ELEMENTAL   :=  0x00e;  // Earth Elemental.
Const CID_FIRE_ELEMENTAL    :=  0x00f;  // Fire Elemental.
Const CID_WATER_ELEMENTAL   :=  0x010;  // Water elemental.
Const CID_ORC               :=  0x011;  // Orc.
Const CID_ETTIN_AXE         :=  0x012;  // Ettin with axe.
Const CID_GIANT_SNAKE       :=  0x015;  // Giant

Const CID_HUMAN_MALE        :=  0x190;
Const CID_HUMAN_FEMALE      :=  0x191;
Const CID_ELF_MALE          :=  0x25D;
Const CID_ELF_FEMALE        :=  0x25E;

////
// Directions
///
Const DIR_N             :=  0x00;   // North.
Const DIR_NE            :=  0x01;   // Northeast.
Const DIR_E             :=  0x02;   // East.
Const DIR_SE            :=  0x03;   // Southeast.
Const DIR_S             :=  0x04;   // South.
Const DIR_SW            :=  0x05;   // Southwest.
Const DIR_W             :=  0x06;   // West.
Const DIR_NW            :=  0x07;   // Northwest.

// * Colors *
// This is not suposed to be a "complete" colorchart, but a
// quick way to access normal colors.
Const COLOR_DEFAULT         := 0x0000;
Const COLOR_BLACK           := 0x0001;
Const COLOR_INDIGO_DARK     := 0x0007;
Const COLOR_INDIGO          := 0x0008;
Const COLOR_INDIGO_LIGHT    := 0x0009;

Const COLOR_VIOLET_DARK     := 0x000c;
Const COLOR_VIOLET          := 0x000d;
Const COLOR_VIOLET_LIGHT    := 0x000e;
Const COLOR_MAGENTA_DARK    := 0x0011;
Const COLOR_MAGENTA         := 0x0012;
Const COLOR_MAGENTA_LIGHT   := 0x0013;
Const COLOR_RED_DARK        := 0x0020;
Const COLOR_RED             := 0x0022;
Const COLOR_RED_LIGHT       := 0x0023;
Const COLOR_ORANGE_DARK     := 0x002a;
Const COLOR_ORANGE          := 0x002b;
Const COLOR_ORANGE_LIGHT    := 0x002c;
Const COLOR_YELLOW_DARK     := 0x0034;
Const COLOR_YELLOW          := 0x0035;
Const COLOR_YELLOW_LIGHT    := 0x0036;
Const COLOR_GREEN_DARK      := 0x003e;
Const COLOR_GREEN           := 0x003f;
Const COLOR_GREEN_LIGHT     := 0x0040;
Const COLOR_CYAN_DARK       := 0x0057;
Const COLOR_CYAN            := 0x0058;
Const COLOR_CYAN_LIGHT      := 0x0059;
Const COLOR_BLUE_DARK       := 0x0061;
Const COLOR_BLUE            := 0x0062;
Const COLOR_BLUE_LIGHT      := 0x0063;
Const COLOR_GRAY_DARK       := 0x0386;
Const COLOR_GRAY            := 0x0387;
Const COLOR_GRAY_LIGHT      := 0x0388;
Const COLOR_BLACKMETAL      := 0x044e;
Const COLOR_GOLDMETAL       := 0x046e;
Const COLOR_BSHIMMER        := 0x0480;	// T2A Only : Waterelemental color.
Const COLOR_WHITE           := 0x0481;	// T2A Only : True white.
Const COLOR_STONE           := 0x0482;	// T2A Only : Rock ( stoneharpy?)
Const COLOR_SPECTRAL        := 0x4631;  // Spectral
Const COLOR_TRANSLUCENT     := 0x7cf8;  // Translucent

Const COLOR_GOLD            := 0x0499;  // via InsideUO getestet

/////////////////
// Text-Colors //
/////////////////

Const FONTCOLOR_RED:=38;
Const FONTCOLOR_GREEN:=467;//68;
Const FONTCOLOR_BLUE:=93;//90;
Const FONTCOLOR_MAUVE:=30;
Const FONTCOLOR_LAVENDER:=20;
Const FONTCOLOR_BLACK:=1;
Const FONTCOLOR_BROWN:=46;
Const FONTCOLOR_YELLOW:=53;
Const FONTCOLOR_REDBROWN:=238;
Const FONTCOLOR_PURPLE:=1289;
Const FONTCOLOR_TURQUOISE:=183;
Const FONTCOLOR_GREEN_DARK:=1367;
Const FONTCOLOR_WHITE:=1152;

Const FONTCOLOR_GUILD_OWN:=2211;//2117;
Const FONTCOLOR_GUILD_ALLY:=3;//2211;
Const FONTCOLOR_GUILD_ALL:=2117;//3;

////////////////////////////
// selbstdefiniertes Zeugs
////////////////////////////

Const UOBJECT_DOORS_START:=0x0675;
Const UOBJECT_DOORS_END:=0x06f4;

////////////////////////////////
// htmlgumpcolor Color-Werte
////////////////////////////////

Const HTML_BLACK         :="#000000";
Const HTML_DARKGRAY      :="#404040";
Const HTML_GRAY          :="#808080";
Const HTML_LIGHTGRAY     :="#C0C0C0";
Const HTML_DARKRED       :="#800000";
Const HTML_RED           :="#FF0000";
Const HTML_DARKGREEN     :="#008000";
Const HTML_GREEN         :="#00FF00";
Const HTML_DARKYELLOW    :="#808000";
Const HTML_YELLOW        :="#FFFF00";
Const HTML_DARKBLUE      :="#000080";
Const HTML_BLUE          :="#0000FF";
Const HTML_DARKPURPLE    :="#800080";
Const HTML_PURPLE        :="#FF00FF";
Const HTML_DARKTURQUOISE :="#008080";
Const HTML_TURQUOISE     :="#00FFFF";
Const HTML_WHITE         :="#FFFFFF";

////////////////////////////////
// xmfhtmlgumpcolor Color-Werte
////////////////////////////////

Const XMFCOLOR_BLACK          :=0;
Const XMFCOLOR_WHITE          :=32767;
Const XMFCOLOR_BLUE1          :=15;
Const XMFCOLOR_BLUE2          :=5295;
Const XMFCOLOR_RED1           :=15360;
Const XMFCOLOR_RED2           :=20480;
Const XMFCOLOR_RED3           :=25600;
Const XMFCOLOR_RED4           :=30720;
Const XMFCOLOR_RED5           :=32000;
Const XMFCOLOR_RED6           :=32272;
Const XMFCOLOR_RED7           :=64512;
Const XMFCOLOR_GRAYRED        :=16715;
Const XMFCOLOR_PURPLE1        :=16927;
Const XMFCOLOR_PURPLE2        :=19777215;
Const XMFCOLOR_GRAY1          :=15855;
Const XMFCOLOR_GRAY2          :=16912;
Const XMFCOLOR_OCHER1         :=23341;
Const XMFCOLOR_OCHER2         :=49671;
Const XMFCOLOR_OCHER3         :=90000;
Const XMFCOLOR_GREEN1         :=33280;
Const XMFCOLOR_GREEN2         :=33760;
Const XMFCOLOR_YELLOW         :=32512;
Const XMFCOLOR_TURQUOISE      :=1023;
Const XMFCOLOR_TURQUOISEGREEN :=10000;

