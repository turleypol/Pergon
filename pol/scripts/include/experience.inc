////////////////////////////////////////////////////////////////////////////
//
//    Include für Erfahrungspunkte
//
//      regelt alles was mit Erfahrungspunkten durch Monstertöten zu tun hat
//
//    2007-05-28   -   Seppel
//
//
//
//
//
////////////////////////////////////////////////////////////////////////////

Include "include/modifyskill";
Include "include/msgs";


Const EXPERIENCE_PER_RANK            := 30;
Const EXPERIENCE_BONUS_PER_PLAYER    := 10;
Const EXPERIENCE_RANGE               := 15;


Function DoExperienceStuff(corpse)

  Var expOfMob:= GetExpOfMob(corpse);
  Var heroes:= array;

  Foreach mobile in (ListMobilesNearLocationEx(corpse.x, corpse.y, LIST_IGNORE_Z, EXPERIENCE_RANGE, LISTEX_FLAG_NORMAL,corpse.realm))
    If (!mobile.isA(POLCLASS_NPC) && mobile.cmdlevel < CMDLEVEL_QUESTCHAR)
      heroes.append(mobile);
    EndIf
    SleepMS(2);
  EndForEach

  Var amount:= CInt(expOfMob/heroes.size());
  Var bonus:= CInt((heroes.size() - 1) * EXPERIENCE_BONUS_PER_PLAYER);

  ForEach hero in heroes
    DealExperience(hero, amount, bonus);
    SleepMS(2);
  EndForEach

EndFunction


Function DealExperience(who, amount, bonus)

  Var oldRank:= GetRank(who);
  Var maxAmount:= oldRank * EXPERIENCE_PER_RANK;
  maxAmount:= max(maxAmount, EXPERIENCE_PER_RANK);

  amount:= min(amount, maxAmount);
  amount:= max(amount,0);

  amount:= CInt(amount *(100.0+bonus)/100.0);
  AddExperience(who, amount, oldRank);

  //Var newRank:= GetRank(who);
  //If (oldRank <> newRank)
  //  Var newRankName:= GetRankName(newRank);
  //  SendSysMessagepergon(who, "Ihr seid nun ein " + newRankName);
  //EndIf

EndFunction


Function AddExperience(who, amount, oldRank)

  var oldkrit:=Is_Critical();
  Set_Critical(1);

  Var actualExp:= Max(0, CInt(GetObjProperty(who, EXPERIENCE)));

  //SendSysMessagePergon(who, "oldExp: " + actualExp);
  //Var newExp:= actualExp + amount;
  //SendSysMessagePergon(who, "newExp: " + newExp);
  SetObjProperty(who, EXPERIENCE, actualExp + amount);

  Set_Critical(oldkrit);

  var newRank:=GetRank(who);
  If (oldRank<>newRank)  // Wenn neuer Rank dann IncRevision damit Tooltip aktuell ist
  	// Rank speichern, damit innerhalb von CriticalTooltip nicht Rank berechnet werden muss
    who.setprop(EXPERIENCE_RANK,newRank);
    IncRevision(who);
  EndIf

EndFunction


Function GetExpOfMob(corpse)

  Var expOfMob:=corpse.getprop("exp");

  If (!expOfMob)
    //Mobs die keinen Eintrag haben geben keine Punkte
    expOfMob:=0;
  EndIf
  expOfMob:=Min(1000,expOfMob); //mehr als 1000 Punkte gibt es nicht pro Mob
  Return(expOfMob);

EndFunction


Function GetRank(who)

  Var exp:= CInt(GetObjProperty(who, EXPERIENCE));
  var rank:= Max(0,(logE(exp/2000)/logE(2))+1);
  Return(CInt(rank));

EndFunction


