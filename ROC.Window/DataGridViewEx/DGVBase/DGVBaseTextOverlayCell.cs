using System;
using System.Drawing;
using System.Diagnostics;
using System.Drawing.Drawing2D;
using System.Windows.Forms;

namespace DataGridViewEx
{
	public class DGVBaseTextOverlayCell : DGVBaseCellNoPaint
	{
		private DGVBaseTextOverlayColumn owningColumn
		{
			get
			{
				return (DGVBaseTextOverlayColumn)OwningColumn;
			}
		}

		private string SecurityStatus
		{
			get
			{
				try
				{
					if (RowIndex >= 0 && RowIndex <= DataGridView.Rows.Count - 1 && 
						DataGridView.Columns.Contains("SecurityStatus") && 
						OwningRow.Cells["SecurityStatus"].Value != null && 
						OwningRow.Cells["SecurityStatus"].Value.ToString() != "")
					{
						return OwningRow.Cells["SecurityStatus"].Value.ToString();
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
				return "";
			}
		}

		private bool _flaged = false;
		public bool Flaged
		{
			get
			{
				return _flaged;
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

					if (SecurityStatus != "")
					{
						SetUnderlyImage(cellBounds, cellStyle, graphics, selected);
						done = true;
						_flaged = true;
					}
					else
					{
						_flaged = false;
					}

					if (done)
					{
						paintParts = DataGridViewPaintParts.ContentForeground;
						base.Paint(graphics, clipBounds, cellBounds, rowIndex, cellState, value, formattedValue, errorText, cellStyle, advancedBorderStyle, paintParts);

						paintParts = DataGridViewPaintParts.Border;
						base.Paint(graphics, clipBounds, cellBounds, rowIndex, cellState, value, formattedValue, errorText, cellStyle, advancedBorderStyle, paintParts);
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

		public override string GetDisplayValue(object value)
		{
			if (value != null && value.ToString() != "")
			{
				if (owningColumn.ShowLength == 0)
				{
					return value.ToString().ToUpper();
				}
				else
				{
					return value.ToString().Substring(0, owningColumn.ShowLength).ToUpper();
				}
			}

			return "";
		}

		#region - Underly Image -

		private void SetUnderlyImage(Rectangle cellBounds, DataGridViewCellStyle cellStyle, Graphics graphics, bool selected)
		{
			try
			{
				if (selected)
				{
					graphics.FillRectangle(new SolidBrush(cellStyle.SelectionBackColor), cellBounds);
				}
				else
				{
					graphics.FillRectangle(new SolidBrush(cellStyle.BackColor), cellBounds);
				}

				Rectangle cellRect = new Rectangle(cellBounds.X + 1, cellBounds.Y + 1, cellBounds.Width - 4, cellBounds.Height - 4);

				// Set image size by cell height
				Size imageBorderSize = new Size(cellRect.Height - 4, cellRect.Height - 4);
				// Reposition the image in the center of the cell
				Point imageBorderPoint = new Point();
				imageBorderPoint.X = cellRect.X + (cellRect.Width - imageBorderSize.Height);
				imageBorderPoint.Y = cellRect.Y + ((cellRect.Height - imageBorderSize.Height) / 2) + 1;

				// Recreate the Image Rect
				Rectangle imageRect = new Rectangle(imageBorderPoint, imageBorderSize);
				graphics.DrawImage(owningColumn.NotifyImageOpac, imageRect);
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
