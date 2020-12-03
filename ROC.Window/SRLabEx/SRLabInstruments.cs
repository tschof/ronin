using System;
using System.Collections.Generic;
using System.Text;
using CSMD;

namespace SRLabEx
{
	public sealed class SRLabInstruments
	{
		private string _smName = "";
		public string SMName
		{
			get
			{
				return _smName;
			}
		}

		private Dictionary<string, InstrumentDef> _instruments;
		public Dictionary<string, InstrumentDef> Instruments
		{
			get
			{
				if (_instruments == null)
				{
					_instruments = new Dictionary<string, InstrumentDef>();
				}
				return _instruments;
			}
			set
			{
				_instruments = value;
			}
		}

		public SRLabInstruments(string smName)
		{
			_smName = SMName;
		}

		public InstrumentDef Set(string symbol, string key)
		{
			return Set(symbol, "", key);
		}
		public InstrumentDef Set(string symbol, string source, string key)
		{
			if (!Instruments.ContainsKey(key))
			{
				Instruments.Add(key, new InstrumentDef(InstrumentDef.InstrumentType.EXCH_UNKNOWN, source, symbol, ""));
			}

			return Instruments[key];
		}
	}
}
