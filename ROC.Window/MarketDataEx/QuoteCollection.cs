using System.Collections;
using System.Collections.Generic;
using Price = Common.Price;

namespace MarketData
{
	// Represents one side of the market (bid or ask) for a given asset.
	public class QuoteCollection : IEnumerable<Quote>
	{
		private List<Quote> _quotes;

		public bool Empty => (_quotes == null) || (_quotes.Count == 0);
		public int Count => (_quotes == null) ? 0 : _quotes.Count;

		public QuoteCollection()
		{
			_quotes = new List<Quote>();
		}

		public QuoteCollection(QuoteCollection other)
		{
			_quotes = new List<Quote>(other._quotes);
		}

		public QuoteCollection(List<Quote> quotes)
		{
			_quotes = quotes;
		}

		public bool HasLevel(Price level)
		{
			return _quotes.Exists(n => n.QuotePrice == level);
		}

		public void SetLevel(Price level, long volume)
		{
			lock (this) {
				int i = _quotes.FindIndex(n => n.QuotePrice == level);
				if (i < 0) {
					_quotes.Add(new Quote(level, volume));
				} else {
					_quotes[i].SetSize(volume);
				}
			}
		}

		public bool Remove(Price level)
		{
			if (_quotes == null)
				return false;

			int removed;
			lock (this) {
				removed = _quotes.RemoveAll(n => n.QuotePrice == level);
			}
			return removed > 0;
		}

		public void Merge(QuoteCollection other)
		{
			if ((_quotes == null) || (other == null))
				return;

			lock (this) {
				lock (other) {
					foreach (Quote q in other._quotes) {
						int index = _quotes.FindIndex(n => n.QuotePrice == q.QuotePrice);
						if (index < 0) {
							_quotes.Add(q);
						} else {
							_quotes[index].SetSize(q.Size);
						}
					}
				}
			}
		}

		// Snap quotes to a given tick size, adjusting non-conformant
		// levels by 'adjustment'.
		public void SnapQuotes(double ticksize, double adjustment)
		{
			lock (this) {
				List<Quote> raw = _quotes;
				_quotes = new List<Quote>();

				foreach (Quote q in raw) {
					Price key;

					if (Price.IsZeroValue(q.QuotePrice.Value % ticksize)) {
						key = q.QuotePrice;
					} else {
						key = new Price(q.QuotePrice.Value + adjustment);
					}

					int index = _quotes.FindIndex(n => n.QuotePrice == key);
					if (index < 0) {
						_quotes.Add(new Quote(key, q.Size));
					} else {
						_quotes[index].SetSize(q.Size);
					}
				}
			}
		}

		// Sort quotes and truncates to the given depth, and remove prices that 
		// violate top-of-book as given by limitPrice.
		// Use depthLimit = -1 for no depth limit.
		// Use priceLimit = Price.UnsetPrice for no price limit.
		// Refactored from BookSupport.cs:CheckBook and AutoSpreadSupport.cs:CheckLegBooks.
		public void SortLevels(Price.SortOrder order, int depthLimit, Price priceLimit, System.Converter<long, long> sizeXformFn = null)
		{
			if ((_quotes == null) || (_quotes.Count == 0))
				return;

			List<Quote> result = new List<Quote>();

			lock (this) {
				// bid = descending (highest bid is top), ask = ascending (lowest ask is top)
				_quotes.Sort(new QuoteComparer(order));

				if (priceLimit.IsZero) {
					// Set limitPrice to be the top of the book.
					priceLimit = System.Math.Round(_quotes[0].QuotePrice.Value, 7);
					if (priceLimit.IsZero)
						return;
				}

				int count = _quotes.Count;
				if ((depthLimit > 0) && (depthLimit < count))
					count = depthLimit;

				for (int i = 0; i < count; ++i) {
					(Price price, long size) = _quotes[i];
					Price currentPrice = System.Math.Round(price.Value, 7);

					bool wantLevel;
					if (!priceLimit.HasValue)
						wantLevel = true;
					else if (order == Price.SortOrder.Ask)
						wantLevel = currentPrice >= priceLimit;
					else
						wantLevel = currentPrice <= priceLimit;

					if (wantLevel && !result.Exists(n => n.QuotePrice == currentPrice))
						result.Add(new Quote(currentPrice, sizeXformFn == null ? size : sizeXformFn(size)));
				}

				_quotes = result;
			}
		}

		private class QuoteComparer : IComparer<Quote>
		{
			private readonly Price.SortOrder _order;

			internal QuoteComparer(Price.SortOrder order)
			{
				_order = order;
			}

			public int Compare(Quote x, Quote y)
			{
				return x.QuotePrice.CompareTo(y.QuotePrice);
			}
		}

		public IEnumerator<Quote> GetEnumerator()
		{
			return _quotes.GetEnumerator();
		}

		IEnumerator IEnumerable.GetEnumerator()
		{
			return _quotes.GetEnumerator();
		}
	}
}
