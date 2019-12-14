use file;
use os;
use uo;
include "include/server";
include "include/properties";

// da werden die Maildateien abgelegt
Const MAIL_PATH :=           "::mail";
// aktuelle Mail-Nummer (fuer Dateinamen)
Const GPROP_MAIL_CURR_NUM := "#MailCurrent";
// Liste zu verschickender Mails
Const GPROP_MAIL_DONE :=     "#MailDone";
// Adresse des Staffs
// Adresse des Namensteams
// Adresse der Accountkuemmerer
// Absender-Adresse, die der Server benutzt
// Adresse, an die eventuelle Fehlermails gehen sollen
// (erst einmal an mich, sollte man spaeter durch eine spezielle Adresse
// ersetzen, die empfangene Mails auswertet und kaputte Adressen mit
// .invalid markiert; wahlweise auch an die Staffliste, aber IIRC filtert
// der Mailman derartige Bounces weg)

// Prop am Account, in der der Status bei Adresswechseln festgehalten wird
// struct{
//   time,     - wann wurde der Wechsel angefangen
//   addr,     - neue Adresse, die eingetragen werden soll
//   code_old, - Code, der an alte Adresse geschickt wurde
//   code_new, - Code, der an alte Adresse geschickt wurde
//   ok_old,   - 1, wenn Code (alte Adresse) eingegeben wurde, sonst 0
//   ok_new,   - 1, wenn Code (neue Adresse) eingegeben wurde, sonst 0
// }
Const MAIL_CHANGE_STATUS := "ChangeEmail";

// Liste wartender Mails abrufen
Function GetWaitingMail() // {{{
    return GetGlobalPropOrDefault(GPROP_MAIL_DONE, array{});
EndFunction // }}}

// eine Mail verschicken
Function SendMail(from, rcpt, subject, body) // {{{
    // from:    Account, Character oder String
    // rcpt:    Account, Character oder String
    // subject: String
    // body:    String oder Array

    // Checks {{{
    If (GetGlobalProperty(PROP_SHUTDOWN_RUNNING))
        // da ist nicht sicher, ob das schnell genug von aussen abgeholt
        // werden kann
        return error{errortext := "Keine Mailzustellung waehrend Shutdown"};
    EndIf

    // Absender
    var addrfrom := SendMailPrepareAddr(from);
    If (addrfrom == error)
        return addrfrom;
    EndIf
    // Empfaenger
    var addrrcpt := SendMailPrepareAddr(rcpt);
    If (addrrcpt == error)
        return addrrcpt;
    EndIf
    // Betreff
    If (TypeOfInt(subject) <> OT_STRING)
        return error{errortext := "Betreff muss ein String sein"};
    EndIf
    If (subject == "")
        return error{errortext := "Betreff darf nicht leer sein"};
    EndIf
    // Mailkoerper
    If (TypeOfInt(body) <> OT_STRING and TypeOfInt(body) <> OT_ARRAY)
        return error{errortext := "Body muss String oder Array sein"};
    EndIf
    If (TypeOfInt(body) == OT_STRING)
        If (body == "")
            return error{errortext := "Body darf nicht leer sein"};
        EndIf
        // in Array konvertieren
        body := array{body};
    ElseIf (TypeOfInt(body) == OT_ARRAY and body.size() == 0)
        return error{errortext := "Body darf nicht leer sein"};
    EndIf
    // Ende Parametercheck }}}

    // aktuelle Mail-Nummer besorgen und hochzaehlen {{{
    var crit := Is_Critical();
    Set_Critical(1);
    var mailid := CInt(GetGlobalProperty(GPROP_MAIL_CURR_NUM));
    SetGlobalProperty(GPROP_MAIL_CURR_NUM, 1+mailid);
    Set_Critical(crit);
    // }}}

    // Mail zurechtformatieren {{{
    // Header
    // TODO: Absender- und Empfaenger optional mit Namen ausstatten
    var mail := array{
        "Return-Path: "+            MADDR_ERROR,
        "From: "+                   addrfrom,
        "Sender: "+                 MADDR_SERVER,
        // Sender: wird von Mailinglisten ggf. ueberschrieben
        "X-Sender: "+               MADDR_SERVER,
        "To: "+                     addrrcpt,
        "Reply-To: "+               addrfrom,
        "Subject: "+                subject,
        "X-Mailer: "+               "Pergon-Mail-Modul",
        "MIME-Version: "+           "1.0",
        "Content-Transfer-Encoding: 8bit",
        "Content-Type: "+           "text/plain; charset=iso-8859-1",
        "" // Trennzeile zwischen Header und Body
    };
    // Body anhaengen
    // TODO: Body nach Sonderzeichen filtern, Zeilenumbruch, ...
    mail += body;
    // }}}

    // Mail in eine Datei schreiben {{{
    var mailfile := MAIL_PATH+"/"+mailid+".txt";
    var result   := WriteFile(mailfile, mail);
    If (result == error)
        return result;
    EndIf
    // }}}

    // Mail in die Liste fertiger Mails eintragen {{{
    crit := Is_Critical();
    Set_Critical(1);
    var list := GetWaitingMail();
    list.append(mailid);
    SetGlobalProperty(GPROP_MAIL_DONE, list);
    Set_Critical(crit);
    // }}}

    // damit kann man unter anderem Testen, ob die Mail noch da liegt
    return mailfile;

    // Nun ruft ein externer Job regelmaessig per Management die Liste
    // ab und verschickt alle Mails, die darin aufgelistet sind (soll
    // verhindern, dass halbfertige Mails verschickt werden, wenn man
    // nur das Verzeichnis ueberwacht); danach loescht er verschickte
    // Mail-Dateien wieder, damit er nichts doppelt sendet.
    // Durch die Verwendung temporaerer GProps geht es nach einem
    // Neustart von vorn los, daher spare ich mir das Aufraeumen
    // der Liste.
