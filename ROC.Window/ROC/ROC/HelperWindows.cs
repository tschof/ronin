using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Drawing;

using Common;
using ContextMenuEx;

namespace ROC
{
	internal sealed class WindowMenuTypes
	{
		internal const int ShowAll = 0;
		internal const int HideAll = 1;
		internal const int WatchLists = 2;
		internal const int PlotLists = 3;
		internal const int StockTickets = 4;
		internal const int OptionTickets = 5;
		internal const int FutureTickets = 6;
		internal const int QuickTickets = 7;
		internal const int BatchTickets = 8;
		internal const int BatchMarketTickets = 9;
		internal const int AutoSpreadTickets = 10;
		internal const int FutureMatrixTickets = 11;
		internal const int OrderWindows = 12;
		internal const int TradeWindows = 13;
		internal const int PositionWindows = 14;
	}

	internal sealed class NotificationMenuTypes
	{
		internal const int ShowAll = 0;
		internal const int HideAll = 1;
		internal const int WatchList = 2;
		internal const int PlotList = 3;
		internal const int StockTicket = 4;
		internal const int OptionTicket = 5;
		internal const int FutureTicket = 6;
		internal const int QuickTicket = 7;
		internal const int BatchTicket = 8;
		internal const int BatchMarketTickets = 9;
		internal const int AutoSpreadTicket = 10;
		internal const int FutureMatrixTicket = 11;
		internal const int Order = 12;
		internal const int Trade = 13;
		internal const int Position = 14;
		internal const int Windows = 15;
		internal const int Exit = 16;
	}

	internal class HelperWindows
	{
		internal HelperWindows()
		{

		}

		private Dictionary<IntPtr, frmAlert> _alertWindows;
		internal Dictionary<IntPtr, frmAlert> AlertWindows
		{
			get
			{
				if (_alertWindows == null)
				{
					_alertWindows = new Dictionary<IntPtr, frmAlert>();
				}
				return _alertWindows;
			}
			set
			{
				_alertWindows = value;
			}
		}

		private Dictionary<IntPtr, frmClock> _clockWindows;
		internal Dictionary<IntPtr, frmClock> ClockWindows
		{
			get
			{
				if (_clockWindows == null)
				{
					_clockWindows = new Dictionary<IntPtr, frmClock>();
				}
				return _clockWindows;
			}
			set
			{
				_clockWindows = value;
			}
		}

		internal frmMain MainWindow;

		private Dictionary<IntPtr, frmUserProfile> _userProfileWindows;
		internal Dictionary<IntPtr, frmUserProfile> UserProfileWindows
		{
			get
			{
				if (_userProfileWindows == null)
				{
					_userProfileWindows = new Dictionary<IntPtr, frmUserProfile>();
				}
				return _userProfileWindows;
			}
			set
			{
				_userProfileWindows = value;
			}
		}

		private Dictionary<IntPtr, frmSettings> _settingsWindows;
		internal Dictionary<IntPtr, frmSettings> SettingsWindows
		{
			get
			{
				if (_settingsWindows == null)
				{
					_settingsWindows = new Dictionary<IntPtr, frmSettings>();
				}
				return _settingsWindows;
			}
			set
			{
				_settingsWindows = value;
			}
		}

		private Dictionary<IntPtr, frmEMail> _emailWindows;
		internal Dictionary<IntPtr, frmEMail> EMailWindows
		{
			get
			{
				if (_emailWindows == null)
				{
					_emailWindows = new Dictionary<IntPtr, frmEMail>();
				}
				return _emailWindows;
			}
			set
			{
				_emailWindows = value;
			}
		}

		private Dictionary<IntPtr, frmAbout> _aboutWindows;
		internal Dictionary<IntPtr, frmAbout> AboutWindows
		{
			get
			{
				if (_aboutWindows == null)
				{
					_aboutWindows = new Dictionary<IntPtr, frmAbout>();
				}
				return _aboutWindows;
			}
			set
			{
				_aboutWindows = value;
			}
		}

		private Dictionary<IntPtr, frmWatchList> _watchListWindows;
		internal Dictionary<IntPtr, frmWatchList> WatchListWindows
		{
			get
			{
				if (_watchListWindows == null)
				{
					_watchListWindows = new Dictionary<IntPtr, frmWatchList>();
				}
				return _watchListWindows;
			}
			set
			{
				_watchListWindows = value;
			}
		}

		private Dictionary<IntPtr, frmPlotList> _plotListWindows;
		internal Dictionary<IntPtr, frmPlotList> PlotListWindows
		{
			get
			{
				if (_plotListWindows == null)
				{
					_plotListWindows = new Dictionary<IntPtr, frmPlotList>();
				}
				return _plotListWindows;
			}
			set
			{
				_plotListWindows = value;
			}
		}

		private Dictionary<IntPtr, frmStockTicket> _stockTicketWindows;
		internal Dictionary<IntPtr, frmStockTicket> StockTicketWindows
		{
			get
			{
				if (_stockTicketWindows == null)
				{
					_stockTicketWindows = new Dictionary<IntPtr, frmStockTicket>();
				}
				return _stockTicketWindows;
			}
			set
			{
				_stockTicketWindows = value;
			}
		}

		private Dictionary<IntPtr, frmOptionTicket> _optionTicketWindows;
		internal Dictionary<IntPtr, frmOptionTicket> OptionTicketWindows
		{
			get
			{
				if (_optionTicketWindows == null)
				{
					_optionTicketWindows = new Dictionary<IntPtr, frmOptionTicket>();
				}
				return _optionTicketWindows;
			}
			set
			{
				_optionTicketWindows = value;
			}
		}

		private Dictionary<IntPtr, frmFutureTicket> _futureTicketWindows;
		internal Dictionary<IntPtr, frmFutureTicket> FutureTicketWindows
		{
			get
			{
				if (_futureTicketWindows == null)
				{
					_futureTicketWindows = new Dictionary<IntPtr, frmFutureTicket>();
				}
				return _futureTicketWindows;
			}
			set
			{
				_futureTicketWindows = value;
			}
		}

		private Dictionary<IntPtr, frmQuickTicket> _quickTicketWindows;
		internal Dictionary<IntPtr, frmQuickTicket> QuickTicketWindows
		{
			get
			{
				if (_quickTicketWindows == null)
				{
					_quickTicketWindows = new Dictionary<IntPtr, frmQuickTicket>();
				}
				return _quickTicketWindows;
			}
			set
			{
				_quickTicketWindows = value;
			}
		}

