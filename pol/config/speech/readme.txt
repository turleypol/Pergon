SpeechingSystem @ Pergon
------------------------

Ok, dann versuche ich mal den Kram verstaendlich zu erklaeren.

Also, es gibt insgesamt 4 Ebenen, die im Zweifelsfall durchlaufen werden.
Zuerst bestimmt er, wo sich der Typ grade aufhaelt, das sind die selben
Locations, wie sie in der WWW-Online-Uebersicht genutzt werden. Dann ermit-
telt er noch, von welchem Typ der NPC ist (die Typen stehen weiter unten
als "Tabelle"). Und dann sucht er halt in bis zu 4 ebenen nach dem ent-
sprechenden wort oder der wortkombination (die muessten dann per Space
getrennt hintereinander aufgefuehrt werden und werden 1:1 so abgecheckt):

  1.)  Gibt es eine Datei: "speech/location/"+npclocation+"/"+npctyp ?
       Falls Nein -> 2.
       Falls Ja, gibt es eine Struktur
         "Platzhalter wort
          {
              Antwort text
          }" ?
       Falls Nein -> 2.
       Falls Ja -> Sammel die Antworten in eine Struktur und mische sie mit
       denen aus 2. und laber danach

  2.)  Gibt es eine Datei: "speech/location/"+npctyp ?
       Falls Nein -> 2a.
       Falls Ja, gibt es eine Struktur
         "Platzhalter wort
          {
              Antwort text
          }" ?
       Falls Nein -> 2a.
       Falls Ja -> Mische diese Antworten zu denen aus 1. hinzu

  2a.) Gibt es Antworten aus 1.+2. ?
       Falls Nein -> 3.
       Falls Ja -> Suche dir per zufall eine antwort aus und laber sie -> 6.

  3.)  Gibt es eine Datei: "speech/location/"+npclocation+"/all" ?
       Falls Nein -> 4.
       Falls Ja, gibt es eine Struktur
         "Platzhalter wort
          {
              Antwort text
          }" ?
       Falls Nein -> 4.
       Falls Ja -> Suche dir per zufall eine antwort aus und laber sie -> 6.

  4.)  Gibt es eine Datei: "speech/global" ?
       Falls Nein -> 5.
       Falls Ja, gibt es eine Struktur
         "Platzhalter wort
          {
              Antwort text
          }" ?
       Falls Nein -> 5.
       Falls Ja -> Suche dir per zufall eine antwort aus und laber sie -> 6.

  5.)  Laber "Dazu weiss ich leider nichts." (will ich noch variabel machen,
       d.h. nich nur den, sondern auch andere Texte).

  6.)  Ende

Ok, das war die abfolge, nun noch ein paar worte zu den eintraegen "Antwort":
Der NPC labert dann alles, was unmittelbar dahintersteht. es ist auch m�glich.
den Kram geschlechtsspezifisch zu gestalten, das sieht dann so aus:
  AntwortM Dies laber ich nur, wenn ich maennlich bin!
  AntwortW Dies laber ich nur, wenn ich weiblich bin!
  Antwort  Dies laber ich, unabhaengig vom geschlecht.
er mischt sich das halt zusammen und nimmt per zufall eins. da ich es hier
fuer wenig sinnvoll halte, auch noch einen zufallswert anzugeben (bei den
klamotten machen mir es noch inkl. zufall), kann man die haeufigkeit hier
steuern, indem man eintraege doppelt angibt . wird manchmal schon gemacht.

weiterhin gibt es, aktuell, noch 2 spezialantworttypen, die da waeren:
  wachen_halt
  seid_gegruesst
diese teile werden "kuenstlich" abgerufen, wenn man zu nem NPC hinlatscht.
(sprich: ich habe hier das speech-sytem dafuer missbraucht, weils einfacher
zu konfiggen is.)

Durch das obige 4-stufige System ist es moeglich, und genau das war angedacht,
allgemeine antworten zu definieren und sie in eine hoehere stufe zu packen,
sie aber durch spezialantworten ersetzen zu lassen (befinden sich dann in
einer niedrigeneren stufe).

Jo, "location" duerfte klar sein, und "npctyp", sind folgende, die wir
aktuell haben:  (wie du siehst, gibt es noch nicht fuer jeden typ spezial-
antworten, dann einfach 'ne bereits existente datei nehmen, umbenennen und
modifizieren)

  advertiser    - Werbende
  alchemist     - Alchemist
  architect     - Architekt
  armorer       - Ruestungsbauer
  baker         - Baecker
  barber        - Barbier
  barkeeper     - Barkeeper
  banker        - Baenker
  bard          - Barde
  beebreeder    - Imker
  bowyer        - Bogner
  butcher       - Metzger
  carpenter     - Zimmermann
  castleguard   - Waechter
  cobbler       - Schuster
  crier         - Stadtschreier
  farmer        - Bauer
  fisherman     - Fischer
  healer        - Heiler
  guildmaster   - Gildenmeister
  jeweler       - Juwelier
  leatherworker - Lederer
  mage          - Magier
  mapmaker      - Kartenzeichner
  miner         - Minenarbeiter
  person        - Person
  playervendor  - Haendler
  provisioner   - Kraemer
  quest_target  - Der, der wo Ziel der Nerverei ist
  questie       - Der, der was will und rumnervt
  ranger        - Waldlaeufer
  scribe        - Schreiber
  shipwright    - Schiffsbauer
  smith         - Schmied
  stable        - Stallmeister
  tailor        - Schneider
  tinker        - Bastler
  townguard     - Stadtgardist
  townperson    - Stadtbewohner
  turkey        - Haendler, der besonders penetrant nervige welche
  weapon1       - Schuetze
  weapon2       - Schwertmeister
  weapon3       - Kampfmeister
  weaponsmith   - Waffenschmied

Wie dir sicher aufgefallen ist, gibt es noch keine einzige Datei fuer den
Typ 1, weil wir bisher nicht die rechte Zeit dazu hatten. Naja, ueber den
kann man den Miner in Minoc ganz andere Sachen, oder aber mehr sachen,
erzaehlen lassen, als andere Miner (z.B.)

Wegens testen und so... wenn ich von dir Files hab, die du testen willst,
mussich sie nur hochschieben und sie sind SOFORT aktiv.

Jason
