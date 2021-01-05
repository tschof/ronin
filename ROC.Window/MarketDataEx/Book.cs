using System.Collections.Generic;
using DateTime = System.DateTime;

using Common;

namespace MarketData
{
	public class Book
	{
		public enum SideEnum { Ask, Bid };

		public enum FieldEnum
		{
			// Double fields
			AdjPrevClosePrice,
			AskClose,
			AskHigh,
			AskLow,
			AskPrice,
			BidClose,
			BidHigh,
			BidLow,
			BidPrice,
			ClosePrice,
			DisplayConversionFactor,
			HighPrice,
			HighLimit,
			LastBestAskPrice,
			LastBestBidPrice,
			LowLimit,
			LowPrice,
			NetChange,
			OpenPrice,
			PctChange,
			PrevAskClose,
			PrevBidClose,
			PrevClosePrice,
			SettlePrice,
			StrikePrice,
			TicketIncrement,
			TotalValue,
			TradePrice,
			uDecodedTime,
			Vwap,

			// String fields
			AskCloseDate,
			AskPartId,
			BidCloseDate,
			BidPartId,
			ExpirationDate,
			InstrumentType,
			IssueSymbol,
			LineTime,
			MainFraction,
			Maturity,
			MaxPriceVariation,
			PrevAskCloseDate,
			PrevBidCloseDate,
			PrevCloseDate,
			PrimExch,
			ProductInd,
			PutCall,
			QuoteDate,
			QuoteTime,
			SecurityCondition,
			SecurityStatus,
			SettleDate,
			TradeDate,
			TradePartId,
			TradeTick,
			TradeTime,
			uParticipantSymbol,
			uParticipantName,
			uSubscriptionKey,
			uSubscriptionSource,
			uSubscriptionSymbol,
			uSymbol,

			// Long fields
			AskSize,
			BidSize,
			BookDepth,
			ExchTotalVol,
			OpenInterest,
			OrderMaxQty,
			OrderMinQty,
			QuoteCount,
			TotalVolume,
			TradeCount,
			TradeVolume,

			// Derived fields (based on other fields)
				// DateTime
			SettleDateDT,
			uClientRecivedTime,
			uDisplayTime,
			uOnMsgTime,
			uServerSentTime,
				// double
			uClientRecievedTimeDelta,
			uOnMsgTimeDelta
		}

		private const int FIELD_COUNT = 80; // The number of values defined in FieldEnum.
		private const byte UNSET_INDEX = byte.MaxValue;

		private QuoteCollection _asks;
		private QuoteCollection _bids;
		private QuoteCollection _volumes;

		// A dictionary of fields would be easier, but we want fast more than we want easy.
		// We use an array of field info values to quickly look up fields by FieldEnum.  We
		// also use a FieldTypeEnum to facilitate merges and to keep our _fieldInfo lookup
		// accurate even if we somehow get different data types for a given field.
		private readonly List<string> _strings;
		private readonly List<double> _doubles;
		private readonly List<long> _longs;
		private readonly byte[] _indexes;
		private int _setFieldCount = 0;

		public QuoteCollection Bids { get { return _bids; } }
		public QuoteCollection Asks { get { return _asks; } }
		public QuoteCollection TradedVolumes { get { return _volumes; } }

		private static class TimeFormats
		{
			internal static bool TryParse(string text, out DateTime value)
			{
				return DateTime.TryParseExact(text, "HH:mm:ss.fff", System.Globalization.CultureInfo.CurrentCulture, System.Globalization.DateTimeStyles.None, out value);
			}
		}

		public long Count {
			get {
				return _setFieldCount + _asks.Count + _bids.Count;
			}
		}

		public Book()
		{
			_asks = new QuoteCollection();
			_bids = new QuoteCollection();
			_volumes = new QuoteCollection();

			_strings = new List<string>();
			_doubles = new List<double>();
			_longs = new List<long>();
			_indexes = new byte[FIELD_COUNT];

			_setFieldCount = 0;
			for (int i = 0; i < FIELD_COUNT; ++i)
				_indexes[i] = UNSET_INDEX;
		}

