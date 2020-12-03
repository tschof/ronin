using System;
using System.Windows.Forms;
using System.Drawing;

namespace DataGridViewEx
{
	public class DGVOrderStatusCell : DataGridViewTextBoxCell
	{
		private long _statusCode = -1;
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

		private DGVOrderStatusColumn _owningColumn;
		private DGVOrderStatusColumn owningColumn
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
				_owningColumn = (DGVOrderStatusColumn)OwningColumn;
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
					_statusCode = (long)value;
					if (CSVEx.CSVFieldIDs.StatusCodes.Descriptions.ContainsKey(_statusCode))
					{
						formattedValue = CSVEx.CSVFieldIDs.StatusCodes.Descriptions[_statusCode].ToUpper();
					}

					if ((cellState & DataGridViewElementStates.Selected) == DataGridViewElementStates.Selected)
					{
						_selected = true;
					}
					else
					{
						_selected = false;
					}

					switch (_statusCode)
					{
						case CSVEx.CSVFieldIDs.StatusCodes.PartiallyFilled:
							if (_selected)
							{
								cellStyle.SelectionForeColor = SetCellForeColor(owningColumn.PartialFilledColor);
							}
							else
							{
								cellStyle.ForeColor = SetCellForeColor(owningColumn.PartialFilledColor);
							}
							break;
						case CSVEx.CSVFieldIDs.StatusCodes.Filled:
							if (_selected)
							{
								cellStyle.SelectionForeColor = SetCellForeColor(owningColumn.FilledColor);
							}
							else
							{
								cellStyle.ForeColor = SetCellForeColor(owningColumn.FilledColor);
							}
							break;
						case CSVEx.CSVFieldIDs.StatusCodes.DoneForDay:
							if (_selected)
							{
								cellStyle.SelectionForeColor = SetCellForeColor(owningColumn.DoneForDayColor);
							}
							else
							{
								cellStyle.ForeColor = SetCellForeColor(owningColumn.DoneForDayColor);
							}
							break;
						case CSVEx.CSVFieldIDs.StatusCodes.Canceled:
							if (_selected)
							{
								cellStyle.SelectionForeColor = SetCellForeColor(owningColumn.CancelledColor);
							}
							else
							{
								cellStyle.ForeColor = SetCellForeColor(owningColumn.CancelledColor);
							}
							break;
						case CSVEx.CSVFieldIDs.StatusCodes.Replaced:
							if (_selected)
							{
								cellStyle.SelectionForeColor = SetCellForeColor(owningColumn.ReplacedColor);
							}
							else
							{
								cellStyle.ForeColor = SetCellForeColor(owningColumn.ReplacedColor);
							}
							break;
						case CSVEx.CSVFieldIDs.StatusCodes.PendingCancel:
							if (_selected)
							{
								cellStyle.SelectionForeColor = SetCellForeColor(owningColumn.PendingCancelColor);
							}
							else
							{
								cellStyle.ForeColor = SetCellForeColor(owningColumn.PendingCancelColor);
							}
							break;
						case CSVEx.CSVFieldIDs.StatusCodes.Stopped:
							if (_selected)
							{
								cellStyle.SelectionForeColor = SetCellForeColor(owningColumn.StoppedColor);
							}
							else
							{
								cellStyle.ForeColor = SetCellForeColor(owningColumn.StoppedColor);
							}
							break;
						case CSVEx.CSVFieldIDs.StatusCodes.Rejected:
							if (_selected)
							{
								cellStyle.SelectionForeColor = SetCellForeColor(owningColumn.RejectedColor);
							}
							else
							{
								cellStyle.ForeColor = SetCellForeColor(owningColumn.RejectedColor);
							}
							break;
						case CSVEx.CSVFieldIDs.StatusCodes.Calculated:
							if (_selected)
							{
								cellStyle.SelectionForeColor = SetCellForeColor(owningColumn.CalculatedColor);
							}
							else
							{
								cellStyle.ForeColor = SetCellForeColor(owningColumn.CalculatedColor);
							}
							break;
						case CSVEx.CSVFieldIDs.StatusCodes.Expired:
							if (_selected)
							{
								cellStyle.SelectionForeColor = SetCellForeColor(owningColumn.OtherColor);
							}
							else
							{
								cellStyle.ForeColor = SetCellForeColor(owningColumn.OtherColor);
							}
							break;
						case CSVEx.CSVFieldIDs.StatusCodes.Open:
							if (_selected)
							{
								cellStyle.SelectionForeColor = SetCellForeColor(owningColumn.OpenColor);
							}
							else
							{
								cellStyle.ForeColor = SetCellForeColor(owningColumn.OpenColor);
							}
							break;
						case CSVEx.CSVFieldIDs.StatusCodes.CancelRejected:
							if (_selected)
							{
								cellStyle.SelectionForeColor = SetCellForeColor(owningColumn.CancelRejectedColor);
							}
							else
							{
								cellStyle.ForeColor = SetCellForeColor(owningColumn.CancelRejectedColor);
							}
							break;
						case CSVEx.CSVFieldIDs.StatusCodes.Corrected:
							if (_selected)
							{
								cellStyle.SelectionForeColor = SetCellForeColor(owningColumn.CorrectedColor);
							}
							else
							{
								cellStyle.ForeColor = SetCellForeColor(owningColumn.CorrectedColor);
							}
							break;
						case CSVEx.CSVFieldIDs.StatusCodes.Busted:
							if (_selected)
							{
								cellStyle.SelectionForeColor = SetCellForeColor(owningColumn.BustedColor);
							}
							else
							{
								cellStyle.ForeColor = SetCellForeColor(owningColumn.BustedColor);
							}
							break;
						case CSVEx.CSVFieldIDs.StatusCodes.ReplaceRejected:
							if (_selected)
							{
								cellStyle.SelectionForeColor = SetCellForeColor(owningColumn.ReplaceRejectedColor);
							}
							else
							{
								cellStyle.ForeColor = SetCellForeColor(owningColumn.ReplaceRejectedColor);
							}
							break;
						case CSVEx.CSVFieldIDs.StatusCodes.ReplacePending:
							if (_selected)
							{
								cellStyle.SelectionForeColor = SetCellForeColor(owningColumn.ReplacePendingColor);
							}
							else
							{
								cellStyle.ForeColor = SetCellForeColor(owningColumn.ReplacePendingColor);
							}
							break;
						case CSVEx.CSVFieldIDs.StatusCodes.Exposed:
							if (_selected)
							{
								cellStyle.SelectionForeColor = SetCellForeColor(owningColumn.OtherColor);
							}
							else
							{
								cellStyle.ForeColor = SetCellForeColor(owningColumn.OtherColor);
							}
							break;
						case CSVEx.CSVFieldIDs.StatusCodes.DirectedThruIML:
							if (_selected)
							{
								cellStyle.SelectionForeColor = SetCellForeColor(owningColumn.OtherColor);
							}
							else
							{
								cellStyle.ForeColor = SetCellForeColor(owningColumn.OtherColor);
							}
							break;
						case CSVEx.CSVFieldIDs.StatusCodes.Directed:
							if (_selected)
							{
								cellStyle.SelectionForeColor = SetCellForeColor(owningColumn.OtherColor);
							}
							else
							{
								cellStyle.ForeColor = SetCellForeColor(owningColumn.OtherColor);
							}
							break;
						case CSVEx.CSVFieldIDs.StatusCodes.DirectedReject:
							if (_selected)
							{
								cellStyle.SelectionForeColor = SetCellForeColor(owningColumn.OtherColor);
							}
							else
							{
								cellStyle.ForeColor = SetCellForeColor(owningColumn.OtherColor);
							}
							break;
						case CSVEx.CSVFieldIDs.StatusCodes.ROMCancelPending:
							if (_selected)
							{
								cellStyle.SelectionForeColor = SetCellForeColor(owningColumn.OtherColor);
							}
							else
							{
								cellStyle.ForeColor = SetCellForeColor(owningColumn.OtherColor);
							}
							break;
						case CSVEx.CSVFieldIDs.StatusCodes.ROMReplacePending:
							if (_selected)
							{
								cellStyle.SelectionForeColor = SetCellForeColor(owningColumn.OtherColor);
							}
							else
							{
								cellStyle.ForeColor = SetCellForeColor(owningColumn.OtherColor);
							}
							break;
						case 99:
							formattedValue = "Filled-Canceled";
							if (_selected)
							{
								cellStyle.SelectionForeColor = SetCellForeColor(owningColumn.FilledColor);
							}
							else
							{
								cellStyle.ForeColor = SetCellForeColor(owningColumn.FilledColor);
							}
							break;
						case CSVEx.CSVFieldIDs.StatusCodes.New:
							if (_selected)
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
