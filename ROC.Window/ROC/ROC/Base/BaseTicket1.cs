using System;
using System.Collections.Generic;
using System.Text;

namespace ROC
{
	public class BaseTicket
	{
		public sealed class TradeTicks
		{
			public const int Up = 0;
			public const int None = 1;
			public const int Down = 2;
		}

		private Dictionary<string, string> _imSymbolNeeded;
		public Dictionary<string, string> ImSymbolNeeded
		{
			get
			{
				if (_imSymbolNeeded == null)
				{
					_imSymbolNeeded = new Dictionary<string, string>();
				}
				return _imSymbolNeeded;
			}
			set
			{
				_imSymbolNeeded = value;
			}
		}

		#region - Current Ticket Properties -

		private double _tickSize = 0.01;
		public double TickSize
		{
			get
			{
				return _tickSize;
			}
			set
			{
				_tickSize = value;
			}
		}

		public decimal TickSizeDec
		{
			get
			{
				return Convert.ToDecimal(TickSize);
			}
		}

		private double _contractSize = 1;
		public double ContractSize
		{
			get
			{
				return _contractSize;
			}
			set
			{
				_contractSize = value;
			}
		}

		private List<string> _mdSymbol = new List<string>();
		public List<string> MDSymbol
		{
			get
			{
				return _mdSymbol;
			}
			set
			{
				_mdSymbol = value;
			}
		}

		#endregion
	}

}
