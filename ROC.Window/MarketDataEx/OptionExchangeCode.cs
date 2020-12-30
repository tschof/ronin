using System.Collections.Generic;

namespace MarketData
{
	public sealed class OptionExchangeCode
	{
		public const string CBOE = "C";
		public const string C2 = "W";
		public const string BOX = "B";
		public const string AMEX = "A";
		public const string PHLX = "X";
		public const string PCX = "N";
		public const string ISE = "I";
		public const string NSDQ = "Q";
		public const string BATS = "Z";

		public static bool TryGetName(string code, out string name)
		{
			int i = _exchanges.FindIndex(n => n.Code == code);
			if (i < 0) {
				name = null;
				return false;
			}
			name = _exchanges[i].Name;
			return true;
		}

		public static void AddOrReplace(Dictionary<string, string> exchanges)
		{
			Dictionary<string, string>.Enumerator i = exchanges.GetEnumerator();
			while (i.MoveNext()) {
				string name = i.Current.Key;
				string code = i.Current.Value;

				int index = _exchanges.FindIndex(n => n.Name == name);
				var entry = (code, name);

				if (index >= 0) {
					_exchanges[index] = entry;
				} else {
					_exchanges.Add(entry);
				}
			}
		}

		public static void ForEach(System.Action<string, string> action)
		{
			foreach (var entry in _exchanges)
				action(entry.Code, entry.Name);
		}

		private static List<(string Code, string Name)> _exchanges = new List<(string Code, string Name)>() {
				("C", "CBOE"),
				("W", "C2"),
				("B", "BOX"),
				("A", "AMEX"),
				("X", "PHLX"),
				("N", "PCX"),
				("I", "ISE"),
				("Q", "NSDQ"),
				("Z", "BATS")
			};
	}
}
