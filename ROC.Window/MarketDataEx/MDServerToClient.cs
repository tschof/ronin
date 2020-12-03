using System;
using System.Collections.Generic;
using System.Runtime.Serialization;
using SerializationEx;
using System.Diagnostics;

using DictionaryEx;

namespace MarketDataEx
{
	public static class TimeFormats
	{
		private static string[] _formats = new string[0];
		public static string[] Formats
		{
			get
			{
				if (_formats.Length == 0)
				{
					_formats = new string[1];
					_formats[0] = "HH:mm:ss.fff";
					//_formats[1] = "yyyyMMdd";
					//_formats[2] = "yyyy-MM-dd";

					//_formats[0] = "HHmmss";
					//_formats[1] = "yyyyMMdd-HH:mm:ss";
					//_formats[2] = "yyyyMMdd";
					//_formats[3] = "yyyyMM";
					//_formats[3] = "yyyyMMdd-HH:mm:ss.fff";
				}
				return _formats;
			}
		}
	}

	[Serializable]
	public class MDServerToClient : MutiTypedDictionary, ISerializable
	{
		private object _syncObj = new object();

		private Dictionary<double, long> _bidBooks;
		public Dictionary<double, long> BidBooks
		{
			get
			{
				if (_bidBooks == null)
				{
					_bidBooks = new Dictionary<double, long>();
				}
				return _bidBooks;
			}
			set
			{
				_bidBooks = value;
			}
		}

		private Dictionary<double, long> _askBooks;
		public Dictionary<double, long> AskBooks
		{
			get
			{
				if (_askBooks == null)
				{
					_askBooks = new Dictionary<double, long>();
				}
				return _askBooks;
			}
			set
			{
				_askBooks = value;
			}
		}

		private Dictionary<double, long> _tradedVolumes;
		public Dictionary<double, long> TradedVolumes
		{
			get
			{
				if (_tradedVolumes == null)
				{
					_tradedVolumes = new Dictionary<double, long>();
				}
				return _tradedVolumes;
			}
			set
			{
				_tradedVolumes = value;
			}
		}

		public MDServerToClient()
		{
		}
		public MDServerToClient(MDServerToClient locScr)
		{
			Update(locScr);
		}
		public MDServerToClient(MDServerToClient locScr, bool fast)
		{
			Update(locScr, fast);
		}

		public bool AddAsk(double key, long val)
		{
			return ModifyAsk(key, val);
		}
		public bool AddBid(double key, long val)
		{
			return ModifyBid(key, val);
		}
		public bool ResetTradedVolumes(double key, long val)
		{
			return ModifyTraded(key, val, true);
		}

		public bool Remove(double key)
		{
			bool changed = false;

			lock (_syncObj)
			{
				if (BidBooks.ContainsKey(key))
				{
					BidBooks.Remove(key);
					changed = true;
				}

				if (AskBooks.ContainsKey(key))
				{
					AskBooks.Remove(key);
					changed = true;
				}
			}

			return changed;
		}

		public bool Update(MDServerToClient locScr)
		{
			return Update(locScr, false);
		}
		public bool Update(MDServerToClient locScr, bool fast)
		{
			bool changed = false;

			changed = Update((MutiTypedDictionary)locScr, fast);

			if (locScr.BidBooks != null)
			{
				lock (locScr.BidBooks)
				{
					//BidBooks.Clear();
					if (fast)
					{
						BidBooks = new Dictionary<double, long>(locScr.BidBooks);
					}
					else
					{
					    foreach (KeyValuePair<double, long> kvp in locScr.BidBooks)
					    {
					        if (!changed)
					        {
					            changed = UpdateBid(kvp.Key, kvp.Value);
					        }
					        else
					        {
					            UpdateBid(kvp.Key, kvp.Value);
					        }
					    }
					}
				}
			}

			if (locScr.AskBooks != null)
			{
				lock (locScr.AskBooks)
				{
					//AskBooks.Clear();
					if (fast)
					{
						AskBooks = new Dictionary<double, long>(locScr.AskBooks);
					}
					else
					{
					    foreach (KeyValuePair<double, long> kvp in locScr.AskBooks)
					    {
					        if (!changed)
					        {
					            changed = UpdateAsk(kvp.Key, kvp.Value);
					        }
					        else
					        {
					            UpdateAsk(kvp.Key, kvp.Value);
					        }
					    }
					}
				}
			}

			if (locScr.TradedVolumes != null)
			{
				lock (locScr.TradedVolumes)
				{
					if (TotalVolume == 0)
					{
						if (TradedVolumes.Count > 0)
						{
							// End of Day reset
							TradedVolumes.Clear();
						}
					}
					else
					{
						foreach (KeyValuePair<double, long> kvp in locScr.TradedVolumes)
						{
							if (!changed)
							{
								changed = UpdateTraded(kvp.Key, kvp.Value);
							}
							else
							{
								UpdateTraded(kvp.Key, kvp.Value);
							}
						}
					}
				}
			}

			return changed;
		}

