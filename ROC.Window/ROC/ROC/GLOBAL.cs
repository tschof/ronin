using System;
using System.Diagnostics;
using System.Windows.Forms;
using System.Collections.Generic;
using System.Linq;

using Common;
using RDSEx;

namespace ROC
{
	internal enum TicketTypes
	{
		None,
		Stock,
		Future,
		Quick,
		Option,
		AutoSpread,
		FutureMatrix,
	}

	internal sealed class AutomationType
	{
		internal const string AutoCancelStock = "AUTO_CANCEL_STOCK";
		internal const string AutoCancelFuture = "AUTO_CANCEL_FUTURE";
		internal const string AutoCancelOption = "AUTO_CANCEL_OPTION";
		internal const string AutoStop = "AUTO_STOP";
	}

	internal sealed class TicketDefaults
	{
		internal const string Stock = "STK_DEF";
		internal const string Future = "FUT_DEF";
		internal const string Option = "OPT_DEF";
	}

	internal sealed class QuickButtonSides
	{
		internal const string Buy = "BUY";
		internal const string Sell = "SELL";
		internal const string Short = "SHORT";

		internal const string BuyOpen = "BUYOPEN";
		internal const string BuyClose = "BUYCLOSE";
		internal const string SellOpen = "SELLOPEN";
		internal const string SellClose = "SELLCLOSE";
	}

	internal sealed class QuickButtonBasePriceSource
	{
		internal const string Bid = "BID";
		internal const string Ask = "ASK";
		internal const string Last = "LAST";
		internal const string High = "HIGH";
		internal const string Low = "LOW";
	}

	internal sealed class AccountTypeToGet
	{
		internal bool CS { get; private set; } = false;
		internal bool FUT { get; private set; } = false;
		internal bool OPT { get; private set; } = false;

		internal AccountTypeToGet(bool cs, bool fut, bool opt)
		{
			CS = cs;
			FUT = fut;
			OPT = opt;
		}
	}

	internal sealed class BaseSecurityInfo
	{
		internal string MDSymbol = "";
		internal string MDSource = "";
		internal double TickSize = 0.01;
		internal double ContractSize = 1.0;
		internal string SecType = "";
		internal string Underlying = "";
		internal string LongName = "";
		internal string Expiration = "";

		internal BaseSecurityInfo()
		{
			_ssfChain = null;
			_optionChain = null;
		}

		internal decimal TickSizeDec
		{
			get
			{
				return Convert.ToDecimal(TickSize);
			}
		}

		internal int DecimalPlaces
		{
			get
			{
				string[] strs = TickSizeDec.ToString().Split(new char[] { '.' }, StringSplitOptions.None);
				if (strs.Length > 1)
				{
					return strs[1].Trim().Length;
				}
				else
				{
					return 0;
				}
			}
		}


		internal bool TryGetExpiration(out DateTime expiration)
		{
			if (string.IsNullOrEmpty(Expiration)) {
				expiration = default;
				return false;
			}
			return GLOBAL.TimeFormats.TryParse(Expiration, out expiration, "yyyyMMdd");
		}

		private Dictionary<string, IMSSFutureInfo> _ssfChain;
		internal Dictionary<string, IMSSFutureInfo> SSFChain
		{
			get
			{
				if (_ssfChain == null)
				{
					_ssfChain = new Dictionary<string, IMSSFutureInfo>();
				}
				return _ssfChain;
			}
			set
			{
				_ssfChain = value;
			}
		}

		private Dictionary<string, IMOptionInfo> _optionChain;
		internal Dictionary<string, IMOptionInfo> OptionChain
		{
			get
			{
				if (_optionChain == null)
				{
					_optionChain = new Dictionary<string, IMOptionInfo>();
				}
				return _optionChain;
			}
			set
			{
				_optionChain = value;
			}
		}
	}

	internal static class ROCOrderTypes
	{
		internal static string AutoSpread = "[AS]";
	}

