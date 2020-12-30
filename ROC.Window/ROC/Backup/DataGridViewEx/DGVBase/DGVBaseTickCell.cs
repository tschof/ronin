using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Drawing;
using DataGridViewEx.Properties;
using System.Diagnostics;

namespace DataGridViewEx
{
	public class DGVBaseTickCell : DGVBaseCellNoPaint
	{
		private string _lastTickValue = "";

		private bool isInEditMode
		{
			get
			{
				try
				{
					if (this.RowIndex >= 0 &&
						this.IsInEditMode)
					{
						return true;
					}
				}
				catch (Exception ex)
				{
					if (Debugger.IsAttached)
					{
						Debug.Print(ex.Message);
						Debug.Print(ex.StackTrace);
					}
				}

				return false;
			}
		}

		private Color _currentForeColor = Color.White;
		public Color CurrentForeColor
		{
			get
			{
				return _currentForeColor;
			}
		}

		private Color _currentBackColor = Color.Black;
		public Color CurrentBackColor
		{
			get
			{
				return _currentBackColor;
			}
		}

		private System.Windows.Forms.Timer _notifyTimer;

		#region - Notify Timer -

		private void StartNotify()
		{
			if (DataGridView != null && owningColumn.IsNotified && this.RowIndex >= 0)
			{
				if (_notifyTimer != null)
				{
					// reset
					StopNotify();
				}

				if (_notifyTimer == null)
				{
					_notifyTimer = new System.Windows.Forms.Timer();
					_notifyTimer.Interval = owningColumn.NotifyDuration;
					_notifyTimer.Tick += new EventHandler(NotifyTimer_Tick);
					_notifyTimer.Enabled = true;
				}
			}
		}

		private void StopNotify()
		{
			if (_notifyTimer != null)
			{
				_notifyTimer.Stop();
				_notifyTimer.Tick -= new EventHandler(NotifyTimer_Tick);
				_notifyTimer.Dispose();
			}
			_notifyTimer = null;
		}

		private void NotifyTimer_Tick(object sender, EventArgs e)
		{
			StopNotify();

			this.Value = _lastTickValue;
		}

		#endregion

		protected override void Dispose(bool disposing)
		{
			if (disposing)
			{
				StopNotify();
			}
			base.Dispose(disposing);
		}

		private DGVBaseTickColumn owningColumn
		{
			get
			{
				return (DGVBaseTickColumn)OwningColumn;
			}
		}

		protected override void Paint(Graphics graphics, Rectangle clipBounds, Rectangle cellBounds, int rowIndex, DataGridViewElementStates cellState, object value, object formattedValue, string errorText, DataGridViewCellStyle cellStyle, DataGridViewAdvancedBorderStyle advancedBorderStyle, DataGridViewPaintParts paintParts)
		{
			try
			{
				bool selected = false;
				bool changed = false;

				if (!isInEditMode)
				{
					formattedValue = "";
				}

				if (!isInEditMode && (paintParts & DataGridViewPaintParts.ContentForeground) == DataGridViewPaintParts.ContentForeground)
				{
					if ((cellState & DataGridViewElementStates.Selected) == DataGridViewElementStates.Selected)
					{
						selected = true;
					}
					else
					{
						selected = false;
					}

					string tick = GetTick(value);
					if (owningColumn.IsNotified)
					{
						switch (owningColumn.NotifyType)
						{
							case NotifyTypes.Border:
								if (tick != "")
								{
									changed = true;
								}
								break;
							case NotifyTypes.BackGround:
								if (selected)
								{
									cellStyle.SelectionBackColor = SetCellBackColor(selected, tick);
								}
								else
								{
									cellStyle.BackColor = SetCellBackColor(selected, tick);
								}
								break;
							case NotifyTypes.ForeGround:
								if (selected)
								{
									cellStyle.SelectionForeColor = SetCellForeColor(selected, tick);
								}
								else
								{
									cellStyle.ForeColor = SetCellForeColor(selected, tick);
								}
								break;
							default:
								break;
						}
					}

					base.Paint(graphics, clipBounds, cellBounds, rowIndex, cellState, value, formattedValue, errorText, cellStyle, advancedBorderStyle, paintParts);

					if (tick == "")
					{
						tick = _lastTickValue;
					}
					SetCellImage(tick, cellBounds, graphics, owningColumn.NotifyType == NotifyTypes.Border && changed);
				}
			}
			catch (Exception ex)
			{
				if (Debugger.IsAttached)
				{
					Debug.Print(ex.Message);
					Debug.Print(ex.StackTrace);
				}
			}
		}

