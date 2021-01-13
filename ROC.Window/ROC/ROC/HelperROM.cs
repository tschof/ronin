using System;
using System.Collections.Generic;
using System.Data;

using Common;
using ROMEx;
using SocketEx;
using DateTimeEx;
using RDSEx;
using CSVEx;
using System.Windows.Forms;

namespace ROC
{
	public class HelperROM : HelperStatusBase
	{
		public enum StatusTypes
		{
			None,

			Starting,
			Started,
			Stopping,
			Stopped,

			LoggingIn,
			LoggedIn,
			LogInFailed,

			Error,
		}

		#region - Property -

		private string _userName;
		public string UserName
		{
			get
			{
				return _userName;
			}
			set
			{
				_userName = value;
			}
		}

		private string _password;
		public string Password
		{
			get
			{
				return _password;
			}
			set
			{
				_password = value;
			}
		}

		private StatusTypes _status = StatusTypes.Stopped;
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

		private string _romMsgDsp = "";
		public string RomMsgDsp
		{
			get
			{
				return _romMsgDsp;
			}
			set
			{
				_romMsgDsp = value;
			}
		}

		private LockList<string> _rawMessages = new LockList<string>();
		private LockList<string> _romMsgLogs = new LockList<string>();
		private LockList<CSV> _newCriticalRomCSVs = new LockList<CSV>();
		private LockList<CSV> _newRomCSVs = new LockList<CSV>();
		private LockList<string> _criticalOrderIDs = new LockList<string>();
		private List<string> _canceledOrderIDs = new List<string>();
		private LockDictionary<string, CSV> _originalCSVs = new LockDictionary<string, CSV>();
		private LockDictionary<string, ROCOrder> _newOrders = new LockDictionary<string, ROCOrder>();
		private LockDictionary<string, ROCTrade> _newTrades = new LockDictionary<string, ROCTrade>();

		private DateTime _lastHartbeatTime = DateTime.Now;
		public DateTime LastHartbeatTime
		{
			get
			{
				return _lastHartbeatTime;
			}
		}
		
		#endregion

		#region - Local Variable -

		private bool _showInternalStatus = false;

		private ROMClient _rom;
		private ROMMessageMaker _romMessageMaker;
		public ROMMessageMaker RomMessageMaker
		{
			get
			{
				if (_romMessageMaker == null)
				{
					_romMessageMaker = new ROMMessageMaker();
				}
				return _romMessageMaker;
			}
		}

		private DateTimeHP _dtHP = new DateTimeHP();

		private long _endOfQueuedMsgCount = 0;
		public long EndOfQueuedMsgCount
		{
			get
			{
				return _endOfQueuedMsgCount;
			}
		}

		private bool _endOfQueuedMsg = false;
		public bool EndOfQueuedMsg
		{
			get
			{
				return _endOfQueuedMsg;
			}
			set
			{
				_endOfQueuedMsg = value;
			}
		}

		private bool _waitForQueueMsg = true;

		#endregion

		public HelperROM()
		{
			_waitForQueueMsg = Configuration.ROM.Default.WaitForQueueMsg;
		}

		public void Disconnect()
		{
			if (_rom != null)
			{
				SetStatus(StatusTypes.Stopped, string.Concat("ROM|Disconnect... From ", Configuration.ROM.Default.IP, " : ", Configuration.ROM.Default.Port));
				_rom.OnClientEvent -= new ClientEventHandler(rom_OnClientEvent);
				_rom.Stop();
			}
			_rom = null;
		}

		public void Connect()
		{
			if (Status != StatusTypes.Stopped)
			{
				Disconnect();
			}

			if (_rom == null)
			{
				_rom = new ROMClient(Configuration.ROM.Default.IP, Configuration.ROM.Default.Port, new byte[Configuration.ROM.Default.BufferSize], Configuration.ROM.Default.ClientBufferSizeLimit);
				_rom.OnClientEvent += new ClientEventHandler(rom_OnClientEvent);
			}

			while (Status != StatusTypes.Stopped) ;

			SetStatus(StatusTypes.Starting, string.Concat("ROM|Connecting... To ", Configuration.ROM.Default.IP, " : ", Configuration.ROM.Default.Port));
			_rom.Start();

			_showInternalStatus = Configuration.User.Default.ShowInternalStatus;
		}

