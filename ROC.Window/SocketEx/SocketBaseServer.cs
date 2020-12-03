using System;
using System.Net.Sockets;
using System.Net;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Diagnostics;

namespace SocketEx
{
	public enum ServerEventTypes
	{
		// Server Only
		OnListenerStart,
		OnListenerStop,
		OnListenerAccept,

		OnDisconnect,
		OnRawReceive,
		OnReceive,
		OnSend,
		OnError,
	}

	public delegate void ServerEventHandler(object sender, ServerEventArgs e);

	public abstract class SocketBaseServer : IDisposable
	{
		private object _syncObj = new object();
		public object SyncObj
		{
			get
			{
				return _syncObj;
			}
		}

		#region - Events -

		public event ServerEventHandler OnServerEvent;

		protected virtual void OnSocketEvent(ServerEventArgs e)
		{
			if (OnServerEvent != null)
			{
				OnServerEvent(this, e);
			}
		}

		#endregion

		#region - Local Variable -

		//private bool _stopping = false;

		private bool _listenerRunning = false;
		public bool ListenerRunning
		{
			get
			{
				return _listenerRunning;
			}
		}

		private TcpListener _listener;

		private string _ip;
		private int _port;
		private int _maxBackLog;
		private Byte[] _buffer;
		private int _clientBufferSizeLimit = 1;
		public int ClientBufferSizeLimit
		{
			get
			{
				return _clientBufferSizeLimit;
			}
		}

		#endregion
		
		#region - Clients -

		private Dictionary<IntPtr, SocketBaseClient> _clients = new Dictionary<IntPtr, SocketBaseClient>();
		public Dictionary<IntPtr, SocketBaseClient> Clients
		{
			get
			{
				return _clients;
			}
		}

		private Dictionary<IntPtr, List<string>> _sendByClientMsgs;
		public Dictionary<IntPtr, List<string>> SendByClientMsgs
		{
			get
			{
				if (_sendByClientMsgs == null)
				{
					_sendByClientMsgs = new Dictionary<IntPtr, List<string>>();
				}
				return _sendByClientMsgs;
			}
			set
			{
				_sendByClientMsgs = value;
			}
		}

		#endregion

		#region - Constractor -

		public SocketBaseServer()
		{
			Initialize("10.103.102.29", 3456, 64, new byte[0x4000], 1);
		}
		public SocketBaseServer(string ip, int port, int maxBackLog, Byte[] buffer, int clientBufferSizeLimit)
		{
			Initialize(ip, port, maxBackLog, buffer, clientBufferSizeLimit);
		}

		#endregion

		private void Initialize(string serverIP, int port, int maxBackLog, Byte[] buffer, int clilentBufferSizeLimit)
		{
			_ip = serverIP;
			_port = port;
			_maxBackLog = maxBackLog;
			_buffer = buffer;
			_clientBufferSizeLimit = clilentBufferSizeLimit;
		}

		public void Start()
		{
			try
			{
				//_stopping = false;

				IPHostEntry hostEntry = Dns.GetHostEntry(_ip);
				IPAddress[] hostIPs = hostEntry.AddressList;

				_listenerRunning = true;
				_listener = new TcpListener(hostIPs[0], _port);
				_listener.Start(_maxBackLog);

				OnSocketEvent(new ServerEventArgs(ServerEventTypes.OnListenerStart, "Listener|Start: Server Started."));
				SetupAcceptSocketCallBack(_listener);
			}
			catch (Exception ex)
			{
				if (Debugger.IsAttached)
				{
					Debug.Print(ex.Message);
					Debug.Print(ex.StackTrace);
				}

				OnSocketEvent(new ServerEventArgs(ServerEventTypes.OnError, String.Concat("Listener|Error: Server Start... ", ex.Message, " ", ex.StackTrace)));
			}
		}

		#region - Accept -

		public void SetupAcceptSocketCallBack(TcpListener listener)
		{
			AsyncCallback cbListen = new AsyncCallback(OnListenerAccept);
			listener.BeginAcceptSocket(cbListen, listener);
		}

		public abstract void OnListenerAccept(IAsyncResult ar);

		#endregion

