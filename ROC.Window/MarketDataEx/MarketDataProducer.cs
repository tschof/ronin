using System.Collections.Generic;
using Log = Common.Log;

namespace MarketData
{
	public abstract class MarketDataProducer
	{
		public interface IVisitor
		{
			void OnConnect(string message);
			void OnDisconnect(string message);
			void OnMarketData(string symbol, Book update);
			void AddAlert(string alertText);
			void AddException(System.Exception x);
			void AddStatus(string statusText);
		}

		public abstract bool Connect(string username, string password);
		public abstract void Disconnect();

		public delegate MarketDataProducer ServerFactory(string ip, int port);

		protected readonly IVisitor _visitor;

		protected MarketDataProducer(IVisitor visitor)
		{
			_visitor = visitor;
		}

		public static List<MarketDataProducer> CreateServersFromCsv(string addressCsv, string portCsv, ServerFactory factory)
		{
			List<MarketDataProducer> servers = new List<MarketDataProducer>();
			char[] delims = new char[] { ',' };
			string[] ipList = addressCsv.Split(delims, System.StringSplitOptions.RemoveEmptyEntries);
			string[] portList = portCsv.Split(delims, System.StringSplitOptions.RemoveEmptyEntries);
			int ipCount = (ipList == null) ? 0 : ipList.GetLength(0);
			int portCount = (portList == null) ? 0 : portList.GetLength(0);
			int serverCount = System.Math.Min(ipCount, portCount);

			if (serverCount > 0) {
				for (int i = 0; i < serverCount; ++i) {
					if (int.TryParse(portList[i], out int port)) {
						try {
							MarketDataProducer server = factory?.Invoke(ipList[i], port);
							if (server != null)
								servers.Add(server);
						} catch (System.Exception x) {
							Log.Error(Log.Target.General, $"Can't add market data server {ipList[i]}:{portList[i]}", x);
						}
					} else {
						Log.Error(Log.Target.General, $"Can't add market data server {ipList[i]}:{portList[i]}: invalid port");
					}
				}
			} else {
				Log.Error(Log.Target.General, "Can't add market data servers from empty IP or port list.");
			}

			return servers;
		}
	}
}
