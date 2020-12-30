using System.Net.Sockets;

namespace MarketData.Ronin
{
	// Handles the excessively complicated task of receiving market data messages
	// from the remote server.
	internal class ProducerSocket : SocketEx.SocketBaseClient // From MDSClient.cs, HelperMDS.cs
	{
		private readonly System.Action<SocketEx.ClientEventArgs> _onSocketEvent;
		private readonly System.Action _onActivity;

		internal ProducerSocket(
			string ip, int port, int bufferSize, int clientBufferSizeLimit,
			System.Action<SocketEx.ClientEventArgs> onSocketEvent, System.Action onActivity) :
			base(ip, port, new byte[bufferSize], clientBufferSizeLimit)
		{
			_onSocketEvent = onSocketEvent;
			_onActivity = onActivity;
		}

		public override void OnReceive(System.IAsyncResult ar)
		{
			Socket socket = (Socket)ar.AsyncState;

			try {
				int bytesRecived = socket.EndReceive(ar);
				if (bytesRecived > 0) {
					_onActivity();

					byte[] input = new byte[bytesRecived];
					input = SocketEx.ByteArrayOperations.CopyFromByteArray(base.ReceivedBuffer, 0, bytesRecived);
					lock (this) {
						base.Incoming.AddRange(input);
					}

					base.SetupBeginReceiveCallBack(socket);

					if (!base.Stopping) {
						base.StartReceiveThread();
					}
				} else {
					base.Connected = false;
					base.ReadyToSend = false;

					_onSocketEvent(new SocketEx.ClientEventArgs(SocketEx.ClientEventTypes.OnDisconnect, socket, "Socket|OnReceive|OnDisconnect: Client Disconnect With Zero Bytes From "));
					socket.Close();
				}
			} catch (SocketException ex) {
				if (System.Diagnostics.Debugger.IsAttached) {
					System.Diagnostics.Debug.Print(ex.Message);
					System.Diagnostics.Debug.Print(ex.StackTrace);
				}

				switch (ex.ErrorCode) {
					case 10054: //An existing connection was forcibly closed by the remote host
						base.Connected = false;
						base.ReadyToSend = false;

						_onSocketEvent(new SocketEx.ClientEventArgs(SocketEx.ClientEventTypes.OnDisconnect, socket, "Socket|OnReceive|OnConnectReset: Client Disconnect From "));
						break;
					default:
						_onSocketEvent(new SocketEx.ClientEventArgs(SocketEx.ClientEventTypes.OnError, socket, "Socket|OnReceive|OnError: " + ex.Message));
						break;
				}
			}
		}

		public override void ProcessReceived()
		{
			byte[] incoming = new byte[0];

			while (!base.ThreadRecivedProcessShouldStop) {
				try {
					if (base.Incoming.Count > 0) {
						lock (this) {
							incoming = SocketEx.ByteArrayOperations.AddByteArrays(incoming, base.Incoming.ToArray());
							base.Incoming.Clear();
						}
						incoming = processChunk(incoming);
					} else {
						System.Threading.Thread.Sleep(1);
					}
				} catch (System.Exception ex) {
					if (System.Diagnostics.Debugger.IsAttached) {
						System.Diagnostics.Debug.Print(ex.Message);
						System.Diagnostics.Debug.Print(ex.StackTrace);
					}
					System.Threading.Thread.Sleep(1);
				}
			}

			base.ThreadRecivedProcessStarted = false;
		}

		public override void Send(string msg)
		{
			// Not used.
		}

		private byte[] processChunk(byte[] incoming)
		{
			long start = 0;
			long end = 0;

			if ((incoming == null) || (incoming.Length <= 0))
				return incoming;

			if (incoming.Length > 0) {
				long totalLength = incoming.Length;
				while ((start < totalLength) && !base.ThreadRecivedProcessShouldStop) {
					// Get the Data Length Prefix
					int length = incoming[start];
					// Offset form the first byte
					start += 1;

					if ((start + length >= totalLength) && (end != 0)) {
						// Partial Recived
						var slice = new byte[totalLength - end];
						System.Array.Copy(incoming, end, slice, 0, totalLength - end);
						return slice;
					} else {
						// Get the data length
						var slice = new byte[length];
						System.Array.Copy(incoming, start, slice, 0, length);
						length = System.BitConverter.ToInt32(slice, 0);

						// Real data start point
						start += slice.Length;

						if ((start + length >= totalLength) && (end != 0)) {
							// Partial Recived
							slice = new byte[totalLength - end];
							System.Array.Copy(incoming, end, slice, 0, totalLength - end);
							return slice;
						}

						slice = new byte[length];
						System.Array.Copy(incoming, start, slice, 0, length);
						_onSocketEvent(new SocketEx.ClientEventArgs(SocketEx.ClientEventTypes.OnReceive, base.ClientSocket, "", slice));

						// for the next start;
						start += slice.Length;
						end = start;

						if (start == totalLength) {
							// Got everything clear local buffer
							return new byte[0];
						}
					}
				}
			}
			return incoming;
		}
	}
}
