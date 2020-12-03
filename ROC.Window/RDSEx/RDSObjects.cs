using System;
using System.Collections.Generic;

using DictionaryEx;
using System.Diagnostics;
using CSVEx;
using System.Runtime.Serialization;

namespace RDSEx
{
	#region - User Profiles -

	public class DestinationMap : MutiTypedDictionary
	{
		public Nullable<long> destID
		{
			get
			{
				if (Longs.ContainsKey(DestinationFieldIDs.DestID))
				{
					return Longs[DestinationFieldIDs.DestID];
				}
				else
				{
					return null;
				}
			}
		}

		public string secType
		{
			get
			{
				if (Strings.ContainsKey(DestinationFieldIDs.SecType))
				{
					return Strings[DestinationFieldIDs.SecType];
				}
				else
				{
					return null;
				}
			}
		}

		public string shortName
		{
			get
			{
				if (Strings.ContainsKey(DestinationFieldIDs.ShortName))
				{
					return Strings[DestinationFieldIDs.ShortName];
				}
				else
				{
					return null;
				}
			}
		}

		public string giveupFirmID
		{
			get
			{
				if (Strings.ContainsKey(DestinationFieldIDs.GiveupFirmID))
				{
					return Strings[DestinationFieldIDs.GiveupFirmID];
				}
				else
				{
					return null;
				}
			}
		}

		public string cmtaFirmID
		{
			get
			{
				if (Strings.ContainsKey(DestinationFieldIDs.CMTAFirmID))
				{
					return Strings[DestinationFieldIDs.CMTAFirmID];
				}
				else
				{
					return null;
				}
			}
		}

		public string algoFlag
		{
			get
			{
				if (Strings.ContainsKey(DestinationFieldIDs.AlgoFlag))
				{
					return Strings[DestinationFieldIDs.AlgoFlag];
				}
				else
				{
					return null;
				}
			}
		}

		public bool isAlgo
		{
			get
			{
				if (algoFlag != null && algoFlag == "1")
				{
					return true;
				}
				return false;
			}
		}
	}

	public class AccountMap : MutiTypedDictionary
	{
		private Dictionary<int, DestinationMap> _destinations;
		public Dictionary<int, DestinationMap> Destinations
		{
			get
			{
				if (_destinations == null)
				{
					_destinations = new Dictionary<int, DestinationMap>();
				}
				return _destinations;
			}
			set
			{
				_destinations = value;
			}
		}

		public Nullable<long> interfaceID
		{
			get
			{
				if (Longs.ContainsKey(AccountFieldIDs.InterfaceID))
				{
					return Longs[AccountFieldIDs.InterfaceID];
				}
				else
				{
					return null;
				}
			}
		}

		public string account
		{
			get
			{
				if (Strings.ContainsKey(AccountFieldIDs.Account))
				{
					return Strings[AccountFieldIDs.Account];
				}
				else
				{
					return null;
				}
			}
		}

		public string type
		{
			get
			{
				if (Strings.ContainsKey(AccountFieldIDs.Type))
				{
					return Strings[AccountFieldIDs.Type];
				}
				else
				{
					return null;
				}
			}
		}

		public string clearingAcID
		{
			get
			{
				if (Strings.ContainsKey(AccountFieldIDs.ClearingAcID))
				{
					return Strings[AccountFieldIDs.ClearingAcID];
				}
				else
				{
					return null;
				}
			}
		}

		public string clearingAcIDShort
		{
			get
			{
				if (Strings.ContainsKey(AccountFieldIDs.ClearingAcID))
				{
					if (Strings[AccountFieldIDs.ClearingAcID].Length > 5)
					{
						return Strings[AccountFieldIDs.ClearingAcID].Substring(0, 5);
					}
					else
					{
						return Strings[AccountFieldIDs.ClearingAcID];
					}
				}
				else
				{
					return null;
				}
			}
		}

		public string localAcAcrn
		{
			get
			{
				if (Strings.ContainsKey(AccountFieldIDs.LocalAcAcrn))
				{
					return Strings[AccountFieldIDs.LocalAcAcrn];
				}
				else
				{
					return null;
				}
			}
		}

		public string firmAcr
		{
			get
			{
				if (Strings.ContainsKey(AccountFieldIDs.FirmAcr))
				{
					return Strings[AccountFieldIDs.FirmAcr];
				}
				else
				{
					return null;
				}
			}
		}

		public string omAcctType
		{
			get
			{
				if (Strings.ContainsKey(AccountFieldIDs.OmAcctType))
				{
					return Strings[AccountFieldIDs.OmAcctType];
				}
				else
				{
					return null;
				}
			}
		}

		public string capacity
		{
			get
			{
				if (Strings.ContainsKey(AccountFieldIDs.Capacity))
				{
					return Strings[AccountFieldIDs.Capacity];
				}
				else
				{
					return null;
				}
			}
		}

		public string clearingFirmID
		{
			get
			{
				if (Strings.ContainsKey(AccountFieldIDs.ClearingFirmID))
				{
					return Strings[AccountFieldIDs.ClearingFirmID];
				}
				else
				{
					return null;
				}
			}
		}

		public string occAcr
		{
			get
			{
				if (Strings.ContainsKey(AccountFieldIDs.OccAcr))
				{
					return Strings[AccountFieldIDs.OccAcr];
				}
				else
				{
					return null;
				}
			}
		}

		public string homeExchange
		{
			get
			{
				if (Strings.ContainsKey(AccountFieldIDs.HomeExchange))
				{
					return Strings[AccountFieldIDs.HomeExchange];
				}
				else
				{
					return null;
				}
			}
		}
	}

	public class LocalAccountMap : BaseTypedAccountMap
	{
	}

	public class TraderMap : BaseTypedAccountMap
	{
		public string tradeFor
		{
			get
			{
				if (Strings.ContainsKey(TraderFieldIDs.TradeFor))
				{
					return Strings[TraderFieldIDs.TradeFor];
				}
				else
				{
					return "";
				}
			}
		}

		public string localAcAcrn
		{
			get
			{
				if (Strings.ContainsKey(TraderFieldIDs.LocalAcAcrn))
				{
					return Strings[TraderFieldIDs.LocalAcAcrn];
				}
				else
				{
					return "";
				}
			}
		}
	}

	#endregion

	#region - ROC Trades, Orders and Positions - 

	[Serializable]
	public class ROCExecution : BaseExecution
	{
		public override string Symbol
		{
			get
			{
				switch (SecType)
				{
					//case "O":
					//case "I":
					//case "M":
					//    return Underlying;
					default:
						if (Strings.ContainsKey(TradedFieldIDs.ROC.symbol))
						{
							return Strings[TradedFieldIDs.ROC.symbol];
						}
						else
						{
							return "";
						}
				}
			}
			set
			{
				Update(TradedFieldIDs.ROC.symbol, value);
			}
		}

		public override string Underlying
		{
			get
			{
				if (Strings.ContainsKey(TradedFieldIDs.ROC.underlying))
				{
					return Strings[TradedFieldIDs.ROC.underlying];
				}
				else
				{
					return "";
				}
			}
		}

		public override string ExpDate
		{
			get
			{
				if (Strings.ContainsKey(TradedFieldIDs.ROC.expDate))
				{
					if (Underlying != "" && MaturityDay != "")
					{
						// IsOption
						return Strings[TradedFieldIDs.ROC.expDate] + MaturityDay;
					}
					return Strings[TradedFieldIDs.ROC.expDate];
				}
				else
				{
					return "";
				}
			}
		}

		public string MaturityDay
		{
			get
			{
				if (Strings.ContainsKey(TradedFieldIDs.ROC.maturityDay))
				{
					return Strings[TradedFieldIDs.ROC.maturityDay];
				}
				else
				{
					return "";
				}
			}
		}

