using System;
using System.Data;
using System.Net.Sockets;
using System.Text;
using System.Reflection;
using System.Threading;
namespace OptionManWebService
{
	/// <summary>
	/// Summary description for ClientSocket.
	/// </summary>
	public class ClientSocket : TcpClient
	{
		private Timer timer;
		private TimerCallback tcb = new TimerCallback(ClientSocket.OnTimer);
		private static ClientSocket m_Socket = null;
		private bool bConnect = false;
		public static void Run()
		{
			ClientSocket sock = ClientSocket.getInstance();
			string szHost = "rag19";
			int nPort = 8088;
			NetworkStream networkStream = null;
			while( sock.bConnect == false )
			{
				try
				{
					sock.Connect(szHost, nPort);
					networkStream = sock.GetStream();
					sock.bConnect = true;
				}
				catch( Exception e )
				{
					System.Console.WriteLine(e.StackTrace);
					System.Console.WriteLine(e.Message);
				}
			}
			while(!sock.Active)
			{
				if( networkStream.CanRead )
				{
					byte[] bytes = new byte[sock.ReceiveBufferSize];
					networkStream.Read(bytes, 0, (int) sock.ReceiveBufferSize);
					String returndata = Encoding.ASCII.GetString(bytes);
					continue;
				}
			}
		}
	
		private static void OnTimer( object param )
		{
		}
		private ClientSocket()
		{
			//
			// TODO: Add constructor logic here
			//
			timer = new Timer(tcb,this,0,5000);
			/// set up the timer event handler
			/// set the interval
			/// start the timer
			Thread thread = new Thread( new ThreadStart(ClientSocket.Run) );
			thread.Start();
		}
		public static ClientSocket getInstance()
		{
			if( m_Socket == null )
				m_Socket = new ClientSocket();
			return m_Socket;
		}
		public bool Send( string szMsg )
		{
			try
			{
				if( !GetStream().CanWrite )
					return false;
				Byte[] sendBytes = Encoding.ASCII.GetBytes(szMsg);
				GetStream().Write(sendBytes, 0, sendBytes.Length);
			}
			catch( Exception e )
			{
				System.Console.WriteLine(e.StackTrace);
				System.Console.WriteLine(e.Message);
			}
			return true;
		}
	}
}
