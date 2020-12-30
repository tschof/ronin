#if OLD
using System;
using System.Collections.Generic;

using System.Net.Sockets;
using System.Text;
using System.Diagnostics;

namespace SocketEx
{
	public class MDSServer : SocketBaseServer
	{
		public MDSServer()
			: base()
		{
		}

		public MDSServer(string ip, int port, int maxBackLog, byte[] buffer, int bufferSizeLimit)
			: base(ip, port, maxBackLog, buffer, bufferSizeLimit)
		{
		}

		public override void Send(string msg)
		{
			lock (base.SyncObj)
			{
				byte[] data = Encoding.ASCII.GetBytes(msg.ToCharArray());
				data = SocketTransmitionControl.AddPrefix(data);

				try
				{
					foreach (IntPtr key in base.Clients.Keys)
					{
						Send(base.Clients[key], data);
					}
				}
				catch (Exception ex)
				{
					if (Debugger.IsAttached)
					{
						Debug.Print(ex.Message);
						Debug.Print(ex.StackTrace);
					}

					base.OnSocketEvent(new ServerEventArgs(ServerEventTypes.OnError, "Client Already Removed."));
				}
			}
		}

		public override void Send(IntPtr key, string msg)
		{
			lock (base.SyncObj)
			{
				// Send to only specified
				byte[] data = Encoding.ASCII.GetBytes(msg.ToCharArray());
				data = SocketTransmitionControl.AddPrefix(data);

				if (base.Clients.TryGetValue(key, out SocketBaseClient target))
				{
					try
					{
						Send(target, data);
					}
					catch (Exception ex)
					{
						if (Debugger.IsAttached)
						{
							Debug.Print(ex.Message);
							Debug.Print(ex.StackTrace);
						}

						base.OnSocketEvent(new ServerEventArgs(ServerEventTypes.OnError, "Client Already Removed."));
					}
				}
			}
		}

		public void Send(IntPtr key, List<string> msgs)
		{
			try
			{
				lock (base.SyncObj)
				{
					//Send to only specified
					if (base.Clients.TryGetValue(key, out SocketBaseClient target))
					{
						foreach (string msg in msgs)
						{
							byte[] data = Encoding.ASCII.GetBytes(msg.ToCharArray());
							data = SocketTransmitionControl.AddPrefix(data);
							Send(target, data);
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

		public void SendRawBytes(IntPtr key, byte[] msg)
		{
			lock (base.SyncObj)
			{
				// Send to only specified
				//byte[] data = Encoding.ASCII.GetBytes(msg.ToCharArray());
				byte[] data = SocketTransmitionControl.AddPrefix(msg);

				if (base.Clients.TryGetValue(key, out SocketBaseClient target))
				{
					try
					{
						Send(target, data);
					}
					catch (Exception ex)
					{
						if (Debugger.IsAttached)
						{
							Debug.Print(ex.Message);
							Debug.Print(ex.StackTrace);
						}

						base.OnSocketEvent(new ServerEventArgs(ServerEventTypes.OnError, "Client Already Removed."));
					}
				}
			}
		}

		public void SendRawBytes(IntPtr key, List<byte[]> msgs)
		{
			lock (base.SyncObj)
			{
				//Send to only specified
				if (base.Clients.TryGetValue(key, out SocketBaseClient target))
				{
					foreach (byte[] msg in msgs)
					{
						byte[] data = SocketTransmitionControl.AddPrefix(msg);

						try
						{
							Send(target, data);
						}
						catch (Exception ex)
						{
							if (Debugger.IsAttached)
							{
								Debug.Print(ex.Message);
								Debug.Print(ex.StackTrace);
							}

							base.OnSocketEvent(new ServerEventArgs(ServerEventTypes.OnError, "Client Already Removed."));
						}
					}
				}
			}
		}

		private void Send(SocketBaseClient client, byte[] data)
		{
			try
			{
				if (client.OutGoingData.Count + data.Length < client.ClientBufferSizeLimit)
				{
					client.Send(data);
				}
				else
				{
					// Wait for the buffer to drain
					//OnSocketEvent(new ClientEventArgs(ClientEventTypes.OnError, "OutGoingData Buffer Full"));
				}
			}
			catch (Exception ex)
			{
				if (Debugger.IsAttached)
				{
					Debug.Print(ex.Message);
					Debug.Print(ex.StackTrace);
				}

				base.OnSocketEvent(new ServerEventArgs(ServerEventTypes.OnError, "Client Already Removed."));
			}
		}

		public override void OnListenerAccept(IAsyncResult ar)
		{
			try
			{
				if (base.ListenerRunning)
				{
					TcpListener listener = (TcpListener)ar.AsyncState;
					Socket socket = listener.EndAcceptSocket(ar);

					IntPtr key = socket.Handle;
					if (!base.Clients.ContainsKey(key))
					{
						lock (base.SyncObj)
						{
							MDSClient sc = new MDSClient(socket, base.ClientBufferSizeLimit);
							sc.OnClientEvent += new ClientEventHandler(base.OnClientEvent);
							base.Clients.Add(key, sc);

							base.OnSocketEvent(new ServerEventArgs(ServerEventTypes.OnListenerAccept, socket, "Listener|Accept: New Client From ", null));
						}
					}
					base.SetupAcceptSocketCallBack(listener);
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
	}
}
#endif // OLD