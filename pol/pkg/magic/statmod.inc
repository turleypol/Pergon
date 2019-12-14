use uo;
use util;
use polsys;
include "include/modifyskill";

Function CanMod(who, stat)
  Var mod;

  Case (stat)
    "str": mod:="#ss";
    "dex": mod:="#dd";
    "int": mod:="#ii";
    "ar":  mod:="#aa";
    "p":   mod:="#pp";
    "b":   mod:="#bb";
    "c":   mod:="#cc";
  EndCase

  If (!GetObjProperty(who, mod))
    Return (1);
  EndIf
EndFunction

Function DoTempMod(who, stat, amt, duration)
  Var mod;
  if (stat == "p")
  	if (who.frozen)
  		return;
  	endif
  endif

  Case (stat)
    "str": mod:="#ss";
           SetStrModPergon(who, GetStrModPergon(who)+amt);
    "dex": mod:="#dd";
           SetDexModPergon(who, GetDexModPergon(who)+amt);
    "int": mod:="#ii";
           SetIntModPergon(who, GetIntModPergon(who)+amt);
    "ar":  mod:="#aa";
           who.ar_mod:=who.ar_mod+amt;
    "p":   mod:="#pp";
           who.paralyzed:=1;
           who.frozen   :=1;
    "b":   mod:="#bb";
           SetIntModPergon(who, GetIntModPergon(who)+amt);
           SetDexModPergon(who, GetDexModPergon(who)+amt);
           SetStrModPergon(who, GetStrModPergon(who)+amt);
    "c":   mod:="#cc";
           SetIntModPergon(who, GetIntModPergon(who)-amt);
           SetDexModPergon(who, GetDexModPergon(who)-amt);
           SetStrModPergon(who, GetStrModPergon(who)-amt);
  EndCase
  IncRevision(who);
  SetObjProperty(who, mod, amt);

  Var params:={};
  params[1]:=stat;
  params[2]:=who;
  params[3]:=amt;
  params[4]:=duration;

  Start_ScriptPergon("::misc/unmod", params);
EndFunction

/***********************
 * obsolete for Pergon *
 ***********************/

Function GetModAmount(magery)
  Return (RandomInt(10)+(magery/6));
EndFunction

Function GetModDuration(magery)
  Return (60+((magery/15)*60));
EndFunction
