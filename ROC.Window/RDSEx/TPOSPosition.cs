using CSVEx;
using System.Collections.Generic;

namespace RDSEx
{
	public class TPOSPosition : BasePosition
	{
		public void Merge(TPOSPosition other)
		{
			base.Merge(other);
		}

		public override string Symbol {
			get => _data.TryGet(PositionFieldIDs.TPOS.symbol, out string value) ? value : "";
			set => _data.Set(PositionFieldIDs.TPOS.symbol, value);
		}

		public double BidPrice {
			get => _data.TryGet(PositionFieldIDs.MD.bidPrice, out double value) ? value : 0;
			set => _data.Set(PositionFieldIDs.MD.bidPrice, value);
		}

		public long BidSize {
			get => _data.TryGet(PositionFieldIDs.MD.bidSize, out long value) ? value : 0;
			set => _data.Set(PositionFieldIDs.MD.bidSize, value);
		}

		public double AskPrice {
			get => _data.TryGet(PositionFieldIDs.MD.askPrice, out double value) ? value : 0;
			set => _data.Set(PositionFieldIDs.MD.askPrice, value);
		}

		public long AskSize {
			get => _data.TryGet(PositionFieldIDs.MD.askSize, out long value) ? value : 0;
			set => _data.Set(PositionFieldIDs.MD.askSize, value);
		}

		public double LastTraded {
			get => _data.TryGet(PositionFieldIDs.MD.tradePrice, out double value) ? value : 0;
			set => _data.Set(PositionFieldIDs.MD.tradePrice, value);
		}

		public double NetChange {
			get => _data.TryGet(PositionFieldIDs.MD.netChange, out double value) ? value : 0;
			set => _data.Set(PositionFieldIDs.MD.netChange, value);
		}

		public double PctChange {
			get => _data.TryGet(PositionFieldIDs.MD.pctChange, out double value) ? value : 0;
			set => _data.Set(PositionFieldIDs.MD.pctChange, value);
		}

		public long TotalVolume {
			get => _data.TryGet(PositionFieldIDs.MD.totalVolume, out long value) ? value : 0;
			set => _data.Set(PositionFieldIDs.MD.totalVolume, value);
		}

		public override string ClearingAcct {
			get => _data.TryGet(PositionFieldIDs.TPOS.clearingAccount, out string value) ? value : "";
			set => _data.Set(PositionFieldIDs.TPOS.clearingAccount, value);
		}

		public override string Trader {
			get => _data.TryGet(PositionFieldIDs.TPOS.portfolio, out string value) ? value : "";
			set => _data.Set(PositionFieldIDs.TPOS.portfolio, value);
		}

		public long OpenQty {
			get => _data.TryGet(PositionFieldIDs.TPOS.openQty, out long value) ? value : 0;
			set => _data.Set(PositionFieldIDs.TPOS.openQty, value);
		}

		public double OpenAvg {
			get => _data.TryGet(PositionFieldIDs.TPOS.avgCost, out double value) ? value : 0;
			set => _data.Set(PositionFieldIDs.TPOS.avgCost, value);
		}

		public long BuyQty {
			get => _data.TryGet(PositionFieldIDs.buyQty, out long value) ? value : 0;
			set => _data.Set(PositionFieldIDs.buyQty, value);
		}

		public double BuyAvg {
			get => _data.TryGet(PositionFieldIDs.buyAvg, out double value) ? value : 0;
			set => _data.Set(PositionFieldIDs.buyAvg, value);
		}

		public long SellQty {
			get => _data.TryGet(PositionFieldIDs.sellQty, out long value) ? value : 0;
			set => _data.Set(PositionFieldIDs.sellQty, value);
		}

		public double SellAvg {
			get => _data.TryGet(PositionFieldIDs.sellAvg, out double value) ? value : 0;
			set => _data.Set(PositionFieldIDs.sellAvg, value);
		}

		public long DayQty {
			get {
				return BuyQty - SellQty;
			}
		}

		public long CurrentQty {
			get {
				return OpenQty + DayQty;
			}
		}

		public double ContractSize {
			get {
				switch (SecType) {
					case CSVFieldIDs.SecurityTypes.Option:
					case CSVFieldIDs.SecurityTypes.OptionFuture:
					case CSVFieldIDs.SecurityTypes.OptionIndex:
						return 100;
					case CSVFieldIDs.SecurityTypes.Equity:
						return 1;
					default:
						return _data.TryGet(PositionFieldIDs.contractSize, out double value) ? value : 0;
				}
			}
			set {
				_data.Set(PositionFieldIDs.contractSize, value);
			}
		}

		public double TickSize {
			get {
				if (_data.TryGet(PositionFieldIDs.MD.tickSize, out double value)) {
					return value;
				} else {
					return 0.01;
				}
			}
			set {
				_data.Set(PositionFieldIDs.MD.tickSize, value);
			}
		}

