//
//  Tierzucht.Inc - Sammelbecken fuer Funktionen, die das Zucht-Package betreffen
//
//  16.10.2003 Fraggulus
//


Function DestroyItemAtLocation(x, y, z, dest)

  ForEach item in ListItemsNearLocation(x, y, z, 0)
    If (item.objtype == dest)
      ReserveItem(item);
      DestroyItem(item);
      Return (1);
    EndIf
  EndForEach

endfunction