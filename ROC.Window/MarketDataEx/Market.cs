using System.Collections;
using System.Collections.Generic;

namespace MarketData
{
	public class Market : IEnumerable<(string, Book)>
	{
		private Dictionary<string, Book> _market;

		public bool Empty => _market.Count == 0;

		public int Count => _market.Count;

		public Market()
		{
			_market = new Dictionary<string, Book>();
		}

		private Market(Dictionary<string, Book> viscera)
		{
			_market = viscera;
		}

		public bool TryGet(string symbol, out Book book)
		{
			if (!string.IsNullOrEmpty(symbol) && _market.TryGetValue(symbol, out book) && (book != null))
				return true;
			book = null;
			return false;
		}

		public void Merge(Market other)
		{
			if (other != null) {
				lock (this) {
					var i = other._market.GetEnumerator();
					while (i.MoveNext()) {
						if (!string.IsNullOrEmpty(i.Current.Key) && _market.TryGetValue(i.Current.Key, out Book found) && (found != null))
							found.Merge(i.Current.Value);
						else
							_market.Add(i.Current.Key, i.Current.Value);
					}
				}
			}
		}

		public bool Add(string symbol, Book book)
		{
			if (!string.IsNullOrEmpty(symbol) && (book != null) && !_market.ContainsKey(symbol)) {
				lock (this) {
					_market.Add(symbol, book);
					return true;
				}
			}
			return false;
		}

		public void AddRange(IEnumerable<(string, Book)> collection)
		{
			lock (this) {
				foreach ((string symbol, Book book) in collection) {
					if (!string.IsNullOrEmpty(symbol) && (book != null) && !_market.ContainsKey(symbol))
						_market.Add(symbol, book);
				}
			}
		}

		public void Update(string symbol, Book book)
		{
			if (!string.IsNullOrEmpty(symbol) && (book != null)) {
				lock (this) {
					if (_market.TryGetValue(symbol, out Book found))
						found.Merge(book);
					else
						_market.Add(symbol, book);
				}
			}
		}

		public void Remove(string symbol)
		{
			lock (this) {
				_market.Remove(symbol);
			}
		}

		public void Clear()
		{
			lock (this) {
				_market.Clear();
			}
		}

		public void CopySymbolsTo(ICollection<string> collection)
		{
			lock (this) {
				var i = _market.GetEnumerator();
				while (i.MoveNext())
					collection.Add(i.Current.Key);
			}
		}

		public static Market Replace(Market other)
		{
			Dictionary<string, Book> viscera = other._market;
			other._market = new Dictionary<string, Book>();
			return new Market(viscera);
		}

		#region IEnumerable methods

		// Create our own IEnumerable methods to enable iterating over the Market
		// class while abstracting the underlying data structure, and ensuring that
		// access exclusivity is preserved.
		// 
		// The enumerated value is a (string, Book) tuple:
		//
		//     foreach ((string symbol, Book book) in stockMarket) {
		//         displayBook(symbol, book);
		//     }

		public IEnumerator<(string, Book)> GetEnumerator()
		{
			return new Enumerator(_market);
		}

		IEnumerator IEnumerable.GetEnumerator()
		{
			return new Enumerator(_market);
		}

		#endregion // IEnumerable methods

		#region IEnumerator implementation

		// Presents (string, Book) tuples, encapsulating the implementation of
		// markets as Dictionary<string, Book>.

		public class Enumerator : IEnumerator<(string, Book)>
		{
			private List<(string, Book)> _updates = new List<(string, Book)>();
			private int _index = -1;
			private static (string, Book) UNSET = (null, null);

			internal Enumerator(Dictionary<string, Book> container)
			{
				lock (container) {
					var i = container.GetEnumerator();
					while (i.MoveNext()) {
						if (!string.IsNullOrEmpty(i.Current.Key) && (i.Current.Value != null))
							_updates.Add((i.Current.Key, i.Current.Value));
					}
				}
			}

			public (string, Book) Current => _updates == null ? UNSET : _updates[_index];

			object IEnumerator.Current => _updates == null ? UNSET : _updates[_index];

			public void Dispose()
			{
				_updates = null;
			}

			public bool MoveNext()
			{
				if (_updates == null) {
					return false;
				} else if (++_index < _updates.Count) {
					return true;
				} else {
					_updates = null;
					return false;
				}
			}

			public void Reset()
			{
				_index = -1;
			}
		}

		#endregion // IEnumerator implementation
	}
}
