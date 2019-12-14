///////////////////////////////////////////////////////////////////////////
// Packets Include - Alles, um den Client direkt zu beeinflussen
//
// based on Packets Include
// Author: S1tH
// ICQ#  : 38288137
//
// Author: Shinigami

///////////////////////////////////////////////////////////////////////////
// Funktionen
//   UpdatePlayer              - Aendert die Grafik eines Chars fuer einen Client
//   DrawGamePlayer            - Veraendert das Aussehen des eigenen Chars, ohne dass andere es sehen
//   ReDrawGamePlayer          - Setzt das Aussehen zurueck
//   PlayCharAnimPrivate       - Spielt eine Animation, die nur einer sieht
//   PlayCharAnimRepeatPrivate - Spielt eine Animation, sich wiederholend, ab, die nur einer sieht
//   PlayMidiMusic             - Spielt einen Sound
//   SetOverallLightForAll     - Setzt fuer alle das Licht
//   SetOverallLightForPlayer  - Setzt fuer einen Einzelnen das Licht
//   SetCursorHue              - Setzt die Cursor-Farbe
//   SetQuestArrow             - Setzt ... (Muss man auch abschalten!)
//   UpdateMobiles             - Schickt Mobilesdaten neu im Umkreis "Range" und ListMobilesFlags
//   CloseGump                 - Schließt Gump von einem Script(PID) mit einer Returnbuttonid
//
// Anwendungen
//   IsInFeMNet                - Befindet sich der Spieler direkt im FeM-Net?
//   GetNumericIP              - Wandelt eine IP in Stringform in ein numerisches Array
//
// Konvertierung
//   BytePacket                - Liefert ein Byte im Packet-Format
//   WordPacket                - Liefert ein Word im Packet-Format
//   DoubleWordPacket          - Liefert ein DoubleWord im Packet-Format

use polsys;
include "client";
include "urls";

///////////////
// Konstanten
///////////////

Const ANIM_DIR_FORWARDS  := 0;
Const ANIM_DIR_BACKWARDS := 1;
Const ANIM_DEF_SPEED     := 0; // 0 = fastest, 30 = slow, 70 = wtf are you doing?

Const CURSOR_NORM := 0; // Felucca / Old UO
Const CURSOR_GOLD := 1; // Trammel

Const QUESTARROW_ON  := 1;
Const QUESTARROW_OFF := 0;

////////////////////
//   Funktionen   //
////////////////////

////////////////////////////////////////////////////////////////////
// UpdatePlayer - Aendert die Grafik eines Chars fuer einen Client
////////////////////////////////////////////////////////////////////
Function UpdatePlayer(who, forwho, graphic:=0)
  If (!graphic)
    graphic:=who.graphic;
  EndIf
  var packet:=CreatePacket(0x77,0x11);
  packet.setint32(1,who.serial);
  packet.setint16(5,graphic);
  packet.setint16(7,who.x);
  packet.setint16(9,who.y);
  packet.setint8(11,who.z);
  packet.setint8(12,who.facing);
  packet.sendpacket(forwho);
EndFunction

//////////////////////////////////////////////////////////////////////////////////////////
// DrawGamePlayer - Veraendert das Aussehen des eigenen Chars, ohne dass andere es sehen
//////////////////////////////////////////////////////////////////////////////////////////
Function DrawGamePlayer(who, bodygraphic, color)
  var packet:=CreatePacket(0x20,0x13);
  packet.setint32(1,who.serial);
  packet.setint16(5,bodygraphic);
  packet.setint16(8,color);
  packet.setint16(11,who.x);
  packet.setint16(13,who.y);
  packet.setint8(17,who.facing);
  packet.setint8(18,who.z);
  packet.sendpacket(who);
EndFunction

//////////////////////////////////////////////////
// ReDrawGamePlayer - Setzt das Aussehen zurueck
//////////////////////////////////////////////////
Function ReDrawGamePlayer(who)
  DrawGamePlayer(who, who.graphic, who.color);
