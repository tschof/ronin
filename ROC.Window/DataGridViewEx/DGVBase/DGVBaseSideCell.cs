using System;
using System.Windows.Forms;
using System.Drawing;

namespace DataGridViewEx
{
	public class DGVBaseSideCell : DGVBaseCellNoPaint
	{
		private long _sideCode = -1;

		private Color _currentForeColor = Color.White;
		public Color CurrentForeColor
		{
			get
			{
				return _currentForeColor;
			}
		}

		private DGVBaseSideColumn owningColumn
		{
			get
			{
				return (DGVBaseSideColumn)OwningColumn;
			}
		}

		protected override void Paint(System.Drawing.Graphics graphics, System.Drawing.Rectangle clipBounds, System.Drawing.Rectangle cellBounds, int rowIndex, DataGridViewElementStates cellState, object value, object formattedValue, string errorText, DataGridViewCellStyle cellStyle, DataGridViewAdvancedBorderStyle advancedBorderStyle, DataGridViewPaintParts paintParts)
		{
			if ((cellState & DataGridViewElementStates.Visible) == DataGridViewElementStates.Visible)
			{
				bool selected = false;

				if ((cellState & DataGridViewElementStates.Selected) == DataGridViewElementStates.Selected)
				{
					selected = true;
				}
				else
				{
					selected = false;
				}

				switch (_sideCode)
				{
					case CSVEx.CSVFieldIDs.SideCodes.Buy:
						if (selected)
						{
							cellStyle.SelectionForeColor = SetCellForeColor(owningColumn.BuyColor);
						}
						else
						{
							cellStyle.ForeColor = SetCellForeColor(owningColumn.BuyColor);
						}
						break;
					case CSVEx.CSVFieldIDs.SideCodes.Sell:
						if (selected)
						{
							cellStyle.SelectionForeColor =SetCellForeColor(owningColumn.SellColor);
						}
						else
						{
							cellStyle.ForeColor = SetCellForeColor(owningColumn.SellColor);
						}
						break;
					case CSVEx.CSVFieldIDs.SideCodes.Short:
						if (selected)
						{
							cellStyle.SelectionForeColor = SetCellForeColor(owningColumn.SellShortColor);
						}
						else
						{
							cellStyle.ForeColor = SetCellForeColor(owningColumn.SellShortColor);
						}
						break;
					case CSVEx.CSVFieldIDs.SideCodes.SSE:
						if (selected)
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

				base.Paint(graphics, clipBounds, cellBounds, rowIndex, cellState, value, formattedValue, errorText, cellStyle, advancedBorderStyle, paintParts);
			}
		}

		public override string GetDisplayValue(object value)
		{
			if (value is long sideCode)
			{
				_sideCode = sideCode;
				return CSVEx.CSVFieldIDs.SideCodes.GetDescription(sideCode);
			}

			return "";
		}

		private Color SetCellForeColor(Color foreColor)
		{
			_currentForeColor = foreColor;
			return _currentForeColor;
		}
	}
}
