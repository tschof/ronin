#!/usr/bin/perl -w
use strict;
use Time::Local;
require fix_constants;
my $cur_date = shift;
my $iscentral = 0;
my $server_trans_num = 1;
my %mkt_seg;
&get_mkt_seg();
my %cancels;
my %ex_ids;
my %ord_ids;
my %ml_ids;
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
                my $cl_ord_id = substr($big_sub, 0, ($pos3 -1));
                $fix_message =~ m/\b37=/g;
                my $pos = pos($fix_message);
                my $big_num = substr($fix_message, $pos);
                $big_num =~ m/\x01/g;
                my $pos1 = pos($big_num);
                my $ord_id = substr($big_num, 0, ($pos1 -1));
                $ord_ids{$cl_ord_id} = $ord_id;
                $fix_message =~ m/\b17=/g;
                my $pos4 = pos($fix_message);
                my $big_num1 = substr($fix_message, $pos4);
                $big_num1 =~ m/\x01/g;
                my $pos5 = pos($big_num1);
                my $ex_id = substr($big_num1, 0, ($pos5 -1));
                $ex_ids{$cl_ord_id} = $ex_id;
            }
        } else {
            my @lroms = split(/,/, $fix_message);
            if($lroms[0] =~ /  S/) {
                if(defined $lroms[15] && defined $lroms[17]) {
                    $cancels{$lroms[15]} = $lroms[17];
                    $ml_ids{$lroms[15]} = [$lroms[17],1];
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
            my $ord_status;
            my $fm_direct;
            my $sm_direct;
            my $fm_efn = "";
            my $sm_efn = "";
            my $fm_session = "";
            my $sm_session = "";
            my $man_ind = $message_tags{1028};
            if(!defined $man_ind) {
                $man_ind = "";
            }
            my $fm_man_ind = $man_ind;
            my $sm_man_ind = $man_ind;
            my $cti = $message_tags{9702};
            if(!defined $cti) {
                $cti = "";
            }
            my $fm_cti = $cti;
            my $sm_cti = $cti;
            my $origin = $message_tags{204};
            if(!defined $origin) {
                $origin = "";
            }
            my $ifm = "";
            my $fm_origin = $origin;
            my $sm_origin = $origin;
            my $mtype = $message_tags{35};
            my $mstatus = $message_tags{150};
            my $clordid = $message_tags{11}; 
            my $last_qty = $message_tags{32};
            if(!defined $last_qty) {
                $last_qty = "";
            }
            my $cum_qty = $message_tags{14};
            if(!defined $cum_qty) {
                $cum_qty = "";
            }
            my $lvs_qty = $message_tags{151};
            if(!defined $lvs_qty) {
                $lvs_qty = "";
            }
            my $fm_exe_restate_reason = $message_tags{378};
            my $sm_exe_restate_reason = $message_tags{378};
            if(!defined $fm_exe_restate_reason) {
                $fm_exe_restate_reason = "";
                $sm_exe_restate_reason = "";
            }
            my $fm_cross_id = $message_tags{548};
            my $sm_cross_id = $message_tags{548};
            my $sender_sub = $message_tags{50};
            if(!defined $fm_cross_id) {
                $fm_cross_id = "";
                $sm_cross_id = "";
            }
            my $fill_price = "";
            my $bs = $message_tags{54};
            my $fm_ord_id;
            my $sm_ord_id;
            if(defined $clordid) {
                $fm_ord_id = $ord_ids{$clordid};
                $sm_ord_id = $ord_ids{$clordid};
                if(!defined $fm_ord_id) {
                    $fm_ord_id = $cancels{$clordid};
                    $sm_ord_id = $cancels{$clordid};
                } 
                if(!defined $fm_ord_id) {
                    $fm_ord_id = "NONE_FOUND";
                    $sm_ord_id = "NONE_FOUND";
                }
            } else {
                $fm_ord_id = "";
                $sm_ord_id = "";
            }
            my $fm_ex_id = $message_tags{17};
            my $sm_ex_id = $message_tags{17};
            my $smpid = $message_tags{7928};
            if(!defined $smpid) {
                $smpid = "";
            }
            my $fm_smpid = $smpid;
            my $sm_smpid = $smpid;
            my $sec_desc = $message_tags{107};
            my $fm_mkt_seg = "";
            my $sm_mkt_seg = "";
            my @times = split(/-/, $message_tags{52});
            my $send_date_str = &create_date_str($times[0]);
            $send_date_str = sprintf(",%s-%s",$send_date_str,$times[1]);
            @times = split(/-/, $message_tags{60});
            my $trans_date_str = &create_date_str($times[0]);
            $trans_date_str = sprintf("%s-%s,",$trans_date_str,$times[1]);
            my $fm_date_str = "";
            my $sm_date_str = "";
            if(defined $mtype && $mtype eq "D") {
                $mess_type = "NEW ORDER"; 
                $ord_status = "";
                $fm_direct = "FROM CLIENT";
                $sm_direct = "TO CME";
                $fm_smpid = "";
                $fm_man_ind = "";
                $fm_ord_id = "";
                $fm_ex_id = "";
                $fm_cti = "";
                $fm_origin = "";
                $fm_date_str = $send_date_str;
                $sm_date_str = $trans_date_str;
                $fm_exe_restate_reason = "";
                $fm_cross_id = "";
                $sm_mkt_seg = $mkt_seg{$sec_desc};
                $sm_efn = &get_exe_firm_num($message_tags{49});
                $sm_session = &get_session_id($message_tags{49}); 
                $clordid = $message_tags{11};
            } elsif (defined $mtype && $mtype eq "G") {
                $mess_type = "MODIFY";
                $ord_status = "";
                $fm_direct = "FROM CLIENT";
                $sm_direct = "TO CME";
                $fm_smpid = "";
                $fm_man_ind = "";
                $fm_ord_id = "";
                $fm_ex_id = "";
                $fm_cti = "";
                $fm_origin = "";
                $fm_exe_restate_reason = "";
                $fm_cross_id = "";
                $ifm = "Y";
                $fm_date_str = $send_date_str;
                $sm_date_str = $trans_date_str;
                $sm_mkt_seg = $mkt_seg{$sec_desc};
                $sm_efn = &get_exe_firm_num($message_tags{49});
                $sm_session = &get_session_id($message_tags{49}); 
                $clordid = $message_tags{11};
            } elsif (defined $mtype && $mtype eq "F") {
                $mess_type = "CANCEL";
                $ord_status = "";
                $fm_direct = "FROM CLIENT";
                $sm_direct = "TO CME";
                $fm_smpid = "";
                $fm_man_ind = "";
                $fm_ord_id = "";
                $fm_ex_id = "";
                $fm_cti = "";
                $ifm = "Y";
                $fm_origin = "";
                $fm_exe_restate_reason = "";
                $fm_cross_id = "";
                $fm_date_str = $send_date_str;
                $sm_date_str = $trans_date_str;
                $sm_mkt_seg = $mkt_seg{$sec_desc};
                $sm_efn = &get_exe_firm_num($message_tags{49});
                $sm_session = &get_session_id($message_tags{49}); 
                $clordid = $message_tags{41};
            } elsif (defined $mtype && $mtype eq "3") {
                $mess_type = "REJECTED";
                $ord_status = "REJECTED";
                $fm_direct = "FROM CME";
                $sm_direct = "TO CLIENT";
                $sm_smpid = "";
                $sm_man_ind = "";
                $sm_ord_id = "";
                $sm_ex_id = "";
                $sm_cti = "";
                $sm_origin = "";
                $sm_exe_restate_reason = "";
                $sm_cross_id = "";
                $sm_date_str = $send_date_str;
                $fm_date_str = $trans_date_str;
                $fm_mkt_seg = $mkt_seg{$sec_desc};
                $sender_sub = $message_tags{57};
                $fm_efn = &get_exe_firm_num($message_tags{56});
                $fm_session = &get_session_id($message_tags{56}); 
            } elsif(defined $mtype && $mtype eq "8" && ($mstatus eq "4" || $mstatus eq "C")) {
                $mess_type = "EXECUTION";
                if($mstatus eq "4") {
                    $ord_status = "CANCEL ACK";
                    $clordid = $message_tags{11};
                } else {
                    $ord_status = "EXPIRED";
                }
                $fm_direct = "FROM CME";
                $sm_direct = "TO CLIENT";
                $sm_smpid = "";
                $sm_man_ind = "";
                $sm_ord_id = "";
                $sm_ex_id = "";
                $sm_cti = "";
                $sm_origin = "";
                $sm_exe_restate_reason = "";
                $sm_cross_id = "";
                $ifm = "Y";
                $sm_date_str = $send_date_str;
                $fm_date_str = $trans_date_str;
                $fm_mkt_seg = $mkt_seg{$sec_desc};
                $sender_sub = $message_tags{57};
                $fm_efn = &get_exe_firm_num($message_tags{56});
                $fm_session = &get_session_id($message_tags{56}); 
                
                if(!defined $clordid) {
                    my $clordid = $message_tags{11};
                }
            }  elsif(defined $mtype && $mtype eq "8" && $mstatus eq "0") {
                $mess_type = "EXECUTION";
                $ord_status = "NEW ORDER ACK";
                $fm_direct = "FROM CME";
                $sm_direct = "TO CLIENT";
                $sm_smpid = "";
                $sm_man_ind = "";
                $sm_ord_id = "";
                $sm_ex_id = "";
                $sm_cti = "";
                $sm_origin = "";
                $sm_exe_restate_reason = "";
                $sm_cross_id = "";
                $sm_date_str = $send_date_str;
                $fm_date_str = $trans_date_str;
                $fm_mkt_seg = $mkt_seg{$sec_desc};
                $fm_efn = &get_exe_firm_num($message_tags{56});
                $fm_session = &get_session_id($message_tags{56}); 
                $clordid = $message_tags{11};
                $sender_sub = $message_tags{57};
            }  elsif(defined $mtype && $mtype eq "8" && $mstatus eq "5") {
                $mess_type = "EXECUTION";
                $ord_status = "MODIFY ACK";
                $fm_direct = "FROM CME";
                $sm_direct = "TO CLIENT";
                $sm_smpid = "";
                $sm_man_ind = "";
                $sm_ord_id = "";
                $sm_ex_id = "";
                $sm_cti = "";
                $sm_origin = "";
                $ifm = "Y";
                $sm_exe_restate_reason = "";
                $sm_cross_id = "";
                $sm_date_str = $send_date_str;
                $fm_date_str = $trans_date_str;
                $fm_mkt_seg = $mkt_seg{$sec_desc};
                $fm_efn = &get_exe_firm_num($message_tags{56});
                $fm_session = &get_session_id($message_tags{56}); 
                $sender_sub = $message_tags{57};
                $clordid = $message_tags{11};
            } elsif(defined $mtype && $mtype eq "8" && $mstatus eq "H") {
                $mess_type = "EXECUTION";
                $ord_status = "TRADE CANCEL";
                $fm_direct = "FROM CME";
                $sm_direct = "TO CLIENT";
                $sm_smpid = "";
                $sm_man_ind = "";
                $sm_ord_id = "";
                $sm_ex_id = "";
                $sm_cti = "";
                $sm_origin = "";
                $sm_exe_restate_reason = "";
                $sm_cross_id = "";
                $sm_date_str = $send_date_str;
                $fm_date_str = $trans_date_str;
                $fm_mkt_seg = $mkt_seg{$sec_desc};
                $fm_efn = &get_exe_firm_num($message_tags{56});
                $fm_session = &get_session_id($message_tags{56}); 
                $sender_sub = $message_tags{57};
                $clordid = $message_tags{11};
            } elsif(defined $mtype && $mtype eq "8" && $mstatus eq "8") {
                $mess_type = "EXECUTION";
                $ord_status = "REJECTED";
                $fm_direct = "FROM CME";
                $sm_direct = "TO CLIENT";
                $sm_smpid = "";
                $sm_man_ind = "";
                $sm_ord_id = "";
                $sm_ex_id = "";
                $sm_cti = "";
                $sm_origin = "";
                $sm_exe_restate_reason = "";
                $sm_cross_id = "";
                $sm_date_str = $send_date_str;
                $fm_date_str = $trans_date_str;
                $fm_mkt_seg = $mkt_seg{$sec_desc};
                $fm_efn = &get_exe_firm_num($message_tags{56});
                $fm_session = &get_session_id($message_tags{56}); 
                $sender_sub = $message_tags{57};
                $clordid = $message_tags{11};
            } elsif(defined $mtype && $mtype eq 8 && ($mstatus eq "1" || $mstatus eq "2")) {
                $mess_type = "EXECUTION";
                if($mstatus eq "1") {
                    $ord_status = "PARTIAL FILL";
                } else {
                    $ord_status = "COMPLETE FILL";
                }
                $fm_direct = "FROM CME";
                $sm_direct = "TO CLIENT";
                $sm_smpid = "";
                $sm_man_ind = "";
                $sm_ord_id = "";
                $sm_ex_id = "";
                $sm_cti = "";
                $sm_origin = "";
                $sm_exe_restate_reason = "";
                $sm_cross_id = "";
                $sm_date_str = $send_date_str;
                $fm_date_str = $trans_date_str;
                $fm_mkt_seg = $mkt_seg{$sec_desc};
                $fm_efn = &get_exe_firm_num($message_tags{56});
                $fm_session = &get_session_id($message_tags{56}); 
                $clordid = $message_tags{11};
                #$quant = $message_tags{32}; 
                $sender_sub = $message_tags{57};
                $fill_price = $message_tags{31};
            } elsif(defined $mtype && $mtype eq "A") {
                $mess_type = "LOGIN"; 
                $ord_status = "";
                my $l_sender = $message_tags{49};
                if(defined $l_sender && $l_sender eq "CME") {
                    $fm_direct = "FROM CME";
                    $sm_direct = "TO CLIENT";
                    $sm_smpid = "";
                    $sm_man_ind = "";
                    $sm_ord_id = "";
                    $sm_ex_id = "";
                    $sm_cti = "";
                    $sm_origin = "";
                    $sm_exe_restate_reason = "";
                    $sm_cross_id = "";
                    $sm_date_str = $send_date_str;
                    $fm_date_str = $send_date_str;
                    $fm_efn = &get_exe_firm_num($message_tags{56});
                    $fm_session = &get_session_id($message_tags{56}); 
                } else {
                    $fm_direct = "FROM CLIENT";
                    $sm_direct = "TO CME";
                    $fm_smpid = "";
                    $fm_man_ind = "";
                    $fm_ord_id = "";
                    $fm_ex_id = "";
                    $fm_cti = "";
                    $fm_origin = "";
                    $fm_exe_restate_reason = "";
                    $fm_cross_id = "";
                    $fm_date_str = $send_date_str;
                    $sm_date_str = $send_date_str;
                    $sm_efn = &get_exe_firm_num($message_tags{49});
                    $sm_session = &get_session_id($message_tags{49}); 
                }
                $clordid = $message_tags{11};

            } elsif(defined $mtype && $mtype eq "5") {
                $mess_type = "LOGIN";
                $ord_status = "";
                my $l_sender = $message_tags{49};
                if(defined $l_sender && $l_sender eq "CME") {
                    $fm_direct = "FROM CME";
                    $sm_direct = "TO CLIENT";
                    $sm_smpid = "";
                    $sm_man_ind = "";
                    $sm_ord_id = "";
                    $sm_ex_id = "";
                    $sm_cti = "";
                    $sm_origin = "";
                    $sm_exe_restate_reason = "";
                    $sm_cross_id = "";
                    $sm_date_str = $send_date_str;
                    $fm_date_str = $send_date_str;
                    $fm_efn = &get_exe_firm_num($message_tags{56});
                    $sender_sub = $message_tags{57};
                    $fm_session = &get_session_id($message_tags{56}); 
                } else {
                    $fm_direct = "FROM CLIENT";
                    $sm_direct = "TO CME";
                    $fm_smpid = "";
                    $fm_man_ind = "";
                    $fm_ord_id = "";
                    $fm_ex_id = "";
                    $fm_cti = "";
                    $fm_origin = "";
                    $fm_exe_restate_reason = "";
                    $fm_cross_id = "";
                    $fm_date_str = $send_date_str;
                    $sm_date_str = $send_date_str;
                    $sm_efn = &get_exe_firm_num($message_tags{49});
                    $sm_session = &get_session_id($message_tags{49}); 
                    $sender_sub = $message_tags{57};
                }

            }else {
                return;
            }
            my $message_type = "";
            if(defined $mtype){
                $message_type = $mtype;
                if(defined $mstatus){
                    $message_type .= "/$mstatus";
                }
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
            my $quant = $message_tags{38};
            if(!defined $quant) {
                    $quant = "";
            }
             
            my $max_show = $message_tags{210};
            if(!defined $max_show) {
                $max_show = "";
            }
            my $min_qty = $message_tags{110};
            if(!defined $min_qty) {
                $min_qty = "";
            }
            my $tif = &get_tif($message_tags{59});

            my $mat_date;
            my $product;
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
                $product = "";
                $sec_desc = "";
            } elsif(defined $sec_type && $sec_type eq "OPT") {
                $product = &get_product($sec_desc);
                $mat_date = &get_maturity($sec_desc);
            } else {
                $product = &get_product($sec_desc);
                $mat_date = &get_maturity($sec_desc);
            }
            my $error_string = $message_tags{58};
            if(!defined $error_string) {
                $error_string = "";
            }
            my $account = $message_tags{1};
            if(!defined $account) {
                $account = "";
            }
            if(!defined $sender_sub) {
                $sender_sub = $message_tags{50};
                if(!defined $sender_sub) {
                    $sender_sub = "";
                }
            }

            my $country = $message_tags{142};
            if(!defined $country) {
                $country = "";
            }else{
                $country =~ s/,.*//g;
            }
            
            my $aggressor = $message_tags{1057};
            if(!defined $aggressor) {
                $aggressor = "";
            }
            my $cfi_code = &get_cfi($sec_type);
            my $mlid = $ml_ids{$clordid}[0];
            my $ofid = $mlid;
            if($ofid ne ""){
                $ofid .= "-$ml_ids{$clordid}[1]";
            }
            #my $output = sprintf( "%d,%s-%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,,,,,,,,,,,,,,,,,,,,\n",  
            my $output = sprintf( "%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,,%s,,,,,,,\n",    
                #$server_trans_num,         
                $fm_date_str,
                $fm_direct,
                $sender_sub,
                $fm_smpid,
                $account,
                $fm_session,
                $fm_efn, 
                $fm_man_ind,
                $message_type,
                $fm_cti,
                $fm_origin,
                $fm_ex_id,
                $ofid,
                $mlid,
                $sec_desc,
                $fm_mkt_seg,
                $clordid,
                $fm_ord_id,               
                $bs,
                $quant,
                $price,
                $stop_price,
                $ord_type,
                $tif,
                $ifm,
                $max_show,
                $min_qty,
                $country,
                $fill_price,
                $last_qty,
                $cum_qty,
                $lvs_qty,
                $aggressor,
                $fm_exe_restate_reason,
                $error_string,
                $fm_cross_id
                #$ord_status,
                #$product,
                #$mat_date,
                #$cfi_code,
                );
                #$server_trans_num += 1;

            print $FILEH $output;
            #my $output2 = sprintf( "%d,%s-%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,,,,,,,,,,,,,,,,,,,,\n",
            my $output2 = sprintf( "%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,,%s,,,,,,,\n",  
                #$server_trans_num,         
                $sm_date_str,
                $sm_direct,
                $sender_sub,
                $sm_smpid,
                $account,
                $sm_session,
                $sm_efn, 
                $sm_man_ind,
                $message_type,
                $sm_cti,
                $sm_origin,
                $sm_ex_id,
                $ofid,
                $mlid,
                $sec_desc,
                $sm_mkt_seg,
                $clordid,
                $sm_ord_id,
                $bs,
                $quant,
                $price,
                $stop_price,
                $ord_type,
                $tif,
                $ifm,
                $max_show,
                $min_qty,
                $country,
                $fill_price,
                $last_qty,
                $cum_qty,
                $lvs_qty,
                $aggressor,
                $sm_exe_restate_reason,
                $error_string,
                $sm_cross_id
                #$ord_status,
               
                #$product,
                #$mat_date,
                #$cfi_code,
                
                
                
                
                );
                #$server_trans_num += 1;
            $ml_ids{$clordid}[1] += 1;
            print $FILEH $output2;

        }
    }
    return;
}

