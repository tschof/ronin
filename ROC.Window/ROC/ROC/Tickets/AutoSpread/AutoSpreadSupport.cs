using System;
using System.Collections.Generic;

using Common;
using MarketData;
using RDSEx;
using CSVEx;

using Price = Common.Price;
using AutoSpreadItem = ROC.Tickets.AutoSpread.AutoSpreadItem;
using IndexLegCollection = Common.PairCollection<int, ROC.Tickets.AutoSpread.AutoSpreadItem>;
using IndexQuoteCollection = Common.PairCollection<int, MarketData.Quote>;

namespace ROC
{
	internal class AutoSpreadSupport
	{
		#region - Private Types -

		// keep the one price and all the volume for this price
		private sealed class LevelItem
		{
			private Price _price = 0;

			public LevelItem(Price price, long volume)
			{
				AddLeg(price, volume, new long[0]);
			}

			public LevelItem(Price price, long volume, IEnumerable<long> volumes)
			{
				AddLeg(price, volume, volumes);
			}

			public void AddLeg(Price price, long volume, IEnumerable<long> volumes)
			{
				_price = _price + price;
				Volumes.Add(volume);
				if (volumes != null)
					Volumes.AddRange(volumes);
			}

			public Price Price => _price.Round(7);

			private List<long> _volumes;
			public List<long> Volumes {
				get {
					if (_volumes == null) {
						_volumes = new List<long>();
					}
					return _volumes;
				}
				set {
					Volumes = value;
				}
			}

			public long Volume {
				get {
					Volumes.Sort();
					return Volumes[0];
				}
			}
		}

		private class LevelItemCollection : Common.PairCollection<Price, LevelItem>
		{
			public void AddLeg(Price basePrice, List<Quote> pvs)
			{
				if (TryGetValue(basePrice, out LevelItem basePriceItem)) {
					foreach (Quote q in pvs) {
						if (FindOrAdd(q.QuotePrice, out LevelItem quotePriceItem, () => new LevelItem(q.QuotePrice, q.Size)))
							quotePriceItem.Volumes.Add(q.Size);
					}
				}
				Remove(basePrice);
			}
		}
		#endregion // - Private Types -

		// Used to when doing complex calculation, 0 means its not used
		private int _levelLimit = 3;
		// Used to round up or down prices for the 
		private int _roundLimit = 2;
		// Parent Ticket Handler
		private IntPtr _ticket_handler;

		private IndexLegCollection _legs = new IndexLegCollection();

		public int BookDepthLimit { get; private set; } = 10;

		public int ID {
			get {
				return LongName.GetHashCode() + (int)_ticket_handler;
				//return LongName.GetHashCode();
			}
		}

		public double Ratio => _legs.TryGetBack(out var last) ? last.Ratio : 1;

		private IndexQuoteCollection _syntheticBidBook;
		public IndexQuoteCollection SyntheticBidBook
		{
			get
			{
				if (_syntheticBidBook == null)
				{
					_syntheticBidBook = new IndexQuoteCollection();
				}
				return _syntheticBidBook;
			}
		}

		// Level - Price
		private IndexQuoteCollection _syntheticAskBook;
		public IndexQuoteCollection SyntheticAskBook
		{
			get
			{
				if (_syntheticAskBook == null)
				{
					_syntheticAskBook = new IndexQuoteCollection();
				}
				return _syntheticAskBook;
			}
		}

		public Price SyntheticBidPrice = 0;
		public long SyntheticBidVolume { get; private set; } = 0;
		public Price SyntheticAskPrice = 0;
		public long SyntheticAskVolume { get; private set; } = 0;
		public Price SyntheticTradedPrice = 0;
		public long SyntheticTradedVolume { get; private set; } = 0;
		public bool EquityOnly => HasAllOriginInfo;

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

		#region - Constructor -

		public AutoSpreadSupport(int bookDepthLimit, int primeLeg, IntPtr ticket_handler)
		{
			BookDepthLimit = bookDepthLimit;
			PrimeLegNumber = primeLeg;
			_ticket_handler = ticket_handler;
		}

