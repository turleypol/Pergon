#!/usr/bin/perl

use strict;
use warnings;

my $in_section = 0;
my $search_for = $ARGV[0];
die "Need argument to search for" unless $search_for;

sub trim_whitespaces {
    my ($line) = @_;

    #$line =~ s/^\s*(\S*)\s*$/$1/og;
    $line =~ s/^\s*//og;
    $line =~ s/\s*$//og;
    $line =~ s/\s+/ /og;

    return $line;
}

open(CONFIG, '<', 'config/npcdesc.cfg') or
    die "Cannot open config/npcdesc.cfg: $!";
my $key; my $name; my $type; my $found = 0;
my %result = ();
while (<CONFIG>) {
    chomp;
    # ignore comments and "empty" lines
    next if m{^\s*(//|#|$)}o;
    # trim comments
    s{\s*(//|#).*$}{}o;
    if (m/^\s*{/o) {
        $in_section = 1;
        $found = 0;
    }
    $key =  $_ unless $in_section;
    $name = $_ if $in_section and m{^\s*Name}oi;
    $type = uc($_) if $in_section and m{^\s*Objtype}oi;
    $found = 1 if $in_section and m{$search_for}oi;

    $in_section = 0
        if m/^\s*}/o;

    if ($found and not $in_section) {
        $type =~ s{^.*[A-Za-z]\s+([0-9a-fx]*)}{$1}oi;
        $type =~ s{X}{x}o;
        $type =  hex($type);
        $key  = trim_whitespaces($key);
        $name = trim_whitespaces($name);
        $result{$type} = {
            "type" => "Nummer ".$type,
            "key"  => $key,
            "name" => $name,
        };
        $found = 0;
    }
}
close(CONFIG);

foreach my $elem (sort { $a <=> $b } keys %result) {
    printf "%-15s %s\n", split(m/ /o, $result{$elem}{type}, 2);
    printf "%-15s %s\n", split(m/ /o, $result{$elem}{key},  2);
    printf "%-15s %s\n", split(m/ /o, $result{$elem}{name}, 2);
    print "\n";
}
