using System;
using System.Collections.Generic;
using System.Text;
using MarketDataEx;
using ArrayEx;
using RDSEx;
using System.Data;
using CSVEx;

namespace ROC
{
	public sealed class TagKeyItems
	{
		private char[] _splitArray = new char[] { ';' };
		private string _tag = "";

		private int _masterOrderID = 0;
		private int _legNumber = 1;
		private int _limitMarketPrice = 2;
		private int _stopMarketPrice = 3;

		private Dictionary<int, string> _items;
		public Dictionary<int, string> Items
		{
			get
			{
				if (_items == null)
				{
					_items = new Dictionary<int, string>();
				}
				return _items;
			}
			set
			{
				_items = value;
			}
		}

		public TagKeyItems()
		{
		}
		public TagKeyItems(string tag)
		{
			Decode(tag);
		}
		public void Decode(string tag)
		{
			_tag = tag;
			string[] items = tag.Split(_splitArray, StringSplitOptions.None);

			int key = 0;
			Items.Clear();
			foreach (string item in items)
			{
				Items.Add(key, item);
				key++;
			}
		}

		public string Tag
		{
			get
			{
				return _tag;
			}
		}

		public string MasterOrderID
		{
			get
			{
				if (Items.ContainsKey(_masterOrderID))
				{
					return Items[_masterOrderID];
				}
				return "";
			}
		}

		public string LegNumber
		{
			get
			{
				if (Items.ContainsKey(_legNumber))
				{
					return Items[_legNumber];
				}
				return "";
			}
		}

		public Nullable<int> LegNumberInt
		{
			get
			{
				if (LegNumber != "")
				{
					try
					{
						return Convert.ToInt32(LegNumber);
					}
					catch (Exception ex)
					{
						GLOBAL.HROC.AddToException(ex);
					}
				}

				return null;
			}
		}

		public string LimitMarketPrice
		{
			get
			{
				if (Items.ContainsKey(_limitMarketPrice))
				{
					return Items[_limitMarketPrice];
				}
				return "";
			}
		}

		public string StopMarketPrice
		{
			get
			{
				if (Items.ContainsKey(_stopMarketPrice))
				{
					return Items[_stopMarketPrice];
				}
				return "";
			}
		}
	}

	public sealed class ParentTagKeyItems
	{
		private char[] _splitArray = new char[] { ';' };

		private int _masterOrderID = 0;
		private int _limitSpreadPrice = 1;
		private int _stopSpreadPrice = 2;
		private int _sideSpread = 3;
		private int _qtySpread = 4;

		private Dictionary<int, string> _items;
		public Dictionary<int, string> Items
		{
			get
			{
				if (_items == null)
				{
					_items = new Dictionary<int, string>();
				}
				return _items;
			}
			set
			{
				_items = value;
			}
		}

		public ParentTagKeyItems()
		{
		}
		public ParentTagKeyItems(string parentTag)
		{
			Decode(parentTag);
		}
		public void Decode(string parentTag)
		{
			string[] items = (parentTag).Split(_splitArray);

			int key = 0;
			Items.Clear();
			foreach (string item in items)
			{
				Items.Add(key, item);
				key++;
			}
		}

		public string MasterOrderID
		{
			get
			{
				if (Items.ContainsKey(_masterOrderID))
				{
					try
					{
						return Items[_masterOrderID];
					}
					catch (Exception ex)
					{
						GLOBAL.HROC.AddToException(ex);
					}
				}
				return "";
			}
		}

		public string LimitSpreadPrice
		{
			get
			{
				if (Items.ContainsKey(_limitSpreadPrice))
				{
					return Items[_limitSpreadPrice];
				}
				return "";
			}
		}

		public Nullable<double> LimitSpreadPriceDouble
		{
			get
			{
				if (LimitSpreadPrice != "")
				{
					try
					{
						return Convert.ToDouble(LimitSpreadPrice);
					}
					catch (Exception ex)
					{
						GLOBAL.HROC.AddToException(ex);
					}
				}
				return null;
			}
		}

		public string StopSpreadPrice
		{
			get
			{
				if (Items.ContainsKey(_stopSpreadPrice))
				{
					return Items[_stopSpreadPrice];
				}
				return "";
			}
		}

		public Nullable<double> StopSpreadPriceDouble
		{
			get
			{
				if (StopSpreadPrice != "")
				{
					try
					{
						return Convert.ToDouble(StopSpreadPrice);
					}
					catch (Exception ex)
					{
						GLOBAL.HROC.AddToException(ex);
					}
				}
				return null;
			}
		}

		public string MasterSideSpread
		{
			get
			{
				if (Items.ContainsKey(_sideSpread))
				{
					return Items[_sideSpread];
				}
				return "";
			}
		}

		public string MasterSideSpreadName
		{
			get
			{
				switch (MasterSideSpread)
				{
					case "1":
						return "Buy";
					case "2":
						return "Sell";
					default:
						return "";
				}
			}
		}

		public Nullable<long> MasterSideSpreadLong
		{
			get
			{
				switch (MasterSideSpread)
				{
					case "1":
						return CSVEx.CSVFieldIDs.SideCodes.Buy;
					case "2":
						return CSVEx.CSVFieldIDs.SideCodes.Sell;
					default:
						return null;
				}
			}
		}

		public string MasterQtySpread
		{
			get
			{
				if (Items.ContainsKey(_qtySpread))
				{
					return Items[_qtySpread];
				}
				return "";
			}
		}

		public Nullable<long> MasterQtySpreadLong
		{
			get
			{
				if (MasterQtySpread != "")
				{
					try
					{
						return Convert.ToInt64(MasterQtySpread);
					}
					catch (Exception ex)
					{
						GLOBAL.HROC.AddToException(ex);
					}
				}
				return null;
			}
		}
	}

	public sealed class AutoSpreadItem
	{
		public AutoSpreadItem(int legNumber, int legBookDepthLimit)
		{
			_legNumber = legNumber;
			_legBookDepthLimit = legBookDepthLimit;
		}

		private int _legBookDepthLimit = 3;
		public int LegBookDepthLimit
		{
			get
			{
				return _legBookDepthLimit;
			}
			set
			{
				_legBookDepthLimit = value;
			}
		}

		private int _legNumber = 0;
		public int LegNumber
		{
			get
			{
				return _legNumber;
			}
		}

		private int _rowIndex = 0;
		public int RowIndex
		{
			get
			{
				return _rowIndex;
			}
			set
			{
				_rowIndex = value;
			}
		}

		private BaseSecurityInfo _secInfo = null;
		public BaseSecurityInfo SecInfo
		{
			get
			{
				if (_secInfo == null)
				{
					_secInfo = new BaseSecurityInfo();
				}
				return _secInfo;
			}
		}

		private string _symbolDetail = "";
		public string SymbolDetail
		{
			get
			{
				return _symbolDetail;
			}
			set
			{
				if (_symbolDetail != value.ToUpper())
				{
					_symbolDetail = value.ToUpper();
					if (_symbolDetail != "")
					{
						_secInfo = GLOBAL.HRDS.GetSecurityInfoBySymbolDetail(_symbolDetail);
					}
					else
					{
						_secInfo = new BaseSecurityInfo();
					}
				}
			}
		}

		private string _exchange = "";
		public string Exchange
		{
			get
			{
				return _exchange;
			}
			set
			{
				_exchange = value;
			}
		}

