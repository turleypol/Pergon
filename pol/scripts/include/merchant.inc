///////////////////////////////////////////////////////////////////////////
// Merchant-Prototype v1.0 (2000/03/29)
//
// based on Stable Keeper Script v0.18 (1999/11/30)
// Author: RoTTer
//
// Author: Shinigami

///////////////////////////////////////////////////////////////////////////
// Neue Ideen:
//   Beschäftigung der NPCs, wenn sie nix zu tun haben, so gemäß:
//     *kratz*, *hust* etc.
//   Wenn 'n Player neben einem NPC steht und ihn ständig _nicht_ anlabert,
//     aber redet, dann soll er den Player ignorieren und erst wieder reagieren,
//     wenn dieser weglatscht oder ihn anspricht -> Liste von Playern, die
//     ignoriert werden.
//   Sphere-Speech-Konverter

// ToDo:
//   medata.+minelocation := PlaceName(me);
//   "Seid gegruesst" nicht nur ueber speech-file, sondern auch tageszeitlich
//     abhaengig, mehrere eintraege
//   VisibleCheck, d.h. wenn [N]PC range betritt und Ereignis ausgeloest wird,
//     dann soll der nur labern, wenn Sichtlinie, wenn nicht Sichtlinie, dann
//     loift 'ne Schleife (mit SleepMS(500)), wo der schaut, ob endlich
//     Sichtlinie und gruesst dann, wenn ja und beendet die Schleife (hierbei
//     am Besten entfernungsabhaengige begruessungsformeln.. SpeechEvent, so
//     von wegen ("uups, hab dich garnet gesehen ..."). problem: schleifenende
//     ... im script (NUR wenn je [N]PC _eine_ scriptdateninstanz gestartet
//     wird (dann gehts mit lokalen vars) -> bei left_area variable setzen,
//     die in der laufenden entered_area getestet wird (muss beim ersten mal
//     geloescht werden!) / sonst muss im [N]PC 'ne variable (abhaengig von
//     NPC serial (temporaer) gespeichert werden, die die aufgabe uebernimmt,
//     aber auch nur so'ne killvariable, wie oben im script). weiterhin muss
//     vermerkt werden, ob der typ ueberhaupt gegruesst wurde, falls net, wird
//     er auch net verabschiedet (fuer, z.b. am haus vorbeilaufen).
//   Anrede ohne Namen -> die npcs rundum kriegen entered_area und jeder
//     checkt dann, wenn halt labern ohne vorher anreden, ob 'n gespraech
//     besteht. falls net, labern _alle_ was, von wegen "wenn du mit mir labern
//     willst, dan red mich an, oder so", bzw. wenn nur einer da is, dann
//     wird automatisch 'n gespraech aufgebaut.
//   MerchantType per GetObjProp holen und dann aber ueberschreibbar lassen
//   Merchant cuttet gespraech, wenn oeffnungszeiten vorbei ...
//   Bei "Zeit" checken, ob der NPC 'ne Uhr traegt, falls net, kanner
//     die Zeit net sagen ("tut mir leid" etc.). Wenn man dem NPC 'ne
//     Uhr schenkt, dann "zieht" er die "an" und kann dann die Zeit sagen
//   Tierchens verkoifen ... abhaengig von mrcspawn machen
//   Labern, nur, wenn ich und er im Haus oder beide ausserhalb
//     oder Sichtlinie ... (siehe ONEList, da war mal was ...)
//   Training-Animationen
//   Sleeping
//   Smalltalk zwischen den NPC's

//////////////////////////
// Linking-Anforderungen
//////////////////////////
//
// Der, diese Datei benutzende, Quelltext muss folgende Funktion enthalten:
//
//   Function SetMerchantType(meprototype)
//     [...]
//     return (meprototype);
//   EndFunction

/////////////////
// Bibliotheken&Includes
/////////////////
use math;
use os;
use polsys;
use uo;
include ":spawnnet_new:spawnnet";
include "include/animal";
include "include/client";
include "include/clock";
include "include/eventid";
include "include/modifyskill";
include "include/mrcspawn";
include "include/names";
include "include/npc";
include "include/npcwalkpath";
include "include/objtype";
include "include/packets";
include "include/storage";
include "include/vetement"; // Random Clothing


//////////////////////
// Typendefinitionen
//////////////////////

var meprototype := struct;
    meprototype.+merchanttype := "";
    meprototype.+threshold_enter := 7;
    meprototype.+threshold_leave := meprototype.threshold_enter+1;
    meprototype.+threshold_speech := 3;
    meprototype.+threshold_callguards := 21;
    meprototype.+buynsold := 1;
    // meprototype.+move_action := 0;
    // meprototype.+merchant_sold := 0;
    // meprototype.+entered_area := 0;
    // meprototype.+left_area := 0;
    // meprototype.+item_given_sleeping := 0;
    // meprototype.+item_given := 0;
    // meprototype.+speech_open := 0;
    // meprototype.+speech_sleeping := 0;
    // meprototype.+speech_name := 0;
    // meprototype.+speech_default := 0;
    // meprototype.+ghost_speech_default := 0;

///////////////
// Konstanten
///////////////

Const RESPAWN_MINUTES := 30;
Const RESPAWN_MINUTES_DIFF := 10;

Const MUTE_TIME := 180;

Const PROP_CURRENT_CUSTOMER := "#current_customer";
Const PROP_CURRENT_BRIBE    := "bribed_npc";

//////////////////////
// Globale Variablen
//////////////////////

var me := Self();

