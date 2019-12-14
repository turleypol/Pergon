#!/usr/bin/perl
#
use strict;
use warnings;

# Konvertiert Nahrungsklassen in Items, Duplikate werden aussortiert
# liest animals-eat.classes und baut daraus eine animals-eat.cfg

# Changelog
# 2008-04-16 mehdorn        erstellt

# Originalnahrung aus dem Fress-Script {{{
#    0x09d0, 0x09d1, 0x09d2, 0x0c5c, 0x0c5d, 0x0c64, 0x0c65, 0x0c66,
#    0x0c67, 0x0c6a, 0x0c6b, 0x0c6c, 0x0c6d, 0x0c6e, 0x0c70, 0x0c71,
#    0x0c72, 0x0c73, 0x0c74, 0x0c75, 0x0c77, 0x0c78, 0x0c79, 0x0c7a,
#    0x0c7b, 0x0c7c, 0x0c7f, 0x0c80, 0x0c81, 0x0c82, 0x0d39, 0x0d3a,
#    0x0f36, 0x100c, 0x100d, 0x171d, 0x171e, 0x171f, 0x1720, 0x1721,
#    0x1722, 0x1723, 0x1724, 0x1725, 0x1726, 0x1727, 0x1728, 0x1729,
#    0x172a, 0x172b, 0x172c, 0x172d, 0x1a92, 0x1a93, 0x1a94, 0x1a95,
#    0x1a96, 0x1ebd, 0x1ebe, 0x1ebf
# }}}

my %itemlist = ( # {{{
    "Aas"           => [0x1ce2, 0x1ce5, 0x1cec..0x1cf0, 0x1d9f..0x1da4, ],
    "Eierschalen"   => [0x9b4, ],
    "Fisch"         => [0x97a, 0x97b, 0x9cc..0x9cf, 0xdd6..0xdd9, ],
    "Frischfleisch" => [0x9f1, 0x9b9, 0x1607, 0x1609, 0x9c9, 0x976, 0x977, ],
    "Garfleisch"    => [0x9f2, 0x9b8, 0x1608, 0x160a, 0x9c0, 0x1044, ],
    "Gemuese"       => [
        0xc6d, 0xc6e, 0xc71, 0xc77, 0xc78, 0xc7c, 0xd1a, 0xd39, 0xd3a,
        0xf84, 0x18dd, 0x18e3, 0x18eb, 0x1aa2,
    ],
    "Getreide"      => [0xc7f, 0xc81, ],
    "Gras"          => [
        0xc37, 0xc38, 0xc45..0xc4e, 0xc83, 0xc85..0xc89, 0xc8c..0xc8e,
        0xc93, 0xc97, 0xcac..0xcb6, 0xcb9..0xcc1, 0xcc5, 0xcc6,
        0xd32..0xd34, 0xdc5, 0xdc6, 0x1a9b, 0x1ebd, 0x234b..0x234d,
    ],
    "Kaese"         => [0x97c..0x97e, ],
    "Kekse"         => [0x160c, ],
    "Knochen"       => [0x0F7E, 0x0F80, 0x6322, 0x6328, 0x6333, ],
    "Obst"          => [
        0x994, 0x9d0..0x9d2, 0xc5c, 0xc6a, 0xc6b, 0xc72, 0xc75, 0xc79,
        0xd1a, 0x1720, 0x1721, 0x1724, 0x1727, 0x1728, 0x172a, 0x172c,
        0x172d,
    ],
    "Salat"         => [0xc71, ],
    "Samen"         => [
        0xff87, 0xda00..0xda0e, 0xda10..0xda21, 0xda23, 0xda24,
        0xefec..0xeff0, 0xeff2..0xefff,
    ],
    "TODO"          => [0x160c, ],
); # }}}

sub resolve_class { # {{{
    my $class = shift;
    my @classes = split(m/ /, $class);
    my %res = ();

    foreach my $class (@classes) {
        die "Cannot resolve '$class'" unless exists $itemlist{$class};
        foreach my $item (@{$itemlist{$class}}) {
            $res{$item} = 1;
        }
    }
    return join(',', map { sprintf("0x%x", $_) } sort keys %res);
} # }}}

# main {{{
open(CLASS, '<', 'config/animals-eat.classes') or
    die "Cannot open animal classes: $!";
open(CFG, '>', 'config/animals-eat.cfg') or
    die "Cannot open animal config: $!";
print CFG "# Bitte nicht bearbeiten, stattdessen animals-eat.classes aendern\n";
print CFG "# und animals-eat.pl laufen lassen\n";
print CFG "\n";

my $number; my $class; my $temp;
while (<CLASS>) {
    chomp;
    ($number) = m/([0-9]+)/o if m/^Nummer/o;
    ($class) = m/^Klasse\s+(.*)\s*$/o if m/^Klasse/o;
    if (m/NpcTemplate/oi) {
        ($temp) = m/^NpcTemplate\s+(.*)\s*/o;
        print CFG "Animal $number # $temp\n";
        print CFG "{\n";
        print CFG "    Food ". resolve_class($class) ."\n";
        print CFG "}\n\n";
    }
}
print CFG "# v"."im: foldmarker=Animal,} foldmethod=marker nowrap\n";
close(CLASS);
close(CFG);
# }}}
