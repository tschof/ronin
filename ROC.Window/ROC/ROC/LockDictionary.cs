using System.Collections.Generic;

namespace ROC
{
	internal class LockDictionary<TKey, TValue>
	{
		private Dictionary<TKey, TValue> _collection = new Dictionary<TKey, TValue>();

		internal bool Has(TKey key)
		{
			lock (this) {
				return _collection.ContainsKey(key);
			}
		}

		internal void Add(TKey key, TValue value)
		{
			lock (this) {
				_collection[key] = value;
			}
		}

		internal bool TryAdd(TKey key, TValue value)
		{
			lock (this) {
				if (_collection.ContainsKey(key))
					return false;
				_collection.Add(key, value);
			}
			return true;
		}

		internal bool TryGetValue(TKey key, out TValue value) => _collection.TryGetValue(key, out value);

		internal Dictionary<TKey, TValue> TakeAll()
		{
			Dictionary<TKey, TValue> taken, reset = new Dictionary<TKey, TValue>();
			lock (this) {
				taken = _collection;
				_collection = reset;
			}
			return taken;
		}
	}
}
