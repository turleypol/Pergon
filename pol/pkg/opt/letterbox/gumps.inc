include "include/gumputil";

// show send status?
Const SEND_STATUS    := 1;
Const SEND_NO_STATUS := 0;

// BUTTON IDs
Const NEW_LETTER_BTN  := 0xA0000;
Const DELETE_ALL_BTN  := 0xA0001;
Const DELETE_MSG_BTN  := 0xA0002;
Const CLOSE_EMAIL_BTN := 0xA0003;
Const WORD_WRAP_BTN := 0xA0004;
Const SEND_EMAIL_BTN  := 0xA0005;
Const ADDRESS_BOOK_BTN  := 0xA0006;
Const BLOCK_LIST_BTN  := 0xA0007;
Const UPDATE_INBOX_BTN  := 0xA0008;
Const UPDATE_LIST_BTN := 0xA0009;

// BUTTONS FOR READING EMAILS
Const READMAIL_START  := 0xB0009;

// THE SEND BUTTONS NEXT TO AN ADDRESS BOOK BUTTON
Const ADDYBOOK_START  := 0xC0009;

// TEXT ENTRY IDs
Const SEND_TO_TXT := 0xB01;
Const SUBJECT_TXT := 0xB02;
Const TXT_START   := 0xB05;

Function BuildInboxGump(byref box_id, byref data_elem)
  Var layout:={"nodispose",
               "noclose",
               "resizepic 0 0 3600 640 480",
               "resizepic 15 15 5054 480 450",
               "htmlgump 20 20 90 20 0 0 0",
               "htmlgump 70 20 195 20 1 0 0",
               "htmlgump 280 20 210 20 2 0 0",
               "resizepic 500 15 5054 125 50",
               "htmlgump 500 20 125 20 3 0 0",
               "htmlgump 500 40 125 20 4 0 0",
               "resizepic 500 70 5054 125 395",
               "button 505 83 2117 2118 1 0 "+UPDATE_INBOX_BTN,
               "htmlgump 525 80 100 20 5 0 0",
               "button 505 108 2117 2118 1 0 "+NEW_LETTER_BTN,
               "htmlgump 525 105 100 20 6 0 0",
               "button 505 133 2117 2118 1 0 "+DELETE_ALL_BTN,
               "htmlgump 525 130 100 20 7 0 0",
               "button 505 158 2117 2118 1 0 "+ADDRESS_BOOK_BTN,
               "htmlgump 525 155 100 20 8 0 0",
               "button 505 183 2117 2118 1 0 "+BLOCK_LIST_BTN,
               "htmlgump 525 180 100 20 9 0 0",
               "button 505 208 2117 2118 1 0 "+CLOSE_EMAIL_BTN,
               "htmlgump 525 205 100 20 10 0 0",
               "page 1"
              };
  Var data:={"<basefont color=#FFF8DC size=5>Lesen",
             "<basefont color=#FFF8DC size=5>Absender",
             "<basefont color=#FFF8DC size=5>Betreff",
             "<basefont color=#FFF8DC size=5><center>Briefkasten Nr</center>",
             "<basefont color=#FFF8DC size=5><center>"+CStr(box_id)+"</center>",
             "<basefont color=#FFF8DC size=5>Abrufen",
             "<basefont color=#FFF8DC size=5>Verfassen",
             "<basefont color=#FFF8DC size=5>Alle Löschen",
             "<basefont color=#FFF8DC size=5>Adressbuch",
             "<basefont color=#FFF8DC size=5>Ignorieren",
             "<basefont color=#FFF8DC size=5>Schließen"
            };

  Var y_pos := 50;
  Var counter := 1;
  Var page:=1;
  Var mail_info;
  ForEach mail_number in ( data_elem.PropNames() )
    If ( mail_number != "NextMsgID" )
      mail_info := data_elem.GetProp(mail_number);
      layout.append("button 30 "+(y_pos+3)+" 2117 2118 1 0 "+(READMAIL_START+CInt(mail_number)));
      layout.append("htmlgump 70 "+y_pos+" 195 20 "+data.size()+" 0 0");
      If (mail_info.read>0)
        data.append("<basefont color=#FFFFFF size=5>"+mail_info.from_name);
      Else
        data.append("<basefont color=#00008B size=5>"+mail_info.from_name);
      EndIf
      layout.append("htmlgump 280 "+y_pos+" 210 20 "+data.size()+" 0 0");
      If (mail_info.read>0)
        data.append("<basefont color=#FFFFFF size=5>"+mail_info.subject);
      Else
        data.append("<basefont color=#00008B size=5>"+mail_info.subject);
      EndIf
      y_pos+=20;
      counter+=1;

      If ( counter >= 20 )
        layout.append("button 465 440 2648 2649 0 "+(page+1)+" 1");
        page+=1;
        layout.append("page "+page);
        layout.append("button 465 20 2650 2651 0 "+(page-1)+" 1");
        counter := 1;
        y_pos := 50;
      EndIf
    EndIf
    SleepMS(5);
  EndForEach

  Return ({layout,data});
