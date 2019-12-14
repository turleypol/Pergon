///////////////////////////////////////////////////////////////////////////
// PergonInfos Include - Liefert nuetzliche Infos rund um Pergon
//
// Author: Shinigami

///////////////////////////////////////////////////////////////////////////
// Statistiken
//   CountAllAccountsAndCharacters    - Wieviele Accounts und Character existieren?
//   CountActiveCharacters            - Wieviele Character wurden in letzter Zeit benutzt?
//   CountInActiveCharacters          - Wieviele Character wurden in letzter Zeit nicht benutzt?
//   CountUnusedAccounts              - Wieviele ungenutze Accounts gibt es?
//   ListBerufe                       - Erstellt eine Liste der benutzen Berufe
//   ListBerufeExactly                - Erstellt eine ausfuehrlichere Liste der benutzen Berufe
//   ListBerufeInfos                  - Erstellt eine Liste der genutzten Berufe (fuer Webserver)
//   ListDeathCounterInfos            - Erstellt eine Liste der meiststerbenden Character (fuer Webserver)
//   ListQuestInfos                   - Erstellt eine Liste der aktivsten Quester (fuer Webserver)
//   ListHouseInfos                   - Erstellt eine Liste der Haeusern (fuer Webserver)
//   ListeMailAddresses               - Alle eMail-Adressen auflisten
//   ListOnlineCharacterInfos         - Erstellt eine Liste der eingeloggten Character (fuer Webserver)
//   ListCharacterInfos               - Erstellt eine Liste der Character (fuer Webserver)
//   ListCMDLevelCharacterInfo        - Erstellt eine Liste der CMDLevel-Character
//   ListGuildInfos                   - Erstellt eine Liste mit allen Gildeninfos
//   ListNewsAccounts                 - Erstellt eine Liste aller NewsAccounts (fuer NewsServer)
//   ListSpawnNet                     - Erstellt eine Liste aller SpawnRunen (fuer Webserver)
//   GetServerInfos                   - Liefert bestimmte Server-Informationen (im Sphere-Stil... evil)
//
// Pruefroutinen
//   CheckItemDescContainerUseScripts - Ueberprueft, ob die Container keine UseScripts aufweisen
//   LogContainerMaxProperties        - Gibt die Max-Properties der Container aus (zum Balancing)
//   CheckItemsForIllegalObjType      - Ueberprueft, ob die Items einen korrekten ObjType haben
//   CheckAccountProperties           - Ueberprueft bestimmte Werte in den Accounts (z.B. eMail-Adresse)
//   CheckNPCDescProperties           - Ueberprueft bestimmte Werte in der NPCDesc
//   ListSpawnNet                     - Erstellt Liste aller Spawnrunen
//   SaveXML                          - ?

use os;
include "include/place";
include "include/server";

// Konstanten
// Liefere die Daten fuer das AUX-Interface oder als Datei
Const LIST_AUX  := 0;
Const LIST_FILE := 1;

// Character sind [potentiell] eingeloggt oder nicht
Const SERVER_OFFLINE := 0;
Const SERVER_ONLINE  := 1;

/////////////////////
//                 //
//   Statistiken   //
//                 //
/////////////////////
use datafile;
////////////////////////////////////////////////////////////////////////////////
// CountAllAccountsAndCharacters - Wieviele Accounts und Character existieren?
////////////////////////////////////////////////////////////////////////////////

Function CountAllAccountsAndCharacters()
  Var accounts_all:=0; // Anzahl aller Accounts
  Var chars_all:=0;    // Anzahl aller Character

  Var active_check:=ReadGameClock()-ACTIVE_TIME;

  ForEach accountname in ListAccounts() // Alle Accountnamen auflisten
    Var account:=FindAccount(accountname); // ...und den Account dazu suchen
    If (account) // Sollte eigentlich immer funzen...
      For charnr:=1 To 5 // Jeder Account kann bis zu 5 Character haben
        Var char:=account.getcharacter(charnr);
        If (char) // Der Character existiert auch (muss nicht zwingend!)
          chars_all+=1; // Hochzaehlen
        EndIf
      EndFor

      accounts_all+=1; // Hochzaehlen
    Else
      SysLog("FEHLER: '"+accountname+"' gibt es nicht !");
    EndIf
  EndForEach

  Return ({accounts_all, chars_all});
EndFunction

///////////////////////////////////////////////////////////////////////////////
// CountActiveCharacters - Wieviele Character wurden in letzter Zeit benutzt?
///////////////////////////////////////////////////////////////////////////////

Function CountActiveCharacters(loggen:=0)
  Var accounts_all:=0;       // Anzahl aller Accounts                                            (inkl. der aktiven)
  Var accounts_all_gm:=0;    // Anzahl aller Accounts mit mind. 1 CommandLevel-Character         (inkl. der aktiven)
  Var accounts_active:=0;    // Anzahl aller aktiven Accounts
  Var accounts_active_gm:=0; // Anzahl aller aktiven Accounts mit mind. 1 CommandLevel-Character
  Var chars_all:=0;          // Anzahl aller Character                                           (inkl. der aktiven)
  Var chars_all_gm:=0;       // Anzahl aller CommandLevel-Character                              (inkl. der aktiven)
  Var chars_active:=0;       // Anzahl aller aktiven Character
  Var chars_active_gm:=0;    // Anzahl aller aktiven CommandLevel-Character

  Var active_check:=ReadGameClock()-ACTIVE_TIME;

  ForEach accountname in ListAccounts() // Alle Accountnamen auflisten
    Var account:=FindAccount(accountname); // ...und den Account dazu suchen
    If (account) // Sollte eigentlich immer funzen...
      Var account_active:=0; // Ist der Account aktiv?
      Var account_gm:=0;     // Besitzt der Account einen CommandLevel-Character?
      Var logstring:="";       // String, der ausgegeben wird

      For charnr:=1 To 5 // Jeder Account kann bis zu 5 Character haben
        Var char_active:=0; // Ist der Character aktiv?

        Var char:=account.getcharacter(charnr);
        If (char) // Der Character existiert auch (muss nicht zwingend!)
          If (!GetObjProperty(char, "LastLog"))
            SetObjProperty(char, "LastLog", 1); // Mal irgendwie initialisieren
          EndIf

          If (GetObjProperty(char, "LastLog")>=active_check) // Ist der Character aktiv?
            char_active:=1;    // Jupp
            account_active:=1; // ...und damit der Account auch

            If (logstring=="") // Erster aktiver Character im Account?
              logstring:=accountname+";"+account.getprop(PLAYEREMAIL)+";";
            EndIf
          EndIf

          If (char.cmdlevel >= CMDLEVEL_SEER) // Besitzt der Account einen CommandLevel-Character?
            chars_all_gm+=1; // Hochzaehlen

            If (char_active)
              chars_active_gm+=1; // Hochzaehlen

              logstring+=GetCMDLevelTitle(char.cmdlevel)+" "+char.name+" ["+
                CInt((ReadGameClock()-GetObjProperty(char, "LastLog"))/86400)+" Tage];";
            EndIf

            account_gm:=1; // Jupp
          Else
            If (char_active)
              logstring:=logstring+"Player "+char.name+";";
            EndIf
          EndIf

          chars_all+=1; // Hochzaehlen
          If (char_active)
            chars_active+=1; // Hochzaehlen
          EndIf
        EndIf
      EndFor

      If (loggen) // Daten ausgeben?
        If (logstring<>"") // Irgendein inaktiver Character im Account?
          SysLog(logstring); // Ausgeben
        EndIf
      EndIf

      If (account_gm) // Besitzt der Account einen CommandLevel-Character?
        accounts_all_gm+=1; // Hochzaehlen
        If (account_active)
          accounts_active_gm+=1; // Hochzaehlen
        EndIf
      EndIf

      accounts_all+=1; // Hochzaehlen
      If (account_active)
        accounts_active+=1; // Hochzaehlen
      EndIf
    Else
      SysLog("FEHLER: '"+accountname+"' gibt es nicht !");
    EndIf
    SleepMS(2);
  EndForEach

  Return ({ACTIVE_TIME, accounts_all, accounts_all_gm, accounts_active, accounts_active_gm, chars_all, chars_all_gm, chars_active, chars_active_gm});
