///////////////////////////////////////////////////////////////////////////
//
//						Bountbox - Box fuer Gesuchte (neues Konzept der verwundbaren Townguards
//
//						Author: Hotny
//						06.08.2006
//
//						Modifikations:
//
///////////////////////////////////////////////////////////////////////////

CONST BOUNTBOX_AREA := "Bountbox";
Const CITY_BOUNTBOX := "Bountbox of ";
CONST BOUNTPAPER		:= 0x66b1;

function OpenBountBoxArea()
	Var bountboxarea:=FindStorageArea(BOUNTBOX_AREA);
  If (!bountboxarea)
    bountboxarea:=CreateStorageArea(BOUNTBOX_AREA);
  EndIf

  // Dieser Fall sollte niemals auftreten... (theoretisch :o)
  If (!bountboxarea)
    SysLog("FEHLER: Konnte die Bountbox Area weder oeffnen noch anlegen!");
  EndIf

  Return (bountboxarea);
endfunction

function findbountbox(city)
	var bountboxarea:=OpenBountBoxArea();
	if (bountboxarea)
		var boxname := CITY_BOUNTBOX + lower(city);
		var bountbox:=FindRootItemInStorageArea(bountboxarea,boxname);
		if (!bountbox)
			bountbox:=CreateRootItemInStorageArea(bountboxarea,boxname, UOBJ_BANKBOX);
		endif
		
		//Der Fall sollte nie auftreten, aber mal aus Sicherheit
		if (!bountbox)
			Syslog("FEHLER: Konnte Bountbox fuer " + city + " weder oeffnen noch anlegen");
		endif
		return (bountbox);		
	endif
endfunction

//////////////////////////////////////////////////////////////
// Prüft, ob angegebene Person ein Bürgermeister oder GM ist.
//////////////////////////////////////////////////////////////

Function IsPersonBMorGM(who)
  If (who.cmdlevel >= CMDLEVEL_HIGHGM)
    Return 1;
  EndIf

  Var city := GetObjProperty(who, "MajorOf");

  If (city)     // Bürgermeister einer Stadt?
    // müsste alle Stadtsteine innerhalb von 0,0-5000,5000 finden.
    Var townstones := ListItemsNearLocationOfType(2500, 2500, 10, 2500, UOBJ_TOWNSTONE,REALM_BRITANNIA);

    // Prüft, ob Person noch als Bürgermeister in einer Stadt eingetragen ist.
    Foreach stone in townstones
      If ((GetObjProperty(stone, "mayorserial") == who.serial) && (PlaceInfos(who).region == PlaceInfos(stone).region))
        Return 1;
      EndIf
    Endforeach
  EndIf

  Return 0;
EndFunction
