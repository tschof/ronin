using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Drawing;
using DataGridViewEx.Properties;

namespace DataGridViewEx
{
	public class DGVBaseCheckCell : DGVBaseCellNoPaint
	{
		private string _formatedValue = "";

		// Last Cell Bound;
		private Rectangle _cellBound;

		// Calculated Item Rect
		private Rectangle _cellRect;

		// The Tick Image
		private Image _image = Resources.Question;
		private Rectangle _imageRect;
		private Point _imageBorderPoint;
		private Size _imageBorderSize;

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

		#region - Owning Grid -

		private ROCWatchList _owningGrid;
		private ROCWatchList owningGrid
		{
			get
			{
				setOwningGrid();
				return _owningGrid;
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

		private void owningGrid_RowsRemoved(object sender, DataGridViewRowsRemovedEventArgs e)
		{
		}

		#endregion

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
					SetCellBorder(cellBounds);

					graphics.DrawImage(_image, _imageRect);
				}
			}
		}

		public override string GetDisplayValue(object value)
		{
			_image = Resources.Question;
			_formatedValue = "";

			if (value != null && value is long)
			{
				if ((int)value == 0)
				{
					_formatedValue = "0";
					_image = Resources.Failed;
				}
				else if ((int)value == 1)
				{
					_formatedValue = "1";
					_image = Resources.Checked;
				}
				else
				{
					_formatedValue = value.ToString();
					_image = Resources.Waiting;
				}
			}

			return _formatedValue;
		}

		private void SetCellBorder(Rectangle cellBounds)
		{
			if (_cellBound != cellBounds)
			{
				_cellBound = cellBounds;

				// Recreate the Cell Rect
				_cellRect = new Rectangle(_cellBound.X + 1, _cellBound.Y + 1, _cellBound.Width - 4, _cellBound.Height - 4);

				// Center Point of the cell
				_imageBorderPoint = new Point(_cellRect.X + _cellRect.Width / 2, _cellRect.Y + _cellRect.Height / 2);
				// Set image size by cell height
				_imageBorderSize = new Size(_cellRect.Height, _cellRect.Height);
				// Reposition the image in the center of the cell
				_imageBorderPoint.X = _imageBorderPoint.X - (_imageBorderSize.Width / 2);
				_imageBorderPoint.Y = _imageBorderPoint.Y - (_imageBorderSize.Height / 2);

				// Recreate the Image Rect
				_imageRect = new Rectangle(_imageBorderPoint, _imageBorderSize);
			}
		}
	}
}
