namespace DataGridViewEx
{
	public class DGVBaseAlgoTypeCell : DGVBaseCell
	{
		public override string GetDisplayValue(object value)
		{
			return (value is long algoType) ? CSVEx.CSVFieldIDs.AlgoTypes.GetDescription(algoType) : "";
		}
	}
}
