using System;
using System.ComponentModel;
using System.Collections.Generic;

using RDSEx;
using CSVEx;
using MarketData;

namespace ROC
{
	internal class HelperRDS : HelperStatusBase
	{
		internal enum StatusTypes
		{
			None,
			GotError,
			GettingVersion,
			GotVersion,
			GettingUserprofiles,
			GotUserprofiles,
			GettingUserMarketDataSources,
			GotUserMarketDataSources,
			GettingUserOrders,
			GotUserOrders,
			GettingUserExecutions,
			GotUserExecutions,
			GettingTPOSPositions,
			GotTPOSPositions,
			GettingTPOSTrades,
			GotTPOSTrades,

			ConnectingToMDS,
			ConnectedToMDS,
			ConnectionToMDSFailed,

			Done,
			Failed,
		}

		#region - Property -

		internal StatusTypes Status { get; private set; } = StatusTypes.None;

		private bool _extended = false;
		internal bool Extended
		{
			get
			{
				return _extended;
			}
			set
			{
				if (_extended != value)
				{
					_extended = value;

					if (_extended)
					{
						StartTimer();
					}
					else
					{
						StopTimer();
					}
				}
			}
		}

		private bool _newSecurityInfo = false;
		internal bool NewSecufityInfo
		{
			get
			{
				return _newSecurityInfo;
			}
			set
			{
				_newSecurityInfo = value;
			}
		}

		#endregion

		#region - RDS Property -

		internal string ErrorMsg
		{
			get
			{
				return _rds.ErrorMsg;
			}
		}

		internal string Address
		{
			get
			{
				return _rds.Address;
			}
		}

		internal string Version
		{
			get
			{
				return _rds.Version;
			}
		}

		internal bool GotUserProfiles
		{
			get
			{
				return _rds.GotUserProfiles;
			}
		}

		internal Dictionary<string, TraderMap> UserProfiles
		{
			get
			{
				return _rds.UserProfiles;
			}
		}

		internal bool GotUserMarketDataSources
		{
			get
			{
				return _rds.GotUserMarketDataSources;
			}
		}

		internal List<string> UserMarketDataSources
		{
			get
			{
				return _rds.UserMarketDataSources;
			}
		}

		#region - Trader to Stock Acct Map -

		internal Dictionary<string, List<AccountMap>> TraderToStockAccts
		{
			get
			{
				return _rds.TraderToStockAccts;
			}
		}

		internal Dictionary<string, List<string>> TraderToStockAcctNames
		{
			get
			{
				return _rds.TraderToStockAcctNames;
			}
		}

		#endregion

		#region - Trader to Future Acct Map -

		internal Dictionary<string, List<AccountMap>> TraderToFutureAccts
		{
			get
			{
				return _rds.TraderToFutureAccts;
			}
		}

		internal Dictionary<string, List<string>> TraderToFutureAcctNames
		{
			get
			{
				return _rds.TraderToFutureAcctNames;
			}
		}

		#endregion

		#region - Trader to Option Acct Map -

		internal Dictionary<string, List<AccountMap>> TraderToOptionAccts
		{
			get
			{
				return _rds.TraderToOptionAccts;
			}
		}

		internal Dictionary<string, List<string>> TraderToOptionAcctNames
		{
			get
			{
				return _rds.TraderToOptionAcctNames;
			}
		}

		#endregion

		#region - Stock Acct to Dest Maps -

		internal Dictionary<string, List<DestinationMap>> LocalStockAcctToDests
		{
			get
			{
				return _rds.LocalStockAcctToDests;
			}
		}

		internal Dictionary<string, List<int>> LocalStockAcctToDestIDs
		{
			get
			{
				return _rds.LocalStockAcctToDestIDs;
			}
		}

		internal Dictionary<string, List<string>> LocalStockAcctToDestNames
		{
			get
			{
				return _rds.LocalStockAcctToDestNames;
			}
		}

		#endregion

		#region - Future Acct to Dest Maps -

