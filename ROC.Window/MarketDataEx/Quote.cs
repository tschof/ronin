using Price = Common.Price;

namespace MarketData
{
	public class Quote
	{
		public readonly Price QuotePrice;
		public long Size { get; private set; } = 0;

		public Quote(Quote other)
		{
			QuotePrice = other.QuotePrice;
			Size = other.Size;
		}

		public Quote(Price p, long s)
		{
			QuotePrice = p;
			Size = s;
		}

		public Quote(double p, long s)
		{
			QuotePrice = new Price(p);
			Size = s;
		}

		public void AddSize(long s)
		{
			Size += s;
		}

		public void SetSize(long s)
		{
			Size = s;
		}

		public void Deconstruct(out Price price, out long size)
		{
			price = QuotePrice;
			size = Size;
		}
	}
}
