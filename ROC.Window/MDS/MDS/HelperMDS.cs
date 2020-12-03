using System;
using System.Collections.Generic;
using System.Net.Sockets;

using SocketEx;
using System.ComponentModel;
using DateTimeEx;
using MarketDataEx;
using SerializationEx;
using System.Runtime.Serialization.Formatters.Binary;
using System.IO;
using System.Diagnostics;
using CSVEx;

namespace MDS
{
	public class HelperMDS : HelperStatusBase
	{
		public enum StatusTypes
		{
			Starting,
			Started,
			Stopping,
			Stopped,
		}

		#region - Property -

		private bool _statusChanged = true;
		public bool StatusChanged
		{
			get
			{
				return _statusChanged;
			}
			set
			{
				_statusChanged = value;
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
					StatusChanged = true;
				}
			}
		}

		private string _lastReceivedMsg = "";
		public string LastReceivedMsg
		{
			get
			{
				return _lastReceivedMsg;
			}
			set
			{
				_lastReceivedMsg = value;
			}
		}

		private List<string> _receivedMsgs;
		public List<string> ReceivedMsgs
		{
			get
			{
				if (_receivedMsgs == null)
				{
					_receivedMsgs = new List<string>();
				}
				return _receivedMsgs;
			}
			set
			{
				_receivedMsgs = value;
			}
		}

		private string _lastSentMsg = "";
		public string LastSentMsg
		{
			get
			{
				return _lastSentMsg;
			}
			set
			{
				_lastSentMsg = value;
			}
		}

		private List<string> _sentMsgs;
		public List<string> SentMsgs
		{
			get
			{
				if (_sentMsgs == null)
				{
					_sentMsgs = new List<string>();
				}
				return _sentMsgs;
			}
			set
			{

				_sentMsgs = value;
			}
		}

		private List<string> _mdsLogs;
		public List<string> MDSLogs
		{
			get
			{
				if (_mdsLogs == null)
				{
					_mdsLogs = new List<string>();
				}
				return _mdsLogs;
			}
			set
			{
				_mdsLogs = value;
			}
		}

		// List of SymbolMap = Symbol + Source
		private Dictionary<IntPtr, List<string>> _clientToSymbolMap;
		public Dictionary<IntPtr, List<string>> ClientToSymbolMap
		{
			get
			{
				if (_clientToSymbolMap == null)
				{
					_clientToSymbolMap = new Dictionary<IntPtr, List<string>>();
				}
				return _clientToSymbolMap;
			}
			set
			{
				_clientToSymbolMap = value;
			}
		}

		// Key = Symbol + Source
		private Dictionary<string, List<IntPtr>> _symbolToClientList;
		public Dictionary<string, List<IntPtr>> SymbolToClientList
		{
			get
			{
				if (_symbolToClientList == null)
				{
					_symbolToClientList = new Dictionary<string, List<IntPtr>>();
				}
				return _symbolToClientList;
			}
			set
			{
				_symbolToClientList = value;
			}
		}

		private Dictionary<IntPtr, MDClientToServer> _clientToUserMap;
		public Dictionary<IntPtr, MDClientToServer> ClientToUserMap
		{
			get
			{
				if (_clientToUserMap == null)
				{
					_clientToUserMap = new Dictionary<IntPtr, MDClientToServer>();
				}
				return _clientToUserMap;
			}
			set
			{
				_clientToUserMap = value;
			}
		}

		private Dictionary<IntPtr, DateTime> _clientHartbeatMap;
		public Dictionary<IntPtr, DateTime> ClientHartbeatMap
		{
			get
			{
				if (_clientHartbeatMap == null)
				{
					_clientHartbeatMap = new Dictionary<IntPtr, DateTime>();
				}
				return _clientHartbeatMap;
			}
			set
			{
				_clientHartbeatMap = value;
			}
		}

		public Dictionary<IntPtr, SocketBaseClient> Clients
		{
			get
			{
				if (_mds != null)
				{
					lock (_mds.Clients)
					{
						return new Dictionary<IntPtr, SocketBaseClient>(_mds.Clients);
					}
				}
				return new Dictionary<IntPtr, SocketBaseClient>();
			}
		}

		public Dictionary<IntPtr, List<string>> Buffer
		{
			get
			{
				if (_mds != null)
				{
					lock (_mds.SendByClientMsgs)
					{
						return new Dictionary<IntPtr, List<string>>(_mds.SendByClientMsgs);
					}
				}
				return new Dictionary<IntPtr, List<string>>();
			}
		}

		// Check to be safe
		private List<IntPtr> _disconnectedClients;
		public List<IntPtr> DisconnectedClients
		{
			get
			{
				if (_disconnectedClients == null)
				{
					_disconnectedClients = new List<IntPtr>();
				}
				return _disconnectedClients;
			}
			set
			{
				_disconnectedClients = value;
			}
		}

		#endregion

		#region - Local Variable -

		private ToBinary _toBinary = new ToBinary();
		private ToBinary _fromBinary = new ToBinary();

		private DateTime _pTime = DateTime.Now;
		private TimeSpan _pTimeSpan = new TimeSpan();