EndFunction // }}}

// Mail verschicken und warten, bis die tatsaechlich weg ist
Function SendMailBlocking(from, rcpt, subject, body, max_time := 120) // {{{
    // ganz normal SendMail aufrufen
    var mailfile := SendMail(from, rcpt, subject, body);
    If (mailfile == error)
        return mailfile;
    EndIf

    // einige Zeit regelmaessig testen, ob die Mail weg ist
    var now := ReadGameClock();
    While (FileExists(mailfile))
        Sleep(10);
        If (now+max_time < ReadGameClock())
            // Abbruch erzwingen
            return error{errortext := "Sending mail timed out"};
        EndIf
    EndWhile

    // soll sich ansonsten wie SendMail verhalten
    return mailfile;
EndFunction // }}}

// interne Funktion fuer Sendmail zur Adressaufbereitung
Function SendMailPrepareAddr(src) // {{{
    If (!src)
        return error{errortext := "Kein Absender"};
    EndIf
    If (src.isa(POLCLASS_NPC))
        return error{errortext := "NPCs haben keine Adressen im Account"};
    EndIf
    If (TypeOfInt(src) <> OT_STRING)
        // Adresse fuer Char bzw. Account holen
        src := GetMailAddress(src);
    EndIf
    If (IsInvalidMailAddress(src))
        return error{errortext := "Keine gueltige Absenderadresse: "+src};
    EndIf

    return src;
EndFunction // }}}

// holt die Mailadresse des Accounts/Spielers
// (Leerstring bei fehlender/ungueltiger Adresse)
Const MAIL_KILL_INVALID   := 0; // Leerstring bei "ungueltig"
Const MAIL_ACCEPT_INVALID := 1; // ungueltige Adresse erlauben
Function GetMailAddress(char_or_acct, invalid := MAIL_KILL_INVALID) // {{{
    If (TypeOfInt(char_or_acct) <> OT_ACCOUNTREF)
        char_or_acct := char_or_acct.acct;
    EndIf

    var mailaddr := char_or_acct.getprop(PLAYEREMAIL);
    If (!mailaddr)
        return "";
    EndIf

    If (IsInvalidMailAddress(mailaddr))
        If (invalid == MAIL_ACCEPT_INVALID)
            return mailaddr;
        EndIf
        return "";
    EndIf

    return mailaddr;
EndFunction // }}}

// ist eine Mailadresse als ungueltig (grob!)
Function IsInvalidMailAddress(addrstr) // {{{
    If (!addrstr)
        // offenbar leer oder gar kein String
        return 1;
    EndIf

        return 1;
    EndIf

    If (!addrstr["."])
        // ein "." muss schon sein
        return 1;
    EndIf

    If (Len(addrstr) - Find(addrstr, ".invalid", 1) == 7)
        // als ungueltig markiert
        return 1;
    EndIf

    return 0;
EndFunction // }}}

// .invalid hinten abschneiden
Function MailAddressOldFromInvalid(addrstr) // {{{
    return SubStr(addrstr, 1, Find(addrstr, ".invalid", 1) - 1);
EndFunction // }}}
