using System;
using System.Collections.Generic;
using System.Windows.Forms;

using SerializationEx;

namespace ROC
{
	[Serializable]
	public class UIProfile
	{
		private bool _changed = false;
		private bool changed
		{
			set
			{
				if (_changed == false && _changed != value)
				{
					_changed = value;
				}
			}
		}

		private Dictionary<IntPtr, UIProfileMain> _mainUIProfiles;
		public Dictionary<IntPtr, UIProfileMain> MainUIProfiles
		{
			get
			{
				if (_mainUIProfiles == null)
				{
					_mainUIProfiles = new Dictionary<IntPtr, UIProfileMain>();
				}
				return _mainUIProfiles;
			}
			set
			{
				_mainUIProfiles = value;
			}
		}

		private Dictionary<IntPtr, UIProfileWatchList> _watchListUIProfiles;
		public Dictionary<IntPtr, UIProfileWatchList> WatchListUIProfiles
		{
			get
			{
				if (_watchListUIProfiles == null)
				{
					_watchListUIProfiles = new Dictionary<IntPtr, UIProfileWatchList>();
				}
				return _watchListUIProfiles;
			}
			set
			{
				_watchListUIProfiles = value;
			}
		}

		private Dictionary<IntPtr, UIProfilePlotList> _plotListUIProfiles;
		public Dictionary<IntPtr, UIProfilePlotList> PlotListUIProfiles
		{
			get
			{
				if (_plotListUIProfiles == null)
				{
					_plotListUIProfiles = new Dictionary<IntPtr, UIProfilePlotList>();
				}
				return _plotListUIProfiles;
			}
			set
			{
				_plotListUIProfiles = value;
			}
		}

		private Dictionary<IntPtr, UIProfileStockTicket> _stockTicketUIProfiles;
		public Dictionary<IntPtr, UIProfileStockTicket> StockTicketUIProfiles
		{
			get
			{
				if (_stockTicketUIProfiles == null)
				{
					_stockTicketUIProfiles = new Dictionary<IntPtr, UIProfileStockTicket>();
				}
				return _stockTicketUIProfiles;
			}
			set
			{
				_stockTicketUIProfiles = value;
			}
		}

		private Dictionary<IntPtr, UIProfileOptionTicket> _optionTicketUIProfiles;
		public Dictionary<IntPtr, UIProfileOptionTicket> OptionTicketUIProfiles
		{
			get
			{
				if (_optionTicketUIProfiles == null)
				{
					_optionTicketUIProfiles = new Dictionary<IntPtr, UIProfileOptionTicket>();
				}
				return _optionTicketUIProfiles;
			}
			set
			{
				_optionTicketUIProfiles = value;
			}
		}

		private Dictionary<IntPtr, UIProfileFutureTicket> _futureTicketUIProfiles;
		public Dictionary<IntPtr, UIProfileFutureTicket> FutureTicketUIProfiles
		{
			get
			{
				if (_futureTicketUIProfiles == null)
				{
					_futureTicketUIProfiles = new Dictionary<IntPtr, UIProfileFutureTicket>();
				}
				return _futureTicketUIProfiles;
			}
			set
			{
				_futureTicketUIProfiles = value;
			}
		}

		private Dictionary<IntPtr, UIProfileQuickTicket> _quickTicketUIProfiles;
		public Dictionary<IntPtr, UIProfileQuickTicket> QuickTicketUIProfiles
		{
			get
			{
				if (_quickTicketUIProfiles == null)
				{
					_quickTicketUIProfiles = new Dictionary<IntPtr, UIProfileQuickTicket>();
				}
				return _quickTicketUIProfiles;
			}
			set
			{
				_quickTicketUIProfiles = value;
			}
		}

		private Dictionary<IntPtr, UIProfileBatchTicket> _batchTicketUIProfiles;
		public Dictionary<IntPtr, UIProfileBatchTicket> BatchTicketUIProfiles
		{
			get
			{
				if (_batchTicketUIProfiles == null)
				{
					_batchTicketUIProfiles = new Dictionary<IntPtr, UIProfileBatchTicket>();
				}
				return _batchTicketUIProfiles;
			}
			set
			{
				_batchTicketUIProfiles = value;
			}
		}

		private Dictionary<IntPtr, UIProfileOrders> _ordersUIProfiles;
		public Dictionary<IntPtr, UIProfileOrders> OrdersUIProfiles
		{
			get
			{
				if (_ordersUIProfiles == null)
				{
					_ordersUIProfiles = new Dictionary<IntPtr, UIProfileOrders>();
				}
				return _ordersUIProfiles;
			}
			set
			{
				_ordersUIProfiles = value;
			}
		}

