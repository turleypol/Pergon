//////////////////////////////////////////////////////////////////
// NonMerchant-Prototype v1.0 (2000/03/29)
//
// based on Stable Keeper Script v0.18 (1999/11/30)
// Author: RoTTer
//
// Author: Shinigami

//////////////////////////
// Linking-Anforderungen
//////////////////////////
//
// Der, diese Datei benutzende, Quelltext muss folgende Funktion enthalten:
//
//   Function SetNonMerchantType(meprototype)
//     [...]
//     Return (meprototype);
//   EndFunction

use math;
use os;
use uo;
include "include/animal";
include "include/bank";
include "include/client";
include "include/clock";
include "include/eventid";
include "include/msgs";
include "include/names";
include "include/npcwalkpath";
include "include/place";
include "include/vetement"; // random clothing

//////////////////////
// Typendefinitionen
//////////////////////

Var meprototype:=struct;
       meprototype.+nonmerchanttype:="";
       meprototype.+withopenclose:=1;
       meprototype.+threshold_enter:=7;
       meprototype.+threshold_leave:=meprototype.threshold_enter+1;
       meprototype.+threshold_speech:=3;
       meprototype.+threshold_callguards:=21;
       // meprototype.+move_action:=0;
       // meprototype.+entered_area:=0;
       // meprototype.+left_area:=0;
       // meprototype.+speech_open:=0;
       // meprototype.+speech_sleeping:=0;
       // meprototype.+speech_name:=0;
       // meprototype.+speech_default:=0;

///////////////
// Konstanten
///////////////

Const MUTE_TIME:=180;

//////////////////////
// Globale Variablen
//////////////////////

Var me:=Self();

//////////////////////////////////
// NonMerchant - Initialisierung
//////////////////////////////////

Program NonMerchant()
  meprototype:=SetNonMerchantType(meprototype);

  Var medata:=struct; // Informationen ueber mich selber sammeln

  SetObjProperty(me, "serial", me.serial);

  If (GetObjProperty(me, "frozen"))
    me.frozen:=1;
    IncRevision(me);
  EndIf

  If (me.name["<random>"])
    Var npcname:=RandomName(me);
    SetName(me, npcname);
    SetName(me, npcname+NPCTitle(me));

    npcname:=me.name+" ";

    Var pos:=Find(npcname, ",", 1);
    If (pos)
      npcname:=npcname[1, pos-1];
    EndIf

    SetObjProperty(me, "myname", npcname);

    Habille(me, GetObjProperty(me, "Equipt")); // Random Clothing

    If (!me.backpack)
      Var newbackpack:=CreateItemAtLocationPergon(me.x, me.y, me.z, 0xe75, 1,me.realm);
      If (!EquipItem(me, newbackpack))
        SysLog("WARNUNG: Merchant "+Lower(Hex(me.serial))+" mag kein Backpack!");
      EndIf
    EndIf
  EndIf

  medata.+minename:=GetObjProperty(me, "myname");
  medata.+minelocation:=PlaceName(me);
  medata.+open:=CheckObjProperty(me, "open", "06:00");
  medata.+close:=CheckObjProperty(me, "close", "20:00");
  medata.+sleeping:=0;
  medata.+talktome:=0;
  medata.+mute:=0;
  medata.+npcpath:=0;

  SetWarMode(0);
  DropAnchor();

  NonMerchant_MainLoop(me, medata); // Hauptschleife aufrufen
EndProgram

/////////////////////////////////////////
// NonMerchant_MainLoop - Hauptschleife
/////////////////////////////////////////

