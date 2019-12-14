//////////////////////////////////////////////
//
//  Shinigami's GumpUtil v1.03 (2001/03/14)
//    (some nice Functions for Gumps)
//
//  Author:    Shinigami
//  Languages: English/German
//
//   Modifications:
//     2001/03/14 Shinigami: GumpTextCropped
//
//////////////////////////////////////////////

/////////////////
// Installation
/////////////////
//
// Move it to /pol/scripts/include and use it in your script:
//   Include "include/gumputil";

//////////////
// Functions
//////////////
//
// InitGump                          // Initialize Gump-Functions
//
// GumpTextWidth(gumptext)           // Returns the Width of GumpText
// GumpTextWidthFast(gumptext)       // Returns the Width of GumpText
// GumpTextLeft(gumptext, x, width)  // Returns the X-Value (Left-Alignment)
// GumpTextRight(gumptext, x, width) // Returns the X-Value (Right-Alignment)
// GumpTextMid(gumptext, x, width)   // Returns the X-Value (Mid-Alignment)
// GumpTextCropped(gumptext, width)  // Returns the cropped GumpText
//
//   Parameters:
//     gumptext - a String
//     x        - X-Value from the Textbox
//     width    - Width from the Textbox
//
// Create_ScrollBar(x, y, height, pages, page, layout) // Shows a Scrollbar
//
//   Parameters:
//     x,y    - X-/Y-Value of the left-top Corner
//     height - Height
//     pages  - Number of Pages
//     page   - Actual Page Number
//     layout - Contains the actual Gump Layout
//   Returns:
//     Updated "layout"

////////////////
// Gump-Syntax
////////////////
//
// button
//
// checkbox
//
// checkertrans     x y breite hoehe
//                  Definiert alle vorher definierten Bestandteile als halbtransparent
//
// croppedtext      x y breite hoehe color dataID
//                  wenn der Text nicht reinpasst, dann wird abgeschnitten und mittels Punkten aufgefuellt
//                    dataID - Index im Daten-Array
//
// gumppic
//
// gumppictiled     x y breite hoehe gumpID
//
// group
//
// htmlgump         x y breite hoehe dataID transbkg showbar
//                  Scrollbox mit Textinhalt
//                    dataID   - Index im Daten-Array (im Text... '\n' oder CChr(10) = Zeilenumbruch)
//                    transbkg - Hintergrund anzeigen (0=Nein (d.h. transparent), sonst=Ja (Standard))
//                    showbar  - Scrollbar anzeigen   (0=Nein, sonst=Ja (Standard))
//
// noclose
//
// nodispose
//
// nomove
//
// page
//
// radio
//
// resizepic
//
// text
//
// textentry
//
// tilepic
//
// tilepichue
//
// xmfhtmlgump      x y breite hoehe
//                  Funzt irgendwie nicht
//
// xmfhtmlgumpcolor x y breite hoehe messageID backgroud:=0 scroll:=0 color:=16777215

//////////////////////
// Original Location
//////////////////////
//
// www.yahoogroups.com -> Pol-Scriptforum -> /Includes/gumputil.zip

///////////////////////////////////////////////////////////////////////////////

//////////////
// Constants
//////////////

Const croppedfiller:="...";

//////////////////////////////////////////////////
//////////////////////////////////////////////////

// Backgrounds
const BKGND_MARBLE			:= 0x0a28;

// Stretchable Dividers
const DIV_BLACK_LINE		:= 0x005d;

// Book
const G_BIGBOOK				:= 0x01fe;
const G_BIGBOOK_L_CORNER	:= 0x01ff;
const G_BIGBOOK_R_CORNER	:= 0x0200;

// Small circles with spell circle numbers 1-8.
const G_CIRCLE1				:= 0x08b1;
const G_CIRCLE2				:= 0x08b2;
const G_CIRCLE3				:= 0x08b3;
const G_CIRCLE4				:= 0x08b4;
const G_CIRCLE5				:= 0x08b5;
const G_CIRCLE6				:= 0x08b6;
const G_CIRCLE7				:= 0x08b7;
const G_CIRCLE8				:= 0x08b8;