		public override string StrikePrice
		{
			get
			{
				if (Strings.ContainsKey(TradedFieldIDs.ROC.strikePrice))
				{
					return Strings[TradedFieldIDs.ROC.strikePrice];
				}
				else if (Doubles.ContainsKey(TradedFieldIDs.ROC.strikePrice))
				{
					double sp = Doubles[TradedFieldIDs.ROC.strikePrice];
					Update(TradedFieldIDs.ROC.strikePrice, sp.ToString(GetStrikFormat(sp)));
					return Strings[TradedFieldIDs.ROC.strikePrice];
				}
				else
				{
					return "0";
				}
			}
		}

		public override string CallPut
		{
			get
			{
				if (Strings.ContainsKey(TradedFieldIDs.ROC.callPut))
				{
					return Strings[TradedFieldIDs.ROC.callPut];
				}
				else
				{
					return "";
				}
			}
		}

		public override string SecType
		{
			get
			{
				if (Strings.ContainsKey(TradedFieldIDs.ROC.secType))
				{
					return Strings[TradedFieldIDs.ROC.secType];
				}
				else
				{
					return "";
				}
			}
		}

		public override string Trader
		{
			get
			{
				if (Strings.ContainsKey(TradedFieldIDs.ROC.trader))
				{
					return Strings[TradedFieldIDs.ROC.trader];
				}
				else
				{
					return "";
				}
			}
		}

		public string LocalAcct
		{
			get
			{
				if (Strings.ContainsKey(TradedFieldIDs.ROC.localAcct))
				{
					return Strings[TradedFieldIDs.ROC.localAcct];
				}
				else
				{
					return "";
				}
			}
		}

		public override string ClearingAcct
		{
			get
			{
				if (Strings.ContainsKey(TradedFieldIDs.ROC.account))
				{
					return Strings[TradedFieldIDs.ROC.account];
				}
				else
				{
					return "";
				}
			}
		}

		public override string TradeID
		{
			get
			{
				if (Strings.ContainsKey(TradedFieldIDs.ROC.omExecTag))
				{
					return Strings[TradedFieldIDs.ROC.omExecTag];
				}
				else
				{
					return "";
				}
			}
		}

		public override long Qty
		{
			get
			{
				if (Longs.ContainsKey(TradedFieldIDs.ROC.execQty))
				{
					return Longs[TradedFieldIDs.ROC.execQty];
				}
				else
				{
					return 0;
				}
			}
		}

		public override double Price
		{
			get
			{
				if (Doubles.ContainsKey(TradedFieldIDs.ROC.execPrice))
				{
					return Doubles[TradedFieldIDs.ROC.execPrice];
				}
				else
				{
					return 0;
				}
			}
		}

		public override double TradeValue
		{
			get
			{
				return Math.Round(Qty * Price * ContractSize, 7);
			}
		}

		public override DateTime TradeTime
		{
			get
			{
				if (Doubles.ContainsKey(TradedFieldIDs.ROC.execTime))
				{
					return DateTime.FromOADate(Doubles[TradedFieldIDs.ROC.execTime]);
				}
				else
				{
					return DateTime.Now;
				}
			}
		}

		public override string OmTag
		{
			get
			{
				if (Strings.ContainsKey(TradedFieldIDs.ROC.omTag))
				{
					return Strings[TradedFieldIDs.ROC.omTag];
				}
				else
				{
					return "";
				}
			}
		}

		public override string OpenClose
		{
			get
			{
				if (Strings.ContainsKey(TradedFieldIDs.ROC.openClose))
				{
					return Strings[TradedFieldIDs.ROC.openClose];
				}
				else
				{
					return "";
				}
			}
		}

		public string OmExecTag
		{
			get
			{
				if (Strings.ContainsKey(TradedFieldIDs.ROC.omExecTag))
				{
					return Strings[TradedFieldIDs.ROC.omExecTag];
				}
				else
				{
					return "";
				}
			}
		}

		public Nullable<long> Side
		{
			get
			{
				if (Longs.ContainsKey(TradedFieldIDs.ROC.side))
				{
					return Longs[TradedFieldIDs.ROC.side];
				}
				else
				{
					return null;
				}
			}
		}

		public long DestID
		{
			get
			{
				if (Longs.ContainsKey(TradedFieldIDs.ROC.destID))
				{
					return Longs[TradedFieldIDs.ROC.destID];
				}
				else
				{
					return -1;
				}
			}
		}

		public double ContractSize
		{
			get
			{
				switch (SecType)
				{
					case CSVFieldIDs.SecutrityTypes.Option:
					case CSVFieldIDs.SecutrityTypes.OptionFuture:
					case CSVFieldIDs.SecutrityTypes.OptionIndex:
						return 100;
					case CSVFieldIDs.SecutrityTypes.Equity:
						return 1;
					default:
						if (Doubles.ContainsKey(TradedFieldIDs.ROC.contractSize))
						{
							return Doubles[TradedFieldIDs.ROC.contractSize];
						}
						else
						{
							return 0;
						}
				}
			}
			set
			{
				Update(TradedFieldIDs.ROC.contractSize, value);
			}
		}

		public double TickSize
		{
			get
			{
				if (Doubles.ContainsKey(TradedFieldIDs.ROC.tickSize))
				{
					return Doubles[TradedFieldIDs.ROC.tickSize];
				}
				else
				{
					return 0.01;
				}
			}
			set
			{
				Update(TradedFieldIDs.ROC.tickSize, value);
			}
		}

		public double DisplayFactor
		{
			get
			{
				if (Doubles.ContainsKey(TradedFieldIDs.ROC.displayFactor))
				{
					return Doubles[TradedFieldIDs.ROC.displayFactor];
				}
				else
				{
					return 1;
				}
			}
			set
			{
				Update(TradedFieldIDs.ROC.displayFactor, value);
			}
		}

		public long CplxOrderType
		{
			get
			{
				if (Longs.ContainsKey(TradedFieldIDs.ROC.cplxOrderType))
				{
					return Longs[TradedFieldIDs.ROC.cplxOrderType];
				}
				else
				{
					return 0;
				}
			}
			set
			{
				Update(TradedFieldIDs.ROC.cplxOrderType, value);
			}
		}

		public bool UpdateTrade
		{
			get
			{
				switch (CplxOrderType)
				{
					case 5:
						return false;
					case 6:
					default:
						return true;
				}
			}
		}

		public ROCExecution()
		{
		}

		public ROCExecution(SerializationInfo info, StreamingContext ctxt)
			: base(info, ctxt)
		{
		}
	}

	[Serializable]
	public class ROCOrder : BaseOrder
	{
		public override bool HasTrades
		{
			get
			{
				if (Bools.ContainsKey(OrderFieldIDs.hasTrades))
				{
					return Bools[OrderFieldIDs.hasTrades];
				}
				else
				{
					return false;
				}
			}
		}

		public override string Symbol
		{
			get
			{
				switch (SecType)
				{
					//case "O":
					//case "I":
					//case "M":
					//    return Underlying;
					default:
						if (Strings.ContainsKey(OrderFieldIDs.ROC.symbol))
						{
							return Strings[OrderFieldIDs.ROC.symbol];
						}
						else
						{
							return "";
						}
				}
			}
			set
			{
				Update(OrderFieldIDs.ROC.symbol, value);
			}
		}

