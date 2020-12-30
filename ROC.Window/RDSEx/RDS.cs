using System;
using System.ComponentModel;
using System.Collections.Generic;

using RDSEx.WEB;
using DictionaryEx;
using System.Diagnostics;
using CSVEx;

namespace RDSEx
{
	public delegate void SecurityEventHandler(object sender, SecurityEventArgs e);
	public delegate void OptionChainEventHandler(object sender, OptionChainEventArgs e);
	public delegate void SSFutureChainEventHandler(object sender, SSFutureChainEventArgs e);

	public class RDS : INotifyPropertyChanged
	{
		#region - INotifyPropertyChanged Members -

		public event PropertyChangedEventHandler PropertyChanged;

		protected virtual void onPropertyChanged(PropertyChangedEventArgs e)
		{
			if (PropertyChanged != null)
			{
				PropertyChanged(this, e);
			}
		}

		#endregion

		#region - Event -

		public event SecurityEventHandler OnSecurityEvent;
		protected virtual void OnSecurityReceived(SecurityEventArgs e)
		{
			if (OnSecurityEvent != null)
			{
				OnSecurityEvent(this, e);
			}
		}

		public event OptionChainEventHandler OnOptionChainEvent;
		protected virtual void OnOptionChainReceived(OptionChainEventArgs e)
		{
			if (OnOptionChainEvent != null)
			{
				OnOptionChainEvent(this, e);
			}
		}

		public event SSFutureChainEventHandler OnSSFutureChainEvent;
		protected virtual void OnSSFutureChainReceived(SSFutureChainEventArgs e)
		{
			if (OnSSFutureChainEvent != null)
			{
				OnSSFutureChainEvent(this, e);
			}
		}

		#endregion

		private RdsService _rdsService;

		private object _syncObj = new object();

		public RDS(string webServiceLocation)
		{
			Properties.Settings.Default.RDSEx_WEB_RdsService = webServiceLocation;
			Properties.Settings.Default.Save();

			if (_rdsService == null)
			{
				_rdsService = new RdsService();
				_rdsService.GetVersionCompleted += new GetVersionCompletedEventHandler(rdsService_GetVersionCompleted);
				
				// ROM
				_rdsService.GetUserMarketDataSourcesCompleted += new GetUserMarketDataSourcesCompletedEventHandler(rdsService_GetUserMarketDataSourcesCompleted);
				_rdsService.GetUserProfileCompleted += new GetUserProfileCompletedEventHandler(rdsService_GetUserProfileCompleted);
				_rdsService.GetUserOrdersCompleted += new GetUserOrdersCompletedEventHandler(rdsService_GetUserOrdersCompleted);
				_rdsService.GetUserExecutionsCompleted += new GetUserExecutionsCompletedEventHandler(rdsService_GetUserExecutionsCompleted);

				// IM
				_rdsService.GetSecurityByTickerCompleted += new GetSecurityByTickerCompletedEventHandler(rdsService_GetSecurityByTickerCompleted);
				_rdsService.GetOptionChainCompleted += new GetOptionChainCompletedEventHandler(rdsService_GetOptionChainCompleted);
				_rdsService.GetSSFutureChainCompleted += new GetSSFutureChainCompletedEventHandler(rdsService_GetSSFutureChainCompleted);

				// TPOS
				_rdsService.GetTposPositionsBylocalAcctCompleted += new GetTposPositionsBylocalAcctCompletedEventHandler(rdsService_GetTposPositionsBylocalAcctCompleted);
				_rdsService.RtrvTposTradesByTraderCompleted += new RtrvTposTradesByTraderCompletedEventHandler(rdsService_GetTposTradesByTraderCompleted);

				// Optional
				//_rdsService.GetSpreadByTickerCompleted += new GetSpreadByTickerCompletedEventHandler(rdsService_GetSpreadByTickerCompleted);
				//_rdsService.GetMappedSecurityByTickerCompleted += new GetMappedSecurityByTickerCompletedEventHandler(rdsService_GetMappedSecurityByTickerCompleted);
			}
		}
		
		public string Address
		{
			get
			{
				return Properties.Settings.Default.RDSEx_WEB_RdsService;
			}
		}

		private string _errorMsg = "";
		public string ErrorMsg
		{
			get
			{
				return _errorMsg;
			}
			set
			{
				if (_errorMsg != value)
				{
					_errorMsg = value;
					onPropertyChanged(new PropertyChangedEventArgs("ErrorMsg"));
				}
			}
		}

		private string _warningMsg = "";
		public string WarningMsg
		{
			get
			{
				return _warningMsg;
			}
			set
			{
				if (_warningMsg != value)
				{
					_warningMsg = value;
					onPropertyChanged(new PropertyChangedEventArgs("WarningMsg"));
				}
			}
		}

		private string _version = "";
		public string Version
		{
			get
			{
				return _version;
			}
			set
			{
				if (_version != value)
				{
					_version = value;
					onPropertyChanged(new PropertyChangedEventArgs("Version"));
				}
			}
		}

		private string _user = "";
		private string _pass = "";

		private Dictionary<string, TraderMap> _userProfiles;
		public Dictionary<string, TraderMap> UserProfiles
		{
			get
			{
				if (_userProfiles == null)
				{
					_userProfiles = new Dictionary<string, TraderMap>();
				}
				return _userProfiles;
			}
			set
			{
				_userProfiles = value;
			}
		}

		private List<string> _userMarketDataSources;
		public List<string> UserMarketDataSources
		{
			get
			{
				if (_userMarketDataSources == null)
				{
					_userMarketDataSources = new List<string>();
				}
				return _userMarketDataSources;
			}
			set
			{
				_userMarketDataSources = value;
			}
		}

		#region - Trader to Stock Acct Map -

		private Dictionary<string, List<AccountMap>> _traderToStockAccts;
		public Dictionary<string, List<AccountMap>> TraderToStockAccts
		{
			get
			{
				if (_traderToStockAccts == null)
				{
					_traderToStockAccts = new Dictionary<string, List<AccountMap>>();
				}
				return _traderToStockAccts;
			}
			set
			{
				_traderToStockAccts = value;
			}
		}

		private Dictionary<string, List<string>> _traderToStockAcctNames;
		public Dictionary<string, List<string>> TraderToStockAcctNames
		{
			get
			{
				if (_traderToStockAcctNames == null)
				{
					_traderToStockAcctNames = new Dictionary<string, List<string>>();
				}
				return _traderToStockAcctNames;
			}
			set
			{
				_traderToStockAcctNames = value;
			}
		}

		#endregion

		#region - Trader to Future Acct Map -

		private Dictionary<string, List<AccountMap>> _traderToFutureAccts;
		public Dictionary<string, List<AccountMap>> TraderToFutureAccts
		{
			get
			{
				if (_traderToFutureAccts == null)
				{
					_traderToFutureAccts = new Dictionary<string, List<AccountMap>>();
				}
				return _traderToFutureAccts;
			}
			set
			{
				_traderToFutureAccts = value;
			}
		}

		private Dictionary<string, List<string>> _traderToFutureAcctNames;
		public Dictionary<string, List<string>> TraderToFutureAcctNames
		{
			get
			{
				if (_traderToFutureAcctNames == null)
				{
					_traderToFutureAcctNames = new Dictionary<string, List<string>>();
				}
				return _traderToFutureAcctNames;
			}
			set
			{
				_traderToFutureAcctNames = value;
			}
		}

		#endregion

		#region - Trader to Option Acct Map -

		private Dictionary<string, List<AccountMap>> _traderToOptionAccts;
		public Dictionary<string, List<AccountMap>> TraderToOptionAccts
		{
			get
			{
				if (_traderToOptionAccts == null)
				{
					_traderToOptionAccts = new Dictionary<string, List<AccountMap>>();
				}
				return _traderToOptionAccts;
			}
			set
			{
				_traderToOptionAccts = value;
			}
		}

		private Dictionary<string, List<string>> _traderToOptionAcctNames;
		public Dictionary<string, List<string>> TraderToOptionAcctNames
		{
			get
			{
				if (_traderToOptionAcctNames == null)
				{
					_traderToOptionAcctNames = new Dictionary<string, List<string>>();
				}
				return _traderToOptionAcctNames;
			}
			set
			{
				_traderToOptionAcctNames = value;
			}
		}

		#endregion

		#region - Stock Acct to Dest Maps -

		private Dictionary<string, List<DestinationMap>> _localStockAcctToDests;
		public Dictionary<string, List<DestinationMap>> LocalStockAcctToDests
		{
			get
			{
				if (_localStockAcctToDests == null)
				{
					_localStockAcctToDests = new Dictionary<string, List<DestinationMap>>();
				}
				return _localStockAcctToDests;
			}
			set
			{
				_localStockAcctToDests = value;
			}
		}

		private Dictionary<string, List<int>> _localStockAcctToDestIDs;
		public Dictionary<string, List<int>> LocalStockAcctToDestIDs
		{
			get
			{
				if (_localStockAcctToDestIDs == null)
				{
					_localStockAcctToDestIDs = new Dictionary<string, List<int>>();
				}
				return _localStockAcctToDestIDs;
			}
			set
			{
				_localStockAcctToDestIDs = value;
			}
		}

		private Dictionary<string, List<string>> _localStockAcctToDestNames;
		public Dictionary<string, List<string>> LocalStockAcctToDestNames
		{
			get
			{
				if (_localStockAcctToDestNames == null)
				{
					_localStockAcctToDestNames = new Dictionary<string, List<string>>();
				}
				return _localStockAcctToDestNames;
			}
			set
			{
				_localStockAcctToDestNames = value;
			}
		}

		#endregion

		#region - Future Acct to Dest Maps -

		private Dictionary<string, List<DestinationMap>> _localFutureAcctToDests;
		public Dictionary<string, List<DestinationMap>> LocalFutureAcctToDests
		{
			get
			{
				if (_localFutureAcctToDests == null)
				{
					_localFutureAcctToDests = new Dictionary<string, List<DestinationMap>>();
				}
				return _localFutureAcctToDests;
			}
			set
			{
				_localFutureAcctToDests = value;
			}
		}

		private Dictionary<string, List<int>> _localFutureAcctToDestIDs;
		public Dictionary<string, List<int>> LocalFutureAcctToDestIDs
		{
			get
			{
				if (_localFutureAcctToDestIDs == null)
				{
					_localFutureAcctToDestIDs = new Dictionary<string, List<int>>();
				}
				return _localFutureAcctToDestIDs;
			}
			set
			{
				_localFutureAcctToDestIDs = value;
			}
		}

		private Dictionary<string, List<string>> _localFutureAcctToDestNames;
		public Dictionary<string, List<string>> LocalFutureAcctToDestNames
		{
			get
			{
				if (_localFutureAcctToDestNames == null)
				{
					_localFutureAcctToDestNames = new Dictionary<string, List<string>>();
				}
				return _localFutureAcctToDestNames;
			}
			set
			{
				_localFutureAcctToDestNames = value;
			}
		}

		#endregion

		#region - Option Acct To Dest Maps -

		private Dictionary<string, List<DestinationMap>> _localOptionAcctToDests;
		public Dictionary<string, List<DestinationMap>> LocalOptionAcctToDests
		{
			get
			{
				if (_localOptionAcctToDests == null)
				{
					_localOptionAcctToDests = new Dictionary<string, List<DestinationMap>>();
				}
				return _localOptionAcctToDests;
			}
			set
			{
				_localOptionAcctToDests = value;
			}
		}

