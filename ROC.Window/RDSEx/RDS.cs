using System;
using System.ComponentModel;
using System.Collections.Generic;

using RDSEx.WEB;
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

		private Dictionary<string, ROCTrade> _rocExecutions;
		public Dictionary<string, ROCTrade> RocExecutions
		{
			get
			{
				if (_rocExecutions == null)
				{
					_rocExecutions = new Dictionary<string, ROCTrade>();
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
		private Dictionary<string, ROCPosition> _tposPositions;
		public Dictionary<string, ROCPosition> TposPositions
		{
			get
			{
				if (_tposPositions == null)
				{
					_tposPositions = new Dictionary<string, ROCPosition>();
				}
				return _tposPositions;
			}
			set
			{
				_tposPositions = value;
			}
		}

		private Dictionary<string, ROCPosition> _newTposPositions;
		public Dictionary<string, ROCPosition> NewTposPositions
		{
			get
			{
				if (_newTposPositions == null)
				{
					_newTposPositions = new Dictionary<string, ROCPosition>();
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
		private Dictionary<string, ROCTrade> _tposExecutions;
		public Dictionary<string, ROCTrade> TposExecutions
		{
			get
			{
				if (_tposExecutions == null)
				{
					_tposExecutions = new Dictionary<string, ROCTrade>();
				}
				return _tposExecutions;
			}
			set
			{
				_tposExecutions = value;
			}
		}

		private object _newTposExecutionsLock = new object();
		private Dictionary<string, ROCTrade> _newTposExecutions = new Dictionary<string, ROCTrade>();

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
		private Dictionary<string, BaseSSFutureInfo> _symbolToSSFInfos;
		public Dictionary<string, BaseSSFutureInfo> SymbolToSSFInfos
		{
			get
			{
				if (_symbolToSSFInfos == null)
				{
					_symbolToSSFInfos = new Dictionary<string, BaseSSFutureInfo>();
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
				ErrorMsg = string.Concat("GetUserProfile|Cancelled", e.Error.Message, " ", e.Error.StackTrace);
			}
			else if (e.Error != null)
			{
				ErrorMsg = string.Concat("GetUserProfile|Error ", e.Error.Message, " ", e.Error.StackTrace);
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
								ErrorMsg = string.Concat("GetUserProfile|Result Empty");
							}
						}
						catch (Exception ex)
						{
							ErrorMsg = string.Concat("GetUserProfile|Exception ", ex.Message, " : ", ex.StackTrace);
						}
						break;
					default:
						ErrorMsg = string.Concat("GetUserProfile|Result|StatusCode ", e.Result.statusCode.ToString(), " : ", e.Result.statusMessage);
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
				WarningMsg = string.Concat("GetUserMarketDataSources|Cancelled", e.Error.Message, " ", e.Error.StackTrace);
			}
			else if (e.Error != null)
			{
				WarningMsg = string.Concat("GetUserMarketDataSources|Error ", e.Error.Message, " ", e.Error.StackTrace);
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
									if (e.Result.retValues is object[] userMarketDataObjs) {
										foreach (object userMarketDataObj in userMarketDataObjs) {
											if (userMarketDataObj != null) {
												UserMarketDataSources.Add((string)userMarketDataObj);
											}
										}
									}
								}
							}
							else
							{
								WarningMsg = "GetUserMarketDataSources|Result Empty";
							}
						}
						catch (Exception ex)
						{
							WarningMsg = string.Concat("GetUserMarketDataSources|Exception ", ex.Message, " : ", ex.StackTrace);
						}
						break;
					default:
						WarningMsg = string.Concat("GetUserMarketDataSources|Result|StatusCode ", e.Result.statusCode.ToString(), " : ", e.Result.statusMessage);
						break;
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
				if (traderAcrObj is TraderAcr traderAcr)
				{
					tradeFor = "";
					traderMap = new TraderMap();
					acctProfileObjs = DecodeTrader(traderAcr, ref traderMap, ref tradeFor);

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
			if (!UserProfiles.TryGetValue(tradeFor, out traderMap))
			{
				traderMap = new TraderMap();
				UserProfiles.Add(tradeFor, traderMap);
			}

			traderMap.tradeFor = traderAcr.tradeFor.ToUpper();
			traderMap.localAcAcrn = traderAcr.localAcAcrn.ToUpper();

			return traderAcr.acctProfiles;
		}

		#endregion

		#region - User Profile - Traders - Accounts -

		private void DecodAcctProfileObj(object[] acctProfileObjs, ref TraderMap traderMap)
		{
			foreach (object acctProfileObj in acctProfileObjs)
			{
				if (acctProfileObj is AcctProfile acctProfile) {
					AccountMap acctMap = GetAcctMapByType(acctProfile, ref traderMap);
					acctMap.Update(acctProfile);

					if (acctProfile.accountDestinations != null) {
						DecodeDestinationObj(acctProfile.accountDestinations, acctMap);
					}
				}
			}
		}

		private AccountMap GetAcctMapByType(AcctProfile acctProfile, ref TraderMap traderMap)
		{
			Dictionary<string, AccountMap> accounts;
			AccountMap acctMap;

			switch (acctProfile.type)
			{
				case AccountTypes.Stock:
					accounts = traderMap.CSAccounts;
					break;
				case AccountTypes.Option:
					accounts = traderMap.OPTAccounts;
					break;
				case AccountTypes.Future:
					accounts = traderMap.FUTAccounts;
					break;
				default:
					accounts = null;
					break;
			}

			if (accounts == null) {
				return null;
			} else if (!accounts.TryGetValue(acctProfile.account, out acctMap)) {
				acctMap = new AccountMap();
				accounts.Add(acctProfile.account, acctMap);
			}

			return acctMap;
		}

		#endregion

		#region - User Profile - Traders - Accounts - Destinations -

		private void DecodeDestinationObj(object[] acctDestinationObjs, AccountMap acctMap)
		{
			foreach (object acctDestinationObj in acctDestinationObjs)
			{
				if (acctDestinationObj is AcctDestination acctDest) {
					DestinationMap destMap = acctMap.Destinations.FindOrAdd(acctDest.destID);

					destMap.destID = acctDest.destID;
					destMap.secType = acctDest.secType.ToUpper();
					destMap.shortName = acctDest.shortName.ToUpper();
					destMap.cmtaFirmID = acctDest.cmtaFirmID.ToUpper();
					destMap.giveupFirmID = acctDest.giveupFirmID.ToUpper();

					string algoFlag = (acctDest.algoFlag == null) ? "0" : acctDest.algoFlag.ToUpper();
					destMap.algoFlag = algoFlag;

					BuildDestinationMaps(destMap);
				}
			}
		}

		#endregion

		#region - Quick Local Account - Destinations Maps

		private void buildAccounts(
			string tradeFor,
			Dictionary<string, AccountMap> accounts,
			Dictionary<string, List<AccountMap>> traderAccounts,
			Dictionary<string, List<string>> traderAccountNames,
			Dictionary<string, List<DestinationMap>> localAccountToDests,
			Dictionary<string, List<int>> localAccountToDestIds,
			Dictionary<string, List<string>> localAccountToDestNames
			)
		{
			Dictionary<string, AccountMap>.Enumerator stockAccounts = accounts.GetEnumerator();
			while (stockAccounts.MoveNext()) {
				AccountMap acctVal = stockAccounts.Current.Value;
				AddToTposAccountFilter(acctVal);

				if (!traderAccounts.TryGetValue(tradeFor, out List<AccountMap> accts)) {
					accts = new List<AccountMap>();
					traderAccounts[tradeFor] = accts;
				}
				accts.Add(acctVal);

				if (!traderAccountNames.TryGetValue(tradeFor, out List<string> acctNames)) {
					acctNames = new List<string>();
					traderAccountNames[tradeFor] = acctNames;
				}
				acctNames.Add(acctVal.account);

				DestinationMap.Collection.Enumerator destinations = accounts[acctVal.account].Destinations.GetEnumerator();
				while (destinations.MoveNext()) {
					DestinationMap destVal = destinations.Current;

					if (!localAccountToDests.TryGetValue(acctVal.account, out List<DestinationMap> dests)) {
						dests = new List<DestinationMap>();
						localAccountToDests.Add(acctVal.account, dests);
					}
					dests.Add(destVal);

					if (!localAccountToDestIds.TryGetValue(acctVal.account, out List<int> destIDs)) {
						destIDs = new List<int>();
						localAccountToDestIds.Add(acctVal.account, destIDs);
					}

					if (destVal.destID != null)
						destIDs.Add((int)destVal.destID);

					if (!localAccountToDestNames.TryGetValue(acctVal.account, out List<string> destNames)) {
						destNames = new List<string>();
						localAccountToDestNames.Add(acctVal.account, destNames);
					}
					destNames.Add(destVal.shortName);
				}
			}
		}

		private void BuildLocalAccounts()
		{
			if (GotUserProfiles)
			{
				Dictionary<string, TraderMap>.Enumerator traders = UserProfiles.GetEnumerator();
				while (traders.MoveNext()) {
					TraderMap tm = traders.Current.Value;

					// Stock Maps
					buildAccounts(
						tm.tradeFor,
						tm.CSAccounts,
						TraderToStockAccts,
						TraderToStockAcctNames,
						LocalStockAcctToDests,
						LocalStockAcctToDestIDs,
						LocalStockAcctToDestNames);

					// Future Maps
					buildAccounts(
						tm.tradeFor,
						tm.FUTAccounts,
						TraderToFutureAccts,
						TraderToFutureAcctNames,
						LocalFutureAcctToDests,
						LocalFutureAcctToDestIDs,
						LocalFutureAcctToDestNames);

					// Option Maps
					buildAccounts(
						tm.tradeFor,
						tm.OPTAccounts,
						TraderToOptionAccts,
						TraderToOptionAcctNames,
						LocalOptionAcctToDests,
						LocalOptionAcctToDestIDs,
						LocalOptionAcctToDestNames);
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
				ErrorMsg = string.Concat("GetUserOrder|Cancelled", e.Error.Message, " ", e.Error.StackTrace);
			}
			else if (e.Error != null)
			{
				ErrorMsg = string.Concat("GetUserOrder|Error ", e.Error.Message, " ", e.Error.StackTrace);
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
							ErrorMsg = string.Concat("GetUserOrder|Exception ", ex.Message, " : ", ex.StackTrace);
						}
						break;
					default:
						ErrorMsg = string.Concat("GetUserOrder|Result|StatusCode ", e.Result.statusCode.ToString(), " : ", e.Result.statusMessage);
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
			if (!RocOrders.TryGetValue(userOrder.omTag, out ord)) {
				ord = new ROCOrder(userOrder);
				RocOrders.Add(userOrder.omTag, ord);
			} else {
				ord.Update(userOrder);
			}

			UpdateRocOrders(userOrder.omTag, ord);
		}

		public void UpdateRocOrders(string key, ROCOrder ord)
		{
			ord.MakeRocStatus();
			lock (RocOrders)
			{
				RocOrders[key] = ord;
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
				ErrorMsg = string.Concat("GetUserExecutions|Cancelled", e.Error.Message, " ", e.Error.StackTrace);
			}
			else if (e.Error != null)
			{
				ErrorMsg = string.Concat("GetUserExecutions|Error ", e.Error.Message, " ", e.Error.StackTrace);
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
							ErrorMsg = string.Concat("GetUserExecutions|Exception ", ex.Message, " : ", ex.StackTrace);
						}
						break;
					default:
						ErrorMsg = string.Concat("GetUserExecutions|Result|StatusCode ", e.Result.statusCode.ToString(), " : ", e.Result.statusMessage);
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

			if (omExecTag != "" && omTag != "" && RocOrders.TryGetValue(omTag, out ROCOrder order))
			{
				ROCTrade traded;
				if (order.TryGetTrade(omExecTag, out traded))
				{
					// trade again?
					ErrorMsg = string.Concat("DecodeUserExecution|Duplication ", omExecTag);
					traded.Update(userExecution, order);
				} else
				{
					traded = new ROCTrade(userExecution, order);
					order.AddTrade(omExecTag, traded);
				}
				RocExecutions.Add(omExecTag, traded);
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
				ErrorMsg = string.Concat("GetTposPosition|Cancelled", e.Error.Message, " ", e.Error.StackTrace);
			}
			else if (e.Error != null)
			{
				ErrorMsg = string.Concat("GetTposPosition|Error ", e.Error.Message, " ", e.Error.StackTrace);
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
						ErrorMsg = string.Concat("GetTposPosition|Result|StatusCode ", e.Result.statusCode.ToString(), " : ", e.Result.statusMessage);
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
				if (VerifyTposPosition(pos))
				{
					ROCPosition position = new ROCPosition(pos);

					lock (TposPositions)
					{
						if (!TposPositions.TryGetValue(position.PositionKey, out ROCPosition found) || (found.Version != pos.m_Version)) {
							TposPositions[position.PositionKey] = position;
							AddToNewPositions(position);
						}
					}
				}
			}
			catch (Exception ex)
			{
				ErrorMsg = string.Concat("DecodeTposPosition|Error ", ex.Message, " : ", ex.StackTrace);
			}
		}

		private bool VerifyTposPosition(TposPosition pos)
		{
			return AccountFilterMap.Contains(pos.m_ClearingAccount);
		}

		private void AddToNewPositions(ROCPosition position)
		{
			if (!_firstTposPositionCall)
			{
				lock (NewTposPositions)
				{
					NewTposPositions[position.PositionKey] = position;
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

		public List<ROCTrade> TakeNewTPosExecutions()
		{
			Dictionary<string, ROCTrade> taken, reset = new Dictionary<string, ROCTrade>();
			lock (_newTposExecutionsLock) {
				taken = _tposExecutions;
				_tposExecutions = reset;
			}

			if ((taken != null) && (taken.Count > 0)) {
				List<ROCTrade> result = new List<ROCTrade>();
				result.AddRange(taken.Values);
				return result;
			}
			return null;
		}

		private void rdsService_GetTposTradesByTraderCompleted(object sender, RtrvTposTradesByTraderCompletedEventArgs e)
		{
			if (e.Cancelled)
			{
				ErrorMsg = string.Concat("GetTposTrades|Cancelled", e.Error.Message, " ", e.Error.StackTrace);
			}
			else if (e.Error != null)
			{
				ErrorMsg = string.Concat("GetTposTrades|Error ", e.Error.Message, " ", e.Error.StackTrace);
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
						ErrorMsg = string.Concat("GetTposTrades|Result|StatusCode ", e.Result.statusCode.ToString(), " : ", e.Result.statusMessage);
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
					ROCTrade exec;
					lock (TposExecutions)
					{
						if (!TposExecutions.TryGetValue(tposTrade.m_TradeID, out exec)) {
							exec = new ROCTrade(tposTrade);
							TposExecutions.Add(tposTrade.m_TradeID, exec);
						} else if ((exec.Version != tposTrade.m_Version) || (exec.LastModReasonID != tposTrade.m_LastModReasonID)) {
							exec.Update(tposTrade);
						} else {
							return; // Same trade, exit.
						}
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
				ErrorMsg = string.Concat("DecodeTposTrades|Error ", ex.Message, " : ", ex.StackTrace);
			}
		}

		private void AddToNewExecutions(ROCTrade exec)
		{
			if (!_firstTposExecutionCall)
			{
				lock (_newTposExecutionsLock)
				{
					_newTposExecutions[exec.TradeID] = exec;
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

				if (!SymbolSecurityInfos.TryGetValue(rocSymbol, out IMSecurityInfo found))
				{
					AddToSecurityInfoWaitingList(rocSymbol, "");
				}
				else
				{
					OnSecurityReceived(new SecurityEventArgs(found));
				}
			}
			catch (Exception ex)
			{
				ErrorMsg = string.Concat("GetSecurityByTicker|Error ", ex.Message, " : ", ex.StackTrace);
			}
		}

		private void rdsService_GetSecurityByTickerCompleted(object sender, GetSecurityByTickerCompletedEventArgs e)
		{
			string rocSymbol = (string)e.UserState;
			bool gotSecInfo = false;

			if (e.Cancelled)
			{
				ErrorMsg = string.Concat("GetSecurityByTicker|Cancelled", e.Error.Message, " ", e.Error.StackTrace);
			}
			else if (e.Error != null)
			{
				ErrorMsg = string.Concat("GetSecurityByTicker|Error ", e.Error.Message, " ", e.Error.StackTrace);
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
						ErrorMsg = string.Concat("GetSecurityByTicker|Result|StatusCode ", e.Result.statusCode.ToString(), " : ", e.Result.statusMessage);
						break;
				}
			}

			lock (ImSecurityInfoWaitingList)
			{
				IMSecurityInfo found;

				RemoveFromSecurityInfoWaitingList(rocSymbol);

				if (ImOPTChainInfoWaitingList.Contains(rocSymbol))
				{
					GetOptionChainInfo(rocSymbol);

					if (gotSecInfo && SymbolSecurityInfos.TryGetValue(rocSymbol, out found))
					{
						OnSecurityReceived(new SecurityEventArgs(found));
					}
					return;
				}
				else if (ImSFFChainInfoWaitingList.Contains(rocSymbol))
				{
					GetSSFutureChainInfo(rocSymbol);

					if (gotSecInfo && SymbolSecurityInfos.TryGetValue(rocSymbol, out found))
					{
						OnSecurityReceived(new SecurityEventArgs(found));
					}
					return;
				}

				// Done
				if (gotSecInfo && SymbolSecurityInfos.TryGetValue(rocSymbol, out found))
				{
					OnSecurityReceived(new SecurityEventArgs(found));
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
				SymbolSecurityInfos[rocSymbol] = info;
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
			else if (!SymbolSecurityInfos.TryGetValue(rocSymbol, out IMSecurityInfo found) || found.OptionChain.Count == 0)
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
				ErrorMsg = string.Concat("GetOptionChain|Cancelled", e.Error.Message, " ", e.Error.StackTrace);
			}
			else if (e.Error != null)
			{
				ErrorMsg = string.Concat("GetOptionChain|Error ", e.Error.Message, " ", e.Error.StackTrace);
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
						ErrorMsg = string.Concat("GetOptionChain|Result|StatusCode ", e.Result.statusCode.ToString(), " : ", e.Result.statusMessage);
						break;
				}
			}

			lock (ImSecurityInfoWaitingList)
			{
				IMSecurityInfo found;

				RemoveFromOPTChainInfoWaitingList(rocSymbol);

				if (ImSFFChainInfoWaitingList.Contains(rocSymbol))
				{
					GetSSFutureChainInfo(rocSymbol);

					if (gotSecInfo && SymbolSecurityInfos.TryGetValue(rocSymbol, out found))
					{
						OnOptionChainReceived(new OptionChainEventArgs(found));
					}
					return;
				}

				// Done
				if (gotSecInfo && SymbolSecurityInfos.TryGetValue(rocSymbol, out found))
				{
					OnOptionChainReceived(new OptionChainEventArgs(found));
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

				if (gotSecInfo && SymbolSecurityInfos.TryGetValue(rocSymbol, out IMSecurityInfo found))
				{
					OnSecurityReceived(new SecurityEventArgs(found));
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
				if (SymbolSecurityInfos.TryGetValue(rocSymbol, out IMSecurityInfo secinfo))
				{
					info.MDSource = secinfo.MDSource;
					if (secinfo.SecType == CSVFieldIDs.SecurityTypes.Future)
					{
						info.SecType = CSVFieldIDs.SecurityTypes.OptionFuture;
						info.TickSize = secinfo.TickSize;
					}
					else
					{
						info.SecType = CSVFieldIDs.SecurityTypes.Option;
					}

					secinfo.OptionChain[info.MDSymbol] = info;
				} else
				{
					Debug.Assert(!Debugger.IsAttached, "Missing Symbol For Option Chain");
				}
			}

			lock (SymbolToOptionInfos)
			{
				SymbolToOptionInfos[info.MDSymbol] = info;
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
			else if (!SymbolSecurityInfos.TryGetValue(rocSymbol, out var found) || (found.SSFutureChain.Count == 0))
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
				ErrorMsg = string.Concat("GetSSFutureChain|Cancelled", e.Error.Message, " ", e.Error.StackTrace);
			}
			else if (e.Error != null)
			{
				ErrorMsg = string.Concat("GetSSFutureChain|Error ", e.Error.Message, " ", e.Error.StackTrace);
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
						ErrorMsg = string.Concat("GetSSFutureChain|Result|StatusCode ", e.Result.statusCode.ToString(), " : ", e.Result.statusMessage);
						break;
				}
			}

			lock (ImSecurityInfoWaitingList)
			{
				IMSecurityInfo found;

				RemoveFromSFFChainInfoWaitingList(rocSymbol);

				if (ImOPTChainInfoWaitingList.Contains(rocSymbol))
				{
					GetOptionChainInfo(rocSymbol);

					if (gotSecInfo && SymbolSecurityInfos.TryGetValue(rocSymbol, out found))
					{
						OnSSFutureChainReceived(new SSFutureChainEventArgs(found));
					}
					return;
				}

				// Done
				if (gotSecInfo && SymbolSecurityInfos.TryGetValue(rocSymbol, out found))
				{
					OnSSFutureChainReceived(new SSFutureChainEventArgs(found));
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

				if (gotSecInfo && SymbolSecurityInfos.TryGetValue(rocSymbol, out IMSecurityInfo found))
				{
					OnSecurityReceived(new SecurityEventArgs(found));
				}
			}

			return gotSecInfo;
		}

		private void DecodeSSFuture(FutureDesc ssfuture, string rocSymbol)
		{
			if (ssfuture != null)
			{
				IMSSFutureInfo info = new IMSSFutureInfo(ssfuture);
			
				lock (SymbolSecurityInfos)
				{
					if (SymbolSecurityInfos.TryGetValue(rocSymbol, out var found))
					{
						found.SSFutureChain[info.MDSymbol] = info;
					} else
					{
						Debug.Assert(!Debugger.IsAttached, "Missing Symbol For SSFuture Chain");
					}
				}

				lock (SymbolToSSFInfos)
				{
					SymbolToSSFInfos[ssfuture.fullSymbol] = info;
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
				ErrorMsg = string.Concat("UpdateUserPassword|Error ", ex.Message, " : ", ex.StackTrace);
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
				ErrorMsg = string.Concat("ResetSecurityInfo|Error ", ex.Message, " : ", ex.StackTrace);
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
				ImSecurityInfoWaitingList.Remove(rocSymbol);
			}
		}

		private void RemoveFromOPTChainInfoWaitingList(string rocSymbol)
		{
			lock (ImOPTChainInfoWaitingList)
			{
				ImOPTChainInfoWaitingList.Remove(rocSymbol);
			}
		}

		private void RemoveFromSFFChainInfoWaitingList(string rocSymbol)
		{
			lock (ImSFFChainInfoWaitingList)
			{
				ImSFFChainInfoWaitingList.Remove(rocSymbol);
			}
		}

		// Used by Sercrity Info Only
		private string ConvertToIMSymbol(string rocSymbol)
		{
			string _tposSymbol = string.IsNullOrEmpty(rocSymbol) ? "": rocSymbol.Replace("/PR", "/P");
			return _tposSymbol;
		}

		private void AddToSymbolDetailToRocSymbolMap(string symbolDetail, string rocSymbol)
		{
			lock (SymbolDetailToRocSymbolMap)
			{
				SymbolDetailToRocSymbolMap[symbolDetail] = rocSymbol;
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
		public readonly IMSecurityInfo Info;

		public SecurityEventArgs(IMSecurityInfo info)
		{
			Info = info;
		}
	}

	public class OptionChainEventArgs : EventArgs
	{
		public readonly IMSecurityInfo Info;

		public OptionChainEventArgs(IMSecurityInfo info)
		{
			Info = info;
		}
	}

	public class SSFutureChainEventArgs : EventArgs
	{
		public readonly IMSecurityInfo Info;

		public SSFutureChainEventArgs(IMSecurityInfo info)
		{
			Info = info;
		}
	}
}
