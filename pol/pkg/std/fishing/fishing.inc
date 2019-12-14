////////////////////////////////////////////////////////////////////////////
// fishing.inc defines the constants that are unique to the fishing skill
// It is intended to be left in the pkg folder to keep these constants with
// the source files for the scripts
///////////////////////////////////////////////////////////////////////////


// animation number for fishing
Const UACTION_FISHING := 0x9;

// splash sound effect
Const SFX_SKILL_FISHING := 0x0240;

// target radius (in tiles)
// was 6 while fishing in squares, changed to 8 because the area of a
// square is approx. factor 4/pi (= 1.273...) times the area of a circle
// which multiplies to 7.8...
Const FISHING_RANGE := 8;

// raw skill point gain per success
// Const FISHING_GAIN_AMOUNT   :=    100;

// this is used with harvest difficulty to calculate skill gain
Const GAIN_MULTIPLIER := 10;

// this is the percentage of getting a misc treasure when fishing an SOS
Const MISC_SOS_TREASURE_PCT := 75;

// Skill points awarded for a successful SOS fishing attempt
Const SOS_SKILL_POINTS := 500;

// Skill level to check SOS attempts against
Const SOS_SKILL_THRESHOLD := 80;

// the number of unique SOS messages currently supported
Const MSG_COUNT := 4;

// chance to catch anything bad (0..100)
Const CHANCE_NOFISH := 5;

// objectype constants
Const UOBJ_TREASURE_CHEST   := 0x0E40;  // treasure chest
Const UOBJ_SOS_BOTTLE       := 0xA360;  // SOS bottle
Const UOBJ_SOS_MESSAGE      := 0xA363;  // SOS message


///////////////////////////////////////////////////////////////////////////
// These functions define the skill level at which a certain type of
// object can be caught while fishing
Function GoodCatches()
    var res := dictionary;

    // normale Fische
    res.insert(
        5,
        struct{
            text := "Ihr fangt einen grossen Fisch.",
            what := array{0x09cc, 0x09cd, 0x09ce, 0x09cf},
            freq := 8 // Haeufigkeit
        }
    );
    // Seegras
    res.insert(
        30,
        struct{
            text := "Ihr findet etwas Seegras.",
            what := array{0x0dba}, // , 0x0dbb},
            freq := 1,
            plus := 5
        }
    );
    // Magische Fische
    res.insert(
        62,
        struct{
            text := "Ihr fangt einen magischen Fisch!",
            what := array{
                0xa368, 0xa369, 0xa36a, 0xa36b, 0xa36c, 0xa36d, 0xa36e, 0xa36f
            },
            freq := 2,
            plus := 10
        }
    );
    // Schatzkarte
    res.insert(
        72,
        struct{
            text := "Ihr findet eine alte zerfetzte Karte!",
            what := array{"level1map", "level2map", "level3map", "level4map"},
            freq := 1,
            plus := 15
        },
    );
    // SOS-Flasche
    res.insert(
        79,
        struct{
            text := "Ihr findet eine Flaschenpost!",
            what := array{0xa360},
            freq := 1,
            plus := 20
        },
    );

    return res;
EndFunction

Function BadCatches()
    var res := dictionary;

    // Schuhe
    res.insert(
        0, // min skill + tool skill
        struct{
            text := "Ihr zieht ein paar alte Schuhe an Land.",
            what := array{0x170b, 0x170f, 0x170f},
            freq := 1,
            age  := 1
        }
    );
    // Elementar
    res.insert(
        15, // min skill + tool skill
        struct {
            text  := "Ihr zieht die Aufmerksamkeit eines Wasserelementars "+
                "auf Euch!",
            what  := "lesserwaterelemental",
            freq  := 4,
            npc   := 1,
            plus  := 5
        }
    );
    // Delphin
    res.insert(
        22,
        struct {
            text  := "Ein Delphin wird neugierig auf Euch.",
            what  := "dolphin",
            freq  := 2,
            npc   := 1,
            plus  := 10,
            // nur erzeugen, wenn nicht mehr als 10 Tiere in Umgebung
            limit := 10
        }
    );
    // Walross
    res.insert(
        27,
        struct {
            text  := "Ihr scheucht ein Walross auf.",
            what  := "walrus",
            freq  := 2,
            npc   := 1,
            plus  := 15,
            // nur erzeugen, wenn nicht mehr als 10 Tiere in Umgebung
            limit := 10
        }
    );
    // Seeschlange
    res.insert(
        30,
        struct {
            text  := "Ups, das ist aber kein Fisch!",
            what  := "seaserpent",
            freq  := 4,
            npc   := 1,
            plus  := 20
        }
    );

    return res;
EndFunction

///////////////////////////////////////////////////////////////////////////
// These constants define the operation of miscellaneous SOS treasures.
// Some of these constants define the beginning and ending objtypes of the
// items that will be fished up as misc items while fishing an SOS message.
// The NUM_SOS_MISC constant reflects the number of different misc SOS
// treasure categories that are currently in use.  You must also alter the
// GetSOSTreasure function in fishing.src if you want to add new misc SOS
// item categories.
///////////////////////////////////////////////////////////////////////////

// number of misc SOS item categories
Const NUM_SOS_MISC  :=      4;
Const BEGINSHELL    := 0x0fc4;
Const ENDSHELL      := 0x0fcc;
Const BEGINART      := 0x0e9f;
Const ENDART        := 0x0eaf;
Const BEGINWIRE     := 0x1876;
Const ENDWIRE       := 0x1879;
Const BEGINPILLOW   := 0x13a4;
Const ENDPILLOW     := 0x13ae;

