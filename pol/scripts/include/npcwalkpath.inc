
use util;
use os;
use uo;


Function NPCWalkPath (Who)

  // Error checking
  
  var rulenum := GetProperty("NpcPathRule");
  
  if (!rulenum)
  	SetProperty("NpcPathRule", 1);
  	rulenum := 1;
  endif
  
  var pathname := GetProperty("NPCPath");
  
  if (!pathname)
  	return;
  endif
  
  var NPCPathList := GetGlobalProperty("path_"+pathname);
  
  if (!npcpathlist)
  	return;
  endif
  
  // Get elements X Y Z
  
  var NPCListRule := NPCPathList[ rulenum ];
  
  //print("NPCListRule=" + npclistrule);
  
  var nx := NPCListRule[1];
  var ny := NPCListRule[2];
  var z := NPCListRule[3];
  
  //print("nx=" + nx + " ny=" + ny + " z=" + z);
  
  var failures := 0;
  
  var t := 0;
  var i := 0;
  var speed := 0;
  
  Case (nx)
  	"action":
  
  		for(i := 0; i < z; i += 1)
  			PerformAction(who, ny);
  			sleep(1);
  		endfor
  		t := 1;
  
  	"facing":
  
  		who.facing := ny;
  		t := 1;
  
  	"pause":
  
  		sleep(ny);
  		t := 1;
  
  	"run":
  
  		speed := 1;
  
  	"sfx":
  
  		for(i := 0; i < z; i += 1)
  			PlaySoundEffect(who, ny);
  			sleep(1);
  		endfor
  
  		t := 1;
  
  	"walk":
  
  		speed := 0;
  
  endcase
  
  // Add New Clauses here
  
  
  if (t == 0)
  
  	while (who.x != ny or who.y != z)
  		var olddistance := DistanceToLocation(who , ny, z);
  
  		if (speed == 0)
  			WalkTowardLocation(ny,z);
  		else
  			RunTowardLocation(ny,z);
  		endif
  
  		sleepms(10);
  
  		var newdistance := DistanceToLocation(who , ny, z);
  
  		var diff := newdistance - olddistance;
  
  		If (diff == 0)
  			failures += 1;
  			if (failures >= 10)
  				failures := 0;
  				SayPergon(0, "Zeit weiterzugehen ...");
  				break;
  			endif
  		else
  			failures := 0;
  		endif
  
  //Print ("Who.x= " + who.x + " who.y= " + who.y + "Failes: " + failures);
  //Print ("ny=" + ny + " z=" + z);
  
  	endwhile
  endif
  
  // Increment Path Rule # (rulenum)
  rulenum += 1;
  if (rulenum > NPCPathList.size())
  	rulenum := 1;
  endif
  SetProperty("NpcPathRule", rulenum);

endfunction


// This function check the distance from a mobile to an (X , y) Formated location

function DistanceToLocation (who, locx, locy)

	Var xdiff := Abs(who.x - locx);
	var ydiff := Abs(who.y - locy);

	if (xdiff > ydiff)
		return xdiff;
	else
		return ydiff;
	endif

endfunction
