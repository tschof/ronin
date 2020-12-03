using System;
using System.Net.Sockets;
using System.Text;
using System.Collections.Generic;
using System.Diagnostics;

namespace SocketEx
{
	public class ROMClient : SocketBaseClient
	{
		private const string NUL = "\0";
		private const string EOL = "\n";
		private byte EOL_Byte = 10;
		private string[] EOL_Splits = new string[] { EOL };

		public ROMClient()
			: base()
		{
		}

		public ROMClient(Socket socket, int bufferSizeLimit)
			: base(socket, bufferSizeLimit)
		{
		}

		public ROMClient(string ip, int port, byte[] buff, int bufferSizeLimit)
			: base(ip, port, buff, bufferSizeLimit)
		{
		}

		public override void Send(string msg)
		{
			lock (base.SyncObj)
			{
				byte[] data = Encoding.ASCII.GetBytes(msg.ToCharArray());
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
					Array.Copy(base.ReceivedBuffer, 0, input, 0, bytesRecived);
					lock (base.IncommingData)
					{
						base.IncommingData.AddRange(input);
					}

					if (!base.Stopping)
					{
						base.StartReceiveThread();
					}

					base.SetupBeginReceiveCallBack(socket);
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
						OnSocketEvent(new ClientEventArgs(ClientEventTypes.OnError, socket, "Socket|OnReceive|OnConnectReset: Client Disconnect From "));
						break;
				}
			}
		}

		public override void ProcessRecived()
		{
			List<byte> locIncommingData = new List<byte>();

			while (!base.ThreadRecivedProcessShouldStop)
			{
				lock (base.IncommingData)
				{
					if (base.IncommingData.Count > 0)
					{
						locIncommingData.AddRange(base.IncommingData.ToArray());
						base.IncommingData.Clear();
					}
				}

				if (locIncommingData.Count > 0)
				{
					#region - Not Used -

					//List<string> reciveds = new List<string>();
					//int lastIndex = 0;
					//int endIndex = -1;
					//byte[] locMessagData = new byte[0];

					#endregion

					int endIndex = locIncommingData.IndexOf(EOL_Byte);
					if (endIndex == -1)
					{
						// nothing found
					}
					else if (endIndex > -1)
					{
						// found something
						byte[] locMessagData = new byte[endIndex];
						locIncommingData.CopyTo(0, locMessagData, 0, endIndex);
						locIncommingData.RemoveRange(0, endIndex + 1);

						List<string> reciveds = new List<string>(Encoding.ASCII.GetString(locMessagData).Split(EOL_Splits, StringSplitOptions.RemoveEmptyEntries));

						OnSocketEvent(new ClientEventArgs(ClientEventTypes.OnReceive, base.ClientSocket, reciveds.ToArray()));
					}
					else if (endIndex == locIncommingData.Count - 1)
					{
						// all in buffer is completed message
						List<string> reciveds = new List<string>(Encoding.ASCII.GetString(locIncommingData.ToArray()).Split(EOL_Splits, StringSplitOptions.RemoveEmptyEntries));
						locIncommingData = new List<byte>();

						OnSocketEvent(new ClientEventArgs(ClientEventTypes.OnReceive, base.ClientSocket, reciveds.ToArray()));
					}

					#region - Not Used -

					//while (index > 0)
					//{
					//    locMessagData = new byte[index - lastIndex];
					//    Array.Copy(locIncommingData.ToArray(), lastIndex, locMessagData, 0, locMessagData.Length);

					//    lastIndex = index + 1;

					//    // Add single message
					//    reciveds.Add(Encoding.ASCII.GetString(locMessagData));

					//    if (reciveds.Count > 10)
					//    {
					//        OnSocketEvent(new ClientEventArgs(ClientEventTypes.OnReceive, base.ClientSocket, reciveds.ToArray()));
					//        reciveds.Clear();
					//        endIndex = index;
					//        break;
					//    }

					//    index = locIncommingData.IndexOf(EOL_Byte, lastIndex);
					//}

					//for (int index = 0; index < locIncommingData.Count; index++)
					//{
					//    if (locIncommingData[index] == EOL_Byte)
					//    {
					//        locMessagData = new byte[index - lastIndex];
					//        Array.Copy(locIncommingData.ToArray(), lastIndex, locMessagData, 0, locMessagData.Length);

					//        lastIndex = index + 1;

					//        // Add single message
					//        reciveds.Add(Encoding.ASCII.GetString(locMessagData));

					//        if (reciveds.Count > 100)
					//        {
					//            OnSocketEvent(new ClientEventArgs(ClientEventTypes.OnReceive, base.ClientSocket, reciveds.ToArray()));
					//            reciveds.Clear();
					//            endIndex = index;
					//            break;
					//        }
					//    }
					//}

					//if (reciveds.Count > 0)
					//{
					//    OnSocketEvent(new ClientEventArgs(ClientEventTypes.OnReceive, base.ClientSocket, reciveds.ToArray()));
					//}

					//if (endIndex == -1)
					//{
					//    // All data processed
					//    locIncommingData = new List<byte>();
					//}
					//else
					//{
					//    locIncommingData.RemoveRange(0, endIndex);
					//}

					#endregion
				}
				else
				{
					System.Threading.Thread.Sleep(1);
				}
			}

			base.ThreadRecivedProcessStarted = false;
		}

