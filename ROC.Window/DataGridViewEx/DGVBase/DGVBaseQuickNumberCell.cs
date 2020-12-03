using System;
using System.Drawing;
using System.Diagnostics;
using System.Drawing.Drawing2D;
using System.Windows.Forms;

namespace DataGridViewEx
{
	public class DGVBaseQuickNumberCell : DGVBaseNumberCell
	{
		private DGVBaseQuickNumberColumn owningColumn
		{
			get
			{
				return (DGVBaseQuickNumberColumn)OwningColumn;
			}
		}

		protected override void Paint(Graphics graphics, Rectangle clipBounds, Rectangle cellBounds, int rowIndex, DataGridViewElementStates cellState, object value, object formattedValue, string errorText, DataGridViewCellStyle cellStyle, DataGridViewAdvancedBorderStyle advancedBorderStyle, DataGridViewPaintParts paintParts)
		{
			try
			{
				bool done = false;
				bool selected = false;

				if ((cellState & DataGridViewElementStates.Visible) == DataGridViewElementStates.Visible && (paintParts & DataGridViewPaintParts.ContentForeground) == DataGridViewPaintParts.ContentForeground)
				{
					if ((cellState & DataGridViewElementStates.Selected) == DataGridViewElementStates.Selected)
					{
						selected = true;
					}
					else
					{
						selected = false;
					}

					if (!selected)
					{
						if (owningColumn.ShowBar)
						{
							SetBarBorder(cellBounds, cellStyle, graphics, paintParts);
							done = true;
						}
						else if (owningColumn.IsTradePrice)
						{
							SetTradePriceBorder(cellBounds, cellStyle, graphics);
							done = true;
						}
						else if (owningColumn.FlagStopOpen)
						{
							done = SetStopPriceBorder(cellBounds, cellStyle, graphics);
						}

						if (done)
						{
							paintParts = DataGridViewPaintParts.ContentForeground;
							base.Paint(graphics, clipBounds, cellBounds, rowIndex, cellState, value, formattedValue, errorText, cellStyle, advancedBorderStyle, paintParts);

							paintParts = DataGridViewPaintParts.Border;
							base.Paint(graphics, clipBounds, cellBounds, rowIndex, cellState, value, formattedValue, errorText, cellStyle, advancedBorderStyle, paintParts);
						}
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
							barBrush = new LinearGradientBrush(cellBounds, Color.FromArgb(100, cellStyle.ForeColor), Color.FromArgb(50, cellStyle.ForeColor), LinearGradientMode.Horizontal);
							break;
						default:
							cellBarRect = new Rectangle(cellBounds.X + 1, cellBounds.Y + 1, cellBarWidth, cellBounds.Height - 4);
							barBrush = new LinearGradientBrush(cellBounds, Color.FromArgb(50, cellStyle.ForeColor), Color.FromArgb(100, cellStyle.ForeColor), LinearGradientMode.Horizontal);
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

		#region - Flag -

		private PathGradientBrush BidFlag(Rectangle cellBounds)
		{
			int x = cellBounds.X + 1;
			int y = cellBounds.Y + 1;
			int h = cellBounds.Height - 3;
			int w = h / 2;

			Point[] array = new Point[] { 
				 new Point(x, y),
				 new Point(x + w, y + w),
				 new Point(x, y + h) };
			
			return new PathGradientBrush(array, WrapMode.Clamp);
		}

		private PathGradientBrush AskFlag(Rectangle cellBounds)
		{
			int x = cellBounds.X + cellBounds.Width - 1;
			int y = cellBounds.Y + 1;
			int h = cellBounds.Height - 3;
			int w = h / 2;

			Point[] array = new Point[] { 
				 new Point(x, y),
				 new Point(x - w, y + w),
				 new Point(x, y + h) };

			return new PathGradientBrush(array, WrapMode.Clamp);
		}

		#endregion

		#region - Image -

		private void BuyImage(Rectangle cellBounds, Graphics graphics)
		{
			Rectangle cellRect = new Rectangle(cellBounds.X + 1, cellBounds.Y + 1, cellBounds.Width - 4, cellBounds.Height - 4);

			// Set image size by cell height
			Size imageBorderSize = new Size(cellRect.Height - 4, cellRect.Height - 4);
			// Reposition the image in the center of the cell
			Point imageBorderPoint = new Point();
			imageBorderPoint.X = cellRect.X + (cellRect.Width - imageBorderSize.Height);
			imageBorderPoint.Y = cellRect.Y + ((cellRect.Height - imageBorderSize.Height) / 2) + 1;

			// Recreate the Image Rect
			Rectangle imageRect = new Rectangle(imageBorderPoint, imageBorderSize);
			graphics.DrawImage(owningColumn.StopImage, imageRect);
		}

		private void SellImage(Rectangle cellBounds, Graphics graphics)
		{
			Rectangle cellRect = new Rectangle(cellBounds.X + 1, cellBounds.Y + 1, cellBounds.Width - 4, cellBounds.Height - 4);

			// Set image size by cell height
			Size imageBorderSize = new Size(cellRect.Height - 4, cellRect.Height - 4);
			// Reposition the image in the center of the cell
			Point imageBorderPoint = new Point();
			imageBorderPoint.X = cellRect.X + 4;
			imageBorderPoint.Y = cellRect.Y + ((cellRect.Height - imageBorderSize.Height) / 2) + 1;

			// Recreate the Image Rect
			Rectangle imageRect = new Rectangle(imageBorderPoint, imageBorderSize);
			graphics.DrawImage(owningColumn.StopImage, imageRect);
		}

		#endregion

		#region - Price Flags -

		private void SetTradePriceBorder(Rectangle cellBounds, DataGridViewCellStyle cellStyle, Graphics graphics)
		{
			try
			{
				Brush tradeBrush;
				Rectangle bound;
				int barWidth = 3;

				if (RowIndex >= 0 && OwningRow != null)
				{
					// Paint the cell background first (Traded Price)
					if (hasTradedFlag)
					{
						switch (getFlagValue("PriceFlag"))
						{
							case PriceFlages.tradeUp:
							case PriceFlages.tradeDown:
								tradeBrush = new SolidBrush(Color.FromArgb(150, cellStyle.BackColor));
								graphics.FillRectangle(tradeBrush, cellBounds);
								break;
							default:
								tradeBrush = new SolidBrush(cellStyle.BackColor);
								graphics.FillRectangle(tradeBrush, cellBounds);
								break;
						}
					}
					else
					{
						tradeBrush = new SolidBrush(Color.WhiteSmoke);
						graphics.FillRectangle(tradeBrush, cellBounds);
					}

					// Add the high layer
					if (hasHighFlag)
					{
						bound = new Rectangle(cellBounds.X + barWidth + 1, cellBounds.Y + 1, cellBounds.Width - (barWidth * 2) - 1, barWidth);
						tradeBrush = new LinearGradientBrush(bound, Color.LimeGreen, Color.WhiteSmoke, LinearGradientMode.Vertical);
						graphics.FillRectangle(tradeBrush, bound);
					}

					// Add the low layer
					if (hasLowFlag)
					{
						bound = new Rectangle(cellBounds.X + barWidth + 1, cellBounds.Bottom - barWidth - 1, cellBounds.Width - (barWidth * 2) - 1, barWidth);
						tradeBrush = new LinearGradientBrush(bound, Color.WhiteSmoke, Color.Red, LinearGradientMode.Vertical);
						graphics.FillRectangle(tradeBrush, bound);
					}

					// Add the bid layer
					if (hasBidFlag)
					{
						tradeBrush = BidFlag(cellBounds);
						((PathGradientBrush)tradeBrush).SurroundColors = new Color[] { Color.LimeGreen };
						((PathGradientBrush)tradeBrush).CenterColor = Color.WhiteSmoke;
						graphics.FillRectangle(tradeBrush, cellBounds);
					}

					// Add the ask layer
					if (hasAskFlag)
					{
						tradeBrush = AskFlag(cellBounds);
						((PathGradientBrush)tradeBrush).SurroundColors = new Color[] { Color.Red };
						((PathGradientBrush)tradeBrush).CenterColor = Color.WhiteSmoke;
						graphics.FillRectangle(tradeBrush, cellBounds);
					}
				}
				else
				{
					tradeBrush = new SolidBrush(Color.WhiteSmoke);
					graphics.FillRectangle(tradeBrush, cellBounds);
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

		private long getFlagValue(string colName)
		{
			return (long)OwningRow.Cells[colName].Value;
		}

		private bool hasTradedFlag
		{
			get
			{
				if (OwningRow.Index >= 0)
				{
					if (OwningRow.Cells["PriceFlag"].Value != null && OwningRow.Cells["PriceFlag"].Value.ToString() != "")
					{
						return true;
					}
				}
				return false;
			}
		}

		private bool hasHighFlag
		{
			get
			{
				if (OwningRow.Index >= 0)
				{
					if (OwningRow.Cells["PriceHighFlag"].Value != null && OwningRow.Cells["PriceHighFlag"].Value.ToString() != "")
					{
						return true;
					}
				}
				return false;
			}
		}

		private bool hasLowFlag
		{
			get
			{
				if (OwningRow.Index >= 0)
				{
					if (OwningRow.Cells["PriceLowFlag"].Value != null && OwningRow.Cells["PriceLowFlag"].Value.ToString() != "")
					{
						return true;
					}
				}
				return false;
			}
		}

		private bool hasBidFlag
		{
			get
			{
				if (OwningRow.Index >= 0)
				{
					if (OwningRow.Cells["PriceBidFlag"].Value != null && OwningRow.Cells["PriceBidFlag"].Value.ToString() != "")
					{
						return true;
					}
				}
				return false;
			}
		}

		private bool hasAskFlag
		{
			get
			{
				if (OwningRow.Index >= 0)
				{
					if (OwningRow.Cells["PriceAskFlag"].Value != null && OwningRow.Cells["PriceAskFlag"].Value.ToString() != "")
					{
						return true;
					}
				}
				return false;
			}
		}

		#endregion

		#region - Stop Image -

		private bool SetStopPriceBorder(Rectangle cellBounds, DataGridViewCellStyle cellStyle, Graphics graphics)
		{
			//Brush stopBrush;

			if (hasBuyStopFlag && IsBuyOpenColumn)
			{
				//stopBrush = new SolidBrush(Color.FromArgb(100, Color.Pink));
				//graphics.FillRectangle(stopBrush, cellBounds);
				//return true;
				cellStyle.ForeColor = owningColumn.StopForeColor;
				return false;
			}

			if (hasSellStopFlag && IsSellOpenColumn)
			{
				//stopBrush = new SolidBrush(Color.FromArgb(100, Color.Pink));
				//graphics.FillRectangle(stopBrush, cellBounds);
				//return true;
				cellStyle.ForeColor = owningColumn.StopForeColor;
				return false;
			}

			return false;
		}

		private bool IsBuyOpenColumn
		{
			get
			{
				if (OwningColumn.Name == "BuyOpenQty")
				{
					return true;
				}
				return false;
			}
		}

		private bool hasBuyStopFlag
		{
			get
			{
				if (OwningRow.Index >= 0)
				{
					if (OwningRow.Cells["BuyStopFlag"].Value != null && OwningRow.Cells["BuyStopFlag"].Value.ToString() != "")
					{
						return true;
					}
				}
				return false;
			}
		}

		private bool IsSellOpenColumn
		{
			get
			{
				if (OwningColumn.Name == "SellOpenQty")
				{
					return true;
				}
				return false;
			}
		}

		private bool hasSellStopFlag
		{
			get
			{
				if (OwningRow.Index >= 0)
				{
					if (OwningRow.Cells["SellStopFlag"].Value != null && OwningRow.Cells["SellStopFlag"].Value.ToString() != "")
					{
						return true;
					}
				}
				return false;
			}
		}

		#endregion
	}
}
