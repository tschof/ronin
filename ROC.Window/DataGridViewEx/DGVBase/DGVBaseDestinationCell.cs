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
			if (value is long destID) {
				if (owningColumn.DestIDToName.TryGetValue(destID, out string name)) {
					return name;
				} else if (destID == -1) {
					return "TPOS";
				} else {
					return destID.ToString();
				}
			}

			return "";
		}
	}
}