		private string _account = "";
		public string LocalAcctAcrn
		{
			get
			{
				return _account;
			}
			set
			{
				_account = value;
			}
		}

		private string _tradeFor = "";
		public string TradeFor
		{
			get
			{
				return _tradeFor;
			}
			set
			{
				_tradeFor = value;
			}
		}

		private string _shortLender = "";
		public string ShortLender
		{
			get
			{
				return _shortLender;
			}
			set
			{
				_shortLender = value;
			}
		}

		private double _factor = 1;
		public double Factor
		{
			get
			{
				return _factor;
			}
			set
			{
				_factor = value;
			}
		}

		public decimal FactorDec
		{
			get
			{
				return Convert.ToDecimal(_factor);
			}
		}

		private double _ratio = 0;
		public double Ratio
		{
			get
			{
				if (UserRatio != 0)
				{
					return UserRatio;
				}
				return _ratio;
			}
			set
			{
				_ratio = value;
			}
		}

		private double _userRatio = 0;
		public double UserRatio
		{
			get
			{
				return _userRatio;
			}
			set
			{
				_userRatio = value;
			}
		}

		private long _tickLimit = 5;
		public long TickLimit
		{
			get
			{
				return _tickLimit;
			}
			set
			{
				_tickLimit = value;
			}
		}

		private bool _isLean = false;
		public bool IsLean
		{
			get
			{
				return _isLean;
			}
			set
			{
				_isLean = value;
			}
		}

		private bool _isNewL1 = true;
		public bool IsNewL1
		{
			get
			{
				return _isNewL1;
			}
			set
			{
				_isNewL1 = value;
			}
		}

		private bool _isNewBook = true;
		public bool IsNewBook
		{
			get
			{
				return _isNewBook;
			}
			set
			{
				_isNewBook = value;
			}
		}

		public bool HasBooks
		{
			get
			{
				if (MarketData.BidBooks.Count > 0 && MarketData.AskBooks.Count > 0)
				{
					return true;
				}
				return false;
			}
		}

		private MDServerToClient _marketData;
		public MDServerToClient MarketData
		{
			get
			{
				if (_marketData == null)
				{
					_marketData = new MDServerToClient();
				}
				return _marketData;
			}
			set
			{
				_marketData = value;
			}
		}

		public void CheckLegBooks()
		{
			if (MarketData.BidPrice != null)
			{
				MarketData.BidBooks = CheckLegBook("Bid", MarketData.BidBooks, (double)MarketData.BidPrice);
			}
			else
			{
				MarketData.BidBooks.Clear();
			}
			if (MarketData.AskPrice != null)
			{
				MarketData.AskBooks = CheckLegBook("Ask", MarketData.AskBooks, (double)MarketData.AskPrice);
			}
			else
			{
				MarketData.AskBooks.Clear();
			}
		}
		private Dictionary<double, long> CheckLegBook(string side, Dictionary<double, long> book, double limitPrice)
		{
			Dictionary<double, long> result = new Dictionary<double, long>();

			if (book.Count > 0)
			{
				List<double> prices = ArrayHelper.ConvertToList(book);
				switch (side)
				{
					case "Bid":
						prices.Sort();
						prices.Reverse();
						break;
					case "Ask":
						prices.Sort();
						break;
				}

				foreach (double price in prices)
				{
					if (result.Count > LegBookDepthLimit)
					{
						// End of the book limit
						break;
					}

					switch (side)
					{
						case "Bid":
							// Bid
							if (limitPrice != 0 && price <= limitPrice && !result.ContainsKey(price))
							{
								result.Add(price, CalculateSyntheticVolume(book[price]));
							}
							break;
						case "Ask":
							// Ask
							if (limitPrice != 0 && price >= limitPrice && !result.ContainsKey(price))
							{
								result.Add(price, CalculateSyntheticVolume(book[price]));
							}
							break;
					}
				}
			}

			return result;
		}
		private long CalculateSyntheticVolume(long volume)
		{
			switch (SecInfo.SecType)
			{
				case CSVFieldIDs.SecutrityTypes.Equity:
					return Convert.ToInt64(Math.Floor(volume * 100 / Math.Abs(Factor)));
				default:
					return Convert.ToInt64(Math.Floor(volume / Math.Abs(Factor)));
			}
		}

		private Nullable<double> _buyLimitPrice;
		public Nullable<double> BuyLimitPrice
		{
			get
			{
				return _buyLimitPrice;
			}
			set
			{
				_buyLimitPrice = value;
			}
		}
		public string BuyLimitPriceSTR
		{
			get
			{
				if (_buyLimitPrice != null)
				{
					if (SecInfo != null)
					{
						return ((double)_buyLimitPrice).ToString("F" + SecInfo.DecimalPlaces);
					}
					else
					{
						((double)_buyLimitPrice).ToString();
					}
				}
				return "";
			}
		}
		public Nullable<long> BuyLimitPriceFromBidPrice
		{
			get
			{
				if (BuyLimitPrice != null)
				{
					if (SecInfo != null)
					{
						return (long)(Math.Abs((double)BuyLimitPrice - BidPrice) / SecInfo.TickSize);
					}
				}

				return null;
			}
		}

		private Nullable<double> _buyStopPrice;
		public Nullable<double> BuyStopPrice
		{
			get
			{
				return _buyStopPrice;
			}
			set
			{
				_buyStopPrice = value;
			}
		}
		public string BuyStopPriceSTR
		{
			get
			{
				if (_buyStopPrice != null)
				{
					if (SecInfo != null)
					{
						return ((double)_buyStopPrice).ToString("F" + SecInfo.DecimalPlaces);
					}
					else
					{
						((double)_buyStopPrice).ToString();
					}
				}
				return "";
			}
		}

		private string _bidMarketPrice = "";
		public string BidMarketPrice
		{
			get
			{
				return _bidMarketPrice;
			}
			set
			{
				_bidMarketPrice = value;
			}
		}

		private Nullable<double> _sellLimitPrice;
		public Nullable<double> SellLimitPrice
		{
			get
			{
				return _sellLimitPrice;
			}
			set
			{
				_sellLimitPrice = value;
			}
		}
		public string SellLimitPriceSTR
		{
			get
			{
				if (_sellLimitPrice != null)
				{
					if (SecInfo != null)
					{
						return ((double)_sellLimitPrice).ToString("F" + SecInfo.DecimalPlaces);
					}
					else
					{
						((double)_sellLimitPrice).ToString();
					}
				}
				return "";
			}
		}
		public Nullable<long> SellLimitPriceFromAskPrice
		{
			get
			{
				if (SellLimitPrice != null)
				{
					if (SecInfo != null)
					{
						return (long)(Math.Abs((double)SellLimitPrice - AskPrice) / SecInfo.TickSize);
					}
				}

				return null;
			}
		}

		private Nullable<double> _sellStopPrice;
		public Nullable<double> SellStopPrice
		{
			get
			{
				return _sellStopPrice;
			}
			set
			{
				_sellStopPrice = value;
			}
		}
		public string SellStopPriceSTR
		{
			get
			{
				if (_sellStopPrice != null)
				{
					if (SecInfo != null)
					{
						return ((double)_sellStopPrice).ToString("F" + SecInfo.DecimalPlaces);
					}
					else
					{
						((double)_sellStopPrice).ToString();
					}
				}
				return "";
			}
		}

		private string _askMarketPrice = "";
		public string AskMarketPrice
		{
			get
			{
				return _askMarketPrice;
			}
			set
			{
				_askMarketPrice = value;
			}
		}