EndFunction

/////////////////////////////////////////////////////////////////////
// PlayCharAnimPrivate - Spielt eine Animation, die nur einer sieht
/////////////////////////////////////////////////////////////////////

Function PlayCharAnimPrivate(who, animid, forwho, speed:=ANIM_DEF_SPEED, dir:=ANIM_DIR_FORWARDS)
  TranslateToMountAnim(who,animid);
  var packet:=CreatePacket(0x6E,0xE);
  packet.setint32(1,who.serial);
  packet.setint16(5,animid);
  packet.setint16(7,0x05); // FrameCount 5 passt immer
  packet.setint16(9,0x1);  // RepeatCount
  packet.setint8(11,dir);
  packet.setint8(12,0x0);  // Repeat
  packet.setint8(13,speed);

  packet.sendpacket(forwho);
EndFunction

//////////////////////////////////////////////////////////////////////////////////////////////////
// PlayCharAnimRepeatPrivate - Spielt eine Animation, sich wiederholend, ab, die nur einer sieht
//////////////////////////////////////////////////////////////////////////////////////////////////
Function PlayCharAnimRepeatPrivate(who, animid, reptimes, forwho, speed:=ANIM_DEF_SPEED, dir:=ANIM_DIR_FORWARDS)
  TranslateToMountAnim(who,animid);
  var packet:=CreatePacket(0x6E,0xE);
  packet.setint32(1,who.serial);
  packet.setint16(5,animid);
  packet.setint16(7,0x05); // FrameCount 5 passt immer
  packet.setint16(9,reptimes);  // RepeatCount
  packet.setint8(11,dir);
  packet.setint8(12,0x1);  // Repeat
  packet.setint8(13,speed);

  packet.sendpacket(forwho);
EndFunction

///////////////////////////////////////
// PlayMidiMusic - Spielt einen Sound
///////////////////////////////////////
Function PlayMidiMusic(who, musicid)
  var packet:=CreatePacket(0x6D,0x3);
  packet.setint16(1,musicid);

  packet.sendpacket(who);
EndFunction

//////////////////////////////////////////////////////
// SetOverallLightForAll - Setzt fuer alle das Licht
//////////////////////////////////////////////////////
Function SetOverallLightForAll(light)
  var packet:=CreatePacket(0x4F,0x2);
  packet.setint8(1,light);

  ForEach chr in EnumerateOnlineCharacters()
    packet.sendpacket(chr);
  EndForEach
EndFunction

////////////////////////////////////////////////////////////////////
// SetOverallLightForPlayer - Setzt fuer einen Einzelnen das Licht
////////////////////////////////////////////////////////////////////
Function SetOverallLightForPlayer(who, light)
  var packet:=CreatePacket(0x4F,0x2);
  packet.setint8(1,light);
  packet.sendpacket(who);
EndFunction

//////////////////////////////////////////
// SetCursorHue - Setzt die Cursor-Farbe
//////////////////////////////////////////
Function SetCursorHue(who, hue)
  var packet:=CreatePacket(0xBF,0x6);
  packet.setint16(1,0x6);
  packet.setint16(3,0x8);
  packet.setint8(5,hue);

  packet.sendpacket(who);
EndFunction

//////////////////////////////////////////////////////////
// SetQuestArrow - Setzt ... (Muss man auch abschalten!)
//////////////////////////////////////////////////////////
Function SetQuestArrow(who, active, x, y)
  var packet:=CreatePacket(0xBA,0x6);
  packet.setint8(1,active);
  packet.setint16(2,x);
  packet.setint16(4,y);

  packet.sendpacket(who);
EndFunction

