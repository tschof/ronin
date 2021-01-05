using System.Collections.Generic;
using System.Linq;

namespace MarketData.Ronin
{
	public class SubscriptionManager : MarketData.ISubscriptionManager
	{
		private readonly Dictionary<string, MDClientToServer> _reSubscribeList = new Dictionary<string, MDClientToServer>();
		private Dictionary<string, MDServerToClient> _subscribed = new Dictionary<string, MDServerToClient>();

		private readonly Server _channel;
		private readonly System.Action<string> _addToReplayFn; // Function to add symbol to application replay list.

		public SubscriptionManager(Server server, System.Action<string> addToReplayList)
		{
			_channel = server;
			_addToReplayFn = addToReplayList;
		}

		public void Subscribe(string symbol, string source, string subscriptionType, string secType)
		{
			switch (secType) {
				case CSVEx.CSVFieldIDs.SecurityTypes.Equity:
					if (symbol.Contains("/P")) {
						symbol = symbol.Replace("/P", "p");
					} else if (symbol.Contains("/RT")) {
						symbol = symbol.Replace("/RT", "r");
					} else if (symbol.Contains("/RTWI")) {
						symbol = symbol.Replace("/RTWI", "rw");
					}
					break;
			}

			var request = new MDClientToServer("S", symbol, source, subscriptionType, secType, false);

			string key = string.Join(",", symbol, source, subscriptionType, secType);
			lock (_reSubscribeList) {
				if (!_reSubscribeList.ContainsKey(key)) {
					_reSubscribeList.Add(key, request);
				}
			}

			if (!_subscribed.ContainsKey(symbol)) {
				_channel?.Send(request);
			} else {
				// Play Back
				switch (subscriptionType) {
					case "L2":
						_addToReplayFn?.Invoke(symbol);
						foreach (KeyValuePair<string, MDServerToClient> entry in _subscribed) {
							MDServerToClient data = entry.Value;
							if ((data.uSymbol == symbol) && !string.IsNullOrEmpty(data.uParticipantSymbol))
								_addToReplayFn?.Invoke(data.IssueSymbol);
						}
						break;
					default:
						_addToReplayFn?.Invoke(symbol);
						break;
				}
			}
		}

		public void SubscribeToOptionChain(List<string> symbols, string baseSymbol, string source, string type)
		{
			if (!_subscribed.ContainsKey(baseSymbol)) {
				_channel?.Send(new MDClientToServer("S", baseSymbol, source, type, false));
			} else {
				_addToReplayFn?.Invoke(baseSymbol);
			}

			foreach (string symbol in symbols) {
				if (!_subscribed.ContainsKey(symbol)) {
					_channel?.Send(new MDClientToServer("S", symbol, source, type, false));
				} else {
					_addToReplayFn?.Invoke(symbol);
				}
			}
		}

		public void Unsubscribe(string symbol)
		{
			_channel?.Send(new MDClientToServer(symbol, false));
		}

		public void Resubscribe()
		{
			if (_reSubscribeList.Count > 0) {
				List<MDClientToServer> pending;
				lock (_reSubscribeList) {
					pending = _reSubscribeList.Values.ToList();
				}

				if ((pending != null) && (pending.Count > 0)) {
					foreach (MDClientToServer request in pending)
						_channel?.Send(request);
				}
			}
		}

		// Re-subscribe by MDS Cache File
		public void Resubscribe(string symbol, string source, string subscriptionType, string secType)
		{
			_channel?.Send(new MDClientToServer("S", symbol, source, subscriptionType, secType, true));
		}
	}
}
