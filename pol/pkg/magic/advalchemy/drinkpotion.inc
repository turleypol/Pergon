//
//  17.11.2000 - Fraggulus - Bugfixing ... da war'n '::' zuviel vorm 'itemdesc'
//  28.04.2003 - Fraggulus - Berechnung des Giftlevels des Trankes korrigiert
//

Include "include/poison";
include "include/objtype";

Var itemdefs := ReadConfigFile("itemdesc");
Const POISON_SOUND := 0x247;
Const DRINK_SOUND := 0x32;
//Const UOBJ_EMPTY_BOTTLE := 0x0f0e; // Mai - die ist schon in "include/objtype" definiert
Const EFFECT_BLUE_SPARKLE  := 0x373a;
Const SOUND_EFFECT_CURE	:= 0x1e1;
Const EFFECT_SPARKLE  := 0x374a;
Const HIT_ACTION := 0x148b;
Const MOVING_EFFECT_PURPLE_POTION  := 0x0F0D;
Const SOUND_EFFECT_EXPLOSION	:= 0x208;
Const SOUND_EFFECT_HEAL	:= 0x1f3;
Const EFFECT_EXPLOSION := 0x36b0;

function checkifcandrink(character)
	if (getobjproperty(character, "#potion"))
		if (getobjproperty(character, "#potion") > readgameclock() )
			SendSysMessagePergon(character, "Ihr muesst warten, bis Ihr noch einmal trinken koennt!");
			return 0;
		else
			setobjproperty(character, "#potion", readgameclock() + 10);
			return 1;
		endif
	else
		setobjproperty(character, "#potion", readgameclock() + 10);
		return 1;
	endif
endfunction


function candrink(character)
	eraseobjproperty(character, "#potion");
endfunction

function empty_bottle(character, potion)
	PlaySoundEffect (character, DRINK_SOUND);
	CreateItemInBackpackPergon(character, UOBJ_EMPTY_BOTTLE, 1);
	SubtractAmount(potion, 1);
	PlayObjectCenteredEffect( character, EFFECT_BLUE_SPARKLE, 10,10);
endfunction

function get_potion_strength(potion)
	return itemdefs[potion.objtype].strength;
endfunction

function is_poison(potion)
	Var poisonlevel := GetPoisonLevel(potion);

	if (poisonlevel)
		return poisonlevel;
	elseif (potion.objtype IN {0xDC04, 0xDC05, 0xDC06, 0xDC17})
		return itemdefs[potion.objtype].strength;
	EndIf
endfunction

function do_poison(character, potion)
	Var potion_strength := is_poison(potion);

	if (!potion_strength)
		potion_strength := 1;
	EndIf

	if (CheckSkillPergon(character, SKILLID_VORSCHMECKEN, (potion_strength * 15),  (potion_strength * 10)))
		SendSysMessagePergon(character, "Dieser Trank ist vergiftet!");
		return;
	endif

	empty_bottle(character, potion);
	PrintTextAbovePrivatePergon (character, "Ihr wurdet vergiftet!", "", character);

	AddPoisonLevel(character, potion_strength); // Vergiftung durchfuehren
endfunction