		public double TradedPrice
		{
			get
			{
				if (MarketData.TradePrice != null)
				{
					return (double)MarketData.TradePrice;
				}
				return 0;
			}
		}
		public string TradedPriceSTR
		{
			get
			{
				if (SecInfo != null)
				{
					return TradedPrice.ToString("F" + SecInfo.DecimalPlaces);
				}
				else
				{
					return TradedPrice.ToString();
				}
			}
		}
		public long TradedVolume
		{
			get
			{
				if (MarketData.TradeVolume != null)
				{
					return (long)MarketData.TradeVolume;
				}
				return 0;
			}
		}

		public double BidPrice
		{
			get
			{
				if (MarketData.BidPrice != null)
				{
					return (double)MarketData.BidPrice;
				}
				return 0;
			}
		}
		public string BidPriceSTR
		{
			get
			{
				if (SecInfo != null)
				{
					return BidPrice.ToString("F" + SecInfo.DecimalPlaces);
				}
				else
				{
					return BidPrice.ToString();
				}
			}
		}
		public long BidSize
		{
			get
			{
				if (MarketData.BidSize != null)
				{
					return (long)MarketData.BidSize;
				}
				return 0;
			}
		}

		public double AskPrice
		{
			get
			{
				if (MarketData.AskPrice != null)
				{
					return (double)MarketData.AskPrice;
				}
				return 0;
			}
		}
		public string AskPriceSTR
		{
			get
			{
				if (SecInfo != null)
				{
					return AskPrice.ToString("F" + SecInfo.DecimalPlaces);
				}
				else
				{
					return AskPrice.ToString();
				}
			}
		}
		public long AskSize
		{
			get
			{
				if (MarketData.AskSize != null)
				{
					return (long)MarketData.AskSize;
				}
				return 0;
			}
		}
	}

	internal sealed class LevelItems
	{
		private Dictionary<double, LevleItem> _item = new Dictionary<double, LevleItem>();
		public Dictionary<double, LevleItem> Item
		{
			get
			{
				return _item;
			}
		}

		public LevelItems(PriceVolumePair pv)
		{
			_item.Add(pv.Price, new LevleItem(pv.Price, pv.Volume));
		}

		public void AddLeg(double basePrice, List<PriceVolumePair> pvs)
		{
			if (_item.ContainsKey(basePrice))
			{
				foreach (PriceVolumePair pv in pvs)
				{
					if (!_item.ContainsKey(pv.Price))
					{
						_item.Add(pv.Price, new LevleItem(pv.Price, pv.Volume, _item[basePrice].Volumes.ToArray()));
					}
					else
					{
						_item[pv.Price].Volumes.Add(pv.Volume);
					}
				}
				_item.Remove(basePrice);
			}
		}
	}

	// keep the one price and all the volume for this price
	internal sealed class LevleItem
	{
		private double _price = 0;

		public LevleItem(double price, long volume)
		{
			AddLeg(price, volume, new long[0]);
		}
		public LevleItem(double price, long volume, long[] volumes)
		{
			AddLeg(price, volume, volumes);
		}

		public void AddLeg(double price, long volume, long[] volumes)
		{
			_price = _price + price;
			Volumes.Add(volume);
			if (volumes.Length > 0)
			{
				Volumes.AddRange(volumes);
			}
		}

		public double Price
		{
			get
			{
				return Math.Round(_price, 7);
			}
		}

		private List<long> _volumes;
		public List<long> Volumes
		{
			get
			{
				if (_volumes == null)
				{
					_volumes = new List<long>();
				}
				return _volumes;
			}
			set
			{
				Volumes = value;
			}
		}

		public long Volume
		{
			get
			{
				Volumes.Sort();
				//for (int i = 0; i < Volumes.Count; i++)
				//{
				//    if (Volumes[i] <= 0)
				//    {
				//        Volumes.RemoveAt(i);
				//    }
				//}
				return Volumes[0];
			}
		}
	}

	internal sealed class PriceVolumePair
	{
		private double _price = 0;
		private long _volume = 0;

		public PriceVolumePair(double price, long volume)
		{
			_price = price;
			_volume = volume;
		}

		public double Price
		{
			get
			{
				return Math.Round(_price, 7);
			}
		}

		public long Volume
		{
			get
			{
				return _volume;
			}
		}
	}

	internal class AutoSpreadSupport
	{
		// Used to when doing complex calculation, 0 means its not used
		private int _levelLimit = 3;
		// Used to round up or down prices for the 
		private int _roundLimit = 2;
		// Parent Ticket Handler
		private IntPtr _ticket_handler;

		#region - Properties -

		private int _bookDepthLimit = 10;
		public int BookDepthLimit
		{
			get
			{
				return _bookDepthLimit;
			}
		}

		public string PrimeSymbolDetail
		{
			get
			{
				if (ItemByLegs.ContainsKey(PrimeLeg))
				{
					return ItemByLegs[PrimeLeg].SymbolDetail;
				}
				return "";
			}
		}

		public double PrimeFactor
		{
			get
			{
				if (ItemByLegs.ContainsKey(PrimeLeg))
				{
					return ItemByLegs[PrimeLeg].Factor;
				}
				return 0;
			}
		}

		public string LongName
		{
			get
			{
				if (ItemByLegs.Count > 0)
				{
					string longName = "";
					foreach (AutoSpreadItem item in ItemByLegs.Values)
					{
						if (longName == "")
						{
							longName = longName + item.SymbolDetail;
						}
						else
						{
							longName = longName + "-" + item.SymbolDetail;
						}
					}
					return longName;
				}
				return "Auto Spread Ticket";
			}
		}

		private int _primeLeg = 0;
		public int PrimeLeg
		{
			get
			{
				return _primeLeg;
			}
		}

		public int ID
		{
			get
			{
				return LongName.GetHashCode() + (int)_ticket_handler;
				//return LongName.GetHashCode();
			}
		}

		private int _legs = 0;
		public int Legs
		{
			get
			{
				return _legs;
			}
			set
			{
				if (_legs != value)
				{
					_legs = value;
				}
			}
		}

		public double Ratio
		{
			get
			{
				// Use the last ration
				return ItemByLegs[ItemByLegs.Count - 1].Ratio;
			}
		}

		// Leg - Item
		private Dictionary<int, AutoSpreadItem> _itemByLegs;
		public Dictionary<int, AutoSpreadItem> ItemByLegs
		{
			get
			{
				if (_itemByLegs == null)
				{
					_itemByLegs = new Dictionary<int, AutoSpreadItem>();
				}
				return _itemByLegs;
			}
			set
			{
				_itemByLegs = value;
			}
		}

		private Dictionary<int, PriceVolumePair> _syntheticBidBook;
		public Dictionary<int, PriceVolumePair> SyntheticBidBook
		{
			get
			{
				if (_syntheticBidBook == null)
				{
					_syntheticBidBook = new Dictionary<int, PriceVolumePair>();
				}
				return _syntheticBidBook;
			}
		}

		// Level - Price
		private Dictionary<int, PriceVolumePair> _syntheticAskBook;
		public Dictionary<int, PriceVolumePair> SyntheticAskBook
		{
			get
			{
				if (_syntheticAskBook == null)
				{
					_syntheticAskBook = new Dictionary<int, PriceVolumePair>();
				}
				return _syntheticAskBook;
			}
		}

