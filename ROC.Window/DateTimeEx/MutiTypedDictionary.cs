using System;
using System.Collections.Generic;

namespace DictionaryEx
{
	[Serializable]
	public abstract class MutiTypedDictionary
	{
		private object _syncObj;
		public object SyncObj
		{
			get
			{
				if (_syncObj == null)
				{
					_syncObj = new object();
				}
				return _syncObj;
			}
		}

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
		}

		private Dictionary<int, DateTime> _dateTimes;
		public Dictionary<int, DateTime> DateTimes
		{
			get
			{
				if (_dateTimes == null)
				{
					_dateTimes = new Dictionary<int, DateTime>();
				}
				return _dateTimes;
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
		public MutiTypedDictionary(int key, DateTime val)
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
		public bool Add(int key, DateTime val)
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

			lock (SyncObj)
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

				if (DateTimes.ContainsKey(key))
				{
					DateTimes.Remove(key);
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

		public bool Update(MutiTypedDictionary b)
		{
			// Todo Update of b while b is still in use?
			bool changed = false;

			lock (SyncObj)
			{
				if (b.Strings != null)
				{
					foreach (int key in b.Strings.Keys)
					{
						if (!changed)
						{
							changed = Update(key, b.Strings[key]);
						}
						else
						{
							Update(key, b.Strings[key]);
						}
					}
				}

				if (b.Doubles != null)
				{
					foreach (int key in b.Doubles.Keys)
					{
						if (!changed)
						{
							changed = Update(key, b.Doubles[key]);
						}
						else
						{
							Update(key, b.Doubles[key]);
						}
					}
				}

				if (b.Longs != null)
				{
					foreach (int key in b.Longs.Keys)
					{
						if (!changed)
						{
							changed = Update(key, b.Longs[key]);
						}
						else
						{
							Update(key, b.Longs[key]);
						}
					}
				}

				if (b.DateTimes != null)
				{
					foreach (int key in b.DateTimes.Keys)
					{
						if (!changed)
						{
							changed = Update(key, b.DateTimes[key]);
						}
						else
						{
							Update(key, b.DateTimes[key]);
						}
					}
				}

				if (b.Bools != null)
				{
					foreach (int key in b.Bools.Keys)
					{
						if (!changed)
						{
							changed = Update(key, b.Bools[key]);
						}
						else
						{
							Update(key, b.Bools[key]);
						}
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
		public bool Update(int key, DateTime val)
		{
			return Modify(key, val, true);
		}
		public bool Update(int key, bool val)
		{
			return Modify(key, val, true);
		}

		public void Clear()
		{
			Longs.Clear();
			Doubles.Clear();
			Strings.Clear();
			DateTimes.Clear();
			Bools.Clear();
		}

		public long Count
		{
			get
			{
				return Longs.Count + Doubles.Count + Strings.Count + DateTimes.Count + Bools.Count;
			}
		}

		private bool Modify(int key, long val)
		{
			return Modify(key, val, false);
		}
		private bool Modify(int key, long val, bool update)
		{
			bool changed = false;

			lock (SyncObj)
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

			lock (SyncObj)
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

			lock (SyncObj)
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

		private bool Modify(int key, DateTime val)
		{
			return Modify(key, val, false);
		}
		private bool Modify(int key, DateTime val, bool update)
		{
			bool changed = false;

			lock (SyncObj)
			{
				if (DateTimes.ContainsKey(key))
				{
					if (update)
					{
						if (DateTimes[key] != val)
						{
							DateTimes[key] = val;
							changed = true;
						}
					}
				}
				else
				{
					DateTimes.Add(key, val);
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

			lock (SyncObj)
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
	}
}
