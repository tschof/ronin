using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Collections;
using System.Diagnostics;

using Wombat;

using MarketDataEx;
using DateTimeEx;

namespace LBMEx
{
	public delegate void LBMChangedEventHandler(object sender, LBMDataChangedEventArgs e);

	#region - Subscription Fields -

	// Used for varification
	public class SubscriptionFields
	{
		private string _symbol = "";
		private string _source = "";
		private string _type = "";

		public SubscriptionFields(string symbol, string source, string type)
		{
			_symbol = symbol;
			_source = source;
			_type = type;
		}

		public string Symbol
		{
			get
			{
				return _symbol;
			}
		}

		public string Source
		{
			get
			{
				return _source;
			}
		}

		public string Type
		{
			get
			{
				return _type;
			}
		}
	}

	#endregion

	public abstract class LBMBase : MamaDictionaryCallback, MamaSubscriptionCallback, MamdaOrderBookHandler, IDisposable
	{
		#region - User Defined Fields -

		private sealed class LocFieldNeams
		{
			public const ushort wIssueSymbol = 305;
			public const ushort wIndexSymbol = 293;

			// Trade
			public const ushort wTradePrice = 481;
			public const ushort wIndexValue = 295;
			public const ushort wTradeDate = 476;
			public const ushort wTradeTime = 485;
			public const ushort wTradeTick = 484;
			public const ushort wTradeVolume = 488;
			public const ushort wTradePartId = 480;
			public const ushort wTradeCount = 901;

			public const ushort wNetChange = 341;
			public const ushort wPctChange = 431;
			public const ushort wTotalVolume = 475;
			public const ushort wTotalValue = 474;
			public const ushort wVwap = 494;

			// Ask
			public const ushort wAskPrice = 109;
			public const ushort wIndexAsk = 279;
			public const ushort wAskSize = 110;
			public const ushort wAskPartId = 108;
			public const ushort wAskHigh = 105;
			public const ushort wAskLow = 106;

			// Bid
			public const ushort wBidPrice = 237;
			public const ushort wIndexBid = 280;
			public const ushort wBidSize = 238;
			public const ushort wBidPartId = 236;
			public const ushort wBidHigh = 233;
			public const ushort wBidLow = 234;

			// L1
			public const ushort wPrimExch = 645;
			public const ushort wSecurityStatus = 451;
			public const ushort wCondition = 243;

			public const ushort wOpenPrice = 407;
			public const ushort wAdjPrevClose = 520;
			public const ushort wPrevClosePrice = 436;
			public const ushort wPrevCloseDate = 435;
			public const ushort wClosePrice = 242;

			public const ushort wSettlePrice = 458;
			public const ushort wSettleDate = 456;

			public const ushort wHighPrice = 278;
			public const ushort wIndexHigh = 286;
			public const ushort wLowPrice = 327;
			public const ushort wIndexLow = 287;

			public const ushort wLineTime = 1174;

			public const ushort wPlAction = 652;
			public const ushort wPlPrice = 653;
			public const ushort wPlSide = 654;
			public const ushort wPlSize = 655;
			public const ushort wPlTime = 658;
			public const ushort wPriceLevels = 699;
		}

		private static Dictionary<ushort, MamaFieldDescriptor> _locfields;
		public static Dictionary<ushort, MamaFieldDescriptor> LocFields
		{
			get
			{
				if (_locfields == null)
				{
					_locfields = new Dictionary<ushort, MamaFieldDescriptor>();
					_locfields.Add(305, new MamaFieldDescriptor(305, mamaFieldType.MAMA_FIELD_TYPE_STRING, "wIssueSymbol"));
					_locfields.Add(293, new MamaFieldDescriptor(293, mamaFieldType.MAMA_FIELD_TYPE_STRING, "wIndexSymbol"));

					_locfields.Add(7001, new MamaFieldDescriptor(7001, mamaFieldType.MAMA_FIELD_TYPE_STRING, "SubscriptionSoruce"));
					_locfields.Add(7002, new MamaFieldDescriptor(7002, mamaFieldType.MAMA_FIELD_TYPE_STRING, "SubscriptionSymbol"));

					_locfields.Add(298, new MamaFieldDescriptor(298, mamaFieldType.MAMA_FIELD_TYPE_STRING, "wInstrumentType"));
					_locfields.Add(1174, new MamaFieldDescriptor(1174, mamaFieldType.MAMA_FIELD_TYPE_STRING, "wLineTime"));

					_locfields.Add(341, new MamaFieldDescriptor(341, mamaFieldType.MAMA_FIELD_TYPE_F64, "wNetChange"));
					_locfields.Add(431, new MamaFieldDescriptor(431, mamaFieldType.MAMA_FIELD_TYPE_F64, "wPctChange"));
					_locfields.Add(475, new MamaFieldDescriptor(475, mamaFieldType.MAMA_FIELD_TYPE_I64, "wTotalVolume"));

					_locfields.Add(278, new MamaFieldDescriptor(278, mamaFieldType.MAMA_FIELD_TYPE_F64, "wHighPrice"));
					_locfields.Add(286, new MamaFieldDescriptor(286, mamaFieldType.MAMA_FIELD_TYPE_F64, "wIndexHigh"));
					_locfields.Add(327, new MamaFieldDescriptor(327, mamaFieldType.MAMA_FIELD_TYPE_F64, "wLowPrice"));
					_locfields.Add(287, new MamaFieldDescriptor(287, mamaFieldType.MAMA_FIELD_TYPE_F64, "wIndexLow"));
					_locfields.Add(407, new MamaFieldDescriptor(407, mamaFieldType.MAMA_FIELD_TYPE_F64, "wOpenPrice"));
					_locfields.Add(520, new MamaFieldDescriptor(520, mamaFieldType.MAMA_FIELD_TYPE_F64, "wAdjPrevClose"));
					_locfields.Add(436, new MamaFieldDescriptor(436, mamaFieldType.MAMA_FIELD_TYPE_F64, "wPrevClosePrice"));
					_locfields.Add(435, new MamaFieldDescriptor(435, mamaFieldType.MAMA_FIELD_TYPE_STRING, "wPrevCloseDate"));
					_locfields.Add(242, new MamaFieldDescriptor(242, mamaFieldType.MAMA_FIELD_TYPE_F64, "wClosePrice"));
					_locfields.Add(474, new MamaFieldDescriptor(474, mamaFieldType.MAMA_FIELD_TYPE_F64, "wTotalValue"));
					_locfields.Add(494, new MamaFieldDescriptor(494, mamaFieldType.MAMA_FIELD_TYPE_F64, "wVwap"));

					_locfields.Add(237, new MamaFieldDescriptor(237, mamaFieldType.MAMA_FIELD_TYPE_F64, "wBidPrice"));
					_locfields.Add(280, new MamaFieldDescriptor(280, mamaFieldType.MAMA_FIELD_TYPE_F64, "wIndexBid"));
					_locfields.Add(238, new MamaFieldDescriptor(238, mamaFieldType.MAMA_FIELD_TYPE_I64, "wBidSize"));
					_locfields.Add(234, new MamaFieldDescriptor(234, mamaFieldType.MAMA_FIELD_TYPE_F64, "wBidLow"));
					_locfields.Add(233, new MamaFieldDescriptor(233, mamaFieldType.MAMA_FIELD_TYPE_F64, "wBidHigh"));
					_locfields.Add(236, new MamaFieldDescriptor(236, mamaFieldType.MAMA_FIELD_TYPE_STRING, "wBidPartId"));

					_locfields.Add(109, new MamaFieldDescriptor(109, mamaFieldType.MAMA_FIELD_TYPE_F64, "wAskPrice"));
					_locfields.Add(279, new MamaFieldDescriptor(279, mamaFieldType.MAMA_FIELD_TYPE_F64, "wIndexAsk"));
					_locfields.Add(110, new MamaFieldDescriptor(110, mamaFieldType.MAMA_FIELD_TYPE_I64, "wAskSize"));
					_locfields.Add(106, new MamaFieldDescriptor(106, mamaFieldType.MAMA_FIELD_TYPE_F64, "wAskLow"));
					_locfields.Add(105, new MamaFieldDescriptor(105, mamaFieldType.MAMA_FIELD_TYPE_F64, "wAskHigh"));
					_locfields.Add(108, new MamaFieldDescriptor(108, mamaFieldType.MAMA_FIELD_TYPE_STRING, "wAskPartId"));

					_locfields.Add(519, new MamaFieldDescriptor(519, mamaFieldType.MAMA_FIELD_TYPE_STRING, "wQuoteDate"));
					_locfields.Add(442, new MamaFieldDescriptor(442, mamaFieldType.MAMA_FIELD_TYPE_STRING, "wQuoteTime"));
					_locfields.Add(1034, new MamaFieldDescriptor(1034, mamaFieldType.MAMA_FIELD_TYPE_I64, "wQuoteCount"));

					_locfields.Add(481, new MamaFieldDescriptor(481, mamaFieldType.MAMA_FIELD_TYPE_F64, "wTradePrice"));
					_locfields.Add(295, new MamaFieldDescriptor(295, mamaFieldType.MAMA_FIELD_TYPE_F64, "wIndexValue"));
					_locfields.Add(476, new MamaFieldDescriptor(476, mamaFieldType.MAMA_FIELD_TYPE_STRING, "wTradeDate"));
					_locfields.Add(485, new MamaFieldDescriptor(485, mamaFieldType.MAMA_FIELD_TYPE_STRING, "wTradeTime"));
					_locfields.Add(484, new MamaFieldDescriptor(484, mamaFieldType.MAMA_FIELD_TYPE_STRING, "wTradeTick"));
					_locfields.Add(488, new MamaFieldDescriptor(488, mamaFieldType.MAMA_FIELD_TYPE_I64, "wTradeVolume"));
					_locfields.Add(480, new MamaFieldDescriptor(480, mamaFieldType.MAMA_FIELD_TYPE_STRING, "wTradePartId"));
					_locfields.Add(901, new MamaFieldDescriptor(901, mamaFieldType.MAMA_FIELD_TYPE_I64, "wTradeCount"));

					// Stock L1
					_locfields.Add(645, new MamaFieldDescriptor(645, mamaFieldType.MAMA_FIELD_TYPE_STRING, "wPrimExch"));
					_locfields.Add(451, new MamaFieldDescriptor(451, mamaFieldType.MAMA_FIELD_TYPE_STRING, "wSecurityStatus"));
					_locfields.Add(243, new MamaFieldDescriptor(243, mamaFieldType.MAMA_FIELD_TYPE_STRING, "wCondition"));

					// Future L1
					_locfields.Add(1520, new MamaFieldDescriptor(1520, mamaFieldType.MAMA_FIELD_TYPE_STRING, "wProductInd"));
					_locfields.Add(1518, new MamaFieldDescriptor(1518, mamaFieldType.MAMA_FIELD_TYPE_F64, "wDisplayConversionFactor"));
					_locfields.Add(513, new MamaFieldDescriptor(513, mamaFieldType.MAMA_FIELD_TYPE_I64, "wOrderMinQty"));
					_locfields.Add(514, new MamaFieldDescriptor(514, mamaFieldType.MAMA_FIELD_TYPE_I64, "wOrderMaxQty"));
					_locfields.Add(3341, new MamaFieldDescriptor(3341, mamaFieldType.MAMA_FIELD_TYPE_STRING, "wMaxPriceVariation"));
					_locfields.Add(1525, new MamaFieldDescriptor(1525, mamaFieldType.MAMA_FIELD_TYPE_STRING, "wMaturity"));
					_locfields.Add(2276, new MamaFieldDescriptor(2276, mamaFieldType.MAMA_FIELD_TYPE_I64, "wBookDepth"));
					_locfields.Add(515, new MamaFieldDescriptor(515, mamaFieldType.MAMA_FIELD_TYPE_F64, "wTickIncrement"));
					_locfields.Add(510, new MamaFieldDescriptor(510, mamaFieldType.MAMA_FIELD_TYPE_STRING, "wInstrumentGroup"));
					_locfields.Add(3345, new MamaFieldDescriptor(3345, mamaFieldType.MAMA_FIELD_TYPE_F64, "wMinPriceIncrementAmount"));
					_locfields.Add(3342, new MamaFieldDescriptor(3342, mamaFieldType.MAMA_FIELD_TYPE_STRING, "wMainFraction"));
					_locfields.Add(227, new MamaFieldDescriptor(227, mamaFieldType.MAMA_FIELD_TYPE_F64, "wHighLimit"));
					_locfields.Add(326, new MamaFieldDescriptor(326, mamaFieldType.MAMA_FIELD_TYPE_F64, "wLowLimit"));
					_locfields.Add(458, new MamaFieldDescriptor(458, mamaFieldType.MAMA_FIELD_TYPE_F64, "wSettlePrice"));
					_locfields.Add(456, new MamaFieldDescriptor(456, mamaFieldType.MAMA_FIELD_TYPE_STRING, "wSettleDate"));
					_locfields.Add(4026, new MamaFieldDescriptor(4026, mamaFieldType.MAMA_FIELD_TYPE_F64, "wLastBestAskPrice"));
					_locfields.Add(4025, new MamaFieldDescriptor(4025, mamaFieldType.MAMA_FIELD_TYPE_F64, "wLastBestBidPrice"));
					_locfields.Add(2516, new MamaFieldDescriptor(2516, mamaFieldType.MAMA_FIELD_TYPE_I64, "wExchTotalVol"));

					// Option
					_locfields.Add(439, new MamaFieldDescriptor(439, mamaFieldType.MAMA_FIELD_TYPE_STRING, "wPutCall"));
					_locfields.Add(262, new MamaFieldDescriptor(262, mamaFieldType.MAMA_FIELD_TYPE_STRING, "wExpirationDate"));
					_locfields.Add(469, new MamaFieldDescriptor(469, mamaFieldType.MAMA_FIELD_TYPE_F64, "wStrikePrice"));
					_locfields.Add(406, new MamaFieldDescriptor(406, mamaFieldType.MAMA_FIELD_TYPE_I64, "wOpenInterest"));
					_locfields.Add(433, new MamaFieldDescriptor(433, mamaFieldType.MAMA_FIELD_TYPE_F64, "wPrevBidClose"));
					_locfields.Add(434, new MamaFieldDescriptor(434, mamaFieldType.MAMA_FIELD_TYPE_STRING, "wPrevBidCloseDate"));
					_locfields.Add(504, new MamaFieldDescriptor(504, mamaFieldType.MAMA_FIELD_TYPE_F64, "wBidClose"));
					_locfields.Add(505, new MamaFieldDescriptor(505, mamaFieldType.MAMA_FIELD_TYPE_STRING, "wBidCloseDate"));
					_locfields.Add(506, new MamaFieldDescriptor(506, mamaFieldType.MAMA_FIELD_TYPE_F64, "wPrevAskClose"));
					_locfields.Add(507, new MamaFieldDescriptor(507, mamaFieldType.MAMA_FIELD_TYPE_STRING, "PrevAskCloseDate"));
					_locfields.Add(502, new MamaFieldDescriptor(502, mamaFieldType.MAMA_FIELD_TYPE_F64, "wAskClose"));
					_locfields.Add(503, new MamaFieldDescriptor(503, mamaFieldType.MAMA_FIELD_TYPE_STRING, "wAskClosedate"));
				
					// Book
					_locfields.Add(652, new MamaFieldDescriptor(652, mamaFieldType.MAMA_FIELD_TYPE_I64, "wPlAction"));
					_locfields.Add(653, new MamaFieldDescriptor(653, mamaFieldType.MAMA_FIELD_TYPE_F64, "wPlPrice"));
					_locfields.Add(654, new MamaFieldDescriptor(654, mamaFieldType.MAMA_FIELD_TYPE_I64, "wPlSide"));
					_locfields.Add(655, new MamaFieldDescriptor(655, mamaFieldType.MAMA_FIELD_TYPE_I64, "wPlSize"));
					_locfields.Add(658, new MamaFieldDescriptor(658, mamaFieldType.MAMA_FIELD_TYPE_STRING, "wPlTime"));
					_locfields.Add(699, new MamaFieldDescriptor(699, mamaFieldType.MAMA_FIELD_TYPE_VECTOR_MSG, "wPriceLevels"));
				}
				return _locfields;
			}
		}