	internal static class GLOBAL
	{
		internal static bool ByPassRomLogin { get; private set; } = Configuration.Override.Default.ByPassROM;
		internal static bool ByPassRdsLogin { get; private set; } = Configuration.Override.Default.ByPassRDS;
		internal static bool ByPassMdsLogin { get; private set; } = false;
		internal static bool Interrupt = false;
		internal static bool ShuttingDown = false;
		internal static bool AdminMode = false;
		internal static bool UseDelayedUpdate { get; private set; } = false;
		internal static bool ReloadProcessSettings = false;
		internal static int AvgPriceResolution => Configuration.User.Default.OrderAvgPriceResolution;
		internal static Version ROCVersion = new Version(System.Windows.Forms.Application.ProductVersion);
		internal static bool ShownDM => !Debugger.IsAttached;

		internal static class TimeFormats
		{
			private static string[] _formats = new string[] { "HH:mm:ss", "yyyyMM" };

			internal static bool TryParse(string text, out DateTime value)
			{
				return TryParse(text, out value, _formats);
			}

			internal static bool TryParse(string text, out DateTime value, params string[] formats)
			{
				return DateTime.TryParseExact(text, formats, System.Globalization.CultureInfo.CurrentCulture, System.Globalization.DateTimeStyles.None, out value);
			}
		}

		private class DateTimeSetting
		{
			private DateTime? _value;
			private readonly string _fallbackText;
			private readonly Converter<Configuration.User, string> _getter;
			private readonly Action<Configuration.User, string> _setter;

			internal DateTimeSetting(string fallbackText, Converter<Configuration.User, string> getter, Action<Configuration.User, string> setter)
			{
				_fallbackText = fallbackText;
				_getter = getter;
				_setter = setter;
			}

			internal DateTime GetValue()
			{
				if (!_value.HasValue) {
					DateTime when;
					if (GLOBAL.TimeFormats.TryParse(_getter(Configuration.User.Default), out when)) {
						_value = when;
					} else if (GLOBAL.TimeFormats.TryParse(_fallbackText, out when)) {
						_value = when;
						_setter(Configuration.User.Default, _fallbackText);
						Configuration.User.Default.Save();
					} else {
						_value = default;
					}
				}
				return _value.Value;
			}

			internal void SetValue(DateTime value)
			{
				_value = value;
			}
		}

		private static DateTimeSetting _stopTime = new DateTimeSetting("23:55:00", n => n.StopTime, (n, s) => n.StopTime = s);
		internal static DateTime StopTime => _stopTime.GetValue();

		private static DateTimeSetting _stockAutoCancelTime = new DateTimeSetting("15:00:00", n => n.StockAutoCancelTime, (n, s) => n.StockAutoCancelTime = s);
		internal static DateTime StockAutoCancelTime
		{
			get => _stockAutoCancelTime.GetValue();
			set => _stockAutoCancelTime.SetValue(value);
		}

		private static DateTimeSetting _futureAutoCancelTime = new DateTimeSetting("15:30:00", n => n.FutureAutoCancelTime, (n, s) => n.FutureAutoCancelTime = s);
		internal static DateTime FutureAutoCancelTime
		{
			get => _futureAutoCancelTime.GetValue();
			set => _futureAutoCancelTime.SetValue(value);
		}

		private static DateTimeSetting _optionAutoCancelTime = new DateTimeSetting("15:00:00", n => n.OptionAutoCancelTime, (n, s) => n.OptionAutoCancelTime = s);
		internal static DateTime OptionAutoCancelTime
		{
			get => _optionAutoCancelTime.GetValue();
			set => _optionAutoCancelTime.SetValue(value);
		}

		internal static bool ROCLoginCompleted
		{
			get
			{
				if (GLOBAL.OrderManagers.Status == HelperROM.StatusTypes.LoggedIn &&
					GLOBAL.HRDS.Status == HelperRDS.StatusTypes.Done)
				{
					// Login is completed even if MDS is not connected
					return true;
				}
				return false;
			}
		}

		#region - ROM Login -

