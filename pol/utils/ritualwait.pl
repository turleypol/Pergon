#!/usr/bin/perl
#
use strict;
use warnings;
use POSIX;

open(RIT, '<', $ARGV[0]) or
    die "Cannot open $ARGV[0]: $!";
while (<RIT>) {
    chomp;
    print "$_\n";
    if (m/^\s*part\s+(emote|speak)/o) {
        my ($indent, $what, $text) =
            $_ =~ m/^(\s*)part\s+(emote|speak)\s*(.*)/o;
        my $wait;
        $wait = ceil(length($text) * 0.3) if $what eq "emote";
        $wait = ceil(length($text) * 0.1) if $what eq "speak";
        print $indent ."part wait ".($wait+1)."000\n";
    }
}
close(RIT);