		#endregion
		
		#region - Event -

		public event LBMChangedEventHandler LBMChanged;
		private void OnLBMChanged(LBMDataChangedEventArgs e)
		{
			if (LBMChanged != null)
			{
				LBMChanged(this, e);
			}
		}

		#endregion

		#region - Queue & Decode Logic -

		private object _syncObj = new object();

		private int _mamaMsgQueueLimitPerSymbol = 5;

		// Raw queue list
		private List<MamaMsgQueue> _mamaMsgQueueRaw = new List<MamaMsgQueue>();

		// key by symbol
		private Dictionary<string, MamaMsgQueue> _mamaMsgQueue = new Dictionary<string, MamaMsgQueue>();

		// This should olny be used for Inital Snap Shots
		private List<MamaMsgQueue> _mamaCriticalMsgQueue = new List<MamaMsgQueue>();

		// key by symbol, serve as a _mamaMsgQueueTemplate (local)
		private Dictionary<string, MamaMsgQueue> _knownSymbols = new Dictionary<string, MamaMsgQueue>();

		private class MamaMsgQueue
		{
			private DateTimeHP _dtHP = new DateTimeHP();

			private double _double = 0;
			private long _long = 0;
			private long _size = 0;

			private MamaMsg[] _msgArray = new MamaMsg[0];
			private MamaMsgField _msgField;
			private mamaFieldType _fieldType;

			private int _bookDepthLimit = 20;

			private OnMsgObject _onMsgObj;
			public OnMsgObject OnMsgObj
			{
				get
				{
					return _onMsgObj;
				}
				set
				{
					_onMsgObj = value;
				}
			}

			public MamaMsgQueue(int bookDepthLimit)
			{
				_bookDepthLimit = bookDepthLimit;
			}

			public void Add(OnMsgObject onMsgObj)
			{
				_onMsgObj = onMsgObj;
			}

			#region - Non Critical Message Update -

			public void Update(OnMsgObject onMsgObj)
			{
				lock (_onMsgObj.Deltas)
				{
					_onMsgObj.Deltas.Update(onMsgObj.Deltas);
					_onMsgObj.Deltas.UpdateAsk(onMsgObj.Deltas.AskBooks);
					_onMsgObj.Deltas.UpdateBid(onMsgObj.Deltas.BidBooks);
				}
			}

			public void Update(MamaMsg msg, bool saveTradeAndPrice)
			{
				lock (_onMsgObj.Deltas)
				{
					PickMsg(msg, saveTradeAndPrice);
				}
			}