		public bool UpdateBid(double key, long val)
		{
			return ModifyBid(key, val, true);
		}
		public bool UpdateBid(Dictionary<double, long> books)
		{
			bool changed = false;

			lock (books)
			{
				foreach (KeyValuePair<double, long> kvp in books)
				{
					if (!changed)
					{
						changed = UpdateBid(kvp.Key, kvp.Value);
					}
					else
					{
						UpdateBid(kvp.Key, kvp.Value);
					}
				}
			}

			return changed;
		}

		public bool UpdateAsk(double key, long val)
		{
			return ModifyAsk(key, val, true);
		}
		public bool UpdateAsk(Dictionary<double, long> books)
		{
			bool changed = false;

			lock (books)
			{
				foreach (KeyValuePair<double, long> kvp in books)
				{
					if (!changed)
					{
						changed = UpdateAsk(kvp.Key, kvp.Value);
					}
					else
					{
						UpdateAsk(kvp.Key, kvp.Value);
					}
				}
			}

			return changed;
		}
		
		public bool UpdateTraded(double key, long val)
		{
			return ModifyTraded(key, val, false);
		}

		public new void Clear()
		{
			lock (_syncObj)
			{
				Longs.Clear();
				Doubles.Clear();
				Strings.Clear();
				Bools.Clear();
				AskBooks.Clear();
				BidBooks.Clear();
			}
		}

		public new long Count
		{
			get
			{
				return Longs.Count + Doubles.Count + Strings.Count + Bools.Count + AskBooks.Count + BidBooks.Count;
			}
		}

		private bool ModifyBid(double key, long val)
		{
			return ModifyBid(key, val, false);
		}
		private bool ModifyBid(double key, long val, bool update)
		{
			bool changed = false;

			lock (BidBooks)
			{
				if (BidBooks.ContainsKey(key))
				{
					if (update)
					{
						if (BidBooks[key] != val)
						{
							BidBooks[key] = val;
							changed = true;
						}
					}
				}
				else
				{
					BidBooks.Add(key, val);
					changed = true;
				}
			}

			return changed;
		}

		private bool ModifyAsk(double key, long val)
		{
			return ModifyAsk(key, val, false);
		}
		private bool ModifyAsk(double key, long val, bool update)
		{
			bool changed = false;

			lock (AskBooks)
			{
				if (AskBooks.ContainsKey(key))
				{
					if (update)
					{
						if (AskBooks[key] != val)
						{
							AskBooks[key] = val;
							changed = true;
						}
					}
				}
				else
				{
					AskBooks.Add(key, val);
					changed = true;
				}
			}

			return changed;
		}

		private bool ModifyTraded(double key, long val, bool reset)
		{
			lock (TradedVolumes)
			{
				if (TradedVolumes.ContainsKey(key))
				{
					if (reset)
					{
						TradedVolumes[key] = val;
					}
					else
					{
						TradedVolumes[key] = TradedVolumes[key] + val;
					}
				}
				else
				{
					TradedVolumes.Add(key, val);
				}
			}

			return true;
		}

		#region - Named Fields -

