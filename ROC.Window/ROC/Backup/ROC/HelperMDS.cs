using System;
using System.ComponentModel;
using System.Collections.Generic;

using SocketEx;
using DateTimeEx;
using MarketDataEx;
using SerializationEx;
using System.Diagnostics;
using System.Windows.Forms;

namespace ROC
{
	public class HelperMDS : HelperStatusBase
	{
		public enum StatusTypes
		{
			None,
			Starting,
			Started,
			Stopping,
			Stopped,
		}

		#region - Property -

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

		private Dictionary<string, MDServerToClient> _latestDelta;
		public Dictionary<string, MDServerToClient> LatestDelta
		{
			get
			{
				if (_latestDelta == null)
				{
					_latestDelta = new Dictionary<string, MDServerToClient>();
				}
				return _latestDelta;
			}
			set
			{
				_latestDelta = value;
			}
		}

		private string _serverIP = "";
		public string ServerIP
		{
			get
			{
				return _serverIP;
			}
		}

		private int _serverPort = 0;
		public int ServerPort
		{
			get
			{
				return _serverPort;
			}
		}

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

		private ToBinary _toBinary = new ToBinary();
		private ToBinary _fromBinary = new ToBinary();

		private DateTimeHP _dtHP;

		private MDSClient _mds;

		private MDClientToServer _loginMsg;
		private MDClientToServer _hartbeatMsg;

		#endregion

		public HelperMDS(string serverIP, int serverPort)
		{
			_serverIP = serverIP;
			_serverPort = serverPort;
			_dtHP = new DateTimeHP();
		}

