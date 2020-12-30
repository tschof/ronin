using System;
using System.Windows.Forms;

namespace DataGridViewEx
{
	public class DGVBaseDestinationCell : DGVBaseCell
	{
		private DGVBaseDestinationColumn owningColumn
		{
			get
			{
				return (DGVBaseDestinationColumn)OwningColumn;
			}
		}

		public override string GetDisplayValue(object value)
		{
			long destID = -1;
			if (value != null && value is long)
			{
				destID = (long)value;
				if (owningColumn.DestIDToName.ContainsKey(destID))
				{
					return owningColumn.DestIDToName[destID];
				}
				else
				{
					if (destID == -1)
					{
						return "TPOS";
					}
					else
					{
						return destID.ToString();
					}
				}
			}

			return "";
		}
	}
}