		private bool _logPerformance = false; 

		private DateTimeHP _dtHP = new DateTimeHP();

		private MDSServer _mds;

		private object _syncObj = new object();

		private int _recapLimit = 10;

		private char[] _keySpliter = new char[] { ',' };

		// Key = Symbol + Source
		private Dictionary<string, List<IntPtr>> _symbolToRecapList;
		public Dictionary<string, List<IntPtr>> SymbolToRecapList
		{
			get
			{
				if (_symbolToRecapList == null)
				{
					_symbolToRecapList = new Dictionary<string, List<IntPtr>>();
				}
				return _symbolToRecapList;
			}
			set
			{
				_symbolToRecapList = value;
			}
		}

		#endregion

		public HelperMDS()
		{
			_logPerformance = Configuration.User.Default.LogPerformance;
			_recapLimit = Configuration.MDS.Default.RecapLimit;
		}

		private void mds_OnServerEvent(object sender, ServerEventArgs e)
		{
			switch (e.Type)
			{
				case ServerEventTypes.OnListenerStart:
					SetStatus(StatusTypes.Started, String.Concat(new object[] { "Server|OnListenerStart At ", Configuration.MDS.Default.IP, " Port: ", Configuration.MDS.Default.Port }));
					break;
				case ServerEventTypes.OnListenerStop:
					SetStatus(StatusTypes.Stopped, String.Concat(new object[] { "Server|OnListenerStop At ", Configuration.MDS.Default.IP, " Port: ", Configuration.MDS.Default.Port }));
					break;
				case ServerEventTypes.OnListenerAccept:
					if (e.Client != null)
					{
						SetStatus(StatusTypes.Started, String.Concat(new object[] { "Server|OnListenerAccept Soket ", e.Message, " From ", e.LocalEndPoint }));
						AddClient(e.Client);
					}
					break;
				case ServerEventTypes.OnRawReceive:
					break;
				case ServerEventTypes.OnReceive:
					DecodeReceivedMsg(e);
					break;
				case ServerEventTypes.OnSend:
					break;
				case ServerEventTypes.OnDisconnect:
					try
					{
						SetStatus(StatusTypes.Started,
							string.Format("Server|OnDisconnect {0} LocalEndPoint {1} RemoteEndPoint {2}",
							new object[] { e.Message, e.LocalEndPoint, e.RemoteEndPoint }));

						AddToCleanUpList(e.Client.Handle);
						//ProcessCleanUp(e.Client.Handle);
					}
					catch (Exception ex)
					{
						GLOBAL.AddToExecption(ex);
					} 
					break;
				case ServerEventTypes.OnError:
					try
					{
						SetStatus(StatusTypes.Started, 
							string.Format("Server|OnError {0} LocalEndPoint {1} RemoteEndPoint {2}", 
							new object[] { e.Message, e.LocalEndPoint, e.RemoteEndPoint }));

						AddToCleanUpList(e.Client.Handle);
						//ProcessCleanUp(e.Client.Handle);
					}
					catch (Exception ex)
					{
						GLOBAL.AddToExecption(ex);
					}
					break;
				default:
					break;
			}
		}

		private void AddClient(Socket client)
		{
			lock (ClientToSymbolMap)
			{
				if (ClientToSymbolMap.ContainsKey(client.Handle))
				{
					SetStatus(StatusTypes.Started, string.Format("Server|Error Duplicated client handle detected From {0}", client.LocalEndPoint));
				}
				else
				{
					ClientToSymbolMap.Add(client.Handle, new List<string>());
				}
			}
		}

		#region - Set Status Display, Log and Alert -

		private void SetStatus(StatusTypes type, string statusDsp)
		{
			SetStatus(type, statusDsp, true);
		}
		private void SetStatus(StatusTypes type, string statusDsp, bool addTolog)
		{
			Status = type;
			StatusDsp = statusDsp;

			if (addTolog)
			{
				AddToStatusLogs(statusDsp);
			}
		}

		#endregion

		#region - Sent -

		private void SetSentMsg(string client, string symbol, string changes)
		{
			if (Configuration.MDS.Default.EnableTCPLog)
			{
				if (client != null)
				{
					lock (SentMsgs)
					{
						LastSentMsg = string.Format("{0,-22}: Changes=[{1,-3}] IssueSymbol=[{2}]", client, changes, symbol);
						SentMsgs.Add(String.Concat(new object[] { _dtHP.Now.ToString("HH:mm:ss.fffffff"), " ", LastSentMsg }));
					}
				}
			}
		}

