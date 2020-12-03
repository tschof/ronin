using System;
using System.Collections.Generic;
using System.Drawing;
using System.Runtime.Serialization;
using SerializationEx;

namespace DictionaryEx
{
	[Serializable]
	public abstract class MutiTypedUIDictionary
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
			set
			{
				_dateTimes = value;
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

		private Dictionary<int, Font> _fonts;
		public Dictionary<int, Font> Fonts
		{
			get
			{
				if (_fonts == null)
				{
					_fonts = new Dictionary<int, Font>();
				}
				return _fonts;
			}
			set
			{
				_fonts = value;
			}
		}

		private Dictionary<int, Color> _colors;
		public Dictionary<int, Color> Colors
		{
			get
			{
				if (_colors == null)
				{
					_colors = new Dictionary<int, Color>();
				}
				return _colors;
			}
			set
			{
				_colors = value;
			}
		}

		public MutiTypedUIDictionary()
		{
		}

		public MutiTypedUIDictionary(int key, string val)
		{
			Modify(key, val);
		}
		public MutiTypedUIDictionary(int key, double val)
		{
			Modify(key, val);
		}
		public MutiTypedUIDictionary(int key, long val)
		{
			Modify(key, val);
		}
		public MutiTypedUIDictionary(int key, DateTime val)
		{
			Modify(key, val);
		}
		public MutiTypedUIDictionary(int key, bool val)
		{
			Modify(key, val);
		}
		public MutiTypedUIDictionary(int key, Font val)
		{
			Modify(key, val);
		}
		public MutiTypedUIDictionary(int key, Color val)
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
		public bool Add(int key, Font val)
		{
			return Modify(key, val);
		}
		public bool Add(int key, Color val)
		{
			return Modify(key, val);
		}

		public bool Remove(int key)
		{
			bool changed = false;

			lock (_syncObj)
			{
				if (_strings.ContainsKey(key))
				{
					_strings.Remove(key);
					changed = true;
				}

				if (_doubles.ContainsKey(key))
				{
					_doubles.Remove(key);
					changed = true;
				}

				if (_longs.ContainsKey(key))
				{
					_longs.Remove(key);
					changed = true;
				}

				if (_dateTimes.ContainsKey(key))
				{
					_dateTimes.Remove(key);
					changed = true;
				}

				if (_bools.ContainsKey(key))
				{
					_bools.Remove(key);
					changed = true;
				}
			}

			return changed;
		}

		public bool Update(MutiTypedUIDictionary b)
		{
			// Todo Update of b while b is still in use?
			bool changed = false;

			if (b.Strings != null)
			{
				lock (b.Strings)
				{
					foreach (KeyValuePair<int, string> kvp in b.Strings)
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
			}

			if (b.Doubles != null)
			{
				lock (b.Doubles)
				{
					foreach (KeyValuePair<int, double> kvp in b.Doubles)
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
			}

			if (b.Longs != null)
			{
				lock (b.Longs)
				{
					foreach (KeyValuePair<int, long> kvp in b.Longs)
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
			}

			if (b.DateTimes != null)
			{
				lock (b.DateTimes)
				{
					foreach (KeyValuePair<int, DateTime> kvp in b.DateTimes)
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
			}

			if (b.Bools != null)
			{
				lock (b.Bools)
				{
					foreach (KeyValuePair<int, bool> kvp in b.Bools)
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
			}

			if (b.Fonts != null)
			{
				lock (b.Fonts)
				{
					foreach (KeyValuePair<int, Font> kvp in b.Fonts)
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
			}

			if (b.Colors != null)
			{
				lock (b.Colors)
				{
					foreach (KeyValuePair<int, Color> kvp in b.Colors)
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
		public bool Update(int key, Font val)
		{
			return Modify(key, val, true);
		}
		public bool Update(int key, Color val)
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
				DateTimes.Clear();
				Bools.Clear();
				Fonts.Clear();
				Colors.Clear();
			}
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

			lock (_syncObj)
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

		private bool Modify(int key, DateTime val)
		{
			return Modify(key, val, false);
		}
		private bool Modify(int key, DateTime val, bool update)
		{
			bool changed = false;

			lock (_syncObj)
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

			lock (_syncObj)
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

		private bool Modify(int key, Font val)
		{
			return Modify(key, val, false);
		}
		private bool Modify(int key, Font val, bool update)
		{
			bool changed = false;

			lock (_syncObj)
			{
				if (Fonts.ContainsKey(key))
				{
					if (update)
					{
						if (Fonts[key] != val)
						{
							Fonts[key] = val;
							changed = true;
						}
					}
				}
				else
				{
					Fonts.Add(key, val);
					changed = true;
				}
			}

			return changed;
		}

		private bool Modify(int key, Color val)
		{
			return Modify(key, val, false);
		}
		private bool Modify(int key, Color val, bool update)
		{
			bool changed = false;

			lock (_syncObj)
			{
				if (Colors.ContainsKey(key))
				{
					if (update)
					{
						if (Colors[key] != val)
						{
							Colors[key] = val;
							changed = true;
						}
					}
				}
				else
				{
					Colors.Add(key, val);
					changed = true;
				}
			}

			return changed;
		}
	}
}
