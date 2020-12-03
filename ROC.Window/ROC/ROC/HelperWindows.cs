using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Drawing;
using ContextMenuEx;

namespace ROC
{
	public sealed class WindowMenuTypes
	{
		public const int ShowAll = 0;
		public const int HideAll = 1;
		public const int WatchLists = 2;
		public const int PlotLists = 3;
		public const int StockTickets = 4;
		public const int OptionTickets = 5;
		public const int FutureTickets = 6;
		public const int QuickTickets = 7;
		public const int BatchTickets = 8;
		public const int BatchMarketTickets = 9;
		public const int AutoSpreadTickets = 10;
		public const int FutureMatrixTickets = 11;
		public const int OrderWindows = 12;
		public const int TradeWindows = 13;
		public const int PositionWindows = 14;
	}

	public sealed class NotificationMenuTypes
	{
		public const int ShowAll = 0;
		public const int HideAll = 1;
		public const int WatchList = 2;
		public const int PlotList = 3;
		public const int StockTicket = 4;
		public const int OptionTicket = 5;
		public const int FutureTicket = 6;
		public const int QuickTicket = 7;
		public const int BatchTicket = 8;
		public const int BatchMarketTickets = 9;
		public const int AutoSpreadTicket = 10;
		public const int FutureMatrixTicket = 11;
		public const int Order = 12;
		public const int Trade = 13;
		public const int Position = 14;
		public const int Windows = 15;
		public const int Exit = 16;
	}

	public class HelperWindows
	{
		public HelperWindows()
		{

		}

		private Dictionary<IntPtr, frmAlert> _alertWindows;
		public Dictionary<IntPtr, frmAlert> AlertWindows
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
		public Dictionary<IntPtr, frmClock> ClockWindows
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

		private Dictionary<IntPtr, frmMain> _mainWindows;
		public Dictionary<IntPtr, frmMain> MainWindows
		{
			get
			{
				if (_mainWindows == null)
				{
					_mainWindows = new Dictionary<IntPtr, frmMain>();
				}
				return _mainWindows;
			}
			set
			{
				_mainWindows = value;
			}
		}

