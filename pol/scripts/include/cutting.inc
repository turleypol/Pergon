// Include for butchering / skinning

Const CORPSE_IS_BUSY := "busy";
Const CORPSE_IS_SKINNED := "skinned";
Const CORPSE_IS_BUTCHERED := "butchered";

var UOBJ_HIDES_String := {
      "fell", "hasenfell", "schlangenhaut", "wolffell",
      "hoehlenwolffell", "gremlinfell", "llamafell", "baerenfell",
      "grizlyfell", "pantherfell", "gatorfell", "feuergatorhaut",
      "ostardfell", "waldostardfell", "raubostardfell", "trollfell",
      "bluefell", "seeschlangenfell", "drachenfell", "olddragonfell",
      "zentaurenfell1", "zentaurenfell2", "zentaurenfell3",
      "zentaurenfell4"
  };

var Mat_Cfg := ReadConfigFile(":crafting:materials");

var DrachenCorpseTemplates := {
      0xc, 0x2e, 0x3b, 0x3c, 0x3d, 0x3e, 0x31a, 0x31f, 0x6a, 0xb4
    };
var DaemonenCorpseTemplates := {
      0x95, 0x9, 0x31B, 0x66, 0x318, 0x2b, 0x13E, 0x310, 0x26
    };
var EinhornCorpseTemplates := {0x7A};

Function CheckPreconditionsAndGetTarget(who)
    If (GetObjProperty(who, ARENABATTLE))
      // In der Arena darf nicht geschnibbelt werden ...
      SendSysMessagePergon(who, "Ihr befindet euch in einer Arena!", "");
      return;
    EndIf

    SendSysMessagePergon(who, "Worauf wollt Ihr das anwenden?", "");
    var object := TargetCoordinates(who);
    If (!object)
        SendSysMessagePergon(who, "Abbruch", "Abort");
        return;
    EndIf

    If (object.item.objtype == UOBJ_CORPSE and object.item.getprop("nocarve"))
        SendSysMessagePergon(who, "Dies könnt ihr nicht häuten oder schlachten.", "You cannot carve this.");
        return;
    EndIf

    // Sichtlinie checken
    If (object.item.objtype == UOBJ_CORPSE and !CheckLineOfSight(who, object.item))
        SendSysMessagePergon(who, "Ihr habt kein freies Blickfeld darauf.", "");
        return;
    EndIf

    // Entfernung checken...
    If (Distance(who, object.item) > 2)
        SendSysMessagePergon(who, "Ihr kommt dort nicht ran. Das ist zu weit entfernt.", "");
        return;
    EndIf

    // eigentlich schon vernichtete Leichen nicht haeuten
    If (GetObjProperty(object.item, "guardkill"))
        SendSysMessagePergon(who, "Diese kann nicht geschlachtet oder gehäutet werden.", "This corpse cannot be skinned or butchered.");
        return;
    EndIf

    return object;
EndFunction

Function IsCorpsePolymorphed(corpse)
    If (
        // um rauszufinden ob es sich um Polymorphte handelt
        // alte Magie
        GetObjProperty(corpse, "origform") or
        // neue Magie
        GetObjProperty(corpse, "spell_poly") or
        // Censorschleim
        GetObjProperty(corpse, "PunishOldGraphic")
    )
        return 1;
    EndIf
EndFunction

Function GetGenerationOffset(corpse)
    var generation := GetObjProperty(corpse, "Generation");
    var generation_offset;
    If (generation)
        generation_offset := generation;
    Else
        generation_offset := 0; // 0%
    EndIf

    return generation_offset;
EndFunction


// Ein paar Aeste vom Baum abschneiden
Function CarveTree(who, tree)
    If (Distance(who, tree) > 2)
        SendSysMessagePergon(who, "Ihr kommt dort nicht ran. Eure Arme sind zu kurz.", "You cannot reach this.");
        return;
    EndIf

    SendSysMessagePergon(who, "Ihr schneidet ein paar dünne Äste ab und legt sie in Euren Rucksack.", "You cut off some branches and put them in your backpack.");
    CreateItemInBackpackPergon(who, UOBJ_KINDLING, 1);
EndFunction

