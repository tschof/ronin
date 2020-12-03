using System;

using DictionaryEx;
using System.Drawing;
using System.Collections.Generic;

namespace ROC
{
	[Serializable]
	public abstract class UIProfileBase : MutiTypedUIDictionary
	{
		public UIProfileBase()
		{
		}

		public Nullable<long> top
		{
			get
			{
				if (Longs.ContainsKey(FormUIProfileFieldID.Top))
				{
					return Longs[FormUIProfileFieldID.Top];
				}
				else
				{
					return null;
				}
			}
		}

		public Nullable<long> left
		{
			get
			{
				if (Longs.ContainsKey(FormUIProfileFieldID.Left))
				{
					return Longs[FormUIProfileFieldID.Left];
				}
				else
				{
					return null;
				}
			}
		}

		public Nullable<long> width
		{
			get
			{
				if (Longs.ContainsKey(FormUIProfileFieldID.Width))
				{
					return Longs[FormUIProfileFieldID.Width];
				}
				else
				{
					return null;
				}
			}
		}

		public Nullable<long> height
		{
			get
			{
				if (Longs.ContainsKey(FormUIProfileFieldID.Height))
				{
					return Longs[FormUIProfileFieldID.Height];
				}
				else
				{
					return null;
				}
			}
		}

		public Font font
		{
			get
			{
				if (Fonts.ContainsKey(FormUIProfileFieldID.Font))
				{
					return Fonts[FormUIProfileFieldID.Font];
				}
				else
				{
					return null;
				}
			}
		}

		public Color backcolor
		{
			get
			{
				if (Colors.ContainsKey(FormUIProfileFieldID.BackColor))
				{
					return Colors[FormUIProfileFieldID.BackColor];
				}
				else
				{
					return Color.Empty;
				}
			}
		}

		public string caption
		{
			get
			{
				if (Strings.ContainsKey(FormUIProfileFieldID.Caption))
				{
					return Strings[FormUIProfileFieldID.Caption];
				}
				else
				{
					return "";
				}
			}
		}

		public string watchList
		{
			get
			{
				if (Strings.ContainsKey(FormUIProfileFieldID.WatchList))
				{
					return Strings[FormUIProfileFieldID.WatchList];
				}
				else
				{
					return "";
				}
			}
		}

		public string ordersList
		{
			get
			{
				if (Strings.ContainsKey(FormUIProfileFieldID.OrdersList))
				{
					return Strings[FormUIProfileFieldID.OrdersList];
				}
				else
				{
					return "";
				}
			}
		}

		public string tradesList
		{
			get
			{
				if (Strings.ContainsKey(FormUIProfileFieldID.TradesList))
				{
					return Strings[FormUIProfileFieldID.TradesList];
				}
				else
				{
					return "";
				}
			}
		}

		public string positionsList
		{
			get
			{
				if (Strings.ContainsKey(FormUIProfileFieldID.PositionsList))
				{
					return Strings[FormUIProfileFieldID.PositionsList];
				}
				else
				{
					return "";
				}
			}
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
	public class UIProfileBatchTicket : UIProfileBase
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