		private Dictionary<IntPtr, frmAutoSpreadTicket> _autoSpreadTicketWindows;
		internal Dictionary<IntPtr, frmAutoSpreadTicket> AutoSpreadTicketWindows
		{
			get
			{
				if (_autoSpreadTicketWindows == null)
				{
					_autoSpreadTicketWindows = new Dictionary<IntPtr, frmAutoSpreadTicket>();
				}
				return _autoSpreadTicketWindows;
			}
			set
			{
				_autoSpreadTicketWindows = value;
			}
		}

		private Dictionary<IntPtr, frmFutureMatrixTicket> _futureMatrixTicketWindows;
		internal Dictionary<IntPtr, frmFutureMatrixTicket> FutureMatrixTicketWindows
		{
			get
			{
				if (_futureMatrixTicketWindows == null)
				{
					_futureMatrixTicketWindows = new Dictionary<IntPtr, frmFutureMatrixTicket>();
				}
				return _futureMatrixTicketWindows;
			}
			set
			{
				_futureMatrixTicketWindows = value;
			}
		}

		private Dictionary<IntPtr, frmBatchTicket> _batchTicketsWindows;
		internal Dictionary<IntPtr, frmBatchTicket> BatchTicketWindows
		{
			get
			{
				if (_batchTicketsWindows == null)
				{
					_batchTicketsWindows = new Dictionary<IntPtr, frmBatchTicket>();
				}
				return _batchTicketsWindows;
			}
			set
			{
				_batchTicketsWindows = value;
			}
		}

		private Dictionary<IntPtr, frmBatchMarketTicket> _batchMarketTicketsWindows;
		internal Dictionary<IntPtr, frmBatchMarketTicket> BatchMarketTicketWindows
		{
			get
			{
				if (_batchMarketTicketsWindows == null)
				{
					_batchMarketTicketsWindows = new Dictionary<IntPtr, frmBatchMarketTicket>();
				}
				return _batchMarketTicketsWindows;
			}
			set
			{
				_batchMarketTicketsWindows = value;
			}
		}

		private Dictionary<IntPtr, frmOrders> _orderWindows;
		internal Dictionary<IntPtr, frmOrders> OrderWindows
		{
			get
			{
				if (_orderWindows == null)
				{
					_orderWindows = new Dictionary<IntPtr, frmOrders>();
				}
				return _orderWindows;
			}
			set
			{
				_orderWindows = value;
			}
		}

		private Dictionary<IntPtr, frmTrades> _tradeWindows;
		internal Dictionary<IntPtr, frmTrades> TradeWindows
		{
			get
			{
				if (_tradeWindows == null)
				{
					_tradeWindows = new Dictionary<IntPtr, frmTrades>();
				}
				return _tradeWindows;
			}
			set
			{
				_tradeWindows = value;
			}
		}

		private Dictionary<IntPtr, frmPositions> _positionWindows;
		internal Dictionary<IntPtr, frmPositions> PositionWindows
		{
			get
			{
				if (_positionWindows == null)
				{
					_positionWindows = new Dictionary<IntPtr, frmPositions>();
				}
				return _positionWindows;
			}
			set
			{
				_positionWindows = value;
			}
		}

		private VistaContextMenu _windowsMenu;
		internal VistaContextMenu WindowsMenu
		{
			get
			{
				if (_windowsMenu == null)
				{
					LoadWindowsMenu();
				}
				return _windowsMenu;
			}
		}

		private VistaContextMenu _notificationMenu;
		internal VistaContextMenu NotificationMenu
		{
			get
			{
				if (_notificationMenu == null)
				{
					LoadNotificationMenu();
				}
				return _notificationMenu;
			}
		}

		#region - Extended -

		// Check to see if any window is in extended mode or not
		// If not stop TPOS pulling
		internal bool IsExtended
		{
			get
			{
				foreach ((IntPtr _, frmTrades w) in TradeWindows)
				{
					if (w.Extended)
					{
						return true;
					}
				}

				foreach ((IntPtr _, frmPositions w) in PositionWindows)
				{
					if (w.Extended)
					{
						return true;
					}
				}

				return false;
			}
		}

		private bool _okToCloseNotificationMenu = true;

		#endregion

		#region - Notification Menu -

		private void LoadNotificationMenu()
		{
			_notificationMenu = new VistaContextMenu();
			_notificationMenu.Opening += new System.ComponentModel.CancelEventHandler(WindowsMenu_Opening);
			_notificationMenu.MouseLeave += new EventHandler(NotificationMenu_Leave);

			_notificationMenu.Items.Add("Show All", null, new EventHandler(NotificationMenuEvent));
			_notificationMenu.Items.Add("Hide All", null, new EventHandler(NotificationMenuEvent));
			_notificationMenu.Items.Add("Watch List", null, new EventHandler(NotificationMenuEvent));
			_notificationMenu.Items.Add("Plot List", null, new EventHandler(NotificationMenuEvent));
			_notificationMenu.Items.Add("Stock Ticket", null, new EventHandler(NotificationMenuEvent));
			_notificationMenu.Items.Add("Option Ticket", null, new EventHandler(NotificationMenuEvent));
			_notificationMenu.Items.Add("Future Ticket", null, new EventHandler(NotificationMenuEvent));
			_notificationMenu.Items.Add("Quick Trade Ticket", null, new EventHandler(NotificationMenuEvent));
			_notificationMenu.Items.Add("Batch Ticket", null, new EventHandler(NotificationMenuEvent));
			_notificationMenu.Items.Add("MD Batch Ticket", null, new EventHandler(NotificationMenuEvent));
			_notificationMenu.Items.Add("Auto Spread Ticket", null, new EventHandler(NotificationMenuEvent));
			_notificationMenu.Items.Add("Future Matrix Ticket", null, new EventHandler(NotificationMenuEvent));
			_notificationMenu.Items.Add("Order", null, new EventHandler(NotificationMenuEvent));
			_notificationMenu.Items.Add("Trade", null, new EventHandler(NotificationMenuEvent));
			_notificationMenu.Items.Add("Position", null, new EventHandler(NotificationMenuEvent));
			_notificationMenu.Items.Add("Windows", null, new EventHandler(NotificationMenuEvent));
			_notificationMenu.Items.Add("Exit", null, new EventHandler(NotificationMenuEvent));

			LoadNotificationWindowsMenu((ToolStripMenuItem)_notificationMenu.Items[NotificationMenuTypes.Windows]);
		}

