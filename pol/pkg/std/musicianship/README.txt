Ueberblick

npc-music.src
  Ist das Musikscript fuer Barden-NPCs und liegt nur
  thematisch passend hier. Es hat nichts mit dem Skill
  Musizieren zu tun.

use-instrument.src
  Etwas Klimpern auf den Instrumenten, SkillCheck auf 20%.
  Recht zum Skillen, Bardenlieder werden nicht gespielt.
  Wurde moeglicherweise beim Friedensstiften benutzt.

notenleer.src
  Eigene Noten schreiben

noten.src
  Selbstgeschriebene Noten spielen, keine Sonderwirkungen

bardsong.src
  Spielt Bardenlied bei Doppelklick auf (vorgefertigte)
  Notenrolle, Sonderwirkungen treten auf

musicbook.src
  Musikbuchverwaltung, spielt bei passender Auswahl
  vorgefertigte oder selbstgemachte Noten ab

Die Hauptfunktionen werden in den Includes abgewickelt.
Grundsätzlich läuft es so:

Wenn ein Barde spielt, wird eine entsprechende Property mit
der liednr, die er spielt, gesetzt. Die Noten selbst werden
vom abgekoppelten Script playnotes gespielt. Falls der Barde
(noch) erfolgreich spielt, schaut PlayBardSong alle REGDELAY
Sekunden nach Mobiles der Umgebung und regeneriert die, die
den Barden neu hoeren, bzw. ihm die ganze Zeit schon
zuhoeren.

Wenn ihn ein Mobile zum ersten Mal hoert, werden seine Stats
per EvokeEffects erhoeht und eine Property wird gesetzt,
welchem Barden er lauscht. Zudem wird das Unbard-Script pro
Hoerer gestartet.

Alle STATDELAY Sekunden prüft das Unbard-Script für jeden
Mobile, ob der Barde noch in der Naehe ist und noch das
gleiche Lied spielt. Wenn ja, passiert nichts, wenn nein,
werden die Stats wieder zurueckgesetzt.

vim: tw=60
