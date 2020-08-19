#ifndef __BATS_BINARY_FIELDS_H__
#define __BATS_BINARY_FIELDS_H__

/* Fixed Fields */
#define StartOfMessage          { BinaryHex,   2, "StartOfMessage"          }
#define MessageLength           { Binary,      2, "MessageLength"           }
#define MessageType             { BinaryHex,   1, "MessageType"             }
#define MatchingUnit            { Binary,      1, "MatchingUnit"            }
#define SequenceNumber          { Binary,      4, "SequenceNumber"          }
#define LoginResponseStatus     { Text,        1, "LoginResponseStatus"     }
#define LoginResponseText       { Text,       60, "LoginResponseText"       }
#define NoUnspecifiedUnitReplay { Binary,      2, "NoUnspecifiedUnitReplay" }
#define ClOrdID                 { Text,       20, "ClOrdID"                 }
#define TransactionTime         { DateTime,    8, "TransactionDateTime"     }
#define OrderID                 { Binary,      8, "OrderID"                 }
#define OrderRejectReason       { Text,        1, "OrderRejectReason"       }
#define TextField               { Text,       60, "Text"                    }
#define RestatementReason       { Text,        1, "RestatementReason"       }
#define ModifyRejectReason      { Text,        1, "ModifyRejectReason"      }
#define CancelReason            { Text,        1, "CancelReason"            }
#define CancelRejectReason      { Text,        1, "CancelRejectReason"      }
#define ExecID                  { Binary,      8, "ExecID"                  }
#define SubLiquidityIndicator   { Text,        1, "SubLiquidityIndicator"   }
#define AccessFee               { Binary,      8, "AccessFee"               }
#define ContraBroker            { Text,        4, "ContraBroker"            }
#define ExecRefID               { Binary,      8, "ExecRefID"               }
#define CorrectedPrice          { BinaryPrice, 8, "CorrectedPrice"          }
#define OrigTime                { DateTime,    8, "OrigTime"                }
#define SessionSubID            { Text,        4, "SessionSubID"            }
#define Username                { Text,        4, "Username"                }
#define Password                { Text,       10, "Password"                }
#define LogoutReason            { Text,        1, "LogoutReason"            }
#define LogoutReasonText        { Text,       60, "LogoutReasonText"        }
#define LastReceivedSequenceNumber { Binary,   4, "LastReceivedSequenceNumber" }
#define NumberOfUnits           { Binary,      1, "NumberOfUnits"           }

#define MessageHeader                           \
    StartOfMessage,                             \
    MessageLength,                              \
    MessageType,                                \
    MatchingUnit,                               \
    SequenceNumber

/* Optional Fields */
#define Account                { Text, 16, "Account"                }
#define AttributedQuote        { Text,  1, "AttributedQuote"        }
#define BaseLiquidityIndicator { Text,  1, "BaseLiquidityIndicator" }
#define CancelOrigOnReject     { Text,  1, "CancelOrigOnReject"     }
#define Capacity               { Text,  1, "Capacity"               }
#define ClearingAccount        { Text,  4, "ClearingAccount"        }
#define ClearingFirm { Text, 4, "ClearingFirm" }
#define DiscretionAmount { Binary, 2, "DiscretionAmount" }
#define DisplayIndicator { Text, 1, "DisplayIndicator" }
#define DisplayPrice { BinaryPrice, 8, "DisplayIndicator" }
#define ExecInst { Text, 1, "ExecInst" }
#define ExpireTime { DateTime, 8, "ExpireTime" }
#define LastPx { BinaryPrice, 8, "LastPx" }
#define LastShares { Binary, 4, "LastShares" }
#define LeavesQty { Binary, 4, "LeavesQty" }
#define LocateReqd { Text, 1, "LocateReqd" }
#define MaxFloor { Binary, 4, "MaxFloor" }
#define MaxRemovePct { Binary, 1, "MaxRemovePct" }
#define MinQty { Binary, 4, "MinQty" }
#define OrderQty { Binary, 4, "OrderQty" }
#define OrdType { Text, 1, "OrdType" }
#define OrigClOrdID { Text, 20, "OrigClOrdID" }
#define PegDifference { BinaryPrice, 8, "PegDifference" }
#define PreventMemberMatch { Text, 3, "PreventMemberMatch" }
#define Price { BinaryPrice, 8, "Price" }
#define RoutingInst { Text, 4, "RoutingInst" }
#define SecondaryOrderID { Binary, 8, "SecondaryOrderID" }
#define Side          { Text,        1, "Side"                    }
#define Symbol        { Text,        8, "Symbol"                  }
#define SymbolSfx     { Text,        8, "SymbolSfx"               }
#define TimeInForce { Text, 1, "TimeInForce" }
#define WorkingPrice { BinaryPrice, 8, "WorkingPrice" }
#define MaturityDate { Binary, 4, "MaturityDate" }
#define StrikePrice { BinaryPrice, 8, "StrikePrice" }
#define PutOrCall { Text, 1, "PutOrCall" }
#define RiskReset { Text, 8, "RiskReset" }
#define OpenClose { Text, 1, "OpenClose" }
#define CMTANumber { Binary, 4, "CMTANumber" }
#define MassCancel { Text, 1, "MassCancel" }
#define OsiRoot { Text, 6, "OsiRoot" }
#define MassCancelID { Text, 20, "MassCancelID" }
#define ClOrdIDBatch { Text, 20, "ClOrdIDBatch" }
#define ContraCapacity { Text, 1, "ContraCapacity" }
#define CorrectedSize { Binary, 4, "CorrectedSize" }

#define Reserved(nbits)      { None,        (nbits), "Reserved"                }

#endif
