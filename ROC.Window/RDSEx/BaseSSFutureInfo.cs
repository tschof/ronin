namespace RDSEx
{
	public class BaseSSFutureInfo
	{
		public string SecType { get; } = CSVEx.CSVFieldIDs.SecurityTypes.Future;
		public string MDSource = "ONEC"; // SecurityFieldIDs.SSFuture.dataSource
		public string MDSymbol = ""; // SecurityFieldIDs.SSFuture.fullSymbol
		public string ExpDate = null;
		public double ContractSize = 1;
		public double TickSize = 1;
	}
}