sub get_tif {
    my $tif = shift;
    if(defined $tif && $tif eq "1") {
        $tif;
    } elsif( defined $tif && $tif eq "3") {
        $tif;   
    } else {
        '0';
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
    sprintf("%s%s%s", $year, $mon, $day);
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

sub get_mkt_seg{
use Net::FTP;
use Archive::Extract;    
    my $ftp = Net::FTP->new("ftp.cmegroup.com", Debug => 0, Timeout => 6000)
     or die "Cannot connect to ftp.cmegroup.com: $@";
    $ftp->login("anonymous",'-anonymous@')
      or die "Cannot login ", $ftp->message;
    $ftp->cwd("/SBEFix/Production")
       or die "Cannot change working directory ", $ftp->message;
    $ftp->binary;
    $ftp->get("secdef.dat.gz")
        or die "get failed ", $ftp->message;
    $ftp->quit;
    my $ae = Archive::Extract->new( archive => 'secdef.dat.gz' );
    my $ok = $ae->extract or die $ae->error;
    my $filesize = -s "secdef.dat";
    sleep(1);
    if(-s "secdef.dat" ne $filesize){
        while (-s "secdef.dat" ne $filesize){
            $filesize = -s "secdef.dat";
            sleep(1);
        }
    }
    my @files = <secdef.dat>;
    foreach my $file (@files) {
        open (IN, "<$file") or die "cannot open $file\n";
        while(<IN>) {
            chomp;
            my $sline = $_;
            if($sline =~ m/35=d/){
                my $index = index $sline, '55=';
                my $sym = substr $sline, $index + 3;
                $index = index $sym, chr(1);
                $sym = substr $sym, 0, $index;
                $index = index $sline, '1300=';
                my $mkt_seg_id = substr $sline, $index + 5;
                $index = index $mkt_seg_id, chr(1);
                $mkt_seg_id = substr $mkt_seg_id, 0, $index;
                $mkt_seg{$sym} = $mkt_seg_id;
            }
        }
    }
    unlink "secdef.dat.gz";
    unlink "secdef.dat";
    $mkt_seg{"ESH6"} = "64";
}
1;