		public bool IsOptionOnFuture
		{
			get
			{
				try
				{
					if (Underlying != "" && Underlying.Length >= 4)
					{
						int num = 0;
						if (Int32.TryParse(Underlying.Substring(Underlying.Length - 1, 1), out num))
						{
							switch (Underlying.Substring(Underlying.Length - 2, 1))
							{
								case GLOBAL.MonthCodes.January:
								case GLOBAL.MonthCodes.February:
								case GLOBAL.MonthCodes.March:
								case GLOBAL.MonthCodes.April:
								case GLOBAL.MonthCodes.May:
								case GLOBAL.MonthCodes.June:
								case GLOBAL.MonthCodes.July:
								case GLOBAL.MonthCodes.August:
								case GLOBAL.MonthCodes.September:
								case GLOBAL.MonthCodes.October:
								case GLOBAL.MonthCodes.November:
								case GLOBAL.MonthCodes.December:
									return true;
								default:
									break;
							}
						}
					}
				}
				catch (Exception ex)
				{
					if (Debugger.IsAttached)
					{
						Debug.Print(ex.Message);
						Debug.Print(ex.StackTrace);
					}
				}

				return false;
			}
		}

		public override string Underlying
		{
			get
			{
				if (Strings.ContainsKey(OrderFieldIDs.ROC.underlying))
				{
					return Strings[OrderFieldIDs.ROC.underlying];
				}
				else
				{
					return "";
				}
			}
		}

		public override string ExpDate
		{
			get
			{
				if (Strings.ContainsKey(OrderFieldIDs.ROC.expDate))
				{
					if (Underlying != "" && MaturityDay != "")
					{
						// IsOption
						return Strings[OrderFieldIDs.ROC.expDate] + MaturityDay;
					}
					return Strings[OrderFieldIDs.ROC.expDate];
				}
				else
				{
					return "";
				}
			}
		}

		public string MaturityDay
		{
			get
			{
				if (Strings.ContainsKey(OrderFieldIDs.ROC.maturityDay))
				{
					return Strings[OrderFieldIDs.ROC.maturityDay];
				}
				else
				{
					return "";
				}
			}
		}

		public override string StrikePrice
		{
			get
			{
				if (Strings.ContainsKey(OrderFieldIDs.ROC.strikePrice))
				{
					return Strings[OrderFieldIDs.ROC.strikePrice];
				}
				else if (Doubles.ContainsKey(OrderFieldIDs.ROC.strikePrice))
				{
					double sp = Doubles[OrderFieldIDs.ROC.strikePrice];
					Update(OrderFieldIDs.ROC.strikePrice, sp.ToString(GetStrikFormat(sp)));
					return Strings[OrderFieldIDs.ROC.strikePrice];
				}
				else
				{
					return "0";
				}
			}
		}

		public override string CallPut
		{
			get
			{
				if (Strings.ContainsKey(OrderFieldIDs.ROC.callPut))
				{
					return Strings[OrderFieldIDs.ROC.callPut];
				}
				else
				{
					return "";
				}
			}
		}

		public override string SecType
		{
			get
			{
				if (Strings.ContainsKey(OrderFieldIDs.ROC.secType))
				{
					return Strings[OrderFieldIDs.ROC.secType];
				}
				else
				{
					return "";
				}
			}
		}

		public override string Trader
		{
			get
			{
				if (Strings.ContainsKey(OrderFieldIDs.ROC.trader))
				{
					return Strings[OrderFieldIDs.ROC.trader];
				}
				else
				{
					return "";
				}
			}
		}

		public string LocalAcct
		{
			get
			{
				if (Strings.ContainsKey(OrderFieldIDs.ROC.localAcct))
				{
					return Strings[OrderFieldIDs.ROC.localAcct];
				}
				else
				{
					return "";
				}
			}
		}

		public override string ClearingAcct
		{
			get
			{
				if (Strings.ContainsKey(OrderFieldIDs.ROC.clearingAccount))
				{
					return Strings[OrderFieldIDs.ROC.clearingAccount];
				}
				else
				{
					return "";
				}
			}
		}

		public override string OmTag
		{
			get
			{
				if (Strings.ContainsKey(OrderFieldIDs.ROC.omTag))
				{
					return Strings[OrderFieldIDs.ROC.omTag];
				}
				else
				{
					return "";
				}
			}
		}

		public override string OpenClose
		{
			get
			{
				if (Strings.ContainsKey(OrderFieldIDs.ROC.openClose))
				{
					return Strings[OrderFieldIDs.ROC.openClose];
				}
				else
				{
					return "";
				}
			}
		}

		public Nullable<long> Side
		{
			get
			{
				if (Longs.ContainsKey(OrderFieldIDs.ROC.side))
				{
					return Longs[OrderFieldIDs.ROC.side];
				}
				else
				{
					return null;
				}
			}
		}

		public string Tag
		{
			get
			{
				if (Strings.ContainsKey(OrderFieldIDs.ROC.tag))
				{
					return Strings[OrderFieldIDs.ROC.tag];
				}
				else
				{
					return null;
				}
			}
		}

		public long Status
		{
			get
			{
				if (Longs.ContainsKey(OrderFieldIDs.ROC.status))
				{
					return Longs[OrderFieldIDs.ROC.status];
				}
				else
				{
					return 0;
				}
			}
			set
			{
				if (Longs.ContainsKey(OrderFieldIDs.ROC.status))
				{
					Longs[OrderFieldIDs.ROC.status] = value;
				}
				else
				{
					Longs.Add(OrderFieldIDs.ROC.status, value);
				}
			}
		}

		public bool IsActive
		{
			get
			{
				switch (Status)
				{
					case CSVFieldIDs.StatusCodes.Busted:
					case CSVFieldIDs.StatusCodes.Expired:
					case CSVFieldIDs.StatusCodes.Rejected:
					case CSVFieldIDs.StatusCodes.Canceled:
					case CSVFieldIDs.StatusCodes.Filled:
					case CSVFieldIDs.StatusCodes.FilledAndCancelled:
					case CSVFieldIDs.StatusCodes.ReplacedAndFilled:
						return false;
					default:
						return true;
				}
			}
		}

		public long Qty
		{
			get
			{
				if (Longs.ContainsKey(OrderFieldIDs.ROC.qty))
				{
					return Longs[OrderFieldIDs.ROC.qty];
				}
				else
				{
					return 0;
				}
			}
			set
			{
				Update(OrderFieldIDs.ROC.qty, value);
			}
		}

		public long LeaveQty
		{
			get
			{
				if (Longs.ContainsKey(OrderFieldIDs.ROC.leavesQty))
				{
					return Longs[OrderFieldIDs.ROC.leavesQty];
				}
				else
				{
					return 0;
				}
			}
			set
			{
				Update(OrderFieldIDs.ROC.leavesQty, value);
			}
		}

		public double Price
		{
			get
			{
				if (Doubles.ContainsKey(OrderFieldIDs.ROC.price))
				{
					return Doubles[OrderFieldIDs.ROC.price];
				}
				else
				{
					return 0;
				}
			}
			set
			{
				Update(OrderFieldIDs.ROC.price, value);
			}
		}

		public double OrigionalPrice
		{
			get
			{
				if (Doubles.ContainsKey(OrderFieldIDs.ROC.originalPrice))
				{
					return Doubles[OrderFieldIDs.ROC.originalPrice];
				}
				else
				{
					return 0;
				}
			}
			set
			{
				Update(OrderFieldIDs.ROC.originalPrice, value);
			}
		}

		public double ExecPrice
		{
			get
			{
				if (Doubles.ContainsKey(OrderFieldIDs.ROC.execPrice))
				{
					return Doubles[OrderFieldIDs.ROC.execPrice];
				}
				else
				{
					return 0;
				}
			}
		}

		public double StopPrice
		{
			get
			{
				if (Doubles.ContainsKey(OrderFieldIDs.ROC.stopPrice))
				{
					return Doubles[OrderFieldIDs.ROC.stopPrice];
				}
				else
				{
					return 0;
				}
			}
			set
			{
				Update(OrderFieldIDs.ROC.stopPrice, value);
			}
		}

