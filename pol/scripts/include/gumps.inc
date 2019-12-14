// eigentlich sollten gumps.inc und gumps_new.inc zueinander kompatibel sein, sind sie aber nich...

/////////////////////////////////////////////////////////////////
//
// GUMPS.inc
//
// Gump Functions - to make life easier! :)
//
//
// Created: 24-Jan-2000, Myrathi
//
// Updated: 25-Jan-2000, Myrathi
//	- GFInitGump now auto-inserts "nodispose".
//	- GFSetRadioGroup added for advanced radio-button support
//
// Updated: 18-Jan-2006, Taya
// - HtmlGump added
// - Error in GFUser2 fixed
//
// Notes:
//	+ All functions take hex values and convert to integer
//	  form, where necessary (gumps crash with hex values! Ugh!)
//
//	+ "Page 0" is only a background! It exists for ALL pages!
//	  Anything you want on separate pages should be on page
//	  1, 2, 3 and so on...
//
// To do:
//	+ Add multiple gump support (waiting on core support)
//	+ Add the option of supplying the KeyIDs (is this wanted?)
//	+ Create a const-list of commonly used ID's
//	+ Work out how to change font (other than when it does it
//	  itself, without warning or reason :P figures...)
//
// Bugs:
//	+ None known! :-)
//	+ If you find any, let me know! Heh. (email above)
//
// Thanks to:
//	+ Ignatius for his GUMPDOCS file which cleared up a couple
//	  of issues I was unsure of. :)
//	+ Racalac for his "runebook" source (for the "tilepic"
//	  command, mostly)
//
/////////////////////////////////////////////////////////////////

use uo;


// Function List
/////////////////////////////////////////////////////////////////
//
// GFSetRef( x, y )
// GFPage( page )
// GFNoClose()
// GFNoDispose()
// GFNoMove()
// GFInitGump()
// GFUser1( line )
// GFUser2( l_line, d_pos, d_line )
// GFGumpPic( x, y, gumpid )
// GFResizePic( x, y, gumpid, w, h )
// GFTilePic( x, y, tileid )
// GFTextLine( x, y, colour, text );
// GFTextEntry( x, y, w, h, colour, text := "" )
// GFButtonPage( x, y, release, press, page )
// GFButtonID( x, y, release, press, close )
// GFCheckBox( x, y, off, on, state )
// GFRadioButton( x, y, off, on, state )
// GFSetRadioGroup( group )
//
// GFSendGump( who )
//
/////////////////////////////////////////////////////////////////

// Var variables
/////////////////////////////////////////////////////////////////
var GFref := { 0, 0 };	// X- and Y-offset for gump operations
var GFlayout := { };	// Main layout array for entire gump
var GFdata := { };	// Storage array for static-text and
				//  text-entry initialisation strings
var GFdtidx :=   0;	// DaTa-InDeX for text (into GFdata)
var GFtxtid := 101;	// Next KeyID for TeXT-entry returns
var GFbutid := 301;	// Next KeyID for 'action' BUTtons
var GFcbxid := 501;	// Next KeyID for CheckBoXes
var GFradid := 701;	// Next KeyID for RADio buttons
/////////////////////////////////////////////////////////////////

// Function Definitions
/////////////////////////////////////////////////////////////////

// Sets the top-left co-ordinate origin
//
function GFSetRef( x, y )
	GFref[1] := CInt(x);
	GFref[2] := CInt(y);
endfunction

// Gump page #x - start of definition
//
// Returns: Page number (integer form)
//
function GFPage( page )
	page := CInt(page);
	GFlayout.append( "page " + CStr(page) );
	return page;
endfunction

// Won't let the gump be closed with a right-click
//
function GFNoClose()
	GFlayout.append( "noclose" );
endfunction

// Won't let the gump be closed by clicking ESCAPE
//
function GFNoDispose()
	GFlayout.append( "nodispose" );
endfunction

// Disallows dragging of the gump.
//
function GFNoMove()
	GFlayout.append( "nomove" );
endfunction

// Initialises this include file's Var variables
//
function GFInitGump()
	GFlayout := { "page 0", "nodispose" };
	GFdata := { };
	GFdtidx := 0;
	GFtxtid := 101;
	GFbutid := 301;
	GFcbxid := 501;
	GFradid := 701;
