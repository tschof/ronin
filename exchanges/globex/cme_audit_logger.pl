#!/usr/bin/perl -w
use strict;
use Time::Local;
require fix_constants;
my $cur_date = shift;
my $iscentral = 0;
my $server_trans_num = 1;

my %cancels;
my %ex_ids;

my $file_name = "CMEAudit" . $cur_date . ".txt";
my $FILEH;
open($FILEH, ">", $file_name) or die "cannot open $file_name\n";


my $tzinfo = `strings /etc/localtime | egrep -o "[CE]ST[56][CE]DT"`;
if (!defined($tzinfo) || length($tzinfo) == 0) {
print "cannot determine time zone\n";
}
if ($tzinfo =~ "CST6CDT") {
$iscentral = 1;
print "Central time\n";
} else {
print "Eastern time\n";
}

my @files = <*$cur_date*.LOG>;

foreach my $file (@files) {
open(IN, "<$file") or die "cannot open $file\n";
while(<IN>) {
chomp;
my $fix_message = $_;
if ($fix_message =~ /35=(.+?)\x1/) {
if ($1 eq "8") {
    $fix_message =~ m/\b11=/g;
    my $pos2 = pos($fix_message);
    my $big_sub = substr($fix_message, $pos2);
    $big_sub =~ m/\x01/g ;
    my $pos3 = pos($big_sub);
    my $ord_id = substr($big_sub, 0, ($pos3 -1));
    $fix_message =~ m/\b37=/g;
    my $pos = pos($fix_message);
    my $big_num = substr($fix_message, $pos);
    $big_num =~ m/\x01/g;
    my $pos1 = pos($big_num);
    my $ex_id = substr($big_num, 0, ($pos1 -1));
   $ex_ids{$ord_id} = $ex_id; 
}
} else {
my @lroms = split(/,/, $fix_message);
if($lroms[0] =~ /  S/) {
   if(defined $lroms[15] && defined $lroms[17]) {
       $cancels{$lroms[15]} = $lroms[17];
   }
}
}

}
close(IN);
}

foreach my $file (@files) {
open(IN, "<$file") or die "cannot open $file\n";
while(<IN>) {
chomp;
&dump_fix($_);
}
close(IN);
}


