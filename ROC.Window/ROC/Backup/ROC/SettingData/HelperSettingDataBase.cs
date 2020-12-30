using System;
using System.Collections.Generic;
using System.Text;

namespace ROC
{
	public abstract class HelperSettingDataBase
	{
		public string GetBaseSymbol(string symbolDetail)
		{
			int num = 0;
			if (Int32.TryParse(symbolDetail.Substring(symbolDetail.Length - 1, 1), out num))
			{
				switch (symbolDetail.Substring(symbolDetail.Length - 2, 1))
				{
					case GLOBAL.MonthCodes.January:
					case GLOBAL.MonthCodes.February:
					case GLOBAL.MonthCodes.March:
					case GLOBAL.MonthCodes.April:
					case GLOBAL.MonthCodes.May:
					case GLOBAL.MonthCodes.June:
					case GLOBAL.MonthCodes.July:
					case GLOBAL.MonthCodes.August:
					case GLOBAL.MonthCodes.September:
					case GLOBAL.MonthCodes.October:
					case GLOBAL.MonthCodes.November:
					case GLOBAL.MonthCodes.December:
						return symbolDetail.Substring(0, symbolDetail.Length - 2);
					default:
						break;
				}
			}
			return "";
		}
	}
}
