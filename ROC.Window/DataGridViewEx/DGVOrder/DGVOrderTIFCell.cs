using System;
using System.Windows.Forms;

namespace DataGridViewEx
{
	public class DGVOrderTIFCell : DataGridViewTextBoxCell
	{
		private long _tifCode = -1;
		
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
					_tifCode = (long)value;
					if (CSVEx.CSVFieldIDs.TIFCodes.Descriptions.ContainsKey(_tifCode))
					{
						formattedValue = CSVEx.CSVFieldIDs.TIFCodes.Descriptions[_tifCode].ToUpper();
					}
				}

				base.Paint(graphics, clipBounds, cellBounds, rowIndex, cellState, value, formattedValue, errorText, cellStyle, advancedBorderStyle, paintParts);
			}
		}
	}
}
