///////////////////////////////////////////////////////////////////////////
//
// balancing.inc
//
// Funktionalität und Constanten für Absicherung von Balancinggrenzen
//
// Author: Fox
//
//   Modification: 05.01.08 Fox: Man musste noch die Infos aus der Itemdesc->Coverage berücksichtigen
//                 06.01.08 Fox: Berechnung Waffenquality überarbeitet
//
//

Const MAX_QUAL_TINKERING_M_MH:=4.7375; //Werkzeug aus Holz+Metall und Holz, Bogensehnen, Kampfwerkzeuge(Hammer usw.)
Const MAX_QUAL_BLACKSMITH_M:=4.3585; //Rüsten und Waffen aus Metall
Const MAX_QUAL_CARPENTRY_B:=5.0255; //Bögen (Einfluss Materialart Sehne und Holz)
Const MAX_QUAL_CARPENTRY_HM:=5.685; //verstärkter Stab (Holz+Metall)
Const MAX_QUAL_CARPENTRY_H:=4.548; //Stab(Holz), Schild(Holz)
Const MAX_QUAL_TAILORING_LK:=4.45; //Rüste (Leder+Knochen)
Const MAX_QUAL_TAILORING_LS:=4.4; //Rüste (Leder+Schuppen)
Const MAX_QUAL_TAILORING_LM:=5.495; //Rüste (Leder+Metall)
Const MAX_QUAL_TAILORING_L:=4.35; //Rüste (Leder)
Const MAX_QUAL_TAILORING_S:=6.5; //Rüste (Stoff)


Function CalcFaktorFight(who, enemy, skill, speak, who_talk, calculate, collecttext:=0,byref collectforhtml:=0)
//skill: der Skill auf den der Faktor angewendet werden soll
//speak: 0 - keine Hinweise
//       1 - Spielerhinweise
//       2 - Commandlevelhinweise
//       3 - Spielerhinweise nur für Sonderregeln
//       4 - Spielerhinweise nur für Sonderregeln die für jeden Skill anders lauten
//calculate: 0 - in Timer Abstand berechnen
//           1 - Berechnung durchführen
//collecttext: sammelt Meldungen (nur bei speak2) und liefert sie zurück (mit collectforhtml)
//nur in festen Abständen die Berechnungen durchführen, ausser calculate gesetzt

  Var sonder_speak:=0;
  Var extraspeak:=1;
  If (speak==3)
    sonder_speak:=1;
    speak:=1;
  EndIf
  If (speak==4)
    sonder_speak:=1;
    speak:=1;
    extraspeak:=0;
  EndIf

  If (!(CInt(GetGlobalProperty(PROP_WEAPON_RULES_ACTIVE))==111))
    If ( (speak == 2) && (calculate == 1) )
      If (!collecttext)
        SendSysMessagePergon(who_talk, "Waffen-Regeln sind deaktiviert, Anzeige nur per status von GM.");
      EndIf
    Else
      Return(1); //keine Veränderung wenn die Kalkulation nicht aktiviert ist
    EndIf
  EndIf

  If (
    (CInt(GetObjProperty(who_talk, PROP_NO_WEAPON_RULE_NOTICE)) == 1) &&
    (CInt(calculate) != 1))
    speak:=0;
  EndIf

  //Waffe suchen
  Var equipment1:=GetEquipmentByLayer(who, LAYER_HAND1);
  Var equipment2:=GetEquipmentByLayer(who, LAYER_HAND2);
  Var weapon;
  If (equipment1.isa(POLCLASS_WEAPON))
    weapon:=equipment1;
  ElseIf (equipment2.isa(POLCLASS_WEAPON))
    weapon:=equipment2;
  EndIf

  //nur die komplexen Ausrüstungsregeln speichern
  Var faktor_ausruestung:=1.0;
  Var lastCalc:=struct;
  Var doCalculate:=1;
  lastCalc:=GetObjProperty(who, "LastEquipmentCalc");
  If (calculate < 1)
    If ((lastCalc && ((lastCalc.Time + 60) > ReadGameClock())))
      faktor_ausruestung:=lastCalc.Value;
      doCalculate:=0;
    Else
      doCalculate:=1;
    EndIf
  Else
    doCalculate:=1;
  EndIf

  If (doCalculate)
    faktor_ausruestung:=CalcFaktorEquipment(who, weapon, speak, who_talk,collecttext,collectforhtml);

    Var lastCalc2:=struct;
    lastCalc2.+Value:=faktor_ausruestung;
    lastCalc2.+Time:=ReadGameClock();
    SetObjProperty(who, "LastEquipmentCalc", lastCalc2);
  EndIf

  Var faktor:=1.0;
  faktor*=faktor_ausruestung;

  //Da die Sonderregeln von Skill zu Skill variieren immer berechnen, aber nur Talken wenn auch die Ausruestung berechnet wird
  If (sonder_speak)
    speak:=1;
    doCalculate:=1;
  EndIf
  If (doCalculate)
    faktor*=CalcFaktorSonderregeln(who, enemy, skill, weapon, speak, who_talk, collecttext,collectforhtml);
  Else
    faktor*=CalcFaktorSonderregeln(who, enemy, skill, weapon, 0, who_talk, collecttext,collectforhtml);
  EndIf

  If (speak == 2)
    If (!collecttext)
      SendSysMessagePergon(who_talk, "Summe der Regeln ergibt Faktor="+faktor);
    Else
      collectforhtml.append("Summe der Regeln ergibt Faktor="+faktor);
    EndIf
  EndIf

  Return(faktor);
