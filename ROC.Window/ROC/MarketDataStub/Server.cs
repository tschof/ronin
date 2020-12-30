using System.Collections.Generic;
using DateTime = System.DateTime;

using Log = Common.Log;

namespace MarketData.Stub
{
	public class Server : MarketDataProducer, ISubscriptionManager
	{
		private const int UPDATE_INTERVAL = 2000;
		private const int BOOK_DEPTH = 3;
		private const double MARKET_LO = 5;
		private const double MARKET_HI = 15;
		private const double MARKET_TICK = 0.125;
		private const long MARKET_SIZE = 50;
		private const string DATE_FORMAT = "yyyy-MM-dd";
		private const string TIME_FORMAT = "HH:mm:ss.fff";

		private readonly Market _subscribed = new Market();
		private readonly System.Action<string> _addToReplayFn;
		private System.Threading.Timer _timer;

		// For generating market data.
		private readonly System.Random _random;
		private static string _today = null;
		private static string _yesterday = null;
		private static string _later = null;

		public Server(IVisitor visitor, System.Action<string> addToReplayList) : base(visitor)
		{
			_addToReplayFn = addToReplayList;
			_random = new System.Random((int)DateTime.Now.Ticks);

			if (_today == null) {
				int shift;
				switch (DateTime.Today.DayOfWeek) {
					case System.DayOfWeek.Monday: shift = -3; break;
					case System.DayOfWeek.Sunday: shift = -2; break;
					default: shift = -1; break;
				}
				_today = DateTime.Today.ToString(DATE_FORMAT);
				_yesterday = DateTime.Today.AddDays(shift).ToString(DATE_FORMAT);
				_later = DateTime.Today.AddDays(14).ToString(DATE_FORMAT);
			}
		}

		public override bool Connect(string username, string password)
		{
			Log.Debug(Log.Target.General, "Market data server stub connected.");
			_visitor?.OnConnect("MarketData Producer Stub connected.");
			Generator.Subscribe(this);
			startTimer();
			return true;
		}

		public override void Disconnect()
		{
			stopTimer();
			Log.Debug(Log.Target.General, "Market data server stub disconnected.");
			_visitor?.OnDisconnect("MarketData Producer Stub disconnected.");
		}

		public void Resubscribe()
		{
			if (_visitor != null) {
				foreach ((string symbol, Book book) in _subscribed)
					_visitor.OnMarketData(symbol, book);
			}
		}

		public void Resubscribe(string symbol, string source, string subscriptionType, string secType)
		{
			if ((_visitor != null) && _subscribed.TryGet(symbol, out Book book))
				_visitor.OnMarketData(symbol, book);
		}

		public void Subscribe(string symbol, string source, string subscriptionType, string secType)
		{
			if (_subscribed.Add(symbol, generateBook(symbol, source, secType)))
				_addToReplayFn?.Invoke(symbol);
		}

		public void SubscribeToOptionChain(List<string> symbols, string baseSymbol, string source, string type)
		{
			List<(string, Book)> add = new List<(string, Book)>();

			foreach (string symbol in symbols) {
				add.Add((symbol, generateBook(symbol, source, type)));
				_addToReplayFn?.Invoke(symbol);
			}
			_subscribed.AddRange(add);
		}

		public void Unsubscribe(string symbol)
		{
			_subscribed.Remove(symbol);
		}

		private void startTimer()
		{
			if (_timer == null)
				_timer = new System.Threading.Timer(onTimerElapsed);
			_timer.Change(UPDATE_INTERVAL, UPDATE_INTERVAL);
		}

		private void stopTimer()
		{
			if (_timer != null) {
				_timer.Change(System.Threading.Timeout.Infinite, System.Threading.Timeout.Infinite);
				_timer = null;
			}
		}

		private void onTimerElapsed(object state)
		{
			if (_visitor != null) {
				Log.Debug(Log.Target.General, $"Market data server stub: sending market data for {_subscribed.Count} symbols.");
				foreach ((string symbol, Book delta) in _subscribed)
					_visitor.OnMarketData(symbol, delta);
			}
			stopTimer();
		}

