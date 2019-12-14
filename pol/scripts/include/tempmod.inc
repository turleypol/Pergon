Use os;

Function Do_Temp_Mod(who, modval, amt, duration)
  Var parms:={ };
  parms[2]:=who;
  parms[1]:=modval;
  parms[3]:=amt;
  parms[4]:=duration;

  Start_ScriptPergon("::misc/demod", parms);
EndFunction

Function can_stat_mod(who, stat)

  Var mod_;

  Case (stat)
    "str": mod_ := "#ss";
    "dex" : mod_ := "#dd";
    "int" : mod_ := "#ii";
    "ar"  : mod_ := "#aa";
  EndCase

  If (getobjproperty(who, mod_))
    return 0;
  Else
    setobjproperty(who, mod_, 1);
    return 1;
  EndIf

EndFunction

Function allow_stat_mod(who, stat, duration)
  detach();
  Var mod_;

  Case (stat)
    "str": mod_ := "#ss";
    "dex" : mod_ := "#dd";
    "int" : mod_ := "#ii";
    "ar"  : mod_ := "#aa";
  EndCase

  While (1)
    sleep(duration);
    If (who)
      eraseobjproperty(who, mod_);
      Return;
    EndIf
  EndWhile

EndFunction