///////////////////////////////
// Merchant - Initialisierung
///////////////////////////////
Program Merchant()
  meprototype := SetMerchantType(meprototype);

  // Informationen ueber mich selber sammeln
  var medata := struct;

  SetObjProperty(me, "serial", me.serial);

  If (GetObjProperty(me, "frozen"))
    me.frozen := 1;
    IncRevision(me);
  EndIf

  If ((me.name["<random>"]) Or ((me.name["<norandom>"])))
    If (me.name["<random>"])
      var npcname := RandomName(me);
      SetName(me, npcname);
      SetName(me, npcname+NPCTitle(me));

      npcname := me.name+" ";

      var pos := Find(npcname, ", ", 1);
      If (pos)
        npcname := npcname[1, pos-1];
      EndIf

      SetObjProperty(me, "myname", npcname);
    Else
      me.name := me.name[11, Len(me.name)-10];
      SetObjProperty(me, "myname", me.name);
    EndIf

    Habille(me, GetObjProperty(me, "Equipt")); // Random Clothing

    If (!me.backpack)
      var newbackpack := CreateItemAtLocationPergon(
        me.x, me.y, me.z, 0xe75, 1, me.realm
      );
      If (!EquipItem(me, newbackpack))
        syslog(
          "WARNUNG: Merchant "+Lower(Hex(me.serial))+" mag kein Backpack!"
        );
      EndIf
    EndIf
  EndIf

  medata.+minename     := GetObjProperty(me, "myname");
  medata.+minelocation := PlaceName(me);
  medata.+open         := CheckObjProperty(me, "open",  "06:00");
  medata.+close        := CheckObjProperty(me, "close", "20:00");
  medata.+sleeping     := 0;
  medata.+talktome     := 0;
  medata.+mute         := 0;
  medata.+npcpath      := 0;

  // Handelsrucksaecke vorbereiten {{{
  If (meprototype.buynsold)
    medata.+merchant_type := CheckObjProperty(
      me, "MerchantType", meprototype.merchanttype
    );

    var storage := FindStorageArea(STORAGE_MERCHANT);
    If (!storage)
      syslog(
        "FEHLER: "+medata.merchant_type+" "+me.name+" -> FindStorageArea"
      );
      // Selbstmord, Braindead ...
      return;
    EndIf

    medata.+inv_fs := FindOrCreateRootItemInStorage(
      storage, me.serial+" FS", 0xe7c
    );
    If (!medata.inv_fs)
      syslog("FEHLER: "+medata.merchant_type+" "+me.name+" -> inv_fs");
      // Selbstmord, Braindead ...
      return;
    EndIf

    medata.+inv_pb := FindOrCreateRootItemInStorage(
      storage, me.serial+" PB", 0xe7c
    );
    If (!medata.inv_pb)
      syslog("FEHLER: "+medata.merchant_type+" "+me.name+" -> inv_pb");
      // Selbstmord, Braindead ...
      return;
    EndIf

    medata.+inv_1c := FindOrCreateRootItemInStorage(
      storage, me.serial+" 1C", 0xe7c
    );
    If (!medata.inv_1c)
      syslog("FEHLER: "+medata.merchant_type+" "+me.name+" -> inv_1c");
      // Selbstmord, Braindead ...
      return;
    EndIf
  EndIf
  // }}}

  SetWarMode(0);
  DropAnchor();

  // Hauptschleife aufrufen
  Merchant_MainLoop(me, medata);
EndProgram