			private void PickMsg(MamaMsg msg, bool saveTradeAndPrice)
			{
				bool _updateMisc = false;
				bool _hasTradedPrice = false;
				bool _hasTradedSize = false;

				double _double = 0;
				long _long = 0;
				int _int = 0;
				string _string = "";
				char _char = new char();

				DateTime start = _dtHP.Now;

				if (msg.tryString("", LocFieldNeams.wIssueSymbol, ref _string) || msg.tryString("", LocFieldNeams.wIndexSymbol, ref _string))
				{
					_onMsgObj.Deltas.Update((int)LocFieldNeams.wIssueSymbol, _string);

					if (msg.tryString("", LocFieldNeams.wLineTime, ref _string))
					{
						_onMsgObj.Deltas.Update((int)LocFieldNeams.wLineTime, _string);
					}

					if (msg.tryChar("", LocFieldNeams.wCondition, ref _char))
					{
						_onMsgObj.Deltas.Update((int)LocFieldNeams.wCondition, Convert.ToString(_char));
					}

					if (msg.tryString("", LocFieldNeams.wSecurityStatus, ref _string))
					{
						_onMsgObj.Deltas.Update((int)LocFieldNeams.wSecurityStatus, _string);
					}

					#region - Update Trade -

					if (msg.tryF64("", LocFieldNeams.wTradePrice, ref _double) || msg.tryF64("", LocFieldNeams.wIndexValue, ref _double))
					{
						_onMsgObj.Deltas.Update((int)LocFieldNeams.wTradePrice, _double);
						_hasTradedPrice = true;
						_updateMisc = true;
					}
					if (msg.tryI64("", LocFieldNeams.wTradeVolume, ref _long))
					{
						_onMsgObj.Deltas.Update((int)LocFieldNeams.wTradeVolume, _long);
						_hasTradedSize = true;
					}
					if (saveTradeAndPrice && _hasTradedPrice && _hasTradedSize)
					{
						// Save the Traded Price and Size
						_onMsgObj.Deltas.UpdateTraded(_double, _long);
					}

					if (msg.tryString("", LocFieldNeams.wTradeTime, ref _string))
					{
						_onMsgObj.Deltas.Update((int)LocFieldNeams.wTradeTime, _string);
					}
					if (msg.tryString("", LocFieldNeams.wTradeTick, ref _string))
					{
						_onMsgObj.Deltas.Update((int)LocFieldNeams.wTradeTick, _string);
					}
					if (msg.tryString("", LocFieldNeams.wTradePartId, ref _string))
					{
						_onMsgObj.Deltas.Update((int)LocFieldNeams.wTradePartId, _string);
					}
					if (msg.tryI32("", LocFieldNeams.wTradeCount, ref _int))
					{
						_onMsgObj.Deltas.Update((int)LocFieldNeams.wTradeCount, _int);
					}

					#endregion

					#region - Update Bid -

					if (msg.tryF64("", LocFieldNeams.wBidPrice, ref _double) || msg.tryF64("", LocFieldNeams.wIndexBid, ref _double))
					{
						_onMsgObj.Deltas.Update((int)LocFieldNeams.wBidPrice, _double);

						if (msg.tryF64("", LocFieldNeams.wBidHigh, ref _double))
						{
							_onMsgObj.Deltas.Update((int)LocFieldNeams.wBidHigh, _double);
						}
						if (msg.tryF64("", LocFieldNeams.wBidLow, ref _double))
						{
							_onMsgObj.Deltas.Update((int)LocFieldNeams.wBidLow, _double);
						}
					}

					if (msg.tryI64("", LocFieldNeams.wBidSize, ref _long))
					{
						_onMsgObj.Deltas.Update((int)LocFieldNeams.wBidSize, _long);
					}
					if (msg.tryString("", LocFieldNeams.wBidPartId, ref _string))
					{
						_onMsgObj.Deltas.Update((int)LocFieldNeams.wBidPartId, _string);
					}

					#endregion

					#region - Update Ask -

					if (msg.tryF64("", LocFieldNeams.wAskPrice, ref _double) || msg.tryF64("", LocFieldNeams.wIndexAsk, ref _double))
					{
						_onMsgObj.Deltas.Update((int)LocFieldNeams.wAskPrice, _double);

						if (msg.tryF64("", LocFieldNeams.wAskHigh, ref _double))
						{
							_onMsgObj.Deltas.Update((int)LocFieldNeams.wAskHigh, _double);
						}
						if (msg.tryF64("", LocFieldNeams.wAskLow, ref _double))
						{
							_onMsgObj.Deltas.Update((int)LocFieldNeams.wAskLow, _double);
						}
					}

					if (msg.tryI64("", LocFieldNeams.wAskSize, ref _long))
					{
						_onMsgObj.Deltas.Update((int)LocFieldNeams.wAskSize, _long);
					}
					if (msg.tryString("", LocFieldNeams.wAskPartId, ref _string))
					{
						_onMsgObj.Deltas.Update((int)LocFieldNeams.wAskPartId, _string);
					}

					#endregion

					#region - Update Other -

					if (msg.tryF64("", LocFieldNeams.wClosePrice, ref _double))
					{
						_onMsgObj.Deltas.Update((int)LocFieldNeams.wClosePrice, _double);

						_updateMisc = true;
					}

					if (msg.tryF64("", LocFieldNeams.wPrevClosePrice, ref _double))
					{
						_onMsgObj.Deltas.Update((int)LocFieldNeams.wPrevClosePrice, _double);

						_updateMisc = true;
					}

					if (msg.tryF64("", LocFieldNeams.wAdjPrevClose, ref _double))
					{
						_onMsgObj.Deltas.Update((int)LocFieldNeams.wAdjPrevClose, _double);

						_updateMisc = true;
					}

					if (msg.tryF64("", LocFieldNeams.wSettlePrice, ref _double))
					{
						_onMsgObj.Deltas.Update((int)LocFieldNeams.wSettlePrice, _double);

						_updateMisc = true;
					}

					if (msg.tryF64("", LocFieldNeams.wOpenPrice, ref _double))
					{
						_onMsgObj.Deltas.Update((int)LocFieldNeams.wOpenPrice, _double);

						_updateMisc = true;
					}

					#endregion

					#region - Update Misc -

					if (msg.tryF64("", LocFieldNeams.wHighPrice, ref _double) || msg.tryF64("", LocFieldNeams.wIndexHigh, ref _double))
					{
						_onMsgObj.Deltas.Update((int)LocFieldNeams.wHighPrice, _double);
						_updateMisc = true;
					}
					if (msg.tryF64("", LocFieldNeams.wLowPrice, ref _double) || msg.tryF64("", LocFieldNeams.wIndexLow, ref _double))
					{
						_onMsgObj.Deltas.Update((int)LocFieldNeams.wLowPrice, _double);
						_updateMisc = true;
					}

					if (_updateMisc)
					{
						if (msg.tryI64("", LocFieldNeams.wTotalVolume, ref _long))
						{
							_onMsgObj.Deltas.Update((int)LocFieldNeams.wTotalVolume, _long);
						}

						if (msg.tryF64("", LocFieldNeams.wTotalValue, ref _double))
						{
							_onMsgObj.Deltas.Update((int)LocFieldNeams.wTotalValue, _double);
						}
						if (msg.tryF64("", LocFieldNeams.wVwap, ref _double))
						{
							_onMsgObj.Deltas.Update((int)LocFieldNeams.wVwap, _double);
						}
						if (msg.tryF64("", LocFieldNeams.wNetChange, ref _double))
						{
							_onMsgObj.Deltas.Update((int)LocFieldNeams.wNetChange, _double);
						}
						if (msg.tryF64("", LocFieldNeams.wPctChange, ref _double))
						{
							_onMsgObj.Deltas.Update((int)LocFieldNeams.wPctChange, _double);
						}
					}

					#endregion

					TimeSpan ts = _dtHP.Now.Subtract(start);
					_onMsgObj.Deltas.Update(WombatFieldIDs.uDecodedTime, Math.Abs(ts.TotalMilliseconds));
					// On Msg Time
					_onMsgObj.Deltas.uOnMsgTime = start;
				}
			}

			#endregion

			#region - Critical Message Update -

			public void Update(MamaMsg msg, MamaMsgIterator msgIterator, bool useMamaFieldIterator)
			{
				lock (_onMsgObj.Deltas)
				{
					InteratMsg(msg, msgIterator, useMamaFieldIterator);
				}
			}

			private void InteratMsg(MamaMsg msg, MamaMsgIterator msgIterator, bool useMamaFieldIterator)
			{
				DateTime start = _dtHP.Now;

				if (useMamaFieldIterator)
				{
					DecodeByMamaField(msg, msgIterator);
				}
				else
				{
					DecodeByLocalField(msg);
				}

				if (_onMsgObj.Deltas.IssueSymbol != null && _onMsgObj.Deltas.IssueSymbol != "")
				{
					TimeSpan ts = _dtHP.Now.Subtract(start);
					_onMsgObj.Deltas.Update(WombatFieldIDs.uDecodedTime, ts.TotalMilliseconds);
				}
			}

			private void DecodeByLocalField(MamaMsg msg)
			{
				string locString = "";
				double locDouble = 0;
				long locLong = 0;
				foreach (MamaFieldDescriptor field in LocFields.Values)
				{
					switch (field.getType())
					{
						case mamaFieldType.MAMA_FIELD_TYPE_STRING:
							if (msg.tryString(field, ref locString))
							{
								_onMsgObj.Deltas.Update(field.getFid(), locString);
							}
							break;
						case mamaFieldType.MAMA_FIELD_TYPE_F64:
							if (msg.tryF64(field, ref locDouble))
							{
								_onMsgObj.Deltas.Update(field.getFid(), locDouble);
							}
							break;
						case mamaFieldType.MAMA_FIELD_TYPE_I64:
							if (msg.tryI64(field, ref locLong))
							{
								_onMsgObj.Deltas.Update(field.getFid(), locLong);
							}
							break;
					}
				}
			}

			private void DecodeByMamaField(MamaMsg msg, MamaMsgIterator msgIterator)
			{
				MamaMsgField field;
				int fid = 0;

				try
				{
					msg.begin(ref msgIterator);
					while ((field = msgIterator.getField()) != null)
					{
						fid = field.getFid();
						if (MDOutputFieldIDs.FNames.ContainsKey(fid))
						{
							switch (field.getType())
							{
								case mamaFieldType.MAMA_FIELD_TYPE_CHAR:
									_onMsgObj.Deltas.Update(fid, Convert.ToString(field.getChar()));
									break;
								case mamaFieldType.MAMA_FIELD_TYPE_STRING:
									_onMsgObj.Deltas.Update(fid, field.getString());
									break;
								case mamaFieldType.MAMA_FIELD_TYPE_F64:
									_onMsgObj.Deltas.Update(fid, field.getF64());
									break;
								case mamaFieldType.MAMA_FIELD_TYPE_I64:
									_onMsgObj.Deltas.Update(fid, field.getI64());
									break;
								case mamaFieldType.MAMA_FIELD_TYPE_I32:
									_onMsgObj.Deltas.Update(fid, field.getI32());
									break;
							}
						}

						msgIterator++;
					}
				}
				catch (Exception ex)
				{
					if (Debugger.IsAttached)
					{
						Debug.Print(ex.Message);
						Debug.Print(ex.StackTrace);
					}
				}
			}

