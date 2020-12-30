using System.Collections.Generic;
using DictionaryEx;

namespace MarketData.Ronin
{
	internal class BookAdapter : MarketData.IBookAdapter
	{
		private readonly MDServerToClient _data;

		private static Dictionary<int, Book.FieldEnum> _translate = new Dictionary<int, Book.FieldEnum>()
		{
			{ WombatFieldIDs.AdjPrevClosePrice, Book.FieldEnum.AdjPrevClosePrice },
			{ WombatFieldIDs.AskClose, Book.FieldEnum.AskClose },
			{ WombatFieldIDs.AskClosedate, Book.FieldEnum.AskCloseDate },
			{ WombatFieldIDs.AskHigh, Book.FieldEnum.AskHigh },
			{ WombatFieldIDs.AskLow, Book.FieldEnum.AskLow },
			{ WombatFieldIDs.AskPartId, Book.FieldEnum.AskPartId },
			{ WombatFieldIDs.AskPrice, Book.FieldEnum.AskPrice },
			{ WombatFieldIDs.AskSize, Book.FieldEnum.AskSize },
			{ WombatFieldIDs.BidClose, Book.FieldEnum.BidClose },
			{ WombatFieldIDs.BidCloseDate, Book.FieldEnum.BidCloseDate },
			{ WombatFieldIDs.BidHigh, Book.FieldEnum.BidHigh },
			{ WombatFieldIDs.BidLow, Book.FieldEnum.BidLow },
			{ WombatFieldIDs.BidPartId, Book.FieldEnum.BidPartId },
			{ WombatFieldIDs.BidPrice, Book.FieldEnum.BidPrice },
			{ WombatFieldIDs.BidSize, Book.FieldEnum.BidSize },
			{ WombatFieldIDs.BookDepth, Book.FieldEnum.BookDepth },
			{ WombatFieldIDs.ClosePrice, Book.FieldEnum.ClosePrice },
			{ WombatFieldIDs.DisplayConversionFactor, Book.FieldEnum.DisplayConversionFactor },
			{ WombatFieldIDs.ExchTotalVol, Book.FieldEnum.ExchTotalVol },
			{ WombatFieldIDs.ExpirationDate, Book.FieldEnum.ExpirationDate },
			{ WombatFieldIDs.HighPrice, Book.FieldEnum.HighPrice },
			{ WombatFieldIDs.HighLimit, Book.FieldEnum.HighLimit },
			{ WombatFieldIDs.InstrumentType, Book.FieldEnum.InstrumentType },
			{ WombatFieldIDs.IssueSymbol, Book.FieldEnum.IssueSymbol },
			{ WombatFieldIDs.LastBestAskPrice, Book.FieldEnum.LastBestAskPrice },
			{ WombatFieldIDs.LastBestBidPrice, Book.FieldEnum.LastBestBidPrice },
			{ WombatFieldIDs.LineTime, Book.FieldEnum.LineTime },
			{ WombatFieldIDs.LowLimit, Book.FieldEnum.LowLimit },
			{ WombatFieldIDs.LowPrice, Book.FieldEnum.LowPrice },
			{ WombatFieldIDs.MainFraction, Book.FieldEnum.MainFraction },
			{ WombatFieldIDs.Maturity, Book.FieldEnum.Maturity },
			{ WombatFieldIDs.MaxPriceVariation, Book.FieldEnum.MaxPriceVariation },
			{ WombatFieldIDs.NetChange, Book.FieldEnum.NetChange },
			{ WombatFieldIDs.OpenInterest, Book.FieldEnum.OpenInterest },
			{ WombatFieldIDs.OpenPrice, Book.FieldEnum.OpenPrice },
			{ WombatFieldIDs.OrderMaxQty, Book.FieldEnum.OrderMaxQty },
			{ WombatFieldIDs.OrderMinQty, Book.FieldEnum.OrderMinQty },
			{ WombatFieldIDs.PctChange, Book.FieldEnum.PctChange },
			{ WombatFieldIDs.PrevAskClose, Book.FieldEnum.PrevAskClose },
			{ WombatFieldIDs.PrevAskCloseDate, Book.FieldEnum.PrevAskCloseDate },
			{ WombatFieldIDs.PrevBidClose, Book.FieldEnum.PrevBidClose },
			{ WombatFieldIDs.PrevBidCloseDate, Book.FieldEnum.PrevBidCloseDate },
			{ WombatFieldIDs.PrevCloseDate, Book.FieldEnum.PrevCloseDate },
			{ WombatFieldIDs.PrevClosePrice, Book.FieldEnum.PrevClosePrice },
			{ WombatFieldIDs.PrimExch, Book.FieldEnum.PrimExch },
			{ WombatFieldIDs.ProductInd, Book.FieldEnum.ProductInd },
			{ WombatFieldIDs.PutCall, Book.FieldEnum.PutCall },
			{ WombatFieldIDs.QuoteCount, Book.FieldEnum.QuoteCount },
			{ WombatFieldIDs.QuoteDate, Book.FieldEnum.QuoteDate },
			{ WombatFieldIDs.QuoteTime, Book.FieldEnum.QuoteTime },
			{ WombatFieldIDs.SecurityStatus, Book.FieldEnum.SecurityStatus },
			{ WombatFieldIDs.SettleDate, Book.FieldEnum.SettleDate },
			{ WombatFieldIDs.SettlePrice, Book.FieldEnum.SettlePrice },
			{ WombatFieldIDs.StrikePrice, Book.FieldEnum.StrikePrice },
			{ WombatFieldIDs.TicketIncrement, Book.FieldEnum.TicketIncrement },
			{ WombatFieldIDs.TotalValue, Book.FieldEnum.TotalValue },
			{ WombatFieldIDs.TotalVolume, Book.FieldEnum.TotalVolume },
			{ WombatFieldIDs.TradeCount, Book.FieldEnum.TradeCount },
			{ WombatFieldIDs.TradeDate, Book.FieldEnum.TradeDate },
			{ WombatFieldIDs.TradePartId, Book.FieldEnum.TradePartId },
			{ WombatFieldIDs.TradePrice, Book.FieldEnum.TradePrice },
			{ WombatFieldIDs.TradeTick, Book.FieldEnum.TradeTick },
			{ WombatFieldIDs.TradeTime, Book.FieldEnum.TradeTime },
			{ WombatFieldIDs.TradeVolume, Book.FieldEnum.TradeVolume },
			{ WombatFieldIDs.uDecodedTime, Book.FieldEnum.uDecodedTime },
			{ WombatFieldIDs.uParticipentName, Book.FieldEnum.uParticipantName },
			{ WombatFieldIDs.uParticipentSymbol, Book.FieldEnum.uParticipantSymbol },
			{ WombatFieldIDs.uSymbol, Book.FieldEnum.uSymbol },
			{ WombatFieldIDs.Vwap, Book.FieldEnum.Vwap }
		};