		internal Dictionary<string, List<DestinationMap>> LocalFutureAcctToDests
		{
			get
			{
				return _rds.LocalFutureAcctToDests;
			}
		}

		internal Dictionary<string, List<int>> LocalFutureAcctToDestIDs
		{
			get
			{
				return _rds.LocalFutureAcctToDestIDs;
			}
		}

		internal Dictionary<string, List<string>> LocalFutureAcctToDestNames
		{
			get
			{
				return _rds.LocalFutureAcctToDestNames;
			}
		}

		#endregion

		#region - Option Acct To Dest Maps -

		private Dictionary<string, List<DestinationMap>> _localOptionAcctToDests;
		internal Dictionary<string, List<DestinationMap>> LocalOptionAcctToDests
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
		internal Dictionary<string, List<int>> LocalOptionAcctToDestIDs
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
		internal Dictionary<string, List<string>> LocalOptionAcctToDestNames
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

		internal Dictionary<int, string> DestIDToName
		{
			get
			{
				return _rds.DestIDToName;
			}
		}

		internal Dictionary<int, DestinationMap> DestIDToDestinationMap
		{
			get
			{
				return _rds.DestIDToDestinationMap;
			}
		}

		#endregion

		#region - Dest Name To ID and Map -

		internal Dictionary<string, int> DestNameToID
		{
			get
			{
				return _rds.DestNameToID;
			}
		}

		internal Dictionary<string, DestinationMap> DestNameToDestinationMap
		{
			get
			{
				return _rds.DestNameToDestinationMap;
			}
		}

		#endregion

		internal bool GotRocOrders
		{
			get
			{
				if (Configuration.User.Default.UseROMDatabase)
				{
					return _rds.GotRocOrders;
				}
				else
				{
					return true;
				}
			}
		}

		internal Dictionary<string, ROCOrder> RocOrders
		{
			get
			{
				return _rds.RocOrders;
			}
			set
			{
				_rds.RocOrders = value;
			}
		}

		internal bool GotRocExecutions
		{
			get
			{
				if (Configuration.User.Default.UseROMDatabase)
				{
					return _rds.GotRocExecutions;
				}
				else
				{
					return true;
				}
			}
		}

		internal Dictionary<string, ROCExecution> RocExecutions
		{
			get
			{
				return _rds.RocExecutions;
			}
			set
			{
				_rds.RocExecutions = value;
			}

		}

		internal bool GotTposExecutions
		{
			get
			{
				if (Configuration.User.Default.UseTPOS)
				{
					return _rds.GotTposExecutions;
				}
				else
				{
					return true;
				}
			}
		}

		internal Dictionary<string, TPOSExecution> TposExecutions
		{
			get
			{
				return _rds.TposExecutions;
			}
		}

		internal bool GotTposPositions
		{
			get
			{
				if (Configuration.User.Default.UseTPOS)
				{
					return _rds.GotTposPositions;
				}
				else
				{
					return true;
				}
			}
		}

		internal Dictionary<string, RDSPosition> TposPositions
		{
			get
			{
				return _rds.TposPositions;
			}
		}

		internal Dictionary<string, RDSPosition> NewTposPositions
		{
			get
			{
				return _rds.NewTposPositions;
			}
		}

		internal Dictionary<string, IMSecurityInfo> SymbolSecurityInfos
		{
			get
			{
				return _rds.SymbolSecurityInfos;
			}
		}

		internal Dictionary<string, IMOptionInfo> SymbolToOptionInfoMap
		{
			get
			{
				return _rds.SymbolToOptionInfos;
			}
		}

		internal Dictionary<string, string> SymbolDetailToRocSymbolMap
		{
			get
			{
				return _rds.SymbolDetailToRocSymbolMap;
			}
		}

		#endregion

		#region - Local Variable -

		private RDS _rds;

		#endregion

		internal HelperRDS()
		{
			Initialize();
		}