		public double DisplayFactor {
			get {
				if (_data.TryGet(PositionFieldIDs.MD.displayFactor, out double value)) {
					return value;
				} else {
					return 1;
				}
			}
			set {
				_data.Set(PositionFieldIDs.MD.displayFactor, value);
			}
		}

		public double OpenPnL {
			get {
				if (LastTraded > 0) {
					if (OpenAvg != 0) {
						if (OpenQty > 0) // Open balance is LONG(BUY)
						{
							return OpenQty * (LastTraded - OpenAvg) * ContractSize;
						} else if (OpenQty < 0) //Open balance is SHORT(SELL)
						  {
							return -OpenQty * (OpenAvg - LastTraded) * ContractSize;
						}
					}
				}

				return 0;
			}
		}

		public double DayUnRealizedPnL {
			get {
				if (LastTraded > 0) {
					if (DayQty > 0) // Day balance is LONG(BUY)
					{
						return DayQty * (LastTraded - BuyAvg) * ContractSize;
					} else if (DayQty < 0)  //Day balance is SHORT(SELL)
					  {
						return -DayQty * (SellAvg - LastTraded) * ContractSize;
					}
				}

				return 0;
			}
		}

		public double DayRealizedPnL {
			get {
				if (BuyQty > SellQty) // Day balance is LONG(BUY)
				{
					return SellQty * (SellAvg - BuyAvg) * ContractSize;
				} else if (BuyQty < SellQty) //Day balance is SHORT(SELL)
				  {
					return BuyQty * (SellAvg - BuyAvg) * ContractSize;
				} else if (BuyQty > 0 && SellQty > 0 && BuyQty == SellQty) {
					return BuyQty * (SellAvg - BuyAvg) * ContractSize;
				} else {
					return 0;
				}
			}
		}

		public double DayPnL {
			get {
				return DayUnRealizedPnL + DayRealizedPnL;
			}
		}

		public double TotalPnL {
			get {
				return DayPnL + OpenPnL;
			}
		}

		public double MarketValue {
			get {
				return LastTraded * CurrentQty * ContractSize;
			}
		}

		public double SettlePrice {
			get {
				if (_data.TryGet(PositionFieldIDs.TPOS.settlePrice, out double value)) {
					return value;
				} else {
					return 0;
				}
			}
			set {
				_data.Set(PositionFieldIDs.TPOS.settlePrice, value);
			}
		}

		public override string Underlying {
			get {
				if (_data.TryGet(PositionFieldIDs.TPOS.undSymbol, out string value)) {
					return value;
				} else {
					return "";
				}
			}
			set {
				_data.Set(PositionFieldIDs.TPOS.undSymbol, value);
			}
		}

		public override string ExpDate {
			get {
				if (_data.TryGet(PositionFieldIDs.TPOS.expDate, out string value)) {
					return value;
				} else {
					return "";
				}
			}
			set {
				_data.Set(PositionFieldIDs.TPOS.expDate, value);
			}
		}

		public override string StrikePrice {
			get {
				if (_data.TryGet(PositionFieldIDs.TPOS.strike, out string value)) {
					return value;
				} else if (_data.TryGet(PositionFieldIDs.TPOS.strike, out double dval)) {
					_data.Set(PositionFieldIDs.TPOS.strike, dval.ToString());
					return value;
				} else {
					return "";
				}
			}
			set {
				_data.Set(PositionFieldIDs.TPOS.strike, value);
			}
		}

		public override string CallPut {
			get {
				if (_data.TryGet(PositionFieldIDs.TPOS.putCall, out string value)) {
					return value;
				} else {
					return "";
				}
			}
			set {
				_data.Set(PositionFieldIDs.TPOS.putCall, value);
			}
		}

		public override string SecType {
			get {
				if (_data.TryGet(PositionFieldIDs.TPOS.secType, out string value)) {
					return value;
				} else {
					return "";
				}
			}
			set {
				_data.Set(PositionFieldIDs.TPOS.secType, value);
			}
		}

		public string SecurityStatus {
			get {
				if (_data.TryGet(PositionFieldIDs.TPOS.securityStatus, out string value)) {
					return value;
				} else {
					return "";
				}
			}
			set {
				_data.Set(PositionFieldIDs.TPOS.securityStatus, value);
			}
		}

		private List<string> _omTags;
		public override List<string> OmTags {
			get {
				if (_omTags == null) {
					_omTags = new List<string>();
				}
				return _omTags;
			}
			set {
				_omTags = value;
			}
		}

		public override bool HasOrders {
			get {
				if (_omTags == null || _omTags.Count == 0) {
					return false;
				} else {
					return true;
				}
			}
		}

		public long Version {
			get {
				if (_data.TryGet(PositionFieldIDs.TPOS.version, out long value)) {
					return value;
				} else {
					return 0;
				}
			}
		}

		public void Set<T>(int key, T value)
		{
			_data.Set(key, value);
		}
	}
}
