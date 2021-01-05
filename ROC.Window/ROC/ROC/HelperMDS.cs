#define WANT_MDS_STUB
#define WANT_MDS_RONIN

using MarketData;
using System;
using System.Collections.Generic;

namespace ROC
{
	public class HelperMDS : HelperStatusBase, MarketDataProducer.IVisitor
	{

		private MarketData.MarketDataProducer _server;
		private MarketData.Market _deltas = null;
		private MarketData.ISubscriptionManager _subscription;

		public string ServerIP { get; private set; }
		public int ServerPort { get; private set; }
		public bool IsConnected { get; private set; }

		public HelperMDS(string serverIP, int serverPort)
		{
			ServerIP = serverIP;
			ServerPort = serverPort;

#if WANT_MDS_STUB
			var server = new MarketData.Stub.Server(this, GLOBAL.HProcess.AddToReplayList);
			_server = server;
			_subscription = server;
#elif WANT_MDS_RONIN
			var server = new MarketData.Ronin.Server(
				serverIP,
				serverPort,
				Configuration.MDS.Default.BufferSize,
				Configuration.MDS.Default.ClientBufferSizeLimit,
				this);

			_server = server;
			_subscription = new MarketData.Ronin.SubscriptionManager(
				server,
				GLOBAL.HProcess.AddToReplayList);
#endif // WANT_MDS_RONIN
		}


		public void Disconnect()
		{
			_server.Disconnect();
		}

		public bool Connect()
		{
			return _server.Connect(GLOBAL.HROM.UserName, GLOBAL.HROM.Password);
		}

		public void Reconnect()
		{
			Disconnect();
			if (Connect())
				_subscription.Resubscribe();
		}

		public void Subscribe(string symbol, string source, string subscriptionType, string secType)
		{
			_subscription.Subscribe(symbol, source,  subscriptionType,  secType);
		}

		public void SubscribeToOptionChain(List<string> symbols, string baseSymbol, string source, string type)
		{
			_subscription.SubscribeToOptionChain(symbols, baseSymbol, source, type);
		}

		public void Unsubscribe(string symbol)
		{
			_subscription.Unsubscribe(symbol);
		}

		public void Resubscribe()
		{
			_subscription.Resubscribe();
		}

		public void Resubscribe(string symbol, string source, string subscriptionType, string secType)
		{
			_subscription.Resubscribe(symbol, source, subscriptionType, secType);
		}

		public MarketData.Market GetMarketDeltas()
		{
			MarketData.Market deltas;
			lock (this) {
				deltas = _deltas;
				_deltas = null;
			}
			return deltas;
		}

		public void OnConnect(string message)
		{
			IsConnected = true;
		}

		public void OnDisconnect(string message)
		{
			IsConnected = false;
		}

		public void OnMarketData(string symbol, Book update)
		{
			if (!string.IsNullOrEmpty(symbol) && (update != null)) {
				lock (this) {
					if (_deltas == null)
						_deltas = new MarketData.Market();
					_deltas.Update(symbol, update);
				}
			}
		}

		public void AddToReplayList(string symbol)
		{
			GLOBAL.HProcess.AddToReplayList(symbol);
		}

		public void AddAlert(string alertText)
		{
			base.StatusDsp = alertText;
			base.AddToAlerts(alertText);
		}

		public void AddException(Exception x)
		{
			GLOBAL.HROC.AddToException(x);
		}

		public void AddStatus(string statusText)
		{
			base.StatusDsp = statusText;
			base.AddToStatusLogs(statusText);
		}
	}
}
