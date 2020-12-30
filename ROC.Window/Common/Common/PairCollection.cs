using System.Collections.Generic;
using System.Linq;

namespace Common
{
	// The Dictionary<> class is used all over tthis code, often for very
	// small collections, which is inefficient.  This class implements
	// a small KeyValuePair collection as a List<>.  For > 100 entries or
	// so, use an actual Dictionary<> instead.
	// Also see the KeyValuePair Deconstruct() extension in Extensions.cs.

	public class PairCollection<TKey, TValue>
	{
		private List<KeyValuePair<TKey, TValue>> _container;

		public int Count => _container.Count;

		public PairCollection()
		{
			_container = new List<KeyValuePair<TKey, TValue>>();
		}

		public PairCollection(PairCollection<TKey, TValue> other)
		{
			_container = other._container;
		}

		public bool Exists(System.Predicate<TValue> match)
		{
			return _container.Exists(n => match(n.Value));
		}

		public bool TryGetValue(TKey key, out TValue value)
		{
			int index = _container.FindIndex(n => n.Key.Equals(key));
			if (index < 0) {
				value = default;
				return false;
			}
			value = _container[index].Value;
			return true;
		}

		public bool TryGetFront(out TValue first)
		{
			if (_container.Count > 0) {
				first = _container[0].Value;
				return true;
			}
			first = default;
			return false;
		}

		public bool TryGetBack(out TValue last)
		{
			if (_container.Count > 0) {
				last = _container[_container.Count - 1].Value;
				return true;
			}
			last = default;
			return false;
		}

		/// <summary>
		/// Assigns the given value to the entry with the given key.  If no such entry
		/// exists, an applicable entry is added only if addIfMissing is true.  If no
		/// applicable entry exists and addIfMissing is false, no action is taken.
		/// </summary>
		/// <param name="key">The key of the entry to update.</param>
		/// <param name="value">The value to assign the entry with the given key.</param>
		/// <param name="addIfMissing">If no entry for the given key exists, an entry
		/// is added only if this parameter is set to true.</param>
		/// <returns>True if the collection had an existing entry for the given key,
		/// false otherwise.</returns>
		public bool Set(TKey key, TValue value, bool addIfMissing = true)
		{
			int index = _container.FindIndex(n => n.Key.Equals(key));
			if (index >= 0) {
				_container[index] = new KeyValuePair<TKey, TValue>(key, value);
				return true;
			} else if (addIfMissing) {
				_container.Add(new KeyValuePair<TKey, TValue>(key, value));
			}
			return false;
		}

		/// <summary>
		/// Sets 'value' to the value corresponding to the given key.  If no such entry
		/// exists and 'factory' is not null, an applicable entry is added and 'value'
		/// set to the value of the new entry.
		/// </summary>
		/// <param name="key">The key of the requested entry.</param>
		/// <param name="value">This parameter is assigned to the value of the found
		/// or added entry.</param>
		/// <param name="factory">A function for creating a new entry in case no entry
		/// with the given key already exists.</param>
		/// <returns>Returns true if an entry with the given key was found, false if
		/// an applicable entry was created or no action was taken.</returns>
		public bool FindOrAdd(TKey key, out TValue value, System.Func<TValue> factory)
		{
			int index = _container.FindIndex(n => n.Key.Equals(key));
			if (index >= 0) {
				value = _container[index].Value;
				return true;
			} else if (factory != null) {
				value = factory();
				_container.Add(new KeyValuePair<TKey, TValue>(key, value));
			} else {
				value = default;
			}
			return false;
		}

		public void Remove(TKey key)
		{
			_container.RemoveAll(n => n.Key.Equals(key));
		}

		public void Clear()
		{
			_container.Clear();
		}

		// Expose Linq.Select(), to permit Select() on this collection.
		public IEnumerable<TOut> Select<TOut>(System.Converter<TValue, TOut> xform)
		{
			return _container.Select(n => xform(n.Value));
		}

		public Enumerator GetEnumerator()
		{
			return new Enumerator(_container);
		}

		// Hide the actual container implementation behind a custom enumerator.
		public class Enumerator : IEnumerator<KeyValuePair<TKey, TValue>>
		{
			private readonly List<KeyValuePair<TKey, TValue>> _list;
			private int _current = -1;

			public Enumerator(List<KeyValuePair<TKey, TValue>> collection)
			{
				_list = collection;
			}

			public KeyValuePair<TKey, TValue> Current => ok() ? _list[_current] : default;

			object System.Collections.IEnumerator.Current => ok() ? _list[_current] : default;

			public void Dispose()
			{
			}

			public bool MoveNext()
			{
				if (ok()) {
					++_current;
					return ok();
				}
				return false;
			}

			public void Reset()
			{
				_current = -1;
			}

			private bool ok() => (_list != null) && (_current < _list.Count);
		}
	}
}