EndFunction

///////////////////////////////////////////////////////////////////////////////////////
// CountInActiveCharacters - Wieviele Character wurden in letzter Zeit nicht benutzt?
///////////////////////////////////////////////////////////////////////////////////////

Function CountInActiveCharacters(loggen:=0, timeframe:=ACTIVE_TIME, delete_character:=0)
  Var accounts_all:=0;         // Anzahl aller Accounts                                              (inkl. der inaktiven)
  Var accounts_all_gm:=0;      // Anzahl aller Accounts mit mind. 1 CommandLevel-Character           (inkl. der inaktiven)
  Var accounts_inactive:=0;    // Anzahl aller inaktiven Accounts
  Var accounts_inactive_gm:=0; // Anzahl aller inaktiven Accounts mit mind. 1 CommandLevel-Character
  Var chars_all:=0;            // Anzahl aller Character                                             (inkl. der inaktiven)
  Var chars_all_gm:=0;         // Anzahl aller CommandLevel-Character                                (inkl. der inaktiven)
  Var chars_inactive:=0;       // Anzahl aller inaktiven Character
  Var chars_inactive_gm:=0;    // Anzahl aller inaktiven CommandLevel-Character

  If (delete_character) // Schutzfunktion, um ungewolltes Loeschen zu verhindern
    If (timeframe<ANCIENT_TIME)
      SysLog("WARNUNG: TimeFrame wird in CountInActiveCharacters von "+timeframe+" auf "+ANCIENT_TIME+" hochgesetzt!");
      timeframe:=ANCIENT_TIME;
    EndIf

    If (loggen) // Daten ausgeben?
      SysLog("Antike Character werden geloescht");
    EndIf
  EndIf

  Var active_check:=ReadGameClock()-timeframe;

  ForEach accountname in ListAccounts() // Alle Accountnamen auflisten
    Var account:=FindAccount(accountname); // ...und den Account dazu suchen
    If (account) // Sollte eigentlich immer funzen...
      Var account_inactive:=1; // Ist der Account inaktiv?
      Var account_gm:=0;       // Besitzt der Account einen CommandLevel-Character?
      Var logstring:="";       // String, der ausgegeben wird

      For charnr:=1 To 5 // Jeder Account kann bis zu 5 Character haben
        Var char_inactive:=0; // Ist der Character inaktiv?

        Var char:=account.getcharacter(charnr);
        If (char) // Der Character existiert auch (muss nicht zwingend!)
          If (!GetObjProperty(char, "LastLog"))
            SetObjProperty(char, "LastLog", 1); // Mal irgendwie initialisieren
          EndIf

          If (GetObjProperty(char, "LastLog")<active_check) // Ist der Character inaktiv?
            char_inactive:=1;    // Jupp

            If (logstring=="") // Erster inaktiver Character im Account?
              logstring:=accountname+";"+account.getprop(PLAYEREMAIL)+";";
            EndIf
          Else // Noe
            account_inactive:=0; // ...und damit der Account nicht mehr
          EndIf

          If (char.cmdlevel >= CMDLEVEL_SEER) // Besitzt der Account einen CommandLevel-Character?
            chars_all_gm+=1; // Hochzaehlen

            If (char_inactive)
              chars_inactive_gm+=1; // Hochzaehlen

              logstring:=logstring+GetCMDLevelTitle(char.cmdlevel)+" "+char.name+" ["+
                CInt((ReadGameClock()-GetObjProperty(char, "LastLog"))/86400)+" Tage];";
            EndIf

            account_gm:=1; // Jupp
          Else
            If (char_inactive)
              logstring+="Player "+char.name+";";
            EndIf
          EndIf

          chars_all+=1; // Hochzaehlen
          If (char_inactive)
            If (delete_character) // Character loeschen?
              DeleteCharacterPergon(account, charnr);
            EndIf

            chars_inactive+=1; // Hochzaehlen
          EndIf
        EndIf
      EndFor

      If (loggen) // Daten ausgeben?
        If (logstring<>"") // Irgendein inaktiver Character im Account?
          SysLog(logstring); // Ausgeben
        EndIf
      EndIf

      If (account_gm) // Besitzt der Account einen CommandLevel-Character?
        accounts_all_gm+=1; // Hochzaehlen
        If (account_inactive)
          accounts_inactive_gm+=1; // Hochzaehlen
        EndIf
      EndIf

      accounts_all+=1; // Hochzaehlen
      If (account_inactive)
        accounts_inactive+=1; // Hochzaehlen
      EndIf
    Else
      SysLog("FEHLER: '"+accountname+"' gibt es nicht !");
    EndIf
  EndForEach

  Return ({timeframe, accounts_all, accounts_all_gm, accounts_inactive, accounts_inactive_gm, chars_all, chars_all_gm, chars_inactive, chars_inactive_gm});
EndFunction

///////////////////////////////////////////////////////////////
// CountUnusedAccounts - Wieviele ungenutze Accounts gibt es?
///////////////////////////////////////////////////////////////

Function CountUnusedAccounts()
  Var accounts_all:=0;   // Anzahl aller Accounts
  Var accounts_empty:=0; // Anzahl aller ungenutzten Accounts

  ForEach accountname in ListAccounts() // Alle Accountnamen auflisten
    Var account:=FindAccount(accountname); // ...und den Account dazu suchen
    If (account) // Sollte eigentlich immer funzen...
      Var account_chars:=0;    // Besitzt der Account Character?

      For charnr:=1 To 5 // Jeder Account kann bis zu 5 Character haben
        Var char:=account.getcharacter(charnr);
        If (char) // Der Character existiert auch (muss nicht zwingend!)
          account_chars+=1;
        EndIf
      EndFor

      If (!account_chars) // Der Account ist leer...
        accounts_empty+=1; // Hochzaehlen

        SysLog(accountname+";"+account.getprop(PLAYEREMAIL)); // Ausgeben
      EndIf

      accounts_all+=1; // Hochzaehlen
    Else
      SysLog("FEHLER: '"+accountname+"' gibt es nicht !");
    EndIf
  EndForEach

  Return ({accounts_all, accounts_empty});
EndFunction

/////////////////////////////////////////////////////////
// ListBerufe - Erstellt eine Liste der benutzen Berufe
/////////////////////////////////////////////////////////