// Hauptschleife
Function Merchant_MainLoop(me, medata)
  Set_Priority(10);

  EnableEvents(SYSEVENT_SPEECH+SYSEVENT_GHOST_SPEECH+SYSEVENT_ENGAGED+SYSEVENT_DISENGAGED+SYSEVENT_DAMAGED, meprototype.threshold_speech);
  EnableEvents(SYSEVENT_ENTEREDAREA, meprototype.threshold_enter);
  EnableEvents(SYSEVENT_LEFTAREA, meprototype.threshold_leave);
  EnableEvents(SYSEVENT_OPPONENT_MOVED);
  EnableEvents(SYSEVENT_ITEM_GIVEN, meprototype.threshold_speech);

  var next_restock := ReadGameClock()+RandomInt(RESPAWN_MINUTES_DIFF);

  If (medata.merchant_type) // nur ausfuehren, wenn Merchant
    RestockInventory(me.npctemplate, medata.merchant_type, medata.inv_fs);
  EndIf

  var next_wander := ReadGameClock()+10;

  While (me)
    SleepMs(2);
    PerformAction(me, UACTION_LOOK_AROUND);

    // Neubestueckung {{{
    If (meprototype.buynsold and medata.merchant_type)
      // nur ausfuehren, wenn Merchant
      If (ReadGameClock() > next_restock)
        next_restock :=
          ReadGameClock() + 60 * CInt(
            RESPAWN_MINUTES +
            RandomIntMinMax(-RESPAWN_MINUTES_DIFF/2, RESPAWN_MINUTES_DIFF/2)
          );
        RestockInventory(
          me.npctemplate, medata.merchant_type, medata.inv_fs
        );
      EndIf
    EndIf
    // }}}

    // auf Events warten, ggf. herumlaufen {{{
    var ev;
    If (GetObjProperty(me, "NPCPath"))
      medata.npcpath := 1;
      ev := Wait_for_EventLogging(1);
    Else
      medata.npcpath := 0;
      ev := Wait_for_EventLogging(120);
    EndIf

    If (medata.npcpath)
      If (!medata.talktome)
        NPCWalkPath(me);
      EndIf
      SleepMS(10);
    EndIf

    If (ReadGameClock() >= next_wander)
      next_wander := ReadGameClock()+10;
      If ((!medata.npcpath) And (!medata.talktome))
        WanderPergon();
      EndIf
    EndIf
    // }}}

    If (!ev.source.isa(POLCLASS_MOBILE))
      // nur auf NPCs/Spieler reagieren
      continue;
    EndIf

    // NPC-Events bearbeiten {{{
    If (ev.source.isa(POLCLASS_NPC))
      Case (ev.type)
      SYSEVENT_ENGAGED:
        CallGuards(ev.source, "");
        Run(ev.source, meprototype.threshold_enter);

      SYSEVENT_DAMAGED:
        CallGuards(ev.source, "");
        If (ev.source)
          Run(ev.source, meprototype.threshold_enter);
        EndIf

      EndCase
      continue;
    EndIf
    // }}}

    If (ev.source.dead)
      // tote Spieler ignorieren
      continue;
    EndIf

    // Player-Events bearbeiten {{{
    If (GetObjProperty(ev.source, "talkto") == medata.minename)
      medata.talktome := ev.source;
    Else
      AutoTalkTo(me, medata, ev.source);
    EndIf

    var los := CheckLineOfSight(me, ev.source);
    // generelle Aktionen, wenn Spieler gesehen wird {{{
    If (los)
      // Oeffnen/Schliessen {{{
      var daytime := GetPergonTimeOfDay();
      If (medata.open<medata.close)
        If ((daytime<medata.open) Or (daytime>medata.close))
          medata.sleeping := 1;
        Else
          medata.sleeping := 0;
        EndIf
      Else
        If ((daytime<medata.open) And (daytime>medata.close))
          medata.sleeping := 1;
        Else
          medata.sleeping := 0;
        EndIf
      EndIf
      // }}}

      // zum Spieler drehen, ggf. hinlaufen {{{
      If (
        // mit Ausnahmen
        (ev.type<>SYSEVENT_SPEECH) And
        (ev.type<>SYSEVENT_GHOST_SPEECH) And
        (!medata.sleeping)
      )
        If (ev.type == SYSEVENT_ENTEREDAREA)
          SleepMS(RandomInt(2000));
        EndIf
        TurnToward(ev.source);
        If (meprototype.move_action)
          Hook_Move_Action(me, medata, ev);
        EndIf
      EndIf
      // }}}
    EndIf
    // }}}

    Case (ev.type)
    SYSEVENT_ENTEREDAREA: // {{{
      If (!los or medata.sleeping)
        break;
      EndIf

      If (meprototype.entered_area)
        Hook_Entered_Area(me, medata, ev);
      Else
        // ToDo: folgende Zeile is drinne, weil die wegrennen *g*
        medata.minelocation := PlaceName(me);
        NPCSayTextTraditional(
          me, Answering("seid_gegruesst", medata.minelocation, me), ""
        );
      EndIf
      // }}}

    SYSEVENT_LEFTAREA: // {{{
      If (medata.talktome)
        If (los)
          If (!medata.sleeping)
            SleepMS(RandomInt(1000));
            If (meprototype.left_area)
              Hook_Left_Area(me, medata, ev);
            Else
              NPCSayTextTraditional(me, "Empfehlt uns bitte weiter", "");
            EndIf
          EndIf
        EndIf

        EraseObjProperty(ev.source, "trainmaxcost");
        EraseObjProperty(ev.source, "skillid");
        EraseObjProperty(ev.source, "sleepmsg");
        EraseObjProperty(ev.source, "talkto");
        medata.talktome := 0;
      EndIf
      EraseObjProperty(ev.source, "townquest");
      // }}}

    SYSEVENT_SPEECH: // {{{
      If (
        // nicht sichtbar oder nichts gesagt oder
        !los or len(ev.text) <= 0 or
        // Emote
        (ev.text[1] == "*" and ev.text[len(ev.text)] == "*")
      )
        // ignorieren
        break;
      EndIf

      // ausgelagert, da laenglich
      Event_Speech(me, medata, ev);
      // }}}

    SYSEVENT_GHOST_SPEECH:
      Event_Ghost_Speech(me, medata, ev);

    SYSEVENT_ENGAGED: // {{{
      If (!ev.source.concealed)
        CallGuards(ev.source, "Aaahhh! Hilfe! Hilfe! Man greift mich an!");
        Run(ev.source, meprototype.threshold_enter);
      EndIf
      // }}}

    SYSEVENT_DAMAGED: // {{{
      If (!ev.source.concealed)
        CallGuards(ev.source, "Was habe ich nur getan. Zu Hilfe, Wachen!!!");
        If (ev.source)
          Run(ev.source, meprototype.threshold_enter);
        Else
          SayPergon(ev.source, "So viele gegen mich alleine? Hilfe!!!");
        EndIf
      EndIf
      // }}}

    SYSEVENT_MERCHANT_BOUGHT:
      // syslog("verkauft");

      // Erstellt LogInfos nachm Verkaufen
      // LogGoldProblemsAfterSelling(ev.source, ev.amount);

      // ermitteln, ob Gold ausgezahlt wurde, ggf. nachzahlen {{{
      var moneycheck := me.getprop("#MoneyCheckSell");
      me.eraseprop("#MoneyCheckSell");
      If (moneycheck.customer == ev.source.serial)
        // nur bei selbem Kunden ist Check sinnvoll

        var lostmoney :=
          // Gold, was er rechnerisch haben sollte
          moneycheck.gold + ev.amount -
          // Gold, was er hat
          ev.source.gold;

        If (lostmoney > 0)
          NPCSayTextTraditional(me,
            "Ich muss etwas Gold auf den Boden legen.",
            "I have to put some gold to the ground."
          );
          // da stimmt etwas nicht, Restgeld auszahlen
          While (lostmoney > 0)
            var size := Min(lostmoney, 60000);
            CreateItemAtLocation(
              ev.source.x, ev.source.y, ev.source.z,
              UOBJ_GOLD_COIN, size, ev.source.realm
            );
            lostmoney -= size;
          EndWhile
        EndIf
      EndIf
      // }}}

      // Tierersatzitems auswerten {{{
      var playerpets := GetObjProperty(ev.source, "playerpets");
      If (playerpets)
        // Items im Backpack entfernen, sowie die Listen bereinigen
        SleepMS(2);
        var temparray:=PetTamedItem()+PetEnticedItem();
        ForEach item in (EnumerateItemsInContainer(
          ev.source.backpack, ENUMERATE_ROOT_ONLY
        ))
          If (item.objtype in (temparray))
            SleepMs(2);
            // Tierchen nicht verkauft
            If (item.serial in playerpets.itemserial)
              // TODO: druebergucken, warum das funktioniert
              var index := 1; // Item aus den Listen loeschen
              While (index <= Len(playerpets.itemserial))
                If (playerpets.itemserial[index] == item.serial)
                  playerpets.itemserial.erase(index);
                  playerpets.petserial.erase(index);
                EndIf

                index += 1;
              EndWhile
            EndIf

            DestroyItem(item);
          EndIf
        EndForEach

        // Verkaufte Tiere dem ehemaligen Besitzer entziehen
        ForEach playerpet in (playerpets.petserial)
          SleepMs(2);
          var pet := SystemFindObjectBySerial(playerpet);
          If (pet)
            // falls es nur gezähmt wurde aber nicht abgerichtet
            If (pet.script == "tamedanimal")
              // Entfernt den NPC aus dem SpawnNet
              // FIXME: noetig?
              SpawnNet_Clear(me);
            EndIf
            KillNPC(pet, "merchant-verkauft", KILLNPC_NOCORPSE);
          EndIf
        EndForEach

        EraseObjProperty(ev.source, "playerpets");
      EndIf
      // }}}

      // Die vom NPC angekauften Sachen zerstoeren
      ForEach item in (EnumerateItemsInContainer(
        medata.inv_pb, ENUMERATE_ROOT_ONLY
      ))
        SleepMs(2);
        DestroyItem(item);
      EndForEach

      // Den Player ueber seinen Lohn informieren // {{{
      If (ev.amount > 0)
        If (los)
          NPCSayTextTraditional(me,
            "Hier sind Eure "+ev.amount+" Gold.",
            "Here are your "+ev.amount+" gold."
          );
        Else
          NPCSayTextTraditional(me,
            "Wartet. Hier sind noch Eure "+ev.amount+" Gold.", ""
          );
        EndIf
      EndIf
      // }}}

      // aktuellen Kunden vergessen, da Handel erledigt
      me.eraseprop(PROP_CURRENT_CUSTOMER);

    SYSEVENT_MERCHANT_SOLD: // {{{
      // syslog("gekauft");

      // kein Moneycheck, das prueft der Core selbst

      // Gegenstaende beweglich machen und
      // gekaufte Tiere dem neuen Besitzer uebergeben
      // {{{
      var petcount := 0;
      ForEach item in (EnumerateItemsInContainer(
        ev.source.backpack, ENUMERATE_ROOT_ONLY
      ))
        SleepMS(2);
        If (!item.movable)
          // Unbewegliche Gegenstaende im Backpack beweglich machen
          // Im Merchant-Storage funzt das irgendwie nicht ...
          item.movable := 1;
        EndIf

        // Handelt es sich um frisch gekaufte Tierchen?
        // Gezaehmtes Tierchen {{{
        var pettempl := GetObjProperty(item, "tamedanimal");
        If (pettempl)
          Var override:=struct;
          override.+CProps:=dictionary;
          override.+script:="tamedanimal";
          override.+Master:=ev.source.serial;
          override.CProps.insert("master",ev.source.serial);
          override.CProps.insert("PrevTamed",1);
          override.CProps.insert("RoamsFreeAt",ReadGameClock()+21600);
          var pet := CreateNPCFromTemplate(
            pettempl,
            ev.source.x, ev.source.y, ev.source.z, override, ev.source.realm
          );
          If (pet)
            petcount += 1;
            DestroyItem(item);
          EndIf
          continue;
        EndIf
        // }}}

        // Abgerichtetes Tierchen {{{
        pettempl := GetObjProperty(item, "enticedanimal");
        If (pettempl)
          Var override:=struct;
          override.+CProps:=dictionary;
          override.+script:="enticedanimal";
          override.+Master:=ev.source.serial;
          override.CProps.insert("master",ev.source.serial);
          var pet := CreateNPCFromTemplate(
            pettempl,
            ev.source.x, ev.source.y, ev.source.z, override, ev.source.realm
          );
          If (pet)
            petcount += 1;
            DestroyItem(item);
          EndIf
        EndIf
        // }}}
      EndForEach
      // }}}

      // Den Player ueber seine Kosten informieren // {{{
      If (ev.amount > 0)
        // If (los)
        NPCSayTextTraditional(me,
          "Ihr habt "+ev.amount+" Gold ausgegeben.",
          "You payed out "+ev.amount+" gold."
        );
        // EndIf
      EndIf
      // }}}

      // Zusatztexte behandeln {{{
      var saytext := "";

      // Den Player ueber seine Tiere informieren
      If (petcount == 1)
        saytext := "Passt gut auf Euer Tier auf und fuettert es immer!";
      ElseIf (petcount >= 1)
        saytext := "Passt gut auf Eure Tiere auf und fuettert sie immer!";
      EndIf

      // Den Player ueber sonstiges (Hook) informieren
      If (meprototype.merchant_sold)
        var hooksaytext := Hook_Merchant_Sold(me, medata, ev);
        If (hooksaytext<>"")
          If (saytext<>"")
            saytext := saytext+" "+hooksaytext;
          Else
            saytext := hooksaytext;
          EndIf
        EndIf
      EndIf
      If (saytext <> "")
        // da war noch etwas zu sagen
        NPCSayTextTraditional(me, saytext, "");
      EndIf
      // }}}

      // aktuellen Kunden vergessen, da Handel erledigt
      me.eraseprop(PROP_CURRENT_CUSTOMER);
      // }}}

    SYSEVENT_ITEM_GIVEN:
      If (los)
        If (medata.sleeping)
          var bribed := BribeMe(me, ev.source, ev.item);
          Case (bribed)
          BRIBE_OK:
            NPCSayTextTraditional(me, "Vielen Dank.", "");
          BRIBE_FEW:
            NPCSayTextTraditional(me, "Was soll das werden? Habt Ihr keine Ehre?", "");
          BRIBE_NO:
          default:
            If (meprototype.item_given_sleeping)
              Hook_Item_Given_Sleeping(me, medata, ev);
            Else
              NPCSayTextTraditional(me, "Kommt doch gegen "+medata.open+" wieder.", "");
            EndIf
          EndCase
        Else
          If (!MakeMerchantTrain(me, ev.source, ev.item))
            var result := 0;
            If (meprototype.item_given)
              result := Hook_Item_Given(me, medata, ev); // Wenn das Item nicht bearbeitet wird, muss es manuell zurueckgegeben werden!
            EndIf
            If (!result)
              NPCSayTextTraditional(me, "Sowas brauche ich nicht.", "I don't need this.");
              MoveItemToContainer(ev.item, ev.source.backpack);
            EndIf
          EndIf
        EndIf

        EraseObjProperty(ev.source, "trainmaxcost");
        EraseObjProperty(ev.source, "skillid");
        SetObjProperty(ev.source, "talkto", medata.minename);
        medata.talktome := ev.source;
      Else
        MoveItemToContainer(ev.item, ev.source.backpack);
      EndIf
    EndCase
      // }}}
  EndWhile
