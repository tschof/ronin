using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Runtime.Serialization;
using System.Drawing;

using SerializationEx;
using System.Diagnostics;

namespace ROC
{
	[Serializable]
	public class UIProfile : IDisposable
	{
		#region - IDisposable Members -

		public void Dispose()
		{
			MainUIProfiles.Clear();
			WatchListUIProfiles.Clear();
			PlotListUIProfiles.Clear();
			StockTicketUIProfiles.Clear();
			OptionTicketUIProfiles.Clear();
			FutureTicketUIProfiles.Clear();
			QuickTicketUIProfiles.Clear();
			BatchTicketUIProfiles.Clear();
			OrdersUIProfiles.Clear();
			TradesUIProfiles.Clear();
			PositionsUIProfiles.Clear();
		}

		#endregion

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

		private Color _positiveColor = Color.LimeGreen;
		public Color PositiveColor
		{
			get
			{
				return _positiveColor;
			}
			set
			{
				_positiveColor = value;
			}
		}

		private Color _negativeColor = Color.Red;
		public Color NegativeColor
		{
			get
			{
				return _negativeColor;
			}
			set
			{
				_negativeColor = value;
			}
		}

		private Color _normalColor = Color.White;
		public Color NormalColor
		{
			get
			{
				return _normalColor;
			}
			set
			{
				_normalColor = value;
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

		private Dictionary<IntPtr, UIProfileClock> _clockUIProfiles;
		public Dictionary<IntPtr, UIProfileClock> ClockUIProfiles
		{
			get
			{
				if (_clockUIProfiles == null)
				{
					_clockUIProfiles = new Dictionary<IntPtr, UIProfileClock>();
				}
				return _clockUIProfiles;
			}
			set
			{
				_clockUIProfiles = value;
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

		private Dictionary<IntPtr, UIProfileAutoSpreadTicket> _autoSpreadTicketUIProfiles;
		public Dictionary<IntPtr, UIProfileAutoSpreadTicket> AutoSpreadTicketUIProfiles
		{
			get
			{
				if (_autoSpreadTicketUIProfiles == null)
				{
					_autoSpreadTicketUIProfiles = new Dictionary<IntPtr, UIProfileAutoSpreadTicket>();
				}
				return _autoSpreadTicketUIProfiles;
			}
			set
			{
				_autoSpreadTicketUIProfiles = value;
			}
		}

		private Dictionary<IntPtr, UIProfileFutureMatrixTicket> _futureMatrixTicketUIProfiles;
		public Dictionary<IntPtr, UIProfileFutureMatrixTicket> FutureMatrixTicketUIProfiles
		{
			get
			{
				if (_futureMatrixTicketUIProfiles == null)
				{
					_futureMatrixTicketUIProfiles = new Dictionary<IntPtr, UIProfileFutureMatrixTicket>();
				}
				return _futureMatrixTicketUIProfiles;
			}
			set
			{
				_futureMatrixTicketUIProfiles = value;
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

		private Dictionary<IntPtr, UIProfileBatchMarketTicket> _batchMarketTicketUIProfiles;
		public Dictionary<IntPtr, UIProfileBatchMarketTicket> BatchMarketTicketUIProfiles
		{
			get
			{
				if (_batchMarketTicketUIProfiles == null)
				{
					_batchMarketTicketUIProfiles = new Dictionary<IntPtr, UIProfileBatchMarketTicket>();
				}
				return _batchMarketTicketUIProfiles;
			}
			set
			{
				_batchMarketTicketUIProfiles = value;
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

		public UIProfile(SerializationInfo info, StreamingContext ctxt)
		{
		}

		#region - Save -

		public void SaveAllOrderInfo()
		{
			try
			{
				GLOBAL.HOrders.Export();
				GLOBAL.HExecutions.Export();

				// All ROM Order and Execution are saved locally, its safe to load from local dat file on next restart
				//Configuration.User.Default.UseROMDatabase = false;
				//Configuration.User.Default.Save();
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		public void SaveAllUIProfile()
		{
			ClearAllUIProfile();
			SaveAll();
		}

		public void Save(bool isAuto)
		{
			Save("", "", isAuto);
		}

		public void Save(string filePath, string fileName, bool isAuto)
		{
			_changed = false;

			ClearAllUIProfile();
			SaveAll();

			if (_changed)
			{
				if (isAuto)
				{
					SaveToFile(filePath, fileName);
				}
				else
				{
					DialogResult dr = MessageBox.Show(String.Concat(new object[] { "Save current profile?" }), "User Profile", MessageBoxButtons.YesNo);
					if (dr == DialogResult.Yes)
					{
						SaveToFile(filePath, fileName);
					}
					else
					{
						System.Threading.Thread.Sleep(1000);
						Application.DoEvents();
					}
				}
			}
		}

		private void SaveToFile(string filePath, string fileName)
		{
			byte[] bytes = new ToBinary().Serialize(GLOBAL.UserUIProfile, SerializationTypes.Normal);
			string data = System.Convert.ToBase64String(bytes);

			if (fileName == "")
			{
				if (GLOBAL.HROM.UserName != "")
				{
					fileName = String.Concat(new object[] { GLOBAL.HROM.UserName, ".profile" });
				}
				else
				{
					fileName = "Default.profile";
				}
			}
			if (filePath == "")
			{
				HelperFile.Save(data, Configuration.Path.Default.ProfilePath, fileName);
			}
			else
			{
				HelperFile.Save(data, filePath, fileName, filePath == Configuration.Path.Default.ProfilePath);
			}
			
			//GLOBAL.FILE.Profile.Export(Configuration.Path.Default.ProfilePath, data);

			//if (GLOBAL.HROM.Status == HelperROM.StatusTypes.LoggedIn)
			//{
			//    if (fileName == "")
			//    {
			//        fileName = String.Concat(new object[] { Configuration.Path.Default.ProfilePath, @"\", GLOBAL.HROM.UserName, ".profile" });
			//    }
			//    GLOBAL.FILE.Profile.Export(Configuration.Path.Default.ProfilePath, fileName, data);
			//}
		}

		private void ClearAllUIProfile()
		{
			MainUIProfiles.Clear();
			WatchListUIProfiles.Clear();
			PlotListUIProfiles.Clear();
			StockTicketUIProfiles.Clear();
			OptionTicketUIProfiles.Clear();
			FutureTicketUIProfiles.Clear();
			QuickTicketUIProfiles.Clear();
			AutoSpreadTicketUIProfiles.Clear();
			FutureMatrixTicketUIProfiles.Clear();
			BatchTicketUIProfiles.Clear();
			BatchMarketTicketUIProfiles.Clear();
			OrdersUIProfiles.Clear();
			TradesUIProfiles.Clear();
			PositionsUIProfiles.Clear();
		}

		#endregion

		#region - Save All Windows -

		private void SaveAll()
		{
			GLOBAL.HSymbolSettingData.Export();
			GLOBAL.HQuickButtonSettingData.Export();

			foreach (frmMain w in GLOBAL.HWindows.MainWindows.Values)
			{
				Save(w);
			}

			foreach (frmClock w in GLOBAL.HWindows.ClockWindows.Values)
			{
				Save(w);
			}

			foreach (frmWatchList w in GLOBAL.HWindows.WatchListWindows.Values)
			{
				Save(w);
			}

			foreach (frmPlotList w in GLOBAL.HWindows.PlotListWindows.Values)
			{
				Save(w);
			}

			foreach (frmStockTicket w in GLOBAL.HWindows.StockTicketWindows.Values)
			{
				Save(w);
			}

			foreach (frmOptionTicket w in GLOBAL.HWindows.OptionTicketWindows.Values)
			{
				Save(w);
			}

			foreach (frmFutureTicket w in GLOBAL.HWindows.FutureTicketWindows.Values)
			{
				Save(w);
			}

			foreach (frmQuickTicket w in GLOBAL.HWindows.QuickTicketWindows.Values)
			{
				Save(w);
			}

			foreach (frmAutoSpreadTicket w in GLOBAL.HWindows.AutoSpreadTicketWindows.Values)
			{
				Save(w);
			}

			foreach (frmFutureMatrixTicket w in GLOBAL.HWindows.FutureMatrixTicketWindows.Values)
			{
				Save(w);
			}

			foreach (frmBatchTicket w in GLOBAL.HWindows.BatchTicketWindows.Values)
			{
				Save(w);
			}

			foreach (frmBatchMarketTicket w in GLOBAL.HWindows.BatchMarketTicketWindows.Values)
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

			changed = MainUIProfiles[key].Update(FormUIProfileFieldID.InSystemTray, w.InSystemTray);
		}

		public void Save(frmClock w)
		{
			IntPtr key = w.Handle;
			if (!ClockUIProfiles.ContainsKey(key))
			{
				changed = true;
				ClockUIProfiles.Add(key, new UIProfileClock());
			}

			changed = ClockUIProfiles[key].Update(FormUIProfileFieldID.Top, w.Top);
			changed = ClockUIProfiles[key].Update(FormUIProfileFieldID.Left, w.Left);
			changed = ClockUIProfiles[key].Update(FormUIProfileFieldID.Width, w.Width);
			changed = ClockUIProfiles[key].Update(FormUIProfileFieldID.Height, w.Height);
			changed = ClockUIProfiles[key].Update(FormUIProfileFieldID.Font, w.Font);
			changed = ClockUIProfiles[key].Update(FormUIProfileFieldID.BackColor, w.BackColor);

			changed = ClockUIProfiles[key].Update(FormUIProfileFieldID.Pinned, w.TopMost);
			changed = ClockUIProfiles[key].Update(FormUIProfileFieldID.ClockDisplayFormat, w.ClockDisplayFormat);
			changed = ClockUIProfiles[key].Update(FormUIProfileFieldID.ClockTimeZoneDisplayFormat, w.ClockTimeZoneDisplayFormat);
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
			changed = WatchListUIProfiles[key].Update(FormUIProfileFieldID.Pinned, w.TopMost);

			changed = WatchListUIProfiles[key].Update(FormUIProfileFieldID.WatchList, w.ExportGrid());
		}

		private void Save(frmPlotList w)
		{
			IntPtr key = w.Handle;
			if (!PlotListUIProfiles.ContainsKey(key))
			{
				changed = true;
				PlotListUIProfiles.Add(key, new UIProfilePlotList());
			}

			changed = PlotListUIProfiles[key].Update(FormUIProfileFieldID.Top, w.Top);
			changed = PlotListUIProfiles[key].Update(FormUIProfileFieldID.Left, w.Left);
			changed = PlotListUIProfiles[key].Update(FormUIProfileFieldID.Width, w.Width);
			changed = PlotListUIProfiles[key].Update(FormUIProfileFieldID.Height, w.Height);
			changed = PlotListUIProfiles[key].Update(FormUIProfileFieldID.Font, w.Font);
			changed = PlotListUIProfiles[key].Update(FormUIProfileFieldID.BackColor, w.BackColor);

			changed = PlotListUIProfiles[key].Update(FormUIProfileFieldID.Caption, w.Caption);
			changed = PlotListUIProfiles[key].Update(FormUIProfileFieldID.Pinned, w.TopMost);

			changed = PlotListUIProfiles[key].Update(FormUIProfileFieldID.PlotList, w.ExportGrid());
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
			//GLOBAL.HROC.AddToStatusLogs("Save StockWindow: " + w.Name + " " + w.Height);
			changed = StockTicketUIProfiles[key].Update(FormUIProfileFieldID.Font, w.Font);
			changed = StockTicketUIProfiles[key].Update(FormUIProfileFieldID.BackColor, w.BackColor);

			changed = StockTicketUIProfiles[key].Update(FormUIProfileFieldID.Caption, w.Caption);
			changed = StockTicketUIProfiles[key].Update(FormUIProfileFieldID.Pinned, w.TopMost);

			changed = StockTicketUIProfiles[key].Update(FormUIProfileFieldID.Level2BidList, w.ExportBidGrid());
			changed = StockTicketUIProfiles[key].Update(FormUIProfileFieldID.Level2AskList, w.ExportAskGrid());

			changed = StockTicketUIProfiles[key].Update(FormUIProfileFieldID.StockTicket, w.ExportTicket());
		}

		private void Save(frmOptionTicket w)
		{
			IntPtr key = w.Handle;
			if (!OptionTicketUIProfiles.ContainsKey(key))
			{
				changed = true;
				OptionTicketUIProfiles.Add(key, new UIProfileOptionTicket());
			}

			changed = OptionTicketUIProfiles[key].Update(FormUIProfileFieldID.Top, w.Top);
			changed = OptionTicketUIProfiles[key].Update(FormUIProfileFieldID.Left, w.Left);
			changed = OptionTicketUIProfiles[key].Update(FormUIProfileFieldID.Width, w.Width);
			changed = OptionTicketUIProfiles[key].Update(FormUIProfileFieldID.Height, w.Height);
			changed = OptionTicketUIProfiles[key].Update(FormUIProfileFieldID.Font, w.Font);
			changed = OptionTicketUIProfiles[key].Update(FormUIProfileFieldID.BackColor, w.BackColor);

			changed = OptionTicketUIProfiles[key].Update(FormUIProfileFieldID.Caption, w.Caption);
			changed = OptionTicketUIProfiles[key].Update(FormUIProfileFieldID.Pinned, w.TopMost);
			
			changed = OptionTicketUIProfiles[key].Update(FormUIProfileFieldID.OptionList, w.ExportOptionGrid());

			changed = OptionTicketUIProfiles[key].Update(FormUIProfileFieldID.OptionTicket, w.ExportTicket());
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
			changed = FutureTicketUIProfiles[key].Update(FormUIProfileFieldID.Pinned, w.TopMost);

			changed = FutureTicketUIProfiles[key].Update(FormUIProfileFieldID.BookBidList, w.ExportBidGrid());
			changed = FutureTicketUIProfiles[key].Update(FormUIProfileFieldID.BookAskList, w.ExportAskGrid());

			changed = FutureTicketUIProfiles[key].Update(FormUIProfileFieldID.FutureTicket, w.ExportTicket());
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
			changed = QuickTicketUIProfiles[key].Update(FormUIProfileFieldID.Pinned, w.TopMost);

			changed = QuickTicketUIProfiles[key].Update(FormUIProfileFieldID.QuickList, w.ExportQuickGrid());

			changed = QuickTicketUIProfiles[key].Update(FormUIProfileFieldID.QuickTicket, w.ExportTicket());
		}

		private void Save(frmAutoSpreadTicket w)
		{
			IntPtr key = w.Handle;
			if (!AutoSpreadTicketUIProfiles.ContainsKey(key))
			{
				changed = true;
				AutoSpreadTicketUIProfiles.Add(key, new UIProfileAutoSpreadTicket());
			}

			changed = AutoSpreadTicketUIProfiles[key].Update(FormUIProfileFieldID.Top, w.Top);
			changed = AutoSpreadTicketUIProfiles[key].Update(FormUIProfileFieldID.Left, w.Left);
			changed = AutoSpreadTicketUIProfiles[key].Update(FormUIProfileFieldID.Width, w.Width);
			changed = AutoSpreadTicketUIProfiles[key].Update(FormUIProfileFieldID.Height, w.Height);
			changed = AutoSpreadTicketUIProfiles[key].Update(FormUIProfileFieldID.Font, w.Font);
			changed = AutoSpreadTicketUIProfiles[key].Update(FormUIProfileFieldID.BackColor, w.BackColor);

			changed = AutoSpreadTicketUIProfiles[key].Update(FormUIProfileFieldID.Caption, w.Caption);
			changed = AutoSpreadTicketUIProfiles[key].Update(FormUIProfileFieldID.Pinned, w.TopMost);

			changed = AutoSpreadTicketUIProfiles[key].Update(FormUIProfileFieldID.AutoSpreadSettingsList, w.ExportAutoSpreadSettingsGrid());
			changed = AutoSpreadTicketUIProfiles[key].Update(FormUIProfileFieldID.AutoSpreadList, w.ExportAutoSpreadGrid());
			changed = AutoSpreadTicketUIProfiles[key].Update(FormUIProfileFieldID.AutoSpreadTicket, w.ExportTicket());
		}

		private void Save(frmFutureMatrixTicket w)
		{
			IntPtr key = w.Handle;
			if (!FutureMatrixTicketUIProfiles.ContainsKey(key))
			{
				changed = true;
				FutureMatrixTicketUIProfiles.Add(key, new UIProfileFutureMatrixTicket());
			}

			changed = FutureMatrixTicketUIProfiles[key].Update(FormUIProfileFieldID.Top, w.Top);
			changed = FutureMatrixTicketUIProfiles[key].Update(FormUIProfileFieldID.Left, w.Left);
			changed = FutureMatrixTicketUIProfiles[key].Update(FormUIProfileFieldID.Width, w.Width);
			changed = FutureMatrixTicketUIProfiles[key].Update(FormUIProfileFieldID.Height, w.Height);
			changed = FutureMatrixTicketUIProfiles[key].Update(FormUIProfileFieldID.Font, w.Font);
			changed = FutureMatrixTicketUIProfiles[key].Update(FormUIProfileFieldID.BackColor, w.BackColor);

			changed = FutureMatrixTicketUIProfiles[key].Update(FormUIProfileFieldID.Caption, w.Caption);
			changed = FutureMatrixTicketUIProfiles[key].Update(FormUIProfileFieldID.Pinned, w.TopMost);

			changed = FutureMatrixTicketUIProfiles[key].Update(FormUIProfileFieldID.FutureMatrixRange, w.MatrixRange);
			changed = FutureMatrixTicketUIProfiles[key].Update(FormUIProfileFieldID.FutureMatrixInterval, w.MatrixInterval);

			changed = FutureMatrixTicketUIProfiles[key].Update(FormUIProfileFieldID.FutureMatrixTicket, w.ExportTicket());
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
			changed = BatchTicketUIProfiles[key].Update(FormUIProfileFieldID.Pinned, w.TopMost);

			changed = BatchTicketUIProfiles[key].Update(FormUIProfileFieldID.BatchList, w.ExportGrid());
		}

		private void Save(frmBatchMarketTicket w)
		{
			IntPtr key = w.Handle;
			if (!BatchMarketTicketUIProfiles.ContainsKey(key))
			{
				changed = true;
				BatchMarketTicketUIProfiles.Add(key, new UIProfileBatchMarketTicket());
			}

			changed = BatchMarketTicketUIProfiles[key].Update(FormUIProfileFieldID.Top, w.Top);
			changed = BatchMarketTicketUIProfiles[key].Update(FormUIProfileFieldID.Left, w.Left);
			changed = BatchMarketTicketUIProfiles[key].Update(FormUIProfileFieldID.Width, w.Width);
			changed = BatchMarketTicketUIProfiles[key].Update(FormUIProfileFieldID.Height, w.Height);
			changed = BatchMarketTicketUIProfiles[key].Update(FormUIProfileFieldID.Font, w.Font);
			changed = BatchMarketTicketUIProfiles[key].Update(FormUIProfileFieldID.BackColor, w.BackColor);

			changed = BatchMarketTicketUIProfiles[key].Update(FormUIProfileFieldID.Caption, w.Caption);
			changed = BatchMarketTicketUIProfiles[key].Update(FormUIProfileFieldID.Pinned, w.TopMost);

			changed = BatchMarketTicketUIProfiles[key].Update(FormUIProfileFieldID.BatchMarketList, w.ExportGrid());
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
			changed = OrdersUIProfiles[key].Update(FormUIProfileFieldID.Pinned, w.TopMost);

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
			changed = TradesUIProfiles[key].Update(FormUIProfileFieldID.Pinned, w.TopMost);

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
			changed = PositionsUIProfiles[key].Update(FormUIProfileFieldID.Pinned, w.TopMost);

			changed = PositionsUIProfiles[key].Update(FormUIProfileFieldID.PositionsList, w.ExportGrid());
			changed = PositionsUIProfiles[key].Update(FormUIProfileFieldID.PositionsListGroupBy, w.GroupBy);

			changed = PositionsUIProfiles[key].Update(FormUIProfileFieldID.PositionsShowOpenQty, w.ShowOpenQty);
			changed = PositionsUIProfiles[key].Update(FormUIProfileFieldID.PositionsShowBuyQty, w.ShowBuyQty);
			changed = PositionsUIProfiles[key].Update(FormUIProfileFieldID.PositionsShowSellQty, w.ShowSellQty);

			changed = PositionsUIProfiles[key].Update(FormUIProfileFieldID.PositionsShowOpenPnL, w.ShowOpenPnL);
			changed = PositionsUIProfiles[key].Update(FormUIProfileFieldID.PositionsShowDayPnL, w.ShowDayPnL);
			changed = PositionsUIProfiles[key].Update(FormUIProfileFieldID.PositionsShowRealizedPnL, w.ShowRealizedPnL);
			changed = PositionsUIProfiles[key].Update(FormUIProfileFieldID.PositionsShowTotalPnL, w.ShowTotalPnL);
			changed = PositionsUIProfiles[key].Update(FormUIProfileFieldID.PositionsShowCurrentQty, w.ShowCurrentQty);
			changed = PositionsUIProfiles[key].Update(FormUIProfileFieldID.PositionsShowAllAggregation, w.ShowAllAggregation);
		}

		#endregion

		#region - Load -

		public void LoadAllOrderInfo(string username)
		{
			if (!Configuration.User.Default.UseROMDatabase)
			{
				GLOBAL.HOrders.Import(username);
				GLOBAL.HExecutions.Import(username);
			}
		}

		public void Load()
		{
			string data = LoadFromFile("", "");

			if (data != "")
			{
				byte[] bytes = System.Convert.FromBase64String(data);
				UIProfile prof = (UIProfile)new ToBinary().Deserialize(bytes, SerializationTypes.Normal);

				if (prof != null)
				{
					foreach (UIProfileMain val in prof.MainUIProfiles.Values)
					{
						Load(val);
					}

					LoadAll(prof);
				}
			}
		}

		private string LoadFromFile(string filePath, string fileName)
		{
			string result = "";
			if (filePath == "")
			{
				filePath = Configuration.Path.Default.ProfilePath;
			}

			if (GLOBAL.HROM.Status == HelperROM.StatusTypes.LoggedIn)
			{
				if (fileName != "")
				{
					result = HelperFile.Load(filePath, fileName, filePath == Configuration.Path.Default.ProfilePath);

					if (result == "")
					{
						result = HelperFile.Load(filePath, fileName, false);
					}
				}
				else
				{
					fileName = String.Concat(new object[] { GLOBAL.HROM.UserName, ".profile" });
					result = HelperFile.Load(filePath, fileName);

					if (result == "")
					{
						result = HelperFile.Load(filePath, fileName, false);
					}
				}
			}
			else
			{
				fileName = "Default.profile";
				result = HelperFile.Load(filePath, fileName);
				
				if (result == "")
				{
					result = HelperFile.Load(filePath, fileName, false);
				}
			}

			return result;
		}

		private void Load(UIProfileMain val)
		{
			frmMain w = GLOBAL.HWindows.OpenWindow(new frmMain());

			w.Top = (int)val.top;
			w.Left = (int)val.left;
			w.Width = (int)val.width;
			//w.Height = (int)val.height;
			w.Height = w.MaximumSize.Height;
			w.Font = val.font;
			//w.BackColor = val.backcolor;
			w.BackColor = Color.FromKnownColor(KnownColor.Control);
			w.InSystemTray = val.inSystemTray;

			GLOBAL.HWindows.OpenWindow(w);
		}

		#endregion

		#region - Reload All Windows -

		public void ReLoad()
		{
			ReLoad("", "");
		}
		public void ReLoad(string filePath, string fileName)
		{
			string data = LoadFromFile(filePath, fileName);

			if (data != "")
			{
				byte[] bytes = System.Convert.FromBase64String(data);
				UIProfile prof = (UIProfile)new ToBinary().Deserialize(bytes, SerializationTypes.Normal);

				if (prof != null)
				{
					foreach (UIProfileMain val in prof.MainUIProfiles.Values)
					{
						ReLoad(val);
					}

					UnLoadAll();

					GLOBAL.HRDS.ResetSecurityInfo();

					LoadAll(prof);
				}

				prof.Dispose();
			}

			data = "";
		}

		private void ReLoad(UIProfileMain val)
		{
			GLOBAL.MainForm.Top = (int)val.top;
			GLOBAL.MainForm.Left = (int)val.left;
			GLOBAL.MainForm.Width = (int)val.width;
			GLOBAL.MainForm.Height = (int)val.height;
			GLOBAL.MainForm.Font = val.font;
			GLOBAL.MainForm.BackColor = val.backcolor;
		}

		// Only reload the user interface
		public void ReloadUI()
		{
			foreach (frmStockTicket w in GLOBAL.HWindows.StockTicketWindows.Values)
			{
				w.ReloadTicket();
			}

			foreach (frmFutureTicket w in GLOBAL.HWindows.FutureTicketWindows.Values)
			{
				w.ReloadTicket();
			}

			foreach (frmOptionTicket w in GLOBAL.HWindows.OptionTicketWindows.Values)
			{
				w.ReloadTicket();
			}

			foreach (frmQuickTicket w in GLOBAL.HWindows.QuickTicketWindows.Values)
			{
				w.ReloadTicket();
			}
		}

		#endregion

		#region - Load All Windows -

		private void LoadAll(UIProfile prof)
		{
			GLOBAL.HSymbolSettingData = new HelperSymbolSettingData();
			GLOBAL.HSymbolSettingData.Import();

			GLOBAL.HQuickButtonSettingData = new HelperQuickButtonSettingData();
			GLOBAL.HQuickButtonSettingData.Import();

			foreach (UIProfileClock val in prof.ClockUIProfiles.Values)
			{
				Load(val);
			}

			foreach (UIProfileWatchList val in prof.WatchListUIProfiles.Values)
			{
				Load(val);
			}

			foreach (UIProfilePlotList val in prof.PlotListUIProfiles.Values)
			{
				Load(val);
			}

			foreach (UIProfileStockTicket val in prof.StockTicketUIProfiles.Values)
			{
				Load(val);
			}

			foreach (UIProfileOptionTicket val in prof.OptionTicketUIProfiles.Values)
			{
				Load(val);
			}

			foreach (UIProfileFutureTicket val in prof.FutureTicketUIProfiles.Values)
			{
				Load(val);
			}

			foreach (UIProfileQuickTicket val in prof.QuickTicketUIProfiles.Values)
			{
				Load(val);
			}

			foreach (UIProfileAutoSpreadTicket val in prof.AutoSpreadTicketUIProfiles.Values)
			{
				Load(val);
			}

			foreach (UIProfileFutureMatrixTicket val in prof.FutureMatrixTicketUIProfiles.Values)
			{
				Load(val);
			}

			foreach (UIProfileBatchTicket val in prof.BatchTicketUIProfiles.Values)
			{
				Load(val);
			}

			foreach (UIProfileBatchMarketTicket val in prof.BatchMarketTicketUIProfiles.Values)
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

			prof.Dispose();
		}

		private void Load(UIProfileClock val)
		{
			frmClock w = GLOBAL.HWindows.OpenWindow(new frmClock());

			w.Top = (int)val.top;
			w.Left = (int)val.left;
			w.Width = (int)val.width;
			w.Height = (int)val.height;

			w.OnTop = val.pinned;
			w.ClockDisplayFormat = val.clockDisplayFormat;
			w.ClockTimeZoneDisplayFormat = val.clockTimeZoneDisplayFormat;

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
			w.OnTop = val.pinned;

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
			w.OnTop = val.pinned;

			w.CurrentSymbolDetail = val.caption;

			w.ImportGrid(val.plotList);

			GLOBAL.HWindows.OpenWindow(w, false);
		}

		private void Load(UIProfileStockTicket val)
		{
			frmStockTicket w = new frmStockTicket();

			w.Top = (int)val.top;
			w.Left = (int)val.left;
			w.Width = (int)val.width;
			w.Height = (int)val.height;
			//GLOBAL.HROC.AddToStatusLogs("Load StockWindow: " + w.Name + " " + w.Height);
			w.Font = val.font;
			w.BackColor = val.backcolor;

			w.Caption = val.caption;
			w.OnTop = val.pinned;

			w.ImportBidGrid(val.level2BidList);
			w.ImportAskGrid(val.level2AskList);

			w.ImportTicket(val.stockTicket);

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
			w.OnTop = val.pinned;

			w.ImportOptionGrid(val.optionList);

			w.ImportTicket(val.optionTicket);

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
			w.OnTop = val.pinned;

			w.ImportBidGrid(val.bookBidList);
			w.ImportAskGrid(val.bookAskList);

			w.ImportTicket(val.futureTicket);

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
			w.OnTop = val.pinned;

			w.ImportQuickGrid(val.quickList);

			w.ImportTicket(val.quickTicket);

			GLOBAL.HWindows.OpenWindow(w, false);
		}

		private void Load(UIProfileAutoSpreadTicket val)
		{
			frmAutoSpreadTicket w = new frmAutoSpreadTicket();

			w.Top = (int)val.top;
			w.Left = (int)val.left;
			w.Width = (int)val.width;
			w.Height = (int)val.height;
			w.Font = val.font;
			w.BackColor = val.backcolor;

			w.Caption = val.caption;
			w.OnTop = val.pinned;

			w.ImportAutoSpreadSettingsGrid(val.autoSpreadSettingsList);
			w.ImportAutoSpreadGrid(val.autoSpreadList);
			w.ImportTicket(val.autoSpreadTicket);
			
			GLOBAL.HWindows.OpenWindow(w, false);
		}

		private void Load(UIProfileFutureMatrixTicket val)
		{
			frmFutureMatrixTicket w = new frmFutureMatrixTicket();

			w.Top = (int)val.top;
			w.Left = (int)val.left;
			w.Width = (int)val.width;
			w.Height = (int)val.height;
			w.Font = val.font;
			w.BackColor = val.backcolor;

			w.Caption = val.caption;
			w.OnTop = val.pinned;

			w.MatrixRange = val.matrixRange;
			w.MatrixInterval = val.matrixInterval;

			w.ImportTicket(val.futureMatrixTicket);

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
			w.OnTop = val.pinned;

			w.ImportGrid(val.batchList);

			GLOBAL.HWindows.OpenWindow(w, false);
		}

		private void Load(UIProfileBatchMarketTicket val)
		{
			frmBatchMarketTicket w = new frmBatchMarketTicket();

			w.Top = (int)val.top;
			w.Left = (int)val.left;
			w.Width = (int)val.width;
			w.Height = (int)val.height;
			w.Font = val.font;
			w.BackColor = val.backcolor;

			w.Caption = val.caption;
			w.OnTop = val.pinned;

			w.ImportGrid(val.batchMarketList);

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
			w.OnTop = val.pinned;

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
			w.OnTop = val.pinned;

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
			w.OnTop = val.pinned;

			w.GroupBy = (int)val.positionsListGroupBy;
			w.ImportGrid(val.positionsList);

			w.ShowOpenQty = val.positionsShowOpenQty;
			w.ShowBuyQty = val.positionsShowBuyQty;
			w.ShowSellQty = val.positionsShowSellQty;

			w.ShowOpenPnL = val.positionsShowOpenPnL;
			w.ShowDayPnL = val.positionsShowDayPnL;
			w.ShowRealizedPnL = val.positionsShowRealizedPnL;
			w.ShowTotalPnL = val.positionsShowTotalPnL;
			w.ShowCurrentQty = val.positionsShowCurrentQty;
			w.ShowAllAggregation = val.positionsShowAllAggregation;

			GLOBAL.HWindows.OpenWindow(w, false);
		}

		#endregion

		#region - Unload All Windows -

		public void UnLoadAll()
		{
			IntPtr[] keys = new IntPtr[0];

			if (GLOBAL.HWindows.WatchListWindows.Count > 0)
			{
				lock (GLOBAL.HWindows.WatchListWindows)
				{
					keys = new IntPtr[GLOBAL.HWindows.WatchListWindows.Count];
					GLOBAL.HWindows.WatchListWindows.Keys.CopyTo(keys, 0);

					foreach (IntPtr key in keys)
					{
						if (GLOBAL.HWindows.WatchListWindows.ContainsKey(key))
						{
							GLOBAL.HWindows.WatchListWindows[key].Close();
						}
					}
				}
			}

			if (GLOBAL.HWindows.PlotListWindows.Count > 0)
			{
				lock (GLOBAL.HWindows.PlotListWindows)
				{
					keys = new IntPtr[GLOBAL.HWindows.PlotListWindows.Count];
					GLOBAL.HWindows.PlotListWindows.Keys.CopyTo(keys, 0);

					foreach (IntPtr key in keys)
					{
						if (GLOBAL.HWindows.PlotListWindows.ContainsKey(key))
						{
							GLOBAL.HWindows.PlotListWindows[key].Close();
						}
					}
				}
			}

			if (GLOBAL.HWindows.StockTicketWindows.Count > 0)
			{
				lock (GLOBAL.HWindows.StockTicketWindows)
				{
					keys = new IntPtr[GLOBAL.HWindows.StockTicketWindows.Count];
					GLOBAL.HWindows.StockTicketWindows.Keys.CopyTo(keys, 0);

					foreach (IntPtr key in keys)
					{
						if (GLOBAL.HWindows.StockTicketWindows.ContainsKey(key))
						{
							GLOBAL.HWindows.StockTicketWindows[key].Close();
						}
					}
				}
			}

			if (GLOBAL.HWindows.OptionTicketWindows.Count > 0)
			{
				lock (GLOBAL.HWindows.OptionTicketWindows)
				{
					keys = new IntPtr[GLOBAL.HWindows.OptionTicketWindows.Count];
					GLOBAL.HWindows.OptionTicketWindows.Keys.CopyTo(keys, 0);

					foreach (IntPtr key in keys)
					{
						if (GLOBAL.HWindows.OptionTicketWindows.ContainsKey(key))
						{
							GLOBAL.HWindows.OptionTicketWindows[key].Close();
						}
					}
				}
			}

			if (GLOBAL.HWindows.FutureTicketWindows.Count > 0)
			{
				lock (GLOBAL.HWindows.FutureTicketWindows)
				{
					keys = new IntPtr[GLOBAL.HWindows.FutureTicketWindows.Count];
					GLOBAL.HWindows.FutureTicketWindows.Keys.CopyTo(keys, 0);

					foreach (IntPtr key in keys)
					{
						if (GLOBAL.HWindows.FutureTicketWindows.ContainsKey(key))
						{
							GLOBAL.HWindows.FutureTicketWindows[key].Close();
						}
					}
				}
			}

			if (GLOBAL.HWindows.QuickTicketWindows.Count > 0)
			{
				lock (GLOBAL.HWindows.QuickTicketWindows)
				{
					keys = new IntPtr[GLOBAL.HWindows.QuickTicketWindows.Count];
					GLOBAL.HWindows.QuickTicketWindows.Keys.CopyTo(keys, 0);

					foreach (IntPtr key in keys)
					{
						if (GLOBAL.HWindows.QuickTicketWindows.ContainsKey(key))
						{
							GLOBAL.HWindows.QuickTicketWindows[key].Close();
						}
					}
				}
			}

			if (GLOBAL.HWindows.BatchTicketWindows.Count > 0)
			{
				lock (GLOBAL.HWindows.BatchTicketWindows)
				{
					keys = new IntPtr[GLOBAL.HWindows.BatchTicketWindows.Count];
					GLOBAL.HWindows.BatchTicketWindows.Keys.CopyTo(keys, 0);

					foreach (IntPtr key in keys)
					{
						if (GLOBAL.HWindows.BatchTicketWindows.ContainsKey(key))
						{
							GLOBAL.HWindows.BatchTicketWindows[key].Close();
						}
					}
				}
			}

			if (GLOBAL.HWindows.BatchMarketTicketWindows.Count > 0)
			{
				lock (GLOBAL.HWindows.BatchMarketTicketWindows)
				{
					keys = new IntPtr[GLOBAL.HWindows.BatchMarketTicketWindows.Count];
					GLOBAL.HWindows.BatchMarketTicketWindows.Keys.CopyTo(keys, 0);

					foreach (IntPtr key in keys)
					{
						if (GLOBAL.HWindows.BatchMarketTicketWindows.ContainsKey(key))
						{
							GLOBAL.HWindows.BatchMarketTicketWindows[key].Close();
						}
					}
				}
			}

			if (GLOBAL.HWindows.AutoSpreadTicketWindows.Count > 0)
			{
				lock (GLOBAL.HWindows.AutoSpreadTicketWindows)
				{
					keys = new IntPtr[GLOBAL.HWindows.AutoSpreadTicketWindows.Count];
					GLOBAL.HWindows.AutoSpreadTicketWindows.Keys.CopyTo(keys, 0);

					foreach (IntPtr key in keys)
					{
						if (GLOBAL.HWindows.AutoSpreadTicketWindows.ContainsKey(key))
						{
							GLOBAL.HWindows.AutoSpreadTicketWindows[key].Close();
						}
					}
				}
			}

			if (GLOBAL.HWindows.FutureMatrixTicketWindows.Count > 0)
			{
				lock (GLOBAL.HWindows.FutureMatrixTicketWindows)
				{
					keys = new IntPtr[GLOBAL.HWindows.FutureMatrixTicketWindows.Count];
					GLOBAL.HWindows.FutureMatrixTicketWindows.Keys.CopyTo(keys, 0);

					foreach (IntPtr key in keys)
					{
						if (GLOBAL.HWindows.FutureMatrixTicketWindows.ContainsKey(key))
						{
							GLOBAL.HWindows.FutureMatrixTicketWindows[key].Close();
						}
					}
				}
			}

			if (GLOBAL.HWindows.OrderWindows.Count > 0)
			{
				lock (GLOBAL.HWindows.OrderWindows)
				{
					keys = new IntPtr[GLOBAL.HWindows.OrderWindows.Count];
					GLOBAL.HWindows.OrderWindows.Keys.CopyTo(keys, 0);

					foreach (IntPtr key in keys)
					{
						if (GLOBAL.HWindows.OrderWindows.ContainsKey(key))
						{
							GLOBAL.HWindows.OrderWindows[key].Close();
						}
					}
				}
			}

			if (GLOBAL.HWindows.TradeWindows.Count > 0)
			{
				lock (GLOBAL.HWindows.TradeWindows)
				{
					keys = new IntPtr[GLOBAL.HWindows.TradeWindows.Count];
					GLOBAL.HWindows.TradeWindows.Keys.CopyTo(keys, 0);

					foreach (IntPtr key in keys)
					{
						if (GLOBAL.HWindows.TradeWindows.ContainsKey(key))
						{
							GLOBAL.HWindows.TradeWindows[key].Close();
						}
					}
				}
			}

			if (GLOBAL.HWindows.PositionWindows.Count > 0)
			{
				lock (GLOBAL.HWindows.PositionWindows)
				{
					keys = new IntPtr[GLOBAL.HWindows.PositionWindows.Count];
					GLOBAL.HWindows.PositionWindows.Keys.CopyTo(keys, 0);

					foreach (IntPtr key in keys)
					{
						if (GLOBAL.HWindows.PositionWindows.ContainsKey(key))
						{
							GLOBAL.HWindows.PositionWindows[key].Close();
						}
					}
				}
			}

			Dispose();
		}

		#endregion
	}
}
