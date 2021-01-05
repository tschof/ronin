namespace RDSEx
{
	public class IMSSFutureInfo : BaseSSFutureInfo
	{
		public string FullSymbol { get; private set; } = "";
		public string Exchange { get; private set; } = "";
		public string DataSource { get; private set; } = "";
		public string ExpirationDate { get; private set; } = "";
		public string CurCode { get; private set; } = "";
		public string LongName { get; private set; } = "";

		public IMSSFutureInfo(RDSEx.WEB.FutureDesc ssfuture)
		{
			FullSymbol = ssfuture.fullSymbol;
			DataSource = ssfuture.exchange;
			ExpirationDate = ssfuture.expirationDate;
			ContractSize = ssfuture.contractSize;
			TickSize = ssfuture.tickSize;
			CurCode = ssfuture.curCode;
			LongName = ssfuture.longName;
		}
	}
}
