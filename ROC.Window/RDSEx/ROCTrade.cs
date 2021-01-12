using System;
using System.Runtime.Serialization;

using CSVEx;
using Price = Common.Price;

namespace RDSEx
{
	[Serializable]
	public class ROCTrade : AssetShared, ISerializable
	{
		public readonly long AlgoDestID = 0;
		public string Account { get; private set; } = "";
		public string ActiveState { get; private set; } = "";
		public string ClearingStatus { get; private set; } = "";
		public double Commission { get; private set; } = 0;
		public string ContraBroker { get; private set; } = "";
		public string ContraFirm { get; private set; } = "";
		public readonly int CplxOrderType = 0;
		public long DestID { get; private set; } = 0;
		public string Exchange { get; private set; } = "";
		public long ExecQty { get; private set; } = 0;
		public DateTime ExecTime { get; private set; } = DateTime.Today;
		public string ExtDescription { get; private set; } = "";
		public string ExtTradeID { get; private set; } = "";
		public bool IsCanceled { get => LastModReasonID == 3; }
		public string LastModDate { get; private set; } = "";
		public string LastModReason { get; private set; } = "";
		public int LastModReasonID { get; private set; } = 0;
		public string LastModTime { get; private set; } = "";
		public string LocalAcct { get; private set; } = "";
		protected override string MaturityDay => _maturityDay;
		public string Note { get; private set; } = "";
		//public string OmExecTag { get; private set; } = ""; OmExecTag is ROC's equivalent of TradeID.
		public string OmTag { get; private set; } = "";
		public string OpenClose { get; private set; } = "";
		public string Portfolio { get; private set; } = "";
		public readonly string ProgramTrade = "";
		public string PutCall { get; private set; } = "";
		public long Qty { get; private set; } = 0;
		public string SettleDate { get; private set; } = "";
		public string ShortSaleFlag { get; private set; } = "";
		public long Side { get; private set; } = 0;
		public double Strike { get; private set; } = 0;
		public string Ticker { get; private set; } = "";
		public string TradeDate { get; private set; } = "";
		public string TradeGroup { get; private set; } = "";
		public string TradeID { get; private set; } = "";
		public string TraderAcronym { get; private set; } = "";
		public Price ExecPrice { get; private set; } = 0;
		public string TradeSource { get; private set; } = "";
		public string TradeTime { get; private set; } = "";
		public Price TradeValue {
			get {
				return Math.Round(Qty * ExecPrice.Value * ContractSize, 7);
			}
		}
		public string UndSecType { get; private set; } = "";
		public string UndSymbol { get; private set; } = "";
		public bool UpdateTrade => CplxOrderType == 5;
		public int Version { get; private set; } = 0;

		public ROCTrade(RDSEx.WEB.Execution trade, ROCOrder order) : base()
		{
			Update(trade, order);
		}

		public ROCTrade(RDSEx.WEB.TposTrade tposTrade)
		{
			Update(tposTrade);
		}

		public ROCTrade(CSVEx.CSV csv)
		{
			string sval;
			if (csv.TryGetValue(CSVFieldIDs.OmExecTag, out sval)) TradeID = sval;
			if (csv.TryGetValue(CSVFieldIDs.ClearingAcct, out sval)) Account = sval;
			if (csv.TryGetValue(CSVFieldIDs.Trader, out sval)) Trader = sval;
			if (csv.TryGetValue(CSVFieldIDs.LocalAcct, out sval)) LocalAcct = sval;
			if (csv.TryGetValue(CSVFieldIDs.Symbol, out sval)) Symbol = sval;
			if (csv.TryGetValue(CSVFieldIDs.ExpDate, out sval)) ExpDate = sval;
			if (csv.TryGetValue(CSVFieldIDs.MaturityDay, out sval)) _maturityDay = sval;
			if (csv.TryGetValue(CSVFieldIDs.CallPut, out sval)) CallPut = sval;
			if (csv.TryGetValue(CSVFieldIDs.Underlying, out sval)) Underlying = sval;
			if (csv.TryGetValue(CSVFieldIDs.SecType, out sval)) SecType = sval;
			if (csv.TryGetValue(CSVFieldIDs.CplxOrderType, out sval) && int.TryParse(sval, out int orderType)) CplxOrderType = orderType;
			if (csv.TryGetValue(CSVFieldIDs.ProgramTrade, out sval)) ProgramTrade = sval;

			double dval;
			if (csv.TryGetValue(CSVFieldIDs.ExecPrice, out dval)) ExecPrice = dval;
			if (csv.TryGetValue(CSVFieldIDs.StrikePrice, out dval)) StrikePrice = dval;
			if (csv.TryGetValue(CSVFieldIDs.ExecutionTime, out dval)) ExecTime = DateTime.FromOADate(dval);

			long lval;
			if (csv.TryGetValue(CSVFieldIDs.LastShares, out lval)) ExecQty = lval;
			if (csv.TryGetValue(CSVFieldIDs.ExchangeID, out lval)) DestID = lval;
			if (csv.TryGetValue(CSVFieldIDs.AlgoExchangeID, out lval)) AlgoDestID = lval;
			if (csv.TryGetValue(CSVFieldIDs.Side, out lval)) Side = lval;
			if (csv.TryGetValue(CSVFieldIDs.CplxOrderType, out lval)) CplxOrderType = (int)lval;
		}