Function GetRankName(who,rank)

  Var rankName;
  //Print("Expierence rank: " + rank);
  Case (CInt(rank))
    0: If (who.murderer)
    		If (who.gender)
    			rankName:="unerfahrene Halunkin";
    		Else
    			rankName:= "unerfahrener Halunke";
    		EndIF
    	 Else
    	 	If (who.gender)
    	 		rankName:="unerfahrene Städterin";
    	 	Else
    			rankName:= "unerfahrener Städter";
    		EndIf
    	 EndIf
    1: If (who.murderer)
    		If (who.gender)
    			rankName:="unangenehme Halunkin";
    		Else
    			rankName:= "unangenehmer Halunke";
    		EndIf
    	Else
    		If (who.gender)
    			rankName:="hilfreiche Städterin";
    		Else
    			rankName:= "hilfreicher Städter";
    		EndIf
    	EndIf
    2: If (who.murderer)
    		If (who.gender)
    			rankName:="unangenehme Gaunerin";
    		Else
    			rankName:= "unangenehmer Gauner";
    		EndIf
    	Else
    		If (who.gender)
    			rankName:="hilfreiche Wanderin";
    		Else
    			rankName:= "hilfreicher Wanderer";
    		EndIf
    	EndIf
    3: If (who.murderer)
    		If (who.gender)
    			rankName:="arglistige Gaunerin";
    		Else
    			rankName:= "arglistiger Gauner";
    		EndIf
    	Else
    		If (who.gender)
    			rankName:="zuverlässige Wanderin";
    		Else
    			rankName:= "zuverlässiger Wanderer";
    		EndIf
    	EndIf
    4: If (who.murderer)
    		If (who.gender)
    			rankName:="heimtückische Gaunerin";
    		Else
    			rankName:= "heimtückischer Gauner";
    		EndIf
    	Else
    		If (who.gender)
    			rankName:="beachtete Wanderin";
    		Else
    			rankName:= "beachteter Wanderer";
    		EndIf
    	EndIf
    5: If (who.murderer)
    		If (who.gender)
    			rankName:="unangenehme Schurkin";
    		Else
    			rankName:= "unangenehmer Schurke";
    		EndIf
    	Else
    		If (who.gender)
    			rankName:="hilfreiche Kennerin der Wälder";
    		Else
    			rankName:= "hilfreicher Kenner der Wälder";
    		EndIf
    	EndIf
    6: If (who.murderer)
    		If (who.gender)
    			rankName:="arglistige Schurkin";
    		Else
    			rankName:= "arglistiger Schurke";
    		EndIf
    	Else
    		If (who.gender)
    			rankName:="zuverlässige Kennerin der Wälder";
    		Else
    			rankName:= "zuverlässiger Kenner der Wälder";
    		EndIf
    	EndIf
    7: If (who.murderer)
    		If (who.gender)
    			rankName:="heimtückische Schurkin";
    		Else
    			rankName:= "heimtückischer Schurke";
    		EndIf
    	Else
    		If (who.gender)
    			rankName:="beachtete Kennerin der Wälder";
    		Else
    			rankName:= "beachteter Kenner der Wälder";
    		EndIf
    	EndIf
    8: If (who.murderer)
    		If (who.gender)
    			rankName:="unangenehme Wegelagerin";
    		Else
    			rankName:= "unangenehmer Wegelagerer";
    		EndIf
    	Else
    		If (who.gender)
    			rankName:="hilfreiche Hüterin der Wälder";
    		Else
    			rankName:= "hilfreicher Hüter der Wälder";
    		EndIf
    	EndIf
    9: If (who.murderer)
    		If (who.gender)
    			rankName:="arglistige Wegelagerin";
    		Else
    			rankName:= "arglistiger Wegelagerer";
    		EndIf
    	Else
    		If (who.gender)
    			rankName:="zuverlässige Hüterin der Wälder";
    		Else
    			rankName:= "zuverlässiger Hüter der Wälder";
    		EndIf
    	EndIf
    10: If (who.murderer)
    		If (who.gender)
    			rankName:="heimtückische Wegelagerin";
    		Else
    			rankName:= "heimtückischer Wegelagerer";
    		EndIf
    	Else
    		If (who.gender)
    			rankName:="beachtete Hüterin der Wälder";
    		Else
    			rankName:= "beachteter Hüter der Wälder";
    		EndIf
    	EndIf
    11: If (who.murderer)
    		rankName:= "hinterhältige Geißel";
    	Else
    		If (who.gender)
    			rankName:="herrausragende Beschützerin der Wälder";
    		Else
	    		rankName:= "herrausragender Beschützer der Wälder";
	    	EndIf
    	EndIf
    12: If (who.murderer)
    		rankName:= "verabscheute Geißel";
    	Else
    		If (who.gender)
    			rankName:="berühmte Beschützerin der Wälder";
    		Else
    			rankName:= "berühmter Beschützer der Wälder";
    		EndIf
    	EndIf
    13: If (who.murderer)
    		rankName:= "ruchlose Geißel";
    	Else
    		If (who.gender)
    			rankName:="edle Beschützerin der Wälder";
    		Else
    			rankName:= "edler Beschützer der Wälder";
    		EndIf
    	EndIf
    14: If (who.murderer)
    		If (who.gender)
    			rankName:="hinterhältige Henkerin";
    		Else
    			rankName:= "hinterhältiger Henker";
    		EndIf
    	Else
    		If (who.gender)
    			rankName:="herrausragende Bewahrerin der geheimen Orte";
    		Else
	    		rankName:= "herrausragender Bewahrer der geheimen Orte";
	    	EndIf
    	EndIf
    15: If (who.murderer)
    		If (who.gender)
    			rankName:="verabscheute Henkerin";
    		Else
    			rankName:= "verabscheuter Henker";
    		EndIf
    	Else
    		If (who.gender)
    			rankName:="berühmte Bewahrerin der geheimen Orte";
    		Else
    			rankName:= "berühmter Bewahrer der geheimen Orte";
    		EndIf
    	EndIf
    16: If (who.murderer)
    		If (who.gender)
    			rankName:="ruchlose Henkerin";
    		Else
    			rankName:= "ruchloser Henker";
    		EndIf
    	Else
    		If (who.gender)
    			rankName:="edle Bewahrerin der geheimen Orte";
    		Else
    			rankName:= "edler Bewahrer der geheimen Orte";
    		EndIf
    	EndIf
    17: If (who.murderer)
    		If (who.gender)
    			rankName:="hinterhältige Vernichterin";
    		Else
    			rankName:= "hinterhältiger Vernichter";
    		EndIf
    	Else
    		If (who.gender)
    			rankName:="herrausragende Verteidigerin des Landes";
    		Else
    			rankName:= "herrausragender Verteidiger des Landes";
    		EndIf
    	EndIf
    18: If (who.murderer)
    		If (who.gender)
    			rankName:="verabscheute Vernichterin";
    		Else
    			rankName:= "verabscheuter Vernichter";
    		EndIf
    	Else
    		If (who.gender)
    			rankName:="berühmte Verteidigerin des Landes";
    		Else
    			rankName:= "berühmter Verteidiger des Landes";
    		EndIf
    	EndIf
    19: If (who.murderer)
    		If (who.gender)
    			rankName:="ruchlose Vernichterin";
    		Else
    			rankName:= "ruchloser Vernichter";
    		EndIf
    	 Else
    	 	If (who.gender)
    	 		rankName:="edle Verteidigerin des Landes";
    	 	Else
    	 		rankName:= "edler Verteidiger des Landes";
    	 	EndIf
    	 EndIf
    Default: If (who.murderer)
    		If (who.gender)
    			rankName:="ausgestossene schwarze Reiterin";
    		Else
    			rankName:= "ausgestossener schwarzer Reiter";
    		EndIf
    	Else
    		If (who.gender)
    			rankName:="glorreiche Heldin des Landes";
    		Else
    			rankName:= "glorreicher Held des Landes";
    		EndIf
    	EndIf
  EndCase

  Return(rankName);