		private void Initialize()
		{
			if (_rds == null)
			{
				_rds = new RDS(Configuration.RDS.Default.WebServiceLocation);
				_rds.PropertyChanged += new PropertyChangedEventHandler(rds_PropertyChanged);

				_rds.OnSecurityEvent += new SecurityEventHandler(rds_OnSecurityEvent);
				_rds.OnOptionChainEvent += new OptionChainEventHandler(rds_OnOptionChainEvent);
				_rds.OnSSFutureChainEvent += new SSFutureChainEventHandler(rds_OnSSFutureChainEvent);
			}
		}

		internal void GetVersion()
		{
			SetStatus(StatusTypes.GettingVersion, string.Concat("RDS|GetVersion Getting... "));
			_rds.GetVersion();
		}

		internal void GetUserProfile(string user, string pass)
		{
			SetStatus(StatusTypes.GettingUserprofiles, string.Concat("RDS|GetUserProfile Getting... "));
			_rds.GetUserProfile(user, pass);
		}

		internal void GetUserMarketDataSource()
		{
			SetStatus(StatusTypes.GettingUserprofiles, string.Concat("RDS|GetUserMarketDataSources Getting... "));
			_rds.GetUserMarketDataSources();
		}

		internal void GetUserOrders()
		{
			if (Configuration.User.Default.UseROMDatabase)
			{
				SetStatus(StatusTypes.GettingUserOrders, string.Concat("RDS|GetUserOrders Getting... "));
				_rds.GetUserOrders();
			}
			else
			{
				SetStatus(StatusTypes.GotUserOrders, string.Concat("RDS|GetUserOrders Skipped "));
			}
		}

		internal void GetUserExecutions()
		{
			if (Configuration.User.Default.UseROMDatabase)
			{
				SetStatus(StatusTypes.GettingUserExecutions, string.Concat("RDS|GetUserExecutions Getting... "));
				_rds.GetUserExecutions();
			}
			else
			{
				SetStatus(StatusTypes.GotUserExecutions, string.Concat("RDS|GetUserExecutions Skipped "));
			}
		}

		internal void GetTPOSPositions()
		{
			if (Configuration.User.Default.UseTPOS)
			{
				SetStatus(StatusTypes.GettingTPOSPositions, string.Concat("RDS|GetTPOSPositions Getting... "));
				_rds.GetTPosPositions();
			}
			else
			{
				SetStatus(StatusTypes.GotTPOSPositions, string.Concat("RDS|GetTPOSPositions Skipped "));
			}
		}

		internal void GetTPOSTrades(bool refresh)
		{
			if (Configuration.User.Default.UseTPOS)
			{
				if (!refresh)
				{
					SetStatus(StatusTypes.GettingTPOSTrades, string.Concat("RDS|GetTPOSTrades Getting... "));
				}
				_rds.GetTPosExecutions();
			}
			else
			{
				if (!refresh)
				{
					SetStatus(StatusTypes.GotTPOSTrades, string.Concat("RDS|GetTPOSTrades Skipped "));
				}
			}
		}

		internal void GetSecurityByTicker(string symbol)
		{
			if (symbol != "")
			{
				_rds.GetSecurityByTicker(symbol);
			}
		}

		internal void GetOptionChain(string symbol)
		{
			if (symbol != "")
			{
				_rds.GetOptionChain(symbol);
			}
		}

		internal void GetSSFutureChain(string symbol)
		{
			if (symbol != "")
			{
				_rds.GetSSFutureChain(symbol);
			}
		}

		internal void UpdateUserPassword(string username, string oldPassword, string newPassword)
		{
			_rds.UpdateUserPassword(username, oldPassword, newPassword);
		}

		internal List<TPOSExecution> TakeNewTposExecutions() => _rds.TakeNewTPosExecutions();

		internal void TPOSRefresh()
		{
			lock (TposPositions)
			{
				if (TposPositions.Count == 0)
				{
					// TODO: We didn't get any position from TPOS, try again.
					_rds.GetTPosPositions();
				}
				else
				{
					// TODO: Pull Only Trades Positions Should Already Benn Loaded
					GetTPOSTrades(true);
				}
			}
		}

