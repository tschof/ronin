using System.Collections.Generic;

namespace RDSEx
{
	public class IMSecurityInfo : IMSecurityBase
	{
		public string BaseSymbol { get; private set; } = "";
		public string CallPut { get; private set; } = "";
		public string DataSourceInfo { get; private set; } = "";
		public int DecimalPrecision { get; private set; } = 0;
		public string DisplayFormat { get; private set; } = "";
		public string Exchange { get; private set; } = "";
		public int ExpDay { get; private set; } = 0;
		public int ExpMonth { get; private set; } = 0;
		public int ExpYear { get; private set; } = 0;
		public string FullSymbol { get; private set; } = "";
		public string GenericMDSymbol { get; private set; } = "";
		public string RocSymbol { get; private set; } = "";
		public double Strike { get; private set; } = 0;
		public string Ticker { get; private set; } = "";
		public string UndExpirationDate { get; private set; } = "";
		public string UndInstrumentId { get; private set; } = "";

		public IMSecurityInfo(RDSEx.WEB.SecurityDesc sec, string rocSymbol) : base()
		{
			RocSymbol = rocSymbol;
			BaseSymbol = sec.baseSymbol;
			ContractSize = sec.contractSize;
			DataSourceInfo = sec.dataSourceInfo;
			DecimalPrecision = sec.DecimalPrecision;
			DisplayFormat = sec.DisplayFormat;
			Exchange = sec.exchange;
			ExpDay = sec.ExpDay;
			ExpirationText = sec.expirationDate;
			ExpMonth = sec.ExpMonth;
			ExpYear = sec.ExpYear;
			FullSymbol = sec.fullSymbol;
			GenericMDSymbol = sec.genericMDSymbol;
			LongName = sec.longName;
			CallPut = sec.PutCall;

			if (sec.genericMDSymbol.Contains(".IDX")) {
				// This from IM means it is an option index
				SecType = CSVEx.CSVFieldIDs.SecurityTypes.OptionIndex;
			} else {
				SecType = sec.securityType;
			}
			Strike = sec.Strike;
			Ticker = sec.Ticker;
			if (sec.tickSize != 0) {
				TickSize = sec.tickSize;
			}
			UndExpirationDate = sec.UndExpirationDate;
			UndInstrumentId = sec.UndInstrumentId;
		}
	}
}