		public void Reconnect()
		{
			Disconnect();

			Connect();

			if (!ConnectTimeOut())
			{
				Login(UserName, Password);
			}
		}

		private bool ConnectTimeOut()
		{
			DateTime startTime = DateTime.Now;
			bool connectTimeOut = false;
			while (Status != StatusTypes.Started)
			{
				TimeSpan span = DateTime.Now - startTime;
				if (span.Seconds > 3)
				{
					Status = StatusTypes.Stopped;
					connectTimeOut = true;
					break;
				}

				System.Threading.Thread.Sleep(100);
			}

			return connectTimeOut;
		}

		public void Login(string username, string password)
		{
			_userName = username;
			_password = password;

			SetStatus(StatusTypes.LoggingIn, string.Concat("ROM|Logging... For ", _userName));

			string msg = RomMessageMaker.GetLogin(_userName, _password, Configuration.ROM.Default.CancelOnDisconnect, Configuration.ROM.Default.SkipGTCandGTD);
			AddToRomLogs(msg);
			
			_rom.Send(msg);
		}

		public string GetOrderID()
		{
			return GetOrderID("");
		}
		public string GetOrderID(string note)
		{
			if (note == "")
			{
				return RomMessageMaker.GetOrderID(_userName);
			}
			else
			{
				return string.Concat(RomMessageMaker.GetOrderID(_userName), "-", note);
			}
		}

		public List<CSV> TakeNewCriticalRomCSVs()
		{
			return _newCriticalRomCSVs.TakeAll();
		}

		public List<CSV> TakeNewROMCSVs()
		{
			return _newRomCSVs.TakeAll();
		}

		public List<ROCOrder> TakeNewOrders()
		{
			Dictionary<string, ROCOrder> taken = _newOrders.TakeAll();
			if ((taken != null) && (taken.Count > 0)) {
				List<ROCOrder> result = new List<ROCOrder>();
				result.AddRange(taken.Values);
				return result;
			}
			return null;
		}

		public List<ROCTrade> TakeNewExecutions()
		{
			Dictionary<string, ROCTrade> taken = _newTrades.TakeAll();
			if ((taken != null) && (taken.Count > 0)) {
				List<ROCTrade> result = new List<ROCTrade>();
				result.AddRange(taken.Values);
				return result;
			}
			return null;
		}

		public bool FetchOrginalCSVs(string tag, out CSV found)
		{
			return _originalCSVs.TryGetValue(tag, out found);
		}

		public void RemoveCanceledOrderIDs(string tag)
		{
			_canceledOrderIDs.Remove(tag);
		}

		// Store the E messages because it has all the fields
		private void AddToOrginalOrders(CSV csv, string msg)
		{
			// New Orders
			if (_originalCSVs.TryAdd(csv.Tag, csv)) {
				// Display Sent Orders
				if (msg != "") {
					CSV add = new CSV(msg); // Create outside of lock.
					_newRomCSVs.Add(add);
				}
			} else {
				GLOBAL.HROC.AddToStatusLogs("Duplicated Tag" + csv.Tag);
			}
		}

		private void AddToCriticalOrders(CSV csv)
		{
			if (!_criticalOrderIDs.TryAdd(csv.Tag))
				GLOBAL.HROC.AddToStatusLogs("Duplicated Critical Tag" + csv.Tag);
		}

