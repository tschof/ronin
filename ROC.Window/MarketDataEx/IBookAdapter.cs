namespace MarketData
{
	public abstract class IBookAdapter
	{
		public abstract QuoteCollection GetAsks();
		public abstract QuoteCollection GetBids();
		public abstract QuoteCollection GetTradedVolumes();
		public abstract void VisitFields(Book book);
	}
}
