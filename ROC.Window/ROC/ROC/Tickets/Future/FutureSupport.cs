using System;
using System.Collections.Generic;
using System.Text;

namespace ROC
{
	public class FutureSupport : BookSupport
	{
		internal bool IsSpread
		{
			get
			{
				if (CurrentSymbolDetail.Contains("-"))
				{
					return true;
				}
				return false;
			}
		}

		private List<string> _gettingSSFChainSymbols;
		internal List<string> GettingSSFChainSymbols
		{
			get
			{
				if (_gettingSSFChainSymbols == null)
				{
					_gettingSSFChainSymbols = new List<string>();
				}
				return _gettingSSFChainSymbols;
			}
			set
			{
				_gettingSSFChainSymbols = value;
			}
		}
	}
}