		private Dictionary<string, List<int>> _localOptionAcctToDestIDs;
		public Dictionary<string, List<int>> LocalOptionAcctToDestIDs
		{
			get
			{
				if (_localOptionAcctToDestIDs == null)
				{
					_localOptionAcctToDestIDs = new Dictionary<string, List<int>>();
				}
				return _localOptionAcctToDestIDs;
			}
			set
			{
				_localOptionAcctToDestIDs = value;
			}
		}

		private Dictionary<string, List<string>> _localOptionAcctToDestNames;
		public Dictionary<string, List<string>> LocalOptionAcctToDestNames
		{
			get
			{
				if (_localOptionAcctToDestNames == null)
				{
					_localOptionAcctToDestNames = new Dictionary<string, List<string>>();
				}
				return _localOptionAcctToDestNames;
			}
			set
			{
				_localOptionAcctToDestNames = value;
			}
		}

		#endregion

		#region - Dest ID to Name and Map -

		private Dictionary<int, string> _destIDToName;
		public Dictionary<int, string> DestIDToName
		{
			get
			{
				if (_destIDToName == null)
				{
					_destIDToName = new Dictionary<int, string>();
				}
				return _destIDToName;
			}
			set
			{
				_destIDToName = value;
			}
		}

		private Dictionary<int, DestinationMap> _destIDToDestinationMap;
		public Dictionary<int, DestinationMap> DestIDToDestinationMap
		{
			get
			{
				if (_destIDToDestinationMap == null)
				{
					_destIDToDestinationMap = new Dictionary<int, DestinationMap>();
				}
				return _destIDToDestinationMap;
			}
			set
			{
				_destIDToDestinationMap = value;
			}
		}

		#endregion

		#region - Dest Name To ID and Map -

		private Dictionary<string, int> _destNameToID;
		public Dictionary<string, int> DestNameToID
		{
			get
			{
				if (_destNameToID == null)
				{
					_destNameToID = new Dictionary<string, int>();
				}
				return _destNameToID;
			}
			set
			{
				_destNameToID = value;
			}
		}

		private Dictionary<string, DestinationMap> _destNameToDestinationMap;
		public Dictionary<string, DestinationMap> DestNameToDestinationMap
		{
			get
			{
				if (_destNameToDestinationMap == null)
				{
					_destNameToDestinationMap = new Dictionary<string, DestinationMap>();
				}
				return _destNameToDestinationMap;
			}
			set
			{
				_destNameToDestinationMap = value;
			}
		}

		#endregion

		#region - Account & LocalAcAcrn Map & TradeFor -

		private List<string> _accountFilterMap;
		public List<string> AccountFilterMap
		{
			get
			{
				if (_accountFilterMap == null)
				{
					_accountFilterMap = new List<string>();
				}
				return _accountFilterMap;
			}
			set
			{
				_accountFilterMap = value;
			}
		}

		#endregion

		private bool _gotUserProfiles = false;
		public bool GotUserProfiles
		{
			get
			{
				return _gotUserProfiles;
			}
			set
			{
				if (_gotUserProfiles != value)
				{
					_gotUserProfiles = value;
					onPropertyChanged(new PropertyChangedEventArgs("GotUserProfiles"));
				}
			}
		}

		private bool _gotUserMarketDataSources = false;
		public bool GotUserMarketDataSources
		{
			get
			{
				return _gotUserMarketDataSources;
			}
			set
			{
				if (_gotUserMarketDataSources != value)
				{
					_gotUserMarketDataSources = value;
					onPropertyChanged(new PropertyChangedEventArgs("GotUserMarketDataSources"));
				}
			}
		}

		private Dictionary<string, ROCOrder> _rocOrders;
		public Dictionary<string, ROCOrder> RocOrders
		{
			get
			{
				if (_rocOrders == null)
				{
					_rocOrders = new Dictionary<string, ROCOrder>();
				}
				return _rocOrders;
			}
			set
			{
				_rocOrders = value;
			}
		}

		private bool _gotRocOrders = false;
		public bool GotRocOrders
		{
			get
			{
				return _gotRocOrders;
			}
			set
			{
				if (_gotRocOrders != value)
				{
					_gotRocOrders = value;
					onPropertyChanged(new PropertyChangedEventArgs("GotRocOrders"));
				}
			}
		}

		private Dictionary<string, ROCExecution> _rocExecutions;
		public Dictionary<string, ROCExecution> RocExecutions
		{
			get
			{
				if (_rocExecutions == null)
				{
					_rocExecutions = new Dictionary<string, ROCExecution>();
				}
				return _rocExecutions;
			}
			set
			{
				_rocExecutions = value;
			}
		}

		private bool _gotRocExecutions = false;
		public bool GotRocExecutions
		{
			get
			{
				return _gotRocExecutions;
			}
			set
			{
				if (_gotRocExecutions != value)
				{
					_gotRocExecutions = value;
					onPropertyChanged(new PropertyChangedEventArgs("GotRocExecutions"));
				}
			}
		}

		// Flages for the fist TPOS load, don't put the stuff in to the new collection;
		private bool _firstTposPositionCall = true;
		private bool _firstTposExecutionCall = true;

		// Tpos Positions nested with Tpos Trades By instrument ID
		private Dictionary<string, TPOSPosition> _tposPositions;
		public Dictionary<string, TPOSPosition> TposPositions
		{
			get
			{
				if (_tposPositions == null)
				{
					_tposPositions = new Dictionary<string, TPOSPosition>();
				}
				return _tposPositions;
			}
			set
			{
				_tposPositions = value;
			}
		}

		private Dictionary<string, TPOSPosition> _newTposPositions;
		public Dictionary<string, TPOSPosition> NewTposPositions
		{
			get
			{
				if (_newTposPositions == null)
				{
					_newTposPositions = new Dictionary<string, TPOSPosition>();
				}
				return _newTposPositions;
			}
			set
			{
				_newTposPositions = value;
			}
		}

		private bool _gotTposPositions = false;
		public bool GotTposPositions
		{
			get
			{
				return _gotTposPositions;
			}
			set
			{
				if (_gotTposPositions != value)
				{
					_gotTposPositions = value;
					onPropertyChanged(new PropertyChangedEventArgs("GotTposPositions"));
				}
			}
		}

		// Tpos Traded Only
		private Dictionary<string, TPOSExecution> _tposExecutions;
		public Dictionary<string, TPOSExecution> TposExecutions
		{
			get
			{
				if (_tposExecutions == null)
				{
					_tposExecutions = new Dictionary<string, TPOSExecution>();
				}
				return _tposExecutions;
			}
			set
			{
				_tposExecutions = value;
			}
		}

		private Dictionary<string, TPOSExecution> _newTposExecutions;
		public Dictionary<string, TPOSExecution> NewTposExecutions
		{
			get
			{
				if (_newTposExecutions == null)
				{
					_newTposExecutions = new Dictionary<string, TPOSExecution>();
				}
				return _newTposExecutions;
			}
			set
			{
				_newTposExecutions = value;
			}
		}

		private bool _gotTposExecutions = false;
		public bool GotTposExecutions
		{
			get
			{
				return _gotTposExecutions;
			}
			set
			{
				if (_gotTposExecutions != value)
				{
					_gotTposExecutions = value;
					onPropertyChanged(new PropertyChangedEventArgs("GotTposExecutions"));
				}
			}
		}

		private bool _gotTposPositionsRefresh = false;
		public bool GotTposPositionsRefresh
		{
			get
			{
				return _gotTposPositionsRefresh;
			}
			set
			{
				if (_gotTposPositionsRefresh != value)
				{
					_gotTposPositionsRefresh = value;
					onPropertyChanged(new PropertyChangedEventArgs("GotTposPositionsRefresh"));
				}
			}
		}

		private bool _gotTposExecutionsRefresh = false;
		public bool GotTposExecutionsRefresh
		{
			get
			{
				return _gotTposExecutionsRefresh;
			}
			set
			{
				if (_gotTposExecutionsRefresh != value)
				{
					_gotTposExecutionsRefresh = value;
					onPropertyChanged(new PropertyChangedEventArgs("GotTposExecutionsRefresh"));
				}
			}
		}

		// <rocSymbol, IMSecurityInfo>
		private Dictionary<string, IMSecurityInfo> _symbolSecurityInfos;
		public Dictionary<string, IMSecurityInfo> SymbolSecurityInfos
		{
			get
			{
				if (_symbolSecurityInfos == null)
				{
					_symbolSecurityInfos = new Dictionary<string, IMSecurityInfo>();
				}
				return _symbolSecurityInfos;
			}
			set
			{
				_symbolSecurityInfos = value;
			}
		}

		// <symbol, IMOptionInfo>
		private Dictionary<string, IMOptionInfo> _symbolToOptionInfos;
		public Dictionary<string, IMOptionInfo> SymbolToOptionInfos
		{
			get
			{
				if (_symbolToOptionInfos == null)
				{
					_symbolToOptionInfos = new Dictionary<string, IMOptionInfo>();
				}
				return _symbolToOptionInfos;
			}
			set
			{
				_symbolToOptionInfos = value;
			}
		}

		// <symbol, IMSSFutureInfo>
		private Dictionary<string, IMSSFutureInfo> _symbolToSSFInfos;
		public Dictionary<string, IMSSFutureInfo> SymbolToSSFInfos
		{
			get
			{
				if (_symbolToSSFInfos == null)
				{
					_symbolToSSFInfos = new Dictionary<string, IMSSFutureInfo>();
				}
				return _symbolToSSFInfos;
			}
			set
			{
				_symbolToSSFInfos = value;
			}
		}

		// <SymbolDetail, rocSymbol>
		private Dictionary<string, string> _symbolDetailToRocSymbolMap;
		public Dictionary<string, string> SymbolDetailToRocSymbolMap
		{
			get
			{
				if (_symbolDetailToRocSymbolMap == null)
				{
					_symbolDetailToRocSymbolMap = new Dictionary<string, string>();
				}
				return _symbolDetailToRocSymbolMap;
			}
			set
			{
				_symbolDetailToRocSymbolMap = value;
			}
		}

		private bool _imBusy = false;
		private List<string> _imSecurityInfoWaitingList;
		public List<string> ImSecurityInfoWaitingList
		{
			get
			{
				if (_imSecurityInfoWaitingList == null)
				{
					_imSecurityInfoWaitingList = new List<string>();
				}
				return _imSecurityInfoWaitingList;
			}
			set
			{
				_imSecurityInfoWaitingList = value;
			}
		}

		private List<string> _imOPTChainInfoWaitingList;
		public List<string> ImOPTChainInfoWaitingList
		{
			get
			{
				if (_imOPTChainInfoWaitingList == null)
				{
					_imOPTChainInfoWaitingList = new List<string>();
				}
				return _imOPTChainInfoWaitingList;
			}
			set
			{
				_imOPTChainInfoWaitingList = value;
			}
		}

		private List<string> _imSFFChainInfoWaitingList;
		public List<string> ImSFFChainInfoWaitingList
		{
			get
			{
				if (_imSFFChainInfoWaitingList == null)
				{
					_imSFFChainInfoWaitingList = new List<string>();
				}
				return _imSFFChainInfoWaitingList;
			}
			set
			{
				_imSFFChainInfoWaitingList = value;
			}
		}

		#region - Version -

		public void GetVersion()
		{
			_rdsService.GetVersionAsync();
		}

