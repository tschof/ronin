using System;
using System.Collections.Generic;
using System.Text;

namespace ROC
{
	public sealed class FormUIProfileFieldID
	{
		public const int Top = 1;
		public const int Left = 2;
		public const int Width = 3;
		public const int Height = 4;

		public const int Font = 5;
		public const int BackColor = 6;

		public const int Caption = 7;
		public const int Pinned = 8;

		public const int WatchList = 101;
		public const int OrdersList = 102;
		public const int TradesList = 103;
		
		public const int PositionsList = 104;
		public const int PositionsListGroupBy = 1041;
		public const int PositionsShowOpenQty = 10411;
		public const int PositionsShowBuyQty = 10412;
		public const int PositionsShowSellQty = 10413;
		public const int PositionsShowOpenPnL = 10414;
		public const int PositionsShowDayPnL = 10415;
		public const int PositionsShowRealizedPnL = 10416;
		public const int PositionsShowTotalPnL = 10417;
		public const int PositionsShowCurrentQty = 10418;
		public const int PositionsShowAllAggregation = 10419;

		public const int PlotList = 105;
		public const int StockTicket = 106;
		public const int OptionTicket = 107;
		public const int FutureTicket = 108;
		public const int QuickTicket = 109;

		// Batch Ticket
		public const int BatchList = 110;
		public const int BatchMarketList = 1101;

		// Auto Spread Ticket
		public const int AutoSpreadTicket = 111;
		public const int AutoSpreadSettingsList = 1111;
		public const int AutoSpreadList = 1112;

		// Future Matrix Ticket
		public const int FutureMatrixTicket = 112;
		public const int FutureMatrixRange = 113;
		public const int FutureMatrixInterval = 114;

		// frmMain Prams
		public const int InSystemTray = 200;
		public const int SymbolSettingData = 201;

		// Stock Ticket
		public const int Level2BidList = 300;
		public const int Level2AskList = 301;

		// Future Ticket
		public const int BookBidList = 401;
		public const int BookAskList = 402;
		public const int SSFList = 403;

		public const int QuickList = 501;

		// Option Ticket
		public const int OptionList = 601;

		// Clock Window
		public const int ClockDisplayFormat = 701;
		public const int ClockTimeZoneDisplayFormat = 702;
	}
}
