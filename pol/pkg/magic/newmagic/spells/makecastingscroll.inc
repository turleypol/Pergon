////////////////////////////////////////
// SeBB - Erstellt 2001-03-01 / 17:00 //
////////////////////////////////////////

////////////////////////////////////////////////////////////////
//
// MakeCastingScroll - Converts Spell Scroll to Castable Scroll
//
////////////////////////////////////////////////////////////////

use uo;
use os;

Function MakeCastingScroll (scroll)

   Var container:= scroll.container;

   CreateItemInContainerPergon(container, CInt(scroll.objtype) + 512, 1);
   SubtractAmount(scroll, 1);

   return;

EndFunction
