///////////////////////////////////////////////////////////////////////////
// multi -- Nuetzliche Funktionen fuer Multis

///////////////////////////////////////////////////////////////////////////
// Modification:
// $Log: not supported by cvs2svn $

use polsys;
use os;
use uo;

Function MoveMultiToLocation(multi, x, y, z, realm := _DEFAULT_REALM);
    var item := GetItemDescriptor(multi.objtype);

    // sicherheitshalber CProps uebernehmen, bevor jemand
    // Playerhaeuser damit zerstoert
    ForEach name in (GetObjPropertyNames(multi))
        SleepMs(2);
        var val := multi.getprop(name);
        If (val == error)
            // Fehler werden nicht kopiert
            continue;
        EndIf
        If (item.cprops.exists(name))
            item.cprops[name] := val;
        Else
            item.cprops.insert(name, val);
        EndIf
    EndForEach

    var destroy := DestroyMulti(multi);
    If (destroy == error)
        return destroy;
    EndIf
    var create := CreateMultiAtLocation(
        x, y, z, item, CRMULTI_IGNORE_ALL, realm
    );
    If (create == error)
        return create;
    EndIf
    return 1;
EndFunction