EndFunction

Function CalcFaktorEquipment(who, weapon, speak, who_talk,collecttext:=0,byref collectforhtml:=0)
//speak: 0 - keine Hinweise
//       1 - Spielerhinweise
//       2 - Commandlevelhinweise

//Umsetzung der Waffenregeln
//- 1. Ermitteln der aktuellen Gesamtqualität der Rüstung (nach Anteiltabelle) quali_ruestung
//- 2. Verrechnen mit der Qualität der Waffe
//    - 20% Freiraum für Faktor 1
//    - faktor=quali_waffe/quali_ruestung
//    - Stoffteile (Teile mit niedriger ar) gehen nicht in die Berechnung ein
// Probleme: - Stimmen die Qualitäten der Waffen (Metall, Holz, Fernkampf) mit denen der Rüsten (Metall, Leder, Stoff) überein?
//           - Einfluss von Waffenart einfliesen lassen? <- Nein, da erheblicher Aufwand der Einordnung und Ungleichbehandlung verschiedener Kampfskills.
//           - Orientierung für Spieler? <- in .status packen
//           - Bogenschützen brauchen zum skillen gar keine Rüste theoretisch
//           - Taktik und Anatomyverweigerer ab 80% abstrafen
//
//Beschädigungen der Waffe beeinflussen den Skillgewinn
//- 20% Freiraum für Faktor 1
//- faktor=Waffe_Quali_org/Waffe_Quali_now
//
//- Per Global CProps "EquipmentFactor" deaktivierbar, um auf Fehler ragieren zu können
//- Einbauen das die Aktualisierung nur ab und zu geschieht, so das etwas alle 1min das Equipment gescannt wird. Mit .status immer rechnen.

