///////////////////////////////////////////////////////////////////////////
// 2004-02-26 Mai Funktionen fuer alle Tiere
//      Pathfinding:
//      Position des Gegners wird bestimmt (N,NO,...)
//      da der direkte Weg versperrt ist, wird der naechstmoegliche Weg probiert
//      bei Erfolg wird ein Schritt weitergegangen
//      bei Misserfolg wird die naechste Richtung probiert
// 2006-01-05 Fox: KI Neustricken mit PathFind Funktion vom Core
// 2007-06-15 Fox: Einen Wert einführen der über Erfolg Misserfolg einer Laufaktion informiert.
///////////////////////////////////////////////////////////////////////////

Enum Nature
  NATURE_KILLERNEAR   :=1, //greift an, kaempft bis zum schluss, einzeln
  NATURE_KILLERFAR,        //greift aus der ferne an (schuetze), einzeln
  NATURE_PACK,             //rudeltier, kämpft im rudel
  NATURE_HERD,             //herdentier, fluechtet immer
  NATURE_MAGEATTACK,       //kampfmagier
  NATURE_ELEMENTUSER,      //z.b. drache (kann nicht wirklich zaubern, nur feuerspeien)
  NATURE_TOWNGUARD,
  NATURE_MAGEHELP,         //magier, versucht vornehmlich andere monster zu heilen
  NATURE_IMMOBILE,         //Unbeweglicher Nahkämpfer
  NATURE_SHAPESHIFTER,     //Wechselt Grafik ab %hp und Castet abhängig davon
  NATURE_MEEK,             //Fluchttier, einzeln
  NATURE_KILLANYNEAR,      //Nahkämpfer der alles angreift
  NATURE_SLIME,            //Nahkämpfer der unteranderem Rüste beschädigt

  NATURE_QUEST_SCHMUGGLER, //passiver Charakter der sich Selbstverteidigt
  NATURE_QUEST_BUERGER,    //passiver Charakter der um Hilfe ruft bei Bedrohung
  NATURE_QUEST_ESKORT,     //passiver Charakter der jemandem folgen kann
  NATURE_QUEST_MAGE        //passiver Charakter der Zauber zur Selbstverteidigung
  
EndEnum

// ToBeDone bzw will man die wirklich haben?
//CONST NATURE_LONER        :="loner";       //einzelgaenger, greift nicht an, kaempft nicht bis zum schluss
//CONST NATURE_CLAN         :="clan";    //clan, arbeitet zusammen, hat verschiedene positionen zu besetzen

CONST PROP_OPPONENT := "opponent";

CONST DEFELEMENTDAMAGE := 30;
CONST DEFELEMENTFREQUENCY := 30; //chance von 0.03
CONST DEFELEMENTAMOUNT := 3;

CONST DEFCASTDELAY      :=10;
CONST DEFHERDSIZE       := 5;    //default-herd-size
CONST DEFIDLESPEED  := 30;   //default-idle-speed
CONST DEFIDLESTEPS := 0;
CONST DEFAILEVEL := 1; //1 Minimal KI
CONST DEFGROUPDIST  := 2;
CONST RANGE_DEFSENSE    := 5;    //in diesem radius jemanden bemerken
CONST RANGE_DEFFOLLOW   := 25;   //bis zu diesem radius ueberhaupt kaempfen (verfolgen)
CONST RANGE_LEADER  := 2;    //um diesen Betrag sieht das Leittier weiter voraus

CONST FLEETIME       := 2;
CONST PANICTIME      := 4;

CONST MAXMOBILESINRANGE := 18; //20 weil die eventqueue 20 eintraege gross ist... -> obsolet

CONST MAXBLOCKEDTIME := 1;
CONST PROP_BLOCKED   := "#blocked";

CONST MAXTRAPPEDTIME := 5;
CONST PROP_TRAPPED   := "#trapped";

CONST MAXPATHSTEPS   := 6;
CONST MINPATHSTEPS   := 3;
CONST PROP_PATHFIND  := "#pathfind"; //Informationen für Pathfinding (nicht schön das einige Elementen verschiedene Bedeutungen zukommen)
          //.+path          Modul1: Pfad der letzten Core Planung
          //                Modul3: gibt die letzte Laufrichtung an (Bereich 1 bis 8)
          //.+typ           Modul das beim nächsten Schritt zum Einsatz kommt
          //.+steps         Modul1: Schritte auf Pfad
          //                Modul2: Schritte für Mai Algorithmus
          //                Modul3: sind die verbleibenden Gesamtschritte
          //.+uzs           Modul3: die Uhrzeiggerrichtung in der gesucht wird
          //.+MobDistStart  Modul3: Abstand zum Mob beim Start von Weg2 Modus
          //.+ZielPose      Modul3: Mobposition bei Start (face im Bereich 1 bis 8)
          //.+Typ1Test      Modul3: verbleibende Schritte bis wieder ein Typ 1 Test gemacht wird
          //.+count          Modul2: Zähler der erfolgreichen Schritte


CONST PROP_GUARD         := "#guard"; //zahme tiere im guard-modus beschuetzen ihren Meister vor all Unbill und Gefahr
CONST PROP_FOLLOWMASTER  := "#followmaster";  //zahme tiere folgen ihrem meister
CONST PROP_STAY          := "#stay";  //zahme tiere kann man zum bleiben/stillstehen auffordern
CONST PROP_MASTERDEAD    := "#masterdead";  //der Meister ist gestorben und noch nicht wieder beim Tier vorbeigekommen
CONST PROP_MASTER        := "master"; //die Serialnr des Meisters

CONST PROP_FOLLOW   := "#followleader"; //jemandem folgen für Group und Guard (ist eine Serial)
CONST FIGHT_INFO    := "#fight_info"; //Informationen für Guard zur Fortsetzung des Kampfes
CONST WALKBACK      := "#walkback"; //Informationen für Guard zur Rückkehr zum Startpunkt nach Kampf oder Follow
CONST IGNORE_LIST   := "#ignore_list"; //Liste von Mobs die beim Lookaround ignoriert werden, weil eh nicht erreichbar


CONST MAXPATHSTEPSKOMPLEX := 50; //maximale Schrittzahl für umgehen komplexer Hindernisse
CONST ENEMYCHANGE := 5; //Chance in Prozent auf ein Lookaround Ereignis

Enum ParamsStruct
  AILEVEL         :="ai",   //gestaffelte Intelligenz beim Wege suchen
  RANGESENSE      :="r_s",  //ab diesem radius bemerkt das tier jemanden
  RANGEFOLLOW     :="r_f",  //bis dahin verfolgt es jemanden / fluechtet es vor jemandem
  IDLESPEED       :="i_s",  //wahrscheinlichkeit mit der es sich fortbewegt bei langeweile
  IDLESTEPS       :="i_st", //Schritte die ein NPC nach Aktion mit idlespeed wandert (wenn alle NPC's ständig wandern summiert sich ordentlich Last zusammen)
  IDLESOUND       :="s_i",  //Soundeffekt fürs nichtstun
  ANGERSOUND      :="s_a",  //Feind entdeckt

  GROUP           :="g",    //gruppe von npcs zu denen es gehoert
  GROUPSIZE       :="g_s",  //herd- bzw packsize
  GROUPDIST       :="g_d",  //abstand den herdentiere vom leittier und untereinander einhalten
  RANGESENSELEADER:="r_s_l",//ab diesem radius bemerkt das leittier jemanden
  RANGESENSEORIG  :="r_s_o",//ab diesem radius bemerkt das tier jemanden (original)

  ELEMENT         :="e",    //element dass der elementuser benutzt
  ELEMENTDMGNEAR  :="e_d_n",//schaden den der elementuser mit seinem element anrichtet (nahkampf fernkampf)
  ELEMENTDMGFAR   :="e_d_f",//schaden den der elementuser mit seinem element anrichtet (nahkampf fernkampf)
  ELEMENTFRQNEAR  :="e_f_n",//wahrscheinlichkeit wie oft er mit dem element wird (nahkampf fernkampf)
  ELEMENTFRQFAR   :="e_f_f",//wahrscheinlichkeit wie oft er mit dem element wird (nahkampf fernkampf)
  ELEMENTAMOUNT   :="e_a",  //maximale anzahl an opfern die mit einer welle getroffen werden (fernkampf)
  ELEMENTSPECIAL  :="e_s",  //pushback-effekt, wand, ...

  STARTZONE       :="stzone",//Zonen Infos über den Platz wo der NPC gestartet wird

  SPELLCASTER     :="c",    // Kann er zauber sprechen?
  SPELLS          :="c_sp", // Was für welche?
  CASTDELAY       :="c_d",  // mindestzeit zwischen zwei Sprüchen
  FIRSTCAST       :="c_f",  // ist es das erstemal das er zaubert? (gegen diesen Gegner)
  SPELLREFLECT    :="c_r",  // Hat er den Reflectspell
  REFLECTCHANCE   :="c_rc", // Wie hoch ist die Wahrscheinlichkeit, den Spiegel zu erneuern (PingPong)
  SPELLHEAL       :="c_he", // Hat er nen Heilspruch wenn ja welchen
  SPELLCURE       :="c_cu", // Kann ich Gegengift
  SPELLDISPEL     :="c_di", // Kann ich Dispelfield
  CASTDISPEL      :="c_di_f",// Soll ich Dispelfield casten? [0,1,2] [nicht gecastet,casten,grad erst gecastet]
  CURRNATURE      :="curr", // Zusatzspeicherung der aktuellen Nature für Mageattack (wechsel zu killernear)

  WAKEUPTIME      :="t_w",  // Wann wach ich auf
  SLEEPTIME       :="t_s",  // Wann schlaf ich (verstecken)
  
  SHAPES          :="sh",   // Array aus Shapedefinitionen
  SHAPECOUNTER    :="sh_cnt"// Welche Shape ist aktuell
