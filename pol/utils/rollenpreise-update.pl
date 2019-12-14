#!/usr/bin/perl

use strict;
use warnings;

use constant INFILE  => 'pkg/magic/newmagic/magic/itemdesc-fact.cfg';
use constant OUTFILE => 'pkg/magic/newmagic/magic/itemdesc.cfg';
use constant ENDTIME => 1262300399; # Ende 2009

my $fact = sprintf("%.5f", 10 + (ENDTIME - time())/2498000);
$fact = 10 if $fact < 10;
print 'Current factor: '. $fact ."\n";

open (my $in, '<', INFILE) or
    die "Cannot read ". INFILE .": $!";
open (my $out, '>', OUTFILE) or
    die "Cannot write ". OUTFILE .": $!";

my $bigbuy = 0;
while (<$in>) {
    if (m{\*fact}) {
        chomp;
        my ($front, $num) = m{^(\s*\S+\s+)(.*)\*fact$}o;
        $num = sprintf("%d", $num*$fact);
        print $out "$front$num\n";
        $bigbuy = $num if $num > $bigbuy and $front =~ m{vendorbuys}io;
    } else {
        print $out $_;
    }
}

close $in;
close $out;

print 'Heiliger Geist/Armageddon: '. $bigbuy ."\n";
print 'cvs commit ', OUTFILE, "\n";
