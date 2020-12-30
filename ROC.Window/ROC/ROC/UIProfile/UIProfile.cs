using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Runtime.Serialization;
using System.Drawing;

using Common;
using SerializationEx;

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
				_changed = _changed || value;
			}
		}

		public Color PositiveColor = Color.LimeGreen;
		public Color NegativeColor = Color.Red;
		public Color NormalColor = Color.White;

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
						System.Windows.Forms.Application.DoEvents();
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
					fileName = GLOBAL.HROM.UserName + ".profile";
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
				HelperFile.Save(data, filePath, fileName);
			}
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

			GLOBAL.HWindows.MainWindows.WithValues(Save);
			GLOBAL.HWindows.ClockWindows.WithValues(Save);
			GLOBAL.HWindows.WatchListWindows.WithValues(Save);
			GLOBAL.HWindows.PlotListWindows.WithValues(Save);
			GLOBAL.HWindows.StockTicketWindows.WithValues(Save);
			GLOBAL.HWindows.OptionTicketWindows.WithValues(Save);
			GLOBAL.HWindows.FutureTicketWindows.WithValues(Save);
			GLOBAL.HWindows.QuickTicketWindows.WithValues(Save);
			GLOBAL.HWindows.AutoSpreadTicketWindows.WithValues(Save);
			GLOBAL.HWindows.FutureMatrixTicketWindows.WithValues(Save);
			GLOBAL.HWindows.BatchTicketWindows.WithValues(Save);
			GLOBAL.HWindows.BatchMarketTicketWindows.WithValues(Save);
			GLOBAL.HWindows.OrderWindows.WithValues(Save);
			GLOBAL.HWindows.TradeWindows.WithValues(Save);
			GLOBAL.HWindows.PositionWindows.WithValues(Save);
		}

		private T setProfileFromWindow<T>(Dictionary<IntPtr, T> collection, FormEx.VistaWindowBase source) where T : DictionaryEx.MultiTypedDictionary, new()
		{
			IntPtr key = source.Handle;
			if (!collection.TryGetValue(key, out var profile)) {
				profile = new T();
				collection.Add(key, profile);
				changed = true;
			}

			changed = profile.Set(FormUIProfileFieldID.Top, source.Top);
			changed = profile.Set(FormUIProfileFieldID.Left, source.Left);
			changed = profile.Set(FormUIProfileFieldID.Width, source.Width);
			changed = profile.Set(FormUIProfileFieldID.Height, source.Height);
			changed = profile.Set(FormUIProfileFieldID.Font, source.Font);
			changed = profile.Set(FormUIProfileFieldID.BackColor, source.BackColor);
			changed = profile.Set(FormUIProfileFieldID.Caption, source.Caption);
			changed = profile.Set(FormUIProfileFieldID.Pinned, source.TopMost);
			return profile;
		}

		public void Save(frmMain w)
		{
			var profile = setProfileFromWindow(MainUIProfiles, w);
			changed = profile.Set(FormUIProfileFieldID.InSystemTray, w.InSystemTray);
		}

		public void Save(frmClock w)
		{
			var profile = setProfileFromWindow(ClockUIProfiles, w);
			changed = profile.Set(FormUIProfileFieldID.ClockDisplayFormat, w.ClockDisplayFormat);
			changed = profile.Set(FormUIProfileFieldID.ClockTimeZoneDisplayFormat, w.ClockTimeZoneDisplayFormat);
		}

		private void Save(frmWatchList w)
		{
			var profile = setProfileFromWindow(WatchListUIProfiles, w);
			changed = profile.Set(FormUIProfileFieldID.WatchList, w.ExportGrid());
		}

		private void Save(frmPlotList w)
		{
			var profile = setProfileFromWindow(PlotListUIProfiles, w);
			changed = profile.Set(FormUIProfileFieldID.PlotList, w.ExportGrid());
		}

		private void Save(frmStockTicket w)
		{
			var profile = setProfileFromWindow(StockTicketUIProfiles, w);
			changed = profile.Set(FormUIProfileFieldID.Level2BidList, w.ExportBidGrid());
			changed = profile.Set(FormUIProfileFieldID.Level2AskList, w.ExportAskGrid());
			changed = profile.Set(FormUIProfileFieldID.StockTicket, w.ExportTicket());
		}

		private void Save(frmOptionTicket w)
		{
			var profile = setProfileFromWindow(OptionTicketUIProfiles, w);
			changed = profile.Set(FormUIProfileFieldID.OptionList, w.ExportOptionGrid());
			changed = profile.Set(FormUIProfileFieldID.OptionTicket, w.ExportTicket());
		}

		private void Save(frmFutureTicket w)
		{
			var profile = setProfileFromWindow(FutureTicketUIProfiles, w);
			changed = profile.Set(FormUIProfileFieldID.BookBidList, w.ExportBidGrid());
			changed = profile.Set(FormUIProfileFieldID.BookAskList, w.ExportAskGrid());
			changed = profile.Set(FormUIProfileFieldID.FutureTicket, w.ExportTicket());
		}

		private void Save(frmQuickTicket w)
		{
			var profile = setProfileFromWindow(QuickTicketUIProfiles, w);
			changed = profile.Set(FormUIProfileFieldID.QuickList, w.ExportQuickGrid());
			changed = profile.Set(FormUIProfileFieldID.QuickTicket, w.ExportTicket());
		}

		private void Save(frmAutoSpreadTicket w)
		{
			var profile = setProfileFromWindow(AutoSpreadTicketUIProfiles, w);
			changed = profile.Set(FormUIProfileFieldID.AutoSpreadSettingsList, w.ExportAutoSpreadSettingsGrid());
			changed = profile.Set(FormUIProfileFieldID.AutoSpreadList, w.ExportAutoSpreadGrid());
			changed = profile.Set(FormUIProfileFieldID.AutoSpreadTicket, w.ExportTicket());
		}

		private void Save(frmFutureMatrixTicket w)
		{
			var profile = setProfileFromWindow(FutureMatrixTicketUIProfiles, w);
			changed = profile.Set(FormUIProfileFieldID.FutureMatrixRange, w.MatrixRange);
			changed = profile.Set(FormUIProfileFieldID.FutureMatrixInterval, w.MatrixInterval);
			changed = profile.Set(FormUIProfileFieldID.FutureMatrixTicket, w.ExportTicket());
		}

		private void Save(frmBatchTicket w)
		{
			var profile = setProfileFromWindow(BatchTicketUIProfiles, w);
			changed = profile.Set(FormUIProfileFieldID.BatchList, w.ExportGrid());
		}

		private void Save(frmBatchMarketTicket w)
		{
			var profile = setProfileFromWindow(BatchMarketTicketUIProfiles, w);
			changed = profile.Set(FormUIProfileFieldID.BatchMarketList, w.ExportGrid());
		}

		private void Save(frmOrders w)
		{
			var profile = setProfileFromWindow(OrdersUIProfiles, w);
			changed = profile.Set(FormUIProfileFieldID.OrdersList, w.ExportGrid());
		}

		private void Save(frmTrades w)
		{
			var profile = setProfileFromWindow(TradesUIProfiles, w);
			changed = profile.Set(FormUIProfileFieldID.TradesList, w.ExportGrid());
		}

		private void Save(frmPositions w)
		{
			var profile = setProfileFromWindow(PositionsUIProfiles, w);
			changed = profile.Set(FormUIProfileFieldID.PositionsList, w.ExportGrid());
			changed = profile.Set(FormUIProfileFieldID.PositionsListGroupBy, w.GroupBy);
			changed = profile.Set(FormUIProfileFieldID.PositionsShowOpenQty, w.ShowOpenQty);
			changed = profile.Set(FormUIProfileFieldID.PositionsShowBuyQty, w.ShowBuyQty);
			changed = profile.Set(FormUIProfileFieldID.PositionsShowSellQty, w.ShowSellQty);
			changed = profile.Set(FormUIProfileFieldID.PositionsShowOpenPnL, w.ShowOpenPnL);
			changed = profile.Set(FormUIProfileFieldID.PositionsShowDayPnL, w.ShowDayPnL);
			changed = profile.Set(FormUIProfileFieldID.PositionsShowRealizedPnL, w.ShowRealizedPnL);
			changed = profile.Set(FormUIProfileFieldID.PositionsShowTotalPnL, w.ShowTotalPnL);
			changed = profile.Set(FormUIProfileFieldID.PositionsShowCurrentQty, w.ShowCurrentQty);
			changed = profile.Set(FormUIProfileFieldID.PositionsShowAllAggregation, w.ShowAllAggregation);
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
					prof.MainUIProfiles.WithValues(Load);
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
					result = HelperFile.Load(filePath, fileName);

					if (result == "")
					{
						result = HelperFile.Load(filePath, fileName);
					}
				}
				else
				{
					fileName = GLOBAL.HROM.UserName + ".profile";
					result = HelperFile.Load(filePath, fileName);

					if (result == "")
					{
						result = HelperFile.Load(filePath, fileName);
					}
				}
			}
			else
			{
				fileName = "Default.profile";
				result = HelperFile.Load(filePath, fileName);
				
				if (result == "")
				{
					result = HelperFile.Load(filePath, fileName);
				}
			}

			return result;
		}

		private void setWindowFromProfile(UIProfileBase profile, FormEx.VistaWindowBase window, bool wantBackColor = false, bool wantCaption = false, bool wantPinned = false)
		{
			long lval;
			if (profile.TryGet(FormUIProfileFieldID.Top, out lval))
				window.Top = (int)lval;
			if (profile.TryGet(FormUIProfileFieldID.Left, out lval))
				window.Left = (int)lval;
			if (profile.TryGet(FormUIProfileFieldID.Width, out lval))
				window.Width = (int)lval;
			if (profile.TryGet(FormUIProfileFieldID.Height, out lval))
				window.Height = (int)lval;
			if (profile.TryGet(FormUIProfileFieldID.Font, out Font font))
				window.Font = font;
			if (wantBackColor && profile.TryGet(FormUIProfileFieldID.BackColor, out Color color))
				window.BackColor = color;
			if (wantCaption && profile.TryGet(FormUIProfileFieldID.Caption, out string caption))
				window.Caption = caption;
			if (wantPinned && profile.TryGet(FormUIProfileFieldID.Pinned, out bool pinned))
				window.OnTop = pinned;
		}

		private void Load(UIProfileMain val)
		{
			frmMain w = GLOBAL.HWindows.OpenWindow(new frmMain());
			setWindowFromProfile(val, w);

			if (val.TryGet(FormUIProfileFieldID.InSystemTray, out bool bval))
				w.InSystemTray = bval;

			w.BackColor = Color.FromKnownColor(KnownColor.Control);
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
					prof.MainUIProfiles.WithValues(n => setWindowFromProfile(n, GLOBAL.MainForm, true));
					UnLoadAll();

					GLOBAL.HRDS.ResetSecurityInfo();

					LoadAll(prof);
				}

				prof.Dispose();
			}

			data = "";
		}

		// Only reload the user interface
		public void ReloadUI()
		{
			GLOBAL.HWindows.StockTicketWindows.WithValues(n => n.ReloadTicket());
			GLOBAL.HWindows.FutureTicketWindows.WithValues(n => n.ReloadTicket());
			GLOBAL.HWindows.OptionTicketWindows.WithValues(n => n.ReloadTicket());
			GLOBAL.HWindows.QuickTicketWindows.WithValues(n => n.ReloadTicket());
		}

		#endregion

		#region - Load All Windows -

		private void LoadAll(UIProfile prof)
		{
			GLOBAL.HSymbolSettingData = new HelperSymbolSettingData();
			GLOBAL.HSymbolSettingData.Import();

			GLOBAL.HQuickButtonSettingData = new HelperQuickButtonSettingData();
			GLOBAL.HQuickButtonSettingData.Import();

			prof.ClockUIProfiles.WithValues(Load);
			prof.WatchListUIProfiles.WithValues(Load);
			prof.PlotListUIProfiles.WithValues(Load);
			prof.StockTicketUIProfiles.WithValues(Load);
			prof.OptionTicketUIProfiles.WithValues(Load);
			prof.FutureTicketUIProfiles.WithValues(Load);
			prof.QuickTicketUIProfiles.WithValues(Load);
			prof.AutoSpreadTicketUIProfiles.WithValues(Load);
			prof.FutureMatrixTicketUIProfiles.WithValues(Load);
			prof.BatchTicketUIProfiles.WithValues(Load);
			prof.BatchMarketTicketUIProfiles.WithValues(Load);
			prof.OrdersUIProfiles.WithValues(Load);
			prof.TradesUIProfiles.WithValues(Load);
			prof.PositionsUIProfiles.WithValues(Load);

			prof.Dispose();
		}

		private void Load(UIProfileClock val)
		{
			frmClock w = GLOBAL.HWindows.OpenWindow(new frmClock());

			string sval;
			setWindowFromProfile(val, w, false, false, true);
			if (val.TryGet(FormUIProfileFieldID.ClockDisplayFormat, out sval))
				w.ClockDisplayFormat = sval;
			if (val.TryGet(FormUIProfileFieldID.ClockTimeZoneDisplayFormat, out sval))
				w.ClockTimeZoneDisplayFormat = sval;

			GLOBAL.HWindows.OpenWindow(w);
		}

		private void Load(UIProfileWatchList val)
		{
			frmWatchList w = new frmWatchList();

			setWindowFromProfile(val, w, true, true, true);
			if (val.TryGet(FormUIProfileFieldID.WatchList, out string sval))
				w.ImportGrid(sval);

			GLOBAL.HWindows.OpenWindow(w, false);
		}

		private void Load(UIProfilePlotList val)
		{
			frmPlotList w = new frmPlotList();

			string sval;
			setWindowFromProfile(val, w, true, true, true);
			if (val.TryGet(FormUIProfileFieldID.Caption, out sval))
				w.CurrentSymbolDetail = sval;
			if (val.TryGet(FormUIProfileFieldID.PlotList, out sval))
				w.ImportGrid(sval);

			GLOBAL.HWindows.OpenWindow(w, false);
		}

		private void Load(UIProfileStockTicket val)
		{
			frmStockTicket w = new frmStockTicket();

			string sval;
			setWindowFromProfile(val, w, true, true, true);
			if (val.TryGet(FormUIProfileFieldID.Level2BidList, out sval))
				w.ImportBidGrid(sval);
			if (val.TryGet(FormUIProfileFieldID.Level2AskList, out sval))
				w.ImportAskGrid(sval);
			if (val.TryGet(FormUIProfileFieldID.StockTicket, out sval))
				w.ImportTicket(sval);

			GLOBAL.HWindows.OpenWindow(w, false);
		}

		private void Load(UIProfileOptionTicket val)
		{
			frmOptionTicket w = new frmOptionTicket();

			string sval;
			setWindowFromProfile(val, w, true, true, true);
			if (val.TryGet(FormUIProfileFieldID.OptionList, out sval))
				w.ImportOptionGrid(sval);
			if (val.TryGet(FormUIProfileFieldID.OptionTicket, out sval))
				w.ImportTicket(sval);

			GLOBAL.HWindows.OpenWindow(w, false);
		}

		private void Load(UIProfileFutureTicket val)
		{
			frmFutureTicket w = new frmFutureTicket();

			string sval;
			setWindowFromProfile(val, w, true, true, true);
			if (val.TryGet(FormUIProfileFieldID.BookBidList, out sval))
				w.ImportBidGrid(sval);
			if (val.TryGet(FormUIProfileFieldID.BookAskList, out sval))
				w.ImportAskGrid(sval);
			if (val.TryGet(FormUIProfileFieldID.FutureTicket, out sval))
				w.ImportTicket(sval);

			GLOBAL.HWindows.OpenWindow(w, false);
		}

		private void Load(UIProfileQuickTicket val)
		{
			frmQuickTicket w = new frmQuickTicket();

			string sval;
			setWindowFromProfile(val, w, true, true, true);
			if (val.TryGet(FormUIProfileFieldID.QuickList, out sval))
				w.ImportQuickGrid(sval);
			if (val.TryGet(FormUIProfileFieldID.QuickTicket, out sval))
				w.ImportTicket(sval);

			GLOBAL.HWindows.OpenWindow(w, false);
		}

		private void Load(UIProfileAutoSpreadTicket val)
		{
			frmAutoSpreadTicket w = new frmAutoSpreadTicket();

			string sval;
			setWindowFromProfile(val, w, true, true, true);
			if (val.TryGet(FormUIProfileFieldID.AutoSpreadSettingsList, out sval))
				w.ImportAutoSpreadSettingsGrid(sval);
			if (val.TryGet(FormUIProfileFieldID.AutoSpreadList, out sval))
				w.ImportAutoSpreadGrid(sval);
			if (val.TryGet(FormUIProfileFieldID.AutoSpreadTicket, out sval))
				w.ImportTicket(sval);

			GLOBAL.HWindows.OpenWindow(w, false);
		}

		private void Load(UIProfileFutureMatrixTicket val)
		{
			frmFutureMatrixTicket w = new frmFutureMatrixTicket();

			long lval;
			setWindowFromProfile(val, w, true, true, true);
			if (val.TryGet(FormUIProfileFieldID.FutureMatrixRange, out lval))
				w.MatrixRange = (int)lval;
			if (val.TryGet(FormUIProfileFieldID.FutureMatrixInterval, out lval))
				w.MatrixInterval = (int)lval;
			if (val.TryGet(FormUIProfileFieldID.FutureMatrixTicket, out string sval))
				w.ImportTicket(sval);

			GLOBAL.HWindows.OpenWindow(w, false);
		}

		private void Load(UIProfileBatchTicket val)
		{
			frmBatchTicket w = new frmBatchTicket();

			setWindowFromProfile(val, w, true, true, true);
			if (val.TryGet(FormUIProfileFieldID.BatchList, out string sval))
				w.ImportGrid(sval);

			GLOBAL.HWindows.OpenWindow(w, false);
		}

		private void Load(UIProfileBatchMarketTicket val)
		{
			frmBatchMarketTicket w = new frmBatchMarketTicket();

			setWindowFromProfile(val, w, true, true, true);
			if (val.TryGet(FormUIProfileFieldID.BatchMarketList, out string sval))
				w.ImportGrid(sval);

			GLOBAL.HWindows.OpenWindow(w, false);
		}

		private void Load(UIProfileOrders val)
		{
			frmOrders w = new frmOrders();

			setWindowFromProfile(val, w, true, true, true);
			if (val.TryGet(FormUIProfileFieldID.OrdersList, out string sval))
				w.ImportGrid(sval);

			GLOBAL.HWindows.OpenWindow(w, false);
		}

		private void Load(UIProfileTrades val)
		{
			frmTrades w = new frmTrades();

			setWindowFromProfile(val, w, true, true, true);
			if (val.TryGet(FormUIProfileFieldID.TradesList, out string sval))
				w.ImportGrid(sval);

			GLOBAL.HWindows.OpenWindow(w, false);
		}

		private void Load(UIProfilePositions val)
		{
			frmPositions w = new frmPositions();

			setWindowFromProfile(val, w, true, true, true);

			long lval;
			string sval;
			bool bval;

			if (val.TryGet(FormUIProfileFieldID.PositionsListGroupBy, out lval))
				w.GroupBy = (int)lval;
			if (val.TryGet(FormUIProfileFieldID.PositionsList, out sval))
				w.ImportGrid(sval);
			if (val.TryGet(FormUIProfileFieldID.PositionsShowOpenQty, out bval))
				w.ShowOpenQty = bval;
			if (val.TryGet(FormUIProfileFieldID.PositionsShowBuyQty, out bval))
				w.ShowBuyQty = bval;
			if (val.TryGet(FormUIProfileFieldID.PositionsShowSellQty, out bval))
				w.ShowSellQty = bval;
			if (val.TryGet(FormUIProfileFieldID.PositionsShowSellQty, out bval))
				w.ShowOpenPnL = bval;
			if (val.TryGet(FormUIProfileFieldID.PositionsShowDayPnL, out bval))
				w.ShowDayPnL = bval;
			if (val.TryGet(FormUIProfileFieldID.PositionsShowRealizedPnL, out bval))
				w.ShowRealizedPnL = bval;
			if (val.TryGet(FormUIProfileFieldID.PositionsShowTotalPnL, out bval))
				w.ShowTotalPnL = bval;
			if (val.TryGet(FormUIProfileFieldID.PositionsShowCurrentQty, out bval))
				w.ShowCurrentQty = bval;
			if (val.TryGet(FormUIProfileFieldID.PositionsShowAllAggregation, out bval))
				w.ShowAllAggregation = bval;

			GLOBAL.HWindows.OpenWindow(w, false);
		}

		#endregion

		#region - Unload All Windows -

		public void UnLoadAll()
		{
			foreach ((IntPtr _, var form) in GLOBAL.HWindows.WatchListWindows)
				form.Close();

			foreach ((IntPtr _, var form) in GLOBAL.HWindows.PlotListWindows)
				form.Close();

			foreach ((IntPtr _, var form) in GLOBAL.HWindows.StockTicketWindows)
				form.Close();

			foreach ((IntPtr _, var form) in GLOBAL.HWindows.OptionTicketWindows)
				form.Close();

			foreach ((IntPtr _, var form) in GLOBAL.HWindows.FutureTicketWindows)
				form.Close();

			foreach ((IntPtr _, var form) in GLOBAL.HWindows.QuickTicketWindows)
				form.Close();

			foreach ((IntPtr _, var form) in GLOBAL.HWindows.BatchTicketWindows)
				form.Close();

			foreach ((IntPtr _, var form) in GLOBAL.HWindows.BatchMarketTicketWindows)
				form.Close();

			foreach ((IntPtr _, var form) in GLOBAL.HWindows.AutoSpreadTicketWindows)
				form.Close();

			foreach ((IntPtr _, var form) in GLOBAL.HWindows.FutureMatrixTicketWindows)
				form.Close();

			foreach ((IntPtr _, var form) in GLOBAL.HWindows.OrderWindows)
				form.Close();

			foreach ((IntPtr _, var form) in GLOBAL.HWindows.TradeWindows)
				form.Close();

			foreach ((IntPtr _, var form) in GLOBAL.HWindows.PositionWindows)
				form.Close();

			Dispose();
		}

		#endregion
	}
}
