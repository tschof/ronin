namespace Common
{
	public static class Constants
	{
		public static class FuturesMonthCodes
		{
			public const char January = 'F';
			public const char February = 'G';
			public const char March = 'H';
			public const char April = 'J';
			public const char May = 'K';
			public const char June = 'M';
			public const char July = 'N';
			public const char August = 'Q';
			public const char September = 'U';
			public const char October = 'V';
			public const char November = 'X';
			public const char December = 'Z';

			private const string _codes = "FGHJKMNQUVXZ";
			public static bool IsMonthCode(char code)
			{
				return _codes.IndexOf(code) >= 0;
			}

			public static int GetMonthIndex(char code)
			{
				return _codes.IndexOf(code);
			}

			public static char GetMonthCode(int monthIndex) // 1-based
			{
				return ((monthIndex > 0) && (monthIndex < 13)) ? _codes[monthIndex - 1] : (char)0;
			}
		}
	}
}
