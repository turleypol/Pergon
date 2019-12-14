#!/usr/bin/perl

sub ReadConfigFile { # {{{
    my ($filename) = @_;

    my $elems     = {};
    my $insection = 0;
    my $sectname  = '';
    my $sectcont  = {};

    open(my $cfg, '<', $filename) or die "Cannot open $filename: $!";
    while (<$cfg>) {
        chomp;
        # comments, empty lines {{{
        # ignore empty and comment only lines
        next if m{^($|\s+(//|#))}o;
        # trim comments
        s{\s*(//|#).*}{}o;
        # }}}

        # begin and end sections {{{
        if ($_ eq '{') {
            $insection++;
            die "Error reading $filename: Fucked up brace matching"
                if $insection > 1;
            # clear section content hash
            $sectcont = {};
            next;
        }
        if ($_ eq '}') {
            $insection--;
            die "Error reading $filename: Fucked up brace matching"
                if $insection < 0;
            die "Could not find section name in $filename"
                if $sectname eq '';
            # store new section in element hash
            $elems->{$sectname} = $sectcont;
            # reset sectname
            $sectname = '';
            next;
        }
        # }}}

        # split up section {{{
        if ($insection == 0) {
            # out of a section any word is a section name
            $sectname = $_ if m{^\w+}o;
        } else {
            # in a section -- split up values and fill section content
            # kill trailing spaces
            s{\s+$}{}o;
            # split
            my ($key, $value) = m{^\s*(\S+)\s+(.*)$}o;

            # simple checks
            die "Could not read key in $sectname from $filename"
                unless defined $key;
            die "Could not read value for $key in $sectname from $filename"
                unless defined $value;

            $key = ucfirst lc $key;
            # store
            $sectcont->{$key} = $value;
        }
        # }}}
    }

    close($cfg) or die "Cannot close $filename: $!";

    return $elems;
} # }}}

1;