endfunction

// Adds a generic line to the GFlayout array
//
function GFUser1( line )
	GFlayout.append( line );
endfunction

// Adds a generic line to the GFlayout array and
// a line to the GFdata array at position 'd_pos'
//
function GFUser2( l_line, d_pos, d_line )
//	GFlayour.append( line );
// korrigiert von Taya
	GFlayout.append( l_line );
	GFdata[CInt(d_pos)] := d_line;
endfunction

// Single "art gump"
// (Uses "Gump" art from InsideUO)
//
function GFGumpPic( x, y, gumpid )
	Var line := "gumppic " + CStr(GFref[1] + CInt(x)) + " " + CStr(GFref[2] + CInt(y));
	line := line + " " + CStr(CInt(gumpid));
	GFlayout.append( line );
endfunction

// Resized "gump" made up of art from 'gumpid' to 'gumpid+8'
// (Uses "Gump" art from InsideUO)
//
function GFResizePic( x, y, gumpid, w, h )
	Var line := "resizepic " + CStr(GFref[1] + CInt(x)) + " " + CStr(GFref[2] + CInt(y));
	line := line + " " + CStr(CInt(gumpid)) + " " + CStr(CInt(w)) + " " + CStr(CInt(h));
	GFlayout.append( line );
endfunction

// Single "tile" graphic
// (Uses "Static Tiles" from InsideUO [under Artwork])
//
function GFTilePic( x, y, tileid )
	Var line := "tilepic " + CStr(GFref[1] + CInt(x)) + " " + CStr(GFref[2] + CInt(y));
	line := line + " " + CStr(CInt(tileid));
	GFlayout.append( line );
endfunction

// Display a line of text on the gump
//
// Returns: Position in the GFdata array
//
function GFTextLine( x, y, colour, text )
	Var line := "text " + CStr(GFref[1] + CInt(x)) + " " + CStr(GFref[2] + CInt(y));
	line := line + " " + CStr(CInt(colour)) + " " + CStr(GFdtidx);
	GFlayout.append( line );
	GFdtidx := GFdtidx + 1;		// Update index into GFdata[]
	GFdata[GFdtidx] := text;	// arrays start at #1... id#0 goes into [1]!!
	return (GFdtidx - 1);
endfunction

// Display a text-entry control on the gump
//
// Syntax : x   y   w   h  colour  txtid  dtidx
// Example: 125 300 150 50 32      1050   0
//
// Returns:
//    Array: 1: Position in the GFdata array where the text is stored
//           2: KeyID allocated to the text when the Gump returns (this method's daft!)
//              (Returns eg:  "Key=2024, Value=2024: This is the text" )
//
function GFTextEntry( x, y, w, h, colour, text := "" )
	Var line := "textentry " + CStr(GFref[1] + CInt(x)) + " " + CStr(GFref[2] + CInt(y));
	line := line + " " + CStr(CInt(w)) + " " + CStr(CInt(h)) + " " + CStr(CInt(colour));
	line := line + " " + CStr(GFtxtid) + " " + CStr(GFdtidx);
	GFlayout.append( line );
	GFdtidx := GFdtidx + 1;		// Update index into GFdata[]
	GFdata[GFdtidx] := text;	// arrays start at #1... id#0 goes into [1]!!
	Var ret := struct;
	ret.+dtidx := GFdtidx;
	ret.+txtid := GFtxtid;
	GFtxtid := GFtxtid + 1;		// Update KeyID (txtid)
	return ret;
endfunction

// Standard "button" syntax:
// "button x y release press close page keyid"

// Display a button that changes to another page
//
// Syntax: x, y, release, press, 0, page
// (close=0 - don't want it closing when it's changing page, eh?)
//
// Returns: The page number the button will change to
//
function GFButtonPage( x, y, release, press, page )
	// "button x y release press 0 page"
	Var line := "button " + CStr(GFref[1] + CInt(x)) + " " + CStr(GFref[2] + CInt(y));
	line := line + " " + CStr(CInt(release)) + " " + CStr(CInt(press));
	line := line + " 0 " + CStr(CInt(page));
	GFlayout.append( line );
	return page;