		public Book(Book other)
		{
			_asks = new QuoteCollection(other._asks);
			_bids = new QuoteCollection(other._bids);
			_volumes = new QuoteCollection(other._volumes);
			_strings = new List<string>(other._strings);
			_doubles = new List<double>(other._doubles);
			_longs = new List<long>(other._longs);
			_indexes = (byte[])other._indexes.Clone();
			_setFieldCount = other._setFieldCount;
		}

		public Book(IBookAdapter adapter)
		{
			_asks = adapter.GetAsks();
			_bids = adapter.GetBids();
			_volumes = adapter.GetTradedVolumes();
			adapter.VisitFields(this);
		}

		public void SetAsk(Price price, long size)
		{
			_asks.SetLevel(price, size);
		}

		public void SetBid(Price price, long size)
		{
			_bids.SetLevel(price, size);
		}

		public bool RemoveQuote(Price price)
		{
			bool changedAsks = _asks.Remove(price);
			bool changedBids = _bids.Remove(price);

			return changedBids || changedAsks;
		}

		public bool TryGetField(FieldEnum which, out DateTime value)
		{
			// All 'DateTime' fields are derived from 'double' fields.
			FieldEnum? sourceField = null;

			switch (which) {
				case FieldEnum.SettleDateDT:
					sourceField = FieldEnum.SettleDate;
					break;
				case FieldEnum.uClientRecivedTime:
				case FieldEnum.uDisplayTime:
				case FieldEnum.uOnMsgTime:
				case FieldEnum.uServerSentTime:
					sourceField = which;
					break;
			}

			if (sourceField.HasValue && tryGetField(sourceField.Value, _doubles, null, out double ticks, 0)) {
				value = DateTime.FromOADate(ticks);
				return true;
			}

			value = DateTime.MinValue;
			return false;
		}

		public bool TryGetField(FieldEnum which, out string value)
		{
			return tryGetField(which, _strings, isStringField, out value, string.Empty);
		}

		public bool TryGetField(FieldEnum which, out double value)
		{
			// Some 'double' fields are derived from other fields.
			switch (which) {
				case FieldEnum.uOnMsgTimeDelta:
					if (tryGetField(FieldEnum.LineTime, _strings, null, out string lineTimeText, null)) {
						if (TryGetField(FieldEnum.uOnMsgTime, out DateTime onMsgTime)) {
							if (TimeFormats.TryParse(lineTimeText, out DateTime lineTime)) {
								value = onMsgTime.Subtract(lineTime.ToLocalTime()).TotalMilliseconds;
								return true;
							}
						}
					}
					break;
				case FieldEnum.uClientRecievedTimeDelta:
					if (tryGetField(FieldEnum.uServerSentTime, _doubles, null, out double spanStart, 0)) {
						if (tryGetField(FieldEnum.uClientRecivedTime, _doubles, null, out double spanStop, 0)) {
							value = DateTime.FromOADate(spanStop).Subtract(DateTime.FromOADate(spanStart)).TotalMilliseconds;
							return true;
						}
					}
					break;
			}
			return tryGetField(which, _doubles, isDoubleField, out value, 0);
		}

		public bool TryGetField(FieldEnum which, out long value)
		{
			return tryGetField(which, _longs, isLongField, out value, 0);
		}

		// A convenience method for retrieving strings that are not null or empty.
		public bool TryGetNonEmpty(FieldEnum which, out string value)
		{
			if (tryGetField(which, _strings, isStringField, out value, null))
				return !string.IsNullOrEmpty(value);
			return false;
		}

		// A convenience method for retrieving nonzero prices.
		public bool TryGetNonZero(FieldEnum which, out double value)
		{
			if (tryGetField(which, _doubles, isDoubleField, out value, 0))
				return !Price.IsZeroValue(value);
			return false;
		}

