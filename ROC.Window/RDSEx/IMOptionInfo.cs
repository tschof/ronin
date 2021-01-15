using System;

using CSVEx;

namespace RDSEx
{
	public class IMOptionInfo : ROCSecurity
	{
		public string DataSource { get; private set; }
		public string Exchange { get; private set; }
		public override string ExpDate { // yyyyMMdd
			get {
				if (_expDateText != null)
					return _expDateText;
				if (_expDateDate.HasValue)
					return _expDateDate.Value.ToString("yyyyMMdd");
				return "";
			}
			protected set {
				_expDateText = value;
				_expDateDate = null;
			}
		}
		public DateTime? ExpDateDT {
			get {
				if (_expDateDate.HasValue)
					return _expDateDate;

				if ((_expDateText != null) && DateTime.TryParse(_expDateText, out DateTime when)) {
					_expDateDate = when;
					return when;
				}

				return null;
			}
			set {
				_expDateDate = value;
				_expDateText = null;
			}
		}
		public string GenericMDSymbol { get; private set; }
		public string LongName { get; private set; }
		protected override string MaturityDay => "";
		public string MDSource { get; private set; } = "";
		public string MDSymbol { get => Symbol; set => Symbol = value; }
		public string OpraSymbol { get; private set; }
		public string OptionSymbol { get; private set; }
		public string RoninSymbol { get; private set; }
		public string StockSymbol { get; private set; }
		public string UndExpirationDate { get; private set; }

		public IMOptionInfo(RDSEx.WEB.OptionDesc option, IMSecurityInfo secinfo, string rocSymbol)
		{
			StockSymbol = option.stockSymbol;
			OptionSymbol = option.optionSymbol;
			OpraSymbol = option.opraSymbol;
			DataSource = option.exchange;
			StrikePrice = option.strike;
			ExpDate = option.expirationDate;
			ContractSize = option.contractSize;
			CallPut = option.putCall;
			if (option.tickSize != 0)
				TickSize = option.tickSize;
			LongName = option.longName;
			GenericMDSymbol = option.genericMDSymbol;
			UndExpirationDate = option.undExpirationDate;
			RoninSymbol = option.roninSymbol;
			Underlying = rocSymbol;

			if (secinfo != null) {
				MDSource = secinfo.MDSource;
				if (secinfo.SecType == CSVFieldIDs.SecurityTypes.Future) {
					SecType = CSVFieldIDs.SecurityTypes.OptionFuture;
					TickSize = secinfo.TickSize;
				} else {
					SecType = CSVFieldIDs.SecurityTypes.Option;
				}
			}
		}

		#region - private members -

		private string _expDateText = null;
		private DateTime? _expDateDate = null;

		#endregion // - private members -
	}
}