		public string IssueSymbol
		{
			get
			{
				if (Strings.ContainsKey(WombatFieldIDs.IssueSymbol))
				{
					return Strings[WombatFieldIDs.IssueSymbol];
				}
				else if (Strings.ContainsKey(WombatFieldIDs.IndexSymbol))
				{
					return Strings[WombatFieldIDs.IndexSymbol];
				}
				else
				{
					return null;
				}
			}
		}
		public string uSymbol
		{
			get
			{
				if (Strings.ContainsKey(WombatFieldIDs.uSymbol))
				{
					return Strings[WombatFieldIDs.uSymbol];
				}
				else
				{
					return null;
				}
			}
		}
		public string uSubscriptionSoruce
		{
			get
			{
				if (Strings.ContainsKey(WombatFieldIDs.SubscriptionSoruce))
				{
					return Strings[WombatFieldIDs.SubscriptionSoruce];
				}
				else
				{
					return null;
				}
			}
		}
		public string uSubscriptionSymbol
		{
			get
			{
				if (Strings.ContainsKey(WombatFieldIDs.SubscriptionSymbol))
				{
					return Strings[WombatFieldIDs.SubscriptionSymbol];
				}
				else
				{
					return null;
				}
			}
		}
		public string uSubscriptionKey
		{
			get
			{
				return string.Format("{0},{1}", new object[] { uSubscriptionSymbol, uSubscriptionSoruce });
			}
		}
		public string uParticipentSymbol
		{
			get
			{
				if (Strings.ContainsKey(WombatFieldIDs.uParticipentSymbol))
				{
					return Strings[WombatFieldIDs.uParticipentSymbol];
				}
				else
				{
					return null;
				}
			}
		}
		public string uParticipentName
		{
			get
			{
				if (Strings.ContainsKey(WombatFieldIDs.uParticipentName))
				{
					return Strings[WombatFieldIDs.uParticipentName];
				}
				else
				{
					return null;
				}
			}
		}
		public string InstrumentType
		{
			get
			{
				if (Strings.ContainsKey(WombatFieldIDs.InstrumentType))
				{
					return Strings[WombatFieldIDs.InstrumentType];
				}
				else
				{
					return null;
				}
			}
		}
		public string LineTime
		{
			get
			{
				if (Strings.ContainsKey(WombatFieldIDs.LineTime))
				{
					return Strings[WombatFieldIDs.LineTime];
				}
				else
				{
					return null;
				}
			}
		}

		public Nullable<DateTime> uOnMsgTime
		{
			get
			{
				if (Doubles.ContainsKey(WombatFieldIDs.uOnMsgTime))
				{
					return DateTime.FromOADate(Doubles[WombatFieldIDs.uOnMsgTime]);
				}
				else
				{
					return null;
				}
			}
			set
			{
				Update(WombatFieldIDs.uOnMsgTime, ((DateTime)value).ToOADate());
			}
		}
		public Nullable<double> uOnMsgTimeDelta
		{
			get
			{
				if (LineTime != "" && uOnMsgTime != null)
				{
					DateTime tDT;
					if (DateTime.TryParseExact(LineTime, TimeFormats.Formats, System.Globalization.CultureInfo.CurrentCulture, System.Globalization.DateTimeStyles.None, out tDT))
					{
						TimeSpan tTS = ((DateTime)uOnMsgTime).Subtract(tDT.ToLocalTime());
						return tTS.TotalMilliseconds;
					}
				}
				return null;
			}
		}
		public Nullable<double> uDecodedTime
		{
			get
			{
				if (Doubles.ContainsKey(WombatFieldIDs.uDecodedTime))
				{
					return Doubles[WombatFieldIDs.uDecodedTime];
				}
				else
				{
					return null;
				}
			}
		}
		public Nullable<DateTime> uServerSentTime
		{
			get
			{
				if (Doubles.ContainsKey(WombatFieldIDs.uServerSentTime))
				{
					return DateTime.FromOADate(Doubles[WombatFieldIDs.uServerSentTime]);
				}
				else
				{
					return null;
				}
			}
			set
			{
				Update(WombatFieldIDs.uServerSentTime, ((DateTime)value).ToOADate());
			}
		}
		public Nullable<DateTime> uClientRecivedTime
		{
			get
			{
				if (Doubles.ContainsKey(WombatFieldIDs.uClientRecivedTime))
				{
					return DateTime.FromOADate(Doubles[WombatFieldIDs.uClientRecivedTime]);
				}
				else
				{
					return null;
				}
			}
			set
			{
				Update(WombatFieldIDs.uClientRecivedTime, ((DateTime)value).ToOADate());
			}
		}
		public Nullable<double> uClientRecivedTimeDelta
		{
			get
			{
				if (uServerSentTime != null && uClientRecivedTime != null)
				{
					TimeSpan tTS = ((DateTime)uClientRecivedTime).Subtract((DateTime)uServerSentTime);
					return tTS.TotalMilliseconds;
				}
				return null;
			}
		}
		public Nullable<DateTime> uDisplayTime
		{
			get
			{
				if (Doubles.ContainsKey(WombatFieldIDs.uDisplayTime))
				{
					return DateTime.FromOADate(Doubles[WombatFieldIDs.uDisplayTime]);
				}
				else
				{
					return null;
				}
			}
			set
			{
				Update(WombatFieldIDs.uDisplayTime, ((DateTime)value).ToOADate());
			}
		}

