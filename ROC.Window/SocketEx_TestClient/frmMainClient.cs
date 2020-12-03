using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

using SocketEx;
using DateTimeEx;
using LoggerEx;

namespace SocketEx_TestClient
{
	public partial class frmMainClient : Form
	{
		private Logger _log = new Logger(LogTypes.TCP);
		private DateTimeHP _dtHP = new DateTimeHP();

		private MDSClient _client;
		private Dictionary<string, DateTime> _messages = new Dictionary<string,DateTime>();
		private string _status;
		private Timer _timer;

		private object _syncObj = new object();
		private bool _stopSend = false;

		public frmMainClient()
		{
			InitializeComponent();

			if (_client == null)
			{
				_client = new MDSClient("ddart-ltc-rom1", 8008, new byte[4048], 1);
				_client.OnClientEvent += new ClientEventHandler(OnClientEvent);
			}

			ShowConnectUI();
			StartTimer();
		}

		private void cmdConnect_Click(object sender, EventArgs e)
		{
			_client.Start();
			ShowDisconnectUI();
		}

		private void cmdDisconnect_Click(object sender, EventArgs e)
		{
			_client.Stop();
			ShowConnectUI();
		}

		private void cmdSend_Click(object sender, EventArgs e)
		{
			_client.Start();
			_client.Send(txtSend.Text);
		}

		private void ShowConnectUI()
		{
			cmdConnect.Enabled = true;
			cmdDisconnect.Enabled = false;
		}

		private void ShowDisconnectUI()
		{
			cmdConnect.Enabled = false;
			cmdDisconnect.Enabled = true;
		}

		private void StartTimer()
		{
			if (_timer == null)
			{
				_timer = new Timer();
				_timer.Interval = 100;
				_timer.Tick += new EventHandler(Timer_Tick);
				_timer.Enabled = true;
			}
		}

		private void Timer_Tick(object sender, EventArgs e)
		{
			_timer.Stop();

			txtStatus.Text = _status;

			string[] mkeys = new string[0];
			lock(_syncObj)
			{
				mkeys = new string[_messages.Count];
				_messages.Keys.CopyTo(mkeys, 0);
				_messages.Clear();
			}
			foreach (string mkey in mkeys)
			{
				_log.Out(mkey);

				if (rtbMessages.Lines.Length > 10)
				{
					rtbMessages.Text = "";
				}

				rtbMessages.AppendText(String.Concat(new object[] { mkey, "\n" }));
			}

			//rtbMessages.SelectionStart = rtbMessages.TextLength;
			//rtbMessages.ScrollToCaret();

			_timer.Start();
		}

		private void OnClientEvent(object sender, ClientEventArgs e)
		{
			switch (e.Type)
			{
				case ClientEventTypes.OnDisconnect:
					_status = e.Message + e.Client.RemoteEndPoint.ToString();
					break;
				case ClientEventTypes.OnConnect:
					_status = e.Message + e.Client.RemoteEndPoint.ToString();
					break;
				case ClientEventTypes.OnError:
					_status = e.Message;
					break;
				case ClientEventTypes.OnRawReceive:
					//_status = e.Message + e.Client.RemoteEndPoint.ToString();
					break;
				case ClientEventTypes.OnReceive:
					lock (_syncObj)
					{
						_messages.Add(e.Message, _dtHP.UtcNow);
					}
					break;
				case ClientEventTypes.OnSend:
					_status = e.Message + e.Client.RemoteEndPoint.ToString();
					break;
			}
		}

		private void cmdAutoSend_Click(object sender, EventArgs e)
		{
			_stopSend = false;
			System.Threading.ThreadPool.QueueUserWorkItem(AutoSend);
		}

		private void cmdStopAutoSend_Click(object sender, EventArgs e)
		{
			_stopSend = true;
		}

		private void AutoSend(object info)
		{
			Random rand = new Random(_dtHP.UtcNow.Millisecond);
			int count = 0;
			while (!_stopSend && count < 10000)
			{
				count++;
				_client.Send(String.Concat(new object[] { _dtHP.UtcNow.ToString("hh:mm:ss.fffffff"), " [", count.ToString("00000000"), "] ", " [", rand.Next().ToString("00000000000"), "]" }));
			}

			_client.Send(String.Concat(new object[] { _dtHP.UtcNow.ToString("hh:mm:ss.fffffff"), "END" }));
		}

		private void frmMain_FormClosing(object sender, FormClosingEventArgs e)
		{
			_client.Stop();
		}
	}
}