///////////////////////////////////////////////////////////////
// UpdateMobiles - Schickt Mobilesdaten neu im Umkreis "Range" und ListMobilesFlags
///////////////////////////////////////////////////////////////
Function UpdateMobiles(who,range,flags)
  var packet,statusflag:=0,i;

  Foreach mob in (ListMobilesNearLocationEx(who.x,who.y,who.z,range,flags,who.realm))
    packet:=CreatePacket(0x78,MSGLEN_VARIABLE);
    packet.setInt32( 3,mob.serial);  // ObjectID
    packet.setInt16( 7,mob.graphic); // ObjectModell
    packet.setInt16( 9,mob.x);       // xLoc
    packet.setInt16(11,mob.y);       // yLoc
    packet.setInt8( 13,mob.z);       // zLoc
    packet.SetInt8( 14,mob.facing);  // Direction
    packet.SetInt16(15,mob.color);   // Color
    statusflag:=0x0;
    If (mob.invisible)
      statusflag:=statusflag | 0x80;
    EndIf
    If (mob.warmode)
      statusflag:=statusflag | 0x40;
    EndIf
    If (mob.poisoned)
      statusflag:=statusflag | 0x04;
    EndIf
    If (mob.gender)
      statusflag:=statusflag | 0x02;
    EndIf
    packet.SetInt8(17,statusflag);  //Status
    // Noto Options:
    // 0x1 Innocent (Blue)
    // 0x2 Friend (Green)
    // 0x3 Grey (Grey - Animal)
    // 0x4 Criminal (Grey)
    // 0x5 Enemy (Orange)
    // 0x6 Murderer (Red)
    // 0x7 Invulnerable (Yellow)
    If (mob.enabled("invul"))
      packet.SetInt8(18,0x7);
    ElseIf (mob.murderer)
      packet.SetInt8(18,0x6);
    ElseIf (mob.criminal)
      packet.SetInt8(18,0x4);
    Else
      packet.SetInt8(18,0x1);
    EndIf
    i:=19;
    ForEach item in (ListEquippedItems(mob))
      packet.SetInt32(i,item.serial);             // EquippedItemID
      i+=4;
      If (item.color)
        packet.SetInt16(i,(item.graphic+0x8000)); // Graphic + Colorflag
        i+=2;
        packet.SetInt8(i,item.layer);             // Layer
        i+=1;
        packet.SetInt16(i,item.color);            // Color
        i+=2;
      Else
        packet.SetInt16(i,item.graphic);          // Graphic
        i+=2;
        packet.SetInt8(i,item.layer);             // Layer
        i+=1;
      EndIf
    EndForEach
    packet.SetInt32(i,0);                         // Doneflag
    packet.SendPacket(who);                       // *winke*
  EndForEach
EndFunction

//////////////////////////////////////////////////////
// CloseGump - Schließt ein Gump ( Packet 0xBF - Subcommand 0x4 )
//////////////////////////////////////////////////////
Function CloseGumpPergon(who, pid, id)
  var packet:=CreatePacket(0xBF, MSGLEN_VARIABLE);
  packet.setint16(3, 0x4); // SubCommand
  packet.setint32(5, pid); // GumpPID
  packet.setint8( 9, id);  // AntwortID
  packet.sendpacket(who);
EndFunction