		private void NotificationMenuEvent(object sender, EventArgs e)
		{
			switch (((ToolStripMenuItem)sender).Text)
			{
				case "Show All":
					ShowAll();
					break;
				case "Hide All":
					HideAll();
					break;
				case "Watch List":
					lock (WatchListWindows)
					{
						OpenWindow(new frmWatchList(), true);
					}
					break;
				case "Plot List":
					lock (PlotListWindows)
					{
						OpenWindow(new frmPlotList(), true);
					}
					break;
				case "Stock Ticket":
					lock (StockTicketWindows)
					{
						OpenWindow(new frmStockTicket(), true);
					}
					break;
				case "Option Ticket":
					lock (OptionTicketWindows)
					{
						OpenWindow(new frmOptionTicket(), true);
					}
					break;
				case "Future Ticket":
					lock (FutureTicketWindows)
					{
						OpenWindow(new frmFutureTicket(), true);
					}
					break;
				case "Quick Trade Ticket":
					lock (QuickTicketWindows)
					{
						OpenWindow(new frmQuickTicket(), true);
					}
					break;
				case "Auto Spread Ticket":
					lock (AutoSpreadTicketWindows)
					{
						OpenWindow(new frmAutoSpreadTicket(), true);
					}
					break;
				case "Future Matrix Ticket":
					lock (FutureMatrixTicketWindows)
					{
						OpenWindow(new frmFutureMatrixTicket(), true);
					}
					break;
				case "Batch Ticket":
					lock (BatchTicketWindows)
					{
						OpenWindow(new frmBatchTicket(), true);
					}
					break;
				case "MD Batch Ticket":
					lock (BatchMarketTicketWindows)
					{
						OpenWindow(new frmBatchMarketTicket(), true);
					}
					break;
				case "Order":
					lock (OrderWindows)
					{
						OpenWindow(new frmOrders(), true);
					}
					break;
				case "Trade":
					lock (TradeWindows)
					{
						OpenWindow(new frmTrades(), true);
					}
					break;
				case "Position":
					lock (PositionWindows)
					{
						OpenWindow(new frmPositions(), true);
					}
					break;
				case "Windows":
					LoadNotificationWindowsMenu((ToolStripMenuItem)_notificationMenu.Items[NotificationMenuTypes.Windows]);
					break;
				case "Exit":
					GLOBAL.MainForm.ExitApplication();
					break;
			}
		}

		private void NotificationMenu_Leave(object sender, EventArgs e)
		{
			if (_okToCloseNotificationMenu)
			{
				_notificationMenu.Visible = false;
			}
		}

		private void LoadNotificationWindowsMenu(ToolStripMenuItem menu)
		{
			menu.DropDownItems.Clear();
			menu.DropDownOpened += new EventHandler(Menu_DropDownOpened);
			menu.DropDownClosed += new EventHandler(Menu_DropDownClosed);
			menu.Disposed += new EventHandler(Menu_Disposed);

			menu.DropDownItems.Add("Show All", null, new EventHandler(NotificationMenuEvent));
			menu.DropDownItems.Add("Hide All", null, new EventHandler(NotificationMenuEvent));

			menu.DropDownItems.Add("Watch Lists");
			menu.DropDownItems.Add("Plot Lists");

			menu.DropDownItems.Add("Stock Tickets");
			menu.DropDownItems.Add("Option Tickets");
			menu.DropDownItems.Add("Future Tickets");
			menu.DropDownItems.Add("Quick Trade Tickets");
			menu.DropDownItems.Add("Batch Tickets");
			menu.DropDownItems.Add("MD Batch Tickets");
			menu.DropDownItems.Add("Auto Spread Tickets");
			menu.DropDownItems.Add("Future Matrix Tickets");

			menu.DropDownItems.Add("Order Windows");
			menu.DropDownItems.Add("Trade Windows");
			menu.DropDownItems.Add("Position Windows");

			LoadWatchLists((ToolStripMenuItem)menu.DropDownItems[WindowMenuTypes.WatchLists]);
			LoadPlotLists((ToolStripMenuItem)menu.DropDownItems[WindowMenuTypes.PlotLists]);

			LoadStockTickets((ToolStripMenuItem)menu.DropDownItems[WindowMenuTypes.StockTickets]);
			LoadOptionTickets((ToolStripMenuItem)menu.DropDownItems[WindowMenuTypes.OptionTickets]);
			LoadFutureTickets((ToolStripMenuItem)menu.DropDownItems[WindowMenuTypes.FutureTickets]);
			LoadQuickTickets((ToolStripMenuItem)menu.DropDownItems[WindowMenuTypes.QuickTickets]);
			LoadBatchTickets((ToolStripMenuItem)menu.DropDownItems[WindowMenuTypes.BatchTickets]);
			LoadBatchMarketTickets((ToolStripMenuItem)menu.DropDownItems[WindowMenuTypes.BatchMarketTickets]);
			LoadAutoSpreadTickets((ToolStripMenuItem)menu.DropDownItems[WindowMenuTypes.AutoSpreadTickets]);
			LoadFutureMatrixTickets((ToolStripMenuItem)menu.DropDownItems[WindowMenuTypes.FutureMatrixTickets]);

			LoadOrderWindows((ToolStripMenuItem)menu.DropDownItems[WindowMenuTypes.OrderWindows]);
			LoadTradeWindows((ToolStripMenuItem)menu.DropDownItems[WindowMenuTypes.TradeWindows]);
			LoadPositionWindows((ToolStripMenuItem)menu.DropDownItems[WindowMenuTypes.PositionWindows]);
		}

		private void Menu_DropDownOpened(object sender, EventArgs e)
		{
			_okToCloseNotificationMenu = false;
		}

		private void Menu_DropDownClosed(object sender, EventArgs e)
		{
			_okToCloseNotificationMenu = true;
		}

		private void Menu_Disposed(object sender, EventArgs e)
		{
			((ToolStripMenuItem)sender).DropDownOpened -= new EventHandler(Menu_DropDownOpened);
			((ToolStripMenuItem)sender).DropDownClosed -= new EventHandler(Menu_DropDownClosed);
			((ToolStripMenuItem)sender).Disposed -= new EventHandler(Menu_Disposed);
		}

		#endregion

		#region - Windows Menu -

