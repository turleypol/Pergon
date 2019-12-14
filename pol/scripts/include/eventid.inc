// event IDs to be returned in eventid member of wait_for_event
// result.
//
// See documentation for details.
//
// Author: ens

include "include/sysevent";

// KI Events
Const EVID_ALL_FOLLOW_CMD         := 0x0011;
Const EVID_ALL_ATTACK_CMD         := 0x0012;
Const EVID_STILLFIGHT             := 0x0013;
Const EVID_LOOKAROUND             := 0x0014;
Const EVID_STILLRUN               := 0x0015;
Const EVID_ASKJOIN                := 0x0016; //fuer herden/rudeltiere
Const EVID_GRANTJOIN              := 0x0017;
Const EVID_REFUSEJOIN             := 0x0019;
Const EVID_WALK    	              := 0x001A;
Const EVID_ATTACK    	            := 0x001B; //angreifen
Const EVID_FOLLOW    	            := 0x001C; //fuer zahme tiere im followmodus damit sie schnell wieder aus dem waitforevent herauskommen
Const EVID_TU_NIX    	            := 0x001D; //fuer zahme Tiere, die zur Statue gemacht werden
Const EVID_PARKING                := 0x001F; //Angebunden
// 0x20 reserved
Const EVID_QUEST_WAYPOINT         := 0x0021;
Const EVID_QUEST_NEMESIS          := 0x0022; //für Quest-NPCs
Const EVID_AI_RESTART			        := 0x0023; //Restart Event für KIs
Const EVID_SELF_KILL			        := 0x0024; //Selbstkillevent
Const EVID_CAST_WAIT			        := 0x0025; //CAST_WAIT für KIs
Const EVID_CHANGE_TARGET          := 0x0026; //TargetChange ereignis für KI's
Const EVID_QUEST_SPEECH		        := 0x0027;
Const EVID_ROBOT_INIT             := 0x0028;
Const EVID_GUARDS_HELP            := 0x0029;
Const EVID_SPELL_ATTACK           := 0x002A;
Const EVID_PEACEMADE              := 0x002B;
//Domestizierte Viecher
Const EVID_DOMEST_EAT             := 0x002C;
Const EVID_DOMEST_DRINK           := 0x002D;
Const EVID_DOMEST_TIME            := 0x002E;
Const EVID_DOMEST_SOUND           := 0x002F;
Const EVID_DOMEST_BABY            := 0x0030;

//Steuerung der Schachki
Const EVID_CHESSCMD               := 0x0031;
// Event von death.src
Const EVID_NPC_DIED               := 0x0032;
// Boat
Const EVID_BOAT_SOUND             := 0x0033;
Const EVID_BOAT_MAP               := 0x0034;
Const EVID_BOAT_SPEECH            := 0x0035;
Const EVID_BOAT_PLACE             := 0x0036;
Const EVID_BOAT_STAY1             := 0x0037;
Const EVID_BOAT_STAY2             := 0x0038;
Const EVID_BOAT_DELAY             := 0x0039;
Const EVID_BOAT_DAMAGE            := 0x003A;
Const EVID_BOAT_GHOST             := 0x003B;
Const EVID_BOAT_ENCOUNTER         := 0x003C;
// Boot und Fähre unterschiedliche Events, damit es nicht mehrfach
// vorkommen kann
Const EVID_BOAT_SOUND1            := 0x003D;
Const EVID_BOAT_COURSE            := 0x003E;
// Turnierstein
Const EVID_TOURNAMENT_CHAR_DIED   := 0x003F;
// 0x0040 reserved
Const EVID_TOURNAMENT_RESET       := 0x0041;
// DungeonControlSkript
Const EVID_DUNGEONTIMECONTROL     := 0x0042;
Const EVID_DUNGEONBOSSFIELDAKTIV  := 0x0043;
Const EVID_DUNGEONFIELDRESET      := 0x0044;
Const EVID_DUNGEONWALKOVER        := 0x0045;
Const EVID_DUNGEONBOSSATTACKED    := 0x0046;
Const EVID_DUNGEONTELEPORT        := 0x0047;
Const EVID_DUNGEONASKFORPLAYER    := 0x0048;
// Lampen-/Ambientsoundsteine
Const EVID_MASTER_HELLO           := 0x0049;
Const EVID_MASTER_RECALL          := 0x004A;
// Spawnsystem
Const EVID_SPAWNPOINT_ADD         := 0x004B;
Const EVID_SPAWNPOINT_DEL         := 0x004C;
Const EVID_SPAWNPOINT_REFRESH     := 0x004D;
Const EVID_SPAWNPOINT_RESPAWN     := 0x004E;
Const EVID_SPAWNPOINT_LINK        := 0x004F;
// Poker
Const EVID_POKER_FOLD             := 0x0050;
Const EVID_POKER_CALL             := 0x0051;
Const EVID_POKER_RAISE            := 0x0052;
Const EVID_POKER_LEAVE            := 0x0053;
Const EVID_POKER_OK               := 0x0054;
// Lift (new)
Const EVID_LIFT_CALL              := 0x0055;
Const EVID_LIFT_MODIFY            := 0x0056;
// Magischer Spiegel
Const EVID_REFLECTION             := 0x0057;

// 0x0080 reserved
