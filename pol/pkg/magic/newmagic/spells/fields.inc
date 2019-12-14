use math;
use os;
use polsys;

function GetDirection (character, x, y)

   if (Abs(y - character.y) > Abs(x - character.x))
      if (y > character.y)
         return ("sued");
      elseif (y < character.y)
         return ("nord");
      endif
   elseif (Abs(x - character.x) > Abs(y - character.y))
      if (x > character.x)
         return ("ost");
      elseif (x < character.x)
         return ("west");
      endif
   endif

   return (0);

endfunction



function Get_Direction (character, x, y, z)

   Var ns_dir:= "";
   Var ew_dir:= "";
   Var dir;

   if (y > character.y)
      ns_dir:= "sued";
   elseif (y < character.y)
      ns_dir:= "nord";
   endif

   if (x > character.x)
      ew_dir:= "ost";
   elseif (x < character.x)
      ew_dir:= "west";
   endif

   if (ns_desc == "" and ew_desc == "")
      return ("nicht feststellbar");
   else
      return (ns_dir + ew_dir);
   endif

endfunction



Function CreateNorthSouthField (caster, power, fieldtype, x, y, z, halflength:= 2)

  //needs to return an array of field elements

  Var fires:= {};
  Var fire:= GetItemDescriptor(fieldtype);
  Var base_z:= z;
  Var counter:= 1;
  Var i;

  fire.CProps.insert("creator", caster.serial);
  fire.CProps.insert("f", power);
  fires[counter]:= CreateItemAtLocationPergon(x, y, z, fire, 1, caster.realm);
  fires[counter].saveonexit:=0;

  for (i:= 1; i <= halflength; i+= 1)
    z:= GetFieldZ(x + i, y, z,caster.realm);
    if (z < 128)
      counter+= 1;
      fires[counter]:= CreateItemAtLocationPergon(x + i, y, z, fire, 1, caster.realm);
      fires[counter].saveonexit:=0;
    endif
  endfor

  z:= base_z;
  for (i:= 1; i <= halflength; i+= 1)
    z:= GetFieldZ(x - i, y, z,caster.realm);
    if (z < 128)
      counter+= 1;
      fires[counter]:= CreateItemAtLocationPergon(x - i, y, z, fire, 1, caster.realm);
      fires[counter].saveonexit:=0;
    endif
  endfor

  return fires;

EndFunction



Function CreateEastWestField (caster, power, fieldtype, x, y, z, halflength:= 2)

  //needs to return an array of field elements

  Var fires:= {};
  Var fire:= GetItemDescriptor(fieldtype);
  Var base_z:= z;
  Var counter:= 1;
  Var i;

  fire.CProps.insert("creator", caster.serial);
  fire.CProps.insert("f", power);
  fires[counter]:= CreateItemAtLocationPergon(x, y, z, fire, 1, caster.realm);
  fires[counter].saveonexit:=0;


  for (i:= 1; i <= halflength; i+= 1)
    z:= GetFieldZ(x, y + i, z,caster.realm);
    if (z < 128)
      counter+= 1;
      fires[counter]:= CreateItemAtLocationPergon(x, y + i, z, fire, 1, caster.realm);
      fires[counter].saveonexit:=0;
    endif
  endfor

  z:= base_z;
  for (i:= 1; i <= halflength; i+= 1)
    z:= GetFieldZ(x, y - i, z,caster.realm);
    if (z < 128)
      counter+= 1;
      fires[counter]:= CreateItemAtLocationPergon(x, y - i, z, fire, 1, caster.realm);
      fires[counter].saveonexit:=0;
    endif
  endfor

  return fires;

EndFunction



Function GetFieldZ (x, y, base_z,realm)

	Var height;
	Var z:= GetWorldHeight(x, y,realm);
	If (!z.errortext)
		while (!(Abs(base_z - z) < 10))
			height:= GetStandingHeight(x, y, z + 1,realm);
			if ((z != height.z) && (!height.errortext))
				z:= height.z;
			else
				break;
			endif
		endwhile
		if (Abs(base_z - z) < 10)
			return z;
		EndIf
	EndIf

	Return 200;
EndFunction