		private void LoadWindowsMenu()
		{
			_windowsMenu = new VistaContextMenu();
			_windowsMenu.Opening += new System.ComponentModel.CancelEventHandler(WindowsMenu_Opening);

			_windowsMenu.Items.Add("Show All", null, new EventHandler(WindowsMenuEvent));
			_windowsMenu.Items.Add("Hide All", null, new EventHandler(WindowsMenuEvent));

			_windowsMenu.Items.Add("Watch Lists");
			_windowsMenu.Items.Add("Plot Lists");
			_windowsMenu.Items.Add("Stock Tickets");
			_windowsMenu.Items.Add("Option Tickets");
			_windowsMenu.Items.Add("Future Tickets");
			_windowsMenu.Items.Add("Quick Trade Tickets");
			_windowsMenu.Items.Add("Batch Tickets");
			_windowsMenu.Items.Add("MD Batch Tickets");
			_windowsMenu.Items.Add("Auto Spread Tickets");
			_windowsMenu.Items.Add("Future Spread Tickets");
			_windowsMenu.Items.Add("Order Windows");
			_windowsMenu.Items.Add("Trade Windows");
			_windowsMenu.Items.Add("Position Windows");
		}

		private void WindowsMenu_Opening(object sender, EventArgs e)
		{
			LoadWatchLists((ToolStripMenuItem)WindowsMenu.Items[WindowMenuTypes.WatchLists]);
			LoadPlotLists((ToolStripMenuItem)WindowsMenu.Items[WindowMenuTypes.PlotLists]);
			
			LoadStockTickets((ToolStripMenuItem)WindowsMenu.Items[WindowMenuTypes.StockTickets]);
			LoadOptionTickets((ToolStripMenuItem)WindowsMenu.Items[WindowMenuTypes.OptionTickets]);
			LoadFutureTickets((ToolStripMenuItem)WindowsMenu.Items[WindowMenuTypes.FutureTickets]);
			LoadQuickTickets((ToolStripMenuItem)WindowsMenu.Items[WindowMenuTypes.QuickTickets]);
			LoadBatchTickets((ToolStripMenuItem)WindowsMenu.Items[WindowMenuTypes.BatchTickets]);
			LoadBatchMarketTickets((ToolStripMenuItem)WindowsMenu.Items[WindowMenuTypes.BatchMarketTickets]);
			LoadAutoSpreadTickets((ToolStripMenuItem)WindowsMenu.Items[WindowMenuTypes.AutoSpreadTickets]);
			LoadFutureMatrixTickets((ToolStripMenuItem)WindowsMenu.Items[WindowMenuTypes.FutureMatrixTickets]);

			LoadOrderWindows((ToolStripMenuItem)WindowsMenu.Items[WindowMenuTypes.OrderWindows]);
			LoadTradeWindows((ToolStripMenuItem)WindowsMenu.Items[WindowMenuTypes.TradeWindows]);
			LoadPositionWindows((ToolStripMenuItem)WindowsMenu.Items[WindowMenuTypes.PositionWindows]);
		}

		private void WindowsMenuEvent(object sender, EventArgs e)
		{
			switch (((ToolStripMenuItem)sender).Text)
			{
				case "Show All":
					ShowAll();
					break;
				case "Hide All":
					HideAll();
					break;
			}
		}

		private bool _busyShowAll = false;
		internal bool BusyShowAll
		{
			get
			{
				return _busyShowAll;
			}
		}

		internal void ShowAll()
		{
			if (!_busyShowAll)
			{
				_busyShowAll = true;

				foreach ((IntPtr _, frmWatchList w) in WatchListWindows)
				{
					if (!w.IsProcessing)
					{
						ShowWindow(w);
					}
				}

				foreach ((IntPtr _, frmPlotList w) in PlotListWindows) {
					if (!w.IsProcessing) {
						ShowWindow(w);
					}
				}

				foreach ((IntPtr _, frmStockTicket w) in StockTicketWindows) {
					ShowWindow(w);
				}

				foreach ((IntPtr _, frmOptionTicket w) in OptionTicketWindows) {
					if (!w.IsProcessing) {
						ShowWindow(w);
					}
				}

				foreach ((IntPtr _, frmFutureTicket w) in FutureTicketWindows) {
					ShowWindow(w);
				}

				foreach ((IntPtr _, frmQuickTicket w) in QuickTicketWindows) {
					ShowWindow(w);
				}

				foreach ((IntPtr _, frmAutoSpreadTicket w) in AutoSpreadTicketWindows) {
					if (!w.IsProcessing) {
						ShowWindow(w);
					}
				}

				foreach ((IntPtr _, frmFutureMatrixTicket w) in FutureMatrixTicketWindows) {
					if (!w.IsProcessing) {
						ShowWindow(w);
					}
				}

				foreach ((IntPtr _, frmBatchTicket w) in BatchTicketWindows) {
					if (!w.IsProcessing) {
						ShowWindow(w);
					}
				}

				foreach ((IntPtr _, frmBatchMarketTicket w) in BatchMarketTicketWindows) {
					if (!w.IsProcessing) {
						ShowWindow(w);
					}
				}

				foreach ((IntPtr _, frmOrders w) in OrderWindows) {
					if (!w.IsProcessing) {
						ShowWindow(w);
					}
				}

				foreach ((IntPtr _, frmTrades w) in TradeWindows) {
					if (!w.IsProcessing) {
						ShowWindow(w);
					}
				}

				foreach ((IntPtr _, frmPositions w) in PositionWindows) {
					if (!w.IsProcessing) {
						ShowWindow(w);
					}
				}

				foreach ((IntPtr _, frmAlert w) in AlertWindows) {
					ShowWindow(w);
				}

				foreach ((IntPtr _, frmClock w) in ClockWindows) {
					ShowWindow(w);
				}

				_busyShowAll = false;
			}
		}

		private void HideAll()
		{
			foreach ((IntPtr _, frmAlert w) in AlertWindows) {
				w.Hide();
			}

			foreach ((IntPtr _, frmClock w) in ClockWindows) {
				w.Hide();
			}

			foreach ((IntPtr _, frmWatchList w) in WatchListWindows) {
				w.Hide();
			}

			foreach ((IntPtr _, frmPlotList w) in PlotListWindows) {
				w.Hide();
			}

			foreach ((IntPtr _, frmStockTicket w) in StockTicketWindows) {
				w.Hide();
			}

			foreach ((IntPtr _, frmOptionTicket w) in OptionTicketWindows) {
				w.Hide();
			}

			foreach ((IntPtr _, frmFutureTicket w) in FutureTicketWindows) {
				w.Hide();
			}

			foreach ((IntPtr _, frmQuickTicket w) in QuickTicketWindows) {
				w.Hide();
			}

			foreach ((IntPtr _, frmAutoSpreadTicket w) in AutoSpreadTicketWindows) {
				w.Hide();
			}

			foreach ((IntPtr _, frmFutureMatrixTicket w) in FutureMatrixTicketWindows) {
				w.Hide();
			}

			foreach ((IntPtr _, frmBatchTicket w) in BatchTicketWindows) {
				w.Hide();
			}

			foreach ((IntPtr _, frmBatchMarketTicket w) in BatchMarketTicketWindows) {
				w.Hide();
			}

			foreach ((IntPtr _, frmOrders w) in OrderWindows) {
				w.Hide();
			}

			foreach ((IntPtr _, frmTrades w) in TradeWindows) {
				w.Hide();
			}

			foreach ((IntPtr _, frmPositions w) in PositionWindows) {
				w.Hide();
			}
		}

