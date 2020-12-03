using System;
using System.Windows.Forms;
using System.Drawing;
using System.ComponentModel;

namespace DataGridViewEx
{
	public class DGVAnimatedCell : DataGridViewTextBoxCell
	{
		private object _lastValue;
		private System.Timers.Timer _notifyTimer;

		private bool _inNotifyMode = false;
		private int _notifyDirection = 0;

		// Save the last cell size after repaint
		private Rectangle _lastCellBound;

		private Rectangle _customCellBound;
		private Rectangle _customImageBound;
		private Point _customImagePoint;

		// The Tick Image
		private Image _returnImage;
		// The Tick Image Size is based on cell height.
		private Size _imageSize = Size.Empty;

		private DGVTickColumn _owningColumn;
		private DGVTickColumn owningColumn
		{
			get
			{
				if (_owningColumn == null)
				{
					_owningColumn = (DGVTickColumn)OwningColumn;
					_owningColumn.Disposed += new EventHandler(owningColumn_Disposed);
				}
				return _owningColumn;
			}
		}

		private void owningColumn_Disposed(object sender, EventArgs e)
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
				return null;
			}
		}

		protected override object GetFormattedValue(object value, int rowIndex, ref DataGridViewCellStyle cellStyle, TypeConverter valueTypeConverter, TypeConverter formattedValueTypeConverter, DataGridViewDataErrorContexts context)
		{
			_notifyDirection = 0;

			if (value != null)
			{
				switch (ValueType.Name)
				{
					case "String":
						break;
					case "Decimal":
					case "Double":
					case "Int":
					case "Int64":
					case "Long":
					default:
						break;
				}

				if (_lastValue != value)
				{
					_lastValue = value;
					StartNotify();
				}
			}

			return _returnImage;

			//return base.GetFormattedValue(value, rowIndex, ref cellStyle, valueTypeConverter, formattedValueTypeConverter, context);
		}

		protected override void Paint(Graphics graphics, Rectangle clipBounds, Rectangle cellBounds, int rowIndex, DataGridViewElementStates cellState, object value, object formattedValue, string errorText, DataGridViewCellStyle cellStyle, DataGridViewAdvancedBorderStyle advancedBorderStyle, DataGridViewPaintParts paintParts)
		{
			base.Paint(graphics, clipBounds, cellBounds, rowIndex, cellState, value, formattedValue, errorText, cellStyle, advancedBorderStyle, paintParts);

			if (!IsInEditMode && (paintParts & DataGridViewPaintParts.ContentForeground) == DataGridViewPaintParts.ContentForeground)
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

				graphics.DrawImage(_returnImage, _customImageBound);
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