		private void mds_OnClientEvent(object sender, ClientEventArgs e)
		{
			switch (e.Type)
			{
				case ClientEventTypes.OnConnect:
					SetStatus(StatusTypes.Started, String.Concat(new object[] { "MDS|OnConnect ", e.Message, " From ", e.Client.LocalEndPoint.ToString() }));

					Login();
					break;
				case ClientEventTypes.OnDisconnect:
					SetStatus(StatusTypes.Stopped, String.Concat(new object[] { "MDS|OnDisconnect ", e.Message, " From ", e.Client.LocalEndPoint.ToString() }));
					break;
				case ClientEventTypes.OnRawReceive:
					break;
				case ClientEventTypes.OnReceive:
					if (e.ByteMessage != null && e.ByteMessage.Length > 0)
					{
						ConvertFromInput(e.ByteMessage);
					}
					break;
				case ClientEventTypes.OnSend:
				case ClientEventTypes.OnDebug:
					break;
				case ClientEventTypes.OnError:
				default:
					if (e.Client == null)
					{
						SetStatus(StatusTypes.Stopping, String.Concat(new object[] { "MDS|OnError ", e.Message, }), true);
					}
					else
					{
						SetStatus(StatusTypes.Stopping, String.Concat(new object[] { "MDS|OnError ", e.Message, " From ", e.Client.LocalEndPoint.ToString() }), true);
					}
					_mds.Stop();
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

		#region - Login -

		public void Login()
		{
			if (_mds != null && _mds.Connected)
			{
				// Send User Info
				if (GLOBAL.HROM.Status == HelperROM.StatusTypes.LoggedIn && _loginMsg == null)
				{
					_loginMsg = new MDClientToServer(GLOBAL.HROM.UserName, GLOBAL.HROM.Password);
				}
				else
				{
					_loginMsg = null;
				}

				if (_loginMsg != null)
				{
					_mds.SendRawBytes(ConvertToOutput(_loginMsg));
				}
			}
		}

		#endregion

		#region - Hartbeat -

		public void Hartbeat()
		{
			if (_mds != null && _mds.Connected)
			{
				// Send Hartbeat
				if (GLOBAL.HROM.Status == HelperROM.StatusTypes.LoggedIn && _hartbeatMsg == null)
				{
					_hartbeatMsg = new MDClientToServer(GLOBAL.HROM.UserName, GLOBAL.HROM.Password);
				}
				else
				{
					_hartbeatMsg = null;
				}

				if (_hartbeatMsg != null)
				{
					_lastHartbeatTime = DateTime.Now;
					_mds.SendRawBytes(ConvertToOutput(_hartbeatMsg));
				}
			}
		}

		#endregion

		#region - Connect & Disconnect -

		public void Disconnect()
		{
			if (_mds != null)
			{
				SetStatus(StatusTypes.Stopped, String.Concat(new object[] { "MDS|Disconnect... From ", Configuration.MDS.Default.IP, " : ", Configuration.MDS.Default.Port }));
				_mds.Stop();

				_mds.OnClientEvent -= new ClientEventHandler(mds_OnClientEvent);
			}
			_mds = null;
		}

		public void Connect()
		{
			if (Status != StatusTypes.Stopped)
			{
				Disconnect();
			}

			if (_mds == null)
			{
				_mds = new MDSClient(_serverIP, _serverPort, new byte[Configuration.MDS.Default.BufferSize], Configuration.MDS.Default.ClientBufferSizeLimit);
				_mds.OnClientEvent += new ClientEventHandler(mds_OnClientEvent);
			}

			if (!DisconnectTimeOut())
			{
				SetStatus(StatusTypes.Starting, String.Concat(new object[] { "MDS|Connecting... To ", Configuration.MDS.Default.IP, " : ", Configuration.MDS.Default.Port }));
				_mds.Start();
			}
		}

		public void Reconnect()
		{
			Disconnect();

			Connect();

			if (!ConnectTimeOut())
			{
				Resubscribe();
			}
		}

		private bool DisconnectTimeOut()
		{
			DateTime startTime = DateTime.Now;
			bool disconnectTimeOut = false;
			while (Status != StatusTypes.Stopped)
			{
				TimeSpan span = DateTime.Now - startTime;
				if (span.Seconds > 3)
				{
					Status = StatusTypes.Stopped;
					disconnectTimeOut = true;
					break;
				}

				System.Threading.Thread.Sleep(100);
			}

			return disconnectTimeOut;
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

		#endregion

		#region - Subscribe & Unsubscribe -

		//private static List<string> _symbols = new List<string>();

		public void Subscribe(string symbol, string source, string subscriptionType, string secType)
		{
			if (_mds != null && _mds.Connected)
			{
				switch (secType)
				{
					case CSVEx.CSVFieldIDs.SecutrityTypes.Equity:
						if (symbol.Contains("/P"))
						{
							symbol = symbol.Replace("/P", "p");
						}
						else if (symbol.Contains("/RT"))
						{
							symbol = symbol.Replace("/RT", "r");
						}
						else if (symbol.Contains("/RTWI"))
						{
							symbol = symbol.Replace("/RTWI", "rw");
						}
						break;
				}

				UpdateResubscribeList(symbol, source, subscriptionType, secType);

				if (!GLOBAL.HMarketData.Current.ContainsKey(symbol))
				{
					_mds.SendRawBytes(ConvertToOutput(new MDClientToServer("S", symbol, source, subscriptionType, secType, false)));
				}
				else
				{
					// Play Back
					switch (subscriptionType)
					{
						case "L2":
							GLOBAL.HProcess.AddToReplayList(symbol);
							foreach (MDServerToClient data in GLOBAL.HMarketData.Current.Values)
							{
								if (data.uSymbol == symbol && data.uParticipentSymbol != "")
								{
									GLOBAL.HProcess.AddToReplayList(data.IssueSymbol);
								}
							}
							break;
						default:
							GLOBAL.HProcess.AddToReplayList(symbol);
							break;
					}
				}
			}
		}

		public void SubscribeToOptionChain(List<string> symbols, string baseSymbol, string source, string type)
		{
			if (_mds != null && _mds.Connected)
			{
				if (!GLOBAL.HMarketData.Current.ContainsKey(baseSymbol))
				{
					MDClientToServer mdcs = new MDClientToServer("S", baseSymbol, source, type, false);
					_mds.SendRawBytes(ConvertToOutput(mdcs));
				}
				else
				{
					GLOBAL.HProcess.AddToReplayList(baseSymbol);
				}

				foreach (string symbol in symbols)
				{
					if (!GLOBAL.HMarketData.Current.ContainsKey(symbol))
					{
						MDClientToServer mdcs = new MDClientToServer("S", symbol, source, type, false);
						_mds.SendRawBytes(ConvertToOutput(mdcs));
					}
					else
					{
						GLOBAL.HProcess.AddToReplayList(symbol);
					}
				}
			}
		}

		public void Unsubscribe(string symbol)
		{
			if (_mds != null && _mds.Connected)
			{
				_mds.SendRawBytes(ConvertToOutput(new MDClientToServer(symbol, false)));
			}
		}

		#endregion

		#region - ReSubscribe -

		private Dictionary<string, MDClientToServer> _reSubscribeList = new Dictionary<string, MDClientToServer>();
		private string _keyFormat = "{0},{1},{2},{3}";

		private void UpdateResubscribeList(string symbol, string source, string subscriptionType, string secType)
		{
			string key = string.Format(_keyFormat, symbol, source, subscriptionType, secType);

			lock (_reSubscribeList)
			{
				if (!_reSubscribeList.ContainsKey(key))
				{
					_reSubscribeList.Add(key, new MDClientToServer("S", symbol, source, subscriptionType, secType, false));
				}
			}
		}

		private void Resubscribe()
		{
			if (_mds != null && _mds.Connected)
			{
				Login();

				if (_reSubscribeList.Count > 0)
				{
					Dictionary<string, MDClientToServer> locReSubscribeList = new Dictionary<string, MDClientToServer>();
					lock (_reSubscribeList)
					{
						locReSubscribeList = new Dictionary<string, MDClientToServer>(_reSubscribeList);
					}

					foreach (MDClientToServer reSub in locReSubscribeList.Values)
					{
						_mds.SendRawBytes(ConvertToOutput(reSub));
					}
				}
			}
		}

		// Re-subscribe by MDS Cache File
		public void ReSubscribe(string symbol, string source, string subscriptionType, string secType)
		{
			if (_mds != null && _mds.Connected)
			{
				_mds.SendRawBytes(ConvertToOutput(new MDClientToServer("S", symbol, source, subscriptionType, secType, true)));
			}
		}

		#endregion

		#region - Input & Output -

		public byte[] ConvertToOutput(MDClientToServer toClient)
		{
			return _toBinary.Serialize(toClient, SerializationTypes.Normal);
		}

		public void ConvertFromInput(byte[] fromServer)
		{
			if (fromServer != null && fromServer.Length > 0)
			{
				try
				{
					//byte[] bytes = System.Convert.FromBase64String(fromServer);
					MDServerToClient changes = (MDServerToClient)new ToBinary().Deserialize(fromServer, SerializationTypes.Normal);
					if (changes != null)
					{
						StoreChanges(changes);
					}
				}
				catch (Exception ex)
				{
					GLOBAL.HROC.AddToException(ex);
				}
			}
		}

		private void StoreChanges(MDServerToClient changes)
		{
			string issueSymbol = "";

			if (changes.Strings.ContainsKey(WombatFieldIDs.IssueSymbol))
			{
			    issueSymbol = changes.Strings[WombatFieldIDs.IssueSymbol];
			}

			if (issueSymbol != "")
			{
				if (issueSymbol.Contains("p"))
				{
					issueSymbol = issueSymbol.Replace("p", "/PR");
					changes.Update(WombatFieldIDs.IssueSymbol, issueSymbol);
				}
				else if (issueSymbol.Contains("r"))
				{
					issueSymbol = issueSymbol.Replace("r", "/RT");
					changes.Update(WombatFieldIDs.IssueSymbol, issueSymbol);
				}
				else if (issueSymbol.Contains("rw"))
				{
					issueSymbol = issueSymbol.Replace("rw", "/RTWI");
					changes.Update(WombatFieldIDs.IssueSymbol, issueSymbol);
				}

				lock (LatestDelta)
				{
					if (LatestDelta.ContainsKey(issueSymbol))
					{
						LatestDelta[issueSymbol].Update(changes);
					}
					else
					{
						LatestDelta.Add(issueSymbol, new MDServerToClient(changes));
					}
				}
			}

			//StatusMsg = String.Concat(new object[] { "Wombat Msg: ", issueSymbol });
		}

		#endregion

		#region - MDS Hart Beat -

		private Timer _hartbeat;
		private int _hartBeatRate = Configuration.MDS.Default.HartBeatRate;

		public void StartHartbeat()
		{
			if (_hartbeat == null)
			{
				_hartbeat = new Timer();
				_hartbeat.Interval = _hartBeatRate * 1000;
				_hartbeat.Tick += new EventHandler(Hearbeat_Tick);
				_hartbeat.Enabled = true;
			}
			_hartbeat.Start();
		}

		private void Hearbeat_Tick(object sender, EventArgs e)
		{
			Hartbeat();
		}

		public void StopHartbeat()
		{
			if (_hartbeat != null)
			{
				_hartbeat.Stop();
				_hartbeat.Tick -= new EventHandler(Hearbeat_Tick);
			}
			_hartbeat = null;
		}

		public void HartbeatTimeOutCheck()
		{
			if (_mds != null && _mds.Connected && _hartbeat != null)
			{
				TimeSpan diff = DateTime.Now.Subtract(GLOBAL.HROM.LastHartbeatTime);
				if (diff.TotalSeconds > _hartBeatRate * 4)
				{
					SetStatus(StatusTypes.Stopped, String.Concat(new object[] { "MDS|HartbeatTimeOut : " + diff.TotalSeconds }), true);
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