EndEnum

Enum MobType
  MOB_UNKNOWN:=0,
  MOB_HUMAN,
  MOB_SEA,
  MOB_ANIMAL,
  MOB_MONSTER
EndEnum


/*Begriffserklärung
  Freez - steht für Paralyse und Freez
  nicht erkennbares Hindernis - Hindernisse die die Core Lauffunktionen nicht erkennen und die den Schritt verhindern: Energiezaun
  Block - Schritt nicht machbar
  totaler Block - in keine Richtung mehr ein Schritt machbar
  Rückstoß - ein Schritt wird durch Rücksetzen anuliert, Energiezaun
  Freilauf - use_adjustments=1, Core Lauffunktion macht immer einen Schritt falls in eine beliebige Richtung einer machbar ist
  Richtlauf - use_adjustments=0, geht genau auf das genannte Ziel (Point oder Richtungsbestimmt)
  klopfen - gemeint ist ein Schritt in eine bestimmte Richtung um zu prüfen ob dort irgend ein Hinderniss ist
*/
/*Funktionsübersicht:
  DistanceEul(x1, y1, x2, y2) - Eulerdistanz
  min_diff_modulo(a, b) - kleinste Differenz zwischen 2 Werten auf Modulo 8

  EquipMe() - Ausrüstung anlegen

  --KI Hauptfunktionen--
  MainRunToward(byref mob, byref feedback) - zum bewegen auf ein Mobile zu
  MainRunAwayFrom(byref mob) - zum bewegen von einem Mobile weg
  MainGoToLOS(byref mob, byref localParams) - Umkreisen eines Mobiles um LOS zu gewinnen
  GoToMob(byref mob, mindist) - Einem Mobile folgen

  --KI Subfunktionen AILevel--
  -Level alle-
  WalkToLOS(byref mob, byref localParams) - Umkreisen eines Mobiles um LOS zu gewinnen
                                            Freilauf, totaler Block und Sleep sorgen für Sleep
  -Level 1-
  RunTowardStep(byref mob) - Core, Freilauf, totaler Block und Freez sorgen für Sleep
  RunAwayFromStep(byref mob) - Core, Freilauf, totaler Block und Freez sorgen für Sleep
  -Level 2-
  RunTowardAIStep(byref mob) - Erweitertes Coreverhalten für Zäune (Mai AI), Richtlauf, bei Freez Sleep
  -Level 3+4+5 -
  RunTowardPath(byref mob, byref searchskirt, byref feedback) - Komplex AI, bewältigt auch Komplexe Hindernisse, Richtlauf
                                                Modulsteuerung, bei Freez Sleep

  --KI Modulfunktionen--
  -Level 2+3-
  PathFind2(byref mob, pathSteps, dir) - Modul 0 (Level 2), Modul 2 (Level 3), in zufällige Richtung am Hindernis entlang laufen,
                                         Schrittzahl mit Versuchen steigend, Richtlauf, volle Hinderniserkennung
  -Level ab 3 -
  MacheWegDefault(byref mob, byref pathProp) - Modul 0, einen EinzelSchritt zum Gegner machen
  MacheWeg1(byref mob, byref pathProp, searchskirt) - Modul 1, einen Schritt auf mit Core PathFind gefunden Pfad machen
  MacheWeg2(byref mob, byref pathProp, searchskirt) - Modul 2, immer an der Wand lang Modus zum umgehen unklarer Hindernisse,
                                                      bei Totalblock Sleep
  MacheWeg3(byref mob, byref pathProp, searchskirt) - Modul 3, Fluchtmodus, nicht implementiert

  --KI Hilfsfunktionen
  DefaultRunToward3(byref mob) - Richtlauf, Core Schritt mit voller Hinderniserkennung
  DefaultRunToward4(byref mob) - Richtschritt, Schritt nach Core PathFind mit voller Hinderniserkennung (für den Schritt)
                                 PathFind berücksichtigt keine nicht erkennbaren Hindernisse
                                 für jeden Schritt wird ein neuer Pfad ermittelt
  MachePfadSchritt(byref mob, byref pathProp) - auf gegebener Schrittliste einen Schritt machen
  NoTargetPath(byref mob, byref pathProp) - prüft das Distanzkriterium bei dem ein Pfad verworfen wird
  PathFindWithCore(byref pathProp, zX, zY, zZ, searchskirt) - kapselt FindPath(Core)
  TryStepTowardLocation2(dir) - Schritt in Richtung Dir mit voller Hindernis- und Blockerkennung
  StepSleep(faktor, run) - macht Faktor viele Schrittpausen (Dex bzw Speedabhängig)
  CalcTimeSteps(time, run) - berechnet wieviele Schritte auf den Zeitverbrauch bezogen gemacht worden

  Runden(zahl) - auch gerundet muss mal werden
  ConvertDirectionToLocX(dir) - Facing in Koordinatenmod umwandeln (X-Koord)
  ConvertDirectionToLocY(dir) - Facing in Koordinatenmod umwandeln (Y-Koord)
    -> verschoben nach scripts/include/mobile.inc

  GetRandomTarget(dist) - Zufallsziel mit Dist Abstand
  VisualPath(path, typ) - visualisiert eine Schrittliste
  VisualPoint(x,y) - visualisiert einen Punkt
  VisualPoint2(x,y) - nur anderes Symbol
*/

/*Ablaufpläne
 --AILevel 3-- (so noch nicht implementiert)
 Modul 0 --mehrmals blockiert--> Modul 1
 Modul 1 --(NoTargetPath) or (Pfadende)--> Modul 0
         --(!PathFindWithCore) or (mehrmals geblockt)--> Modul 2
 Modul 2(Mai suchen)
         --(Schritt zum Ziel gelungen) or (MAXPATHSTEPS erreicht)--> Modul 0
 --AILevel 4+5
 Modul 0 (ab 5 mit PathFind(Core))
         --mehrmals blockiert--> Modul 1
 Modul 1 --(NoTargetPath) or (Pfadende)--> Modul 0
         --(!PathFindWithCore)--> Modul 2 (Mai suchen <- in Feindnähe bleiben)
         --(mehrmals massiv geblockt)--> Modul 1 neustart
         --(mehrmals Rückstoß)--> Modul 3 (Komplex suchen)
 Modul 2(Mai suchen)
         --(MAXPATHSTEPS erreicht)--> Modul 0
         --(totaler Block)-->Sleep
 Modul 3(Komplex suchen)
         --(MAXPATHSTEPSKOMPLEX Schritte gemacht) or (Distanz zum Mobile verringert) or (125° Kriterium)--> Modul 0
         -- (totaler Block) --> Sleep
         -- Goodwalk --> Modul 3, Klopfen aussetzen
         -- (MAXSTEPS_BIS_FLUCHT and (HP% < HP_PROZENT_FLUCHT) --> Modul 4 (nicht implementiert)
 Modul 4 -- ((auser Schussreichweite) or (HP% >HP_PROZENT_ENDE_FLUCHT)) --> Modul 0
*/

/*Notizen
- KI Reaktion auf extern initierte Bewegung? Rückstoßzauber, Rückstoß per Energiezaun drauf casten, Anziehen durch Charybdis
- unterbrechen Events die Modulare Pfadsuche ?
- main_ai2 killerfar greift unconcealten commandlevel an
- rausbekommen warum Viechter ohne Waffen die falsche Animation machen
- FindPath2 muss sich nicht unbedingt zurücksetzen bei nem Rückstoß, abwägen gegen Testkosten
- Questspeak einbauen
- bei Modul2 ne gute Chance auf Gegnerwechsel machen
*/

include "include/mobile";

Function FindOpponentDirection(byref opponent)
 var dir:=GetFacing(Me.x, Me.y, opponent.x, opponent.y);
 If (dir.errortext)
   Return(0);
 EndIf
 Return(dir);
EndFunction


Function TryStepTowardLocation(byref dir)
  Var oldx := Me.x;
  Var oldy := Me.y;
  Var xmod:=ConvertDirectionToLocX(dir);
  Var ymod:=ConvertDirectionToLocY(dir);
  //Var z;
  //das dir+1 im runtoward hat seinen sinn :)
  If( CanMove( dir ) != 0)
    If (RunTowardLocation( (Me.x + xmod), (Me.y + ymod)))
      If ((oldx == Me.x) && (oldy == Me.y))
        RunTowardLocation( (Me.x + xmod), (Me.y + ymod));
        SleepMs(10); //sleep ist schon im runtoward drin
      EndIf
    EndIf
  EndIf

  If( (oldx == Me.x) && (oldy == Me.y) )
    Return(0);
  Else
    Return(1);
  EndIf

EndFunction


Function TryStepTowardLocationWalk(byref dir)
  Var oldx := Me.x;
  Var oldy := Me.y;
  Var xmod:=ConvertDirectionToLocX(dir);
  Var ymod:=ConvertDirectionToLocY(dir);

  If( CanMove( dir ) != 0)
    If (WalkTowardLocation( (Me.x + xmod), (Me.y + ymod)))
      If ((oldx == Me.x) && (oldy == Me.y))
        WalkTowardLocation( (Me.x + xmod), (Me.y + ymod));
        SleepMs(10); //sleep ist schon im runtoward drin
      EndIf
    EndIf
  EndIf

  If( (oldx == Me.x) && (oldy == Me.y) )
    Return(0);
  Else
    Return(1);
  EndIf

EndFunction


