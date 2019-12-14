///////////////////////////////////////////////////////////////////////////
// TELEPORTERS.inc - Places teleporters into the world.
//
// This file is to be included into start.src and the function
// CreateTeleporters() called from there.
//
// Author: Ignatius 1999/05/27
// Thanks to Raxxla for some tweaks/fixes!

// TODO:
// - walk thru all these and check em out.

///////////////////////////////////////////////////////////////////////////
// System teleporters
//
// Teleporter locations were mainly taken from the UOX project.
// Credit is given where credit is due!
//
// Format:
// { startX, startY, startZ, startRealm, toX, toY, ToZ, ToRealm [, spc] },
//
// if startZ is -255 it should affect *all* Z coordinates on that spot,
// but for now the walkonscript gets the lowest walkable location.
//
// spc is an optional struct of special options,
// see GATE_SPC_* in pkg/opt/teleporters/common.inc

///////////////
// Konstanten
///////////////
Const UOBJ_SYSTELEPORTER := 0x6200;
Const DEBUG              := 0;

include ":teleporters:common"; // fuer GATE_SPC_* und PROP_GATE_SPECIAL
include "include/server";

///////////////////////////
// Teleporter-Defintionen
///////////////////////////

Function GetTeleporters(byref teleporters)
  teleporters := {
    // Moonglow <-> Papua {{{
    {4300,  968, -255, REALM_BRITANNIA, 4442, 1122,    4, REALM_BRITANNIA}, // moonglow_b_x[]
    {4496, 1475, -255, REALM_BRITANNIA, 4442, 1122,    4, REALM_BRITANNIA}, // moonglow_b_x[]
    {4540,  898, -255, REALM_BRITANNIA, 4442, 1122,    4, REALM_BRITANNIA}, // moonglow_b_x[]
    {4663, 1134, -255, REALM_BRITANNIA, 4442, 1122,    4, REALM_BRITANNIA}, // moonglow_b_x[]
    {4449, 1115, -255, REALM_BRITANNIA, 4671, 1135,   10, REALM_BRITANNIA}, // moonglow_1_x[]
    {4443, 1137, -255, REALM_BRITANNIA, 4487, 1475,    4, REALM_BRITANNIA}, // moonglow_2_x[]
    {4436, 1107, -255, REALM_BRITANNIA, 4300,  992,    5, REALM_BRITANNIA}, // moonglow_3_x[]
    {4449, 1107, -255, REALM_BRITANNIA, 4539,  890,   27, REALM_BRITANNIA}, // moonglow_4_y[]
    {4545,  851, -255, REALM_BRITANNIA, 5735, 3196,   11, REALM_BRITANNIA}, // moonglow_to_papua[]
    {5736, 3196,   11, REALM_BRITANNIA, 4545,  852,   31, REALM_BRITANNIA}, // papua_to_moonglow[]
    // }}}

    // Hoehleneingang in Lostlands
    // 5126, 3143, 99

    // Newbie-Level/T2A {{{
    // Steinkreis <-> Insel mit Sandsteinplatz
    {5906, 4069, -255, REALM_BRITANNIA, 2494, 3577,    5, REALM_BRITANNIA},
    {2494, 3576, -255, REALM_BRITANNIA, 5907, 4069,   26, REALM_BRITANNIA},

    // Grab T2A <-> Hoehle
    {5658,  423,    8, REALM_BRITANNIA, 5697, 3659,    3, REALM_BRITANNIA},
    {5697, 3660,   -5, REALM_BRITANNIA, 5658,  424,    0, REALM_BRITANNIA},

    {6024, 1344,  -26, REALM_BRITANNIA, 5138, 3664,   31, REALM_BRITANNIA}, // newbie_lvl_to_W.t2a[]
    {6024, 1345,  -25, REALM_BRITANNIA, 5138, 3665,   32, REALM_BRITANNIA}, // newbie_lvl_to_W.t2a[]
    {6024, 1346,  -25, REALM_BRITANNIA, 5138, 3666,   32, REALM_BRITANNIA}, // newbie_lvl_to_W.t2a[]
    {5137, 3664,   27, REALM_BRITANNIA, 6023, 1344,  -25, REALM_BRITANNIA}, // W.t2a_to_newbie_lvl[]
    {5137, 3665,   31, REALM_BRITANNIA, 6023, 1345,  -25, REALM_BRITANNIA}, // W.t2a_to_newbie_lvl[]
    {5137, 3666,   31, REALM_BRITANNIA, 6023, 1346,  -24, REALM_BRITANNIA}, // W.t2a_to_newbie_lvl[]

    // passt hier eigentlich nicht hin
    // Newbie <-> Dungeon Delucia
//  {5150, 4062,   38, REALM_BRITANNIA, 6004, 1378,    1, REALM_BRITANNIA},
//  {5150, 4063,   38, REALM_BRITANNIA, 6004, 1379,   -1, REALM_BRITANNIA},
//  {6005, 1378,    0, REALM_BRITANNIA, 5151, 4062,   37, REALM_BRITANNIA},
//  {6005, 1379,    2, REALM_BRITANNIA, 5151, 4063,   37, REALM_BRITANNIA},

    // T2A <-> Minoc Gilchhoehle
//  {2399,  198,    1, REALM_BRITANNIA, 6087, 3675,   17, REALM_BRITANNIA},
//  {2400,  198,    1, REALM_BRITANNIA, 6087, 3676,   18, REALM_BRITANNIA},
//  {6086, 3675,   15, REALM_BRITANNIA, 2399,  199,    1, REALM_BRITANNIA},
//  {6086, 3676,   16, REALM_BRITANNIA, 2400,  199,    1, REALM_BRITANNIA},
    // }}}

    // Hythloth {{{
    {4721, 3813,    0, REALM_BRITANNIA, 5905,   20,   44, REALM_BRITANNIA}, // hythloth_out_x[]
    {4722, 3813,    0, REALM_BRITANNIA, 5905,   20,   44, REALM_BRITANNIA}, // hythloth_out_x[]
    {4723, 3813,    0, REALM_BRITANNIA, 5905,   20,   44, REALM_BRITANNIA}, // hythloth_out_x[]
    {5904,   17,   59, REALM_BRITANNIA, 4722, 3815,    1, REALM_BRITANNIA}, // hythloth_ent_out_x[]
    {5905,   17,   59, REALM_BRITANNIA, 4722, 3815,    1, REALM_BRITANNIA}, // hythloth_ent_out_x[]
    {5906,   17,   59, REALM_BRITANNIA, 4722, 3815,    1, REALM_BRITANNIA}, // hythloth_ent_out_x[]

    {5905,   97, -255, REALM_BRITANNIA, 5976,  169,    1, REALM_BRITANNIA}, // hythloth_1_2_x[]
    {5973,  169, -255, REALM_BRITANNIA, 5905,   99,    0, REALM_BRITANNIA}, // hythloth_2_1_x[]

    {5920,  168,   16, REALM_BRITANNIA, 6083,  145,  240, REALM_BRITANNIA}, // hythloth_2_3_x[]
    {5920,  169,   17, REALM_BRITANNIA, 6083,  145,  240, REALM_BRITANNIA}, // hythloth_2_3_x[]
    {5920,  170,   16, REALM_BRITANNIA, 6083,  145,  240, REALM_BRITANNIA}, // hythloth_2_3_x[]
    {6082,  144, -255, REALM_BRITANNIA, 5919,  169,   22, REALM_BRITANNIA}, // hythloth_3_2_x[]
    {6082,  145, -255, REALM_BRITANNIA, 5919,  169,   22, REALM_BRITANNIA}, // hythloth_3_2_x[]
    {6082,  146, -255, REALM_BRITANNIA, 5919,  169,   22, REALM_BRITANNIA}, // hythloth_3_2_x[]

    {6040,  192, -255, REALM_BRITANNIA, 6059,   89,   24, REALM_BRITANNIA}, // hythloth_3_4_x[]
    {6040,  193, -255, REALM_BRITANNIA, 6059,   89,   24, REALM_BRITANNIA}, // hythloth_3_4_x[]
    {6040,  194, -255, REALM_BRITANNIA, 6059,   89,   24, REALM_BRITANNIA}, // hythloth_3_4_x[]
    {6058,   88, -255, REALM_BRITANNIA, 6039,  193,   17, REALM_BRITANNIA}, // hythloth_4_3_x[]
    {6058,   89, -255, REALM_BRITANNIA, 6039,  193,   17, REALM_BRITANNIA}, // hythloth_4_3_x[]
    {6058,   90, -255, REALM_BRITANNIA, 6039,  193,   17, REALM_BRITANNIA}, // hythloth_4_3_x[]
    // }}}

    // Teleporterinsel {{{
    // Zugaenge {{{
    // Cove - Teleporterinsel
    {2250, 1220,    5, REALM_BRITANNIA, 2215, 3124,    5, REALM_BRITANNIA},

    // Minoc - Teleporterinsel
    {2483,  448,   20, REALM_BRITANNIA, 2215, 3124,    5, REALM_BRITANNIA},

    // Yew - Teleporterinsel
    { 532,  979,    5, REALM_BRITANNIA, 2215, 3124,    5, REALM_BRITANNIA},

    // Vesper - Teleporterinsel
    {2909,  689,    5, REALM_BRITANNIA, 2215, 3124,    5, REALM_BRITANNIA},

    // Nujelm - Teleporterinsel
    {3729, 1279,    5, REALM_BRITANNIA, 2215, 3124,    5, REALM_BRITANNIA},

    // Jhe'lom - Teleporterinsel
    {1397, 3810,    5, REALM_BRITANNIA, 2215, 3124,    5, REALM_BRITANNIA},

    // Nirdana - Teleporterinsel
    {3344,  214,    5, REALM_BRITANNIA, 2215, 3124,    5, REALM_BRITANNIA},

    // Aussenposten - Teleporterinsel
    {2067,  800,    5, REALM_BRITANNIA, 2215, 3124,    5, REALM_BRITANNIA},

    // Herberge Baustelle - Teleporterinsel
    { 996, 1925,    5, REALM_BRITANNIA, 2215, 3124,    5, REALM_BRITANNIA},

    // Brueckeninsel - Teleporterinsel
    {1521, 1468,    0, REALM_BRITANNIA, 2215, 3124,    5, REALM_BRITANNIA},
    {1522, 1468,    0, REALM_BRITANNIA, 2215, 3124,    5, REALM_BRITANNIA},
    {1523, 1468,    0, REALM_BRITANNIA, 2215, 3124,    5, REALM_BRITANNIA},
    {1524, 1468,    0, REALM_BRITANNIA, 2215, 3124,    5, REALM_BRITANNIA},
    {1525, 1468,    0, REALM_BRITANNIA, 2215, 3124,    5, REALM_BRITANNIA},

    // Zoo - Teleporterinsel
    {1266,  513,   35, REALM_BRITANNIA, 2215, 3124,    5, REALM_BRITANNIA},

    // Labyrinth - Teleporterinsel
    {1056, 2215,    5, REALM_BRITANNIA, 2215, 3124,    5, REALM_BRITANNIA},
    {1192, 2136,    5, REALM_BRITANNIA, 2215, 3124,    5, REALM_BRITANNIA},
    {1245, 2293,    5, REALM_BRITANNIA, 2215, 3124,    5, REALM_BRITANNIA},
    {1042, 2293,    5, REALM_BRITANNIA, 2215, 3124,    5, REALM_BRITANNIA},
    {1042, 2169,    5, REALM_BRITANNIA, 2215, 3124,    5, REALM_BRITANNIA},
    {1117, 2204,    5, REALM_BRITANNIA, 2215, 3124,    5, REALM_BRITANNIA},

    // Friedhof bei Brueckeninsel - Teleporterinsel
    {1348, 1500,   10, REALM_BRITANNIA, 2215, 3124,    5, REALM_BRITANNIA},
    {1347, 1500,   10, REALM_BRITANNIA, 2215, 3124,    5, REALM_BRITANNIA},
    {1348, 1499,   10, REALM_BRITANNIA, 2215, 3124,    5, REALM_BRITANNIA},
    {1347, 1499,   10, REALM_BRITANNIA, 2215, 3124,    5, REALM_BRITANNIA},
    // }}}

    // Ausgaenge {{{
    // Teleporterinsel - Cove
    {2197, 3116,    5, REALM_BRITANNIA, 2250, 1229,    5, REALM_BRITANNIA,
      struct{j := 1}},

    // Teleporterinsel - Minoc
    {2197, 3108,    5, REALM_BRITANNIA, 2491,  448,   20, REALM_BRITANNIA,
      struct{j := 1}},

    // Teleporterinsel - Yew
    {2197, 3100,    5, REALM_BRITANNIA,  538,  979,    5, REALM_BRITANNIA,
      struct{j := 1}},

    // Teleporterinsel - Vesper
    {2197, 3092,    5, REALM_BRITANNIA, 2909,  695,    5, REALM_BRITANNIA,
      struct{j := 1}},

    // Teleporterinsel - Nujelm
    {2197, 3084,    5, REALM_BRITANNIA, 3737, 1279,    5, REALM_BRITANNIA,
      struct{j := 1}},

    // Teleporterinsel - Nirdana
    {2197, 3076,    5, REALM_BRITANNIA, 3340,  214,    5, REALM_BRITANNIA,
      struct{a := 1, j := 1}},

    // Teleporterinsel - Jhe'lom
    {2197, 3068,    5, REALM_BRITANNIA, 1392, 3815,    5, REALM_BRITANNIA},

    // Teleporterinsel - Zoo
    {2215, 3116,    5, REALM_BRITANNIA, 1268,  513,   35, REALM_BRITANNIA},

    // Teleporterinsel - Herberge bei Baustelle
    {2215, 3108,    5, REALM_BRITANNIA,  996, 1921,    5, REALM_BRITANNIA,
      struct{j := 1}},

    // Teleporterinsel - Labyrinth
    {2215, 3092,    5, REALM_BRITANNIA, 1062, 2215,    5, REALM_BRITANNIA,
      struct{f := 1}},

    // Teleporterinsel - Aussenposten
    {2215, 3084,    5, REALM_BRITANNIA, 2073,  800,    5, REALM_BRITANNIA,
      struct{j := 1}},

    // Teleporterinsel - Brueckeninsel
    {2215, 3076,    5, REALM_BRITANNIA, 1523, 1466,    2, REALM_BRITANNIA,
      struct{j := 1}},
    // }}}
    // }}}

    // Dungeons, Questgegenden {{{
    // Covetous {{{
    // Zugaenge
    // Ein-/Ausgang (Hoehle im Sueden, mittig)
    {2498,  916, -255, REALM_BRITANNIA, 5455, 1863,    0, REALM_BRITANNIA},
    {2499,  916, -255, REALM_BRITANNIA, 5456, 1863,    0, REALM_BRITANNIA},
    {2500,  916, -255, REALM_BRITANNIA, 5457, 1863,    0, REALM_BRITANNIA},
    {5455, 1864, -255, REALM_BRITANNIA, 2498,  917,    0, REALM_BRITANNIA},
    {5456, 1864, -255, REALM_BRITANNIA, 2499,  917,    0, REALM_BRITANNIA},
    {5457, 1864, -255, REALM_BRITANNIA, 2500,  917,    0, REALM_BRITANNIA},

    // Covetous Level 1 <-> Level 2 (linker Felsenweg, Sueden)
    {5392, 1959, -255, REALM_BRITANNIA, 2420,  884,    1, REALM_BRITANNIA},
    {5393, 1959, -255, REALM_BRITANNIA, 2421,  884,    1, REALM_BRITANNIA},
    {5394, 1959, -255, REALM_BRITANNIA, 2422,  884,    1, REALM_BRITANNIA},
    {2420,  883, -255, REALM_BRITANNIA, 5392, 1958,    0, REALM_BRITANNIA},
    {2421,  883, -255, REALM_BRITANNIA, 5393, 1958,    0, REALM_BRITANNIA},
    {2422,  883, -255, REALM_BRITANNIA, 5394, 1958,    0, REALM_BRITANNIA},

    // Covetous Level 2 (linker Felsenweg, Norden) <-> Level 3
    {2384,  836, -255, REALM_BRITANNIA, 5614, 1996,    0, REALM_BRITANNIA},
    {2384,  837, -255, REALM_BRITANNIA, 5614, 1997,    0, REALM_BRITANNIA},
    {2384,  838, -255, REALM_BRITANNIA, 5614, 1998,    0, REALM_BRITANNIA},
    {5615, 1996, -255, REALM_BRITANNIA, 2385,  836,    1, REALM_BRITANNIA},
    {5615, 1997, -255, REALM_BRITANNIA, 2385,  837,    1, REALM_BRITANNIA},
    {5615, 1998, -255, REALM_BRITANNIA, 2385,  838,    1, REALM_BRITANNIA},

    // Covetous Level 3 <-> Level 4 (rechter Felsenweg, Westen)
    {5388, 2027, -255, REALM_BRITANNIA, 2455,  859,    1, REALM_BRITANNIA},
    {5389, 2027, -255, REALM_BRITANNIA, 2456,  859,    1, REALM_BRITANNIA},
    {5390, 2027, -255, REALM_BRITANNIA, 2457,  859,    1, REALM_BRITANNIA},
    {2455,  858, -255, REALM_BRITANNIA, 5388, 2026,    0, REALM_BRITANNIA},
    {2456,  858, -255, REALM_BRITANNIA, 5389, 2026,    0, REALM_BRITANNIA},
    {2457,  858, -255, REALM_BRITANNIA, 5390, 2026,    0, REALM_BRITANNIA},

    // Covetous Level 4 (rechter Felsenweg, Osten) <-> Level 5
    {2544,  850,    0, REALM_BRITANNIA, 5578, 1924,    0, REALM_BRITANNIA},
    {2545,  850,    0, REALM_BRITANNIA, 5579, 1924,    0, REALM_BRITANNIA},
    {2546,  850,    0, REALM_BRITANNIA, 5580, 1924,    0, REALM_BRITANNIA},
    {5578, 1926, -255, REALM_BRITANNIA, 2544,  851,    1, REALM_BRITANNIA},
    {5579, 1926, -255, REALM_BRITANNIA, 2545,  851,    1, REALM_BRITANNIA},
    {5580, 1926, -255, REALM_BRITANNIA, 2546,  851,    1, REALM_BRITANNIA},

    // Covetous Level 5 <-> Level 6
    {5556, 1827, -255, REALM_BRITANNIA, 5551, 1804,   17, REALM_BRITANNIA},
    {5557, 1827, -255, REALM_BRITANNIA, 5552, 1804,   17, REALM_BRITANNIA},
    {5551, 1803,   20, REALM_BRITANNIA, 5556, 1826,  -10, REALM_BRITANNIA},
    {5552, 1803,   20, REALM_BRITANNIA, 5557, 1826,  -10, REALM_BRITANNIA},
    {5553, 1803,   20, REALM_BRITANNIA, 5557, 1826,  -10, REALM_BRITANNIA},

    // Covetous <-> Gang zur Eishoehle
    {5595, 1840, -255, REALM_BRITANNIA, 5599, 1808,    7, REALM_BRITANNIA},
    {5595, 1841, -255, REALM_BRITANNIA, 5599, 1809,    7, REALM_BRITANNIA},
    {5598, 1808,   10, REALM_BRITANNIA, 5594, 1840,  -10, REALM_BRITANNIA},
    {5598, 1809,   10, REALM_BRITANNIA, 5594, 1841,  -10, REALM_BRITANNIA},
    {5598, 1810,   10, REALM_BRITANNIA, 5594, 1841,  -10, REALM_BRITANNIA},

    // Gang zur Eishoehle <-> Gang
    {5613, 1808,   -8, REALM_BRITANNIA, 5463, 1795,   12, REALM_BRITANNIA},
    {5613, 1809,   -8, REALM_BRITANNIA, 5463, 1796,   12, REALM_BRITANNIA},
    {5613, 1810,   -8, REALM_BRITANNIA, 5463, 1797,   12, REALM_BRITANNIA},
    {5462, 1795,   17, REALM_BRITANNIA, 5612, 1808,   -3, REALM_BRITANNIA},
    {5462, 1796,   17, REALM_BRITANNIA, 5612, 1809,   -3, REALM_BRITANNIA},
    {5462, 1797,   17, REALM_BRITANNIA, 5612, 1810,   -3, REALM_BRITANNIA},
    // }}}

    // Covetous-Minenstadt {{{
    // Ein-/Ausgaenge {{{
    // Covetous-Ameisenhaus Kellertreppe <-> Covetous-Minen-Level2
    {2270,  926,    5, REALM_BRITANNIA, 5138, 1874,    8, REALM_BRITANNIA},
    {2270,  927,   -3, REALM_BRITANNIA, 5138, 1875,    8, REALM_BRITANNIA},
    {2270,  928,   -3, REALM_BRITANNIA, 5138, 1876,    8, REALM_BRITANNIA},
    {5137, 1873,   12, REALM_BRITANNIA, 2269,  926,    2, REALM_BRITANNIA},
    {5137, 1874,   12, REALM_BRITANNIA, 2269,  926,    2, REALM_BRITANNIA},
    {5137, 1875,   12, REALM_BRITANNIA, 2269,  927,    2, REALM_BRITANNIA},
    {5137, 1876,   12, REALM_BRITANNIA, 2269,  928,    2, REALM_BRITANNIA},
    {5137, 1877,   12, REALM_BRITANNIA, 2269,  928,    2, REALM_BRITANNIA},

    // Teleport-Zugaenge
    // Ameisenhaus <-> Minenstadt
    {2267,  927,   35, REALM_BRITANNIA, 5454, 1922,   20, REALM_BRITANNIA},
    {5454, 1921,   20, REALM_BRITANNIA, 2274,  927,   35, REALM_BRITANNIA},

    // Hoehle Suedwest -> Covetous-Minen-Level2
    {2399,  915,    0, REALM_BRITANNIA, 5171, 1923,    0, REALM_BRITANNIA},
    // Hoehle Sued     -> Covetous-Minen-Level2
    {2430,  867,    0, REALM_BRITANNIA, 5183, 1827,    0, REALM_BRITANNIA},
    // Hoehle Suedost  -> Covetous-Minen-Level2
    {2558,  889,    0, REALM_BRITANNIA, 5195, 1887,    0, REALM_BRITANNIA},
    // }}}

    // Covetous-Minen-Level2 -> Covetous-Minen-Level3
    {5290, 1890,    0, REALM_BRITANNIA, 5134, 1948,    4, REALM_BRITANNIA},
    // Covetous-Minen-Level2 -> Covetous-Minen-Level3
    {5330, 1929,    0, REALM_BRITANNIA, 5157, 2013,    4, REALM_BRITANNIA},

    // Covetous-Minen-Level3 -> Ameisenhaus
    {5331, 1977,    0, REALM_BRITANNIA, 2274,  927,   35, REALM_BRITANNIA},
    // }}}

    // Eisdungeon, Khas {{{
    // Ein-/Ausgang Eisdungeon
    {1997,   80,    7, REALM_BRITANNIA, 5881,  241,    0, REALM_BRITANNIA},
    {1997,   81,    7, REALM_BRITANNIA, 5881,  242,    0, REALM_BRITANNIA},
    {1997,   82,    6, REALM_BRITANNIA, 5881,  243,    0, REALM_BRITANNIA},
    {5882,  241,    0, REALM_BRITANNIA, 1998,   80,    5, REALM_BRITANNIA},
    {5882,  242,    0, REALM_BRITANNIA, 1998,   81,    5, REALM_BRITANNIA},
    {5882,  243,    0, REALM_BRITANNIA, 1998,   82,    5, REALM_BRITANNIA},

    // Eisdungeon -> Icelands
//  {5873,  145, -255, REALM_BRITANNIA, 5203, 2328,   30, REALM_BRITANNIA},
//  {5874,  145, -255, REALM_BRITANNIA, 5203, 2328,   30, REALM_BRITANNIA},
//  {5875,  145, -255, REALM_BRITANNIA, 5208, 2323,   31, REALM_BRITANNIA},
//  {5876,  145, -255, REALM_BRITANNIA, 5208, 2323,   31, REALM_BRITANNIA},
//  {5877,  145, -255, REALM_BRITANNIA, 5203, 2328,   30, REALM_BRITANNIA},
//  {5878,  145, -255, REALM_BRITANNIA, 5203, 2328,   30, REALM_BRITANNIA},
//  {5203, 2327, -255, REALM_BRITANNIA, 5876,  146,   27, REALM_BRITANNIA},
//  {5207, 2322, -255, REALM_BRITANNIA, 5876,  146,   27, REALM_BRITANNIA},
//  {5207, 2323, -255, REALM_BRITANNIA, 5876,  146,   27, REALM_BRITANNIA},

    // Eisdungeon <-> Grab des Erik
    {5762,  139,  -13, REALM_BRITANNIA, 5682,  262,    0, REALM_BRITANNIA},
    {5763,  139,  -13, REALM_BRITANNIA, 5683,  262,    0, REALM_BRITANNIA},
    {5682,  263,    0, REALM_BRITANNIA, 5762,  140,  -10, REALM_BRITANNIA},
    {5683,  263,    0, REALM_BRITANNIA, 5763,  140,  -10, REALM_BRITANNIA},

    // Eisdungeon <-> Wikinger
    {5831,  323,   34, REALM_BRITANNIA, 5849,  237,  -24, REALM_BRITANNIA},
    {5832,  323,   34, REALM_BRITANNIA, 5850,  237,  -24, REALM_BRITANNIA},
    {5833,  323,   33, REALM_BRITANNIA, 5851,  237,  -24, REALM_BRITANNIA},
    {5834,  323,   33, REALM_BRITANNIA, 5852,  237,  -25, REALM_BRITANNIA},
    {5835,  323,   33, REALM_BRITANNIA, 5853,  237,  -23, REALM_BRITANNIA},

    {5849,  238,  -25, REALM_BRITANNIA, 5831,  324,   27, REALM_BRITANNIA},
    {5850,  238,  -25, REALM_BRITANNIA, 5832,  324,   26, REALM_BRITANNIA},
    {5851,  238,  -25, REALM_BRITANNIA, 5833,  324,   28, REALM_BRITANNIA},
    {5852,  238,  -25, REALM_BRITANNIA, 5834,  324,   27, REALM_BRITANNIA},
    {5853,  238,  -23, REALM_BRITANNIA, 5835,  324,   27, REALM_BRITANNIA},

    // Eisdungeon <-> Tempel Khas
    {5704,  145,  -45, REALM_BRITANNIA, 5731,  300,    0, REALM_BRITANNIA},
    {5704,  146,  -45, REALM_BRITANNIA, 5731,  301,    0, REALM_BRITANNIA},
    {5704,  147,  -45, REALM_BRITANNIA, 5731,  302,    0, REALM_BRITANNIA},
    {5733,  300,    0, REALM_BRITANNIA, 5705,  145,  -44, REALM_BRITANNIA},
    {5733,  301,    0, REALM_BRITANNIA, 5705,  146,  -44, REALM_BRITANNIA},
    {5733,  302,    0, REALM_BRITANNIA, 5705,  147,  -44, REALM_BRITANNIA},

    // Khas-Tempel <-> die beide Tuerme
    {5667,  315,  -25, REALM_BRITANNIA, {5677, 5677}, {322, 309}, {44, 44}, REALM_BRITANNIA},
    {5668,  315,  -25, REALM_BRITANNIA, {5677, 5677}, {322, 309}, {44, 44}, REALM_BRITANNIA},
    {5667,  316,  -25, REALM_BRITANNIA, {5677, 5677}, {322, 309}, {44, 44}, REALM_BRITANNIA},
    {5668,  316,  -25, REALM_BRITANNIA, {5677, 5677}, {322, 309}, {44, 44}, REALM_BRITANNIA},
    {5683,  322,   44, REALM_BRITANNIA, 5669,  317,  -25, REALM_BRITANNIA},
    {5684,  322,   44, REALM_BRITANNIA, 5669,  317,  -25, REALM_BRITANNIA},
    {5683,  323,   44, REALM_BRITANNIA, 5669,  317,  -25, REALM_BRITANNIA},
    {5684,  323,   44, REALM_BRITANNIA, 5669,  317,  -25, REALM_BRITANNIA},
    {5683,  308,   44, REALM_BRITANNIA, 5669,  317,  -25, REALM_BRITANNIA},
    {5684,  308,   44, REALM_BRITANNIA, 5669,  317,  -25, REALM_BRITANNIA},
    {5683,  309,   44, REALM_BRITANNIA, 5669,  317,  -25, REALM_BRITANNIA},
    {5684,  309,   44, REALM_BRITANNIA, 5669,  317,  -25, REALM_BRITANNIA},
    // }}}

    // Deceit (altes Questdungeon, Eisinsel) {{{
    // Ein-/Ausgang
    {4110,  430,    5, REALM_BRITANNIA, 5187,  638,    0, REALM_BRITANNIA},
    {4111,  430,    5, REALM_BRITANNIA, 5188,  638,    0, REALM_BRITANNIA},
    {4112,  430,    5, REALM_BRITANNIA, 5189,  638,    0, REALM_BRITANNIA},
    {5186,  639,    0, REALM_BRITANNIA, 4110,  431,    5, REALM_BRITANNIA},
    {5187,  639,    0, REALM_BRITANNIA, 4110,  431,    5, REALM_BRITANNIA},
    {5188,  639,    0, REALM_BRITANNIA, 4111,  431,    5, REALM_BRITANNIA},
    {5189,  639,    0, REALM_BRITANNIA, 4112,  431,    5, REALM_BRITANNIA},

    // Level 1 <-> Level 1 (rotes Pentagramm)
    {5227,  563,    0, REALM_BRITANNIA, 5176,  616,    0, REALM_BRITANNIA},
    {5175,  615,    0, REALM_BRITANNIA, 5226,  564,    0, REALM_BRITANNIA},

    // Level 1 <-> Level 2
    {5216,  586,  -13, REALM_BRITANNIA, 5304,  533,    6, REALM_BRITANNIA},
    {5217,  586,  -13, REALM_BRITANNIA, 5305,  533,    6, REALM_BRITANNIA},
    {5218,  586,  -13, REALM_BRITANNIA, 5306,  533,    6, REALM_BRITANNIA},
    {5304,  532,    7, REALM_BRITANNIA, 5216,  585,   -8, REALM_BRITANNIA},
    {5305,  532,    7, REALM_BRITANNIA, 5217,  585,   -8, REALM_BRITANNIA},
    {5306,  532,    7, REALM_BRITANNIA, 5218,  585,   -8, REALM_BRITANNIA},

    // Level 1 <-> Level 3 (graues Pentagramm)
    {5226,  541,    0, REALM_BRITANNIA, 5220,  732,  -20, REALM_BRITANNIA},
    {5221,  731,  -20, REALM_BRITANNIA, 5225,  542,    0, REALM_BRITANNIA},

    // Level 2 <-> Level 3
    {5346,  578,    5, REALM_BRITANNIA, 5138,  650,    5, REALM_BRITANNIA},
    {5137,  649,    5, REALM_BRITANNIA, 5345,  579,    5, REALM_BRITANNIA},

    // Level 3 <-> Level 3 (graues Pentagramm)
    {5151,  717,    0, REALM_BRITANNIA, 5219,  732,  -20, REALM_BRITANNIA},
    {5218,  731,  -20, REALM_BRITANNIA, 5150,  718,    0, REALM_BRITANNIA},

    // Level 3 <-> Level 4
    {5218,  762,  -35, REALM_BRITANNIA, 5305,  651,    7, REALM_BRITANNIA},
    {5219,  762,  -28, REALM_BRITANNIA, 5306,  651,    7, REALM_BRITANNIA},
    {5305,  650,   12, REALM_BRITANNIA, 5218,  761,  -28, REALM_BRITANNIA},
    {5306,  650,   12, REALM_BRITANNIA, 5219,  761,  -28, REALM_BRITANNIA},
    // }}}

    // Questinsel und neues Questdungeon {{{
    // Ein-/Ausgang (Quest-Tal <-> Quest-Hoehle)
    {4829,  286,    0, REALM_BRITANNIA, 5137, 1204,    0, REALM_BRITANNIA},
    {5136, 1205,    0, REALM_BRITANNIA, 4828,  287,    0, REALM_BRITANNIA},
    {5137, 1205,    0, REALM_BRITANNIA, 4829,  287,    0, REALM_BRITANNIA},
    {5138, 1205,    0, REALM_BRITANNIA, 4830,  287,    0, REALM_BRITANNIA},

    // Quest-Hoehle <-> neues Questdungeon
    {5146, 1190,    0, REALM_BRITANNIA, 5708,  945,   20, REALM_BRITANNIA},
    {5146, 1191,    0, REALM_BRITANNIA, 5708,  946,   20, REALM_BRITANNIA},
    {5146, 1192,    0, REALM_BRITANNIA, 5708,  947,   20, REALM_BRITANNIA},
    {5146, 1193,    0, REALM_BRITANNIA, 5708,  948,   20, REALM_BRITANNIA},
    {5146, 1194,    0, REALM_BRITANNIA, 5708,  949,   20, REALM_BRITANNIA},
    {5146, 1195,    0, REALM_BRITANNIA, 5708,  950,   20, REALM_BRITANNIA},
    {5146, 1196,    0, REALM_BRITANNIA, 5708,  951,   20, REALM_BRITANNIA},
    {5146, 1197,    0, REALM_BRITANNIA, 5708,  952,   20, REALM_BRITANNIA},
    {5146, 1198,    0, REALM_BRITANNIA, 5708,  953,   20, REALM_BRITANNIA},
    {5146, 1199,    0, REALM_BRITANNIA, 5708,  954,   20, REALM_BRITANNIA},
    {5707,  945,   20, REALM_BRITANNIA, 5145, 1190,    0, REALM_BRITANNIA},
    {5707,  946,   20, REALM_BRITANNIA, 5145, 1191,    0, REALM_BRITANNIA},
    {5707,  947,   20, REALM_BRITANNIA, 5145, 1192,    0, REALM_BRITANNIA},
    {5707,  948,   20, REALM_BRITANNIA, 5145, 1193,    0, REALM_BRITANNIA},
    {5707,  949,   20, REALM_BRITANNIA, 5145, 1194,    0, REALM_BRITANNIA},
    {5707,  950,   20, REALM_BRITANNIA, 5145, 1195,    0, REALM_BRITANNIA},
    {5707,  951,   20, REALM_BRITANNIA, 5145, 1196,    0, REALM_BRITANNIA},
    {5707,  952,   20, REALM_BRITANNIA, 5145, 1197,    0, REALM_BRITANNIA},
    {5707,  953,   20, REALM_BRITANNIA, 5145, 1198,    0, REALM_BRITANNIA},
    {5707,  954,   25, REALM_BRITANNIA, 5145, 1199,    0, REALM_BRITANNIA},
    // }}}

    // Delucia {{{
    // Ein-/Ausgang (Berg bei Trinsic)
    {5898, 1411,   38, REALM_BRITANNIA, 1628, 3321,    0, REALM_BRITANNIA},
    {5899, 1411,   43, REALM_BRITANNIA, 1628, 3321,    0, REALM_BRITANNIA},
    {5900, 1411,   42, REALM_BRITANNIA, 1629, 3321,    0, REALM_BRITANNIA},
    {1628, 3320,    0, REALM_BRITANNIA, 5899, 1410,   42, REALM_BRITANNIA},
    {1629, 3320,    0, REALM_BRITANNIA, 5900, 1410,   40, REALM_BRITANNIA},

    // Level 1 <-> Level 2(?)
    {5918, 1410,  -29, REALM_BRITANNIA, 5963, 1407,   54, REALM_BRITANNIA},
    {5918, 1411,  -29, REALM_BRITANNIA, 5963, 1408,   53, REALM_BRITANNIA},
    {5918, 1412,  -29, REALM_BRITANNIA, 5963, 1409,   53, REALM_BRITANNIA},
    {5962, 1407,   57, REALM_BRITANNIA, 5917, 1410,  -27, REALM_BRITANNIA},
    {5962, 1408,   56, REALM_BRITANNIA, 5917, 1411,  -27, REALM_BRITANNIA},
    {5962, 1409,   56, REALM_BRITANNIA, 5917, 1412,  -27, REALM_BRITANNIA},
    // }}}

    // Despise (Grossdrachenhoehle) {{{
    // Ein-/Ausgang (Grossdrachenhoehle)
    {1296, 1080, -255, REALM_BRITANNIA, 5587,  630,   30, REALM_BRITANNIA},
    {1296, 1081, -255, REALM_BRITANNIA, 5587,  631,   30, REALM_BRITANNIA},
    {1296, 1082, -255, REALM_BRITANNIA, 5587,  632,   30, REALM_BRITANNIA},
    {5588,  630, -255, REALM_BRITANNIA, 1297, 1080,    0, REALM_BRITANNIA},
    {5588,  631, -255, REALM_BRITANNIA, 1297, 1081,    0, REALM_BRITANNIA},
    {5588,  632, -255, REALM_BRITANNIA, 1297, 1082,    0, REALM_BRITANNIA},

    // Eingang <-> Level 2 (Keller)
    {5571,  632,   12, REALM_BRITANNIA, 5521,  672,   27, REALM_BRITANNIA},
    {5571,  633,   10, REALM_BRITANNIA, 5521,  673,   27, REALM_BRITANNIA},
    {5571,  634,   12, REALM_BRITANNIA, 5521,  674,   27, REALM_BRITANNIA},
    {5522,  672,   32, REALM_BRITANNIA, 5572,  632,   17, REALM_BRITANNIA},
    {5522,  673,   32, REALM_BRITANNIA, 5572,  633,   17, REALM_BRITANNIA},
    {5522,  674,   32, REALM_BRITANNIA, 5572,  634,   17, REALM_BRITANNIA},

    // Eingang <-> Level 1 (Obergeschoss)
    // FIXME: um die Treppe nach unten kann man drumherum
    {5572,  628,   45, REALM_BRITANNIA, 5503,  569,   51, REALM_BRITANNIA},
    {5572,  629,   45, REALM_BRITANNIA, 5503,  570,   51, REALM_BRITANNIA},
    {5572,  630,   45, REALM_BRITANNIA, 5503,  571,   51, REALM_BRITANNIA},
    {5504,  569, -255, REALM_BRITANNIA, 5573,  628,   42, REALM_BRITANNIA},
    {5504,  570, -255, REALM_BRITANNIA, 5573,  629,   42, REALM_BRITANNIA},
    {5504,  571, -255, REALM_BRITANNIA, 5573,  630,   42, REALM_BRITANNIA},

    // Despise Level 2 <-> Level 3
    {5386,  755,   -8, REALM_BRITANNIA, 5409,  858,   52, REALM_BRITANNIA},
    {5386,  756,   -8, REALM_BRITANNIA, 5409,  859,   52, REALM_BRITANNIA},
    {5386,  757,   -8, REALM_BRITANNIA, 5409,  860,   52, REALM_BRITANNIA},
    {5410,  858,   57, REALM_BRITANNIA, 5387,  755,   -3, REALM_BRITANNIA},
    {5410,  859,   57, REALM_BRITANNIA, 5387,  756,   -3, REALM_BRITANNIA},
    {5410,  860,   57, REALM_BRITANNIA, 5387,  757,   -3, REALM_BRITANNIA},
    // }}}

    // Destard {{{
    // Ein-/Ausgang (Berg suedlich Waldlabyrinth, noerdlichste Hoehle)
    {1175, 2635,    0, REALM_BRITANNIA, 5242, 1006,    0, REALM_BRITANNIA},
    {1176, 2635,    0, REALM_BRITANNIA, 5243, 1006,    0, REALM_BRITANNIA},
    {1177, 2635,    0, REALM_BRITANNIA, 5244, 1006,    0, REALM_BRITANNIA},
    {5242, 1007,    0, REALM_BRITANNIA, 1175, 2636,    0, REALM_BRITANNIA},
    {5243, 1007,    0, REALM_BRITANNIA, 1176, 2636,    0, REALM_BRITANNIA},
    {5244, 1007,    0, REALM_BRITANNIA, 1177, 2636,    0, REALM_BRITANNIA},

    // Level 1 <-> Level 2 (Rampe)
    {5129,  909,  -28, REALM_BRITANNIA, 5142,  798,   17, REALM_BRITANNIA},
    {5130,  909,  -28, REALM_BRITANNIA, 5143,  798,   17, REALM_BRITANNIA},
    {5131,  909,  -28, REALM_BRITANNIA, 5144,  798,   17, REALM_BRITANNIA},
    {5132,  909,  -28, REALM_BRITANNIA, 5145,  798,   17, REALM_BRITANNIA},
    {5142,  797,   22, REALM_BRITANNIA, 5129,  908,  -23, REALM_BRITANNIA},
    {5143,  797,   22, REALM_BRITANNIA, 5130,  908,  -23, REALM_BRITANNIA},
    {5144,  797,   22, REALM_BRITANNIA, 5131,  908,  -23, REALM_BRITANNIA},
    {5145,  797,   22, REALM_BRITANNIA, 5132,  908,  -23, REALM_BRITANNIA},

    // Level 1 <-> Level 2 (Pentagramm-Teleport)
    {5165,  839,    0, REALM_BRITANNIA, 5205,  779,    0, REALM_BRITANNIA},
    {5205,  778,    0, REALM_BRITANNIA, 5165,  838,    0, REALM_BRITANNIA},

    // Level 2 <-> Level 3
    {5133,  984,   22, REALM_BRITANNIA, 5152,  808,  -19, REALM_BRITANNIA},
    {5133,  985,   22, REALM_BRITANNIA, 5152,  809,  -19, REALM_BRITANNIA},
    {5133,  986,   22, REALM_BRITANNIA, 5152,  810,  -19, REALM_BRITANNIA},
    {5133,  987,   22, REALM_BRITANNIA, 5152,  811,  -19, REALM_BRITANNIA},
    {5153,  808,  -25, REALM_BRITANNIA, 5134,  984,   17, REALM_BRITANNIA},
    {5153,  809,  -25, REALM_BRITANNIA, 5134,  985,   17, REALM_BRITANNIA},
    {5153,  810,  -25, REALM_BRITANNIA, 5134,  986,   17, REALM_BRITANNIA},
    {5153,  811,  -25, REALM_BRITANNIA, 5134,  987,   17, REALM_BRITANNIA},
    // }}}

    // Des-Dungeon (Lava-Dungeon) // {{{
    // Eingang <-> Hafenhoehle Serpents Hold
    {5827, 1502,    0, REALM_BRITANNIA, 2922, 3406,    8, REALM_BRITANNIA},
    {5828, 1502,   -1, REALM_BRITANNIA, 2923, 3406,    8, REALM_BRITANNIA},
    {5829, 1502,    2, REALM_BRITANNIA, 2924, 3406,    7, REALM_BRITANNIA},
    {2922, 3405,    7, REALM_BRITANNIA, 5827, 1501,    0, REALM_BRITANNIA},
    {2923, 3405,    6, REALM_BRITANNIA, 5828, 1501,    0, REALM_BRITANNIA},
    {2924, 3405,    7, REALM_BRITANNIA, 5829, 1501,    1, REALM_BRITANNIA},

    // Eingang <-> Wueste (mit Lava)
    {5792, 1415,   41, REALM_BRITANNIA, 5758, 2907,   15, REALM_BRITANNIA},
    {5792, 1416,   41, REALM_BRITANNIA, 5758, 2908,   15, REALM_BRITANNIA},
    {5792, 1417,   41, REALM_BRITANNIA, 5758, 2909,   15, REALM_BRITANNIA},
    {5757, 2907,   15, REALM_BRITANNIA, 5791, 1415,   38, REALM_BRITANNIA},
    {5757, 2908,   14, REALM_BRITANNIA, 5791, 1416,   38, REALM_BRITANNIA},
    {5757, 2909,   15, REALM_BRITANNIA, 5791, 1417,   40, REALM_BRITANNIA},

    // Level 1 <-> Level 2
    {5701, 1320,   16, REALM_BRITANNIA, 5786, 1337,   -5, REALM_BRITANNIA},
    {5702, 1320,   16, REALM_BRITANNIA, 5787, 1337,   -5, REALM_BRITANNIA},
    {5703, 1320,   16, REALM_BRITANNIA, 5788, 1337,   -5, REALM_BRITANNIA},
    {5786, 1336,   -8, REALM_BRITANNIA, 5701, 1319,   13, REALM_BRITANNIA},
    {5787, 1336,   -8, REALM_BRITANNIA, 5702, 1319,   13, REALM_BRITANNIA},
    {5788, 1336,   -8, REALM_BRITANNIA, 5703, 1319,   13, REALM_BRITANNIA},

    // Haus mit Keller
    {5691, 1347,    0, REALM_BRITANNIA, 5691, 1352,  -27, REALM_BRITANNIA},
    {5691, 1351,  -27, REALM_BRITANNIA, 5691, 1346,    0, REALM_BRITANNIA},

    // noch ein Eingang?
//  {5686, 1424, -255, REALM_BRITANNIA, 2923, 3406,    9, REALM_BRITANNIA},
//  {5687, 1424, -255, REALM_BRITANNIA, 2923, 3406,    9, REALM_BRITANNIA},
//  {5688, 1424, -255, REALM_BRITANNIA, 2923, 3406,    9, REALM_BRITANNIA},

    // fehlt hier ein Gang?
//  {5672, 1415,   70, REALM_BRITANNIA,    0,    0,    0, REALM_BRITANNIA},
//  {5672, 1416,   70, REALM_BRITANNIA,    0,    0,    0, REALM_BRITANNIA},
//  {5672, 1417,   70, REALM_BRITANNIA,    0,    0,    0, REALM_BRITANNIA},
    // }}}

    // Heckenlabyrinth {{{
    // Huegel <-> Villenkeller
    {1144, 2228,   20, REALM_BRITANNIA, 1141, 2228,   20, REALM_BRITANNIA},
    {1142, 2228,   20, REALM_BRITANNIA, 1145, 2228,   20, REALM_BRITANNIA},
    // Villenkeller -> Turmkeller
    {1133, 2230,   20, REALM_BRITANNIA, 1163, 2208,    0, REALM_BRITANNIA},
    // Turmkammer -> Turmdach
    {1165, 2204,   40, REALM_BRITANNIA, 1163, 2207,   60, REALM_BRITANNIA},
    // Turmdach -> Pilzkreis
    {1163, 2208,   60, REALM_BRITANNIA, 1171, 2202,    0, REALM_BRITANNIA},
    // }}}

    // Shame {{{
    // Shame Ein-/Ausgang
    { 512, 1559, -255, REALM_BRITANNIA, 5394,  126,    0, REALM_BRITANNIA},
    { 513, 1559, -255, REALM_BRITANNIA, 5395,  126,    0, REALM_BRITANNIA},
    { 514, 1559, -255, REALM_BRITANNIA, 5396,  126,    0, REALM_BRITANNIA},
    {5394,  127, -255, REALM_BRITANNIA,  512, 1560,    1, REALM_BRITANNIA},
    {5395,  127, -255, REALM_BRITANNIA,  513, 1560,    1, REALM_BRITANNIA},
    {5396,  127, -255, REALM_BRITANNIA,  514, 1560,    1, REALM_BRITANNIA},

    // Shame Teil 1 <-> Teil 2
    {5514,   10, -255, REALM_BRITANNIA, 5489,   19,  -25, REALM_BRITANNIA},
    {5490,   19, -255, REALM_BRITANNIA, 5515,   11,    5, REALM_BRITANNIA},

    // Shame Teil 2 <-> Teil 3
    {5604,  102,    5, REALM_BRITANNIA, 5514,  146,   25, REALM_BRITANNIA},
    {5514,  147,   25, REALM_BRITANNIA, 5604,  101,    5, REALM_BRITANNIA},

    // Shame Teil 3 <-> Teil 4 (kleines Gemaeuer)
    {5513,  176,    5, REALM_BRITANNIA, 5539,  170,    5, REALM_BRITANNIA},
    {5538,  170,    5, REALM_BRITANNIA, 5514,  176,    5, REALM_BRITANNIA},

    // Shame Teil 4 <-> Teil 5
    {5507,  162,    5, REALM_BRITANNIA, 5875,   20,   -5, REALM_BRITANNIA},
    {5875,   19,   -5, REALM_BRITANNIA, 5507,  163,    5, REALM_BRITANNIA},
    // }}}

    // Sumpf-/Abwasser-Dungeon {{{
    // Bogenfoermige Hoehle <-> hoppers_bog
    {6074, 3332,    4, REALM_BRITANNIA, 6126, 1409,   13, REALM_BRITANNIA},
    {6075, 3332,    4, REALM_BRITANNIA, 6127, 1409,   13, REALM_BRITANNIA},
    {6126, 1410,   15, REALM_BRITANNIA, 6074, 3333,    2, REALM_BRITANNIA},
    {6127, 1410,   15, REALM_BRITANNIA, 6075, 3333,    2, REALM_BRITANNIA},

    // Ein-/Ausgang <-> Bogenfoermige Hoehle
    {6137, 1409,    2, REALM_BRITANNIA, 6140, 1432,    4, REALM_BRITANNIA},
    {6138, 1409,    2, REALM_BRITANNIA, 6141, 1432,    4, REALM_BRITANNIA},
    {6140, 1431,    4, REALM_BRITANNIA, 6137, 1408,    2, REALM_BRITANNIA},
    {6141, 1431,    4, REALM_BRITANNIA, 6138, 1408,    2, REALM_BRITANNIA},

    // Abwasser-Dungeon <-> Britain
    {1491, 1640,   24, REALM_BRITANNIA, 6032, 1499,   27, REALM_BRITANNIA},
    {1491, 1642,   24, REALM_BRITANNIA, 6032, 1501,   27, REALM_BRITANNIA},
    {6031, 1499,   42, REALM_BRITANNIA, 1490, 1640,   24, REALM_BRITANNIA},
    {6031, 1501,   42, REALM_BRITANNIA, 1490, 1640,   24, REALM_BRITANNIA},
    // }}}

    // Wrong {{{
    // Ein-/Ausgang
    {2041,  215,   14, REALM_BRITANNIA, 5824,  630,    0, REALM_BRITANNIA},
    {2042,  215,   14, REALM_BRITANNIA, 5825,  630,    0, REALM_BRITANNIA},
    {2043,  215,   14, REALM_BRITANNIA, 5826,  630,    0, REALM_BRITANNIA},
    {5824,  631, -255, REALM_BRITANNIA, 2041,  216,   15, REALM_BRITANNIA},
    {5825,  631, -255, REALM_BRITANNIA, 2042,  216,   15, REALM_BRITANNIA},
    {5826,  631, -255, REALM_BRITANNIA, 2043,  216,   15, REALM_BRITANNIA},

    // Wrong Level 1 <-> Wrong Level 2
    {5826,  517,    0, REALM_BRITANNIA, 5690,  581,    0, REALM_BRITANNIA},
    {5827,  517,    0, REALM_BRITANNIA, 5691,  581,    0, REALM_BRITANNIA},
    {5828,  517,    0, REALM_BRITANNIA, 5692,  581,    0, REALM_BRITANNIA},
    {5690,  582,    0, REALM_BRITANNIA, 5826,  518,    0, REALM_BRITANNIA},
    {5691,  582,    0, REALM_BRITANNIA, 5827,  518,    0, REALM_BRITANNIA},
    {5692,  582,    0, REALM_BRITANNIA, 5828,  518,    0, REALM_BRITANNIA},

    // Wrong Level 2 -> Wrong Level 3
    {5675,  523,    0, REALM_BRITANNIA, 5715,  651,    0, REALM_BRITANNIA},

    // Wrong Level 3 -> Wrong Level 1
    {5690,  626,    0, REALM_BRITANNIA, 5861,  541,   15, REALM_BRITANNIA},

    // kaputte Bruecke ueber den Lavafluss in Wrong - nur beim Entfleuchen
    {5831,  592,   10, REALM_BRITANNIA, 5828,  592,   10, REALM_BRITANNIA},
    {5832,  593,   10, REALM_BRITANNIA, 5829,  593,   10, REALM_BRITANNIA},
    // }}}
    // }}}

    // Staedte, Siedlungen {{{
    // Buccaneers Den {{{
    // von/nach Vesper Magierpentagramme
    {2914,  675,   40, REALM_BRITANNIA, 2751, 2166,   -2, REALM_BRITANNIA,
      // GATE_SPC_NEWBIES, GATE_SPC_NOMOUNT, GATE_SPC_NAKED
      struct{j := 1, m := 1, n := 1}
    },
    {2751, 2157,   -2, REALM_BRITANNIA, 2923,  675,   40, REALM_BRITANNIA,
      // GATE_SPC_NEWBIES
      struct{j := 1}
    },
    // von/nach Jhe'lom
    {1417, 3881,    0, REALM_BRITANNIA, 2751, 2166,   -2, REALM_BRITANNIA,
      // GATE_SPC_NONNEWBIES, GATE_SPC_NOMOUNT, GATE_SPC_NAKED
      struct{f := 1, m := 1, n := 1}
    },
    {2751, 2176,   -2, REALM_BRITANNIA, 1418, 3881,    0, REALM_BRITANNIA,
      // GATE_SPC_NONNEWBIES
      struct{f := 1}
    },

    // kaputte Treppe in Taverne
    {2676, 2241,  -18, REALM_BRITANNIA, 2675, 2241,    2, REALM_BRITANNIA},
    {2676, 2242,  -18, REALM_BRITANNIA, 2675, 2242,    2, REALM_BRITANNIA},
    {2676, 2241,    2, REALM_BRITANNIA, 2677, 2242,  -20, REALM_BRITANNIA},

    // Eingaenge
    // Wald West
    {2611, 2279,    9, REALM_BRITANNIA, 2611, 2279,  -20, REALM_BRITANNIA},

    // Kueste Nordost
    {2758, 2056,   29, REALM_BRITANNIA, 2758, 2056,    0, REALM_BRITANNIA},

    // Kueste Ost
    {2770, 2098,    8, REALM_BRITANNIA, 2770, 2098,  -12, REALM_BRITANNIA},

    // Wald Ost
    {2863, 2246,    5, REALM_BRITANNIA, 2863, 2246,  -15, REALM_BRITANNIA},

    // Ausgaenge
    // Gaenge -> Schreibstube
    {2649, 2195,  -14, REALM_BRITANNIA, 2649, 2194,    4, REALM_BRITANNIA},

    // kaputte Treppe
    {2669, 2072,  -20, REALM_BRITANNIA, 2669, 2072,    5, REALM_BRITANNIA},
    {2669, 2073,  -20, REALM_BRITANNIA, 2669, 2073,    5, REALM_BRITANNIA},
    // }}}

    // Goetterinsel {{{
    // Festhalle rauf
    {5252,  346,   90, REALM_BRITANNIA, 5248,  341,  110, REALM_BRITANNIA},
    // Festhalle runter
    {5248,  350,  110, REALM_BRITANNIA, 5252,  339,   90, REALM_BRITANNIA},

    // Goetterinsel <-> Knast
    {5183,  338,   52, REALM_BRITANNIA, 5273, 1173,   12, REALM_BRITANNIA},
    {5183,  339,   52, REALM_BRITANNIA, 5273, 1174,   12, REALM_BRITANNIA},
    {5183,  340,   52, REALM_BRITANNIA, 5273, 1175,   12, REALM_BRITANNIA},
    {5272, 1173,   17, REALM_BRITANNIA, 5182,  338,   57, REALM_BRITANNIA},
    {5272, 1174,   17, REALM_BRITANNIA, 5182,  339,   57, REALM_BRITANNIA},
    {5272, 1175,   17, REALM_BRITANNIA, 5182,  340,   57, REALM_BRITANNIA},

    // Gehen jetzt alle auf Gerichtsinsel -- siehe unten
    // Goetterinsel <-> Beratungsraum
    // {5198,  330,   60, REALM_BRITANNIA, 5283, 1104,    0, REALM_BRITANNIA},
    // {5282, 1103, -255, REALM_BRITANNIA, 5199,  331,   60, REALM_BRITANNIA},
    // }}}

    // Gerichte {{{
    // Göttergericht <-> Beratungsinsel
    {5198,  330,   60, REALM_BRITANNIA, 4821,  600,    2, REALM_BRITANNIA},
    {4821,  599,    2, REALM_BRITANNIA, 5198,  331,   60, REALM_BRITANNIA},

    // Cove (Rathaus) <-> Beratungsinsel
    {2199, 1183,    0, REALM_BRITANNIA, 4816,  600,    2, REALM_BRITANNIA},
    {4816,  599,    2, REALM_BRITANNIA, 2205, 1186,    0, REALM_BRITANNIA},

    // Gericht Minoc <-> Beratungsinsel
    {2416,  521,    0, REALM_BRITANNIA, 4816,  606,    2, REALM_BRITANNIA},
    {4816,  607,    2, REALM_BRITANNIA, 2416,  523,    0, REALM_BRITANNIA},

    // Vesper hat noch kein Gericht (Insel: 4811 599 2)

    // Gericht Yew <-> Beratungsinsel
    { 360,  893,   0, REALM_BRITANNIA,  4806,  600,    2, REALM_BRITANNIA},
    {4806,  599,   2, REALM_BRITANNIA,   360,  895,    0, REALM_BRITANNIA},

    // Gericht Jhe'lom <-> Beratungsinsel
    {1418, 3696,   1, REALM_BRITANNIA,  4806,  606,    2, REALM_BRITANNIA},
    {4806,  607,   2, REALM_BRITANNIA,  1418, 3698,    1, REALM_BRITANNIA},

    // Gericht Wind <-> Beratungsinsel
//  {3694, 1245,   0, REALM_BRITANNIA,  4811,  606,    2, REALM_BRITANNIA},
//  {4811,  607,   2, REALM_BRITANNIA,  3694, 1245,    0, REALM_BRITANNIA},
    // }}}

    // // Jhe'lom alt {{{
    // // Jhe'lom <-> Jhe'lom Nord
    // {1409, 3824, -255, REALM_BRITANNIA, 1124, 3623,    6, REALM_BRITANNIA},
    // {1142, 3621, -255, REALM_BRITANNIA, 1414, 3828,    6, REALM_BRITANNIA},

    // // Jhe'lom <-> Jhe'lom Sued
    // {1419, 3832, -255, REALM_BRITANNIA, 1466, 4015,    6, REALM_BRITANNIA},
    // {1406, 3996, -255, REALM_BRITANNIA, 1414, 3828,    6, REALM_BRITANNIA},

    // // Jhe'lom <-> Jhe'lom Kampfinsel
    // {1409, 3832,    5, REALM_BRITANNIA, 1081, 3140,    0, REALM_BRITANNIA,
    //   // GATE_SPC_NONNEWBIES und GATE_SPC_ASK
    //   struct{f := 1, a := 1}
    // },
    // {1037, 3171,    5, REALM_BRITANNIA, 1414, 3828,    6, REALM_BRITANNIA},

    // // Jhe'lom <-> Zoo
    // {1419, 3824,    5, REALM_BRITANNIA, 1266,  513,   35, REALM_BRITANNIA,
    //   // GATE_SPC_NONNEWBIES
    //   struct{f := 1}
    // },
    // {1266,  514,   35, REALM_BRITANNIA, 1414, 3828,    6, REALM_BRITANNIA,
    //   // GATE_SPC_NONNEWBIES
    //   struct{f := 1}
    // },
    // // }}}

    // Jhe'lom {{{
    // Jhe'lom <-> Nordinsel
    {1387, 3810,    5, REALM_BRITANNIA, 1126, 3624,    5, REALM_BRITANNIA},
    {1138, 3624,    5, REALM_BRITANNIA, 1392, 3815,    5, REALM_BRITANNIA},

    // Jhe'lom <-> Suedinsel
    {1397, 3820,    5, REALM_BRITANNIA, 1415, 3987,    5, REALM_BRITANNIA},
    {1419, 3970,    5, REALM_BRITANNIA, 1392, 3815,    5, REALM_BRITANNIA},

    // Jhe'lom <-> Kampfinsel
    {1387, 3820,    5, REALM_BRITANNIA, 1082, 3140,    5, REALM_BRITANNIA,
      // GATE_SPC_NONNEWBIES und GATE_SPC_ASK
      struct{f := 1, a := 1}
    },
    {1039, 3176,    5, REALM_BRITANNIA, 1392, 3815,    5, REALM_BRITANNIA},

    // Jhe'lom <-> Zoo
    {1397, 3810,    5, REALM_BRITANNIA, 1266,  513,   35, REALM_BRITANNIA,
      // GATE_SPC_NONNEWBIES
      struct{f := 1}
    },
    {1266,  514,   35, REALM_BRITANNIA, 1392, 3815,    5, REALM_BRITANNIA,
      // GATE_SPC_NONNEWBIES
      struct{f := 1}
    },
    // }}}

    // Wind {{{
    // Schreine {{{
    // Feuer
    {5153,  195,   -3, REALM_BRITANNIA, 5178,  243,   32, REALM_BRITANNIA},
    {5153,  196,   -3, REALM_BRITANNIA, 5178,  244,   32, REALM_BRITANNIA},
    {5153,  197,   -3, REALM_BRITANNIA, 5178,  245,   32, REALM_BRITANNIA},
    {5179,  243,   37, REALM_BRITANNIA, 5154,  195,    2, REALM_BRITANNIA},
    {5179,  244,   37, REALM_BRITANNIA, 5154,  196,    2, REALM_BRITANNIA},
    {5179,  245,   37, REALM_BRITANNIA, 5154,  197,    2, REALM_BRITANNIA},

    // Erde
    {5202,  182,  -18, REALM_BRITANNIA, 5170,  278,   22, REALM_BRITANNIA},
    {5203,  182,  -18, REALM_BRITANNIA, 5171,  278,   22, REALM_BRITANNIA},
    {5204,  182,  -18, REALM_BRITANNIA, 5172,  278,   22, REALM_BRITANNIA},
    {5170,  279,   27, REALM_BRITANNIA, 5202,  183,  -13, REALM_BRITANNIA},
    {5171,  279,   27, REALM_BRITANNIA, 5203,  183,  -13, REALM_BRITANNIA},
    {5172,  279,   27, REALM_BRITANNIA, 5204,  183,  -13, REALM_BRITANNIA},

    // Wasser
    {5185,  161,  -17, REALM_BRITANNIA, 5147,  276,   23, REALM_BRITANNIA},
    {5186,  161,  -17, REALM_BRITANNIA, 5148,  276,   23, REALM_BRITANNIA},
    {5147,  277,   28, REALM_BRITANNIA, 5185,  162,  -12, REALM_BRITANNIA},
    {5148,  277,   28, REALM_BRITANNIA, 5186,  162,  -12, REALM_BRITANNIA},
    {5149,  277,   28, REALM_BRITANNIA, 5186,  162,  -12, REALM_BRITANNIA},

    // Luft
    {5155,  170,    8, REALM_BRITANNIA, 5152,  244,   16, REALM_BRITANNIA},
    {5155,  171,    8, REALM_BRITANNIA, 5152,  245,   16, REALM_BRITANNIA},
    {5153,  244,   16, REALM_BRITANNIA, 5156,  170,    8, REALM_BRITANNIA},
    {5153,  245,   16, REALM_BRITANNIA, 5156,  171,    8, REALM_BRITANNIA},
    // }}}

    // Magierturm: Fallgrube -> Gang
    {2181, 1987,    0, REALM_BRITANNIA, 2186, 1987,    0, REALM_BRITANNIA},
    // }}}

    // Yew {{{
    // Friedhof Yew <-> Gruft
    { 722, 1117,   -8, REALM_BRITANNIA, 5802,  664,   12, REALM_BRITANNIA},
    { 723, 1117,   -8, REALM_BRITANNIA, 5802,  665,   12, REALM_BRITANNIA},
    { 724, 1117,   -8, REALM_BRITANNIA, 5802,  666,   12, REALM_BRITANNIA},
    { 725, 1117,   -8, REALM_BRITANNIA, 5802,  666,   12, REALM_BRITANNIA},
    {5801,  664,   12, REALM_BRITANNIA,  722, 1118,   -8, REALM_BRITANNIA},
    {5801,  665,   12, REALM_BRITANNIA,  723, 1118,   -8, REALM_BRITANNIA},
    {5801,  666,   12, REALM_BRITANNIA,  723, 1118,   -8, REALM_BRITANNIA},

    // Yew Trutzburg -> Yew Rathaus
    { 330,  770,  -20, REALM_BRITANNIA,  621,  809,    0, REALM_BRITANNIA},

    // Yew <-> Hoehle -- gibts net mehr
//  { 312,  786, -255, REALM_BRITANNIA, 5750,  360,    0, REALM_BRITANNIA},
//  { 313,  786, -255, REALM_BRITANNIA, 5749,  360,    1, REALM_BRITANNIA},
//  { 314,  786, -255, REALM_BRITANNIA, 5748,  360,    2, REALM_BRITANNIA},
//  {5748,  361,    2, REALM_BRITANNIA,  312,  787,  232, REALM_BRITANNIA},
//  {5749,  361,    2, REALM_BRITANNIA,  313,  787,  232, REALM_BRITANNIA},
//  {5750,  361,    4, REALM_BRITANNIA,  314,  787,  232, REALM_BRITANNIA},
    // }}}
    // }}}

    // kleinere Bauwerke, Einzelbauten {{{
    // Gildenhaeuser {{{
    {1906,  406,   -1, REALM_BRITANNIA, 5184, 1131,    0, REALM_BRITANNIA}, // Gildenhaus Drachensekte -> Drachenraum
    {5183, 1111,    0, REALM_BRITANNIA, 1906,  406,   21, REALM_BRITANNIA}, // Drachenraum -> Gildenhaus Drachensekte Felsenbau
    {5184, 1111,    0, REALM_BRITANNIA, 1906,  405,   21, REALM_BRITANNIA}, // Drachenraum -> Gildenhaus Drachensekte Felsenbau

    {3470,  274,    0, REALM_BRITANNIA, 3484,  275,  -50, REALM_BRITANNIA}, // Gildehaus OdN oben -> Keller
    {3469,  275,  -50, REALM_BRITANNIA, 3483,  273,    0, REALM_BRITANNIA}, // Keller -> Gildehaus OdN oben

    {5185, 2084,    0, REALM_BRITANNIA, 2777,  894,  -23, REALM_BRITANNIA}, // Gildehaus SIO -> Vesper Friedhof
    {5185, 2085,    0, REALM_BRITANNIA, 2777,  894,  -23, REALM_BRITANNIA}, // Gildehaus SIO -> Vesper Friedhof
    {5185, 2086,    0, REALM_BRITANNIA, 2777,  894,  -23, REALM_BRITANNIA}, // Gildehaus SIO -> Vesper Friedhof
    {5185, 2087,    0, REALM_BRITANNIA, 2777,  894,  -23, REALM_BRITANNIA}, // Gildehaus SIO -> Vesper Friedhof
    {2776,  895,  -23, REALM_BRITANNIA, 5183, 2086,   -1, REALM_BRITANNIA}, // Vesper Friedhof -> Gildehaus SIO
    // }}}

    // Hidden {{{
    // Erdgeschoss <-> Keller
    {1677, 2987,    0, REALM_BRITANNIA, 1675, 2986,   20, REALM_BRITANNIA},
    {1675, 2987,   20, REALM_BRITANNIA, 1676, 2986,    0, REALM_BRITANNIA},
    // }}}

    // Schrein "Spirit" {{{
    // Eingang
    {1600, 2489,   10, REALM_BRITANNIA, 1595, 2489,   20, REALM_BRITANNIA},
    {1600, 2490,   10, REALM_BRITANNIA, 1595, 2490,   20, REALM_BRITANNIA},
    // Ausgang
    {1592, 2489,   17, REALM_BRITANNIA, 1601, 2489,    7, REALM_BRITANNIA},
    {1592, 2490,   17, REALM_BRITANNIA, 1601, 2490,    7, REALM_BRITANNIA},
    // }}}

    // Terathan-Burg(?) // {{{
    // Burg <-> Keller
    {5340, 1598,   47, REALM_BRITANNIA, 5426, 3121,  -65, REALM_BRITANNIA},
    {5341, 1598,   47, REALM_BRITANNIA, 5426, 3121,  -65, REALM_BRITANNIA},
    {5426, 3122,  -74, REALM_BRITANNIA, 5340, 1599,   41, REALM_BRITANNIA},
    // }}}
    // }}}

    // Aussenposten <-> Zoo
    // FIXME: inzwischen neben Plattform
    // {2067,  799,    0, REALM_BRITANNIA, 1266,  513,   35, REALM_BRITANNIA,
    //   // GATE_SPC_NONNEWBIES
    //   struct{j := 1}
    // },
    // {1266,  512,   35, REALM_BRITANNIA, 2067,  800,    0, REALM_BRITANNIA,
    //   // GATE_SPC_NONNEWBIES
    //   struct{j := 1}
    // }

    // Malas/DynDung
    // kleine Hoehle <-> DynDungeon
    {5850,  431,    2, REALM_BRITANNIA,   13,   25,    7, REALM_MALAS},
    {5850,  432,    0, REALM_BRITANNIA,   13,   26,    6, REALM_MALAS},
    {5850,  433,   -2, REALM_BRITANNIA,   13,   26,    6, REALM_MALAS},
    {5850,  434,   -1, REALM_BRITANNIA,   13,   27,    4, REALM_MALAS},
    {  12,   25,    5, REALM_MALAS,     5849,  431,    1, REALM_BRITANNIA},
    {  12,   26,    3, REALM_MALAS,     5849,  432,    1, REALM_BRITANNIA},
    {  12,   27,    3, REALM_MALAS,     5849,  434,   -1, REALM_BRITANNIA},

    // geplantes: kleine Hoehle <-> Bereich mit Moongate
    // kleine Hoehle <-> Teleporterinsel
    {5826,  465,   -1, REALM_BRITANNIA, 5333,  398,    0, REALM_BRITANNIA},
    {5827,  465,   -1, REALM_BRITANNIA, 5334,  398,    0, REALM_BRITANNIA},
    {5828,  465,    0, REALM_BRITANNIA, 5335,  398,    0, REALM_BRITANNIA},
    {5333,  397,    0, REALM_BRITANNIA, 5826,  464,   -1, REALM_BRITANNIA},
    {5334,  397,    0, REALM_BRITANNIA, 5827,  464,   -1, REALM_BRITANNIA}
  };

  var teleporters_pergon := {
    // alter Zugang: Alter Turm <-> Magierturm
    {2252,  513,    0, REALM_BRITANNIA, 2182, 1996,    5, REALM_BRITANNIA},
    {2201, 1980,    5, REALM_BRITANNIA, 2252,  512,    0, REALM_BRITANNIA},
    // Zeitweilig fuer Heilerkreis/Heiligtum
    {1122, 1242,   35, REALM_BRITANNIA, 2141,  833,    5, REALM_BRITANNIA},
    {1148, 1219,   35, REALM_BRITANNIA, 2142,  832,    5, REALM_BRITANNIA},
    {2142,  833,    5, REALM_BRITANNIA, 1123, 1242,   35, REALM_BRITANNIA}
  };

  var teleporters_bau := {
    // Wind {{{
    // Zugang Felsenlabyrinth <-> Wind
    {1361,  883,    0, REALM_BRITANNIA, 5244,  243,    0, REALM_BRITANNIA},
    {5244,  244,    0, REALM_BRITANNIA, 1361,  884,    0, REALM_BRITANNIA},

    // Zwischenstufe je nach Magierichtungen {{{
    // Turm im Magierteil 1
    {5204,  211,    5, REALM_BRITANNIA, 5205,  211,   28, REALM_BRITANNIA},
    {5204,  211,   28, REALM_BRITANNIA, 5205,  211,    5, REALM_BRITANNIA},
    // Magierteil 1 <-> Magierturm
    {5160,  129,   23, REALM_BRITANNIA, 2182, 1996,    5, REALM_BRITANNIA},
    {2181, 1996,    5, REALM_BRITANNIA, 5160,  130,   23, REALM_BRITANNIA},
    // Magierturm <-> Magierteil 2
    {2201, 1980,    5, REALM_BRITANNIA, 5138,  100,    7, REALM_BRITANNIA},
    {5137,  100,    7, REALM_BRITANNIA, 2201, 1981,    5, REALM_BRITANNIA},

    // Nekromantenteil 1 <-> Wrong (Pyramide)
    {5250,  180,    9, REALM_BRITANNIA, 5725,  560,   30, REALM_BRITANNIA},
    {5730,  565,   20, REALM_BRITANNIA, 5250,  181,    9, REALM_BRITANNIA},
    // Sinnlos-Teleporter in Pyramide {{{
    // Balkon Obernekro
    {5720,  555,   20, REALM_BRITANNIA, 5713,  649,   22, REALM_BRITANNIA},
    {5714,  650,   22, REALM_BRITANNIA, 5725,  560,   30, REALM_BRITANNIA},
    // Zelle bei Statuenausstellung
    {5720,  565,   20, REALM_BRITANNIA, 5666,  556,   20, REALM_BRITANNIA},
    {5666,  555,   20, REALM_BRITANNIA, 5725,  560,   30, REALM_BRITANNIA},
    // TODO: noch nichts festgelegt, erst einmal Zentralpunkt Pyramide
    {5730,  555,   20, REALM_BRITANNIA, 5725,  560,   30, REALM_BRITANNIA},
//  {   0,    0,    0, REALM_BRITANNIA, 5725,  560,   30, REALM_BRITANNIA},
    // }}}
    // Wrong <-> Nekromantenteil (TODO: nach Festmachen Z-Werte pruefen)
    {5867,  524,   35, REALM_BRITANNIA, 5248,  160,  -10, REALM_BRITANNIA},
    {5868,  524,   35, REALM_BRITANNIA, 5249,  160,  -10, REALM_BRITANNIA},
    {5869,  524,   35, REALM_BRITANNIA, 5250,  160,  -10, REALM_BRITANNIA},
    {5248,  161,  -15, REALM_BRITANNIA, 5867,  525,   35, REALM_BRITANNIA},
    {5249,  161,  -15, REALM_BRITANNIA, 5868,  525,   35, REALM_BRITANNIA},
    {5250,  161,  -15, REALM_BRITANNIA, 5869,  525,   35, REALM_BRITANNIA},

    // Priesterteil 1 <-> Heilerkreis
    {5371,  129,   32, REALM_BRITANNIA, 1123, 1241,   35, REALM_BRITANNIA},
    {5371,  130,   32, REALM_BRITANNIA, 1123, 1242,   35, REALM_BRITANNIA},
    {5371,  131,   32, REALM_BRITANNIA, 1123, 1243,   35, REALM_BRITANNIA},
    {1122, 1242,   35, REALM_BRITANNIA, 5370,  130,   30, REALM_BRITANNIA},
    // Heilerkreis <-> Priesterteil 2
    {1148, 1219,   35, REALM_BRITANNIA, 5356,  112,   12, REALM_BRITANNIA},
    {5355,  113,   15, REALM_BRITANNIA, 1148, 1220,   35, REALM_BRITANNIA},
    {5356,  113,   15, REALM_BRITANNIA, 1149, 1220,   35, REALM_BRITANNIA},
    // }}}

    // Magische Arena - SeBB {{{
    // Wind Arena Treppe -> Nordwestecke
    {5248,  118,   20, REALM_BRITANNIA, 5237,  107,   24, REALM_BRITANNIA},
    // Wind Arena Treppe -> Nordostecke
    {5249,  118,   20, REALM_BRITANNIA, 5261,  107,   24, REALM_BRITANNIA},
    // Wind Arena Treppe -> Suedostecke
    {5249,  119,   20, REALM_BRITANNIA, 5261,  131,   24, REALM_BRITANNIA},
    // Wind Arena Treppe -> Suedostecke
    {5248,  119,   20, REALM_BRITANNIA, 5237,  131,   24, REALM_BRITANNIA},
    // Wind Arena Nordseite -> Fuss Treppe Nord
    {5249,  107,   54, REALM_BRITANNIA, 5249,  114,    0, REALM_BRITANNIA},
    // Wind Arena Ostseite  -> Fuss Treppe Ost
    {5261,  119,   54, REALM_BRITANNIA, 5254,  119,    0, REALM_BRITANNIA},
    // Wind Arena Suedseite -> Fuss Treppe Sued
    {5249,  131,   54, REALM_BRITANNIA, 5249,  124,    0, REALM_BRITANNIA},
    // Wind Arena Westseite -> Fuss Treppe West
    {5237,  119,   54, REALM_BRITANNIA, 5244,  119,    0, REALM_BRITANNIA}
    // }}}
    // }}}
  };

  If (GetGlobalProperty(PERGON))
    teleporters += teleporters_pergon;
  EndIf
  If (GetGlobalProperty(BAUSERVER))
    teleporters += teleporters_bau;
  EndIf
