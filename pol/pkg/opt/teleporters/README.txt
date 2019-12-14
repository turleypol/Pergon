Vereinheitlichte Teleporter (.create istein)
============================================

Konfiguration teleport.cfg
--------------------------

Die Konfiguration besteht aus mehreren Blöcken, die folgende Struktur
haben:

Stone <id>
{
    Desc            Beschreibung
    AffectCmdLevel  0/1 - Wirkung auf Cmdlevel? (optional)
    Visible         0/1 - Item wird nach Erzeugen unsichtbar
    Color           <nummer> (optional, sinnvoll bei Visible)
    Graphic         <nummer> (optional, sinnvoll bei Visible)
    Script          <script> (optional)
    WalkOnScript    <script>
}

Desc ist eine frei wählbare Beschreibung für das Objekt. Sie wird im
Konfigurationsgump und, sofern das Item sichtbar ist, für den Spieler
als Objektbeschreibung benutzt.

Mit AffectCmdLevel kann man festlegen, ob das Script bei Cmdlevel-Chars
eine Wirkung hat. Ist die Variable nicht gesetzt, kann das Objekt von
diesen einfach passiert werden. Ansonsten kann das WalkOnScript selbst
entscheiden, was passieren soll.

Ist Visible auf 1 gesetzt, bleibt das Objekt nach der Konfiguration
sichtbar. Ansonsten wird es für Spieler versteckt. Sofern es sichtbar
ist, kann man mit Color und Graphic das Aussehen definieren.

Mit der optionalen Angabe "script" kann man ein Doppelklickscript für
das Objekt definieren. Dieses wird beim Konfigurieren des Objektes
automatisch aufgerufen und kann Konfigurationseinstellungen vornehmen,
die über die Auswahl des Steines hinausgehen, z. B. Zielkoordinaten für
Teleporter einrichten.

Per WalkOnScript wird schließlich festgelegt, welches Script beim
Betreten des Objektes ausgeführt werden soll.

Dateien
-------

- block*.src, check*.src, tele*.src fuer Einzelaktionen
- template.src_ als Vorlage fuer neue Einzelaktionen
- walkonaction startet konfiguriertes Script fuer Einzelaktion
- walkonconfig zur Konfiguration moeglicher Aktionen
- setuptelecommon.src zur Konfiguration diverser tele*.src

Historisches
------------

- walkonaction hieß vorher uni_script
- walkonconfig hieß vorher istein


(Weitgehend) unabhaengige Teleporter
====================================

- dungtele.src
  Standard-Teleporter 0x6200 (automatisch erzeugt, ohne Speicherung),
  manuell auch permanente Variante 0x6201 aufbaubar
- setupteledung.src
  Spezialeinstellungen für Standard-Teleporter (Restriktionen etc.)

- returngate.src, returncreate.src
  Benutzung/Erzeugung von Returngates (viele Tore zu einem Ziel,
  Betreten des Ziels bringt zu Starttor zurueck)

vim: tw=72