EndFunction

Function BuildReaderGump(email)
  Var layout:={"nodispose",
               "noclose",
               "resizepic 0 0 3600 640 480",
               "resizepic 15 15 5054 480 75",
               "htmlgump 20 20 300 20 0 0 0",
               "htmlgump 340 20 150 20 1 0 0",
               "htmlgump 20 40 400 20 2 0 0",
               "htmlgump 20 60 400 20 3 0 0",
               "resizepic 500 15 5054 125 75",
               "button 505 23 2117 2118 1 0 "+email.from_box,
               "htmlgump 525 20 100 20 4 0 0",
               "button 505 43 2117 2118 1 0 "+CLOSE_EMAIL_BTN,
               "htmlgump 525 40 100 20 5 0 0",
               "button 505 63 2117 2118 1 0 "+DELETE_MSG_BTN,
               "htmlgump 525 60 100 20 6 0 0",
               "resizepic 15 95 5054 610 370"
              };
  Var data:={"<basefont color=#FFF8DC size=5>Von : "+email.from_name,
             "<basefont color=#FFF8DC size=5><p align=right>Briefkasten : "+email.from_box+"</p>",
             "<basefont color=#FFF8DC size=5>Betreff : "+email.subject,
             "<basefont color=#FFF8DC size=5>Zeit : "+email.time,
             "<basefont color=#FFF8DC size=5>Antworten",
             "<basefont color=#FFF8DC size=5>Schließen",
             "<basefont color=#FFF8DC size=5>Löschen"
            };

  Var y_pos := 100;
  ForEach line in ( email.message )
    layout.append("text 20 "+y_pos+" 2100 "+data.size());
    data.append(line);
    y_pos+=20;

    SleepMS(2);
  EndForEach

  Return ({layout,data});
EndFunction

Function BuildBlockListGump(data_elem, byref entry_list)
  Var layout:={"nodispose",
               "page 0",
               "resizepic 0 0 3600 640 480",
               "resizepic 15 15 5054 610 450",
               "htmlgump 15 25 620 20 0 0 0",
               "button 25 428 2117 2118 1 0 "+UPDATE_LIST_BTN,
               "htmlgump 45 425 125 20 1 0 0",
               "button 225 428 2117 2118 1 0 "+DELETE_ALL_BTN,
               "htmlgump 245 425 125 20 2 0 0",
               "page 1"
              };

  Var data:={"<basefont color=#FFF8DC size=5><center>Ignorier Liste</center>",
             "<basefont color=#FFF8DC size=5>Aktualisieren",
             "<basefont color=#FFF8DC size=5>Liste löschen",
             "<basefont color=#FFF8DC size=5>Nummer",
             "<basefont color=#FFF8DC size=5>Name"
            };

  Var y_pos := 65;
  Var entry_num;
  Var counter := 1;
  Var entry_info;
  Var page:=1;
  For ( entry_num:=1; entry_num<=settings.BlockListEntries; entry_num+=1 )
    entry_info := data_elem.GetProp(Hex(entry_num));
    If ( !entry_info )
      entry_info := struct{"boxid":="", "name":=""};
    EndIf

    If ( entry_num % 2 == 0 )
      //
      // Right boxes in the line
      //
      layout.append("htmlgump 345 "+y_pos+" 125 20 3 0 0");
      layout.append("htmlgump 445 "+y_pos+" 125 20 4 0 0");
      layout.append("resizepic 345 "+(y_pos+20)+" 2620 100 30");
      layout.append("textentry 355 "+(y_pos+25)+" 80 20 2100 "+data.size()+" "+data.size());
      data.append(CStr(entry_info.boxid));
      layout.append("resizepic 440 "+(y_pos+20)+" 2620 180 30");
      layout.append("textentry 450 "+(y_pos+25)+" 180 20 2100 "+data.size()+" "+data.size());
      data.append(CStr(entry_info.name));

      y_pos+=50;
      counter+=1;
    Else
      //
      // Left boxes on the line
      //
      layout.append("htmlgump 20 "+y_pos+" 125 20 3 0 0");
      layout.append("htmlgump 120 "+y_pos+" 125 20 4 0 0");
      layout.append("resizepic 20 "+(y_pos+20)+" 2620 100 30");
      layout.append("textentry 30 "+(y_pos+25)+" 80 20 2100 "+data.size()+" "+data.size());
      data.append(CStr(entry_info.boxid));
      layout.append("resizepic 115 "+(y_pos+20)+" 2620 180 30");
      layout.append("textentry 125 "+(y_pos+25)+" 180 20 2100 "+data.size()+" "+data.size());
      data.append(CStr(entry_info.name));
    EndIf
    entry_list[entry_num] := entry_info;

    If ( ( counter > 7 ) && ( entry_num < settings.BlockListEntries) )
      layout.append("button 605 420 2648 2649 0 "+(page+1)+" 0");
      page+=1;
      layout.append("page "+page);
      layout.append("button 605 40 2650 2651 0 "+(page-1)+" 0");

      counter := 1;
      y_pos := 65;
    EndIf

    SleepMS(2);
  EndFor

  Return({layout,data});
