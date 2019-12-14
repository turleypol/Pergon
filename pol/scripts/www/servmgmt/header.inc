/////////////////////////////////////////////////////////
// header.inc - Zentrale Funktionen für die Seiten
//
// Author: Turley

use http;
use polsys;
include "include/permissions";
include "include/server";
include "www/web";

var user := QueryParam("user");
If (!user)
  user := "";
EndIf
var pass := QueryParam("pass");
If (!pass)
  pass := "";
ElseIf (QueryParam("Einloggen"))
  pass:=MD5Encrypt(user+pass);
EndIf

var isALLOWED := AllowedToAccessWeb(user, pass);
var isALLOWED_STRICT := AllowedToAccessWebFull(user, pass);
var A_PARAM := "user="+user+"&amp;pass="+pass;

If (QueryParam("Einloggen"))
  //Damit Passwort nicht als Klartext erscheint Autorefresh der Seite
  var name:=GetProcess(GetPID()).name;
  name["scripts/www/servmgmt/"]:="";
  WriteHTML("<HEAD>");
  WriteHTML("<meta http-equiv='refresh' content='0; URL="+name+"?"+A_PARAM+"'>");
  WriteHTML("</HEAD>");
  Exit;
EndIf

Function DoHeader(title := "", css_override := "")
  WriteHTML("<HEAD>");
  var servername := GetGlobalProperty(GPROP_SERVERNAME);
  If (!servername)
    servername := "POL Server";
  EndIf
  WriteHTML("<TITLE>"+title+" &mdash; "+servername+" Management</TITLE>");
  If (css_override)
    WriteHTML(css_override);
  Else
    WriteHTML("<style type='text/css'>");
    WriteHTML("</style>");
  EndIf
  WriteHTML("</HEAD>");

  WriteHTML("<BODY>");
EndFunction

Function DenyAccess(allowed)
  If (!allowed)
    WriteHTML("<p class=warning>Access denied</p>");

    LoginThingie();

    WriteHTML("</div>");
    exit;
  EndIf
EndFunction

Function LoginThingie()
    WriteHTML("<fieldset class=login>");
    WriteHTML("<legend><a name=login>Login</a></legend>");
    WriteHTML("<form method=get>");
    WriteHTML("<table><tr><th>");
    WriteHTML("<label for=user>Account</label>");
    WriteHTML("</th><td>");
    WriteHTMLRaw("<input type=text id=user name=user size=20 maxsize=40");
    If (user)
        WriteHTMLRaw(" value="+user);
    EndIf
    WriteHTML(">");
    WriteHTML("</td></tr><tr><th>");
    WriteHTML("<label for=pass>Passwort</label>");
    WriteHTML("</th><td>");
    WriteHTML("<input type=password id=pass name=pass size=20 maxsize=80>");
    WriteHTML("</td></tr><tr><td colspan=2>");
    WriteHTML("<input type=hidden name=Einloggen value=1>");
    WriteHTML("<input type=submit value=Einloggen>");
    WriteHTML("</td></tr></table>");
    WriteHTML("</form></fieldset>");
EndFunction

Function DoFooter()
  WriteHTML("<DIV ID='footer'>");
  WriteHTML("<P>Copyright &copy; 2010 - Pergon</P>");
  WriteHTML("</DIV>");
  WriteHTML("</DIV>");
  WriteHTML("</BODY>");
  WriteHTML("</HTML>");
EndFunction

Function TableHeader(name := "")
  WriteHTML("<TABLE WIDTH='100%' CELLSPACING='1' CELLPADDING='2'>"
    +"<TR>"
    +"<TH VALIGN='TOP' ALIGN='LEFT' WIDTH='100%'><STRONG>"+name+"</STRONG></TH>"
    +"<TH WIDTH='100' VALIGN='TOP' ALIGN='CENTER' NOWRAP><A HREF='index.ecl?"+A_PARAM+"'><FONT SIZE=1>ServMgmt Home</FONT></A></TH>"
    +"</TR>"
    +"</TABLE>");
EndFunction