		private void LoadWatchLists(ToolStripMenuItem menu)
		{
			menu.DropDownItems.Clear();
			foreach ((IntPtr _, frmWatchList w) in WatchListWindows) {
				menu.DropDownItems.Add(new ToolStripMenuItem(w.Caption, null, new EventHandler(WatchLists_MenuEvent), w.Handle.ToString()));
			}
		}

		private void WatchLists_MenuEvent(object sender, EventArgs e)
		{
			foreach ((IntPtr _, frmWatchList w) in WatchListWindows) {
				if (w.Caption == ((ToolStripMenuItem)sender).Text &&
					w.Handle.ToString() == ((ToolStripMenuItem)sender).Name) {
					ShowWindow(w);
				}
			}
		}

		private void LoadPlotLists(ToolStripMenuItem menu)
		{
			menu.DropDownItems.Clear();
			foreach ((IntPtr _, frmPlotList w) in PlotListWindows) {
				menu.DropDownItems.Add(new ToolStripMenuItem(w.Caption, null, new EventHandler(PlotLists_MenuEvent), w.Handle.ToString()));
			}
		}

		private void PlotLists_MenuEvent(object sender, EventArgs e)
		{
			foreach ((IntPtr _, frmPlotList w) in PlotListWindows) {
				if (w.Caption == ((ToolStripMenuItem)sender).Text &&
					w.Handle.ToString() == ((ToolStripMenuItem)sender).Name) {
					ShowWindow(w);
				}
			}
		}

		private void LoadStockTickets(ToolStripMenuItem menu)
		{
			menu.DropDownItems.Clear();
			foreach ((IntPtr _, frmStockTicket w) in StockTicketWindows) {
				menu.DropDownItems.Add(new ToolStripMenuItem(w.Caption, null, new EventHandler(StockTickets_MenuEvent), w.Handle.ToString()));
			}
		}

		private void StockTickets_MenuEvent(object sender, EventArgs e)
		{
			foreach ((IntPtr _, frmStockTicket w) in StockTicketWindows) {
				if (w.Caption == ((ToolStripMenuItem)sender).Text &&
					w.Handle.ToString() == ((ToolStripMenuItem)sender).Name) {
					ShowWindow(w);
				}
			}
		}

		private void LoadOptionTickets(ToolStripMenuItem menu)
		{
			menu.DropDownItems.Clear();
			foreach ((IntPtr _, frmOptionTicket w) in OptionTicketWindows) {
				menu.DropDownItems.Add(new ToolStripMenuItem(w.Caption, null, new EventHandler(OptionTickets_MenuEvent), w.Handle.ToString()));
			}
		}

		private void OptionTickets_MenuEvent(object sender, EventArgs e)
		{
			foreach ((IntPtr _, frmOptionTicket w) in OptionTicketWindows) {
				if (w.Caption == ((ToolStripMenuItem)sender).Text &&
					w.Handle.ToString() == ((ToolStripMenuItem)sender).Name) {
					ShowWindow(w);
				}
			}
		}

		private void LoadFutureTickets(ToolStripMenuItem menu)
		{
			menu.DropDownItems.Clear();
			foreach ((IntPtr _, frmFutureTicket w) in FutureTicketWindows) {
				menu.DropDownItems.Add(new ToolStripMenuItem(w.Caption, null, new EventHandler(FutureTickets_MenuEvent), w.Handle.ToString()));
			}
		}

		private void FutureTickets_MenuEvent(object sender, EventArgs e)
		{
			foreach ((IntPtr _, frmFutureTicket w) in FutureTicketWindows) {
				if (w.Caption == ((ToolStripMenuItem)sender).Text &&
					w.Handle.ToString() == ((ToolStripMenuItem)sender).Name) {
					ShowWindow(w);
				}
			}
		}

		private void LoadQuickTickets(ToolStripMenuItem menu)
		{
			menu.DropDownItems.Clear();
			foreach ((IntPtr _, frmQuickTicket w) in QuickTicketWindows) {
				menu.DropDownItems.Add(new ToolStripMenuItem(w.Caption, null, new EventHandler(QuickTickets_MenuEvent), w.Handle.ToString()));
			}
		}

		private void QuickTickets_MenuEvent(object sender, EventArgs e)
		{
			foreach ((IntPtr _, frmQuickTicket w) in QuickTicketWindows) {
				if (w.Caption == ((ToolStripMenuItem)sender).Text &&
					w.Handle.ToString() == ((ToolStripMenuItem)sender).Name) {
					ShowWindow(w);
				}
			}
		}

		private void LoadBatchTickets(ToolStripMenuItem menu)
		{
			menu.DropDownItems.Clear();
			foreach ((IntPtr _, frmBatchTicket w) in BatchTicketWindows) {
				menu.DropDownItems.Add(new ToolStripMenuItem(w.Caption, null, new EventHandler(BatchTickets_MenuEvent), w.Handle.ToString()));
			}
		}

		private void BatchTickets_MenuEvent(object sender, EventArgs e)
		{
			foreach ((IntPtr _, frmBatchTicket w) in BatchTicketWindows) {
				if (w.Caption == ((ToolStripMenuItem)sender).Text &&
					w.Handle.ToString() == ((ToolStripMenuItem)sender).Name) {
					ShowWindow(w);
				}
			}
		}

		private void LoadBatchMarketTickets(ToolStripMenuItem menu)
		{
			menu.DropDownItems.Clear();
			foreach ((IntPtr _, frmBatchMarketTicket w) in BatchMarketTicketWindows) {
				menu.DropDownItems.Add(new ToolStripMenuItem(w.Caption, null, new EventHandler(BatchMarketTickets_MenuEvent), w.Handle.ToString()));
			}
		}

