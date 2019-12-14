#!/usr/bin/perl

use strict;
use warnings;

unless (defined $ARGV[5]) {
    print "Error: [x1] [y1] [x2] [y2] [realm] [defrag=-1/melt=0/freeze=1]\n";
    exit 1;
}
my $todox1 = $ARGV[0];
my $todoy1 = $ARGV[1];
my $todox2 = $ARGV[2];
my $todoy2 = $ARGV[3];
my $region = $ARGV[4];
my $mode  = $ARGV[5];

# block order on map
#       X ------------ - - ->
#           0     1      2
# Y     +------------- - - -
#       | 8x8 |     |
# | 0   |   0 | 512 | 1024
# |     |-----+-----+- - - -
# |     |     |     |
# | 1   |   1 | 513 | 1025
# |     |-----+-----+- - - -
# |     |     |     |
# :     :     :     :
# |     |     |     |
# |     |-----+-----+- - - -
# |     |     |     |
# | 511 | 511 |1023 |
# V     +------------- - - -
#
# note: block 1 contains the regular coordinates from (0, 0) to (7, 7)
# -> divide world coordinates by 8:
#   block_num = int(x/8) * 512 + int(y/8)

my $indexfile = 'staidx'. $region .'.mul';
# staidx file structure: <n> blocks of:
#
# +-----------------------------+
# |  start  |  length |    ?    |
# |  int16  |  int16  |  int16  |
# +-----------------------------+
# start = 0xFFFF:FFFF -> unused block
#
# each block of staidx references <length> statics blocks in statics file
# beginning at <start>

my $staticsfile = 'statics'. $region .'.mul';
# +----------------------------------------------+
# |    ID   |    X   |    Y   |    Z   |    ?    |
# |  int16  |  int8  |  int8  | s int8 |  int16  |
# +----------------------------------------------+
# ID += 0x4000; X += 8 * blockx; Y += 8 * blocky
# order unknown

open(INDEX_, '<', $indexfile) or
    die "Cannot open $indexfile: $!";
open(STATIN, '<', $staticsfile) or
    die "Cannot open $staticsfile: $!";

open(OUTDEX, '>', $indexfile .'.new') or
    die "Cannot open $indexfile.new: $!";
open(STATOUT, '>', $staticsfile .'.new') or
    die "Cannot open $staticsfile.new: $!";

binmode(INDEX_); binmode(STATIN); binmode(OUTDEX); binmode(STATOUT);


sub FindHighestNumberCFG {
  open(STATCFG,'<','statics.cfg') or
    die "Cannot open statics.cfg: $!";
  my $nr=0;
  while (<STATCFG>) {
    chomp;
    my ($val) = m{^SECTION WORLDITEM\s+([0-9]+)$}o;
    if (defined $val) {
      if ($val > $nr) {
        $nr = $val;
      }
    }
  }
  close STATCFG;
  return $nr;
}

sub ReadCFG{
  my @statitems=();
  my @statitem=();
  my @one_record = ();
  my $x; my $y; my $z; my $id; my $color;
  my $val;
  open(STATCFG,'<','statics.cfg') or
    die "Cannot open statics.cfg: $!";
  while (<STATCFG>) {
    chomp;
    # ignore comment lines
    next if m/^#/o;
    # put one record in the buffer
    push @one_record, $_ if m/^(SECTION WORLDITEM|{|}|\s)/o;
    ($val)    = m{^\s+ID\s+([0-9a-fx]+)$}o;
    $id        = $val if defined $val;
    my ($val) = m{^\s+X\s+([0-9]+)$}o;
    $x        = $val if defined $val;
    ($val)    = m{^\s+Y\s+([0-9]+)$}o;
    $y        = $val if defined $val;
    ($val)    = m{^\s+Z\s+([-0-9]+)$}o;
    $z        = $val if defined $val;
    ($val)    = m{^\s+COLOR\s+([0-9]+)$}o;
    $color        = $val if defined $val;
    
    if (m/^$/o and @one_record) {
      @statitem = ([$id,$x%8,$y%8,$z,$color]);
      push(@{$statitems[$x>>3][$y>>3]},@statitem);
      @one_record = ();
    }
  }
  close STATCFG;
  return @statitems;
}

my $blockx; my $blocky;
if ($region == 0) {
  $blockx = 6144>>3;
  $blocky = 4096>>3;
}
elsif ($region == 2) {
  $blockx = 2304>>3;
  $blocky = 1600>>3;
}
elsif ($region == 3) {
  $blockx = 2560>>3;
  $blocky = 2048>>3;
}
elsif ($region == 4) {
  $blockx = 1448>>3;
  $blocky = 1448>>3;
}
  
my $idxstruct = "i i i";
my $idxstructlen =length(pack($idxstruct));
my $mulstruct = "s C C c s";
my $mulstructlen = length(pack($mulstruct));

