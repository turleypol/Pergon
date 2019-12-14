Use file;
Include "include/place";
Include "include/eventid";

Const EVENTQUEUESIZE_SPAWN:=5000;

Enum CPROPS_PROPS
  PID_SPAWNSYSTEM:="#PID_SPAWNSYSTEM",
  CPROP_DATA     :="spawndata",
  CPROP_USER     :="userdata",
  CPROP_NR       :="spawnnet"
EndEnum

Enum Datafiles
  FILE_REGIONUSER  :=":spawnnet_new:RegionsUser",
  FILE_REGIONGROUPS:=":spawnnet_new:RegionsGroup",
  FILE_RUNEPOS     :=":spawnnet_new:RunePos"
EndEnum

Enum Datafile_Stuff
  D_GROUP_ENTRIES :="EntryList",
  D_GROUP_TYPE    :="Spawntype",
  D_GROUP_FLAGS   :="Flags",
  D_GROUP_GROUPING:="GroupingAmt",
  D_GROUP_STACK   :="StackAmount",
  D_CONTAINER_TYPE:="Containerobjtype"
EndEnum

Enum Types
  TYPE_NPC:=1,
  TYPE_ITEM,
  TYPE_CONTAINER
EndEnum

Enum BinaryFlags
  GROUPING               :=0x01, //NPC,Item
  SAVE_OLD_ITEMS         :=0x02, //Item
  NPC_ANCHOR             :=0x04,
  NPC_FROZEN             :=0x08,
  ITEM_IN_CONTAINER_SPAWN:=0x10,
  CONTAINER_MOVING_SPAWN :=0x20,
  CONTAINER_FLUSH        :=0x40,
  CONTAINER_TRAP         :=0x80
EndEnum

///////////////////////////////////////////////////////////
// DoSendEventWaiter - Schickt Event zum zentralen Script
///////////////////////////////////////////////////////////

Function SendSpawnNet(source, spawnnr, type, oldtype:=0, del:=0)
  Var waiter:=GetProcess(GetGlobalProperty(PID_SPAWNSYSTEM));
  While (!waiter)
    Sleep(5);
    waiter:=GetProcess(GetGlobalProperty(PID_SPAWNSYSTEM));
  EndWhile
  Var event:=struct;
  event.+source:=source;
  event.+type:=type;
  event.+nr:=spawnnr;
  If (oldtype)
    event.+oldtype:=oldtype;
  EndIf
  If (del)
    If (IsRegion(spawnnr))
      event.+spawndata:=del;
    Else
      event.+spawndata:=source.getprop(CPROP_DATA);
      event.+oldplace:=PlaceName(source);
      event.+oldrealm:=source.realm;
      event.oldplace:=StrReplace(event.oldplace," ","_");
    EndIf
  EndIf

  While ((waiter.sendevent(event)).errortext=="Event queue is full, discarding event")
    SleepMS(RandomInt(100)+2);
    LogToFile("::log/spawn.log","FEHLER: SpawnNetEventQueue voll (EventQueueSize = "+EVENTQUEUESIZE_SPAWN+")!");
  EndWhile
EndFunction


///////////////////////////////////////////////////////////
// SpawnNet_Clear - NPC aus Spawnnet entfernen (tod,tame,..)
///////////////////////////////////////////////////////////

Function SpawnNet_Clear(corpse)
  Var spawnnet:=corpse.getprop(CPROP_NR);
  If (spawnnet)
    var type:=corpse.getprop(LOG_LASTDAMAGE);  // Damagetypes die nicht interessieren (Runenzerstörung etc)
    If (type)
      If (type[1] in {"Kill [spawnnet-destroyregiongroup]",
                      "Kill [spawnnet-destroyregion]",
                      "Kill [spawnnet-destroyrune]",
                      "Kill [spawnnet-respawn]"})
        Return;
      EndIf
    EndIf
    If (corpse.isA(POLCLASS_CORPSE))
      SendSpawnNet(corpse.ownerserial, spawnnet[1], EVID_NPC_DIED);
    Else
      SendSpawnNet(corpse.serial, spawnnet[1], EVID_NPC_DIED);
      corpse.eraseprop(CPROP_NR);  // nureinmal abmelden
    EndIf
    Return(1);
  EndIf
  Return(0);
EndFunction

// versuchen, sich zu seiner Spawnrune zu begeben
Function ReturnToSpawnRune(mob) // {{{
  var hasrune := mob.getprop(CPROP_NR);
  If (hasrune)
    If (!IsRegion(hasrune[1]))
      var myrune := SystemFindObjectBySerial(hasrune[1]);
      If (myrune)
        return MoveObjectToLocation(
          mob, myrune.x, myrune.y, myrune.z, myrune.realm,
          MOVEOBJECT_FORCELOCATION
        );
      EndIf
    Endif
  EndIf

  return 0;
EndFunction // }}}

///////////////////////////////////////////////////////////
// IsRegion - Ist es ne Region oder ne normale Rune?
///////////////////////////////////////////////////////////

Function IsRegion(spawnid)
  If (TypeOfInt(spawnid)==OT_STRING)
    Return(1);
  ElseIf (TypeOfInt(spawnid)==OT_ARRAY);
    Return(1);
  Else
    Return(0);
  EndIf
EndFunction