		//private void ProcessRecived1()
		//{
		//    string locIncommingString = "";

		//    while (!base.ThreadRecivedProcessShouldStop)
		//    {
		//        byte[] locInput = new byte[0];
		//        lock (base.IncommingData)
		//        {
		//            if (base.IncommingData.Count > 0)
		//            {
		//                locInput = new byte[base.IncommingData.Count];
		//                Array.Copy(base.IncommingData.ToArray(), 0, locInput, 0, locInput.Length);
		//                //locInput = ByteArrayOperations.CopyFromByteArray(base.IncommingData.ToArray(), 0, locInput.Length);
		//                base.IncommingData.Clear();
		//            }
		//        }

		//        if (locInput.Length > 0)
		//        {
		//            string locString = Encoding.ASCII.GetString(locInput);
		//            //OnSocketEvent(new ClientEventArgs(ClientEventTypes.OnDebug, String.Format("Buffered Input: {0}", locString.Length.ToString())));
		//            if (locIncommingString.Length == 0)
		//            {
		//                locIncommingString = locString;
		//            }
		//            else
		//            {
		//                locIncommingString = String.Concat(locIncommingString, locString);
		//            }
		//        }

		//        //lock (base.IncommingString)
		//        //{
		//        //    if (base.IncommingString.Length > 0)
		//        //    {
		//        //        lock (base.SyncObj)
		//        //        {
		//        //            if (locIncommingString.Length == 0)
		//        //            {
		//        //                locIncommingString = base.IncommingString;
		//        //            }
		//        //            else
		//        //            {
		//        //                locIncommingString = String.Concat(locIncommingString, base.IncommingString);
		//        //            }
		//        //            base.IncommingString = "";
		//        //        }
		//        //    }
		//        //}

		//        if (locIncommingString != "")
		//        {
		//            //lock (base.SyncObj)
		//            //{
		//                List<string> reciveds = new List<string>();
		//                int pos = locIncommingString.IndexOf(EOL);
		//                while (pos > 0)
		//                {
		//                    // Set the one message
		//                    reciveds.Add(locIncommingString.Substring(0, pos + 1));

		//                    // Clear the message out of raw buffer
		//                    locIncommingString = locIncommingString.Substring(pos + 1);

		//                    if (reciveds.Count > 100)
		//                    {
		//                        OnSocketEvent(new ClientEventArgs(ClientEventTypes.OnReceive, base.ClientSocket, reciveds.ToArray()));
		//                        reciveds.Clear();
		//                    }

		//                    pos = locIncommingString.IndexOf(EOL);
		//                    if (locIncommingString.Length == 1 && locIncommingString == NUL)
		//                    {
		//                        locIncommingString = "";
		//                    }
		//                }

		//                if (reciveds.Count > 0)
		//                {
		//                    OnSocketEvent(new ClientEventArgs(ClientEventTypes.OnReceive, base.ClientSocket, reciveds.ToArray()));
		//                }
		//            //}

		//            if (locIncommingString.Trim().Length > 0)
		//            {
		//                // partial message found, keep it in local Incomming String.
		//            }
		//            else
		//            {
		//                locIncommingString = "";
		//            }
		//        }
		//        else
		//        {
		//            System.Threading.Thread.Sleep(1);
		//        }
		//    }

		//    base.ThreadRecivedProcessStarted = false;
		//}
	}
}
