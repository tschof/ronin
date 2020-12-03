using System;
using System.Net.Sockets;
using System.Text;
using System.Collections;
using System.Net;
using System.Collections.Generic;
using System.Diagnostics;

namespace SocketEx
{
	public enum ClientEventTypes
	{
		// Client Only
		OnConnect,

		OnDisconnect,
		OnRawReceive,
		OnReceive,
		OnSend,
		OnError,
		OnDebug,
	}

	public delegate void ClientEventHandler(object sender, ClientEventArgs e);

	public abstract class SocketBaseClient
	{
		private object _syncObj = new object();
		public object SyncObj
		{
			get
			{
				return _syncObj;
			}
		}

		#region - Event -

		public event ClientEventHandler OnClientEvent;

		protected virtual void OnSocketEvent(ClientEventArgs e)
		{
			if (OnClientEvent != null)
			{
				OnClientEvent(this, e);
			}
		}

		#endregion

		#region - Recived Process Thread -

		#region - Local Incomming MDS Data -

		private List<byte> _incommingData = new List<byte>();
		public List<byte> IncommingData
		{
			get
			{
				return _incommingData;
			}
			set
			{
				_incommingData = value;
			}
		}

		#endregion

		#region - Local Incomming ROM Data -

		private string _incommingString = "";
		public string IncommingString
		{
			get
			{
				return _incommingString;
			}
			set
			{
				_incommingString = value;
			}
		}

		#endregion

		private bool _threadReceivedProcessShouldStop = false;
		public bool ThreadRecivedProcessShouldStop
		{
			get
			{
				return _threadReceivedProcessShouldStop;
			}
			set
			{
				_threadReceivedProcessShouldStop = value;
			}
		}

		private bool _threadRecivedProcessStarted = false;
		public bool ThreadRecivedProcessStarted
		{
			get
			{
				return _threadRecivedProcessStarted;
			}
			set
			{
				_threadRecivedProcessStarted = value;
			}
		}

		private System.Threading.Thread _threadReceivedProcess;

		#endregion

		#region - Send Process Thread -

		private bool _threadSendProcessShouldStop = false;
		private bool _threadSendProcessStarted = false;
		private System.Threading.Thread _threadSendProcess;

		private List<byte> _outgoingData = new List<byte>();
		public List<byte> OutGoingData
		{
			get
			{
				return _outgoingData;
			}
		}

		private int _clientBufferSizeLimit = 1;
		public int ClientBufferSizeLimit
		{
			get
			{
				return _clientBufferSizeLimit * 1000000;
			}
		}
		
		private bool _readyToSend = false;
		public bool ReadyToSend
		{
			set
			{
				_readyToSend = value;
			}
		}

		#endregion

		#region - Properties -

		private bool _stopping = false;
		public bool Stopping
		{
			get
			{
				return _stopping;
			}
			set
			{
				_stopping = value;
			}
		}
		
		private bool _connected = false;
		public bool Connected
		{
			get
			{
				return _connected;
			}
			set
			{
				_connected = value;
			}
		}

		// used by socket
		private byte[] _receivedBuffer;
		public byte[] ReceivedBuffer
		{
			get
			{
				return _receivedBuffer;
			}
		}

		private Socket _socket;
		public Socket ClientSocket
		{
			get
			{
				return _socket;
			}
		}

		public string RemoteEndPoint
		{
			get
			{
				if (ClientSocket != null && ClientSocket.RemoteEndPoint != null)
				{
					return string.Format("{0}", ClientSocket.RemoteEndPoint);
				}
				return "";
			}
		}

		public string LocalEndPoint
		{
			get
			{
				if (ClientSocket != null && ClientSocket.LocalEndPoint != null)
				{
					return string.Format("{0}", ClientSocket.LocalEndPoint);
				}
				return "";
			}
		}

		#endregion

		#region - Local Variable -

		private byte[] _sendBuffer;

		private string _serverIP;
		private int _port;

		#endregion

		#region - Constractor -