		private void BatchMarketTickets_MenuEvent(object sender, EventArgs e)
		{
			foreach ((IntPtr _, frmBatchMarketTicket w) in BatchMarketTicketWindows) {
				if (w.Caption == ((ToolStripMenuItem)sender).Text &&
					w.Handle.ToString() == ((ToolStripMenuItem)sender).Name) {
					ShowWindow(w);
				}
			}
		}

		private void LoadAutoSpreadTickets(ToolStripMenuItem menu)
		{
			menu.DropDownItems.Clear();
			foreach ((IntPtr _, frmAutoSpreadTicket w) in AutoSpreadTicketWindows) {
				menu.DropDownItems.Add(new ToolStripMenuItem(w.Caption, null, new EventHandler(AutoSpreadTickets_MenuEvent), w.Handle.ToString()));
			}
		}

		private void AutoSpreadTickets_MenuEvent(object sender, EventArgs e)
		{
			foreach ((IntPtr _, frmAutoSpreadTicket w) in AutoSpreadTicketWindows) {
				if (w.Caption == ((ToolStripMenuItem)sender).Text &&
					w.Handle.ToString() == ((ToolStripMenuItem)sender).Name) {
					ShowWindow(w);
				}
			}
		}

		private void LoadFutureMatrixTickets(ToolStripMenuItem menu)
		{
			menu.DropDownItems.Clear();
			foreach ((IntPtr _, frmFutureMatrixTicket w) in FutureMatrixTicketWindows) {
				menu.DropDownItems.Add(new ToolStripMenuItem(w.Caption, null, new EventHandler(FutureMatrixTickets_MenuEvent), w.Handle.ToString()));
			}
		}

		private void FutureMatrixTickets_MenuEvent(object sender, EventArgs e)
		{
			foreach ((IntPtr _, frmFutureMatrixTicket w) in FutureMatrixTicketWindows) {
				if (w.Caption == ((ToolStripMenuItem)sender).Text &&
					w.Handle.ToString() == ((ToolStripMenuItem)sender).Name) {
					ShowWindow(w);
				}
			}
		}

		private void LoadOrderWindows(ToolStripMenuItem menu)
		{
			menu.DropDownItems.Clear();
			foreach ((IntPtr _, frmOrders w) in OrderWindows) {
				menu.DropDownItems.Add(new ToolStripMenuItem(w.Caption, null, new EventHandler(Order_MenuEvent), w.Handle.ToString()));
			}
		}

		private void Order_MenuEvent(object sender, EventArgs e)
		{
			foreach ((IntPtr _, frmOrders w) in OrderWindows) {
				if (w.Caption == ((ToolStripMenuItem)sender).Text &&
					w.Handle.ToString() == ((ToolStripMenuItem)sender).Name) {
					ShowWindow(w);
				}
			}
		}

		private void LoadTradeWindows(ToolStripMenuItem menu)
		{
			menu.DropDownItems.Clear();
			foreach ((IntPtr _, frmTrades w) in TradeWindows) {
				menu.DropDownItems.Add(new ToolStripMenuItem(w.Caption, null, new EventHandler(Trade_MenuEvent), w.Handle.ToString()));
			}
		}

		private void Trade_MenuEvent(object sender, EventArgs e)
		{
			foreach ((IntPtr _, frmTrades w) in TradeWindows) {
				if (w.Caption == ((ToolStripMenuItem)sender).Text &&
					w.Handle.ToString() == ((ToolStripMenuItem)sender).Name) {
					ShowWindow(w);
				}
			}
		}

		private void LoadPositionWindows(ToolStripMenuItem menu)
		{
			menu.DropDownItems.Clear();
			foreach ((IntPtr _, frmPositions w) in PositionWindows) {
				menu.DropDownItems.Add(new ToolStripMenuItem(w.Caption, null, new EventHandler(Position_MenuEvent), w.Handle.ToString()));
			}
		}

		private void Position_MenuEvent(object sender, EventArgs e)
		{
			foreach ((IntPtr _, frmPositions w) in PositionWindows) {
				if (w.Caption == ((ToolStripMenuItem)sender).Text &&
					w.Handle.ToString() == ((ToolStripMenuItem)sender).Name) {
					ShowWindow(w);
				}
			}
		}

		#endregion

		private Size _destopSize;
		internal Size DesktopSize
		{
			get
			{
				if (_destopSize == null || _destopSize.IsEmpty)
				{
					_destopSize = new Size(0, 0);
					foreach (Screen scr in Screen.AllScreens)
					{
						_destopSize.Width = _destopSize.Width + scr.Bounds.Width;
						_destopSize.Height = _destopSize.Height + scr.Bounds.Height;
					}
				}
				return _destopSize;
			}
		}

		#region - Open Window -

