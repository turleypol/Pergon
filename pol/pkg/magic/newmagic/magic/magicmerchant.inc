///////////////////////////////////////////////////////////////////////////
// MagicMerchant Funktionen v1.0 (2001/12/25)
//
// Author: Bihinos

include "include/bank";
include "spellbook";

//////////////////////
// Gump-Definitionen
//////////////////////

Var layouttemplate:={
  "page 0",                       // Seite 0 (Basisseite)
  "nodispose",
  "resizepic 10 35 2620 435 230",

  "text 25 50 66 0",              // Frage
  "button 30 90 2640 2641 1 0 1",  "text 65 92 40 1",  "text 350 92 5 7",
  "button 30 115 2640 2641 1 0 2", "text 65 117 40 2", "text 350 117 5 8",
  "button 30 140 2640 2641 1 0 3", "text 65 142 40 3", "text 350 142 5 9",
  "button 30 165 2640 2641 1 0 4", "text 65 167 40 4", "text 350 167 5 10",
  "button 30 190 2640 2641 1 0 5", "text 65 192 40 5", "text 350 192 5 11",
  "button 30 215 2640 2641 1 0 6", "text 65 217 40 6", "text 350 217 5 12"
};

Var datatemplate:={"", "leeres Buch", "Buch mit 8 zufaelligen Spruechen", "Buch mit 16 zufaelligen Spruechen",
  "Buch mit allen allgemeinen Spruechen", "Buch mit allen speziellen Spruechen", "Buch mit allen Spruechen"};

Var layoutTemplateArtefacts:={
  "page 0",                       // Seite 0 (Basisseite)
  "nodispose",
  "resizepic 10 35 2620 435 230",

  "text 25 50 66 0",              // Frage
  "button 30 90 2640 2641 1 0 1",  "text 65 92 40 1",  "text 350 92 5 2"
};

Var dataTemplateArtefacts:={"", "leeres Buch"};


///////////////////////////////////////////////////////////////
// Speech_Zauberbuch - Jemand hat zum NPC "zauberbuch" gesagt
///////////////////////////////////////////////////////////////