		#region - Border -

		private string GetTick(object value)
		{
			string tick = "";

			if (value != null && value is string)
			{
				if (_lastTickValue != (string)value && value.ToString() != "")
				{
					if (((string)value).Contains("-"))
					{
						tick = "-";
						_lastTickValue = tick;
					}
					else if (((string)value).Contains("+"))
					{
						tick = "+";
						_lastTickValue = tick;
					}

					StartNotify();
				}
			}

			return tick;
		}

		private Color SetCellForeColor(bool selected, string tick)
		{
			switch (tick)
			{
				case "+":
					_currentForeColor = owningColumn.UpColor;
					break;
				case "-":
					_currentForeColor = owningColumn.DownColor;
					break;
				default:
					if (selected)
					{
						_currentForeColor = owningColumn.DefaultCellStyle.SelectionForeColor;
					}
					else
					{
						_currentForeColor = owningColumn.DefaultCellStyle.ForeColor;
					}
					break;
			}

			return _currentForeColor;
		}

		private Color SetCellBackColor(bool selected, string tick)
		{
			switch (tick)
			{
				case "+":
					_currentBackColor = owningColumn.UpColor;
					break;
				case "-":
					_currentBackColor = owningColumn.DownColor;
					break;
				default:
					if (selected)
					{
						_currentBackColor = owningColumn.DefaultCellStyle.SelectionBackColor;
					}
					else
					{
						_currentBackColor = owningColumn.DefaultCellStyle.BackColor;
					}
					break;
			}

			return _currentBackColor;
		}

		private void SetCellImage(string tick, Rectangle cellBounds, Graphics graphics, bool withBorder)
		{
			if (!isInEditMode && tick != "")
			{
				Rectangle cellRect = new Rectangle(cellBounds.X + 1, cellBounds.Y + 1, cellBounds.Width - 4, cellBounds.Height - 4);

				// Center Point of the cell
				Point imageBorderPoint = new Point(cellRect.X + cellRect.Width / 2, cellRect.Y + cellRect.Height / 2);
				// Set image size by cell height
				Size imageBorderSize = new Size(cellRect.Height, cellRect.Height);
				// Reposition the image in the center of the cell
				imageBorderPoint.X = imageBorderPoint.X - (imageBorderSize.Width / 2);
				imageBorderPoint.Y = imageBorderPoint.Y - (imageBorderSize.Height / 2);

				// Recreate the Image Rect
				Rectangle imageRect = new Rectangle(imageBorderPoint, imageBorderSize);

				switch (tick)
				{
					case "+":
						if (withBorder)
						{
							graphics.DrawRectangle(new Pen(owningColumn.UpColor), cellRect);
						}
						graphics.DrawImage(owningColumn.UpImage, imageRect);
						break;
					case "-":
						if (withBorder)
						{
							graphics.DrawRectangle(new Pen(owningColumn.DownColor), cellRect);
						}
						graphics.DrawImage(owningColumn.DownImage, imageRect);
						break;
					default:
						break;
				}
			}
		}

		#endregion

		public override string GetDisplayValue(object value)
		{
			if (value != null)
			{
				return value.ToString();
			}

			return "";
		}
	}
}