			#endregion

			#region - Books -

			public void UpdateBook(OnMsgObject msg)
			{
				foreach (double price in msg.AskBooks.Keys)
				{
					if (_onMsgObj.AskBooks.ContainsKey(price))
					{
						_onMsgObj.AskBooks[price] = msg.AskBooks[price];
					}
					else
					{
						_onMsgObj.AskBooks.Add(price, msg.AskBooks[price]);
					}
				}

				foreach (double price in msg.BidBooks.Keys)
				{
					if (_onMsgObj.BidBooks.ContainsKey(price))
					{
						_onMsgObj.BidBooks[price] = msg.BidBooks[price];
					}
					else
					{
						_onMsgObj.BidBooks.Add(price, msg.BidBooks[price]);
					}
				}
			}

			public void UpdateBook(MamaMsg msg, MamaMsgIterator msgIterator)
			{
				try
				{
					if (_onMsgObj != null && msg != null)
					{
						lock (msg)
						{
							msg.begin(ref msgIterator);
							while ((_msgField = msgIterator.getField()) != null)
							{
								_fieldType = _msgField.getType();
								switch (_fieldType)
								{
									case mamaFieldType.MAMA_FIELD_TYPE_VECTOR_MSG:
										_msgArray = _msgField.getVectorMsg();

										if (_msgArray.Length > 0 && 
											_msgArray[0].tryI64("", LocFieldNeams.wPlSide, ref _long) &&
											_msgArray[0].tryF64("", LocFieldNeams.wPlPrice, ref _double) &&
											_msgArray[0].tryI64("", LocFieldNeams.wPlSize, ref _size))
										{
											_double = Math.Round(_double, 7);
											switch (_long)
											{
												case 65:
													if (_onMsgObj.AskBooks.ContainsKey(_double))
													{
														_onMsgObj.AskBooks[_double] = _size;
													}
													else
													{
														_onMsgObj.AskBooks.Add(_double, _size);
													}
													break;
												case 66:
													if (_onMsgObj.BidBooks.ContainsKey(_double))
													{
														_onMsgObj.BidBooks[_double] = _size;
													}
													else
													{
														_onMsgObj.BidBooks.Add(_double, _size);
													}
													break;
											}
										}
										break;
								}

								msgIterator++;
							}
						}
					}
				}
				catch (Exception ex)
				{
					Debug.Assert(false, ex.Message, ex.StackTrace);
				}
			}

			public void UpdateBook(MamdaOrderBook book)
			{
				GetBidLevels(book.getBidLevels());
				GetAskLevels(book.getAskLevels());
			}

			private void GetBidLevels(IEnumerable levels)
			{
				double _double = 0;
				int _count = 0;

				_onMsgObj.BidBooks.Clear();
				foreach (MamdaOrderBookPriceLevel level in levels)
				{
					if (_count > _bookDepthLimit)
					{
						break;
					}

					_double = level.getPrice().getValue();

					if (_onMsgObj.BidBooks.ContainsKey(_double))
					{
						_onMsgObj.BidBooks[_double] = level.getSize();
					}
					else
					{
						_onMsgObj.BidBooks.Add(_double, level.getSize());
					}

					_count++;
				}
			}
			
			private void GetAskLevels(IEnumerable levels)
			{
				double _double = 0;
				int _count = 0;

				_onMsgObj.AskBooks.Clear();
				foreach (MamdaOrderBookPriceLevel level in levels)
				{
					if (_count > _bookDepthLimit)
					{
						break;
					}

					_double = level.getPrice().getValue();

					if (_onMsgObj.AskBooks.ContainsKey(_double))
					{
						_onMsgObj.AskBooks[_double] = level.getSize();
					}
					else
					{
						_onMsgObj.AskBooks.Add(_double, level.getSize());
					}

					_count++;
				}
			}

			#endregion
		}

		private class OnMsgObject
		{
			private DateTime _recivedTime;
			public DateTime RecivedTime
			{
				get
				{
					return _recivedTime;
				}
			}

			private string _source = "";
			public string Source
			{
				get
				{
					return _source;
				}
			}

			private string _symbol = "";
			public string Symbol
			{
				get
				{
					return _symbol;
				}
			}

			private object _closure;
			public object Closure
			{
				get
				{
					return _closure;
				}
			}

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

			private MDServerToClient _deltas;
			public MDServerToClient Deltas
			{
				get
				{
					if (_deltas == null)
					{
						_deltas = new MDServerToClient();
					}
					return _deltas;
				}
				set
				{
					_deltas = value;
				}
			}

			public OnMsgObject(string source, string symbol, object closure)
			{
				Initialize(source, symbol, closure);
			}

			private void Initialize(string source, string symbol, object closure)
			{
				_recivedTime = DateTime.Now;

				_source = source;
				_symbol = symbol;
				_closure = closure;
			}
		}

		#endregion

		#region - Local Variable -

		private DateTimeHP _dtHP;

		private string _middleware;
		private int _maxRetries;
		private int _idelTimeBetweenRetries;
		private bool _enableMamaLog;
		private string _mamaLogFilePath;
		private string _transportName;
		private string _dictionarySourceSymbolNamespace;
		private int _throttleDefault;
		private int _throttleInitial;
		private int _throttleRecap;
		private bool _setRecoverGaps = false;

		private double _dictionaryTimeOut = 15;
		private int _dictionaryRetries = 0;

		private double _subscriptionTimeOut = 15;
		private int _subscriptionRetries = 0;

		private MamaBridge _mamaBridge;
		private MamaTransport _mamaTransport;
		private MamaQueue _mamaDefaultQueue;

		private bool _gotDictionaries;
		private MamaSource _mamaDictionarySource;
		private MamaDictionary _mamaDictionary;

		private MamaMsgIterator _mainMsgIterator = null;
		private bool _running;

		// Used the default mama fields, else use local defined fields
		private bool _useMamaFieldIterator = true;

		// Notification Rate Update Rate
		private bool _useChangeNotificationRate = true;
		private uint _changeNotificationRate = 100;

		private bool _forceGC = false;

		// WaitFor
		private string _waitingForSymbol = "";
		private bool _useWaitForSymbol = false;

		// Book Limit
		private int _bookDepthLimit = 20;

		// Key to different sources by string
		private Dictionary<string, MamaSource> _mamaSources = new Dictionary<string, MamaSource>();

		// Key to different symbols + source by string
		private Dictionary<string, MamaSubscription> _mamaSubscriptions = new Dictionary<string, MamaSubscription>();

		// Key to different book Symbols by String
		private Dictionary<string, MamdaSubscription> _mamdaSubscriptions = new Dictionary<string, MamdaSubscription>();

		// Sources by Type
		private List<string> _stockSources = new List<string>();
		private List<string> _futureSources = new List<string>();
		private List<string> _optionSources = new List<string>();

		// Raw queue Limit
		private int _rawQueueCount = 0;
		private int _rawQueueLimit = 1000;

		private bool _logDroppedMessages = false;
		private long _dropedMsgCount = 0;

		#endregion

		#region - Public Properties -

		// For Play back and resubscribe only 
		// Key = Symbol + Source
		private Dictionary<string, SubscriptionFields> _subscriptions = new Dictionary<string, SubscriptionFields>();
		public Dictionary<string, SubscriptionFields> Subscriptions
		{
			get
			{
				return _subscriptions;
			}
		}

		// Key to different symbols + source by string (onCreate)
		private List<string> _subscriptionsCreated = new List<string>();
		public List<string> SubscriptionsCreated
		{
			get
			{
				return _subscriptionsCreated;
			}
		}

		#endregion

		#region - Source and Symbol Subscription Filter -

		private List<string> _soruceList = new List<string>();
		private List<string> _symbolList = new List<string>();

		#endregion

		#region - Constructor -

