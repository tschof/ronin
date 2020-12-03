using System;
using System.IO;
using System.Collections.Generic;

using DictionaryEx;
using System.Runtime.Serialization;
using SerializationEx;

namespace MarketDataEx
{
	[Serializable]
	public class MDServerToClient : ICustomBinarySerializable 
	{
		public MDServerToClient()
		{
		}
		public MDServerToClient(MDServerToClient scrObj)
		{
		}

		public void Update(MDServerToClient scrObj)
		{
		}
		public void Update(int key, object value)
		{
			switch (key)
			{
				case WombatFieldIDs.AskClose:
					AskClose = (double)value;
					break;
				case WombatFieldIDs.AskClosedate:
					AskClosedate = (string)value;
					break;
				case WombatFieldIDs.AskHigh:
					AskHigh = (double)value;
					break;
				case WombatFieldIDs.AskLow:
					AskLow = (double)value;
					break;
				case WombatFieldIDs.AskPartId:
					AskPartId = (string)value;
					break;
				case WombatFieldIDs.AskPrice:
					AskPrice = (double)value;
					break;
				case WombatFieldIDs.AskSize:
					AskSize = (long)value;
					break;
			}
		}

		public int Count
		{
			get
			{
				return 1;
			}
		}

		private string _issueSymbol = "";
		public string IssueSymbol
		{
			get
			{
				return _issueSymbol;
			}
			set
			{
				_issueSymbol = value;
			}
		}

		private string _uSymbol = "";
		public string uSymbol
		{
			get
			{
				return _uSymbol;
			}
			set
			{
				_uSymbol = value;
			}
		}

		private string _uParticipentSymbol = "";
		public string uParticipentSymbol
		{
			get
			{
				return _uParticipentSymbol;
			}
			set
			{
				_uParticipentSymbol = value;
			}
		}

		private string _uParticipentName = "";
		public string uParticipentName
		{
			get
			{
				return _uParticipentName;
			}
			set
			{
				_uParticipentName = value;
			}
		}

		private string _instrumentType = "";
		public string InstrumentType
		{
			get
			{
				return _instrumentType;
			}
			set
			{
				_instrumentType = value;
			}
		}

		private string _lineTime = "";
		public string LineTime
		{
			get
			{
				return _lineTime;
			}
			set
			{
				_lineTime = value;
			}
		}

		private DateTime _uOnMsgTime;
		public DateTime uOnMsgTime
		{
			get
			{
				return _uOnMsgTime;
			}
			set
			{
				_uOnMsgTime = value;
			}
		}

		private DateTime _uDecodedTime;
		public DateTime uDecodedTime
		{
			get
			{
				return _uDecodedTime;
			}
			set
			{
				_uDecodedTime = value;
			}
		}

		private DateTime _uServerSentTime;
		public DateTime uServerSentTime
		{
			get
			{
				return _uServerSentTime;
			}
			set
			{
				_uServerSentTime = value;
			}
		}

		private DateTime _uClientRecivedTime;
		public DateTime uClientRecivedTime
		{
			get
			{
				return _uClientRecivedTime;
			}
			set
			{
				_uClientRecivedTime = value;
			}
		}

		private DateTime _uDisplayTime;
		public DateTime uDisplayTime
		{
			get
			{
				return _uDisplayTime;
			}
			set
			{
				_uDisplayTime = value;
			}
		}

		private double _netChange;
		public double NetChange
		{
			get
			{
				return _netChange;
			}
			set
			{
				_netChange = value;
			}
		}

		private double _pctChange;
		public double PctChange
		{
			get
			{
				return _pctChange;
			}
			set
			{
				_pctChange = value;
			}
		}

		private long _totalVolume;
		public long TotalVolume
		{
			get
			{
				return _totalVolume;
			}
			set
			{
				_totalVolume = value;
			}
		}

		private double _highPrice;
		public double HighPrice
		{
			get
			{
				return _highPrice;
			}
			set
			{
				_highPrice = value;
			}
		}

		private double _lowPrice;
		public double LowPrice
		{
			get
			{
				return _lowPrice;
			}
			set
			{
				_lowPrice = value;
			}
		}

		private double _openPrice;
		public double OpenPrice
		{
			get
			{
				return _openPrice;
			}
			set
			{
				_openPrice = value;
			}
		}

