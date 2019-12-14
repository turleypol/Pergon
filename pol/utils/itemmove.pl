#!/usr/bin/perl
#
use strict;
use warnings;

use Getopt::Long;

my $infile  = '';
my $outfile = '';
my %dev = (
    X => 0,
    Y => 0,
    Z => 0,
);

GetOptions(
    'input=s'   => \$infile,
    'output=s'  => \$outfile,
    'x=i'       => \$dev{X},
    'y=i'       => \$dev{Y},
    'z=i'       => \$dev{Z},
) or die 'Wrong parameters';

die 'Error: Need input and output' unless $infile and $outfile;

open(my $in,  '<', $infile)  or die "Cannot open $infile: $!";
open(my $out, '>', $outfile) or die "Cannot open $outfile: $!";

my @one_record = ();
my $cont;
while (<$in>) {
    chomp;
    # ignore comment lines
    next if m/^#/o;

    # put one record in the buffer
    push @one_record, $_ if m/^(Item|{|}|\s)/o;
    $cont = 1 if m{^\s+Container\s+[0-9a-fx]+$}o;

    # record complete?
    if (m/^$/o and @one_record) {
        if ($cont) {
            # unaltered, do not move container contents
            print $out join("\n", @one_record), "\n\n";
        } else {
            # move as specified
            foreach (@one_record) {
                my ($prefix, $type, $sep, $value) =
                    m{^(\s+)([X-Z])(\s+)(-?\d+)$}o;
                if ($type) {
                    # calculate deviation
                    print $out $prefix, $type, $sep, $value+$dev{$type}, "\n";
                } else {
                    # print unaltered
                    print $out $_, "\n";
                }
            }
            print $out "\n";
        }
        undef $cont;
        @one_record = ();
    }
}
close $in;
close $out;