		public void ProcessRecapQueue()
		{
			if (_mds != null && SymbolToRecapList.Count > 0)
			{
				StartPerformanceLog();

				PerformanceLog("MDS|ProcessRecapQueue", string.Format("{0,12}| Symbols[{1,3}] Recap", "Start", SymbolToRecapList.Count));

				// Make a local copy of the recap queue
				Dictionary<string, List<IntPtr>> locRecapQueue = new Dictionary<string, List<IntPtr>>();
				lock (SymbolToRecapList)
				{
					// Recap 
					//if (SymbolToRecapList.Count <= _recapLimit)
					//{
						locRecapQueue = new Dictionary<string, List<IntPtr>>(SymbolToRecapList);
						SymbolToRecapList.Clear();
					//}
					//else
					//{
					//    string[] keys = new string[SymbolToRecapList.Count];
					//    SymbolToRecapList.Keys.CopyTo(keys, 0);

					//    locRecapQueue = new Dictionary<string, List<IntPtr>>();
					//    for (int count = 0; count <= _recapLimit; count++)
					//    {
					//        locRecapQueue.Add(keys[count], SymbolToRecapList[keys[count]]);
					//        SymbolToRecapList.Remove(keys[count]);
					//    }
					//}
				}

				Dictionary<string, byte[]> locSymbolToRecapMap = ConvertToRecapBytes(locRecapQueue);

				Dictionary<IntPtr, List<byte[]>> clientSendList = ConvertToSendList(locSymbolToRecapMap);

				try
				{
					foreach (IntPtr client in clientSendList.Keys)
					{
						ProcessSend(client, clientSendList[client]);

						PerformanceLog("MDS|ProcessRecapQueue", string.Format("{0,-12}|[{1,-3}]Sends", "[2]" + client, clientSendList[client].Count));
					}
				}
				catch (Exception ex)
				{
					GLOBAL.AddToExecption(ex);
				}

				PerformanceLog("MDS|ProcessRecapQueue", string.Format("{0,12}| Symbols[{1,3}] Recap", "End", locRecapQueue.Count));
			}
		}

		private Dictionary<string, byte[]> ConvertToRecapBytes(Dictionary<string, List<IntPtr>> locRecapQueue)
		{
			Dictionary<string, byte[]> locSymbolToRecapMap = new Dictionary<string, byte[]>();
			lock (GLOBAL.HLBM.LatestData)
			{
				// key = Symbol, Source
				foreach (string key in locRecapQueue.Keys)
				{
					if (GLOBAL.HLBM.LatestData.ContainsKey(key))
					{
						locSymbolToRecapMap.Add(key, ConvertToOutput(GLOBAL.HLBM.LatestData[key]));
						PerformanceLog("MDS|ProcessRecapQueue", string.Format("{0,-12}|[{1,-3}]Deltas", "[1]" + key, GLOBAL.HLBM.LatestData[key].Count));
					}
					else
					{
						// Add back to the recap list
						AddToRecapList(key, locRecapQueue[key]);
					}
				}
			}

			return locSymbolToRecapMap;
		}

		public void ProcessDeltaQueue(MDServerToClient delta)
		{
			string key = delta.uSubscriptionKey;
			IntPtr[] clients = new IntPtr[0];

			if (_mds != null && key != null && key != "")
			{
				StartPerformanceLog();

				PerformanceLog("MDS|ProcessDeltaQueue", string.Format("{0,12}| Symbols[{1,3}] Delta", "Start", delta.Count));

				if (SymbolToClientList.ContainsKey(key))
				{
					clients = SymbolToClientList[key].ToArray();
				}

				try
				{
					if (clients.Length > 0)
					{
						byte[] deltaMsg = ConvertToOutput(delta);

						foreach (IntPtr client in clients)
						{
							ProcessSend(client, delta, deltaMsg);
						}
					}
					else
					{
						lock (SymbolToClientList)
						{
							// Clean up the Symbo To Client List if it has no client mapped to it.
							if (SymbolToClientList.ContainsKey(key))
							{
								SymbolToClientList.Remove(key);
							}
						}
					}
				}
				catch (Exception ex)
				{
					GLOBAL.AddToExecption(ex);
				}

				PerformanceLog("MDS|ProcessDeltaQueue", string.Format("{0,12}| Symbols[{1,3}] Delta", "End", delta.Count));
			}
		}

		public void ProcessDeltaQueue(Dictionary<string, MDServerToClient> deltas)
		{
			if (_mds != null && SymbolToClientList.Count > 0 && deltas.Count > 0)
			{
				StartPerformanceLog();

				PerformanceLog("MDS|ProcessDeltaQueue", string.Format("{0,12}| Symbols[{1,3}] Delta", "Start", deltas.Count));

				Dictionary<string, byte[]> locSymbolToDeltaMap = ConvertToDeltaBytes(deltas);

				Dictionary<IntPtr, List<byte[]>> clientSendList = ConvertToSendList(locSymbolToDeltaMap);

				try
				{
					foreach (IntPtr client in clientSendList.Keys)
					{
						ProcessSend(client, clientSendList[client]);

						PerformanceLog("MDS|ProcessDeltaQueue", string.Format("{0,-12}|[{1,-3}]Sends", "[2]" + client, clientSendList[client].Count));
					}
				}
				catch (Exception ex)
				{
					GLOBAL.AddToExecption(ex);
				}

				PerformanceLog("MDS|ProcessDeltaQueue", string.Format("{0,12}| Symbols[{1,3}] Delta", "End", deltas.Count));
			}
		}

