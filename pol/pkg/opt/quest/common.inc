///////////////////////////////////////////////////////////////////////////
// common - Allgemeine Funktionen fuer Questitems

use uo;
include "include/msgs";
include "include/server";
include "include/string";

Const GPROP_FINDITEM_LIST := "finditem";
Const GPROP_FINDITEM_KILL := "finditemkill";

// find list manipulation
// Structure:
// dict{                          - list
//   item.objtype: dict{          - mytype
//     item.serial: dict{         - finders
//       finder.serial: GameClock

// register item in find list, add a finder
Function FindAddToList(byref list, byref item, finder := 0) // {{{
    If (!list)
        // init main list
        list := dictionary{};
    EndIf

    var mytype := dictionary;
    If (list.exists(item.objtype))
        // load type list
        mytype := list[item.objtype];
    EndIf

    var finders := dictionary;
    If (mytype.exists(item.serial))
        finders := mytype[item.serial];
    EndIf

    var added := 0;
    If (finder)
        If (!finders.exists(finder.serial))
            finders.insert(finder.serial, ReadGameClock());
            added := 1;
        EndIf
    EndIf

    mytype[item.serial] := finders;
    list[item.objtype] := mytype;

    return added;
EndFunction // }}}

// reset the find list of currently selected objtype
Const FIND_RESET_NODELETE := 0; // Reset, keep items
Const FIND_RESET_DELETE   := 1; // Reset and delete items
Function FindResetType(type, delete := FIND_RESET_NODELETE) // {{{
    var list;

    If (delete)
        // do not delete critical as it will give high load
        // (chances of find item creation while deleting is small)
        list := GetGlobalProperty(GPROP_FINDITEM_LIST);
        ForEach serial in (list[type].keys())
            SleepMs(2);
            var item := SystemFindObjectBySerial(
                serial, SYSFIND_SEARCH_OFFLINE_MOBILES
            );
            DestroyItem(item);
        EndForEach
    EndIf

    // change list critical
    var crit := Is_Critical();
    Set_Critical(1);

    list := GetGlobalProperty(GPROP_FINDITEM_LIST);
    var result := list.erase(type);
    SetGlobalProperty(GPROP_FINDITEM_LIST, list);
    Set_Critical(crit);

    return result;
EndFunction // }}}

// helper to order statistic data of find items
Function FindGenStat(byref mytype) // {{{
    var stat := dictionary;
    var def_entry := struct;
    def_entry["serial"] := 0;
    def_entry["amount"] := 0;
    def_entry["time"]   := 0;

    ForEach itemserial in (mytype.keys())
        var finders := mytype[itemserial];
        ForEach userserial in (finders.keys())
            SleepMs(2);
            // prepare entry
            var entry := stat[userserial];
            If (!entry)
                entry := def_entry;
                entry["serial"] := userserial;
            EndIf

            // increment counter, find max time
            entry["amount"] += 1;
            If (finders[userserial] > entry["time"])
                entry["time"] := finders[userserial];
            EndIf

            // save
            stat[userserial] := entry;
        EndForEach
    EndForEach

    // re-order to ease sorting
    var sorthelper := dictionary;
    ForEach serial in (stat.keys())
        SleepMs(2);

        // make a dictionary of amounts containing a dictionary of times
        var times := dictionary;
        If (sorthelper[stat[serial].amount])
            times := sorthelper[stat[serial].amount];
        EndIf

        If (!times[stat[serial].time])
            times[stat[serial].time] := array;
        EndIf

        // the dictionarys of time contain arrays with all players
        // which gained the same amount and time
        times[stat[serial].time].append(stat[serial]);

        sorthelper[stat[serial].amount] := times;
    EndForEach

    // now sort by amount desc
    var amount_order := sorthelper.keys();
    amount_order.sort();
    amount_order.reverse();

    var statlist := array;
    ForEach amount in (amount_order)

        // sort by time asc
        var time_order := sorthelper[amount].keys();
        time_order.sort();
        ForEach time in (time_order)

            ForEach entry in (sorthelper[amount][time]);
                SleepMs(2);
                // get ref and realname and add to result list
                var player := SystemFindObjectBySerial(
                    entry.serial, SYSFIND_SEARCH_OFFLINE_MOBILES
                );
                If (player)
                    entry.player := player;
                    entry.name   := GetRealName(player);
                    statlist.append(entry);
                EndIf
            EndForEach
        EndForEach
    EndForEach

    return statlist;
EndFunction // }}}
