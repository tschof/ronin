#!/usr/bin/perl -w
use strict;

printRash(shift);

sub printRash {
    my $soupPacket = shift;
    print "        Packet Type <", substr($soupPacket, 0, 1), ">\n";
    my $rash = substr($soupPacket, 1);
    print "          RASH Type <", substr($rash, 0, 1), ">\n";
    print "        Order Token <", substr($rash, 1, 14), ">\n";
    print "               Side <", substr($rash, 15, 1), ">\n";
    print "             Shares <", substr($rash, 16, 6), ">\n";
    print "             Symbol <", substr($rash, 22, 8), ">\n";
    print "              Price <", substr($rash, 30, 10), ">\n";
    print "      Time in Force <", substr($rash, 40, 5), ">\n";
    print "               Firm <", substr($rash, 45, 4), ">\n";
    print "            Display <", substr($rash, 49, 1), ">\n";
    print "             MinQty <", substr($rash, 50, 6), ">\n";
    print "           MaxFloor <", substr($rash, 56, 6), ">\n";
    print "           Peg Type <", substr($rash, 62, 1), ">\n";
    print "      Peg Diff Sign <", substr($rash, 63, 1), ">\n";
    print "     Peg Difference <", substr($rash, 64, 10), ">\n";
    print "   Discretion Price <", substr($rash, 74, 10), ">\n";
    print "     Discr Peg Type <", substr($rash, 84, 1), ">\n";
    print "Discr Peg Diff Sign <", substr($rash, 85, 1), ">\n";
    print "Discr Peg Differenc <", substr($rash, 86, 10), ">\n";
    print "           Capacity <", substr($rash, 96, 1), ">\n";
    print "     Random Reserve <", substr($rash, 97, 6), ">\n";
    print "         Route Dest <", substr($rash, 103, 4), ">\n";
    print "             Custom <", substr($rash, 107, 32), ">\n";
    print "                ISO <", substr($rash, 139, 1), ">\n";
    print "         Cross Type <", substr($rash, 140, 1), ">\n";
}