		private Dictionary<IntPtr, UIProfileTrades> _tradesUIProfiles;
		public Dictionary<IntPtr, UIProfileTrades> TradesUIProfiles
		{
			get
			{
				if (_tradesUIProfiles == null)
				{
					_tradesUIProfiles = new Dictionary<IntPtr, UIProfileTrades>();
				}
				return _tradesUIProfiles;
			}
			set
			{
				_tradesUIProfiles = value;
			}
		}

		private Dictionary<IntPtr, UIProfilePositions> _positionsUIProfiles;
		public Dictionary<IntPtr, UIProfilePositions> PositionsUIProfiles
		{
			get
			{
				if (_positionsUIProfiles == null)
				{
					_positionsUIProfiles = new Dictionary<IntPtr, UIProfilePositions>();
				}
				return _positionsUIProfiles;
			}
			set
			{
				_positionsUIProfiles = value;
			}
		}

		public UIProfile()
		{
		}

		#region - Save -

		public void Save()
		{
			_changed = false;

			foreach (frmMain w in GLOBAL.HWindows.MainWindows.Values)
			{
				Save(w);
			}

			foreach (frmWatchList w in GLOBAL.HWindows.WatchListWindows.Values)
			{
				Save(w);
			}

			foreach (frmOrders w in GLOBAL.HWindows.OrderWindows.Values)
			{
				Save(w);
			}

			foreach (frmTrades w in GLOBAL.HWindows.TradeWindows.Values)
			{
				Save(w);
			}

			foreach (frmPositions w in GLOBAL.HWindows.PositionWindows.Values)
			{
				Save(w);
			}

			if (_changed)
			{
				DialogResult dr = MessageBox.Show(String.Concat(new object[] { "Save current profile?" }), "User Profile", MessageBoxButtons.YesNo);
				if (dr == DialogResult.Yes)
				{
					SaveToFile();
				}
			}
		}

