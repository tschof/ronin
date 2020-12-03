using System;
using System.Windows.Forms;
using System.Drawing;
using System.Diagnostics;
using System.Drawing.Drawing2D;

namespace DataGridViewEx
{
	public class DGVBaseWatchNumberCell : DGVBaseNumberCell
	{
		private Color _currentForeColor = Color.White;
		public Color CurrentForeColor
		{
			get
			{
				return _currentForeColor;
			}
		}

		private DGVBaseWatchNumberColumn owningColumn
		{
			get
			{
				return (DGVBaseWatchNumberColumn)OwningColumn;
			}
		}

		protected override void Paint(Graphics graphics, Rectangle clipBounds, Rectangle cellBounds, int rowIndex, DataGridViewElementStates cellState, object value, object formattedValue, string errorText, DataGridViewCellStyle cellStyle, DataGridViewAdvancedBorderStyle advancedBorderStyle, DataGridViewPaintParts paintParts)
		{
			try
			{
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

					if (owningColumn.IsUnderLine && cellStyle.Font.Style != FontStyle.Underline)
					{
						cellStyle.Font = new Font(DataGridView.Font, FontStyle.Underline);
					}

					if (owningColumn.AutoColor)
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
		
					base.Paint(graphics, clipBounds, cellBounds, rowIndex, cellState, value, formattedValue, errorText, cellStyle, advancedBorderStyle, paintParts);
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
					//if (_selected)
					//{
					//    _currentForeColor = owningColumn.DefaultCellStyle.SelectionForeColor;
					//}
					//else
					//{
					//    _currentForeColor = owningColumn.DefaultCellStyle.ForeColor;
					//}	
					_currentForeColor = owningColumn.DefaultColor;
					break;
			}

			return _currentForeColor;
		}

		#endregion
	}
}
