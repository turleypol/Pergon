#!/usr/bin/perl
#
use strict;
use warnings;

use Getopt::Long;
use Pod::Usage;

sub search_polfile {
    my ($options, $serials, $polfile, $outfile, $search) = @_; # {{{

    open(my $items, '<', $polfile) or
        die 'Cannot open ', $polfile, ': ', $!;
    my $size = -s $polfile;

    my $statcount = 0;
    my @one_record = ();
    my $x; my $y; my $realm; my $serial; my $type; my $cont;
    ITEM: while (<$items>) {
        chomp;
        # ignore comment lines
        next if m/^#/o;
        # put one record in the buffer
        push @one_record, $_ if m/^([A-Z]|{|}|\s)/o;
        my ($val) = m{^\s+X\s+([0-9]+)$}o;
        $x        = $val if defined $val;
        ($val)    = m{^\s+Y\s+([0-9]+)$}o;
        $y        = $val if defined $val;
        ($val)    = m{^\s+Realm\s+([A-Za-z]+)$}o;
        $realm    = $val if defined $val;
        ($val)    = m{^\s+ObjType\s+([0-9a-fx]+)$}o;
        $type     = $val if defined $val;
        ($val)    = m{^\s+Container\s+([0-9a-fx]+)$}o;
        $cont     = $val if defined $val;
        ($val)    = m{^\s+Serial\s+([0-9a-fx]+)$}o;
        $serial   = $val if defined $val;

        if (m/^$/o and @one_record) {
            if (
                # wrong realm
                ($search->{realm} and $search->{realm} ne $realm) or
                # always ignore gravestones/runes
                ($type =~ m{(0xa300|0x669a)}o)
            ) {
                undef @one_record; undef $cont;
                next ITEM;
            }

            if ($search->{serial} and $search->{serial} eq $serial) {
                # searching for serials and found it
                ;
            } else {
                if ($cont) {
                    # item is in a container
                    if (
                        # only export contents in recursive mode
                        not $options->{recursive} or
                        # container not seen in range
                        not $serials->{$cont}
                    ) {
                        undef @one_record; undef $cont;
                        next ITEM;
                    }
                } else {
                    # in the world
                    if (
                        # not searching in the world
                        (not $search->{x0}) or
                        # out of range
                        ($search->{x0} > $x) or
                        ($x > $search->{x1}) or
                        ($search->{y0} > $y) or
                        ($y > $search->{y1})
                    ) {
                        undef @one_record; undef $cont;
                        next ITEM;
                    }
                }
            }

            # remember serial (maybe it's a container)
            $serials->{$serial} = 1 if $serial and $options->{recursive};

            print $outfile join("\n", @one_record) ."\n\n";
            undef @one_record; undef $cont;
        }

    } continue {
        unless ($options->{quiet}) {
            $statcount++;
            # status
            if ($statcount >= 500000) {
                my $read = tell($items);
                printf STDERR "%.1f%%\n", 100*$read/$size;
                $statcount = 0;
            }
        }
    }
    close($items);
} # }}}

# documentation/help message {{{
=head1 NAME

itemexport - Export Pol items from datafiles in range or by serial

=head1 DESCRIPTION

Search for items in a specified range or a serial of a specified item
and export it from the raw data files in their internal format.
Useful to extract specific regions from backups to re-integrate them
or to export ingame problem stuff to check them on another (test)
server.

=head1 SYNOPSIS

itemexport [options] X1 Y1 X2 Y2 [realm]
itemexport [options] --serial 0xnnn

=head1 OPTIONS

=over 4

=item B<--container> | B<--recursive>

Recursive mode. In this mode, container contents will be exported,
too.  Normally, just empty top containers or (in serial mode) the
specified container will be exported.

=item B<--mobiles>

Include mobiles in the search.  Useful to export PCs or NPCs with
their backpacks.  You may want to enable recursive mode.

=item B<--output>=filename

Write export output to the given filename, not to STDOUT.

=item B<--quiet>

Quiet mode.  Do not show the initial summary and skip the progress
messages.

=item B<--serial>

Modify search to search for this specified serial, skip range checks.
The comparison is done numeric, so hex or decimal values are accepted.

=back

=head1 AUTHOR

Written by Christoph 'Mehdorn' Weber

=head1 REPORTING BUGS

Report bugs to <kontakt@das-mehdorn.de>

=head1 COPYRIGHT

Copyright 2012 Christoph 'Mehdorn' Weber.  License: Creative Commons
Attribution-Share Alike 3.0 Germany
<http://creativecommons.org/licenses/by-sa/3.0/de/>.
This is free software: you are free to change and redistribute it.
There is NO WARRANTY.

=cut
# }}}

# get/check options {{{
my $options;
GetOptions(
    'container'     => \$options->{recursive},
    'mobiles'       => \$options->{mobiles},
    'output=s'      => \$options->{output},
    'quiet'         => \$options->{quiet},
    'recursive'     => \$options->{recursive},
    'serial=o'      => \$options->{serial},

    'help'          => sub { pod2usage(-exitval => 0, -verbose => 2) },
) or pod2usage(1);

pod2usage(
    -msg     => 'You need to supply four coordinates or a serial (--serial)',
    -exitval => 1,
) if (@ARGV < 4 and not $options->{serial});

my $search;
if ($options->{serial}) {
    $search->{serial} = sprintf('%#x', $options->{serial});
} else {
    # sort and check range
    my @x       = sort { $a <=> $b } $ARGV[1], $ARGV[3];
    my @y       = sort { $a <=> $b } $ARGV[2], $ARGV[4];
    my $realm   = $ARGV[5];
    $realm = 'britannia' unless defined $realm;
    pod2usage(
        -msg     => 'Unknown realm',
        -exitval => 1,
    ) unless $realm =~ m{(britannia|ilshenar|malas|tokuno)}o;
    my $search = {
        x0    => $x[0],
        y0    => $y[0],
        x1    => $x[1],
        y1    => $y[1],
        realm => $realm,
    };
}
# }}}

# summary about used options {{{
if (!$options->{quiet}) {
    if ($options->{serial}) {
        print STDERR 'Exporting serial ', $search->{serial}, "\n";
    } else {
        print STDERR 'Exporting range: ',
            $search->{x0}, ', ', $search->{y0}, ' - ',
            $search->{x1}, ', ', $search->{y1}, "\n";
    }
    print STDERR ($options->{mobiles} ? 'Searching' : 'Not searching'),
        ' mobiles', "\n";
    print STDERR ($options->{recursive} ? 'Exporting' : 'Not exporting'),
        ' container contents', "\n";
}
# }}}

$options->{output} = '/dev/stdout' unless $options->{output};
open(my $outfile, '>', $options->{output}) or
    die 'Cannot open ', $options->{output}, ': ', $!;
my $serials;

my @files;
push @files, qw(data/pcs.txt data/pcequip.txt data/npcs.txt data/npcequip.txt)
    if $options->{mobiles};
push @files, qw(data/multis.txt data/items.txt);

foreach (@files) {
    print STDERR "Searching $_ ...\n" unless $options->{quiet};
    print $outfile '### ', $_, "\n";
    search_polfile($options, $serials, $_, $outfile, $search);
}

close($outfile);
print STDERR "Done\n" unless $options->{quiet};