Function ListBerufe()
  Var active_check:=ReadGameClock()-ACTIVE_TIME; // Wann gilt ein Character als aktiv?
  Var klassenberufe:=KlassenBerufe_einlesen();  // Klassen und Berufe einlesen
  Var berufe:=Dictionary;

  ForEach accountname in ListAccounts() // Alle Accountnamen auflisten
    Var account:=FindAccount(accountname); // ...und den Account dazu suchen
    If (account) // Sollte eigentlich immer funzen...
      For charnr:=1 To 5 // Jeder Account kann bis zu 5 Character haben
        Var char:=account.getcharacter(charnr);
        If (char) // Der Character existiert auch (muss nicht zwingend!)
          If (char.cmdlevel==CMDLEVEL_PLAYER) // CommandLevel-Character haben keinen Beruf
            If (!GetObjProperty(char, "LastLog"))
              SetObjProperty(char, "LastLog", 1); // Mal irgendwie initialisieren
            EndIf

            If (GetObjProperty(char, "LastLog")>=active_check) // Ist der Character aktiv?
              Var klasseberuf:=KlasseBeruf_checken(char, klassenberufe); // Klasse und Beruf ueberpruefen
              If (klasseberuf)
                If (!berufe[klasseberuf.klasse]) // Wurde die Klasse schon angelegt?
                  berufe[klasseberuf.klasse]:=Dictionary;
                EndIf

                If (berufe[klasseberuf.klasse][klasseberuf.beruf]) // Anzahl der Berufsinhaber hochzaehlen
                  If (GetObjProperty(char, TYPNEWBIE))
                    berufe[klasseberuf.klasse][klasseberuf.beruf][2]:=berufe[klasseberuf.klasse][klasseberuf.beruf][2]+1;
                  Else
                    berufe[klasseberuf.klasse][klasseberuf.beruf][1]:=berufe[klasseberuf.klasse][klasseberuf.beruf][1]+1;
                  EndIf
                Else
                  If (GetObjProperty(char, TYPNEWBIE))
                    berufe[klasseberuf.klasse][klasseberuf.beruf]:={0, 1};
                  Else
                    berufe[klasseberuf.klasse][klasseberuf.beruf]:={1, 0};
                  EndIf
                EndIf
              EndIf
            EndIf
          EndIf
        EndIf
      EndFor
    Else
      SysLog("FEHLER: '"+accountname+"' gibt es nicht !");
    EndIf
  EndForEach

  Return (berufe);
EndFunction

////////////////////////////////////////////////////////////////////////////////
// ListBerufeExactly - Erstellt eine ausfuehrlichere Liste der benutzen Berufe
////////////////////////////////////////////////////////////////////////////////

Function ListBerufeExactly()
  Var active_check:=ReadGameClock()-ACTIVE_TIME; // Wann gilt ein Character als aktiv?
  Var klassenberufe:=KlassenBerufe_einlesen();  // Klassen und Berufe einlesen
  Var berufe:=Dictionary;

  ForEach accountname in ListAccounts() // Alle Accountnamen auflisten
    Var account:=FindAccount(accountname); // ...und den Account dazu suchen
    If (account) // Sollte eigentlich immer funzen...
      For charnr:=1 To 5 // Jeder Account kann bis zu 5 Character haben
        Var char:=account.getcharacter(charnr);
        If (char) // Der Character existiert auch (muss nicht zwingend!)
          If (char.cmdlevel==CMDLEVEL_PLAYER) // CommandLevel-Character haben keinen Beruf
            If (!GetObjProperty(char, "LastLog"))
              SetObjProperty(char, "LastLog", 1); // Mal irgendwie initialisieren
            EndIf

            If (GetObjProperty(char, "LastLog")>=active_check) // Ist der Character aktiv?
              Var klasseberuf:=KlasseBeruf_checken(char, klassenberufe); // Klasse und Beruf ueberpruefen
              If (klasseberuf)
                If (!berufe[klasseberuf.klasse]) // Wurde die Klasse schon angelegt?
                  berufe[klasseberuf.klasse]:=Dictionary;
                EndIf

                If (berufe[klasseberuf.klasse][klasseberuf.beruf]) // Anzahl der Berufsinhaber hochzaehlen
                  If (GetObjProperty(char, TYPNEWBIE))
                    berufe[klasseberuf.klasse][klasseberuf.beruf][2].append(char.name+"/"+char.acctname);
                  Else
                    berufe[klasseberuf.klasse][klasseberuf.beruf][1].append(char.name+"/"+char.acctname);
                  EndIf
                Else
                  If (GetObjProperty(char, TYPNEWBIE))
                    berufe[klasseberuf.klasse][klasseberuf.beruf]:={{}, {char.name+"/"+char.acctname}};
                  Else
                    berufe[klasseberuf.klasse][klasseberuf.beruf]:={{char.name+"/"+char.acctname}, {}};
                  EndIf
                EndIf
              EndIf
            EndIf
          EndIf
        EndIf
      EndFor
    Else
      SysLog("FEHLER: '"+accountname+"' gibt es nicht !");
    EndIf
  EndForEach

  Return (berufe);
EndFunction

////////////////////////////////////////////////////////////////////////////////
// ListBerufeInfos - Erstellt eine Liste der genutzten Berufe (fuer Webserver)
////////////////////////////////////////////////////////////////////////////////

Function ListBerufeInfos()
  Var c22:=CChr(0x22);
  Var content:={"<?xml version="+c22+"1.0"+c22+" ?>", "<klassen>"};

  ForEach klasse in ListBerufe()
    Var anzahl:=0;
    Var anzahljhelom:=0;

    content.append("    <klasse>");
    content.append("        <name>"+_klasse_iter+"</name>");

    ForEach beruf in klasse
      If (_beruf_iter<>"None")
        content.append("        <beruf>");
        content.append("            <name>"+_beruf_iter+"</name>");
        content.append("            <anzahl>"+beruf[1]+"</anzahl>");
        content.append("            <anzahljhelom>"+beruf[2]+"</anzahljhelom>");
        content.append("        </beruf>");

        anzahl:=anzahl+beruf[1];
        anzahljhelom:=anzahljhelom+beruf[2];
      EndIf
    EndForEach

    content.append("        <gesamtzahl>"+anzahl+"</gesamtzahl>");
    content.append("        <gesamtzahljhelom>"+anzahljhelom+"</gesamtzahljhelom>");
    content.append("    </klasse>");
  EndForEach

  content.append("</klassen>");

  WriteFile("daten_berufe.xml", content);
EndFunction

///////////////////////////////////////////////////////////////////////////////////////////////
// ListDeathCounterInfos - Erstellt eine Liste der meiststerbenden Character (fuer Webserver)
///////////////////////////////////////////////////////////////////////////////////////////////