		private Book generateBook(string symbol, string source, string type)
		{
			Book b = new Book();
			double lo = MARKET_LO + (BOOK_DEPTH * MARKET_TICK);
			double hi = MARKET_HI - (BOOK_DEPTH * MARKET_TICK);
			double range = hi - lo;
			int bookTickSpan = (int)((hi - lo) / MARKET_TICK);
			int marketTickSpan = (int)((MARKET_HI - MARKET_LO) / MARKET_TICK);
			double mid = lo + _random.Next(0, bookTickSpan) * MARKET_TICK;
			int startSize = 5 + 2 * BOOK_DEPTH;

			// Generate fake quotes.
			double offset = MARKET_TICK;
			long size = startSize;

			for (int i = 0; i < BOOK_DEPTH; ++i, size -= 2, offset += MARKET_TICK) {
				double ask = mid + offset;
				double bid = mid - offset;

				b.SetAsk(ask, size);
				b.SetBid(bid, size);

				if (i == 0) {
					b.SetField(Book.FieldEnum.AskLow, ask);
					b.SetField(Book.FieldEnum.BidHigh, bid);
					b.SetField(Book.FieldEnum.AskPrice, ask);
					b.SetField(Book.FieldEnum.BidPrice, bid);
					b.SetField(Book.FieldEnum.AskSize, size);
					b.SetField(Book.FieldEnum.BidSize, size);
				} else if (i == 1) {
					b.SetField(Book.FieldEnum.LastBestAskPrice, ask);
					b.SetField(Book.FieldEnum.LastBestBidPrice, bid);
				} else if (i == BOOK_DEPTH - 1) {
					b.SetField(Book.FieldEnum.AskHigh, ask);
					b.SetField(Book.FieldEnum.BidLow, bid);
					b.SetField(Book.FieldEnum.HighPrice, ask);
					b.SetField(Book.FieldEnum.LowPrice, bid);
				}
			}

			// double fields.
			double closePrice = lo + _random.NextDouble() * range;
			double netChange = mid > closePrice ? (mid - closePrice) : (closePrice - mid);
			b.SetField(Book.FieldEnum.AdjPrevClosePrice, mid);
			b.SetField(Book.FieldEnum.AskClose, mid + MARKET_TICK);
			b.SetField(Book.FieldEnum.BidClose, mid - MARKET_TICK);
			b.SetField(Book.FieldEnum.ClosePrice, closePrice);
			b.SetField(Book.FieldEnum.DisplayConversionFactor, 1);
			b.SetField(Book.FieldEnum.HighLimit, MARKET_HI);
			b.SetField(Book.FieldEnum.LowLimit, MARKET_LO);
			b.SetField(Book.FieldEnum.NetChange, netChange);
			b.SetField(Book.FieldEnum.OpenPrice, closePrice);
			b.SetField(Book.FieldEnum.PctChange, netChange / mid * 100.0);
			b.SetField(Book.FieldEnum.PrevAskClose, closePrice + MARKET_TICK);
			b.SetField(Book.FieldEnum.PrevBidClose, closePrice - MARKET_TICK);
			b.SetField(Book.FieldEnum.PrevClosePrice, closePrice);
			b.SetField(Book.FieldEnum.SettlePrice, closePrice);
			b.SetField(Book.FieldEnum.StrikePrice, MARKET_LO + _random.Next(0, marketTickSpan) * MARKET_TICK);
			b.SetField(Book.FieldEnum.TicketIncrement, MARKET_TICK);
			//b.SetField(Book.FieldEnum.TotalValue, 0); // What should this be?
			b.SetField(Book.FieldEnum.TradePrice, mid);
			b.SetField(Book.FieldEnum.uDecodedTime, DateTime.Now.Ticks);
			b.SetField(Book.FieldEnum.Vwap, mid);

			// string fields
			string now = DateTime.Now.ToString(TIME_FORMAT);
			b.SetField(Book.FieldEnum.AskCloseDate, _yesterday);
			b.SetField(Book.FieldEnum.AskPartId, "TEST");
			b.SetField(Book.FieldEnum.BidCloseDate, _yesterday);
			b.SetField(Book.FieldEnum.BidPartId, "TEST");
			b.SetField(Book.FieldEnum.ExpirationDate, _later);
			b.SetField(Book.FieldEnum.InstrumentType, type);
			b.SetField(Book.FieldEnum.IssueSymbol, symbol);
			b.SetField(Book.FieldEnum.LineTime, now);
			//b.SetField(Book.FieldEnum.MainFraction, "");
			b.SetField(Book.FieldEnum.Maturity, _later);
			b.SetField(Book.FieldEnum.MaxPriceVariation, MARKET_TICK);
			b.SetField(Book.FieldEnum.PrevAskCloseDate, _yesterday);
			b.SetField(Book.FieldEnum.PrevBidCloseDate, _yesterday);
			b.SetField(Book.FieldEnum.PrevCloseDate, _yesterday);
			b.SetField(Book.FieldEnum.PrimExch, source);
			//b.SetField(Book.FieldEnum.ProductInd, "");
			b.SetField(Book.FieldEnum.PutCall, _random.Next() % 2 > 0 ? "P" : "C");
			b.SetField(Book.FieldEnum.QuoteDate, _today);
			b.SetField(Book.FieldEnum.QuoteTime, now);
			//b.SetField(Book.FieldEnum.SecurityCondition, "");
			//b.SetField(Book.FieldEnum.SecurityStatus, "");
			b.SetField(Book.FieldEnum.SettleDate, _later);
			b.SetField(Book.FieldEnum.TradeDate, _today);
			b.SetField(Book.FieldEnum.TradePartId, "TEST");
			b.SetField(Book.FieldEnum.TradeTick, MARKET_TICK);
			b.SetField(Book.FieldEnum.TradeTime, now);
			b.SetField(Book.FieldEnum.uParticipantSymbol, "TEST");
			b.SetField(Book.FieldEnum.uParticipantName, "TEST");
			b.SetField(Book.FieldEnum.uSubscriptionKey, symbol);
			b.SetField(Book.FieldEnum.uSubscriptionSource, source);
			b.SetField(Book.FieldEnum.uSubscriptionSymbol, symbol);
			b.SetField(Book.FieldEnum.uSymbol, symbol);

			// long fields
			b.SetField(Book.FieldEnum.BookDepth, BOOK_DEPTH);
			b.SetField(Book.FieldEnum.ExchTotalVol, BOOK_DEPTH * 2 * MARKET_SIZE);
			b.SetField(Book.FieldEnum.OpenInterest, BOOK_DEPTH * MARKET_SIZE);
			b.SetField(Book.FieldEnum.OrderMaxQty, 10 * MARKET_SIZE);
			b.SetField(Book.FieldEnum.OrderMinQty, 1);
			b.SetField(Book.FieldEnum.QuoteCount, BOOK_DEPTH);
			b.SetField(Book.FieldEnum.TotalVolume, BOOK_DEPTH * 2 * MARKET_SIZE);
			b.SetField(Book.FieldEnum.TradeCount, BOOK_DEPTH * 2);
			b.SetField(Book.FieldEnum.TradeVolume, MARKET_SIZE);

			return b;
		}

		private static class Generator
		{
			private static (string Symbol, string Name)[] _stocks = new (string Symbol, string Name)[] {
				("MSFT", "Microsoft"),
				("AAPL", "Apple"),
				("V", "Visa"),
				("JPM", "JPMorgan"),
				("JNJ", "Johnson & Johnson"),
				("WMT", "Walmart"),
				("PG", "Procter & Gamble"),
				("INTC", "Intel"),
				("UNH", "UnitedHealth Group"),
				("XOM", "ExxonMobile"),
				("HD", "Home Depot"),
				("DIS", "Disney"),
				("KO", "Coca-Cola"),
				("VZ", "Verizon"),
				("MRK", "Merck & Co"),
				("PFE", "Pfizer")
			};

			internal static void Subscribe(Server server)
			{
				foreach (var entry in _stocks)
					server.Subscribe(entry.Symbol, "ROC", "", "stock");
			}
		}
	}
}
