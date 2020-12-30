namespace ROC.Tickets.AutoSpread
{
	internal struct Utility
	{
		internal static bool TryGetInt(string text, ref int member, out int value)
		{
			return TryGetConverted(text, ref member, out value, 0, n => n == int.MinValue, int.Parse);
		}

		internal static bool TryGetLong(string text, ref long member, out long value)
		{
			return TryGetConverted(text, ref member, out value, 0, n => n == int.MinValue, long.Parse);
		}

		internal static bool TryGetDouble(string text, ref double member, out double value)
		{
			return TryGetConverted(text, ref member, out value, 0, double.IsNaN, double.Parse);
		}

		internal static bool TryGetConverted<T>(string text, ref T member, out T value, T miss, System.Converter<T, bool> check, System.Converter<string, T> xform)
		{
			if (text == "") {
				value = miss;
				return false;
			}

			if (check(member))
				member = xform(text);

			value = member;
			return true;
		}
	}

	internal sealed class TagKeyItems
	{
		internal readonly string MasterOrderID;
		internal readonly string LegNumberText;
		internal readonly string LimitMarketPriceText;
		internal readonly string StopMarketPriceText;

		private int _legNumber = int.MinValue;
		internal bool TryGetLegNumber(out int value) => Utility.TryGetInt(LegNumberText, ref _legNumber, out value);

		internal TagKeyItems()
		{
			MasterOrderID = "";
			LegNumberText = "";
			LimitMarketPriceText = "";
			StopMarketPriceText = "";
		}

		internal TagKeyItems(string tag)
		{
			string[] items;
			int count;

			if (!string.IsNullOrEmpty(tag)) {
				items = tag.Split(new char[] { ';' }, System.StringSplitOptions.None);
				count = items.GetLength(0);
			} else {
				items = null;
				count = 0;
			}

			_legNumber = -1;
			MasterOrderID = count > 0 ? items[0] : "";
			LegNumberText = count > 1 ? items[1] : "";
			LimitMarketPriceText = count > 2 ? items[2] : "";
			StopMarketPriceText = count > 3 ? items[3] : "";
		}
	}

	public sealed class ParentTagKeyItems
	{
		public readonly string MasterOrderID;
		public readonly string LimitSpreadPriceText;
		public readonly string StopSpreadPriceText;
		public readonly string MasterSideSpreadText;
		public readonly string MasterQuantitySpreadText;

		public ParentTagKeyItems()
		{
			MasterOrderID = "";
			LimitSpreadPriceText = "";
			StopSpreadPriceText = "";
			MasterSideSpreadText = "";
			MasterQuantitySpreadText = "";
		}

		public ParentTagKeyItems(string tag)
		{
			string[] items;
			int count;

			if (!string.IsNullOrEmpty(tag)) {
				items = tag.Split(new char[] { ';' }, System.StringSplitOptions.None);
				count = items.GetLength(0);
			} else {
				items = null;
				count = 0;
			}

			MasterOrderID = count > 0 ? items[0] : "";
			LimitSpreadPriceText = count > 1 ? items[1] : "";
			StopSpreadPriceText = count > 2 ? items[2] : "";
			MasterSideSpreadText = count > 3 ? items[3] : "";
			MasterQuantitySpreadText = count > 4 ? items[4] : "";
		}

		private double _limitSpreadPrice = double.NaN;
		public bool TryGetLimitSpreadPrice(out double value) => Utility.TryGetDouble(LimitSpreadPriceText, ref _limitSpreadPrice, out value);

		private double _stopSpreadPrice = double.NaN;
		public bool TryGetStopSpreadPrice(out double value) => Utility.TryGetDouble(StopSpreadPriceText, ref _stopSpreadPrice, out value);

		private long _quantitySpread = long.MinValue;
		public bool TryGetMasterQtySpread(out long value) => Utility.TryGetLong(MasterQuantitySpreadText, ref _quantitySpread, out value);

		public string MasterSideSpreadName {
			get {
				switch (MasterSideSpreadText) {
					case "1":
						return "Buy";
					case "2":
						return "Sell";
					default:
						return "";
				}
			}
		}

		public long? MasterSideSpreadLong {
			get {
				switch (MasterSideSpreadText) {
					case "1":
						return CSVEx.CSVFieldIDs.SideCodes.Buy;
					case "2":
						return CSVEx.CSVFieldIDs.SideCodes.Sell;
					default:
						return null;
				}
			}
		}
	}
}