		private void rdsService_GetVersionCompleted(object sender, GetVersionCompletedEventArgs e)
		{
			try
			{
				Version = e.Result;
			}
			catch (Exception ex)
			{
				Version = ex.Source;
			}
		}

		#endregion

		#region - UserProfile -

		public void GetUserProfile(string user, string pass)
		{
			_user = user;
			_pass = pass;

			_rdsService.GetUserProfileAsync(user, pass);
		}

		private void rdsService_GetUserProfileCompleted(object sender, GetUserProfileCompletedEventArgs e)
		{
			if (e.Cancelled)
			{
				ErrorMsg = String.Concat(new object[] { "GetUserProfile|Cancelled", e.Error.Message, " ", e.Error.StackTrace });
			}
			else if (e.Error != null)
			{
				ErrorMsg = String.Concat(new object[] { "GetUserProfile|Error ", e.Error.Message, " ", e.Error.StackTrace });
			}
			else
			{
				switch (e.Result.statusCode)
				{
					case 0:
						try
						{
							if (e.Result.retValues != null)
							{
								lock (_syncObj)
								{
									DecodeTraderObj((object[])e.Result.retValues);
								}
								GotUserProfiles = true;

								BuildLocalAccounts();
							}
							else
							{
								ErrorMsg = String.Concat(new object[] { "GetUserProfile|Result Empty" });
							}
						}
						catch (Exception ex)
						{
							ErrorMsg = String.Concat(new object[] { "GetUserProfile|Exception ", ex.Message, " : ", ex.StackTrace });
						}
						break;
					default:
						ErrorMsg = String.Concat(new object[] { "GetUserProfile|Result|StatusCode ", e.Result.statusCode.ToString(), " : ", e.Result.statusMessage });
						break;
				}
			}

			if (!GotUserProfiles)
			{
				_user = "";
				_pass = "";
			}
		}

		#region - Market Data Profile -

		public void GetUserMarketDataSources()
		{
			if (_user != "" && _pass != "")
			{
				_rdsService.GetUserMarketDataSourcesAsync(_user, _pass);
			}
		}

		private void rdsService_GetUserMarketDataSourcesCompleted(object sender, GetUserMarketDataSourcesCompletedEventArgs e)
		{
			// Non-Critical Call Back
			// No matter what the login process will continue
			GotUserMarketDataSources = true;

			if (e.Cancelled)
			{
				WarningMsg = String.Concat(new object[] { "GetUserMarketDataSources|Cancelled", e.Error.Message, " ", e.Error.StackTrace });
			}
			else if (e.Error != null)
			{
				WarningMsg = String.Concat(new object[] { "GetUserMarketDataSources|Error ", e.Error.Message, " ", e.Error.StackTrace });
			}
			else
			{
				switch (e.Result.statusCode)
				{
					case 0:
						try
						{
							if (e.Result.retValues != null)
							{
								lock (_syncObj)
								{
									DecodeUserMarketDataObj((object[])e.Result.retValues);
								}
							}
							else
							{
								WarningMsg = String.Concat(new object[] { "GetUserMarketDataSources|Result Empty" });
							}
						}
						catch (Exception ex)
						{
							WarningMsg = String.Concat(new object[] { "GetUserMarketDataSources|Exception ", ex.Message, " : ", ex.StackTrace });
						}
						break;
					default:
						WarningMsg = String.Concat(new object[] { "GetUserMarketDataSources|Result|StatusCode ", e.Result.statusCode.ToString(), " : ", e.Result.statusMessage });
						break;
				}
			}
		}

		private void DecodeUserMarketDataObj(object[] userMarketDataObjs)
		{
			foreach (object userMarketDataObj in userMarketDataObjs)
			{
				if (userMarketDataObj != null)
				{
					UserMarketDataSources.Add((string)userMarketDataObj);
				}
			}
		}

		#endregion

		#region - User Profile - Traders -

		private void DecodeTraderObj(object[] traderAcrObjs)
		{
			object[] acctProfileObjs;

			string tradeFor = "";
			TraderMap traderMap;

			foreach (object traderAcrObj in traderAcrObjs)
			{
				if (traderAcrObj != null)
				{
					tradeFor = "";
					traderMap = new TraderMap();
					acctProfileObjs = DecodeTrader((TraderAcr)traderAcrObj, ref traderMap, ref tradeFor);

					if (acctProfileObjs != null)
					{
						DecodAcctProfileObj(acctProfileObjs, ref traderMap);
					}

					if (tradeFor != "")
					{
						UserProfiles[tradeFor] = traderMap;
					}
				}
			}
		}

		private object[] DecodeTrader(TraderAcr traderAcr, ref TraderMap traderMap, ref string tradeFor)
		{
			tradeFor = traderAcr.tradeFor.ToUpper();
			if (UserProfiles.ContainsKey(tradeFor))
			{
				traderMap = UserProfiles[tradeFor];
			}
			else
			{
				traderMap = new TraderMap();
				UserProfiles.Add(tradeFor, traderMap);
			}

			traderMap.Set(TraderFieldIDs.TradeFor, traderAcr.tradeFor.ToUpper());
			traderMap.Set(TraderFieldIDs.LocalAcAcrn, traderAcr.localAcAcrn.ToUpper());

			return (object[])traderAcr.acctProfiles;
		}

		#endregion

		#region - User Profile - Traders - Accounts -

		private void DecodAcctProfileObj(object[] acctProfileObjs, ref TraderMap traderMap)
		{
			foreach (object acctProfileObj in acctProfileObjs)
			{
				if (acctProfileObj != null)
				{
					DecodeAcctProfile((AcctProfile)acctProfileObj, ref traderMap);
				}
			}
		}

		private void DecodeAcctProfile(AcctProfile acctProfile, ref TraderMap traderMap)
		{
			AccountMap acctMap;
			acctMap = GetAcctMapByType(acctProfile, ref traderMap);

			UpdateAcctMap(acctProfile, ref acctMap);

			if (acctProfile.accountDestinations != null)
			{
				DecodeDestinationObj((object[])acctProfile.accountDestinations, ref acctMap);
			}
		}

		private AccountMap GetAcctMapByType(AcctProfile acctProfile, ref TraderMap traderMap)
		{
			AccountMap acctMap;

			switch (acctProfile.type)
			{
				case AccountTypes.Stock:
					if (traderMap.CSAccounts.ContainsKey(acctProfile.account))
					{
						acctMap = traderMap.CSAccounts[acctProfile.account];
					}
					else
					{
						acctMap = new AccountMap();
						traderMap.CSAccounts.Add(acctProfile.account, acctMap);
					}
					break;
				case AccountTypes.Option:
					if (traderMap.OPTAccounts.ContainsKey(acctProfile.account))
					{
						acctMap = traderMap.OPTAccounts[acctProfile.account];
					}
					else
					{
						acctMap = new AccountMap();
						traderMap.OPTAccounts.Add(acctProfile.account, acctMap);
					}
					break;
				case AccountTypes.Future:
					if (traderMap.FUTAccounts.ContainsKey(acctProfile.account))
					{
						acctMap = traderMap.FUTAccounts[acctProfile.account];
					}
					else
					{
						acctMap = new AccountMap();
						traderMap.FUTAccounts.Add(acctProfile.account, acctMap);
					}
					break;
				default:
					acctMap = null;
					break;
			}

			return acctMap;
		}

		private void UpdateAcctMap(AcctProfile acctProfile, ref AccountMap acctMap)
		{
			acctMap.Set(AccountFieldIDs.InterfaceID, acctProfile.interfaceID);
			acctMap.Set(AccountFieldIDs.Account, acctProfile.account.ToUpper());
			acctMap.Set(AccountFieldIDs.Type, acctProfile.type.ToUpper());
			acctMap.Set(AccountFieldIDs.ClearingAcID, acctProfile.clearingAcID.ToUpper());
			acctMap.Set(AccountFieldIDs.LocalAcAcrn, acctProfile.localAcAcrn.ToUpper());
			acctMap.Set(AccountFieldIDs.FirmAcr, acctProfile.firmAcr.ToUpper());
			acctMap.Set(AccountFieldIDs.OmAcctType, acctProfile.omAcctType.ToUpper());
			acctMap.Set(AccountFieldIDs.Capacity, acctProfile.capacity.ToUpper());
			acctMap.Set(AccountFieldIDs.InterfaceID, acctProfile.interfaceID);
			acctMap.Set(AccountFieldIDs.ClearingFirmID, acctProfile.clearingFirmID.ToUpper());
			acctMap.Set(AccountFieldIDs.OccAcr, acctProfile.occAcr.ToUpper());
			acctMap.Set(AccountFieldIDs.HomeExchange, acctProfile.homeExchange.ToUpper());
		}

		#endregion

		#region - User Profile - Traders - Accounts - Destinations -

		private void DecodeDestinationObj(object[] acctDestinationObjs, ref AccountMap acctMap)
		{
			foreach (object acctDestinationObj in acctDestinationObjs)
			{
				if (acctDestinationObj != null)
				{
					DecodeDestination((AcctDestination)acctDestinationObj, ref acctMap);
				}
			}
		}

		private void DecodeDestination(AcctDestination acctDestination, ref AccountMap acctMap)
		{
			DestinationMap destMap;

			if (acctMap.Destinations.ContainsKey(acctDestination.destID))
			{
				destMap = acctMap.Destinations[acctDestination.destID];
			}
			else
			{
				destMap = new DestinationMap();
				acctMap.Destinations.Add(acctDestination.destID, destMap);
			}

			destMap.Set(DestinationFieldIDs.DestID, acctDestination.destID);
			destMap.Set(DestinationFieldIDs.SecType, acctDestination.secType.ToUpper());
			destMap.Set(DestinationFieldIDs.ShortName, acctDestination.shortName.ToUpper());
			destMap.Set(DestinationFieldIDs.CMTAFirmID, acctDestination.cmtaFirmID.ToUpper());
			destMap.Set(DestinationFieldIDs.GiveupFirmID, acctDestination.giveupFirmID.ToUpper());
			if (acctDestination.algoFlag != null)
			{
				destMap.Set(DestinationFieldIDs.AlgoFlag, acctDestination.algoFlag.ToUpper());
			}
			else
			{
				destMap.Set(DestinationFieldIDs.AlgoFlag, "0");
			}

			acctMap.Destinations[acctDestination.destID] = destMap;

			BuildDestinationMaps(destMap);
		}

		#endregion

		#region - Quick Local Account - Destinations Maps