		public Nullable<double> NetChange
		{
			get
			{
				if (Doubles.ContainsKey(WombatFieldIDs.NetChange))
				{
					return Doubles[WombatFieldIDs.NetChange];
				}
				else
				{
					return null;
				}
			}
		}
		public Nullable<double> PctChange
		{
			get
			{
				if (Doubles.ContainsKey(WombatFieldIDs.PctChange))
				{
					return Doubles[WombatFieldIDs.PctChange];
				}
				else
				{
					return null;
				}
			}
		}
		public Nullable<long> TotalVolume
		{
			get
			{
				if (Longs.ContainsKey(WombatFieldIDs.TotalVolume))
				{
					return Longs[WombatFieldIDs.TotalVolume];
				}
				else
				{
					return null;
				}
			}
		}
		public Nullable<double> HighPrice
		{
			get
			{
				if (Doubles.ContainsKey(WombatFieldIDs.HighPrice))
				{
					return Doubles[WombatFieldIDs.HighPrice];
				}
				else if (Doubles.ContainsKey(WombatFieldIDs.IndexHigh))
				{
					return Doubles[WombatFieldIDs.IndexHigh];
				}
				else
				{
					return null;
				}
			}
		}
		public Nullable<double> LowPrice
		{
			get
			{
				if (Doubles.ContainsKey(WombatFieldIDs.LowPrice))
				{
					return Doubles[WombatFieldIDs.LowPrice];
				}
				else if (Doubles.ContainsKey(WombatFieldIDs.IndexLow))
				{
					return Doubles[WombatFieldIDs.IndexLow];
				}
				else
				{
					return null;
				}
			}
		}
		public Nullable<double> OpenPrice
		{
			get
			{
				if (Doubles.ContainsKey(WombatFieldIDs.OpenPrice))
				{
					return Doubles[WombatFieldIDs.OpenPrice];
				}
				else
				{
					return null;
				}
			}
		}
		public Nullable<double> ClosePrice
		{
			get
			{
				if (Doubles.ContainsKey(WombatFieldIDs.ClosePrice))
				{
					return Doubles[WombatFieldIDs.ClosePrice];
				}
				else
				{
					return null;
				}
			}
		}
		public Nullable<double> PrevClosePrice
		{
			get
			{
				if (Doubles.ContainsKey(WombatFieldIDs.PrevClosePrice))
				{
					return Doubles[WombatFieldIDs.PrevClosePrice];
				}
				else
				{
					return null;
				}
			}
		}
		public Nullable<double> AdjPrevClosePrice
		{
			get
			{
				if (Doubles.ContainsKey(WombatFieldIDs.AdjPrevClosePrice))
				{
					return Doubles[WombatFieldIDs.AdjPrevClosePrice];
				}
				else
				{
					return null;
				}
			}
		}
		public string PrevCloseDate
		{
			get
			{
				if (Strings.ContainsKey(WombatFieldIDs.PrevCloseDate))
				{
					return Strings[WombatFieldIDs.PrevCloseDate];
				}
				else
				{
					return null;
				}
			}
		}
		public Nullable<double> TotalValue
		{
			get
			{
				if (Doubles.ContainsKey(WombatFieldIDs.TotalValue))
				{
					return Doubles[WombatFieldIDs.TotalValue];
				}
				else
				{
					return null;
				}
			}
		}
		public Nullable<double> Vwap
		{
			get
			{
				if (Doubles.ContainsKey(WombatFieldIDs.Vwap))
				{
					return Doubles[WombatFieldIDs.Vwap];
				}
				else
				{
					return null;
				}
			}
		}