EndFunction

Function BuildAddressBookGump(data_elem, byref entry_list)
  Var layout:={"nodispose",
               "page 0",
               "resizepic 0 0 3600 640 480",
               "resizepic 15 15 5054 610 450",
               "htmlgump 15 25 620 20 0 0 0",
               "button 25 428 2117 2118 1 0 "+UPDATE_LIST_BTN,
               "htmlgump 45 425 125 20 1 0 0",
               "button 225 428 2117 2118 1 0 "+DELETE_ALL_BTN,
               "htmlgump 245 425 125 20 2 0 0",
               "page 1"
              };
  Var data:={"<basefont color=#FFF8DC size=5>Adressbuch",
             "<basefont color=#FFF8DC size=5>Aktualisieren",
             "<basefont color=#FFF8DC size=5>Liste Löschen",
             "<basefont color=#FFF8DC size=5>Nummer",
             "<basefont color=#FFF8DC size=5>Name"
            };

  Var y_pos := 65;
  Var entry_num;
  Var counter := 1;
  Var entry_info;
  Var page:=1;
  For ( entry_num:=1; entry_num<=settings.AddressBookEntries; entry_num+=1 )
    entry_info := data_elem.GetProp(Hex(entry_num));
    If ( !entry_info )
      entry_info := struct{"boxid":="", "name":=""};
    EndIf

    If ( entry_num % 2 == 0 )
      //
      // Right boxes in the line
      //
      layout.append("htmlgump 345 "+y_pos+" 125 20 3 0 0");
      layout.append("htmlgump 445 "+y_pos+" 125 20 4 0 0");
      layout.append("resizepic 345 "+(y_pos+20)+" 2620 100 30");
      layout.append("textentry 355 "+(y_pos+25)+" 80 20 2100 "+data.size()+" "+data.size());
      data.append(CStr(entry_info.boxid));
      layout.append("resizepic 440 "+(y_pos+20)+" 2620 180 30");
      layout.append("textentry 450 "+(y_pos+25)+" 140 20 2100 "+data.size()+" "+data.size());
      data.append(CStr(entry_info.name));
      layout.append("button 590 "+(y_pos+25)+" 4029 4030 1 0 "+(ADDYBOOK_START+entry_num));
      y_pos +=50;
      counter +=1;
    Else
      //
      // Left boxes on the line
      //
      layout.append("htmlgump 20 "+y_pos+" 125 20 3 0 0");
      layout.append("htmlgump 120 "+y_pos+" 125 20 4 0 0");
      layout.append("resizepic 20 "+(y_pos+20)+" 2620 100 30");
      layout.append("textentry 30 "+(y_pos+25)+" 80 20 2100 "+data.size()+" "+data.size());
      data.append(CStr(entry_info.boxid));
      layout.append("resizepic 115 "+(y_pos+20)+" 2620 180 30");
      layout.append("textentry 125 "+(y_pos+25)+" 140 20 2100 "+data.size()+" "+data.size());
      data.append(CStr(entry_info.name));
      layout.append("button 265 "+(y_pos+23)+" 4029 4030 1 0 "+(ADDYBOOK_START+entry_num));
    EndIf
    entry_list[entry_num] := entry_info;

    If ( ( counter > 7 ) && ( entry_num < settings.AddressBookEntries) )
      layout.append("button 605 420 2648 2649 0 "+(page+1)+" 0");
      page+=1;
      layout.append("page "+page);
      layout.append("button 605 40 2650 2651 0 "+(page-1)+" 0");
      counter := 1;
      y_pos := 65;
    EndIf

    SleepMS(2);
  EndFor

  Return ({layout,data});