sub dump_fix {
my $num;
my $val;
my $fixmsg = shift;
my %message_tags;
if(&is_order_related($fixmsg)) {
    if ($fixmsg =~ /((\d+=.+?\x01)+)/) {
        my @fields = split(/\x01/, $1);
        foreach my $f (@fields) {
            ($num, $val) = split(/=/, $f);
            $message_tags{$num} = $val;
            }
            my $mess_type;
            my $fm_direct;
            my $sm_direct;
            my $efn;
            my $session;
            my $big_status;
            my $mtype = $message_tags{35};
            my $mstatus = $message_tags{150};
           my $clordid = $message_tags{11};
           my $corr_id;
           my $ex_id;
           my $quant;
           my $fill_price = "";
           my $bs = $message_tags{54};
           if(defined $bs && $bs eq "1") {
               $bs = "B";
           } elsif(defined $bs && $bs eq "2") {
               $bs = "S";
           } else {
               $bs = "";
           }
           if(defined $clordid) {
               $ex_id = $ex_ids{$clordid};
               if(!defined $ex_id) {
                   $ex_id = $cancels{$clordid};
               } 
               if(!defined $ex_id) {
                   $ex_id = "NONE_FOUND";
               }
               $corr_id = $cancels{$clordid}; 
               if(!defined $corr_id) {
                   $corr_id = "";
               }
           } else {
               $corr_id = "";
               $ex_id = "";
           }
            if(defined $mtype && $mtype eq "D") {
               $mess_type = "NEW ORDER"; 
               $fm_direct = "FROM CLIENT";
               $sm_direct = "TO CME";
               $efn = &get_exe_firm_num($message_tags{49});
               $session = &get_session_id($message_tags{49}); 
               $big_status = "OK";
               $clordid = $message_tags{11};
           } elsif (defined $mtype && $mtype eq "G") {
               $mess_type = "MODIFY";
               $fm_direct = "FROM CLIENT";
               $sm_direct = "TO CME";
               $efn = &get_exe_firm_num($message_tags{49});
               $session = &get_session_id($message_tags{49}); 
               $big_status = "OK";
               $clordid = $message_tags{11};
           } elsif (defined $mtype && $mtype eq "F") {
               $mess_type = "CANCEL";
               $fm_direct = "FROM CLIENT";
               $sm_direct = "TO CME";
               $efn = &get_exe_firm_num($message_tags{49});
               $session = &get_session_id($message_tags{49}); 
               $big_status = "OK";
               $clordid = $message_tags{41};
           } elsif (defined $mtype && $mtype eq "3") {
               $mess_type = "REJECTED";
               $fm_direct = "FROM CME";
               $sm_direct = "TO CLIENT";
               $efn = &get_exe_firm_num($message_tags{56});
               $session = &get_session_id($message_tags{56}); 
               $big_status = "Reject";
           } elsif(defined $mtype && $mtype eq "8" && ($mstatus eq "4" || $mstatus eq "12")) {
               $mess_type = "ELIMINATED";
               $fm_direct = "FROM CME";
               $sm_direct = "TO CLIENT";
               $efn = &get_exe_firm_num($message_tags{56});
               $session = &get_session_id($message_tags{56}); 
               $big_status = "OK";
               $clordid = $message_tags{41};
               if(!defined $clordid) {
                   my $clordid = $message_tags{11};
               }
           } elsif(defined $mtype && $mtype eq "8" && $mstatus eq "E") {
               $mess_type = "TRADE CANCELLATION";
               $fm_direct = "FROM CME";
               $sm_direct = "TO CLIENT";
               $efn = &get_exe_firm_num($message_tags{56});
               $session = &get_session_id($message_tags{56}); 
               $big_status = "OK";
               $clordid = $message_tags{11};
           } elsif(defined $mtype && $mtype eq "8" && $mstatus eq "8") {
               $mess_type = "REJECTED";
               $fm_direct = "FROM CME";
               $sm_direct = "TO CLIENT";
               $efn = &get_exe_firm_num($message_tags{56});
               $session = &get_session_id($message_tags{56}); 
               $big_status = "Reject";
               $clordid = $message_tags{11};
           } elsif(defined $mtype && $mtype eq 8 && ($mstatus eq "1" || $mstatus eq "2")) {
               $mess_type = "EXECUTION";
               $fm_direct = "FROM CME";
               $sm_direct = "TO CLIENT";
               $efn = &get_exe_firm_num($message_tags{56});
               $session = &get_session_id($message_tags{56}); 
               $big_status = "OK";
               $clordid = $message_tags{11};
               $quant = $message_tags{32}; 
               $fill_price = $message_tags{31};
           } elsif(defined $mtype && $mtype eq "A") {
                $mess_type = "LOGIN"; 
                my $l_sender = $message_tags{49};
                if(defined $l_sender && $l_sender eq "CME") {
                   $fm_direct = "FROM CME";
                   $sm_direct = "TO CLIENT";
                   $efn = &get_exe_firm_num($message_tags{56});
                   $session = &get_session_id($message_tags{56}); 
                } else {
                   $fm_direct = "FROM CLIENT";
                   $sm_direct = "TO CME";
                   $efn = &get_exe_firm_num($message_tags{49});
                   $session = &get_session_id($message_tags{49}); 
                }
               $big_status = "OK";
               $clordid = $message_tags{11};

           } elsif(defined $mtype && $mtype eq "5") {
                $mess_type = "LOGIN";
                my $l_sender = $message_tags{49};
                if(defined $l_sender && $l_sender eq "CME") {
                   $fm_direct = "FROM CME";
                   $sm_direct = "TO CLIENT";
                   $efn = &get_exe_firm_num($message_tags{56});
                   $session = &get_session_id($message_tags{56}); 
                } else {
                   $fm_direct = "FROM CLIENT";
                   $sm_direct = "TO CME";
                   $efn = &get_exe_firm_num($message_tags{49});
                   $session = &get_session_id($message_tags{49}); 
                }
               $big_status = "Reject";

           }else {
               return;
           }
           my $price = $message_tags{44};
           if(!defined $price) {
               $price = "";
           }
           my $stop_price = $message_tags{99};
           if(!defined $stop_price) {
               $stop_price = "";
           }
           
           if(!defined $clordid) {
               $clordid = "";
           }
           if(!defined $quant) {
               $quant = $message_tags{38};
               if(!defined $quant) {
                   $quant = "";
               }
           } 
           my $max_show = $message_tags{210};
           if(!defined $max_show) {
               $max_show = "";
           }
           my $tif = &get_tif($message_tags{59});
        
           my $mat_date;
           my $strike;
           my $product;
           my $sec_desc = $message_tags{107};
           my $sec_type = $message_tags{167};
           if(!defined $sec_type) {
               $sec_type = "";
           }
           my $ord_type = $message_tags{40};
           if(!defined $ord_type) {
               $ord_type = "";
           }
           if(!defined $sec_desc) {
                $mat_date = "";
                $strike = "";
                $product = "";
                $sec_desc = "";
           } elsif(defined $sec_type && $sec_type eq "OPT") {
                $strike = &get_strike($sec_desc);
                $product = &get_product($sec_desc);
                $mat_date = &get_maturity($sec_desc);
           } else {
                $strike = "";
                $product = &get_product($sec_desc);
                $mat_date = &get_maturity($sec_desc);
           }
           my $error_string = $message_tags{58};
           if(!defined $error_string) {
               $error_string = "";
           }
           my $cti = $message_tags{9702};
           if(!defined $cti) {
               $cti = "";
           }
           my $origin = $message_tags{204};
           if(!defined $origin) {
               $origin = "";
           }
           my $account = $message_tags{1};
           if(!defined $account) {
               $account = "";
           }
           my $man_ind = $message_tags{1028};
           if(!defined $man_ind) {
               $man_ind = "";
           }
           my $sender_sub = $message_tags{50};
           if(!defined $sender_sub) {
               $sender_sub = "";
           }
           my $loc_id = $message_tags{142};
           if(!defined $loc_id) {
               $loc_id = "";
           } else {
               $loc_id = "\"". $loc_id . "\"";
           }
           my $cfi_code = &get_cfi($sec_type);
            my @times = split(/-/, $message_tags{52});
            my $date_str = &create_date_str($times[0]);
            my $output = sprintf( "%d,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,,,,\n",  
                $server_trans_num,         
                $date_str,
                $times[1],
                $loc_id,
                $man_ind,
                "CME",
                $fm_direct,
                $big_status,
                $error_string,
                $sender_sub,
                $account,
                $efn, 
                $session,
                $clordid,
                $corr_id,
                $ex_id,
                $mess_type,
                $bs,
                $quant,
                $max_show,
                $sec_desc,
                $product,
                $mat_date,
                $cfi_code,
                $strike,
                $price,
                $stop_price,
                $fill_price,
                $ord_type,
                $tif,
                $cti,
                $origin);
            $server_trans_num += 1;

      print $FILEH $output;

            my $output2 = sprintf( "%d,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,,,,\n",  
                $server_trans_num,         
                $date_str,
                $times[1],
                $loc_id,
                $man_ind,
                "CME",
                $sm_direct,
                $big_status,
                $error_string,
                $sender_sub,
                $account,
                $efn, 
                $session,
                $clordid,
                $corr_id,
                $ex_id,
                $mess_type,
                $bs,
                $quant,
                $max_show,
                $sec_desc,
                $product,
                $mat_date,
                $cfi_code,
                $strike,
                $price,
                $stop_price,
                $fill_price,
                $ord_type,
                $tif,
                $cti,
                $origin);
            $server_trans_num += 1;

      print $FILEH $output2;

        }
    }
    return;
}

