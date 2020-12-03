using System;
using System.Collections.Generic;
using System.Text;

namespace RDSEx
{
	public static class GLOBAL
	{
		public static class OptionOnFuture
		{
			private static List<string> _showAs64th;
			public static List<string> ShowAs64TH
			{
				get
				{
					if (_showAs64th == null)
					{
						_showAs64th = new List<string>();
					}
					return _showAs64th;
				}
				set
				{
					_showAs64th = value;
				}
			}

			private static List<string> _plusOne;
			public static List<string> PlusOne
			{
				get
				{
					if (_plusOne == null)
					{
						_plusOne = new List<string>();
					}
					return _plusOne;
				}
				set
				{
					_plusOne = value;
				}
			}
		}

		public static class Future
		{
			private static List<string> _plusOne;
			public static List<string> PlusOne
			{
				get
				{
					if (_plusOne == null)
					{
						_plusOne = new List<string>();
					}
					return _plusOne;
				}
				set
				{
					_plusOne = value;
				}
			}
		}

		public static class MonthCodes
		{
			public const string January = "F";
			public const string February = "G";
			public const string March = "H";
			public const string April = "J";
			public const string May = "K";
			public const string June = "M";
			public const string July = "N";
			public const string August = "Q";
			public const string September = "U";
			public const string October = "V";
			public const string November = "X";
			public const string December = "Z";

			private static Dictionary<int, string> _map;
			public static Dictionary<int, string> Map
			{
				get
				{
					if (_map == null)
					{
						_map = new Dictionary<int, string>();
						_map.Add(1, "F");
						_map.Add(2, "G");
						_map.Add(3, "H");
						_map.Add(4, "J");
						_map.Add(5, "K");
						_map.Add(6, "M");
						_map.Add(7, "N");
						_map.Add(8, "Q");
						_map.Add(9, "U");
						_map.Add(10, "V");
						_map.Add(11, "X");
						_map.Add(12, "Z");
					}
					return _map;
				}
			}
		}
	}
}
