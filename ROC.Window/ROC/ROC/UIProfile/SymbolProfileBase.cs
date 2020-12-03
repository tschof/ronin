using System;
using System.Drawing;
using System.Collections.Generic;

using DictionaryEx;

namespace ROC
{
	[Serializable]
	public abstract class SymbolProfileBase : MutiTypedUIDictionary
	{
		public SymbolProfileBase()
		{
		}
	}

	[Serializable]
	public class SymbolProfile : SymbolProfileBase
	{
	}
}
