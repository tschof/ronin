using System;
using System.Collections.Generic;
using System.Text;
using MarketDataEx;

namespace ROC
{
	public interface BookISupport
	{
		MDServerToClient UpdateMarketData(Dictionary<string, MDServerToClient> deltas);
		MDServerToClient UpdateMarketData(Dictionary<string, MDServerToClient> deltas, Nullable<double> askPrice, Nullable<double> bidPrice, Nullable<double> highPrice, Nullable<double> lowPrice);
	}
}
