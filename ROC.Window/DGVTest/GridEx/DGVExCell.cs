using System;
using System.Windows.Forms;
using System.Drawing;

namespace GridEx
{
	public enum CellChanges
	{
		None,
		Up,
		Down,
	}

	public class DGVExCell : DataGridViewTextBoxCell
	{
		private object _lastValue = null;
		
		private double _currentDouble = 0.0;
		private double _lastDouble = 0.0;
		
		private long _currentLong = 0;
		private long _lastLong = 0;

		private string _lastString = null;

		private CellChanges _currentChange  = CellChanges.None;
		private bool _isChanged = false;

		private Color _defaultCellColor = Color.Empty;

		public override object DefaultNewRowValue
		{
			get
			{
				_lastValue = null;
				return _lastValue;
				//return base.DefaultNewRowValue;
			}
		}

		protected override object GetFormattedValue(object value, int rowIndex, ref DataGridViewCellStyle cellStyle, System.ComponentModel.TypeConverter valueTypeConverter, System.ComponentModel.TypeConverter formattedValueTypeConverter, DataGridViewDataErrorContexts context)
		{
			if (value != null)
			{
				if (_lastValue != value)
				{
					_isChanged = true;

					switch (value.GetType().Name)
					{
						case "Decimal":
						case "Double":
							_currentDouble = (double)value;

							if (_currentDouble > _lastDouble)
							{
								_currentChange = CellChanges.Up;
							}
							else if (_currentDouble < _lastDouble)
							{
								_currentChange = CellChanges.Down;
							}
							else
							{
								_currentChange = CellChanges.None;
							}

							_lastDouble = _currentDouble;
							break;
						case "Int":
						case "Int16":
						case "Int32":
						case "Int64":
						case "Long":
							_currentLong = (long)value;

							if (_currentLong > _lastLong)
							{
								_currentChange = CellChanges.Up;
							}
							else if (_currentLong < _lastLong)
							{
								_currentChange = CellChanges.Down;
							}
							else
							{
								_currentChange = CellChanges.None;
							}

							_lastLong = _currentLong;
							break;
						case "String":
							_lastString = (string)value;
							break;
						default:
							break;
					}

					_lastValue = value;
				}
				else
				{
					_isChanged = false;
				}
			}
			else
			{
				// flag the change from Non Null to Null
				if (_lastValue != value)
				{
					_isChanged = true;

					_lastValue = null;
				}
				else
				{
					_isChanged = false;
				}
			}

			return base.GetFormattedValue(value, rowIndex, ref cellStyle, valueTypeConverter, formattedValueTypeConverter, context);
		}

		protected override void Paint(System.Drawing.Graphics graphics, System.Drawing.Rectangle clipBounds, System.Drawing.Rectangle cellBounds, int rowIndex, DataGridViewElementStates cellState, object value, object formattedValue, string errorText, DataGridViewCellStyle cellStyle, DataGridViewAdvancedBorderStyle advancedBorderStyle, DataGridViewPaintParts paintParts)
		{
			if (_defaultCellColor == Color.Empty)
			{
				_defaultCellColor = cellStyle.ForeColor;
			}

			switch (_currentChange)
			{
				case CellChanges.Up:
					cellStyle.ForeColor = Color.Green;
					break;
				case CellChanges.Down:
					cellStyle.ForeColor = Color.Red;
					break;
				case CellChanges.None:
				default:
					cellStyle.ForeColor = _defaultCellColor;
					break;
			}

			base.Paint(graphics, clipBounds, cellBounds, rowIndex, cellState, value, formattedValue, errorText, cellStyle, advancedBorderStyle, paintParts);
		}
	}
}