		public void Stop()
		{
			Dictionary<IntPtr, SocketBaseClient> locCopy = new Dictionary<IntPtr, SocketBaseClient>();
			if (_clients.Count > 0)
			{
				lock (SyncObj)
				{
					locCopy = new Dictionary<IntPtr, SocketBaseClient>(_clients);
				}
			}

			foreach (SocketBaseClient client in locCopy.Values)
			{
				client.Stop();
			}

			_listenerRunning = false;
			if (_listener != null)
			{
				_listener.Stop();
			}

			OnSocketEvent(new ServerEventArgs(ServerEventTypes.OnListenerStop, "Listener|Stop: Server Stopped."));
		}

		public abstract void Send(string msg);
		public abstract void Send(IntPtr client, string msg);

		public void OnClientEvent(object sender, ClientEventArgs e)
		{
			switch (e.Type)
			{
				case ClientEventTypes.OnConnect:
					// Not used in server because OnListenerAccept
					break;
				case ClientEventTypes.OnDisconnect:
					OnSocketEvent(new ServerEventArgs(ServerEventTypes.OnDisconnect, e.Client, e.Message, e.ByteMessage));
					StopClient(e.Client);
					break;
				case ClientEventTypes.OnError:
					OnSocketEvent(new ServerEventArgs(ServerEventTypes.OnError, e.Client, e.Message, e.ByteMessage));
					StopClient(e.Client);
					break;
				case ClientEventTypes.OnRawReceive:
					OnSocketEvent(new ServerEventArgs(ServerEventTypes.OnRawReceive, e.Client, e.Message, e.ByteMessage));
					break;
				case ClientEventTypes.OnReceive:
					OnSocketEvent(new ServerEventArgs(ServerEventTypes.OnReceive, e.Client, e.Message, e.ByteMessage));
					break;
				case ClientEventTypes.OnSend:
					OnSocketEvent(new ServerEventArgs(ServerEventTypes.OnSend, e.Client, e.Message, e.ByteMessage));
					break;
			}
		}

		private void StopClient(Socket client)
		{
			try
			{
				if (client != null)
				{
					IntPtr key = client.Handle;
					lock (SyncObj)
					{
						if (Clients.ContainsKey(key))
						{
							SocketBaseClient sc = Clients[key];
							sc.OnClientEvent -= new ClientEventHandler(OnClientEvent);

							Clients[key].Stop();
						}
					}
				}
			}
			catch (Exception ex)
			{
				if (Debugger.IsAttached)
				{
					Debug.Print(ex.Message);
					Debug.Print(ex.StackTrace);
				}
			}
		}

		#region - IDisposable Members -

		public void Dispose()
		{
			Stop();
		}

		#endregion
	}

	public class ServerEventArgs : EventArgs
	{
		private ServerEventTypes _type;
		private Socket _client;
		private string _message;
		private byte[] _byteMessage;

		public ServerEventArgs(ServerEventTypes type, Socket client)
		{
			SetEventArgs(type, client, "", null);
		}

		public ServerEventArgs(ServerEventTypes type, string message)
		{
			SetEventArgs(type, null, message, null);
		}

		public ServerEventArgs(ServerEventTypes type, Socket client, string message, byte[] byteMessage)
		{
			SetEventArgs(type, client, message, byteMessage);
		}

		private void SetEventArgs(ServerEventTypes type, Socket client, string message, byte[] byteMessage)
		{
			_type = type;
			_message = message;
			_byteMessage = byteMessage;
			_client = client;
		}

		public ServerEventTypes Type
		{
			get
			{
				return _type;
			}
		}

		public Socket Client
		{
			get
			{
				return _client;
			}
		}

		public string RemoteEndPoint
		{
			get
			{
				if (Client != null && Client.RemoteEndPoint != null)
				{
					return string.Format("{0}", Client.RemoteEndPoint);
				}
				return "";
			}
		}

		public string LocalEndPoint
		{
			get
			{
				if (Client != null && Client.LocalEndPoint != null)
				{
					return string.Format("{0}", Client.LocalEndPoint);
				}
				return "";
			}
		}

		public string Message
		{
			get
			{
				return _message;
			}
		}

		public byte[] ByteMessage
		{
			get
			{
				return _byteMessage;
			}
		}
	}
}