Function NonMerchant_MainLoop(me, medata)
  Set_Priority(10);

  EnableEvents(
    SYSEVENT_DAMAGED+SYSEVENT_DISENGAGED+SYSEVENT_ENGAGED+
    SYSEVENT_ITEM_GIVEN+SYSEVENT_SPEECH, meprototype.threshold_speech
  );
  EnableEvents(SYSEVENT_ENTEREDAREA, meprototype.threshold_enter);
  EnableEvents(SYSEVENT_LEFTAREA, meprototype.threshold_leave);
  EnableEvents(SYSEVENT_OPPONENT_MOVED);

  Var next_wander:=ReadGameClock()+10;

  While (1)
    PerformAction(me, UACTION_LOOK_AROUND);

    Var ev;
    If (GetObjProperty(me, "NPCPath"))
      medata.npcpath:=1;
      ev:=Wait_for_EventLogging(1);
    Else
      medata.npcpath:=0;
      ev:=Wait_for_EventLogging(120);
    EndIf

    If ((ev.source).isa(POLCLASS_MOBILE))
      If ((ev.source).isa(POLCLASS_NPC)) // NPC-Events
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
      ElseIf (!ev.source.dead) // Player-Events
        If (GetObjProperty(ev.source, "talkto")==medata.minename)
          medata.talktome:= ev.source;
        Else
          AutoTalkTo(me, medata, ev.source);
        EndIf

        Var los:=CheckLineOfSight(me, ev.source);
        If (los)
          Var daytime:=GetPergonTimeOfDay();
          If (medata.open<medata.close)
            If ((daytime<medata.open) Or (daytime>medata.close))
              medata.sleeping:=1;
            Else
              medata.sleeping:=0;
            EndIf
          Else
            If ((daytime<medata.open) And (daytime>medata.close))
              medata.sleeping:=1;
            Else
              medata.sleeping:=0;
            EndIf
          EndIf

          If ((ev.type<>SYSEVENT_SPEECH) And (!medata.sleeping))
            If (ev.type==SYSEVENT_ENTEREDAREA)
              SleepMS(RandomInt(2000));
            EndIf
            TurnToward(ev.source);
            If (meprototype.move_action)
              Hook_Move_Action(me, medata, ev);
            EndIf
          EndIf
        EndIf

        Case (ev.type)
          SYSEVENT_ENTEREDAREA:

            If (los)
              If (!medata.sleeping)
                If (meprototype.entered_area)
                  Hook_Entered_Area(me, medata, ev);
                Else
                  // folgende Zeile is drinne, weil die wegrennen *g*
                  medata.minelocation:=PlaceName(me);
                  NPCSayTextTraditional(me, Answering(
                    "seid_gegruesst", medata.minelocation, me
                  ), "");
                EndIf
              EndIf
            EndIf

          SYSEVENT_LEFTAREA:
            If (medata.talktome)
              If (los)
                If (!medata.sleeping)
                  SleepMS(RandomInt(1000));
                  If (meprototype.left_area)
                    Hook_Left_Area(me, medata, ev);
                  Else
                    If (meprototype.withopenclose)
                      NPCSayTextTraditional(me,
                        "Empfehlt uns bitte weiter", "",
                        meprototype.threshold_leave + 5
                      );
                    Else
                      NPCSayTextTraditional(me,
                        "Es war nett, mit Euch zu plaudern", "",
                        meprototype.threshold_leave + 5
                      );
                    EndIf
                  EndIf
                EndIf
              EndIf

              EraseObjProperty(ev.source, "sleepmsg");
              EraseObjProperty(ev.source, "talkto");
              medata.talktome:=0;
            EndIf
            EraseObjProperty(ev.source, "townquest");

          SYSEVENT_SPEECH:
            if (me.getprop("questnpc")&&(lower(ev.text) == "auftrag"))
              if (!me.getprop("master"))
                var parms:={};
                parms[1]:=Me;
                parms[2]:=ev.source;
                Start_ScriptPergon(":questsystem:questsystem",parms);
              endif
            endif
            If (los && len(ev.text) >= 1)
              If (!(ev.text[1] == "*" && ev.text[len(ev.text)] =="*"))  // auf Emotes nicht reagieren
                Event_Speech(me, medata, ev);
              EndIf
            EndIf


          SYSEVENT_ENGAGED:
            CallGuards(ev.source, "Aaahhh! Hilfe! Hilfe! Man greift mich an!");
            Run(ev.source, meprototype.threshold_enter);

          SYSEVENT_DAMAGED:
            CallGuards(ev.source, "Was habe ich nur getan. Zu Hilfe, Wachen!!!");
            If (ev.source)
              Run(ev.source, meprototype.threshold_enter);
            Else
              SayPergon(ev.source, "So viele gegen mich alleine? Hilfe!!!");
            EndIf
          SYSEVENT_ITEM_GIVEN:
            HandleGivenCheque(me, ev);
        EndCase
      EndIf
    EndIf

    If (medata.npcpath)
      If (!medata.talktome)
        NPCWalkPath(me);
      EndIf
      SleepMS(10);
    EndIf

    If (ReadGameClock()>=next_wander)
      next_wander:=ReadGameClock()+10;
      If ((!medata.npcpath) And (!medata.talktome))
        WanderPergon();
      EndIf
    EndIf
  EndWhile