// Spinnenseide aus Netzen schneiden
Const UACTION_CHOP := ANIM_ATTACK_2HAND_WIDE;
Function GetSilk(who, web, blade)
    // Entfernung checken ...
    If (CoordinateDistance(who.x, who.y, web.x, web.y) > 2)
        SendSysMessagePergon(who, "Ihr kommt dort nicht ran. Das ist zu weit entfernt.", "You cannot reach that.");
        return;
    EndIf

    var diff := GetHarvestDifficulty("silk", web.x, web.y, web.objtype, web.realm);
    If (diff == error)
        SendSysMessagePergon(who, "Hier ist zu wenig Spinnenseide. Es lohnt sich nicht.", "There is no more silk here.");
        return;
    EndIf

    For i := 1 To 5
        PerformAction(who, UACTION_CHOP);
        PlaySoundEffect(who, SFX_57);
        Sleep(1);

        If ((who.x != who.x) or (who.y != who.y))
            SendSysMessagePergon(who, "Wer herumhüpft bekommt keine Spinnenseide.", "Jumping around won't get you any silk.");
            return;
        EndIf
    EndFor

    // Werkzeug-Eigenschaften auslesen
    var werkzeug := GetPropertyItem(blade);
    var points   := GetSkillDefaultPoints(SKILLID_WILDNISKUNDE);

    If (CheckSkillPergon(who, SKILLID_WILDNISKUNDE, -1, points))
        var silk_amount := HarvestResource("silk", web.x, web.y, 1, 1, web.realm);
        // Menge anpassen:
        silk_amount := CInt(
            // abhaengig vom Skill
            GetSkillPergon(who, (SKILLID_WILDNISKUNDE))/20.0/silk_amount +
            // abhaengig vom Werkzeug
            (werkzeug.quality + 1) * 1.75
        )
        // und abhaengig vom Zufall
        - 2 + RandomInt(5);

        If (silk_amount < 1)
            SendSysMessagePergon(who, "Ihr scheitert bei dem Versuch, die Spinnenseide loszuschneiden.", "You failed cutting off some silk.");
            return;
        EndIf

        CreateItemInBackpackPergon(who, UOBJ_SPINNENSEIDE, silk_amount);
        SendSysMessagePergon(who, "Ihr packt "+silk_amount+" Spinnenseide in Euren Rucksack.", "You put "+silk_amount+" silk in your backpack.");

        // schleichende Werkzeugabnutzung
        If (RandomInt(100) <= 1)
            If (SetWerkzeugSchaden(who, blade, "Euer Messer hat gelitten!", "Euer Messer ist stark beschädigt!", "Euer Messer ist zerbrochen!"))
                return;
            EndIf
        EndIf

        return;
    EndIf

    // gescheitert
    SendSysMessagePergon(who, "Ihr konntet keine Spinnenseide losschneiden.", "You failed cutting off some silk.");
    If (RandomInt(100) >= 10)
        // meist war es das
        return;
    EndIf

    // bei 10% der Fehlversuche wird je nach Skill Spinnenseide auf der
    // Koordinate abgezogen, sind also nicht mehr schneidbar, oder es
    // erfolgt eine andere Bestrafung

    // Auf Jhe'lom kommen keine Spinnen
    var IntWert := 12;
    If (PlaceInfos(web).newbie)
        IntWert := 9;
    EndIf

    Case (RandomInt(IntWert))
    0: 1: 2: // Seide abziehen
        HarvestResource("silk", web.x, web.y, 1, CInt(GetSkillPergon(who, (SKILLID_WILDNISKUNDE))/10.0), web.realm);

    3: 4: 5: 6: // Werkzeug schaedigen
        If (SetWerkzeugSchaden(who, blade, "Euer Messer hat gelitten!", "Euer Messer ist stark beschädigt!", "Euer Messer ist zerbrochen!"))
            return;
        EndIf

    7: 8: // im Netz verfangen
        If (who.cmdlevel >= CMDLEVEL_SEER)
            SendSysMessagePergon(who, "Ihr wäret in den Spinnenweben gefangen!", "");
            return;
        EndIf

        points := GetSkillDefaultPoints(SKILLID_MAGIERESISTENZ);
        If (!CheckSkillPergon(who, SKILLID_MAGIERESISTENZ, -1, points))
            SendSysMessagePergon(who, "Ihr seid in den Spinnenweben gefangen!", "You are ensnared in the cobweb.");
            who.paralyzed := 1;
            var tag := array;
            tag[1] := ReadGameClock() + 15;
            tag[2] := 100;
            tag[5] := ReadMilliSecondClock();
            IncRevision(who);
            SetObjProperty(who, "spell_paralyze", tag);
            SendSpellWaiter(who, "spell_paralyze", 15, tag[5], who);
        EndIf

    // ab hier gibt es Tierchen
    9:
        var it := CreateNpcFromTemplate("spider", web.x, web.y, web.z, 0, who.realm);
        If (it <> error)
            SendSysMessagePergon(who, "Eine Riesenspinne wird neugierig auf Euch.", "A big spider gets interested in you.");
        EndIf

    10:
        var it := CreateNpcFromTemplate("rockspider", web.x, web.y, web.z, 0, who.realm);
        If (it <> error)
            SendSysMessagePergon(who, "Eine Felsenspinne wird neugierig auf Euch.", "A rock spider gets interested in you.");
        EndIf

    11:
        var it := CreateNpcFromTemplate("frostspider", web.x, web.y, web.z, 0, who.realm);
        If (it <> error)
            SendSysMessagePergon(who, "Eine Eistarantel wird neugierig auf Euch.", "A frost spider gets interested in you.");
        EndIf
    EndCase