Function PathFind( byref opponent, byref steps )
  Var i:=1;
  Var a;
  Var newdir;

  //abschalten, damit das tier aehnlich wie bei "Move()" auch wirklich in die gewuenschte richtung zu gehen versucht
  me.use_adjustments:=0; //Use movement adjustments to avoid single-tile obsticals

  If(RandomInt(2))
    for(a:=0;a<steps;a+=1)
      newdir := FindOpponentDirection( opponent );
      While( i<=7 )
        If (TryStepTowardLocation((newdir+i)&7))
          Break;
        Else
          i+=1;
        EndIf
      EndWhile
      i:=1;
    endfor
  Else
    for(a:=0;a<steps;a+=1)
      newdir := FindOpponentDirection( opponent );
      while( i<=7 )
        If (TryStepTowardLocation((newdir-i)&7))
          Break;
        Else
          i+=1;
        EndIf
      endwhile
      i:=1;
    endfor
  EndIf

  me.use_adjustments:=1;

EndFunction

Function PathFindLOS( byref opponent, byref steps )
  //nur solange pathfinden bis die LineOfSight da ist
  Var i:=1;
  Var a;
  Var newdir;

  //abschalten, damit das tier aehnlich wie bei "Move()" auch wirklich in die gewuenschte richtung zu gehen versucht
  me.use_adjustments:=0; //Use movement adjustments to avoid single-tile obsticals

  If(RandomInt(2))
    for(a:=0;a<steps;a+=1)
      If(!CheckLineOfSight(Me,opponent))
        newdir := FindOpponentDirection( opponent );
        while( i<=7 )
          If (TryStepTowardLocation((newdir+i)&7))
            Break;
          Else
            i+=1;
          EndIf
        endwhile
        i:=1;
      EndIf
    endfor
  Else
    for(a:=0;a<steps;a+=1)
      If(!CheckLineOfSight(Me,opponent))
        newdir := FindOpponentDirection( opponent );
        while( i<=7 )
          If (TryStepTowardLocation((newdir-i)&7))
            Break;
          Else
            i+=1;
          EndIf
        endwhile
        i:=1;
      EndIf
    endfor
  EndIf

  me.use_adjustments:=1;

EndFunction


Function PathFindFlee(byref  opponent, byref steps )
  Var pos;
  Var i:=1;
  Var a;

  //wegrennen vom gegner
  //abschalten, damit das tier aehnlich wie bei "Move()" auch wirklich in die gewuenschte richtung zu gehen versucht
  me.use_adjustments:=0; //Use movement adjustments to avoid single-tile obsticals


  If(RandomInt(2))
    for(a:=0;a<steps;a+=1)
      pos := FindOpponentDirection( opponent );
      while( i<=7 )
        If (TryStepTowardLocation((pos+i+4)&7)) //+4 bewirkt dass in die entgegengesetzte richtung gegangen wird
          Break;
        Else
          i+=1;
        EndIf
      endwhile
      i:=1;
    endfor
  Else
    for(a:=0;a<steps;a+=1)
      pos := FindOpponentDirection( opponent );
      while( i<=7 )
        If (TryStepTowardLocation((pos-i+4)&7))
          Break;
        Else
          i+=1;
        EndIf
      endwhile
      i:=1;
    endfor
  EndIf

  me.use_adjustments:=1;

EndFunction

Function PathfindToward(byref mob,byref xold,byref yold)
  //der mobile kann der feind, aber im falle eines herdentieres auch der leader sein
  //Wenn MAXBLOCKEDTIME hintereinander blockiert mit PathFind laufen
  //beim ersten mal MINPATHSTEPS hintereinander laufe damit
  //bei jedem weiteren blockiert sein in Mob Richtung 1 STEP mehr Schritte am Stück mit PathFind laufen
  CheckBlocked(xold,yold);

  If( GetObjProperty(Me,PROP_BLOCKED) > MAXBLOCKEDTIME )
    Var path := GetObjProperty(ME,PROP_PATHFIND);
    If( (!path) || (path >= MAXPATHSTEPS) )
      SetObjProperty(Me,PROP_PATHFIND,MINPATHSTEPS);
      PathFind(mob,MINPATHSTEPS);
    Else
      SetObjProperty(Me,PROP_PATHFIND,path+1);
      PathFind(mob,path+1);
    EndIf
    EraseObjProperty(Me,PROP_BLOCKED);
  EndIf

EndFunction

Function PathfindTowardLOS(byref mob,byref xold,byref yold)
  //fuer killerfar, die den feind nur sehen, nicht aber erreichen wollen

  /*  If( (xold == Me.x) || (xold == Me.y) )
  Var block := CInt(GetObjProperty(Me,PROP_BLOCKED));
  SetObjProperty(Me, PROP_BLOCKED, block+1 );
  Else
  EraseObjProperty(Me,PROP_BLOCKED);
  EndIf
  */
  CheckBlocked(xold,yold);

  If( GetObjProperty(Me,PROP_BLOCKED) > MAXBLOCKEDTIME )
    Var path := GetObjProperty(ME,PROP_PATHFIND);
    If( (!path) || (path >= MAXPATHSTEPS) )
      SetObjProperty(Me,PROP_PATHFIND,MINPATHSTEPS);
      PathFindLOS(mob,MINPATHSTEPS);
    Else
      SetObjProperty(Me,PROP_PATHFIND,path+1);
      PathFindLOS(mob,path+1);
    EndIf
    EraseObjProperty(Me,PROP_BLOCKED);
  EndIf

EndFunction


Function PathfindAwayFrom(byref mob, byref xold, byref yold)
  //von jemandem weg pathfinden
  CheckTrapped(xold,yold);

  If( GetObjProperty(Me,PROP_TRAPPED) > MAXTRAPPEDTIME )
    Var path := GetObjProperty(ME,PROP_PATHFIND);
    If( (!path) || (path >= MAXPATHSTEPS) )
      SetObjProperty(Me,PROP_PATHFIND,MINPATHSTEPS);
      PathFindFlee(mob,MINPATHSTEPS);
    Else
      SetObjProperty(Me,PROP_PATHFIND,path+1);
      PathFindFlee(mob,path+1);
    EndIf
    EraseObjProperty(Me,PROP_TRAPPED);
  EndIf

EndFunction

Function AIRunToward(byref  character )
  me.use_adjustments:=0; //Use movement adjustments to avoid single-tile obsticals

  If( !TryStepTowardLocation(FindOpponentDirection( character )))
    //PathFind(character);
  EndIf
  me.use_adjustments:=1;
EndFunction

Function PlayTime(byref leader, byref xold, byref yold)
  //Herdentiere gehen mal kurz "spielen" wenn sie ausversehen den leader eingesperrt haben
  //ja toll und wo wird das gecheckt?
  xold:=xold;
  yold:=yold;
  SetObjProperty(Me, PROP_GROUPTRAPPED, CInt(GetObjProperty(Me,PROP_GROUPTRAPPED))+1 );

  If( (GetObjProperty(Me, PROP_GROUPTRAPPED) >= MAXGROUPTRAPPEDTIME) && (RandomInt(2)) ) //der leader ist eingesperrt
    Var pos := FindOpponentDirection( leader );
    Var i:=RandomInt(8); //damit nicht immer nur stur vom leader direkt weggerannt wird
    Var k;
    for(k:=0;k<3;k+=1)
      If (!TryStepTowardLocationWalk((pos+i)&7))
        StepSleep(1,0);
        break;
      EndIf
    endfor
    EraseObjProperty( Me, PROP_GROUPTRAPPED );
  EndIf
EndFunction

Function GetNearestMember(byref leader)

  Var members := GetObjProperty( leader, PROP_MEMBERS );
  Var dist:=0;
  Var distmin := 0;
  Var membermin;

  //alle Mitherdentiere durchgehen, kleinsten abstand zwischen member und leittier bestimmen, kleinsten abstand zwischen mir und member bestimmen
  foreach serial in members
    Var member := SystemFindObjectBySerial(serial);
    If( (member) && (serial != Me.serial))
      dist := Distance(Me,member);
      If( distmin )
        If(distmin > dist )
          distmin:=dist;
          membermin:=member;
        EndIf
      Else
        distmin:=dist;
        membermin:=member;
      EndIf
    EndIf
    SleepMs(10); //gegen Runaways
  endforeach

  Return(membermin);

EndFunction

Function AdjustEnterAreaRange(byref range,byref rangeorig)
  Var newrange:=range;

  //mal 3 schritte ueber den aktuellen range mit hinausschauen
  Var mobs:=ListMobilesNearLocation(Me.x, Me.y, Me.z, (range+3),Me.realm);

  If( mobs.size() > MAXMOBILESINRANGE )
    //EnableEvents(SYSEVENT_ENTEREDAREA, max( range - 1, 1 ));
    newrange:=Max( range - 1, 1 );
  Else
    //EnableEvents(SYSEVENT_ENTEREDAREA, min( range + 1, rangeorig ));
    newrange:=Min( range + 1, rangeorig );
  EndIf

  Return(CInt(newrange));

EndFunction