//////////////////////////////////////////////////////
// SendStatLock - Buttons in Statusbar
//  eigentlich pro 3 Zustände wir brauchen nur 3 definierte
//////////////////////////////////////////////////////
//Subcommand: 0x19: Extended stats
//BYTE[1] type // always 2 ? never seen other value
//BYTE[4] serial
//BYTE[1] unknown // always 0 ?
//BYTE[1] lockBits // Bits: XXSS DDII (s=strength, d=dex, i=int), 0 = up, 1 = down, 2 = locked
//Function SendStatLock(who)
//  If ( who.clientver_detail.major < 3 )
//    return(0);
//  EndIf
//
//  var packet:=CreatePacket(0xBF, 12);
//  packet.SetInt8(2, 12);   // Length
//  packet.SetInt8(4, 0x19); // Subcmd
//  packet.SetInt8(5, 2);    // Const
//  packet.SetInt32(6, who.serial);
//
//
//  var statcap:=who.getprop(ATTRIBUTECAP);  // Aktuellen Wert besorgen
//  If (!statcap)  // Standart ist Dex
//    statcap:=ATTRIBUTEID_DEXTERITY;
//    who.setprop(ATTRIBUTECAP, ATTRIBUTEID_DEXTERITY);
//  EndIf
//
//  Case (statcap) // Wir sparen uns BitOperationen da es eh nur 3 Definierte Zustände gibt
//    ATTRIBUTEID_STRENGTH:
//      packet.SetInt8(11, 0x10);
//    ATTRIBUTEID_DEXTERITY:
//      packet.SetInt8(11, 0x04);
//    ATTRIBUTEID_INTELLIGENCE:
//      packet.SetInt8(11, 0x01);
//  EndCase
//  packet.SendPacket(who);
//
//  return(1);
//EndFunction