EndFunction

// Holt und setzt 'ne ObjProperty, ggf. auf Default
Function CheckObjProperty(me, property, defaultvalue := "") // {{{
  var value := GetObjProperty(me, property);
  If (value == error)
    value := defaultvalue;
    SetObjProperty(me, property, value);
  EndIf

  return (value);
EndFunction // }}}

// Verarbeitet alles Gesprochene
Function Event_Speech(me, ByRef medata, ev) // {{{
  var sayed := Lower(ev.text);
  var sayedarray := SplitWords(sayed);
  var sayedfirst := sayedarray[1];

  // Speechevent für Questnpc
  If ((GetObjProperty(me, "questnpc"))&&(sayed == "auftrag"))
    If (!GetObjProperty(me, "master"))
      var parms := {};
      parms[1] := me;
      parms[2] := ev.source;
      Start_ScriptPergon(":questsystem:questsystem", parms);
    EndIf
  EndIf

  // Hier mal der Check ob er einkaufen darf
  // Neues Bounting System für Stadtgardisten
  var wanted := ev.source.getprop("city_wanted");
  var place := lower(PlaceName(me));
  If (wanted[place])
     If (wanted[place] > ReadGameClock())
        NPCSayTextTraditional(me, "Ich verkaufe nicht an Stadtgardistenmoerder!", "I don't sell to police murderer!");
        return;
     Else
        wanted.erase(place);
     EndIf
  EndIf

  If (sayed == "fehler")
    syslog("HINWEIS: Bin ein Bug "+Lower(Hex(me.serial))+" '"+medata.minename+"' gefunden von "+Lower(Hex(ev.source.serial)));
    NPCSayTextTraditional(me, "Bin als boeser Bug markiert ...", "");
    return;
  EndIf

  If (sayed[Lower(medata.minename)])
    If (GetObjProperty(ev.source, "talkto")<>medata.minename)
      EraseObjProperty(ev.source, "trainmaxcost");
      EraseObjProperty(ev.source, "skillid");
      SetObjProperty(ev.source, "talkto", medata.minename);
    EndIf
    medata.talktome := ev.source;

    If (sayedarray[2])
      sayedfirst := sayedarray[2];
      sayed := sayedarray[2];

      For i := 3 To Len(sayedarray)
        sayed := sayed+" "+sayedarray[i];
      EndFor
    Else
      sayed := "hallo";
    EndIf
  Else
    AutoTalkTo(me, medata, ev.source);
  EndIf

  SleepMS(5);

  If (!GetObjProperty(ev.source, "townquest"))
    medata.minelocation := PlaceName(me); // ToDo: folgende Zeile is drinne, weil die wegrennen *g*
    SayTownQuest(ev.source, medata.minelocation);
    SetObjProperty(ev.source, "townquest", 1);
  EndIf

  If (!medata.talktome)
    return;
  EndIf

  If (!medata.sleeping)
    TurnToward(ev.source);
    If (meprototype.move_action)
      Hook_Move_Action(me, medata, ev);
    EndIf
  EndIf

  Case (sayed)
  "zeit": var time := GetPergonTimeofDay();
          NPCSayTextTraditional(me, "Es ist "+time, "It is "+time);
  "ort":  // medata.minelocation := PlaceName(ev.source); // ToDo: folgende Zeile is drinne, weil die wegrennen *g*
          NPCSayTextTraditional(me, "Ihr seid in "+PlaceName(ev.source), "");

  "offen":
    If (meprototype.speech_open)
      Hook_Speech_Open(me, medata, ev);
    Else
      NPCSayTextTraditional(me, "Zwischen "+medata.open+" und "+medata.close+" habe ich Zeit fuer Euch.",
        "My services are available from "+medata.open+" to "+medata.close+".");
    EndIf
    SetObjProperty(ev.source, "sleepmsg", 1);

  default:
    If (medata.sleeping and !IsBribedBy(me, ev.source))
      If (!GetObjProperty(ev.source, "sleepmsg"))
        Sleep(2);

        If (meprototype.speech_sleeping)
          Hook_Speech_Sleeping(me, medata, ev);
        Else
          NPCSayTextTraditional(me, "Tut mir leid, aber es ist Feierabend. Kommt doch zwischen "+medata.open+" und "+medata.close+" wieder.",
            "I'm sorry, but it's end of workday. Come back between "+medata.open+" and "+medata.close+".");
        EndIf
        SetObjProperty(ev.source, "sleepmsg", 1);
      EndIf
      return;
    EndIf

    If (medata.mute)
      If (ReadGameClock() >= medata.mute)
        medata.mute := 0;
      EndIf
      return;
    EndIf

    If (!CheckLosAt(me, ev.source.x, ev.source.y, ev.source.z+16))
      // Merchants sind nicht durch Waende ansprechbar
      Case (RandomInt(6)+1)
        1:
        2: PrintTextAbovePergon(ev.source, me, "Huh? Wer spricht da?");
        3: PrintTextAbovePergon(ev.source, me, "Meint Ihr mich?");
        4: PrintTextAbovePergon(ev.source, me, "Wo seid Ihr?");
        5: PrintTextAbovePergon(ev.source, me, "Nanu? Wo kommt die Stimme her?");
        6: PrintTextAbovePergon(ev.source, me, "Geisterstimmen ... zu Hilfeee");
      EndCase
      return;
    EndIf

    Case (sayed)
    "name":
      If (meprototype.speech_name)
        Hook_Speech_Name(me, medata, ev);
      Else
        NPCSayTextTraditional(me, "Einfach nur "+medata.minename, "");
      EndIf

    "trainiere":
    "trainieren":
      MerchantTrain(me, ev.source);

    "ruhe":
    "ruhig":
    "fresse":
    "schnauze":
    "klappe":
      medata.mute := ReadGameClock()+MUTE_TIME;
      NPCSayTextTraditional(me, "Ok, ich bin ja schon stumm ...", "");

    "musik": // Bissel Musik/Krach machender Barde {{{
      If (!(Lower(me.npctemplate) in {"bardm", "bardw"}))
        NPCSayTextTraditional(me, "Tut mir leid, damit kann ich Euch nicht dienen.", "");
        return;
      EndIf

      If (!GetObjProperty(me, "Playing"))
        SetObjProperty(me, "Playing", 1);
        NPCSayTextTraditional(me,
          "Danke. Dann werde ich Euch nun mit ein paar Musikstueckchen "+
          "erfreuen.", ""
        );
        Start_ScriptPergon(":musicianship:npc-music", {me, ev.source});
      Else
        EraseObjProperty(me, "Playing");
        NPCSayTextTraditional(me,
          "Wenn Ihr meine Musik nicht moegt, dann hoere ich eben auf.", ""
        );
      EndIf
      // }}}

    default:
      var proceeded := 0;
      If (meprototype.buynsold)
        // mehrere gleichzeitige Kaeufe/Verkaeufe vermeiden
        If (!CanBeServed(me, ev.source))
          PrintTextAbovePrivatePergon(
            me, "Tut mir leid, aber ich bediene schon einen Kunden.",
            "Sorry, I am already serving another customer.", ev.source
          );
          return;
        EndIf
        // Kaufen/Verkaufen behandeln
        proceeded := BuySell(sayed, ev.source, medata);
      EndIf

      If ((!proceeded) And meprototype.speech_default)
        proceeded := Hook_Speech_Default(me, medata, ev, sayed);
      EndIf

      If (!proceeded)
        // ToDo: folgende Zeile is drinne, weil die wegrennen *g*
        medata.minelocation := PlaceName(me);
        NPCSayTextTraditional(me, Answering(
          sayed, medata.minelocation, me
        ), "");
      EndIf
    EndCase
  EndCase
