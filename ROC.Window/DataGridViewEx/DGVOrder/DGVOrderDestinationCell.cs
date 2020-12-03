using System;
using System.Windows.Forms;

namespace DataGridViewEx
{
	public class DGVOrderDestinationCell : DataGridViewTextBoxCell
	{
		private long _destID = -1;

		#region - Owning Column -

		private DGVOrderDestinationColumn _owningColumn;
		private DGVOrderDestinationColumn owningColumn
		{
			get
			{
				setOwningColumn();
				return _owningColumn;
			}
		}

		private void setOwningColumn()
		{
			if (_owningColumn == null)
			{
				_owningColumn = (DGVOrderDestinationColumn)OwningColumn;
				_owningColumn.Disposed += new EventHandler(owningColumn_Disposed);
			}
		}

		private void owningColumn_Disposed(object sender, EventArgs e)
		{
		}

		#endregion

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
					_destID = (long)value;
					if (owningColumn.DestIDToName.ContainsKey(_destID))
					{
						formattedValue = owningColumn.DestIDToName[_destID];
					}
				}

				base.Paint(graphics, clipBounds, cellBounds, rowIndex, cellState, value, formattedValue, errorText, cellStyle, advancedBorderStyle, paintParts);
			}
		}
	}
}
