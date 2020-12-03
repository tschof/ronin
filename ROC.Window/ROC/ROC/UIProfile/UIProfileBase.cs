using System;
using System.Drawing;
using System.Collections.Generic;
using System.Runtime.Serialization;

using DictionaryEx;
using System.Diagnostics;

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

		public bool inSystemTray
		{
			get
			{
				if (Bools.ContainsKey(FormUIProfileFieldID.InSystemTray))
				{
					return Bools[FormUIProfileFieldID.InSystemTray];
				}
				else
				{
					return false;
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

		public bool pinned
		{
			get
			{
				if (Bools.ContainsKey(FormUIProfileFieldID.Pinned))
				{
					return Bools[FormUIProfileFieldID.Pinned];
				}
				else
				{
					return false;
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

		public string plotList
		{
			get
			{
				if (Strings.ContainsKey(FormUIProfileFieldID.PlotList))
				{
					return Strings[FormUIProfileFieldID.PlotList];
				}
				else
				{
					return "";
				}
			}
		}

		public string stockTicket
		{
			get
			{
				if (Strings.ContainsKey(FormUIProfileFieldID.StockTicket))
				{
					return Strings[FormUIProfileFieldID.StockTicket];
				}
				else
				{
					return "";
				}
			}
		}

		public string optionTicket
		{
			get
			{
				if (Strings.ContainsKey(FormUIProfileFieldID.OptionTicket))
				{
					return Strings[FormUIProfileFieldID.OptionTicket];
				}
				else
				{
					return "";
				}
			}
		}

		public string futureTicket
		{
			get
			{
				if (Strings.ContainsKey(FormUIProfileFieldID.FutureTicket))
				{
					return Strings[FormUIProfileFieldID.FutureTicket];
				}
				else
				{
					return "";
				}
			}
		}

		public string quickTicket
		{
			get
			{
				if (Strings.ContainsKey(FormUIProfileFieldID.QuickTicket))
				{
					return Strings[FormUIProfileFieldID.QuickTicket];
				}
				else
				{
					return "";
				}
			}
		}

		public string autoSpreadTicket
		{
			get
			{
				if (Strings.ContainsKey(FormUIProfileFieldID.AutoSpreadTicket))
				{
					return Strings[FormUIProfileFieldID.AutoSpreadTicket];
				}
				else
				{
					return "";
				}
			}
		}

		public string level2BidList
		{
			get
			{
				if (Strings.ContainsKey(FormUIProfileFieldID.Level2BidList))
				{
					return Strings[FormUIProfileFieldID.Level2BidList];
				}
				else
				{
					return "";
				}
			}
		}

		public string level2AskList
		{
			get
			{
				if (Strings.ContainsKey(FormUIProfileFieldID.Level2AskList))
				{
					return Strings[FormUIProfileFieldID.Level2AskList];
				}
				else
				{
					return "";
				}
			}
		}

		public string optionList
		{
			get
			{
				if (Strings.ContainsKey(FormUIProfileFieldID.OptionList))
				{
					return Strings[FormUIProfileFieldID.OptionList];
				}
				else
				{
					return "";
				}
			}
		}

		public string bookBidList
		{
			get
			{
				if (Strings.ContainsKey(FormUIProfileFieldID.BookBidList))
				{
					return Strings[FormUIProfileFieldID.BookBidList];
				}
				else
				{
					return "";
				}
			}
		}

		public string bookAskList
		{
			get
			{
				if (Strings.ContainsKey(FormUIProfileFieldID.BookAskList))
				{
					return Strings[FormUIProfileFieldID.BookAskList];
				}
				else
				{
					return "";
				}
			}
		}

		public string ssfList
		{
			get
			{
				if (Strings.ContainsKey(FormUIProfileFieldID.SSFList))
				{
					return Strings[FormUIProfileFieldID.SSFList];
				}
				else
				{
					return "";
				}
			}
		}

		public string quickList
		{
			get
			{
				if (Strings.ContainsKey(FormUIProfileFieldID.QuickList))
				{
					return Strings[FormUIProfileFieldID.QuickList];
				}
				else
				{
					return "";
				}
			}
		}

		public string autoSpreadSettingsList
		{
			get
			{
				if (Strings.ContainsKey(FormUIProfileFieldID.AutoSpreadSettingsList))
				{
					return Strings[FormUIProfileFieldID.AutoSpreadSettingsList];
				}
				else
				{
					return "";
				}
			}
		}

		public string autoSpreadList
		{
			get
			{
				if (Strings.ContainsKey(FormUIProfileFieldID.AutoSpreadList))
				{
					return Strings[FormUIProfileFieldID.AutoSpreadList];
				}
				else
				{
					return "";
				}
			}
		}

		public int matrixRange
		{
			get
			{
				if (Longs.ContainsKey(FormUIProfileFieldID.FutureMatrixRange))
				{
					try
					{
						return Convert.ToInt32(Longs[FormUIProfileFieldID.FutureMatrixRange]);
					}
					catch (Exception ex)
					{
						if (Debugger.IsAttached)
						{
							Debug.Print(ex.Message);
							Debug.Print(ex.StackTrace);
						}
					}
				}
				return 9;
			}
		}

		public int matrixInterval
		{
			get
			{
				if (Longs.ContainsKey(FormUIProfileFieldID.FutureMatrixInterval))
				{
					try
					{
						return Convert.ToInt32(Longs[FormUIProfileFieldID.FutureMatrixInterval]);
					}
					catch (Exception ex)
					{
						if (Debugger.IsAttached)
						{
							Debug.Print(ex.Message);
							Debug.Print(ex.StackTrace);
						}
					}
				}
				return 3;
			}
		}

		public string futureMatrixTicket
		{
			get
			{
				if (Strings.ContainsKey(FormUIProfileFieldID.FutureMatrixTicket))
				{
					return Strings[FormUIProfileFieldID.FutureMatrixTicket];
				}
				else
				{
					return "";
				}
			}
		}

		public string batchList
		{
			get
			{
				if (Strings.ContainsKey(FormUIProfileFieldID.BatchList))
				{
					return Strings[FormUIProfileFieldID.BatchList];
				}
				else
				{
					return "";
				}
			}
		}

		public string batchMarketList
		{
			get
			{
				if (Strings.ContainsKey(FormUIProfileFieldID.BatchMarketList))
				{
					return Strings[FormUIProfileFieldID.BatchMarketList];
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

		public long positionsListGroupBy
		{
			get
			{
				if (Longs.ContainsKey(FormUIProfileFieldID.PositionsListGroupBy))
				{
					return Longs[FormUIProfileFieldID.PositionsListGroupBy];
				}
				else
				{
					return 0;
				}
			}
		}

		public bool positionsShowOpenQty
		{
			get
			{
				if (Bools.ContainsKey(FormUIProfileFieldID.PositionsShowOpenQty))
				{
					return Bools[FormUIProfileFieldID.PositionsShowOpenQty];
				}
				else
				{
					return true;
				}
			}
		}

		public bool positionsShowBuyQty
		{
			get
			{
				if (Bools.ContainsKey(FormUIProfileFieldID.PositionsShowBuyQty))
				{
					return Bools[FormUIProfileFieldID.PositionsShowBuyQty];
				}
				else
				{
					return true;
				}
			}
		}

		public bool positionsShowSellQty
		{
			get
			{
				if (Bools.ContainsKey(FormUIProfileFieldID.PositionsShowSellQty))
				{
					return Bools[FormUIProfileFieldID.PositionsShowSellQty];
				}
				else
				{
					return true;
				}
			}
		}

		public bool positionsShowOpenPnL
		{
			get
			{
				if (Bools.ContainsKey(FormUIProfileFieldID.PositionsShowOpenPnL))
				{
					return Bools[FormUIProfileFieldID.PositionsShowOpenPnL];
				}
				else
				{
					return true;
				}
			}
		}

		public bool positionsShowDayPnL
		{
			get
			{
				if (Bools.ContainsKey(FormUIProfileFieldID.PositionsShowDayPnL))
				{
					return Bools[FormUIProfileFieldID.PositionsShowDayPnL];
				}
				else
				{
					return true;
				}
			}
		}

		public bool positionsShowRealizedPnL
		{
			get
			{
				if (Bools.ContainsKey(FormUIProfileFieldID.PositionsShowRealizedPnL))
				{
					return Bools[FormUIProfileFieldID.PositionsShowRealizedPnL];
				}
				else
				{
					return true;
				}
			}
		}

		public bool positionsShowTotalPnL
		{
			get
			{
				if (Bools.ContainsKey(FormUIProfileFieldID.PositionsShowTotalPnL))
				{
					return Bools[FormUIProfileFieldID.PositionsShowTotalPnL];
				}
				else
				{
					return true;
				}
			}
		}

		public bool positionsShowCurrentQty
		{
			get
			{
				if (Bools.ContainsKey(FormUIProfileFieldID.PositionsShowCurrentQty))
				{
					return Bools[FormUIProfileFieldID.PositionsShowCurrentQty];
				}
				else
				{
					return false;
				}
			}
		}

		public bool positionsShowAllAggregation
		{
			get
			{
				if (Bools.ContainsKey(FormUIProfileFieldID.PositionsShowAllAggregation))
				{
					return Bools[FormUIProfileFieldID.PositionsShowAllAggregation];
				}
				else
				{
					return true;
				}
			}
		}

		public string clockDisplayFormat
		{
			get
			{
				if (Strings.ContainsKey(FormUIProfileFieldID.ClockDisplayFormat))
				{
					return Strings[FormUIProfileFieldID.ClockDisplayFormat];
				}
				else
				{
					return "12Hour";
				}
			}
		}

		public string clockTimeZoneDisplayFormat
		{
			get
			{
				if (Strings.ContainsKey(FormUIProfileFieldID.ClockTimeZoneDisplayFormat))
				{
					return Strings[FormUIProfileFieldID.ClockTimeZoneDisplayFormat];
				}
				else
				{
					return "Local";
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
