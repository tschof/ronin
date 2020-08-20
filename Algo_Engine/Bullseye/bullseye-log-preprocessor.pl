#!/usr/bin/perl -w
use strict;

my %mappings =
(
'L' =>

  {
    Name => 'LOGIN',
       0 => 'Command',
       1 => 'Time',
       2 => 'LoginName',
       3 => 'CancelOption',
       4 => 'CancelWait',
       5 => 'AppName',
       6 => 'AppVersion',
       7 => 'Password',
      72 => 'NextExpectedSeqNo',
  },

'K' =>
  {
    Name => 'LOGIN_REJECT',
      26 => 'Text',
  },

'Q' =>
  {
    Name => 'LOGIN_RESEND_FINISHED',
       0 => 'Command',
       1 => 'Time',
  },

'E' =>
  {
    Name => 'ORDER',
       0 => 'Command',
       1 => 'Time',
       2 => 'Trader',
       3 => 'ClientOrderTag',
       4 => 'Side',
       5 => 'Symbol',
       6 => 'Shares',
       7 => 'LimitPrice',
       8 => 'OrderType',
       9 => 'TimeInForce',
      10 => 'Capacity',
      11 => 'ClearingID',
      12 => 'ClearingAccount',
      13 => 'Exchange',
      14 => 'Status',
      15 => 'Instructions',
      17 => 'StaffordTag',
      18 => 'MinQty',
      20 => 'Discretionary',
      21 => 'Solicited',
      22 => 'LocalAccount',
      23 => 'Firm',
      24 => 'TradeFor',
      26 => 'DBSymbol',
      27 => 'Owner',
      28 => 'Multiplier',
      35 => 'SecurityType',
      37 => 'MarketMakerExchange',
      41 => 'OrderDest',
      48 => 'CumShares',
      49 => 'LeaveShares',
      51 => 'AvgPrice',
      52 => 'OMTime',
      56 => 'Currency',
      66 => 'CPLXOrderType',
      73 => 'AlgoType',
      76 => 'StartTime',
      77 => 'EndTime',
  },

'S' =>
  {
    Name => 'STATUS',
       0 => 'Command',
       1 => 'Time',
       2 => 'Trader',
       3 => 'ClientOrderTag',
       4 => 'Side',
       5 => 'Symbol',
       6 => 'Shares',
       7 => 'LimitPrice',
       8 => 'OrderType',
       9 => 'TimeInForce',
      10 => 'Exchange',
      14 => 'Status',
      17 => 'StaffordTag',
      23 => 'Contra',
      24 => 'TradeFor',
      26 => 'Text',
      35 => 'SecurityType',
      39 => 'TradingSession',
      42 => 'LastShares',
      43 => 'OriginalShares',
      44 => 'ExecPrice',
      48 => 'CumShares',
      49 => 'LeaveShares',
      50 => 'ExecTime',
      51 => 'AvgPrice',
      52 => 'OMTime',
      54 => 'ExchangeTag',
      69 => 'LiquidityIndicator',
  },

);

my %enumerations =
(
 8 =>
  {
     1 => 'Market',
     2 => 'Limit',
     3 => 'Stop',
     4 => 'StopLimit',
     5 => 'MarketOnClose',
     6 => 'WithOrWithout',
     7 => 'LimitOrBetter',
     8 => 'LimitWithOrWithout',
     9 => 'OnBasis',
    10 => 'OnClose',
    11 => 'LimitOnClose',
    12 => 'MarketAtOpen',
    14 => 'VWAP',
   'K' => 'MarketableLimit',
  },

 9 =>
  {
     0 => 'Day',
     1 => 'GTC',
     2 => 'OPG',
     3 => 'IOC',
     4 => 'FOK',
     5 => 'GTX',
     6 => 'GTD',
     7 => 'ExtendedDay',
  },

10 =>
  {
    'A' => 'Agent',
    'P' => 'Principal',
    'S' => 'MarketMaker',
    'R' => 'BrokerDealerClearingCustodianAtOCC',
    'B' => 'BrokerDealerClearingFirmAtOCC',
    'C' => 'Customer',
  },

13 =>
  {
     0 => 'ARCA',
     1 => 'INET',
     2 => 'NYSE',
     3 => 'ISLD (INET)',
     4 => 'AMEX',
     6 => 'GLOBEX',
     8 => 'ESP',
     9 => 'INET_GSEC',
    10 => 'eCBOT',
    11 => 'eCBOT_2',
    19 => 'ISLD_GSEC (INET)',
    21 => 'ISE',
    22 => 'CBOE',
    23 => 'ARCA_GSEC',
    24 => 'BATS',
    25 => 'ARCA_2GS',
    27 => 'CHX',
    28 => 'GLOBEX_2',
    29 => 'BATS_GSEC',
    34 => 'PCX',
    35 => 'PHLX',
    36 => 'EDGA',
    37 => 'EDGX',
    39 => 'PCX_MPRO',
    40 => 'BOX_MPRO',
    42 => 'AMEX',
    44 => 'BOX',
    47 => 'GLOBEX_OPT',
    72 => 'ARCA_D',
    77 => 'Merrill/Broadcort',
    78 => 'OneChicago',
  },

14 =>
  {
     0 => 'New',
     1 => 'PartiallyFilled',
     2 => 'Filled',
     3 => 'DoneForDay',
     4 => 'Canceled',
     5 => 'Replaced',
     6 => 'PendingCancel',
     7 => 'Stopped',
     8 => 'Rejected',
    11 => 'Calculated',
    12 => 'Expired',
    13 => 'Open',
    14 => 'CancelRejected',
    15 => 'Corrected',
    16 => 'Busted',
    20 => 'ReplaceRejected',
    21 => 'ReplacePending',
    22 => 'Exposed(BOX)',
    23 => 'DirectedThruIML(BOX)',
    24 => 'Directed(BOX)',
    25 => 'DirectedReject(BOX)',
    26 => 'ROMCancelPending',
    27 => 'ROMReplacePending',
  },
);

my $multiline = 1;

while (<>) {
    chomp;
    if (/(.*)\{\|(.+)\|\}(.*)/) {
	my @fields = split (/,/, $2);
	if (scalar @fields > 0) {
	    print "$1";
	    if ($multiline) {
		print "\n";
	    }

	    my $command = $fields [0];
	    my $fields_ref = $mappings {$command};
	    if (exists $fields_ref->{Name}) {
		if ($multiline) {
		    print "\t";
		}
		print "$fields_ref->{Name} ";
		if ($multiline) {
		    print "\n";
		}
	    }

	    for (my $i = 0; $i < scalar (@fields); ++$i) {
		next if $fields [$i] eq "";

		if (!$multiline && $i > 0) {
		    print ',';
		}

		if ($multiline) {
		    print "\t";
		}

		my $value = $fields [$i];

		my $enum_int = $value;
		my $enum_ref = $enumerations {$i};
		if (exists ($enum_ref->{$enum_int})) {
		    $value = $enum_ref->{$enum_int};
		    $value .= "($enum_int)";
		}

		if (exists $fields_ref->{$i}) {
		    my $name = $fields_ref->{$i};
		    print "$name($i)=$value";
		}
		else {
		    print "$i=$value";
		}

		if ($multiline) {
		    print "\n";
		}
	    }

	    print "$3";
	}
	print "\n";
    }
    else {
	print "$_\n";
    }
}
