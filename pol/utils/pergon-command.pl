#!/usr/bin/perl

use strict;
use warnings;
use IO::Socket;
use Getopt::Long;

use constant DEF_HOST => 'localhost';
use constant DEF_PORT => 2597;
#use constant DEF_PORT => 6597;
use constant EOL => "\015\012";

sub filter {
    my ($line, $opts) = @_;
    $opts = {} unless defined $opts;

    if ($line =~ s{^s}{}o) {
        return $line;
    }
    if ($line =~ s{^i}{}o) {
        return $line;
    }
    return $line;
}


my $host = DEF_HOST;
my $port = DEF_PORT;
GetOptions(
    "host=s" => \$host,
    "port=i" => \$port,
) or die "Unknown option";
my $cmd = 'Status';
$cmd = join(' ', @ARGV) if @ARGV;

my $socket = IO::Socket::INET->new(
    Proto       => "tcp",
    PeerAddr    => $host,
    PeerPort    => $port,
);
die "Cannot connect to $host:$port" unless $socket;
$socket->autoflush(1);
$/ = EOL;

print $socket 's'. $cmd .EOL;
while (my $line = <$socket>) {
    chomp $line;
    last if $line eq 's<<< EoT >>>';
    print filter($line) ."\n";
}
close $socket;

