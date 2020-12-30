#if OLD
using System;
using System.Net.Sockets;
using System.Text;
using System.Diagnostics;
using System.Collections.Generic;

namespace SocketEx
{
	public class MDSClient : SocketBaseClient
	{
		public MDSClient()
			: base()
		{
		}

		public MDSClient(Socket socket, int bufferSizeLimit)
			: base(socket, bufferSizeLimit)
		{
		}

		public MDSClient(string ip, int port, byte[] buff, int bufferSizeLimit)
			: base(ip, port, buff, bufferSizeLimit)
		{
		}

		public override void Send(string msg)
		{
			lock (base.SyncObj)
			{
				byte[] data = Encoding.ASCII.GetBytes(msg.ToCharArray());
				data = SocketTransmitionControl.AddPrefix(data);
				base.Send(data);
			}
		}

		public void SendRawBytes(byte[] msg)
		{
			lock (base.SyncObj)
			{
				byte[] data = SocketTransmitionControl.AddPrefix(msg);
				base.Send(data);
			}
		}

		public override void OnReceive(IAsyncResult ar)
		{
			Socket socket = (Socket)ar.AsyncState;

			try
			{
				int bytesRecived = socket.EndReceive(ar);
				if (bytesRecived > 0)
				{
					byte[] input = new byte[bytesRecived];
					input = ByteArrayOperations.CopyFromByteArray(base.ReceivedBuffer, 0, bytesRecived);
					lock (base.SyncObj)
					{
						base.Incoming.AddRange(input);
					}

					base.SetupBeginReceiveCallBack(socket);

					if (!base.Stopping)
					{
						base.StartReceiveThread();
					}
				}
				else
				{
					base.Connected = false;
					base.ReadyToSend = false;

					OnSocketEvent(new ClientEventArgs(ClientEventTypes.OnDisconnect, socket, "Socket|OnReceive|OnDisconnect: Client Disconnect With Zero Bytes From "));
					socket.Close();
				}
			}
			catch (SocketException ex)
			{
				if (Debugger.IsAttached)
				{
					Debug.Print(ex.Message);
					Debug.Print(ex.StackTrace);
				}

				switch (ex.ErrorCode)
				{
					case 10054: //An existing connection was forcibly closed by the remote host
						base.Connected = false;
						base.ReadyToSend = false;

						OnSocketEvent(new ClientEventArgs(ClientEventTypes.OnDisconnect, socket, "Socket|OnReceive|OnConnectReset: Client Disconnect From "));
						break;
					default:
						OnSocketEvent(new ClientEventArgs(ClientEventTypes.OnError, socket, "Socket|OnReceive|OnError: " + ex.Message));
						break;
				}
			}
		}

		public override void ProcessReceived()
		{
			byte[] locIncommingData = new byte[0];
			bool hasNewData = false;
			while (!base.ThreadRecivedProcessShouldStop)
			{
				try
				{
					if (base.Incoming.Count > 0)
					{
						lock (base.SyncObj)
						{
							locIncommingData = ByteArrayOperations.AddByteArrays(locIncommingData, base.Incoming.ToArray());
							base.Incoming.Clear();
							hasNewData = true;
						}
					}
					else
					{
						hasNewData = false;
					}

					if (hasNewData)
					{
						long start = 0;
						long end = 0;
						int length = 0;
						byte[] tempData = new byte[0];
						List<byte[]> datasRecived = new List<byte[]>();
						if (locIncommingData.Length > 0)
						{
							long totalLength = locIncommingData.Length;
							while (start < totalLength)
							{
								if (base.ThreadRecivedProcessShouldStop) break;

								// Get the Data Length Prefix
								length = locIncommingData[start];
								// Offset form the first byte
								start = start + 1;

								if (start + length >= totalLength && end != 0)
								{
									// Partial Recived
									tempData = new byte[totalLength - end];
									Array.Copy(locIncommingData, end, tempData, 0, totalLength - end);
									locIncommingData = tempData;
									break;
								}

								// Get the data length
								tempData = new byte[length];
								Array.Copy(locIncommingData, start, tempData, 0, length);
								length = BitConverter.ToInt32(tempData, 0);

								// Real data start point
								start = start + tempData.Length;

								if (start + length >= totalLength && end != 0)
								{
									// Partial Recived
									tempData = new byte[totalLength - end];
									Array.Copy(locIncommingData, end, tempData, 0, totalLength - end);
									locIncommingData = tempData;
									break;
								}

								tempData = new byte[length];
								Array.Copy(locIncommingData, start, tempData, 0, length);
								OnSocketEvent(new ClientEventArgs(ClientEventTypes.OnReceive, base.ClientSocket, "", tempData));

								// for the next start;
								start = start + tempData.Length;
								end = start;

								if (start == totalLength)
								{
									// Got everything clear local buffer
									locIncommingData = new byte[0];
									break;
								}
							}

							#region - Not Used -

							//byte[] dataRecived = SocketTransmitionControl.ExtractIntegratedDataFromBuffer(ref locIncommingData);
							////int count = 0;
							//while (dataRecived.Length > 0)
							//{
							//    if (base.ThreadRecivedProcessShouldStop) break;
							//    OnSocketEvent(new ClientEventArgs(ClientEventTypes.OnReceive, base.ClientSocket, "", dataRecived));

							//    dataRecived = new byte[0];
							//    dataRecived = SocketTransmitionControl.ExtractIntegratedDataFromBuffer(ref locIncommingData);

							//    //if (count > 10)
							//    //{
							//    //    count = 0;
							//    //    System.Threading.Thread.Sleep(1);
							//    //}
							//    //count++;
							//}

							#endregion
						}
					}
					else
					{
						System.Threading.Thread.Sleep(1);
					}
				}
				catch (Exception ex)
				{
					if (Debugger.IsAttached)
					{
						Debug.Print(ex.Message);
						Debug.Print(ex.StackTrace);
					}
					System.Threading.Thread.Sleep(1);
					//OnSocketEvent(new ClientEventArgs(ClientEventTypes.OnError, null, "Socket|ProcessRecived|Exception: " + ex.Message));
					//OnSocketEvent(new ClientEventArgs(ClientEventTypes.OnError, null, "Socket|ProcessRecived|Exception: " + ex.StackTrace));
				}
			}

			base.ThreadRecivedProcessStarted = false;
		}
	}
}
#endif // OLD