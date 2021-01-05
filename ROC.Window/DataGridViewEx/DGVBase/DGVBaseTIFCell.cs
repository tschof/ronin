using System;
namespace DataGridViewEx
{
	public class DGVBaseTIFCell : DGVBaseCell
	{
		public override string GetDisplayValue(object value)
		{
			return (value is long tifCode) ? CSVEx.CSVFieldIDs.TIFCodes.GetDescription(tifCode).ToUpper() : "";
		}
	}
}