		public void SetField<T>(FieldEnum which, T value, bool wantReplace = true)
		{
			switch (value) {
				case DateTime dateval:
					// All 'DateTime' fields are derived from 'double' fields.
					// Those can be set from DateTime values (internally) or
					// from 'double' values (by the exchange).
					switch (which) {
						case FieldEnum.uOnMsgTime:
						case FieldEnum.uServerSentTime:
						case FieldEnum.uClientRecivedTime:
						case FieldEnum.uDisplayTime:
							setField(which, _doubles, dateval.ToOADate(), true);
							break;
					}
					break;
				case string sval:
					if (isStringField(which))
						setField(which, _strings, sval, wantReplace);
					break;
				case double dval:
					if (isDoubleField(which))
						setField(which, _doubles, dval, wantReplace);
					break;
				case long lval:
					if (isLongField(which))
						setField(which, _longs, lval, wantReplace);
					break;
			}
		}

		public void RemoveField(FieldEnum which)
		{
			byte i = _indexes[(byte)which];
			_indexes[(byte)which] = UNSET_INDEX;

			if (i != UNSET_INDEX) {
				--_setFieldCount;
				if (isDoubleField(which))
					_doubles.RemoveAt(i);
				else if (isStringField(which))
					_strings.RemoveAt(i);
				else if (isLongField(which))
					_longs.RemoveAt(i);
			}
		}

		public void Merge(Book other, bool clobber = true)
		{
			_asks.Merge(other._asks);
			_bids.Merge(other._bids);
			_volumes.Merge(other._volumes);
			mergeFields(_doubles, _indexes, other._doubles, other._indexes, FieldEnum.AdjPrevClosePrice, FieldEnum.Vwap, clobber);
			mergeFields(_strings, _indexes, other._strings, other._indexes, FieldEnum.AskCloseDate, FieldEnum.uSymbol, clobber);
			mergeFields(_longs, _indexes, other._longs, other._indexes, FieldEnum.AskSize, FieldEnum.TradeVolume, clobber);
		}

		public void RoundPrices(int digits, Price? askPrice, Price? bidPrice, Price? highPrice, Price? lowPrice)
		{
			if (askPrice.HasValue)
				setField(FieldEnum.AskPrice, _doubles, askPrice.Value.Value, false);

			if (bidPrice.HasValue)
				setField(FieldEnum.BidPrice, _doubles, askPrice.Value.Value, false);

			byte i = _indexes[(byte)FieldEnum.TradePrice];
			if (i != UNSET_INDEX) {
				double tradePrice = _doubles[i];
				double roundedTradePrice = System.Math.Round(tradePrice, digits);

				_doubles[i] = roundedTradePrice;

				if (highPrice.HasValue) {
					if ((highPrice.Value.IsZero) || (highPrice.Value < roundedTradePrice))
						setField(FieldEnum.HighPrice, _doubles, highPrice.Value.Value, false);
				}

				if (lowPrice.HasValue) {
					if ((lowPrice.Value.IsZero) || (lowPrice.Value > roundedTradePrice))
						setField(FieldEnum.LowPrice, _doubles, lowPrice.Value.Value, false);
				}
			}
		}

		public void SnapTo(double ticksize, double adjustment)
		{
			_asks.SnapQuotes(ticksize, adjustment);
			_bids.SnapQuotes(ticksize, adjustment);
			snapFieldTo(FieldEnum.AskPrice, ticksize, adjustment);
			snapFieldTo(FieldEnum.BidPrice, ticksize, adjustment);
			snapFieldTo(FieldEnum.TradePrice, ticksize, adjustment);
			snapFieldTo(FieldEnum.HighPrice, ticksize, adjustment);
			snapFieldTo(FieldEnum.LowPrice, ticksize, adjustment);
			snapFieldTo(FieldEnum.ClosePrice, ticksize, adjustment);
			snapFieldTo(FieldEnum.PrevClosePrice, ticksize, adjustment);
			snapFieldTo(FieldEnum.SettlePrice, ticksize, adjustment);
		}