$todox1=$todox1>>3; #zu verändernder bereich
$todox2=$todox2>>3;
$todoy1=$todoy1>>3;
$todoy2=$todoy2>>3;
my $x=0;
my $y=0;
my $bufferidx_new;
my $bufferstat_new;
my $bufferidx;
my $bufferstat;
my $static_pointer = 0;
my $idx_pointer;
my $cfgnr;
my @cfgimport;

open(STATCFG,'>>','statics.cfg') or
    die "Cannot open statics.cfg: $!";
    
if ($mode==1){
  @cfgimport=ReadCFG;
  print "Freezing statics.cfg\n";
}
elsif ($mode==0){
  $cfgnr=FindHighestNumberCFG;
  $cfgnr++;
  print "Extracting from block ($todox1,$todoy1) to ($todox2,$todoy2)\n";
}
for ($x=0;$x< $blockx;$x++) {
  for ($y=0;$y<$blocky;$y++) {
    $idx_pointer=(($x * $blocky) + $y) * 12;
    seek(INDEX_,$idx_pointer,0);
    read(INDEX_,$bufferidx,$idxstructlen);
    my ($start,$length_,$unknown)=unpack($idxstruct,$bufferidx);
    if ($start!=0xFFFFFFFF and $length_>0) { #was zu tun
      seek (STATIN,$start,0);
      if ($mode == -1) {#defrag
        read (STATIN,$bufferstat,$length_);
        seek (STATOUT,$static_pointer,0);
        print STATOUT $bufferstat;
        $start=$static_pointer;
        $static_pointer+=$length_;
      }
      elsif ($mode == 0) {#melt
        if ($todox1<=$x and $x<=$todox2 and $todoy1<=$y and $y<=$todoy2) { #im bearbeitungsbereich
          my $num_items = $length_ / $mulstructlen;
          for (1 .. $num_items ) {
            read( STATIN, $bufferstat, $mulstructlen );
            my ($static_id,$static_x,$static_y,$static_z,$static_color)=unpack($mulstruct,$bufferstat);
            $static_x+=$x*8;
            $static_y+=$y*8;
            print STATCFG "\n";
            print STATCFG "SECTION WORLDITEM $cfgnr\n";
            print STATCFG "{\n";
            print STATCFG "  NAME  #\n";
            print STATCFG "  ID    $static_id\n";
            print STATCFG "  X     $static_x\n";
            print STATCFG "  Y     $static_y\n";
            print STATCFG "  Z     $static_z\n";
            print STATCFG "  COLOR $static_color\n";
            print STATCFG "  CONT  -1\n";
            print STATCFG "  TYPE  255\n";
            print STATCFG "}\n";
            $cfgnr++;
          }
          $start=0xFFFFFFFF; #block entfernen
          $length_=0;
        }
        else { #einfaches kopieren
          read (STATIN,$bufferstat,$length_);
          seek (STATOUT,$static_pointer,0);
          print STATOUT $bufferstat;
          $start=$static_pointer;
          $static_pointer+=$length_;
        }
      }
      elsif ($mode == 1) { #freeze
        read (STATIN,$bufferstat,$length_);
        seek (STATOUT,$static_pointer,0);
        print STATOUT $bufferstat;
        $start=$static_pointer;
        $static_pointer+=$length_;

        if (exists $cfgimport[$x][$y]) {
          print "freeze $#{$cfgimport[$x][$y]} +1 items\n";
          for (my $i = 0; $i <= $#{$cfgimport[$x][$y]}; $i++) {
            $bufferstat_new=pack($mulstruct,$cfgimport[$x][$y][$i][0],
                                            $cfgimport[$x][$y][$i][1],
                                            $cfgimport[$x][$y][$i][2],
                                            $cfgimport[$x][$y][$i][3],
                                            $cfgimport[$x][$y][$i][4]);
            print STATOUT $bufferstat_new;
            $static_pointer+=$mulstructlen;
            $length_+=$mulstructlen;
          }
        }
      }
    }
    elsif ($mode==1) { #freeze an leerer stelle
      if (exists $cfgimport[$x][$y]) {
        seek (STATOUT,$static_pointer,0);
        $start=$static_pointer;
        $length_=0;
        print "freeze $#{$cfgimport[$x][$y]} +1 items\n";
        for (my $i = 0; $i <= $#{$cfgimport[$x][$y]}; $i++) {
          $bufferstat_new=pack($mulstruct,$cfgimport[$x][$y][$i][0],
                                          $cfgimport[$x][$y][$i][1],
                                          $cfgimport[$x][$y][$i][2],
                                          $cfgimport[$x][$y][$i][3],
                                          $cfgimport[$x][$y][$i][4]);
          print STATOUT $bufferstat_new;
          $static_pointer+=$mulstructlen;
          $length_+=$mulstructlen;
        }
      }
    }
    seek(OUTDEX,$idx_pointer,0);
    $bufferidx_new = pack($idxstruct,$start,$length_,$unknown);
    print OUTDEX $bufferidx_new;
  }
}

close(INDEX_); close(STATIN); close(OUTDEX); close(STATOUT); close(STATCFG);
