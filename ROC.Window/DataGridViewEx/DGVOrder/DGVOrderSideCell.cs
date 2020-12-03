using System;
using System.Windows.Forms;
using System.Drawing;

namespace DataGridViewEx
{
	public class DGVOrderSideCell : DataGridViewTextBoxCell
	{
		private long _sideCode = -1;
		private bool _selected = false;

		private Color _currentForeColor = Color.White;
		public Color CurrentForeColor
		{
			get
			{
				return _currentForeColor;
			}
		}

		#region - Owning Column -

		private DGVOrderSideColumn _owningColumn;
		private DGVOrderSideColumn owningColumn
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
				_owningColumn = (DGVOrderSideColumn)OwningColumn;
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
					_sideCode = (long)value;
					if (CSVEx.CSVFieldIDs.SideCodes.Descriptions.ContainsKey(_sideCode))
					{
						formattedValue = CSVEx.CSVFieldIDs.SideCodes.Descriptions[_sideCode].ToUpper();
					}

					if ((cellState & DataGridViewElementStates.Selected) == DataGridViewElementStates.Selected)
					{
						_selected = true;
					}
					else
					{
						_selected = false;
					}

					switch (_sideCode)
					{
						case CSVEx.CSVFieldIDs.SideCodes.Buy:
							if (_selected)
							{
								cellStyle.SelectionForeColor = SetCellForeColor(owningColumn.BuyColor);
							}
							else
							{
								cellStyle.ForeColor = SetCellForeColor(owningColumn.BuyColor);
							}
							break;
						case CSVEx.CSVFieldIDs.SideCodes.Sell:
							if (_selected)
							{
								cellStyle.SelectionForeColor = SetCellForeColor(owningColumn.SellColor);
							}
							else
							{
								cellStyle.ForeColor = SetCellForeColor(owningColumn.SellColor);
							}
							break;
						case CSVEx.CSVFieldIDs.SideCodes.Short:
							if (_selected)
							{
								cellStyle.SelectionForeColor = SetCellForeColor(owningColumn.SellShortColor);
							}
							else
							{
								cellStyle.ForeColor = SetCellForeColor(owningColumn.SellShortColor);
							}
							break;
						case CSVEx.CSVFieldIDs.SideCodes.SSE:
							if (_selected)
							{
								cellStyle.SelectionForeColor = SetCellForeColor(owningColumn.SSEColor);
							}
							else
							{
								cellStyle.ForeColor = SetCellForeColor(owningColumn.SSEColor);
							}
							break;
						default:
							break;
					}
				}

				base.Paint(graphics, clipBounds, cellBounds, rowIndex, cellState, value, formattedValue, errorText, cellStyle, advancedBorderStyle, paintParts);
			}
		}

		private Color SetCellForeColor(Color foreColor)
		{
			if (_currentForeColor != foreColor)
			{
				_currentForeColor = foreColor;
			}

			return _currentForeColor;
		}
	}
}