////////////////////////////////////////////
// BuffIcons - Schickt/Entfernt BuffIcons
////////////////////////////////////////////
Function BuffIcons(who, spell, time:= 0, remove:=0)
  If ((!who.connected) or (who.isa(POLCLASS_NPC)))
    return;
  EndIf
  If (!(who.clienttype & CLIENTTYPE_5020))
    return;
  EndIf

  var BuffIcon := dictionary{
  "spell_dazzle"          ->struct{iconid := 1041, cliloc := 600002, cliloc2 := 600172},//"Euer Geist ist beeinträchtigt."
  "spell_clumsy"          ->struct{iconid := 1025, cliloc := 600003, cliloc2 := 600173},//"Euer Geschick ist beeinträchtigt."
  "spell_weaken"          ->struct{iconid := 1020, cliloc := 600004, cliloc2 := 600174},//"Eure Kraft ist beeinträchtigt."
  "spell_cunning"         ->struct{iconid := 1015, cliloc := 600011, cliloc2 := 600175},//"Euer Geist ist gestärkt."
  "spell_agility"         ->struct{iconid := 1040, cliloc := 600012, cliloc2 := 600176},//"Eure Geschicklichkeit ist verbessert."
  "spell_strength"        ->struct{iconid := 1016, cliloc := 600013, cliloc2 := 600177},//"Eure Kraft nimmt zu."
  "spell_protect"         ->struct{iconid := 1014, cliloc := 600009, cliloc2 := 600178},//"Ein magischer Schutz umgibt Euch."
  "spell_incognito"       ->struct{iconid := 1006, cliloc := 600028, cliloc2 := 600179},//"Unerkannt geht Ihr durch's Land."
  "spell_poly"            ->struct{iconid := 1002, cliloc := 600031, cliloc2 := 600180},//"Euer Äußeres hat sich verändert."
  "fire_aura"             ->struct{iconid := 1021, cliloc := 600050, cliloc2 := 600181},//"Flammen lodern um Euch herum auf."
  "water_aura"            ->struct{iconid := 1026, cliloc := 600051, cliloc2 := 600182},//"Um Euch wird es kalt."
  "air_aura"              ->struct{iconid := 1039, cliloc := 600053, cliloc2 := 600183},//"Blitze zucken um Euch herum."
  "earth_aura"            ->struct{iconid := 1030, cliloc := 600052, cliloc2 := 600184},//"Steine wirbeln um Euch herum."
  "fire_resist"           ->struct{iconid := 1045, cliloc := 600054, cliloc2 := 600185},//"Ihr seid vor dem Feuer geschützt."
  "water_resist"          ->struct{iconid := 1009, cliloc := 600044, cliloc2 := 600186},//"Ihr seid vor dem Wasser geschützt."
  "air_resist"            ->struct{iconid := 1034, cliloc := 600055, cliloc2 := 600187},//"Ihr seid vor der Luft geschützt."
  "earth_resist"          ->struct{iconid := 1013, cliloc := 600049, cliloc2 := 600188},//"Ihr seid vor der Erde geschützt."
  "poison_resist"         ->struct{iconid := 1046, cliloc := 600111, cliloc2 := 600189},//"Ihr seid vor Gift geschützt."
  "elements_resist"       ->struct{iconid := 1010, cliloc := 600085, cliloc2 := 600190},//"Ihr seid vor den Elementen geschützt."
  "immune"                ->struct{iconid := 1024, cliloc := 600092, cliloc2 := 600191},//"Ihr seid nahezu immun gegen alle Elemente."
  "mirror_single"         ->struct{iconid := 1043, cliloc := 600024, cliloc2 := 600192},//"Ihr reflektiert einen Zauber."
  "holy_mirror"           ->struct{iconid := 1023, cliloc := 600091, cliloc2 := 600193},//"Ihr reflektiert alle Zauber."
  "spell_bless"           ->struct{iconid := 1033, cliloc := 600073, cliloc2 := 600194},//"Ihr fühlt Euch rundum gestärkt."
  "spell_curse"           ->struct{iconid := 1008, cliloc := 1015188, cliloc2 := 600195},//"Ihr fühlt Euch schwach und hilflos."
  "spell_blind"           ->struct{iconid := 1032, cliloc := 600106, cliloc2 := 600196},//"Ihr seht den Wald vor Bäumen nicht."
  "spell_paralyze"        ->struct{iconid := 1001, cliloc := 600078, cliloc2 := 600197},//"Ihr seid bewegungsunfähig."
  "spell_statue"          ->struct{iconid := 1044, cliloc := 600123, cliloc2 := 600198},//"Euer Körper ist massiver Stein."
  "spell_spiritspeak"     ->struct{iconid := 1007, cliloc := 600094, cliloc2 := 600199},//"Stimmen aus dem Nichts."
  "spell_soulseek"        ->struct{iconid := 1031, cliloc := 600112, cliloc2 := 600200},//"Ihr glaubt Gespenster zu sehen."
  "spell_doomed"          ->struct{iconid := 1037, cliloc := 600124, cliloc2 := 600201},//"Das Totenreich umklammert Euch."
  "holy_warrior"          ->struct{iconid := 1011, cliloc := 600090, cliloc2 := 600202},//"Unbändige Kraft durchströmt Euch."
  "dead_knight"           ->struct{iconid := 1017, cliloc := 600125, cliloc2 := 600203},//"Ihr seid dem Tode geweiht."
  "spell_speed"           ->struct{iconid := 1048, cliloc := 600015, cliloc2 := 600204},//"Schnell wie der Wind."
  "spell_slow"            ->struct{iconid := 1027, cliloc := 600014, cliloc2 := 600205},//"Lahm wie eine Schnecke."
  "potion_staerkend"      ->struct{iconid := 1028, cliloc := 600161, cliloc2 := 600206},//"Ihr fühlt euch gestärkt."
  "potion_beweglich"      ->struct{iconid := 1029, cliloc := 600162, cliloc2 := 600207},//"Ihr fühlt euch beweglicher."
  "potion_erhellend"      ->struct{iconid := 1035, cliloc := 600163, cliloc2 := 600208},//"Euer Geist wurde erhellt."
  "potion_feuerresistent" ->struct{iconid := 1047, cliloc := 600164, cliloc2 := 600209},//"Ihr seid vor dem Feuer geschützt."
  "potion_erdresistent"   ->struct{iconid := 1019, cliloc := 600166, cliloc2 := 600210},//"Ihr seid vor der Erde geschützt."
  "potion_wasserresistent"->struct{iconid := 1012, cliloc := 600165, cliloc2 := 600211},//"Ihr seid vor dem Wasser geschützt."
  "potion_luftresistent"  ->struct{iconid := 1022, cliloc := 600167, cliloc2 := 600212},//"Ihr seid vor der Luft geschützt."
  "potion_giftresistent"  ->struct{iconid := 1042, cliloc := 600168, cliloc2 := 600213},//"Ihr seid vor Gift geschützt."
  "fish_cunning"          ->struct{iconid := 1038, cliloc := 600169, cliloc2 := 600214},//"Ein Fisch ist schlau."
  "fish_agility"          ->struct{iconid := 1018, cliloc := 600170, cliloc2 := 600215},//"Ein Fisch ist agil."
  "fish_strength"         ->struct{iconid := 1005, cliloc := 600171, cliloc2 := 600216}};//"Ein Fisch ist stark."

