using System;
using System.Windows.Forms;
using System.ComponentModel;
using System.Diagnostics;
using System.Drawing;

using NumberEx;

namespace DataGridViewEx
{
	public class DGVNumberCell : DataGridViewTextBoxCell
	{
		private object _lastValue;

		private bool _isInEditMode;
		private bool isInEditMode
		{
			get
			{
				if (owningGrid.CurrentCell != null && owningGrid.CurrentCell.RowIndex == RowIndex && owningGrid.CurrentCell.ColumnIndex == ColumnIndex && owningGrid.IsCurrentCellInEditMode)
				{
					_isInEditMode = true;
				}
				else
				{
					_isInEditMode = false;
				}

				return _isInEditMode;
			}
		}

		private double _lastDoubleValue;
		private double _doubleValue;
		private long _lastLongValue;
		private long _longValue;

		private string _formattedValue;
		private int _decimalPlace;

		private System.Timers.Timer _notifyTimer;

		private bool _inNotifyMode = false;
		private int _notifyDirection = 0;

		// Save the last cell size after repaint
		private Rectangle _lastCellBound;
		private Rectangle _customCellBound;

		private FractionFixed _fractionFixed;
		private FractionFixed fractionFixed
		{
			get
			{
				if (_fractionFixed == null)
				{
					_fractionFixed = new FractionFixed();
				}
				return _fractionFixed;
			}
		}

		private ROCWatchList _owningGrid;
		private ROCWatchList owningGrid
		{
			get
			{
				setOwningGrid();
				return _owningGrid;
			}
		}

		private DGVNumberColumn _owningColumn;
		private DGVNumberColumn owningColumn
		{
			get
			{
				setOwningColumn();
				return _owningColumn;
			}
		}

		private void setOwningGrid()
		{
			if (_owningGrid == null)
			{
				_owningGrid = (ROCWatchList)DataGridView;
				_owningGrid.RowsRemoved += new DataGridViewRowsRemovedEventHandler(owningGrid_RowsRemoved);
			}
		}

		private void setOwningColumn()
		{
			if (_owningColumn == null)
			{
				_owningColumn = (DGVNumberColumn)OwningColumn;
				_owningColumn.Disposed += new EventHandler(owningColumn_Disposed);
			}
		}

		private void owningColumn_Disposed(object sender, EventArgs e)
		{
			DisableNotify();
		}

		private void owningGrid_RowsRemoved(object sender, DataGridViewRowsRemovedEventArgs e)
		{
			if (RowIndex == -1)  // Deleted RowIndex is allways -1
			{
				DisableNotify();
			}
		}

		private void DisableNotify()
		{
			StopNotify();

			if (_notifyTimer != null)
			{
				_notifyTimer.Elapsed -= new System.Timers.ElapsedEventHandler(Notify_Elapsed);
				_notifyTimer = null;
			}
		}

		private void StartNotify()
		{
			if (owningGrid != null && owningColumn.IsNotified && this.RowIndex >= 0)
			{
				if (_notifyTimer == null)
				{
					_notifyTimer = new System.Timers.Timer(owningColumn.NotifyDuration);
					_notifyTimer.Elapsed += new System.Timers.ElapsedEventHandler(Notify_Elapsed);
				}
				else
				{
					// reset
					StopNotify();
				}
				_notifyTimer.Enabled = true;

				if (!_inNotifyMode)
				{
					_inNotifyMode = true;
					DataGridView.InvalidateCell(this);
				}
			}
		}

		private void StopNotify()
		{
			if (_notifyTimer != null)
			{
				_notifyTimer.Enabled = false;
			}
		}

		private void Notify_Elapsed(object sender, System.Timers.ElapsedEventArgs e)
		{
			StopNotify();

			try
			{
				if (_inNotifyMode)
				{
					_inNotifyMode = false;
					DataGridView.InvalidateCell(this);
				}
			}
			catch (Exception ex)
			{
				Debug.Assert(!Debugger.IsAttached, ex.Message, ex.StackTrace);
			}
		}

		public override object DefaultNewRowValue
		{
			get
			{
				return null;
			}
		}