		#endregion

		#region - Legs -

		public int PrimeLegNumber { get; private set; } = 0;

		public bool TryGetPrimeLeg(out AutoSpreadItem legItem) => TryGetLeg(PrimeLegNumber, out legItem);

		public double PrimeFactor => TryGetPrimeLeg(out var primeLeg) ? primeLeg.Factor : 0;

		internal IndexLegCollection CloneLegCollection() => new IndexLegCollection(_legs);

		internal IndexLegCollection.Enumerator GetLegEnumerator() => _legs.GetEnumerator();

		internal void ClearLegs() => _legs.Clear();

		internal AutoSpreadItem FindOrAddLeg(int legNumber, int bookDepthLimit)
		{
			bool found = _legs.FindOrAdd(legNumber, out var value, () => new AutoSpreadItem(legNumber, bookDepthLimit));
			if (!found)
				_longName = null; // Added a leg; force rebuild of _longName.
			return value;
		}

		internal int LegCount => _legs.Count;

		internal bool TryGetLeg(int key, out AutoSpreadItem found) => _legs.TryGetValue(key, out found);

		private string _longName = null;
		public string LongName {
			get {
				if (_longName == null) {
					if (_legs.Count > 0) {
						_longName = string.Join("-", _legs.Select(n => n.SymbolDetail));
					} else {
						_longName = "Auto Spread Ticket";
					}
				}
				return _longName;
			}
		}

		private bool _hasAllLegs = false;
		public bool HasAllLegs {
			get {
				if (!_hasAllLegs)
					_hasAllLegs = !_legs.Exists(n => !n.HasBooks);
				return _hasAllLegs;
			}
		}

		public bool HasAllOriginInfo => !_legs.Exists(n => !n.HasOriginInfo);

		#endregion // - Legs -

		// Get the End Of Book Price from Local Spread Book
		public bool TryGetEndOfBookPrice(string side, out Price value)
		{
			switch (side)
			{
				case "Bid":
					if ((SyntheticBidBook.Count > 0) && SyntheticBidBook.TryGetBack(out var quote))
					{
						value = quote.QuotePrice;
						return true;
					}
					break;
				case "Ask":
					if ((SyntheticAskBook.Count > 0) && SyntheticAskBook.TryGetBack(out quote))
					{
						value = quote.QuotePrice;
						return true;
					}
					break;
			}
			value = 0;
			return false;
		}

		public void NewLimitMarketPriceFor(string tag, string newLimitMarketPrice)
		{
			lock (ReplacedLimitMarketPrice)
			{
				ReplacedLimitMarketPrice[tag] = newLimitMarketPrice;
			}
		}

		public void NewStopMarketPriceFor(string tag, string newStopMarketPrice)
		{
			lock (ReplacedStopMarketPrice)
			{
				ReplacedStopMarketPrice[tag] = newStopMarketPrice;
			}
		}

		public double? CalculateLegPrice(AutoSpreadItem legItem, ROCOrder order, double spreadPrice, out string marketPrice)
		{
			marketPrice = "";

			switch (order.Side)
			{
				case CSVFieldIDs.SideCodes.Buy:
					return CalculateLegPriceFromSelectedPriceByLeg("Bid", spreadPrice, legItem, out marketPrice);
				case CSVFieldIDs.SideCodes.Sell:
					return CalculateLegPriceFromSelectedPriceByLeg("Ask", spreadPrice, legItem, out marketPrice);
			}
			return null;
		}

