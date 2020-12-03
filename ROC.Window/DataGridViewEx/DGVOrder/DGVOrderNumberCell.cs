using System;
using System.Windows.Forms;

namespace DataGridViewEx
{
	public class DGVOrderNumberCell : DataGridViewTextBoxCell
	{
		private int _decimalPlace = -1;
		private string _formattedValue = "";

		private double _tickSize = 0.01;
		public double TickSize
		{
			get
			{
				return _tickSize;
			}
			set
			{
				_tickSize = value;
			}
		}

		private double _displayFactor = 1;
		public double DisplayFactor
		{
			get
			{
				return _displayFactor;
			}
			set
			{
				_displayFactor = value;
			}
		}

		public override object DefaultNewRowValue
		{
			get
			{
				return null;
			}
		}

		protected override void Paint(System.Drawing.Graphics graphics, System.Drawing.Rectangle clipBounds, System.Drawing.Rectangle cellBounds, int rowIndex, DataGridViewElementStates cellState, object value, object formattedValue, string errorText, DataGridViewCellStyle cellStyle, DataGridViewAdvancedBorderStyle advancedBorderStyle, DataGridViewPaintParts paintParts)
		{
			if (value != null && (cellState & DataGridViewElementStates.Visible) == DataGridViewElementStates.Visible)
			{
				switch (value.GetType().Name)
				{
					case "Decimal":
					case "Double":
						formattedValue = GetDoubleFormat((double)value);
						break;
					case "Int":
					case "Int64":
					case "Long":
						formattedValue = ((long)value).ToString("N0");
						break;
					case "String":
					default:
						formattedValue = "";
						break;
				}

				base.Paint(graphics, clipBounds, cellBounds, rowIndex, cellState, value, formattedValue, errorText, cellStyle, advancedBorderStyle, paintParts);
			}
		}

		private string GetDoubleFormat(double value)
		{
			value = value * DisplayFactor;
			_formattedValue = (value).ToString("N7").Trim(new char[] { '0' });

			switch (TickSize.ToString())
			{
				case "0.0025":			// 1/400
					break;
				case "0.125":			// 1/8
					break;
				case "0.0625":			// 1/16
					break;
				case "0.03125":			// 1/32
					break;
				case "0.015625":		// 1/64
					break;
				case "0.0078125":		// 1/128
					break;
				default:
					_formattedValue = (value).ToString("N7").Trim(new char[] { '0' });
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
					break;
			}

			return _formattedValue;
		}
	}
}
