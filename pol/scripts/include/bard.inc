// 2004-02-02 Mai: fuer provozieren und friedenstiften auch die entsprechenden sounds verwenden


function play(character,succeed_sound,fail_sound, theskill)
  Var thepoints:=GetSkillDefaultPoints(SKILLID_MUSIZIEREN);
   if (CheckSkillPergon(character, SKILLID_MUSIZIEREN, theskill, thepoints))
      PlaySoundEffect( character, succeed_sound );
      return 1;
   else
      PlaySoundEffect( character, fail_sound);
      return 0;
   endif
endfunction

function musicpeace(character, instrument, diff)

   Var inst_id := instrument.graphic;
   Var ret := 0;

   if (inst_id == UOBJ_HARP)
      ret := play(character,SFX_MUSIC_HARP_PEACE,SFX_HARP_FAIL, diff);
   elseif (inst_id == UOBJ_LUTE)
      ret := play(character,SFX_MUSIC_LUTE_PEACE,SFX_LUTE_FAIL, diff);
   elseif (inst_id == UOBJ_DRUM)
      ret := play(character,SFX_MUSIC_DRUM_PEACE,SFX_DRUM_FAIL, diff);
   elseif (inst_id == UOBJ_TAMB)
      ret := play(character,SFX_MUSIC_TAMB_PEACE,SFX_TAMB_FAIL, diff);
   elseif (inst_id == 0x0e9e) //das Tambourin mit dem roten Band
      ret := play(character,SFX_MUSIC_TAMB_PEACE,SFX_TAMB_FAIL, diff);
   elseif (inst_id == UOBJ_FLUT)
      ret := play(character,SFX_MUSIC_FLUT_PEACE,SFX_FLUT_FAIL, diff);
   else
      PrintTextAbovePrivatePergon( character, "Das ist nichts, womit man musizieren kann.", "", character );
   endif

   return ret;
endfunction

function musicprovoke(character, instrument, diff)

   Var inst_id := instrument.graphic;
   Var ret := 0;

   if (inst_id == UOBJ_HARP)
      ret := play(character,SFX_MUSIC_HARP_BATTLE,SFX_HARP_FAIL, diff);
   elseif (inst_id == UOBJ_LUTE)
      ret := play(character,SFX_MUSIC_LUTE_BATTLE,SFX_LUTE_FAIL, diff);
   elseif (inst_id == UOBJ_DRUM)
      ret := play(character,SFX_MUSIC_DRUM_BATTLE,SFX_DRUM_FAIL, diff);
   elseif (inst_id == UOBJ_TAMB)
      ret := play(character,SFX_MUSIC_TAMB_BATTLE,SFX_TAMB_FAIL, diff);
   elseif (inst_id == 0x0e9e) //das Tambourin mit dem roten Band
      ret := play(character,SFX_MUSIC_TAMB_BATTLE,SFX_TAMB_FAIL, diff);
   elseif (inst_id == UOBJ_FLUT)
      ret := play(character,SFX_MUSIC_FLUT_BATTLE,SFX_FLUT_FAIL,diff);
   else
      PrintTextAbovePrivatePergon( character, "Das ist nichts, womit man musizieren kann.", "", character );
   endif

   return ret;
endfunction


function findinstrument(character)
	Var instruments := {UOBJ_HARP, UOBJ_LUTE, UOBJ_DRUM, UOBJ_TAMB, 0x0e9e, UOBJ_FLUT};

	foreach item in enumerateitemsincontainer(character.backpack)
		if (item.graphic in instruments)
			return item;
		endif
	endforeach
endfunction

function can_sing( me, circle)

	Var cfg := readconfigfile(":combat:itemdesc");

	foreach item in ListEquippedItems(me)
		if ((cfg[item.objtype].blockcircle) && (cfg[item.objtype].blockcircle <= circle))
			SendSysMessagePergon(me, "Eure Ausruestung hindert Euch an der Verwendung dieses Instruments!");
			return 0;
		endif
	endforeach

	return 1;

endfunction