Function EquipMe()
  //stattet Anhand der npcdesc.cfg Einträge mit zusätzlichem Equipment aus
  //Die Hauptausstattung läuft aber über die CProp Equipt
  //Hauptaufgabe ist den gewürfelten AR Wert für den NPC zu setzen über ar_mod

  Var mytemplate:=GetNPCConfig(Me.npctemplate);
  If( mytemplate.equip )
    EquipFromTemplate(me, mytemplate.equip);
  EndIf
  If( mytemplate.ammotype && mytemplate.ammoamount)
    If(!Me.backpack)
      If(!EquipItem(Me, CreateItemAtLocationPergon(Me.x, Me.y, Me.z, 0xe75, 1,Me.realm)))
        LogToFile("::log/ailog.log", "WARNUNG: "+Me.serial+" ["+Me.npctemplate+"] mag kein Backpack");
      EndIf
    EndIf
    CreateItemInBackpackPergon(Me, mytemplate.ammotype, mytemplate.ammoamount);
  EndIf
  If( mytemplate.missileweapon )
    EquipFromTemplate(Me, mytemplate.missileweapon);
  EndIf
  //die in der cfg stehende AR als ar_mod korrektur setzen, die durch eine equippte Rüste ignoriert wird
  If (mytemplate.ar)
    Me.ar_mod:=RandomDiceRoll(CStr(mytemplate.ar))-Me.ar;
  EndIf

EndFunction

