using System;
using System.Windows.Forms;

namespace DataGridViewEx
{
	public class DGVBaseTextCell : DGVBaseCell
	{
		private DGVBaseTextColumn owningColumn
		{
			get
			{
				return (DGVBaseTextColumn)OwningColumn;
			}
		}

		public override string GetDisplayValue(object value)
		{
			if (value != null && value.ToString() != "")
			{
				if (owningColumn.ShowLength == 0)
				{
					return value.ToString().ToUpper();
				}
				else
				{
					return value.ToString().Substring(0, owningColumn.ShowLength).ToUpper();
				}
			}

			return "";
		}
	}
}
