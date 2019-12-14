#!/usr/bin/perl
#
use strict;
use warnings;
use File::Find;

sub trim {
    my ($str) = @_;

    $str =~ s/^\s+//;
    $str =~ s/\s+$//;

    return $str;
}

sub find_itemdesc {
    my @filelist = ();
    find(sub {
        push @filelist, $File::Find::name
            if $File::Find::name =~ m{itemdesc\.cfg$}o;
        },
        ('.')
    );
    return @filelist;
}

sub read_itemdesc {
    my ($file, $desc) = @_;

    open(DESC, '<', $file) or die "Cannot read $file: $!";
    my $obj; my $name;
    while (<DESC>) {
        chomp;
        # ignore comment lines, remove trailing comments
        next if m{^(#|//)}o;
        s{\s+(#|//).*}{}o;
        my (undef, $val) = m{
            ^\s*
            (armor|boat|container|door|house|item|map|spellbook|weapon)\s+
            (.*)
        }oix;
        $obj = lc($val) if $val;
        ($val) = m{^\s*desc\s+(.*)}oi;
        $name = trim($val) if $val;

        $desc->{hex($obj)} = $name if $obj and $name;
        if (m/^\s*$/) {
            $name = '';
            $obj =  '';
        }
    }
    close DESC;
}

sub list_container {
    my ($serial, $desc) = @_;

    print STDERR
        "Searching for items in container with serial $serial ...\n";

    my $itemfile = 'data/items.txt';
    open(ITEMS, '<', $itemfile) or
        die "Cannot open $itemfile: $!";

    my %record = ();
    my $found = 0;
    while (<ITEMS>) {
        chomp;
        # ignore comment lines
        next if m/^#/o;
        # put one record in the buffer
        if (my ($key, $value) = m{^\s+(\w+)\s+(.*)}o) {
            $record{$key} = $value;
        }
        if (m/^$/o) {
            if (
                defined $record{Container} and
                (hex($record{Container}) == $serial)
            ) {
                $record{Desc} = $desc->{hex($record{ObjType})};

                foreach (qw(Name Desc Amount ObjType Serial)) {
                    printf "%-10s %s\n", $_.":", $record{$_} if $record{$_};
                }
                print "\n";
                $found++;
            }
            %record = ();
        }

    }
    close(ITEMS);
    print STDERR "done, $found items found\n";
}


unless (defined $ARGV[0]) {
    print "Error: (Container) serial needed\n";
    exit 1;
}

my $serial = hex($ARGV[0]);
my %desc = ();
print STDERR "Reading item descriptions ";
foreach my $file (find_itemdesc) {
    read_itemdesc($file, \%desc);
    print STDERR '.';
}
print STDERR "\n";
list_container($serial, \%desc);