		public void Update(RDSEx.WEB.Execution rocTrade, ROCOrder order)
		{
			OmTag = rocTrade.omTag;
			ExecTime = rocTrade.execTime.ToLocalTime();
			ExecQty = (long)rocTrade.execQty;
			ExecPrice = rocTrade.execPrice;
			TradeID = rocTrade.omExecTag;

			// Attach some of the order info with the trade
			Symbol = order.Symbol;
			Underlying = order.Underlying;
			StrikePrice = order.StrikePrice;
			CallPut = order.CallPut;
			LocalAcct = order.LocalAcct;
			Account = order.ClearingAcct;
			OpenClose = order.OpenClose;
			Side = order.Side;
			DestID = order.DestID;
		}

		public void Update(RDSEx.WEB.TposTrade tposTrade)
		{
			ActiveState = tposTrade.m_ActiveState;
			ClearingAcct = tposTrade.m_ClearingAccount;
			ClearingStatus = tposTrade.m_ClearingStatus;
			Commission = tposTrade.m_Commission;
			ContraBroker = tposTrade.m_ContraBroker;
			ContraFirm = tposTrade.m_ContraFirm;
			Exchange = tposTrade.m_Exchange;
			ExpDate = tposTrade.m_ExpDate;
			ExtDescription = tposTrade.m_ExtDescription;
			ExtTradeID = tposTrade.m_ExtTradeID;
			LastModDate = tposTrade.m_LastModDate;
			LastModReason = tposTrade.m_LastModReason;
			if (tposTrade.m_LastModReason.Contains("Deleted")) {
				LastModReasonID = 3;
			} else {
				LastModReasonID = tposTrade.m_LastModReasonID;
			}
			LastModTime = tposTrade.m_LastModTime;
			Note = tposTrade.m_Note;
			Portfolio = tposTrade.m_Portfolio;
			PutCall = tposTrade.m_PutCall;
			if (IsCanceled) {
				Qty = 0;
				ExecPrice = 0;
			} else {
				Qty = tposTrade.m_Qty;
				ExecPrice = tposTrade.m_Price;
			}
			SecType = tposTrade.m_SecType;
			SettleDate = tposTrade.m_SettleDate;
			ShortSaleFlag = tposTrade.m_ShortSaleFlag;
			Strike = tposTrade.m_Strike;
			Symbol = tposTrade.m_Symbol;
			Ticker = tposTrade.m_Ticker;
			TradeDate = tposTrade.m_TradeDate;
			TradeGroup = tposTrade.m_TradeGroup;
			TradeID = tposTrade.m_TradeID;
			TraderAcronym = tposTrade.m_TraderAcronym;
			TradeSource = tposTrade.m_TradeSource;
			TradeTime = tposTrade.m_TradeTime;
			UndSecType = tposTrade.m_UndSecType;
			UndSymbol = tposTrade.m_UndSymbol;
			Version = tposTrade.m_Version;
		}

		public void UpdateFromSecurity(string symbol, double tickSize, double contractSize)
		{
			if (string.IsNullOrEmpty(Symbol)) {
				Symbol = symbol;
			}
			TickSize = tickSize;
			ContractSize = contractSize;
		}

		#region - ISerialization -