sub get_tif {
    my $tif = shift;
   if(defined $tif && $tif eq "1") {
       "GTC";
   } elsif( defined $tif && $tif eq "3") {
        "FAK";   
   } else {
      "DAY";
  } 
}
sub get_strike {
    my $sym = shift;
    my @parts = split(/ /, $sym);
    substr($parts[1], 1);
}

sub get_product {
    my $sym = shift;
    my $len = length($sym);
    if($sym =~ m/( |-)/g ) {
        my $pos = pos($sym);
        my $out = substr($sym, 0, ($pos -1));
        substr($out, 0, (length($out) -2));
    } else {
        substr($sym, 0, ($len -2));
    }
}

sub get_maturity {
    my $sym = shift;
    my $ystr;
    my $month;
    my $now_y = substr($cur_date, 3,1);
    if($sym =~ m/( |-)/g ) {
        my $pos = pos($sym);
        my $out = substr($sym, 0, ($pos -1));
        $ystr = substr($out, (length($out) -1), 1);
        $month = $fix_constants::month_codes{substr($out, (length($out) -2),1)};
    } else {
        $ystr = substr($sym, (length($sym) -1),1);
        $month = $fix_constants::month_codes{substr($sym, (length($sym) -2),1)};
    }
    if($ystr >= $now_y) {
        sprintf("%s%s-%s", substr($cur_date, 0, 3), $ystr, $month);
    }
}

sub get_cfi {
    my $sec_type = shift;
    if(defined $sec_type && $sec_type eq "FUT") {
        "FXXXSX";
    } elsif(defined $sec_type && $sec_type eq "OPT") {
        "OXXXPS"; 
    } else{
        "";
    }
}

sub create_date_str {
    my $raw = shift;
    my $year = substr($raw, 0, 4);
    my $mon = substr($raw, 4, 2);
    my $day = substr($raw, 6,2);
    sprintf("%s-%s-%s", $year, $mon, $day);
}

sub get_session_id {
    my $s = shift;
    substr($s,0,3);
}

sub get_exe_firm_num {
    my $s = shift;
    substr($s,3,3);
}

sub is_order_related {
    my $arg=shift;
    if ($arg =~ /35=(.+?)\x1/) {
	if ($1 eq "D" || $1 eq "G" || $1 eq "F" || $1 eq "8" || $1 eq "3" || $1 eq "A" || $1 eq "5") {
	    return 1;
	}  else {
	    return 0;
	}
    }
    return 0;
}

1;
