using System;
using System.Collections.Generic;

using DictionaryEx;

namespace RDSEx
{
	public abstract class BaseTypedAccountMap : MutiTypedDictionary
	{
		private Dictionary<string, AccountMap> _csAccounts;
		public Dictionary<string, AccountMap> CSAccounts
		{
			get
			{
				if (_csAccounts == null)
				{
					_csAccounts = new Dictionary<string, AccountMap>();
				}
				return _csAccounts;
			}
			set
			{
				_csAccounts = value;
			}
		}

		private Dictionary<string, AccountMap> _optAccounts;
		public Dictionary<string, AccountMap> OPTAccounts
		{
			get
			{
				if (_optAccounts == null)
				{
					_optAccounts = new Dictionary<string, AccountMap>();
				}
				return _optAccounts;
			}
			set
			{
				_optAccounts = value;
			}
		}

		private Dictionary<string, AccountMap> _futAccounts;
		public Dictionary<string, AccountMap> FUTAccounts
		{
			get
			{
				if (_futAccounts == null)
				{
					_futAccounts = new Dictionary<string, AccountMap>();
				}
				return _futAccounts;
			}
			set
			{
				_futAccounts = value;
			}
		}
	}

	public abstract class BaseTraded : MutiTypedDictionary
	{
		public abstract bool IsRom
		{
			get;
		}

		public abstract string TradeID
		{
			get;
		}

		public abstract long Qty
		{
			get;
		}

		public abstract double Price
		{
			get;
		}

		public abstract double TradeValue
		{
			get;
		}

		public abstract DateTime TradeTime
		{
			get;
		}
	}

	public abstract class BaseOrder : MutiTypedDictionary
	{
		public abstract bool IsRoc
		{
			get;
		}

		private Dictionary<string, Traded> _trades;
		public Dictionary<string, Traded> Trades
		{
			get
			{
				if (_trades == null)
				{
					_trades = new Dictionary<string, Traded>();
				}
				return _trades;
			}
			set
			{
				_trades = value;
			}
		}

		public abstract string PositionKey
		{
			get;
		}

		public abstract string Symbol
		{
			get;
		}

		public abstract string Underlying
		{
			get;
		}

		public abstract string ExpDate
		{
			get;
		}

		public abstract string StrikePrice
		{
			get;
		}

		public abstract string CallPut
		{
			get;
		}

		public abstract string Trader
		{
			get;
		}

		public abstract string LocalAcct
		{
			get;
		}

		public abstract string OmTag
		{
			get;
		}
	}

	public abstract class BasePosition : MutiTypedDictionary
	{
		public abstract bool HasOrders
		{
			get;
		}

		public abstract List<string> OmTags
		{
			get;
			set;
		}

		private Dictionary<string, Traded> _trades;
		public Dictionary<string, Traded> Trades
		{
			get
			{
				if (_trades == null)
				{
					_trades = new Dictionary<string, Traded>();
				}
				return _trades;
			}
			set
			{
				_trades = value;
			}
		}

		public abstract string PositionKey
		{
			get;
		}

		public abstract string Symbol
		{
			get;
		}

		public abstract string Underlying
		{
			get;
		}

		public abstract string ExpDate
		{
			get;
		}

		public abstract string StrikePrice
		{
			get;
		}

		public abstract string CallPut
		{
			get;
		}

		public abstract string Trader
		{
			get;
		}

		public abstract string LocalAcct
		{
			get;
		}
	}
}