//      ar     real chance
//helm  122    17   0.14
//hals  98     7    0.07
//hand  64     5    0.07
//arm   94     13   0.14
//brust 83     36   0.44
//bein  93     13   0.14
//
//Lederqualitäten liegen deutlich höher als die von Waffen

  Var faktor:=1;

  If (!(weapon)) //Keine Waffe keine Berechnungen notwendig
    If (speak == 2)
      If (!collecttext)
        SendSysMessagePergon(who_talk, "Keine Waffe, keine Anwendung der Waffenregeln.");
      Else
        collectforhtml.append("Keine Waffe, keine Anwendung der Waffenregeln.");
      EndIf
    EndIf
    Return(faktor);
  EndIf

  //Berechnung der Rüstungsqualität
  Var armrzone:=ReadConfigFile("::armrzone");
  If (armrzone)
    Var armorzones:=dictionary;
    Var maxchance:=0; // Sollte eigentlich 100 sein, zum Schluss... aber man weiss ja nie

    ForEach zonename in GetConfigStringKeys(armrzone)
      Var zone:=armrzone[zonename];
      armorzones.insert(zone.name, {zone.name, zone.chance, GetConfigStringArray(zone, "Layer")});
      //SendSysMessagePergon(who_talk, "zone.name/zone.chance/array "+zone.name+"/"+zone.chance+"/"+GetConfigStringArray(zone, "Layer"));
      maxchance+=zone.chance;
    EndForEach


    If (!(maxchance==100))
      //Sollte nicht vorkommen, führt aber nicht zu Fehlern
      LogToFile("::log/balancing.log", "Error [balancing.inc]: maxchance!=100 (who.serial="+who.serial+")");
      If (collecttext)
        collectforhtml.append("Error: maxchance!=100 ("+maxchance+")");
      EndIf
    EndIf

    //Rüstungsteile über die Zones verteilen, da einige mehrere und manche gar keine abdecken, nach itemdesc.coverage
    //Die Teile die über die Layer gefunden werden immer berücksichtigt.
    Var armorzones2:=dictionary;
    Var equip1;
    Var temp:=array;
    ForEach element in armorzones
      ForEach layer in (element[3]) //ermitteln des Teils mit der höchsten Armor auf dieser Zone
        equip1:=GetEquipmentByLayer(who, CInt(layer));
        If (equip1)
          Var item:=GetItemDescriptor(equip1.objtype);
          If (item)
            If (item.coverage)
              ForEach zone in (item.coverage) //Das Item nach seinen itemdesc.coverage Einträgen zu den Zonen hinzufügen
                //SendSysMessagePergon(who_talk, "zone/equip1/anteil "+zone+"/"+equip1.desc+"/"+armorzones[zone][2]);
                If (armorzones2.exists(zone))
                  armorzones2[zone][3].append(equip1);
                Else
                  temp:=array;
                  temp.append(equip1);
                  armorzones2.insert(zone, {zone, armorzones[zone][2], temp}); //zonenname und anteil für späteren Abruf mit merken
                EndIf
              EndForEach
            EndIf
          EndIf
          //SendSysMessagePergon(who_talk, "element[1]/equip1 "+element[1]+"/"+equip1.desc+"/"+element[2]);
          //Das Item nach seinem Equiplayer ablegen. Doppeleinträge stören nicht.
          If (armorzones2.exists(element[1]))
            armorzones2[element[1]][3].append(equip1);
          Else
            temp:=array;
            temp.append(equip1);
            armorzones2.insert(element[1], {element[1], element[2], temp});
          EndIf
        EndIf
        sleepms(2);
      EndForEach
    EndForEach

    //Die Chancen geben den Anteil an der Rüstung an der von einem Rüstungsteil gestellt wird
    //dementsprechend geht seine Qualität auch in die Gesamtqualität ein.
    Var equip2;
    Var max_ar_element;
    Var quality_armor:=CDbl(0); //Quality der Rüstung
    Var sum_ar:=CDbl(0);
    Var max_anteil:=CDbl(0); //bereinigt um nicht eingehenes Equipment
    Var quality_item:=CDbl(0); //Qualität des Items
    Var max_ar_mod_element:=CDbl(0);

    ForEach element in armorzones2
      //SendSysMessagePergon(who_talk, "armorzone"+element+", equips"+element[3]);
      max_ar_element:=-1;
      ForEach equip1 in (element[3]) //ermitteln des Teils mit der höchsten Armor auf dieser Zone
        //SendSysMessagePergon(who_talk, "name="+equip1.name);
        If (equip1.ar)
          If ((equip1.ar-equip1.ar_mod) > max_ar_element)
            equip2:=equip1;
            max_ar_element:=(equip1.ar-equip1.ar_mod); //Nur die AR des Items keine Sondereffekte
            max_ar_mod_element:=equip1.ar_mod;
          EndIf
        Else
          If (max_ar_element < 0)
            equip2:=equip1;
            max_ar_element:=0; //Item besitzt keine Ar (Bsp.: Magierhut)
            max_ar_mod_element:=0;
          EndIf
        EndIf
      EndForEach
      //SendSysMessagePergon(who_talk, "name/zone/max_ar_element/equip2.ar/equip2.ar_base "+equip2.desc+"/"+element[1]+"/"+max_ar_element+"/"+equip2.ar+"/"+equip2.ar_base);
      If (max_ar_element > 0)
        Var element_anteil:=CDbl(element[2])/CDbl(maxchance);
        sum_ar+=(max_ar_element+max_ar_mod_element)*element_anteil;
        //SendSysMessagePergon(who_talk, "name/zone/element_anteil/max_ar_element/equip2.ar/equip2.ar_base "+equip2.desc+"/"+element[1]+"/"+element_anteil+"/"+max_ar_element+"/"+equip2.ar+"/"+equip2.ar_base);

        If (max_ar_element >= 1) //Zur Sicherheit alles ignorieren was weniger als 1 ar hat
          Var quality_ref:=GetReferenceQuality(equip2);
          If (quality_ref > 0)
            quality_item:=max_ar_element/CDbl(equip2.ar_base); //Quality unter Berücksichtigung des Zustands (HP mit drin)
            quality_item*=(100.0/quality_ref); //Normierte Quality des Rüstungsteils ermitteln
            If (quality_item > 100)
              LogToFile("::log/balancing.log", "Error [balancing.inc]: quality_item > 100 (item.name="+equip2.name+", item.serial=" +equip2.serial+", who.serial="+who.serial+")");
              If (collecttext)
                collectforhtml.append("Error: quality_item > 100 (item.name="+equip2.name+", item.serial=" +equip2.serial+","+quality_item+")");
              EndIf
              quality_item:=0;
            EndIf
            //Gesamtqualität wird aus Einzelteilqualität ermittelt mit dem selben Einfluss wie die AR der Einzelteile eingeht
            quality_armor+=(quality_item*element_anteil);
            max_anteil+=element_anteil;
            If (speak==2)
              Var quality_direkt:=equip2.quality*100.0/GetReferenceQuality(equip2); //die Quality ist Basis für den max_hp Wert, passt aber nur für den reparierten Fall
              If (!collecttext)
                SendSysMessagePergon(who_talk, equip2.desc+"("+element[1]+")"+" mit quality="+quality_direkt+" mit quality_ar "+quality_item); //Abweichungen sollten nur Rundungseinflüssen zu verdanken sein
              Else
                collectforhtml.append(equip2.desc+"("+element[1]+")"+" mit quality="+quality_direkt+" mit quality_ar "+quality_item);
              EndIf
            EndIf
          Else
            LogToFile("::log/balancing.log", "Error [balancing.inc]: quality_ref = 0 (item.name="+equip2.name+", item.serial=" +equip2.serial+", who.serial="+who.serial+")");
            If (collecttext)
              collectforhtml.append("Error: quality_ref = 0 (item.name="+equip2.name+", item.serial=" +equip2.serial+", who.serial="+who.serial+")");
            EndIf
          EndIf
        Else
          If (speak==2)
            If (!collecttext)
              SendSysMessagePergon(who_talk, equip2.desc+" faellt raus wegen zu wenig AR");
            Else
              collectforhtml.append(equip2.desc+" faellt raus wegen zu wenig AR ("+max_ar_element+")");
            EndIf
          EndIf
        EndIf
      Else
        If (speak==2)
          If (!collecttext)
            SendSysMessagePergon(who_talk, equip2.desc+" kein Teil");
          Else
            collectforhtml.append(equip2.desc+" kein Teil");
          EndIf
        EndIf
      EndIf
      SleepMS(2);
    EndForEach
    If (max_anteil > 0)
      quality_armor/=CDbl(max_anteil); //Normieren der Ausrüstungs Quality
    Else
      quality_armor:=0; //er trägt keine releavanten Rüstungsteile
    EndIf

    If (speak==2)
      If (!collecttext)
        SendSysMessagePergon(who_talk, "Ruestqualitaet="+quality_armor+" sum_ar="+sum_ar+" who.ar="+who.ar+"  max_anteil="+max_anteil); //Bis auf Rundungsabweichungen müssen die AR Werte überein stimmen
      Else
        collectforhtml.append("Ruestqualitaet="+quality_armor+" sum_ar="+sum_ar+" who.ar="+who.ar+"  max_anteil="+max_anteil);
      EndIf
    EndIf

    //Schild dazu rechnen, wenn vorhanden
    Var shield:=0;
    //Waffe suchen
    Var equipment1:=GetEquipmentByLayer(who, LAYER_HAND1);
    Var equipment2:=GetEquipmentByLayer(who, LAYER_HAND2);
    If (equipment1.isa(POLCLASS_ARMOR))
      shield:=equipment1;
    ElseIf (equipment2.isa(POLCLASS_ARMOR))
      shield:=equipment2;
    EndIf
    If (shield)
      Var shield_ar:=(who.ar-who.ar_mod)-sum_ar;

      //quality_direkt:=shield.quality*100.0/GetReferenceQuality(shield);

      quality_item:=(shield.ar-shield.ar_mod)/CDbl(shield.ar_base); //Quality unter Berücksichtigung des Zustands (HP mit drin)
      quality_item*=(100.0/GetReferenceQuality(shield)); //Normieren

      If (speak == 2)
        If (!collecttext)
          SendSysMessagePergon(who_talk, "Schild Quality="+quality_item+" Anteil="+(shield_ar/(sum_ar+shield_ar)));
        Else
          collectforhtml.append("Schild Quality="+quality_item+" Anteil="+(shield_ar/(sum_ar+shield_ar)));
        EndIf
      EndIf

      quality_armor:=(quality_armor*sum_ar+quality_item*shield_ar)/(sum_ar+shield_ar);
    Else
      //macht nur ohne Schild Sinn weil die Schild Berechnung den Fehler verstecken würde
      If (Abs(sum_ar-(who.ar-who.ar_mod)) > 5)
        LogToFile("::log/balancing.log", "Error [balancing.inc]: sum_ar != who.ar (who.serial="+who.serial+")");
        If (speak > 0)
          If (!collecttext)
            SendSysMessagePergon(who_talk, "ERROR: Ruestungsberechnung inkonsistent (sum_ar != who.ar)" );
          Else
            collectforhtml.append("ERROR: Ruestungsberechnung inkonsistent (sum_ar != who.ar) ("+sum_ar+" != "+(who.ar-who.ar_mod)+")");
          EndIf
        EndIf
        quality_armor:=0; //Ungünstigste Quality für Player
      EndIf
    EndIf
    If (speak == 2)
      If (!collecttext)
        SendSysMessagePergon(who_talk, "Gesamtqualitaet="+quality_armor);
      Else
        collectforhtml.append("Gesamtqualitaet="+quality_armor);
      EndIf
    EndIf

    If (quality_armor > 100)
      LogToFile("::log/balancing.log", "Error [balancing.inc]: quality_armor > 100 (who.serial="+who.serial+")");
      If (collecttext)
        collectforhtml.append("Error: quality_armor > 100 ("+quality_armor+")");
      EndIf
      quality_armor:=0;
    EndIf

    //gegen Waffe rechnen
    If (weapon)
      //weapon.quality wird nach der weapon_werkzeug_prop[1] gesetzt beim reparieren und verändert die weapon.maxhp,
      //wird aber nicht verwendet für den Schaden sondern nur weapon.hp/itemdesc.maxhp

      //weapon.quality:=weapon.maxhp/itemdesc.maxhp
      //schaden:=itemdesc.schaden*weapon.hp/weapon.maxhp
      //akt_quality:=weapon.quality*weapon.hp/weapon.maxhp = weapon.hp/itemdesc.maxhp
      //quality_gerept:=weapon.werkzeug[2]/itemdesc.maxhp  (maxhp_prop:=weapon_werkzeug_prop[2])
      //               =weapon.werkzeug[2]/weapon.quality*weapon.maxhp

      Var weapon_werkzeug_prop:=GetObjProperty(weapon, "Werkzeug");
      Var quality_weapon:=0;
      If (weapon_werkzeug_prop)
        Var quality_base:=weapon.quality*100.0/GetReferenceQuality(weapon); //Normieren
        Var hp_faktor:=weapon.hp/CDbl(weapon.maxhp); //Beschädigung der Waffe berücksichtigen
        quality_weapon:=hp_faktor*quality_base;

        Var quality_gerept:=( weapon_werkzeug_prop[2]/(weapon.maxhp/weapon.quality) )*100.0/GetReferenceQuality(weapon);

        If (quality_weapon > (quality_gerept+5)) //durch Rundungsprobleme beim reppen nen kleinen Sicherheitsbereich von 5%
          LogToFile("::log/balancing.log", "Error [balancing.inc]: Waffe quality_weapon > quality_gerept (who.serial="+who.serial+" weapon.serial="+weapon.serial+")");
          If (speak == 2)
            If (!collecttext)
              SendSysMessagePergon(who_talk, "ERROR: Waffenberechnung inkonsistent (quality_weapon > quality_gerept)" );
            Else
              collectforhtml.append("ERROR: Waffenberechnung inkonsistent (quality_weapon > quality_gerept) ("+quality_weapon+" > "+quality_gerept+")");
            EndIf
          EndIf
          quality_weapon:=0;
        EndIf
        If (speak==2)
          If (!collecttext)
            SendSysMessagePergon(who_talk, "Waffe quality_gerept="+quality_gerept+" quality_weapon="+quality_weapon);
          Else
            collectforhtml.append("Waffe quality_gerept="+quality_gerept+" quality_weapon="+quality_weapon);
          EndIf
        EndIf
      Else //NPC Waffe
        quality_weapon:=3;
      EndIf

      //if (weapon.attribute==SKILLID_BOGENSCHIESSEN)
      //Fernkampfwaffe
      //SendSysMessagePergon(who, "Fernkampf");
      //Keine Sonderbehandlung für Fernkampf
      //endif

      //Waffenquali gegen Skill
      Var skillW:=GetSkillFloatPergon(who, weapon.attribute);
      If (skillW > 0)
        Var quality_vorgabe:=100.0*0.5*(skillW/130.0); //!!!die Vorgabe von 50% muss noch balancet werden
        Var faktor_waffe:=Min(1, quality_weapon/quality_vorgabe);
        If (speak==2)
          If (!collecttext)
            SendSysMessagePergon(who_talk, "Waffenvorgabe ("+skillW+") faktor_waffe="+faktor_waffe+" Vorgabe="+quality_vorgabe);
          Else
            collectforhtml.append("Waffenvorgabe ("+skillW+") faktor_waffe="+faktor_waffe+" Vorgabe="+quality_vorgabe);
          EndIf
        EndIf
        If ((faktor_waffe < 1) && (speak > 0))
          If (!collecttext)
            SendSysMessagePergon(who_talk, "Diese Waffe von minderer Qualitaet wird euren Faehigkeiten nicht gerecht. Es gelingt euch so nur schwer eure Faehigkeiten zu verbessern.");
          Else
            collectforhtml.append("Diese Waffe von minderer Qualitaet wird euren Faehigkeiten nicht gerecht. Es gelingt euch so nur schwer eure Faehigkeiten zu verbessern.");
          EndIf
        EndIf
        faktor*=faktor_waffe;
        If (speak == 2)
          If (!collecttext)
            SendSysMessagePergon(who_talk, "Waffenskill Regel ergibt Faktor="+faktor_waffe);
          Else
            collectforhtml.append("Waffenskill Regel ergibt Faktor="+faktor_waffe);
          EndIf
        EndIf
      Else
        If (speak==2)
          If (!collecttext)
            SendSysMessagePergon(who_talk, "Waffenvorgabe ("+skillW+")");
          Else
            collectforhtml.append("Waffenvorgabe ("+skillW+")");
          EndIf
        EndIf
      EndIf
      //Waffenquali gegen Rüstungsquali
      Var faktor_equip:=1;
      If (quality_armor == 0)
        faktor_equip:=1;
      Else
        faktor_equip:=Min(1, (quality_weapon+10)/quality_armor); //10% Bonus
      EndIf
      If ((faktor_equip < 1) && (speak > 0))
        If (!collecttext)
          SendSysMessagePergon(who_talk, "Gemessen an eurer Rueste ist eure Waffe ungeeignet eure Faehigkeiten zu verbessern.");
        Else
          collectforhtml.append("Gemessen an eurer Rueste ist eure Waffe ungeeignet eure Faehigkeiten zu verbessern.");
        EndIf
      EndIf
      faktor*=faktor_equip;
      If (speak == 2)
        If (!collecttext)
          SendSysMessagePergon(who_talk, "Equipment Regel ergibt Faktor="+faktor_equip);
        Else
          collectforhtml.append("Equipment Regel ergibt Faktor="+faktor_equip);
        EndIf
      EndIf
    EndIf
  Else
    LogToFile("::log/balancing.log", "Error [balancing.inc]: armrzone.cfg kann nicht gelesen werden. (who.serial="+who.serial+")");
  EndIf

  Return(faktor);
