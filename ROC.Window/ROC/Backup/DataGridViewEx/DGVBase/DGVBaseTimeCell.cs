using System;
using System.Windows.Forms;

namespace DataGridViewEx
{
	public class DGVBaseTimeCell : DGVBaseCell
	{
		private DGVBaseTimeColumn owningColumn
		{
			get
			{
				return (DGVBaseTimeColumn)OwningColumn;
			}
		}

		public override string GetDisplayValue(object value)
		{
			if (value != null && value is DateTime)
			{
				if (owningColumn.ToLocal)
				{
					return ((DateTime)value).ToLocalTime().ToString(owningColumn.FormatString).ToUpper();
				}
				else
				{
					return ((DateTime)value).ToString(owningColumn.FormatString).ToUpper();
				}
			}

			return DateTime.Now.ToString(owningColumn.FormatString).ToUpper();
		}
	}
}
