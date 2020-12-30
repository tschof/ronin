using System.Collections.Generic;
using System.Runtime.Serialization;
using DateTime = System.DateTime;
using Price = Common.Price;

namespace MarketData.Ronin
{
	internal class MDServerToClient
	{
		private List<Quote> _asks;
		private List<Quote> _bids;
		private List<Quote> _volumes;
		private DictionaryEx.MultiTypedDictionary _fields;

		internal List<Quote> BidBooks { get { return _bids; } }
		internal List<Quote> AskBooks { get { return _asks; } }
		internal List<Quote> TradedVolumes { get { return _volumes; } }

		private static string[] TimeFormats { get; } = new string[] { "HH:mm:ss.fff" };

		internal long Count {
			get {
				return _fields.Count + AskBooks.Count + BidBooks.Count;
			}
		}

		internal MDServerToClient()
		{
			_asks = new List<Quote>();
			_bids = new List<Quote>();
			_volumes = new List<Quote>();
			_fields = new DictionaryEx.MultiTypedDictionary();
		}

		internal MDServerToClient(MDServerToClient locScr, bool fast = false)
		{
			_asks = new List<Quote>(locScr.AskBooks);
			_bids = new List<Quote>(locScr.BidBooks);
			_volumes = new List<Quote>(locScr.TradedVolumes);
			_fields = new DictionaryEx.MultiTypedDictionary(locScr._fields);
		}

		internal bool TryGet(int key, out string value)
		{
			return _fields.TryGet(key, out value);
		}

		internal bool TryGet(int key, out double value)
		{
			return _fields.TryGet(key, out value);
		}

		internal bool TryGet(int key, out long value)
		{
			return _fields.TryGet(key, out value);
		}

		internal bool TryGet(int key, out bool value)
		{
			return _fields.TryGet(key, out value);
		}

		internal bool Remove(double key)
		{
			bool changedBids = false, changedAsks = false;
			Price priceKey = new Price(key);

			lock (_bids) {
				changedBids = _bids.RemoveAll(n => n.QuotePrice == priceKey) > 0;
			}

			lock (_asks) {
				changedAsks = _asks.RemoveAll(n => n.QuotePrice == priceKey) > 0;
			}

			return changedBids || changedAsks;
		}

		internal bool Merge(MDServerToClient other, bool fast = false)
		{
			bool changed = _fields.Merge(other._fields);
			copyOrReplace(ref _bids, other.BidBooks, fast);
			copyOrReplace(ref _asks, other.AskBooks, fast);
			copyOrReplace(ref _volumes, other.TradedVolumes, fast);
			return changed;
		}

		internal void Clear()
		{
			_fields.Clear();
			lock (_bids) {
				_bids.Clear();
			}
			lock (_asks) {
				_asks.Clear();
			}
			lock (_volumes) {
				_volumes.Clear();
			}
		}

		internal void VisitFields(DictionaryEx.MultiTypedDictionary.IVisitor visitor)
		{
			_fields.Visit(visitor);
		}

		internal bool SetBid(double key, long val, bool updateIfExists = false)
		{
			return update(_bids, key, val, updateIfExists);
		}

		internal bool SetAsk(double key, long val, bool updateIfExists = false)
		{
			return update(_asks, key, val, updateIfExists);
		}

		internal void Set<T>(int key, T val)
		{
			_fields.Set(key, val, true);
		}

		internal bool SetTradedVolume(double key, long val, bool reset = false)
		{
			Price priceKey = new Price(key);
			lock (_volumes) {
				int index = _volumes.FindIndex(n => n.QuotePrice == priceKey);
				if (index < 0) {
					_volumes.Add(new Quote(priceKey, val));
				} else if (reset) {
					_volumes[index].SetSize(val);
				} else {
					_volumes[index].AddSize(val);
				}
			}
			return true;
		}

		private bool update(List<Quote> collection, double key, long val, bool updateIfExists)
		{
			bool changed = false;
			Price priceKey = new Price(key);

			lock (collection) {
				int index = collection.FindIndex(n => n.QuotePrice == priceKey);
				if (index < 0) {
					collection.Add(new Quote(key, val));
					changed = true;
				} else if (updateIfExists && (val != collection[index].Size)) {
					collection[index].SetSize(val);
					changed = true;
				}
			}
			return changed;
		}

