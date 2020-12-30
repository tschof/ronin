using System;
using System.Windows.Forms;
using System.Drawing;

namespace DataGridViewEx
{
	public class DGVBaseStatusCell : DGVBaseCellNoPaint
	{
		private long _statusCode = -1;

		private Color _currentForeColor = Color.White;
		public Color CurrentForeColor
		{
			get
			{
				return _currentForeColor;
			}
		}

		private DGVBaseStatusColumn owningColumn
		{
			get
			{
				return (DGVBaseStatusColumn)OwningColumn;
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

				switch (_statusCode)
				{
					case CSVEx.CSVFieldIDs.StatusCodes.PartiallyFilled:
						if (selected)
						{
							cellStyle.SelectionForeColor = SetCellForeColor(owningColumn.PartialFilledColor);
						}
						else
						{
							cellStyle.ForeColor = SetCellForeColor(owningColumn.PartialFilledColor);
						}
						break;
					case CSVEx.CSVFieldIDs.StatusCodes.Filled:
						if (selected)
						{
							cellStyle.SelectionForeColor = SetCellForeColor(owningColumn.FilledColor);
						}
						else
						{
							cellStyle.ForeColor = SetCellForeColor(owningColumn.FilledColor);
						}
						break;
					case CSVEx.CSVFieldIDs.StatusCodes.DoneForDay:
						if (selected)
						{
							cellStyle.SelectionForeColor = SetCellForeColor(owningColumn.DoneForDayColor);
						}
						else
						{
							cellStyle.ForeColor = SetCellForeColor(owningColumn.DoneForDayColor);
						}
						break;
					case CSVEx.CSVFieldIDs.StatusCodes.Canceled:
						if (selected)
						{
							cellStyle.SelectionForeColor = SetCellForeColor(owningColumn.CancelledColor);
						}
						else
						{
							cellStyle.ForeColor = SetCellForeColor(owningColumn.CancelledColor);
						}
						break;
					case CSVEx.CSVFieldIDs.StatusCodes.Replaced:
						if (selected)
						{
							cellStyle.SelectionForeColor = SetCellForeColor(owningColumn.ReplacedColor);
						}
						else
						{
							cellStyle.ForeColor = SetCellForeColor(owningColumn.ReplacedColor);
						}
						break;
					case CSVEx.CSVFieldIDs.StatusCodes.PendingCancel:
						if (selected)
						{
							cellStyle.SelectionForeColor = SetCellForeColor(owningColumn.PendingCancelColor);
						}
						else
						{
							cellStyle.ForeColor = SetCellForeColor(owningColumn.PendingCancelColor);
						}
						break;
					case CSVEx.CSVFieldIDs.StatusCodes.Stopped:
						if (selected)
						{
							cellStyle.SelectionForeColor = SetCellForeColor(owningColumn.StoppedColor);
						}
						else
						{
							cellStyle.ForeColor = SetCellForeColor(owningColumn.StoppedColor);
						}
						break;
					case CSVEx.CSVFieldIDs.StatusCodes.Rejected:
						if (selected)
						{
							cellStyle.SelectionForeColor = SetCellForeColor(owningColumn.RejectedColor);
						}
						else
						{
							cellStyle.ForeColor = SetCellForeColor(owningColumn.RejectedColor);
						}
						break;
					case CSVEx.CSVFieldIDs.StatusCodes.Calculated:
						if (selected)
						{
							cellStyle.SelectionForeColor = SetCellForeColor(owningColumn.CalculatedColor);
						}
						else
						{
							cellStyle.ForeColor = SetCellForeColor(owningColumn.CalculatedColor);
						}
						break;
					case CSVEx.CSVFieldIDs.StatusCodes.Expired:
						if (selected)
						{
							cellStyle.SelectionForeColor = SetCellForeColor(owningColumn.OtherColor);
						}
						else
						{
							cellStyle.ForeColor = SetCellForeColor(owningColumn.OtherColor);
						}
						break;
					case CSVEx.CSVFieldIDs.StatusCodes.Open:
						if (selected)
						{
							cellStyle.SelectionForeColor = SetCellForeColor(owningColumn.OpenColor);
						}
						else
						{
							cellStyle.ForeColor = SetCellForeColor(owningColumn.OpenColor);
						}
						break;
					case CSVEx.CSVFieldIDs.StatusCodes.CancelRejected:
						if (selected)
						{
							cellStyle.SelectionForeColor = SetCellForeColor(owningColumn.CancelRejectedColor);
						}
						else
						{
							cellStyle.ForeColor = SetCellForeColor(owningColumn.CancelRejectedColor);
						}
						break;
					case CSVEx.CSVFieldIDs.StatusCodes.Corrected:
						if (selected)
						{
							cellStyle.SelectionForeColor = SetCellForeColor(owningColumn.CorrectedColor);
						}
						else
						{
							cellStyle.ForeColor = SetCellForeColor(owningColumn.CorrectedColor);
						}
						break;
					case CSVEx.CSVFieldIDs.StatusCodes.Busted:
						if (selected)
						{
							cellStyle.SelectionForeColor = SetCellForeColor(owningColumn.BustedColor);
						}
						else
						{
							cellStyle.ForeColor = SetCellForeColor(owningColumn.BustedColor);
						}
						break;
					case CSVEx.CSVFieldIDs.StatusCodes.ReplaceRejected:
						if (selected)
						{
							cellStyle.SelectionForeColor = SetCellForeColor(owningColumn.ReplaceRejectedColor);
						}
						else
						{
							cellStyle.ForeColor = SetCellForeColor(owningColumn.ReplaceRejectedColor);
						}
						break;
					case CSVEx.CSVFieldIDs.StatusCodes.ReplacePending:
						if (selected)
						{
							cellStyle.SelectionForeColor = SetCellForeColor(owningColumn.ReplacePendingColor);
						}
						else
						{
							cellStyle.ForeColor = SetCellForeColor(owningColumn.ReplacePendingColor);
						}
						break;
					case CSVEx.CSVFieldIDs.StatusCodes.Exposed:
						if (selected)
						{
							cellStyle.SelectionForeColor = SetCellForeColor(owningColumn.OtherColor);
						}
						else
						{
							cellStyle.ForeColor = SetCellForeColor(owningColumn.OtherColor);
						}
						break;
					case CSVEx.CSVFieldIDs.StatusCodes.DirectedThruIML:
						if (selected)
						{
							cellStyle.SelectionForeColor = SetCellForeColor(owningColumn.OtherColor);
						}
						else
						{
							cellStyle.ForeColor = SetCellForeColor(owningColumn.OtherColor);
						}
						break;
					case CSVEx.CSVFieldIDs.StatusCodes.Directed:
						if (selected)
						{
							cellStyle.SelectionForeColor = SetCellForeColor(owningColumn.OtherColor);
						}
						else
						{
							cellStyle.ForeColor = SetCellForeColor(owningColumn.OtherColor);
						}
						break;
					case CSVEx.CSVFieldIDs.StatusCodes.DirectedReject:
						if (selected)
						{
							cellStyle.SelectionForeColor = SetCellForeColor(owningColumn.OtherColor);
						}
						else
						{
							cellStyle.ForeColor = SetCellForeColor(owningColumn.OtherColor);
						}
						break;
					case CSVEx.CSVFieldIDs.StatusCodes.ROMCancelPending:
						if (selected)
						{
							cellStyle.SelectionForeColor = SetCellForeColor(owningColumn.OtherColor);
						}
						else
						{
							cellStyle.ForeColor = SetCellForeColor(owningColumn.OtherColor);
						}
						break;
					case CSVEx.CSVFieldIDs.StatusCodes.ROMReplacePending:
						if (selected)
						{
							cellStyle.SelectionForeColor = SetCellForeColor(owningColumn.OtherColor);
						}
						else
						{
							cellStyle.ForeColor = SetCellForeColor(owningColumn.OtherColor);
						}
						break;
					case CSVEx.CSVFieldIDs.StatusCodes.FilledAndCancelled:
						if (selected)
						{
							cellStyle.SelectionForeColor = SetCellForeColor(owningColumn.FilledColor);
						}
						else
						{
							cellStyle.ForeColor = SetCellForeColor(owningColumn.FilledColor);
						}
						break;
					case CSVEx.CSVFieldIDs.StatusCodes.ReplacedAndFilled:
						if (selected)
						{
							cellStyle.SelectionForeColor = SetCellForeColor(owningColumn.FilledColor);
						}
						else
						{
							cellStyle.ForeColor = SetCellForeColor(owningColumn.FilledColor);
						}
						break;
					case CSVEx.CSVFieldIDs.StatusCodes.New:
						if (selected)
						{
							cellStyle.SelectionForeColor = SetCellForeColor(owningColumn.OtherColor);
						}
						else
						{
							cellStyle.ForeColor = SetCellForeColor(owningColumn.OtherColor);
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
			if (value != null && value is long)
			{
				_statusCode = (long)value;
				if (CSVEx.CSVFieldIDs.StatusCodes.Descriptions.ContainsKey(_statusCode))
				{
					return CSVEx.CSVFieldIDs.StatusCodes.Descriptions[_statusCode];
				}
				else if (_statusCode == CSVEx.CSVFieldIDs.StatusCodes.FilledAndCancelled)
				{
					return CSVEx.CSVFieldIDs.StatusCodes.Descriptions[_statusCode];
				}
				else if (_statusCode == CSVEx.CSVFieldIDs.StatusCodes.ReplacedAndFilled)
				{
					return CSVEx.CSVFieldIDs.StatusCodes.Descriptions[_statusCode];
				}
				else
				{
					return _statusCode.ToString();
				}
			}

			return "";
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