// Diamond Buttons
const G_BLUE_DIAMOND_1		:= 0x0868;
const G_BLUE_DIAMOND_0		:= 0x086a;
const G_RED_DIAMOND_1		:= 0x09a9;
const G_RED_DIAMOND_0		:= 0x09aa;

// Octagon Skill 1-3 Buttons
const G_SKILL1				:= 0x04d8;
const G_SKILL2				:= 0x04db;
const G_SKILL3				:= 0x04de;

// Standard buttons.
const G_RED_PREV_1			:= 0x04f1;
const G_RED_PREV_0			:= 0x04f2;
const G_GREEN_CONT_1		:= 0x04f4;
const G_GREEN_CONT_0		:= 0x04f5;
const G_GREEN_DONE_1		:= 0x051a;
const G_GREEN_DONE_0		:= 0x0519;
const G_GREEN_OK_0			:= 0x0992;
const G_GREEN_OK_1			:= 0x0993;

// Oval buttons with gold border.
const G_RED_OVAL_CANCEL_1	:= 0x0818;
const G_RED_OVAL_CANCEL_0	:= 0x0817;
const G_RED_OVAL_PREV_1		:= 0x0913;
const G_RED_OVAL_PREV_0		:= 0x0912;
const G_GREEN_OVAL_OK_1		:= 0x081b;
const G_GREEN_OVAL_OK_0		:= 0x081a;
const G_YELLOW_OVAL_APPLY_1	:= 0x084b;
const G_YELLOW_OVAL_APPLY_0	:= 0x084a;

// Other Buttons
const G_SMALL_GEM_0			:= 0x04b9;
const G_SMALL_GEM_1			:= 0x04ba;
const G_TINY_GEM_0			:= 0x0837;
const G_TINY_GEM_1			:= 0x0838;

// Other.
const G_SQUARE_PLUS			:= 0x151a;
const G_SQUARE_MINUS		:= 0x1519;

// Words
const G_SKILLS				:= 0x0834;
const G_SKILL_USE			:= 0x0836;

// Stone Gumps
const G_TAB_0				:= 0x138e;
const G_TAB_1				:= 0x138f;
const G_MINUS_0				:= 0x138c;
const G_MINUS_1				:= 0x138d;
const G_STONE_SPHERE		:= 0x13a8;
const G_STONE_XBOX_0		:= 0x138a;
const G_STONE_XBOX_1		:= 0x138b;
const G_STONE_CHECK_0		:= 0x13ba;
const G_STONE_CHECK_1		:= 0x13bb;
const G_STONE_X_0			:= 0x13bc;
const G_STONE_X_1			:= 0x13bd;
const G_STONE_UP			:= 0x1458;
const G_STONE_DOWN			:= 0x1459;
const G_STONE_LEFT			:= 0x1467;
const G_STONE_RIGHT			:= 0x1468;
const G_STONE_SQUARE		:= 0x00d2;
const G_STONE_SQUARE_CHECK	:= 0x00d3;
const G_STONE_SQUARE_BLANK	:= 0x00d4;
const G_GRAY_CIRCLE_0		:= 0x00d0;
const G_GRAY_CIRCLE_1		:= 0x00d1;

// Arrows
const G_TRIANGLE_UP			:= 0x15e0;
const G_TRIANGLE_DOWN		:= 0x15e2;

// Tombstones
const G_TOMBSTONE_ROUND		:= 0x0065;
const G_TOMBSTONE_SQUARE	:= 0x0066;

// Seperators
const G_WAX_SEAL			:= 0x005c;
const G_FLAT_LINE			:= 0x005d;
const G_LEAF				:= 0x005e;

// COLOURS
const COL_YELLOW			:= 52;
const COL_RED				:= 32;
const COL_BLUE				:= 87;
const COL_DK_BLUE			:= 96;
const COL_LT_BLUE			:= 88;

// Text Entry Backgrounds
const G_TX_MARBLE_L			:= 0x0988;
const G_TX_MARBLE_S_LT		:= 0x098b;
const G_TX_MARBLE_S_DK		:= 0x098c;
const G_TX_MARBLE_M			:= 0x098d;
const G_TX_MARBLE_SQ_L		:= 0x098e;