		private void copyOrReplace(ref List<Quote> target, List<Quote> source, bool clobber)
		{
			if ((target == null) || (source == null)) {
				// Nothing to merge.
			} else if (clobber) {
				target = new List<Quote>(source);
			} else {
				lock (target) {
					lock (source) {
						foreach (Quote q in source) {
							int i = target.FindIndex(n => n.QuotePrice == q.QuotePrice);
							if (i < 0)
								target.Add(q);
							else
								target[i] = q;
						}
					}
				}
			}
		}

		#region - Named Fields -

		internal string IssueSymbol {
			get {
				string val;
				if (_fields.TryGet(WombatFieldIDs.IssueSymbol, out val))
					return val;
				if (_fields.TryGet(WombatFieldIDs.IndexSymbol, out val))
					return val;
				return null;
			}
		}

		internal string uSymbol => _fields.TryGet(WombatFieldIDs.uSymbol, out string val) ? val : null;
		internal string uSubscriptionSource => _fields.TryGet(WombatFieldIDs.SubscriptionSoruce, out string val) ? val : null;
		internal string uSubscriptionSymbol => _fields.TryGet(WombatFieldIDs.SubscriptionSymbol, out string val) ? val : null;
		internal string uSubscriptionKey => string.Format("{0},{1}", uSubscriptionSymbol ?? "?", uSubscriptionSource ?? "?");
		internal string uParticipantSymbol => _fields.TryGet(WombatFieldIDs.uParticipentSymbol, out string val) ? val : null;
		internal string uParticipentName => _fields.TryGet(WombatFieldIDs.uParticipentName, out string val) ? val : null;
		internal string InstrumentType => _fields.TryGet(WombatFieldIDs.InstrumentType, out string val) ? val : null;
		internal string LineTime => _fields.TryGet(WombatFieldIDs.LineTime, out string val) ? val : null;
		internal string PrevCloseDate => _fields.TryGet(WombatFieldIDs.PrevCloseDate, out string val) ? val : null;
		internal string BidPartId => _fields.TryGet(WombatFieldIDs.BidPartId, out string val) ? val : null;
		internal string AskPartId => _fields.TryGet(WombatFieldIDs.AskPartId, out string val) ? val : null;
		internal string QuoteDate => _fields.TryGet(WombatFieldIDs.QuoteDate, out string val) ? val : null;
		internal string QuoteTime => _fields.TryGet(WombatFieldIDs.QuoteTime, out string val) ? val : null;
		internal string TradeDate => _fields.TryGet(WombatFieldIDs.TradeDate, out string val) ? val : null;
		internal string TradeTime => _fields.TryGet(WombatFieldIDs.TradeTime, out string val) ? val : null;
		internal string TradeTick => _fields.TryGet(WombatFieldIDs.TradeTick, out string val) ? val : null;
		internal string TradePartId => _fields.TryGet(WombatFieldIDs.TradePartId, out string val) ? val : null;
		internal string PrimExch => _fields.TryGet(WombatFieldIDs.PrimExch, out string val) ? val : null; // Stock L1
		internal string SecurityStatus => _fields.TryGet(WombatFieldIDs.TradePartId, out string val) ? val : "NONE";
		internal string SecurityCondition => _fields.TryGet(WombatFieldIDs.Condition, out string val) ? val : "";
		internal string ProductInd => _fields.TryGet(WombatFieldIDs.ProductInd, out string val) ? val : null; // Future L1
		internal string MaxPriceVariation => _fields.TryGet(WombatFieldIDs.MaxPriceVariation, out string val) ? val : null;
		internal string Maturity => _fields.TryGet(WombatFieldIDs.Maturity, out string val) ? val : null;
		internal string MainFraction => _fields.TryGet(WombatFieldIDs.MainFraction, out string val) ? val : null;
		internal string SettleDate => _fields.TryGet(WombatFieldIDs.SettleDate, out string val) ? val : null;
		internal string PutCall => _fields.TryGet(WombatFieldIDs.PutCall, out string val) ? val : null; // Option L1
		internal string ExpirationDate => _fields.TryGet(WombatFieldIDs.ExpirationDate, out string val) ? val : null;
		internal string PrevAskCloseDate => _fields.TryGet(WombatFieldIDs.PrevAskCloseDate, out string val) ? val : null;
		internal string PrevBidCloseDate => _fields.TryGet(WombatFieldIDs.PrevBidCloseDate, out string val) ? val : null;
		internal string BidCloseDate => _fields.TryGet(WombatFieldIDs.BidCloseDate, out string val) ? val : null;
		internal string AskCloseDate => _fields.TryGet(WombatFieldIDs.AskClosedate, out string val) ? val : null;

