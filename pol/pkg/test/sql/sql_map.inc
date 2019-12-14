Function update_golocs(byref connection)
  var res := mysql_query(connection, "SELECT * FROM HP_LiveMap_Locations");  
  If (mysql_num_rows(res) > 0)
    res := mysql_query(connection, "UPDATE HP_LiveMap_Locations SET DeleteMe=1");
  EndIf
  
  var gofile := ReadConfigFile("::golocs");
  var areas := GetConfigStringArray(gofile["MainList"], "Area");
  
  ForEach area in areas
    If (gofile[area])
      var locations := GetConfigStringArray(gofile[area], "Location");
      ForEach location in locations
        If (location != "SEPERATOR")
          var time := ReadMilliSecondClock();
          var locationname := SplitWords(location, ":");
          var locationarray := SplitWords(locationname[2]);
          
          var realm := _DEFAULT_REALM;
          If (locationarray[4])
            realm := locationarray[4];
          EndIf
          
          var locstruct := struct{
            area := SQLSanitize(area),
            location := SQLSanitize(locationname[1]),
            x := CInt(locationarray[1]),
            y := CInt(locationarray[2]),
            z := CInt(locationarray[3]),
            realm := SQLSanitize(realm)
          };
                  
          res := mysql_query(connection, "UPDATE HP_LiveMap_Locations SET PosX={1.x}, PosY={1.y}, PosZ={1.z}, Realm='{1.realm}', DeleteMe=0 WHERE Area='{1.area}' AND Location='{1.location}'".format(locstruct));
          If (mysql_affected_rows(res))  
            SQLDebugLog("update_golocs: Updating {1.area}, {1.location} in {2} millisecond(s)".format(locstruct, ReadMilliSecondClock()-time));
          Else
            res := mysql_query(connection, "INSERT INTO HP_LiveMap_Locations (Area, Location, PosX, PosY, PosZ, Realm, DeleteMe) VALUES "+
              "('{1.area}', '{1.location}', {1.x}, {1.y}, {1.z}, '{1.realm}', 0)".format(locstruct)
            );
            If (!res)
              SQLDebugLog("FEHLER: update_golocs "+res);
            Else
              SQLDebugLog("update_golocs: Inserting {1.area}, {1.location} in {2} millisecond(s)".format(locstruct, ReadMilliSecondClock()-time));
            EndIf
          EndIf
        EndIf
      EndForEach
    EndIf
  EndForEach
  
  mysql_query(connection, "DELETE FROM HP_LiveMap_Locations WHERE DeleteMe=1");
  UnloadConfigFile("::golocs");
EndFunction