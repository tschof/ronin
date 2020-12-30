using System.Collections.Generic;

namespace MarketData
{
	public interface ISubscriptionManager
	{
		void Subscribe(string symbol, string source, string subscriptionType, string secType);
		void SubscribeToOptionChain(List<string> symbols, string baseSymbol, string source, string type);
		void Unsubscribe(string symbol);
		void Resubscribe();
		void Resubscribe(string symbol, string source, string subscriptionType, string secType);
	}
}
