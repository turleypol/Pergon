// HitScript Include

// AlterHitScripts(defender, weapon, armor, rawdamage)
// If any of our effects call for altering the hit scripts (such as a
// one-shot weapon, like a Glass Sword, or a magic arrow), we can
// remove them here.   We can also use this to add things like
// communicable disesase, where we add a "diseased" prop to victim.

// Function AlterHitScripts(defender, weapon, armor, rawdamage)

Function MakeAttackSkill(weapon, attacker)
	Var faktor:=1.0;
	Var itemdesc:=ReadConfigFile("::itemdesc");
	If (itemdesc)
  	Var itemdef:=itemdesc[weapon.objtype];
  	If (itemdef)
			faktor:=50.0/CDbl(itemdef.Speed); //sorgt dafür das über eine feste Zeit der Skillgain für verschiedene Waffen gleich ist (noch mit Fehler von 20% schnellste zu langsamste Waffe)
//			If (attacker.cmdlevel >= CMDLEVEL_SEER)
//	      //SendSysMessagePergon(attacker, "Combat: "+attacker.name+" "+GetHPPergon(attacker)+
//	      //  " -> "+defender.name+" "+GetHPPergon(defender), "", _DEFAULT_TEXT_FONT, 40);
//	      SendSysMessagePergon(attacker, "Combat 2: (Speed/Multi)"+itemdef.speed+"/"+faktor, "", _DEFAULT_TEXT_FONT, 40);  
//	    EndIf	
  	EndIf
  EndIf
  Var faktor_waffe:=CalcFaktorFight(attacker, 0, weapon.attribute, 1, attacker, 0);
  Var faktor_taktik:=CalcFaktorFight(attacker, 0, SKILLID_TAKTIK, 1, attacker, 0);
  //SendSysMessagePergon(attacker, "Test Att: rawpoints Taktik = "+TAKTIK_PUNKTE*faktor*faktor_taktik);
	CheckSkillPergon(attacker, weapon.attribute, -1, WAFFENSKILL_PUNKTE*faktor*faktor_waffe);
  CheckSkillPergon(attacker, SKILLID_TAKTIK, -1, TAKTIK_PUNKTE*faktor*faktor_taktik);		
EndFunction

Function MakeDefenderSkill(attacker, defender)
	Var faktor_taktik:=CalcFaktorFight(defender, attacker, SKILLID_TAKTIK, 1, defender, 0);
	//If (defender.cmdlevel >= CMDLEVEL_SEER)
    //SendSysMessagePergon(defender, "Combat: "+attacker.name+" "+GetHPPergon(attacker)+
    //  " -> "+defender.name+" "+GetHPPergon(defender), "", _DEFAULT_TEXT_FONT, 40);
  //EndIf
  //SendSysMessagePergon(defender, "Test Def: rawpoints Taktik = "+TAKTIK_PUNKTE*faktor_ausruestung);
  CheckSkillPergon(defender, SKILLID_TAKTIK, -1, TAKTIK_PUNKTE*faktor_taktik);
EndFunction

Function MakeParrySkill(attacker, defender)
//  If (defender.cmdlevel >= CMDLEVEL_SEER)
//    SendSysMessagePergon(defender, "Parry: "+attacker.name+" "+GetHPPergon(attacker)+
//      " -> "+defender.name+" "+GetHPPergon(defender), "", _DEFAULT_TEXT_FONT, 40);
//  EndIf
  Var faktor_schildkampf:=CalcFaktorFight(defender, attacker, SKILLID_SCHILDKAMPF, 1, defender, 0);
  CheckSkillPergon(defender, SKILLID_SCHILDKAMPF, -1, SCHILDKAMPF_PUNKTE+faktor_schildkampf);
EndFunction