		private Dictionary<IntPtr, frmUserProfile> _userProfileWindows;
		public Dictionary<IntPtr, frmUserProfile> UserProfileWindows
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
		public Dictionary<IntPtr, frmSettings> SettingsWindows
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
		public Dictionary<IntPtr, frmEMail> EMailWindows
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
		public Dictionary<IntPtr, frmAbout> AboutWindows
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
		public Dictionary<IntPtr, frmWatchList> WatchListWindows
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
		public Dictionary<IntPtr, frmPlotList> PlotListWindows
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
		public Dictionary<IntPtr, frmStockTicket> StockTicketWindows
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
		public Dictionary<IntPtr, frmOptionTicket> OptionTicketWindows
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
		public Dictionary<IntPtr, frmFutureTicket> FutureTicketWindows
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
		public Dictionary<IntPtr, frmQuickTicket> QuickTicketWindows
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
		public Dictionary<IntPtr, frmAutoSpreadTicket> AutoSpreadTicketWindows
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
		public Dictionary<IntPtr, frmFutureMatrixTicket> FutureMatrixTicketWindows
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
		public Dictionary<IntPtr, frmBatchTicket> BatchTicketWindows
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
		public Dictionary<IntPtr, frmBatchMarketTicket> BatchMarketTicketWindows
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
		public Dictionary<IntPtr, frmOrders> OrderWindows
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
		public Dictionary<IntPtr, frmTrades> TradeWindows
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
		public Dictionary<IntPtr, frmPositions> PositionWindows
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
		public VistaContextMenu WindowsMenu
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
		public VistaContextMenu NotificationMenu
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
		public bool IsExtended
		{
			get
			{
				foreach (frmTrades w in TradeWindows.Values)
				{
					if (w.Extended)
					{
						return true;
					}
				}

				foreach (frmPositions w in PositionWindows.Values)
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
		public bool BusyShowAll
		{
			get
			{
				return _busyShowAll;
			}
		}

		public void ShowAll()
		{
			if (!_busyShowAll)
			{
				_busyShowAll = true;

				foreach (frmWatchList w in WatchListWindows.Values)
				{
					if (!w.IsProcessing)
					{
						ShowWindow((Form)w);
					}
				}

				foreach (frmPlotList w in PlotListWindows.Values)
				{
					if (!w.IsProcessing)
					{
						ShowWindow((Form)w);
					}
				}

				foreach (frmStockTicket w in StockTicketWindows.Values)
				{
					ShowWindow((Form)w);
				}

				foreach (frmOptionTicket w in OptionTicketWindows.Values)
				{
					if (!w.IsProcessing)
					{
						ShowWindow((Form)w);
					}
				}

				foreach (frmFutureTicket w in FutureTicketWindows.Values)
				{
					ShowWindow((Form)w);
				}

				foreach (frmQuickTicket w in QuickTicketWindows.Values)
				{
					ShowWindow((Form)w);
				}

				foreach (frmAutoSpreadTicket w in AutoSpreadTicketWindows.Values)
				{
					if (!w.IsProcessing)
					{
						ShowWindow((Form)w);
					}
				}

				foreach (frmFutureMatrixTicket w in FutureMatrixTicketWindows.Values)
				{
					if (!w.IsProcessing)
					{
						ShowWindow((Form)w);
					}
				}

				foreach (frmBatchTicket w in BatchTicketWindows.Values)
				{
					if (!w.IsProcessing)
					{
						ShowWindow((Form)w);
					}
				}

				foreach (frmBatchMarketTicket w in BatchMarketTicketWindows.Values)
				{
					if (!w.IsProcessing)
					{
						ShowWindow((Form)w);
					}
				}

				foreach (frmOrders w in OrderWindows.Values)
				{
					if (!w.IsProcessing)
					{
						ShowWindow((Form)w);
					}
				}

				foreach (frmTrades w in TradeWindows.Values)
				{
					if (!w.IsProcessing)
					{
						ShowWindow((Form)w);
					}
				}

				foreach (frmPositions w in PositionWindows.Values)
				{
					if (!w.IsProcessing)
					{
						ShowWindow((Form)w);
					}
				}

				foreach (frmAlert w in AlertWindows.Values)
				{
					ShowWindow((Form)w);
				}

				foreach (frmClock w in ClockWindows.Values)
				{
					ShowWindow((Form)w);
				}

				_busyShowAll = false;
			}
		}

		private void HideAll()
		{
			foreach (frmAlert w in AlertWindows.Values)
			{
				w.Hide();
			}

			foreach (frmClock w in ClockWindows.Values)
			{
				w.Hide();
			}

			foreach (frmWatchList w in WatchListWindows.Values)
			{
				w.Hide();
			}

			foreach (frmPlotList w in PlotListWindows.Values)
			{
				w.Hide();
			}

			foreach (frmStockTicket w in StockTicketWindows.Values)
			{
				w.Hide();
			}

			foreach (frmOptionTicket w in OptionTicketWindows.Values)
			{
				w.Hide();
			}

			foreach (frmFutureTicket w in FutureTicketWindows.Values)
			{
				w.Hide();
			}

			foreach (frmQuickTicket w in QuickTicketWindows.Values)
			{
				w.Hide();
			}

			foreach (frmAutoSpreadTicket w in AutoSpreadTicketWindows.Values)
			{
				w.Hide();
			}

			foreach (frmFutureMatrixTicket w in FutureMatrixTicketWindows.Values)
			{
				w.Hide();
			}

			foreach (frmBatchTicket w in BatchTicketWindows.Values)
			{
				w.Hide();
			}

			foreach (frmBatchMarketTicket w in BatchMarketTicketWindows.Values)
			{
				w.Hide();
			}

			foreach (frmOrders w in OrderWindows.Values)
			{
				w.Hide();
			}

			foreach (frmTrades w in TradeWindows.Values)
			{
				w.Hide();
			}

			foreach (frmPositions w in PositionWindows.Values)
			{
				w.Hide();
			}
		}

		private void LoadWatchLists(ToolStripMenuItem menu)
		{
			menu.DropDownItems.Clear();
			foreach (frmWatchList w in WatchListWindows.Values)
			{
				menu.DropDownItems.Add(new ToolStripMenuItem(w.Caption, null, new EventHandler(WatchLists_MenuEvent), w.Handle.ToString()));
			}
		}

		private void WatchLists_MenuEvent(object sender, EventArgs e)
		{
			foreach (frmWatchList w in WatchListWindows.Values)
			{
				if (w.Caption == ((ToolStripMenuItem)sender).Text && 
					w.Handle.ToString() == ((ToolStripMenuItem)sender).Name)
				{
					ShowWindow((Form)w);
				}
			}
		}

		private void LoadPlotLists(ToolStripMenuItem menu)
		{
			menu.DropDownItems.Clear();
			foreach (frmPlotList w in PlotListWindows.Values)
			{
				menu.DropDownItems.Add(new ToolStripMenuItem(w.Caption, null, new EventHandler(PlotLists_MenuEvent), w.Handle.ToString()));
			}
		}

		private void PlotLists_MenuEvent(object sender, EventArgs e)
		{
			foreach (frmPlotList w in PlotListWindows.Values)
			{
				if (w.Caption == ((ToolStripMenuItem)sender).Text && 
					w.Handle.ToString() == ((ToolStripMenuItem)sender).Name)
				{
					ShowWindow((Form)w);
				}
			}
		}

		private void LoadStockTickets(ToolStripMenuItem menu)
		{
			menu.DropDownItems.Clear();
			foreach (frmStockTicket w in StockTicketWindows.Values)
			{
				menu.DropDownItems.Add(new ToolStripMenuItem(w.Caption, null, new EventHandler(StockTickets_MenuEvent), w.Handle.ToString()));
			}
		}

		private void StockTickets_MenuEvent(object sender, EventArgs e)
		{
			foreach (frmStockTicket w in StockTicketWindows.Values)
			{
				if (w.Caption == ((ToolStripMenuItem)sender).Text && 
					w.Handle.ToString() == ((ToolStripMenuItem)sender).Name)
				{
					ShowWindow((Form)w);
				}
			}
		}

		private void LoadOptionTickets(ToolStripMenuItem menu)
		{
			menu.DropDownItems.Clear();
			foreach (frmOptionTicket w in OptionTicketWindows.Values)
			{
				menu.DropDownItems.Add(new ToolStripMenuItem(w.Caption, null, new EventHandler(OptionTickets_MenuEvent), w.Handle.ToString()));
			}
		}

		private void OptionTickets_MenuEvent(object sender, EventArgs e)
		{
			foreach (frmOptionTicket w in OptionTicketWindows.Values)
			{
				if (w.Caption == ((ToolStripMenuItem)sender).Text && 
					w.Handle.ToString() == ((ToolStripMenuItem)sender).Name)
				{
					ShowWindow((Form)w);
				}
			}
		}

		private void LoadFutureTickets(ToolStripMenuItem menu)
		{
			menu.DropDownItems.Clear();
			foreach (frmFutureTicket w in FutureTicketWindows.Values)
			{
				menu.DropDownItems.Add(new ToolStripMenuItem(w.Caption, null, new EventHandler(FutureTickets_MenuEvent), w.Handle.ToString()));
			}
		}

		private void FutureTickets_MenuEvent(object sender, EventArgs e)
		{
			foreach (frmFutureTicket w in FutureTicketWindows.Values)
			{
				if (w.Caption == ((ToolStripMenuItem)sender).Text && 
					w.Handle.ToString() == ((ToolStripMenuItem)sender).Name)
				{
					ShowWindow((Form)w);
				}
			}
		}

		private void LoadQuickTickets(ToolStripMenuItem menu)
		{
			menu.DropDownItems.Clear();
			foreach (frmQuickTicket w in QuickTicketWindows.Values)
			{
				menu.DropDownItems.Add(new ToolStripMenuItem(w.Caption, null, new EventHandler(QuickTickets_MenuEvent), w.Handle.ToString()));
			}
		}

		private void QuickTickets_MenuEvent(object sender, EventArgs e)
		{
			foreach (frmQuickTicket w in QuickTicketWindows.Values)
			{
				if (w.Caption == ((ToolStripMenuItem)sender).Text && 
					w.Handle.ToString() == ((ToolStripMenuItem)sender).Name)
				{
					ShowWindow((Form)w);
				}
			}
		}

		private void LoadBatchTickets(ToolStripMenuItem menu)
		{
			menu.DropDownItems.Clear();
			foreach (frmBatchTicket w in BatchTicketWindows.Values)
			{
				menu.DropDownItems.Add(new ToolStripMenuItem(w.Caption, null, new EventHandler(BatchTickets_MenuEvent), w.Handle.ToString()));
			}
		}

		private void BatchTickets_MenuEvent(object sender, EventArgs e)
		{
			foreach (frmBatchTicket w in BatchTicketWindows.Values)
			{
				if (w.Caption == ((ToolStripMenuItem)sender).Text && 
					w.Handle.ToString() == ((ToolStripMenuItem)sender).Name)
				{
					ShowWindow((Form)w);
				}
			}
		}

		private void LoadBatchMarketTickets(ToolStripMenuItem menu)
		{
			menu.DropDownItems.Clear();
			foreach (frmBatchMarketTicket w in BatchMarketTicketWindows.Values)
			{
				menu.DropDownItems.Add(new ToolStripMenuItem(w.Caption, null, new EventHandler(BatchMarketTickets_MenuEvent), w.Handle.ToString()));
			}
		}

		private void BatchMarketTickets_MenuEvent(object sender, EventArgs e)
		{
			foreach (frmBatchMarketTicket w in BatchMarketTicketWindows.Values)
			{
				if (w.Caption == ((ToolStripMenuItem)sender).Text &&
					w.Handle.ToString() == ((ToolStripMenuItem)sender).Name)
				{
					ShowWindow((Form)w);
				}
			}
		}

		private void LoadAutoSpreadTickets(ToolStripMenuItem menu)
		{
			menu.DropDownItems.Clear();
			foreach (frmAutoSpreadTicket w in AutoSpreadTicketWindows.Values)
			{
				menu.DropDownItems.Add(new ToolStripMenuItem(w.Caption, null, new EventHandler(AutoSpreadTickets_MenuEvent), w.Handle.ToString()));
			}
		}

		private void AutoSpreadTickets_MenuEvent(object sender, EventArgs e)
		{
			foreach (frmAutoSpreadTicket w in AutoSpreadTicketWindows.Values)
			{
				if (w.Caption == ((ToolStripMenuItem)sender).Text &&
					w.Handle.ToString() == ((ToolStripMenuItem)sender).Name)
				{
					ShowWindow((Form)w);
				}
			}
		}

		private void LoadFutureMatrixTickets(ToolStripMenuItem menu)
		{
			menu.DropDownItems.Clear();
			foreach (frmFutureMatrixTicket w in FutureMatrixTicketWindows.Values)
			{
				menu.DropDownItems.Add(new ToolStripMenuItem(w.Caption, null, new EventHandler(FutureMatrixTickets_MenuEvent), w.Handle.ToString()));
			}
		}

		private void FutureMatrixTickets_MenuEvent(object sender, EventArgs e)
		{
			foreach (frmFutureMatrixTicket w in FutureMatrixTicketWindows.Values)
			{
				if (w.Caption == ((ToolStripMenuItem)sender).Text &&
					w.Handle.ToString() == ((ToolStripMenuItem)sender).Name)
				{
					ShowWindow((Form)w);
				}
			}
		}

		private void LoadOrderWindows(ToolStripMenuItem menu)
		{
			menu.DropDownItems.Clear();
			foreach (frmOrders w in OrderWindows.Values)
			{
				menu.DropDownItems.Add(new ToolStripMenuItem(w.Caption, null, new EventHandler(Order_MenuEvent), w.Handle.ToString()));
			}
		}

		private void Order_MenuEvent(object sender, EventArgs e)
		{
			foreach (frmOrders w in OrderWindows.Values)
			{
				if (w.Caption == ((ToolStripMenuItem)sender).Text &&
					w.Handle.ToString() == ((ToolStripMenuItem)sender).Name)
				{
					ShowWindow((Form)w);
				}
			}
		}

		private void LoadTradeWindows(ToolStripMenuItem menu)
		{
			menu.DropDownItems.Clear();
			foreach (frmTrades w in TradeWindows.Values)
			{
				menu.DropDownItems.Add(new ToolStripMenuItem(w.Caption, null, new EventHandler(Trade_MenuEvent), w.Handle.ToString()));
			}
		}

		private void Trade_MenuEvent(object sender, EventArgs e)
		{
			foreach (frmTrades w in TradeWindows.Values)
			{
				if (w.Caption == ((ToolStripMenuItem)sender).Text && 
					w.Handle.ToString() == ((ToolStripMenuItem)sender).Name)
				{
					ShowWindow((Form)w);
				}
			}
		}

		private void LoadPositionWindows(ToolStripMenuItem menu)
		{
			menu.DropDownItems.Clear();
			foreach (frmPositions w in PositionWindows.Values)
			{
				menu.DropDownItems.Add(new ToolStripMenuItem(w.Caption, null, new EventHandler(Position_MenuEvent), w.Handle.ToString()));
			}
		}

		private void Position_MenuEvent(object sender, EventArgs e)
		{
			foreach (frmPositions w in PositionWindows.Values)
			{
				if (w.Caption == ((ToolStripMenuItem)sender).Text && 
					w.Handle.ToString() == ((ToolStripMenuItem)sender).Name)
				{
					ShowWindow((Form)w);
				}
			}
		}

		#endregion

		private Size _destopSize;
		public Size DesktopSize
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

		public frmAlert OpenWindow(frmAlert w)
		{
			if (AlertWindows.Count > 0)
			{
				foreach (frmAlert wOld in AlertWindows.Values)
				{
					ShowWindow((Form)wOld);
					w = wOld;
				}
			}
			else
			{
				OpenWindow((Form)w);
				AlertWindows.Add(w.Handle, w);
			}

			return w;
		}

		public frmClock OpenWindow(frmClock w)
		{
			if (ClockWindows.Count > 0)
			{
				foreach (frmClock wOld in ClockWindows.Values)
				{
					ShowWindow((Form)wOld);
					w = wOld;
				}
			}
			else
			{
				OpenWindow((Form)w);
				ClockWindows.Add(w.Handle, w);
			}

			return w;
		}

		public frmUserProfile OpenWindow(frmUserProfile w)
		{
			if (UserProfileWindows.Count > 0)
			{
				foreach (frmUserProfile wOld in UserProfileWindows.Values)
				{
					ShowWindow((Form)wOld);
					w = wOld;
				}
			}
			else
			{
				OpenWindow((Form)w);
				UserProfileWindows.Add(w.Handle, w);
			}

			return w;
		}

		public frmSettings OpenWindow(frmSettings w)
		{
			if (SettingsWindows.Count > 0)
			{
				foreach (frmSettings wOld in SettingsWindows.Values)
				{
					ShowWindow((Form)wOld);
					w = wOld;
				}
			}
			else
			{
				OpenWindow((Form)w);
				SettingsWindows.Add(w.Handle, w);
			}

			return w;
		}

		public frmEMail OpenWindow(frmEMail w)
		{
			if (SettingsWindows.Count > 0)
			{
				foreach (frmEMail wOld in EMailWindows.Values)
				{
					ShowWindow((Form)wOld);
					w = wOld;
				}
			}
			else
			{
				OpenWindow((Form)w);
				EMailWindows.Add(w.Handle, w);
			}

			return w;
		}

		public frmAbout OpenWindow(frmAbout w)
		{
			if (AboutWindows.Count > 0)
			{
				foreach (frmAbout wOld in AboutWindows.Values)
				{
					ShowWindow((Form)wOld);
					w = wOld;
				}
			}
			else
			{
				OpenWindow((Form)w);
				AboutWindows.Add(w.Handle, w);
			}

			return w;
		}

		public frmMain OpenWindow(frmMain w)
		{
			if (MainWindows.Count > 0)
			{
				foreach (frmMain wOld in MainWindows.Values)
				{
					ShowWindow((Form)wOld);
					w = wOld;
				}
			}
			else
			{
				MainWindows.Add(w.Handle, w);
			}
			return w;
		}

		public void OpenWindow(frmWatchList w, bool isNew)
		{
			if (isNew)
			{
				OpenWindow((Form)w);
			}
			else
			{
				if (!w.IsProcessing)
				{
					ShowWindow((Form)w);
				}
			}
			WatchListWindows.Add(w.Handle, w);
		}

		public void OpenWindow(frmPlotList w, bool isNew)
		{
			if (isNew)
			{
				OpenWindow((Form)w);
			}
			else
			{
				if (!w.IsProcessing)
				{
					ShowWindow((Form)w);
				}
			}
			PlotListWindows.Add(w.Handle, w);
		}

		public void OpenWindow(frmStockTicket w, bool isNew)
		{
			if (isNew)
			{
				OpenWindow((Form)w);
			}
			else
			{
				ShowWindow((Form)w);
			}
			StockTicketWindows.Add(w.Handle, w);
		}

		public void OpenWindow(frmOptionTicket w, bool isNew)
		{
			if (isNew)
			{
				OpenWindow((Form)w);
			}
			else
			{
				if (!w.IsProcessing)
				{
					ShowWindow((Form)w);
				}
			}
			OptionTicketWindows.Add(w.Handle, w);
		}

		public void OpenWindow(frmFutureTicket w, bool isNew)
		{
			if (isNew)
			{
				OpenWindow((Form)w);
			}
			else
			{
				ShowWindow((Form)w);
			}
			FutureTicketWindows.Add(w.Handle, w);
		}

		public void OpenWindow(frmQuickTicket w, bool isNew)
		{
			if (isNew)
			{
				OpenWindow((Form)w);
			}
			else
			{
				ShowWindow((Form)w);
			}
			QuickTicketWindows.Add(w.Handle, w);
		}

		public void OpenWindow(frmAutoSpreadTicket w, bool isNew)
		{
			if (isNew)
			{
				OpenWindow((Form)w);
			}
			else
			{
				if (!w.IsProcessing)
				{
					ShowWindow((Form)w);
				}
			}
			AutoSpreadTicketWindows.Add(w.Handle, w);
		}

		public void OpenWindow(frmFutureMatrixTicket w, bool isNew)
		{
			if (isNew)
			{
				OpenWindow((Form)w);
			}
			else
			{
				if (!w.IsProcessing)
				{
					ShowWindow((Form)w);
				}
			}
			FutureMatrixTicketWindows.Add(w.Handle, w);
		}

		public void OpenWindow(frmBatchTicket w, bool isNew)
		{
			if (isNew)
			{
				OpenWindow((Form)w);
			}
			else
			{
				if (!w.IsProcessing)
				{
					ShowWindow((Form)w);
				}
			}
			BatchTicketWindows.Add(w.Handle, w);
		}

		public void OpenWindow(frmBatchMarketTicket w, bool isNew)
		{
			if (isNew)
			{
				OpenWindow((Form)w);
			}
			else
			{
				if (!w.IsProcessing)
				{
					ShowWindow((Form)w);
				}
			}
			BatchMarketTicketWindows.Add(w.Handle, w);
		}

		public void OpenWindow(frmOrders w, bool isNew)
		{
			if (isNew)
			{
				OpenWindow((Form)w);
			}
			else
			{
				if (!w.IsProcessing)
				{
					ShowWindow((Form)w);
				}
			}
			OrderWindows.Add(w.Handle, w);
		}

		public void OpenWindow(frmTrades w, bool isNew)
		{
			if (isNew)
			{
				OpenWindow((Form)w);
			}
			else
			{
				if (!w.IsProcessing)
				{
					ShowWindow((Form)w);
				}
			}
			TradeWindows.Add(w.Handle, w);
		}

		public void OpenWindow(frmPositions w, bool isNew)
		{
			if (isNew)
			{
				OpenWindow((Form)w);
			}
			else
			{
				if (!w.IsProcessing)
				{
					ShowWindow((Form)w);
				}
			}
			PositionWindows.Add(w.Handle, w);
		}

		public void OpenWindow(string secType, string symbolDetail)
		{
			if (!CheckTicketWindows(secType, symbolDetail))
			{
				switch (secType)
				{
					case CSVEx.CSVFieldIDs.SecutrityTypes.Equity:
						lock (GLOBAL.HWindows.StockTicketWindows)
						{
							OpenWindow(new frmStockTicket(symbolDetail, Configuration.ROC.Default.ShowLevel2AsDefault), true);
						}
						break;
					case CSVEx.CSVFieldIDs.SecutrityTypes.Option:
						string[] opts = symbolDetail.Split(new string[] { " " }, StringSplitOptions.RemoveEmptyEntries);
						if (opts.Length > 0)
						{
							lock (GLOBAL.HWindows.OptionTicketWindows)
							{
								OpenWindow(new frmOptionTicket(opts[0]), true);
							}
						}
						break;
					case CSVEx.CSVFieldIDs.SecutrityTypes.Future:
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
				case CSVEx.CSVFieldIDs.SecutrityTypes.Equity:
					foreach (frmStockTicket w in StockTicketWindows.Values)
					{
						if (w.CurrentSymbolDetail == symbolDetail)
						{
							ShowWindow((Form)w);
							found = true;
							break;
						}
					}
					break;
				case CSVEx.CSVFieldIDs.SecutrityTypes.Option:
					string[] opts = symbolDetail.Split(new string[] { " " }, StringSplitOptions.RemoveEmptyEntries);
					if (opts.Length > 0)
					{
						foreach (frmOptionTicket w in OptionTicketWindows.Values)
						{
							if (w.CurrentSymbolDetail == opts[0])
							{
								ShowWindow((Form)w);
								found = true;
								break;
							}
						}
					}
					break;
				case CSVEx.CSVFieldIDs.SecutrityTypes.Future:
					foreach (frmFutureTicket w in FutureTicketWindows.Values)
					{
						if (w.CurrentSymbolDetail == symbolDetail)
						{
							ShowWindow((Form)w);
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

		public void CloseWindow(frmAlert w)
		{
			CloseWindow((Form)w);
			AlertWindows.Remove(w.Handle);
		}

		public void CloseWindow(frmClock w)
		{
			CloseWindow((Form)w);
			ClockWindows.Remove(w.Handle);
		}

		public void CloseWindow(frmAbout w)
		{
			CloseWindow((Form)w);
			AboutWindows.Remove(w.Handle);
		}

		public void CloseWindow(frmUserProfile w)
		{
			CloseWindow((Form)w);
			UserProfileWindows.Remove(w.Handle);
		}

		public void CloseWindow(frmSettings w)
		{
			CloseWindow((Form)w);
			SettingsWindows.Remove(w.Handle);
		}

		public void CloseWindow(frmEMail w)
		{
			CloseWindow((Form)w);
			EMailWindows.Remove(w.Handle);
		}

		public void CloseWindow(frmWatchList w)
		{
			CloseWindow((Form)w);
			WatchListWindows.Remove(w.Handle);
		}

		public void CloseWindow(frmPlotList w)
		{
			CloseWindow((Form)w);
			PlotListWindows.Remove(w.Handle);
		}

		public void CloseWindow(frmStockTicket w)
		{
			CloseWindow((Form)w);
			StockTicketWindows.Remove(w.Handle);
		}

		public void CloseWindow(frmOptionTicket w)
		{
			CloseWindow((Form)w);
			OptionTicketWindows.Remove(w.Handle);
		}

		public void CloseWindow(frmFutureTicket w)
		{
			CloseWindow((Form)w);
			FutureTicketWindows.Remove(w.Handle);
		}

		public void CloseWindow(frmQuickTicket w)
		{
			CloseWindow((Form)w);
			QuickTicketWindows.Remove(w.Handle);
		}

		public void CloseWindow(frmAutoSpreadTicket w)
		{
			CloseWindow((Form)w);
			AutoSpreadTicketWindows.Remove(w.Handle);
		}

		public void CloseWindow(frmFutureMatrixTicket w)
		{
			CloseWindow((Form)w);
			FutureMatrixTicketWindows.Remove(w.Handle);
		}

		public void CloseWindow(frmBatchTicket w)
		{
			CloseWindow((Form)w);
			BatchTicketWindows.Remove(w.Handle);
		}

		public void CloseWindow(frmBatchMarketTicket w)
		{
			CloseWindow((Form)w);
			BatchMarketTicketWindows.Remove(w.Handle);
		}

		public void CloseWindow(frmOrders w)
		{
			CloseWindow((Form)w);
			OrderWindows.Remove(w.Handle);
		}

		public void CloseWindow(frmTrades w)
		{
			CloseWindow((Form)w);
			TradeWindows.Remove(w.Handle);
		}

		public void CloseWindow(frmPositions w)
		{
			CloseWindow((Form)w);
			PositionWindows.Remove(w.Handle);
		}

		#endregion

		#region - Zero All Window -

		public void ZeroAllWindowPosition()
		{
			try
			{
				foreach (frmAlert w in AlertWindows.Values)
				{
					ZeroPosition((Form)w);
				}

				foreach (frmClock w in ClockWindows.Values)
				{
					ZeroPosition((Form)w);
				}

				foreach (frmMain w in MainWindows.Values)
				{
					ZeroPosition((Form)w);
				}

				foreach (frmUserProfile w in UserProfileWindows.Values)
				{
					ZeroPosition((Form)w);
				}

				foreach (frmAbout w in AboutWindows.Values)
				{
					ZeroPosition((Form)w);
				}

				foreach (frmWatchList w in WatchListWindows.Values)
				{
					ZeroPosition((Form)w);
				}

				foreach (frmPlotList w in PlotListWindows.Values)
				{
					ZeroPosition((Form)w);
				}

				foreach (frmStockTicket w in StockTicketWindows.Values)
				{
					ZeroPosition((Form)w);
				}

				foreach (frmOptionTicket w in OptionTicketWindows.Values)
				{
					ZeroPosition((Form)w);
				}

				foreach (frmFutureTicket w in FutureTicketWindows.Values)
				{
					ZeroPosition((Form)w);
				}

				foreach (frmQuickTicket w in QuickTicketWindows.Values)
				{
					ZeroPosition((Form)w);
				}

				foreach (frmAutoSpreadTicket w in AutoSpreadTicketWindows.Values)
				{
					ZeroPosition((Form)w);
				}

				foreach (frmFutureMatrixTicket w in FutureMatrixTicketWindows.Values)
				{
					ZeroPosition((Form)w);
				}

				foreach (frmBatchTicket w in BatchTicketWindows.Values)
				{
					ZeroPosition((Form)w);
				}

				foreach (frmBatchMarketTicket w in BatchMarketTicketWindows.Values)
				{
					ZeroPosition((Form)w);
				}

				foreach (frmOrders w in OrderWindows.Values)
				{
					ZeroPosition((Form)w);
				}

				foreach (frmTrades w in TradeWindows.Values)
				{
					ZeroPosition((Form)w);
				}

				foreach (frmPositions w in PositionWindows.Values)
				{
					ZeroPosition((Form)w);
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
