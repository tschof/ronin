using System;
using System.Collections.Generic;
using System.Runtime.Serialization;

using SerializationEx;

namespace DictionaryEx
{
	[Serializable]
	public class MultiTypedDictionary : ISerializable
	{
		public interface IVisitor
		{
			void OnField(int key, string value);
			void OnField(int key, double value);
			void OnField(int key, long value);
			void OnField(int key, bool value);
		}

		protected class GenericDictionary<T>
		{
			private readonly Dictionary<int, T> _collection;

			internal int Count { get { return _collection.Count; } }

			internal GenericDictionary()
			{
				_collection = new Dictionary<int, T>();
			}

			internal GenericDictionary(GenericDictionary<T> other)
			{
				_collection = new Dictionary<int, T>(other._collection);
			}

			internal GenericDictionary(SerializationReader reader)
			{
				_collection = (Dictionary<int, T>)reader.ReadDictionary<int, T>();
			}

			internal bool Has(int key)
			{
				return _collection.ContainsKey(key);
			}

			internal bool TryGet(int key, out T val)
			{
				return _collection.TryGetValue(key, out val);
			}

			internal bool Set(int key, T val, bool updateIfExists)
			{
				// No good ConcurrentDictionary methods for updating
				// the collection and determining if a value was added
				// or modified, so we're kinda stuck with conventional
				// Dictionary.
				bool changed = false;
				lock (_collection) {
					if (!_collection.TryGetValue(key, out T found)) {
						_collection.Add(key, val);
						changed = true;
					} else if (updateIfExists && !val.Equals(found)) {
						_collection[key] = val;
						changed = true;
					}
				}

				return changed;
			}

			internal bool Update(Dictionary<int, T> other, bool updateIfExists)
			{
				bool changed = false;
				List<(int, T)> add = new List<(int, T)>();

				lock (other) {
					Dictionary<int, T>.Enumerator i = other.GetEnumerator();
					while (i.MoveNext()) {
						add.Add((i.Current.Key, i.Current.Value));
					}
				}

				lock (_collection) {
					foreach (var entry in add) {
						if (!_collection.TryGetValue(entry.Item1, out T found)) {
							_collection.Add(entry.Item1, entry.Item2);
							changed = true;
						} else if (updateIfExists && !entry.Item2.Equals(found)) {
							_collection[entry.Item1] = entry.Item2;
							changed = true;
						}
					}
				}

				return changed;
			}

			internal bool Update(GenericDictionary<T> other)
			{
				return Update(other._collection, true);
			}

			internal bool Update(GenericDictionary<T> other, bool updateIfExists)
			{
				return Update(other._collection, updateIfExists);
			}

			internal bool Remove(int key)
			{
				if (_collection.ContainsKey(key)) {
					lock (_collection) {
						return _collection.Remove(key);
					}
				}
				return false;
			}

			internal void Clear()
			{
				lock (_collection) {
					_collection.Clear();
				}
			}

			internal Dictionary<int, T>.Enumerator GetEnumerator()
			{
				return _collection.GetEnumerator();
			}

			internal void Serialize(SerializationWriter writer)
			{
				writer.Write(_collection);
			}
		}

		private GenericDictionary<long> _longs;
		private GenericDictionary<double> _doubles;
		private GenericDictionary<string> _strings;
		private GenericDictionary<bool> _bools;

		public long Count {
			get {
				return _longs.Count + _doubles.Count + _strings.Count + _bools.Count;
			}
		}

		public MultiTypedDictionary()
		{
			_longs = new GenericDictionary<long>();
			_doubles = new GenericDictionary<double>();
			_strings = new GenericDictionary<string>();
			_bools = new GenericDictionary<bool>();
		}

		public MultiTypedDictionary(MultiTypedDictionary other)
		{
			_longs = new GenericDictionary<long>(other._longs);
			_doubles = new GenericDictionary<double>(other._doubles);
			_strings = new GenericDictionary<string>(other._strings);
			_bools = new GenericDictionary<bool>(other._bools);
		}

		public bool Has(int key)
		{
			return _longs.Has(key) || _doubles.Has(key) || _strings.Has(key) || _bools.Has(key);
		}

		public bool TryGet(int key, out long val)
		{
			return _longs.TryGet(key, out val);
		}

		public bool TryGet(int key, out double val)
		{
			return _doubles.TryGet(key, out val);
		}

		public bool TryGet(int key, out string val)
		{
			return _strings.TryGet(key, out val);
		}

		public bool TryGet(int key, out bool val)
		{
			return _bools.TryGet(key, out val);
		}

		public bool Set<T>(int key, T val, bool updateIfExists = false)
		{
			switch (val) {
				case long l: return _longs.Set(key, l, updateIfExists);
				case double d: return _doubles.Set(key, d, updateIfExists);
				case string s: return _strings.Set(key, s, updateIfExists);
				case bool b: return _bools.Set(key, b, updateIfExists);
			}
			return false;
		}

		public bool Remove(int key)
		{
			bool changed = _longs.Remove(key);
			changed = _doubles.Remove(key) || changed;
			changed = _strings.Remove(key) || changed;
			changed = _bools.Remove(key) || changed;
			return changed;
		}

		public bool Merge(MultiTypedDictionary other, bool fast = false)
		{
			bool changed = _longs.Update(other._longs);
			changed = _doubles.Update(other._doubles) || changed;
			changed = _strings.Update(other._strings) || changed;
			changed = _bools.Update(other._bools) || changed;
			return changed;
		}

		public bool Merge<T>(Dictionary<int, T> other)
		{
			switch (other) {
				case Dictionary<int, long> l: return _longs.Update(l, true);
				case Dictionary<int, double> d: return _doubles.Update(d, true);
				case Dictionary<int, string> s: return _strings.Update(s, true);
				case Dictionary<int, bool> b: return _bools.Update(b, true);
			}
			return false;
		}

		public void Clear()
		{
			_longs.Clear();
			_doubles.Clear();
			_strings.Clear();
			_bools.Clear();
		}

		public void Visit(IVisitor visitor)
		{
			Dictionary<int, string>.Enumerator si = _strings.GetEnumerator();
			while (si.MoveNext())
				visitor.OnField(si.Current.Key, si.Current.Value);

			Dictionary<int, double>.Enumerator di = _doubles.GetEnumerator();
			while (di.MoveNext())
				visitor.OnField(di.Current.Key, di.Current.Value);

			Dictionary<int, long>.Enumerator li = _longs.GetEnumerator();
			while (li.MoveNext())
				visitor.OnField(li.Current.Key, li.Current.Value);

			Dictionary<int, bool>.Enumerator bi = _bools.GetEnumerator();
			while (bi.MoveNext())
				visitor.OnField(bi.Current.Key, bi.Current.Value);
		}

		#region ISerializable Members

		// Serialize the object.
		public void GetObjectData(SerializationInfo info, StreamingContext context)
		{
			SerializationWriter writer = new SerializationWriter();
			_strings.Serialize(writer);
			_longs.Serialize(writer);
			_doubles.Serialize(writer);
			_bools.Serialize(writer);
			writer.AddToInfo(info);
		}

		// Deserialization constructor.
		public MultiTypedDictionary(SerializationInfo info, StreamingContext ctxt)
		{
			SerializationReader reader = new SerializationReader(info);
			_strings = new GenericDictionary<string>(reader);
			_longs = new GenericDictionary<long>(reader);
			_doubles = new GenericDictionary<double>(reader);
			_bools = new GenericDictionary<bool>(reader);
		}

		#endregion
	}
}