		public double PegPrice
		{
			get
			{
				if (Doubles.ContainsKey(OrderFieldIDs.ROC.pegPrice))
				{
					return Doubles[OrderFieldIDs.ROC.pegPrice];
				}
				else
				{
					return 0;
				}
			}
			set
			{
				Update(OrderFieldIDs.ROC.pegPrice, value);
			}
		}

		public string MaxFloor
		{
			get
			{
				if (Strings.ContainsKey(OrderFieldIDs.ROC.maxFloor))
				{
					return Strings[OrderFieldIDs.ROC.maxFloor];
				}
				else
				{
					return "";
				}
			}
		}

		public long CumQty
		{
			get
			{
				if (Longs.ContainsKey(OrderFieldIDs.ROC.cumQty))
				{
					return Longs[OrderFieldIDs.ROC.cumQty];
				}
				else
				{
					return 0;
				}
			}
			set
			{
				Update(OrderFieldIDs.ROC.cumQty, value);
			}
		}

		public double AvgPrice
		{
			get
			{
				if (Doubles.ContainsKey(OrderFieldIDs.ROC.avgPrice))
				{
					return Doubles[OrderFieldIDs.ROC.avgPrice];
				}
				else
				{
					return 0;
				}
			}
			set
			{
				Update(OrderFieldIDs.ROC.avgPrice, value);
			}
		}

		public long OrderType
		{
			get
			{
				if (Longs.ContainsKey(OrderFieldIDs.ROC.orderType))
				{
					return Longs[OrderFieldIDs.ROC.orderType];
				}
				else
				{
					return 0;
				}
			}
			set
			{
				Update(OrderFieldIDs.ROC.orderType, value);
			}
		}

		public bool IsMarket
		{
			get
			{
				switch (OrderType)
				{
					case CSVFieldIDs.OrderTypes.Market:
					case CSVFieldIDs.OrderTypes.MarketAtOpen:
					case CSVFieldIDs.OrderTypes.MarketOnClose:
						return true;
					default:
						return false;
				}
			}
		}

		public long TIF
		{
			get
			{
				if (Longs.ContainsKey(OrderFieldIDs.ROC.tif))
				{
					return Longs[OrderFieldIDs.ROC.tif];
				}
				else
				{
					return 0;
				}
			}
			set
			{
				Update(OrderFieldIDs.ROC.tif, value);
			}
		}

		public long AlgoType
		{
			get
			{
				if (Longs.ContainsKey(OrderFieldIDs.ROC.algoType))
				{
					return Longs[OrderFieldIDs.ROC.algoType];
				}
				else
				{
					return 0;
				}
			}
			set
			{
				Update(OrderFieldIDs.ROC.algoType, value);
			}
		}

		public string EndTime
		{
			get
			{
				if (Doubles.ContainsKey(OrderFieldIDs.ROC.endTime))
				{
					return DateTime.FromOADate(Doubles[OrderFieldIDs.ROC.endTime]).ToString("MM-dd HH:mm:ss");
				}
				else
				{
					return "";
				}
			}
		}

		public long DestID
		{
			get
			{
				if (Longs.ContainsKey(OrderFieldIDs.ROC.destID))
				{
					return Longs[OrderFieldIDs.ROC.destID];
				}
				else
				{
					return 0;
				}
			}
		}

		public long AlgoDestID
		{
			get
			{
				if (Longs.ContainsKey(OrderFieldIDs.ROC.algoDestID))
				{
					return Longs[OrderFieldIDs.ROC.algoDestID];
				}
				else
				{
					return 0;
				}
			}
		}

		public DateTime OmTime
		{
			get
			{
				if (Doubles.ContainsKey(OrderFieldIDs.ROC.omTime))
				{
					return DateTime.FromOADate(Doubles[OrderFieldIDs.ROC.omTime]);
				}
				else
				{
					return DateTime.Now;
				}
			}
			set
			{
				Update(OrderFieldIDs.ROC.omTime, value.ToOADate());
			}
		}

		public string Text
		{
			get
			{
				if (Strings.ContainsKey(OrderFieldIDs.ROC.text))
				{
					return Strings[OrderFieldIDs.ROC.text];
				}
				else
				{
					return null;
				}
			}
			set
			{
				Update(OrderFieldIDs.ROC.text, value);
			}
		}

		public double ContractSize
		{
			get
			{
				switch (SecType)
				{
					case CSVFieldIDs.SecutrityTypes.Option:
					case CSVFieldIDs.SecutrityTypes.OptionFuture:
					case CSVFieldIDs.SecutrityTypes.OptionIndex:
						return 100;
					case CSVFieldIDs.SecutrityTypes.Equity:
						return 1;
					default:
						if (Doubles.ContainsKey(OrderFieldIDs.ROC.contractSize))
						{
							return Doubles[OrderFieldIDs.ROC.contractSize];
						}
						else
						{
							return 0;
						}
				}
			}
			set
			{
				Update(OrderFieldIDs.ROC.contractSize, value);
			}
		}

		public double TickSize
		{
			get
			{
				if (Doubles.ContainsKey(OrderFieldIDs.ROC.tickSize))
				{
					return Doubles[OrderFieldIDs.ROC.tickSize];
				}
				else
				{
					return 0.01;
				}
			}
			set
			{
				Update(OrderFieldIDs.ROC.tickSize, value);
			}
		}

		public double DisplayFactor
		{
			get
			{
				if (Doubles.ContainsKey(OrderFieldIDs.ROC.displayFactor))
				{
					return Doubles[OrderFieldIDs.ROC.displayFactor];
				}
				else
				{
					return 1;
				}
			}
			set
			{
				Update(OrderFieldIDs.ROC.displayFactor, value);
			}
		}

		public string TradeFor
		{
			get
			{
				if (Strings.ContainsKey(OrderFieldIDs.ROC.tradeFor))
				{
					return Strings[OrderFieldIDs.ROC.tradeFor];
				}
				else
				{
					return "";
				}
			}
			set
			{
				Update(OrderFieldIDs.ROC.tradeFor, value);
			}
		}

		public long CplxOrderType
		{
			get
			{
				if (Longs.ContainsKey(OrderFieldIDs.ROC.cplxOrderType))
				{
					return Longs[OrderFieldIDs.ROC.cplxOrderType];
				}
				else
				{
					return 0;
				}
			}
			set
			{
				Update(OrderFieldIDs.ROC.cplxOrderType, value);
			}
		}

		public string ParentTag
		{
			get
			{
				if (Strings.ContainsKey(OrderFieldIDs.ROC.parentTag))
				{
					return Strings[OrderFieldIDs.ROC.parentTag];
				}
				else
				{
					return null;
				}
			}
		}

		public string ClientEcho
		{
			get
			{
				if (Strings.ContainsKey(OrderFieldIDs.ROC.clientEcho))
				{
					return Strings[OrderFieldIDs.ROC.clientEcho];
				}
				else
				{
					return null;
				}
			}
		}

		public bool UpdateOrder
		{
			get
			{
				switch (CplxOrderType)
				{
					/*
					case 6:
						return false;
					case 5:
					default:
						return true;
					*/
					case CSVFieldIDs.CplxOrderTypes.Continer:
						return true;
					case CSVFieldIDs.CplxOrderTypes.Leg:
					default:
						return false;
				}
			}
		}

		public string SecurityDefinition
		{
			get
			{
				if (Strings.ContainsKey(OrderFieldIDs.ROC.securityDefinition))
				{
					return Strings[OrderFieldIDs.ROC.securityDefinition];
				}
				else
				{
					return "";
				}
			}
		}

		public string CMTAFirmID
		{
			get
			{
				if (Strings.ContainsKey(OrderFieldIDs.ROC.cmtaFirmID))
				{
					return Strings[OrderFieldIDs.ROC.cmtaFirmID];
				}
				else
				{
					return "";
				}
			}
		}