		internal static void InitializeROM()
		{
			if (GLOBAL.OrderManagers.Status != HelperROM.StatusTypes.Started)
			{
				GLOBAL.LoginForm.StartPosition = FormStartPosition.CenterScreen;

				try
				{
					GLOBAL.LoginForm.ShowDialog();
				}
				catch (Exception ex)
				{
					GLOBAL.HROC.AddToException(ex);
				}
			}
		}

		internal static frmMain MainForm => HWindows.MainWindow;

		private static frmLogin _loginForm;
		internal static frmLogin LoginForm
		{
			get
			{
				if (_loginForm == null)
				{
					_loginForm = new frmLogin();
					_loginForm.Disposed += new EventHandler(LoginForm_Disposed);
				}
				return _loginForm;
			}
			set
			{
				_loginForm = value;
			}
		}

		private static void LoginForm_Disposed(object sender, EventArgs e)
		{
			if (_loginForm != null)
			{
				_loginForm.Disposed -= new EventHandler(LoginForm_Disposed);
			}
			_loginForm = null;
		}

		#endregion

		internal static HelperEMail HMail = new HelperEMail();

		private static List<HelperMDS> _hMDSs;
		internal static List<HelperMDS> MarketDataProviders {
			get {

				if (_hMDSs == null || _hMDSs.Count == 0) {
					_hMDSs = new List<HelperMDS>();

					char[] comma = new char[] { ',' };
					string[] serverIPs = Configuration.MDS.Default.IP.ToUpper().Split(comma, StringSplitOptions.RemoveEmptyEntries);
					string[] serverPorts = Configuration.MDS.Default.Port.ToUpper().Split(comma, StringSplitOptions.RemoveEmptyEntries);
					int serverCount = serverIPs.GetLength(0);
					int portCount = serverPorts.GetLength(0);
					int defaultPort = -1;

					if (serverCount > 0) {
						for (int i = 0; i < serverCount; ++i) {
							int port;

							if (i >= portCount) {
								port = defaultPort;
							} else if (!int.TryParse(serverPorts[i], out port)) {
								Log.Error(Log.Target.General, $"Can't add market data server {serverIPs[i]}:{serverPorts[i]}: bad port");
								port = 0;
							} else if (defaultPort < 0) {
								defaultPort = port;
							}

							if (port > 0) {
								try {
									_hMDSs.Add(new HelperMDS(serverIPs[i], port));
								} catch (Exception x) {
									Log.Error(Log.Target.General, $"Can't add market data server {serverIPs[i]}:{port}", x);
								}
							}
						}
					} else {
						Log.Error(Log.Target.General, "Can't add market data servers from empty IP or port list.");
					}
				}
				return _hMDSs;
			}
			set {
				_hMDSs = value;
			}
		}

		internal static bool MDSsConnected => !GLOBAL.MarketDataProviders.Any(n => !n.IsConnected); // true if all connected, false if any not connected

		internal static void MDSsDisconnect()
		{
			GLOBAL.MarketDataProviders.ForEach(n => n.Disconnect());
		}

		internal static void MDSsReconnect()
		{
			GLOBAL.MarketDataProviders.ForEach(n => n.Reconnect());
		}

		private static HelperROM _hROM;
		internal static HelperROM OrderManagers
		{
			get
			{
				if (_hROM == null)
					_hROM = new HelperROM();
				return _hROM;
			}
			set
			{
				_hROM = value;
			}
		}

		private static HelperRDS _hRDS;
		internal static HelperRDS HRDS
		{
			get
			{
				if (_hRDS == null)
					_hRDS = new HelperRDS();
				return _hRDS;
			}
			set
			{
				_hRDS = value;
			}
		}

		private static HelperROC _hROC;
		internal static HelperROC HROC
		{
			get
			{
				if (_hROC == null)
					_hROC = new HelperROC();
				return _hROC;
			}
			set
			{
				_hROC = value;
			}
		}

		private static HelperProcess _hProcess;
		internal static HelperProcess HProcess
		{
			get
			{
				if (_hProcess == null)
					_hProcess = new HelperProcess();
				return _hProcess;
			}
			set
			{
				_hProcess = value;
			}
		}