/////////////////
//
//   Functions
//
/////////////////

/////////////////////////////////////////
// InitGump - Initialize Gump-Functions
/////////////////////////////////////////

Function InitGump()
  Var length:={
        {"",                                      // Dummy (for Speed...)
         CChr(0xc3),                              //  0
         "|",                                     //  2
         "`',.;:",                                //  3
         CChr(0x22)+"!",                          //  4
         "il1[]",                                 //  5
         "bcejotîˆ(){}="+CChr(0xb6),              //  6
         "adfpqrszIÑ‰~"+CChr(0xa4),               //  7
         "ghnuvyÅ¸ <>-"+CChr(0xbc),               //  8
         "xJ578$?ﬂ+",                             //  9
         "kCE2369^/*",                            // 10
         "mwABDFOPéƒô÷40%"+CChr(0x84)+CChr(0x96), // 11
         "GLNQSZ#",                               // 12
         "HRT&_",                                 // 13
         "Kö‹UY"+CChr(0x9c)+CChr(0x9f),           // 14
         "X",                                     // 15
         "V",                                     // 16
         "M",                                     // 18
         "W"},                                    // 21
        {CChr(0xc3)+CChr(0xc3)+CChr(0xc3)+CChr(0xc3)+CChr(0xc3)+CChr(0xc3)+CChr(0xc3)+CChr(0xc3)+CChr(0xc3)+ //  0 (Space as Filler is not allowed)
         CChr(0xc3)+CChr(0xc3)+CChr(0xc3)+CChr(0xc3)+CChr(0xc3)+CChr(0xc3)+CChr(0xc3)+CChr(0xc3)+            //  0
         "|||||||||||||||||"+                     //  2                                  78
         "`',.;::::::::::::"+                     //  3                                  31
         CChr(0x22)+"!!!!!!!!!!!!!!!!"+           //  4
         "il1[]]]]]]]]]]]]]"+                     //  5
         "bcejotîˆ(){}"+CChr(0xb6)+"===="+        //  6
         "adfpqrszIÑ‰"+CChr(0xa4)+"~~~~~"+        //  7
         "ghnuvyÅ¸ <>"+CChr(0xbc)+"-----"+        //  8
         "xJ578$?ﬂ+++++++++"+                     //  9
         "kCE2369^/********"+                     // 10
         "mwABDFOPéƒô÷40%"+CChr(0x84)+CChr(0x96)+ // 11
         "GLNQSZ###########"+                     // 12
         "HRT&_____________"+                     // 13
         "Kö‹UYYYYYYYYYYY"+CChr(0x9c)+CChr(0x9f)+ // 14
         "XXXXXXXXXXXXXXXXX"+                     // 15
         "\VVVVVVVVVVVVVVVV"+                     // 16
         "MMMMMMMMMMMMMMMMM"+                     // 18
         "WWWWWWWWWWWWWWWWW"},                    // 21
        {0, 2, 3, 4, 5, 6, 7, 8, 9, 10,
         11, 12, 13, 14, 15, 16, 18, 21}};

  SetGlobalProperty("GumpLength", length); // Und weg damit... nicht mehr in jedem Script separat speichern
  SetGlobalProperty("GumpCroppedFillerPoints", GumpTextWidthFast(croppedfiller));
EndFunction

//////////////////////////////////
// Returns the Width of GumpText
//////////////////////////////////

Function GumpTextWidth(gumptext)
  Var length:=GetGlobalProperty("GumpLength");
  Var start_i:=Len(length[1]);
  Var points:=0;
  Var j;
  Var i;

  For (j:=Len(gumptext); j>0; j-=1)
    i:=start_i;
    While ((i>0) And (!length[1][i][CStr(gumptext[j])]))
      i-=1;
    EndWhile;
    If (i>0) // siehe Speed-Dummy
      points:=points+length[3][i-1];
    Else
      SysLog("GumpTextWidth: Unknown Character = '"+gumptext[j]+"'");
    EndIf
  EndFor

  Return (points);
EndFunction

//////////////////////////////////
// Returns the Width of GumpText
//////////////////////////////////

