using System;
using System.Windows.Forms;

namespace DataGridViewEx
{
	public class DGVOrderTimeCell : DataGridViewTextBoxCell
	{
		public override object DefaultNewRowValue
		{
			get
			{
				return null;
			}
		}
		protected override void Paint(System.Drawing.Graphics graphics, System.Drawing.Rectangle clipBounds, System.Drawing.Rectangle cellBounds, int rowIndex, DataGridViewElementStates cellState, object value, object formattedValue, string errorText, DataGridViewCellStyle cellStyle, DataGridViewAdvancedBorderStyle advancedBorderStyle, DataGridViewPaintParts paintParts)
		{
			if ((cellState & DataGridViewElementStates.Visible) == DataGridViewElementStates.Visible)
			{
				if (value != null && value is DateTime)
				{
					formattedValue = ((DateTime)value).ToLocalTime().ToShortTimeString();
				}

				base.Paint(graphics, clipBounds, cellBounds, rowIndex, cellState, value, formattedValue, errorText, cellStyle, advancedBorderStyle, paintParts);
			}
		}
	}
}