		public ROCOrder()
		{
		}

		public ROCOrder(SerializationInfo info, StreamingContext ctxt)
			: base(info, ctxt)
		{
		}
	}

	#endregion

	#region - TPOS Trades, Positions - 

	public class TPOSExecution : BaseExecution
	{
		public override string TradeID
		{
			get
			{
				if (Strings.ContainsKey(TradedFieldIDs.TPOS.tradeID))
				{
					return Strings[TradedFieldIDs.TPOS.tradeID];
				}
				else
				{
					return "";
				}
			}
		}

		public long Version
		{
			get
			{
				if (Longs.ContainsKey(TradedFieldIDs.TPOS.version))
				{
					return Longs[TradedFieldIDs.TPOS.version];
				}
				else
				{
					return 0;
				}
			}
		}

		// 3 means deleted
		public long ModReasonID
		{
			get
			{
				if (Longs.ContainsKey(TradedFieldIDs.TPOS.lastModReasonID))
				{
					return Longs[TradedFieldIDs.TPOS.lastModReasonID];
				}
				else
				{
					return 0;
				}
			}
		}

		public override string Symbol
		{
			get
			{
				if (Strings.ContainsKey(TradedFieldIDs.TPOS.symbol))
				{
					return Strings[TradedFieldIDs.TPOS.symbol];
				}
				else
				{
					return "";
				}
			}
			set
			{
				Update(TradedFieldIDs.TPOS.symbol, value);
			}
		}

		public override string Underlying
		{
			get
			{
				if (Strings.ContainsKey(TradedFieldIDs.TPOS.undSymbol))
				{
					return Strings[TradedFieldIDs.TPOS.undSymbol];
				}
				else
				{
					return "";
				}
			}
		}

		public override string ExpDate
		{
			get
			{
				if (Strings.ContainsKey(TradedFieldIDs.TPOS.expDate))
				{
					return Strings[TradedFieldIDs.TPOS.expDate];
				}
				else
				{
					return "";
				}
			}
		}

		public override string StrikePrice
		{
			get
			{
				if (Strings.ContainsKey(TradedFieldIDs.TPOS.strike))
				{
					return Strings[TradedFieldIDs.TPOS.strike];
				}
				else
				{
					if (Doubles.ContainsKey(TradedFieldIDs.TPOS.strike))
					{
						double sp = Doubles[TradedFieldIDs.TPOS.strike];
						Update(TradedFieldIDs.TPOS.strike, sp.ToString(GetStrikFormat(sp))); 
						return Strings[TradedFieldIDs.TPOS.strike];
					}
					else
					{
						return "";
					}
				}
			}
		}

		public override string CallPut
		{
			get
			{
				if (Strings.ContainsKey(TradedFieldIDs.TPOS.putCall))
				{
					return Strings[TradedFieldIDs.TPOS.putCall];
				}
				else
				{
					return "";
				}
			}
		}

		public override string SecType
		{
			get
			{
				if (Strings.ContainsKey(TradedFieldIDs.TPOS.secType))
				{
					return Strings[TradedFieldIDs.TPOS.secType];
				}
				else
				{
					return "";
				}
			}
		}

		public override string Trader
		{
			get
			{
				if (Strings.ContainsKey(TradedFieldIDs.TPOS.portfolio))
				{
					return Strings[TradedFieldIDs.TPOS.portfolio];
				}
				else
				{
					return "";
				}
				//if (Strings.ContainsKey(TradedFieldIDs.TPOS.traderAcronym))
				//{
				//    return Strings[TradedFieldIDs.TPOS.traderAcronym];
				//}
				//else
				//{
				//    return "";
				//}
			}
		}

		public override string ClearingAcct
		{
			get
			{
				if (Strings.ContainsKey(TradedFieldIDs.TPOS.clearingAccount))
				{
					return Strings[TradedFieldIDs.TPOS.clearingAccount];
				}
				else
				{
					return "";
				}
			}
		}

		public override long Qty
		{
			get
			{
				if (ModReasonID != 3)
				{
					// not deleted
					if (Longs.ContainsKey(TradedFieldIDs.TPOS.qty))
					{
						return Math.Abs(Longs[TradedFieldIDs.TPOS.qty]);
					}
				}
				return 0;
			}
		}

		public override double Price
		{
			get
			{
				if (ModReasonID != 3)
				{
					// not deleted
					if (Doubles.ContainsKey(TradedFieldIDs.TPOS.price))
					{
						return Doubles[TradedFieldIDs.TPOS.price];
					}
				}
				return 0;
			}
		}

		public override double TradeValue
		{
			get 
			{ 
				return Qty * Price;
			}
		}

		public override DateTime TradeTime
		{
			get
			{
				if (Strings.ContainsKey(TradedFieldIDs.TPOS.lastModTime))
				{
					try
					{
						return System.Convert.ToDateTime(Strings[TradedFieldIDs.TPOS.lastModTime]).ToLocalTime();
					}
					catch (Exception ex)
					{
						Debug.Assert(false, ex.Message, ex.StackTrace);
						return DateTime.Now;
					}
				}
				else
				{
					return DateTime.Now;
				}
			}
		}

		public override string OpenClose
		{
			get
			{
				return "";
			}
		}

		public override string OmTag
		{
			get
			{
				if (Strings.ContainsKey(TradedFieldIDs.TPOS.extTradeID))
				{
					return Strings[TradedFieldIDs.TPOS.extTradeID];
				}
				else
				{
					return "";
				}
			}
		}

		public Nullable<long> Side
		{
			get
			{
				if (Longs.ContainsKey(TradedFieldIDs.TPOS.qty))
				{
					if (Longs[TradedFieldIDs.TPOS.qty] > 0)
					{
						// Buy
						return 1;
					}
					else
					{
						// Sell
						return 2;
					}
				}
				else
				{
					return null;
				}
			}
		}

		public long DestID
		{
			get
			{
				if (Longs.ContainsKey(TradedFieldIDs.TPOS.exchange))
				{
					return Longs[TradedFieldIDs.TPOS.exchange];
				}
				else
				{
					long id;
					if (Strings.ContainsKey(TradedFieldIDs.TPOS.exchange))
					{
						if (Int64.TryParse(Strings[TradedFieldIDs.TPOS.exchange], out id))
						{
							Update(TradedFieldIDs.TPOS.exchange, id);
							return id;
						}
					}
					return -1;
				}
			}
		}

		public double ContractSize
		{
			get
			{
				switch (SecType)
				{
					case CSVFieldIDs.SecutrityTypes.Option:
					case CSVFieldIDs.SecutrityTypes.OptionFuture:
					case CSVFieldIDs.SecutrityTypes.OptionIndex:
						return 100;
					case CSVFieldIDs.SecutrityTypes.Equity:
						return 1;
					default:
						if (Doubles.ContainsKey(TradedFieldIDs.TPOS.contractSize))
						{
							return Doubles[TradedFieldIDs.TPOS.contractSize];
						}
						else
						{
							return 0;
						}
				}
			}
			set
			{
				Update(TradedFieldIDs.TPOS.contractSize, value);
			}
		}

		public double TickSize
		{
			get
			{
				if (Doubles.ContainsKey(TradedFieldIDs.TPOS.tickSize))
				{
					return Doubles[TradedFieldIDs.TPOS.tickSize];
				}
				else
				{
					return 0.01;
				}
			}
			set
			{
				Update(TradedFieldIDs.TPOS.tickSize, value);
			}
		}