		private void ShowWindow(Form w)
		{
			try
			{
				switch (w.WindowState)
				{
					case FormWindowState.Minimized:
						w.WindowState = FormWindowState.Normal;
						break;
					default:
						break;
				}

				//if (w.Left < -20 ||
				//    w.Left > DesktopSize.Width - 20 ||
				//    w.Top < -20 ||
				//    w.Top > DesktopSize.Height - 20)
				//{
				//    w.StartPosition = FormStartPosition.WindowsDefaultLocation;
				//}

				w.Show();
				w.BringToFront();
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		private void OpenWindow(Form w)
		{
			switch (w.StartPosition)
			{
				case FormStartPosition.CenterParent:
				case FormStartPosition.CenterScreen:
					break;
				default:
					w.StartPosition = FormStartPosition.WindowsDefaultLocation;
					break;
			}
			w.Show();
			w.BringToFront();
		}

		internal frmAlert OpenWindow(frmAlert w)
		{
			if (AlertWindows.Count > 0) {
				foreach ((IntPtr _, frmAlert wOld) in AlertWindows) {
					ShowWindow(wOld);
					w = wOld;
				}
			} else {
				OpenWindow(w);
				AlertWindows.Add(w.Handle, w);
			}

			return w;
		}

		internal frmClock OpenWindow(frmClock w)
		{
			if (ClockWindows.Count > 0) {
				foreach ((IntPtr _, frmClock wOld) in ClockWindows) {
					ShowWindow(wOld);
					w = wOld;
				}
			} else {
				OpenWindow(w);
				ClockWindows.Add(w.Handle, w);
			}

			return w;
		}

		internal frmUserProfile OpenWindow(frmUserProfile w)
		{
			if (UserProfileWindows.Count > 0) {
				foreach ((IntPtr _, frmUserProfile wOld) in UserProfileWindows) {
					ShowWindow(wOld);
					w = wOld;
				}
			} else {
				OpenWindow(w);
				UserProfileWindows.Add(w.Handle, w);
			}

			return w;
		}

		internal frmSettings OpenWindow(frmSettings w)
		{
			if (SettingsWindows.Count > 0) {
				foreach ((IntPtr _, frmSettings wOld) in SettingsWindows) {
					ShowWindow(wOld);
					w = wOld;
				}
			} else {
				OpenWindow(w);
				SettingsWindows.Add(w.Handle, w);
			}

			return w;
		}

		internal frmEMail OpenWindow(frmEMail w)
		{
			if (SettingsWindows.Count > 0) {
				foreach ((IntPtr _, frmEMail wOld) in EMailWindows) {
					ShowWindow(wOld);
					w = wOld;
				}
			} else {
				OpenWindow(w);
				EMailWindows.Add(w.Handle, w);
			}

			return w;
		}

		internal frmAbout OpenWindow(frmAbout w)
		{
			if (AboutWindows.Count > 0) {
				foreach ((IntPtr _, frmAbout wOld) in AboutWindows) {
					ShowWindow(wOld);
					w = wOld;
				}
			} else {
				OpenWindow(w);
				AboutWindows.Add(w.Handle, w);
			}

			return w;
		}

		internal frmMain OpenWindow(frmMain w)
		{
			if (MainWindow == null)
				MainWindow = w;
			ShowWindow(MainWindow);
			return MainWindow;
		}

		internal void OpenWindow(frmWatchList w, bool isNew)
		{
			if (isNew)
			{
				OpenWindow(w);
			}
			else
			{
				if (!w.IsProcessing)
				{
					ShowWindow(w);
				}
			}
			WatchListWindows.Add(w.Handle, w);
		}

		internal void OpenWindow(frmPlotList w, bool isNew)
		{
			if (isNew)
			{
				OpenWindow(w);
			}
			else
			{
				if (!w.IsProcessing)
				{
					ShowWindow(w);
				}
			}
			PlotListWindows.Add(w.Handle, w);
		}

		internal void OpenWindow(frmStockTicket w, bool isNew)
		{
			if (isNew)
			{
				OpenWindow(w);
			}
			else
			{
				ShowWindow(w);
			}
			StockTicketWindows.Add(w.Handle, w);
		}

		internal void OpenWindow(frmOptionTicket w, bool isNew)
		{
			if (isNew)
			{
				OpenWindow(w);
			}
			else
			{
				if (!w.IsProcessing)
				{
					ShowWindow(w);
				}
			}
			OptionTicketWindows.Add(w.Handle, w);
		}

		internal void OpenWindow(frmFutureTicket w, bool isNew)
		{
			if (isNew)
			{
				OpenWindow(w);
			}
			else
			{
				ShowWindow(w);
			}
			FutureTicketWindows.Add(w.Handle, w);
		}

		internal void OpenWindow(frmQuickTicket w, bool isNew)
		{
			if (isNew)
			{
				OpenWindow(w);
			}
			else
			{
				ShowWindow(w);
			}
			QuickTicketWindows.Add(w.Handle, w);
		}

		internal void OpenWindow(frmAutoSpreadTicket w, bool isNew)
		{
			if (isNew)
			{
				OpenWindow(w);
			}
			else
			{
				if (!w.IsProcessing)
				{
					ShowWindow(w);
				}
			}
			AutoSpreadTicketWindows.Add(w.Handle, w);
		}

		internal void OpenWindow(frmFutureMatrixTicket w, bool isNew)
		{
			if (isNew)
			{
				OpenWindow(w);
			}
			else
			{
				if (!w.IsProcessing)
				{
					ShowWindow(w);
				}
			}
			FutureMatrixTicketWindows.Add(w.Handle, w);
		}

		internal void OpenWindow(frmBatchTicket w, bool isNew)
		{
			if (isNew)
			{
				OpenWindow(w);
			}
			else
			{
				if (!w.IsProcessing)
				{
					ShowWindow(w);
				}
			}
			BatchTicketWindows.Add(w.Handle, w);
		}

		internal void OpenWindow(frmBatchMarketTicket w, bool isNew)
		{
			if (isNew)
			{
				OpenWindow(w);
			}
			else
			{
				if (!w.IsProcessing)
				{
					ShowWindow(w);
				}
			}
			BatchMarketTicketWindows.Add(w.Handle, w);
		}

		internal void OpenWindow(frmOrders w, bool isNew)
		{
			if (isNew)
			{
				OpenWindow(w);
			}
			else
			{
				if (!w.IsProcessing)
				{
					ShowWindow(w);
				}
			}
			OrderWindows.Add(w.Handle, w);
		}

		internal void OpenWindow(frmTrades w, bool isNew)
		{
			if (isNew)
			{
				OpenWindow(w);
			}
			else
			{
				if (!w.IsProcessing)
				{
					ShowWindow(w);
				}
			}
			TradeWindows.Add(w.Handle, w);
		}

		internal void OpenWindow(frmPositions w, bool isNew)
		{
			if (isNew)
			{
				OpenWindow(w);
			}
			else
			{
				if (!w.IsProcessing)
				{
					ShowWindow(w);
				}
			}
			PositionWindows.Add(w.Handle, w);
		}

		internal void OpenWindow(string secType, string symbolDetail)
		{
			if (!CheckTicketWindows(secType, symbolDetail))
			{
				switch (secType)
				{
					case CSVEx.CSVFieldIDs.SecurityTypes.Equity:
						lock (GLOBAL.HWindows.StockTicketWindows)
						{
							OpenWindow(new frmStockTicket(symbolDetail, Configuration.ROC.Default.ShowLevel2AsDefault), true);
						}
						break;
					case CSVEx.CSVFieldIDs.SecurityTypes.Option:
						string[] opts = symbolDetail.Split(new string[] { " " }, StringSplitOptions.RemoveEmptyEntries);
						if (opts.Length > 0)
						{
							lock (GLOBAL.HWindows.OptionTicketWindows)
							{
								OpenWindow(new frmOptionTicket(opts[0]), true);
							}
						}
						break;
					case CSVEx.CSVFieldIDs.SecurityTypes.Future:
						if (Configuration.ROC.Default.ShowQuickTicketAsDefault)
						{
							lock (GLOBAL.HWindows.QuickTicketWindows)
							{
								OpenWindow(new frmQuickTicket(symbolDetail), true);
							}
						}
						else
						{
							lock (GLOBAL.HWindows.FutureTicketWindows)
							{
								OpenWindow(new frmFutureTicket(symbolDetail), true);
							}
						}
						break;
				}
			}
		}

		private bool CheckTicketWindows(string secType, string symbolDetail)
		{
			bool found = false;
			switch (secType)
			{
				case CSVEx.CSVFieldIDs.SecurityTypes.Equity:
					foreach ((IntPtr _, frmStockTicket w) in StockTicketWindows)
					{
						if (w.CurrentSymbolDetail == symbolDetail)
						{
							ShowWindow(w);
							found = true;
							break;
						}
					}
					break;
				case CSVEx.CSVFieldIDs.SecurityTypes.Option:
					string[] opts = symbolDetail.Split(new string[] { " " }, StringSplitOptions.RemoveEmptyEntries);
					if (opts.Length > 0)
					{
						foreach ((IntPtr _, frmOptionTicket w) in OptionTicketWindows)
						{
							if (w.CurrentSymbolDetail == opts[0])
							{
								ShowWindow(w);
								found = true;
								break;
							}
						}
					}
					break;
				case CSVEx.CSVFieldIDs.SecurityTypes.Future:
					foreach ((IntPtr _, frmFutureTicket w) in FutureTicketWindows)
					{
						if (w.CurrentSymbolDetail == symbolDetail)
						{
							ShowWindow(w);
							found = true;
							break;
						}
					}
					break;
			}

			return found;
		}

		#endregion

		#region - Close Window -

		private void CloseWindow(Form w)
		{
			w.Hide();
		}

		internal void CloseWindow(frmAlert w)
		{
			CloseWindow(w);
			AlertWindows.Remove(w.Handle);
		}

		internal void CloseWindow(frmClock w)
		{
			CloseWindow(w);
			ClockWindows.Remove(w.Handle);
		}

		internal void CloseWindow(frmAbout w)
		{
			CloseWindow(w);
			AboutWindows.Remove(w.Handle);
		}

		internal void CloseWindow(frmUserProfile w)
		{
			CloseWindow(w);
			UserProfileWindows.Remove(w.Handle);
		}

		internal void CloseWindow(frmSettings w)
		{
			CloseWindow(w);
			SettingsWindows.Remove(w.Handle);
		}

		internal void CloseWindow(frmEMail w)
		{
			CloseWindow(w);
			EMailWindows.Remove(w.Handle);
		}

		internal void CloseWindow(frmWatchList w)
		{
			CloseWindow(w);
			WatchListWindows.Remove(w.Handle);
		}

		internal void CloseWindow(frmPlotList w)
		{
			CloseWindow(w);
			PlotListWindows.Remove(w.Handle);
		}

		internal void CloseWindow(frmStockTicket w)
		{
			CloseWindow(w);
			StockTicketWindows.Remove(w.Handle);
		}

		internal void CloseWindow(frmOptionTicket w)
		{
			CloseWindow(w);
			OptionTicketWindows.Remove(w.Handle);
		}

		internal void CloseWindow(frmFutureTicket w)
		{
			CloseWindow(w);
			FutureTicketWindows.Remove(w.Handle);
		}

		internal void CloseWindow(frmQuickTicket w)
		{
			CloseWindow(w);
			QuickTicketWindows.Remove(w.Handle);
		}

		internal void CloseWindow(frmAutoSpreadTicket w)
		{
			CloseWindow(w);
			AutoSpreadTicketWindows.Remove(w.Handle);
		}

		internal void CloseWindow(frmFutureMatrixTicket w)
		{
			CloseWindow(w);
			FutureMatrixTicketWindows.Remove(w.Handle);
		}

		internal void CloseWindow(frmBatchTicket w)
		{
			CloseWindow(w);
			BatchTicketWindows.Remove(w.Handle);
		}

		internal void CloseWindow(frmBatchMarketTicket w)
		{
			CloseWindow(w);
			BatchMarketTicketWindows.Remove(w.Handle);
		}

		internal void CloseWindow(frmOrders w)
		{
			CloseWindow(w);
			OrderWindows.Remove(w.Handle);
		}

		internal void CloseWindow(frmTrades w)
		{
			CloseWindow(w);
			TradeWindows.Remove(w.Handle);
		}

		internal void CloseWindow(frmPositions w)
		{
			CloseWindow(w);
			PositionWindows.Remove(w.Handle);
		}

		#endregion

		#region - Zero All Window -

		internal void ZeroAllWindowPosition()
		{
			try
			{
				ZeroPosition(MainWindow);

				foreach ((IntPtr _, frmAlert w) in AlertWindows) {
					ZeroPosition(w);
				}

				foreach ((IntPtr _, frmClock w) in ClockWindows) {
					ZeroPosition(w);
				}

				foreach ((IntPtr _, frmUserProfile w) in UserProfileWindows) {
					ZeroPosition(w);
				}

				foreach ((IntPtr _, frmAbout w) in AboutWindows) {
					ZeroPosition(w);
				}

				foreach ((IntPtr _, frmWatchList w) in WatchListWindows) {
					ZeroPosition(w);
				}

				foreach ((IntPtr _, frmPlotList w) in PlotListWindows) {
					ZeroPosition(w);
				}

				foreach ((IntPtr _, frmStockTicket w) in StockTicketWindows) {
					ZeroPosition(w);
				}

				foreach ((IntPtr _, frmOptionTicket w) in OptionTicketWindows) {
					ZeroPosition(w);
				}

				foreach ((IntPtr _, frmFutureTicket w) in FutureTicketWindows) {
					ZeroPosition(w);
				}

				foreach ((IntPtr _, frmQuickTicket w) in QuickTicketWindows) {
					ZeroPosition(w);
				}

				foreach ((IntPtr _, frmAutoSpreadTicket w) in AutoSpreadTicketWindows) {
					ZeroPosition(w);
				}

				foreach ((IntPtr _, frmFutureMatrixTicket w) in FutureMatrixTicketWindows) {
					ZeroPosition(w);
				}

				foreach ((IntPtr _, frmBatchTicket w) in BatchTicketWindows) {
					ZeroPosition(w);
				}

				foreach ((IntPtr _, frmBatchMarketTicket w) in BatchMarketTicketWindows) {
					ZeroPosition(w);
				}

				foreach ((IntPtr _, frmOrders w) in OrderWindows) {
					ZeroPosition(w);
				}

				foreach ((IntPtr _, frmTrades w) in TradeWindows) {
					ZeroPosition(w);
				}

				foreach ((IntPtr _, frmPositions w) in PositionWindows) {
					ZeroPosition(w);
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		private void ZeroPosition(Form w)
		{
			w.Left = 0;
			w.Top = 0;
		}

		#endregion
	}
}
