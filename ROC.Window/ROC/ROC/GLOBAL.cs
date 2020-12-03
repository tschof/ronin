using System;
using System.Diagnostics;
using System.Windows.Forms;

using LoggerEx;
using BenchMarkEx;
using System.Collections.Generic;
using System.Data;
using RDSEx;
using System.Drawing;
using System.Runtime.InteropServices;

namespace ROC
{
	public enum TicketTypes
	{
		None,
		Stock,
		Future,
		Quick,
		Option,
		AutoSpread,
		FutureMatrix,
	}

	public sealed class AutomationType
	{
		public const string AutoCancelStock = "AUTO_CANCEL_STOCK";
		public const string AutoCancelFuture = "AUTO_CANCEL_FUTURE";
		public const string AutoCancelOption = "AUTO_CANCEL_OPTION";
		public const string AutoStop = "AUTO_STOP";
	}

	public sealed class TicketDefaults
	{
		public const string Stock = "STK_DEF";
		public const string Future = "FUT_DEF";
		public const string Option = "OPT_DEF";
	}

	public sealed class QuickButtonSides
	{
		public const string Buy = "BUY";
		public const string Sell = "SELL";
		public const string Short = "SHORT";

		public const string BuyOpen = "BUYOPEN";
		public const string BuyClose = "BUYCLOSE";
		public const string SellOpen = "SELLOPEN";
		public const string SellClose = "SELLCLOSE";
	}

	public sealed class QuickButtonBasePriceSource
	{
		public const string Bid = "BID";
		public const string Ask = "ASK";
		public const string Last = "LAST";
		public const string High = "HIGH";
		public const string Low = "LOW";
	}

	public sealed class AccountTypeToGet
	{
		private bool _cs = false;
		public bool CS
		{
			get
			{
				return _cs;
			}
		}
		
		private bool _fut = false;
		public bool FUT
		{
			get
			{
				return _fut;
			}
		}

		private bool _opt = false;
		public bool OPT
		{
			get
			{
				return _opt;
			}
		}

		public AccountTypeToGet(bool cs, bool fut, bool opt)
		{
			_cs = cs;
			_fut = fut;
			_opt = opt;
		}
	}

	public sealed class BaseSecurityInfo
	{
		public BaseSecurityInfo()
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
		public string MDSymbol
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
		public string MDSource
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
		public double TickSize
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

		public decimal TickSizeDec
		{
			get
			{
				return Convert.ToDecimal(TickSize);
			}
		}

		public int DecimalPlaces
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
		public double ContractSize
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
		public string SecType
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
		public string Underlying
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
		public string LongName
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
		public string Expiration
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

		public Nullable<DateTime> ExpirationDT
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
		public Dictionary<string, IMSSFutureInfo> SSFChain
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
		public Dictionary<string, IMOptionInfo> OptionChain
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

	public static class ROCOrderTypes
	{
		public static string AutoSpread = "[AS]";
	}

	public static class GLOBAL
	{
		public static bool ByPassRomLogin = Configuration.Override.Default.ByPassROM;
		public static bool ByPassRdsLogin = Configuration.Override.Default.ByPassRDS;
		public static bool ByPassMdsLogin = false;

		public static bool ShownDM
		{
			get
			{
				return !Debugger.IsAttached;
			}
		}
		
		public static Version ROCVersion = new Version(Application.ProductVersion);

		private static string[] _timeFormats = new string[0];
		public static string[] TimeFormats
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
		public static DateTime StopTime
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
		public static DateTime StockAutoCancelTime
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
		public static DateTime FutureAutoCancelTime
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
		public static DateTime OptionAutoCancelTime
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

		public static bool ROCLoginCompleted
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
		public static bool Interrupt
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
		public static bool ShuttingDown
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
		public static bool AdminMode
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
		public static bool UseDelayedUpdate
		{
			get
			{
				return _useDelayedUpdate;
			}
		}

		private static bool _reloadProcessSettings = false;
		public static bool ReloadProcessSettings
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

		public static int AvgPriceResolution
		{
			get
			{
				// Used By Order and Position Window
				return Configuration.User.Default.OrderAvgPriceResolution;
			}
		}

		#region - ROM Login -

		public static void InitializeROM()
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
		public static frmMain MainForm
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
		public static frmLogin LoginForm
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

		public static HelperEMail HMail = new HelperEMail();

		private static bool _useMDSBackup = false;
		public static bool UseMDSBackup
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
		public static List<HelperMDS> HMDSs
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

		private static bool _mdssConnected = false;
		public static bool MDSsConnected
		{
			get
			{
				_mdssConnected = true;
				foreach (HelperMDS mds in GLOBAL.HMDSs)
				{
					if (mds.Status != HelperMDS.StatusTypes.Started)
					{
						_mdssConnected = false;
						break;
					}
				}

				return _mdssConnected;
			}
		}

		public static void MDSsDisconnect()
		{
			foreach (HelperMDS mds in GLOBAL.HMDSs)
			{
				mds.Disconnect();
			}
		}

		public static void MDSsReconnect()
		{
			foreach (HelperMDS mds in GLOBAL.HMDSs)
			{
				mds.Reconnect();
			}
		}

		private static HelperROM _hROM;
		public static HelperROM HROM
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
		public static HelperRDS HRDS
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
		public static HelperROC HROC
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
		public static HelperProcess HProcess
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
		public static HelperWindows HWindows
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
		public static HelperOrdersData HOrders
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
		public static HelperExecutionsData HExecutions
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
		public static HelperPositionsData HPositions
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
		public static HelperMarketData HMarketData
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
		public static HelperSymbolSettingData HSymbolSettingData
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
		public static HelperQuickButtonSettingData HQuickButtonSettingData
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
		public static UIProfile UserUIProfile
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
		public static frmWait WaitWindow
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

		public static class HLog
		{
			public static Logger ROC;
			public static Logger USER;			// User Action Log

			public static Logger MDS;			// MDS TCP Log
			public static Logger RDS;
			public static Logger ROM;

			public static Logger Orders;		// ROM Order Log
			public static Logger Positions;		// TPOS Position Log
			public static Logger Trades;		// TPOS Trade Log

			public static Logger BMK;			// Performance Log
		}

		#endregion

		#region - Option To Exchange Maps -

		private static Dictionary<string, List<string>> _optionToExchangeMaps;
		public static Dictionary<string, List<string>> OptionToExchangeMaps
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
		public static HelperSound ROCSounds
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

		public static class MonthCodes
		{
			public const string January = "F";
			public const string February = "G";
			public const string March = "H";
			public const string April = "J";
			public const string May = "K";
			public const string June = "M";
			public const string July = "N";
			public const string August = "Q";
			public const string September = "U";
			public const string October = "V";
			public const string November = "X";
			public const string December = "Z";
		}

		#endregion

		#region - Micro Bench -

		public static class BENCHAMRK
		{
			public static Benchmark Mark;
		}

		#endregion
	}
}
