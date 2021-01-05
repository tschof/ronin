using MonthCodes = Common.Constants.FuturesMonthCodes;

namespace ROC.SettingData
{
	public static class Utility
	{
		public static bool TryGetBaseSymbol(string symbolDetail, out string symbol)
		{
			int length = string.IsNullOrEmpty(symbolDetail) ? 0 : symbolDetail.Length;
			if ((length > 1) && char.IsDigit(symbolDetail[length - 1]))
			{
				char code = symbolDetail[length - 2];
				if (MonthCodes.IsMonthCode(code)) {
					symbol = string.Empty + code;
					return true;
				}
			}
			symbol = null;
			return false;
		}
	}
}