		public double DisplayFactor
		{
			get
			{
				if (Doubles.ContainsKey(TradedFieldIDs.TPOS.displayFactor))
				{
					return Doubles[TradedFieldIDs.TPOS.displayFactor];
				}
				else
				{
					return 1;
				}
			}
			set
			{
				Update(TradedFieldIDs.TPOS.displayFactor, value);
			}
		}
	}

	public class TPOSPosition : BasePosition
	{
		public override string Symbol
		{
			get
			{
				if (Strings.ContainsKey(PositionFieldIDs.TPOS.symbol))
				{
					return Strings[PositionFieldIDs.TPOS.symbol];
				}
				else
				{
					return "";
				}
			}
			set
			{
				Update(PositionFieldIDs.TPOS.symbol, value);
			}
		}

		public double BidPrice
		{
			get
			{
				if (Doubles.ContainsKey(PositionFieldIDs.MD.bidPrice))
				{
					return Doubles[PositionFieldIDs.MD.bidPrice];
				}
				else
				{
					return 0;
				}
			}
			set
			{
				Update(PositionFieldIDs.MD.bidPrice, value);
			}
		}

		public long BidSize
		{
			get
			{
				if (Longs.ContainsKey(PositionFieldIDs.MD.bidSize))
				{
					return Longs[PositionFieldIDs.MD.bidSize];
				}
				else
				{
					return 0;
				}
			}
			set
			{
				Update(PositionFieldIDs.MD.bidSize, value);
			}
		}

		public double AskPrice
		{
			get
			{
				if (Doubles.ContainsKey(PositionFieldIDs.MD.askPrice))
				{
					return Doubles[PositionFieldIDs.MD.askPrice];
				}
				else
				{
					return 0;
				}
			}
			set
			{
				Update(PositionFieldIDs.MD.askPrice, value);
			}
		}

		public long AskSize
		{
			get
			{
				if (Longs.ContainsKey(PositionFieldIDs.MD.askSize))
				{
					return Longs[PositionFieldIDs.MD.askSize];
				}
				else
				{
					return 0;
				}
			}
			set
			{
				Update(PositionFieldIDs.MD.askSize, value);
			}
		}

		public double LastTraded
		{
			get
			{
				if (Doubles.ContainsKey(PositionFieldIDs.MD.tradePrice))
				{
					return Doubles[PositionFieldIDs.MD.tradePrice];
				}
				else
				{
					return 0;
				}
			}
			set
			{
				Update(PositionFieldIDs.MD.tradePrice, value);
			}
		}

		public double NetChange
		{
			get
			{
				if (Doubles.ContainsKey(PositionFieldIDs.MD.netChange))
				{
					return Doubles[PositionFieldIDs.MD.netChange];
				}
				else
				{
					return 0;
				}
			}
			set
			{
				Update(PositionFieldIDs.MD.netChange, value);
			}
		}

		public double PctChange
		{
			get
			{
				if (Doubles.ContainsKey(PositionFieldIDs.MD.pctChange))
				{
					return Doubles[PositionFieldIDs.MD.pctChange];
				}
				else
				{
					return 0;
				}
			}
			set
			{
				Update(PositionFieldIDs.MD.pctChange, value);
			}
		}

		public long TotalVolume
		{
			get
			{
				if (Longs.ContainsKey(PositionFieldIDs.MD.totalVolume))
				{
					return Longs[PositionFieldIDs.MD.totalVolume];
				}
				else
				{
					return 0;
				}
			}
			set
			{
				Update(PositionFieldIDs.MD.totalVolume, value);
			}
		}

		public override string ClearingAcct
		{
			get
			{
				if (Strings.ContainsKey(PositionFieldIDs.TPOS.clearingAccount))
				{
					return Strings[PositionFieldIDs.TPOS.clearingAccount];
				}
				else
				{
					return "";
				}
			}
			set
			{
				Update(PositionFieldIDs.TPOS.clearingAccount, value);
			}
		}

		public override string Trader
		{
			get
			{
				if (Strings.ContainsKey(PositionFieldIDs.TPOS.protfolio))
				{
					return Strings[PositionFieldIDs.TPOS.protfolio];
				}
				else
				{
					return "";
				}
			}
			set
			{
				Update(PositionFieldIDs.TPOS.protfolio, value);
			}

			//get
			//{
			//    if (Strings.ContainsKey(PositionFieldIDs.TPOS.traderAcronym))
			//    {
			//        return Strings[PositionFieldIDs.TPOS.traderAcronym];
			//    }
			//    else
			//    {
			//        return "";
			//    }
			//}
			//set
			//{
			//    Update(PositionFieldIDs.TPOS.traderAcronym, value);
			//}
		}

		public long OpenQty
		{
			get
			{
				if (Longs.ContainsKey(PositionFieldIDs.TPOS.openQty))
				{
					return Longs[PositionFieldIDs.TPOS.openQty];
				}
				else
				{
					return 0;
				}
			}
			set
			{
				Update(PositionFieldIDs.TPOS.openQty, value);
			}
		}

		public double OpenAvg
		{
			get
			{
				if (Doubles.ContainsKey(PositionFieldIDs.TPOS.avgCost))
				{
					return Doubles[PositionFieldIDs.TPOS.avgCost];
				}
				else
				{
					return 0;
				}
			}
			set
			{
				Update(PositionFieldIDs.TPOS.avgCost, value);
			}
		}

		public long BuyQty
		{
			get
			{
				if (Longs.ContainsKey(PositionFieldIDs.buyQty))
				{
					return Longs[PositionFieldIDs.buyQty];
				}
				else
				{
					return 0;
				}
			}
			set
			{
				Update(PositionFieldIDs.buyQty, value);
			}
		}

		public double BuyAvg
		{
			get
			{
				if (Doubles.ContainsKey(PositionFieldIDs.buyAvg))
				{
					return Doubles[PositionFieldIDs.buyAvg];
				}
				else
				{
					return 0;
				}
			}
			set
			{
				Update(PositionFieldIDs.buyAvg, value);
			}
		}

		public long SellQty
		{
			get
			{
				if (Longs.ContainsKey(PositionFieldIDs.sellQty))
				{
					return Longs[PositionFieldIDs.sellQty];
				}
				else
				{
					return 0;
				}
			}
			set
			{
				Update(PositionFieldIDs.sellQty, value);
			}
		}

		public double SellAvg
		{
			get
			{
				if (Doubles.ContainsKey(PositionFieldIDs.sellAvg))
				{
					return Doubles[PositionFieldIDs.sellAvg];
				}
				else
				{
					return 0;
				}
			}
			set
			{
				Update(PositionFieldIDs.sellAvg, value);
			}
		}

		public long DayQty
		{
			get
			{
				return BuyQty - SellQty;
			}
		}

		public long CurrentQty
		{
			get
			{
				return OpenQty + DayQty;
			}
		}

		public double ContractSize
		{
			get
			{
				switch (SecType)
				{
					case CSVFieldIDs.SecutrityTypes.Option:
					case CSVFieldIDs.SecutrityTypes.OptionFuture:
					case CSVFieldIDs.SecutrityTypes.OptionIndex:
						return 100;
					case CSVFieldIDs.SecutrityTypes.Equity:
						return 1;
					default:
						if (Doubles.ContainsKey(PositionFieldIDs.contractSize))
						{
							return Doubles[PositionFieldIDs.contractSize];
						}
						else
						{
							return 0;
						}
				}
			}
			set
			{
				Update(PositionFieldIDs.contractSize, value);
			}
		}

		public double TickSize
		{
			get
			{
				if (Doubles.ContainsKey(PositionFieldIDs.MD.tickSize))
				{
					return Doubles[PositionFieldIDs.MD.tickSize];
				}
				else
				{
					return 0.01;
				}
			}
			set
			{
				Update(PositionFieldIDs.MD.tickSize, value);
			}
		}