Function ListDeathCounterInfos()
  Var active_check:=ReadGameClock()-ACTIVE_TIME; // Wann gilt ein Character als aktiv?
  Var deathcounter:=Dictionary;

  ForEach accountname in ListAccounts() // Alle Accountnamen auflisten
    Var account:=FindAccount(accountname); // ...und den Account dazu suchen
    If (account) // Sollte eigentlich immer funzen...
      For charnr:=1 To 5 // Jeder Account kann bis zu 5 Character haben
        Var char:=account.getcharacter(charnr);
        If (char) // Der Character existiert auch (muss nicht zwingend!)
          If (char.cmdlevel==CMDLEVEL_PLAYER) // CommandLevel-Character haben keinen Beruf
            If (!GetObjProperty(char, "LastLog"))
              SetObjProperty(char, "LastLog", 1); // Mal irgendwie initialisieren
            EndIf

            If (GetObjProperty(char, "LastLog")>=active_check) // Ist der Character aktiv?
              Var counter:=CInt(GetObjProperty(char, "DeathCounter"));
              If (counter)
                counter:=-counter; // Damit sortiert er das gleich richtig ;o)
              Else
                counter:=0;
              EndIf

              If (deathcounter[counter])
                deathcounter[counter][char.name]:=1;
              Else
                deathcounter[counter]:=Dictionary; // Namen auf selbem Level auch sortieren
                deathcounter[counter][char.name]:=1;
              EndIf
            EndIf
          EndIf
        EndIf
      EndFor
    Else
      SysLog("FEHLER: '"+accountname+"' gibt es nicht !");
    EndIf
  EndForEach

  Var anzahl:=40;

  Var c22:=CChr(0x22);
  Var content:={"<?xml version="+c22+"1.0"+c22+" ?>", "<gestorben>", "    <anzahl>"+anzahl+"</anzahl>"};

  ForEach counterlevel in deathcounter
    ForEach character in counterlevel
      content.append("    <tod>");
      content.append("        <name>"+_character_iter+"</name>");
      content.append("        <anzahl>"+(-_counterlevel_iter)+"</anzahl>");
      content.append("    </tod>");

      anzahl-=1;
      If (anzahl==0)
        Break;
      EndIf
      // Compilerwarnung unterdruecken
      character := character;
    EndForEach

    If (anzahl==0)
      Break;
    EndIf
  EndForEach

  content.append("</gestorben>");

  WriteFile("daten_deathcounter.xml", content);
EndFunction

////////////////////////////////////////////////////////////////////////////////
// ListQuestInfos - Erstellt eine Liste der aktivsten Quester (fuer Webserver)
////////////////////////////////////////////////////////////////////////////////

Function ListQuestInfos()
  //Listet aktiven Player und wieviele Quests (in Prozenten) gelöst
  //Anzeige der Top Ten Player
  Var active_check:=ReadGameClock()-ACTIVE_TIME; // Wann gilt ein Character als aktiv?
  Var questtopten := Dictionary;
  Var anzahl:=0;
  Var questcfg:=ReadConfigFile(":questsystem:quests");
  var maxanzahlquests:=CInt(questcfg["Anzahl"].max);

  //Alle Accounts durchsuchen
  Foreach accountname in ListAccounts()
    var account:=FindAccount(accountname);
    //Aus Sicherheit mal testen obs den Account gibt
    if (account)
      //Jetzt die maximalen 5 Character pro Accounts durchlaufen
      For charnr:=1 to 5
        Var char:=account.getcharacter(charnr);
        if (char)
          //CMDLEVEL können zwar auch Quests machen, aber das ist ja unfair
          if (char.cmdlevel==CMDLEVEL_PLAYER)
            If (!GetObjProperty(char, "LastLog"))
              SetObjProperty(char, "LastLog", 1); // Mal irgendwie initialisieren
            EndIf
            //Nur aktive Charactere gehören aufgelistet
            if (GetObjProperty(char, "LastLog")>=active_check)
              if (char.getprop("quest"))
                var completed_quests := char.getprop("quest")["completedquests"].size();
                if (completed_quests)
                  var guete:=-(completed_quests*100)/maxanzahlquests;
                  //Jetzt hab ich den Player und seine Questguete
                  if (questtopten[guete])
                    questtopten[guete][char.name]:=1;
                  else
                    questtopten[guete]:=Dictionary; // Namen auf selbem Level auch sortieren
                    questtopten[guete][char.name]:=1;
                  endif
                  anzahl+=1;
                endif
              endif
            endif
          endif
        endif
      endfor
    else
      SysLog("FEHLER: '"+accountname+"' gibt es nicht !");
    endif
  endforeach

  if (anzahl>40)
    anzahl:=40;
  endif

  //Aus dem DeathCounterInfo geklaut *g*
  Var c22:=CChr(0x22);
  Var content:={"<?xml version="+c22+"1.0"+c22+" ?>", "<questerfuellt>", "    <anzahl>"+anzahl+"</anzahl>"};

  ForEach counterlevel in questtopten
    ForEach character in counterlevel
      content.append("    <quests>");
      content.append("        <name>"+_character_iter+"</name>");
      content.append("        <guete>"+(-_counterlevel_iter)+"</guete>");
      content.append("    </quests>");

      anzahl-=1;
      If (anzahl==0)
        Break;
      EndIf
      // Compilerwarnung unterdruecken
      character := character;
    EndForEach

    If (anzahl==0)
      Break;
    EndIf
  EndForEach

  content.append("</questerfuellt>");

  WriteFile("daten_quester.xml", content);
EndFunction

///////////////////////////////////////////////////////////////////////
// ListHouseInfos - Erstellt eine Liste der Haeuser (fuer Webserver)
///////////////////////////////////////////////////////////////////////

Function ListHouseInfos()
  var itemdesccfg := ReadConfigFile("::itemdesc");
  If (!itemdesccfg)
    syslog("ListHouseInfos nicht moeglich");
    return;
  EndIf

  var content := array{
    "<?php",
    "",
    "/*****************************/",
    "/** Pergon_Houses_verwalten **/",
    "/*****************************/",
    "",
    "Function Pergon_Houses_verwalten()",
    "{"
  };

  var c22 := CChr(0x22);
  var s22 := c22+","+c22;
  ForEach range in array{array{0x6019, 0x6026}, array{0x6227, 0x628a}}
    For objtype:=range[1] To range[2]
      var housedeed := itemdesccfg[objtype];
      If (!housedeed)
        // kaputte Haeuser ueberspringen
        continue;
      EndIf

      If (!housedeed.vendorsellsfor)
        // unverkaeufliche Haeuser ueberspringen
        continue;
      EndIf

      var multiid := GetConfigInt(
        itemdesccfg[GetObjtypeByName(housedeed.houseobjtype)], "MultiID"
      );

      // Namen vom Bauplan kuerzen
      var name := housedeed.desc;
      name["Baupla%ene/n% fuer "] := "";
      name["Aufbauanleitung%en% fuer "] := "";

      content.append(
        "  Pergon_House_anlegen("+c22+
        multiid+s22+
        Lower(Hex(multiid))+s22+
        name+s22+
        housedeed.vendorsellsfor+s22+
        housedeed.bretter+s22+
        housedeed.staemme+s22+
        housedeed.sandstein+s22+
        housedeed.granit+s22+
        housedeed.marmor+s22+
        housedeed.lehm+s22+
        housedeed.stroh+s22+
        housedeed.tuecher+s22+
        housedeed.barren+s22+
        housedeed.leder+s22+
        housedeed.kristall+s22+
        housedeed.housetype+c22+
        ");"
      );
    EndFor
  EndForEach

  content.append("}");
  content.append("");
  content.append("?>");

  WriteFile("daten_houses.php", content);
EndFunction

///////////////////////////////////////////////////////
// ListeMailAddresses - Alle eMail-Adressen auflisten
///////////////////////////////////////////////////////