		private Dictionary<string, byte[]> ConvertToDeltaBytes(Dictionary<string, MDServerToClient> deltas)
		{
			// Convert All to bytes
			Dictionary<string, byte[]> locSymbolToDeltaMap = new Dictionary<string, byte[]>();
			foreach (KeyValuePair<string, MDServerToClient> kvp in deltas)
			{
				locSymbolToDeltaMap.Add(kvp.Key, ConvertToOutput(kvp.Value));

				PerformanceLog("MDS|ProcessDeltaQueue", string.Format("{0,-12}|[{1,-3}]Deltas", "[1]" + kvp.Key, kvp.Value.Count));
			}

			return locSymbolToDeltaMap;
		}

		private Dictionary<IntPtr, List<byte[]>> ConvertToSendList(Dictionary<string, byte[]> locSymbolToDeltaMap)
		{
			Dictionary<IntPtr, List<byte[]>> clientSendList = new Dictionary<IntPtr, List<byte[]>>();
			lock (SymbolToClientList)
			{
				//string uSymbol = "";
				foreach (string key in locSymbolToDeltaMap.Keys)
				{
					//string[] keyItems = key.Split(_keySpliter);
					//uSymbol = keyItems[0];
					//if (uSymbol.Contains("."))
					//{
					//    uSymbol = uSymbol.Substring(0, uSymbol.IndexOf('.'));
					//}

					if (SymbolToClientList.ContainsKey(key))
					{
						// Get the Client List
						foreach (IntPtr client in SymbolToClientList[key])
						{
							if (clientSendList.ContainsKey(client))
							{
								clientSendList[client].Add(locSymbolToDeltaMap[key]);
							}
							else
							{
								clientSendList.Add(client, new List<byte[]>());
								clientSendList[client].Add(locSymbolToDeltaMap[key]);
							}

							if (_mds.Clients.ContainsKey(client))
							{
								if (_mds.Clients[client].Connected)
								{
									SetSentMsg(_mds.Clients[client].LocalEndPoint, key, "ReCap");
								}
							}

							PerformanceLog("MDS|ConvertToDeltaString", string.Format("{0,-12}|[{1,-3}]Clients", "[2]" + client, clientSendList[client].Count));
						}
					}
				}
			}

			return clientSendList;
		}

		private void ProcessSend(IntPtr client, MDServerToClient delta, byte[] deltaMsg)
		{
			bool cleanUp = false;

			if (DisconnectedClients.Count > 0 && DisconnectedClients.Contains(client))
			{
				return;
			}

			if (_mds != null)
			{
				if (_mds.Clients.ContainsKey(client))
				{
					if (_mds.Clients[client].Connected)
					{
						try
						{
							SetSentMsg(_mds.Clients[client].LocalEndPoint, delta.IssueSymbol, delta.Count.ToString());
							_mds.SendRawBytes(client, deltaMsg);
						}
						catch (Exception ex)
						{
							// Client send error
							GLOBAL.AddToExecption(ex);
							cleanUp = true;
						}
					}
					else
					{
						// Client is already disconnected
						AddToStatusLogs(String.Concat(new object[] { "MDS|ProcessSendDelta|RemoveClient ", client.ToString(), " Soket ID" }));
						cleanUp = true;
					}
				}
				else
				{
					// Client is already removed
					cleanUp = true;
				}

				if (cleanUp)
				{
					AddToCleanUpList(client);
					//ProcessCleanUp(client);
				}
			}
		}

		private void ProcessSend(IntPtr client, string msg)
		{
			bool cleanUp = false;

			if (DisconnectedClients.Count > 0 && DisconnectedClients.Contains(client))
			{
				return;
			}

			if (_mds != null)
			{
				if (_mds.Clients.ContainsKey(client))
				{
					if (_mds.Clients[client].Connected)
					{
						try
						{
							SetSentMsg(_mds.Clients[client].LocalEndPoint, "MSG ", msg);
							_mds.Send(client, msg);
						}
						catch (Exception ex)
						{
							// Client send error
							GLOBAL.AddToExecption(ex);
							cleanUp = true;
						}
					}
					else
					{
						// Client is already disconnected
						AddToStatusLogs(String.Concat(new object[] { "MDS|ProcessSendMSG|RemoveClient ", client.ToString(), " Soket ID" }));
						cleanUp = true;
					}
				}
				else
				{
					// Client is already removed
					cleanUp = true;
				}

				if (cleanUp)
				{
					AddToCleanUpList(client);
					//ProcessCleanUp(client);
				}
			}
		}

		private void ProcessSend(IntPtr client, byte[] msg)
		{
			bool cleanUp = false;

			if (DisconnectedClients.Count > 0 && DisconnectedClients.Contains(client))
			{
				return;
			}

			if (_mds != null)
			{
				if (_mds.Clients.ContainsKey(client))
				{
					if (_mds.Clients[client].Connected)
					{
						try
						{
							SetSentMsg(_mds.Clients[client].LocalEndPoint, "RawBytes ", msg.Length.ToString());
							_mds.SendRawBytes(client, msg);
						}
						catch (Exception ex)
						{
							// Client send error
							GLOBAL.AddToExecption(ex);
							cleanUp = true;
						}
					}
					else
					{
						// Client is already disconnected
						AddToStatusLogs(String.Concat(new object[] { "MDS|ProcessSendMSG|RemoveClient ", client.ToString(), " Soket ID" }));
						cleanUp = true;
					}
				}
				else
				{
					// Client is already removed
					cleanUp = true;
				}

				if (cleanUp)
				{
					AddToCleanUpList(client);
					//ProcessCleanUp(client);
				}
			}
		}