		public void EnterOrder(RomBasicOrder order, bool isCritical)
		{
			CSV csv = new CSV();

			switch (Convert.ToInt64(order.orderType))
			{
				case CSVEx.CSVFieldIDs.OrderTypes.MarketOnClose:
					order.duration = CSVEx.CSVFieldIDs.TIFCodes.Day.ToString();
					break;
				case CSVEx.CSVFieldIDs.OrderTypes.MarketAtOpen:
					order.orderType = CSVEx.CSVFieldIDs.OrderTypes.Market.ToString();
					order.duration = CSVEx.CSVFieldIDs.TIFCodes.OPG.ToString();
					break;
				case CSVEx.CSVFieldIDs.OrderTypes.OCO:
					order.orderType = CSVEx.CSVFieldIDs.OrderTypes.Limit.ToString();
					break;
			}

			string msg = RomMessageMaker.GetOrder(order, ref csv);

			// Store the E Msg
			if (_showInternalStatus)
			{
				AddToOrginalOrders(csv, msg);
			}
			else
			{
				AddToOrginalOrders(csv, "");
			}

			if (isCritical)
			{
				AddToCriticalOrders(csv);
			}

			AddToRomLogs(msg);

			_rom.Send(msg);
		}

		public void ReplaceOrder(string orderID, string newPrice)
		{
			ReplaceOrder(orderID, "", newPrice, "", "", "", false);
		}
		public void ReplaceOrder(string orderID, string newPrice, string newStopPrice)
		{
			ReplaceOrder(orderID, "", newPrice, newStopPrice, "", "", false);
		}
		public void ReplaceOrder(string orderID, string deltaShares, string newPrice, string newStopPrice, string newPegPrice, string newDuration)
		{
			ReplaceOrder(orderID, deltaShares, newPrice, newStopPrice, newPegPrice, newDuration, false);
		}
		public void ReplaceOrder(string orderID, string newShares, string newPrice, string newStopPrice)
		{
			// Used by AutoSpread Ticket
			ReplaceOrder(orderID, newShares, newPrice, newStopPrice, "", "", true);
		}
		public void ReplaceOrder(string orderID, string deltaShares, string newPrice, string newStopPrice, string newPegPrice, string newDuration, bool useDeltaSharesAsNewShares)
		{
			RomBasicReplace replace = new RomBasicReplace();

			try
			{
				if (GLOBAL.HOrders.RocItems.TryGetValue(orderID, out ROCOrder order))
				{
					replace.orderID = order.Tag;
					replace.trader = order.Trader;
					replace.side = order.Side.ToString();
					switch (order.SecType)
					{
						case CSVFieldIDs.SecurityTypes.Option:
						case CSVFieldIDs.SecurityTypes.OptionFuture:
							replace.symbol = order.SymbolDetail;
							break;
						default:
							replace.symbol = order.Symbol;
							break;
					}
					long newShares = 0;
					if (!useDeltaSharesAsNewShares)
					{
						if (deltaShares != "")
						{
							newShares = order.CumQty + order.LeaveQty + Convert.ToInt64(deltaShares);
						}
						else
						{
							newShares = order.CumQty + order.LeaveQty;
						}
					}
					else
					{
						// For AutoSpread Qty Replace
						newShares = Convert.ToInt64(deltaShares);
					}
					if (newShares <= 0)
					{
						// Not a valid replace
						return;
					}
					else
					{
						replace.newShares = Convert.ToString(newShares);
					}
					replace.newPrice = newPrice;
					replace.newStopPrice = newStopPrice;
					replace.newPagPrice = newPegPrice;
					replace.duration = newDuration;
					replace.tradeFor = order.TradeFor;

					GLOBAL.OrderManagers.ReplaceOrder(replace);
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		private void ReplaceOrder(RomBasicReplace replace)
		{
			string msg = RomMessageMaker.GetReplace(replace);
			AddToRomLogs(msg);

			_rom.Send(msg);
		}

		public void CancelSingleOrder(string orderID)
		{
			RomBasicCancel cancel = new RomBasicCancel();

			try
			{
				if (GLOBAL.HOrders.RocItems.TryGetValue(orderID, out ROCOrder order))
				{
					cancel.orderID = order.Tag;
					cancel.side = order.Side.ToString();
					cancel.mdSymbol = order.Symbol;
					cancel.shares = order.LeaveQty.ToString();
					if (order.OrderPrice.IsZero)
					{
						cancel.price = "";
					}
					else
					{
						cancel.price = order.OrderPrice.ToString();
					}

					GLOBAL.OrderManagers.CancelSingleOrder(cancel);
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		public void CancelSingleOrder(CSV csv)
		{
			RomBasicCancel cancel = new RomBasicCancel();

			try
			{
				cancel.orderID = csv.Tag;
				cancel.side = csv.Side.ToString();
				cancel.mdSymbol = csv.Symbol;
				cancel.shares = csv.Shares.ToString();
				if (csv.Price == 0)
				{
					cancel.price = "";
				}
				else
				{
					cancel.price = csv.Price.ToString();
				}

				GLOBAL.OrderManagers.CancelSingleOrder(cancel);
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		public void CancelAllBySide(string orderID, long side)
		{
			if (GLOBAL.HOrders.RocItems.TryGetValue(orderID, out ROCOrder order))
			{
				string orgSymbol = order.Symbol;
				DataRow[] rows;

				switch (side)
				{
					case CSVEx.CSVFieldIDs.SideCodes.Buy:
						if (!Configuration.User.Default.SkipGTCandGTDonAuto)
						{
							rows = GLOBAL.HOrders.Table.Select(
								string.Format("Symbol = '{0}' And ({1}) And ({2})",
										orgSymbol, 
										CSVFieldIDs.StatusCodes.ActiveStatusFilter, 
										CSVFieldIDs.SideCodes.BuySideFilter));
						}
						else
						{
							rows = GLOBAL.HOrders.Table.Select(
								string.Format("Symbol = '{0}' And ({1}) And ({2}) And ({3})",
										orgSymbol, 
										CSVFieldIDs.StatusCodes.ActiveStatusFilter, 
										CSVFieldIDs.SideCodes.BuySideFilter,
										CSVFieldIDs.TIFCodes.TIFDayFilter));
						}
						break;
					default:
						if (!Configuration.User.Default.SkipGTCandGTDonAuto)
						{
							rows = GLOBAL.HOrders.Table.Select(
								string.Format("Symbol = '{0}' And ({1}) And ({2})",
										orgSymbol, 
										CSVFieldIDs.StatusCodes.ActiveStatusFilter, 
										CSVFieldIDs.SideCodes.SellSideFilter));
						}
						else
						{
							rows = GLOBAL.HOrders.Table.Select(
								string.Format("Symbol = '{0}' And ({1}) And ({2}) And ({3})",
										orgSymbol, 
										CSVFieldIDs.StatusCodes.ActiveStatusFilter, 
										CSVFieldIDs.SideCodes.SellSideFilter,
										CSVFieldIDs.TIFCodes.TIFDayFilter));
						}
						break;
				}

				foreach (DataRow row in rows)
				{
					if (row["Tag"] != null)
					{
						GLOBAL.OrderManagers.CancelSingleOrder(row["Tag"].ToString());
					}
				}
			}
		}

		public void CancelBySymbol(string orderID)
		{
			if (GLOBAL.HOrders.RocItems.TryGetValue(orderID, out ROCOrder order))
			{
				string orgSymbolDetail = order.SymbolDetail;
				DataRow[] rows;

				if (!Configuration.User.Default.SkipGTCandGTDonAuto)
				{
					rows = GLOBAL.HOrders.Table.Select(
						string.Format("SymbolDetail = '{0}' And ({1})",
								orgSymbolDetail, 
								CSVFieldIDs.StatusCodes.ActiveStatusFilter));
				}
				else
				{
					rows = GLOBAL.HOrders.Table.Select(
						string.Format("SymbolDetail = '{0}' And ({1}) And ({2})",
								orgSymbolDetail, 
								CSVFieldIDs.StatusCodes.ActiveStatusFilter,
								CSVFieldIDs.TIFCodes.TIFDayFilter));
				}

				foreach (DataRow row in rows)
				{
					if (row["Tag"] != null)
					{
						GLOBAL.OrderManagers.CancelSingleOrder(row["Tag"].ToString());
					}
				}
			}
		}

		public void CancelSingleOrder(RomBasicCancel cancel)
		{
			bool okToSend = false;

			switch (cancel.orderID.Substring(cancel.orderID.Length - 1, 1))
			{
				case "M":
				case "S":
					if (!_canceledOrderIDs.Contains(cancel.orderID))
					{
						okToSend = true;
					}
					break;
				default:
					okToSend = true;
					break;
			}

			if (okToSend)
			{
				string msg = RomMessageMaker.GetCancel(_userName, cancel);
				AddToRomLogs(msg);

				_rom.Send(msg);

				if (!_canceledOrderIDs.Contains(cancel.orderID))
				{
					_canceledOrderIDs.Add(cancel.orderID);
				}
			}
		}

		// Done By ROM
		public void CancelAllOrders(bool dayOrderOnly, string exchange)
		{
			// This will cancel all order by ROM with little c, include GTD and GTC
			string msg = RomMessageMaker.GetCancelAll(_userName, dayOrderOnly, exchange);
			AddToRomLogs(msg);

			_rom.Send(msg);
		}

		public bool HasOpenOrders()
		{
			DataRow[] rows = new DataRow[0];
			if (!Configuration.User.Default.SkipGTCandGTDonAuto)
			{
				rows = GLOBAL.HOrders.Table.Select(
					string.Format("{0}", CSVFieldIDs.StatusCodes.ActiveStatusFilter));
			}
			else
			{
				rows = GLOBAL.HOrders.Table.Select(
					string.Format("{0} And {1}", 
							CSVFieldIDs.StatusCodes.ActiveStatusFilter,
							CSVFieldIDs.TIFCodes.TIFDayFilter));
			}

			if (rows.Length > 0)
			{
				return true;
			}

			return false;
		}

		public void CancelAllOpenOrders(string type)
		{
			DataRow[] rows = new DataRow[0];
			if (!Configuration.User.Default.SkipGTCandGTDonAuto)
			{
				switch (type)
				{
					case CSVFieldIDs.SecurityTypes.Future:
						rows = GLOBAL.HOrders.Table.Select(
							string.Format("{0} And {1}",
									CSVFieldIDs.StatusCodes.ActiveStatusFilter,
									CSVFieldIDs.SecurityTypes.FutureFilter));
						break;
					case CSVFieldIDs.SecurityTypes.Option:
						rows = GLOBAL.HOrders.Table.Select(
							string.Format("{0} And {1}",
									CSVFieldIDs.StatusCodes.ActiveStatusFilter,
									CSVFieldIDs.SecurityTypes.OptionFilter));
						break;
					case CSVFieldIDs.SecurityTypes.Equity:
						rows = GLOBAL.HOrders.Table.Select(
							string.Format("{0} And {1}",
									CSVFieldIDs.StatusCodes.ActiveStatusFilter,
									CSVFieldIDs.SecurityTypes.EquityFilter));
						break;
					default:
						rows = GLOBAL.HOrders.Table.Select(
							string.Format("{0}",
									CSVFieldIDs.StatusCodes.ActiveStatusFilter));
						break;
				}
			}
			else
			{
				// Day Order Only
				switch (type)
				{
					case CSVFieldIDs.SecurityTypes.Future:
						rows = GLOBAL.HOrders.Table.Select(
							string.Format("{0} And {1} And {2}",
									CSVFieldIDs.StatusCodes.ActiveStatusFilter,
									CSVFieldIDs.SecurityTypes.FutureFilter,
									CSVFieldIDs.TIFCodes.TIFDayFilter));
						break;
					case CSVFieldIDs.SecurityTypes.Option:
						rows = GLOBAL.HOrders.Table.Select(
							string.Format("{0} And {1} And {2}",
									CSVFieldIDs.StatusCodes.ActiveStatusFilter,
									CSVFieldIDs.SecurityTypes.OptionFilter,
									CSVFieldIDs.TIFCodes.TIFDayFilter));
						break;
					case CSVFieldIDs.SecurityTypes.Equity:
						rows = GLOBAL.HOrders.Table.Select(
							string.Format("{0} And {1} And {2}",
									CSVFieldIDs.StatusCodes.ActiveStatusFilter,
									CSVFieldIDs.SecurityTypes.EquityFilter,
									CSVFieldIDs.TIFCodes.TIFDayFilter));
						break;
					default:
						rows = GLOBAL.HOrders.Table.Select(
							string.Format("{0} And {1}",
									CSVFieldIDs.StatusCodes.ActiveStatusFilter,
									CSVFieldIDs.TIFCodes.TIFDayFilter));
						break;
				}
			}

			foreach (DataRow row in rows)
			{
				if (row["Tag"] != null)
				{
					GLOBAL.OrderManagers.CancelSingleOrder(row["Tag"].ToString());
				}
			}
		}

		public void Hartbeat()
		{
			string msg = RomMessageMaker.GetHartbeat();
			AddToRomLogs(msg);

			_rom.Send(msg);
		}

		private void rom_OnClientEvent(object sender, ClientEventArgs e)
		{
			switch (e.Type)
			{
				case ClientEventTypes.OnConnect:
					SetStatus(StatusTypes.Started, string.Join(" ", "ROM|OnConnect", e.Message, "From", e.RemoteEndPoint));
					break;
				case ClientEventTypes.OnDisconnect:
					if (Status == StatusTypes.LoggedIn)
					{
						SetStatus(StatusTypes.Stopped, string.Join(" ", "ROM|OnDisconnect", e.Message, "From", e.RemoteEndPoint), true);
					}
					else
					{
						SetStatus(StatusTypes.Stopped, string.Join(" ", "ROM|OnDisconnect", e.Message, "From", e.RemoteEndPoint));
					}
					Disconnect();
					StopHartbeat();
					break;
				case ClientEventTypes.OnRawReceive:
					break;
				case ClientEventTypes.OnReceive:
					_rawMessages.AddRange(e.Messages);
					break;
				case ClientEventTypes.OnDebug:
					_romMsgLogs.Add(_dtHP.Now.ToString("HH:mm:ss.fffffff") + " " + e.Message);
					break;
				case ClientEventTypes.OnSend:
					break;
				case ClientEventTypes.OnError:
				default:
					if (e.Client == null)
					{
						SetStatus(StatusTypes.Error, "ROM|OnError " + e.Message, true);
					}
					else
					{
						SetStatus(StatusTypes.Error, string.Join(" ", "ROM|OnError", e.Message, "From", e.RemoteEndPoint), true);
					}
					Disconnect();
					StopHartbeat();
					break;
			}
		}

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
				AddToAlerts(statusDsp);
			}
		}

		#endregion

		#region - ROM Message Process -

		public void AddToCSVs()
		{
			List<string> locRawMessages = _rawMessages.TakeAll();
			if (locRawMessages.Count > 0)
				AddToCSVs(locRawMessages);
		}

		private void AddToCSVs(IEnumerable<string> messages)
		{
			CSV csv;
			
			_lastHartbeatTime = DateTime.Now;

			foreach (string message in messages)
			{
				csv = new CSV(message);
				switch (csv.Command)
				{
					case CSVFieldIDs.MessageTypes.HeartBeat:
						break;
					default:
						// Log the mssage to .rom if its not a heart beat
						AddToRomLogs(message);
						switch (csv.Command)
						{
							case CSVFieldIDs.MessageTypes.LoggedIn:
								Status = StatusTypes.LoggedIn;

								// Log the event to .log
								StatusDsp = string.Concat("ROM|LoggedIn ", _userName);
								AddToStatusLogs(StatusDsp);

								RomMsgDsp = StatusDsp;

								if (!_waitForQueueMsg)
								{
									_endOfQueuedMsg = true;

									StatusDsp = string.Concat("ROM|EndOfQueuedMsg ", _endOfQueuedMsgCount);
									AddToStatusLogs(StatusDsp);
								}
								break;
							case CSVFieldIDs.MessageTypes.LoginFailed:
								Status = StatusTypes.LogInFailed;

								// Log the event to .log
								StatusDsp = string.Concat("ROM|LoginFailed For ", _userName, " : ", csv.Text);
								AddToStatusLogs(StatusDsp);

								RomMsgDsp = StatusDsp;

								//AddToAlerts(RomMsgDsp);
								break;
							case CSVFieldIDs.MessageTypes.Alert:
								string[] temp = csv.Text.Split(new char[] { '|' }, StringSplitOptions.None);
								
								if (temp.Length != 2) // MDS vs MDS backup was removed.
								{
									RomMsgDsp = string.Concat("ROM|Alert ", csv.Text);
									AddToAlerts(RomMsgDsp);
								}
								break;
							case CSVFieldIDs.MessageTypes.InvalidField:
								RomMsgDsp = string.Concat("ROM|InvalidField ", csv.Text);
								AddToAlerts(RomMsgDsp);
								break;
							case CSVFieldIDs.MessageTypes.WorkingOrder:
							case CSVFieldIDs.MessageTypes.OrderStatus:
								UpdateQueuedMsgCount();

								// Store the E Msg
								if (csv.Command == CSVFieldIDs.CommandTypes.EnterOrder)
								{
									AddToOrginalOrders(csv, "");
								}

								// Store the Manual Orders before the Batch Orders
								if (_criticalOrderIDs.Has(csv.Tag))
								{
									_newCriticalRomCSVs.Add(csv);
									break;
								}

								_newRomCSVs.Add(csv);
								break;
							case CSVFieldIDs.MessageTypes.EndOfQueuedMsg:
								_endOfQueuedMsg = true;

								StatusDsp = string.Concat("ROM|EndOfQueuedMsg ", _endOfQueuedMsgCount);
								AddToStatusLogs(StatusDsp);
								break;
							default:
								break;
						}
						break;
				}
			}
		}

		#endregion

		#region - Update QueuedMsgCount -

		private void UpdateQueuedMsgCount()
		{
			if (!_endOfQueuedMsg)
			{
				if (_endOfQueuedMsgCount == 0)
				{
					StatusDsp = string.Concat("ROM|FirstROMMessage ", _endOfQueuedMsgCount);
				}

				_endOfQueuedMsgCount = _endOfQueuedMsgCount + 1;
			}
		}

		#endregion

		#region - ROM Message Log -

		private void AddToRomLogs(string romMsg)
		{
			_romMsgLogs.Add(string.Concat(_dtHP.Now.ToString("HH:mm:ss.fffffff"), " ", romMsg));
		}

		public void LogRomMessages()
		{
			List<string> msgs = _romMsgLogs.TakeAll();
			foreach (string msg in msgs)
				Common.Log.Info(Common.Log.Target.ROM, msg);
		}

		#endregion

		#region - Update -

		public void Update(ROCOrder order, bool addToNew)
		{
			if (addToNew)
				_newOrders.Add(order.Tag, order);
		}

		public void Update(ROCTrade trade, bool addToNew)
		{
			if (addToNew)
				_newTrades.Add(trade.TradeID, trade);
		}

		#endregion

		#region - ROM Hart Beat -

		private Timer _hearbeat;
		private int _hartBeatRate = Configuration.ROM.Default.HartBeatRate;

		public void StartHartbeat()
		{
			if (_hearbeat == null)
			{
				_hearbeat = new Timer();
				_hearbeat.Interval = _hartBeatRate * 1000;
				_hearbeat.Tick += new EventHandler(Hearbeat_Tick);
				_hearbeat.Enabled = true;
			}
			_hearbeat.Start();
		}

		private void Hearbeat_Tick(object sender, EventArgs e)
		{
			Hartbeat();
		}

		public void StopHartbeat()
		{
			if (_hearbeat != null)
			{
				_hearbeat.Stop();
				_hearbeat.Tick -= new EventHandler(Hearbeat_Tick);
			}
			_hearbeat = null;
		}

		public void HartbeatTimeOutCheck()
		{
			if (_rom != null)
			{
				TimeSpan diff = DateTime.Now.Subtract(GLOBAL.OrderManagers.LastHartbeatTime);
				if (diff.TotalSeconds > _hartBeatRate * 2)
				{
					SetStatus(StatusTypes.Error, string.Concat("ROM|HartbeatTimeOut : " + diff.TotalSeconds), true);
					Disconnect();
					StopHartbeat();
				}
			}
			else
			{
				_lastHartbeatTime = DateTime.Now;
			}
		}

		#endregion
	}
}
