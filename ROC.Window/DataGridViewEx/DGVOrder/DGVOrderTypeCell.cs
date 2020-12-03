using System;
using System.Windows.Forms;

namespace DataGridViewEx
{
	public class DGVOrderTypeCell : DataGridViewTextBoxCell
	{
		private long _typeCode = -1;

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
				if (value != null && value is long)
				{
					_typeCode = (long)value;

					if (CSVEx.CSVFieldIDs.OrderTypes.Descriptions.ContainsKey(_typeCode))
					{
						formattedValue = CSVEx.CSVFieldIDs.OrderTypes.Descriptions[_typeCode].ToUpper();
					}
				}

				base.Paint(graphics, clipBounds, cellBounds, rowIndex, cellState, value, formattedValue, errorText, cellStyle, advancedBorderStyle, paintParts);
			}
		}
	}
}