//ServerID  Gump   Spruch
//  1001   0x754C  Paralyse
//  1002   0x754A  Polymorph
//  1005   0x755E  Magischer Gelbgruen-Fisch
//  1006   0x7549  Inkognito
//  1007   0x7551  Geistersprache
//  1008   0x7556  Fluch
//  1009   0x753A  Wasserresistenz
//  1010   0x754D  Elementarresistenz
//  1011   0x754E  Heiliger Krieger
//  1012   0x7565  Trank der Wasserresistenz
//  1013   0x753B  Erdresistenz
//  1014   0x7543  Schutz
//  1015   0x7544  Scharfsinn
//  1016   0x7546  Kraft
//  1017   0x755C  Todesritter
//  1018   0x755F  Magischer Rot-Fisch
//  1019   0x7566  Trank der Erdresistenz
//  1020   0x7554  Schwäche
//  1021   0x7540  Flammenaura
//  1022   0x7568  Trank der Luftresistenz
//  1023   0x754F  Heiliger Spiegel
//  1024   0x7550  Immunität
//  1025   0x7553  Missgeschick
//  1026   0x753E  Eisaura
//  1027   0x755D  Hinken
//  1028   0x7563  Trank der Stärke
//  1029   0x7562  Trank des Geschicks
//  1030   0x753F  Felsaura
//  1031   0x7559  Seelen Spüren
//  1032   0x7557  Dunkelheit
//  1033   0x754B  Segen
//  1034   0x753D  Luftresistenz
//  1035   0x7561  Trank der Intelligenz
//  1036   0x7547  Rennen
//  1037   0x755B  Verbannung
//  1038   0x7560  Magischer Gelb-Fisch
//  1039   0x7541  Blitzaura
//  1040   0x7545  Geschicklichkeit
//  1041   0x7552  Verwirrung
//  1042   0x7569  Trank der Giftresistenz
//  1043   0x7548  Magischer Spiegel
//  1044   0x755A  Statue
//  1045   0x753C  Feuerresistenz
//  1046   0x7555  Giftresistenz
//  1047   0x7567  Trank der Feuerresistenz
//  1048   0x7542  Hinken

  If (spell == "spell_aura")
    case (who.getprop(spell)[2])
      "fire":  spell := "fire_aura";
      "water": spell := "water_aura";
      "air":   spell := "air_aura";
      "earth": spell := "earth_aura";
    endcase
  ElseIf (spell == "spell_resist")
    case (who.getprop(spell)[2])
      "fire":     spell := "fire_resist";
      "water":    spell := "water_resist";
      "air":      spell := "air_resist";
      "earth":    spell := "earth_resist";
      "poison":   spell := "poison_resist";
      "elements":
      If (who.getprop(spell)[4] == "immune")
        spell := "immune";
      Else
        spell := "elements_resist";
      EndIf
    endcase
  ElseIf (spell == "spell_bless")
    If (who.getprop(spell)[3] == "holy warrior")
      spell := "holy_warrior";
    EndIf
  ElseIf (spell == "spell_curse")
    If (who.getprop(spell)[3] == "dead knight")
      spell := "dead_knight";
    EndIf
  ElseIf (spell == "spell_reflect")
    If (who.getprop(spell)[2] == "mirror")
      spell := "holy_mirror";
    EndIf
  EndIf

  If (!BuffIcon[spell])
    return;
  EndIf

  var packet:=CreatePacket(0xDF,MSGLEN_VARIABLE);
  packet.SetInt32(3,who.serial);
  packet.SetInt16(7,BuffIcon[spell].iconid); // SpellIcon

  If (remove)  // Spellicon entfernen
  	packet.SetInt16(9,0x0);
  	packet.SetSize(13);
  Else
    packet.SetInt16(9,0x1);
    packet.SetInt16(15,BuffIcon[spell].iconid);    // SpellIcon
    packet.SetInt16(17,0x1);
    If (time)
      packet.SetInt16(23,time);    // Timecountdown
    Else
      packet.SetInt16(23,0);
    EndIf
    packet.SetInt32(28,BuffIcon[spell].cliloc);    // CliLoc 1
    packet.SetInt32(32,BuffIcon[spell].cliloc2);   // CliLoc 2
    packet.SetSize(44);
  EndIf
  packet.SendPacket(who);
