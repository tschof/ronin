using System;
using System.Collections.Generic;
using System.Runtime.Serialization;
using SerializationEx;
using System.Diagnostics;

namespace DictionaryEx
{
	[Serializable]
	public abstract class MutiTypedDictionary : ISerializable
	{
		private object _syncObj = new object();

		private Dictionary<int, long> _longs;
		public Dictionary<int, long> Longs
		{
			get
			{
				if (_longs == null)
				{
					_longs = new Dictionary<int, long>();
				}
				return _longs;
			}
			set
			{
				_longs = value;
			}
		}

		private Dictionary<int, double> _doubles;
		public Dictionary<int, double> Doubles
		{
			get
			{
				if (_doubles == null)
				{
					_doubles = new Dictionary<int, double>();
				}
				return _doubles;
			}
			set
			{
				_doubles = value;
			}
		}

		private Dictionary<int, string> _strings;
		public Dictionary<int, string> Strings
		{
			get
			{
				if (_strings == null)
				{
					_strings = new Dictionary<int, string>();
				}
				return _strings;
			}
			set
			{
				_strings = value;
			}
		}

		private Dictionary<int, bool> _bools;
		public Dictionary<int, bool> Bools
		{
			get
			{
				if (_bools == null)
				{
					_bools = new Dictionary<int, bool>();
				}
				return _bools;
			}
			set
			{
				_bools = value;
			}
		}

		public MutiTypedDictionary()
		{
		}

		public MutiTypedDictionary(int key, string val)
		{
			Modify(key, val);
		}
		public MutiTypedDictionary(int key, double val)
		{
			Modify(key, val);
		}
		public MutiTypedDictionary(int key, long val)
		{
			Modify(key, val);
		}
		public MutiTypedDictionary(int key, bool val)
		{
			Modify(key, val);
		}

		public bool Add(int key, string val)
		{
			return Modify(key, val);
		}
		public bool Add(int key, double val)
		{
			return Modify(key, val);
		}
		public bool Add(int key, long val)
		{
			return Modify(key, val);
		}
		public bool Add(int key, bool val)
		{
			return Modify(key, val);
		}

		public bool Remove(int key)
		{
			bool changed = false;

			lock (_syncObj)
			{
				if (Strings.ContainsKey(key))
				{
					Strings.Remove(key);
					changed = true;
				}

				if (Doubles.ContainsKey(key))
				{
					Doubles.Remove(key);
					changed = true;
				}

				if (Longs.ContainsKey(key))
				{
					Longs.Remove(key);
					changed = true;
				}

				if (Bools.ContainsKey(key))
				{
					Bools.Remove(key);
					changed = true;
				}
			}

			return changed;
		}

		public bool Update(MutiTypedDictionary locScr)
		{
			return Update(locScr, false);
		}
		public bool Update(MutiTypedDictionary locScr, bool fast)
		{
			bool changed = false;

			if (locScr.Strings != null)
			{
				lock (locScr.Strings)
				{
					//if (fast)
					//{
					//    Strings = new Dictionary<int, string>(locScr.Strings);
					//}
					//else
					//{
						foreach (KeyValuePair<int, string> kvp in locScr.Strings)
						{
							if (kvp.Value != null)
							{
								if (!changed)
								{
									changed = Update(kvp.Key, kvp.Value);
								}
								else
								{
									Update(kvp.Key, kvp.Value);
								}
							}
						}
					//}
				}
			}

			if (locScr.Doubles != null)
			{
				lock (locScr.Doubles)
				{
					//if (fast)
					//{
					//    Doubles = new Dictionary<int, double>(locScr.Doubles);
					//}
					//else
					//{
						foreach (KeyValuePair<int, double> kvp in locScr.Doubles)
						{
							if (!changed)
							{
								changed = Update(kvp.Key, kvp.Value);
							}
							else
							{
								Update(kvp.Key, kvp.Value);
							}
						}
					//}
				}
			}

			if (locScr.Longs != null)
			{
				lock (locScr.Longs)
				{
					//if (fast)
					//{
					//    Longs = new Dictionary<int, long>(locScr.Longs);
					//}
					//else
					//{
						foreach (KeyValuePair<int, long> kvp in locScr.Longs)
						{
							if (!changed)
							{
								changed = Update(kvp.Key, kvp.Value);
							}
							else
							{
								Update(kvp.Key, kvp.Value);
							}
						}
					//}
				}
			}

			if (locScr.Bools != null)
			{
				lock (locScr.Bools)
				{
					//if (fast)
					//{
					//    Bools = new Dictionary<int, bool>(locScr.Bools);
					//}
					//else
					//{
						foreach (KeyValuePair<int, bool> kvp in locScr.Bools)
						{
							if (!changed)
							{
								changed = Update(kvp.Key, kvp.Value);
							}
							else
							{
								Update(kvp.Key, kvp.Value);
							}
						}
					//}
				}
			}

			return changed;
		}

