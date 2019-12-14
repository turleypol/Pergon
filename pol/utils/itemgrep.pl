#!/usr/bin/perl
#
use strict;
use warnings;

die 'Error: Need at least one pattern' unless $ARGV[0];

my @files = qw(
    data/items.txt
    data/multis.txt
    data/npcequip.txt
    data/npcs.txt
    data/pcequip.txt
    data/pcs.txt
    data/storage.txt
);

FILE: foreach my $itemfile (@files) {
    print "Searching $itemfile ...\n";
    unless (open(ITEMS, '<', $itemfile)) {
        warn 'Cannot open ', $itemfile, ': ', $!;
        next FILE;
    }

    my @one_record = ();
    my %matched = ();
    my $x; my $y; my $r; my $t;
    while (<ITEMS>) {
        chomp;
        # ignore comment lines
        next if m/^#/o;
        # put one record in the buffer
        push @one_record, $_ if m/^(Item|{|}|\s)/o;
        foreach my $expression (@ARGV) {
            if (m/$expression/) {
                $matched{$expression} = 1
            }
        }

        if (m/^$/o and @one_record) {
            if (@ARGV == keys %matched) {
                print $_, "\n" foreach @one_record;
            }
            @one_record = ();
            %matched = ();
        }
    }
    close(ITEMS);
}

print "Done\n";