EndFunction

// Die Systemteleporter erzeugen (werden nie gespeichert...)
Function CreateTeleporters()
  var teleporters := array{};
  GetTeleporters(teleporters);

  var nbrofteleporters:=0; // Wieviele Teleporter wurden erzeugt?

  If (DEBUG)
    syslog("Teleporter erzeugen");
  EndIf

  // Erstmal alle hier definierten Teleporter ...
  ForEach tele in teleporters
    If (!Is_Critical())
      SleepMs(2);
    EndIf
    var from_x     := tele[1]; // Quelle
    var from_y     := tele[2];
    var from_z     := tele[3];
    var from_realm := tele[4];
    If (from_z == -255) // Die Z-Koordinate selber suchen?
      from_z := GetWorldHeight(from_x, from_y, from_realm);
    EndIf

    var to_x     := tele[5]; // Ziel
    var to_y     := tele[6];
    var to_z     := tele[7];
    var to_realm := tele[8];
    If (TypeOfInt(to_x) == OT_ARRAY) // Teleport zu mehreren moeglichen Zielen?
      If ((TypeOfInt(to_y) == OT_ARRAY) And (TypeOfInt(to_z) == OT_ARRAY))
        If ((to_x.size() <> to_y.size()) Or (to_x.size() <> to_z.size()))
          syslog(
            "FEHLER: Der Eintrag "+tele+" ist fehlerhaft "+
            "[to_x, to_y und to_z sind unterschiedlich gross]!"
          );
          // weiter mit naechstem Teleporter
          continue;
        EndIf
      Else
        syslog(
          "FEHLER: Der Eintrag "+tele+" ist fehlerhaft "+
          "[to_y oder to_z sind kein Array]!"
        );
        // weiter mit naechstem Teleporter
        continue;
      EndIf
    ElseIf (to_z == -255)
      // Die Z-Koordinate selber suchen? (Funzt nicht bei Arrays ...)
      to_z := GetWorldHeight(to_x, to_y, to_realm);
    EndIf

    // Eintraege erfolgreich geprueft, Teleport erzeugen ...
    var teletemplate := GetItemDescriptor(UOBJ_SYSTELEPORTER);
    teletemplate["CProps"] := dictionary{
      // Zielkoordinaten festlegen
      "DestX"     -> to_x,
      "DestY"     -> to_y,
      "DestZ"     -> to_z,
      "DestRealm" -> to_realm
    };
    If (GetGlobalProperty(BAUSERVER))
      // auf Bauserver zwecks leichterem Debugging sichtbare Grafik
      // (aber noch invisible)
      teletemplate["graphic"] := 0x1fd4;
    EndIf

    // ggf. Spezialoptionen setzen
    var special_opts := tele[9];
    If (special_opts and TypeOfInt(special_opts) == OT_STRUCT)
      teletemplate["CProps"].insert(PROP_GATE_SPECIAL, special_opts);
    EndIf

    var teleporter := CreateItemAtLocationPergon(
      from_x, from_y, from_z, teletemplate, 1, from_realm
    );
    If (teleporter)
      nbrofteleporters += 1;
      If (DEBUG)
        syslog(
          "  Teleporter von ["+from_x+", "+from_y+", "+from_z+"; "+
          from_realm+"] nach ["+to_x+", "+to_y+", "+to_z+"; "+to_realm+"]"
        );
      EndIf
    Else
      syslog(
        "FEHLER: Include-Teleporter bei "+from_x+", "+from_y+", "+from_z+
        ", "+from_realm+" konnte nicht erzeugt werden:"+
        teleporter.errortext+"!"
      );
    EndIf
  EndForEach

  // wird derzeit nicht benutzt
  // var cfg:=ReadConfigFile("teleporters"); // ...und jetzt noch die aus dem Config-File
  // If (cfg)
  //   For i:=1 To GetConfigMaxIntKey(cfg) // Sofern die alle durchnummeriert sind, funzt das
  //     var from_x:=cfg[i].fromx; // Quelle
  //     var from_y:=cfg[i].fromy;
  //     var from_z:=cfg[i].fromz;
  //     var from_realm:=cfg[i].fromrealm;
  //     If (!from_z)                // Die Z-Koordinate selber suchen?
  //       from_z:=GetWorldHeight(from_x, from_y,from_realm);
  //       If (DEBUG)
  //         syslog("    Teleporter von "+from_x+" "+from_y+" -255 ["+from_z+"] "+from_realm);
  //       EndIf
  //     Else
  //       If (DEBUG)
  //         syslog("    Teleporter von "+from_x+" "+from_y+" "+from_z+" "+from_realm);
  //       EndIf
  //     EndIf

  //     var to_x:=cfg[i].tox; // Ziel
  //     var to_y:=cfg[i].toy;
  //     var to_z:=cfg[i].toz;
  //     var to_realm:=cfg[i].torealm;
  //     If (to_z==-255)          // Die Z-Koordinate selber suchen? (Funzt nicht bei Arrays...)
  //       to_z:=GetWorldHeight(to_x, to_y,to_realm);
  //       If (DEBUG)
  //         syslog("      nach "+to_x+" "+to_y+" -255 ["+to_z+"] "+to_realm);
  //       EndIf
  //     ElseIf (to_z==-255)       // Die Z-Koordinate selber suchen? (Funzt nicht bei Arrays...)
  //       If (DEBUG)
  //         syslog("      nach "+to_x+" "+to_y+" "+to_z+" "+to_realm);
  //       EndIf
  //     EndIf

  //     var teleporter:=CreateItemAtLocationPergon(from_x, from_y, from_z, UOBJ_SYSTELEPORTER, 1, from_realm);
  //     If (teleporter)
  //       SetObjProperty(teleporter, "DestX", to_x); // Zielkoordinaten festlegen
  //       SetObjProperty(teleporter, "DestY", to_y);
  //       SetObjProperty(teleporter, "DestZ", to_z);
  //       SetObjProperty(teleporter, "DestRealm", to_realm);
  //       nbrofteleporters+=1;
  //     Else
  //       syslog("FEHLER: Config-Teleporter bei "+from_x+", "+from_y+", "+from_z+", "+from_realm+" konnte nicht erzeugt werden ("+teleporter.errortext+") !");
  //     EndIf
  //   EndFor
  // Else
  //   syslog("FEHLER: Die 'teleporters.cfg' konnte nicht geoeffnet werden!");
  // EndIf

  syslog(nbrofteleporters+ " Teleporter wurden erzeugt.");
EndFunction

// DestroyTeleporters - Die Systemteleporter zerstoeren
Function DestroyTeleporters(type := UOBJ_SYSTELEPORTER)
  // Alle Realms durchgehen
  ForEach realm in Realms()
    // Die gesamte Welt abgrasen
    ForEach item in ListObjectsInBox(
      0, 0, -128, realm.width, realm.height, 127, _realm_iter
    )
      If (!Is_Critical())
        SleepMs(2);
      EndIf
      If (item.isa(POLCLASS_ITEM))
        If (item.objtype==type)
          DestroyItem(item);
        EndIf
      EndIf
    EndForEach
  EndForEach
EndFunction

// vim: sw=2 sts=2