		// Deserialization.
		protected ROCTrade(SerializationInfo info, StreamingContext context) : base(info, context)
		{
			AlgoDestID = info.GetInt64("AlgoDestID");
			Account = info.GetString("Account");
			ActiveState = info.GetString("ActiveState");
			ClearingStatus = info.GetString("ClearingStatus");
			Commission = info.GetDouble("Commission");
			ContraBroker = info.GetString("ContraBroker");
			ContraFirm = info.GetString("ContraFirm");
			CplxOrderType = info.GetInt32("CplxOrderType");
			DestID = info.GetInt64("DestID");
			Exchange = info.GetString("Exchange");
			ExecQty = info.GetInt64("ExecQty");
			ExecTime = info.GetDateTime("ExecTime");
			ExtDescription = info.GetString("ExtDescription");
			ExtTradeID = info.GetString("ExtTradeID");
			LastModDate = info.GetString("LastModDate");
			LastModReason = info.GetString("LastModReason");
			LastModReasonID = info.GetInt32("LastModReasonID");
			LastModTime = info.GetString("LastModTime");
			LocalAcct = info.GetString("LocalAcct");
			_maturityDay = info.GetString("MaturityDay");
			Note = info.GetString("Note");
			OmTag = info.GetString("OmTag");
			OpenClose = info.GetString("OpenClose");
			Portfolio = info.GetString("Portfolio");
			ProgramTrade = info.GetString("ProgramTrade");
			PutCall = info.GetString("PutCall");
			Qty = info.GetInt64("Qty");
			SettleDate = info.GetString("SettleDate");
			ShortSaleFlag = info.GetString("ShortSaleFlag");
			Side = info.GetInt64("Side");
			Strike = info.GetDouble("Strike");
			Ticker = info.GetString("Ticker");
			TradeDate = info.GetString("TradeDate");
			TradeGroup = info.GetString("TradeGroup");
			TradeID = info.GetString("TradeID");
			TraderAcronym = info.GetString("TraderAcronym");
			ExecPrice = info.GetDouble("ExecPrice");
			TradeSource = info.GetString("TradeSource");
			TradeTime = info.GetString("TradeTime");
			UndSecType = info.GetString("UndSecType");
			UndSymbol = info.GetString("UndSymbol");
			Version = info.GetInt32("Version");
		}

		// Serialization.
		public new void GetObjectData(SerializationInfo info, StreamingContext context)
		{
			base.GetObjectData(info, context); // Serialize AssetShared first.

			info.AddValue("AlgoDestID", AlgoDestID);
			info.AddValue("Account", Account);
			info.AddValue("ActiveState", ActiveState);
			info.AddValue("ClearingStatus", ClearingStatus);
			info.AddValue("Commission", Commission);
			info.AddValue("ContraBroker", ContraBroker);
			info.AddValue("ContraFirm", ContraFirm);
			info.AddValue("CplxOrderType", CplxOrderType);
			info.AddValue("DestID", DestID);
			info.AddValue("Exchange", Exchange);
			info.AddValue("ExecQty", ExecQty);
			info.AddValue("ExecTime", ExecTime);
			info.AddValue("ExtDescription", ExtDescription);
			info.AddValue("ExtTradeID", ExtTradeID);
			info.AddValue("LastModDate", LastModDate);
			info.AddValue("LastModReason", LastModReason);
			info.AddValue("LastModReasonID", LastModReasonID);
			info.AddValue("LastModTime", LastModTime);
			info.AddValue("LocalAcct", LocalAcct);
			info.AddValue("MaturityDay", _maturityDay);
			info.AddValue("Note", Note);
			info.AddValue("OmTag", OmTag);
			info.AddValue("OpenClose", OpenClose);
			info.AddValue("Portfolio", Portfolio);
			info.AddValue("ProgramTrade", ProgramTrade);
			info.AddValue("PutCall", PutCall);
			info.AddValue("Qty", Qty);
			info.AddValue("SettleDate", SettleDate);
			info.AddValue("ShortSaleFlag", ShortSaleFlag);
			info.AddValue("Side", Side);
			info.AddValue("Strike", Strike);
			info.AddValue("Ticker", Ticker);
			info.AddValue("TradeDate", TradeDate);
			info.AddValue("TradeGroup", TradeGroup);
			info.AddValue("TradeID", TradeID);
			info.AddValue("TraderAcronym", TraderAcronym);
			info.AddValue("ExecPrice", ExecPrice.Value);
			info.AddValue("TradeSource", TradeSource);
			info.AddValue("TradeTime", TradeTime);
			info.AddValue("UndSecType", UndSecType);
			info.AddValue("UndSymbol", UndSymbol);
			info.AddValue("Version", Version);
		}

		#endregion // - ISerialization -

		private string _maturityDay = "";
	}
}