		// Sort bids/asks, truncate book to the given depth, and remove prices that violate top-of-book
		// as set in the AskPrice and BidPrice fields.
		// The sizeXformFn is used by spreads, basis, and other derivatives to calculate the price
		// at each level from a leg ratio or other function.
		public void SortLevels(int depth, int digits = -1, System.Converter<long, long> sizeXformFn = null)
		{
			Price limitPrice;

			if (!_asks.Empty) {
				if (tryGetField(FieldEnum.AskPrice, _doubles, null, out double askPrice, 0) && !Price.IsZeroValue(askPrice))
					limitPrice = digits < 0 ? askPrice : System.Math.Round(askPrice, digits);
				else
					limitPrice = Price.UnsetPrice;

				_asks.SortLevels(Price.SortOrder.Ask, depth, limitPrice);
			}

			if (!_bids.Empty) {
				if (tryGetField(FieldEnum.BidPrice, _doubles, null, out double bidPrice, 0) && !Price.IsZeroValue(bidPrice))
					limitPrice = digits < 0 ? bidPrice : System.Math.Round(bidPrice, digits);
				else
					limitPrice = Price.UnsetPrice;

				_bids.SortLevels(Price.SortOrder.Bid, depth, limitPrice);
			}
		}

		private void snapFieldTo(FieldEnum which, double ticksize, double adjustment)
		{
			byte i = _indexes[(byte)which];
			if (i != UNSET_INDEX) {
				if (!Price.IsZeroValue(_doubles[i] % ticksize))
					_doubles[i] += adjustment;
			}
		}

		private bool isDoubleField(FieldEnum which)
		{
			return (which >= FieldEnum.AdjPrevClosePrice) && (which <= FieldEnum.Vwap);
		}

		private bool isStringField(FieldEnum which)
		{
			return (which >= FieldEnum.AskCloseDate) && (which <= FieldEnum.uSymbol);
		}

		private bool isLongField(FieldEnum which)
		{
			return (which >= FieldEnum.AskSize) && (which <= FieldEnum.TradeVolume);
		}

		private bool tryGetField<T>(FieldEnum which, List<T> fields, System.Converter<FieldEnum, bool> validator, out T value, T notFoundValue)
		{
			byte fieldId = (byte)which;
			if ((_indexes[fieldId] != UNSET_INDEX) && ((validator == null) || validator(which)) && (fieldId < fields.Count)) {
				value = fields[_indexes[fieldId]];
				return true;
			}
			value = notFoundValue;
			return false;
		}

		private void setField<T>(FieldEnum which, List<T> container, T value, bool clobber)
		{
			byte fieldId = (byte)which;
			if (_indexes[fieldId] == UNSET_INDEX) {
				_indexes[fieldId] = (byte)container.Count;
				container.Add(value);
				++_setFieldCount;
			} else if (clobber) {
				container[_indexes[fieldId]] = value;
			}
		}

		private static void mergeFields<T>(List<T> targetFields, byte[] targetIndexes, List<T> sourceFields, byte[] sourceIndexes, FieldEnum start, FieldEnum stop, bool clobber)
		{
			for (byte fieldId = (byte)start; fieldId < (byte)stop; ++fieldId) {
				byte sourceIndex = sourceIndexes[fieldId];
				if (sourceIndex != UNSET_INDEX) {
					byte targetIndex = targetIndexes[fieldId];
					if (targetIndex == UNSET_INDEX) {
						targetIndexes[fieldId] = (byte)targetFields.Count;
						targetFields.Add(sourceFields[sourceIndex]);
					} else if (clobber) {
						targetFields[targetIndex] = sourceFields[sourceIndex];
					}
				}
			}
		}
	}
}
