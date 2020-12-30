using System;
using System.Windows.Forms;
using System.Drawing;
using System.Diagnostics;
using System.Drawing.Drawing2D;

namespace DataGridViewEx
{
	public class DGVBaseNotifiedNumberCell : DGVBaseNumberCell
	{
		private double _lastDoubleValue = 0;
		private long _lastLongValue = 0;

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
			if (DataGridView != null && owningColumn.IsNotified && RowIndex >= 0)
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

			if (this.Value is double)
			{
				if ((double)this.Value == _lastDoubleValue)
				{
					this.Value = _lastDoubleValue;
				}
			}
			else if (this.Value is long)
			{
				if ((long)this.Value == _lastLongValue)
				{
					this.Value = _lastLongValue;
				}
			}
		}

		#endregion

		protected override void Dispose(bool disposing)
		{
			if (disposing)
			{
				_lastDoubleValue = 0;
				_lastLongValue = 0;
				StopNotify();
			}
			base.Dispose(disposing);
		}

		private DGVBaseNotifiedNumberColumn owningColumn
		{
			get
			{
				return (DGVBaseNotifiedNumberColumn)OwningColumn;
			}
		}

		protected override void Paint(Graphics graphics, Rectangle clipBounds, Rectangle cellBounds, int rowIndex, DataGridViewElementStates cellState, object value, object formattedValue, string errorText, DataGridViewCellStyle cellStyle, DataGridViewAdvancedBorderStyle advancedBorderStyle, DataGridViewPaintParts paintParts)
		{
			try
			{
				bool done = false;
				bool selected = false;

				if (!isInEditMode && (cellState & DataGridViewElementStates.Visible) == DataGridViewElementStates.Visible)
				{
					if ((cellState & DataGridViewElementStates.Selected) == DataGridViewElementStates.Selected)
					{
						selected = true;
					}
					else
					{
						selected = false;
					}

					if (owningColumn.IsUnderLine && cellStyle.Font.Style != FontStyle.Underline)
					{
						cellStyle.Font = new Font(DataGridView.Font, FontStyle.Underline);
					}

					if (owningColumn.IsNotified)
					{
						string tick = GetTick(value);
						switch (owningColumn.NotifyType)
						{
							case NotifyTypes.Border:
								base.Paint(graphics, clipBounds, cellBounds, rowIndex, cellState, value, formattedValue, errorText, cellStyle, advancedBorderStyle, paintParts);
								SetCellBorder(tick, cellBounds, graphics);
								done = true;
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
					else if (owningColumn.AutoColor)
					{
						if (selected)
						{
							cellStyle.SelectionForeColor = SetCellForeColor(GetPandNState(value));
						}
						else
						{
							cellStyle.ForeColor = SetCellForeColor(GetPandNState(value));
						}
					}
					else if (owningColumn.ShowBar)
					{
						SetBarBorder(cellBounds, cellStyle, graphics, paintParts);

						paintParts = DataGridViewPaintParts.ContentForeground;
						base.Paint(graphics, clipBounds, cellBounds, rowIndex, cellState, value, formattedValue, errorText, cellStyle, advancedBorderStyle, paintParts);
						done = true;
					}
					else if (owningColumn.IsTradePrice)
					{
						SetTradePriceBorder(cellBounds, cellStyle, graphics);

						paintParts = DataGridViewPaintParts.ContentForeground;
						base.Paint(graphics, clipBounds, cellBounds, rowIndex, cellState, value, formattedValue, errorText, cellStyle, advancedBorderStyle, paintParts);

						paintParts = DataGridViewPaintParts.Border;
						base.Paint(graphics, clipBounds, cellBounds, rowIndex, cellState, value, formattedValue, errorText, cellStyle, advancedBorderStyle, paintParts);
						done = true;
					}

					if (!done)
					{
						base.Paint(graphics, clipBounds, cellBounds, rowIndex, cellState, value, formattedValue, errorText, cellStyle, advancedBorderStyle, paintParts);
					}
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

		#region - Auto Color -

		private int GetPandNState(object value)
		{
			try
			{
				if (value != null)
				{
					switch (ValueType.Name)
					{
						case "Decimal":
						case "Double":
							if (value is double)
							{
								if ((double)value > 0)
								{
									return 1;
								}
								else if ((double)value < 0)
								{
									return -1;
								}
							}
							break;
						case "Int":
						case "Int64":
						case "Long":
							if (value is long)
							{
								if ((long)value > 0)
								{
									return 1;
								}
								else if ((long)value < 0)
								{
									return -1;
								}
							}
							break;
						case "String":
						default:
							break;
					}
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

			return 0;
		}

		private Color SetCellForeColor(int colorFlag)
		{
			switch (colorFlag)
			{
				case 1:
					_currentForeColor = owningColumn.UpColor;
					break;
				case -1:
					_currentForeColor = owningColumn.DownColor;
					break;
				default:	
					_currentForeColor = owningColumn.DefaultColor;
					break;
			}

			return _currentForeColor;
		}

		#endregion

		#region - Border -

		private string GetTick(object value)
		{
			string tick = "";

			if (value != null)
			{
				if (value is long)
				{
					if (_lastLongValue != (long)value)
					{
						if (_lastLongValue > (long)value)
						{
							tick = "-";
						}
						else if (_lastLongValue < (long)value)
						{
							tick = "+";
						}

						_lastLongValue = (long)value;
						StartNotify();
					}
				}
				else if (value is double)
				{
					if (_lastDoubleValue != (double)value)
					{
						if (_lastDoubleValue > (double)value)
						{
							tick = "-";
						}
						else if (_lastDoubleValue < (double)value)
						{
							tick = "+";
						}

						_lastDoubleValue = (double)value;
						StartNotify();
					}
				}
			}

			return tick;
		}

		private void SetCellBorder(string tick, Rectangle cellBounds, Graphics graphics)
		{
			if (!isInEditMode)
			{
				Rectangle cellRect = new Rectangle(cellBounds.X + 1, cellBounds.Y + 1, cellBounds.Width - 4, cellBounds.Height - 4);

				switch (owningColumn.NotifyType)
				{
					case NotifyTypes.Border:
						switch (tick)
						{
							case "+":
								graphics.DrawRectangle(new Pen(owningColumn.UpColor), cellRect);
								break;
							case "-":
								graphics.DrawRectangle(new Pen(owningColumn.DownColor), cellRect);
								break;
							default:
								break;
						}
						break;
					default:
						break;
				}
			}
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

		#endregion

		#region - Bar -

		private double GetBarValue()
		{
			try
			{
				switch (owningColumn.Name)
				{
					case "BidSize":
						if (OwningRow.Cells["BidSize"].Value != null && OwningRow.Cells["BidSize"].Value.ToString() != "")
						{
							return Convert.ToDouble(OwningRow.Cells["BidSize"].Value);
						}
						break;
					case "AskSize":
						if (OwningRow.Cells["AskSize"].Value != null && OwningRow.Cells["AskSize"].Value.ToString() != "")
						{
							return Convert.ToDouble(OwningRow.Cells["AskSize"].Value);
						}
						break;
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

			return 0;
		}

		private void SetBarBorder(Rectangle cellBounds, DataGridViewCellStyle cellStyle, Graphics graphics, DataGridViewPaintParts paintParts)
		{
			try
			{
				int cellBarWidth = 0;
				Rectangle cellBarRect;
				Brush barBrush;

				if (RowIndex >= 0 && OwningRow != null && OwningRow.Cells[owningColumn.Index].Value != null && OwningRow.Cells[owningColumn.Index].Value.ToString() != "")
				{
					double lastDoubleValue = Convert.ToDouble(OwningRow.Cells[owningColumn.Index].Value);

					if (lastDoubleValue >= owningColumn.ShowBarMax)
					{
						cellBarWidth = cellBounds.Width - 4;
						//owningColumn.ShowBarMax = (long)OwningRow.Cells[owningColumn.Index].Value;
					}
					else
					{
						cellBarWidth = Convert.ToInt32(lastDoubleValue / owningColumn.ShowBarMax * (cellBounds.Width - 4));
					}

					switch (cellStyle.Alignment)
					{
						case DataGridViewContentAlignment.MiddleRight:
						case DataGridViewContentAlignment.BottomRight:
						case DataGridViewContentAlignment.TopRight:
							cellBarRect = new Rectangle(cellBounds.Right - cellBarWidth - 1, cellBounds.Y + 1, cellBarWidth, cellBounds.Height - 4);
							barBrush = new LinearGradientBrush(cellBounds, Color.FromArgb(150, cellStyle.ForeColor), Color.FromArgb(50, cellStyle.ForeColor), LinearGradientMode.Horizontal);
							break;
						default:
							cellBarRect = new Rectangle(cellBounds.X + 1, cellBounds.Y + 1, cellBarWidth, cellBounds.Height - 4);
							barBrush = new LinearGradientBrush(cellBounds, Color.FromArgb(50, cellStyle.ForeColor), Color.FromArgb(150, cellStyle.ForeColor), LinearGradientMode.Horizontal);
							break;
					}
				}
				else
				{
					barBrush = new SolidBrush(cellStyle.BackColor);
					cellBarRect = Rectangle.Empty;
				}

				if (cellBarRect != Rectangle.Empty && (paintParts & DataGridViewPaintParts.Background) == DataGridViewPaintParts.Background)
				{
					graphics.FillRectangle(barBrush, cellBarRect);
				}
				else
				{
					graphics.FillRectangle(barBrush, cellBounds);
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

		#endregion

		#region - Trade Price Flag -

		private void SetTradePriceBorder(Rectangle cellBounds, DataGridViewCellStyle cellStyle, Graphics graphics)
		{
			try
			{
				Brush tradeBrush;

				if (RowIndex >= 0 && OwningRow != null && OwningRow.Cells["PriceFlag"].Value != null && OwningRow.Cells["PriceFlag"].Value.ToString() != "")
				{
					switch ((long)OwningRow.Cells["PriceFlag"].Value)
					{
						case PriceFlages.tradeUp:
						case PriceFlages.tradeDown:
							tradeBrush = new SolidBrush(Color.FromArgb(150, cellStyle.BackColor));
							break;
						case PriceFlages.low:
							tradeBrush = new LinearGradientBrush(cellBounds, cellStyle.BackColor, Color.WhiteSmoke, LinearGradientMode.Vertical);
							break;
						case PriceFlages.high:
							tradeBrush = new LinearGradientBrush(cellBounds, Color.WhiteSmoke, cellStyle.BackColor, LinearGradientMode.Vertical);
							break;
						default:
							tradeBrush = new SolidBrush(cellStyle.BackColor);
							break;
					}
				}
				else
				{
					tradeBrush = new SolidBrush(Color.WhiteSmoke);
				}

				graphics.FillRectangle(tradeBrush, cellBounds);
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

		#endregion
	}
}
