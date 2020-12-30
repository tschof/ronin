using System;
using System.ComponentModel;
using System.Collections.Generic;

using RDSEx;
using CSVEx;
using MarketDataEx;
using System.Diagnostics;

namespace ROC
{
	public class HelperRDS : HelperStatusBase
	{
		public enum StatusTypes
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

		private StatusTypes _status = StatusTypes.None;
		public StatusTypes Status
		{
			get
			{
				return _status;
			}
			set
			{
				if (_status != value)
				{
					_status = value;
				}
			}
		}

		private bool _extended = false;
		public bool Extended
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
		public bool NewSecufityInfo
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

		public string ErrorMsg
		{
			get
			{
				return _rds.ErrorMsg;
			}
		}

		public string Address
		{
			get
			{
				return _rds.Address;
			}
		}

		public string Version
		{
			get
			{
				return _rds.Version;
			}
		}

		public bool GotUserProfiles
		{
			get
			{
				return _rds.GotUserProfiles;
			}
		}

		public Dictionary<string, TraderMap> UserProfiles
		{
			get
			{
				return _rds.UserProfiles;
			}
		}

		public bool GotUserMarketDataSources
		{
			get
			{
				return _rds.GotUserMarketDataSources;
			}
		}

		public List<string> UserMarketDataSources
		{
			get
			{
				return _rds.UserMarketDataSources;
			}
		}

		#region - Trader to Stock Acct Map -

		public Dictionary<string, List<AccountMap>> TraderToStockAccts
		{
			get
			{
				return _rds.TraderToStockAccts;
			}
		}

		public Dictionary<string, List<string>> TraderToStockAcctNames
		{
			get
			{
				return _rds.TraderToStockAcctNames;
			}
		}

		#endregion

		#region - Trader to Future Acct Map -

		public Dictionary<string, List<AccountMap>> TraderToFutureAccts
		{
			get
			{
				return _rds.TraderToFutureAccts;
			}
		}

		public Dictionary<string, List<string>> TraderToFutureAcctNames
		{
			get
			{
				return _rds.TraderToFutureAcctNames;
			}
		}

		#endregion

		#region - Trader to Option Acct Map -

		public Dictionary<string, List<AccountMap>> TraderToOptionAccts
		{
			get
			{
				return _rds.TraderToOptionAccts;
			}
		}

		public Dictionary<string, List<string>> TraderToOptionAcctNames
		{
			get
			{
				return _rds.TraderToOptionAcctNames;
			}
		}

		#endregion

		#region - Stock Acct to Dest Maps -

		public Dictionary<string, List<DestinationMap>> LocalStockAcctToDests
		{
			get
			{
				return _rds.LocalStockAcctToDests;
			}
		}

		public Dictionary<string, List<int>> LocalStockAcctToDestIDs
		{
			get
			{
				return _rds.LocalStockAcctToDestIDs;
			}
		}

		public Dictionary<string, List<string>> LocalStockAcctToDestNames
		{
			get
			{
				return _rds.LocalStockAcctToDestNames;
			}
		}

		#endregion

		#region - Future Acct to Dest Maps -

		public Dictionary<string, List<DestinationMap>> LocalFutureAcctToDests
		{
			get
			{
				return _rds.LocalFutureAcctToDests;
			}
		}

		public Dictionary<string, List<int>> LocalFutureAcctToDestIDs
		{
			get
			{
				return _rds.LocalFutureAcctToDestIDs;
			}
		}