		private double _closePrice;
		public double ClosePrice
		{
			get
			{
				return _closePrice;
			}
			set
			{
				_closePrice = value;
			}
		}

		private double _prevClosePrice;
		public double PrevClosePrice
		{
			get
			{
				return _prevClosePrice;
			}
			set
			{
				_prevClosePrice = value;
			}
		}

		private string _prevCloseDate = "";
		public string PrevCloseDate
		{
			get
			{
				return _prevCloseDate;
			}
			set
			{
				_prevCloseDate = value;
			}
		}
		
		private double _totalValue;
		public double TotalValue
		{
			get
			{
				return _totalValue;
			}
			set
			{
				_totalValue = value;
			}
		}

		private double _vwap;
		public double Vwap
		{
			get
			{
				return _vwap;
			}
			set
			{
				_vwap = value;
			}
		}

		private double _bidPrice;
		public double BidPrice
		{
			get
			{
				return _bidPrice;
			}
			set
			{
				_bidPrice = value;
			}
		}

		private long _bidSize;
		public long BidSize
		{
			get
			{
				return _bidSize;
			}
			set
			{
				_bidSize = value;
			}
		}

		private double _bidLow;
		public double BidLow
		{
			get
			{
				return _bidLow;
			}
			set
			{
				_bidLow = value;
			}
		}

		private double _bidHigh;
		public double BidHigh
		{
			get
			{
				return _bidHigh;
			}
			set
			{
				_bidHigh = value;
			}
		}

		private string _bidPartId = "";
		public string BidPartId
		{
			get
			{
				return _bidPartId;
			}
			set
			{
				_bidPartId = value;
			}
		}

		private double _askPrice;
		public double AskPrice
		{
			get
			{
				return _askPrice;
			}
			set
			{
				_askPrice = value;
			}
		}

		private long _askSize;
		public long AskSize
		{
			get
			{
				return _askSize;
			}
			set
			{
				_askSize = value;
			}
		}

		private double _askLow;
		public double AskLow
		{
			get
			{
				return _askLow;
			}
			set
			{
				_askLow = value;
			}
		}

		private double _askHigh;
		public double AskHigh
		{
			get
			{
				return _askHigh;
			}
			set
			{
				_askHigh = value;
			}
		}

		private string _askPartId = "";
		public string AskPartId
		{
			get
			{
				return _askPartId;
			}
			set
			{
				_askPartId = value;
			}
		}

		private string _quoteDate = "";
		public string QuoteDate
		{
			get
			{
				return _quoteDate;
			}
			set
			{
				_quoteDate = value;
			}
		}

		private string _quoteTime = "";
		public string QuoteTime
		{
			get
			{
				return _quoteTime;
			}
			set
			{
				_quoteTime = value;
			}
		}

		private long _quoteCount;
		public long QuoteCount
		{
			get
			{
				return _quoteCount;
			}
			set
			{
				_quoteCount = value;
			}
		}

		private double _tradePrice;
		public double TradePrice
		{
			get
			{
				return _tradePrice;
			}
			set
			{
				_tradePrice = value;
			}
		}

		private string _tradeDate = "";
		public string TradeDate
		{
			get
			{
				return _tradeDate;
			}
			set
			{
				_tradeDate = value;
			}
		}

		private string _tradeTime = "";
		public string TradeTime
		{
			get
			{
				return _tradeTime;
			}
			set
			{
				_tradeTime = value;
			}
		}

		private string _tradeTick = "";
		public string TradeTick
		{
			get
			{
				return _tradeTick;
			}
			set
			{
				_tradeTick = value;
			}
		}

		private long _tradeVolume;
		public long TradeVolume
		{
			get
			{
				return _tradeVolume;
			}
			set
			{
				_tradeVolume = value;
			}
		}

		private string _tradePartId = "";
		public string TradePartId
		{
			get
			{
				return _tradePartId;
			}
			set
			{
				_tradePartId = value;
			}
		}

		private long _tradeCount;
		public long TradeCount
		{
			get
			{
				return _tradeCount;
			}
			set
			{
				_tradeCount = value;
			}
		}

		// Stock L1
		private string _primExch = "";
		public string PrimExch
		{
			get
			{
				return _primExch;
			}
			set
			{
				_primExch = value;
			}
		}