EndFunction

//////////////////////////////////////////////////////
// CheckObjProperty - Holt und setzt 'ne ObjProperty
//////////////////////////////////////////////////////

Function CheckObjProperty(me, property, defaultvalue:="")
  Var value:=GetObjProperty(me, property);
  If (!value)
    value:=defaultvalue;
    SetObjProperty(me, property, value);
  EndIf

  Return (value);
EndFunction

/////////////////////////////////////////////////
// Event_Speech - Verarbeitet alles Gesprochene
/////////////////////////////////////////////////

Function Event_Speech(me, ByRef medata, ev)
  Var sayed:=Lower(ev.text);
  Var sayedarray:=SplitWords(sayed);
  Var sayedfirst:=sayedarray[1];

  If (sayed=="fehler")
    SysLog("Bin ein Bug "+Lower(Hex(me.serial))+" '"+medata.minename+"' gefunden von "+Lower(Hex(ev.source.serial)));
    NPCSayTextTraditional(me, "Bin als boeser Bug markiert...", "");
    Return;
  EndIf

  If (sayed[Lower(medata.minename)])
    SetObjProperty(ev.source, "talkto", medata.minename);
    medata.talktome:=ev.source;

    If (sayedarray[2])
      sayedfirst:=sayedarray[2];
      sayed:=sayedarray[2];

      Var i;
      For (i:=3; i<=Len(sayedarray); i+=1)
        sayed:=sayed+" "+sayedarray[i];
      EndFor
    Else
      sayed:="hallo";
    EndIf
  Else
    AutoTalkTo(me, medata, ev.source);
  EndIf

  SleepMS(5);

  If (!GetObjProperty(ev.source, "townquest"))
    medata.minelocation:=PlaceName(me); // ToDo: folgende Zeile is drinne, weil die wegrennen *g*
    SayTownQuest(ev.source, medata.minelocation);
    SetObjProperty(ev.source, "townquest", 1);
  EndIf

  If (!medata.talktome)
    // redet nicht mit mir, Klappe halten
    return;
  EndIf

  If (!medata.sleeping)
    TurnToward(ev.source);
    If (meprototype.move_action)
      Hook_Move_Action(me, medata, ev);
    EndIf
  EndIf

  Case (sayed)
    "zeit": NPCSayTextTraditional(me, "Es ist "+GetPergonTimeofDay(), "");
    "ort": //medata.minelocation:=PlaceName(me); // ToDo: folgende Zeile is drinne, weil die wegrennen *g*
            NPCSayTextTraditional(me, "Ihr seid in "+PlaceName(ev.source), "");

    "offen":
      If (meprototype.speech_open)
        Hook_Speech_Open(me, medata, ev);
      Else
        If (meprototype.withopenclose)
          NPCSayTextTraditional(me, "Zwischen "+medata.open+" und "+medata.close+" habe ich Zeit fuer Euch.", "");
        Else
          // ToDo: folgende Zeile is drinne, weil die wegrennen *g*
          medata.minelocation:=PlaceName(me);
          NPCSayTextTraditional(me, Answering(sayed, medata.minelocation, me), "");
        EndIf
      EndIf
      SetObjProperty(ev.source, "sleepmsg", 1);

    default:
      If (medata.sleeping)
        If (!GetObjProperty(ev.source, "sleepmsg"))
          Sleep(2);

          If (meprototype.speech_sleeping)
            Hook_Speech_Sleeping(me, medata, ev);
          Else
            If (meprototype.withopenclose)
              NPCSayTextTraditional(me, "Tut mir leid, aber es ist Feierabend. Kommt "+
                "doch zwischen "+medata.open+" und "+medata.close+" wieder.", "");
            ElseIf (RandomInt(100)>70)
              NPCSayTextTraditional(me, "Irgendwie fuehle ich mich schlaefrig.", "");
            EndIf
          EndIf
          SetObjProperty(ev.source, "sleepmsg", 1);
        EndIf
      ElseIf (!medata.mute)
        Case (sayed)
          "name":
            If (meprototype.speech_name)
              Hook_Speech_Name(me, medata, ev);
            Else
              NPCSayTextTraditional(me, "Einfach nur "+medata.minename, "");
            EndIf

          "ruhe":
          "fresse":
          "schnauze":
          "klappe":
            medata.mute:=ReadGameClock()+MUTE_TIME;
            NPCSayTextTraditional(me, "Ok, ich bin ja schon stumm...", "");

          default:
            Var proceeded:=0;
            If (meprototype.speech_default)
              proceeded:=Hook_Speech_Default(me, medata, ev, sayed);
            EndIf

            If (proceeded==0)
              // ToDo: folgende Zeile is drinne, weil die wegrennen *g*
              medata.minelocation:=PlaceName(me);
              NPCSayTextTraditional(me, Answering(sayed, medata.minelocation, me), "");
            EndIf
        EndCase
      Else
        If (ReadGameClock()>=medata.mute)
          medata.mute:=0;
        EndIf
      EndIf
  EndCase