EndFunction // }}}

// Items kaufen/verkaufen vorbereiten
Function BuySell(sayed, who, byref medata) // {{{
  If (sayed == "kaufen") // {{{
    // Befindet sich der Spieler als Ex-Newbie auf Jhe'lom? {{{
    If (PlaceNewbie(who) And (!GetObjProperty(who, TYPNEWBIE)))
      NPCSayTextTraditional(me,
        "Tut mir leid, euch verkaufe ich nichts ... "+
        "Ihr seht nicht so aus, als ob ihr hier wohnen wuerdet.", ""
      );
      return 1;
    EndIf
    // }}}

    RemoveOldPetItems(who);

    // Bei sehr vollem Rucksack warnen
    If (!CountBackpackItemsAndWarn(who))
      NPCSayTextTraditional(me, "Dann eben nicht.", "");
      // aktuellen Kunden vergessen, da Handel erledigt
      me.eraseprop(PROP_CURRENT_CUSTOMER);
      return 1;
    EndIf

    If (meprototype.buyhook)
      Hook_Buy(me, medata, who);
    Else
      // Kauffenster oeffnen
      var res := SendBuyWindowPergon(who, medata.inv_fs, me, medata.inv_pb);
      If (!res)
        syslog(
          "FEHLER: "+medata.merchant_type+" "+me.name+
          " -> SendBuyWindowPergon - "+res.errortext
        );
      EndIf
    EndIf

    return 1; // proceeded
    // }}}

  ElseIf (sayed == "verkaufen") // {{{
    // Tier-Items im Backpack entfernen
    RemoveOldPetItems(who);

    // Nahbereich nach gezaehmten/abgerichteten Tieren fuer
    // Verkaufliste abgrasen
    // {{{
    var hideitems := {};
    If (meprototype.merchanttype == "Stable")
      var playerpets := struct;
      playerpets.+petserial := {};
      playerpets.+itemserial := {};
      ForEach pet in ListMobilesInLineOfSight(who, meprototype.threshold_enter)
        SleepMS(2);

        // unverkaeufliche Tiere ignorieren {{{
        If (pet.master <> who)
          // fremde Tier kann man nicht verkaufen
          continue;
        EndIf

        If (pet.getprop(PROP_SUMMONED))
          // beschworene Tiere kann man nicht verkaufen
          continue;
        EndIf

        If (!(pet.script in {"tamedanimal", "enticedanimal", "tamed"}))
          // weder zahm noch abgerichtet, also wild
          continue;
        EndIf

        // Ist es ein Jungtier?
        var youth := GetObjProperty(pet, "Young");
        If (youth)
          If (youth > ReadGameclock())
            // Jungtiere kann man nicht verkaufen
            continue;
          Else
            // Jugendzeit abgelaufen, kein Jungtier mehr
            EraseObjProperty(pet, "Young");
          EndIf
        EndIf
        // }}}

        var itemtype := 0;

        // Items fuer die Verkaufliste heraussuchen {{{
        var pettemplate := Lower(pet.npctemplate);
        var temparr:=PetTemplates();
        var maxp := Len(temparr);
        var i := 1;
        While (
          (i <= maxp) And
          (pettemplate<>lower(temparr[i]))
        )
          SleepMs(2);
          i += 1;
        EndWhile

        If (i <= maxp)
          If (pet.script == "tamedanimal")
            // Gezaehmte Tiere
            itemtype := PetTamedItem()[i];
          Else
            // Abgerichtete Tiere
            itemtype := PetEnticedItem()[i];
          EndIf
        EndIf
        // }}}

        If (!itemtype)
          // Zuordnung hat nicht funktioniert
          // TODO: passendes Syslog
          continue;
        EndIf

        // Item im Backpack fuer die Verkaufliste anlegen
        var item := CreateItemInBackpack(who, itemtype, 1);
        If (!item)
          // TODO: passendes Syslog
          continue;
        EndIf

        // Werte auf Ersatzitem uebertragen {{{
        item.name := pet.name;
        // Shinigami: ohne Farbe hat man weniger Probleme
        // item.color := pet.color;

        // Wenn es ein gezuechtetes Tier ist, Property weiterreichen
        // (fuer Preisberechnung)
        var generation := GetObjProperty(pet, "Generation");
        If (generation)
          SetObjProperty(item, "Generation", generation);
        EndIf
        // }}}

        playerpets.petserial.append(pet.serial);
        playerpets.itemserial.append(item.serial);
        hideitems.append(item);
      EndForEach

      // Kuenstlich zur Verkaufliste hinzugefuegte Items vermerken
      If (Len(playerpets.petserial)>0)
        SetObjProperty(who, "playerpets", playerpets);
      Else
        NPCSayTextTraditional(me, "Ihr habt keine Tiere dabei, die mich interessieren.", "");
      EndIf
    EndIf
    // }}}

    // Bei sehr vollem Rucksack warnen
    If (!CountBackpackItemsAndWarn(who))
      NPCSayTextTraditional(me, "Dann eben nicht.", "");
      // aktuellen Kunden vergessen, da Handel erledigt
      me.eraseprop(PROP_CURRENT_CUSTOMER);
      return 1;
    EndIf

    // Verkaufliste modifizieren (Preise etc. pp.)
    ModifyPCSellList(me.npctemplate, medata.merchant_type, who);

    // Erstellt LogInfos vorm Verkaufen
    // LogGoldProblemsBeforeSelling(who);

    // Goldmenge des Kunden merken
    var moneycheck := struct{
      customer := who.serial,
      gold     := who.gold
    };
    me.setprop("#MoneyCheckSell", moneycheck);

    // Verkauffenster oeffnen
    var res := SendSellWindowPergon(
      who, me, medata.inv_fs, medata.inv_pb, medata.inv_1c
    );

    // Kuenstlich hinzugefuegte Items im Backpack verstecken
    // (muss nach SendSellWindowPergon erfolgen)
    If (meprototype.merchanttype == "Stable")
      ForEach item in hideitems
        SleepMs(2);
        item.graphic := 1000;
      EndForEach
    EndIf

    If (res)
      NPCSayTextTraditional(me, "Was wollt ihr mir verkaufen?", "");
    Else
      syslog(
        "FEHLER: "+medata.merchant_type+" "+me.name+
        " -> SendSellWindowPergon - "+res.errortext
      );
    EndIf

    return 1; // proceeded
    // }}}
  EndIf

  return 0; // not proceeded