		internal BookAdapter(MDServerToClient fromExchange)
		{
			_data = fromExchange;
		}

		public override QuoteCollection GetAsks() => new QuoteCollection(_data.AskBooks);
		public override QuoteCollection GetBids() => new QuoteCollection(_data.BidBooks);
		public override QuoteCollection GetTradedVolumes() => new QuoteCollection(_data.TradedVolumes);

		private class FieldVisitor : MultiTypedDictionary.IVisitor
		{
			private Book _book;

			internal FieldVisitor(Book book)
			{
				_book = book;
			}

			public void OnField(int key, string value)
			{
				if (_translate.TryGetValue(key, out Book.FieldEnum which))
					_book.SetField(which, value, true);
			}

			public void OnField(int key, double value)
			{
				if (_translate.TryGetValue(key, out Book.FieldEnum which))
					_book.SetField(which, value, true);
			}

			public void OnField(int key, long value)
			{
				if (_translate.TryGetValue(key, out Book.FieldEnum which))
					_book.SetField(which, value, true);
			}

			public void OnField(int key, bool value)
			{
				throw new System.NotImplementedException();
			}
		}

		public override void VisitFields(Book book)
		{
			FieldVisitor visitor = new FieldVisitor(book);
			_data.VisitFields(visitor);
		}
	}
}