		// Future L1
		private string _productInd = "";
		public string ProductInd
		{
			get
			{
				return _productInd;
			}
			set
			{
				_productInd = value;
			}
		}

		private double _displayConversionFactor;
		public double DisplayConversionFactor
		{
			get
			{
				return _displayConversionFactor;
			}
			set
			{
				_displayConversionFactor = value;
			}
		}

		private long _orderMinQty;
		public long OrderMinQty
		{
			get
			{
				return _orderMinQty;
			}
			set
			{
				_orderMinQty = value;
			}
		}

		private long _orderMaxQty;
		public long OrderMaxQty
		{
			get
			{
				return _orderMaxQty;
			}
			set
			{
				_orderMaxQty = value;
			}
		}

		private string _maxPriceVariation = "";
		public string MaxPriceVariation
		{
			get
			{
				return _maxPriceVariation;
			}
			set
			{
				_maxPriceVariation = value;
			}
		}

		private string _maturity = "";
		public string Maturity
		{
			get
			{
				return _maturity;
			}
			set
			{
				_maturity = value;
			}
		}

		private long _bookDepth;
		public long BookDepth
		{
			get
			{
				return _bookDepth;
			}
			set
			{
				_bookDepth = value;
			}
		}

		private double _ticketIncrement;
		public double TicketIncrement
		{
			get
			{
				return _ticketIncrement;
			}
			set
			{
				_ticketIncrement = value;
			}
		}

		private string _mainFraction = "";
		public string MainFraction
		{
			get
			{
				return _mainFraction;
			}
			set
			{
				_mainFraction = value;
			}
		}

		private double _hightLimit;
		public double HightLimit
		{
			get
			{
				return _hightLimit;
			}
			set
			{
				_hightLimit = value;
			}
		}

		private double _lowLimit;
		public double LowLimit
		{
			get
			{
				return _lowLimit;
			}
			set
			{
				_lowLimit = value;
			}
		}

		private double _settlePrice;
		public double SettlePrice
		{
			get
			{
				return _settlePrice;
			}
			set
			{
				_settlePrice = value;
			}
		}

		private string _settleDate = "";
		public string SettleDate
		{
			get
			{
				return _settleDate;
			}
			set
			{
				_settleDate = value;
			}
		}

		private double _lastBestAskPrice;
		public double LastBestAskPrice
		{
			get
			{
				return _lastBestAskPrice;
			}
			set
			{
				_lastBestAskPrice = value;
			}
		}

		private double _lastBestBidPrice;
		public double LastBestBidPrice
		{
			get
			{
				return _lastBestBidPrice;
			}
			set
			{
				_lastBestBidPrice = value;
			}
		}

		private long _exchTotalVol;
		public long ExchTotalVol
		{
			get
			{
				return _exchTotalVol;
			}
			set
			{
				_exchTotalVol = value;
			}
		}

		// Option L1
		private string _putCall = "";
		public string PutCall
		{
			get
			{
				return _putCall;
			}
			set
			{
				_putCall = value;
			}
		}

		private string _expirationDate = "";
		public string ExpirationDate
		{
			get
			{
				return _expirationDate;
			}
			set
			{
				_expirationDate = value;
			}
		}

		private double _strikePrice;
		public double StrikePrice
		{
			get
			{
				return _strikePrice;
			}
			set
			{
				_strikePrice = value;
			}
		}

		private long _openlongerest;
		public long Openlongerest
		{
			get
			{
				return _openlongerest;
			}
			set
			{
				_openlongerest = value;
			}
		}

		private double _prevBidClose;
		public double PrevBidClose
		{
			get
			{
				return _prevBidClose;
			}
			set
			{
				_prevBidClose = value;
			}
		}

		private string _prevBidCloseDate = "";
		public string PrevBidCloseDate
		{
			get
			{
				return _prevBidCloseDate;
			}
			set
			{
				_prevBidCloseDate = value;
			}
		}

		private double _bidClose;
		public double BidClose
		{
			get
			{
				return _bidClose;
			}
			set
			{
				_bidClose = value;
			}
		}

		private string _bidCloseDate = "";
		public string BidCloseDate
		{
			get
			{
				return _bidCloseDate;
			}
			set
			{
				_bidCloseDate = value;
			}
		}