		private void BuildLocalAccounts()
		{
			List<AccountMap> accts;
			List<string> acctNames;

			List<DestinationMap> dests;
			List<int> destIDs;
			List<string> destNames;

			if (GotUserProfiles)
			{
				foreach (TraderMap tm in UserProfiles.Values)
				{
					#region - Stock Maps - 

					foreach (AccountMap acctVal in tm.CSAccounts.Values )
					{
						AddToTposAccountFilter(acctVal);

						#region - Trader to Acct -

						if (TraderToStockAccts.ContainsKey(tm.tradeFor))
						{
							accts = TraderToStockAccts[tm.tradeFor];
						}
						else
						{
							accts = new List<AccountMap>();
						}
						accts.Add(acctVal);

						if (TraderToStockAcctNames.ContainsKey(tm.tradeFor))
						{
							acctNames = TraderToStockAcctNames[tm.tradeFor];
						}
						else
						{
							acctNames = new List<string>();
						}
						acctNames.Add(acctVal.account);

						#endregion

						foreach (DestinationMap destVal in tm.CSAccounts[acctVal.account].Destinations.Values)
						{
							#region - Acct to Dest -

							if (LocalStockAcctToDests.ContainsKey(acctVal.account))
							{
								dests = LocalStockAcctToDests[acctVal.account];
							}
							else
							{
								dests = new List<DestinationMap>();
							}
							dests.Add(destVal);

							if (LocalStockAcctToDestIDs.ContainsKey(acctVal.account))
							{
								destIDs = LocalStockAcctToDestIDs[acctVal.account];
							}
							else
							{
								destIDs = new List<int>();
							}
							if (destVal.destID != null)
							{
								destIDs.Add((int)destVal.destID);
							}

							if (LocalStockAcctToDestNames.ContainsKey(acctVal.account))
							{
								destNames = LocalStockAcctToDestNames[acctVal.account];
							}
							else
							{
								destNames = new List<string>();
							}
							destNames.Add(destVal.shortName);

							#endregion

							LocalStockAcctToDests[acctVal.account] = dests;
							LocalStockAcctToDestIDs[acctVal.account] = destIDs;
							LocalStockAcctToDestNames[acctVal.account] = destNames;
						}

						TraderToStockAccts[tm.tradeFor] = accts;
						TraderToStockAcctNames[tm.tradeFor] = acctNames;
					}

					#endregion

					#region - Future Maps -

					foreach (AccountMap acctVal in tm.FUTAccounts.Values)
					{
						AddToTposAccountFilter(acctVal);

						#region - Trader to Acct -

						if (TraderToFutureAccts.ContainsKey(tm.tradeFor))
						{
							accts = TraderToFutureAccts[tm.tradeFor];
						}
						else
						{
							accts = new List<AccountMap>();
						}
						accts.Add(acctVal);

						if (TraderToFutureAcctNames.ContainsKey(tm.tradeFor))
						{
							acctNames = TraderToFutureAcctNames[tm.tradeFor];
						}
						else
						{
							acctNames = new List<string>();
						}
						acctNames.Add(acctVal.account);

						#endregion

						foreach (DestinationMap destVal in tm.FUTAccounts[acctVal.account].Destinations.Values)
						{
							#region - Acct to Dest -

							if (LocalFutureAcctToDests.ContainsKey(acctVal.account))
							{
								dests = LocalFutureAcctToDests[acctVal.account];
							}
							else
							{
								dests = new List<DestinationMap>();
							}
							dests.Add(destVal);

							if (LocalFutureAcctToDestIDs.ContainsKey(acctVal.account))
							{
								destIDs = LocalFutureAcctToDestIDs[acctVal.account];
							}
							else
							{
								destIDs = new List<int>();
							}
							if (destVal.destID != null)
							{
								destIDs.Add((int)destVal.destID);
							}

							if (LocalFutureAcctToDestNames.ContainsKey(acctVal.account))
							{
								destNames = LocalFutureAcctToDestNames[acctVal.account];
							}
							else
							{
								destNames = new List<string>();
							}
							destNames.Add(destVal.shortName);

							#endregion

							LocalFutureAcctToDests[acctVal.account] = dests;
							LocalFutureAcctToDestIDs[acctVal.account] = destIDs;
							LocalFutureAcctToDestNames[acctVal.account] = destNames;
						}

						TraderToFutureAccts[tm.tradeFor] = accts;
						TraderToFutureAcctNames[tm.tradeFor] = acctNames;
					}

					#endregion

					#region - Option Maps -

					foreach (AccountMap acctVal in tm.OPTAccounts.Values)
					{
						AddToTposAccountFilter(acctVal);

						#region - Trader to Acct -

						if (TraderToOptionAccts.ContainsKey(tm.tradeFor))
						{
							accts = TraderToOptionAccts[tm.tradeFor];
						}
						else
						{
							accts = new List<AccountMap>();
						}
						accts.Add(acctVal);

						if (TraderToOptionAcctNames.ContainsKey(tm.tradeFor))
						{
							acctNames = TraderToOptionAcctNames[tm.tradeFor];
						}
						else
						{
							acctNames = new List<string>();
						}
						acctNames.Add(acctVal.account);

						#endregion

						foreach (DestinationMap destVal in tm.OPTAccounts[acctVal.account].Destinations.Values)
						{
							#region - Acct to Dest -

							if (LocalOptionAcctToDests.ContainsKey(acctVal.account))
							{
								dests = LocalOptionAcctToDests[acctVal.account];
							}
							else
							{
								dests = new List<DestinationMap>();
							}
							dests.Add(destVal);

							if (LocalOptionAcctToDestIDs.ContainsKey(acctVal.account))
							{
								destIDs = LocalOptionAcctToDestIDs[acctVal.account];
							}
							else
							{
								destIDs = new List<int>();
							}
							if (destVal.destID != null)
							{
								destIDs.Add((int)destVal.destID);
							}

							if (LocalOptionAcctToDestNames.ContainsKey(acctVal.account))
							{
								destNames = LocalOptionAcctToDestNames[acctVal.account];
							}
							else
							{
								destNames = new List<string>();
							}
							destNames.Add(destVal.shortName);

							#endregion

							LocalOptionAcctToDests[acctVal.account] = dests;
							LocalOptionAcctToDestIDs[acctVal.account] = destIDs;
							LocalOptionAcctToDestNames[acctVal.account] = destNames;
						}

						TraderToOptionAccts[tm.tradeFor] = accts;
						TraderToOptionAcctNames[tm.tradeFor] = acctNames;
					}

					#endregion
				}
			}
		}

		private void AddToTposAccountFilter(AccountMap acctVal)
		{
			if (!AccountFilterMap.Contains(acctVal.localAcAcrn))
			{
				AccountFilterMap.Add(acctVal.localAcAcrn);
			}
			if (!AccountFilterMap.Contains(acctVal.clearingAcIDShort))
			{
				AccountFilterMap.Add(acctVal.clearingAcIDShort);
			}
			if (!AccountFilterMap.Contains(acctVal.clearingAcID))
			{
				AccountFilterMap.Add(acctVal.clearingAcID);
			}
			if (!AccountFilterMap.Contains(acctVal.account))
			{
				AccountFilterMap.Add(acctVal.account);
			}
		}

		#endregion

		#region - Quick Destination Maps -

		private void BuildDestinationMaps(DestinationMap destMap)
		{
			if (!DestIDToName.ContainsKey((int)destMap.destID))
			{
				DestIDToName.Add((int)destMap.destID, destMap.shortName);
			}

			if (!DestIDToDestinationMap.ContainsKey((int)destMap.destID))
			{
				DestIDToDestinationMap.Add((int)destMap.destID, destMap);
			}

			if (!DestNameToID.ContainsKey(destMap.shortName))
			{
				DestNameToID.Add(destMap.shortName, (int)destMap.destID);
			}

			if (!DestNameToDestinationMap.ContainsKey(destMap.shortName))
			{
				DestNameToDestinationMap.Add(destMap.shortName, destMap);
			}
		}

		#endregion

		#endregion

		#region - User Orders -

		public void GetUserOrders()
		{
			if (GotUserProfiles)
			{
				_rdsService.GetUserOrdersAsync(_user, _pass);
			}
		}

		private void rdsService_GetUserOrdersCompleted(object sender, GetUserOrdersCompletedEventArgs e)
		{
			if (e.Cancelled)
			{
				ErrorMsg = String.Concat(new object[] { "GetUserOrder|Cancelled", e.Error.Message, " ", e.Error.StackTrace });
			}
			else if (e.Error != null)
			{
				ErrorMsg = String.Concat(new object[] { "GetUserOrder|Error ", e.Error.Message, " ", e.Error.StackTrace });
			}
			else
			{
				switch (e.Result.statusCode)
				{
					case 0:
						try
						{
							if (e.Result.retValues != null)
							{
								lock (_syncObj)
								{
									DecodeUserOrderObjs((object[])e.Result.retValues);
								}
							}
							GotRocOrders = true;
						}
						catch (Exception ex)
						{
							ErrorMsg = String.Concat(new object[] { "GetUserOrder|Exception ", ex.Message, " : ", ex.StackTrace });
						}
						break;
					default:
						ErrorMsg = String.Concat(new object[] { "GetUserOrder|Result|StatusCode ", e.Result.statusCode.ToString(), " : ", e.Result.statusMessage });
						break;
				}
			}
		}

		private void DecodeUserOrderObjs(object[] userOrderObjs)
		{
			foreach (object userOrderObj in userOrderObjs)
			{
				if (userOrderObj != null)
				{
					DecodeUserOrder((Order)userOrderObj);
				}
			}
		}

		private void DecodeUserOrder(Order userOrder)
		{
			ROCOrder ord;
			if (RocOrders.ContainsKey(userOrder.omTag))
			{
				ord = RocOrders[userOrder.omTag];
			}
			else
			{
				ord = new ROCOrder();
				RocOrders.Add(userOrder.omTag, ord);
			}

			ord.Set(OrderFieldIDs.ROC.avgPrice, userOrder.avgPrice);
			ord.Set(OrderFieldIDs.ROC.callPut, userOrder.callPut);
			ord.Set(OrderFieldIDs.ROC.clearingAccount, userOrder.clearingAccount);
			ord.Set(OrderFieldIDs.ROC.clearingID, userOrder.clearingID);
			ord.Set(OrderFieldIDs.ROC.cumQty, userOrder.cumQty);
			ord.Set(OrderFieldIDs.ROC.destID, userOrder.destID);
			
			//ord.Update(OrderFieldIDs.ROC.effectiveTime, userOrder.effectiveTime);
			
			ord.Set(OrderFieldIDs.ROC.ex_short_name, userOrder.ex_short_name);
			ord.Set(OrderFieldIDs.ROC.expDate, userOrder.expDate);
			ord.Set(OrderFieldIDs.ROC.firm, userOrder.firm);
			ord.Set(OrderFieldIDs.ROC.instructions, userOrder.instructions);
			ord.Set(OrderFieldIDs.ROC.leavesQty, userOrder.leavesQty);
			ord.Set(OrderFieldIDs.ROC.localAcct, userOrder.localAcct);
			ord.Set(OrderFieldIDs.ROC.maxFloor, userOrder.maxFloor);
			
			//ord.Update(OrderFieldIDs.ROC.multiplier, userOrder.multiplier);
			
			ord.Set(OrderFieldIDs.ROC.omTag, userOrder.omTag);
			ord.Set(OrderFieldIDs.ROC.omTime, userOrder.omTime.ToLocalTime().ToOADate());
			ord.Set(OrderFieldIDs.ROC.openClose, userOrder.openClose);
			ord.Set(OrderFieldIDs.ROC.orderExpiresDate, userOrder.orderExpiresDate.ToOADate());
			ord.Set(OrderFieldIDs.ROC.orderType, userOrder.orderType);
			ord.Set(OrderFieldIDs.ROC.originalShares, userOrder.originalShares);
			ord.Set(OrderFieldIDs.ROC.owner, userOrder.owner);
			ord.Set(OrderFieldIDs.ROC.price, userOrder.price);
			ord.Set(OrderFieldIDs.ROC.qty, userOrder.qty);
			ord.Set(OrderFieldIDs.ROC.secType, userOrder.secType);
			ord.Set(OrderFieldIDs.ROC.side, userOrder.side);
			ord.Set(OrderFieldIDs.ROC.status, userOrder.status);
			ord.Set(OrderFieldIDs.ROC.stopPrice, userOrder.stopPrice);
			ord.Set(OrderFieldIDs.ROC.strikePrice, userOrder.strikePrice);
			ord.Set(OrderFieldIDs.ROC.symbol, userOrder.symbol);
			ord.Set(OrderFieldIDs.ROC.tag, userOrder.tag);
			ord.Set(OrderFieldIDs.ROC.text, userOrder.text);
			ord.Set(OrderFieldIDs.ROC.tif, userOrder.tif);
			ord.Set(OrderFieldIDs.ROC.tradeFor, userOrder.tradeFor);
			ord.Set(OrderFieldIDs.ROC.trader, userOrder.trader);
			ord.Set(OrderFieldIDs.ROC.underlying, userOrder.underlying);

			UpdateRocOrders(userOrder.omTag, ord);
		}