		internal void ConnectToMDS()
		{
			if (Configuration.User.Default.UseMarketData)
			{
				SetStatus(StatusTypes.ConnectingToMDS, string.Concat("MDS|Connection Connecting... ", Configuration.MDS.Default.IP, " Port: ", Configuration.MDS.Default.Port));
				foreach (HelperMDS mds in GLOBAL.MarketDataProviders)
				{
					mds.Connect();
				}
			}
			else
			{
				SetStatus(StatusTypes.ConnectedToMDS, string.Concat("MDS|Connection Skipped "));
			}
		}

		internal void ConnectingToMDS()
		{
			bool ready = true;

			foreach (HelperMDS mds in GLOBAL.MarketDataProviders) {
				if (!mds.IsConnected)
					ready = false;
			}

			if (ready) {
				// All mds are connected
				foreach (HelperMDS mds in GLOBAL.MarketDataProviders) {
					SetStatus(StatusTypes.ConnectedToMDS, string.Concat("MDS|Connection Connected ", mds.ServerIP, " Port: ", mds.ServerPort));
				}
			} else {
				SetStatus(StatusTypes.ConnectionToMDSFailed, string.Concat("MDS|Connection Failed "), true);
			}
		}

		internal void LoginDone()
		{
			SetStatus(StatusTypes.Done, string.Concat("ROC|Login Done "));
		}

		internal void LoginFailed()
		{
			SetStatus(StatusTypes.Failed, string.Concat("RDS|Login Failed "), true);
		}

		internal void ResetSecurityInfo()
		{
			_rds.ResetSecurityInfo();
		}

		internal BaseSecurityInfo GetSecurityInfoBySymbolDetail(string symbolDetail)
		{
			BaseSecurityInfo secInfo = new BaseSecurityInfo();

			if (SymbolDetailToRocSymbolMap.TryGetValue(symbolDetail, out string rocSymbol) && !string.IsNullOrEmpty(rocSymbol))
			{
				// Future and Stock
				if (SymbolSecurityInfos.TryGetValue(rocSymbol, out IMSecurityInfo securityInfo))
				{
					secInfo.MDSymbol = securityInfo.MDSymbol;
					secInfo.MDSource = securityInfo.MDSource;
					secInfo.TickSize = securityInfo.TickSize;
					secInfo.SecType = securityInfo.SecType;

					secInfo.LongName = securityInfo.LongName;
					secInfo.ContractSize = securityInfo.ContractSize;

					secInfo.Underlying = securityInfo.Underlying;
					secInfo.Expiration = securityInfo.Expiration;

					if (securityInfo.SSFutureChain.Count > 0)
					{
						secInfo.SSFChain = new Dictionary<string, IMSSFutureInfo>(securityInfo.SSFutureChain);
					}

					if (securityInfo.OptionChain.Count > 0)
					{
						secInfo.OptionChain = new Dictionary<string, IMOptionInfo>(securityInfo.OptionChain);
					}
					return secInfo;
				}

				// Option
				if (SymbolToOptionInfoMap.TryGetValue(rocSymbol, out IMOptionInfo optionInfo))
				{
				    secInfo.MDSymbol = optionInfo.MDSymbol;
				    secInfo.MDSource = optionInfo.MDSource;
				    secInfo.TickSize = optionInfo.TickSize;
				    secInfo.SecType = optionInfo.SecType;

				    secInfo.LongName = optionInfo.SymbolDetail;
					if (optionInfo.ContractSize > 0)
					{
						secInfo.ContractSize = optionInfo.ContractSize;
					}
					else
					{
						secInfo.ContractSize = 100;
					}

				    secInfo.Underlying = optionInfo.Underlying;
				    secInfo.Expiration = optionInfo.ExpDate;

				    return secInfo;
				}
			}

			return null;
		}