Function Speech_Zauberbuch(me, medata, ev, skillid)
  Var layout:=layouttemplate; // Gump-Daten initialisieren
  Var data:=datatemplate;

  Var objtype;
  Var basescroll;
  Var booktitle;

  Case (skillid)
    SKILLID_MAGIE:   objtype:=UOBJ_SPELLBOOK_MAGE;
                     basescroll:=0x66ff;
                     booktitle:="Grimoire";
                     data[1]:="Welches Grimoire wollt ihr kaufen?";
    SKILLID_SEGNEN:  objtype:=UOBJ_SPELLBOOK_CLERIC;
                     basescroll:=0x675f;
                     booktitle:="Liber Castus";
                     data[1]:="Welchen Liber Castus wollt ihr kaufen?";
    SKILLID_FLUCHEN: objtype:=UOBJ_SPELLBOOK_NECRO;
                     basescroll:=0x67bf;
                     booktitle:="paq mu'qaDmey";
                     data[1]:="Welchen paq mu'qaDmey wollt ihr kaufen?";
  EndCase

  If (objtype)
    If (FindSpellbook(ev.source, objtype)) // check if player already has a spellbook
      NPCSayTextTraditional(me, "Ihr besitzt bereits ein "+booktitle+". So geht denn Eurer Wege.", "");

      If ((medata.merchanttype=="ClericHigh") And (skillid==SKILLID_FLUCHEN))
        NPCSayTextTraditional(me, "Solltet Ihr jedoch zu einem Priester werden wollen, so muesst Ihr uns das nur sagen.", "");
      ElseIf ((medata.merchanttype=="NecroHigh") And (skillid==SKILLID_SEGNEN))
        NPCSayTextTraditional(me, "Wenn Ihr zu uns gehoeren wollt, so sagt es einfach. Wenn nicht, macht Euch hinfort.", "");
      EndIf
    Else
      Var bookprices:={10000, 20000, 30000, 40000, 50000, 60000};

      // Guete des Spielers einschaetzen
      Case (GetObjProperty(ev.source, SKILLTYPKENNUNG+skillid)) // get type of player's casting skill
        SKILLTYP_BERUF:   NPCSayTextTraditional(me, "Ahhhh, ein wahrer Anhaenger unserer Profession - Lasst Euch willkommen heissen.", "");
                          bookprices[3]:=0;
        SKILLTYP_HAUPT:   NPCSayTextTraditional(me, "Ich sehe, Ihr seid in unseren Kuensten bewandert - Seid willkommen.", "");
                          bookprices[2]:=0;
        SKILLTYP_NEBEN:   NPCSayTextTraditional(me, "Ein Anhaenger unserer Kuenste - Sehen wir, was wir fuer Euch tuen koennen.", "");
                          bookprices[1]:=0;
        SKILLTYP_WEITERE: NPCSayTextTraditional(me, "So so, Ihr habt Euch also auch schon mal mit unseren Kuensten beschaeftigt - Was kann ich fuer Euch tun?", "");

        default:          NPCSayTextTraditional(me, "Ihr seid ja nicht einmal annaehernd unserer arkanen Kuenste faehig. Ich kann Euch nicht helfen.", "");
                          Return (1); // ignore SpeechConfig
      EndCase

      // Preise der Buecher den GumpDaten hinzufuegen
      ForEach bookprice in bookprices
        data.append(bookprice+" gp");
      EndForEach

      // Buch zum Verkauf anbieten
      Var nummer:=SendDialogGump(ev.source, layout, data)[0];
      If (nummer)
        If (!GetPayment(me, ev.source, bookprices[nummer]))
          NPCSayTextTraditional(me, "Hier ist Euer "+booktitle+".", "");

          Var book:=CreateSpellBookInBackpack(ev.source, objtype);
          If (book)
            Case (nummer)
              2: Var spells:={0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
                 Var full:=0;
                 Var spell;

                 While (full<8) // fill 2nd spellbook with random spells (25% of circles 1-4 = 1/8th of all spells)
                   spell:=RandomInt(32)+1;
                   If (!spells[spell]) // check if spell is already created
                     spells[spell]:=1;

                     If (spell<25) // check if spell is common spell
                       CreateSpellInBook(book, basescroll+spell); // create scroll in book
                     Else
                       CreateSpellInBook(book, basescroll+spell+8);
                     EndIf

                     full:=full+1;
                   EndIf
                 EndWhile

              3: Var spells:={0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
                 Var full:=0;
                 Var spell;

                 While (full<16) // fill 3nd spellbook with random spells (50% of circles 1-4 = quarter of all spells)
                   spell:=RandomInt(32)+1;
                   If (!spells[spell]) // check if spell is already created
                     spells[spell]:=1;

                     If (spell<25) // check if spell is common spell
                       CreateSpellInBook(book, basescroll+spell); // create scroll in book
                     Else
                       CreateSpellInBook(book, basescroll+spell+8);
                     EndIf

                     full+=1;
                   EndIf
                 EndWhile

              4: For spellid:=1 To 32 // fill remaining spellbooks
                   If (spellid<25) // 4th with all common spells in circles 1-4
                     CreateSpellInBook(book, basescroll+spellid);
                   EndIf
                 EndFor

              5: For spellid:=1 To 32 // fill remaining spellbooks
                   If (spellid>=25) // 5th with all specific spells in circles 1-4
                     CreateSpellInBook(book, basescroll+spellid+8);
                   EndIf
                 EndFor

              6: For spellid:=1 To 32 // fill remaining spellbooks
                   If (spellid<25) // 6th with all spells in circles 1-4 = half of all spells
                     CreateSpellInBook(book, basescroll+spellid);
                   Else
                     CreateSpellInBook(book, basescroll+spellid+8);
                   EndIf
                 EndFor
            EndCase
          Else
            SendSysMessagePergon(ev.source, "Das Buch konnte nicht im Backpack erzeugt werden!");
          EndIf
        EndIf
      Else
        SendSysMessagePergon(ev.source, "Abbruch", "Abort");
      EndIf
    EndIf

    Return (1);
  EndIf
EndFunction


///////////////////////////////////////////////////////////////
// Speech_Artefaktbuch - Jemand hat zum NPC "artefaktbuch" gesagt
///////////////////////////////////////////////////////////////

Function Speech_Artefaktbuch(me, ev, skillid)
  
  If(!GetGlobalProperty("ArtefactsActive") == 1)          // nur zugänglich wenn tatsächlich freigegeben
    NPCSayTextTraditional(me, "Tut mir Leid, da kann ich nichts fuer euch tun.", "");
    Return 0;
  EndIf
  
  Var layout:=layoutTemplateArtefacts; // Gump-Daten initialisieren
  Var data:=dataTemplateArtefacts;

  Var objtype;
  
  Var booktitle;

  Case (skillid)
    SKILLID_MAGISCHEGEGENST:   objtype:=UOBJ_ARTEFACTBOOK_MAGE;                     
                               booktitle:="Gremoire";
                               data[1]:="Welches Gremoire wollt ihr kaufen?";
                               
    SKILLID_GESEGNETEGEGENST:  objtype:=UOBJ_ARTEFACTBOOK_CLERIC;                    
                               booktitle:="Liber Apparatus";
                               data[1]:="Welchen Liber Apparatus wollt ihr kaufen?";
                               
    SKILLID_VERFLUCHTEGEGENST: objtype:=UOBJ_ARTEFACTBOOK_NECRO;                  
                                booktitle:="puq ma'qaBmay";
                                data[1]:="Welchen puq ma'qaBmay wollt ihr kaufen?";
  EndCase


  If (objtype)
    If (FindSpellbook(ev.source, objtype)) // check if player already has a spellbook / funzt auch mit artefaktbüchern
      NPCSayTextTraditional(me, "Ihr besitzt bereits ein "+booktitle+". So geht denn Eurer Wege.", "");
      
    Else
      Var bookprices:={60000};

      // Guete des Spielers einschaetzen
      Case (GetObjProperty(ev.source, SKILLTYPKENNUNG+skillid)) // get type of player's casting skill
        SKILLTYP_BERUF:   NPCSayTextTraditional(me, "Ahhhh, ein wahrer Anhaenger unserer Profession - Lasst Euch willkommen heissen.", "");
                          //bookprices[3]:=0;
        SKILLTYP_HAUPT:   NPCSayTextTraditional(me, "Ich sehe, Ihr seid in unseren Kuensten bewandert - Seid willkommen.", "");
                          //bookprices[2]:=0;
        SKILLTYP_NEBEN:   NPCSayTextTraditional(me, "Ein Anhaenger unserer Kuenste - Sehen wir, was wir fuer Euch tuen koennen.", "");
                          //bookprices[1]:=0;
        SKILLTYP_WEITERE: NPCSayTextTraditional(me, "So so, Ihr habt Euch also auch schon mal mit unseren Kuensten beschaeftigt - Was kann ich fuer Euch tun?", "");

        default:          NPCSayTextTraditional(me, "Ihr seid ja nicht einmal annaehernd unserer arkanen Kuenste faehig. Ich kann Euch nicht helfen.", "");
                          Return (1); // ignore SpeechConfig
      EndCase

      // Preise der Buecher den GumpDaten hinzufuegen
      ForEach bookprice in bookprices        
        data.append(bookprice+" gp");
      EndForEach

      // Buch zum Verkauf anbieten
      Var number:=SendDialogGump(ev.source, layout, data)[0];
      If (number)
        If (!GetPayment(me, ev.source, bookprices[number]))
          NPCSayTextTraditional(me, "Hier ist Euer "+booktitle+".", "");

          Var book:=CreateNewBookInBackpack(ev.source, objtype);
          If (book)
                        
          Else
            SendSysMessagePergon(ev.source, "Das Buch konnte nicht im Backpack erzeugt werden!");
          EndIf
        EndIf
      Else
        SendSysMessagePergon(ev.source, "Abbruch", "Abort");
      EndIf
    EndIf

    Return (1);
  EndIf
EndFunction


/////////////////////////////////////////////////////////////
// FindSpellbook - Spellbook in Backpack und Bankbox suchen
/////////////////////////////////////////////////////////////

Function FindSpellbook(player, objtype)
  Var spellbook:=FindItemInContainerTree(player.backpack, objtype);
  If (spellbook)
    Return (1);
  Else
    spellbook:=FindItemInContainerTree(FindBankBox(player), objtype);
    If (spellbook)
      Return (1);
    EndIf
  EndIf
EndFunction

/////////////////////////////////////////////////////
// Speech_Karma - Jemand hat zum NPC "karma" gesagt
/////////////////////////////////////////////////////

Function Speech_Karma(me, ev, skillid)
  Var now:=ReadGameClock();
  Var lastkarma;

  Case (skillid)
    SKILLID_MAGIE: NPCSayTextTraditional(me, "Karma??! Was soll das sein? Unser ist die Lehre der Elemente. Wir brauchen keine Goetter.", "");

    SKILLID_SEGNEN:
      lastkarma:=CInt(GetObjProperty(ev.source, "GotLastKarma")); // check, when player got his last karma
      If (lastkarma+86400<now)
        SetObjProperty(ev.source, "karma", 1); // give karma to player
        SetObjProperty(ev.source, "GotLastKarma", now); // set timestamp

        NPCSayTextTraditional(me, "Seid gesegnet... Geht von dannen und nutzt die Euch gegebene Gabe mit Weisheit.", "");
      Else
        NPCSayTextTraditional(me, "Ihr seid zu gierig. Geht in Euch und geduldet Euch noch eine Weile.", "");
      EndIf

    SKILLID_FLUCHEN:
      lastkarma:=CInt(GetObjProperty(ev.source, "GotLastKarma")); // check, when player got his last karma
      If (lastkarma+86400<now)
        SetObjProperty(ev.source, "karma", -1); // give karma to player
        SetObjProperty(ev.source, "GotLastKarma", now); // set timestamp

        NPCSayTextTraditional(me, "Hahaha - Kein Problem... Jetzt geht und nutzt, was Euch gegeben.", "");
      Else
        NPCSayTextTraditional(me, "Was glaubt Ihr, wer Ihr seid??! Mir aus den Augen - Auf der Stelle!", "");
      EndIf

    default: Return; // Gemaess SpeechConfig antworten
  EndCase

  Return (1);
EndFunction

///////////////////////////////////////////////////////////////////
// Speech_Konvertieren - Jemand hat zum NPC "konvertieren" gesagt
///////////////////////////////////////////////////////////////////

Function Speech_Konvertieren(me, ev, skillid)
  Case (skillid)
    SKILLID_MAGIE: NPCSayTextTraditional(me, "Ihr wollt konvertieren?! Dann tut das bei jemand anders. Magie beherrscht man oder man tut es nicht.", "");

    SKILLID_SEGNEN:
      Var convertquest:=GetObjProperty(ev.source, "ClericConvertQuest"); // get Convert Quest status
      If (convertquest) // check if player has started quest
        If (convertquest=="Open") // check if quest is fullfilled
          NPCSayTextTraditional(me, "Ihr koennt noch nicht zu uns gehoeren, denn Ihr habt die Euch aufgetragene Aufgabe nicht erfuellt.", "");
        Else
          // do something to cleanup remnants of quest
          // check timstamps, items etc.
          // then convert

          NPCSayTextTraditional(me, "Ich sehe, Ihr habt die Euch gestellte Aufgabe erfuellt. Nundenn - Willkommen in den Reihen der Gesegneten.", "");

          // get player's intelligence and casting/item skills
          Var castingskill:=GetSkillPergon(ev.source, SKILLID_FLUCHEN);
          Var itemskill:=GetSkillPergon(ev.source, SKILLID_VERFLUCHTEGEGENST);
          Var intelligence:=GetIntPergon(ev.source);

          // erase former casting skill, set new casting skill
          SetSkillPergon(ev.source, SKILLID_FLUCHEN, 0);
          SetSkillPergon(ev.source, SKILLID_SEGNEN, CInt(castingskill*0.9));
          // adjust skilltypes
          SetObjProperty(ev.source, SKILLTYPKENNUNG+SKILLID_SEGNEN, GetObjProperty(ev.source, SKILLTYPKENNUNG+SKILLID_FLUCHEN));
          EraseObjProperty(ev.source, SKILLTYPKENNUNG+SKILLID_FLUCHEN);

          // erase possible item skill, set new item skill / adjust skilltypes
          If (HasPlayerSkill(ev.source, SKILLID_VERFLUCHTEGEGENST))
            SetSkillPergon(ev.source, SKILLID_VERFLUCHTEGEGENST, 0);
            SetSkillPergon(ev.source, SKILLID_GESEGNETEGEGENST, CInt(itemskill*0.9));
            SetObjProperty(ev.source, SKILLTYPKENNUNG+SKILLID_GESEGNETEGEGENST, GetObjProperty(ev.source, SKILLTYPKENNUNG+SKILLID_VERFLUCHTEGEGENST));
            EraseObjProperty(ev.source, SKILLTYPKENNUNG+SKILLID_VERFLUCHTEGEGENST);
          EndIf

          // reset player's intelligence
          SetIntPergon(ev.source, CInt(intelligence*0.95));

          // fetch player's spellbook and destroy it
          Var book:=FindItemInContainerTree(ev.source.backpack, UOBJ_SPELLBOOK_NECRO);
          If (book)
            DestroyItem(book);
          EndIf

          // fetch necro scrolls (inscription and casting) and destroy them
          Var dmg:=0;
          ForEach item in EnumerateItemsInContainer(ev.source.backpack)
            If (((0x67c0<=item.objtype) And (item.objtype<0x67ff)) Or // necro inscription
                ((0x69c0<=item.objtype) And (item.objtype<0x69ff)))   // necro casting
              DestroyItem(item);
              dmg:=1;
            EndIf
          EndForEach

          If (dmg)
            PlaySoundEffect(ev.source, SFX_55);
            SendSysMessagePergon(ev.source, "Eure Spruchrollen fangen Feuer und verbrennen Euch sehr.");
            ApplyRawDamagePergon(ev.source, GetHPPergon(ev.source)/2);
          EndIf

          // create new spellbook
          CreateSpellBookInBackpack(ev.source, UOBJ_SPELLBOOK_CLERIC);
          SetObjProperty(ev.source, "karma", 1); // reset karma of player
          SetObjProperty(ev.source, "GotLastKarma", ReadGameClock());

          PlaySoundEffect(ev.source, SFX_244);
          SendSysMessagePergon(ev.source, "Ihr seid zum Priester geworden.");

          SysLog("HINWEIS: "+ev.source.name+" konvertierte zum Priester");
        EndIf
      Else // Player hasn't startet quest and wants to convert
        NPCSayTextTraditional(me, "Ihr koennt noch nicht konvertieren.", "");
        Return (1);

        // Noch abgeschaltet...
        If (HasPlayerSkill(ev.source, SKILLID_SEGNEN))
          NPCSayTextTraditional(me, "Ihr seid bereits Priester also belaestigt mich nicht weiter.", "");
        ElseIf (HasPlayerSkill(ev.source, SKILLID_FLUCHEN))
          If (SendYesNoDialog(ev.source, "Wollt Ihr wirklich auf die lichten Weg wechseln?", 0, 0))
            NPCSayTextTraditional(me, "Ihr muesst irgendetwas tun, was ich mir noch nicht ausgedacht habe.", "");
            NPCSayTextTraditional(me, "Kehrt zurueck, wenn Ihr Euch unser wuerdig erwiesen habt.", "");

            SetObjProperty(ev.source, "ClericConvertQuest", "Open"); // start quest
          Else
            NPCSayTextTraditional(me, "Falls Ihr es Euch anders ueberlegt, wisst Ihr, wo Ihr mich finden koennt.", "");
          EndIf
        Else
          NPCSayTextTraditional(me, "Ihr koennt nicht zum Priester werden, egal was Ihr auch versucht.", "");
        EndIf
      EndIf

    SKILLID_FLUCHEN:
      Var convertquest:=GetObjProperty(ev.source, "NecroConvertQuest"); // get Convert Quest status
      If (convertquest) // check if player has started quest
        If (convertquest=="Open") // check if quest is fullfilled
          NPCSayTextTraditional(me, "Ich habe Euch gesagt, was Ihr zu tun habt, also tut es und stehlt nicht meine Zeit.", "");
        Else
          // do something to cleanup remnants of quest
          // check timstamps, items etc.
          // then convert

          NPCSayTextTraditional(me, "Ahh, Ihr habt Euch wuerdig erwiesen, zu uns zu gehoeren. Willkommen unter den Verruchten.", "");

          // get player's intelligence and casting/item skills
          Var castingskill:=GetSkillPergon(ev.source, SKILLID_SEGNEN);
          Var itemskill:=GetSkillPergon(ev.source, SKILLID_GESEGNETEGEGENST);
          Var intelligence:=GetIntPergon(ev.source);

          // erase former casting skill, set new casting skill
          SetSkillPergon(ev.source, SKILLID_SEGNEN, 0);
          SetSkillPergon(ev.source, SKILLID_FLUCHEN, CInt(castingskill*0.9));
          // adjust skilltypes
          SetObjProperty(ev.source, SKILLTYPKENNUNG+SKILLID_FLUCHEN, GetObjProperty(ev.source, SKILLTYPKENNUNG+SKILLID_SEGNEN));
          EraseObjProperty(ev.source, SKILLTYPKENNUNG+SKILLID_SEGNEN);

          // erase possible item skill, set new item skill / adjust skilltypes
          If (HasPlayerSkill(ev.source, SKILLID_GESEGNETEGEGENST))
            SetSkillPergon(ev.source, SKILLID_GESEGNETEGEGENST, 0);
            SetSkillPergon(ev.source, SKILLID_VERFLUCHTEGEGENST, CInt(itemskill*0.9));
            SetObjProperty(ev.source, SKILLTYPKENNUNG+SKILLID_VERFLUCHTEGEGENST, GetObjProperty(ev.source, SKILLTYPKENNUNG+SKILLID_GESEGNETEGEGENST));
            EraseObjProperty(ev.source, SKILLTYPKENNUNG+SKILLID_GESEGNETEGEGENST);
          EndIf

          // reset player's intelligence
          SetIntPergon(ev.source, CInt(intelligence*0.95));

          // fetch player's spellbook and destroy it
          Var book:=FindItemInContainerTree(ev.source.backpack, UOBJ_SPELLBOOK_CLERIC);
          If (book)
            DestroyItem(book);
          EndIf

          // fetch cleric scrolls (inscription and casting) and destroy them
          Var dmg:=0;
          ForEach item in EnumerateItemsInContainer(ev.source.backpack)
            If (((0x6760<=item.objtype) And (item.objtype<=0x67bf)) Or // cleric inscription
                ((0x6960<=item.objtype) And (item.objtype<=0x69bf)))   // cleric casting
              DestroyItem(item);
              dmg:=1;
            EndIf
          EndForEach

          If (dmg)
            PlaySoundEffect(ev.source, SFX_55);
            SendSysMessagePergon(ev.source, "Eure Spruchrollen fangen Feuer und verbrennen Euch sehr.");
            ApplyRawDamagePergon(ev.source, GetHPPergon(ev.source)/2);
          EndIf

          // create new spellbook
          CreateSpellBookInBackpack(ev.source, UOBJ_SPELLBOOK_NECRO);
          SetObjProperty(ev.source, "karma", -1); // reset karma of player
          SetObjProperty(ev.source, "GotLastKarma", ReadGameClock());

          PlaySoundEffect(ev.source, SFX_247);
          SendSysMessagePergon(ev.source, "Ihr seid zum Nekromanten geworden.");

          SysLog("HINWEIS: "+ev.source.name+" konvertierte zum Nekromanten");
        EndIf
      Else // Player hasn't startet quest and wants to convert
        NPCSayTextTraditional(me, "Ihr koennt noch nicht konvertieren.", "");
        Return (1);

        // Noch abgeschaltet...
        If (HasPlayerSkill(ev.source, SKILLID_FLUCHEN))
          NPCSayTextTraditional(me, "Was belaestigt Ihr mich, wo Ihr doch bereits einer von uns seid - Verschwindet!", "");
        ElseIf (HasPlayerSkill(ev.source, SKILLID_SEGNEN))
          If (SendYesNoDialog(ev.source, "Wollt Ihr wirklich zur dunklen Seite wechseln?", 0, 0))
            NPCSayTextTraditional(me, "Ihr muesst was saufieses tun, was verdammt weh tut, ich aber noch nicht weiss.", "");
            NPCSayTextTraditional(me, "Kehrt zurueck, wenn Ihr beweisen koennt, dass wir uns Eurer nicht zu schaemen brauchen.", "");

            SetObjProperty(ev.source, "NecroConvertQuest", "Open"); // start quest
          Else
            NPCSayTextTraditional(me, "Ueberlegt Euch gut, ob Ihr mir nochmal meine Zeit stehlen wollt.", "");
          EndIf
        Else
          NPCSayTextTraditional(me, "Versucht es garnicht erst. Die Nekromantie ist an Euch doch nur verschwendet.", "");
        EndIf
      EndIf

    default: Return; // Gemaess SpeechConfig antworten
  EndCase

  Return (1);
EndFunction