		public void CalculateLegPriceFromSelectedPrice(string side, string type, double selPrice)
		{
			string marketPrice = "";

			foreach ((int _, AutoSpreadItem legItem) in _legs)
			{
				switch (side) {
					case "Bid": // Buy
						switch (type) {
							case "Stop":
								legItem.BuyStopPrice = CalculateLegPriceFromSelectedPriceByLeg(side, selPrice, legItem, out marketPrice);
								break;
							default:
								legItem.BuyLimitPrice = CalculateLegPriceFromSelectedPriceByLeg(side, selPrice, legItem, out marketPrice);
								break;
						}
						legItem.BidMarketPrice = marketPrice;
						break;
					case "Ask": // Sell
						switch (type) {
							case "Stop":
								legItem.SellStopPrice = CalculateLegPriceFromSelectedPriceByLeg(side, selPrice, legItem, out marketPrice);
								break;
							default:
								legItem.SellLimitPrice = CalculateLegPriceFromSelectedPriceByLeg(side, selPrice, legItem, out marketPrice);
								break;
						}
						legItem.AskMarketPrice = marketPrice;
						break;
				}
			}
		}

		public double? CalculateLegPriceFromSelectedPriceByLeg(string side, double selPrice, AutoSpreadItem legItem, out string marketPrice)
		{
			double? marketPriceForLeg;
			double? legPrice;

			legPrice = CalculateLegPriceFromSelectedPriceByLeg(side, selPrice, legItem, out marketPriceForLeg);

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

		private double? CalculateLegPriceFromSelectedPriceByLeg(string side, double selPrice, AutoSpreadItem selectedItem, out double? marketPrice)
		{
			marketPrice = null;

			try
			{
				if (selectedItem != null)
				{
					long tickFactor = GetTickSizeToOne(selectedItem.SecInfo.TickSize);
					double price = selPrice;
					int selectedLegNumber = selectedItem.LegNumber;

					if (TryGetPrimeLeg(out var primeLeg)) {
						foreach ((int _, AutoSpreadItem legItem) in _legs) {
							bool flipped = (selectedLegNumber != PrimeLegNumber) && IsFlipped(primeLeg.Factor, legItem.Factor);
							Book.FieldEnum fieldId = (side == "Bid") || flipped ? Book.FieldEnum.BidPrice : Book.FieldEnum.AskPrice;

							if (legItem.LegNumber != selectedLegNumber) {
								if (legItem.MarketData.TryGetField(Book.FieldEnum.BidPrice, out double topPrice))
									price -= topPrice * legItem.Ratio;
							} else {
								if (legItem.MarketData.TryGetField(Book.FieldEnum.BidPrice, out double topPrice))
									marketPrice = topPrice;
							}
						}
					}

					switch (side)
					{
						case "Bid":
							return Math.Floor(price / selectedItem.Ratio * tickFactor) / tickFactor;
						case "Ask":
							return Math.Ceiling(price / selectedItem.Ratio * tickFactor) / tickFactor;
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

		public void CalculateSyntheticBookFromLegs(string side)
		{
			if (!TryGetPrimeLeg(out var primeLeg) || (primeLeg.Factor == 0))
			{
				// Can't Calculate without Prime Leg
				return;
			}
			
			Dictionary<int, LevelItemCollection> syntheticValueByLevel = CalculateSyntheticPrimeLeg(primeLeg, side);
			syntheticValueByLevel = CalculateSyntheticLeg(primeLeg, side, syntheticValueByLevel);

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
		private Dictionary<int, LevelItemCollection> CalculateSyntheticPrimeLeg(AutoSpreadItem primeLeg, string side)
		{
			QuoteCollection book;

			switch (side)
			{
				case "Bid":
					book = primeLeg.MarketData.Bids;
					break;
				case "Ask":
					book = primeLeg.MarketData.Asks;
					break;
				default:
					return null;
			}

			int levelCount = 0;
			Dictionary<int, LevelItemCollection> result = new Dictionary<int, LevelItemCollection>();

			foreach (Quote quote in book)
			{
				if (levelCount > _levelLimit)
				{
					// End of Level Limit
					break;
				}

				// Initialized the syntheticValueByLevel
				if (!result.ContainsKey(levelCount))
				{
					var items = new LevelItemCollection();
					items.Set(quote.QuotePrice, new LevelItem(quote.QuotePrice, quote.Size), true);
					result.Add(levelCount, items);
				}

				levelCount++;
			}

			return result;
		}

		// Dictionary<int, LevelItems>
		private Dictionary<int, LevelItemCollection> CalculateSyntheticLeg(AutoSpreadItem primeLeg, string side, Dictionary<int, LevelItemCollection> syntheticValueByLevel)
		{
			try {
				foreach ((int legNumber, AutoSpreadItem legItem) in _legs) {
					if (legNumber != PrimeLegNumber && legItem.Factor != 0)
					{
						bool sideFlag = (side == "Bid") || IsFlipped(primeLeg.Factor, legItem.Factor); // true = bid, false = ask
						QuoteCollection quotes = sideFlag ? legItem.MarketData.Bids : legItem.MarketData.Asks;


						if (quotes.Count > 0)
						{
							foreach ((int levelIndex, LevelItemCollection levelItems) in syntheticValueByLevel) {
								if (levelIndex > BookDepthLimit) {
									// End of Book DepthLimit
									syntheticValueByLevel.Remove(levelIndex);
									continue;
								}

								int legLevelCount = 0;
								// Previous Synthetic Leg Price (basePrice to add to for each lavle of this book to add to)
								Dictionary<Price, List<Quote>> legPVs = new Dictionary<Price, List<Quote>>();
								foreach (Quote quote in quotes) {
									if (legLevelCount > _levelLimit) {
										// End of Level Limit
										break;
									}

									foreach (var entry in levelItems) {
										// Check to see if the volume is positive or not for a given leg price
										// Important!
										Price basePrice = entry.Key;
										if (quote.Size > 0) {
											if (!legPVs.TryGetValue(basePrice, out var list)) {
												list = new List<Quote>();
												legPVs.Add(basePrice, list);
											}
											list.Add(new Quote(basePrice + quote.QuotePrice.Value * legItem.Ratio, quote.Size));
										}
									}
									legLevelCount++;
								}

								foreach (Price basePrice in legPVs.Keys) {
									levelItems.AddLeg(basePrice, legPVs[basePrice]);
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

		private IndexQuoteCollection CalculateSyntheticBook(string side, Dictionary<int, LevelItemCollection> syntheticValueByLevel, bool round)
		{
			QuoteCollection book = new QuoteCollection();
			foreach ((Price _, LevelItemCollection items) in syntheticValueByLevel) {
				foreach ((Price price, LevelItem item) in items) {
					book.SetLevel(price, item.Volume);
				}
			}

			return CheckSyntheticBook(side, book, round);
		}

		private IndexQuoteCollection CheckSyntheticBook(string side, QuoteCollection syntheticBook, bool round)
		{
			QuoteCollection book = new QuoteCollection();
			QuoteCollection copy = new QuoteCollection(syntheticBook);

			if (side == "Ask") {
				copy.SortLevels(Price.SortOrder.Ask, BookDepthLimit, Price.UnsetPrice);
			} else if (side == "Bid") {
				copy.SortLevels(Price.SortOrder.Bid, BookDepthLimit, Price.UnsetPrice);
			}

			double priceRounded;
			foreach (Quote quote in copy)
			{
				priceRounded = round ? Math.Round(quote.QuotePrice.Value, _roundLimit) : quote.QuotePrice.Value;
				book.SetLevel(priceRounded, quote.Size);
			}

			// Clean Up (remove any price with volume == 0)
			IndexQuoteCollection result = new IndexQuoteCollection();
			int levelCount = 0;

			foreach (Quote quote in book) {
				if (quote.Size > 0) {
					result.Set(levelCount, new Quote(quote), true);
					++levelCount;
				}
			}

			return result;
		}

		public void CalculateSyntheticLevel1PricesByLegs(AutoSpreadItem primeLeg)
		{
			Quote askPV;
			Quote bidPV;
			Quote tradedPV;

			Price syntheticAskPrice = 0;
			Price syntheticBidPrice = 0;
			Price syntheticTradedPrice = 0;

			long syntheticAskVolume = 0;
			long syntheticBidVolume = 0;
			long syntheticTradedVolume = 0;

			bidPV = new Quote(primeLeg.BidPrice, CalculateSyntheticVolume(primeLeg.BidSize, primeLeg.Factor));
			askPV = new Quote(primeLeg.AskPrice, CalculateSyntheticVolume(primeLeg.AskSize, primeLeg.Factor));
			tradedPV = new Quote(primeLeg.TradedPrice, CalculateSyntheticVolume(primeLeg.TradedVolume, primeLeg.Factor));

			foreach ((int legNumber, AutoSpreadItem legItem) in _legs)
			{
				if (legNumber != PrimeLegNumber && legItem.Factor != 0)
				{
					if (IsFlipped(primeLeg.Factor, legItem.Factor))
					{
						// Ask
						syntheticAskVolume = CalculateSyntheticVolume(legItem.BidSize, legItem.Factor);
						syntheticAskPrice = askPV.QuotePrice + (legItem.BidPrice * legItem.Ratio);

						// Bid
						syntheticBidVolume = CalculateSyntheticVolume(legItem.AskSize, legItem.Factor);
						syntheticBidPrice = bidPV.QuotePrice + (legItem.AskPrice * legItem.Ratio);
					}
					else
					{
						// Ask
						syntheticAskVolume = CalculateSyntheticVolume(legItem.AskSize, legItem.Factor);
						syntheticAskPrice = askPV.QuotePrice + (legItem.AskPrice * legItem.Ratio);

						// Bid
						syntheticBidVolume = CalculateSyntheticVolume(legItem.BidSize, legItem.Factor);
						syntheticBidPrice = bidPV.QuotePrice + (legItem.BidPrice * legItem.Ratio);
					}

					syntheticTradedVolume = CalculateSyntheticVolume(legItem.TradedVolume, legItem.Factor);
					syntheticTradedPrice = tradedPV.QuotePrice + (legItem.TradedPrice * legItem.Ratio);

					askPV = new Quote(syntheticAskPrice, Math.Min(askPV.Size, syntheticAskVolume));
					bidPV = new Quote(syntheticBidPrice, Math.Min(bidPV.Size, syntheticBidVolume));
					tradedPV = new Quote(syntheticTradedPrice, Math.Min(tradedPV.Size, syntheticTradedVolume));
				}
			}

			SyntheticBidPrice = bidPV.QuotePrice;
			SyntheticBidVolume = bidPV.Size;

			SyntheticAskPrice = askPV.QuotePrice;
			SyntheticAskVolume = askPV.Size;

			SyntheticTradedPrice = tradedPV.QuotePrice;
			SyntheticTradedVolume = tradedPV.Size;
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

		public double? GetNewSpreadPrice(Dictionary<int, ROCOrder> orders, int currentLegNumber)
		{
			return GetNewSpreadPrice(orders, currentLegNumber, false);
		}

		public double? GetNewSpreadPrice(Dictionary<int, ROCOrder> orders, int currentLegNumber, bool useStopPrice)
		{
			if (orders.TryGetValue(PrimeLegNumber, out ROCOrder primeOrder) && primeOrder.HasSide)
			{
				string side = GetOrderSide(primeOrder.Side);
				if ((side != "") && TryGetLeg(currentLegNumber, out var currentLegItem) && orders.TryGetValue(currentLegNumber, out var currentOrder))
				{
					double price = (useStopPrice ? currentOrder.StopPrice : currentOrder.OrderPrice.Value) * currentLegItem.Ratio;

					if (TryGetPrimeLeg(out var primeLeg)) {
						foreach (KeyValuePair<int, AutoSpreadItem> entry in _legs) {
							int legNumber = entry.Key;
							AutoSpreadItem legItem = entry.Value;
							if ((legNumber != currentLegNumber) && (legItem.Factor != 0)) {
								bool flipped = IsFlipped(primeLeg.Factor, legItem.Factor);
								Book.FieldEnum fieldId = ((side == "Bid") || flipped) ? Book.FieldEnum.BidPrice : Book.FieldEnum.AskPrice;

								if (legItem.MarketData.TryGetField(fieldId, out double topPrice))
									price += topPrice * legItem.Ratio;
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

}