Function ListeMailAddresses()
  Var usedinactiveaddresses:={};
  Var usedactiveaddresses:={};
  Var unusedaddresses:={};
  Var unusedinactiveaddresses:={}; // UnUsed and InActive

  Var active_check:=ReadGameClock()-ACTIVE_TIME;

  ForEach accountname in ListAccounts() // Alle Accountnamen auflisten
    Var account:=FindAccount(accountname); // ...und den Account dazu suchen
    If (account) // Sollte eigentlich immer funzen...
      Var address:=account.getprop(PLAYEREMAIL);
      If (address)
        Var account_active:=0;
        Var numberchars:=0;

        For charnr:=1 To 5 // Jeder Account kann bis zu 5 Character haben
          Var char:=account.getcharacter(charnr);
          If (char) // Der Character existiert auch (muss nicht zwingend!)
            If (!GetObjProperty(char, "LastLog"))
              SetObjProperty(char, "LastLog", 1); // Mal irgendwie initialisieren
            EndIf

            If (GetObjProperty(char, "LastLog")>=active_check) // Ist der Account aktiv?
              account_active:=1; // Jupp
            EndIf

            numberchars+=1;
          EndIf
        EndFor

        If (numberchars) // Genutzter Account?
          If (account_active) // Aktiver Account?
            If (!(Lower(address) in usedactiveaddresses))
              usedactiveaddresses.append(Lower(address));
            EndIf
          Else
            If (!(Lower(address) in usedinactiveaddresses))
              usedinactiveaddresses.append(Lower(address));
            EndIf

            If (!(Lower(address) in unusedinactiveaddresses))
              unusedinactiveaddresses.append(Lower(address)+";");
            EndIf
          EndIf
        Else
          If (!(Lower(address) in unusedaddresses))
            unusedaddresses.append(Lower(address));
          EndIf

          If (!(Lower(address) in unusedinactiveaddresses))
            unusedinactiveaddresses.append(Lower(address)+";");
          EndIf
        EndIf
      Else
        SysLog("FEHLER: '"+accountname+"' enthaelt keine eMail-Adresse!");
      EndIf
    Else
      SysLog("FEHLER: '"+accountname+"' gibt es nicht!");
    EndIf
  EndForEach

  Var datetimestr:=GetDateTimeStr();

  LogToFile("log/emailadresses.log", "UsedActiveAddresses ("+usedactiveaddresses.size()+") - "+datetimestr);
  LogToFile("log/emailadresses.log", "");
  ForEach address in usedactiveaddresses
    LogToFile("log/emailadresses.log", address);
  EndForEach
  LogToFile("log/emailadresses.log", "");

  LogToFile("log/emailadresses.log", "UsedInActiveAddresses ("+usedinactiveaddresses.size()+") - "+datetimestr);
  LogToFile("log/emailadresses.log", "");
  ForEach address in usedinactiveaddresses
    If (!(address in usedactiveaddresses))
      LogToFile("log/emailadresses.log", address);
    EndIf
  EndForEach
  LogToFile("log/emailadresses.log", "");

  LogToFile("log/emailadresses.log", "UnUsedAddresses ("+unusedaddresses.size()+") - "+datetimestr);
  LogToFile("log/emailadresses.log", "");
  ForEach address in unusedaddresses
    If ((!(address in usedactiveaddresses)) And (!(address in usedinactiveaddresses)))
      LogToFile("log/emailadresses.log", address);
    EndIf
  EndForEach
  LogToFile("log/emailadresses.log", "");

  LogToFile("log/emailadresses.log", "UnUsedInActiveAddresses ("+unusedinactiveaddresses.size()+") - "+datetimestr);
  LogToFile("log/emailadresses.log", "");
  ForEach address in unusedinactiveaddresses
    LogToFile("log/emailadresses.log", address);
  EndForEach
  LogToFile("log/emailadresses.log", "");
EndFunction

///////////////////////////////////////////////////////////////////////////////////////////////
// ListOnlineCharacterInfos - Erstellt eine Liste der eingeloggten Character (fuer Webserver)
// DEPRECATED!!
///////////////////////////////////////////////////////////////////////////////////////////////

Function ListOnlineCharacterInfos(list_mode:=LIST_AUX, server_mode:=SERVER_ONLINE)
  Var template;
  If (GetGlobalProperty(BAUSERVER)) // Handelt es sich um den Bauserver?
    template:=developertemplate;    // Jupp, alle Chars auflisten
  Else
    template:=playertemplate;       // Nein, GMs etc. bei Bedarf verstecken
  EndIf

  Var onlinechars:=EnumerateVisibleOnlineCharacters(template, LIST_ME); // Liste mit {char.name, char}
 // Var zahlen:=CountAllAccountsAndCharacters();
  Var zahlen:=CountActiveCharacters();

  Var shutdownmsg:="";
  // ShutDown-Meldung anzeigen, sofern gerade der CountDown laeuft
  If (GetGlobalProperty(PROP_SHUTDOWN_RUNNING))
    Var time:=GetGlobalProperty(PROP_SHUTDOWN_RUNNING)-ReadGameClock();
    Var minutes:=CInt(time/60);
    Var seconds:=time-minutes*60;

    shutdownmsg:="In "+minutes+" Minute";
    If (minutes>1)
      shutdownmsg+="n";
    EndIf
    shutdownmsg+=" und "+seconds+" Sekunde";
    If (seconds>1)
      shutdownmsg+="n";
    EndIf
    shutdownmsg+=" wird der Server heruntergefahren.";
  EndIf

  Var system:=PolCore();
  Var infos:={};
  infos.append(GetPergonTimeOfDay());                        // Serverzeit
  infos.append(onlinechars.size());                          // Personen online
 // infos.append(zahlen[1]);                                   // Anzahl aller Accounts
 // infos.append(zahlen[2]);                                   // Anzahl aller Character
  infos.append(zahlen[4]);                                   // Anzahl aller Accounts
  infos.append(zahlen[8]);                                   // Anzahl aller Character
  infos.append(system.mobilecount);                          // Anzahl der NPCs
  infos.append(system.itemcount);                            // Anzahl der Top-Layer-Items
  infos.append(Format_Time(PolCore().uptime));               // Server online seit
  infos.append(system.verstr);                               // Versions von Pol
  infos.append(GetGlobalProperty(GM_PAGE_LIST).size());      // Anzahl der GM-Pages
  infos.append(shutdownmsg);                                 // ShutDown-Meldung

  If (GetGlobalProperty("#WebSync")) // Wurde ein neuer Account angelegt?
    infos.append(1);
    EraseGlobalProperty("#WebSync");
  Else
    infos.append(0);
  EndIf

  If (onlinechars.size())
    Var gameclock:=ReadGameClock();
    Var playerinfo:={};
    Var cmdinfo:={};

    // Informationen ueber jeden Character zusammentragen
    ForEach char in onlinechars
      // Online-Zeit ermitteln
      Var time:=GetObjProperty(char[2], "lifetime_session");
      If (time)
        time:="seit "+Format_Time(gameclock-time);
      Else
        SetObjProperty(char[2], "lifetime_session", gameclock);
        time:="";
      EndIf

      // CommandoLevel ermitteln
      If (char[2].cmdlevel >= CMDLEVEL_SEER)
        cmdinfo.append({char[1], GetCMDLevelTitle(char[2].cmdlevel), KONTINENT, time});
      Else
        Var playerlevel;
        If (char[2].gender)
          playerlevel:="weibl.";
        Else
          playerlevel:="m&auml;nnl.";
        EndIf

        If (char[2].dead)
          playerinfo.append({char[1], playerlevel, TOTENREICH, time});
        ElseIf ((char[2].multi) And (char[2].multi.isa(POLCLASS_BOAT)))
          playerinfo.append({char[1], playerlevel, BOOTFAEHRE, time});
        Else
          playerinfo.append({char[1], playerlevel, PlaceName(char[2]), time});
        EndIf
      EndIf

      SleepMS(2);
    EndForEach

    // CommandLevel - Character-Informationen eintragen
    ForEach player in SortMultiArrayByIndexNotCase(cmdinfo, 1)
      infos.append(player);
    EndForEach

    // Character-Informationen eintragen
    ForEach player in SortMultiArrayByIndexNotCase(playerinfo, 1)
      infos.append(player);
    EndForEach
  EndIf

  If (list_mode==LIST_FILE) // Daten [asynchron] wegschreiben?
    Var c22:=CChr(0x22);
    Var content:={"<?xml version="+c22+"1.0"+c22+" ?>", "<pergon>", "  <serverstatus>"};

    If (server_mode==SERVER_ONLINE) // UO-Server ist Online
      content.append("    <online>1</online>");
      content.append("    <serverzeit>"+infos[1]+"</serverzeit>");
      content.append("    <character_online>"+infos[2]+"</character_online>");
      content.append("    <accounts>"+infos[3]+"</accounts>");
      content.append("    <character>"+infos[4]+"</character>");
      content.append("    <npcs>"+infos[5]+"</npcs>");
      content.append("    <items>"+infos[6]+"</items>");
      content.append("    <serveronline>"+infos[7]+"</serveronline>");
      content.append("    <polversion>"+infos[8]+"</polversion>");
      content.append("    <gmpages>"+infos[9]+"</gmpages>");
      content.append("    <shutdownmsg>"+infos[10]+"</shutdownmsg>");
      content.append("  </serverstatus>");

      content.append("  <onlinecheck>");

      For index:=12 To infos.size()
        Var player:=infos[index];

        content.append("    <held>");
        content.append("      <name>"+player[1]+"</name>");
        content.append("      <char>"+player[2]+"</char>");
        content.append("      <ort>"+player[3]+"</ort>");
        content.append("      <online_zeit>"+player[4]+"</online_zeit>");
        content.append("    </held>");

        SleepMS(2);
      EndFor

      content.append("  </onlinecheck>");
    Else
      content.append("    <online>0</online>");
      content.append("  </serverstatus>");
    EndIf

    content.append("</pergon>");

    WriteFile("daten_online.xml", content);
  Else
    Return (infos);
  EndIf
