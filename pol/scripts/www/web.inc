///////////////////////////////////////////////////////////////////////////
// zentrale Web-Funktionen

Function EscapeHTML(text)
    text := CStr(text);
    If (Len(text) > 0)
      text := StrReplace(text, CChr(0x22), "&quot;"); // "
      text := StrReplace(text, "<", "&lt;");
      text := StrReplace(text, ">", "&gt;");
    EndIf

    return text;
EndFunction

Function EscapeLink(text)
    text := CStr(text);
    If (Len(text) > 0)
      text := StrReplace(text," ","%20");
    EndIf

    return text;
EndFunction

// Quoting-Hilfe
Function q(str)
  return CChr(0x22)+str+CChr(0x22);
EndFunction