		private double _prevAskClose;
		public double PrevAskClose
		{
			get
			{
				return _prevAskClose;
			}
			set
			{
				_prevAskClose = value;
			}
		}

		private string _prevAskCloseDate = "";
		public string PrevAskCloseDate
		{
			get
			{
				return _prevAskCloseDate;
			}
			set
			{
				_prevAskCloseDate = value;
			}
		}

		private double _askClose;
		public double AskClose
		{
			get
			{
				return _askClose;
			}
			set
			{
				_askClose = value;
			}
		}

		private string _askCloseDate = "";
		public string AskClosedate
		{
			get
			{
				return _askCloseDate;
			}
			set
			{
				_askCloseDate = value;
			}
		}

		// Book
		private long[] _bookBidSizes = new long[15];
		public long[] BookBidSizes
		{
			get
			{
				return _bookBidSizes;
			}
			set
			{
				_bookBidSizes = value;
			}
		}

		private double[] _bookBidPrices = new double[15];
		public double[] BookBidPrices
		{
			get
			{
				return _bookBidPrices;
			}
			set
			{
				_bookBidPrices = value;
			}
		}
		
		private long[] _bookAskSizes = new long[15];
		public long[] BookAskSizes
		{
			get
			{
				return _bookAskSizes;
			}
			set
			{
				_bookAskSizes = value;
			}
		}

		private double[] _bookAskPrices = new double[15];
		public double[] BookAskPrices
		{
			get
			{
				return _bookAskPrices;
			}
			set
			{
				_bookAskPrices = value;
			}
		}
		
		#region ICustomBinarySerializable Members

		public void WriteDataTo(BinaryWriter writer)
		{
			writer.Write(_issueSymbol);
			writer.Write(_uSymbol);
			writer.Write(_uParticipentSymbol);
			writer.Write(_uParticipentName);
			writer.Write(_instrumentType);
			writer.Write(_lineTime);

			writer.Write(_uOnMsgTime.ToBinary());
			writer.Write(_uDecodedTime.ToBinary());
			writer.Write(_uServerSentTime.ToBinary());
			writer.Write(_uClientRecivedTime.ToBinary());
			writer.Write(_uDisplayTime.ToBinary());

			writer.Write(_netChange);
			writer.Write(_pctChange);
			writer.Write(_totalVolume);
			writer.Write(_highPrice);
			writer.Write(_lowPrice);
			writer.Write(_closePrice);
			writer.Write(_prevClosePrice);
			writer.Write(_prevCloseDate);
			writer.Write(_totalValue);
			writer.Write(_vwap);

			writer.Write(_bidPrice);
			writer.Write(_bidSize);
			writer.Write(_bidLow);
			writer.Write(_bidHigh);
			writer.Write(_bidPartId);

			writer.Write(_askPrice);
			writer.Write(_askSize);
			writer.Write(_askLow);
			writer.Write(_askHigh);
			writer.Write(_askPartId);

			writer.Write(_quoteDate);
			writer.Write(_quoteTime);
			writer.Write(_quoteCount);

			writer.Write(_tradePrice);
			writer.Write(_tradeDate);
			writer.Write(_tradeTime);
			writer.Write(_tradeTick);

			writer.Write(_tradeVolume);
			writer.Write(_tradePartId);
			writer.Write(_tradeCount);

			// Stock L1
			writer.Write(_primExch);

			// Future L1
			writer.Write(_productInd);
			writer.Write(_displayConversionFactor);
			writer.Write(_orderMinQty);
			writer.Write(_orderMaxQty);
			writer.Write(_maxPriceVariation);
			writer.Write(_maturity);
			writer.Write(_bookDepth);
			writer.Write(_ticketIncrement);
			writer.Write(_mainFraction);
			writer.Write(_hightLimit);
			writer.Write(_lowLimit);
			writer.Write(_settlePrice);
			writer.Write(_settleDate);
			writer.Write(_lastBestAskPrice);
			writer.Write(_lastBestBidPrice);
			writer.Write(_exchTotalVol);
			
			// Option L1
			writer.Write(_putCall);
			writer.Write(_expirationDate);
			writer.Write(_strikePrice);
			writer.Write(_openlongerest);
			writer.Write(_prevBidClose);
			writer.Write(_prevBidCloseDate);
			writer.Write(_bidClose);
			writer.Write(_bidCloseDate);
			writer.Write(_prevAskClose);
			writer.Write(_prevAskCloseDate);
			writer.Write(_askClose);
			writer.Write(_askCloseDate);

			for (int i = 0; i < 15; i++)
			{
				writer.Write(_bookBidSizes[i]);
				writer.Write(_bookBidPrices[i]);
				writer.Write(_bookAskSizes[i]);
				writer.Write(_bookAskPrices[i]);
			}
		}