EndFunction

////////////////////////////////////////////////////////////////////////////
// ListCharacterInfos - Erstellt eine Liste der Character (fuer Webserver)
////////////////////////////////////////////////////////////////////////////

Function ListCharacterInfos()
  Var active_check:=ReadGameClock()-ACTIVE_TIME;
  Var infos:={};

  ForEach accountname in ListAccounts() // Alle Accountnamen auflisten
    Var account:=FindAccount(accountname); // ...und den Account dazu suchen
    If (account) // Sollte eigentlich immer funzen...
      Var password:=account.usernamepasswordhash;
      If (account.banned)
        password:="banned"; // Account deaktivieren (ungueltiger Hash)
      EndIf

      Var email:=account.getprop(PLAYEREMAIL);
      Var logincharnr:=0;

      For charnr:=1 To 5 // Jeder Account kann bis zu 5 Character haben
        Var char:=account.getcharacter(charnr);
        If (char) // Der Character existiert auch (muss nicht zwingend!)
          logincharnr+=1;

          Var charname:=GetObjProperty(char, "spell_incognito");
          If (charname)
            charname:=charname[2];
          Else
            charname:=GetObjProperty(char, "realrname");
            If (!charname)
              charname:=char.name;
            EndIf
          EndIf

          Var char_active:=0;
          If (GetObjProperty(char, "LastLog")>=active_check) // Ist der Character aktiv?
            char_active:=1;
          EndIf

          Var lifetime:=GetObjProperty(char, "lifetime");
          If (!lifetime)
            lifetime:=-1;
          EndIf

          infos.append({char.serial, accountname, password, email, logincharnr, charname, char.cmdlevel, char_active,
            lifetime, GetObjProperty(char, SKKLASSE), GetObjProperty(char, SKBERUF)});
        EndIf
      EndFor

      If (logincharnr==0) // Account hat keinen Character
        infos.append({0, accountname, password, email, 0, "", 0, 0, -1, "", ""});
      EndIf
    Else
      SysLog("FEHLER: '"+accountname+"' gibt es nicht !");
    EndIf
  EndForEach

  Return (infos);
EndFunction

///////////////////////////////////////////////////////////////////////////
// ListCMDLevelCharacterInfo - Erstellt eine Liste der CMDLevel-Character
///////////////////////////////////////////////////////////////////////////

Function ListCMDLevelCharacterInfo()
  ForEach accountname in ListAccounts() // Alle Accountnamen auflisten
    Var account:=FindAccount(accountname); // ...und den Account dazu suchen
    If (account) // Sollte eigentlich immer funzen...
      For charnr:=1 To 5 // Jeder Account kann bis zu 5 Character haben
        Var char:=account.getcharacter(charnr);
        If (char) // Der Character existiert auch (muss nicht zwingend!)
          If (char.cmdlevel >= CMDLEVEL_SEER) // Besitzt der Account einen CommandLevel-Character?
            SysLog(char.acctname+";"+GetCMDLevelTitle(char.cmdlevel)+" "+char.name+" ["+
              CInt((ReadGameClock()-GetObjProperty(char, "LastLog"))/86400)+" Tage];");
          EndIf
        EndIf
      EndFor
    Else
      SysLog("FEHLER: '"+accountname+"' gibt es nicht !");
    EndIf
  EndForEach
EndFunction

///////////////////////////////////////////////////////////////
// ListGuildInfos - Erstellt eine Liste mit allen Gildeninfos
///////////////////////////////////////////////////////////////