		public Nullable<double> BidPrice
		{
			get
			{
				if (Doubles.ContainsKey(WombatFieldIDs.BidPrice))
				{
					return Doubles[WombatFieldIDs.BidPrice];
				}
				else if (Doubles.ContainsKey(WombatFieldIDs.IndexBid))
				{
					return Doubles[WombatFieldIDs.IndexBid];
				}
				else
				{
					return null;
				}
			}
		}
		public Nullable<long> BidSize
		{
			get
			{
				if (Longs.ContainsKey(WombatFieldIDs.BidSize))
				{
					return Longs[WombatFieldIDs.BidSize];
				}
				else
				{
					return null;
				}
			}
		}
		public Nullable<double> BidLow
		{
			get
			{
				if (Doubles.ContainsKey(WombatFieldIDs.BidLow))
				{
					return Doubles[WombatFieldIDs.BidLow];
				}
				else
				{
					return null;
				}
			}
		}
		public Nullable<double> BidHigh
		{
			get
			{
				if (Doubles.ContainsKey(WombatFieldIDs.BidHigh))
				{
					return Doubles[WombatFieldIDs.BidHigh];
				}
				else
				{
					return null;
				}
			}
		}
		public string BidPartId
		{
			get
			{
				if (Strings.ContainsKey(WombatFieldIDs.BidPartId))
				{
					return Strings[WombatFieldIDs.BidPartId];
				}
				else
				{
					return null;
				}
			}
		}

		public Nullable<double> AskPrice
		{
			get
			{
				if (Doubles.ContainsKey(WombatFieldIDs.AskPrice))
				{
					return Doubles[WombatFieldIDs.AskPrice];
				}
				else if (Doubles.ContainsKey(WombatFieldIDs.IndexAsk))
				{
					return Doubles[WombatFieldIDs.IndexAsk];
				}
				else
				{
					return null;
				}
			}
		}
		public Nullable<long> AskSize
		{
			get
			{
				if (Longs.ContainsKey(WombatFieldIDs.AskSize))
				{
					return Longs[WombatFieldIDs.AskSize];
				}
				else
				{
					return null;
				}
			}
		}
		public Nullable<double> AskLow
		{
			get
			{
				if (Doubles.ContainsKey(WombatFieldIDs.AskLow))
				{
					return Doubles[WombatFieldIDs.AskLow];
				}
				else
				{
					return null;
				}
			}
		}
		public Nullable<double> AskHigh
		{
			get
			{
				if (Doubles.ContainsKey(WombatFieldIDs.AskHigh))
				{
					return Doubles[WombatFieldIDs.AskHigh];
				}
				else
				{
					return null;
				}
			}
		}
		public string AskPartId
		{
			get
			{
				if (Strings.ContainsKey(WombatFieldIDs.AskPartId))
				{
					return Strings[WombatFieldIDs.AskPartId];
				}
				else
				{
					return null;
				}
			}
		}

		public string QuoteDate
		{
			get
			{
				if (Strings.ContainsKey(WombatFieldIDs.QuoteDate))
				{
					return Strings[WombatFieldIDs.QuoteDate];
				}
				else
				{
					return null;
				}
			}
		}
		public string QuoteTime
		{
			get
			{
				if (Strings.ContainsKey(WombatFieldIDs.QuoteTime))
				{
					return Strings[WombatFieldIDs.QuoteTime];
				}
				else
				{
					return null;
				}
			}
		}
		public Nullable<long> QuoteCount
		{
			get
			{
				if (Longs.ContainsKey(WombatFieldIDs.QuoteCount))
				{
					return Longs[WombatFieldIDs.QuoteCount];
				}
				else
				{
					return null;
				}
			}
		}