		public double DisplayFactor
		{
			get
			{
				if (Doubles.ContainsKey(PositionFieldIDs.MD.displayFactor))
				{
					return Doubles[PositionFieldIDs.MD.displayFactor];
				}
				else
				{
					return 1;
				}
			}
			set
			{
				Update(PositionFieldIDs.MD.displayFactor, value);
			}
		}

		public double OpenPnL
		{
			get
			{
				if (LastTraded > 0)
				{
					if (OpenAvg != 0)
					{
						if (OpenQty > 0) // Open balance is LONG(BUY)
						{
							return OpenQty * (LastTraded - OpenAvg) * ContractSize;
						}
						else if (OpenQty < 0) //Open balance is SHORT(SELL)
						{
							return -OpenQty * (OpenAvg - LastTraded) * ContractSize;
						}
					}
				}

				return 0;
			}
		}

		public double DayUnRealizedPnL
		{
			get
			{
				if (LastTraded > 0)
				{
					if (DayQty > 0) // Day balance is LONG(BUY)
					{
						return DayQty * (LastTraded - BuyAvg) * ContractSize;
					}
					else if (DayQty < 0)  //Day balance is SHORT(SELL)
					{
						return -DayQty * (SellAvg - LastTraded) * ContractSize;
					}
				}

				return 0;
			}
		}

		public double DayRealizedPnL
		{
			get
			{
				if (BuyQty > SellQty) // Day balance is LONG(BUY)
				{
					return SellQty * (SellAvg - BuyAvg) * ContractSize;
				}
				else if (BuyQty < SellQty) //Day balance is SHORT(SELL)
				{
					return BuyQty * (SellAvg - BuyAvg) * ContractSize;
				}
				else if (BuyQty > 0 && SellQty > 0 && BuyQty == SellQty)
				{
					return BuyQty * (SellAvg - BuyAvg) * ContractSize;
				}
				else
				{
					return 0;
				}
			}
		}

		public double DayPnL
		{
			get
			{
				return DayUnRealizedPnL + DayRealizedPnL;
			}
		}

		public double TotalPnL
		{
			get
			{
				return DayPnL + OpenPnL;
			}
		}

		public double MarketValue
		{
			get
			{
				return LastTraded * CurrentQty * ContractSize;
			}
		}

		public double SettlePrice
		{
			get
			{
				if (Doubles.ContainsKey(PositionFieldIDs.TPOS.settlePrice))
				{
					return Doubles[PositionFieldIDs.TPOS.settlePrice];
				}
				else
				{
					return 0;
				}
			}
			set
			{
				Update(PositionFieldIDs.TPOS.settlePrice, value);
			}
		}

		public override string Underlying
		{
			get
			{
				if (Strings.ContainsKey(PositionFieldIDs.TPOS.undSymbol))
				{
					return Strings[PositionFieldIDs.TPOS.undSymbol];
				}
				else
				{
					return "";
				}
			}
			set
			{
				Update(PositionFieldIDs.TPOS.undSymbol, value);
			}
		}

		public override string ExpDate
		{
			get
			{
				if (Strings.ContainsKey(PositionFieldIDs.TPOS.expDate))
				{
					return Strings[PositionFieldIDs.TPOS.expDate];
				}
				else
				{
					return "";
				}
			}
			set
			{
				Update(PositionFieldIDs.TPOS.expDate, value);
			}
		}

		public override string StrikePrice
		{
			get
			{
				if (Strings.ContainsKey(PositionFieldIDs.TPOS.strike))
				{
					return Strings[PositionFieldIDs.TPOS.strike];
				}
				else
				{
					if (Doubles.ContainsKey(PositionFieldIDs.TPOS.strike))
					{
						Strings.Add(PositionFieldIDs.TPOS.strike, Doubles[PositionFieldIDs.TPOS.strike].ToString());
						return Strings[PositionFieldIDs.TPOS.strike];
					}
					else
					{
						return "";
					}
				}
			}
			set
			{
				Update(PositionFieldIDs.TPOS.strike, value);
			}
		}

		public override string CallPut
		{
			get
			{
				if (Strings.ContainsKey(PositionFieldIDs.TPOS.putCall))
				{
					return Strings[PositionFieldIDs.TPOS.putCall];
				}
				else
				{
					return "";
				}
			}
			set
			{
				Update(PositionFieldIDs.TPOS.putCall, value);
			}
		}

		public override string SecType
		{
			get
			{
				if (Strings.ContainsKey(PositionFieldIDs.TPOS.secType))
				{
					return Strings[PositionFieldIDs.TPOS.secType];
				}
				else
				{
					return "";
				}
			}
			set
			{
				Update(PositionFieldIDs.TPOS.secType, value);
			}
		}

		public string SecurityStatus
		{
			get
			{
				if (Strings.ContainsKey(PositionFieldIDs.TPOS.securityStatus))
				{
					return Strings[PositionFieldIDs.TPOS.securityStatus];
				}
				else
				{
					return "";
				}
			}
			set
			{
				Update(PositionFieldIDs.TPOS.securityStatus, value);
			}
		}

		private List<string> _omTags;
		public override List<string> OmTags
		{
			get
			{
				if (_omTags == null)
				{
					_omTags = new List<string>();
				}
				return _omTags;
			}
			set
			{
				_omTags = value;
			}
		}

		public override bool HasOrders
		{
			get
			{
				if (_omTags == null || _omTags.Count == 0)
				{
					return false;
				}
				else
				{
					return true;
				}
			}
		}

		public long Version
		{
			get
			{
				if (Longs.ContainsKey(PositionFieldIDs.TPOS.version))
				{
					return Longs[PositionFieldIDs.TPOS.version];
				}
				else
				{
					return 0;
				}
			}
		}
	}

	#endregion

	#region - IM SecurityInfo, Option Chain, SSFuture Chain -

	public class IMSecurityInfo : BaseSecurityInfo
	{
		public override string MDSource
		{
			get
			{
				if (Strings.ContainsKey(SecurityFieldIDs.Security.dataSourceInfo))
				{
					return Strings[SecurityFieldIDs.Security.dataSourceInfo];
				}
				else
				{
					return "";
				}
			}
		}

		public override string MDSymbol
		{
			get
			{
				if (Strings.ContainsKey(SecurityFieldIDs.Security.genericMDSymbol))
				{
					return Strings[SecurityFieldIDs.Security.genericMDSymbol];
				}
				else
				{
					return "";
				}
			}
		}

		public override double ContractSize
		{
			get
			{
				if (Doubles.ContainsKey(SecurityFieldIDs.Security.contractSize))
				{
					return Doubles[SecurityFieldIDs.Security.contractSize];
				}
				else
				{
					return 1;
				}
			}
		}

		public override double TickSize
		{
			get
			{
				if (Doubles.ContainsKey(SecurityFieldIDs.Security.tickSize))
				{
					return Doubles[SecurityFieldIDs.Security.tickSize];
				}
				else
				{
					return 0.01;
				}
			}
		}

		public override string SecType
		{
			get
			{
				if (Strings.ContainsKey(SecurityFieldIDs.Security.securityType))
				{
					return Strings[SecurityFieldIDs.Security.securityType];
				}
				else
				{
					return "";
				}
			}
		}

		public override string LongName
		{
			get
			{
				if (Strings.ContainsKey(SecurityFieldIDs.Security.longName))
				{
					return Strings[SecurityFieldIDs.Security.longName];
				}
				else
				{
					return "";
				}
			}
		}

		public string Underlying
		{
			get
			{
				if (Strings.ContainsKey(SecurityFieldIDs.Security.baseSymbol))
				{
					return Strings[SecurityFieldIDs.Security.baseSymbol];
				}
				else
				{
					return "";
				}
			}
		}