		public SocketBaseClient()
		{
			Initialize("10.103.102.29", 3456, new byte[0x15000], 1);
		}

		public SocketBaseClient(string serverIP, int port, Byte[] buffer, int clientBufferSizeLimit)
		{
			Initialize(serverIP, port, buffer, clientBufferSizeLimit);
		}

		// used by server only
		public SocketBaseClient(Socket socket, int clientBufferSizeLimit)
		{
			_socket = socket;

			if (_socket.Connected)
			{
				_connected = true;
				_readyToSend = true;
			}

			_clientBufferSizeLimit = clientBufferSizeLimit;

			SetupBeginReceiveCallBack(socket);
		}

		#endregion

		private void Initialize(string serverIP, int port, Byte[] buffer, int clientBufferSizeLimit)
		{
			_serverIP = serverIP;
			_port = port;
			_receivedBuffer = buffer;
			_sendBuffer = buffer;
			_clientBufferSizeLimit = clientBufferSizeLimit;
		}

		public void Start()
		{
			_stopping = false;

			_socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
			_socket.Blocking = false;
			_socket.NoDelay = true;
			SetupBeginConnectCallBack(_socket);
		}

		#region - Connect -

		private void SetupBeginConnectCallBack(Socket socket)
		{
			try
			{
				IPHostEntry hostEntry = Dns.GetHostEntry(_serverIP);
				IPAddress[] hostIPs = hostEntry.AddressList;

				AsyncCallback cbConnect = new AsyncCallback(OnConnect);
				_socket.BeginConnect(new IPEndPoint(hostIPs[0], _port), cbConnect, socket);
			}
			catch (Exception ex)
			{
				if (Debugger.IsAttached)
				{
					Debug.Print(ex.Message);
					Debug.Print(ex.StackTrace);
				}

				OnSocketEvent(new ClientEventArgs(ClientEventTypes.OnError, String.Concat("Client|Error: Connecting... ", ex.Message, " ", ex.StackTrace)));
			}
		}

