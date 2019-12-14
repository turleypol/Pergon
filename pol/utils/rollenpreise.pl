#!/usr/bin/perl

use strict;
use warnings;
push @INC, 'utils';
require corefun;

sub main {
    my $items  = ReadConfigFile('pkg/magic/newmagic/magic/itemdesc.cfg');
    my $spells = ReadConfigFile('pkg/magic/newmagic/spells/spells.cfg');

    #my $maxprice =     0;
    #my $maxskill =     0;
    #my $minprice = 60000;
    #my $minskill =   130;
    foreach my $item (sort keys %$items) {
        next unless $items->{$item}{'Desc'} =~ m{Zauberrolle}oi;

        my $spellid   = 'Spell '. $items->{$item}{'Spellid'};
        my $buyprice  = $items->{$item}{'Vendorbuysfor'};
        my $sellprice = $items->{$item}{'Vendorsellsfor'};
        my $skill     = $spells->{$spellid}{'Skill'};

        print ucfirst lc $item ."\n";
        print "{\n";
        #printf "%.2f\n", $sellprice/$buyprice;
        #$maxprice = $price if $price > $maxprice;
        #$maxskill = $skill if $skill > $maxskill;
        #$minprice = $price if $price < $minprice;
        #$minskill = $skill if $skill < $minskill;

        printf "    %-27s %s\n", 'Name', $items->{$item}{'Name'};
        delete $items->{$item}{'Name'};
        printf "    %-27s %s\n", 'Desc', $items->{$item}{'Desc'};
        delete $items->{$item}{'Desc'};

        foreach my $key (sort keys %{$items->{$item}}) {
            unless ($key =~ m{Vendor(buy|sell)sfor}o) {
                printf "    %-27s %s\n", $key, $items->{$item}{$key};
            } else {
                printf "    %-27s %d*fact\n", "VendorBuysFor", $skill
                    if $key eq 'Vendorbuysfor';
                printf "    %-27s %d*fact\n", "VendorSellsFor", $skill*5
                    if $key eq 'Vendorsellsfor';
            }
        }
        print "}\n";
        print "\n";
    }

    #print "Min. Skill: ". $minskill ."\n";
    #print "Min. Price: ". $minprice ."\n";

    #print "Max. Skill: ". $maxskill ."\n";
    #print "Max. Price: ". $maxprice ."\n";
}

main();