EndFunction

Function BuildComposeGump(send_to:="", subject:="", byref msg_array:=array)
  Var layout:={"nodispose",
               "noclose",
               "page 0",
               "resizepic 0 0 3600 640 480",
               "resizepic 15 15 5054 480 450",
               // Empfaengerzeile
               "htmlgump 20 23 125 20 0 0 0",
               "gumppic 100 20 1141",
               "textentry 110 22 250 20 2100 "+SEND_TO_TXT+" 7",
               // Adressen-Button mit Beschriftung
               "button 380 23 4008 4009 1 0 "+ADDRESS_BOOK_BTN,
               "htmlgump 415 23 125 20 1 0 0",
               // Ende Button
               "htmlgump 20 48 125 20 2 0 0",
               "gumppic 100 45 1141",
               "textentry 110 47 250 20 2100 "+SUBJECT_TXT+" 8",
               "htmlgump 20 70 480 20 3 0 0",
               "resizepic 500 15 5054 125 80",
               "button 505 23 2117 2118 1 0 "+SEND_EMAIL_BTN,
               "htmlgump 525 20 125 20 4 0 0",
               "button 505 43 2117 2118 1 0 "+WORD_WRAP_BTN,
               "htmlgump 525 40 125 20 5 0 0",
               "button 505 63 2117 2118 1 0 "+CLOSE_EMAIL_BTN,
               "htmlgump 525 60 125 20 6 0 0",
               "resizepic 15 95 2620 610 370"
              };

  Var data:={"<basefont color=#FFF8DC size=5>Empfänger:",
             "<basefont color=#FFF8DC size=5>Adressen",
             "<basefont color=#FFF8DC size=5>Betreff:",
             "<basefont color=#FFF8DC size=5>Mehrere Empfänger per Leerzeichen trennen.",
             "<basefont color=#FFF8DC size=5>Senden",
             "<basefont color=#FFF8DC size=5>Zeilenumbruch",
             "<basefont color=#FFF8DC size=5>Abbruch",
             CStr(send_to),
             CStr(subject)
            };

  Var y_pos := 100;
  Var i, text;
  For ( i:=1; i<=18; i+=1 )
    text := msg_array[i];
    if ( !text )
      text := "";
    endif
    layout.append("textentry 20 "+y_pos+" 595 20 2100 "+(TXT_START+i)+" "+data.size());
    data.append(text);
    msg_array[i] := TXT_START+i;
    y_pos+=20;
    SleepMS(2);
  EndFor

  Return ({layout,data});
EndFunction

Function GFWordWrap(text, pixels)
  Var wrap_lines := array{};
  Var word_list := SplitWords(text);

  Var space_size := GumpTextWidth(" ");

  Var cur_line := "";
  Var cur_length := 0;
  Var start_new := 0;

  ForEach word in ( word_list )
    Var word_width := GumpTextWidth(word);

    If ( word == "[P]" )
      // [P] will force a line break
      word := "";
      word_width := 0;
      start_new := 1;
    ElseIf ( (word_width+cur_length) > pixels )
      start_new := 1;
      If ( cur_line[Len(cur_line)] == " " )
        cur_line[Len(cur_line)] := "";
      EndIf
    EndIf

    If ( start_new )
      start_new := 0;
      wrap_lines.Append(cur_line);
      cur_line := "";
      cur_length := 0;
    EndIf
    If ( word )
      cur_line+=word+" ";
      cur_length+=word_width+space_size;
    EndIf

    SleepMS(2);
  EndForEach

  //Make sure nothing got left behind.
  If ( wrap_lines[wrap_lines.Size()] != cur_line )
    If ( cur_line[Len(cur_line)] == " " )
      cur_line[Len(cur_line)] := "";
    EndIf
    wrap_lines.Append(cur_line);
  EndIf

  Return(wrap_lines);
EndFunction