		internal bool CheckSource(string source)
		{
			bool result = false;

			if (UserMarketDataSources.Count > 0)
			{
				switch (source.ToUpper())
				{
					case "OPRANBBO":
						if (UserMarketDataSources.Contains("OPRA"))
						{
							return true;
						}
						break;
					default:
						if (UserMarketDataSources.Contains(source))
						{
							return true;
						}
						break;
				}
			}

			return result;
		}

		#region - RDS Events -

		private void rds_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			switch (e.PropertyName)
			{
				case "Version":
					SetStatus(StatusTypes.GotVersion, string.Concat("RDS|GotVersion ", Version));
					break;
				case "GotUserProfiles":
					SetStatus(StatusTypes.GotUserprofiles, string.Concat("RDS|GotUserprofiles "));
					break;
				case "GotUserMarketDataSources":
					SetStatus(StatusTypes.GotUserMarketDataSources, string.Concat("RDS|GotUserMarketDataSources "));
					break;
				case "GotRocOrders":
					SetStatus(StatusTypes.GotUserOrders, string.Concat("RDS|GotUserOrders = ", RocOrders.Count));
					// Put all the RDS(old) orders into the main orders collection
					GLOBAL.HOrders.Update(RocOrders);
					break;
				case "GotRocExecutions":
					SetStatus(StatusTypes.GotUserExecutions, string.Concat("RDS|GotUserExecutions = ", RocExecutions.Count));
					// Put all the RDS(old) executions into the main executions collection
					GLOBAL.HExecutions.Update(RocExecutions);
					GLOBAL.HPositions.Update(RocExecutions);
					break;
				case "GotTposExecutions":
					SetStatus(StatusTypes.GotTPOSTrades, string.Concat("RDS|GotTposExecutions = ", TposExecutions.Count));
					// Put all the tpos executions into the main executions collection
					GLOBAL.HExecutions.Update(TposExecutions);
					GLOBAL.HPositions.Update(TposExecutions);
					break;
				case "GotTposPositions":
					SetStatus(StatusTypes.GotTPOSPositions, string.Concat("RDS|GotTposPositions = ", TposPositions.Count));
					// Put all the tpos position into the main position collection
					GLOBAL.HPositions.Update(TposPositions);
					break;
				case "GotTposPositionsRefresh":
					AddToStatusLogs(string.Concat("RDS|GotTposPositionsRefresh = ", TposPositions.Count));

					// Repopulate Positions Again.
					lock (GLOBAL.HPositions.TposItems)
					{
						if (GLOBAL.HPositions.TposItems.Count == 0 && TposPositions.Count > 0)
						{
							AddToStatusLogs(string.Concat("RDS|GotTposPositionsRefresh|Repopulate = ", TposPositions.Count));
							GLOBAL.HPositions.Update(TposPositions);
						}
					}
					if (Extended)
					{
						GetTPOSTrades(true);
					}
					else
					{
						StopTimer();
					}
					break;
				case "GotTposExecutionsRefresh":
					CheckExtendedTimer();
					break;
				case "WarningMsg":
					// No Status Change, just a waring msg
					SetStatus(Status, string.Concat("RDS|GotWarning ", ErrorMsg), true);
					break;
				case "ErrorMsg":
				default:
					CheckExtendedTimer();
					SetStatus(StatusTypes.GotError, string.Concat("RDS|GotError ", ErrorMsg), true);
					break;
			}
		}

		private void rds_OnSecurityEvent(object sender, SecurityEventArgs e)
		{
			NewSecufityInfo = true;
		}

		private void rds_OnOptionChainEvent(object sender, OptionChainEventArgs e)
		{
			NewSecufityInfo = true;
		}

		private void rds_OnSSFutureChainEvent(object sender, SSFutureChainEventArgs e)
		{
			NewSecufityInfo = true;
		}

		#endregion

		#region - RDS Event Helper -