		public void UpdateRocOrders(string key, ROCOrder ord)
		{
			lock (RocOrders)
			{
				MakeRocStatus(ref ord);

				if (RocOrders.ContainsKey(key))
				{
					RocOrders[key] = ord;
				}
				else
				{
					RocOrders.Add(key, ord);
				}
			}
		}

		public void MakeRocStatus(ref ROCOrder ord)
		{
			if (ord.Status == CSVFieldIDs.StatusCodes.Canceled && ord.CumQty > 0) // Cancelled and had some filled already
			{
				ord.Status = CSVFieldIDs.StatusCodes.FilledAndCancelled;  //Filled & Cancelled
			}
			else if (ord.Status == CSVFieldIDs.StatusCodes.Replaced && ord.LeaveQty == 0) // Replaced to 0 LeaveQty
			{
				ord.Status = CSVFieldIDs.StatusCodes.ReplacedAndFilled;  //Replaced and Filled
			}
		}

		#endregion

		#region - User Executions -

		public void GetUserExecutions()
		{
			if (GotUserProfiles)
			{
				_rdsService.GetUserExecutionsAsync(_user, _pass);
			}
		}

		private void rdsService_GetUserExecutionsCompleted(object sender, GetUserExecutionsCompletedEventArgs e)
		{
			if (e.Cancelled)
			{
				ErrorMsg = String.Concat(new object[] { "GetUserExecutions|Cancelled", e.Error.Message, " ", e.Error.StackTrace });
			}
			else if (e.Error != null)
			{
				ErrorMsg = String.Concat(new object[] { "GetUserExecutions|Error ", e.Error.Message, " ", e.Error.StackTrace });
			}
			else
			{
				switch (e.Result.statusCode)
				{
					case 0:
						try
						{
							if (e.Result.retValues != null)
							{
								lock (_syncObj)
								{
									DecodeUserExecutionObjs((object[])e.Result.retValues);
								}
							}
							GotRocExecutions = true;
						}
						catch (Exception ex)
						{
							ErrorMsg = String.Concat(new object[] { "GetUserExecutions|Exception ", ex.Message, " : ", ex.StackTrace });
						}
						break;
					default:
						ErrorMsg = String.Concat(new object[] { "GetUserExecutions|Result|StatusCode ", e.Result.statusCode.ToString(), " : ", e.Result.statusMessage });
						break;
				}
			}
		}

		private void DecodeUserExecutionObjs(object[] userExecutionObjs)
		{
			foreach (object userExecutionObj in userExecutionObjs)
			{
				if (userExecutionObj != null)
				{
					DecodeUserExecution((Execution)userExecutionObj);
				}
			}
		}

		private void DecodeUserExecution(Execution userExecution)
		{
			string omTag = userExecution.omTag;
			string omExecTag = userExecution.omExecTag;

			if (omExecTag != "" && omTag != "" && RocOrders.ContainsKey(omTag))
			{
				ROCExecution traded;
				if (RocOrders[omTag].Trades.ContainsKey(omExecTag))
				{
					// trade again?
					traded = RocOrders[omTag].Trades[omExecTag];
					ErrorMsg = String.Concat(new object[] { "DecodeUserExecution|Duplication ", omExecTag });
				}
				else
				{
					traded = new ROCExecution();
					RocOrders[omTag].Trades.Add(omExecTag, traded);
				}

				traded.Set(TradedFieldIDs.ROC.omTag, userExecution.omTag);
				traded.Set(TradedFieldIDs.ROC.omExecTag, userExecution.omExecTag);
				traded.Set(TradedFieldIDs.ROC.execTime, userExecution.execTime.ToLocalTime().ToOADate());
				traded.Set(TradedFieldIDs.ROC.execQty, (long)userExecution.execQty);
				traded.Set(TradedFieldIDs.ROC.execPrice, userExecution.execPrice);

				// Attach some of the order info with the trade
				traded.Set(TradedFieldIDs.ROC.symbol, RocOrders[omTag].Symbol);
				traded.Set(TradedFieldIDs.ROC.underlying, RocOrders[omTag].Underlying);
				traded.Set(TradedFieldIDs.ROC.expDate, RocOrders[omTag].ExpDate);
				traded.Set(TradedFieldIDs.ROC.strikePrice, RocOrders[omTag].StrikePrice);
				traded.Set(TradedFieldIDs.ROC.callPut, RocOrders[omTag].CallPut);
				traded.Set(TradedFieldIDs.ROC.secType, RocOrders[omTag].SecType);
				traded.Set(TradedFieldIDs.ROC.trader, RocOrders[omTag].Trader);
				traded.Set(TradedFieldIDs.ROC.localAcct, RocOrders[omTag].LocalAcct);
				traded.Set(TradedFieldIDs.ROC.account, RocOrders[omTag].ClearingAcct);
				traded.Set(TradedFieldIDs.ROC.openClose, RocOrders[omTag].OpenClose);
				traded.Set(TradedFieldIDs.ROC.side, (long)RocOrders[omTag].Side);
				traded.Set(TradedFieldIDs.ROC.destID, RocOrders[omTag].DestID);

				RocOrders[omTag].Set(OrderFieldIDs.hasTrades, true);
				RocOrders[omTag].Trades[omExecTag] = traded;

				if (RocExecutions.ContainsKey(omExecTag))
				{
					// Traded Twice?
					RocExecutions[omExecTag] = traded;
				}
				else
				{
					RocExecutions.Add(omExecTag, traded);
				}
			}
		}

		#endregion

		#region - TPOS Positions -

		public void GetTPosPositions()
		{
			if (GotUserProfiles)
			{
				_gotTposPositionsRefresh = false;
				_rdsService.GetTposPositionsBylocalAcctAsync(_user, "");
			}
		}

		private void rdsService_GetTposPositionsBylocalAcctCompleted(object sender, GetTposPositionsBylocalAcctCompletedEventArgs e)
		{
			if (e.Cancelled)
			{
				ErrorMsg = String.Concat(new object[] { "GetTposPosition|Cancelled", e.Error.Message, " ", e.Error.StackTrace });
			}
			else if (e.Error != null)
			{
				ErrorMsg = String.Concat(new object[] { "GetTposPosition|Error ", e.Error.Message, " ", e.Error.StackTrace });
			}
			else
			{
				switch (e.Result.statusCode)
				{
					case 0:
						if (e.Result.retValues != null)
						{
							DecodeTposPositionObj((object[])e.Result.retValues);
						}
						if (_firstTposPositionCall)
						{
							_firstTposPositionCall = false;
							GotTposPositions = true;
						}
						else
						{
							GotTposPositionsRefresh = true;
						}

						e.Result.retValues = null;
						break;
					default:
						ErrorMsg = String.Concat(new object[] { "GetTposPosition|Result|StatusCode ", e.Result.statusCode.ToString(), " : ", e.Result.statusMessage });
						break;
				}
			}
		}

		private void DecodeTposPositionObj(object[] tposPositionObjs)
		{
			foreach (object tposPositionObj in tposPositionObjs)
			{
				if (tposPositionObj != null)
				{
					DecodeTposPosition((TposPosition)tposPositionObj);
				}
			}
		}

		private void DecodeTposPosition(TposPosition pos)
		{
			try
			{
				if (VarifyTposPosition(pos))
				{
					TPOSPosition position = new TPOSPosition();
					position.Set(PositionFieldIDs.TPOS.isTPOS, true);
					position.Set(PositionFieldIDs.TPOS.asOfDate, pos.m_AsOfDate);
					position.Set(PositionFieldIDs.TPOS.avgCost, pos.m_AvgCost);
					position.Set(PositionFieldIDs.TPOS.clearingAccount, pos.m_ClearingAccount);
					position.Set(PositionFieldIDs.TPOS.curQty, pos.m_CurQty);
					position.Set(PositionFieldIDs.TPOS.displayFormat, pos.m_DisplayFormat);
					position.Set(PositionFieldIDs.TPOS.expDate, pos.m_ExpDate);
					position.Set(PositionFieldIDs.TPOS.instrumentID, pos.m_InstrumentID);
					position.Set(PositionFieldIDs.TPOS.multiplier, pos.m_Multiplier);
					position.Set(PositionFieldIDs.TPOS.notionalAmount, pos.m_NotionalAmount);
					position.Set(PositionFieldIDs.TPOS.openMark, pos.m_OpenMark);
					position.Set(PositionFieldIDs.TPOS.openQty, pos.m_OpenQty);
					position.Set(PositionFieldIDs.TPOS.portfolio, pos.m_Portfolio);
					position.Set(PositionFieldIDs.TPOS.putCall, pos.m_PutCall);
					position.Set(PositionFieldIDs.TPOS.realizedPnL, pos.m_RealizedPnL);
					position.Set(PositionFieldIDs.TPOS.secType, pos.m_SecType);
					position.Set(PositionFieldIDs.TPOS.strike, pos.m_Strike.ToString());
					position.Set(PositionFieldIDs.TPOS.symbol, pos.m_Symbol);
					position.Set(PositionFieldIDs.TPOS.tradeGroup, pos.m_TradeGroup);
					position.Set(PositionFieldIDs.TPOS.traderAcronym, pos.m_TraderAcronym);
					//position.Update(PositionFieldIDs.TPOS.traderAcronym, pos.m_Portfolio);
					position.Set(PositionFieldIDs.TPOS.undSecType, pos.m_UndSecType);
					if (pos.m_UndSecType == CSVFieldIDs.SecurityTypes.Equity)
					{
						position.Set(PositionFieldIDs.TPOS.undSymbol, pos.m_Symbol);
					}
					else
					{
						position.Set(PositionFieldIDs.TPOS.undSymbol, pos.m_UndSymbol);
					}
					position.Set(PositionFieldIDs.TPOS.validTraderKey, pos.m_ValidTraderKey);
					position.Set(PositionFieldIDs.TPOS.version, pos.m_Version);

					lock (TposPositions)
					{
						if (TposPositions.ContainsKey(position.PositionKey))
						{
							if (TposPositions[position.PositionKey].Version != pos.m_Version)
							{
								// new version of the position
								TposPositions[position.PositionKey] = position;
								AddToNewPositions(position);
							}
						}
						else
						{
							TposPositions.Add(position.PositionKey, position);
							AddToNewPositions(position);
						}
					}
				}
			}
			catch (Exception ex)
			{
				ErrorMsg = String.Concat(new object[] { "DecodeTposPosition|Error ", ex.Message, " : ", ex.StackTrace });
			}
		}

		private bool VarifyTposPosition(TposPosition pos)
		{
			return AccountFilterMap.Contains(pos.m_ClearingAccount);
		}

		private void AddToNewPositions(TPOSPosition position)
		{
			if (!_firstTposPositionCall)
			{
				lock (NewTposPositions)
				{
					if (NewTposPositions.ContainsKey(position.PositionKey))
					{
						NewTposPositions[position.PositionKey] = position;
					}
					else
					{
						NewTposPositions.Add(position.PositionKey, position);
					}
				}
			}
		}

