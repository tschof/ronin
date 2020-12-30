using System;
using System.Windows.Forms;

namespace DataGridViewEx
{
	public class DGVBaseTypeCell : DGVBaseCell
	{
		public override string GetDisplayValue(object value)
		{
			long typeCode = -1;
			if (value != null && value is long)
			{
				typeCode = (long)value;
				if (CSVEx.CSVFieldIDs.OrderTypes.Descriptions.ContainsKey(typeCode))
				{
					return CSVEx.CSVFieldIDs.OrderTypes.Descriptions[typeCode].ToUpper();
				}
				else
				{
					return typeCode.ToString();
				}
			}

			return "";
		}
	}
}
