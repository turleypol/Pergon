#!/usr/bin/perl

use strict;
use warnings;

use constant BASEDIR => '/gameworld/Pol/';
my $log = $ARGV[0];
$log = 'log/pol.log' unless $log;
use constant PCS =>     'data/pcs.txt';

my %evil = ();

open(LOGFILE, '<', BASEDIR.$log) or
    die "Cannot open ".BASEDIR.$log.": ".$!;

while (<LOGFILE>) {
    chomp;

    if (my ($char) = m{Character 0*([0-9A-Za-z]+) tried to drop item }o) {
        $char = lc($char);
        if (defined $evil{$char}) {
            $evil{$char}{'count'}++;
        } else {
            $evil{$char} = {count => 1};
        }
    }
}
close(LOGFILE);

open(CHARFILE, '<', BASEDIR.PCS) or
    die "Cannot open ".BASEDIR.PCS.": ".$!;

my $name; my $acct;
while (<CHARFILE>) {
    chomp;

    my ($val) = m{\s+Account\s+([\s\w-]+)}o;
    $acct = $val if $val;
    ($val) = m{\s+Name\s+([\s\w-]+)}o;
    $name = $val if $val;
    ($val) = m{\s+Serial\s+0x([\s\w]+)}o;
    if ($val and grep($_ eq $val, keys %evil)) {
        $evil{$val}{'name'} = $name;
        $evil{$val}{'account'} = $acct;
    }
}
close(CHARFILE);

foreach (sort { $evil{$a}{'count'} <=> $evil{$b}{'count'} } keys %evil) {
    printf(
        "%5d %-30s - %-20s (%s)\n",
        $evil{$_}{'count'}, $evil{$_}{'name'}, $evil{$_}{'account'}, $_,
    );
}

__DATA__
Character 007DF599 tried to drop item 00000055, but had not gotten an item.
[04/29 14:00:02] Character 0099878F tried to drop item 00000000, but had not gotten an item.
