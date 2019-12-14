//////////////////////////////////////////////////////
// Bank Include - Funktionen rund um die Bank

//////////////////////////////////////////////////////
// OpenWorldBank     - Oeffnet die WeltBank
// FindBankBox       - Offnet die Bankbox des Spielers
// WipeBankBox       - Loescht die Bankbox des Spielers
//
// HideBankBoxes     - Bankboxen der Player verstecken
// UnHideBankBoxes   - Bankboxen der Player wiederholen
// GetHiddenBankBox  - Ermittelt die versteckte Bankbox des Spielers
// GetAccountBalance - Ermittelt aktuellen Kontostand
// StopTransaction   - hebt Reservierungen des Kontos auf
// TryToGetPayment   - versucht Geld vom Konto abzuheben
//                     (einzahlen mit negativem Betrag geht auch)

use os;
use storage;
use uo;
include "include/logutil";
include "include/objtype";
include "include/storage";
include "include/stringcalc";
include "include/utility";

// Globale Property, ob Bankboxen versteckt sind, oder nicht
Const GPROP_HIDDEN_BOXES :=      "HiddenBoxes";
// CProp an der Bankbox, in der Goldkontostand steht
Const CPROP_GOLD_ACCT :=         "GoldAccount";
// CProp am Spieler, waehrend eine Transaktion laeuft
Const CPROP_GOLD_TRANS :=        "#GoldTransaction";
// maximaler Abstand fuer diverse Bankaktionen
Const MAXDIST_BANKCUST :=        4;
// Ein-/Auszahlzeitverzoegerung zwischen Stacks
Const STACK_CONSUME_DELAY :=     25;
// Fehlermeldungskonstanten
Const ERR_TRANSACTION_RUNNING := "Another transaction is running";
Const ERR_NO_ENOUGH_MONEY :=     "Could not debit enough money";

Enum TransactionGump
  TXT_BETRAG := 10,
  BTN_OK
EndEnum

// OpenWorldBank - Oeffnet die WeltBank
Function OpenWorldBank()
  Var bank:=FindStorageArea(STORAGE_BANK);
  If (!bank)
    bank:=CreateStorageArea(STORAGE_BANK);
  EndIf

  // Dieser Fall sollte niemals auftreten... (theoretisch :o)
  If (!bank)
    SysLog("FEHLER: Konnte die WeltBank weder oeffnen noch anlegen!");
  EndIf

  Return (bank);
EndFunction

// FindBankBox - Offnet die Bankbox des Spielers
Function FindBankBox(who)
  If (who.isa(POLCLASS_MOBILE) And (!who.isa(POLCLASS_NPC))) // Nur Spieler
    Var worldbank := OpenWorldBank();
    If (worldbank)
      Var bank_obj_name := ST_PREF_BANK+who.serial;

      Var bankbox:=FindRootItemInStorageArea(worldbank, bank_obj_name);
      If (!bankbox)
        bankbox:=CreateRootItemInStorageArea(worldbank, bank_obj_name, UOBJ_BANKBOX);
      EndIf

      // Dieser Fall sollte niemals auftreten... (theoretisch :o)
      If (!bankbox)
        SysLog("FEHLER: Konnte die Bankbox weder oeffnen noch anlegen ("+who.serial+") !");
      EndIf

      // Wurden die Bankboxen versteckt?
      Var gprop:=GetGlobalProperty(GPROP_HIDDEN_BOXES);
      If (gprop)
        bankbox:=GetHiddenBankBox(bankbox, gprop);
      EndIf

      Return (bankbox);
    EndIf
  EndIf
EndFunction

// WipeBankBox - Loescht die Bankbox des Spielers
Function WipeBankBox(who, ignore_player_check:=0)
  If ((who.isa(POLCLASS_MOBILE) And (!who.isa(POLCLASS_NPC))) Or ignore_player_check) // Nur Spieler
    Var worldbank:=OpenWorldBank();
    If (worldbank)
      Return (DestroyRootItemInStorageArea(worldbank, ST_PREF_BANK+who.serial));
    EndIf
  EndIf
EndFunction

// HideBankBoxes - Bankboxen der Player verstecken
Function HideBankBoxes()
  SetGlobalProperty(GPROP_HIDDEN_BOXES, ReadGameClock());
EndFunction

// UnHideBankBoxes - Bankboxen der Player wiederholen
Function UnHideBankBoxes()
  EraseGlobalProperty(GPROP_HIDDEN_BOXES);
EndFunction