EndFunction // }}}

// bei Stallmeister (alte) Tier-Items im Backpack entfernen
Function RemoveOldPetItems(who) // {{{
  If (meprototype.merchanttype == "Stable")
    var tmp:=PetTamedItem()+PetEnticedItem();
    ForEach item in (EnumerateItemsInContainer(
        who.backpack, ENUMERATE_ROOT_ONLY
    ))
      SleepMS(2);
      If (item.objtype in (tmp))
        DestroyItem(item);
      EndIf
    EndForEach
    EraseObjProperty(who, "playerpets");
  EndIf
EndFunction // }}}

////////////////////////////////////////////////////////////////////
// Event_Ghost_Speech - Verarbeitet alles von Geistern Gesprochene
////////////////////////////////////////////////////////////////////
Function Event_Ghost_Speech(me, ByRef medata, ev)
  If (meprototype.ghost_speech_default)
    var sayed := Lower(ev.text);
    var sayedarray := SplitWords(sayed);
    var sayedfirst := sayedarray[1];

    If (sayed == "fehler")
      syslog("Bin ein Bug "+Lower(Hex(me.serial))+" '"+medata.minename+"' gefunden von "+Lower(Hex(ev.source.serial)));
      NPCSayTextTraditional(me, "Bin als boeser Bug markiert ...", "");
      return;
    EndIf

    If (sayedarray[1] == Lower(medata.minename))
      If (GetObjProperty(ev.source, "talkto")<>medata.minename)
        EraseObjProperty(ev.source, "trainmaxcost");
        EraseObjProperty(ev.source, "skillid");
        SetObjProperty(ev.source, "talkto", medata.minename);
      EndIf
      medata.talktome := ev.source;

      If (sayedarray[2])
        sayedfirst := sayedarray[2];
        sayed := sayedarray[2];

        For i := 3 To Len(sayedarray)
          sayed += " "+sayedarray[i];
        EndFor
      Else
        sayed := "hallo";
      EndIf
    Else
      var mobile := ListMobilesNearLocationEx(ev.source.x, ev.source.y, ev.source.z, meprototype.threshold_speech, LISTEX_FLAG_NORMAL, ev.source.realm);
      If (Len(mobile) == 2)
        If (((mobile[1].serial == me.serial) And (mobile[2].serial == (ev.source).serial)) Or
           ((mobile[2].serial == me.serial) And (mobile[1].serial == (ev.source).serial)))
          SetObjProperty(ev.source, "talkto", medata.minename);
          medata.talktome := ev.source;
        EndIf
      EndIf
    EndIf

    SleepMS(5);

    If (medata.talktome)
      TurnToward(ev.source);
      Hook_Ghost_Speech_Default(me, medata, ev, sayed);
    EndIf
  EndIf
EndFunction

// Wachen herbeirufen
Function CallGuards(who, text) // {{{
  If (text<>"")
    SayPergon(who, text);
  EndIf

  var event     := struct;
  event.+type   := EVID_GUARDS_HELP;
  event.+source := me;

  ForEach mobile in (ListMobilesNearLocation(
    me.x, me.y, me.z, meprototype.threshold_callguards, me.realm
  ))
    SleepMs(2);
    If (mobile.isa(POLCLASS_NPC))
      If (mobile.script == ":main_ai3:townguard")
        SendEvent(mobile, event);
      EndIf
    EndIf
  EndForEach
EndFunction // }}}

//////////////////////////////////////////////////////////////////////////
// PlayGiveItem - Simuliert die Uebergabe des Items mittels MovingEffekt
//////////////////////////////////////////////////////////////////////////
Function PlayGiveItem(source, dest, item, speed := 4)
  var height := 8;
  If (GetEquipmentByLayer(dest, 25))
    // reitet, ist daher hoeher
    height := 15;
  EndIf

  PlayMovingEffectXYZ(
    source.x, source.y, source.z+8, dest.x, dest.y, dest.z+height,
    item, speed, 0, 0, source.realm
  );
EndFunction