		public void SetDataFrom(BinaryReader reader)
		{
			_issueSymbol = reader.ReadString();
			_uSymbol = reader.ReadString();
			_uParticipentSymbol = reader.ReadString();
			_uParticipentName = reader.ReadString();
			_instrumentType = reader.ReadString();
			_lineTime = reader.ReadString();

			_uOnMsgTime = DateTime.FromBinary(reader.ReadInt64());
			_uDecodedTime = DateTime.FromBinary(reader.ReadInt64());
			_uServerSentTime = DateTime.FromBinary(reader.ReadInt64());
			_uClientRecivedTime = DateTime.FromBinary(reader.ReadInt64());
			_uDisplayTime = DateTime.FromBinary(reader.ReadInt64());

			_netChange = reader.ReadDouble();
			_pctChange = reader.ReadDouble();
			_totalVolume = reader.ReadInt64();
			_highPrice = reader.ReadDouble();
			_lowPrice = reader.ReadDouble();
			_closePrice = reader.ReadDouble();
			_prevClosePrice = reader.ReadDouble();
			_prevCloseDate = reader.ReadString();
			_totalValue = reader.ReadDouble();
			_vwap = reader.ReadDouble();

			_bidPrice = reader.ReadDouble();
			_bidSize = reader.ReadInt64();
			_bidLow = reader.ReadDouble();
			_bidHigh = reader.ReadDouble();
			_bidPartId = reader.ReadString();

			_askPrice = reader.ReadDouble();
			_askSize = reader.ReadInt64();
			_askLow = reader.ReadDouble();
			_askHigh = reader.ReadDouble();
			_askPartId = reader.ReadString();

			_quoteDate = reader.ReadString();
			_quoteTime = reader.ReadString();
			_quoteCount = reader.ReadInt64();

			_tradePrice = reader.ReadDouble();
			_tradeDate = reader.ReadString();
			_tradeTime = reader.ReadString();
			_tradeTick = reader.ReadString();
			_tradeVolume = reader.ReadInt64();
			_tradePartId = reader.ReadString();
			_tradeCount = reader.ReadInt64();

			// Stock L1
			_primExch = reader.ReadString();
			
			// Future L1
			_productInd = reader.ReadString();
			_displayConversionFactor = reader.ReadDouble();
			_orderMinQty = reader.ReadInt64();
			_orderMaxQty = reader.ReadInt64();
			_maxPriceVariation = reader.ReadString();
			_maturity = reader.ReadString();
			_bookDepth = reader.ReadInt64();
			_ticketIncrement = reader.ReadDouble();
			_mainFraction = reader.ReadString();
			_hightLimit = reader.ReadDouble();
			_lowLimit = reader.ReadDouble();
			_settlePrice = reader.ReadDouble();
			_settleDate = reader.ReadString();
			_lastBestAskPrice = reader.ReadDouble();
			_lastBestBidPrice = reader.ReadDouble();
			_exchTotalVol = reader.ReadInt64();

			// Option L1
			_putCall = reader.ReadString();
			_expirationDate = reader.ReadString();
			_strikePrice = reader.ReadDouble();
			_openlongerest = reader.ReadInt64();
			_prevBidClose = reader.ReadDouble();
			_prevBidCloseDate = reader.ReadString();
			_bidClose = reader.ReadDouble();
			_bidCloseDate = reader.ReadString();
			_prevAskClose = reader.ReadDouble();
			_prevAskCloseDate = reader.ReadString();
			_askClose = reader.ReadDouble();
			_askCloseDate = reader.ReadString();

			for (int i = 0; i < 15; i++)
			{
				_bookBidSizes[i] = reader.ReadInt64();
				_bookBidPrices[i] = reader.ReadDouble();
				_bookAskSizes[i] = reader.ReadInt64();
				_bookAskPrices[i] = reader.ReadDouble();
			}
		}

		#endregion	
	}
}