Function ListGuildInfos()
  Var accountnewsgroups:=Dictionary;
  Var guildstones:={};
  Var guildinfos:=Dictionary;
  guildinfos["Gilden"]:=Dictionary;

  // Gildensteine suchen
  ForEach guildstone in ListObjectsInBox(0, 0, -128, 6143, 4095, 127,REALM_BRITANNIA)
    If (guildstone.objtype==0xa390)
      Var guildid:=CInt(GetObjProperty(guildstone, "guild_id"));
      If (guildid)
        guildstones[guildid]:={guildstone.serial, guildstone.x, guildstone.y, guildstone.z};
      EndIf
    EndIf
  EndForEach

  // Daten gemaess Guild.txt sammeln
  ForEach guild in ListGuilds()
    Var newsgroup:=0;
    Case (guild.guildid)
      2:  newsgroup:="lyrell";
      3:  newsgroup:="sio";           // Societas Imperii Obscurae
      4:  newsgroup:="osin";
      5:  newsgroup:="drachentempel";
      6:  newsgroup:="igmetall";
      7:  newsgroup:="idf";
      8:  newsgroup:="nerkzul";
      9:  newsgroup:="flamme";
      10: newsgroup:="diebesgilde";
     // 11: ;                         // Goettergilde
      12: newsgroup:="rosekelch";
      13: newsgroup:="natur";
     // 14: newsgroup:="phoenix";     // aufgeloest
      15: newsgroup:="morituria";
      // 16: newsgroup:="woelfe";     // 2009-01-26 aufgeloest
      17: newsgroup:="phoenix";       // Neugruendung
     // -1: newsgroup:="cam";         // loeschen
    EndCase

    Var guildinfo:=Dictionary;
    guildinfo["Gilde"]:={guild.getprop("name"), guild.getprop("guild_tag"), newsgroup};
    If (guildstones[guild.guildid])
      guildinfo["Stein"]:=guildstones[guild.guildid];
    Else
      guildinfo["Stein"]:=0;
    EndIf

    guildinfo["Meister"]:={};
    ForEach leaderserial in (guild.getprop("leaders"))
      Var leader:=SystemFindObjectBySerial(leaderserial, SYSFIND_SEARCH_OFFLINE_MOBILES);
      If (leader)
        If (leader.getprop("guildtitle"))
          guildinfo["Meister"].append({leader.name, leader.getprop("guildtitle")});
        Else
          guildinfo["Meister"].append({leader.name, 0});
        EndIf
      EndIf
    EndForEach

    Var members:={};
    ForEach member in (guild.members)
      If (member.getprop("guildtitle"))
        members.append({member.serial, member.name, member.getprop("guildtitle"), member.acctname});
      Else
        members.append({member.serial, member.name, 0, member.acctname});
      EndIf

      // Zuordnung der Newsgroups zu den Accounts herstellen
      If (newsgroup)
        If (accountnewsgroups[member.acctname])
          If (!(newsgroup in accountnewsgroups[member.acctname]))
            accountnewsgroups[member.acctname].append(newsgroup);
          EndIf
        Else
          accountnewsgroups[member.acctname]:={newsgroup};
        EndIf
      EndIf
    EndForEach
    guildinfo["Mitglieder"]:=members;

    guildinfos["Gilden"][guild.guildid]:=guildinfo;
  EndForEach
  guildinfos["NewsGroups"]:=accountnewsgroups;

  Return (guildinfos);
EndFunction

/////////////////////////////////////////////////////////////////////////////////////
// GetServerInfos - Liefert bestimmte Server-Informationen (im Sphere-Stil... evil)
/////////////////////////////////////////////////////////////////////////////////////

Function GetServerInfos(list_mode:=LIST_AUX)
  Var infos:="Pol, Name=Eine Welt namens Pergon, Age="+CInt(ReadGameClock()/86400)+", Ver="+PolCore().verstr+", TZ=1, "+

  If (list_mode==LIST_FILE) // Daten [asynchron] wegschreiben?
    WriteFile("serverinfos.txt", {infos});
  Else
    Return (infos);
  EndIf
EndFunction

///////////////////////
//                   //
//   Pruefroutinen   //
//                   //
///////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////
// CheckItemDescContainerUseScripts - Ueberprueft, ob die Container keine UseScripts aufweisen
////////////////////////////////////////////////////////////////////////////////////////////////

Function CheckItemDescContainerUseScripts()
  Var itemdesccfg:=ReadConfigFile("::itemdesc");
  If (itemdesccfg)
    Var checkarea:=FindOrCreateStorage(STORAGE_TESTAREA);
    If (checkarea)
      Var checkcontainer:=InitRootItemInStorage(checkarea, "ContainerUseScripts");
      If (checkcontainer)
        ForEach objtype in GetConfigStringKeys(itemdesccfg)
          Var itemdesc:=itemdesccfg[objtype];
          If (itemdesc) // Sollte eigentlich immer funktionieren... (egal, die Fkt. wird nur beim Serverstart genutzt)
            Var item:=CreateItemInContainerPergon(checkcontainer, CInt(objtype));
            If (item)
              If (item.isa(POLCLASS_CONTAINER)) // Nur Container duerfen kein UseScript besitzen
                If (itemdesc.script) // ...und, hat'er trotzdem eins?
                  SysLog("WARNUNG: Container "+Lower(Hex(objtype))+" darf kein UseScript besitzen (ItemDesc) !");
                EndIf
              EndIf
            EndIf
          Else
            SysLog("FEHLER: Konnte die ItemDesc fuer "+objtype+" nicht finden!");
          EndIf
        EndForEach
      EndIf
    EndIf
  Else
    SysLog("FEHLER: Konnte die ItemDesc.cfg nicht oeffnen!");
  EndIf
EndFunction

//////////////////////////////////////////////////////////////////////////////////////////
// LogContainerMaxProperties - Gibt die Max-Properties der Container aus (zum Balancing)
//////////////////////////////////////////////////////////////////////////////////////////

Function LogContainerMaxProperties()
  Var itemdesccfg:=ReadConfigFile("::itemdesc");
  If (itemdesccfg)
    SysLog("<Container>");
    ForEach objtype in GetConfigStringKeys(itemdesccfg)
      Var itemdesc:=itemdesccfg[objtype];
      If (itemdesc) // Sollte eigentlich immer funktionieren... (egal, die Fkt. wird nur beim Serverstart genutzt)
        Var item := CreateItemAtLocationPergon(5376, 1081, 0, CInt(objtype), 1);
        If (item)
          If (item.isa(POLCLASS_CONTAINER)) // Nur Container duerfen kein UseScript besitzen
            SysLog("  "+Lower(Hex(item.objtype))+" '"+item.desc+"' MaxWeight="+itemdesc.maxweight+" MaxItems="+itemdesc.maxitems);
          EndIf
        EndIf
        DestroyItem(item);
      Else
        SysLog("FEHLER: Konnte die ItemDesc fuer "+objtype+" nicht finden!");
      EndIf
    EndForEach
    SysLog("</Container>");
  Else
    SysLog("FEHLER: Konnte die ItemDesc.cfg nicht oeffnen!");
  EndIf
EndFunction

//////////////////////////////////////////////////////////////////////////////////////////
// CheckItemsForIllegalObjType - Ueberprueft, ob die Items einen korrekten ObjType haben
//////////////////////////////////////////////////////////////////////////////////////////

Function CheckItemsForIllegalObjType()
  For objtype:=0 To 0x3fff
    Var itemdesc:=GetItemDescriptor(objtype);
    If (itemdesc)
      If (itemdesc.graphic<>objtype)
        SysLog("FEHLER: Item '"+itemdesc.desc+"' ["+Lower(Hex(objtype))+"] hat die Grafik "+Lower(Hex(itemdesc.graphic)));
      EndIf
    EndIf
  EndFor
EndFunction

//////////////////////////////////////////////////////////////////////////////////////////////
// CheckAccountProperties - Ueberprueft bestimmte Werte in den Accounts (z.B. eMail-Adresse)
//////////////////////////////////////////////////////////////////////////////////////////////

Function CheckAccountProperties()
  SysLog("Accounts werden ueberprueft ...");

  Var problem := 0;

  ForEach accountname in ListAccounts();
    Var account := FindAccount(accountname);
    If (!account)
      SysLog("  FEHLER: '"+accountname+"' gibt es nicht!");
      problem := 1;
      continue;
    EndIf

    Var email := account.getprop(PLAYEREMAIL);
    If (!email)
      SysLog("  WARNUNG: '"+accountname+"' besitzt keine eMail-Adresse");
      problem := 1;
      continue;
    EndIf

    If (
      !position or
      // am Anfang
      position == 1 or
      // am Ende
      position == Len(email)
    )
      SysLog(
        "  WARNUNG: '"+accountname+
      );
      problem := 1;
      continue;
    // Else
    //   email[1, position] := "";
    EndIf

    position := Find(email, ".", 1);
    If (
      // gar kein '.'
      !position or
      // am Anfang
      position == 1
    )
      SysLog(
        "  WARNUNG: '"+accountname+
        "' besitzt keine gueltige eMail-Adresse (.)"
      );
      problem := 1;
      continue;
    EndIf

    // keine Leerzeichen, egal wo
    If (email[" "])
      SysLog(
        "  WARNUNG: '"+accountname+
        "' besitzt keine gueltige eMail-Adresse ( )"
      );
      problem := 1;
    EndIf
  EndForEach

  If (!problem)
    SysLog("... kein Problem gefunden");
  EndIf