// Bei sehr vollem Rucksack warnen und Rueckfrage
Function CountBackpackItemsAndWarn(who) // {{{
  var count :=
    EnumerateItemsInContainer(who.backpack, ENUMERATE_ROOT_ONLY).size();
  // syslog("CountBackpackItemsAndWarn: "+count);
  If (!count)
    // FIXME: Sinnvoll? Vielleicht hat er kein Backpack
    count := 1;
  EndIf

  // 999 hoechste Zauberrollen bringen derzeit 42 Goldstacks
  If (count < 149 - 42)
    // keine Warnung, Handel erlauben
    return 1;
  EndIf

  return SendYesNoGump(who,
    "Euer Rucksack ist sehr voll, daher kann es zu Problemen beim "+
    "Handel kommen!<br>Wollt Ihr wirklich handeln?",
    // "Your backpack is very full.<br>"+
    // "There could be problems with trading!<br>"+
    // "Do you really want to trade?"
    10, 10, TE_CANCEL_ENABLE, TE_SCROLLBAR_ENABLE
  );
EndFunction // }}}

////////////////////////////////////////////////
// MerchantTrain - Bietet das Skilltraining an
////////////////////////////////////////////////
Function MerchantTrain(me, player)
  var layout := {"page 0", "nodispose"};
  var trainables := {};
  var data := {};

  // Character und Player nach Skills durchsuchen

  ForEach sorted in (GetSkillsSorted())
    var skillinfo:=GetSkillInfo(sorted);
    If (GetObjProperty(player, SKILLTYPKENNUNG+skillinfo.name) Or (player.cmdlevel >= CMDLEVEL_SEER))
      var npcskill := GetSkillFloatPergon(me, skillinfo.name);
      If (npcskill)
        var playerskill := GetSkillFloatPergon(player, skillinfo.name);
        If (playerskill<CInt(npcskill/3))
          trainables.append({skillinfo.TrainName, skillinfo.name, CInt(npcskill/3), playerskill});
        EndIf
      EndIf
    EndIf
    SleepMS(2);
  EndForEach

  // Kann der NPC den Character trainieren?
  If (trainables.size()>0)
    NPCSayTextTraditional(me, "Welchen Skill wollt ihr trainieren?", "Which skill you want to have trained?");

    // TrainWindow zusammenbasteln
    layout.append("resizepic 10 10 3500 320 "+((trainables.size()*20)+90));
    layout.append("text 40 30 50 0");
    data.append("Welchen Skill wollt ihr trainieren?");

    var index;
    For (index := 1; index <= trainables.size(); index += 1)
      layout.append("button 60 "+(index*20+54)+" 2104 2103 1 0 "+index);
      layout.append("text 80 "+(index*20+50)+" 0 "+data.size());
      data.append(trainables[index][1]);
    EndFor

    // TrainWindow aufrufen
    var result := CInt(SendDialogGump(player, layout, data)[0]);
    If (result>0)
      var trainable := trainables[result];
      SetObjProperty(player, "NPCTrain", {trainable[1], trainable[2]});

      var gold := GoldForSkillGain(trainable[3], trainable[4]);
      NPCSayTextTraditional(me, "Ich kann Euch in "+trainable[1]+" fuer maximal "+gold+" Gold trainieren.",
        "I can train you in "+trainable[1]+" For "+gold+" gold.");
    EndIf
  Else
    NPCSayTextTraditional(me, "Ich koennte Euch nichts mehr beibringen.", "");
  EndIf
EndFunction

///////////////////////////////////////////////////////
// MakeMerchantTrain - Fuehrt das Skilltraining durch
///////////////////////////////////////////////////////
Function MakeMerchantTrain(me, player, item) // {{{
  // diverse Abbruchgruende {{{
  If ((item.objtype <> UOBJ_GOLD_COIN) or (item.amount <= 0))
    // irgendwas unsinniges gegeben
    return 0;
  EndIf

  If (item.amount < 100)
    NPCSayTextTraditional(me,
      "Fuer die Menge Gold lohnt sich kein Training. "+
      "Bitte bringt mehr Gold vorbei.", ""
    );
    return 1;
  EndIf

  ReserveItem(item);

  var npctrain := GetObjProperty(player, "NPCTrain");
  If (!npctrain)
    NPCSayTextTraditional(me, "Ich weiss zwar nicht wofuer dies ist, aber danke!", "");
    DestroyItem(item);
    return 1;
  EndIf

  var npcskill := GetSkillFloatPergon(me, npctrain[2]);
  If (!npcskill)
    NPCSayTextTraditional(me, "Ich kenne mich im Skill "+npctrain[1]+" nicht aus.", "");
    MoveItemToContainer(item, player.backpack);
    ReleaseItem(item);
    return 1;
  EndIf

  npcskill := CInt(npcskill/3.0);
  var playerskill := GetSkillFloatPergon(player, npctrain[2]);
  If (playerskill >= npcskill)
    NPCSayTextTraditional(me,
      "Ich bin in "+npctrain[1]+" nicht besonders gut. "+
      "Sucht Euch einen anderen Trainer.", ""
    );
    MoveItemToContainer(item, player.backpack);
    ReleaseItem(item);
    return 1;
  EndIf
  // }}}

  // abhaengig vom uebergebenen Gold Skillanstieg und Text ermitteln {{{
  var trainmaxcost := GoldForSkillGain(npcskill, playerskill);
  var closetext := {"Die Bezahlung stimmt genau.", ""};
  If (item.amount < trainmaxcost)
    closetext := {
      "Ich werde Euch weitertrainieren, wenn ihr mehr Gold vorbeibringt.",
      "I will give your more training, If you give me more gold."
    };
    // Skillziel berechnen
    playerskill := SkillGainForGold(npcskill, playerskill, item.amount);
  Else
    If (item.amount > trainmaxcost)
      closetext := {"Dankeschoen fuer Euer Trinkgeld.", ""};
    EndIf

    // Skillziel einfach nur setzen
    playerskill := npcskill;
    // Wert loeschen, da austrainiert
    EraseObjProperty(player, "NPCTrain");
  EndIf
  // }}}

  // Player trainieren {{{
  // Geld einstecken
  DestroyItem(item);

  // etwas Zeit verbraten
  NPCSayTextTraditional(me, "Nun gut, ich werde Euch trainieren.", "");
  If (RandomInt(2) == 1)
    Sleep(8);
    NPCSayTextTraditional(me,
      "Stellt Euch nicht so an, so schwer ist es nicht!",
      "Don't act like a child, it isn't that difficult!"
    );
  EndIf
  Sleep(5);
  If (RandomInt(2) == 1)
    NPCSayTextTraditional(me,
      "Ihr wart ein gelehriger Schueler.",
      "You were a teachable apprentice."
    );
  EndIf

  // Skill des Player erhoehen
  SetSkillPergon(player, npctrain[2], playerskill);
  ShowSkillAdvMsgPergon(player, npctrain[2]);
  Sleep(3);
  // }}}

  // Abschlusstext aufsagen
  NPCSayTextTraditional(me, closetext[1], closetext[2]);

  return 1;
EndFunction // }}}