		#endregion

		#region - TPOS Executions -

		public void GetTPosExecutions()
		{
			if (GotUserProfiles)
			{
				_gotTposExecutionsRefresh = false;
				_rdsService.RtrvTposTradesByTraderAsync(_user, "", true);
			}
		}

		private void rdsService_GetTposTradesByTraderCompleted(object sender, RtrvTposTradesByTraderCompletedEventArgs e)
		{
			if (e.Cancelled)
			{
				ErrorMsg = String.Concat(new object[] { "GetTposTrades|Cancelled", e.Error.Message, " ", e.Error.StackTrace });
			}
			else if (e.Error != null)
			{
				ErrorMsg = String.Concat(new object[] { "GetTposTrades|Error ", e.Error.Message, " ", e.Error.StackTrace });
			}
			else
			{
				switch (e.Result.statusCode)
				{
					case 0:
						if (e.Result.retValues != null)
						{
							DecodeTposTradeObj((object[])e.Result.retValues);
						}
						if (_firstTposExecutionCall)
						{
							_firstTposExecutionCall = false;
							GotTposExecutions = true;
						}
						else
						{
							GotTposExecutionsRefresh = true;
						}

						e.Result.retValues = null;
						break;
					default:
						ErrorMsg = String.Concat(new object[] { "GetTposTrades|Result|StatusCode ", e.Result.statusCode.ToString(), " : ", e.Result.statusMessage });
						break;
				}
			}
		}

		private void DecodeTposTradeObj(object[] tposTradeObjs)
		{
			foreach (object tposTradeObj in tposTradeObjs)
			{
				if (tposTradeObj != null)
				{
					DecodeTposTrade((TposTrade)tposTradeObj);
				}
			}
		}

		private void DecodeTposTrade(TposTrade tposTrade)
		{
			try
			{
				if (VarifyTposTrade(tposTrade))
				{
					TPOSExecution exec = new TPOSExecution();
					exec.Set(TradedFieldIDs.TPOS.isTPOS, true);
					lock (TposExecutions)
					{
						if (TposExecutions.ContainsKey(tposTrade.m_TradeID))
						{
							if (TposExecutions[tposTrade.m_TradeID].Version != tposTrade.m_Version ||
								TposExecutions[tposTrade.m_TradeID].ModReasonID != tposTrade.m_LastModReasonID)
							{
								exec = TposExecutions[tposTrade.m_TradeID];
							}
							else
							{
								// Same trade, exit.
								return;
							}
						}
						else
						{
							TposExecutions.Add(tposTrade.m_TradeID, exec);
						}
					}

					exec.Set(TradedFieldIDs.TPOS.activeState, tposTrade.m_ActiveState);
					exec.Set(TradedFieldIDs.TPOS.clearingAccount, tposTrade.m_ClearingAccount);
					exec.Set(TradedFieldIDs.TPOS.clearingStatus, tposTrade.m_ClearingStatus);
					exec.Set(TradedFieldIDs.TPOS.commission, tposTrade.m_Commission);
					exec.Set(TradedFieldIDs.TPOS.contraBroker, tposTrade.m_ContraBroker);
					exec.Set(TradedFieldIDs.TPOS.contraFirm, tposTrade.m_ContraFirm);
					exec.Set(TradedFieldIDs.TPOS.exchange, tposTrade.m_Exchange);
					exec.Set(TradedFieldIDs.TPOS.expDate, tposTrade.m_ExpDate);
					exec.Set(TradedFieldIDs.TPOS.extDescription, tposTrade.m_ExtDescription);
					exec.Set(TradedFieldIDs.TPOS.extTradeID, tposTrade.m_ExtTradeID);
					exec.Set(TradedFieldIDs.TPOS.lastModDate, tposTrade.m_LastModDate);
					exec.Set(TradedFieldIDs.TPOS.lastModeReason, tposTrade.m_LastModReason);
					if (tposTrade.m_LastModReason.Contains("Deleted"))
					{
						exec.Set(TradedFieldIDs.TPOS.lastModReasonID, 3);
					}
					else
					{
						exec.Set(TradedFieldIDs.TPOS.lastModReasonID, tposTrade.m_LastModReasonID);
					}
					exec.Set(TradedFieldIDs.TPOS.lastModTime, tposTrade.m_LastModTime);
					exec.Set(TradedFieldIDs.TPOS.note, tposTrade.m_Note);
					exec.Set(TradedFieldIDs.TPOS.portfolio, tposTrade.m_Portfolio);
					exec.Set(TradedFieldIDs.TPOS.putCall, tposTrade.m_PutCall);
					if (exec.ModReasonID == 3)
					{
						exec.Set(TradedFieldIDs.TPOS.qty, 0);
						exec.Set(TradedFieldIDs.TPOS.price, 0);
					}
					else
					{
						exec.Set(TradedFieldIDs.TPOS.qty, tposTrade.m_Qty);
						exec.Set(TradedFieldIDs.TPOS.price, tposTrade.m_Price);
					}
					exec.Set(TradedFieldIDs.TPOS.secType, tposTrade.m_SecType);
					exec.Set(TradedFieldIDs.TPOS.settleDate, tposTrade.m_SettleDate);
					exec.Set(TradedFieldIDs.TPOS.shortSaleFlag, tposTrade.m_ShortSaleFlag);
					exec.Set(TradedFieldIDs.TPOS.strike, tposTrade.m_Strike);
					exec.Set(TradedFieldIDs.TPOS.symbol, tposTrade.m_Symbol);
					exec.Set(TradedFieldIDs.TPOS.ticker, tposTrade.m_Ticker);
					exec.Set(TradedFieldIDs.TPOS.tradeDate, tposTrade.m_TradeDate);
					exec.Set(TradedFieldIDs.TPOS.tradeGroup, tposTrade.m_TradeGroup);
					exec.Set(TradedFieldIDs.TPOS.tradeID, tposTrade.m_TradeID);
					exec.Set(TradedFieldIDs.TPOS.traderAcronym, tposTrade.m_TraderAcronym);
					//exec.Update(TradedFieldIDs.TPOS.traderAcronym, tposTrade.m_Portfolio);
					exec.Set(TradedFieldIDs.TPOS.tradeSource, tposTrade.m_TradeSource);
					exec.Set(TradedFieldIDs.TPOS.tradeTime, tposTrade.m_TradeTime);
					exec.Set(TradedFieldIDs.TPOS.undSecType, tposTrade.m_UndSecType);
					exec.Set(TradedFieldIDs.TPOS.undSymbol, tposTrade.m_UndSymbol);
					exec.Set(TradedFieldIDs.TPOS.version, tposTrade.m_Version);

					if (exec.SymbolDetail == "")
					{
						// Force a Symbol Reload
						exec.Set(TradedFieldIDs.symbolDetail, exec.SymbolDetail);
					}

					lock (TposExecutions)
					{
						if (TposExecutions.ContainsKey(exec.TradeID))
						{
							TposExecutions[exec.TradeID] = exec;
							AddToNewExecutions(exec);
						}
					}
				}
			}
			catch (Exception ex)
			{
				ErrorMsg = String.Concat(new object[] { "DecodeTposTrades|Error ", ex.Message, " : ", ex.StackTrace });
			}
		}

		private void AddToNewExecutions(TPOSExecution exec)
		{
			if (!_firstTposExecutionCall)
			{
				lock (NewTposExecutions)
				{
					if (NewTposExecutions.ContainsKey(exec.TradeID))
					{
						NewTposExecutions[exec.TradeID] = exec;
					}
					else
					{
						NewTposExecutions.Add(exec.TradeID, exec);
					}
				}
			}
		}

		private bool VarifyTposTrade(TposTrade tposTrade)
		{
			return AccountFilterMap.Contains(tposTrade.m_ClearingAccount);
		}

		#endregion

		#region - Get Security By Ticker -

		public void GetSecurityByTicker(string rocSymbol)
		{
			GetSecurityByTicker(rocSymbol, false);
		}
		public void GetSecurityByTicker(string rocSymbol, bool reload)
		{
			try
			{
				if (reload)
				{
					AddToSecurityInfoWaitingList(rocSymbol, "");
				}

				if (!SymbolSecurityInfos.ContainsKey(rocSymbol))
				{
					AddToSecurityInfoWaitingList(rocSymbol, "");
				}
				else
				{
					OnSecurityReceived(new SecurityEventArgs(SymbolSecurityInfos[rocSymbol]));
				}
			}
			catch (Exception ex)
			{
				ErrorMsg = String.Concat(new object[] { "GetSecurityByTicker|Error ", ex.Message, " : ", ex.StackTrace });
			}
		}

		private void rdsService_GetSecurityByTickerCompleted(object sender, GetSecurityByTickerCompletedEventArgs e)
		{
			string rocSymbol = (string)e.UserState;
			bool gotSecInfo = false;

			if (e.Cancelled)
			{
				ErrorMsg = String.Concat(new object[] { "GetSecurityByTicker|Cancelled", e.Error.Message, " ", e.Error.StackTrace });
			}
			else if (e.Error != null)
			{
				ErrorMsg = String.Concat(new object[] { "GetSecurityByTicker|Error ", e.Error.Message, " ", e.Error.StackTrace });
			}
			else
			{
				switch (e.Result.statusCode)
				{
					case 0:
						gotSecInfo = DecodeSecurityDescObj((object[])e.Result.retValues, rocSymbol);
						e.Result.retValues = null;
						break;
					default:
						ErrorMsg = String.Concat(new object[] { "GetSecurityByTicker|Result|StatusCode ", e.Result.statusCode.ToString(), " : ", e.Result.statusMessage });
						break;
				}
			}

			lock (ImSecurityInfoWaitingList)
			{
				RemoveFromSecurityInfoWaitingList(rocSymbol);

				if (ImOPTChainInfoWaitingList.Contains(rocSymbol))
				{
					GetOptionChainInfo(rocSymbol);

					if (gotSecInfo && SymbolSecurityInfos.ContainsKey(rocSymbol))
					{
						OnSecurityReceived(new SecurityEventArgs(SymbolSecurityInfos[rocSymbol]));
					}
					return;
				}
				else if (ImSFFChainInfoWaitingList.Contains(rocSymbol))
				{
					GetSSFutureChainInfo(rocSymbol);

					if (gotSecInfo && SymbolSecurityInfos.ContainsKey(rocSymbol))
					{
						OnSecurityReceived(new SecurityEventArgs(SymbolSecurityInfos[rocSymbol]));
					}
					return;
				}

				// Done
				if (gotSecInfo && SymbolSecurityInfos.ContainsKey(rocSymbol))
				{
					OnSecurityReceived(new SecurityEventArgs(SymbolSecurityInfos[rocSymbol]));
				}
				
				// Get Next Symbol
				if (GotNextSymbol())
				{
					return;
				}
			}

			_imBusy = false;
		}

		private bool DecodeSecurityDescObj(object[] securityDescObjs, string rocSymbol)
		{
			bool gotSecInfo = false;

			foreach (object securityDescObj in securityDescObjs)
			{
				if (securityDescObj != null)
				{
					DecodeSecurityDesc((SecurityDesc)securityDescObj, rocSymbol, securityDescObjs.Length);
					gotSecInfo = true;
				}
			}

			return gotSecInfo;
		}