		protected override object GetFormattedValue(object value, int rowIndex, ref DataGridViewCellStyle cellStyle, TypeConverter valueTypeConverter, TypeConverter formattedValueTypeConverter, DataGridViewDataErrorContexts context)
		{
			if (value != null)
			{
				if (_lastValue != value)
				{
					switch (value.GetType().Name)
					{
						case "Decimal":
						case "Double":
							_doubleValue = (double)value;

							if (_lastDoubleValue != _doubleValue)
							{
								if (_lastDoubleValue < _doubleValue)
								{
									_notifyDirection = 1;
								}
								else if (_lastDoubleValue > _doubleValue)
								{
									_notifyDirection = -1;
								}

								_lastDoubleValue = _doubleValue;
							}
							break;
						case "Int":
						case "Int64":
						case "Long":
							_longValue = (long)value;

							if (_lastLongValue != _longValue)
							{
								if (_lastLongValue < _longValue)
								{
									_notifyDirection = 1;
								}
								else if (_lastLongValue > _longValue)
								{
									_notifyDirection = -1;
								}

								_lastLongValue = _longValue;
							}
							break;
						case "String":
						default:
							break;
					}

					_lastValue = value;
					if (_notifyDirection != 0)
					{
						StartNotify();
					}
				}
			}

			return base.GetFormattedValue(value, rowIndex, ref cellStyle, valueTypeConverter, formattedValueTypeConverter, context);
		}

		protected override void Paint(Graphics graphics, Rectangle clipBounds, Rectangle cellBounds, int rowIndex, DataGridViewElementStates cellState, object value, object formattedValue, string errorText, DataGridViewCellStyle cellStyle, DataGridViewAdvancedBorderStyle advancedBorderStyle, DataGridViewPaintParts paintParts)
		{
			if ((cellState & DataGridViewElementStates.Visible) == DataGridViewElementStates.Visible)
			{
				Color _foreColor = cellStyle.ForeColor;

				switch (owningColumn.NotifyType)
				{
					case NotifyTypes.ForeGround:
					case NotifyTypes.Border:
						SetForeColor(ref cellStyle, paintParts);
						break;
				}

				if (value != null)
				{
					GetFormattedValue(value);
					formattedValue = _formattedValue;
				}

				base.Paint(graphics, clipBounds, cellBounds, rowIndex, cellState, value, formattedValue, errorText, cellStyle, advancedBorderStyle, paintParts);

				switch (owningColumn.NotifyType)
				{
					case NotifyTypes.Border:
						SetBorderColor(graphics, cellBounds, paintParts);
						break;
				}

				// reset notification direction
				_notifyDirection = 0;

				cellStyle.ForeColor = _foreColor;
			}
		}

		private void SetForeColor(ref DataGridViewCellStyle cellStyle, DataGridViewPaintParts paintParts)
		{
			if (!isInEditMode && (paintParts & DataGridViewPaintParts.ContentForeground) == DataGridViewPaintParts.ContentForeground)
			{
				if (_inNotifyMode)
				{
					if (_notifyDirection > 0)
					{
						cellStyle.ForeColor = owningColumn.NotifyPositiveColor;
					}
					else if (_notifyDirection < 0)
					{
						cellStyle.ForeColor = owningColumn.NotifyNegativeColor;
					}
				}
			}
		}