Function CheckBlocked(byref xold, byref yold)

  //If( (xold == Me.x) || (yold == Me.y) )// || ((xold == Me.x -1) && (yold == Me.y+1)) || ((xold == Me.x +1) && (yold == Me.y+1)) || ((xold == Me.x+1) && (yold == Me.y-1))|| ((xold == Me.x-1) && (yold == Me.y-1)))
  If( (Abs(xold-Me.x)<=1) && (Abs(yold-Me.y)<=1))
    SetObjProperty(Me, PROP_BLOCKED, (CInt(GetObjProperty(Me,PROP_BLOCKED)+1) );
    Return(1);
  Else
    EraseObjProperty(Me,PROP_BLOCKED);
    Return(0);
  EndIf

EndFunction

Function CheckTrapped(byref xold, byref yold)

  If( (xold == Me.x) || (yold == Me.y) ) //|| ((xold == Me.x -1) && (yold == Me.y+1)) || ((xold == Me.x +1) && (yold == Me.y+1)) || ((xold == Me.x+1) && (yold == Me.y-1))|| ((xold == Me.x-1) && (yold == Me.y-1)))
    SetObjProperty(Me, PROP_TRAPPED, CInt(GetObjProperty(Me,PROP_TRAPPED))+1 );
    Return(1);
  Else
    EraseObjProperty(Me,PROP_TRAPPED);
    Return(0);
  EndIf

EndFunction

//--------------------------------------------------------------------------------------------
//Neue MainAI2 Funktionen

Function MainRunToward(byref mob, byref feedback:=struct)
  // ailevel      Abgestuftes KI Level in Bezug auf die PfadPlanung
  //              1 .. Minimal KI, arbeitet mit Core Funktion, umgeht nur massive Hinderniss
  //              2 .. Minimal KI, arbeitet mit AI Funktion, umgeht auch einfache Energiezäune
  //              3 .. Default latschen ohne FindPath(Core), Hindernisse umgehen mit FindPath(Core) (Abweichung 6)
  //              4 .. Default latschen mit FindPath(Core), Hindernisse umgehen mit FindPath(Core) (Abweichung 12)
  feedback.exist:=0; //1 wenn die KI Stufe ein Feedback über den Erfolg der Bewegung liefert
  feedback.plan:=0;   //1..es wurde kein Weg zum Ziel gefunden mit Core Path
  //2..es wurde ein Wege gefunden
  //3..ist gefreezt
  case (params[AILEVEL])
    1 : RunTowardStep(mob);
    2 : GoTowardAIStep(mob, 0);
    3 : RunTowardPath(mob, 20, feedback);
    4 : RunTowardPath(mob, 12, feedback);
    default: RunTowardStep(mob);
  endcase
EndFunction

Function MainWalkToward(byref mob)
  // ailevel      Abgestuftes KI Level in Bezug auf die PfadPlanung
  //              1 .. Minimal KI, arbeitet mit Core Funktion, umgeht nur massive Hinderniss
  //              2 .. Minimal KI, arbeitet mit AI Funktion, umgeht auch einfache Energiezäune
  //              3 .. Default latschen ohne FindPath(Core), Hindernisse umgehen mit FindPath(Core) (Abweichung 6)
  //              4 .. Default latschen mit FindPath(Core), Hindernisse umgehen mit FindPath(Core) (Abweichung 12)

  case (params[AILEVEL])
    1 : WalkTowardStep(mob);
    2 : GoTowardAIStep(mob, 1);
    default: WalkTowardStep(mob);
  endcase
EndFunction


Function MainRunAwayFrom(byref mob)
// ailevel      Abgestuftes KI Level in Bezug auf die PfadPlanung
//              1 .. Minimal KI, kann nur kleinsten Hindernissen ausweichen
//              2 .. Default latschen ohne FindPath(Core), Hindernisse umgehen mit FindPath(Core) (Abweichung 6)
//              3 .. Default latschen mit FindPath(Core), Hindernisse umgehen mit FindPath(Core) (Abweichung 12)
  If ((Me.paralyzed)||(Me.frozen))
    TestMessage("Bin paralysiert oder frozen.", 1);
    StepSleep(10, 1);
  Else
    case (params[AILEVEL])
      1 : RunAwayFromStep(mob);
      //2 :
      //3 :
      default: RunAwayFromStep(mob);
    endcase
  EndIf
EndFunction

Function MainGoToLOS(byref mob, byref localParams)
// ailevel      Abgestuftes KI Level in Bezug auf die PfadPlanung
//              default .. im Radius um den Gegner herumlaufen
  If ((Me.paralyzed)||(Me.frozen))
    TestMessage("Bin paralysiert oder frozen.", 1);
    StepSleep(10, 0);
  Else
    WalkToLOS(mob, localParams);
    //case (params.ailevel)
    //  default: RunToLOS(mob, localParams);
    //endcase
  EndIf
EndFunction

Function RunTowardStep(byref mob)
  If (!RunToward(mob))
    StepSleep(10, 1); //Freez oder komplett blockiert
  EndIf
EndFunction

Function WalkTowardStep(byref mob)
  If (!WalkToward(mob))
    StepSleep(10, 0); //Freez oder komplett blockiert
  EndIf
EndFunction

Function RunAwayFromStep(byref mob)
  If (!RunAwayFrom(mob))
    StepSleep(10, 1); //Freez oder komplett blockiert
  EndIf
EndFunction

Function GoTowardAIStep(byref mob, walk:=0)
  //der mobile kann der feind, aber im falle eines herdentieres auch der leader sein
  //Wenn MAXBLOCKEDTIME hintereinander blockiert mit PathFind laufen (verursacht die kurzen Pausen beim rennen)
  //beim ersten mal MINPATHSTEPS hintereinander laufe damit
  //bei jedem weiteren blockiert sein in Mob Richtung 1 STEP mehr Schritte am Stück mit PathFind laufen
  //"walk" nur bei einfachen Schritten sonst "Run"
  If ((Me.paralyzed)||(Me.frozen))
    TestMessage("Bin paralysiert oder frozen.", 1);
    StepSleep(10, (walk==0));
    EraseObjProperty(Me, PROP_BLOCKED);
    EraseObjProperty(Me, PROP_PATHFIND);
    Return;
  EndIf
  Var block:=0;
  Var go_erg;
  If (walk)
    go_erg:=DefaultGoToward3(mob, 1);
  Else
    go_erg:=DefaultGoToward3(mob, 0);
  EndIf
  If (go_erg)
    //SayPergon(Me, "*normal*");
    EraseObjProperty(Me, PROP_BLOCKED);
    EraseObjProperty(Me, PROP_PATHFIND);
  Else
    block := CInt(GetObjProperty(Me,PROP_BLOCKED));
    SetObjProperty(Me, PROP_BLOCKED, (block+1) );
  EndIf

  If( block > MAXBLOCKEDTIME )
    Var pathSteps := GetObjProperty(ME,PROP_PATHFIND);
    If( (!pathSteps) || (pathSteps >= MAXPATHSTEPS) )
      //SayPergon(Me, "PathFind2: "+MINPATHSTEPS);
      SetObjProperty(Me,PROP_PATHFIND,MINPATHSTEPS);
      PathFind2(mob,MINPATHSTEPS, GetFacing(Me.x,Me.y,mob.x,mob.y));
    Else
      //SayPergon(Me, "*more*");
      If (pathSteps < MAXPATHSTEPS)
        pathSteps+=1;
        SetObjProperty(Me,PROP_PATHFIND,pathSteps);
      EndIf
      //SayPergon(Me, "PathFind2: "+pathSteps);
      PathFind2(mob,pathSteps, GetFacing(Me.x,Me.y,mob.x,mob.y));
    EndIf
    EraseObjProperty(Me,PROP_BLOCKED); //nach jedem Findlauf erstmal wieder testen
  EndIf
EndFunction

Function PathFind2(byref mob, byref pathSteps, byref dir)
  //geht mehrere Schritte am Stück in eine Ausweichrichtung, Problem Abbruchbedingung wird nicht geprüft
  //mache Schritt in freie Richtung
  If (params[AILEVEL] >= 2)
    CheckForBlockingLooseItems();
  EndIf

  TestMessage("PathFind2: Ausweichschritt",4);
  Var res:=1;
  Var a;
  Var i;
  If(RandomInt(2))
    for(a:=0;a<pathSteps;a+=1) //pathSteps Schritte laufen
      for( i:=0; i<8; i+=1 ) //eine freie Richtung ermitteln für nächsten Schritt
        res := TryStepTowardLocation2((dir+i)&7);
        TestMessage("PathFind2: Ausweichtest res="+res,4);
        If(res)
          break;
        EndIf
      endfor
      If (!res)
        //blockiert, z.B. durch Freez
        StepSleep(10, 1);
      Else
        //prüfen ob Ziel erreicht
        If ((Distance( Me, mob ) < 2 )&&(abs(Me.z-mob.z) < 15))
          Return;
        EndIf
      EndIf
    endfor
  Else
    for(a:=0;a<pathSteps;a+=1)
      for( i:=0; i<8; i+=1 )
        res := TryStepTowardLocation2((dir-i)&7);
        TestMessage("PathFind2: Ausweichtest res="+res,4);
        If(res)
          break;
        EndIf
      endfor
      If (!res)
        //blockiert, z.B. durch Freez
        StepSleep(10, 1);
      Else
        //prüfen ob Ziel erreicht
        If ((Distance( Me, mob ) < 2 )&&(abs(Me.z-mob.z) < 15))
          Return;
        EndIf
      EndIf
    endfor
  EndIf
EndFunction


Function DefaultGoToward3(byref mob, walk:=0)
  //in Richtung Gegner laufen mit RunToward
  //eigene Pose vorm Schritt merken
  Var oldX:=Me.x;
  Var oldY:=Me.y;
  Var oldFace:=Me.facing;

  Me.use_adjustments:=0; //sonst probiert er ein bisschen rum und Blockierung ist nicht feststellbar
  Var go_erg;
  If (walk)
    go_erg:=WalkToward(mob);
  Else
    go_erg:=RunToward(mob);
  EndIf
  If (go_erg)
    If (oldFace!=Me.facing)
      //nur Richtung geändert
      If (walk)
        go_erg:=WalkToward(mob);
      Else
        go_erg:=RunToward(mob);
      EndIf
      If (!go_erg)
        TestMessage("Ungeplanter Schritt fehlgeschlagen 3.", 4);
        Me.use_adjustments:=1;
        Return 0;
      EndIf
    EndIf
    Me.use_adjustments:=1;
    If ((oldX!=Me.x)||(oldY!=Me.y)||(oldFace!=Me.facing))
      Return 1;
    Else
      TestMessage("Ungeplanter Schritt fehlgeschlagen 2.", 4);
      Return 0;
    EndIf
  Else
    TestMessage("Ungeplanter Schritt fehlgeschlagen 1.", 4);
    Me.use_adjustments:=1;
    Return 0;
  EndIf
EndFunction

Function DefaultRunToward4(byref mob)
  //ungeplant in Richtung Gegner laufen, mit FindPath
  Var goErg:=0;
  Var path:=FindPath( Me.x, Me.y, Me.z, mob.x, mob.y, mob.z, Me.realm, 1, 1); //sparsamstes mögliches Planen
  If (path)
    VisualPath(path, 0x1ebd);
    VisualPoint(path[1].x, path[1].y);
    If (RunTowardLocation(path[1].x, path[1].y)) //funzt nicht der wackelt dauernd hin und her
      goErg:=1;
    Else
      TestMessage("Ungeplanter Schritt fehlgeschlagen 1.", 4);
    EndIf
  Else
    TestMessage("Ungeplanter Schritt fehlgeschlagen 2.", 4);
  EndIf
  Return goErg;
EndFunction

Function MachePfadSchritt(byref mob, byref pathProp)
  //nur Aufrufen wenn auch ein Pfad exisitiert
  mob:=mob;
  //eigene Pose vorm Schritt merken
  Var oldX:=Me.x;
  Var oldY:=Me.y;
  Var oldFace:=Me.facing;

  Var zX:=pathProp.path[pathProp.steps].x;
  Var zY:=pathProp.path[pathProp.steps].y;
  VisualPoint(zX,zY);

  If (RunTowardLocation(zX, zY))
    If (oldFace!=Me.facing)
      //nur Blickrichtung geändert, dann gleich nochmal nen Schritt machen
      oldFace:=Me.facing;
      If (!RunTowardLocation(zX, zY))
        Return 0;
      EndIf
    EndIf
    If ((oldX!=Me.x)||(oldY!=Me.y)||(oldFace!=Me.facing))
      //hat sich bewegt
      Return 2;
    Else
      //Rückstoß
      Return 1;
    EndIf
  Else
    //geblockt
    Return 0;
  EndIf
  //sollte nie erreicht werden
  Return 0;
EndFunction

Function NoTargetPath(byref mob, byref pathProp)
  If (pathProp.path)
    Var dist:=CoordinateDistanceEuclidean(mob.x, mob.y, pathProp.path[pathProp.path.size()].x, pathProp.path[pathProp.path.size()].y);
    TestMessage("steps="+pathProp.steps+" dist="+dist+" max_step="+(pathProp.path.size()-dist), 1);
    If (pathProp.steps < (pathProp.path.size()-dist))
      TestMessage("NoTargetPath: Pfad behalten.", 1);
    Else
      TestMessage("NoTargetPath: Pfad verwerfen.", 1);
      Return 1;
    EndIf
  EndIf
  Return 0;
EndFunction

Function Modul0(byref mob, byref pathProp)
//einfachen Schritt mit RunToward
//pathProp wird weder hier noch in der Aufrufenden Funktion ausgewertet
  TestMessage("Modul 0", 2);
  Var bSchritt:=0;
  case (params[AILEVEL])
    5 : If (DefaultRunToward4(mob))
          bSchritt:=1;
        EndIf
    default:
        If (DefaultGoToward3(mob, 0))
          bSchritt:=1;
        EndIf
  endcase
  If (bSchritt)
    TestMessage("Weg Default Schritt erfolgreich.", 4);
    EraseObjProperty(Me, PROP_BLOCKED); //ein Erfolgreicher Schritt, Blocks vergessen
  Else
    TestMessage("Weg Default Schritt blockiert.", 4);
    Var blocks:=CInt(GetObjProperty(Me, PROP_BLOCKED))+1;
    If (blocks > 1) //Nach erster Blockierung auf Weg 1 schalten
      TestMessage("Weg Default Wechsel auf Weg 1.", 4);
      pathProp:=struct{steps,path,uzs,MobDistStart,ZielPose,Typ1Test,count};//resetten
      pathProp.path:=0;
      pathProp.typ:=1;
      SetObjProperty(Me, PROP_PATHFIND, pathProp); //auf Wegtyp 1 umsteigen
    Else
      SetObjProperty(Me, PROP_BLOCKED, blocks);
    EndIf
  EndIf
EndFunction

Function Modul1(byref mob, byref pathProp, byref searchskirt, byref feedback)
//Schritt auf Weg mit FindPath geplant
  TestMessage("Modul 1", 2);
  If ((!mob) || (!mob.connected)) //mob tod bzw nicht mehr da
    Return(0);
  EndIf
  Var bSchritt:=0;
  If (pathProp.path)
    If (NoTargetPath(mob, pathProp)) //prüfen ob der Weg noch zum Target führt
      TestMessage("Weg 1: Weg verwerfen.", 1);
      EraseObjProperty(Me, PROP_BLOCKED);
      EraseObjProperty(Me, PROP_PATHFIND);
      pathProp:=struct{steps,path,uzs,MobDistStart,ZielPose,Typ1Test,count};//resetten
      pathProp.typ:=0;
      //und gleich noch nen Schritt um keine Pause zu bekommen
      Modul0(mob, pathProp);
      Return;
    Else
      bSchritt:=1;
    EndIf
  Else
    TestMessage("Weg 1: Weg suchen.", 1);
    //SayPergon(Me, "*gruebel*");
    If (PathFindWithCore(pathProp, mob.x, mob.y, mob.z, searchskirt))
      bSchritt:=1;
      feedback.plan:=2;
    Else
      //Kein Weg zum Target gefunden
      TestMessage("Weg 1: Kein Weg gefunden.", 4);
      StepSleep(1, 1); //Schrittpause
      pathProp:=struct{steps,path,uzs,MobDistStart,ZielPose,Typ1Test,count};//resetten
      pathProp.path:=0;
      pathProp.typ:=2; //einfachen Mai Algorithmus zur Zielführung nutzen (ein bisschen Sinnlos bewegen, denn einen Weg gibt es laut Core ja defintiv nicht)
      SetObjProperty(Me, PROP_PATHFIND, pathProp);
      EraseObjProperty(Me, PROP_BLOCKED);
      feedback.plan:=1;
      Return;
    EndIf
  EndIf

  If (bSchritt)
    Var schrittart:=MachePfadSchritt(mob, pathProp);
    If (schrittart==1) //Rückstoß
      TestMessage("Modul 1 Schritt Rückstoß", 4);
      pathProp:=struct{steps,path,uzs,MobDistStart,ZielPose,Typ1Test,count};//resetten
      pathProp.path:=0;
      pathProp.steps:=0;
      pathProp.typ:=3; //immer an der Wand lang Algorithmus nehmen
      SetObjProperty(Me, PROP_PATHFIND, pathProp);
      EraseObjProperty(Me, PROP_BLOCKED);
    ElseIf (schrittart==2) //Schritt gemacht
      TestMessage("Modul 1 Schritt "+pathProp.steps+" von "+pathProp.path.size()+" erfolgt.", 4);
      pathProp.steps:=pathProp.steps+1;
      EraseObjProperty(Me, PROP_BLOCKED);
      If (pathProp.path.size() >= CInt(pathProp.steps))
        SetObjProperty(Me, PROP_PATHFIND, pathProp);
        Return 1;
      Else
        TestMessage("Pfad erfolgreich zuende.", 4);
        EraseObjProperty(Me, PROP_PATHFIND); //weiter mit Default Weg
        Return 1;
      EndIf
    Else //massiv geblockt
      TestMessage("Modul 1 Schritt geblockt", 4);
      Var blocks:=CInt(GetObjProperty(Me, PROP_BLOCKED)+1);
      If (blocks > 2) //Nach ditter Blockierung neuen Pfad berechnen
        pathProp:=struct{steps,path,uzs,MobDistStart,ZielPose,Typ1Test,count};//resetten
        pathProp.path:=0;
        pathProp.typ:=1;
        SetObjProperty(Me, PROP_PATHFIND, pathProp);
        EraseObjProperty(Me, PROP_BLOCKED);
      Else
        SetObjProperty(Me, PROP_BLOCKED, blocks);
      EndIf
    EndIf
  EndIf
EndFunction

Function Modul2(byref mob, byref pathProp)
//kapselt PathFind2 als Modul
//der mobile kann der feind, aber im falle eines herdentieres auch der leader sein
//Wenn MAXBLOCKEDTIME hintereinander blockiert mit PathFind laufen (verursacht die kurzen Pausen beim rennen)
//beim ersten mal MINPATHSTEPS hintereinander laufe damit
//bei jedem weiteren blockiert sein in Mob Richtung 1 STEP mehr Schritte am Stück mit PathFind laufen
  TestMessage("Modul 2", 2);
  If ((Me.paralyzed)||(Me.frozen))
    TestMessage("Bin paralysiert oder frozen.", 1);
    StepSleep(10, 1);
    EraseObjProperty(Me, PROP_BLOCKED);
    EraseObjProperty(Me, PROP_PATHFIND);
    Return;
  EndIf
  Var block:=0;
  If (DefaultGoToward3(mob, 0))
    //SayPergon(Me, "*normal*");
    EraseObjProperty(Me, PROP_BLOCKED);
    //wenn ein erfolgreicher Schritt nach einem Stück normalen Suchen kommt,
    //dann heist das nicht unbedingt der Weg ist jetzt frei, sondern es kann sein das man
    //sich einfach nur bei der Suche entfernt hat und jetzt ein paar Schritte frei hat bis
    //zum Hinderniss. Ergo nicht gleich auf Modul 0 wechseln sondern prüfen vorher.
    If (pathProp.count)
      pathProp.count:=pathProp.count+1;
      TestMessage("pathProp.count="+pathProp.count, 3);
      If (pathProp.count > pathProp.steps)
        //Wechsel zu Modul 0
        EraseObjProperty(Me, PROP_PATHFIND);
      Else
        SetObjProperty(Me, PROP_PATHFIND, pathProp);
      EndIf
    Else
      pathProp.count:=1;
      SetObjProperty(Me, PROP_PATHFIND, pathProp);
    EndIf
    Return;
  Else
    block := CInt(GetObjProperty(Me,PROP_BLOCKED));
    //TestMessage("Modul2: block="+(block), 1);
    SetObjProperty(Me, PROP_BLOCKED, (block+1) );
    pathProp.count:=0; //wird aber nur bei einer Suchrunde gespeichert
  EndIf

  If( block > MAXBLOCKEDTIME )
    TestMessage("Modul2: steps(1)="+(pathProp.steps), 3);
    If (!(pathProp.steps))
      //SayPergon(Me, "*gruebel*");
      pathProp.steps:=MINPATHSTEPS;
      SetObjProperty(Me, PROP_PATHFIND, pathProp);
      PathFind2(mob, pathProp.steps, GetFacing(Me.x,Me.y,mob.x,mob.y));
    ElseIf (pathProp.steps < MAXPATHSTEPS)
      //SayPergon(Me, "*more*");
      pathProp.steps:=pathProp.steps+1;
      SetObjProperty(Me, PROP_PATHFIND, pathProp);
      TestMessage("FightGuard: steps(2)="+(pathProp.steps), 3);
      PathFind2(mob,pathProp.steps, GetFacing(Me.x,Me.y,mob.x,mob.y));
    Else
      TestMessage("Modul2: MAXPATHSTEPS erreicht(2)", 3);
      //Wechsel zu Modul 0, wenn maximale Suchreichweite auch nichts gebracht hat
      EraseObjProperty(Me,PROP_BLOCKED); //nach jedem Findlauf erstmal wieder testen
      EraseObjProperty(Me, PROP_PATHFIND);
    EndIf
  EndIf
EndFunction

Function Modul3(byref mob, byref pathProp)
//Ausweichschritt mit initialer Zufallsuhrzeigerrichtung zum umgehen von Hindernissen,
//auch nicht sichtbaren und umlaufen von Gegnertrauben
//Immer an der Wand lang Algorithmus
//
//Verfahren: Beginn mit Richtung zum Mob in uzs Richtung einen machbaren Schritt zu finden
//1. Für jeden zweiten weiteren Schritte prüfen ob in 45° entgegen uzs der Weg frei ist, wenn ja diese
// zur neuen Suchrichtung setzen (Schritt ist dann schon gemacht)
//2. wenn nicht 1. prüfen ob Schrittrichtung frei, wenn nicht im uzs neue Suchen (45° Drehungen)
//
//Probleme:
//- Gegner bewegt sich, was nun <- Abbruch wenn Richtung zum Mob um 90° geändert
//   <- Gegner schützt sich mit inversem U <- was solls mit etwas Glück sucht er ja
//      in der Richtigen Richtung weiter beim nächsten mal
//- Gibt es ein positives Abbruchkriterium? <- Distanz zur Startzielposition verringert
//- Entweder man eiert um einen eingeschlossenen Gegner rum oder man umgeht Wände
//- wie sehr stören andere Herumwuselnde Mobiles?
//- Wenn die Wand weg ist fängt er an im Kreis zu laufen
//
//Zieldefinition:
//Die Richtung zum Gegner um mehr als 90° zu verändern
//Mittel an der Wand lang laufen bis er wieder ZielPose.face laufen kann und das solange laufen
//bis Ziel erreicht oder neues Hindernis auftaucht
//
//Todo:
//- Aufgeben wenn die Sichtweite überschritten wird, sonst hauen die in Höhlen ab auf nimmer wiedersehen
//  <- dann muss der Gegner nur genug Abstand zum Hindernis nehmen und schon hängt der NPC drin
//  <- es muss also einen Unterschied geben ob massive oder undetektierbare Hindernisse den Weg versperren,
//     doch wie Unterscheiden ob ein mobiles oder neues Hindernis den Weg versperrt hat
//  <- CanMove sollte das leisten
//- prüfen ob der Gegner über die Main_Rounds erhalten bleibt

//20 Ausweichschritte maximal
//5 Ausweichschritte werden als Erfolg gewertet

//pathProp.steps sind die verbleibenden Gesamtschritte
//pathProp.path gibt die letzte Laufrichtung an (Bereich 1 bis 8)
//pathProp.uzs die Uhrzeiggerrichtung in der gesucht wird
//pathProp.MobDistStart Abstand zum Mob beim Start von Weg2 Modus
//pathProp.ZielPose Mobposition bei Start (face im Bereich 1 bis 8)
//pathProp.Typ1Test verbleibende Schritte bis wieder ein Typ 1 Test gemacht wird

TestMessage("Modul 3", 2);
TestMessage("pathProp.path: "+pathProp.path, 1);
  If (!(pathProp.steps))
    If ((!mob) || (!mob.connected)) //mob tod bzw nicht mehr da
      Return(0);
    EndIf
    pathProp:=struct{steps,path,uzs,MobDistStart,ZielPose,Typ1Test,count};
    //Initialisierung
//    SayPergon(Me, "*schnaub*");
    TestMessage("Ausweichen Initialisieren", 1);
    pathProp.path:= GetFacing(Me.x,Me.y,mob.x,mob.y);
    TestMessage("GetFacing(Me.x,Me.y,mob.x,mob.y): "+pathProp.path,1);
    pathProp.steps:=MAXPATHSTEPSKOMPLEX; //bis x Schritte nach einem Ausweg suchen
    pathProp.uzs:=RandomInt(2); //im/gegen Uhrzeigersinn suchen
    pathProp.MobDistStart:=Ceil(DistanceEuclidean(Me, mob));
    pathProp.ZielPose:=struct{x,y,facing};
    pathProp.ZielPose.x:=mob.x;
    pathProp.ZielPose.y:=mob.y;
    pathProp.ZielPose.facing:=pathProp.path; //Initialenemyfacing
    pathProp.Typ1Test:=2; //bei 0 ist Test fällig
    //Sleep(5);
  EndIf

  Var i:=1;
  //abschalten, damit der NPC aehnlich wie bei "Move()" auch wirklich in die gewuenschte richtung zu gehen versucht
  me.use_adjustments:=0; //Use movement adjustments to avoid single-tile obsticals
  Var res:=0;

  //Var oldFace:=Me.facing;
  Var goStep:=0;

  If (pathProp.Typ1Test < 1)
    //90° entgegen aktueller Richtung, Ziel abklopfen
    //wenn dabei das Initialenemyfacing überschritten wird dann nur 45°
    TestMessage("Klopf Schritt Versuch.",4);
    Var newdir;
    If (pathProp.uzs)
      newdir:=(pathProp.path-1)&7;
      If (newdir==Me.facing)
        res := TryStepTowardLocation2(newdir);
      Else
        newdir:=(pathProp.path-2)&7;
        res := TryStepTowardLocation2(newdir);
      EndIf
      If (res)
        TestMessage("Klopf Schritt erfolgreich.",1);
        pathProp.path:=newdir; //neue Richtung festschreiben
      EndIf
    Else
      newdir:=(pathProp.path+1)&7;
      If (newdir==Me.facing)
        res := TryStepTowardLocation2(newdir);
      Else
        newdir:=(pathProp.path+2)&7;
        res := TryStepTowardLocation2(newdir);
      EndIf
      If (res)
        TestMessage("Klopf Schritt erfolgreich.",1);
        pathProp.path:=newdir; //neue Richtung festschreiben
      EndIf
    EndIf
    If (res)
      pathProp.Typ1Test:=1; //1 Ausweichschritt bis zum nächsten Klopfschritt
    Else
      pathProp.Typ1Test:=2; //2 Ausweichschritt bis zum nächsten Klopfschritt
    EndIf
  Else
    //mache Schritt in freie Richtung
    TestMessage("Ausweichschritt",4);
    If (pathProp.uzs)
      for( i:=1; i<8; i+=1 )
        res := TryStepTowardLocation2(pathProp.path&7);
        TestMessage("Ausweichtest res="+res,4);
        If(res)
          pathProp.steps:=pathProp.steps-1; //Ausweichschritt erfolgt
          pathProp.Typ1Test:=pathProp.Typ1Test-1;
          break;
        EndIf
        pathProp.path:=(pathProp.path+1)&7;
      endfor
    Else
      for( i:=8; i>1; i-=1 )
        res := TryStepTowardLocation2(pathProp.path&7);
        TestMessage("Ausweichtest res="+res,4);
        If(res)
          pathProp.steps:=pathProp.steps-1; //Ausweichschritt erfolgt
          pathProp.Typ1Test:=pathProp.Typ1Test-1;
          break;
        EndIf
        pathProp.path:=(pathProp.path-1)&7;
      endfor
    EndIf

//    If ((Me.facing!=oldFace)&&(0))  // XXX
//      //eine Richtungsänderung durchgeführt durch ausweichen
////      SayPergon(Me, "*dreh*");
//      TestMessage("oldFace:facing "+oldFace+":"+Me.facing,1);
//      pathProp.Typ1Test:=0; //beim nächsten Schritt schaun ob wieder frei ist
//      Sleep(1);
//    EndIf

    If (!res)
      TestMessage("Kein Ausweichweg findbar, total blockiert",1);
//      SayPergon(Me, "*zweifel*");
      //dann sollte auch nix weiter gemacht werden als zu pausieren
      StepSleep(5, 1);
      TestMessage("Ausweichschritte fehlgeschlagen.",1);
      //wahrscheinlich durch einfrieren passiert, oder er steht in ner Gruppe von NPC's oder in ein Loch gefallen
      pathProp.steps:=1; //nun kann er prüfen ob es wieder eine freie Richtung gibt und geht dann gleich auf Weg Default
      pathProp.Typ1Test:=2; //damit er nicht doppelt prüft
      SetObjProperty(Me, PROP_PATHFIND, pathProp);
      Return 0;
    EndIf
  EndIf

  me.use_adjustments:=1;

  If (pathProp.steps < 1)
    //Wechsel zu Defaultweg
//    SayPergon(Me, "*Gebs auf*");
    TestMessage("Ausweichschritte aufgebraucht. Wechsel auf Weg Default.",1);
    EraseObjProperty(Me, PROP_BLOCKED);
    EraseObjProperty(Me, PROP_PATHFIND);
  EndIf

  If ((!mob) || (!mob.connected)) //mob tod bzw nicht mehr da
    Return(0);
  EndIf
  //Distanzabbruchkriterium prüfen
  If (pathProp.MobDistStart > DistanceEuclidean(Me, mob))
    //Wechsel zu Defaultweg
//    SayPergon(Me, "*Naeher Juhu*");
    TestMessage("D > new D: "+pathProp.MobDistStart+" > "+DistanceEuclidean(Me, mob),1);
    TestMessage("Distanz Kriterium erfüllt. Wechsel auf Weg Default.",1);
    //Sleep(2);
    EraseObjProperty(Me, PROP_BLOCKED);
    EraseObjProperty(Me, PROP_PATHFIND);
    Return 1;
  EndIf

  //125° Abbruchkriterium prüfen (90°sind zu wenig, da schon bei etwa 70° das angezeigt wird)
  Var dirMob:=GetFacing(Me.x,Me.y,mob.x,mob.y);
  If (min_diff_modulo(dirMob, pathProp.ZielPose.facing) > 2)
    //Wechsel zu Defaultweg
//    SayPergon(Me, "*90 sind rum*");
    TestMessage("90 Grad Kriterium erfüllt. Wechsel auf Weg Default.",1);
    TestMessage("min_diff_modulo(dirMob, pathProp.ZielPose.facing):"+min_diff_modulo(dirMob, pathProp.ZielPose.facing),1);
    TestMessage("dirMob/pathProp.ZielPose.facing:"+dirMob+" "+pathProp.ZielPose.facing,1);
    TestMessage("pathProp.ZielPose.facing: "+pathProp.ZielPose.facing,1);
    EraseObjProperty(Me, PROP_BLOCKED);
    EraseObjProperty(Me, PROP_PATHFIND);
    //Sleep(10);
    Return 1;
  EndIf

  //Good Walk, Facing ist günstig (==Initialenemyfacing)
  If (pathProp.ZielPose.facing==Me.facing)
    //laufe in die richtige Richtung, keinen Klopf-Schritte machen
    TestMessage("Augeklopft",1);
    TestMessage("pathProp.ZielPose.facing: "+pathProp.ZielPose.facing,1);
    TestMessage("Me.facing: "+Me.facing,1);
    pathProp.Typ1Test:=2;
    goStep:=1;
  EndIf

  //Chance sich nach einem neuen Gegner umzuschauen
//  If ((RandomInt(100) < ENEMYCHANGE)&&(0))
////    SayPergon(Me, "*such*");
//    TestMessage("RunToward2: Neuen Gegner suchen.", 1);
//    Var ev:=struct;
//    ev.+type:=EVID_LOOKAROUND;
//    ev.+source:=Me;
//    SendEvent(Me,ev);
//  EndIf

  SetObjProperty(Me, PROP_PATHFIND, pathProp);
  TestMessage("Ausweichschritte erfolgt.",1);
  TestMessage("path="+pathProp.path,1);
EndFunction

Function Modul4(byref mob, byref pathProp)
//Fliehen mit Hindernis ausweichen
  TestMessage("Modul 4", 2);
//  SayPergon(Me, "Quiek");
  Var last_instr:=Me.process.instr_cycles;
  FindPath( Me.x, Me.y, Me.z, mob.x, mob.y, mob.z, Me.realm, 0, 30);
  TestMessage("path instr: " + CStr(Me.process.instr_cycles-last_instr), 1);
  //TestMessage("path: " + path, 1);
  Sleep(2);
EndFunction

Function RunTowardPath(byref mob, byref searchskirt, byref feedback)
//Wenn sich der Gegner bewegt, verwerfe alles auser ein Ausweichpfad wird gelaufen

//wenn nix geplant ist DefaultRunToward
//wenn was geplant ist nächster Schritt auf Path

//wenn ein Schritt schief geht nächstes Bearbeitungslevel (manchmal Schrittwiederholung, falls durch Mobiles blockiert)
//  Default geht schief, guten Weg zum Target suchen (1mal wiederholen)
//  TargetPath Schritt geht schief, Ausweichweg suchen (3mal wiederholen)
//  Ausweich Pfad Schritt geht schief, nen neuen Berechnen (0mal wiederholen)

//PROP_PATHFIND
//  path ... Weg (x,y)
//  step ... aktueller Schritt auf Weg

//Distance(Me, mob)
//GetFacing()

//Problemfälle:
//- Pfadplanen Fehlgeschlagen
//- Gegner hat sich bewegt
//- Pausen wenn nicht gelaufen wird machen
//- Freez des NPC's

//Todo
//- neuen Weg Gegner wechseln wenn Target Weg suchen fehlschlägt
  feedback.exist:=1; //Feedback ist erhältlich

  If ((Me.paralyzed)||(Me.frozen))
    TestMessage("Bin paralysiert oder frozen.", 1);
    StepSleep(10, 1);
    EraseObjProperty(Me, PROP_BLOCKED);
    EraseObjProperty(Me, PROP_PATHFIND);
    feedback.plan:=3;
    Return;
  EndIf

  Var pathProp := GetObjProperty(Me,PROP_PATHFIND);
  If (pathProp)
    //TestMessage("Modul: "+pathProp.typ, 1);
    case (pathProp.typ)
      1 : Modul1(mob, pathProp, searchskirt, feedback);
      2 : Modul2(mob, pathProp);
      3 : Modul3(mob, pathProp);
      4 : Modul3(mob, pathProp);
      default: //Default Weg 0
        EraseObjProperty(Me,PROP_BLOCKED);
        EraseObjProperty(Me,PROP_PATHFIND);
        Modul0(mob, pathProp);
    endcase
  Else
    //Default Weg 0
    Modul0(mob, pathProp);
  EndIf
EndFunction

Function PathFindWithCore(byref pathProp, byref zX, byref zY, zZ, byref searchskirt)
  zZ:=GetStandingHeight(zX,zY,zZ,Me.realm);
  If (CoordinateDistanceEuclidean(Me.x, Me.y, zX, zY) > 40)
    //FindPath ist Kreuzgefährlich wenn die Wege zu lang sind
    //kann den Server komplett anhalten
    //deshalb ne Längensicherung
    TestMessage("Ziel zu weit entfernt.", 4);
    Return 0;
  EndIf
  TestMessage("Pfad zu ("+zX+", "+zY+", "+zZ.z+") suchen.", 4);
  Var path:=FindPath( Me.x, Me.y, Me.z, zX, zY, zZ.z, Me.realm, 0, searchskirt);
  If (path)
    pathProp.path:=path;
    pathProp.steps:=1;
    TestMessage("Pfad gefunden.", 4);
    VisualPath(path, 0xB26);
    Return 1;
  Else
    TestMessage("Kein Pfad gefunden path="+path,4);
  EndIf
  Return 0;
EndFunction

//Function GetRandomTarget(dist)
//  rand:=RandomInt(8) // facing
//  Var ziel:=struct;
//  ziel.+x:=Me.x+ConvertDirectionToLocX(rand);
//  ziel.+y:=Me.y+ConvertDirectionToLocY(rand);
//
//  Return ziel;
//EndFunction

Function VisualPath(byref path, byref typ)
  Var talkme:=GetObjProperty(Me, "talkme");
  If (talkme)
    //If (CInt(talkme.stufe) >= CInt(4))
    If (CInt(talkme.stufe) >= 1)
      Var item;
      If (path)
        foreach point in path
          item:=CreateItemAtLocationPergon(point.x, point.y, point.z, typ, 1,Me.realm);
          SleepMs(100);
          If (item)
            DestroyItemPergon(item);
          Else
            TestMessage("Marke fehlgeschlagen item="+item, 4);
          EndIf
        endforeach
      Else
        TestMessage("Visual Fehler Pfad: "+path,4);
      EndIf
    EndIf
  EndIf
EndFunction

Function VisualPoint(byref x,byref y)
  Var talkme:=GetObjProperty(Me, "talkme");
  If (talkme)
    If (CInt(talkme.stufe) >= 1)
      Var item;
      Var z:=GetStandingHeight(x,y,Me.z,Me.realm);
      item:=CreateItemAtLocationPergon(x, y, z.z, 0x3B0, 1,Me.realm);
      SleepMs(500);
      If (item)
        DestroyItemPergon(item);
      Else
        TestMessage("Marke fehlgeschlagen item="+item, 4);
      EndIf
    EndIf
  EndIf
EndFunction

Function VisualPoint2(byref x,byref y)
  Var talkme:=GetObjProperty(Me, "talkme");
  If (talkme)
    If (CInt(talkme.stufe) >= 1)
      Var item;
      Var z:=GetStandingHeight(x,y,Me.z,Me.realm);
      item:=CreateItemAtLocationPergon(x, y, z.z, 0x3A9, 1,Me.realm);
      SleepMs(500);
      If (item)
        DestroyItemPergon(item);
      Else
        TestMessage("Marke fehlgeschlagen item="+item, 4);
      EndIf
    EndIf
  EndIf
EndFunction

Function TryStepTowardLocation2(byref dir)
  //eigene Pose vorm Schritt merken
  Var oldX:=Me.x;
  Var oldY:=Me.y;
  Var oldFace:=Me.facing;

  If ((!dir) && (dir<>0)) // Quick'n'Dirty Fix
    dir:=Me.facing; // Desöfteren ist dir=error kA wo und wann, viel Spaß Lax :)
  EndIf
  Var xmod:=ConvertDirectionToLocX(dir);
  Var ymod:=ConvertDirectionToLocY(dir);
  TestMessage("try step dir: " + dir, 4);

  If( CanMove( dir ) != 0)
    If (RunTowardLocation( (Me.x + xmod), (Me.y + ymod)))
      If (oldFace!=Me.facing)
        //nur Richtungsänderung
        oldFace:=Me.facing;
        If (!RunTowardLocation( (Me.x + xmod), (Me.y + ymod)))
          Return 0;
        EndIf
      EndIf
      If( (oldx == Me.x) && (oldy == Me.y) && (oldFace==Me.facing))
        //TestMessage("Ausweich Schritt fehlgeschlagen 3", 4);
        Return(0);
      Else
        //TestMessage("Ausweich Schritt erfolgt.", 4);
        Return(1);
      EndIf
    Else
      //TestMessage("Ausweich Schritt fehlgeschlagen 2", 4);
      Return 0;
    EndIf
  Else
    //TestMessage("Ausweich Schritt fehlgeschlagen 1", 4);
    Return 0;
  EndIf
EndFunction

Function WalkToLOS(byref mob, byref localParams)
  Var dist:=Distance(Me,mob);
  Var goDir:=GetFacing(Me.x,Me.y,mob.x,mob.y);
  If (localParams.bevorzugteRichtung)
    If (dist <= localParams.nloRadiusMin)
      //OpponentRichtung-90°-45°
      goDir-=3;
    ElseIf (dist >= localParams.nloRadiusMax)
      //OpponentRichtung-90°+45°
      goDir-=1;
    Else
      //OpponentRichtung-90°
      goDir-=2;
    EndIf
  Else
    If (dist <= localParams.nloRadiusMin)
      //OpponentRichtung+90°+45°
      goDir+=3;
    ElseIf (dist >= localParams.nloRadiusMax)
      //OpponentRichtung+90°-45°
      goDir+=1;
    Else
      //OpponentRichtung+90°
      goDir+=2;
    EndIf
  EndIf
  Var oldFace:=Me.facing;
  //Var toX2:=Me.x + dirs[modulo(mobDir,8)+1][1]*5;
  //Var toY2:=Me.y + dirs[modulo(mobDir,8)+1][2]*5;
  //TestMessage("Face (X,Y): ("+dirs[modulo(mobDir,8)+1][1]*2+", "+dirs[modulo(mobDir,8)+1][2]*2+")",1);
  //VisualPoint2(toX2, toY2);
  goDir:=goDir&7;
  Var toX:=Me.x + ConvertDirectionToLocX(goDir) * 5;
  Var toY:=Me.y + ConvertDirectionToLocY(goDir) * 5;
  //TestMessage("GODir (X,Y): ("+dirs[modulo(goDir,8)+1][1]*5+", "+dirs[modulo(goDir,8)+1][2]*5+")",1);
  //TestMessage("GODir Index: "+(modulo(goDir,8)+1),1);
  //VisualPoint(toX, toY);
  If (WalkTowardLocation(toX, toY))
    If (oldFace!=Me.facing)
      If (!WalkTowardLocation(toX, toY))
        //Kann sich nicht bewegen
        StepSleep(10, 0);
      EndIf
    EndIf
  Else
    //Kann sich nicht bewegen
    StepSleep(10, 0);
  EndIf

  If(!CheckLineOfSight(Me, mob))
    //10% Chance sich dann neu nach einem Gegner umzuschauen
    If (!RandomInt(10))
      TestMessage("WalkToLOS: Neuen Gegner suchen.", 1);
      Var ev:=struct;
      ev.+type:=EVID_LOOKAROUND;
      ev.+source:=Me;
      SendEvent(Me,ev);
    EndIf
  EndIf
EndFunction

Function StepSleep(byref faktor, byref run1)
  If (run1)
    //TestMessage("StepSleep fuer "+( (1000.0 - Me.run_speed * 3.0)/2.0 * faktor )+"ms",4);
    SleepMs( CInt((1000.0 - Me.run_speed * 3.0)/2.0 * faktor) );
  Else
    //TestMessage("StepSleep fuer "+( (1000.0 - Me.run_speed * 3.0) * faktor )+"ms",4);
    SleepMs( CInt((1000.0 - Me.run_speed * 3.0) * faktor) );
  EndIf
EndFunction

Function CalcTimeSteps(byref time, byref run1)
  If (run1)
    Return (time/((1000.0 - Me.run_speed * 3.0)/2.0));
  Else
    Return (time/(1000.0 - Me.run_speed * 3.0));
  EndIf
EndFunction

Function GoToMob(byref mob, byref mindist)
  TestMessage("GoToMob", 2);
  Var dist;
//  Var steps:=0;

  dist:=Distance(Me, mob);
  If ( dist > mindist)
    //SetWarmode(1); //notwendig für die KI Funktionen, weil Master wie Opponent behandelt werden muss? (Fox)
    //<- Ursache vermutlich der Anchor, der wird im Warmode deaktiviert und funkt hier sonst rein, besser Anchor deaktivieren
    If ((mindist + 2) < dist)
      TestMessage("hinrennen",3);
      MainRunToward(mob);
    Else
      TestMessage("hingehen",3);
      MainWalkToward(mob);
    EndIf
    //SetWarmode(0); //sollte man schon machen wenn man nicht in der Kampffunktion landen will
  Else
    StepSleep(1, 0);
  EndIf
EndFunction

Function min_diff_modulo(byref a, byref b)
  Return(CInt(Min((a-b)%8,(b-a)%8)));
EndFunction

Function CheckIgnore(byref opponent)
  Var ignoreList:=GetObjProperty(Me, IGNORE_LIST);
  If (ignoreList)
    If (ignoreList[opponent.serial])
      If (ignoreList[opponent.serial] > ReadGameClock())
        Return 1;
      EndIf
    EndIf
  EndIf
  Return 0;
EndFunction

// lose Items, die den Weg blockieren, mitnehmen
Function CheckForBlockingLooseItems()
  ForEach item in (ListItemsNearLocationWithFlag(
    me.x, me.y, LIST_IGNORE_Z, 1,
    TILEDATA_FLAG_CONTAINER+ TILEDATA_FLAG_DOOR+
    TILEDATA_FLAG_IMPASSABLE+TILEDATA_FLAG_WALL,
    me.realm
  ))
    SleepMs(2);
    If (!item.movable or (Abs(me.z - item.z) > 15))
      // ist fest oder deutlich weiter oben/unten
      continue;
    EndIf

    // Einsammelversuch
    var res := MoveObjectToLocation(item, me.x, me.y, me.z, me.realm);
    If (res)
      TestMessage("LooseItems: Verschiebe "+item.name, 4);
      return;
    EndIf
  EndForEach
  TestMessage("LooseItems: Nichts zu tun", 8);
EndFunction
