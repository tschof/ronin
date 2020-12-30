#undef RONIN

#if RONIN
using System;
using System.Net.Sockets;
using System.Diagnostics;

namespace SocketEx
{
	public class ROMServer : SocketBaseServer
	{
		public ROMServer()
			: base()
		{
		}

		public ROMServer(string ip, int port, int maxBackLog, byte[] buffer, int bufferSizeLimit)
			: base(ip, port, maxBackLog, buffer, bufferSizeLimit)
		{
		}

		public override void Send(string msg)
		{
			lock (base.SyncObj)
			{
				foreach (IntPtr key in base.Clients.Keys)
				{
					try
					{
						base.Clients[key].Send(msg);
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

		public override void Send(IntPtr key, string msg)
		{
			lock (base.SyncObj)
			{
				// Send to only specified
				if (base.Clients.ContainsKey(key))
				{
					try
					{
						base.Clients[key].Send(msg);
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

		public override void OnListenerAccept(IAsyncResult ar)
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
						ROMClient sc = new ROMClient(socket, base.ClientBufferSizeLimit);
						sc.OnClientEvent += new ClientEventHandler(base.OnClientEvent);
						base.Clients.Add(key, sc);

						base.OnSocketEvent(new ServerEventArgs(ServerEventTypes.OnListenerAccept, socket, "Listener|Accept: New Client From ", null));
					}
				}
				base.SetupAcceptSocketCallBack(listener);
			}
		}
	}
}
#else
namespace SocketEx
{
	public class ROMServer
	{
		public ROMServer()
		{
		}

		public ROMServer(string ip, int port, int maxBackLog, byte[] buffer, int bufferSizeLimit)
		{
			
		}

		public void Send(string msg)
		{
		}

		public void Send(System.IntPtr key, string msg)
		{
		}
	}
}
#endif // RONIN