		private void SaveToFile()
		{
			ToBinary b = new ToBinary();
			byte[] bytes = b.Serialize(GLOBAL.UserUIProfile, SerializationTypes.Normal);
			string data = System.Convert.ToBase64String(bytes);

			GLOBAL.FILE.Profile.Export(Configuration.Path.Default.ProfilePath, data);

			if (GLOBAL.HROM.Status == HelperROM.StatusTypes.LoggedIn)
			{
				string fileName = String.Concat(new object[] { Configuration.Path.Default.ProfilePath, @"\", GLOBAL.HROM.UserName, ".profile" });
				GLOBAL.FILE.Profile.Export(Configuration.Path.Default.ProfilePath, fileName, data);
			}
		}

		public void Save(frmMain w)
		{
			IntPtr key = w.Handle;
			if (!MainUIProfiles.ContainsKey(key))
			{
				changed = true;
				MainUIProfiles.Add(key, new UIProfileMain());
			}

			changed = MainUIProfiles[key].Update(FormUIProfileFieldID.Top, w.Top);
			changed = MainUIProfiles[key].Update(FormUIProfileFieldID.Left, w.Left);
			changed = MainUIProfiles[key].Update(FormUIProfileFieldID.Width, w.Width);
			changed = MainUIProfiles[key].Update(FormUIProfileFieldID.Height, w.Height);
			changed = MainUIProfiles[key].Update(FormUIProfileFieldID.Font, w.Font);
			changed = MainUIProfiles[key].Update(FormUIProfileFieldID.BackColor, w.BackColor);
		}

		private void Save(frmWatchList w)
		{
			IntPtr key = w.Handle;
			if (!WatchListUIProfiles.ContainsKey(key))
			{
				changed = true;
				WatchListUIProfiles.Add(key, new UIProfileWatchList());
			}

			changed = WatchListUIProfiles[key].Update(FormUIProfileFieldID.Top, w.Top);
			changed = WatchListUIProfiles[key].Update(FormUIProfileFieldID.Left, w.Left);
			changed = WatchListUIProfiles[key].Update(FormUIProfileFieldID.Width, w.Width);
			changed = WatchListUIProfiles[key].Update(FormUIProfileFieldID.Height, w.Height);
			changed = WatchListUIProfiles[key].Update(FormUIProfileFieldID.Font, w.Font);
			changed = WatchListUIProfiles[key].Update(FormUIProfileFieldID.BackColor, w.BackColor);

			changed = WatchListUIProfiles[key].Update(FormUIProfileFieldID.Caption, w.Caption);

			changed = WatchListUIProfiles[key].Update(FormUIProfileFieldID.WatchList, w.ExportGrid());
		}

		private void Save(frmStockTicket w)
		{
			IntPtr key = w.Handle;
			if (!StockTicketUIProfiles.ContainsKey(key))
			{
				changed = true;
				StockTicketUIProfiles.Add(key, new UIProfileStockTicket());
			}

			changed = StockTicketUIProfiles[key].Update(FormUIProfileFieldID.Top, w.Top);
			changed = StockTicketUIProfiles[key].Update(FormUIProfileFieldID.Left, w.Left);
			changed = StockTicketUIProfiles[key].Update(FormUIProfileFieldID.Width, w.Width);
			changed = StockTicketUIProfiles[key].Update(FormUIProfileFieldID.Height, w.Height);
			changed = StockTicketUIProfiles[key].Update(FormUIProfileFieldID.Font, w.Font);
			changed = StockTicketUIProfiles[key].Update(FormUIProfileFieldID.BackColor, w.BackColor);

			changed = StockTicketUIProfiles[key].Update(FormUIProfileFieldID.Caption, w.Caption);
		}

		private void Save(frmFutureTicket w)
		{
			IntPtr key = w.Handle;
			if (!FutureTicketUIProfiles.ContainsKey(key))
			{
				changed = true;
				FutureTicketUIProfiles.Add(key, new UIProfileFutureTicket());
			}

			changed = FutureTicketUIProfiles[key].Update(FormUIProfileFieldID.Top, w.Top);
			changed = FutureTicketUIProfiles[key].Update(FormUIProfileFieldID.Left, w.Left);
			changed = FutureTicketUIProfiles[key].Update(FormUIProfileFieldID.Width, w.Width);
			changed = FutureTicketUIProfiles[key].Update(FormUIProfileFieldID.Height, w.Height);
			changed = FutureTicketUIProfiles[key].Update(FormUIProfileFieldID.Font, w.Font);
			changed = FutureTicketUIProfiles[key].Update(FormUIProfileFieldID.BackColor, w.BackColor);

			changed = FutureTicketUIProfiles[key].Update(FormUIProfileFieldID.Caption, w.Caption);
		}

		private void Save(frmQuickTicket w)
		{
			IntPtr key = w.Handle;
			if (!QuickTicketUIProfiles.ContainsKey(key))
			{
				changed = true;
				QuickTicketUIProfiles.Add(key, new UIProfileQuickTicket());
			}

			changed = QuickTicketUIProfiles[key].Update(FormUIProfileFieldID.Top, w.Top);
			changed = QuickTicketUIProfiles[key].Update(FormUIProfileFieldID.Left, w.Left);
			changed = QuickTicketUIProfiles[key].Update(FormUIProfileFieldID.Width, w.Width);
			changed = QuickTicketUIProfiles[key].Update(FormUIProfileFieldID.Height, w.Height);
			changed = QuickTicketUIProfiles[key].Update(FormUIProfileFieldID.Font, w.Font);
			changed = QuickTicketUIProfiles[key].Update(FormUIProfileFieldID.BackColor, w.BackColor);

			changed = QuickTicketUIProfiles[key].Update(FormUIProfileFieldID.Caption, w.Caption);
		}

		private void Save(frmBatchTicket w)
		{
			IntPtr key = w.Handle;
			if (!BatchTicketUIProfiles.ContainsKey(key))
			{
				changed = true;
				BatchTicketUIProfiles.Add(key, new UIProfileBatchTicket());
			}

			changed = BatchTicketUIProfiles[key].Update(FormUIProfileFieldID.Top, w.Top);
			changed = BatchTicketUIProfiles[key].Update(FormUIProfileFieldID.Left, w.Left);
			changed = BatchTicketUIProfiles[key].Update(FormUIProfileFieldID.Width, w.Width);
			changed = BatchTicketUIProfiles[key].Update(FormUIProfileFieldID.Height, w.Height);
			changed = BatchTicketUIProfiles[key].Update(FormUIProfileFieldID.Font, w.Font);
			changed = BatchTicketUIProfiles[key].Update(FormUIProfileFieldID.BackColor, w.BackColor);

			changed = BatchTicketUIProfiles[key].Update(FormUIProfileFieldID.Caption, w.Caption);
		}

		private void Save(frmOrders w)
		{
			IntPtr key = w.Handle;
			if (!OrdersUIProfiles.ContainsKey(key))
			{
				changed = true;
				OrdersUIProfiles.Add(key, new UIProfileOrders());
			}

			changed = OrdersUIProfiles[key].Update(FormUIProfileFieldID.Top, w.Top);
			changed = OrdersUIProfiles[key].Update(FormUIProfileFieldID.Left, w.Left);
			changed = OrdersUIProfiles[key].Update(FormUIProfileFieldID.Width, w.Width);
			changed = OrdersUIProfiles[key].Update(FormUIProfileFieldID.Height, w.Height);
			changed = OrdersUIProfiles[key].Update(FormUIProfileFieldID.Font, w.Font);
			changed = OrdersUIProfiles[key].Update(FormUIProfileFieldID.BackColor, w.BackColor);

			changed = OrdersUIProfiles[key].Update(FormUIProfileFieldID.Caption, w.Caption);

			changed = OrdersUIProfiles[key].Update(FormUIProfileFieldID.OrdersList, w.ExportGrid());
		}

		private void Save(frmTrades w)
		{
			IntPtr key = w.Handle;
			if (!TradesUIProfiles.ContainsKey(key))
			{
				changed = true;
				TradesUIProfiles.Add(key, new UIProfileTrades());
			}

			changed = TradesUIProfiles[key].Update(FormUIProfileFieldID.Top, w.Top);
			changed = TradesUIProfiles[key].Update(FormUIProfileFieldID.Left, w.Left);
			changed = TradesUIProfiles[key].Update(FormUIProfileFieldID.Width, w.Width);
			changed = TradesUIProfiles[key].Update(FormUIProfileFieldID.Height, w.Height);
			changed = TradesUIProfiles[key].Update(FormUIProfileFieldID.Font, w.Font);
			changed = TradesUIProfiles[key].Update(FormUIProfileFieldID.BackColor, w.BackColor);

			changed = TradesUIProfiles[key].Update(FormUIProfileFieldID.Caption, w.Caption);

			changed = TradesUIProfiles[key].Update(FormUIProfileFieldID.TradesList, w.ExportGrid());
		}

		private void Save(frmPositions w)
		{
			IntPtr key = w.Handle;
			if (!PositionsUIProfiles.ContainsKey(key))
			{
				changed = true;
				PositionsUIProfiles.Add(key, new UIProfilePositions());
			}

			changed = PositionsUIProfiles[key].Update(FormUIProfileFieldID.Top, w.Top);
			changed = PositionsUIProfiles[key].Update(FormUIProfileFieldID.Left, w.Left);
			changed = PositionsUIProfiles[key].Update(FormUIProfileFieldID.Width, w.Width);
			changed = PositionsUIProfiles[key].Update(FormUIProfileFieldID.Height, w.Height);
			changed = PositionsUIProfiles[key].Update(FormUIProfileFieldID.Font, w.Font);
			changed = PositionsUIProfiles[key].Update(FormUIProfileFieldID.BackColor, w.BackColor);

			changed = PositionsUIProfiles[key].Update(FormUIProfileFieldID.Caption, w.Caption);

			changed = PositionsUIProfiles[key].Update(FormUIProfileFieldID.PositionsList, w.ExportGrid());
		}

		#endregion
		
		#region - Load -

		public void Load()
		{
			string data = LoadFromFile();

			if (data != "")
			{
				ToBinary b = new ToBinary();
				byte[] bytes = System.Convert.FromBase64String(data);

				UIProfile prof = (UIProfile)b.Deserialize(bytes, SerializationTypes.Normal);

				foreach (UIProfileMain val in prof.MainUIProfiles.Values)
				{
					Load(val);
				}

				foreach (UIProfileWatchList val in prof.WatchListUIProfiles.Values)
				{
					Load(val);
				}

				foreach (UIProfileOrders val in prof.OrdersUIProfiles.Values)
				{
					Load(val);
				}

				foreach (UIProfileTrades val in prof.TradesUIProfiles.Values)
				{
					Load(val);
				}

				foreach (UIProfilePositions val in prof.PositionsUIProfiles.Values)
				{
					Load(val);
				}
			}
		}

		private string LoadFromFile()
		{
			if (GLOBAL.HROM.Status == HelperROM.StatusTypes.LoggedIn)
			{
				return GLOBAL.FILE.Profile.Import(Configuration.Path.Default.ProfilePath, String.Concat(new object[] { Configuration.Path.Default.ProfilePath, @"\", GLOBAL.HROM.UserName, ".profile" }));
			}
			else
			{
				return GLOBAL.FILE.Profile.Import(Configuration.Path.Default.ProfilePath, String.Concat(new object[] { Configuration.Path.Default.ProfilePath, @"\", "Default.profile" }));
			}
		}

		private void Load(UIProfileMain val)
		{
			frmMain w = GLOBAL.HWindows.OpenWindow(new frmMain());
			
			w.Top = (int)val.top;
			w.Left = (int)val.left;
			w.Width = (int)val.width;
			w.Height = (int)val.height;
			w.Font = val.font;
			w.BackColor = val.backcolor;

			GLOBAL.HWindows.OpenWindow(w);
		}

		private void Load(UIProfileWatchList val)
		{
			frmWatchList w = new frmWatchList();

			w.Top = (int)val.top;
			w.Left = (int)val.left;
			w.Width = (int)val.width;
			w.Height = (int)val.height;
			w.Font = val.font;
			w.BackColor = val.backcolor;

			w.Caption = val.caption;

			w.ImportGrid(val.watchList);

			GLOBAL.HWindows.OpenWindow(w, false);
		}

		private void Load(UIProfilePlotList val)
		{
			frmPlotList w = new frmPlotList();

			w.Top = (int)val.top;
			w.Left = (int)val.left;
			w.Width = (int)val.width;
			w.Height = (int)val.height;
			w.Font = val.font;
			w.BackColor = val.backcolor;

			w.Caption = val.caption;

			GLOBAL.HWindows.OpenWindow(w, false);
		}

		private void Load(UIProfileStockTicket val)
		{
			frmStockTicket w = new frmStockTicket();

			w.Top = (int)val.top;
			w.Left = (int)val.left;
			w.Width = (int)val.width;
			w.Height = (int)val.height;
			w.Font = val.font;
			w.BackColor = val.backcolor;

			w.Caption = val.caption;

			GLOBAL.HWindows.OpenWindow(w, false);
		}

		private void Load(UIProfileOptionTicket val)
		{
			frmOptionTicket w = new frmOptionTicket();

			w.Top = (int)val.top;
			w.Left = (int)val.left;
			w.Width = (int)val.width;
			w.Height = (int)val.height;
			w.Font = val.font;
			w.BackColor = val.backcolor;

			w.Caption = val.caption;

			GLOBAL.HWindows.OpenWindow(w, false);
		}

		private void Load(UIProfileFutureTicket val)
		{
			frmFutureTicket w = new frmFutureTicket();

			w.Top = (int)val.top;
			w.Left = (int)val.left;
			w.Width = (int)val.width;
			w.Height = (int)val.height;
			w.Font = val.font;
			w.BackColor = val.backcolor;

			w.Caption = val.caption;

			GLOBAL.HWindows.OpenWindow(w, false);
		}

		private void Load(UIProfileQuickTicket val)
		{
			frmQuickTicket w = new frmQuickTicket();

			w.Top = (int)val.top;
			w.Left = (int)val.left;
			w.Width = (int)val.width;
			w.Height = (int)val.height;
			w.Font = val.font;
			w.BackColor = val.backcolor;

			w.Caption = val.caption;

			GLOBAL.HWindows.OpenWindow(w, false);
		}

		private void Load(UIProfileBatchTicket val)
		{
			frmBatchTicket w = new frmBatchTicket();

			w.Top = (int)val.top;
			w.Left = (int)val.left;
			w.Width = (int)val.width;
			w.Height = (int)val.height;
			w.Font = val.font;
			w.BackColor = val.backcolor;

			w.Caption = val.caption;

			GLOBAL.HWindows.OpenWindow(w, false);
		}

		private void Load(UIProfileOrders val)
		{
			frmOrders w = new frmOrders();

			w.Top = (int)val.top;
			w.Left = (int)val.left;
			w.Width = (int)val.width;
			w.Height = (int)val.height;
			w.Font = val.font;
			w.BackColor = val.backcolor;

			w.Caption = val.caption;

			w.ImportGrid(val.ordersList);

			GLOBAL.HWindows.OpenWindow(w, false);
		}

		private void Load(UIProfileTrades val)
		{
			frmTrades w = new frmTrades();

			w.Top = (int)val.top;
			w.Left = (int)val.left;
			w.Width = (int)val.width;
			w.Height = (int)val.height;
			w.Font = val.font;
			w.BackColor = val.backcolor;

			w.Caption = val.caption;

			w.ImportGrid(val.tradesList);

			GLOBAL.HWindows.OpenWindow(w, false);
		}

		private void Load(UIProfilePositions val)
		{
			frmPositions w = new frmPositions();

			w.Top = (int)val.top;
			w.Left = (int)val.left;
			w.Width = (int)val.width;
			w.Height = (int)val.height;
			w.Font = val.font;
			w.BackColor = val.backcolor;

			w.Caption = val.caption;

			w.ImportGrid(val.positionsList);

			GLOBAL.HWindows.OpenWindow(w, false);
		}

		#endregion
	}
}