		private void SubscribeFullFutureOptionChain(OptionChainEventArgs e)
		{
			string[] chainSymbols = new string[e.Info.OptionChain.Count];
			e.Info.OptionChain.Keys.CopyTo(chainSymbols, 0);

			List<string> symbols = new List<string>();
			symbols.AddRange(chainSymbols);

			foreach (HelperMDS mds in GLOBAL.MarketDataProviders)
			{
				mds.SubscribeToOptionChain(symbols, e.Info.MDSymbol, e.Info.MDSource, CSVFieldIDs.SecurityTypes.OptionFuture);
			}
		}

		private void SubScribeFullOptionChain(OptionChainEventArgs e)
		{
			string[] opraSymbols = new string[e.Info.OptionChain.Count];
			e.Info.OptionChain.Keys.CopyTo(opraSymbols, 0);

			List<string> symbols = new List<string>();
			List<string> nbboSymbols = new List<string>();

			foreach (string opraSymbol in opraSymbols)
			{
				symbols.Add(string.Concat(opraSymbol, ".", OptionExchangeCode.AMEX));
				symbols.Add(string.Concat(opraSymbol, ".", OptionExchangeCode.BOX));
				symbols.Add(string.Concat(opraSymbol, ".", OptionExchangeCode.CBOE));
				symbols.Add(string.Concat(opraSymbol, ".", OptionExchangeCode.C2));
				symbols.Add(string.Concat(opraSymbol, ".", OptionExchangeCode.ISE));
				symbols.Add(string.Concat(opraSymbol, ".", OptionExchangeCode.NSDQ));
				symbols.Add(string.Concat(opraSymbol, ".", OptionExchangeCode.PCX));
				symbols.Add(string.Concat(opraSymbol, ".", OptionExchangeCode.PHLX));
				nbboSymbols.Add(opraSymbol);
			}

			foreach (HelperMDS mds in GLOBAL.MarketDataProviders)
			{
				mds.SubscribeToOptionChain(symbols, e.Info.MDSymbol, Constants.OptionDataSource.OPRA, CSVFieldIDs.SecurityTypes.Option);
				mds.SubscribeToOptionChain(nbboSymbols, e.Info.MDSymbol, Constants.OptionDataSource.OPRANBBO, CSVFieldIDs.SecurityTypes.Option);
			}

			opraSymbols = null;
			nbboSymbols = null;
		}

		#endregion

		#region - Set Status Display, Log and Alert -

		private void SetStatus(StatusTypes type, string statusDsp)
		{
			SetStatus(type, statusDsp, true, false);
		}
		private void SetStatus(StatusTypes type, string statusDsp, bool addToAlarm)
		{
			SetStatus(type, statusDsp, true, addToAlarm);
		}
		private void SetStatus(StatusTypes type, string statusDsp, bool addTolog, bool addToAlarm)
		{
			Status = type;
			StatusDsp = statusDsp;

			if (addTolog)
			{
				AddToStatusLogs(statusDsp);
			}

			if (addToAlarm)
			{
				// Auto Reset
				//Status = StatusTypes.Done;
				//AddToAlerts(statusDsp);
			}
		}
		internal void ResetStatus()
		{
			Status = StatusTypes.Done;
		}

		#endregion

		#region - TPOS Pool Timer -

		private System.Windows.Forms.Timer _poolTimer;

		private void CheckExtendedTimer()
		{
			if (Extended)
			{
				StartTimer();
			}
			else
			{
				StopTimer();
			}
		}

		private void StartTimer()
		{
			if (_poolTimer == null)
			{
				_poolTimer = new System.Windows.Forms.Timer();
				_poolTimer.Tick += new EventHandler(poolTimer_Tick);
				_poolTimer.Interval = Configuration.User.Default.TPOSUpdateRate;
				_poolTimer.Enabled = true;
			}
		}

		private void StopTimer()
		{
			if (_poolTimer != null)
			{
				_poolTimer.Stop();
				_poolTimer.Tick -= new EventHandler(poolTimer_Tick);
				_poolTimer = null;
			}
			_poolTimer = null;
		}

		private void poolTimer_Tick(object sender, EventArgs e)
		{
			StopTimer();
			TPOSRefresh();
		}

		#endregion
	}
}