		internal double? NetChange => getDouble(WombatFieldIDs.NetChange);
		internal double? PctChange => getDouble(WombatFieldIDs.PctChange);
		internal double? uDecodedTime => getDouble(WombatFieldIDs.uDecodedTime);
		internal double? HighPrice => getFirstDouble(WombatFieldIDs.HighPrice, WombatFieldIDs.IndexHigh);
		internal double? LowPrice => getFirstDouble(WombatFieldIDs.LowPrice, WombatFieldIDs.IndexLow);
		internal double? OpenPrice => getDouble(WombatFieldIDs.OpenPrice);
		internal double? ClosePrice => getDouble(WombatFieldIDs.ClosePrice);
		internal double? PrevClosePrice => getDouble(WombatFieldIDs.PrevClosePrice);
		internal double? AdjPrevClosePrice => getDouble(WombatFieldIDs.AdjPrevClosePrice);
		internal double? TotalValue => getDouble(WombatFieldIDs.TotalValue);
		internal double? Vwap => getDouble(WombatFieldIDs.Vwap);
		internal double? BidPrice => getFirstDouble(WombatFieldIDs.BidPrice, WombatFieldIDs.IndexBid);
		internal double? BidLow => getDouble(WombatFieldIDs.BidLow);
		internal double? BidHigh => getDouble(WombatFieldIDs.BidHigh);
		internal double? AskPrice => getFirstDouble(WombatFieldIDs.AskPrice, WombatFieldIDs.IndexAsk);
		internal double? AskLow => getDouble(WombatFieldIDs.AskLow);
		internal double? AskHigh => getDouble(WombatFieldIDs.AskHigh);
		internal double? TradePrice => getFirstDouble(WombatFieldIDs.TradePrice, WombatFieldIDs.IndexValue);
		internal double? LastBestAskPrice => getDouble(WombatFieldIDs.LastBestAskPrice);
		internal double? LastBestBidPrice => getDouble(WombatFieldIDs.LastBestBidPrice);
		internal double? DisplayConversionFactor => getDouble(WombatFieldIDs.DisplayConversionFactor);
		internal double? TicketIncrement => getDouble(WombatFieldIDs.TicketIncrement);
		internal double? HightLimit => getDouble(WombatFieldIDs.HighLimit);
		internal double? LowLimit => getDouble(WombatFieldIDs.LowLimit);
		internal double? SettlePrice => getDouble(WombatFieldIDs.SettlePrice);
		internal double? StrikePrice => getDouble(WombatFieldIDs.StrikePrice);
		internal double? PrevBidClose => getDouble(WombatFieldIDs.PrevBidClose);
		internal double? BidClose => getDouble(WombatFieldIDs.BidClose);
		internal double? PrevAskClose => getDouble(WombatFieldIDs.PrevAskClose);
		internal double? AskClose => getDouble(WombatFieldIDs.AskClose);

		internal long? TotalVolume => getLong(WombatFieldIDs.TotalVolume);
		internal long? BidSize => getLong(WombatFieldIDs.BidSize);
		internal long? AskSize => getLong(WombatFieldIDs.AskSize);
		internal long? QuoteCount => getLong(WombatFieldIDs.QuoteCount);
		internal long? TradeVolume => getLong(WombatFieldIDs.TradeVolume);
		internal long? TradeCount => getLong(WombatFieldIDs.TradeCount);
		internal long? OrderMinQty => getLong(WombatFieldIDs.OrderMinQty);
		internal long? OrderMaxQty => getLong(WombatFieldIDs.OrderMaxQty);
		internal long? BookDepth => getLong(WombatFieldIDs.BookDepth);
		internal long? ExchTotalVol => getLong(WombatFieldIDs.ExchTotalVol);
		internal long? OpenInterest => getLong(WombatFieldIDs.OpenInterest);

		internal double? uClientRecivedTimeDelta => getSpan(WombatFieldIDs.uServerSentTime, WombatFieldIDs.uClientRecivedTime);

