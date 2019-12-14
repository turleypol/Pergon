#!/usr/bin/perl

use strict;
use warnings;

open(IN,  '<', 'itemdesc.cfg.base') or die "$!";
open(OUT, '>', 'itemdesc.cfg') or die "$!";

my $count := 0x6f00;

while (<IN>) {
    chomp;
    if (m{^House}) {
        $_ .= ' '. sprintf("%#04x", $count);
        $count++;
    }
    print OUT $_ ."\n";
}

close(IN);
close(OUT);