EndFunction

////////////////////////////////////////////////////////////////////////
// CheckNPCDescProperties - Ueberprueft bestimmte Werte in der NPCDesc
////////////////////////////////////////////////////////////////////////

Function CheckNPCDescProperties()
  Var npcdesc:=ReadConfigFile("npcdesc");
  If (npcdesc)
    ForEach templatename in GetConfigStringKeys(npcdesc)
      Var template:=npcdesc[templatename];

      If (template.scriptdomest in {":tierzucht:m_domest", ":tierzucht:w_domest"})
        If (!(((template.scriptdomest==":tierzucht:m_domest") And (!template.gender)) Or
           ((template.scriptdomest==":tierzucht:w_domest") And (template.gender))))
          SysLog("FEHLER: NPC-Template '"+templatename+"' hat eine falsche Zuordnung von Gender ("+
            template.gender+") und ScriptDomest ("+template.scriptdomest+") !");
        EndIf
      EndIf
    EndForEach
  EndIf
EndFunction

/////////////////////////////////////////////////////////////////////////
// ListSpawnNet - Erstellt eine Liste aller Spawnrunen (fuer Webserver)
/////////////////////////////////////////////////////////////////////////

Function ListSpawnNet()
  Var i,ii,mapx,mapy, group:="", userdata, spawndata;

  Var c22:=CChr(0x22);
  Var content:={"<?xml version="+c22+"1.0"+c22+" ?>","<Runen>"};

  For (i:=0;i<=134;i+=1)  // Alle Bilder durchgehen
    Case (len(CStr(i))) // Führende Nullen dranpappen
      1:       ii:="00"+i;
      2:       ii:="0"+i;
      default: ii:=CStr(i);
    EndCase
    content.append("  <map"+ii+">");

    mapx:=i % 15;// Umrechnung in Weltkoords
    mapy:=CInt(i / 15);
    mapx*=500; // Größe des Bildes
    mapy*=500;

    ForEach item in (ListObjectsInBox(mapx,mapy,-128,mapx+500,mapy+500,127,REALM_BRITANNIA))  // Runen im Bereich finden
      If (item.isa(POLCLASS_ITEM)) // Ist es ein Item?
        If (item.objtype==UOBJ_SPAWNRUNE) // Ist es eine SpawnRune?
          userdata:=item.getprop("userdata");
          spawndata:=item.getprop("spawndata");
          content.append("    <rune>");
          content.append("      <x>"+item.x+"</x>");
          content.append("      <y>"+item.y+"</y>");
          content.append("      <template>"+SaveXML(userdata.template)+"</template>");
          content.append("      <amount>"+userdata.amount+"</amount>");
          content.append("      <range>"+userdata.range+"</range>");
          content.append("      <mintime>"+userdata.mintime+"</mintime>");
          content.append("      <maxtime>"+userdata.maxtime+"</maxtime>");
          content.append("      <flags>"+userdata.flags+"</flags>");
          content.append("      <spawntype>"+userdata.spawntype+"</spawntype>");
          content.append("      <stackamount>"+userdata.stackamount+"</stackamount>");
          if (!(lower(userdata.containertype) in {"<keine>","<keiner>"}))
            content.append("      <containertype>"+SaveXML(userdata.containertype)+"</containertype>");
          endif
          if (!(lower(userdata.note) in {"<none>","<keiner>"}))
            content.append("      <note>"+SaveXML(userdata.note)+"</note>");
          endif
          content.append("      <questnr>"+userdata.questnr+"</questnr>");
          content.append("      <grouping>"+userdata.group+"</grouping>");
          If (spawndata.templatearray<>{})  // Gruppendef?
            group:="      <group>";
            ForEach spawn in (spawndata.templatearray)
              If (!group[spawn+", "])
                group+=spawn+", ";
              EndIf
            EndForEach
            content.append(group[1,Len(group)-2]+"</group>");
          EndIf
          content.append("    </rune>");
        EndIf
        SleepMS(5);
      EndIf
    EndForEach

    content.append("  </map"+ii+">");
  EndFor

  // SpawnRegions
  Var dataregion:=OpenDataFile(":spawnnet_new:RegionsUser");
  Var datagroup:=OpenDataFile(":spawnnet_new:RegionsGroup");
  Var groupdata,region;
  ForEach regionid in (dataregion.Keys())  // Alle Regionen durchgehen
    SleepMS(2);
    If (regionid=="Realms")  // dich nicht
      continue;
    EndIf
    region:=dataregion.FindElement(regionid);
    If (region.getprop("realm")<>REALM_BRITANNIA)
      continue;
    EndIf
    //spawndata:=region.getprop("spawndata");
    userdata:=region.getprop("userdata");
    content.append("  <region>");
    content.append("    <name>"+regionid+"</name>");
    content.append("    <x1>"+userdata.koords[1]+"</x1>");
    content.append("    <x2>"+userdata.koords[3]+"</x2>");
    content.append("    <y1>"+userdata.koords[2]+"</y1>");
    content.append("    <y2>"+userdata.koords[4]+"</y2>");
    content.append("    <mintime>"+userdata.mintime+"</mintime>");
    content.append("    <maxtime>"+userdata.maxtime+"</maxtime>");
    ForEach group in (userdata.groups)
      content.append("    <group>");
      content.append("      <name>"+_group_iter+"</name>");
      content.append("      <enabled>"+group.enabled+"</enabled>");
      content.append("      <max_amt>"+group.max_amt+"</max_amt>");
      content.append("      <spawn_factor>"+group.spawn_factor+"</spawn_factor>");
      groupdata:=datagroup.FindElement(_group_iter);
      content.append("      <containerobjtype>"+groupdata.getprop("Containerobjtype")+"</containerobjtype>");
      content.append("      <flags>"+groupdata.getprop("Flags")+"</flags>");
      content.append("      <groupingamt>"+groupdata.getprop("GroupingAmt")+"</groupingamt>");
      content.append("      <spawntype>"+groupdata.getprop("Spawntype")+"</spawntype>");
      content.append("      <stackamount>"+groupdata.getprop("StackAmount")+"</stackamount>");
      ForEach template in (groupdata.getprop("EntryList"))
        content.append("      <template procent="+c22+template+c22+">"+_template_iter+"</template>");
      EndForEach
      content.append("    </group>");
    EndForEach
    content.append("  </region>");
  EndForEach
  content.append("</Runen>");

  WriteFile("daten_spawninfos.xml", content);
EndFunction

Function SaveXML(text)
  text:=CStr(text);
  If (Len(text)>0)
    text:=StrReplace(text, CChr(0x22), "&quot;"); // "
    text:=StrReplace(text, "<", "&lt;");
    text:=StrReplace(text, ">", "&gt;");
  EndIf

  return text;
EndFunction