EndFunction

Function CalcFaktorSonderregeln(who, enemy, skillID, weapon, speak, who_talk, collecttext := 0,byref collectforhtml:=0)
//extraspeak..muss 1 sein für Sprüche die für jeden Skill gleich kommen würden wie Faustkampf
  Var faktor:=1;
  //Taktik und Anatomyverweigerer abstrafen
  Var faktor_skill:=1;
  Var skillT:=GetSkillFloatPergon(who, SKILLID_TAKTIK);
  Var skillA:=GetSkillFloatPergon(who, SKILLID_ANATOMIE);

  //ab 80 müssen Taktik und Anatomy mind auf 60 sein
  If ((skillT >= 60)&&(skillA >= 60))
    //ok
    faktor_skill:=1.0;
  Else
    If (skillID)
      Var skill:=GetSkillFloatPergon(who, skillID);
      If (skill > 80.0)
        //nicht erlaubt
        If (speak)
          If (!collecttext)
            SendSysMessagePergon(who_talk, "Taktik oder Anatomy nicht >= 60%, kein Skillanstieg gewaehrt fuer "+skillID+".");
          Else
            collectforhtml.append("Taktik oder Anatomy nicht >= 60%, kein Skillanstieg gewaehrt fuer "+skillID+".");
          EndIf
        EndIf
        faktor_skill:=0.0;
      EndIf
    Else
      //kein Skill übergeben allgemein Anwenden
      //if (speak) <- macht Probleme bei .status bei der Anzeige der Ergebnisse der Qualityregeln
      //  SendSysMessagePergon(who_talk, "Taktik oder Anatomy nicht >= 60%, kein Skillanstieg gewaehrt fuer Kampfskills >80%.");
      //endif
      faktor_skill:=0.0;
    EndIf
  EndIf

  If (speak == 2)
    If (!collecttext)
      SendSysMessagePergon(who_talk, "Min-Skill-Regel ergibt Faktor="+faktor_skill);
    Else
      collectforhtml.append("Min-Skill-Regel ergibt Faktor="+faktor_skill);
    EndIf
  EndIf
  faktor*=faktor_skill;

  //Prüfen ob mit Fernkampfwaffe im Nahkampf geskillt wird
  Var faktor_far:=1.0;
  If (weapon && enemy)
    Var weapon_type:=GetObjProperty(weapon, "Quality_Typ");
    If (weapon_type=="CARPENTRY_B")
      //Da sind nur Bögen und Armbrüste bei
      If ( ( abs(who.x-enemy.x) > 0 ) && ( abs(who.y-enemy.y) > 0 ) )
        faktor_far:=1.0;
      Else
        faktor_far:=0.0;
        If (speak)
          If (!collecttext)
            SendSysMessagePergon(who_talk, "Mit einer Fernkampfwaffe werdet ihr im Nahkampf nichts lernen.");
          Else
            collectforhtml.append("Mit einer Fernkampfwaffe werdet ihr im Nahkampf nichts lernen.");
          EndIf
        EndIf
      EndIf
    EndIf
    If (speak == 2)
      If (!collecttext)
        SendSysMessagePergon(who_talk, "Min-Far-Dist-Regel ergibt Faktor="+faktor_far);
      Else
        collectforhtml.append("Min-Far-Dist-Regel ergibt Faktor="+faktor_far);
      EndIf
    EndIf
  EndIf
  faktor*=faktor_far;

  //Faustkampf, checken ob Skill ausgemaxt ist, dann soll man keine anderen Kampfskills effektiv trainieren können
  //!!! Sinn das auf Faustkampf zu beschränken ??