endfunction

// Display an "action button" (returns a key value and can close gump)
//
// Syntax: x, y, release, press, close, 0, keyid
// (page=0 - it returns a value instead of changing page)
//
// Returns: The allocated "keyid" that the button will return if pressed
//
function GFButtonID( x, y, release, press, close )
	// "button x y release press close 0 key"
	Var line := "button " + CStr(GFref[1] + CInt(x)) + " " + CStr(GFref[2] + CInt(y));
	line := line + " " + CStr(CInt(release)) + " " + CStr(CInt(press)) + " ";
	if ( close == 0 )
		line := line + "0 0 ";
	else
		line := line + "1 0 ";
	endif
	line := line + CStr(GFbutid);
	GFbutid := GFbutid + 1;		// Update KeyID (butid)
	GFlayout.append( line );
	return (GFbutid - 1);
endfunction

// Displays a check-box (off/on)
//
// Syntax: x, y, off, on, initial state (0/1), cbxid
//
// Returns: The "keyid" allocated to the checkbox (cbxid)
//
function GFCheckBox( x, y, off, on, state )
	Var line := "checkbox " + CStr(GFref[1] + CInt(x)) + " " + CStr(GFref[2] + CInt(y));
	line := line + " " + CStr(CInt(off)) + " " + CStr(CInt(on)) + " ";
	if ( state == 0 )
		line := line + "0 ";
	else
		line := line + "1 ";
	endif
	line := line + CStr(GFcbxid);
	GFcbxid := GFcbxid + 1;		// Update KeyID (cbxid)
	GFlayout.append( line );
	return (GFcbxid - 1);
endfunction

// Displays a radio button (off/on)
//
// Syntax: x, y, off, on, initial state (0/1), radid
//
// Returns: The "keyid" allocated to the radio-button (radid)
//
function GFRadioButton( x, y, off, on, state )
	Var line := "radio " + CStr(GFref[1] + CInt(x)) + " " + CStr(GFref[2] + CInt(y));
	line := line + " " + CStr(CInt(off)) + " " + CStr(CInt(on)) + " ";
	if ( state == 0 )
		line := line + "0 ";
	else
		line := line + "1 ";
	endif
	line := line + CStr(GFradid);
	GFradid := GFradid + 1;		// Update KeyID (radid)
	GFlayout.append( line );
	return (GFradid - 1);
endfunction

// Groups radio button (I think)
//
// Returns: The group number (integer form)
//
function GFSetRadioGroup( group )
	group := CInt(group);
	GFlayout.append( "group " + CStr(group) );
	return group;
endfunction

// Call this to display/use the defined gump
//
// Returns: an array of keys and values
//          (ie. what the SendDialogGump() returns)
//
// NOTE: This WILL crash the client if you've created duff arrays!!
//
function GFSendGump( who )
	return SendDialogGump( who, GFlayout, GFdata );
endfunction

// Änderung von Taya.
// Hinzunahmen von
// HtmlGump [x] [y] [width] [height] [text-id] [background] [scrollbar]
// Beschreibung:
// As the name indicates, this command allows you to use html-commands.
// Actually it creates a text-area, either with a background or without
// it and with or without a scrollbar. Additionally you pass a text-id -
// and this text can be formatted in basic html code such as centering,
// linebreaks, basefont color=#, etc
//
// backround 0 = keiner, 1 = anzeigen
// scrollbar 0 = keine, 1 = anzeigen

// Display HTML text on the gump
//
// Returns: Position in the GFdata array
//
function GFHtmlGump( x, y, widht, height, text, background, scrollbar )
	Var line := "HtmlGump " + CStr(GFref[1] + CInt(x)) + " " + CStr(GFref[2] + CInt(y));
	line := line + " " + CStr(widht) + " " + CStr(height) + " " + CStr(GFdtidx);
	line := line + " " + CStr(background) + " " + CStr(scrollbar);
	GFlayout.append( line );
	GFdtidx := GFdtidx + 1;		// Update index into GFdata[]
	GFdata[GFdtidx] := text;	// arrays start at #1... id#0 goes into [1]!!
	return (GFdtidx - 1);
endfunction