EndFunction

// Questitems gewinnen
Function QuestCarve(character, cadaver)
    var i, j := 0, beweis;
    var prop := character.getprop("quest");
    var group := GetNPCConfig(cadaver.getprop("npctemplate")).group;
    ForEach key in (prop.keys())  // Alle offenen Quests durchgehen
        If (key<>"completedquests")  // Den brauchen wir nicht
            If (prop[key][1] == "kill")  // Metzelquest? Braucht es Beweise
                For (i := 1;i <= prop[key][2].size();i += 1)  // Alle Templates durchgehen
                    If ((Lower(prop[key][2][i][1]) == Lower(cadaver.getprop("npctemplate")))||(Lower(prop[key][2][i][1]) == lower(group)))
                        ForEach item in FindSubstance(character.backpack, QUEST_DUMMY, 1, 0, FINDSUBSTANCE_FIND_ALL)
                            If ((Lower(item.getprop("template")) == Lower(cadaver.getprop("npctemplate"))) || (lower(item.getprop("template")) == lower(group)))
                                j += 1;
                            EndIf
                        EndForEach
                        If (j<CInt(prop[key][2][i][2]))  // Nur wenn weniger vorhanden sind, werden mehr erstellt
                            beweis := CreateItemInBackpackPergon(character, QUEST_DUMMY, 1, 0);  // DummyQuestItem
                            beweis.graphic := CInt(prop[key][2][i][4]);
                            beweis.name := StrReplace(prop[key][2][i][3], "_", " ");
                            beweis.setprop("ctquest", ReadGameClock()); // Sinnlos CProp damit nicht mehr Stackbar
                            If (prop[key][2][i][5])
                                beweis.color := prop[key][2][i][5];
                            EndIf
                            If (Lower(cadaver.getprop("npctemplate")) == Lower(prop[key][2][i][1]))
                                beweis.setprop("template", cadaver.getprop("npctemplate"));
                            ElseIf (Lower(group) == Lower(prop[key][2][i][1]))
                                beweis.setprop("template", group);
                            Else
                                syslog("Ungueltiges Questitem: " +beweis.serial);
                            EndIf
                            SendSysMessagePergon(character, "Ihr schneidet "+beweis.name+" aus der Leiche.");
                            If ((j+1) == CInt(prop[key][2][i][2]))
                                SendSysMessagePergon(character, "Das sollte reichen.");
                                PlaySoundEffectPrivate(character, SFX_5B7, character);
                            EndIf
                            return (1); // positiver return um direkt Blut erzeugen, sonst normal weitermachen
                        EndIf
                    EndIf
                EndFor
            ElseIf (prop[key][1] == "npcitem") // NPCItem
                For (i := 1;i <= prop[key][2].size();i += 1)  // Alle Templates durchgehen
                    If ((Lower(prop[key][2][i][1]) == Lower(cadaver.getprop("npctemplate"))) || (Lower(prop[key][2][i][1]) == Lower(group)))
                        ForEach item in FindSubstance(character.backpack, QUEST_DUMMY, 1, 0, FINDSUBSTANCE_FIND_ALL)
                            If ((Lower(item.getprop("npcitemtemplate")) == Lower(cadaver.getprop("npctemplate")))||(Lower(item.getprop("npcitemtemplate")) == Lower(group)))
                                j += 1;
                            EndIf
                        EndForEach
                        If (j<CInt(prop[key][2][i][2]))  // Nur wenn weniger vorhanden sind, werden mehr erstellt
                            If (RandomInt(100)<CInt(prop[key][2][i][5])) // DropChance
                                beweis := CreateItemInBackpackPergon(character, QUEST_DUMMY, 1, 0);  // DummyQuestItem
                                beweis.graphic := CInt(prop[key][2][i][4]);
                                beweis.name := StrReplace(prop[key][2][i][3], "_", " ");;
                                beweis.setprop("ctquest", ReadGameClock()); // Sinnlos CProp damit nicht mehr Stackbar
                                If (prop[key][2][i][6])
                                beweis.color := prop[key][2][i][6];
                                EndIf
                                If (Lower(prop[key][2][i][1]) == Lower(cadaver.getprop("npctemplate")))
                                beweis.setprop("npcitemtemplate", cadaver.getprop("npctemplate"));
                                ElseIf (Lower(prop[key][2][i][1]) == Lower(group))
                                beweis.setprop("npcitemtemplate", group);
                                Else
                                syslog("Falsches NPCItem erzeugt, besitzt keine Zuordnung: " + beweis.serial);
                                EndIf
                                SendSysMessagePergon(character, "Ihr schneidet "+beweis.name+" aus der Leiche.");
                                If ((j+1) == CInt(prop[key][2][i][2]))
                                SendSysMessagePergon(character, "Das sollte reichen.");
                                PlaySoundEffectPrivate(character, SFX_5B7, character);
                                EndIf
                                return (1); // positiver return um direkt Blut erzeugen, sonst normal weitermachen
                            EndIf
                        EndIf
                    EndIf
                EndFor
            EndIf
        EndIf
    EndForEach