		public Nullable<double> TradePrice
		{
			get
			{
				if (Doubles.ContainsKey(WombatFieldIDs.TradePrice))
				{
					return Doubles[WombatFieldIDs.TradePrice];
				}
				else if (Doubles.ContainsKey(WombatFieldIDs.IndexValue))
				{
					return Doubles[WombatFieldIDs.IndexValue];
				}
				else
				{
					return null;
				}
			}
		}
		public string TradeDate
		{
			get
			{
				if (Strings.ContainsKey(WombatFieldIDs.TradeDate))
				{
					return Strings[WombatFieldIDs.TradeDate];
				}
				else
				{
					return null;
				}
			}
		}
		public string TradeTime
		{
			get
			{
				if (Strings.ContainsKey(WombatFieldIDs.TradeTime))
				{
					return Strings[WombatFieldIDs.TradeTime];
				}
				else
				{
					return null;
				}
			}
		}
		public string TradeTick
		{
			get
			{
				if (Strings.ContainsKey(WombatFieldIDs.TradeTick))
				{
					return Strings[WombatFieldIDs.TradeTick];
				}
				else
				{
					return null;
				}
			}
		}

		public Nullable<long> TradeVolume
		{
			get
			{
				if (Longs.ContainsKey(WombatFieldIDs.TradeVolume))
				{
					return Longs[WombatFieldIDs.TradeVolume];
				}
				else
				{
					return null;
				}
			}
		}
		public string TradePartId
		{
			get
			{
				if (Strings.ContainsKey(WombatFieldIDs.TradePartId))
				{
					return Strings[WombatFieldIDs.TradePartId];
				}
				else
				{
					return null;
				}
			}
		}
		public Nullable<long> TradeCount
		{
			get
			{
				if (Longs.ContainsKey(WombatFieldIDs.TradeCount))
				{
					return Longs[WombatFieldIDs.TradeCount];
				}
				else
				{
					return null;
				}
			}
		}