EndFunction

////////////////////////////////////
// CallGuards - Wachen herbeirufen
////////////////////////////////////

Function CallGuards(who, text)
  If (text<>"")
    SayPergon(who, text);
  EndIf

  Var event:=struct;
  event.+type:=EVID_GUARDS_HELP;
  event.+source:=me;

  ForEach mobile in ListMobilesNearLocation(me.x, me.y, me.z, meprototype.threshold_callguards,me.realm)
    If (mobile.isa(POLCLASS_NPC))
      If (mobile.script == ":main_ai3:townguard")
        SendEvent(mobile, event);
      EndIf
    EndIf
  EndForEach
EndFunction

// Baenker sollen Schecks annehmen
Function HandleGivenCheque(npc, byref ev)
  If (!(me.script["banker"]))
    NPCSayTextTraditional(npc, "Seh ich aus wie 'ne Bank?", "I'm not a bank!");
    return;
  EndIf

  If (ev.item.objtype == UOBJ_GOLD_COIN)
    NPCSayTextTraditional(npc,
      "Gold koennt Ihr einzahlen oder ausgeben, "+
      "aber mir nicht einfach so geben.",
      "You may bank your gold or spend it. Don't just give it to me!"
    );
    return;
  EndIf

  If (ev.item.objtype <> 0x66b2)
    NPCSayTextTraditional(npc, "Bin ich ein Pfandleiher?", "I'm not a pawnbroker!");
    return;
  EndIf

  If (!ReserveItem(ev.item))
    NPCSayTextTraditional(npc,
      "Den Scheck scheint schon jemand zu benutzen.",
      "This cheque is already in use."
    );
    return;
  EndIf

  var amount := CInt(ev.item.getprop(CPROP_GOLD_ACCT));
  If (TryToGetPayment(ev.source, -1*amount))
    PrintTextAbovePrivatePergon(npc,
      "Ich habe Euch "+amount+" Goldstuecke gutgeschrieben.",
      "I booked "+amount+" gold coins onto your account.",
      ev.source
    );
    // nach dem Einzahlen vernichten
    DestroyItem(ev.item);
  Else
    NPCSayTextTraditional(npc,
      "Mit dem Scheck stimmt etwas nicht.",
      "Something is wrong about this cheque."
    );
    ReleaseItem(ev.item);
  EndIf
EndFunction
// vim: sw=2 sts=2
