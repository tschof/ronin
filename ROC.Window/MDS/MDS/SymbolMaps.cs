using System;
using System.Collections.Generic;

using MarketDataEx;

namespace MDS
{
	public class SymbolMapCollection
	{
		public SymbolMapCollection()
		{
		}

		public void Update(MDClientToServer symbolMap)
		{
			if (!MDSymbols.Contains(symbolMap.MDSymbol))
			{
				MDSymbols.Add(symbolMap.MDSymbol);
			}

			if (SymbolMaps.ContainsKey(symbolMap.MDSymbol))
			{
				SymbolMaps[symbolMap.MDSymbol] = symbolMap;
			}
			else
			{
				SymbolMaps.Add(symbolMap.MDSymbol, symbolMap);
			}
		}

		public void Remove(MDClientToServer symbolMap)
		{
			if (MDSymbols.Contains(symbolMap.MDSymbol))
			{
				MDSymbols.Remove(symbolMap.MDSymbol);
			}

			if (SymbolMaps.ContainsKey(symbolMap.MDSymbol))
			{
				SymbolMaps.Remove(symbolMap.MDSymbol);
			}
		}

		private List<string> _mdSymbols;
		public List<string> MDSymbols
		{
			get
			{
				if (_mdSymbols == null)
				{
					_mdSymbols = new List<string>();
				}
				return _mdSymbols;
			}
		}

		private Dictionary<string, MDClientToServer> _symbolMaps;
		public Dictionary<string, MDClientToServer> SymbolMaps
		{
			get
			{
				if (_symbolMaps == null)
				{
					_symbolMaps = new Dictionary<string, MDClientToServer>();
				}
				return _symbolMaps;
			}
			set
			{
				_symbolMaps = value;
			}
		}
	}
}
