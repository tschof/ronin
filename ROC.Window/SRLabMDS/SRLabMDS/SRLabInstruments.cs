using System;
using System.Collections.Generic;
using System.Text;
using CSMD;

namespace SRLabTest
{
	internal sealed class SRLabInstruments
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

		public InstrumentDef Set(string symbol)
		{
			return Set(symbol, "");
		}
		public InstrumentDef Set(string symbol, string source)
		{
			if (!Instruments.ContainsKey(symbol))
			{
				Instruments.Add(symbol, new InstrumentDef(InstrumentDef.InstrumentType.EXCH_UNKNOWN, source, symbol, ""));
			}

			return Instruments[symbol];
		}
	}
}
