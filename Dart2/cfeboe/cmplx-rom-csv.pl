#!/usr/bin/perl
use strict;
sub set_file {
    my $file_name = shift;
    my $FILEH;
    open($FILEH, ">", $file_name) or die "cannot open $file_name\n";
    my $conn = {};
    $conn->{"file"} = $FILEH;
    $conn->{"rec"} = 0;
    $conn;
}
# This simple script will extract the raw ROM CSV message content from a ROM log file.
my $file_h = &set_file("rom_to_cfe_cmplx.map");
my @files = <*.csv>;
foreach my $file (@files) {
open(IN, "<$file") or die "cannot open $file\n";
while(<IN>) {
chomp;
my @lroms = split(/,/);
   $lroms[0] =~ tr/"//d;
   $lroms[2] =~ tr/"//d;
   $lroms[2] =~ tr/\///d;
   $lroms[2] =~ tr/:1:S//d;
   $lroms[2] =~ tr/:2:S//d;
   $lroms[2] =~ tr/:1:B//d;
   $lroms[2] =~ tr/:2:B//d;
   $lroms[2] =~ tr/' '//d;
   my $foolen = length $lroms[1];
   if($foolen eq 0) {
   my $output = sprintf("%s,%s\n",
   $lroms[2],
   $lroms[0]);
   my $lf = $file_h->{"file"};
print $lf $output;
}
}
}

1;