		private void GetFormattedValue(object value)
		{
			//if (RowIndex >= 0)
			//{
				//if (owningGrid["TickSize", RowIndex].Value != null)
				//{
				//    _fractionFixed.TickSize = (double)owningGrid["TickSize", RowIndex].Value;
				//}

				//if (owningGrid["DisplayFactor", RowIndex].Value != null)
				//{
				//    _fractionFixed.DisplayFactor = (double)owningGrid["DisplayFactor", RowIndex].Value;
				//}

				//switch (value.GetType().Name)
				//{
				//    case "Decimal":
				//    case "Double":
				//        if (owningColumn.IsFraction)
				//        {
				//            _lastDoubleValue = Math.Round(_lastDoubleValue, 7);
				//            _formattedValue = _lastDoubleValue.ToString("###,###,###,###,###,###.#######");
				//        }
				//        else
				//        {
				//            _lastDoubleValue = Math.Round(_lastDoubleValue, 7);
				//            _formattedValue = _lastDoubleValue.ToString("###,###,###,###,###,###.#######");
				//        }
				//        break;
				//    case "Int":
				//    case "Int64":
				//    case "Long":
				//        _formattedValue = (_lastLongValue).ToString("N0");
				//        break;
				//    case "String":
				//    default:
				//        break;
				//}

				//if (_formattedValue != "" && owningColumn.IsPercent)
				//{
				//    _formattedValue = String.Concat(new object[] { _formattedValue, "%" });
				//}
			//}

			switch (value.GetType().Name)
			{
			    case "Decimal":
			    case "Double":
					if (owningColumn.IsFraction)
					{
						//fractionFixed.TickSize = (double)OwningRow.Cells["TickSize"].Value;
						//fractionFixed.DisplayFactor = (double)OwningRow.Cells["DisplayFactor"].Value;

						_formattedValue = fractionFixed.ToString(_lastDoubleValue);
					}
					else
					{
						_formattedValue = (_lastDoubleValue).ToString("N7").Trim(new char[] { '0' });
						_decimalPlace = _formattedValue.IndexOf('.');

						if (_decimalPlace == -1)
						{
							// No decimal at all
							_formattedValue = String.Concat(new object[] { _formattedValue, ".00" });
						}
						else if (_decimalPlace == _formattedValue.Length - 1)
						{
							// Int
							_formattedValue = String.Concat(new object[] { _formattedValue, "00" });
						}
						else if (_decimalPlace == _formattedValue.Length - 2)
						{
							// Single decimal
							_formattedValue = String.Concat(new object[] { _formattedValue, "0" });
						}
						else
						{
							// Two decimal or more
						}
					}

					//_formattedValue = (_lastDoubleValue).ToString("N7").Trim(new char[] { '0' });
					//_decimalPlace = _formattedValue.IndexOf('.');

					//if (_decimalPlace == -1)
					//{
					//    // No decimal at all
					//    _formattedValue = String.Concat(new object[] { _formattedValue, ".00" });
					//}
					//else if (_decimalPlace == _formattedValue.Length - 1)
					//{
					//    // Int
					//    _formattedValue = String.Concat(new object[] { _formattedValue, "00" });
					//}
					//else if (_decimalPlace == _formattedValue.Length - 2)
					//{
					//    // Single decimal
					//    _formattedValue = String.Concat(new object[] { _formattedValue, "0" });
					//}
					//else if (_decimalPlace == _formattedValue.Length - 3)
					//{
					//    // Two decimal
					//}
					//else if (_decimalPlace == _formattedValue.Length - 4)
					//{
					//    // Three decimal
					//}
					//else if (_decimalPlace < _formattedValue.Length - 4)
					//{
					//    // More then three decimals
					//    if (!isInEditMode && owningColumn.IsFraction)
					//    {
					//        // Convert to fraction display
					//        _formattedValue = fractionFixed.ToString(_lastDoubleValue);
					//    }
					//}
			        break;
			    case "Int":
			    case "Int64":
			    case "Long":
			        _formattedValue = (_lastLongValue).ToString("N0");
			        break;
			    case "String":
			    default:
			        _formattedValue = "";
			        break;
			}
		}

		private void SetBorderColor(Graphics graphics, Rectangle cellBounds, DataGridViewPaintParts paintParts)
		{
			if (!isInEditMode && (paintParts & DataGridViewPaintParts.ContentForeground) == DataGridViewPaintParts.ContentForeground)
			{
				GetCellRects(cellBounds);

				if (_inNotifyMode)
				{
					if (_notifyDirection > 0)
					{
						graphics.DrawRectangle(owningColumn.NotifyPositivePen, _customCellBound);
					}
					else if (_notifyDirection < 0)
					{
						graphics.DrawRectangle(owningColumn.NotifyNegativePen, _customCellBound);
					}
				}
			}
		}

		private void GetCellRects(Rectangle cellBounds)
		{
			if (_lastCellBound != cellBounds)
			{
				_lastCellBound = cellBounds;

				_customCellBound = new Rectangle(_lastCellBound.X + 1, _lastCellBound.Y + 1, _lastCellBound.Width - 4, _lastCellBound.Height - 4);
			}
		}
	}
}