Function GumpTextWidthFast(gumptext)
  Var length:=GetGlobalProperty("GumpLength");
  Var points:=0;
  Var j;
  Var i;

  For (j:=Len(gumptext); j>0; j-=1)
    i:=Find(length[2], gumptext[j], 1)-1;
    If (i>=0)
      points:=points+length[3][CInt(i/17)+1];
    Else
      SysLog("GumpTextWidthFast: Unknown Character = '"+gumptext[j]+"'");
    EndIf
  EndFor

  Return (points);
EndFunction

/////////////////////////////////////////
// Returns the X-Value (Left-Alignment)
/////////////////////////////////////////

Function GumpTextLeft(gumptext, x, width)
  Return (x);
EndFunction

//////////////////////////////////////////
// Returns the X-Value (Right-Alignment)
//////////////////////////////////////////

Function GumpTextRight(gumptext, x, width)
  Return (x+(width-GumpTextWidthFast(gumptext)));
EndFunction

////////////////////////////////////////
// Returns the X-Value (Mid-Alignment)
////////////////////////////////////////

Function GumpTextMid(gumptext, x, width)
  Return (x+CInt((width-GumpTextWidthFast(gumptext))/2));
EndFunction

/////////////////////////////////
// Returns the cropped GumpText
/////////////////////////////////

Function GumpTextCropped(gumptext, width)
  Var length:=GetGlobalProperty("GumpLength");
  Var croppedtext:="";

  If (GumpTextWidthFast(gumptext)>width)
    Var points:=width-GetGlobalProperty("GumpCroppedFillerPoints");
    Var j:=1;
    Var i;

    While (points>0)
      i:=Find(length[2], gumptext[j], 1)-1;
      If (i>=0)
        points:=points-length[3][CInt(i/15)+1];
        croppedtext:=croppedtext+gumptext[j];
      Else
        SysLog("GumpTextCropped: Unknown Character = '"+gumptext[j]+"'");
      EndIf

      j+=1;
    EndWhile

    croppedtext:=croppedtext+croppedfiller;
  Else
    croppedtext:=gumptext;
  EndIf

  Return (croppedtext);
EndFunction

//////////////////////////////////////////////////
// Create_ScrollBar - Erzeugt einen Scrollbalken
//////////////////////////////////////////////////

Function Create_ScrollBar(x, y, height, pages, page, layout)
  If (pages>1)
    Var x_str:=x+" ";
    Var button_x_str:="button "+x_str;
    Var gumppic_x_str:="gumppic "+x_str;
    Var page_before:=" 0 "+(page-1);
    Var page_next:=" 0 "+(page+1);

    Var y_mark;
    If (pages==page)
      y_mark:=y+height-46;
      layout.append(gumppic_x_str+(y_mark-7)+" 256");
    Else
      y_mark:=y+CInt((height-67)/(pages-1))*(page-1)+21;
      layout.append(gumppic_x_str+y_mark+" 256");
    EndIf
    layout.append("gumppic "+(x+1)+" "+y_mark+" 254");

    Var y_bkg;
    Var y_bkg_end;
    If (page>1)
      y_bkg_end:=y+21;
      For (y_bkg:=y_mark-32; y_bkg>y_bkg_end; y_bkg:=y_bkg-32)
        layout.append(button_x_str+y_bkg+" 256 256"+page_before);
      EndFor
      layout.append(button_x_str+y_bkg_end+" 256 256"+page_before);
      layout.append(button_x_str+y+" 250 251"+page_before);
    Else
      layout.append(gumppic_x_str+y+" 250");
    EndIf
    If (page<pages)
      y_bkg_end:=y+height-21-32;
      For (y_bkg:=y_mark+25; y_bkg<y_bkg_end; y_bkg+=32)
        layout.append(button_x_str+y_bkg+" 256 256"+page_next);
      EndFor
      layout.append(button_x_str+y_bkg_end+" 256 256"+page_next);
      layout.append(button_x_str+(y+height-21)+" 252 253"+page_next);
    Else
      layout.append(gumppic_x_str+(y+height-21)+" 252");
    EndIf
  EndIf

  Return (layout);
EndFunction
