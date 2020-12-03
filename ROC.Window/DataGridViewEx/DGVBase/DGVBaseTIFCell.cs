using System;
using System.Windows.Forms;

namespace DataGridViewEx
{
	public class DGVBaseTIFCell : DGVBaseCell
	{
		public override string GetDisplayValue(object value)
		{
			long tifCode = -1;
			if (value != null && value is long)
			{
				tifCode = (long)value;
				if (CSVEx.CSVFieldIDs.TIFCodes.Descriptions.ContainsKey(tifCode))
				{
					return CSVEx.CSVFieldIDs.TIFCodes.Descriptions[tifCode].ToUpper();
				}
				else
				{
					return tifCode.ToString();
				}
			}

			return "";
		}
	}
}
