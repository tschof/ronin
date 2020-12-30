using MarketData;

namespace ROC.Lists
{
	internal static class Utility
	{
		internal static void ApplyRowDelta(System.Data.DataRow row, Book delta, bool useExtendedLastTrade, bool wantLatency)
		{
			applyRowDelta(
				(k, d) => row[k] = d,
				(k, l) => row[k] = l,
				(k, s) => row[k] = s,
				delta,
				useExtendedLastTrade,
				wantLatency);
		}

		internal static void ApplyRowDelta(System.Data.DataRowView row, Book delta, bool useExtendedLastTrade, bool wantLatency)
		{
			applyRowDelta(
				(k, d) => row[k] = d,
				(k, l) => row[k] = l,
				(k, s) => row[k] = s,
				delta,
				useExtendedLastTrade,
				wantLatency);
		}

		// Need the Action<> args to use this method with different
		// 'row' types: DataRow and DataViewRow.
		private static void applyRowDelta(
			System.Action<string, double> setDouble,
			System.Action<string, long> setLong,
			System.Action<string, string> setString,
			Book delta,
			bool useExtendedLastTraded,
			bool wantLatency)
		{
			double price, change;
			long size;
			string text;

			if (delta.TryGetField(Book.FieldEnum.BidPrice, out price))
				setDouble("BidPrice", price);

			if (delta.TryGetField(Book.FieldEnum.BidSize, out size))
				setLong("BidSize", size);

			if (delta.TryGetField(Book.FieldEnum.BidPartId, out text))
				setString("BidPartID", text);

			if (delta.TryGetField(Book.FieldEnum.BidHigh, out price))
				setDouble("BidHigh", price);

			if (delta.TryGetField(Book.FieldEnum.BidLow, out price))
				setDouble("BidLow", price);

			if (delta.TryGetField(Book.FieldEnum.AskPrice, out price))
				setDouble("AskPrice", price);

			if (delta.TryGetField(Book.FieldEnum.AskSize, out size))
				setLong("AskSize", size);

			if (delta.TryGetField(Book.FieldEnum.AskPartId, out text))
				setString("AskPartID", text);

			if (delta.TryGetField(Book.FieldEnum.AskHigh, out price))
				setDouble("AskHigh", price);

			if (delta.TryGetField(Book.FieldEnum.AskLow, out price))
				setDouble("AskLow", price);

			double closePrice;
			if (delta.TryGetNonZero(Book.FieldEnum.ClosePrice, out closePrice))
				setDouble("Close", closePrice);
			else if (delta.TryGetNonZero(Book.FieldEnum.PrevClosePrice, out closePrice))
				setDouble("Close", closePrice);
			else
				closePrice = double.NaN;

			if (delta.TryGetNonZero(Book.FieldEnum.TradePrice, out price))
				setDouble("LastTraded", price);
			else if (useExtendedLastTraded && !double.IsNaN(closePrice))
				setDouble("LastTraded", closePrice);

			if (delta.TryGetField(Book.FieldEnum.TradePartId, out text))
				setString("TradePartID", text);

			if (delta.TryGetField(Book.FieldEnum.TradeVolume, out size))
				setLong("TradeVolume", size);

			if (delta.TryGetField(Book.FieldEnum.TradeTick, out text))
				setString("Tick", text);

			if (delta.TryGetField(Book.FieldEnum.NetChange, out change))
				setDouble("NetChange", change);

			if (delta.TryGetField(Book.FieldEnum.PctChange, out change))
				setDouble("PctChange", change);

			if (delta.TryGetField(Book.FieldEnum.TotalVolume, out size))
				setLong("Volume", size);

			if (delta.TryGetField(Book.FieldEnum.HighPrice, out price))
				setDouble("High", price);

			if (delta.TryGetField(Book.FieldEnum.LowPrice, out price))
				setDouble("Low", price);

			if (delta.TryGetField(Book.FieldEnum.OpenPrice, out price))
				setDouble("Open", price);

			if (delta.TryGetField(Book.FieldEnum.Vwap, out price))
				setDouble("Vwap", price);

			if (delta.TryGetField(Book.FieldEnum.OpenInterest, out size))
				setLong("OpenInterest", size);

			if (delta.TryGetField(Book.FieldEnum.SettlePrice, out price))
				setDouble("SettlePrice", price);

			if (delta.TryGetField(Book.FieldEnum.DisplayConversionFactor, out change))
				setDouble("DisplayFactor", change);
			else
				setDouble("DisplayFactor", 1.0);

			if (delta.TryGetField(Book.FieldEnum.SecurityStatus, out text)) {
				switch (text.ToLower()) {
					case "none":
						break;
					case "normal":
						setString("SecurityStatus", string.Empty);
						break;
					default:
						setString("SecurityStatus", text);
						break;
				}
			}

			if (wantLatency) {
				if (delta.TryGetNonEmpty(Book.FieldEnum.LineTime, out text))
					setString("LineTime", text);

				if (delta.TryGetField(Book.FieldEnum.uDecodedTime, out price))
					setString("DecodeTime", price.ToString("F4"));

				if (delta.TryGetField(Book.FieldEnum.uServerSentTime, out System.DateTime when))
					setString("ServerSentTime", when.ToString("HH:mm:ss.fff"));
			}
		}
	}
}
