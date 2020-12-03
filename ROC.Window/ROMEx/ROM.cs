using System;
using System.ComponentModel;
using System.Net;
using System.Net.Sockets;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Text;
using System.Collections.Generic;

namespace ROMEx
{
	public enum ROMStatusTypes
	{
		NotConnected,
		Connecting,
		Connected,
		Disconnecting,
		Disconnected,

		Error,
	}

	public class ROM : INotifyPropertyChanged, IDisposable
	{
		#region - API -

		[DllImport("kernel32.dll")]
		private static extern void Sleep(uint dwMilliseconds);

		[DllImport("kernel32.dll")]
		private static extern uint SleepEx(uint dwMilliseconds, bool bAlertable);

		#endregion

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

		#region - IDisposable Members -

		public void Dispose()
		{
			Disconnect();
		}

		#endregion

		#region - Property With Events -

		private ROMStatusTypes _status = ROMStatusTypes.NotConnected;
		public ROMStatusTypes Status
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
					onPropertyChanged(new PropertyChangedEventArgs("Status"));
				}
			}
		}

		private string _statusMsg = "";
		public string StatusMsg
		{
			get
			{
				return _statusMsg;
			}
			set
			{
				if (_statusMsg != value)
				{
					_statusMsg = value;
					onPropertyChanged(new PropertyChangedEventArgs("StatusMsg"));
				}
			}
		}

		private List<CSV> _recivedCSVs = new List<CSV>();
		public List<CSV> RecivedCSVs
		{
			get
			{
				return _recivedCSVs;
			}
			set
			{
				_recivedCSVs = value;
				onPropertyChanged(new PropertyChangedEventArgs("RecivedCSVs"));
			}
		}

		#endregion

		#region - Recived Process Thread -

		// used to store raw messages
		private string _recivedMsg = "";

		private bool _threadReciveProcessShouldStop = false;
		private bool _threadReciveProcessStarted = false;
		private System.Threading.Thread _threadReciveProcess;

		#endregion

		#region - Send Process Thread - 

		private List<string> _sendMsgs = new List<string>();

		private bool _threadSendProcessShouldStop = false;
		private bool _threadSendProcessStarted = false;
		private System.Threading.Thread _threadSendProcess;

		private bool _sending = false;

		#endregion

		#region - Local Variable -

		private const string EOL = "\n";

		// used by socket
		private byte[] _buffer;
		private string _ip;
		private int _port;

		private IPEndPoint _romIP;
		private Socket _socket;

		private object _syncObj = new object();

		#endregion

		public ROM(string ip, int port)
		{
			_ip = ip;
			_port = port;
		}

		private void ProcessRecivedMessage()
		{
			string locMsg = "";
			string locPartialMsg = "";

			while (!_threadReciveProcessShouldStop && locPartialMsg != "")
			{
				lock (_syncObj)
				{
					locMsg = locPartialMsg + _recivedMsg;
					_recivedMsg = "";
				}

				List<CSV> msgCSVs = new List<CSV>();
				if (locMsg != "")
				{
					CSV msgCSV;
					string msg = "";
					int pos = locMsg.IndexOf(EOL);

					while (pos > 0)
					{
						// Get the one message
						msg = locMsg.Substring(0, pos + 1);
						// Clear the message out of raw buffer
						locMsg = locMsg.Substring(pos + 1);

						// Make the message into a csv
						msgCSV = new CSV(msg);
						// Add to local array for later notification
						msgCSVs.Add(msgCSV);

						// Find the next message end point
						pos = locMsg.IndexOf(EOL);
					}

					if (locMsg.Trim().Length > 0)
					{
						// Partial message found, save it
						locPartialMsg = locMsg;
					}
					else
					{
						locPartialMsg = "";
					}
				} //if (locMsg != "")
				else
				{
					// Exit the thread
					_threadReciveProcessShouldStop = true;
				}

				if (msgCSVs.Count > 0)
				{
					lock (_syncObj)
					{
						// Add to the notification List
						RecivedCSVs.AddRange(msgCSVs.ToArray());
					}
				}

				Sleep(1);
			}

			_threadReciveProcessStarted = false;
		}

		private void OnReceive(IAsyncResult ar)
		{
			Socket socket = (Socket)ar.AsyncState;

			try
			{
				int bytesRecived = socket.EndReceive(ar);
				if (bytesRecived > 0)
				{
					string recived = Encoding.ASCII.GetString(_buffer, 0, bytesRecived);
					lock (_syncObj)
					{
						_recivedMsg = _recivedMsg + recived;
					}

					if (!_threadReciveProcessStarted)
					{
						_threadReciveProcessShouldStop = false;

						_threadReciveProcess = new System.Threading.Thread(new System.Threading.ThreadStart(ProcessRecivedMessage));
						_threadReciveProcess.Name = "ROMProcessRecived";
						_threadReciveProcess.Start();

						while (!_threadReciveProcess.IsAlive) ;
						_threadReciveProcessStarted = true;
					}

					SetupBeginReceiveCallBack(socket);
				}
				else
				{
					StatusMsg = String.Concat("OnReceive|Empty.");
					Disconnect();
				}
			}
			catch (Exception ex)
			{
				StatusMsg = String.Concat("OnReceive|Error ", ex.Message, " - ", ex.StackTrace);
				Disconnect();
			}
		}

		private void SetupBeginReceiveCallBack(Socket socket)
		{
			AsyncCallback cbReceive = new AsyncCallback(OnReceive);

			if (_buffer == null)
			{
				_buffer = new byte[socket.ReceiveBufferSize + socket.SendBufferSize];
			}
			socket.BeginReceive(_buffer, 0, _buffer.Length, SocketFlags.None, cbReceive, socket);
		}

		private void OnConnect(IAsyncResult ar)
		{
			Socket socket = (Socket)ar.AsyncState;

			try
			{
				if (socket.Connected)
				{
					Status = ROMStatusTypes.Connected;
					SetupBeginReceiveCallBack(socket);
				}
				else
				{
					Disconnect();
				}
			}
			catch (Exception ex)
			{
				StatusMsg = String.Concat("OnConnect|Error ", ex.Message, " - ", ex.StackTrace);
				Disconnect();
			}
		}

		public void Connect()
		{
			try
			{
				Status = ROMStatusTypes.Connecting;

				IPHostEntry hostEntry = Dns.GetHostEntry(_ip);
				IPAddress[] hostIPs = hostEntry.AddressList;
				_romIP = new IPEndPoint(hostIPs[0], _port);

				// Create new socket
				if (_socket == null)
				{
					_socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
				}
				_socket.Blocking = false;

				AsyncCallback cbConnect = new AsyncCallback(OnConnect);
				_socket.BeginConnect(_romIP, cbConnect, _socket);
			}
			catch (Exception ex)
			{
				StatusMsg = String.Concat("Connect|Error ", ex.Message, " - ", ex.StackTrace);
				Disconnect();
			}
		}

		public void Disconnect()
		{
			try
			{
				_threadReciveProcessShouldStop = true;
				_threadSendProcessShouldStop = true;

				if (_threadReciveProcess != null)
				{
					while (_threadReciveProcess.IsAlive) ;
				}
				if (_threadSendProcess != null)
				{
					while (_threadSendProcess.IsAlive) ;
				}

				Status = ROMStatusTypes.Disconnecting;

				if (_socket != null && _socket.Connected)
				{
					_socket.Shutdown(SocketShutdown.Both);
					Sleep(100);
				}
			}
			catch (Exception ex)
			{
				StatusMsg = String.Concat("Disconnect|Error ", ex.Message, " - ", ex.StackTrace);
			}

			_socket = null;
			Status = ROMStatusTypes.Disconnected;
		}

		public void Send(string msg)
		{
			lock (_syncObj)
			{
				_sendMsgs.Add(msg);
			}

			if (!_threadSendProcessStarted)
			{
				_threadSendProcessShouldStop = false;
				
				_threadSendProcess = new System.Threading.Thread(new System.Threading.ThreadStart(ProcessSendMessage));
				_threadSendProcess.Name = "ROMProcessSend";
				_threadSendProcess.Start();

				while (!_threadSendProcess.IsAlive) ;
				_threadSendProcessStarted = true;
			}
		}

		private void ProcessSendMessage()
		{
			while (!_threadSendProcessShouldStop)
			{
				string[] locMsgs;
				lock (_syncObj)
				{
					if (!_sending && _sendMsgs.Count > 0)
					{
						locMsgs = _sendMsgs.ToArray();
						_sendMsgs.Clear();
					}
					else
					{
						locMsgs = null;
						Sleep(1);
					}
				}

				if (locMsgs != null)
				{
					string msg = "";
					foreach (string logMsg in locMsgs)
					{
						msg = msg + logMsg;
					}

					lock (_syncObj)
					{
						byte[] bm = Encoding.ASCII.GetBytes(msg.ToCharArray());

						AsyncCallback cbSend = new AsyncCallback(OnSend);
						_socket.BeginSend(bm, 0, bm.Length, SocketFlags.None, cbSend, _socket);

						_sending = true;
					}

					Sleep(1);
				} //if (locMsgs != null)
			}

			_threadSendProcessStarted = false;
		}

		private void OnSend(IAsyncResult ar)
		{
			Socket socket = (Socket)ar.AsyncState;

			try
			{
				socket.EndSend(ar);
			}
			catch (Exception ex)
			{
				StatusMsg = String.Concat("OnSend|Error ", ex.Message, " - ", ex.StackTrace);
				Disconnect();
			}

			lock (_syncObj)
			{
				_sending = false;
			}
		}
	}
}