		internal DateTime? SettleDateDT => getDateTime(WombatFieldIDs.SettleDate);
		internal DateTime? uOnMsgTime
		{
			get => getDateTime(WombatFieldIDs.uOnMsgTime);
			set {
				if (value.HasValue)
					_fields.Set(WombatFieldIDs.uOnMsgTime, value.Value.ToOADate());
			}
		}
		internal double? uOnMsgTimeDelta {
			get {
				if (_fields.TryGet(WombatFieldIDs.LineTime, out string lineTimeText)) {
					DateTime? onMsgTime = uOnMsgTime;
					if (onMsgTime.HasValue) {
						DateTime lineTime;
						if (DateTime.TryParseExact(lineTimeText, TimeFormats, System.Globalization.CultureInfo.CurrentCulture, System.Globalization.DateTimeStyles.None, out lineTime))
							return onMsgTime.Value.Subtract(lineTime.ToLocalTime()).TotalMilliseconds;
					}
				}
				return null;
			}
		}
		internal DateTime? uServerSentTime {
			get => getDateTime(WombatFieldIDs.uServerSentTime);
			set {
				if (value.HasValue)
					_fields.Set(WombatFieldIDs.uServerSentTime, value.Value.ToOADate());
			}
		}
		internal DateTime? uClientRecivedTime {
			get => getDateTime(WombatFieldIDs.uClientRecivedTime);
			set {
				if (value.HasValue)
					_fields.Set(WombatFieldIDs.uClientRecivedTime, value.Value.ToOADate());
			}
		}
		internal DateTime? uDisplayTime {
			get => getDateTime(WombatFieldIDs.uDisplayTime);
			set {
				if (value.HasValue)
					_fields.Set(WombatFieldIDs.uDisplayTime, value.Value.ToOADate());
			}
		}
		internal int SecurityStatusINT {
			get {
				switch (SecurityStatus.ToUpper()) {
					case "NORMAL":
					case "CLOSED":
						return SecurityStates.Normal;
					//return SecurityStates.Closed;
					case "HALTED":
						return SecurityStates.Halted;
					case "NOTEXIST":
						return SecurityStates.NotExist;
					case "DELETED":
						return SecurityStates.Deleted;
					case "AUCTION":
						return SecurityStates.Auction;
					case "CROSSING":
						return SecurityStates.Crossing;
					default:
						return SecurityStates.None;
				}
			}
		}

		private long? getLong(int key)
		{
			if (_fields.TryGet(key, out long val))
				return val;
			return null;
		}

		private double? getDouble(int key)
		{
			if (_fields.TryGet(key, out double val))
				return val;
			return null;
		}

		private double? getFirstDouble(params int[] keys)
		{
			foreach (int key in keys) {
				if (_fields.TryGet(key, out double val))
					return val;
			}
			return null;
		}

		private double? getSpan(int spanStartKey, int spanStopKey)
		{
			if (!_fields.TryGet(spanStartKey, out double spanStart))
				return null;
			if (!_fields.TryGet(spanStopKey, out double spanStop))
				return null;
			return DateTime.FromOADate(spanStop).Subtract(DateTime.FromOADate(spanStart)).TotalMilliseconds;
		}

		private DateTime? getDateTime(int key)
		{
			if (_fields.TryGet(key, out double val))
				return DateTime.FromOADate(val);
			return null;
		}

		#endregion

		#region - ISerializable Members -

		private void serializeQuoteList(List<Quote> list, SerializationEx.SerializationWriter writer)
		{
			writer.Write(list.Count);
			foreach (Quote q in list) {
				writer.Write(q.QuotePrice.Value);
				writer.Write(q.Size);
			}
		}

		private List<Quote> deserializeQuoteList(SerializationEx.SerializationReader reader)
		{
			reader.Read(out int count);
			List<Quote> list = new List<Quote>(count);
			for (int i = 0; i < count; ++i) {
				reader.Read(out double price);
				reader.Read(out long volume);
				list.Add(new Quote(price, volume));
			}
			return list;
		}

		// Serialize the object.
		internal void GetObjectData(SerializationInfo info, StreamingContext context)
		{
			_fields.GetObjectData(info, context);

			var writer = new SerializationEx.SerializationWriter();
			serializeQuoteList(_asks, writer);
			serializeQuoteList(_bids, writer);
			serializeQuoteList(_volumes, writer);
			writer.AddToInfo(info);
		}

		// Deserialization constructor.
		internal MDServerToClient(SerializationInfo info, StreamingContext context)
		{
			_fields = new DictionaryEx.MultiTypedDictionary(info, context);

			var sr = new SerializationEx.SerializationReader(info);
			_asks = deserializeQuoteList(sr);
			_bids = deserializeQuoteList(sr);
			_volumes = deserializeQuoteList(sr);
		}

		#endregion
	}
}