// Maximal Kosten fuer das Skilltraining
Function GoldForSkillGain(npcskill, playerskill) // {{{
  return CInt((Pow(npcskill, 3)-Pow(playerskill, 3))*2.192);
EndFunction // }}}

// Auf welchen Wert wird mein Skill ansteigen
Function SkillGainForGold(npcskill, playerskill, trainmaxcost) // {{{
  return Min(
    Pow(trainmaxcost/2.192+Pow(playerskill, 3), 1.0/3),
    // NPC kann nicht mehr beibringen, als er selbst kann
    npcskill
  );
EndFunction // }}}

// Kohle beim Player abziehen
Function GetPayment(me, player, price)
  If (price)
    If (player.gold >= price)
      player.spendgold(price);
    Else
      NPCSayTextTraditional(me, "Ihr habt keine "+price+" Goldstuecke!", "");
      return (1);
    EndIf
  EndIf
EndFunction

////////////////////////////////////////////////////////////////////
// LogGoldProblemsBeforeSelling - Erstellt LogInfos vorm Verkaufen
////////////////////////////////////////////////////////////////////
Function LogGoldProblemsBeforeSelling(who)
  var items := {};
  var price := 0;

  ForEach item in EnumerateItemsInContainer(who.backpack, ENUMERATE_ROOT_ONLY)
    items.append({item.objtype, item.amount, item.buyprice});
    price := price+(item.buyprice*item.amount);
    SleepMS(2);
  EndForEach

  SetObjProperty(who, "TestLogGoldItems", items);
  SetObjProperty(who, "TestLogGoldPrice", price);
EndFunction

////////////////////////////////////////////////////////////////////
// LogGoldProblemsAfterSelling - Erstellt LogInfos nachm Verkaufen
////////////////////////////////////////////////////////////////////
Function LogGoldProblemsAfterSelling(who, amount)
  If (amount>1000000) // Mehr als 1 Mio Einnahmen pro Verkauf sind schon ziemlich viel, verdaechtig
    var price := 0;

    syslog("DEBUG: LogGoldProblemsAfterSelling "+who.name+" ["+Lower(Hex(who.serial))+"] / amount = "+amount);
    syslog("  Vorher...");
    ForEach item in GetObjProperty(who, "TestLogGoldItems")
      syslog("    "+item[2]+" von "+Lower(Hex(item[1]))+" zu "+item[3]+"gc");
    EndForEach
    syslog("  Preis = "+GetObjProperty(who, "TestLogGoldPrice"));

    syslog("  Nachher...");
    ForEach item in EnumerateItemsInContainer(who.backpack, ENUMERATE_ROOT_ONLY)
      syslog("    "+item.amount+" von "+Lower(Hex(item.objtype))+" zu "+item.buyprice+"gc, ");
      price := price+(item.buyprice*item.amount);
      SleepMS(2);
    EndForEach
    syslog("  Preis = "+price);
  EndIf

  EraseObjProperty(who, "TestLogGoldItems");
  EraseObjProperty(who, "TestLogGoldPrice");
EndFunction

// sicherstellen, dass immer nur ein Kunde bedient wird
Const SERVE_OK := 1;
Const SERVE_NO := 0;
Function CanBeServed(merch, cust) // {{{
  var current := merch.getprop(PROP_CURRENT_CUSTOMER);
  // Trivialfall: kein Kunde bisher bzw. Handel korrekt abgeschlossen
  If (!current)
    SetNewCustomer(merch, cust);
    return SERVE_OK;
  EndIf

  // es gibt vielleicht einen Käufer
  var active := SystemFindObjectBySerial(current.customer);

  If (!active or !active.connected or active.dead)
    // ausgeloggt oder tot -> neuer Kunde darf
    SetNewCustomer(merch, cust);
    return SERVE_OK;
  EndIf

  If (active.serial == cust.serial)
    // mehrfach "kaufen" gesagt? Ignorieren und erlauben
    return SERVE_OK;
  EndIf

  If (
    (Distance(merch, active) > 5) or
    active.hidden or (active.concealed > merch.concealed) or
    !CheckLineOfSight(merch, active)
  )
    // alter Kunde ist "weg" -> neuer Kunde darf
    SetNewCustomer(merch, cust);
    return SERVE_OK;
  EndIf

  If (current.time + 5*60 < ReadGameClock())
    // laenger als 10 Minuten her? Timeout, neuer Kunde darf
    SetNewCustomer(merch, cust);
    return SERVE_OK;
  EndIf

  return SERVE_NO;
EndFunction // }}}

// Hilffunktion fuer CanBeServed zum Festlegen des aktuellen Kundens
Function SetNewCustomer(merch, cust) // {{{
  var current := struct{
    customer := cust.serial,
    time     := ReadGameClock()
  };
  merch.setprop(PROP_CURRENT_CUSTOMER, current);
EndFunction // }}}

// Bestechbarkeit von NPCs (bedienen auch, wenn der Laden dicht ist)
Const BRIBE_OK := 1;
Const BRIBE_NO := 0;
Const BRIBE_FEW := 2;
// pruefen, ob Kunde cust den Haendler merch bestochen hat
Function IsBribedBy(merch, cust) // {{{
  var current := cust.getprop(PROP_CURRENT_BRIBE);

  If (!current)
    // Kunde hat gar nicht bestochen
    return BRIBE_NO;
  EndIf

  If (current.npc <> merch.serial)
    // Kunde hat zwar bestochen, aber jemand anderen
    return BRIBE_NO;
  EndIf

  If (current.time + 15*60 < ReadGameClock())
    // bestochen, aber laenger als 15 Minuten her?
    return BRIBE_NO;
  EndIf

  // alle Fallstricke ueberwunden, Bestechung wirkt
  return BRIBE_OK;
EndFunction // }}}

// Kunde cust will Haendler merch mit Item gold bestechen
Function BribeMe(merch, cust, gold) // {{{
  If (gold.objtype <> UOBJ_GOLD_COIN)
    // Bestechen geht derzeit nur mit Gold
    MoveItemToContainer(gold, cust.backpack);
    return BRIBE_NO;
  EndIf

  var config := GetNPCConfig(merch.npctemplate);
  If (!config.bribecost)
    // dieser NPC ist nicht bestechbar
    MoveItemToContainer(gold, cust.backpack);
    return BRIBE_NO;
  EndIf

  // Basiswert, unter 1000 Gold geht nichts
  var cost := config.bribecost * 1000;

  // TODO: Charisma einrechnen
  // var cha := GetCharisma(cust);
  // cost := tollerechnung;

  If (gold.amount < cost)
    // zu wenig Gold gegeben
    MoveItemToContainer(gold, cust.backpack);
    // spezieller Returncode, damit Haendler empoert reagieren kann
    return BRIBE_FEW;
  EndIf

  // alles ist gut, Gold nehmen und Marke setzen
  var bribed := struct;
  bribed.time := ReadGameClock();
  bribed.npc  := merch.serial;

  cust.setprop(PROP_CURRENT_BRIBE, bribed);

  DestroyItem(gold);
  return BRIBE_OK;
EndFunction // }}}

// vim: sw=2 sts=2