		public Dictionary<string, List<string>> LocalFutureAcctToDestNames
		{
			get
			{
				return _rds.LocalFutureAcctToDestNames;
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

		public Dictionary<int, string> DestIDToName
		{
			get
			{
				return _rds.DestIDToName;
			}
		}

		public Dictionary<int, DestinationMap> DestIDToDestinationMap
		{
			get
			{
				return _rds.DestIDToDestinationMap;
			}
		}

		#endregion

		#region - Dest Name To ID and Map -

		public Dictionary<string, int> DestNameToID
		{
			get
			{
				return _rds.DestNameToID;
			}
		}

		public Dictionary<string, DestinationMap> DestNameToDestinationMap
		{
			get
			{
				return _rds.DestNameToDestinationMap;
			}
		}

		#endregion

		public bool GotRocOrders
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

		public Dictionary<string, ROCOrder> RocOrders
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

		public bool GotRocExecutions
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

		public Dictionary<string, ROCExecution> RocExecutions
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

		public bool GotTposExecutions
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

		public Dictionary<string, TPOSExecution> TposExecutions
		{
			get
			{
				return _rds.TposExecutions;
			}
		}

		public Dictionary<string, TPOSExecution> NewTposExecutions
		{
			get
			{

				return _rds.NewTposExecutions;
			}
		}

		public bool GotTposPositions
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

		public Dictionary<string, TPOSPosition> TposPositions
		{
			get
			{
				return _rds.TposPositions;
			}
		}

		public Dictionary<string, TPOSPosition> NewTposPositions
		{
			get
			{
				return _rds.NewTposPositions;
			}
		}

		public Dictionary<string, IMSecurityInfo> SymbolSecurityInfos
		{
			get
			{
				return _rds.SymbolSecurityInfos;
			}
		}

		public Dictionary<string, IMOptionInfo> SymbolToOptionInfoMap
		{
			get
			{
				return _rds.SymbolToOptionInfos;
			}
		}

		public Dictionary<string, string> SymbolDetailToRocSymbolMap
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

		public HelperRDS()
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

		public void GetVersion()
		{
			SetStatus(StatusTypes.GettingVersion, String.Concat(new object[] { "RDS|GetVersion Getting... " }));
			_rds.GetVersion();
		}

		public void GetUserProfile(string user, string pass)
		{
			SetStatus(StatusTypes.GettingUserprofiles, String.Concat(new object[] { "RDS|GetUserProfile Getting... " }));
			_rds.GetUserProfile(user, pass);
		}

		public void GetUserMarketDataSource()
		{
			SetStatus(StatusTypes.GettingUserprofiles, String.Concat(new object[] { "RDS|GetUserMarketDataSources Getting... " }));
			_rds.GetUserMarketDataSources();
		}

		public void GetUserOrders()
		{
			if (Configuration.User.Default.UseROMDatabase)
			{
				SetStatus(StatusTypes.GettingUserOrders, String.Concat(new object[] { "RDS|GetUserOrders Getting... " }));
				_rds.GetUserOrders();
			}
			else
			{
				SetStatus(StatusTypes.GotUserOrders, String.Concat(new object[] { "RDS|GetUserOrders Skipped " }));
			}
		}

		public void GetUserExecutions()
		{
			if (Configuration.User.Default.UseROMDatabase)
			{
				SetStatus(StatusTypes.GettingUserExecutions, String.Concat(new object[] { "RDS|GetUserExecutions Getting... " }));
				_rds.GetUserExecutions();
			}
			else
			{
				SetStatus(StatusTypes.GotUserExecutions, String.Concat(new object[] { "RDS|GetUserExecutions Skipped " }));
			}
		}

		public void GetTPOSPositions()
		{
			if (Configuration.User.Default.UseTPOS)
			{
				SetStatus(StatusTypes.GettingTPOSPositions, String.Concat(new object[] { "RDS|GetTPOSPositions Getting... " }));
				_rds.GetTPosPositions();
			}
			else
			{
				SetStatus(StatusTypes.GotTPOSPositions, String.Concat(new object[] { "RDS|GetTPOSPositions Skipped " }));
			}
		}

		public void GetTPOSTrades(bool refresh)
		{
			if (Configuration.User.Default.UseTPOS)
			{
				if (!refresh)
				{
					SetStatus(StatusTypes.GettingTPOSTrades, String.Concat(new object[] { "RDS|GetTPOSTrades Getting... " }));
				}
				_rds.GetTPosExecutions();
			}
			else
			{
				if (!refresh)
				{
					SetStatus(StatusTypes.GotTPOSTrades, String.Concat(new object[] { "RDS|GetTPOSTrades Skipped " }));
				}
			}
		}

		public void GetSecurityByTicker(string symbol)
		{
			if (symbol != "")
			{
				_rds.GetSecurityByTicker(symbol);
			}
		}

		public void GetOptionChain(string symbol)
		{
			if (symbol != "")
			{
				_rds.GetOptionChain(symbol);
			}
		}

		public void GetSSFutureChain(string symbol)
		{
			if (symbol != "")
			{
				_rds.GetSSFutureChain(symbol);
			}
		}

		public void UpdateUserPassword(string username, string oldPassword, string newPassword)
		{
			_rds.UpdateUserPassword(username, oldPassword, newPassword);
		}

		public void TPOSRefresh()
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

		public void ConnectToMDS()
		{
			if (Configuration.User.Default.UseMarketData)
			{
				SetStatus(StatusTypes.ConnectingToMDS, String.Concat(new object[] { "MDS|Connection Connecting... ", Configuration.MDS.Default.IP, " Port: ", Configuration.MDS.Default.Port }));
				foreach (HelperMDS mds in GLOBAL.HMDSs)
				{
					mds.Connect();
				}
			}
			else
			{
				SetStatus(StatusTypes.ConnectedToMDS, String.Concat(new object[] { "MDS|Connection Skipped " }));
			}
		}

		public void ConnectingToMDS()
		{
			bool mdsStopped = false;
			bool mdsConnected = true;

			if (GLOBAL.HMDSs.Count != 0)
			{
				foreach (HelperMDS mds in GLOBAL.HMDSs)
				{
					switch (mds.Status)
					{
						case HelperMDS.StatusTypes.Stopping:
						case HelperMDS.StatusTypes.Stopped:
							mdsStopped = true;
							break;
						case HelperMDS.StatusTypes.Started:
						default:
							break;
					}
				}
			}
			else
			{
				mdsStopped = true;
			}

			if (mdsStopped)
			{
				// One of the server didn't start
				SetStatus(StatusTypes.ConnectionToMDSFailed, String.Concat(new object[] { "MDS|Connection Failed " }), true);
			}
			else
			{
				// no error detected, check to see all mds is connected
				foreach (HelperMDS mds in GLOBAL.HMDSs)
				{
					if (mds.Status != HelperMDS.StatusTypes.Started)
					{
						// still not ready
						mdsConnected = false;
						break;
					}
				}

				if (mdsConnected)
				{
					foreach (HelperMDS mds in GLOBAL.HMDSs)
					{
						// All mds are connected
						SetStatus(StatusTypes.ConnectedToMDS, String.Concat(new object[] { "MDS|Connection Connected ", mds.ServerIP, " Port: ", mds.ServerPort }));
					}
				}
			}
		}

		public void LoginDone()
		{
			SetStatus(StatusTypes.Done, String.Concat(new object[] { "ROC|Login Done "}));
		}

		public void LoginFailed()
		{
			SetStatus(StatusTypes.Failed, String.Concat(new object[] { "RDS|Login Failed " }), true);
		}

		public void MakeRocStatus(ref ROCOrder ord)
		{
			_rds.MakeRocStatus(ref ord);
		}

		public void ResetSecurityInfo()
		{
			_rds.ResetSecurityInfo();
		}

		public BaseSecurityInfo GetSecurityInfoBySymbolDetail(string symbolDetail)
		{
			BaseSecurityInfo secInfo = new BaseSecurityInfo();
			string rocSymbol = "";

			if (SymbolDetailToRocSymbolMap.ContainsKey(symbolDetail))
			{
				rocSymbol = SymbolDetailToRocSymbolMap[symbolDetail];
			}

			if (rocSymbol != "")
			{
				// Future and Stock
				if (SymbolSecurityInfos.ContainsKey(rocSymbol))
				{
					secInfo.MDSymbol = SymbolSecurityInfos[rocSymbol].MDSymbol;
					secInfo.MDSource = SymbolSecurityInfos[rocSymbol].MDSource;
					secInfo.TickSize = SymbolSecurityInfos[rocSymbol].TickSize;
					secInfo.SecType = SymbolSecurityInfos[rocSymbol].SecType;

					secInfo.LongName = SymbolSecurityInfos[rocSymbol].LongName;
					secInfo.ContractSize = SymbolSecurityInfos[rocSymbol].ContractSize;

					secInfo.Underlying = SymbolSecurityInfos[rocSymbol].Underlying;
					secInfo.Expiration = SymbolSecurityInfos[rocSymbol].Expiration;

					if (SymbolSecurityInfos[rocSymbol].SSFutureChain.Count > 0)
					{
						secInfo.SSFChain = new Dictionary<string, IMSSFutureInfo>(SymbolSecurityInfos[rocSymbol].SSFutureChain);
					}

					if (SymbolSecurityInfos[rocSymbol].OptionChain.Count > 0)
					{
						secInfo.OptionChain = new Dictionary<string, IMOptionInfo>(SymbolSecurityInfos[rocSymbol].OptionChain);
					}
					return secInfo;
				}

				// Option
				if (SymbolToOptionInfoMap.ContainsKey(rocSymbol))
				{
				    secInfo.MDSymbol = SymbolToOptionInfoMap[rocSymbol].MDSymbol;
				    secInfo.MDSource = SymbolToOptionInfoMap[rocSymbol].MDSource;
				    secInfo.TickSize = SymbolToOptionInfoMap[rocSymbol].TickSize;
				    secInfo.SecType = SymbolToOptionInfoMap[rocSymbol].SecType;

				    secInfo.LongName = SymbolToOptionInfoMap[rocSymbol].SymbolDetail;
					if (SymbolToOptionInfoMap[rocSymbol].ContractSize > 0)
					{
						secInfo.ContractSize = SymbolToOptionInfoMap[rocSymbol].ContractSize;
					}
					else
					{
						secInfo.ContractSize = 100;
					}

				    secInfo.Underlying = SymbolToOptionInfoMap[rocSymbol].Underlying;
				    secInfo.Expiration = SymbolToOptionInfoMap[rocSymbol].ExpDate;

				    return secInfo;
				}
			}

			return null;
		}

		public bool CheckSource(string source)
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
					SetStatus(StatusTypes.GotVersion, String.Concat(new object[] { "RDS|GotVersion ", Version }));
					break;
				case "GotUserProfiles":
					SetStatus(StatusTypes.GotUserprofiles, String.Concat(new object[] { "RDS|GotUserprofiles " }));
					break;
				case "GotUserMarketDataSources":
					SetStatus(StatusTypes.GotUserMarketDataSources, String.Concat(new object[] { "RDS|GotUserMarketDataSources " }));
					break;
				case "GotRocOrders":
					SetStatus(StatusTypes.GotUserOrders, String.Concat(new object[] { "RDS|GotUserOrders = ", RocOrders.Count}));
					// Put all the RDS(old) orders into the main orders collection
					GLOBAL.HOrders.Update(RocOrders);
					break;
				case "GotRocExecutions":
					SetStatus(StatusTypes.GotUserExecutions, String.Concat(new object[] { "RDS|GotUserExecutions = ", RocExecutions.Count }));
					// Put all the RDS(old) executions into the main executions collection
					GLOBAL.HExecutions.Update(RocExecutions);
					GLOBAL.HPositions.Update(RocExecutions);
					break;
				case "GotTposExecutions":
					SetStatus(StatusTypes.GotTPOSTrades, String.Concat(new object[] { "RDS|GotTposExecutions = ", TposExecutions.Count }));
					// Put all the tpos executions into the main executions collection
					GLOBAL.HExecutions.Update(TposExecutions);
					GLOBAL.HPositions.Update(TposExecutions);
					break;
				case "GotTposPositions":
					SetStatus(StatusTypes.GotTPOSPositions, String.Concat(new object[] { "RDS|GotTposPositions = ", TposPositions.Count }));
					// Put all the tpos position into the main position collection
					GLOBAL.HPositions.Update(TposPositions);
					break;
				case "GotTposPositionsRefresh":
					AddToStatusLogs(String.Concat(new object[] { "RDS|GotTposPositionsRefresh = ", TposPositions.Count }));

					// Repopulate Positions Again.
					lock (GLOBAL.HPositions.TposItems)
					{
						if (GLOBAL.HPositions.TposItems.Count == 0 && TposPositions.Count > 0)
						{
							AddToStatusLogs(String.Concat(new object[] { "RDS|GotTposPositionsRefresh|Repopulate = ", TposPositions.Count }));
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
					SetStatus(Status, String.Concat(new object[] { "RDS|GotWarning ", ErrorMsg }), true);
					break;
				case "ErrorMsg":
				default:
					CheckExtendedTimer();
					SetStatus(StatusTypes.GotError, String.Concat(new object[] { "RDS|GotError ", ErrorMsg }), true);
					break;
			}
		}

		private void rds_OnSecurityEvent(object sender, SecurityEventArgs e)
		{
			NewSecufityInfo = true;
			//foreach (HelperMDS mds in GLOBAL.HMDSs)
			//{
			//    switch (mds.Status)
			//    {
			//        case HelperMDS.StatusTypes.Started:
			//            NewSecufityInfo = true;
			//            HelperSubscriber.Subscribe(e.Info.MDSymbol, e.Info.MDSource, e.Info.SecType);
			//            break;
			//        case HelperMDS.StatusTypes.Starting:
			//        case HelperMDS.StatusTypes.Stopped:
			//        case HelperMDS.StatusTypes.Stopping:
			//            SetStatus(StatusTypes.GotError, String.Concat(new object[] { "MDS|NotConnected" }));
			//            mds.Connect();
			//            break;
			//    }
			//}
		}

		private void rds_OnOptionChainEvent(object sender, OptionChainEventArgs e)
		{
			NewSecufityInfo = true;
			//if (GLOBAL.MDSsConnected)
			//{
			//    foreach (HelperMDS mds in GLOBAL.HMDSs)
			//    {
			//        switch (e.Info.SecType)
			//        {
			//            case CSVFieldIDs.SecutrityTypes.Future:
			//                SubscribeFullFutureOptionChain(e);
			//                break;
			//            default:
			//                SubScribeFullOptionChain(e);
			//                break;
			//        }
			//    }
			//}
		}

		private void rds_OnSSFutureChainEvent(object sender, SSFutureChainEventArgs e)
		{
			NewSecufityInfo = true;
			//if (GLOBAL.MDSsConnected)
			//{
			//    foreach (HelperMDS mds in GLOBAL.HMDSs)
			//    {
			//        mds.Subscribe(e.Info.MDSymbol, e.Info.MDSource, "", e.Info.SecType);
			//    }
			//}
		}

		#endregion

		#region - RDS Event Helper -

		private void SubscribeFullFutureOptionChain(OptionChainEventArgs e)
		{
			string[] chainSymbols = new string[e.Info.OptionChain.Count];
			e.Info.OptionChain.Keys.CopyTo(chainSymbols, 0);

			List<string> symbols = new List<string>();
			symbols.AddRange(chainSymbols);

			foreach (HelperMDS mds in GLOBAL.HMDSs)
			{
				mds.SubscribeToOptionChain(symbols, e.Info.MDSymbol, e.Info.MDSource, CSVFieldIDs.SecutrityTypes.OptionFuture);
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
				symbols.Add(string.Concat(new object[] { opraSymbol, ".", OptionExchangeCode.AMEX }));
				symbols.Add(string.Concat(new object[] { opraSymbol, ".", OptionExchangeCode.BOX }));
				symbols.Add(string.Concat(new object[] { opraSymbol, ".", OptionExchangeCode.CBOE }));
				symbols.Add(string.Concat(new object[] { opraSymbol, ".", OptionExchangeCode.C2 }));
				symbols.Add(string.Concat(new object[] { opraSymbol, ".", OptionExchangeCode.ISE }));
				symbols.Add(string.Concat(new object[] { opraSymbol, ".", OptionExchangeCode.NSDQ }));
				symbols.Add(string.Concat(new object[] { opraSymbol, ".", OptionExchangeCode.PCX }));
				symbols.Add(string.Concat(new object[] { opraSymbol, ".", OptionExchangeCode.PHLX }));
				nbboSymbols.Add(opraSymbol);
			}

			foreach (HelperMDS mds in GLOBAL.HMDSs)
			{
				mds.SubscribeToOptionChain(symbols, e.Info.MDSymbol, OptionDataSource.OPRA, CSVFieldIDs.SecutrityTypes.Option);
				mds.SubscribeToOptionChain(nbboSymbols, e.Info.MDSymbol, OptionDataSource.OPRANBBO, CSVFieldIDs.SecutrityTypes.Option);
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