		// Stock L1
		public string PrimExch
		{
			get
			{
				if (Strings.ContainsKey(WombatFieldIDs.PrimExch))
				{
					return Strings[WombatFieldIDs.PrimExch];
				}
				else
				{
					return null;
				}
			}
		}
		public string SecurityStatus
		{
			get
			{
				if (Strings.ContainsKey(WombatFieldIDs.SecurityStatus))
				{
					return Strings[WombatFieldIDs.SecurityStatus];
				}
				else
				{
					return "NONE";
				}

				//return SecurityCondition;
			}
		}
		public int SecurityStatusINT
		{
			get
			{
				switch (SecurityStatus.ToUpper())
				{
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
		public string SecurityCondition
		{
			get
			{
				if (Strings.ContainsKey(WombatFieldIDs.Condition))
				{
					return Strings[WombatFieldIDs.Condition];
				}
				else
				{
					return "";
				}
			}
		}

		// Future L1
		public string ProductInd
		{
			get
			{
				if (Strings.ContainsKey(WombatFieldIDs.ProductInd))
				{
					return Strings[WombatFieldIDs.ProductInd];
				}
				else
				{
					return null;
				}
			}
		}
		public Nullable<double> DisplayConversionFactor
		{
			get
			{
				if (Doubles.ContainsKey(WombatFieldIDs.DisplayConversionFactor))
				{
					return Doubles[WombatFieldIDs.DisplayConversionFactor];
				}
				else
				{
					return null;
				}
			}
		}
		public Nullable<long> OrderMinQty
		{
			get
			{
				if (Longs.ContainsKey(WombatFieldIDs.OrderMinQty))
				{
					return Longs[WombatFieldIDs.OrderMinQty];
				}
				else
				{
					return null;
				}
			}
		}
		public Nullable<long> OrderMaxQty
		{
			get
			{
				if (Longs.ContainsKey(WombatFieldIDs.OrderMaxQty))
				{
					return Longs[WombatFieldIDs.OrderMaxQty];
				}
				else
				{
					return null;
				}
			}
		}
		public string MaxPriceVariation
		{
			get
			{
				if (Strings.ContainsKey(WombatFieldIDs.MaxPriceVariation))
				{
					return Strings[WombatFieldIDs.MaxPriceVariation];
				}
				else
				{
					return null;
				}
			}
		}
		public string Maturity
		{
			get
			{
				if (Strings.ContainsKey(WombatFieldIDs.Maturity))
				{
					return Strings[WombatFieldIDs.Maturity];
				}
				else
				{
					return null;
				}
			}
		}
		public Nullable<long> BookDepth
		{
			get
			{
				if (Longs.ContainsKey(WombatFieldIDs.BookDepth))
				{
					return Longs[WombatFieldIDs.BookDepth];
				}
				else
				{
					return null;
				}
			}
		}
		public Nullable<double> TicketIncrement
		{
			get
			{
				if (Doubles.ContainsKey(WombatFieldIDs.TicketIncrement))
				{
					return Doubles[WombatFieldIDs.TicketIncrement];
				}
				else
				{
					return null;
				}
			}
		}
		public string MainFraction
		{
			get
			{
				if (Strings.ContainsKey(WombatFieldIDs.MainFraction))
				{
					return Strings[WombatFieldIDs.MainFraction];
				}
				else
				{
					return null;
				}
			}
		}
		public Nullable<double> HightLimit
		{
			get
			{
				if (Doubles.ContainsKey(WombatFieldIDs.HightLimit))
				{
					return Doubles[WombatFieldIDs.HightLimit];
				}
				else
				{
					return null;
				}
			}
		}
		public Nullable<double> LowLimit
		{
			get
			{
				if (Doubles.ContainsKey(WombatFieldIDs.LowLimit))
				{
					return Doubles[WombatFieldIDs.LowLimit];
				}
				else
				{
					return null;
				}
			}
		}
		public Nullable<double> SettlePrice
		{
			get
			{
				if (Doubles.ContainsKey(WombatFieldIDs.SettlePrice))
				{
					return Doubles[WombatFieldIDs.SettlePrice];
				}
				else
				{
					return null;
				}
			}
		}
		public string SettleDate
		{
			get
			{
				if (Strings.ContainsKey(WombatFieldIDs.SettleDate))
				{
					return Strings[WombatFieldIDs.SettleDate];
				}
				else
				{
					return null;
				}
			}
		}
		public Nullable<DateTime> SettleDateDT
		{
			get
			{
				if (SettleDate != "")
				{
					DateTime tDT;
					if (DateTime.TryParse(SettleDate, out tDT))
					{
						return tDT;
					}
				}
				return null;
			}
		}
		public Nullable<double> LastBestAskPrice
		{
			get
			{
				if (Doubles.ContainsKey(WombatFieldIDs.LastBestAskPrice))
				{
					return Doubles[WombatFieldIDs.LastBestAskPrice];
				}
				else
				{
					return null;
				}
			}
		}
		public Nullable<double> LastBestBidPrice
		{
			get
			{
				if (Doubles.ContainsKey(WombatFieldIDs.LastBestBidPrice))
				{
					return Doubles[WombatFieldIDs.LastBestBidPrice];
				}
				else
				{
					return null;
				}
			}
		}
		public Nullable<long> ExchTotalVol
		{
			get
			{
				if (Longs.ContainsKey(WombatFieldIDs.ExchTotalVol))
				{
					return Longs[WombatFieldIDs.ExchTotalVol];
				}
				else
				{
					return null;
				}
			}
		}

		// Option L1
		public string PutCall
		{
			get
			{
				if (Strings.ContainsKey(WombatFieldIDs.PutCall))
				{
					return Strings[WombatFieldIDs.PutCall];
				}
				else
				{
					return null;
				}
			}
		}
		public string ExpirationDate
		{
			get
			{
				if (Strings.ContainsKey(WombatFieldIDs.ExpirationDate))
				{
					return Strings[WombatFieldIDs.ExpirationDate];
				}
				else
				{
					return null;
				}
			}
		}
		public Nullable<double> StrikePrice
		{
			get
			{
				if (Doubles.ContainsKey(WombatFieldIDs.StrikePrice))
				{
					return Doubles[WombatFieldIDs.StrikePrice];
				}
				else
				{
					return null;
				}
			}
		}
		public Nullable<long> OpenInterest
		{
			get
			{
				if (Longs.ContainsKey(WombatFieldIDs.OpenInterest))
				{
					return Longs[WombatFieldIDs.OpenInterest];
				}
				else
				{
					return null;
				}
			}
		}
		public Nullable<double> PrevBidClose
		{
			get
			{
				if (Doubles.ContainsKey(WombatFieldIDs.PrevBidClose))
				{
					return Doubles[WombatFieldIDs.PrevBidClose];
				}
				else
				{
					return null;
				}
			}
		}
		public string PrevBidCloseDate
		{
			get
			{
				if (Strings.ContainsKey(WombatFieldIDs.PrevBidCloseDate))
				{
					return Strings[WombatFieldIDs.PrevBidCloseDate];
				}
				else
				{
					return null;
				}
			}
		}
		public Nullable<double> BidClose
		{
			get
			{
				if (Doubles.ContainsKey(WombatFieldIDs.BidClose))
				{
					return Doubles[WombatFieldIDs.BidClose];
				}
				else
				{
					return null;
				}
			}
		}
		public string BidCloseDate
		{
			get
			{
				if (Strings.ContainsKey(WombatFieldIDs.BidCloseDate))
				{
					return Strings[WombatFieldIDs.BidCloseDate];
				}
				else
				{
					return null;
				}
			}
		}
		public Nullable<double> PrevAskClose
		{
			get
			{
				if (Doubles.ContainsKey(WombatFieldIDs.PrevAskClose))
				{
					return Doubles[WombatFieldIDs.PrevAskClose];
				}
				else
				{
					return null;
				}
			}
		}
		public string PrevAskCloseDate
		{
			get
			{
				if (Strings.ContainsKey(WombatFieldIDs.PrevAskCloseDate))
				{
					return Strings[WombatFieldIDs.PrevAskCloseDate];
				}
				else
				{
					return null;
				}
			}
		}
		public Nullable<double> AskClose
		{
			get
			{
				if (Doubles.ContainsKey(WombatFieldIDs.AskClose))
				{
					return Doubles[WombatFieldIDs.AskClose];
				}
				else
				{
					return null;
				}
			}
		}
		public string AskClosedate
		{
			get
			{
				if (Strings.ContainsKey(WombatFieldIDs.AskClosedate))
				{
					return Strings[WombatFieldIDs.AskClosedate];
				}
				else
				{
					return null;
				}
			}
		}

		#endregion

		#region - ISerializable Members -
		
		// Serialize the object.
		public new void GetObjectData(SerializationInfo info, StreamingContext context)
		{
			SerializationWriter sw = SerializationWriter.GetWriter();

			sw.Write<int, string>(Strings);
			sw.Write<int, long>(Longs);
			sw.Write<int, double>(Doubles);
			sw.Write<int, bool>(Bools);
			sw.Write<double, long>(BidBooks);
			sw.Write<double, long>(AskBooks);
			sw.Write<double, long>(TradedVolumes);
			sw.AddToInfo(info);
		}

		// Deserialization constructor.
		public MDServerToClient(SerializationInfo info, StreamingContext ctxt)
		{
		    SerializationReader sr = SerializationReader.GetReader(info);

			Strings = (Dictionary<int, string>)sr.ReadDictionary<int, string>();
			Longs = (Dictionary<int, long>)sr.ReadDictionary<int, long>();
			Doubles = (Dictionary<int, double>)sr.ReadDictionary<int, double>();
			Bools = (Dictionary<int, bool>)sr.ReadDictionary<int, bool>();
		    BidBooks = (Dictionary<double, long>)sr.ReadDictionary<double, long>();
		    AskBooks = (Dictionary<double, long>)sr.ReadDictionary<double, long>();
			TradedVolumes = (Dictionary<double, long>)sr.ReadDictionary<double, long>();
		}

		#endregion
	}
}
