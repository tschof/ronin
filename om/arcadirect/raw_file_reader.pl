#!/usr/bin/perl
use Switch;

sub ntohl
{
    pack("V", unpack("N", $_[0]));
}

sub ntohs
{
    pack("v", unpack("n*", $_[0]));
}


my $buffer = "";
my $infile = shift; 

my $outfile = "test.out";

open (INFILE, "<", $infile) or die "Not able to open file passed in\n";

open (OUTFILE, ">", $outfile) or die "Not able to open output file \n";

binmode(INFILE);

#binmode(OUTFILE);
sub parse_msg
{
    use bytes;
    my $t_str = shift;
    my $total = 0;
        my $time = substr $t_str, 0, 23;
        print OUTFILE $time;
        my $rest = substr $t_str, 23;
        my $type = substr $rest, 0, 1;
        print "Message Type = $type \n";
        my $slen = unpack('n*', substr($rest, 2,2));
        $tlen = $slen + 0;
        my $plen = sprintf("%d", $tlen);
        print "Message Length = $plen \n";
        if(length($rest) >= $tlen) {
            switch($type) {
                case "A" {
                    print OUTFILE " Logon message ";
                    #my $len = unpack('n*', substr($rest, 2,2));
                    print OUTFILE " Length: $slen "; 
                }
                case "L" {
                    print OUTFILE " Logon Rejected $rest ";
                }
                case "D" {
                    print OUTFILE " New Order ";
                    print OUTFILE " Length: $slen "; 
                    my $clordid = unpack('N*', substr($rest, 9, 4));
                    print OUTFILE " CLORDID: $clordid ";
                    my $qty = unpack('N*', substr($rest, 17, 4));
                    print OUTFILE " QTY: $qty ";
                    my $price = unpack('N*', substr($rest, 21, 4));
                    print OUTFILE " PRICE: $price ";
                    my $ex_dest = unpack('n*', substr($rest, 25,2));
                    print OUTFILE " ex_dest: $ex_dest ";
                    $tlen = $tlen + 1; 
                    #print OUTFILE (substr($rest, 26, $slen - 26));
                }
                else {
                    my $prest = substr($rest, 0, $tlen);
                    print OUTFILE " Unknown message $prest ";
                }
           }
           print OUTFILE " --end\n";
           #$t_str = substr($t_str, ($tlen +24));
           $total = 24 + $tlen;
            print "total Length = $total \n";
        }
   $total;
}

while((read(INFILE, $buffer, 65536)) != 0) {
    use bytes;
    my $t_len = 1;
    while(length($buffer) > 23 and $t_len > 0) {
        $t_len = &parse_msg($buffer);
        $buffer = substr($buffer, $t_len);
        print "parsed Length = $t_len \n";
       print "----start{$buffer}tstr_end----\n";

    }
}

close(INFILE) or die "Not able to close in file\n";
close(OUTFILE) or die "Not able to close out file\n";

print "Successfully copied $infile to $outfile. \n";


