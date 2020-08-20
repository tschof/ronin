#!/usr/bin/perl
#
use strict;
use Archive::Zip qw( :ERROR_CODES :CONSTANTS );
use File::Spec;
use File::stat;
use bignum;
use Class::Struct;
#require constants;
my @files = <PQ.GMISEFT.*.txt>;

my $zip = Archive::Zip->new();
my $res = AZ_OK;
foreach my $file (@files){
	$zip->addFile( "$file" );
	my $filename = $file;
	$filename =~ s/\.txt$/\.gz/;
	$res = $zip->writeToFileNamed("$filename")	
}