EndFunction

// Ist es ein Spinnennetz?
Function IsSpiderWeb(web)
    If (
        (0x0ee3 <= web.objtype and web.objtype <= 0x0ee6) or
        (0x10b8 <= web.objtype and web.objtype <= 0x10d7) or
        (0x10da <= web.objtype and web.objtype <= 0x10dc)
    )
        return 1;
    EndIf

    return 0;
EndFunction

Function CreateLimbs(pos)
    var al := CreateItemAtLocation(pos.x, pos.y, pos.z, UOBJ_ARM_LEFT,  1, pos.realm);
    MoveObjectToLocation(al, pos.x - 1, pos.y,     pos.z, pos.realm);
    var ar := CreateItemAtLocation(pos.x, pos.y, pos.z, UOBJ_ARM_RIGHT, 1, pos.realm);
    MoveObjectToLocation(ar, pos.x - 1, pos.y + 1, pos.z, pos.realm);
    var ll := CreateItemAtLocation(pos.x, pos.y, pos.z, UOBJ_LEG_LEFT,  1, pos.realm);
    MoveObjectToLocation(ll, pos.x + 1, pos.y,     pos.z, pos.realm);
    var lr := CreateItemAtLocation(pos.x, pos.y, pos.z, UOBJ_LEG_RIGHT, 1, pos.realm);
    MoveObjectToLocation(lr, pos.x + 1, pos.y + 1, pos.z, pos.realm);

    CreateItemAtLocation(pos.x, pos.y, pos.z, UOBJ_TORSO, 1, pos.realm);
    CreateItemAtLocation(pos.x, pos.y, pos.z, UOBJ_TRUNK, 1, pos.realm);
    CreateItemAtLocation(pos.x, pos.y, pos.z, UOBJ_HEART, 1, pos.realm);
EndFunction