		private void ProcessSend(IntPtr client, List<byte[]> msgs)
		{
			bool cleanUp = false;

			if (DisconnectedClients.Count > 0 && DisconnectedClients.Contains(client))
			{
				return;
			}

			if (_mds != null)
			{
				if (_mds.Clients.ContainsKey(client))
				{
					if (_mds.Clients[client].Connected)
					{
						try
						{
							SetSentMsg(_mds.Clients[client].LocalEndPoint, "BytesList ", msgs.Count.ToString());
							_mds.SendRawBytes(client, msgs);
						}
						catch (Exception ex)
						{
							// Client send error
							GLOBAL.AddToExecption(ex);
							cleanUp = true;
						}
					}
					else
					{
						// Client is already disconnected
						AddToStatusLogs(String.Concat(new object[] { "MDS|ProcessSendMSGS|RemoveClient ", client.ToString(), " Soket ID" }));
						cleanUp = true;
					}
				}
				else
				{
					// Client is already removed
					cleanUp = true;
				}

				if (cleanUp)
				{
					AddToCleanUpList(client);
					//ProcessCleanUp(client);
				}
			}
		}

		private void AddToCleanUpList(IntPtr client)
		{
			lock (DisconnectedClients)
			{
				if (!DisconnectedClients.Contains(client))
				{
					DisconnectedClients.Add(client);
				}
			}
		}

		public void ProcessCleanUP()
		{
			List<IntPtr> locClients = new List<IntPtr>();
			lock (DisconnectedClients)
			{
				if (DisconnectedClients.Count > 0)
				{
					locClients = new List<IntPtr>(DisconnectedClients.ToArray());
					DisconnectedClients.Clear();
				}
			}

			if (locClients.Count > 0)
			{
				foreach (IntPtr locClient in locClients)
				{
					ProcessCleanUp(locClient);
				}
			}
		}

		private void ProcessCleanUp(IntPtr client)
		{
			AddToStatusLogs(String.Concat(new object[] { "MDS|ProcessCleanUp|Start ", client.ToString(), " Soket ID" }));

			try
			{
				lock (ClientToSymbolMap)
				{
					if (ClientToSymbolMap.ContainsKey(client))
					{
						// Make a local copy
						// List of MDKey = Symbol + Source
						List<string> locSymbolMapCollection = new List<string>();
						lock (ClientToSymbolMap)
						{
							locSymbolMapCollection = ClientToSymbolMap[client];
							ClientToSymbolMap.Remove(client);
						}

						try
						{
							if (locSymbolMapCollection.Count > 0)
							{
								lock (SymbolToClientList)
								{
									AddToStatusLogs(String.Concat(new object[] { "MDS|ProcessCleanUp|SymbolToClientList|Start ", SymbolToClientList.Count.ToString(), " SymbolToClientList Count" }));

									foreach (string key in locSymbolMapCollection)
									{
										if (SymbolToClientList.ContainsKey(key))
										{
											if (SymbolToClientList[key].Contains(client))
											{
												// Remove from client list
												SymbolToClientList[key].Remove(client);
											}
										}
									}

									AddToStatusLogs(String.Concat(new object[] { "MDS|ProcessCleanUp|SymbolToClientList|End ", SymbolToClientList.Count.ToString(), " SymbolToClientList Count" }));
								}
							}
						}
						catch (Exception ex)
						{
							GLOBAL.AddToExecption(ex);
						}
					}
				}
			}
			catch (Exception ex)
			{
				GLOBAL.AddToExecption(ex);
			}

			try
			{
				lock (ClientToUserMap)
				{
					AddToStatusLogs(String.Concat(new object[] { "MDS|ProcessCleanUp|ClientToUserMap|Start ", ClientToUserMap.Count.ToString(), " ClientToUserMap Count" }));

					if (ClientToUserMap.ContainsKey(client))
					{
						ClientToUserMap.Remove(client);
					}

					AddToStatusLogs(String.Concat(new object[] { "MDS|ProcessCleanUp|ClientToUserMap|End ", ClientToUserMap.Count.ToString(), " ClientToUserMap Count" }));
				}
			}
			catch (Exception ex)
			{
				GLOBAL.AddToExecption(ex);
			}

			try
			{
				lock (_mds.Clients)
				{
					AddToStatusLogs(String.Concat(new object[] { "MDS|ProcessCleanUp|_mds.Clients|Start ", _mds.Clients.Count.ToString(), " _mds.Clients Count" }));

					if (_mds.Clients.ContainsKey(client))
					{
						_mds.Clients[client].Stop();
						_mds.Clients.Remove(client);
					}

					AddToStatusLogs(String.Concat(new object[] { "MDS|ProcessCleanUp|_mds.Clients|End ", _mds.Clients.Count.ToString(), " _mds.Clients Count" }));
				}
			}
			catch (Exception ex)
			{
				GLOBAL.AddToExecption(ex);
			}

			AddToStatusLogs(String.Concat(new object[] { "MDS|ProcessCleanUp|End ", client.ToString(), " Soket ID" }));
		}