		private void DecodeSecurityDesc(SecurityDesc sec, string rocSymbol, int count)
		{
			if (sec != null)
			{
				switch (sec.securityType)
				{
					case CSVFieldIDs.SecurityTypes.Option:
						break;
					case CSVFieldIDs.SecurityTypes.OptionIndex:
						if (count == 1 || rocSymbol.Substring(0, 1) == "^")
						{
							SubDecodeSecurityDesc(sec, rocSymbol);
						}
						break;
					default:
						SubDecodeSecurityDesc(sec, rocSymbol);
						break;
				}
			}
		}

		private void SubDecodeSecurityDesc(SecurityDesc sec, string rocSymbol)
		{
			IMSecurityInfo info = new IMSecurityInfo();

			info.Set(SecurityFieldIDs.Security.rocSymbol, rocSymbol);
			info.Set(SecurityFieldIDs.Security.baseSymbol, sec.baseSymbol);
			info.Set(SecurityFieldIDs.Security.contractSize, sec.contractSize);
			//info.Update(SecurityFieldIDs.Security.conversion, sec.Conversion);
			//info.Update(SecurityFieldIDs.Security.coupon, sec.Coupon);
			//info.Update(SecurityFieldIDs.Security.couponFreq, sec.CouponFreq);
			//info.Update(SecurityFieldIDs.Security.curCode, sec.CurCode);
			//info.Update(SecurityFieldIDs.Security.cusip, sec.Cusip);
			info.Set(SecurityFieldIDs.Security.dataSourceInfo, sec.dataSourceInfo);
			info.Set(SecurityFieldIDs.Security.decimalPrecision, sec.DecimalPrecision);
			info.Set(SecurityFieldIDs.Security.displayFormat, sec.DisplayFormat);
			info.Set(SecurityFieldIDs.Security.exchange, sec.exchange);
			//info.Update(SecurityFieldIDs.Security.exerType, sec.ExerType);
			info.Set(SecurityFieldIDs.Security.expDay, sec.ExpDay);
			info.Set(SecurityFieldIDs.Security.expirationDate, sec.expirationDate);
			info.Set(SecurityFieldIDs.Security.expMonth, sec.ExpMonth);
			info.Set(SecurityFieldIDs.Security.expYear, sec.ExpYear);
			//info.Update(SecurityFieldIDs.Security.firstCouponDate, sec.FirstCouponDate);
			info.Set(SecurityFieldIDs.Security.fullSymbol, sec.fullSymbol);
			info.Set(SecurityFieldIDs.Security.genericMDSymbol, sec.genericMDSymbol);
			//info.Update(SecurityFieldIDs.Security.inrstAccuralDate, sec.InrstAccuralDate);
			//info.Update(SecurityFieldIDs.Security.instrumentId, sec.InstrumentId);
			//info.Update(SecurityFieldIDs.Security.issueDate, sec.IssueDate);
			info.Set(SecurityFieldIDs.Security.longName, sec.longName);
			//info.Update(SecurityFieldIDs.Security.notionalAmount, sec.NotionalAmount);
			//info.Update(SecurityFieldIDs.Security.obsoleteDateTime, sec.ObsoleteDateTime);
			info.Set(SecurityFieldIDs.Security.putCall, sec.PutCall);
			//info.Update(SecurityFieldIDs.Security.redemption, sec.Redemption);
			if (sec.genericMDSymbol.Contains(".IDX"))
			{
				// This from IM means it is an option index
				info.Set(SecurityFieldIDs.Security.securityType, CSVEx.CSVFieldIDs.SecurityTypes.OptionIndex);
			}
			else
			{
				info.Set(SecurityFieldIDs.Security.securityType, sec.securityType);
			}
			//info.Update(SecurityFieldIDs.Security.spcStlFlag, sec.SpcStlFlag);
			info.Set(SecurityFieldIDs.Security.strike, sec.Strike);
			info.Set(SecurityFieldIDs.Security.ticker, sec.Ticker);
			if (sec.tickSize != 0)
			{
				info.Set(SecurityFieldIDs.Security.tickSize, sec.tickSize);
			}
			info.Set(SecurityFieldIDs.Security.undExpirationDate, sec.UndExpirationDate);
			info.Set(SecurityFieldIDs.Security.undInstrumentId, sec.UndInstrumentId);

			lock (SymbolSecurityInfos)
			{
				if (SymbolSecurityInfos.ContainsKey(rocSymbol))
				{
					SymbolSecurityInfos[rocSymbol] = info;
				}
				else
				{
					SymbolSecurityInfos.Add(rocSymbol, info);
				}
			}

			AddToSymbolDetailToRocSymbolMap(rocSymbol, rocSymbol);
		}

		#endregion

		#region - Get Option Chain -

		public void GetOptionChain(string rocSymbol)
		{
			GetOptionChain(rocSymbol, false);
		}
		public void GetOptionChain(string rocSymbol, bool reload)
		{
			if (reload)
			{
				AddToSecurityInfoWaitingList(rocSymbol, CSVFieldIDs.SecurityTypes.Option);
			}
			else if (!SymbolSecurityInfos.ContainsKey(rocSymbol) || SymbolSecurityInfos[rocSymbol].OptionChain.Count == 0)
			{
				AddToSecurityInfoWaitingList(rocSymbol, CSVFieldIDs.SecurityTypes.Option);
			}
			else
			{
				OnSecurityReceived(new SecurityEventArgs(SymbolSecurityInfos[rocSymbol]));
			}
		}

		private void rdsService_GetOptionChainCompleted(object sender, GetOptionChainCompletedEventArgs e)
		{
			string rocSymbol = (string)e.UserState;
			bool gotSecInfo = false;

			if (e.Cancelled)
			{
				ErrorMsg = String.Concat(new object[] { "GetOptionChain|Cancelled", e.Error.Message, " ", e.Error.StackTrace });
			}
			else if (e.Error != null)
			{
				ErrorMsg = String.Concat(new object[] { "GetOptionChain|Error ", e.Error.Message, " ", e.Error.StackTrace });
			}
			else
			{
				switch (e.Result.statusCode)
				{
					case 0:
						gotSecInfo = DecodeOptionChainObj((object[])e.Result.retValues, rocSymbol);
						e.Result.retValues = null;
						break;
					default:
						ErrorMsg = String.Concat(new object[] { "GetOptionChain|Result|StatusCode ", e.Result.statusCode.ToString(), " : ", e.Result.statusMessage });
						break;
				}
			}

			lock (ImSecurityInfoWaitingList)
			{
				RemoveFromOPTChainInfoWaitingList(rocSymbol);

				if (ImSFFChainInfoWaitingList.Contains(rocSymbol))
				{
					GetSSFutureChainInfo(rocSymbol);

					if (gotSecInfo && SymbolSecurityInfos.ContainsKey(rocSymbol))
					{
						OnOptionChainReceived(new OptionChainEventArgs(SymbolSecurityInfos[rocSymbol]));
					}
					return;
				}

				// Done
				if (gotSecInfo && SymbolSecurityInfos.ContainsKey(rocSymbol))
				{
					OnOptionChainReceived(new OptionChainEventArgs(SymbolSecurityInfos[rocSymbol]));
				}

				// Get Next Symbol
				if (GotNextSymbol())
				{
					return;
				}				
			}

			_imBusy = false;
		}

		private bool DecodeOptionChainObj(object[] optionChainObjs, string rocSymbol)
		{
			bool gotSecInfo = false;

			if (optionChainObjs.Length > 0)
			{
				foreach (object optionChainObj in optionChainObjs)
				{
					if (optionChainObj != null)
					{
						DecodeOption((OptionDesc)optionChainObj, rocSymbol);
						gotSecInfo = true;
					}
				}

				if (gotSecInfo && SymbolSecurityInfos.ContainsKey(rocSymbol))
				{
					OnSecurityReceived(new SecurityEventArgs(SymbolSecurityInfos[rocSymbol]));
				}
			}

			return gotSecInfo;
		}

		private void DecodeOption(OptionDesc option, string rocSymbol)
		{
			IMOptionInfo info = new IMOptionInfo();

			info.Set(SecurityFieldIDs.Option.stockSymbol, option.stockSymbol);
			info.Set(SecurityFieldIDs.Option.optionSymbol, option.optionSymbol);
			info.Set(SecurityFieldIDs.Option.opraSymbol, option.opraSymbol);
			//info.Update(SecurityFieldIDs.Option.exchange, option.exchange);
			info.Set(SecurityFieldIDs.Option.dataSource, option.exchange);
			info.Set(SecurityFieldIDs.Option.strikPrice, option.strike);
			info.Set(SecurityFieldIDs.Option.expirationDate, option.expirationDate);
			info.Set(SecurityFieldIDs.Option.contractSize, option.contractSize);
			info.Set(SecurityFieldIDs.Option.putCall, option.putCall);
			//info.Update(SecurityFieldIDs.Option.spcStlFlag, option.spcStlFlag);
			if (option.tickSize != 0)
			{
				info.Set(SecurityFieldIDs.Option.tickSize, option.tickSize);
			}
			//info.Update(SecurityFieldIDs.Option.curCode, option.curCode);
			//info.Update(SecurityFieldIDs.Option.exerType, option.exerType);
			//info.Update(SecurityFieldIDs.Option.instrumentId, option.instrumentId);
			//info.Update(SecurityFieldIDs.Option.undInstrumentId, option.undInstrumentId);
			info.Set(SecurityFieldIDs.Option.longName, option.longName);
			info.Set(SecurityFieldIDs.Option.genericMDSymbol, option.genericMDSymbol);
			//info.Update(SecurityFieldIDs.Option.obsoleteDateTime, option.obsoleteDateTime);
			info.Set(SecurityFieldIDs.Option.undExpirationDate, option.undExpirationDate);
			info.Set(SecurityFieldIDs.Option.roninSymbol, option.roninSymbol);
			info.Set(SecurityFieldIDs.Option.underlying, rocSymbol);

			lock (SymbolSecurityInfos)
			{
				if (SymbolSecurityInfos.ContainsKey(rocSymbol))
				{
					info.MDSource = SymbolSecurityInfos[rocSymbol].MDSource;
					if (SymbolSecurityInfos[rocSymbol].SecType == CSVFieldIDs.SecurityTypes.Future)
					{
						info.SecType = CSVFieldIDs.SecurityTypes.OptionFuture;
						info.TickSize = SymbolSecurityInfos[rocSymbol].TickSize;
					}
					else
					{
						info.SecType = CSVFieldIDs.SecurityTypes.Option;
					}

					if (SymbolSecurityInfos[rocSymbol].OptionChain.ContainsKey(info.MDSymbol))
					{
						SymbolSecurityInfos[rocSymbol].OptionChain[info.MDSymbol] = info;
					}
					else
					{
						SymbolSecurityInfos[rocSymbol].OptionChain.Add(info.MDSymbol, info);
					}
				}
				else
				{
					Debug.Assert(!Debugger.IsAttached, "Missing Symbol For Option Chain");
				}
			}

			lock (SymbolToOptionInfos)
			{
				if (SymbolToOptionInfos.ContainsKey(info.MDSymbol))
				{
					SymbolToOptionInfos[info.MDSymbol] = info;
				}
				else
				{
					SymbolToOptionInfos.Add(info.MDSymbol, info);
				}
			}

			AddToSymbolDetailToRocSymbolMap(info.SymbolDetail, info.MDSymbol);
		}

		#endregion

		#region - Get SSFuture Chain -

