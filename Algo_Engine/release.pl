#!/usr/bin/perl -w
use strict;
use File::Copy;
use File::Basename;
use File::Path;
use File::Spec;
use Cwd;

# adding a comment

my $build_cfg=shift;
die "usage: release.pl build-config-file\n" if not defined $build_cfg;

open(CFG,"<$build_cfg") or die "Cannot open $build_cfg\n";

my @targets;
my $resfile;
while (<CFG>) {
    if (/^\s*target=(.+)/i) {
	push @targets,$1;
    }
    elsif (/^\s*resource=(.+)/i) {
	$resfile=$1;
    }
}

my $name;
my $ver;
open(RESFILE,"<$resfile") or die("Cannot open $resfile\n");
while(<RESFILE>) {
    if (/VALUE "InternalName", "(.+)"/) {
	$name=$1;
    }
    elsif (/VALUE "ProductVersion", "(.+)"/) {
	$ver=$1;
    }
}

die "Cannot find InternalName field\n" if not defined $name;
die "Cannot find ProductVersion field\n" if not defined $ver;

$ver=~s/,/./g;
$ver=~s/\s//g;
my $dirname="$name-$ver";
my $targetdir=$ENV{ALGO_TARGETS};
if (not defined $targetdir) {
    $targetdir=".";
}

my $fullpath=File::Spec->catdir(($targetdir,$dirname));

die("Version already exists: $fullpath\n") if -e $fullpath;

# Now make the dir
eval { mkpath($fullpath) };
if ($@) {
    die("Couldn't create dir: $@");
}

my ($target_volume,$target_directories,$target_file)=File::Spec->splitpath($targetdir);
$target_directories=File::Spec->catdir(($target_directories,$dirname));

foreach my $filetocopy (@targets) {
    my $workdir=getcwd();
    my ($volume,$directories,$file)=File::Spec->splitpath($filetocopy);
    print "Does $filetocopy exist?  ";
    if (-e $filetocopy) {
	print "Yes!\n";
    }
    else {
	print "NO!\n";
	die("Required target $filetocopy does not exist, aborting\n");
    }

    my $copytarget=File::Spec->catpath($target_volume,$target_directories,$file);
    print "Copying $filetocopy => $copytarget...";
    if (copy($filetocopy,$copytarget)) {
	print "Successfull!\n";
    }
    else {
	print "Failed!\n";
	die("Failed to copy target... aborting\n");
    }
}

print "\n===> SUCCESSFULL RELEASE! <===\n";