		private void OnConnect(IAsyncResult ar)
		{
			try
			{
				Socket socket = (Socket)ar.AsyncState;

				if (socket.Connected)
				{
					_connected = true;
					_readyToSend = true;

					SetupBeginReceiveCallBack(socket);

					OnSocketEvent(new ClientEventArgs(ClientEventTypes.OnConnect, socket, "Client|OnConnect: To Server At " + _serverIP));
				}
				else
				{
					_connected = false;
					_readyToSend = false;

					OnSocketEvent(new ClientEventArgs(ClientEventTypes.OnError, "Client|OnConnect: Missing Server"));
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

		#endregion

		public void Stop()
		{
			try
			{
				_stopping = true;

				StopReceiveThread();
				StopSendThread();

				if (_connected && _socket != null)
				{
					lock (_socket)
					{
						_socket.Shutdown(SocketShutdown.Both);
						_connected = false;
						_readyToSend = false;
					}
				}
				_socket = null;

				_outgoingData.Clear();
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

		#region - Receive -

		public void StartReceiveThread()
		{
			if (_threadReceivedProcess == null && !_threadRecivedProcessStarted && !_stopping)
			{
				_threadReceivedProcessShouldStop = false;

				_threadReceivedProcess = new System.Threading.Thread(new System.Threading.ThreadStart(ProcessRecived));
				_threadReceivedProcess.Name = "SocketReceivedProcess";
				_threadReceivedProcess.Start();

				while (!_threadReceivedProcess.IsAlive) ;
				_threadRecivedProcessStarted = true;
			}
		}

		public void SetupBeginReceiveCallBack(Socket socket)
		{
			AsyncCallback cbReceive = new AsyncCallback(OnReceive);

			if (_receivedBuffer == null)
			{
				_receivedBuffer = new byte[socket.ReceiveBufferSize];
			}

			if (_sendBuffer == null)
			{
				_sendBuffer = new byte[socket.SendBufferSize];
			}

			try
			{
				socket.BeginReceive(_receivedBuffer, 0, _receivedBuffer.Length, SocketFlags.None, cbReceive, socket);
			}
			catch (SocketException ex)
			{
				if (Debugger.IsAttached)
				{
					Debug.Print(ex.Message);
					Debug.Print(ex.StackTrace);
				}

				switch (ex.SocketErrorCode)
				{
					case SocketError.WouldBlock:
					case SocketError.IOPending:
					case SocketError.NoBufferSpaceAvailable:
						OnSocketEvent(new ClientEventArgs(ClientEventTypes.OnReceive, socket, "Socket|BeginReceive|WouldBlock: " + ex.Message));
						break;
					default:
						OnSocketEvent(new ClientEventArgs(ClientEventTypes.OnError, socket, "Socket|BeginReceive: " + ex.Message));
						break;
				}
			}
		}

		public abstract void OnReceive(IAsyncResult ar);

		public abstract void ProcessRecived();

		private void StopReceiveThread()
		{
			try
			{
				_threadReceivedProcessShouldStop = true;
				if (_threadReceivedProcess != null)
				{
					int count = 0;
					while (_threadReceivedProcess.IsAlive)
					{
						count = count + 1;
						if (count > 100)
						{
							_threadReceivedProcess.Abort();
							break;
						}
						System.Threading.Thread.Sleep(1);
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

		#endregion

		public abstract void Send(string msg);

		public void Send(byte[] data)
		{
			try
			{
				//if (_outgoingData.Count + data.Length < _clientBufferSizeLimit)
				//{
					lock (SyncObj)
					{
						_outgoingData.AddRange(data);
					}
					StartSendThread();
				//}
				//else
				//{
				//    // Stop Feeding the Client
				//    OnSocketEvent(new ClientEventArgs(ClientEventTypes.OnError, "OutGoingData Buffer Full"));
				//}
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

		#region - Send -

		public void StartSendThread()
		{
			if (_threadSendProcess == null && !_threadSendProcessStarted && !_stopping)
			{
				_threadSendProcessShouldStop = false;

				_threadSendProcess = new System.Threading.Thread(new System.Threading.ThreadStart(ProcessSend));
				_threadSendProcess.Name = "SocketSendProcess";
				_threadSendProcess.Start();

				while (!_threadSendProcess.IsAlive) ;
				_threadSendProcessStarted = true;
			}
		}

		private void ProcessSend()
		{
			while (!_threadSendProcessShouldStop)
			{
				if (_readyToSend && _outgoingData.Count > 0)
				{
					SetupBeginSendCallBack(_socket);
				}
				System.Threading.Thread.Sleep(1);
			}
			_threadSendProcessStarted = false;
		}

		private void SetupBeginSendCallBack(Socket socket)
		{
			bool removeRange = false;
			bool clear = false;

			try
			{
				lock (SyncObj)
				{
					_readyToSend = false;

					byte[] dataToSend = new byte[0];
					if (_outgoingData.Count >= socket.SendBufferSize)
					{
					    dataToSend = _outgoingData.GetRange(0, socket.SendBufferSize).ToArray();
					    removeRange = true;
					}
					else if (_outgoingData.Count > 0)
					//if (_outgoingData.Count > 0)
					{
						dataToSend = _outgoingData.ToArray();
						clear = true;
					}

					if (dataToSend.Length > 0)
					{
						AsyncCallback cbSend = new AsyncCallback(OnSend);
						socket.BeginSend(dataToSend, 0, dataToSend.Length, SocketFlags.None, cbSend, socket);
					}
					else
					{
						_readyToSend = true;
						//OnSocketEvent(new ClientEventArgs(ClientEventTypes.OnError, socket, "Empty Buffer"));
					}

					// Remove from buffer if there is no execption on send
					// Prevent data corruption
					if (removeRange)
					{
						_outgoingData.RemoveRange(0, dataToSend.Length);
					}

					//OnSocketEvent(new ClientEventArgs(ClientEventTypes.OnDebug, String.Format("data sent: {0}", dataToSend.Length)));
					if (clear)
					{
						_outgoingData.Clear();
					}
					else
					{
						//OnSocketEvent(new ClientEventArgs(ClientEventTypes.OnDebug, String.Format("data after sent: {0}", _outgoingData.Count)));
					}
				}
			}
			catch (SocketException ex)
			{
				if (Debugger.IsAttached)
				{
					Debug.Print(ex.Message);
					Debug.Print(ex.StackTrace);
				}

				if (ex.SocketErrorCode == SocketError.WouldBlock ||
					ex.SocketErrorCode == SocketError.IOPending ||
					ex.SocketErrorCode == SocketError.NoBufferSpaceAvailable)
				{
					_readyToSend = true;
					// Do sent again on the next cycle;
					//OnSocketEvent(new ClientEventArgs(ClientEventTypes.OnDebug, socket, "WouldBlock " + ex.Message));
				}
				else
				{
					OnSocketEvent(new ClientEventArgs(ClientEventTypes.OnError, socket, "SetupBeginSendCallBack " + ex.Message));
				}
			}
		}

		private void OnSend(IAsyncResult ar)
		{
			//OnSocketEvent(new ClientEventArgs(ClientEventTypes.OnDebug, String.Format("send complete:")));

			Socket socket = (Socket)ar.AsyncState;

			//lock (SyncObj)
			//{
				try
				{
					socket.EndSend(ar);
					//int bytesSent = socket.EndSend(ar);
					//if (bytesSent > 0)
					//{
					//    string output = Encoding.ASCII.GetString(_sendBuffer, 0, bytesSent);
					//    OnSocketEvent(new ClientEventArgs(ClientEventTypes.OnSend, socket, bytesSent.ToString()));
					//}
					_readyToSend = true;
				}
				catch (Exception ex)
				{
					if (Debugger.IsAttached)
					{
						Debug.Print(ex.Message);
						Debug.Print(ex.StackTrace);
					}
					//socket.Close();
					// Socket Closed By Remote Host Exception Handling
					OnSocketEvent(new ClientEventArgs(ClientEventTypes.OnError, socket, "OnSend " + ex.Message));
				}
			//}
		}

		public void StopSendThread()
		{
			try
			{
				_threadSendProcessShouldStop = true;
				if (_threadSendProcess != null)
				{
					int count = 0;
					while (_threadSendProcess.IsAlive)
					{
						count = count + 1;
						if (count > 100)
						{
							_threadSendProcess.Abort();
							break;
						}
						System.Threading.Thread.Sleep(1);
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

		#endregion
	}

	public class ClientEventArgs : EventArgs
	{
		private ClientEventTypes _type;
		private Socket _client;
		private string _message;
		private byte[] _byteMessage;
		private string[] _messages;

		public ClientEventArgs(ClientEventTypes type)
		{
			SetEventArgs(type, null, "", new string[] { }, new byte[] { });
		}

		public ClientEventArgs(ClientEventTypes type, string message)
		{
			SetEventArgs(type, null, message, new string[] { }, new byte[] { });
		}
		public ClientEventArgs(ClientEventTypes type, Socket client, string message)
		{
			SetEventArgs(type, client, message, new string[] { }, new byte[] { });
		}

		public ClientEventArgs(ClientEventTypes type, Socket client, string message, byte[] byteMessage)
		{
			SetEventArgs(type, client, message, new string[] { }, byteMessage);
		}

		public ClientEventArgs(ClientEventTypes type, Socket client, string[] messages)
		{
			SetEventArgs(type, client, "", messages, new byte[] { });
		}

		private void SetEventArgs(ClientEventTypes type, Socket client, string message, string[] messages, byte[] byteMessage)
		{
			_type = type;
			_client = client;
			_message = message;
			_messages = messages;
			_byteMessage = byteMessage;		}


		public ClientEventTypes Type
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

		public string[] Messages
		{
			get
			{
				return _messages;
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
