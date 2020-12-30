#if OLD
using System.Collections.Generic;
using Book = MarketData.Ronin.Book;

namespace ROC
{
	public interface BookISupport
	{
		Book UpdateMarketData(Market deltas);
		Book UpdateMarketData(Market deltas, double? askPrice, double? bidPrice, double? highPrice, double? lowPrice);
	}
}
#endif // OLD