		private double _syntheticBidPrice = 0;
		public double SyntheticBidPrice
		{
			get
			{
				//if (SyntheticBidBook.Count > 0)
				//{
				//    _syntheticBidPrice =  SyntheticBidBook[0].Price;
				//}
				return _syntheticBidPrice;
			}
			set
			{
				_syntheticBidPrice = value;
			}
		}

		private long _syntheticBidVolume = 0;
		public long SyntheticBidVolume
		{
			get
			{
				return _syntheticBidVolume;
			}
		}

		private double _syntheticAskPrice = 0;
		public double SyntheticAskPrice
		{
			get
			{
				//if (SyntheticAskBook.Count > 0)
				//{
				//    _syntheticAskPrice = SyntheticAskBook[0].Price;
				//}
				return _syntheticAskPrice;
			}
			set
			{
				_syntheticAskPrice = value;
			}
		}

		private long _syntheticAskVolume = 0;
		public long SyntheticAskVolume
		{
			get
			{
				return _syntheticAskVolume;
			}
		}

		private double _syntheticTradedPrice = 0;
		public double SyntheticTradedPrice
		{
			get
			{
				return _syntheticTradedPrice;
			}
			set
			{
				_syntheticTradedPrice = value;
			}
		}

		private long _syntheticTradedVolume = 0;
		public long SyntheticTradedVolume
		{
			get
			{
				return _syntheticTradedVolume;
			}
		}

		// Tag - Replaced Limit Market Price
		private Dictionary<string, string> _replacedLimitMarketPrice;
		public Dictionary<string, string> ReplacedLimitMarketPrice
		{
			get
			{
				if (_replacedLimitMarketPrice == null)
				{
					_replacedLimitMarketPrice = new Dictionary<string, string>();
				}
				return _replacedLimitMarketPrice;
			}
			set
			{
				_replacedLimitMarketPrice = value;
			}
		}

		// Tag - Replaced Stop Market Price
		private Dictionary<string, string> _replacedStopMarketPrice;
		public Dictionary<string, string> ReplacedStopMarketPrice
		{
			get
			{
				if (_replacedStopMarketPrice == null)
				{
					_replacedStopMarketPrice = new Dictionary<string, string>();
				}
				return _replacedStopMarketPrice;
			}
			set
			{
				_replacedStopMarketPrice = value;
			}
		}

		#endregion

		#region - Constructor -

		public AutoSpreadSupport(int bookDepthLimit, int primeLeg, IntPtr ticket_handler)
		{
			_bookDepthLimit = bookDepthLimit;
			_primeLeg = primeLeg;
			_ticket_handler = ticket_handler;
		}

		#endregion

		#region - Get Properties -

		// Get the End Of Book Price from Local Spread Book
		public Nullable<double> EndOfBookPrice(string side)
		{
			switch (side)
			{
				case "Bid":
					if (SyntheticBidBook.Count > 0 && SyntheticBidBook.ContainsKey(SyntheticBidBook.Count - 1))
					{
						return SyntheticBidBook[SyntheticBidBook.Count - 1].Price;
					}
					break;
				case "Ask":
					if (SyntheticAskBook.Count > 0 && SyntheticAskBook.ContainsKey(SyntheticAskBook.Count - 1))
					{
						return SyntheticAskBook[SyntheticAskBook.Count - 1].Price;
					}
					break;
			}
			return null;
		}

		private bool _hasAllLegs = false;
		public bool HasAllLegs
		{
			get
			{
				if (!_hasAllLegs)
				{
					foreach (AutoSpreadItem item in ItemByLegs.Values)
					{
						if (!item.HasBooks)
						{
							return false;
						}
					}
					_hasAllLegs = true;
				}
				
				return _hasAllLegs;
			}
			set
			{
				_hasAllLegs = value;
			}
		}

		#endregion

		public void NewLimitMarketPriceFor(string tag, string newLimitMarketPrice)
		{
			lock (ReplacedLimitMarketPrice)
			{
				if (ReplacedLimitMarketPrice.ContainsKey(tag))
				{
					ReplacedLimitMarketPrice[tag] = newLimitMarketPrice;
				}
				else
				{
					ReplacedLimitMarketPrice.Add(tag, newLimitMarketPrice);
				}
			}
		}

		public void NewStopMarketPriceFor(string tag, string newStopMarketPrice)
		{
			lock (ReplacedStopMarketPrice)
			{
				if (ReplacedStopMarketPrice.ContainsKey(tag))
				{
					ReplacedStopMarketPrice[tag] = newStopMarketPrice;
				}
				else
				{
					ReplacedStopMarketPrice.Add(tag, newStopMarketPrice);
				}
			}
		}

