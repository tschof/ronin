using System.Collections.Generic;

namespace ROC
{
	internal class LockList<T>
	{
		private List<T> _collection = new List<T>();

		internal bool Has(T value)
		{
			lock (this) {
				return _collection.Contains(value);
			}
		}

		internal void Add(T value)
		{
			lock (this) {
				_collection.Add(value);
			}
		}

		internal bool TryAdd(T value)
		{
			lock (this) {
				if (_collection.Contains(value))
					return false;
				_collection.Add(value);
			}
			return true;
		}

		internal void AddRange(IEnumerable<T> values)
		{
			lock (this) {
				_collection.AddRange(values);
			}
		}

		internal List<T> TakeAll()
		{
			List<T> values, reset = new List<T>();
			lock (this) {
				values = _collection;
				_collection = reset;
			}
			return values;
		}
	}
}