// Fisch zerlegen/filetieren
Function CarveFish(who, fish, blade)
    If (!ReserveItem(fish))
        return;
    EndIf

    SendSysMessagePergon(who, "Ihr nehmt den Fisch aus ...");
    // fuer jeden auch ohne Skill gibt es ein Filet
    CreateItemInBackpackPergon(who, UOBJ_FISHSTEAK, 1);

    // Wieviel Fish hab ich?
    var sourceamount := fish.amount;
    // Wieviel Filets lassen sich max. erzeugen
    var destamount := fish.amount;
    // Prozente des Schlachten-Skill gegenueber der Difficulty des Fleisches
    var aufschlag := 0;

    // Werkzeug-Eigenschaften auslesen
    var werkzeug := GetPropertyItem(blade);
    var toolQuality := werkzeug.quality;

    // Verschafft mir mein Werkzeug einen Bonus?
    If (toolQuality+GetSkillPergon(who, SKILLID_SCHLACHTEN)>10)
        // Max. 3 * Anzahl Filets mit Char-Skill > 10%
        aufschlag := Min(300, (toolQuality+GetSkillPergon(who, SKILLID_SCHLACHTEN))*10.0);
        destamount := CInt(sourceamount/100.0*aufschlag);
    EndIf

    var pos := struct;
    pos.x := who.x;
    pos.y := who.y;

    // -1, denn man bekommt immer ein "Trostfilet"
    destamount -= 1;
    var countsucc := 1;

    If (!destamount)
        SubtractAmount(fish, 1);
    EndIf

    // Fisch-Filets erzeugen
    While (fish and destamount>0)
        Sleep(1);
        PlaySoundEffect(who, SFX_57);

        If (fish)
            If ((fish.amount > (destamount*100.0)/aufschlag) or !aufschlag)
                // Fisch abziehen
                SubtractAmount(fish, 1);
            EndIf
        EndIf

        // 10% auch ohne Skill
        If (CheckSkillPergon(who, SKILLID_SCHLACHTEN, CInt(10-toolQuality), CInt((toolQuality+10)*6)) or (RandomInt(100) < 10))
            CreateItemInBackpackPergon(who, UOBJ_FISHSTEAK, 1);
            countsucc += 1;

            // schleichende Werkzeugabnutzung
            If (RandomInt(100) <= 1)
                If (SetWerkzeugSchaden(who, blade, "Euer Messer hat gelitten!", "Euer Messer ist stark beschädigt!", "Euer Messer ist zerbrochen!"))
                    return;
                EndIf
            EndIf
        Else
            SendSysMessagePergon(who,"Ihr konntet kein Filet herauslösen.", "You couldn't carve a filet.");

            If (SetWerkzeugSchaden(who, blade, "Euer Messer hat gelitten!", "Euer Messer ist stark beschädigt!", "Euer Messer ist zerbrochen!"))
                break;
            EndIf
        EndIf

        destamount -= 1;

        If (CoordinateDistance(who.x, who.y, pos.x, pos.y) > 0)
            SendSysMessagePergon(who, "Unterwegs könnt Ihr keinen Fisch schneiden. Ihr gebt auf.", "You cannot carve fish whilst moving.");
            break;
        EndIf
    EndWhile

    SendSysMessagePergon(who, "Ihr habt "+countsucc+" Filets gewonnen");
EndFunction

// Ist es ein Fisch?
Function IsFish(fish)
    If ((UOBJ_FISH_START <= fish.objtype and fish.objtype <= UOBJ_FISH_END) or
        (UOBJ_SFISH_START <= fish.objtype and fish.objtype <= UOBJ_SFISH_END))
        return 1;
    EndIf

    return 0;
EndFunction

Function GenerateBlood(corpse)
    var isPoly := IsCorpsePolymorphed(corpse);

    // etwas Blut erzeugen (Drachen geben grÃ¼nes Blut)
    var bloodtype;
    If (isPoly == 0)
        If(corpse.corpsetype in DrachenCorpseTemplates)
            bloodtype := "dragonblood";
        ElseIf(corpse.corpsetype in DaemonenCorpseTemplates)
            bloodtype := "daemonbloodpool";
        ElseIf(corpse.corpsetype in EinhornCorpseTemplates)
            bloodtype := "unicornbloodpool";
        Else
            bloodtype := UOBJ_BLOOD;
        EndIf
    Else
        bloodtype := UOBJ_BLOOD;
    EndIf

    var theblood := CreateItemAtLocationPergon(corpse.x, corpse.y, corpse.z, bloodtype, 1, corpse.realm);
    theblood.movable := 0;
    theblood.saveonexit := 0;

    return theblood;
EndFunction