		public Nullable<double> CalculateLegPrice(int legNumber, ROCOrder order, double spreadPrice, out string marketPrice)
		{
			marketPrice = "";

			switch (order.Side)
			{
				case CSVFieldIDs.SideCodes.Buy:
					return CalculateLegPriceFromSelectedPriceByLeg("Bid", spreadPrice, legNumber, out marketPrice);
				case CSVFieldIDs.SideCodes.Sell:
					return CalculateLegPriceFromSelectedPriceByLeg("Ask", spreadPrice, legNumber, out marketPrice);
			}
			return null;
		}
		public void CalculateLegPriceFromSelectedPrice(string side, string type, double selPrice)
		{
			string marketPrice = "";

			for (int legCount = ItemByLegs.Count - 1; legCount >= 0; legCount--)
			{
				marketPrice = "";
				switch (side)
				{
					case "Bid": // Buy
						switch (type)
						{
							case "Stop":
								ItemByLegs[legCount].BuyStopPrice = CalculateLegPriceFromSelectedPriceByLeg(side, selPrice, legCount, out marketPrice);
								break;
							default:
								ItemByLegs[legCount].BuyLimitPrice = CalculateLegPriceFromSelectedPriceByLeg(side, selPrice, legCount, out marketPrice);
								break;
						}
						ItemByLegs[legCount].BidMarketPrice = marketPrice;
						break;
					case "Ask": // Sell
						switch (type)
						{
							case "Stop":
								ItemByLegs[legCount].SellStopPrice = CalculateLegPriceFromSelectedPriceByLeg(side, selPrice, legCount, out marketPrice);
								break;
							default:
								ItemByLegs[legCount].SellLimitPrice = CalculateLegPriceFromSelectedPriceByLeg(side, selPrice, legCount, out marketPrice);
								break;
						}
						ItemByLegs[legCount].AskMarketPrice = marketPrice;
						break;
				}
			}
		}
		public Nullable<double> CalculateLegPriceFromSelectedPriceByLeg(string side, double selPrice, int legNumber, out string marketPrice)
		{
			Nullable<double> marketPriceForLeg = null;
			Nullable<double> legPrice = null;

			legPrice = CalculateLegPriceFromSelectedPriceByLeg(side, selPrice, legNumber, out marketPriceForLeg);

			if (marketPriceForLeg == null)
			{
				marketPrice = "";
			}
			else
			{
				marketPrice = ((double)marketPriceForLeg).ToString();
			}
			return legPrice;
		}
		private Nullable<double> CalculateLegPriceFromSelectedPriceByLeg(string side, double selPrice, int legNumber, out Nullable<double> marketPrice)
		{
			marketPrice = null;

			try
			{
				if (ItemByLegs.ContainsKey(legNumber))
				{
					long tickFactor = GetTickSizeToOne(ItemByLegs[legNumber].SecInfo.TickSize);
					double price = selPrice;

					foreach (AutoSpreadItem item in ItemByLegs.Values)
					{
						if (item.LegNumber != legNumber)
						{
							if (legNumber != _primeLeg && IsFlipped(ItemByLegs[_primeLeg].Factor, item.Factor))
							{
								switch (side)
								{
									case "Bid":
										if (item.MarketData.AskPrice != null)
										{
											price = price - ((double)item.MarketData.AskPrice * item.Ratio);
										}
										break;
									case "Ask":
										if (item.MarketData.BidPrice != null)
										{
											price = price - ((double)item.MarketData.BidPrice * item.Ratio);
										}
										break;
								}
							}
							else
							{
								switch (side)
								{
									case "Bid":
										if (item.MarketData.BidPrice != null)
										{
											price = price - ((double)item.MarketData.BidPrice * item.Ratio);
										}
										break;
									case "Ask":
										if (item.MarketData.AskPrice != null)
										{
											price = price - ((double)item.MarketData.AskPrice * item.Ratio);
										}
										break;
								}
							}
						}
						else
						{
							if (legNumber != _primeLeg && IsFlipped(ItemByLegs[_primeLeg].Factor, item.Factor))
							{
								switch (side)
								{
									case "Bid":
										marketPrice = item.MarketData.AskPrice;
										break;
									case "Ask":
										marketPrice = item.MarketData.BidPrice;
										break;
								}
							}
							else
							{
								switch (side)
								{
									case "Bid":
										marketPrice = item.MarketData.BidPrice;
										break;
									case "Ask":
										marketPrice = item.MarketData.AskPrice;
										break;
								}
							}
						}
					}
					switch (side)
					{
						case "Bid":
							return Math.Floor(price / ItemByLegs[legNumber].Ratio * tickFactor) / tickFactor;
						case "Ask":
							return Math.Ceiling(price / ItemByLegs[legNumber].Ratio * tickFactor) / tickFactor;
					}
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}

			return null;
		}
		private long GetTickSizeToOne(double tickSize)
		{
			return Convert.ToInt64(1 / tickSize);
		}

		public void CalculateSyntheticBookFromLegs(int[] legs, string side)
		{
			if (!ItemByLegs.ContainsKey(_primeLeg) || ItemByLegs[_primeLeg].Factor == 0)
			{
				// Can't Calculate without Prime Leg
				return;
			}
			
			Dictionary<int, LevelItems> syntheticValueByLevel = CalculateSyntheticPrimeLeg(side);
			syntheticValueByLevel = CalculateSyntheticLeg(legs, side, syntheticValueByLevel);

			if (syntheticValueByLevel.Count > 0)
			{
				switch (side)
				{
					case "Bid":
						_syntheticBidBook = CalculateSyntheticBook(side, syntheticValueByLevel, true);
						break;
					case "Ask":
						_syntheticAskBook = CalculateSyntheticBook(side, syntheticValueByLevel, true);
						break;
				}
			}
		}
		private Dictionary<int, LevelItems> CalculateSyntheticPrimeLeg(string side)
		{
			Dictionary<int, LevelItems> result = new Dictionary<int, LevelItems>();
			Dictionary<double, long> book = new Dictionary<double, long>();

			switch (side)
			{
				case "Bid":
					book = new Dictionary<double, long>(ItemByLegs[_primeLeg].MarketData.BidBooks);
					break;
				case "Ask":
					book = new Dictionary<double, long>(ItemByLegs[_primeLeg].MarketData.AskBooks);
					break;
			}

			int levelCount = 0;
			foreach (double price in book.Keys)
			{
				if (levelCount > _levelLimit)
				{
					// End of Level Limit
					break;
				}

				// Initialized the syntheticValueByLevel
				if (!result.ContainsKey(levelCount))
				{
					result.Add(levelCount, new LevelItems(new PriceVolumePair(price, book[price])));
				}

				levelCount++;
			}

			return result;
		}
		private Dictionary<int, LevelItems> CalculateSyntheticLeg(int[] legs, string side, Dictionary<int, LevelItems> syntheticValueByLevel)
		{
			Dictionary<double, long> book = new Dictionary<double, long>();

			try
			{
				foreach (int leg in legs)
				{
					if (leg != _primeLeg && ItemByLegs[leg].Factor != 0)
					{
						if (IsFlipped(ItemByLegs[_primeLeg].Factor, ItemByLegs[leg].Factor))
						{
							switch (side)
							{
								case "Bid": //Buy
									book = new Dictionary<double, long>(ItemByLegs[leg].MarketData.AskBooks);
									break;
								case "Ask": // Sell
									book = new Dictionary<double, long>(ItemByLegs[leg].MarketData.BidBooks);
									break;
							}
						}
						else
						{
							switch (side)
							{
								case "Bid": //Buy
									book = new Dictionary<double, long>(ItemByLegs[leg].MarketData.BidBooks);
									break;
								case "Ask": // Sell
									book = new Dictionary<double, long>(ItemByLegs[leg].MarketData.AskBooks);
									break;
							}
						}

						if (book.Count > 0)
						{
							int[] levels = new int[syntheticValueByLevel.Count];
							syntheticValueByLevel.Keys.CopyTo(levels, 0);

							foreach (int level in levels)
							{
								if (level > BookDepthLimit)
								{
									// End of Book DepthLimit
									syntheticValueByLevel.Remove(level);
								}
								else
								{
									int legLevelCount = 0;
									// Previous Synthetic Leg Price (basePrice to add to for each lavle of this book to add to)
									Dictionary<double, List<PriceVolumePair>> legPVs = new Dictionary<double, List<PriceVolumePair>>();
									foreach (double legPrice in book.Keys)
									{
										if (legLevelCount > _levelLimit)
										{
											// End of Level Limit
											break;
										}
										foreach (double basePrice in syntheticValueByLevel[level].Item.Keys)
										{
											// Check to see if the volume is positive or not for a given leg price
											// Important!
											if (book[legPrice] > 0)
											{
												if (!legPVs.ContainsKey(basePrice))
												{
													legPVs.Add(basePrice, new List<PriceVolumePair>());
												}
												legPVs[basePrice].Add(new PriceVolumePair(basePrice + legPrice * ItemByLegs[leg].Ratio, book[legPrice]));
											}
										}
										legLevelCount++;
									}
									foreach (double basePrice in legPVs.Keys)
									{
										syntheticValueByLevel[level].AddLeg(basePrice, legPVs[basePrice]);
									}
								}
							}
						}
						else
						{
							// Incomplet Leg Info
							syntheticValueByLevel.Clear();
							break;
						}
					}
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
				syntheticValueByLevel.Clear();
			}

			return syntheticValueByLevel;
		}
		private Dictionary<int, PriceVolumePair> CalculateSyntheticBook(string side, Dictionary<int, LevelItems> syntheticValueByLevel, bool round)
		{
			Dictionary<double, long> book = new Dictionary<double, long>();

			foreach (int level in syntheticValueByLevel.Keys)
			{
				foreach (double basePrice in syntheticValueByLevel[level].Item.Keys)
				{
					if (!book.ContainsKey(syntheticValueByLevel[level].Item[basePrice].Price))
					{
						book.Add(syntheticValueByLevel[level].Item[basePrice].Price, syntheticValueByLevel[level].Item[basePrice].Volume);
					}
				}
			}

			return CheckSyntheticBook(side, book, round);
		}
		private Dictionary<int, PriceVolumePair> CheckSyntheticBook(string side, Dictionary<double, long> syntheticBook, bool round)
		{
			Dictionary<int, PriceVolumePair> result = new Dictionary<int, PriceVolumePair>();
			Dictionary<double, long> book = new Dictionary<double, long>();

			List<double> priceList = ArrayHelper.ConvertToList(syntheticBook);
			switch (side)
			{
				case "Bid":
					priceList.Sort();
					priceList.Reverse();
					break;
				case "Ask":
					priceList.Sort();
					break;
			}

			double priceRounded = 0;
			foreach (double price in priceList)
			{
				if (book.Count > BookDepthLimit)
				{
					// End of the book limit
					break;
				}

				if (round)
				{
					priceRounded = Math.Round(price, _roundLimit);
				}
				else
				{
					priceRounded = price;
				}

				if (book.ContainsKey(priceRounded))
				{
					book[priceRounded] = Math.Min(book[priceRounded], syntheticBook[price]);
				}
				else
				{
					book.Add(priceRounded, syntheticBook[price]);
				}
			}

			// Clean Up (remove any price with volume == 0)
			priceList = ArrayHelper.ConvertToList(book);
			int levelCount = 0;
			foreach (double price in priceList)
			{
				if (book[price] > 0)
				{
					result.Add(levelCount, new PriceVolumePair(price, book[price]));
					levelCount++;
				}
			}

			return result;
		}

		public void CalculateSyntheticLevel1PricesByLegs(int[] legs)
		{
			PriceVolumePair askPV = null;
			PriceVolumePair bidPV = null;
			PriceVolumePair tradedPV = null;

			double syntheticAskPrice = 0;
			double syntheticBidPrice = 0;
			double syntheticTradedPrice = 0;

			long syntheticAskVolume = 0;
			long syntheticBidVolume = 0;
			long syntheticTradedVolume = 0;

			bidPV = new PriceVolumePair(ItemByLegs[PrimeLeg].BidPrice, CalculateSyntheticVolume(ItemByLegs[PrimeLeg].BidSize, PrimeFactor));
			askPV = new PriceVolumePair(ItemByLegs[PrimeLeg].AskPrice, CalculateSyntheticVolume(ItemByLegs[PrimeLeg].AskSize, PrimeFactor));
			tradedPV = new PriceVolumePair(ItemByLegs[PrimeLeg].TradedPrice, CalculateSyntheticVolume(ItemByLegs[PrimeLeg].TradedVolume, PrimeFactor));

			foreach (int leg in legs)
			{
				if (leg != _primeLeg && ItemByLegs[leg].Factor != 0)
				{
					if (IsFlipped(PrimeFactor, ItemByLegs[leg].Factor))
					{
						// Ask
						syntheticAskVolume = CalculateSyntheticVolume(ItemByLegs[leg].BidSize, ItemByLegs[leg].Factor);
						syntheticAskPrice = askPV.Price + (ItemByLegs[leg].BidPrice * ItemByLegs[leg].Ratio);

						// Bid
						syntheticBidVolume = CalculateSyntheticVolume(ItemByLegs[leg].AskSize, ItemByLegs[leg].Factor);
						syntheticBidPrice = bidPV.Price + (ItemByLegs[leg].AskPrice * ItemByLegs[leg].Ratio);
					}
					else
					{
						// Ask
						syntheticAskVolume = CalculateSyntheticVolume(ItemByLegs[leg].AskSize, ItemByLegs[leg].Factor);
						syntheticAskPrice = askPV.Price + (ItemByLegs[leg].AskPrice * ItemByLegs[leg].Ratio);

						// Bid
						syntheticBidVolume = CalculateSyntheticVolume(ItemByLegs[leg].BidSize, ItemByLegs[leg].Factor);
						syntheticBidPrice = bidPV.Price + (ItemByLegs[leg].BidPrice * ItemByLegs[leg].Ratio);
					}

					syntheticTradedVolume = CalculateSyntheticVolume(ItemByLegs[leg].TradedVolume, ItemByLegs[leg].Factor);
					syntheticTradedPrice = tradedPV.Price + (ItemByLegs[leg].TradedPrice * ItemByLegs[leg].Ratio);

					askPV = new PriceVolumePair(syntheticAskPrice, Math.Min(askPV.Volume, syntheticAskVolume));
					bidPV = new PriceVolumePair(syntheticBidPrice, Math.Min(bidPV.Volume, syntheticBidVolume));
					tradedPV = new PriceVolumePair(syntheticTradedPrice, Math.Min(tradedPV.Volume, syntheticTradedVolume));
				}
			}

			_syntheticBidPrice = bidPV.Price;
			_syntheticBidVolume = bidPV.Volume;

			_syntheticAskPrice = askPV.Price;
			_syntheticAskVolume = askPV.Volume;

			_syntheticTradedPrice = tradedPV.Price;
			_syntheticTradedVolume = tradedPV.Volume;
		}
		private long CalculateSyntheticVolume(long volume, double factor)
		{
			return Convert.ToInt64(Math.Floor(volume / Math.Abs(factor)));
		}

		public bool IsFlipped(double primeFactor, double legFactor)
		{
			if (primeFactor > 0)
			{
				if (legFactor < 0)
				{
					return true;
				}
			}
			else if (primeFactor < 0)
			{
				if (legFactor > 0)
				{
					return true;
				}
			}

			return false;
		}

		public Nullable<double> GetNewSpreadPrice(Dictionary<int, ROCOrder> orders, int currentLegNumber)
		{
			return GetNewSpreadPrice(orders, currentLegNumber, false);
		}
		public Nullable<double> GetNewSpreadPrice(Dictionary<int, ROCOrder> orders, int currentLegNumber, bool useStopPrice)
		{
			if (orders[_primeLeg].Side == null)
			{
				string side = GetOrderSide((long)orders[_primeLeg].Side);
				if (side != "")
				{
					double price = 0;
					if (useStopPrice)
					{
						price = orders[currentLegNumber].StopPrice * ItemByLegs[currentLegNumber].Ratio;
					}
					else
					{
						price = orders[currentLegNumber].Price * ItemByLegs[currentLegNumber].Ratio;
					}
					foreach (int leg in ItemByLegs.Keys)
					{
						if (leg != currentLegNumber && ItemByLegs[_primeLeg].Factor != 0)
						{
							switch (side)
							{
								case "Bid":
									if (IsFlipped(ItemByLegs[_primeLeg].Factor, ItemByLegs[leg].Factor))
									{
										// Opposite Side From Prime
										if (ItemByLegs[leg].MarketData.AskPrice != null)
										{
											price = price + (double)ItemByLegs[leg].MarketData.AskPrice * ItemByLegs[leg].Ratio;
										}
									}
									else
									{
										// Same Side From Prime
										if (ItemByLegs[leg].MarketData.BidPrice != null)
										{
											price = price + (double)ItemByLegs[leg].MarketData.BidPrice * ItemByLegs[leg].Ratio;
										}
									}
									break;
								case "Ask":
									if (IsFlipped(ItemByLegs[_primeLeg].Factor, ItemByLegs[leg].Factor))
									{
										// Opposite Side From Prime
										if (ItemByLegs[leg].MarketData.BidPrice != null)
										{
											price = price + (double)ItemByLegs[leg].MarketData.BidPrice * ItemByLegs[leg].Ratio;
										}
									}
									else
									{
										// Same Side From Prime
										if (ItemByLegs[leg].MarketData.AskPrice != null)
										{
											price = price + (double)ItemByLegs[leg].MarketData.AskPrice * ItemByLegs[leg].Ratio;
										}
									}
									break;
							}
						}
					}

					return price;
				}
			}

			return null;
		}		
		private string GetOrderSide(long sideCode)
		{
			switch (sideCode)
			{
				case CSVFieldIDs.SideCodes.Buy:
					return "Bid";
				case CSVFieldIDs.SideCodes.Sell:
					return "Ask";
			}

			return "";
		}
	}

	public class QuickAutoSpreadSupport : BaseTicket
	{
		private int _echoID = 0;
		public int EchoID
		{
			get
			{
				return _echoID;
			}
			set
			{
				_echoID = value;
			}
		}

		private int _primeLeg = 0;
		public int PrimeLeg
		{
			get
			{
				return _primeLeg;
			}
			set
			{
				_primeLeg = value;
			}
		}

		public double PrimeFactor
		{
			get
			{
				if (ItemByLegs.ContainsKey(PrimeLeg))
				{
					return ItemByLegs[PrimeLeg].Factor;
				}
				return 0;
			}
		}

		private Dictionary<int, AutoSpreadItem> _itemByLegs;
		public Dictionary<int, AutoSpreadItem> ItemByLegs
		{
			get
			{
				if (_itemByLegs == null)
				{
					_itemByLegs = new Dictionary<int, AutoSpreadItem>();
				}
				return _itemByLegs;
			}
			set
			{
				_itemByLegs = value;
			}
		}

		private sealed class FilterType
		{
			public const int Default = 0;
			public const int WithMarketOrder = 2;
			public const int OnlyStopOrder = 3;
		}

		private bool _showSelectedAccountOnly = false;
		public bool ShowSelectedAccountOnly
		{
			get
			{
				return _showSelectedAccountOnly;
			}
			set
			{
				_showSelectedAccountOnly = value;
			}
		}

		#region - OrderID Generator - 

		public string GetOrderID(string orderIDBase, TagKeyItems tagItems)
		{
			return GetOrderID(orderIDBase, tagItems.LegNumber, tagItems.LimitMarketPrice, tagItems.StopMarketPrice);
		}
		public string GetOrderID(string orderIDBase, string legNumber, string limitPrice, string stopPrice)
		{
			return string.Format("{0};{1};{2};{3}", new object[] { orderIDBase, legNumber, limitPrice, stopPrice });
		}

		public string GetParentTag(string orderIDBase, ParentTagKeyItems ptagItems)
		{
			return GetParentTag(orderIDBase, ptagItems.LimitSpreadPrice, ptagItems.StopSpreadPrice, ptagItems.MasterSideSpread, ptagItems.MasterQtySpread);
		}
		public string GetParentTag(string orderIDBase, string limitPrice, string stopPrice, string sideCode, string qty)
		{
			return string.Format("{0};{1};{2};{3};{4}", new object[] { orderIDBase, limitPrice, stopPrice, sideCode, qty });
		}

		#endregion

		#region - Orders -

		internal long GetOpenQty(long side)
		{
			long result = 0;

			string filter = GetOpenOrderFilter(side, FilterType.Default);
			if (filter != "")
			{
				lock (GLOBAL.HOrders.Table)
				{
					Int64.TryParse(GLOBAL.HOrders.Table.Compute("Sum(LeaveQty)", filter).ToString(), out result);
				}
			}

			return result;
		}

		internal long GetFilledQty(long side)
		{
			long result = 0;

			string filter = GetFilledOrderFilter(side);
			if (filter != "")
			{
				lock (GLOBAL.HOrders.Table)
				{
					Int64.TryParse(GLOBAL.HOrders.Table.Compute("Sum(CumQty)", filter).ToString(), out result);
				}
			}

			return result;
		}

		internal Nullable<double> GetStopLimitPrice(string side, double price)
		{
			Nullable<double> stopLimitPrice = null;

			Dictionary<string, ROCOrder> orders = new Dictionary<string, ROCOrder>();

			DataRow[] rows = new DataRow[0];
			switch (side)
			{
				case "Buy":
					rows = GetOpenOrderRows(CSVFieldIDs.SideCodes.Buy, FilterType.OnlyStopOrder, price);
					break;
				case "Sell":
					rows = GetOpenOrderRows(CSVFieldIDs.SideCodes.Sell, FilterType.OnlyStopOrder, price);
					break;
			}

			foreach (DataRow row in rows)
			{
				string tag = "";
				if (row["Tag"] != DBNull.Value && row["Tag"].ToString() != "")
				{
					tag = row["Tag"].ToString();
					if (GLOBAL.HOrders.RocItems.ContainsKey(tag) && !orders.ContainsKey(tag))
					{
						orders.Add(tag, GLOBAL.HOrders.RocItems[tag]);
					}
				}
			}

			switch (side)
			{
				case "Buy":
					foreach (ROCOrder order in orders.Values)
					{
						if (stopLimitPrice == null || (double)stopLimitPrice < order.Price)
						{
							stopLimitPrice = order.Price;
						}
					}
					break;
				case "Sell":
					foreach (ROCOrder order in orders.Values)
					{
						if (stopLimitPrice == null || (double)stopLimitPrice > order.Price)
						{
							stopLimitPrice = order.Price;
						}
					}
					break;
			}

			return stopLimitPrice;
		}

		internal Dictionary<string, ROCOrder> GetOpenOrder(string side, double price)
		{
			switch (side)
			{
				case "Buy":
					return GetOpenOrder(CSVFieldIDs.SideCodes.Buy, price);
				case "Sell":
					return GetOpenOrder(CSVFieldIDs.SideCodes.Sell, price);
			}

			return new Dictionary<string, ROCOrder>();
		}
		internal Dictionary<string, ROCOrder> GetOpenOrder(long side, Nullable<double> price)
		{
			Dictionary<string, ROCOrder> result = new Dictionary<string, ROCOrder>();

			DataRow[] rows = new DataRow[0];
			if (price == null)
			{
				rows = GetOpenOrderRows(side, FilterType.Default);
			}
			else
			{
				rows = GetOpenOrderRows(side, FilterType.Default, (double)price);
			}
			foreach (DataRow row in rows)
			{
				string tag = "";
				if (row["Tag"] != DBNull.Value && row["Tag"].ToString() != "")
				{
					tag = row["Tag"].ToString();
					if (GLOBAL.HOrders.RocItems.ContainsKey(tag) && !result.ContainsKey(tag))
					{
						result.Add(tag, GLOBAL.HOrders.RocItems[tag]);
					}
				}
			}

			return result;
		}

		internal sealed class OpenOrderItems
		{
			private Dictionary<double, long> _openOrderPriceQty;
			private List<double> _openStopOrderPrice;

			public Dictionary<double, long> OpenOrderPriceQty
			{
				get
				{
					if (_openOrderPriceQty == null)
					{
						_openOrderPriceQty = new Dictionary<double, long>();
					}
					return _openOrderPriceQty;
				}
				set
				{
					_openOrderPriceQty = value;
				}
			}

			public List<double> OpenStopOrderPrice
			{
				get
				{
					if (_openStopOrderPrice == null)
					{
						_openStopOrderPrice = new List<double>();
					}
					return _openStopOrderPrice;
				}
				set
				{
					_openStopOrderPrice = value;
				}
			}
		}

		internal OpenOrderItems GetOpenPriceQty(string side)
		{
			switch (side)
			{
				case "Buy":
					return GetOpenPriceQty(CSVFieldIDs.SideCodes.Buy);
				case "Sell":
					return GetOpenPriceQty(CSVFieldIDs.SideCodes.Sell);
					//OpenOrderItems shorts = GetOpenPriceQty(CSVFieldIDs.SideCodes.Short);
					//foreach (double price in shorts.OpenOrderPriceQty.Keys)
					//{
					//    if (result.OpenOrderPriceQty.ContainsKey(price))
					//    {
					//        result.OpenOrderPriceQty[price] = result.OpenOrderPriceQty[price] + shorts.OpenOrderPriceQty[price];
					//    }
					//    else
					//    {
					//        result.OpenOrderPriceQty.Add(price, shorts.OpenOrderPriceQty[price]);
					//    }
					//}
					//return result;
			}

			return new OpenOrderItems();
		}
		internal OpenOrderItems GetOpenPriceQty(long side)
		{
			OpenOrderItems result = new OpenOrderItems();

			bool hasPrice = false;
			double price = 0;
			long qty;

			DataRow[] rows = GetOpenOrderRows(side, FilterType.Default);

			foreach (DataRow row in rows)
			{
				hasPrice = false;

				ParentTagKeyItems pTagItems = new ParentTagKeyItems();
				if (row["ParentTag"] != DBNull.Value && row["ParentTag"].ToString() != "")
				{
					pTagItems = new ParentTagKeyItems(row["ParentTag"].ToString());
				}

				TagKeyItems tagItems = new TagKeyItems();
				if (row["Tag"] != DBNull.Value && row["Tag"].ToString() != "")
				{
					tagItems = new TagKeyItems(row["Tag"].ToString());
				}

				if (tagItems.LegNumberInt != null && (int)tagItems.LegNumberInt == PrimeLeg)
				{
					if (row["OrderType"] != DBNull.Value && row["OrderType"].ToString() != "")
					{
						switch ((long)row["OrderType"])
						{
							case CSVFieldIDs.OrderTypes.Stop:
							case CSVFieldIDs.OrderTypes.StopLimit:
								if (pTagItems.StopSpreadPriceDouble != null)
								{
									// Stop Price
									price = (double)pTagItems.StopSpreadPriceDouble;
									//if (price != (double)row["StopPrice"])
									//{
									//    price = (double)row["StopPrice"];
									//}
									hasPrice = true;
								}
								break;
							default:
								if (pTagItems.LimitSpreadPriceDouble != null)
								{
									// Limit Price
									price = (double)pTagItems.LimitSpreadPriceDouble;
									//if (price != (double)row["Price"])
									//{
									//    price = (double)row["Price"];
									//}
									hasPrice = true;
								}
								break;
						}
					}

					if (hasPrice)
					{
						if (pTagItems.MasterQtySpreadLong != null)
						{
							// LeaveQty (OriginalQty)
							qty = (long)pTagItems.MasterQtySpreadLong;
							if (qty != (long)row["LeaveQty"])
							{
								qty = (long)row["LeaveQty"];
								//GLOBAL.HROC.AddToException("row[LeaveQty] " + qty.ToString());
							}
							if (result.OpenOrderPriceQty.ContainsKey(price))
							{
								result.OpenOrderPriceQty[(double)price] = result.OpenOrderPriceQty[(double)price] + qty;
								// Debug
								//GLOBAL.HROC.AddToException("OpenOrderPriceQty add price " + ((double)price).ToString() + " qty " + qty.ToString());
							}
							else
							{
								result.OpenOrderPriceQty.Add((double)price, qty);
								// Debug
								//GLOBAL.HROC.AddToException("OpenOrderPriceQty add price " + ((double)price).ToString() + " qty " + qty.ToString());
							}
						}
					}
				}
			}

			return result;
		}

		private DataRow[] GetOpenOrderRows(long side, int filterType)
		{
			if (CurrentSecInfo != null && EchoID != 0)
			{
				string filter = GetOpenOrderFilter(side, filterType);
				// Debug
				//GLOBAL.HROC.AddToException("GetOpenOrderRows " + filter);
				if (filter != "")
				{
					lock (GLOBAL.HOrders.Table)
					{
						return GLOBAL.HOrders.Table.Select(filter);
					}
				}
			}

			return new DataRow[0];
		}
		private DataRow[] GetOpenOrderRows(long side, int filterType, double price)
		{
			if (CurrentSecInfo != null && EchoID != 0)
			{
				string filter = GetOpenOrderFilter(side, filterType, price);
				if (filter != "")
				{
					lock (GLOBAL.HOrders.Table)
					{
						return GLOBAL.HOrders.Table.Select(filter);
					}
				}
			}

			return new DataRow[0];
		}

		private string GetOpenOrderFilter(long side, int filterType)
		{
			string result = "Echo = '" + EchoID.ToString() + "'";
			switch (side)
			{
				case CSVFieldIDs.SideCodes.Buy:
					result = result + string.Format(" And ({0})", CSVFieldIDs.SideCodes.BuySideFilter);
					break;
				case CSVFieldIDs.SideCodes.Sell:
				case CSVFieldIDs.SideCodes.Short:
					result = result + string.Format(" And ({0})", CSVFieldIDs.SideCodes.SellSideFilter);
					break;
				default:
					return "";
			}
			result = result + string.Format(" And ({0})", CSVFieldIDs.StatusCodes.ActiveStatusFilter);
			result = result + " And LeaveQty > 0";

			if (ShowSelectedAccountOnly && DDAccountDropDown.CurrentAcctountInfo != null)
			{
				result = result + string.Format(" And ClearingAcct = '{0}'", DDAccountDropDown.CurrentAcctountInfo.clearingAcID);
			}

			switch (filterType)
			{
				case FilterType.WithMarketOrder:
					break;
				case FilterType.OnlyStopOrder:
					result = result + string.Format(" And ({0})", CSVFieldIDs.OrderTypes.StopOrderOnlyFilter);
					break;
				case FilterType.Default:
					// Non Market Order Only
					result = result + string.Format(" And ({0})", CSVFieldIDs.OrderTypes.NonMarketOrderOnlyFilter);
					break;
			}
			return result;
		}
		private string GetOpenOrderFilter(long side, int filterType, double price)
		{
			string result = GetOpenOrderFilter(side, filterType);

			return result + string.Format(" And (Price = '{0}' Or StopPrice = '{0}')", price);
		}

		private string GetFilledOrderFilter(long side)
		{
			string result = "Echo = '" + EchoID.ToString() + "'";
			switch (side)
			{
				case CSVFieldIDs.SideCodes.Buy:
					result = result + string.Format(" And ({0})", CSVFieldIDs.SideCodes.BuySideFilter);
					break;
				case CSVFieldIDs.SideCodes.Sell:
				case CSVFieldIDs.SideCodes.Short:
					result = result + string.Format(" And ({0})", CSVFieldIDs.SideCodes.SellSideFilter);
					break;
				default:
					return "";
			}
			result = result + " And CumQty > 0";

			if (ShowSelectedAccountOnly && DDAccountDropDown.CurrentAcctountInfo != null)
			{
				result = result + string.Format(" And ClearingAcct = '{0}'", DDAccountDropDown.CurrentAcctountInfo.clearingAcID);
			}

			return result;
		}

		#endregion
	}
}
