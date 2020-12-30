using System;
using System.Diagnostics;
using System.Windows.Forms;
using BenchMarkEx;
using System.Collections.Generic;
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
		private bool _cs = false;
		internal bool CS
		{
			get
			{
				return _cs;
			}
		}
		
		private bool _fut = false;
		internal bool FUT
		{
			get
			{
				return _fut;
			}
		}

		private bool _opt = false;
		internal bool OPT
		{
			get
			{
				return _opt;
			}
		}

		internal AccountTypeToGet(bool cs, bool fut, bool opt)
		{
			_cs = cs;
			_fut = fut;
			_opt = opt;
		}
	}

	internal sealed class BaseSecurityInfo
	{
		internal BaseSecurityInfo()
		{
			_mdSymbol = "";
			_tickSize = 0.01;
			_contractSize = 1;
			_secType = "";
			_longName = "";
			_expiration = "";

			_ssfChain = null;
			_optionChain = null;
		}

		private string _mdSymbol = "";
		internal string MDSymbol
		{
			get
			{
				return _mdSymbol;
			}
			set
			{
				_mdSymbol = value;
			}
		}

		private string _mdSource = "";
		internal string MDSource
		{
			get
			{
				return _mdSource;
			}
			set
			{
				_mdSource = value;
			}
		}

		private double _tickSize = 0.01;
		internal double TickSize
		{
			get
			{
				return _tickSize;
			}
			set
			{
				_tickSize = value;
			}
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

		private double _contractSize = 1;
		internal double ContractSize
		{
			get
			{
				return _contractSize;
			}
			set
			{
				_contractSize = value;
			}
		}

		private string _secType = "";
		internal string SecType
		{
			get
			{
				return _secType;
			}
			set
			{
				_secType = value;
			}
		}

		private string _underlying = "";
		internal string Underlying
		{
			get
			{
				return _underlying;
			}
			set
			{
				_underlying = value;
			}
		}

		private string _longName = "";
		internal string LongName
		{
			get
			{
				return _longName;
			}
			set
			{
				_longName = value;
			}
		}

		private string _expiration = "";
		internal string Expiration
		{
			get
			{
				return _expiration;
			}
			set
			{
				_expiration = value;
			}
		}

		internal Nullable<DateTime> ExpirationDT
		{
			get
			{
				if (Expiration != "")
				{
					DateTime tDT;
					if (DateTime.TryParseExact(Expiration, "yyyyMMdd", System.Globalization.CultureInfo.CurrentCulture, System.Globalization.DateTimeStyles.None, out tDT))
					{
						return tDT;
					}
				}
				return null;
			}
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

		internal static bool ShownDM
		{
			get
			{
				return !Debugger.IsAttached;
			}
		}
		
		internal static Version ROCVersion = new Version(Application.ProductVersion);

		private static string[] _timeFormats = new string[0];
		internal static string[] TimeFormats
		{
			get
			{
				if (_timeFormats.Length == 0)
				{
					_timeFormats = new string[2];
					_timeFormats[0] = "HH:mm:ss";
					_timeFormats[1] = "yyyyMM";
				}
				return _timeFormats;
			}
		}

		private static Nullable<DateTime> _stopTime = null;
		internal static DateTime StopTime
		{
			get
			{
				if (_stopTime == null)
				{
					DateTime tDT;
					if (DateTime.TryParseExact(Configuration.User.Default.StopTime, TimeFormats, System.Globalization.CultureInfo.CurrentCulture, System.Globalization.DateTimeStyles.None, out tDT))
					{
					}
					else
					{
						Configuration.User.Default.StopTime = "23:55:00";
						Configuration.User.Default.Save();
						DateTime.TryParseExact(Configuration.User.Default.StopTime, TimeFormats, System.Globalization.CultureInfo.CurrentCulture, System.Globalization.DateTimeStyles.None, out tDT);
					}

					_stopTime = tDT;
				}

				return (DateTime)_stopTime;
			}
		}

		private static Nullable<DateTime> _stockAutoCancelTime = null;
		internal static DateTime StockAutoCancelTime
		{
			get
			{
				if (_stockAutoCancelTime == null)
				{
					DateTime tDT;
					if (DateTime.TryParseExact(Configuration.User.Default.StockAutoCancelTime, TimeFormats, System.Globalization.CultureInfo.CurrentCulture, System.Globalization.DateTimeStyles.None, out tDT))
					{
					}
					else
					{
						Configuration.User.Default.StockAutoCancelTime = "15:00:00";
						Configuration.User.Default.Save();
						DateTime.TryParseExact(Configuration.User.Default.StockAutoCancelTime, TimeFormats, System.Globalization.CultureInfo.CurrentCulture, System.Globalization.DateTimeStyles.None, out tDT);
					}

					_stockAutoCancelTime = tDT;
				}

				return (DateTime)_stockAutoCancelTime;
			}
			set
			{
				_stockAutoCancelTime = null;
			}
		}

		private static Nullable<DateTime> _futureAutoCancelTime = null;
		internal static DateTime FutureAutoCancelTime
		{
			get
			{
				if (_futureAutoCancelTime == null)
				{
					DateTime tDT;
					if (DateTime.TryParseExact(Configuration.User.Default.FutureAutoCancelTime, TimeFormats, System.Globalization.CultureInfo.CurrentCulture, System.Globalization.DateTimeStyles.None, out tDT))
					{
					}
					else
					{
						Configuration.User.Default.FutureAutoCancelTime = "15:30:00";
						Configuration.User.Default.Save();
						DateTime.TryParseExact(Configuration.User.Default.FutureAutoCancelTime, TimeFormats, System.Globalization.CultureInfo.CurrentCulture, System.Globalization.DateTimeStyles.None, out tDT);
					}

					_futureAutoCancelTime = tDT;
				}

				return (DateTime)_futureAutoCancelTime;
			}
			set
			{
				_futureAutoCancelTime = null;
			}
		}

		private static Nullable<DateTime> _optionAutoCancelTime = null;
		internal static DateTime OptionAutoCancelTime
		{
			get
			{
				if (_optionAutoCancelTime == null)
				{
					DateTime tDT;
					if (DateTime.TryParseExact(Configuration.User.Default.OptionAutoCancelTime, TimeFormats, System.Globalization.CultureInfo.CurrentCulture, System.Globalization.DateTimeStyles.None, out tDT))
					{
					}
					else
					{
						Configuration.User.Default.OptionAutoCancelTime = "15:00:00";
						Configuration.User.Default.Save();
						DateTime.TryParseExact(Configuration.User.Default.OptionAutoCancelTime, TimeFormats, System.Globalization.CultureInfo.CurrentCulture, System.Globalization.DateTimeStyles.None, out tDT);
					}

					_optionAutoCancelTime = tDT;
				}

				return (DateTime)_optionAutoCancelTime;
			}
			set
			{
				_optionAutoCancelTime = null;
			}
		}

		internal static bool ROCLoginCompleted
		{
			get
			{
				if (GLOBAL.HROM.Status == HelperROM.StatusTypes.LoggedIn &&
					GLOBAL.HRDS.Status == HelperRDS.StatusTypes.Done)
				{
					//if (Configuration.User.Default.UseMarketData)
					//{
					//    if (GLOBAL.MDSsConnected)
					//    {
					//        return true;
					//    }
					//}
					//else
					//{
					//    return true;
					//}

					// Login is completed even if MDS is not connected
					return true;
				}
				return false;
			}
		}

		private static bool _interrupt = false;
		internal static bool Interrupt
		{
			get
			{
				return _interrupt;
			}
			set
			{
				_interrupt = value;

				//if (value && Configuration.ROC.Default.LargeScreenSize)
				//{
				//    _interrupt = true;
				//}
				//else
				//{
				//    _interrupt = false;
				//}
			}
		}

		private static bool _shuttingDown = false;
		internal static bool ShuttingDown
		{
			get
			{
				return _shuttingDown;
			}
			set
			{
				_shuttingDown = value;
			}
		}

		private static bool _adminMode = false;
		internal static bool AdminMode
		{
			get
			{
				return _adminMode;
			}
			set
			{
				_adminMode = value;
			}
		}

		private static bool _useDelayedUpdate = false;
		internal static bool UseDelayedUpdate
		{
			get
			{
				return _useDelayedUpdate;
			}
		}

		private static bool _reloadProcessSettings = false;
		internal static bool ReloadProcessSettings
		{
			get
			{
				return _reloadProcessSettings;
			}
			set
			{
				_reloadProcessSettings = value;
			}
		}

		internal static int AvgPriceResolution
		{
			get
			{
				// Used By Order and Position Window
				return Configuration.User.Default.OrderAvgPriceResolution;
			}
		}

		#region - ROM Login -

		internal static void InitializeROM()
		{
			if (GLOBAL.HROM.Status != HelperROM.StatusTypes.Started)
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

		private static frmMain _mainForm;
		internal static frmMain MainForm
		{
			get
			{
				if (_mainForm == null)
				{
					foreach (frmMain main in HWindows.MainWindows.Values)
					{
						// There should only be one main form
						_mainForm = main;
						break;
					}
				}
				return _mainForm;
			}
		}

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

		private static bool _useMDSBackup = false;
		internal static bool UseMDSBackup
		{
			get
			{
				return _useMDSBackup;
			}
			set
			{
				_useMDSBackup = value;
			}
		}

		private static List<HelperMDS> _hMDSs;
		private static List<HelperMDS> _hMDSBackups;
		internal static List<HelperMDS> HMDSs
		{
			get
			{
				if (_useMDSBackup)
				{
					#region - MDS Backup -

					if (_hMDSBackups == null || _hMDSBackups.Count == 0)
					{
						_hMDSBackups = new List<HelperMDS>();

						string[] serverIPs = Configuration.MDS.Default.IPBackup.ToUpper().Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries);
						string[] serverPorts = Configuration.MDS.Default.PortBackup.ToUpper().Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries);

						int ipCount = 0;
						List<int> ports = new List<int>();
						foreach (string serverPort in serverPorts)
						{
							try
							{
								ports.Add(Convert.ToInt32(serverPort));
							}
							catch
							{
							}
						}

						foreach (string serverIP in serverIPs)
						{
							if (ipCount < ports.Count)
							{
								_hMDSBackups.Add(new HelperMDS(serverIP, ports[ipCount]));
							}
							else
							{
								_hMDSBackups.Add(new HelperMDS(serverIP, ports[0]));
							}
							ipCount++;
						}
					}

					#endregion

					return _hMDSBackups;
				}
				else
				{
					#region - MDS -

					if (_hMDSs == null || _hMDSs.Count == 0)
					{
						_hMDSs = new List<HelperMDS>();

						string[] serverIPs = Configuration.MDS.Default.IP.ToUpper().Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries);
						string[] serverPorts = Configuration.MDS.Default.Port.ToUpper().Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries);

						int ipCount = 0;
						List<int> ports = new List<int>();
						foreach (string serverPort in serverPorts)
						{
							try
							{
								ports.Add(Convert.ToInt32(serverPort));
							}
							catch
							{
							}
						}

						foreach (string serverIP in serverIPs)
						{
							if (ipCount < ports.Count)
							{
								_hMDSs.Add(new HelperMDS(serverIP, ports[ipCount]));
							}
							else
							{
								_hMDSs.Add(new HelperMDS(serverIP, ports[0]));
							}
							ipCount++;
						}
					}

					#endregion

					return _hMDSs;
				}
			}
			set
			{
				if (_useMDSBackup)
				{
					_hMDSBackups = value;
				}
				else
				{
					_hMDSs = value;
				}
			}
		}

		internal static bool MDSsConnected
		{
			get
			{
				foreach (HelperMDS mds in GLOBAL.HMDSs)
				{
					if (!mds.IsConnected)
						return false;
				}

				return true;
			}
		}

		internal static void MDSsDisconnect()
		{
			foreach (HelperMDS mds in GLOBAL.HMDSs)
			{
				mds.Disconnect();
			}
		}

		internal static void MDSsReconnect()
		{
			foreach (HelperMDS mds in GLOBAL.HMDSs)
			{
				mds.Reconnect();
			}
		}

		private static HelperROM _hROM;
		internal static HelperROM HROM
		{
			get
			{
				if (_hROM == null)
				{
					_hROM = new HelperROM();
				}
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
				{
					_hRDS = new HelperRDS();
				}
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
				{
					_hROC = new HelperROC();
				}
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
				{
					_hProcess = new HelperProcess();
				}
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
				{
					_hWindows = new HelperWindows();
				}
				return _hWindows;
			}
		}

		private static HelperOrdersData _hOrders;
		internal static HelperOrdersData HOrders
		{
			get
			{
				if (_hOrders == null)
				{
					_hOrders = new HelperOrdersData();
				}
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
				{
					_hExecutions = new HelperExecutionsData();
				}
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
				{
					_hPositions = new HelperPositionsData();
				}
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
				{
					_hMarketData = new HelperMarketData();
				}
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
				{
					_userUIProfile = new UIProfile();
				}
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

#region - Log -
#if OLD
		internal static class HLog
		{
			internal static void Create(string logPath, int logRetentionDays, long logFileSizeLimit)
			{
				ROC = new Logger(logPath, LogTypes.Log, logRetentionDays, logFileSizeLimit);
				USER = new Logger(logPath, LogTypes.User, logRetentionDays, logFileSizeLimit);

				MDS = new Logger(logPath, LogTypes.MDS, logRetentionDays, logFileSizeLimit);
				RDS = new Logger(logPath, LogTypes.RDS, logRetentionDays, logFileSizeLimit);
				ROM = new Logger(logPath, LogTypes.ROM, logRetentionDays, logFileSizeLimit);

				Orders = new Logger(logPath, LogTypes.Orders, logRetentionDays, logFileSizeLimit);
				Positions = new Logger(logPath, LogTypes.Positions, logRetentionDays, logFileSizeLimit);
				Trades = new Logger(logPath, LogTypes.Trades, logRetentionDays, logFileSizeLimit);

				BMK = new Logger(logPath, LogTypes.Benchmark, logRetentionDays, logFileSizeLimit);
			}

			internal static Logger ROC { get; private set; }
			internal static Logger USER { get; private set; }         // User Action Log

			internal static Logger MDS { get; private set; }          // MDS TCP Log
			internal static Logger RDS { get; private set; }
			internal static Logger ROM { get; private set; }

			internal static Logger Orders { get; private set; }       // ROM Order Log
			internal static Logger Positions { get; private set; }        // TPOS Position Log
			internal static Logger Trades { get; private set; }       // TPOS Trade Log

			internal static Logger BMK { get; private set; }          // Performance Log
		}
#endif // OLD
#endregion

#region - Option To Exchange Maps -

		private static Dictionary<string, List<string>> _optionToExchangeMaps;
		internal static Dictionary<string, List<string>> OptionToExchangeMaps
		{
			get
			{
				if (_optionToExchangeMaps == null)
				{
					_optionToExchangeMaps = new Dictionary<string,List<string>>();

					List<string> exchanges = new List<string>();
					//exchanges.Add(MarketDataEx.OptionExchangeCode.CBOE);
					_optionToExchangeMaps.Add("SPX", exchanges);
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
				{
					_rocSounds = new HelperSound();
				}
				return _rocSounds;
			}
		}

#endregion

#region - Month Codes -

		internal static class MonthCodes
		{
			internal const string January = "F";
			internal const string February = "G";
			internal const string March = "H";
			internal const string April = "J";
			internal const string May = "K";
			internal const string June = "M";
			internal const string July = "N";
			internal const string August = "Q";
			internal const string September = "U";
			internal const string October = "V";
			internal const string November = "X";
			internal const string December = "Z";
		}

#endregion

#region - Micro Bench -

		internal static class BENCHAMRK
		{
			internal static Benchmark Mark;
		}

#endregion
	}
}