//  if (!(weapon))
//    Var faktor_faust:=1;
//    Var rawvalue:=GetRawSkillPergon(who, SKILLID_FAUSTKAMPF);
//    Case (GetObjProperty(who, SKILLTYPKENNUNG+SKILLID_FAUSTKAMPF))
//      SKILLTYP_BERUF:   If (rawvalue==SKILLLIMITRAW_BERUF)
//                          faktor_faust:=1;
//                        EndIf
//      SKILLTYP_HAUPT:   If (rawvalue==SKILLLIMITRAW_HAUPT)
//                          faktor_faust:=1;
//                        EndIf
//      SKILLTYP_NEBEN:   If (rawvalue==SKILLLIMITRAW_NEBEN)
//                          faktor_faust:=0.1;
//                        EndIf
//      SKILLTYP_WEITERE: If (rawvalue==SKILLLIMITRAW_WEITERE)
//                          faktor_faust:=0.1;
//                        EndIf
//      default:          faktor_faust:=0;
//    EndCase
//    faktor:=faktor*faktor_faust;
//    if ((faktor_faust < 1) && (speak > 0) && (extraspeak))
//      SendSysMessagePergon(who_talk, "Unbewaffnet wird es euch schwerfallen eure Faehigkeiten zu verbessern.");
//    endif
//    if (speak == 2)
//      SendSysMessagePergon(who_talk, "Faustkampf Regel ergibt Faktor="+faktor_faust);
//    endif
//  endif

  Return(faktor);