EndFunction

Function CalculationNewExperience(who)
	If (who.cmdlevel<CMDLEVEL_QUESTCHAR)
	  var lastlogintime:=who.getprop("LastLog");

	  //jetzt berechnen, was zwischen den zwei logins an punkten verloren gegangen ist
	  var rank:=GetRank(who);
	  var MaxPerRank:=rank*rank*EXPERIENCE_PER_RANK;//wert für 24h -> jetzt noch normieren

	  var abzug:=CInt(MaxPerRank*(ReadGameClock()-lastlogintime)/(24.0*60.0*60.0));//auf 24h normiert

	  var oldkrit:=Is_Critical();
	  Set_Critical(1);
	  who.setprop(EXPERIENCE,CInt(who.getprop(EXPERIENCE))-abzug);
	  If ((rank==1) && (who.getprop(EXPERIENCE)<2000))  // Rank darf nicht unter 1 fallen wenn erstmal geschafft
	  	who.setprop(EXPERIENCE,2000);
	  ElseIf (who.getprop(EXPERIENCE)<0)
	  	who.setprop(EXPERIENCE,0);
	  EndIf
	  Set_Critical(oldkrit);

    // Rank speichern, damit innerhalb von CriticalTooltip nicht Rank berechnet werden muss
    who.setprop(EXPERIENCE_RANK,GetRank(who));  // Hier immer setzen egal was ist, damit jeder Player def. CProp hat
    // IncRevision ist schon in der logon
  EndIf

EndFunction