		public string Expiration
		{
			get
			{
				if (Strings.ContainsKey(SecurityFieldIDs.Security.expirationDate))
				{
					return Strings[SecurityFieldIDs.Security.expirationDate];
				}
				else
				{
					return "";
				}
			}
		}

		private Dictionary<string, IMOptionInfo> _optionChain;
		public Dictionary<string, IMOptionInfo> OptionChain
		{
			get
			{
				if (_optionChain == null)
				{
					_optionChain = new Dictionary<string, IMOptionInfo>();
				}
				return _optionChain;
			}
			set
			{
				_optionChain = value;
			}
		}

		private Dictionary<string, IMSSFutureInfo> _ssfutureChain;
		public Dictionary<string, IMSSFutureInfo> SSFutureChain
		{
			get
			{
				if (_ssfutureChain == null)
				{
					_ssfutureChain = new Dictionary<string, IMSSFutureInfo>();
				}
				return _ssfutureChain;
			}
			set
			{
				_ssfutureChain = value;
			}
		}
	}

	public class IMOptionInfo : BaseOptionInfo
	{
		public string Exchange
		{
			get
			{
				if (Strings.ContainsKey(SecurityFieldIDs.Option.exchange))
				{
					return Strings[SecurityFieldIDs.Option.exchange];
				}
				else
				{
					return "";
				}
			}
		}

		public override string SecType
		{
			get
			{
				if (Strings.ContainsKey(SecurityFieldIDs.Option.secType))
				{
					return Strings[SecurityFieldIDs.Option.secType];
				}
				else
				{
					return "";
				}
			}
			set
			{
				Update(SecurityFieldIDs.Option.secType, value);
			}
		}

		public string MDSource
		{
			get
			{
				if (Strings.ContainsKey(SecurityFieldIDs.Option.dataSource))
				{
					return Strings[SecurityFieldIDs.Option.dataSource];
				}
				else
				{
					return "";
				}
			}
			set
			{
				Update(SecurityFieldIDs.Option.dataSource, value);
			}
		}

		public override string MDSymbol
		{
			get
			{
				if (Strings.ContainsKey(SecurityFieldIDs.Option.genericMDSymbol))
				{
					return Strings[SecurityFieldIDs.Option.genericMDSymbol];
				}
				else
				{
					return "";
				}
			}
		}

		public override double ContractSize
		{
			get
			{
				if (Doubles.ContainsKey(SecurityFieldIDs.Option.contractSize))
				{
					return Doubles[SecurityFieldIDs.Option.contractSize];
				}
				else
				{
					return 100;
				}
			}
		}

		public override double TickSize
		{
			get
			{
				if (Doubles.ContainsKey(SecurityFieldIDs.Option.tickSize))
				{
					return Doubles[SecurityFieldIDs.Option.tickSize];
				}
				else
				{
					return 0.01;
				}
			}
			set
			{
				Update(SecurityFieldIDs.Option.tickSize, value);
			}
		}

		public override string ExpDate
		{
			get
			{
				if (Strings.ContainsKey(SecurityFieldIDs.Option.expirationDate))
				{
					DateTime tDT;
					if (DateTime.TryParseExact(Strings[SecurityFieldIDs.Option.expirationDate], TimeFormats.Formats, System.Globalization.CultureInfo.CurrentCulture, System.Globalization.DateTimeStyles.None, out tDT))
					{
						return tDT.ToString("yyyyMMdd");
					}
				}

				return "";
			}
		}

		public Nullable<DateTime> ExpDateDT
		{
			get
			{
				if (Strings.ContainsKey(SecurityFieldIDs.Option.expirationDate))
				{
					DateTime tDT;
					if (DateTime.TryParseExact(Strings[SecurityFieldIDs.Option.expirationDate], TimeFormats.Formats, System.Globalization.CultureInfo.CurrentCulture, System.Globalization.DateTimeStyles.None, out tDT))
					{
						return tDT;
					}
					else
					{
						return null;
					}
				}
				else
				{
					return null;
				}
			}
		}

		public override string StrikePrice
		{
			get
			{
				if (Strings.ContainsKey(SecurityFieldIDs.Option.strikPrice))
				{
					return Strings[SecurityFieldIDs.Option.strikPrice];
				}
				else if (Doubles.ContainsKey(SecurityFieldIDs.Option.strikPrice))
				{
					Update(SecurityFieldIDs.Option.strikPrice, Doubles[SecurityFieldIDs.Option.strikPrice].ToString());
					return Strings[SecurityFieldIDs.Option.strikPrice];
				}

				return "";
			}
		}

		public Nullable<double> StrikePriceD
		{
			get
			{
				if (Doubles.ContainsKey(SecurityFieldIDs.Option.strikPrice))
				{
					return Doubles[SecurityFieldIDs.Option.strikPrice];
				}
				else if (Strings.ContainsKey(SecurityFieldIDs.Option.strikPrice))
				{
					Update(SecurityFieldIDs.Option.strikPrice, Convert.ToDouble(Strings[SecurityFieldIDs.Option.strikPrice]));
					return Doubles[SecurityFieldIDs.Option.strikPrice];
				}

				return null;
			}
		}

		public override string CallPut
		{
			get
			{
				if (Strings.ContainsKey(SecurityFieldIDs.Option.putCall))
				{
					return Strings[SecurityFieldIDs.Option.putCall];
				}
				else
				{
					return "";
				}
			}
		}

		public override string Underlying
		{
			get
			{
				if (Strings.ContainsKey(SecurityFieldIDs.Option.underlying))
				{
					return Strings[SecurityFieldIDs.Option.underlying];
				}
				else
				{
					return "";
				}
			}
		}

		public override string OptionSymbol
		{
			get
			{
				if (Strings.ContainsKey(SecurityFieldIDs.Option.optionSymbol))
				{
					return Strings[SecurityFieldIDs.Option.optionSymbol];
				}
				else
				{
					return "";
				}
			}
		}
	}

	public class IMSSFutureInfo : BaseSSFutureInfo
	{
		public override string MDSource
		{
			get
			{
				if (Strings.ContainsKey(SecurityFieldIDs.SSFuture.dataSource))
				{
					return Strings[SecurityFieldIDs.SSFuture.dataSource];
				}
				else
				{
					return "ONEC";
				}
			}
		}

		public override string MDSymbol
		{
			get
			{
				if (Strings.ContainsKey(SecurityFieldIDs.SSFuture.fullSymbol))
				{
					return Strings[SecurityFieldIDs.SSFuture.fullSymbol];
				}
				else
				{
					return "";
				}
			}
		}

		public override double ContractSize
		{
			get
			{
				if (Doubles.ContainsKey(SecurityFieldIDs.SSFuture.contractSize))
				{
					return Doubles[SecurityFieldIDs.SSFuture.contractSize];
				}
				else
				{
					return 1;
				}
			}
		}

		public override double TickSize
		{
			get
			{
				if (Doubles.ContainsKey(SecurityFieldIDs.SSFuture.tickSize))
				{
					return Doubles[SecurityFieldIDs.SSFuture.tickSize];
				}
				else
				{
					return 0.01;
				}
			}
			set
			{
				Update(SecurityFieldIDs.SSFuture.tickSize, value);
			}
		}

		public override string ExpDate
		{
			get
			{
				if (Strings.ContainsKey(SecurityFieldIDs.SSFuture.expirationDate))
				{
					return Strings[SecurityFieldIDs.SSFuture.expirationDate];
				}
				else
				{
					return null;
				}
			}
		}

		public override string LongName
		{
			get
			{
				if (Strings.ContainsKey(SecurityFieldIDs.SSFuture.longName))
				{
					return Strings[SecurityFieldIDs.SSFuture.longName];
				}
				else
				{
					return "";
				}
			}
		}
	}

	#endregion
}