// GetHiddenBankBox - Ermittelt die versteckte Bankbox des Spielers
Function GetHiddenBankBox(bankbox, gprop)
  Var hiddenbox;

  // Nach der versteckten BankBox suchen und aeltere ggf. "entfernen"
  ForEach item in EnumerateItemsInContainer(bankbox)
    If (item.objtype==UOBJ_BANKBOX)
      Var cprop:=GetObjProperty(item, GPROP_HIDDEN_BOXES);
      If (cprop) // Es ist eine versteckte Bankbox
        If (cprop==gprop) // Ist es auch die zum akt. Quest gehoerende?
          If (!hiddenbox)
            hiddenbox:=item;
          Else
            SysLog("FEHLER: Es gibt mehrere versteckte Bankboxen gleichen Typs in der Bankbox "+Lower(Hex(bankbox.serial))+" !");
          EndIf
        Else // Nein, also Markierung entfernen
          EraseObjProperty(item, GPROP_HIDDEN_BOXES);
        EndIf
      EndIf
    EndIf
  EndForEach

  // Keine versteckte Bankbox gefunden, also neu anlegen
  If (!hiddenbox)
    hiddenbox:=CreateItemInContainerPergon(bankbox, UOBJ_BANKBOX);
    If (hiddenbox)
      SetObjProperty(hiddenbox, GPROP_HIDDEN_BOXES, gprop);
    Else
      SysLog("FEHLER: Konnte in der Bankbox "+Lower(Hex(bankbox.serial))+" keine versteckte Bankbox anlegen!");
    EndIf
  EndIf

  Return (hiddenbox);
EndFunction

// ermittelt aktuellen Kontostand
Function GetAccountBalance(bankbox)
    If (bankbox.isa(POLCLASS_MOBILE))
        // alternativer Aufruf mit Character statt Box
        var who := bankbox;
        bankbox := FindBankBox(who);
        If (!bankbox)
            syslog(
                "WARNUNG: Der Spieler "+CharInfoStr(who)+" hat keine Bankbox"
            );
            return error{
                errortext := "Player has no bankbox: "+bankbox.errortext
            };
        EndIf
    EndIf

    var acct := GetObjProperty(bankbox, CPROP_GOLD_ACCT);
    If (!acct)
        acct := "0";
    EndIf

    return acct;
EndFunction

// hebt Reservierungen des Kontos auf
Function StopTransaction(customer, tgt, unreserve := 0)
    If (tgt and tgt.isa(POLCLASS_CONTAINER))
        ReleaseItem(tgt);
    EndIf
    EraseObjProperty(customer, CPROP_GOLD_TRANS);

    If (TypeOfInt(unreserve) != OT_ARRAY)
        // nur Goldanzeige aktualisieren
        SendStatus(customer);
        return;
    EndIf

    // sonst ggf. weitere Items freigeben
    ForEach item in (unreserve)
        If (item)
            ReleaseItem(item);
        EndIf
        SleepMS(2);
    EndForEach

    // und danach Goldanzeige aktualisieren
    SendStatus(customer);
EndFunction

// versucht <cost> Geld vom Konto von who abzuheben
// (einzahlen mit negativem Betrag geht auch)
Function TryToGetPayment(who, cost)
    If (GetObjProperty(who, CPROP_GOLD_TRANS))
        // Abbruch, falls noch Transaktion laeuft
        // (sollte nie auftreten)
        return error{errortext := ERR_TRANSACTION_RUNNING};
    EndIf

    // Kontostand holen
    var bankbox := FindBankBox(who);
    If (!bankbox)
        syslog(
            "WARNUNG: Der Spieler "+CharInfoStr(who)+" hat keine Bankbox"
        );
        return error{
            errortext := "Player has no bankbox: "+bankbox.errortext
        };
    EndIf

    // Abbuchversuch
    var crit := Is_Critical();
    Set_Critical(1);
    var balance := GetObjProperty(bankbox, CPROP_GOLD_ACCT);
    var result := StrSubtraktion(balance, cost);
    If (result)
        // bei Erfolg Kontostand aktualisieren
        SetObjProperty(bankbox, CPROP_GOLD_ACCT, result);
        Set_Critical(crit);
        return 1;
    EndIf
    Set_Critical(crit);

    // Misserfolg (zu wenig Gold etc.)
    return error{errortext := ERR_NO_ENOUGH_MONEY};
EndFunction

Function SendTransactionGump(who, text1, acct, text2)
  
  var layout := {
    "Page 0",
    "NoDispose",
    
    "ResizePic 100 100 5170 400 225",
    "HtmlGump 150 150 300 20 0 0 0",
    "GumpPic 162 175 1143",
    "TextEntryLimited 172 178 300 20 0 "+TXT_BETRAG+" 1 30",
    "HtmlGump 150 210 300 20 2 0 0",
    "Button 175 250 247 248 1 0 "+BTN_OK,
    "Button 360 250 242 241 1 0 0"
  };
  
  var data := {
    "<CENTER>"+text1+"</CENTER>",
    "0",
    "<CENTER>"+text2+"</CENTER>"
  };
  
  var betrag, success := 0;
  Repeat
    var ret := SendDialogGump(who, layout, data);
    betrag := StrCutLeadingZeros(ret[TXT_BETRAG][5, len(ret[TXT_BETRAG])]);
    If (ret[BTN_OK] && betrag != "0" && betrag != "")
      If (StrIsNumeric(betrag))
        If (StrSubtraktion(acct, betrag))
          success := 1;
        Else
          return error{errortext := "So viel Gold ist nicht vorhanden!"};
        EndIf
      Else
        return error{errortext := "Ihr müsst eine Zahl eingeben!"};
      EndIf
    Else
      SendSysMessagePergon(who, "Abbruch", "Abort");
      return 0;
    EndIf
  Until (success);
  
  return betrag;
EndFunction