		public void GetSSFutureChain(string rocSymbol)
		{
			GetSSFutureChain(rocSymbol, false);
		}
		public void GetSSFutureChain(string rocSymbol, bool reload)
		{
			if (reload)
			{
				AddToSecurityInfoWaitingList(rocSymbol, CSVFieldIDs.SecurityTypes.SingleStockFuture);
			}
			else if (!SymbolSecurityInfos.ContainsKey(rocSymbol) || SymbolSecurityInfos[rocSymbol].SSFutureChain.Count == 0)
			{
				AddToSecurityInfoWaitingList(rocSymbol, CSVFieldIDs.SecurityTypes.SingleStockFuture);
			}
			else
			{
				OnSecurityReceived(new SecurityEventArgs(SymbolSecurityInfos[rocSymbol]));
			}
		}

		private void rdsService_GetSSFutureChainCompleted(object sender, GetSSFutureChainCompletedEventArgs e)
		{
			string rocSymbol = (string)e.UserState;
			bool gotSecInfo = false;

			if (e.Cancelled)
			{
				ErrorMsg = String.Concat(new object[] { "GetSSFutureChain|Cancelled", e.Error.Message, " ", e.Error.StackTrace });
			}
			else if (e.Error != null)
			{
				ErrorMsg = String.Concat(new object[] { "GetSSFutureChain|Error ", e.Error.Message, " ", e.Error.StackTrace });
			}
			else
			{
				switch (e.Result.statusCode)
				{
					case 0:
						gotSecInfo = DecodeSSFutureChainObj((object[])e.Result.retValues, rocSymbol);
						e.Result.retValues = null;
						break;
					default:
						ErrorMsg = String.Concat(new object[] { "GetSSFutureChain|Result|StatusCode ", e.Result.statusCode.ToString(), " : ", e.Result.statusMessage });
						break;
				}
			}

			lock (ImSecurityInfoWaitingList)
			{
				RemoveFromSFFChainInfoWaitingList(rocSymbol);

				if (ImOPTChainInfoWaitingList.Contains(rocSymbol))
				{
					GetOptionChainInfo(rocSymbol);

					if (gotSecInfo && SymbolSecurityInfos.ContainsKey(rocSymbol))
					{
						OnSSFutureChainReceived(new SSFutureChainEventArgs(SymbolSecurityInfos[rocSymbol]));
					}
					return;
				}

				// Done
				if (gotSecInfo && SymbolSecurityInfos.ContainsKey(rocSymbol))
				{
					OnSSFutureChainReceived(new SSFutureChainEventArgs(SymbolSecurityInfos[rocSymbol]));
				}

				// Get Next Symbol
				if (GotNextSymbol())
				{
					return;
				}
			}

			_imBusy = false;
		}

		private bool DecodeSSFutureChainObj(object[] ssfutureChainObjs, string rocSymbol)
		{
			bool gotSecInfo = false;

			if (ssfutureChainObjs.Length > 0)
			{
				foreach (object ssfutureChainObj in ssfutureChainObjs)
				{
					if (ssfutureChainObj != null)
					{
						DecodeSSFuture((FutureDesc)ssfutureChainObj, rocSymbol);
						gotSecInfo = true;
					}
				}

				if (gotSecInfo && SymbolSecurityInfos.ContainsKey(rocSymbol))
				{
					OnSecurityReceived(new SecurityEventArgs(SymbolSecurityInfos[rocSymbol]));
				}
			}

			return gotSecInfo;
		}

		private void DecodeSSFuture(FutureDesc ssfuture, string rocSymbol)
		{
			if (ssfuture != null)
			{
				IMSSFutureInfo info = new IMSSFutureInfo();

				info.Set(SecurityFieldIDs.SSFuture.fullSymbol, ssfuture.fullSymbol);
				//info.Update(SecurityFieldIDs.SSFuture.exchange, ssfuture.exchange);
				info.Set(SecurityFieldIDs.SSFuture.dataSource, ssfuture.exchange);
				info.Set(SecurityFieldIDs.SSFuture.expirationDate, ssfuture.expirationDate);
				info.Set(SecurityFieldIDs.SSFuture.contractSize, ssfuture.contractSize);
				info.Set(SecurityFieldIDs.SSFuture.tickSize, ssfuture.tickSize);
				info.Set(SecurityFieldIDs.SSFuture.curCode, ssfuture.curCode);
				info.Set(SecurityFieldIDs.SSFuture.longName, ssfuture.longName);
			
				lock (SymbolSecurityInfos)
				{
					if (SymbolSecurityInfos.ContainsKey(rocSymbol))
					{
						if (SymbolSecurityInfos[rocSymbol].SSFutureChain.ContainsKey(info.MDSymbol))
						{
							SymbolSecurityInfos[rocSymbol].SSFutureChain[info.MDSymbol] = info;
						}
						else
						{
							SymbolSecurityInfos[rocSymbol].SSFutureChain.Add(info.MDSymbol, info);
						}
					}
					else
					{
						Debug.Assert(!Debugger.IsAttached, "Missing Symbol For SSFuture Chain");
					}
				}

				lock (SymbolToSSFInfos)
				{
					if (SymbolToSSFInfos.ContainsKey(ssfuture.fullSymbol))
					{
						SymbolToSSFInfos[ssfuture.fullSymbol] = info;
					}
					else
					{
						SymbolToSSFInfos.Add(ssfuture.fullSymbol, info);
					}
				}

				AddToSymbolDetailToRocSymbolMap(ssfuture.fullSymbol, rocSymbol);
			}
		}

		#endregion

		#region - Set User Password -

		public void UpdateUserPassword(string username, string oldpassword, string newpassword)
		{
			try
			{
				_rdsService.SetUserPassword(username, oldpassword, newpassword);
			}
			catch (Exception ex)
			{
				ErrorMsg = String.Concat(new object[] { "UpdateUserPassword|Error ", ex.Message, " : ", ex.StackTrace });
			}
		}

		#endregion

		public void ResetSecurityInfo()
		{
			try
			{
				SymbolSecurityInfos.Clear();
				SymbolToOptionInfos.Clear();
				SymbolToSSFInfos.Clear();
				SymbolDetailToRocSymbolMap.Clear();
				ImSecurityInfoWaitingList.Clear();
			}
			catch (Exception ex)
			{
				ErrorMsg = String.Concat(new object[] { "ResetSecurityInfo|Error ", ex.Message, " : ", ex.StackTrace });
			}
		}

		#region - Shared IM Functions -

		private bool GotNextSymbol()
		{
			foreach (string nextSymbol in ImSecurityInfoWaitingList)
			{
				GetSecurityInfo(nextSymbol);
				return true;
			}

			foreach (string nextSymbol in ImOPTChainInfoWaitingList)
			{
				GetOptionChain(nextSymbol);
				return true;
			}

			foreach (string nextSymbol in ImSFFChainInfoWaitingList)
			{
				GetSSFutureChain(nextSymbol);
				return true;
			}

			return false;
		}

		private void AddToSecurityInfoWaitingList(string rocSymbol, string secType)
		{
			lock (ImSecurityInfoWaitingList)
			{
				if (!ImSecurityInfoWaitingList.Contains(rocSymbol))
				{
					ImSecurityInfoWaitingList.Add(rocSymbol);
				}
			}

			switch (secType)
			{
				case CSVFieldIDs.SecurityTypes.SingleStockFuture:
					lock (ImSFFChainInfoWaitingList)
					{
						if (!ImSFFChainInfoWaitingList.Contains(rocSymbol))
						{
							ImSFFChainInfoWaitingList.Add(rocSymbol);
						}
					}
					break;
				case CSVFieldIDs.SecurityTypes.Option:
					lock (ImSFFChainInfoWaitingList)
					{
						if (!ImOPTChainInfoWaitingList.Contains(rocSymbol))
						{
							ImOPTChainInfoWaitingList.Add(rocSymbol);
						}
					}
					break;
			}

			if (!_imBusy)
			{
				if (GotNextSymbol())
				{
					_imBusy = true;
				}
			}
		}

		private void RemoveFromSecurityInfoWaitingList(string rocSymbol)
		{
			lock (ImSecurityInfoWaitingList)
			{
				if (ImSecurityInfoWaitingList.Contains(rocSymbol))
				{
					ImSecurityInfoWaitingList.Remove(rocSymbol);
				}
			}
		}

		private void RemoveFromOPTChainInfoWaitingList(string rocSymbol)
		{
			lock (ImOPTChainInfoWaitingList)
			{
				if (ImOPTChainInfoWaitingList.Contains(rocSymbol))
				{
					ImOPTChainInfoWaitingList.Remove(rocSymbol);
				}
			}
		}

		private void RemoveFromSFFChainInfoWaitingList(string rocSymbol)
		{
			lock (ImSFFChainInfoWaitingList)
			{
				if (ImSFFChainInfoWaitingList.Contains(rocSymbol))
				{
					ImSFFChainInfoWaitingList.Remove(rocSymbol);
				}
			}
		}

		// Used by Sercrity Info Only
		private string ConvertToIMSymbol(string rocSymbol)
		{
			string _tposSymbol = rocSymbol;

			if (_tposSymbol.Contains("/PR"))
			{
				_tposSymbol = _tposSymbol.Replace("/PR", "/P");
			}
			//else if (_tposSymbol.Contains("/RT"))
			//{
			//    _tposSymbol = _tposSymbol.Replace("/RT", "/RT");
			//}
			//else if (_tposSymbol.Contains("/RTWI"))
			//{
			//    _tposSymbol = _tposSymbol.Replace("/RTWI", "/RTWI");
			//}

			return _tposSymbol;
		}

		private void AddToSymbolDetailToRocSymbolMap(string symbolDetail, string rocSymbol)
		{
			lock (SymbolDetailToRocSymbolMap)
			{
				if (!SymbolDetailToRocSymbolMap.ContainsKey(symbolDetail))
				{
					SymbolDetailToRocSymbolMap.Add(symbolDetail, rocSymbol);
				}
				else
				{
					SymbolDetailToRocSymbolMap[symbolDetail] = rocSymbol;
				}
			}
		}

		private void GetSecurityInfo(string rocSymbol)
		{
			_rdsService.GetSecurityByTickerAsync(ConvertToIMSymbol(rocSymbol), rocSymbol);
		}

		private void GetOptionChainInfo(string rocSymbol)
		{
			_rdsService.GetOptionChainAsync(ConvertToIMSymbol(rocSymbol), rocSymbol);
		}

		private void GetSSFutureChainInfo(string rocSymbol)
		{
			_rdsService.GetSSFutureChainAsync(ConvertToIMSymbol(rocSymbol), rocSymbol);
		}

		#endregion
	}

	public class SecurityEventArgs : EventArgs
	{
		private IMSecurityInfo _info;

		public SecurityEventArgs(IMSecurityInfo info)
		{
			_info = info;
		}

		public IMSecurityInfo Info
		{
			get
			{
				return _info;
			}
		}
	}

	public class OptionChainEventArgs : EventArgs
	{
		private IMSecurityInfo _info;

		public OptionChainEventArgs(IMSecurityInfo info)
		{
			_info = info;
		}

		public IMSecurityInfo Info
		{
			get
			{
				return _info;
			}
		}
	}

	public class SSFutureChainEventArgs : EventArgs
	{
		private IMSecurityInfo _info;

		public SSFutureChainEventArgs(IMSecurityInfo info)
		{
			_info = info;
		}

		public IMSecurityInfo Info
		{
			get
			{
				return _info;
			}
		}
	}
}