		public LBMBase(
			string middleware, 
			int maxRetries, 
			uint idelTimeBetweenRetries, 
			bool enableMamaLog, 
			string mamaLogFilePath, 
			string transportName, 
			string dictionarySourceSymbolNamespace, 
			int throttleDefault, 
			int throttleInitial, 
			int throttleRecap, 
			int changeNotificationRate, 
			bool useChangeNotificationRate,
			bool useWaitForSymbol,
			int mamaMsgQueueLimitPerSymbol,
			string sourceRange,
			string symbolRange,
			int bookDepthLimit,
			string stockSources,
			string futureSources,
			string optionSources,
			bool setRecoverGaps,
			int rawQueueLimit,
			bool logDroppedMessages)
		{
			_middleware = middleware;
			_maxRetries = maxRetries;
			_idelTimeBetweenRetries = (int)idelTimeBetweenRetries;
			_enableMamaLog = enableMamaLog;
			_mamaLogFilePath = mamaLogFilePath;
			_transportName = transportName;
			_dictionarySourceSymbolNamespace = dictionarySourceSymbolNamespace;
			_throttleDefault = throttleDefault;
			_throttleInitial = throttleInitial;
			_throttleRecap = throttleRecap;
			_setRecoverGaps = setRecoverGaps;
			_rawQueueLimit = rawQueueLimit;
			_logDroppedMessages = logDroppedMessages;

			// maxium mama msg queue limit per symbol
			if (mamaMsgQueueLimitPerSymbol > 5)
			{
				_mamaMsgQueueLimitPerSymbol = mamaMsgQueueLimitPerSymbol;
			}
			// accepted sources
			if (sourceRange != "" && sourceRange.ToUpper() != "ALL")
			{
				string[] sources = sourceRange.ToUpper().Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries);
				if (sources.Length > 0)
				{
					_soruceList.AddRange(sources);
				}
			}
			// accepted symbols
			if (symbolRange != "" && symbolRange.ToUpper() != "ALL")
			{
				string[] symbols = symbolRange.ToUpper().Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries);
				if (symbols.Length > 0)
				{
					_symbolList.AddRange(symbols);
				}
			}
			// stock sources
			if (stockSources != "" && stockSources.ToUpper() != "ALL")
			{
				string[] sources = stockSources.ToUpper().Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries);
				if (sources.Length > 0)
				{
					_stockSources.AddRange(sources);
				}
			}
			else
			{
				_stockSources.Add("CTA");
				_stockSources.Add("NASDAQ");
			}
			// future sources
			if (futureSources != "" && futureSources.ToUpper() != "ALL")
			{
				string[] sources = futureSources.ToUpper().Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries);
				if (sources.Length > 0)
				{
					_futureSources.AddRange(sources);
				}
			}
			else
			{
				_futureSources.Add("CME");
				_futureSources.Add("ICE");
			}
			// option sources
			if (optionSources != "" && optionSources.ToUpper() != "ALL")
			{
				string[] sources = optionSources.ToUpper().Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries);
				if (sources.Length > 0)
				{
					_optionSources.AddRange(sources);
				}
			}
			else
			{
				_optionSources.Add("OPRA");
				_optionSources.Add("OPRANBBO");
			}


			_changeNotificationRate = (uint)changeNotificationRate;
			_useChangeNotificationRate = useChangeNotificationRate;

			_useWaitForSymbol = useWaitForSymbol;

			_bookDepthLimit = bookDepthLimit;

			Initialize();
		}
		
		private void Initialize()
		{
			_dtHP = new DateTimeHP();

			_mamaBridge = null;
			_mamaDefaultQueue = null;

			_gotDictionaries = false;
			_mamaDictionarySource = null;
			_mamaDictionary = null;

			_running = false;
		}

		#endregion

		#region - Public Method -

		public void Start()
		{
			bool redo = false;
			int retries = 0;

			#region - Mama Bridge -

			do
			{
				try
				{
					_mamaBridge = new MamaBridge(_middleware);
					Mama.open();
					redo = false;
				}
				catch (Exception ex)
				{
					if (retries < _maxRetries)
					{
						System.Threading.Thread.Sleep(_idelTimeBetweenRetries);
						redo = true;
						++retries;
					}
					else
					{
						OnLBMChanged(new LBMDataChangedEventArgs(LBMDataChangedEventTypes.OnExecption, String.Concat(new object[] { "OnStart.Open() ", ex.Message + ": " + ex.StackTrace })));
					}
				}
			}
			while (redo && retries < _maxRetries);

			#endregion

			#region - Mama Log -

			if (_enableMamaLog)
			{
				Mama.logToFile(String.Format(@"C:\WOMBAT_{0:G}{1:G3}.log", DateTime.Today.Year, DateTime.Today.DayOfYear), MamaLogLevel.MAMA_LOG_LEVEL_FINEST);
			}

			#endregion

			#region - Mama Transport -

			_mamaTransport = new MamaTransport();
			_mamaTransport.create(_transportName, _mamaBridge);

			#endregion

			#region - Mama Queue -

			_mamaDefaultQueue = Mama.getDefaultEventQueue(_mamaBridge);

			#endregion

			#region - Mama Dictionary -

			_gotDictionaries = false;
			_mamaDictionarySource = new MamaSource();

			_mamaDictionarySource.symbolNamespace = _dictionarySourceSymbolNamespace;
			_mamaDictionarySource.transport = _mamaTransport;
			_mamaDictionary = new MamaDictionary();

			_mamaDictionary.create(_mamaDefaultQueue, this, _mamaDictionarySource, _dictionaryRetries, _dictionaryTimeOut);

			#endregion

			try
			{
				Mama.start(_mamaBridge);

				if (!_gotDictionaries)
				{
					OnLBMChanged(new LBMDataChangedEventArgs(LBMDataChangedEventTypes.OnInitializeFailed));
				}
				else
				{
					MamdaOrderBookFields.setDictionary(_mamaDictionary, null);

					// Set Iterator
					_mainMsgIterator = new MamaMsgIterator(_mamaDictionary);

					OnLBMChanged(new LBMDataChangedEventArgs(LBMDataChangedEventTypes.OnInitialized));

					_mamaTransport.setOutboundThrottle(MamaTransport.MamaThrottleInstance.MAMA_THROTTLE_DEFAULT, _throttleDefault);
					_mamaTransport.setOutboundThrottle(MamaTransport.MamaThrottleInstance.MAMA_THROTTLE_INITIAL, _throttleInitial);
					_mamaTransport.setOutboundThrottle(MamaTransport.MamaThrottleInstance.MAMA_THROTTLE_RECAP, _throttleRecap);

					_running = true;

					StartDeltaNotifyThread();
					StartDecodeThread();

					Mama.start(_mamaBridge);
				}
			}
			catch (Exception ex)
			{
				OnLBMChanged(new LBMDataChangedEventArgs(LBMDataChangedEventTypes.OnExecption, String.Concat(new object[] { "OnStart.Start() ", ex.Message + ": " + ex.StackTrace })));
			}
		}

		public void SubscribeBook(string symbol, string source, bool reload)
		{
			if (PreSubscribeCheck(symbol, source))
			{
				subscribeBook(symbol, source, reload);
			}
		}

		public void SubscribeL2(string symbol, string source, bool reload)
		{
			if (PreSubscribeCheck(symbol, source))
			{
				subscribe(symbol + ".GRP", source, mamaSubscriptionType.MAMA_SUBSC_TYPE_GROUP, reload);
			}
		}

		public void Subscribe(string symbol, string source, bool reload)
		{
			if (PreSubscribeCheck(symbol, source))
			{
				subscribe(symbol, source, mamaSubscriptionType.MAMA_SUBSC_TYPE_NORMAL, reload);
			}
		}

		public void Unsubscribe(string symbol)
		{
			lock (_mamaSubscriptions)
			{
				if (_mamaSubscriptions.ContainsKey(symbol))
				{
					_mamaSubscriptions[symbol].destroy();
					_mamaSubscriptions[symbol].Dispose();

					_mamaSubscriptions.Remove(symbol);
				}
			}
		}

		public void ForceGC()
		{
			_forceGC = true;
		}

		private void subscribe(string symbol, string source, mamaSubscriptionType type, bool reload)
		{
			if (_running)
			{
				#region - Source -

				MamaSource src;
				lock (_mamaSources)
				{
					if (_mamaSources.ContainsKey(source))
					{
						src = _mamaSources[source];
					}
					else
					{
						src = new MamaSource();
						src.symbolNamespace = source;
						src.transport = _mamaTransport;

						// remember the source
						_mamaSources.Add(source, src);
					}
				}

				#endregion

				string key = GetSubscribeKey(symbol, source);
				//if (_mamaSubscriptions.ContainsKey(key))
				//{
				//    if (_mamaSubscriptions[key].subscSource != source)
				//    {
				//        reload = true;
				//    }
				//}

				if (!_mamaSubscriptions.ContainsKey(key) || reload)
				{
					#region - Subscription -

					MamaSubscription sub = new MamaSubscription();

					sub.setServiceLevel(mamaServiceLevel.MAMA_SERVICE_LEVEL_REAL_TIME);
					sub.setSubscriptionType(type);
					sub.setTimeout(_subscriptionTimeOut);
					sub.setRetries(_subscriptionRetries);
					if (_middleware == "lbm")
					{
						sub.setRecoverGaps(_setRecoverGaps);
					}
					sub.setRequiresInitial(true);

					try
					{
						sub.create(_mamaDefaultQueue, this, src, symbol, type);
					}
					catch (Exception ex)
					{
						OnLBMChanged(new LBMDataChangedEventArgs(LBMDataChangedEventTypes.OnSubscriptionFailed, String.Concat(new object[] { "Wombat Exception " + symbol + ": " + source + " " + ex.Message})));
					}

					#endregion

					lock (_mamaSubscriptions)
					{
						if (_mamaSubscriptions.ContainsKey(key))
						{
							// Resubscribe
							_mamaSubscriptions[key] = sub;
						}
						else
						{
							_mamaSubscriptions.Add(key, sub);
						}

						lock (_subscriptions)
						{
							if (_subscriptions.ContainsKey(key))
							{
								_subscriptions[key] = new SubscriptionFields(symbol, source, "");
							}
							else
							{
								_subscriptions.Add(key, new SubscriptionFields(symbol, source, ""));
							}
						}

						if (_useWaitForSymbol && !symbol.Contains("."))
						{
							_waitingForSymbol = symbol;
							if (WaitForSubscriptionCompleted())
							{
								OnLBMChanged(new LBMDataChangedEventArgs(LBMDataChangedEventTypes.OnSubscriptionFailed, String.Concat(new object[] { "Missing First Msg " + symbol + ": " + source })));
							}
						}
					}
				}
			}
			else
			{
				OnLBMChanged(new LBMDataChangedEventArgs(LBMDataChangedEventTypes.OnSubscriptionFailed, String.Concat(new object[] { "Wombat Not Running " + symbol + ": " + source })));
			}
		}

		private void subscribeBook(string symbol, string source, bool reload)
		{
			string key = GetSubscribeKey(symbol, source);

			#region - Subscription -

			MamdaSubscription bookSubscription = new MamdaSubscription();
			MamdaOrderBookListener bookListener = new MamdaOrderBookListener();

			bookListener.addHandler(this);
			bookSubscription.addMsgListener(bookListener);

			bookSubscription.setType(mamaSubscriptionType.MAMA_SUBSC_TYPE_BOOK);
			bookSubscription.setServiceLevel(mamaServiceLevel.MAMA_SERVICE_LEVEL_REAL_TIME, 0);
			bookSubscription.setTimeout(_subscriptionTimeOut);
			bookSubscription.setRetries(_subscriptionRetries);
			bookSubscription.setRequireInitial(true);

			#endregion

			bookSubscription.create(_mamaTransport, _mamaDefaultQueue, source, symbol, mamaSubscriptionType.MAMA_SUBSC_TYPE_BOOK);

			lock (_mamdaSubscriptions)
			{
				if (_mamdaSubscriptions.ContainsKey(key))
				{
					// Resubscribe
					_mamdaSubscriptions[key] = bookSubscription;
				}
				else
				{
					_mamdaSubscriptions.Add(key, bookSubscription);
				}

				lock (_subscriptions)
				{
					if (_subscriptions.ContainsKey(key))
					{
						_subscriptions[key] = new SubscriptionFields(symbol, source, "BOOK");
					}
					else
					{
						_subscriptions.Add(key, new SubscriptionFields(symbol, source, "BOOK"));
					}
				}
			}
		}

		private bool PreSubscribeCheck(string symobl, string source)
		{
			bool sourceOk = false;
			bool symbolOk = false;

			if (_soruceList.Count > 0)
			{
				if (_soruceList.Contains(source))
				{
					sourceOk = true;
				}
			}
			else
			{
				sourceOk = true;
			}

			if (_symbolList.Count > 0)
			{
				if (_symbolList.Contains(symobl.Substring(0, 1)))
				{
					symbolOk = true;
				}
				else if (symobl.Length > 2 && _symbolList.Contains(symobl.Substring(0, 2)))
				{
					symbolOk = true;
				}
			}
			else
			{
				symbolOk = true;
			}

			if (sourceOk && symbolOk)
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		private string GetSubscribeKey(string symbol, string source)
		{
			return string.Format("{0},{1}", new object[] { symbol, source });
		}

		#endregion

		#region - Local Method -

		private void Stop()
		{
			if (_running)
			{
				_running = false;

				StopDecodeThread();
				StopDeltaNotifyThread();

				_mamaSubscriptions = new Dictionary<string, MamaSubscription>();
				_mamdaSubscriptions = new Dictionary<string, MamdaSubscription>();
				_mamaSources = new Dictionary<string, MamaSource>();

				_subscriptions.Clear();
				_subscriptionsCreated.Clear();
				_waitingForSymbol = "";

				_gotDictionaries = false;
				_running = false;

				Mama.stop(_mamaBridge);

				Initialize();

				OnLBMChanged(new LBMDataChangedEventArgs(LBMDataChangedEventTypes.OnStop));

				MemClean();
			}
		}

		private void ResizeRawBuffer()
		{
			MamaMsgQueue[] msgArray = _mamaMsgQueueRaw.ToArray();
			_mamaMsgQueueRaw.Clear();
			msgArray = RemoveFromArray(msgArray, 0, _rawQueueLimit / 2);
			_mamaMsgQueueRaw.AddRange(msgArray);
			_rawQueueCount = _mamaMsgQueueRaw.Count;
		}

		private MamaMsgQueue[] RemoveFromArray(MamaMsgQueue[] sourceArray, long startIndex, long count)
		{
			MamaMsgQueue[] msgArrayResult = new MamaMsgQueue[sourceArray.Length - count];
			Array.Copy(sourceArray, count, msgArrayResult, 0, msgArrayResult.Length);
			return msgArrayResult;
		}

		#endregion

		#region - MamaDictionaryCallback Members -

		public void onComplete(MamaDictionary dictionary)
		{
			_gotDictionaries = true;
			Mama.stop(_mamaBridge);
		}

		public void onError(MamaDictionary dictionary, string message)
		{
			OnLBMChanged(new LBMDataChangedEventArgs(LBMDataChangedEventTypes.OnInitializeError, String.Concat(new object[] { "MamaDictionary:Error " + message })));
			Mama.stop(_mamaBridge);
		}

		public void onTimeout(MamaDictionary dictionary)
		{
			OnLBMChanged(new LBMDataChangedEventArgs(LBMDataChangedEventTypes.OnInitializeError, "MamaDictionary:Timeout " + dictionary.ToString()));
			Mama.stop(_mamaBridge);
		}

		#endregion

		#region - MamaSubscriptionCallback Members -

		public void onCreate(MamaSubscription subscription)
		{
			lock (_subscriptionsCreated)
			{
				string key = GetSubscribeKey(subscription.subscSymbol, subscription.subscSource);
				if (!_subscriptionsCreated.Contains(key))
				{
					_subscriptionsCreated.Add(key);
				}
			}
			//OnLBMChanged(new LBMDataChangedEventArgs(LBMDataChangedEventTypes.OnSubscribed, String.Concat(new object[] { "MamaSubscription|onCreate ", subscription.subscSymbol, " : ", subscription.subscSource })));
		}

		public void onError(MamaSubscription subscription, MamaStatus.mamaStatus status, string subject)
		{
			OnLBMChanged(new LBMDataChangedEventArgs(LBMDataChangedEventTypes.OnSubscriptionError, String.Concat(new object[] { "MamaSubscription|onError ", subscription.subscSymbol, " : ", subscription.subscSource, " Status: " + status.ToString(), " Subject: ", subject })));
		}

		public void onGap(MamaSubscription subscription)
		{
			//OnLBMChanged(new LBMDataChangedEventArgs(LBMDataChangedEventTypes.OnSubscriptionGap, String.Concat(new object[] { "MamaSubscription|onGap ", subscription.subscSymbol, " : ", subscription.subscSource })));
		}

		public void onMsg(MamaSubscription subscription, MamaMsg msg)
		{
			try
			{
				if ((mamaSubscriptionType)subscription.subscClosure == mamaSubscriptionType.MAMA_SUBSC_TYPE_BOOK)
				{
					if (_useChangeNotificationRate)
					{
						CopyMamaBookMessages(subscription, msg);
					}
					else
					{
						NotifyMamaBookMessage(subscription, msg);
					}
				}
				else
				{
					if (_useChangeNotificationRate)
					{
						CopyMamaMessages(subscription, msg);
					}
					else
					{
						NotifyMamaMessages(subscription, msg);
					}
				}
			}
			catch (Exception ex)
			{
				OnLBMChanged(new LBMDataChangedEventArgs(LBMDataChangedEventTypes.OnWarring, String.Concat(new object[] { "MamaSubscription|onMsg", ex.Message, " : ", ex.StackTrace })));
			}
		}

		private void CopyMamaBookMessagesRaw(MamaSubscription subscription, MamaMsg msg)
		{
			lock (_syncObj)
			{
				if (_rawQueueCount < _rawQueueLimit)
				{
					MamaMsgQueue queue = new MamaMsgQueue(_bookDepthLimit);
					queue.OnMsgObj = new OnMsgObject(subscription.subscSource, subscription.subscSymbol, subscription.subscClosure);
					queue.UpdateBook(msg, new MamaMsgIterator(_mamaDictionary));
					_mamaMsgQueueRaw.Add(queue);
					_rawQueueCount++;

					if (_logDroppedMessages && _dropedMsgCount > 0)
					{
						OnLBMChanged(new LBMDataChangedEventArgs(LBMDataChangedEventTypes.OnWarring, String.Concat(new object[] { "Dropped Msg Count ", _dropedMsgCount })));
						_dropedMsgCount = 0;
					}
				}
				else
				{
					if (_logDroppedMessages)
					{
						_dropedMsgCount++;
					}
				}
			}
		}

		private void CopyMamaMessagesRaw(MamaSubscription subscription, MamaMsg msg)
		{
			lock (_syncObj)
			{
				if (_rawQueueCount < _rawQueueLimit)
				{
					MamaMsgQueue queue = new MamaMsgQueue(_bookDepthLimit);
					queue.OnMsgObj = new OnMsgObject(subscription.subscSource, subscription.subscSymbol.Replace(".IDX", ""), subscription.subscClosure);
					queue.Update(msg, (_stockSources.Contains(subscription.subscSource) || _futureSources.Contains(subscription.subscSource)));
					_mamaMsgQueueRaw.Add(queue);
					_rawQueueCount++;

					if (_logDroppedMessages && _dropedMsgCount > 0)
					{
						OnLBMChanged(new LBMDataChangedEventArgs(LBMDataChangedEventTypes.OnWarring, String.Concat(new object[] { "Dropped Msg Count ", _dropedMsgCount })));
						_dropedMsgCount = 0;
					}
				}
				else
				{
					if (_logDroppedMessages)
					{
						_dropedMsgCount++;
					}
				}
			}
		}

		private void NotifyMamaBookMessage(MamaSubscription subscription, MamaMsg msg)
		{
			string locOnMsgSubscriptionSymbol = "";
			string key = GetSubscribeKey(subscription.subscSymbol, subscription.subscSource);

			if (msg.tryString("", LocFieldNeams.wIssueSymbol, ref locOnMsgSubscriptionSymbol) || msg.tryString("", LocFieldNeams.wIndexSymbol, ref locOnMsgSubscriptionSymbol))
			{
				// has issuesymbol
			}
			else
			{
				locOnMsgSubscriptionSymbol = subscription.subscSymbol;
			}

			MamaMsgQueue queue = new MamaMsgQueue(_bookDepthLimit);
			queue.OnMsgObj = new OnMsgObject(subscription.subscSource, locOnMsgSubscriptionSymbol, subscription.subscClosure);
			queue.UpdateBook(msg, new MamaMsgIterator(_mamaDictionary));

			// Decode Without Update Rate
			PickMsg(queue.OnMsgObj);
		}

		private void NotifyMamaMessages(MamaSubscription subscription, MamaMsg msg)
		{
			string locOnMsgSubscriptionSymbol = "";
			string key = GetSubscribeKey(subscription.subscSymbol, subscription.subscSource);

			if (msg.tryString("", LocFieldNeams.wIssueSymbol, ref locOnMsgSubscriptionSymbol) || msg.tryString("", LocFieldNeams.wIndexSymbol, ref locOnMsgSubscriptionSymbol))
			{
				if (locOnMsgSubscriptionSymbol != "")
				{
					locOnMsgSubscriptionSymbol = subscription.subscSymbol;
					locOnMsgSubscriptionSymbol.Replace(".IDX", "");
				}

				if (IsCritical(locOnMsgSubscriptionSymbol))
				{
					MamaMsgQueue queue = new MamaMsgQueue(_bookDepthLimit);
					queue.OnMsgObj = new OnMsgObject(subscription.subscSource, locOnMsgSubscriptionSymbol, subscription.subscClosure);
					queue.Update(msg, new MamaMsgIterator(_mamaDictionary), _useMamaFieldIterator);

					// Must do full playback
					PickMsg(queue.OnMsgObj);
				}
				else
				{
					MamaMsgQueue queue = new MamaMsgQueue(_bookDepthLimit);
					queue.OnMsgObj = new OnMsgObject(subscription.subscSource, locOnMsgSubscriptionSymbol, subscription.subscClosure);
					queue.Update(msg, (_stockSources.Contains(subscription.subscSource) || _futureSources.Contains(subscription.subscSource)));

					// Decode Without Update Rate
					PickMsg(queue.OnMsgObj);
				}
			}
			else
			{
				// Not a valid message
			}
		}

		private void CopyMamaBookMessages(MamaSubscription subscription, MamaMsg msg)
		{
			string locOnMsgSubscriptionSymbol = "";
			string key = GetSubscribeKey(subscription.subscSymbol, subscription.subscSource);

			if (msg.tryString("", LocFieldNeams.wIssueSymbol, ref locOnMsgSubscriptionSymbol) || msg.tryString("", LocFieldNeams.wIndexSymbol, ref locOnMsgSubscriptionSymbol))
			{
				// has issuesymbol
			}
			else
			{
				locOnMsgSubscriptionSymbol = subscription.subscSymbol;
			}

			IsCritical(locOnMsgSubscriptionSymbol);

			if (_useChangeNotificationRate)
			{
				lock (_syncObj)
				{
					if (!_mamaMsgQueue.ContainsKey(key))
					{
						_mamaMsgQueue.Add(key, new MamaMsgQueue(_bookDepthLimit));
						_mamaMsgQueue[key].Add(new OnMsgObject(subscription.subscSource, locOnMsgSubscriptionSymbol, subscription.subscClosure));
					}

					// Convert To Books
					_mamaMsgQueue[key].UpdateBook(msg, new MamaMsgIterator(_mamaDictionary));
				}
			}
			else
			{
				MamaMsgQueue queue = new MamaMsgQueue(_bookDepthLimit);
				queue.OnMsgObj = new OnMsgObject(subscription.subscSource, locOnMsgSubscriptionSymbol, subscription.subscClosure);
				queue.UpdateBook(msg, new MamaMsgIterator(_mamaDictionary));

				// Decode Without Update Rate
				PickMsg(queue.OnMsgObj);
			}
		}

		private void CopyMamaMessages(MamaSubscription subscription, MamaMsg msg)
		{
			string locOnMsgSubscriptionSymbol = "";
			string key = GetSubscribeKey(subscription.subscSymbol, subscription.subscSource);

			if (msg.tryString("", LocFieldNeams.wIssueSymbol, ref locOnMsgSubscriptionSymbol) || msg.tryString("", LocFieldNeams.wIndexSymbol, ref locOnMsgSubscriptionSymbol))
			{
				if (locOnMsgSubscriptionSymbol != "")
				{
					locOnMsgSubscriptionSymbol = subscription.subscSymbol;
					locOnMsgSubscriptionSymbol.Replace(".IDX", "");
				}

				if (IsCritical(locOnMsgSubscriptionSymbol))
				{
					lock (_syncObj)
					{
						MamaMsgQueue queue = new MamaMsgQueue(_bookDepthLimit);
						queue.OnMsgObj = new OnMsgObject(subscription.subscSource, locOnMsgSubscriptionSymbol, subscription.subscClosure);
						queue.Update(msg, new MamaMsgIterator(_mamaDictionary), _useMamaFieldIterator);

						// Must do full playback
						_mamaCriticalMsgQueue.Add(queue);
					}
				}
				else
				{
					if (_useChangeNotificationRate)
					{
						// Not First Update
						lock (_syncObj)
						{
							if (!_mamaMsgQueue.ContainsKey(key))
							{
								_mamaMsgQueue.Add(key, new MamaMsgQueue(_bookDepthLimit));
								_mamaMsgQueue[key].Add(new OnMsgObject(subscription.subscSource, locOnMsgSubscriptionSymbol, subscription.subscClosure));
							}

							// Covert To Deltas
							_mamaMsgQueue[key].Update(msg, (_stockSources.Contains(subscription.subscSource) || _futureSources.Contains(subscription.subscSource)));
						}
					}
					else
					{
						MamaMsgQueue queue = new MamaMsgQueue(_bookDepthLimit);
						queue.OnMsgObj = new OnMsgObject(subscription.subscSource, locOnMsgSubscriptionSymbol, subscription.subscClosure);
						queue.Update(msg, (_stockSources.Contains(subscription.subscSource) || _futureSources.Contains(subscription.subscSource)));

						// Decode Without Update Rate
						PickMsg(queue.OnMsgObj);
					}
				}
			}
			else
			{
				// Not a valid message
			}
		}

		public void onQuality(MamaSubscription subscription, mamaQuality quality, string symbol)
		{
			//OnLBMChanged(new LBMDataChangedEventArgs(LBMDataChangedEventTypes.OnSubscriptionQuality, String.Concat(new object[] { "MamaSubscription|onQuality ", symbol, " : ", subscription.subscSource, " Quality: " + quality.ToString() })));
		}

		public void onRecapRequest(MamaSubscription subscription)
		{
			//OnLBMChanged(new LBMDataChangedEventArgs(LBMDataChangedEventTypes.OnSubscriptionRecapRequest, String.Concat(new object[] { "MamaSubscription|onRecapRequest ", subscription.subscSymbol, " : ", subscription.subscSource })));
		}

		private bool IsCritical(string symbol)
		{
			// Subscription blocker reset on first message recieved
			if (_waitingForSymbol == symbol)
			{
				_waitingForSymbol = "";
			}

			lock (_knownSymbols)
			{
				if (!_knownSymbols.ContainsKey(symbol))
				{
					_knownSymbols.Add(symbol, new MamaMsgQueue(_bookDepthLimit));

					if (symbol.Length > 0 && symbol.Substring(0, 1) != "b")
					{
						// Is the First Msg
						return true;
					}
				}
			}

			return false;
		}

		#endregion

		#region - MamdaOrderBookHandler Members -

		public void onBookClear(MamdaSubscription subscription, MamdaOrderBookListener listener, MamaMsg msg, MamdaOrderBookDelta delta, MamdaOrderBookRecap fullBook)
		{
			//OnLBMChanged(new LBMDataChangedEventArgs(LBMDataChangedEventTypes.OnBookClear, String.Concat(new object[] { "MamdaSubscription|OnBookClear ", subscription.getSymbol(), " : ", subscription.getSource() })));
		}

		public void onBookDelta(MamdaSubscription subscription, MamdaOrderBookListener listener, MamaMsg msg, MamdaOrderBookDelta delta, MamdaOrderBookRecap fullBook)
		{
			//OnLBMChanged(new LBMDataChangedEventArgs(LBMDataChangedEventTypes.OnBookDelta, String.Concat(new object[] { "MamdaSubscription|OnBookDelta ", subscription.getSymbol(), " : ", subscription.getSource() })));
			//onBookUpdate(subscription, delta.getDeltaOrderBook());
			onBookUpdate(subscription, fullBook.getFullOrderBook());
		}

		public void onBookGap(MamdaSubscription subscription, MamdaOrderBookListener listener, MamaMsg msg, MamdaOrderBookGap gapEvent, MamdaOrderBookRecap fullBook)
		{
			//OnLBMChanged(new LBMDataChangedEventArgs(LBMDataChangedEventTypes.OnBookGap, String.Concat(new object[] { "MamdaSubscription|OnBookGap ", subscription.getSymbol(), " : ", subscription.getSource() })));
		}

		public void onBookRecap(MamdaSubscription subscription, MamdaOrderBookListener listener, MamaMsg msg, MamdaOrderBookDelta delta, MamdaOrderBookRecap fullBook)
		{
			lock (_subscriptionsCreated)
			{
				string key = GetSubscribeKey(subscription.getSymbol(), subscription.getSource());
				if (!_subscriptionsCreated.Contains(key))
				{
					_subscriptionsCreated.Add(key);
				}
			}

			// Full Book Snap Shot
			//OnLBMChanged(new LBMDataChangedEventArgs(LBMDataChangedEventTypes.OnBookRecap, String.Concat(new object[] { "MamdaSubscription|onBookRecap ", subscription.getSymbol(), " : ", subscription.getSource() })));
			onBookUpdate(subscription, fullBook.getFullOrderBook());
		}

		private void onBookUpdate(MamdaSubscription subscription, MamdaOrderBook book)
		{
			if (_useChangeNotificationRate)
			{
				CopyMamaBookMessages(subscription, book);
			}
			else
			{
				NotifyMamaBookMessage(subscription, book);
			}
		}

		private void CopyMamaBookMessagesRaw(MamdaSubscription subscription, MamdaOrderBook book)
		{
			lock (_syncObj)
			{
				if (_rawQueueCount < _rawQueueLimit)
				{
					MamaMsgQueue queue = new MamaMsgQueue(_bookDepthLimit);
					queue.OnMsgObj = new OnMsgObject(subscription.getSource(), subscription.getSymbol(), subscription.getClosure());
					queue.UpdateBook(book);
					_mamaMsgQueueRaw.Add(queue);
					_rawQueueCount++;

					if (_logDroppedMessages && _dropedMsgCount > 0)
					{
						OnLBMChanged(new LBMDataChangedEventArgs(LBMDataChangedEventTypes.OnWarring, String.Concat(new object[] { "Dropped Msg Count ", _dropedMsgCount })));
						_dropedMsgCount = 0;
					}
				}
				else
				{
					if (_logDroppedMessages)
					{
						_dropedMsgCount++;
					}
				}
			}
		}

		private void CopyMamaBookMessages(MamdaSubscription subscription, MamdaOrderBook book)
		{
			string key = GetSubscribeKey(subscription.getSymbol(), subscription.getSource());

			lock (_syncObj)
			{
				if (!_mamaMsgQueue.ContainsKey(key))
				{
					_mamaMsgQueue.Add(key, new MamaMsgQueue(_bookDepthLimit));
					_mamaMsgQueue[key].Add(new OnMsgObject(subscription.getSource(), subscription.getSymbol(), subscription.getClosure()));
				}

				// Convert To Books
				_mamaMsgQueue[key].UpdateBook(book);
			}
		}

		private void NotifyMamaBookMessage(MamdaSubscription subscription, MamdaOrderBook book)
		{
			MamaMsgQueue queue = new MamaMsgQueue(_bookDepthLimit);
			queue.Add(new OnMsgObject(subscription.getSource(), subscription.getSymbol(), subscription.getClosure()));
			queue.UpdateBook(book);
			PickMsg(queue.OnMsgObj);
		}

		#endregion

		#region - Decode Thread -

		//private System.Threading.Timer _timerDecode;
		//private System.Threading.TimerCallback _timerDecodeDelagate;

		private void StartDecodeThread()
		{
			//if (_running)
			//{
			//    if (_timerDecodeDelagate == null)
			//    {
			//        _timerDecodeDelagate = new System.Threading.TimerCallback(DecodeRawQueue);
			//    }

			//    if (_timerDecode == null)
			//    {
			//        _timerDecode = new System.Threading.Timer(_timerDecodeDelagate, null, 0, 1);
			//    }
			//}
		}

		private void DecodeRawQueue(object input)
		{
			try
			{
				List<MamaMsgQueue> localQueueRaw = new List<MamaMsgQueue>();
				lock (_syncObj)
				{
					if (_mamaMsgQueueRaw.Count > 0)
					{
						localQueueRaw.AddRange(_mamaMsgQueueRaw.ToArray());
						_mamaMsgQueueRaw.Clear();
						_rawQueueCount = 0;
					}
				}

				if (localQueueRaw.Count > 0)
				{
					bool writeOnceOnly = false;
					string key = "";
					Dictionary<string, MamaMsgQueue> localQueue = new Dictionary<string, MamaMsgQueue>();

					if (_forceGC)
					{
						writeOnceOnly = true;
					}

					lock (_syncObj)
					{
						if (_mamaMsgQueue.Count != 0)
						{
							if (!writeOnceOnly)
							{
								localQueue = new Dictionary<string, MamaMsgQueue>(_mamaMsgQueue);
							}

							_mamaMsgQueue.Clear();
						}
					}

					if (!writeOnceOnly)
					{
						foreach (MamaMsgQueue queueRaw in localQueueRaw)
						{
							key = GetSubscribeKey(queueRaw.OnMsgObj.Symbol, queueRaw.OnMsgObj.Source);
							if (localQueue.ContainsKey(key))
							{
								localQueue[key].Update(queueRaw.OnMsgObj);
								localQueue[key].UpdateBook(queueRaw.OnMsgObj);
							}
							else
							{
								localQueue.Add(key, queueRaw);
							}
						}
					}
					else
					{
						// Do it backwards
						for (int i = localQueueRaw.Count - 1; i >= 0; i--)
						{
							key = GetSubscribeKey(localQueueRaw[i].OnMsgObj.Symbol, localQueueRaw[i].OnMsgObj.Source);
							if (!localQueue.ContainsKey(key))
							{
								localQueue.Add(key, localQueueRaw[i]);
							}
						}
					}

					lock (_syncObj)
					{
						_mamaMsgQueue = new Dictionary<string, MamaMsgQueue>(localQueue);
					}
				}
			}
			catch (Exception ex)
			{
				OnLBMChanged(new LBMDataChangedEventArgs(LBMDataChangedEventTypes.OnWarring, String.Concat(new object[] { "DecodeRawQueue", ex.Message, " : ", ex.StackTrace })));
			}
		}

		private void StopDecodeThread()
		{
			//if (_timerDecode != null)
			//{
			//    _timerDecode.Dispose();
			//}
			//_timerDecode = null;
			//_timerDecodeDelagate = null;
		}

		#endregion

		#region - Delta & Book Notify Thread -

		private System.Threading.Timer _timerDeltaNotify;
		private System.Threading.TimerCallback _timerDeltaNotifyDelagate;

		private void StartDeltaNotifyThread()
		{
			if (_running)
			{
				if (_timerDeltaNotifyDelagate == null)
				{
					_timerDeltaNotifyDelagate = new System.Threading.TimerCallback(DeltaNotify);
				}

				if (_timerDeltaNotify == null)
				{
					if (_useChangeNotificationRate)
					{
						_timerDeltaNotify = new System.Threading.Timer(_timerDeltaNotifyDelagate, null, 0, _changeNotificationRate);
					}
					else
					{
						_timerDeltaNotify = new System.Threading.Timer(_timerDeltaNotifyDelagate, null, 0, 100);
					}
				}
			}
		}

		private void DeltaNotify(object input)
		{
			MamaMsgQueue[] criticalMsgs = new MamaMsgQueue[0];
			Dictionary<string, MamaMsgQueue> msgs = new Dictionary<string, MamaMsgQueue>();

			lock (_syncObj)
			{
				if (_mamaCriticalMsgQueue.Count > 0)
				{
					criticalMsgs = _mamaCriticalMsgQueue.ToArray();
					_mamaCriticalMsgQueue.Clear();
				}

				if (_mamaMsgQueue.Count > 0)
				{
					msgs = new Dictionary<string, MamaMsgQueue>(_mamaMsgQueue);
					_mamaMsgQueue.Clear();
				}
			}

			foreach (MamaMsgQueue criticalMsg in criticalMsgs)
			{
				// Full interation of the initial snap shoot
				PickMsg(criticalMsg.OnMsgObj);
			}

			foreach (string key in msgs.Keys)
			{
				if (!_running) break;
				if (msgs[key].OnMsgObj != null)
				{
					PickMsg(msgs[key].OnMsgObj);
				}
			}

			#region - GC -

			if (_forceGC)
			{
				MemClean();

				_forceGC = false;
			}

			#endregion
		}

		private void PickMsg(OnMsgObject onMsgObj)
		{
			try
			{
				if (onMsgObj.Symbol.Length > 0)
				{
					if (onMsgObj.Symbol.Substring(0, 1) == "b")
					{
						PickMsgBook(onMsgObj);
					}
					else if (onMsgObj.Symbol != null && onMsgObj.Symbol != "")
					{
						PickMsgNormal(onMsgObj);
					}
					else
					{
						// Not A Message
					}
				}
			}
			catch (Exception ex)
			{
				Debug.Print(ex.Message);
			}
		}

		private void PickMsgNormal(OnMsgObject onMsgObj)
		{
			if (onMsgObj.Symbol.Length > 0)
			{
				MDServerToClient locChange = new MDServerToClient();

				string locString = onMsgObj.Symbol;

				string locSymbol = "";
				string locParticipentSymbol = "";
				DeltaIssueSymbol(locString, ref locSymbol, ref locParticipentSymbol);

				if (onMsgObj.Deltas.Count > 0)
				{
					locChange.Update(onMsgObj.Deltas);
				}

				locChange.Update(WombatFieldIDs.IssueSymbol, locString);
				locChange.Update(WombatFieldIDs.uSymbol, locSymbol);
				locChange.Update(WombatFieldIDs.uParticipentSymbol, locParticipentSymbol);

				locChange.Update(WombatFieldIDs.SubscriptionSoruce, onMsgObj.Source);
				locChange.Update(WombatFieldIDs.SubscriptionSymbol, onMsgObj.Symbol);

				if (locChange.IssueSymbol != null && locChange.IssueSymbol != "")
				{
					OnLBMChanged(new LBMDataChangedEventArgs(LBMDataChangedEventTypes.OnSubscriptionMsg, locChange));
				}
			}
		}

		private void PickMsgBook(OnMsgObject onMsgObj)
		{
			if (onMsgObj.Symbol.Length > 0)
			{
				MDServerToClient locChange = new MDServerToClient();

				// Book Message
				string locString = onMsgObj.Symbol;

				string locSymbol = locString;
				string locParticipentSymbol = "";

				if (onMsgObj.AskBooks.Count > 0)
				{
					locChange.AskBooks = new Dictionary<double, long>(onMsgObj.AskBooks);
				}
				if (onMsgObj.BidBooks.Count > 0)
				{
					locChange.BidBooks = new Dictionary<double, long>(onMsgObj.BidBooks);
				}

				locChange.Update(WombatFieldIDs.IssueSymbol, locString);
				locChange.Update(WombatFieldIDs.uSymbol, locSymbol);
				locChange.Update(WombatFieldIDs.uParticipentSymbol, locParticipentSymbol);

				locChange.Update(WombatFieldIDs.SubscriptionSoruce, onMsgObj.Source);
				locChange.Update(WombatFieldIDs.SubscriptionSymbol, onMsgObj.Symbol);

				if (locChange.IssueSymbol != null && locChange.IssueSymbol != "" && (locChange.AskBooks.Count > 0 || locChange.BidBooks.Count > 0))
				{
					OnLBMChanged(new LBMDataChangedEventArgs(LBMDataChangedEventTypes.OnSubscriptionMsg, locChange));
				}
			}
		}

		private void DeltaIssueSymbol(string issueSymbol, ref string symbol, ref string participent)
		{
			// valid data
			int div = issueSymbol.IndexOf(".");

			symbol = issueSymbol;
			if (div > 0)
			{
				participent = issueSymbol.Substring(div + 1);
				symbol = issueSymbol.Substring(0, div);
			}
		}

		private void StopDeltaNotifyThread()
		{
			if (_timerDeltaNotify != null)
			{
				_timerDeltaNotify.Dispose();
			}
			_timerDeltaNotify = null;
			_timerDeltaNotifyDelagate = null;
		}

		#endregion

		#region - IDisposable Members -

		public void Dispose()
		{
			Stop();
		}

		#endregion

		#region - Mem Clean -

		private void MemClean()
		{
			GC.Collect();
			GC.WaitForPendingFinalizers();
		}

		#endregion

		#region - Subscription Wait -

		private bool WaitForSubscriptionCompleted()
		{
			DateTime startTime = DateTime.Now;

			while (((TimeSpan)(DateTime.Now.Subtract(startTime))).TotalSeconds < _subscriptionTimeOut && _running)
			{
				if (_waitingForSymbol == "")
				{
					return true;
				}
			}

			return false;
		}

		#endregion
	}
}