		private static HelperWindows _hWindows;
		internal static HelperWindows HWindows
		{
			get
			{
				if (_hWindows == null)
					_hWindows = new HelperWindows();
				return _hWindows;
			}
		}

		private static HelperOrdersData _hOrders;
		internal static HelperOrdersData HOrders
		{
			get
			{
				if (_hOrders == null)
					_hOrders = new HelperOrdersData();
				return _hOrders;
			}
			set
			{
				_hOrders = value;
			}
		}

		private static HelperExecutionsData _hExecutions;
		internal static HelperExecutionsData HExecutions
		{
			get
			{
				if (_hExecutions == null)
					_hExecutions = new HelperExecutionsData();
				return _hExecutions;
			}
			set
			{
				_hExecutions = value;
			}
		}

		private static HelperPositionsData _hPositions;
		internal static HelperPositionsData HPositions
		{
			get
			{
				if (_hPositions == null)
					_hPositions = new HelperPositionsData();
				return _hPositions;
			}
			set
			{
				_hPositions = value;
			}
		}

		private static HelperMarketData _hMarketData;
		internal static HelperMarketData HMarketData
		{
			get
			{
				if (_hMarketData == null)
					_hMarketData = new HelperMarketData();
				return _hMarketData;
			}
			set
			{
				_hMarketData = value;
			}
		}

		private static HelperSymbolSettingData _hSymbolSettingData;
		internal static HelperSymbolSettingData HSymbolSettingData
		{
			get
			{
				if (_hSymbolSettingData == null)
				{
					_hSymbolSettingData = new HelperSymbolSettingData();
					_hSymbolSettingData.CheckDefaults();
				}
				return _hSymbolSettingData;
			}
			set
			{
				_hSymbolSettingData = value;
			}
		}

		private static HelperQuickButtonSettingData _hQuickButtonSettingData;
		internal static HelperQuickButtonSettingData HQuickButtonSettingData
		{
			get
			{
				if (_hQuickButtonSettingData == null)
				{
					_hQuickButtonSettingData = new HelperQuickButtonSettingData();
					_hQuickButtonSettingData.CheckDefaults();
				}
				return _hQuickButtonSettingData;
			}
			set
			{
				_hQuickButtonSettingData = value;
			}
		}

		#region - User UI Profile -

		private static UIProfile _userUIProfile;
		internal static UIProfile UserUIProfile
		{
			get
			{
				if (_userUIProfile == null)
					_userUIProfile = new UIProfile();
				return _userUIProfile;
			}
			set
			{
				_userUIProfile = value;
			}
		}

		#endregion

		#region - Wait Window -

		private static frmWait _waitWindow;
		internal static frmWait WaitWindow
		{
			get
			{
				if (_waitWindow == null)
				{
					_waitWindow = new frmWait();
					_waitWindow.Disposed += new EventHandler(WaitWindow_Disposed);
				}
				return _waitWindow;
			}
			set
			{
				_waitWindow = value;
			}
		}

		private static void WaitWindow_Disposed(object sender, EventArgs e)
		{
			if (_waitWindow != null)
			{
				_waitWindow.Disposed -= new EventHandler(WaitWindow_Disposed);
			}
			_waitWindow = null;
		}

		#endregion

#region - Option To Exchange Maps -

		private static Dictionary<string, List<string>> _optionToExchangeMaps;
		internal static Dictionary<string, List<string>> OptionToExchangeMaps
		{
			get
			{
				if (_optionToExchangeMaps == null)
				{
					_optionToExchangeMaps = new Dictionary<string, List<string>>() {
						{ "SPX", new List<string>() }
					};
				}
				return _optionToExchangeMaps;
			}
		}

#endregion

#region - Sound -

		private static HelperSound _rocSounds;
		internal static HelperSound ROCSounds
		{
			get
			{
				if (_rocSounds == null)
					_rocSounds = new HelperSound();
				return _rocSounds;
			}
		}

#endregion

	}
}
