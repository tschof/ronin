using System;
using System.Windows.Forms;

namespace DataGridViewEx
{
	public abstract class DGVBaseCellNoPaint : DataGridViewTextBoxCell
	{
		public override object DefaultNewRowValue
		{
			get
			{
				return null;
			}
		}

		public new object EditedFormattedValue
		{
			get
			{
				return GetDisplayValue(Value);
			}
		}

		public override void InitializeEditingControl(int rowIndex, object initialFormattedValue, DataGridViewCellStyle dataGridViewCellStyle)
		{
			//if (Value != null)
			//{
			//	initialFormattedValue = Value.ToString();
			//}
			base.InitializeEditingControl(rowIndex, initialFormattedValue, dataGridViewCellStyle);
		}

		protected override object GetFormattedValue(object value, int rowIndex, ref DataGridViewCellStyle cellStyle, System.ComponentModel.TypeConverter valueTypeConverter, System.ComponentModel.TypeConverter formattedValueTypeConverter, DataGridViewDataErrorContexts context)
		{
			return GetDisplayValue(value);
			//return base.GetFormattedValue(value, rowIndex, ref cellStyle, valueTypeConverter, formattedValueTypeConverter, context);
		}

		public abstract string GetDisplayValue(object value);
	}
}
