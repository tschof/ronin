namespace DataGridViewEx
{
	public class DGVBaseTypeCell : DGVBaseCell
	{
		public override string GetDisplayValue(object value)
		{
			return (value is long typeCode) ? CSVEx.CSVFieldIDs.OrderTypes.GetDescription(typeCode).ToUpper() : "";
		}
	}
}