EndFunction

Function GetReferenceQuality(item)
  /*MAX_QUAL_TAILORING_LM   's',Lederei,"beschlagene"       klamotten.cfg
  MAX_QUAL_TAILORING_LK   'b',Lederei,"Knochen"           klamotten.cfg
  MAX_QUAL_TAILORING_LS   's',Lederei,"verzierte"         klamotten.cfg
  MAX_QUAL_TAILORING_L    's',Lederei,"Leder"             klamotten.cfg
  MAX_QUAL_TAILORING_S    's',Tailoring                   klamotten.cfg
  MAX_QUAL_CARPENTRY_H    'c',Carpentry,"aus..Holz"       staebe.cfg
  MAX_QUAL_CARPENTRY_HM   'c',Carpentry,"verstaerkter"  staebe.cfg
  MAX_QUAL_CARPENTRY_B    'c',Carpentry,"Bogen, Armbrust"boegen.cfg
  MAX_QUAL_BLACKSMITH_M  'b',Waffenschmied
  MAX_QUAL_TINKERING_M_MH   't',Werkzeugschmied

  klamotten.cfg:
  materialleder     (Knochen)
  materialknochen   (Knochen)
  materialschaedel  (Knochen)
  material1obj 0x1081 (beschlagen)
  material2obj 0x6300 (beschlagen)
  materialleder       (verzierte)
  materialschuppen    (verzierte)
  material1obj 0x8025 (Leder)

  staebe.cfg:
  Material            (Holz)
  material2obj 0x6300 (verstaerkt)

  boegen.cfg:
  implizit Holz + Sehne*/

  Var item_type:=GetObjProperty(item, "Quality_Typ");
  If (item_type)
    //Das Item wurde schon einmal klassifiziert
  Else
    //Das Item anhand verfügbarer Quellen einem Quality Typ zuordnen
    Var item_hersteller:=GetPropertyItem(item);
    Var type:=item_hersteller.type;
    Var skillid:=item_hersteller.skillid;
    //LogToFile("::log/balancing.log", "balancing.inc: item_hersteller="+item_hersteller);
    If ((type=="b") and (skillid=="WaffenSchmied"))
      item_type:="BLACKSMITH_M";
    ElseIf ((type=="t") and (skillid=="WerkzeugSchmied"))
      item_type:="TINKERING_M_MH";
    ElseIf ((type=="s") and (skillid=="Tailoring"))
      item_type:="TAILORING_S";
    ElseIf ((type=="c") and (skillid=="Carpentry"))
      //Anhand Configfile Einträgen Typisieren
      Var cfg := readconfigfile(":carpentry:staebe");
      Var cfg2:= readconfigfile(":carpentry:boegen");
      Var entry := cfg[item.objtype];
      Var entry2:= cfg2[item.objtype];
      If (entry)
        If (entry.material2obj==0x6300)
          item_type:="CARPENTRY_HM";
        Else
          item_type:="CARPENTRY_H";
        EndIf
      ElseIf (entry2)
        item_type:="CARPENTRY_B";
      Else
        LogToFile("::log/balancing.log", "balancing.inc: Carpenty Item ohne Configfileeintrag. (name="+item.name+")");
      EndIf
    ElseIf ((type=="s") and (skillid=="Lederei"))
      //Anhand Configfile Einträgen Typisieren
      Var cfg := readconfigfile(":tailoring:klamotten");
      Var entry;
      ForEach cfg_key in GetConfigStringKeys( cfg )
        Var cfg_item:=cfg[cfg_key];
        If (CInt(cfg_item.Objtype)==CInt(item.objtype))
          entry:=cfg_item;
          Break;
        EndIf
        sleepms(2);
      EndForEach
      If (entry)
        If (entry.material1obj==0x8025) //Leder
          item_type:="TAILORING_L";
        ElseIf ((entry.material1obj==0x1081)&&(entry.material2obj==0x6300))
          item_type:="TAILORING_LM";
        ElseIf ((entry.materialleder)&&(entry.materialschuppen))
          item_type:="TAILORING_LS";
        ElseIf ((entry.materialleder)&&((entry.materialknochen)||(entry.materialschaedel))) //Knochenhalskrause
          //LogToFile("log/balancing.log", "balancing.inc: Knochen+Leder Item ohne Typ 'b'. (name="+item.name+")");
          item_type:="TAILORING_LK";
        Else
          LogToFile("::log/balancing.log", "balancing.inc: Tailoring Item nicht identifizierbar. (name="+item.name+")");
        EndIf
      Else
        LogToFile("::log/balancing.log", "balancing.inc: Tailoring Item ohne Configfileeintrag. (name="+item.name+", "+item.objtype+")");
      EndIf
    ElseIf ((type=="b") and (skillid=="Lederei"))
      item_type:="TAILORING_LK";
    Else
      LogToFile("::log/balancing.log", "balancing.inc: Unklassifizierbares Item. (name="+item.name+")");
    EndIf
    If (item_type)
      SetObjProperty(item, "Quality_Typ",item_type);
    EndIf
  EndIf
  //MAX_QUAL Werte zurück geben
  If (item_type=="TINKERING_M_MH")
    Return(MAX_QUAL_TINKERING_M_MH);
  ElseIf (item_type=="BLACKSMITH_M")
    Return(MAX_QUAL_BLACKSMITH_M);
  ElseIf (item_type=="CARPENTRY_B")
    Return(MAX_QUAL_CARPENTRY_B);
  ElseIf (item_type=="CARPENTRY_HM")
    Return(MAX_QUAL_CARPENTRY_HM);
  ElseIf (item_type=="CARPENTRY_H")
    Return(MAX_QUAL_CARPENTRY_H);
  ElseIf (item_type=="TAILORING_LK")
    Return(MAX_QUAL_TAILORING_LK);
  ElseIf (item_type=="TAILORING_LS")
    Return(MAX_QUAL_TAILORING_LS);
  ElseIf (item_type=="TAILORING_LM")
    Return(MAX_QUAL_TAILORING_LM);
  ElseIf (item_type=="TAILORING_L")
    Return(MAX_QUAL_TAILORING_L);
  ElseIf (item_type=="TAILORING_S")
    Return(MAX_QUAL_TAILORING_S);
  Else
    Return(0);
  EndIf
EndFunction

//Die einzelnen Handwerker mit einer Maxfunktion versiegeln