		#endregion

		#region - Received -

		private void SetReceivedMsg(Socket client, string receivedMsg)
		{
			if (Configuration.MDS.Default.EnableTCPLog)
			{
				lock (ReceivedMsgs)
				{
					LastReceivedMsg = string.Format("{0,-22}: {1}", client.LocalEndPoint, receivedMsg);
					ReceivedMsgs.Add(String.Concat(new object[] { _dtHP.Now.ToString("HH:mm:ss.fffffff"), " ", LastReceivedMsg }));
				}
			}
		}

		private void SetReceivedMsg(Socket client, MDClientToServer receivedMsg)
		{
			if (Configuration.MDS.Default.EnableTCPLog)
			{
				lock (ReceivedMsgs)
				{
					switch (receivedMsg.MsgType)
					{
						case "H":
							break;
						case "L":
							break;
						case "S":
							LastReceivedMsg = string.Format("{0,-22}: MDSymbol={1,-10} MDDataSource={2,-10} SecType={3,-10} [S]", client.LocalEndPoint, receivedMsg.MDSymbol, receivedMsg.MDDataSource, receivedMsg.SecType);
							ReceivedMsgs.Add(String.Concat(new object[] { _dtHP.Now.ToString("HH:mm:ss.fffffff"), " ", LastReceivedMsg }));
							break;
						case "U":
							break;
					}
				}
			}
		}

		private void DecodeReceivedMsg(ServerEventArgs e)
		{
			lock (e.Client)
			{
				if (e.Client.Connected && ClientToSymbolMap.ContainsKey(e.Client.Handle))
				{
					//IntPtr clientKey = e.Client.Handle;
					MDClientToServer clientMsg = null;

					if (e.ByteMessage != null && e.ByteMessage.Length > 0)
					{
						// Decode Msg
						clientMsg = ConvertFromInput(e.ByteMessage);

						if (clientMsg == null)
						{
							// Not a valid message
							if (Configuration.MDS.Default.EchoOnUnknown)
							{
								// ECHO back
								ProcessSend(e.Client.Handle, e.Message);
							}
							SetReceivedMsg(e.Client, e.Message);
						}
						else
						{
							// valid message
							switch (clientMsg.MsgType)
							{
								case "H":
									// Hartbeat
									if (e.Client.Connected)
									{
										SetReceivedMsg(e.Client, clientMsg);
										Hartbeat(e.Client.Handle);
									}
									break;
								case "L":
									// Login
									SetReceivedMsg(e.Client, clientMsg);
									Login(e.Client.Handle, clientMsg);
									//AddToMDSClientLog(String.Concat(new object[] { "User: ", clientMsg.UserName }));
									break;
								case "S":
									// Subscribe
									SetReceivedMsg(e.Client, clientMsg);
									SubscribeToSymbol(e.Client.Handle, clientMsg);
									AddToMDSClientLog(String.Format("Sub: {0},{1},{2}", new object[] { clientMsg.MDKey, clientMsg.SecType, clientMsg.SubscriptionType }));
									break;
								case "U":
									// Unsubscribe
									SetReceivedMsg(e.Client, clientMsg);
									UnsubscribeToSymbol(e.Client.Handle, clientMsg);
									break;
							}
						}
					}
				}
			}
		}

		private void SubscribeToSymbol(IntPtr clientKey, MDClientToServer symbolMap)
		{
			AddSymbolToClientList(clientKey, symbolMap);
			
			AddClientToSymbolMap(clientKey, symbolMap);

			if (!IsRecap(clientKey, symbolMap))
			{
				// Don't Make New Symbol For Index
				// New Symbol
				//switch (symbolMap.SecType)
				//{
				//    case CSVFieldIDs.SecutrityTypes.OptionIndex:
				//        if (!symbolMap.MDSymbol.Contains(".IDX"))
				//        {
				//            symbolMap.MDSymbol = symbolMap.MDSymbol + ".IDX";
				//        }
				//        break;
				//}
				GLOBAL.HLBM.Subscribe(symbolMap);
			}
		}

		private void AddSymbolToClientList(IntPtr clientKey, MDClientToServer symbolMap)
		{
			lock (SymbolToClientList)
			{
				// Add market data Key (Symbol + Source) to Map
				if (!SymbolToClientList.ContainsKey(symbolMap.MDKey))
				{
					SymbolToClientList.Add(symbolMap.MDKey, new List<IntPtr>());
				}
				if (!SymbolToClientList[symbolMap.MDKey].Contains(clientKey))
				{
					SymbolToClientList[symbolMap.MDKey].Add(clientKey);
				}

				// Add base symbol Key (BaseSymbol + Source) to Map
				if (!SymbolToClientList.ContainsKey(symbolMap.MDBaseKey))
				{
					SymbolToClientList.Add(symbolMap.MDBaseKey, new List<IntPtr>());
				}
				if (!SymbolToClientList[symbolMap.MDBaseKey].Contains(clientKey))
				{
					SymbolToClientList[symbolMap.MDBaseKey].Add(clientKey);
				}
			}
		}

