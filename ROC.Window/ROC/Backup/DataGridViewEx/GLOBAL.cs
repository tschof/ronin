using System;
using System.Collections.Generic;
using System.Text;

namespace DataGridViewEx
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
		}
	}
}
