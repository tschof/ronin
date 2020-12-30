using System;

using DictionaryEx;

namespace ROC
{
	[Serializable]
	public abstract class UIProfileBase : MultiTypedUIDictionary
	{
		public UIProfileBase()
		{
		}
	}

	[Serializable]
	public class UIProfileAlert : UIProfileBase
	{
	}

	[Serializable]
	public class UIProfileUserProfile : UIProfileBase
	{
	}

	[Serializable]
	public class UIProfileMain : UIProfileBase
	{
	}

	[Serializable]
	public class UIProfileClock : UIProfileBase
	{
	}

	[Serializable]
	public class UIProfileWatchList : UIProfileBase
	{
	}

	[Serializable]
	public class UIProfilePlotList : UIProfileBase
	{
	}

	[Serializable]
	public class UIProfileStockTicket : UIProfileBase
	{
	}

	[Serializable]
	public class UIProfileOptionTicket : UIProfileBase
	{
	}

	[Serializable]
	public class UIProfileFutureTicket : UIProfileBase
	{
	}

	[Serializable]
	public class UIProfileQuickTicket : UIProfileBase
	{
	}

	[Serializable]
	public class UIProfileAutoSpreadTicket : UIProfileBase
	{
	}

	[Serializable]
	public class UIProfileFutureMatrixTicket : UIProfileBase
	{
	}

	[Serializable]
	public class UIProfileBatchTicket : UIProfileBase
	{
	}

	[Serializable]
	public class UIProfileBatchMarketTicket : UIProfileBase
	{
	}

	[Serializable]
	public class UIProfileOrders : UIProfileBase
	{
	}

	[Serializable]
	public class UIProfileTrades : UIProfileBase
	{
	}

	[Serializable]
	public class UIProfilePositions : UIProfileBase
	{
	}
}