		private void AddClientToSymbolMap(IntPtr clientKey, MDClientToServer symbolMap)
		{
			lock (ClientToSymbolMap)
			{
				if (!ClientToSymbolMap.ContainsKey(clientKey))
				{
					ClientToSymbolMap.Add(clientKey, new List<string>());
				}

				// Add market data symbol List(Symbol + Source) to Map
				if (!ClientToSymbolMap[clientKey].Contains(symbolMap.MDKey))
				{
					ClientToSymbolMap[clientKey].Add(symbolMap.MDKey);
				}

				// Add base symbol List (BaseSymbol + Source) to Map
				if (!ClientToSymbolMap[clientKey].Contains(symbolMap.MDBaseKey))
				{
					ClientToSymbolMap[clientKey].Add(symbolMap.MDBaseKey);
				}
			}
		}

		private bool IsRecap(IntPtr clientKey, MDClientToServer symbolMap)
		{
			bool recap = false;
			string[] latestDataKeys = new string[0];
			List<string> latestDataKeyList = new List<string>();

			lock (GLOBAL.HLBM.LatestData)
			{
				if (GLOBAL.HLBM.LatestData.ContainsKey(symbolMap.MDKey))
				{
					recap = true;

					latestDataKeys = new string[GLOBAL.HLBM.LatestData.Count];
					GLOBAL.HLBM.LatestData.Keys.CopyTo(latestDataKeys, 0);
					latestDataKeyList = new List<string>(GLOBAL.HLBM.LatestData.Keys);
				}
			}

			if (recap)
			{
				// Recap Now
				//lock (GLOBAL.HLBM.LatestData)
				//{
				//    ProcessSend(clientKey, ConvertToOutput(GLOBAL.HLBM.LatestData[symbolMap.MDSymbol]));
				//}

				// Recap All Later
				AddToRecapList(symbolMap.MDKey, clientKey);
				if (symbolMap.MDSymbol != symbolMap.BaseSymbol)
				{
					AddToRecapList(symbolMap.MDBaseKey, clientKey);
				}

				string mdKey = "";
				switch (symbolMap.MDDataSource.ToUpper())
				{
					case "CTA":
						foreach (string key in L2PartipcantCode.IDsCTA.Keys)
						{
							mdKey = string.Concat(new object[] { symbolMap.BaseSymbol, ".", key, ",", symbolMap.MDDataSource });

							if (latestDataKeyList.Contains(mdKey))
							{
								AddToRecapList(mdKey, clientKey);
							}
						}
						break;
					case "NASDAQ":
						foreach (string key in L2PartipcantCode.IDsNASDAQ.Keys)
						{
							mdKey = string.Concat(new object[] { symbolMap.BaseSymbol, ".", key, ",", symbolMap.MDDataSource });

							if (latestDataKeyList.Contains(mdKey))
							{
								AddToRecapList(mdKey, clientKey);
							}
						}
						break;
				}
			}

			return recap;
		}

		private void AddToRecapList(string key, IntPtr clientKey)
		{
			// Add to recap list
			List<IntPtr> clientKeys = new List<IntPtr>();
			clientKeys.Add(clientKey);
			AddToRecapList(key, clientKeys);
		}
		
		private void AddToRecapList(string key, List<IntPtr> clientKeys)
		{
			lock (SymbolToRecapList)
			{
				if (!SymbolToRecapList.ContainsKey(key))
				{
					SymbolToRecapList.Add(key, new List<IntPtr>());
				}

				foreach (IntPtr clientKey in clientKeys)
				{
					if (!SymbolToRecapList[key].Contains(clientKey))
					{
						SymbolToRecapList[key].Add(clientKey);
					}
				}
			}		
		}

		private void UnsubscribeToSymbol(IntPtr clientKey, MDClientToServer symbolMap)
		{
			lock (SymbolToClientList)
			{
				if (SymbolToClientList.ContainsKey(symbolMap.MDSymbol))
				{
					if (SymbolToClientList[symbolMap.MDSymbol].Contains(clientKey))
					{
						SymbolToClientList[symbolMap.MDSymbol].Remove(clientKey);
					}
				}
			}

			lock (ClientToSymbolMap)
			{
				if (ClientToSymbolMap.ContainsKey(clientKey))
				{
					if (ClientToSymbolMap[clientKey].Contains(symbolMap.MDSymbol))
					{
						ClientToSymbolMap[clientKey].Remove(symbolMap.MDSymbol);
					}
				}
			}
		}