		public bool Update(Dictionary<int, double> doubles)
		{
			bool changed = false;

			lock (doubles)
			{
				foreach (KeyValuePair<int, double> kvp in doubles)
				{
					if (!changed)
					{
						changed = Update(kvp.Key, kvp.Value);
					}
					else
					{
						Update(kvp.Key, kvp.Value);
					}
				}
			}

			return changed;
		}
		public bool Update(Dictionary<int, long> longs)
		{
			bool changed = false;

			lock (longs)
			{
				foreach (KeyValuePair<int, long> kvp in longs)
				{
					if (!changed)
					{
						changed = Update(kvp.Key, kvp.Value);
					}
					else
					{
						Update(kvp.Key, kvp.Value);
					}
				}
			}

			return changed;
		}
		public bool Update(Dictionary<int, string> strings)
		{
			bool changed = false;

			lock (strings)
			{
				foreach (KeyValuePair<int, string> kvp in strings)
				{
					if (!changed)
					{
						changed = Update(kvp.Key, kvp.Value);
					}
					else
					{
						Update(kvp.Key, kvp.Value);
					}
				}
			}

			return changed;
		}

		public bool Update(int key, string val)
		{
			return Modify(key, val, true);
		}
		public bool Update(int key, double val)
		{
			return Modify(key, val, true);
		}
		public bool Update(int key, long val)
		{
			return Modify(key, val, true);
		}
		public bool Update(int key, bool val)
		{
			return Modify(key, val, true);
		}

		public void Clear()
		{
			lock (_syncObj)
			{
				Longs.Clear();
				Doubles.Clear();
				Strings.Clear();
				Bools.Clear();
			}
		}

		public long Count
		{
			get
			{
				return Longs.Count + Doubles.Count + Strings.Count + Bools.Count;
			}
		}

		private bool Modify(int key, long val)
		{
			return Modify(key, val, false);
		}
		private bool Modify(int key, long val, bool update)
		{
			bool changed = false;

			lock (Longs)
			{
				if (Longs.ContainsKey(key))
				{
					if (update)
					{
						if (Longs[key] != val)
						{
							Longs[key] = val;
							changed = true;
						}
					}
				}
				else
				{
					Longs.Add(key, val);
					changed = true;
				}
			}

			return changed;
		}

		private bool Modify(int key, double val)
		{
			return Modify(key, val, false);
		}
		private bool Modify(int key, double val, bool update)
		{
			bool changed = false;

			lock (_syncObj)
			{
				if (Doubles.ContainsKey(key))
				{
					if (update)
					{
						if (Doubles[key] != val)
						{
							Doubles[key] = val;
							changed = true;
						}
					}
				}
				else
				{
					Doubles.Add(key, val);
					changed = true;
				}
			}

			return changed;
		}

		private bool Modify(int key, string val)
		{
			return Modify(key, val, false);
		}
		private bool Modify(int key, string val, bool update)
		{
			bool changed = false;

			lock (_syncObj)
			{
				if (Strings.ContainsKey(key))
				{
					if (update)
					{
						if (Strings[key] != val)
						{
							Strings[key] = val;
							changed = true;
						}
					}
				}
				else
				{
					Strings.Add(key, val);
					changed = true;
				}
			}

			return changed;
		}

		private bool Modify(int key, bool val)
		{
			return Modify(key, val, false);
		}
		private bool Modify(int key, bool val, bool update)
		{
			bool changed = false;

			lock (Bools)
			{
				if (Bools.ContainsKey(key))
				{
					if (update)
					{
						if (Bools[key] != val)
						{
							Bools[key] = val;
							changed = true;
						}
					}
				}
				else
				{
					Bools.Add(key, val);
					changed = true;
				}
			}

			return changed;
		}
		
		#region ISerializable Members
		
		// Serialize the object.
		public void GetObjectData(SerializationInfo info, StreamingContext context)
		{
			SerializationWriter sw = SerializationWriter.GetWriter();

			sw.Write<int, string>(Strings);
			sw.Write<int, long>(Longs);
			sw.Write<int, double>(Doubles);
			sw.Write<int, bool>(Bools);
			sw.AddToInfo(info);
		}

		// Deserialization constructor.
		public MutiTypedDictionary(SerializationInfo info, StreamingContext ctxt)
		{
			SerializationReader sr = SerializationReader.GetReader(info);

			Strings = (Dictionary<int, string>)sr.ReadDictionary<int, string>();
			Longs = (Dictionary<int, long>)sr.ReadDictionary<int, long>();
			Doubles = (Dictionary<int, double>)sr.ReadDictionary<int, double>();
			Bools = (Dictionary<int, bool>)sr.ReadDictionary<int, bool>();
		}

		#endregion
	}
}
