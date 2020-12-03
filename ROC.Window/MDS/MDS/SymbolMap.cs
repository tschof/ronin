using System;
using DictionaryEx;

namespace MDS
{
	public class SymbolMap : MutiTypedDictionary
	{
		public string IssuedSymbol
		{
			get
			{
				if (Strings.ContainsKey(SymbolMapFieldIDs.mdSymbol))
				{
					return Strings[SymbolMapFieldIDs.mdSymbol];
				}
				else
				{
					return "";
				}
			}
		}

		public string DataSource
		{
			get
			{
				if (Strings.ContainsKey(SymbolMapFieldIDs.mdSource))
				{
					return Strings[SymbolMapFieldIDs.mdSource];
				}
				else
				{
					return "";
				}
			}
		}

		public string SecType
		{
			get
			{
				if (Strings.ContainsKey(SymbolMapFieldIDs.secType))
				{
					return Strings[SymbolMapFieldIDs.secType];
				}
				else
				{
					return "";
				}
			}
		}

		public long Updates
		{
			get
			{
				if (Longs.ContainsKey(SymbolMapFieldIDs.updates))
				{
					return Longs[SymbolMapFieldIDs.updates];
				}
				else
				{
					return 0;
				}
			}
		}

		public string Username
		{
			get
			{
				if (Strings.ContainsKey(SymbolMapFieldIDs.username))
				{
					return Strings[SymbolMapFieldIDs.username];
				}
				else
				{
					return "";
				}
			}
		}

		public string Password
		{
			get
			{
				if (Strings.ContainsKey(SymbolMapFieldIDs.password))
				{
					return Strings[SymbolMapFieldIDs.password];
				}
				else
				{
					return "";
				}
			}
		}
	}
}