		private void Login(IntPtr clientKey, MDClientToServer login)
		{
			lock (ClientToUserMap)
			{
				if (!ClientToUserMap.ContainsKey(clientKey))
				{
					ClientToUserMap.Add(clientKey, login);
				}
				else
				{
					ClientToUserMap[clientKey] = login;
				}
			}

			Hartbeat(clientKey);
		}

		private void Hartbeat(IntPtr clientKey)
		{
			lock (ClientHartbeatMap)
			{
				// No Need to Echo back, if there is market data to send.
				if (!ClientHartbeatMap.ContainsKey(clientKey))
				{
					ClientHartbeatMap.Add(clientKey, DateTime.Now);
				}
				else
				{
					ClientHartbeatMap[clientKey] = DateTime.Now;
				}
			}
		}

		#endregion

		#region - Connect & Disconnect -

		public void Connect()
		{
			if (_mds == null)
			{
				_mds = new MDSServer(Configuration.MDS.Default.IP, Configuration.MDS.Default.Port, Configuration.MDS.Default.MaxBackLog, new byte[Configuration.MDS.Default.BufferSize], Configuration.MDS.Default.ClientBufferSizeLimit);
				_mds.OnServerEvent += new ServerEventHandler(mds_OnServerEvent);
			}

			if (Status != StatusTypes.Stopped)
			{
				Disconnect();
			}

			while (Status != StatusTypes.Stopped) ;

			SetStatus(StatusTypes.Stopping, String.Concat(new object[] { "Server|Connecting... To ", Configuration.MDS.Default.IP, " : ", Configuration.MDS.Default.Port }));
			_mds.Start();
		}

		public void Disconnect()
		{
			if (_mds != null)
			{
				SetStatus(StatusTypes.Stopping, String.Concat(new object[] { "Server|Disconnect... From ", Configuration.MDS.Default.IP, " : ", Configuration.MDS.Default.Port }));
				_mds.Stop();
			}
		}

		#endregion

		#region - Serial Input & Output -

		public byte[] ConvertToOutput(MDServerToClient toClient)
		{
			toClient.uServerSentTime = _dtHP.Now;
			return _toBinary.Serialize(toClient, SerializationTypes.Normal);
		}

		public MDClientToServer ConvertFromInput(byte[] fromClient)
		{
			try
			{
				if (fromClient != null && fromClient.Length > 0)
				{
					return (MDClientToServer)_fromBinary.Deserialize(fromClient, SerializationTypes.Normal);
				}
			}
			catch (Exception ex)
			{
				SetStatus(StatusTypes.Started, String.Concat(new object[] { "Server|Message Error ", ex.Message, " ", ex.StackTrace }));
				GLOBAL.AddToExecption(ex);
			}

			return null;
		}

		#endregion

		#region - Log Received & Sent -

		public void LogRecivedMsgs()
		{
			string[] msgs = new string[0];
			if (ReceivedMsgs.Count > 0)
			{
				lock (ReceivedMsgs)
				{
					msgs = ReceivedMsgs.ToArray();
					ReceivedMsgs.Clear();
				}
			}

			foreach (string msg in msgs)
			{
				GLOBAL.LOG.TCP.Out(String.Concat(new object[] { "[R]: ", msg }), false);
			}
		}

		public void LogSentMsgs()
		{
			string[] msgs = new string[0];
			if (SentMsgs.Count > 0)
			{
				lock (SentMsgs)
				{
					msgs = SentMsgs.ToArray();
					SentMsgs.Clear();
				}
			}

			foreach (string msg in msgs)
			{
				GLOBAL.LOG.TCP.Out(String.Concat(new object[] { "[S]: ", msg }), false);
			}
		}

		#endregion

		#region - Performance Log -

		private void StartPerformanceLog()
		{
			if (_logPerformance)
			{
				_pTime = _dtHP.Now;
				_pTimeSpan = new TimeSpan();
			}
		}

		private void PerformanceLog(string header, string details)
		{
			if (_logPerformance)
			{
				_pTimeSpan = _dtHP.Now.Subtract(_pTime);
				AddToMDSLog(string.Format("{0,30} {1,8}ms| {2}", header, _pTimeSpan.TotalMilliseconds, details));
			}
		}

		#endregion

		#region - MDS Log -

		public void AddToMDSClientLog(string msg)
		{
			lock (SyncObj)
			{
				MDSLogs.Add(String.Concat(new object[] { _dtHP.Now.ToString("HH:mm:ss.fffffff"), " ", msg }));
			}
		}

		public void AddToMDSLog(string msg)
		{
			if (Configuration.MDS.Default.EnableMDSLog)
			{
				lock (SyncObj)
				{
					MDSLogs.Add(String.Concat(new object[] { _dtHP.Now.ToString("HH:mm:ss.fffffff"), " ", msg }));
				}
			}
		}

		public void LogMDSMessages()
		{
			string[] msgs = new string[0];
			lock (SyncObj)
			{
				if (MDSLogs.Count > 0)
				{
					msgs = MDSLogs.ToArray();
					MDSLogs.Clear();
				}
			}

			foreach (string msg in msgs)
			{
				GLOBAL.LOG.MDS.Out(msg, false);
			}
		}

		#endregion
	}
}