EndFunction

/////////////////////
//                 //
//   Anwendungen   //
//                 //
/////////////////////

//////////////////////////////////////////////////////////////
// IsInFeMNet - Befindet sich der Spieler direkt im FeM-Net?
//////////////////////////////////////////////////////////////
Function IsInFeMNet(who)
 // If (who.ip<>TUNNEL_SERVER)
 //   return (1);
 // EndIf

  var whoip := GetNumericIP(who.ip);
  If (
    (whoip[1] == 141) and (whoip[2] == 24) and
    (40 <= whoip[3]) and (whoip[3] <= 56)
  )
    return (1);
  EndIf
EndFunction

///////////////////////////////////////////////////////////////////////////
// GetNumericIP - Wandelt eine IP in Stringform in ein nummerisches Array
///////////////////////////////////////////////////////////////////////////
Function GetNumericIP(ip)
  var dot1:=Find(ip, ".", 1);
  var dot2:=Find(ip, ".", dot1+1);
  var dot3:=Find(ip, ".", dot2+1);

  return ({CInt(ip[1, dot1-1]), CInt(ip[dot1+1, dot2-dot1-1]), CInt(ip[dot2+1, dot3-dot2-1]), CInt(ip[dot3+1, Len(ip)-dot3])});
EndFunction

///////////////////////
//                   //
//   Konvertierung   //
//                   //
///////////////////////
Function TranslateToMountAnim(who, byref anim)
  If (GetEquipmentByLayer(who,LAYER_MOUNT))
    If ((anim<=ANIM_DIE_FOR) || (anim>=ANIM_WAR_TURN))
      var cfg:=ReadConfigFile("::animxlate")["OnMount"];
      If (cfg)
        var change:=GetConfigInt(cfg, Hex(anim));
        If (!change.errortext)
          anim:=change;
        EndIf
      EndIf
    EndIf
  EndIf
EndFunction

///////////////////////////////////////////////////
// BytePacket - Liefert ein Byte im Packet-Format
///////////////////////////////////////////////////
Function BytePacket(byte)
  var paket:=Hex(byte);
  paket:="00"+paket[3, Len(paket)-2];
  return (paket[Len(paket)-1, 2]);
EndFunction

///////////////////////////////////////////////////
// WordPacket - Liefert ein Word im Packet-Format
///////////////////////////////////////////////////
Function WordPacket(word)
  var paket:=Hex(word);
  paket:="0000"+paket[3, Len(paket)-2];
  return (paket[Len(paket)-3, 4]);
EndFunction

///////////////////////////////////////////////////////////////
// DoubleWordPacket - Liefert ein DoubleWord im Packet-Format
///////////////////////////////////////////////////////////////
Function DoubleWordPacket(doubleword)
  var paket:=Hex(doubleword);
  paket:="00000000"+paket[3, Len(paket)-2];
  return (paket[Len(paket)-7, 8]);
EndFunction
