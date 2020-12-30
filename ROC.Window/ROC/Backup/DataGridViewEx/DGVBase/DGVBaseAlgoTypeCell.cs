using System;
using System.Collections.Generic;
using System.Text;

namespace DataGridViewEx
{
	public class DGVBaseAlgoTypeCell : DGVBaseCell
	{
		public override string GetDisplayValue(object value)
		{
			long algoType = 0;
			if (value != null && value is long)
			{
				algoType = (long)value;
				if (CSVEx.CSVFieldIDs.AlgoTypes.Descriptions.ContainsKey(algoType))
				{
					return CSVEx.CSVFieldIDs.AlgoTypes.Descriptions[algoType].ToUpper();
				}
				else
				{
					return algoType.ToString();
				}
			}

			return "";
		}
	}
}
