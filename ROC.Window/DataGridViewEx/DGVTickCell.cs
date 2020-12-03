using System;
using System.Windows.Forms;
using System.Drawing;
using System.ComponentModel;

using DataGridViewEx.Properties;

namespace DataGridViewEx
{
	public class DGVTickCell : DataGridViewTextBoxCell
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

		private System.Timers.Timer _notifyTimer;

		private bool _inNotifyMode = false;
		private int _notifyDirection = 0;

		// Save the last cell size after repaint
		private Rectangle _lastCellBound;
		private Rectangle _customCellBound;

		private Rectangle _customImageBound;
		private Point _customImagePoint;

		// The Tick Image
		private Image _returnImage = Resources.Blank;
		// The Tick Image Size is based on cell height.
		private Size _imageSize = Size.Empty;

		private ROCWatchList _owningGrid;
		private ROCWatchList owningGrid
		{
			get
			{
				setOwningGrid();
				return _owningGrid;
			}
		}

		private DGVTickColumn _owningColumn;
		private DGVTickColumn owningColumn
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
				_owningColumn = (DGVTickColumn)OwningColumn;
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

			if (_inNotifyMode)
			{
				_inNotifyMode = false;
				DataGridView.InvalidateCell(this);
			}
		}

		public override object DefaultNewRowValue
		{
			get
			{
				_returnImage = Resources.Blank;
				return "";
			}
		}

		protected override object GetFormattedValue(object value, int rowIndex, ref DataGridViewCellStyle cellStyle, TypeConverter valueTypeConverter, TypeConverter formattedValueTypeConverter, DataGridViewDataErrorContexts context)
		{
			if (value != null)
			{
				switch (value.GetType().Name)
				{
					case "String":
						switch (value.ToString())
						{
							case "+0":
							case "+":
								_notifyDirection = 1;
								_returnImage = Resources.UpGreen;
								break;
							case "-0":
							case "-":
								_notifyDirection = -1;
								_returnImage = Resources.DownRed;
								break;
							default:
								_notifyDirection = 0;
								_returnImage = Resources.Blank;
								value = "";
								break;
						}
						break;
					case "Decimal":
					case "Double":
					case "Int":
					case "Int64":
					case "Long":
					default:
						_notifyDirection = 0;
						_returnImage = Resources.Blank;
						value = "";
						break;
				}

				if (_lastValue != value)
				{
					_lastValue = value;
					if (_notifyDirection != 0)
					{
						StartNotify();
					}
				}
			}
			else
			{
				_returnImage = Resources.Blank;
			}

			return base.GetFormattedValue(value, rowIndex, ref cellStyle, valueTypeConverter, formattedValueTypeConverter, context);
		}

		protected override void Paint(Graphics graphics, Rectangle clipBounds, Rectangle cellBounds, int rowIndex, DataGridViewElementStates cellState, object value, object formattedValue, string errorText, DataGridViewCellStyle cellStyle, DataGridViewAdvancedBorderStyle advancedBorderStyle, DataGridViewPaintParts paintParts)
		{
			if ((cellState & DataGridViewElementStates.Visible) == DataGridViewElementStates.Visible)
			{
				if (!isInEditMode)
				{
					formattedValue = "";
				}

				base.Paint(graphics, clipBounds, cellBounds, rowIndex, cellState, value, formattedValue, errorText, cellStyle, advancedBorderStyle, paintParts);

				if (!isInEditMode && (paintParts & DataGridViewPaintParts.ContentForeground) == DataGridViewPaintParts.ContentForeground)
				{
					GetCellRects(cellBounds);

					switch (owningColumn.NotifyType)
					{
						case NotifyTypes.Border:
							SetBorderColor(graphics, paintParts);
							break;
					}

					graphics.DrawImage(_returnImage, _customImageBound);
				}

				_notifyDirection = 0;
			}
		}

		private void SetBorderColor(Graphics graphics, DataGridViewPaintParts paintParts)
		{
			if (!isInEditMode && (paintParts & DataGridViewPaintParts.ContentForeground) == DataGridViewPaintParts.ContentForeground)
			{
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
				// Center Point of the cell
				_customImagePoint = new Point(_customCellBound.X + _customCellBound.Width / 2, _customCellBound.Y + _customCellBound.Height / 2);
				// Set image size by cell height
				_imageSize = new Size(_customCellBound.Height, _customCellBound.Height);
				
				_customImagePoint.X = _customImagePoint.X - (_imageSize.Width / 2);
				_customImagePoint.Y = _customImagePoint.Y - (_imageSize.Height / 2);

				// Recreate the Image Bound
				_customImageBound = new Rectangle(_customImagePoint, _imageSize);
			}
		}
	}
}
