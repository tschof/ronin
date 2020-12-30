#!/usr/bin/perl -w


use strict;
use IO::Socket::INET;

my $host =  '10.105.96.90';
my $port =  7007;
my $server = '10.105.96.90';
my $kidpid;
my $line;

my $socket = IO::Socket::INET->new(PeerAddr => 'ddrt-orl-lrom1',
                                   PeerPort => '7007',
                                   Proto    => 'tcp')
                                   or die "Cannot create socket\n";
$socket->autoflush(1);
die "can't fork!\n" unless defined ($kidpid = fork());
print "What should we send ROM?\n";
if($kidpid) {
    while(defined ($line = <$socket>)){
        print STDOUT $line;
    }
    kill("TERM", $kidpid);
} else {
    while(defined ($line = <STDIN>)) {
        print $socket $line;
    }
}





