using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Collections;

using SocketEx;
using DateTimeEx;
using LoggerEx;

namespace SocketEx_TestServer
{
	public partial class frmMainServer : Form
	{
		private Logger _log = new Logger(LogTypes.TCP);
		private DateTimeHP _dtHP = new DateTimeHP();

		private SocketServer _server;
		private Dictionary<string, DateTime> _messages = new Dictionary<string, DateTime>();

		private string _status;
		private Timer _timer;

		private object _syncObj = new object();

		public frmMainServer()
		{
			InitializeComponent();

			if (_server == null)
			{
				_server = new SocketServer();
				_server.OnServerEvent += new ServerEventHandler(SocketServer_OnServerEvent);
			}

			ShowStartUI();
			StartTimer();
		}

		private void cmdStart_Click(object sender, EventArgs e)
		{
			_server.Start();
			ShowStopUI();
		}

		private void cmdStop_Click(object sender, EventArgs e)
		{
			_server.Stop();
			ShowStartUI();
		}

		private void ShowStartUI()
		{
			cmdStart.Enabled = true;
			cmdStop.Enabled = false;
		}

		private void ShowStopUI()
		{
			cmdStart.Enabled = false;
			cmdStop.Enabled = true;
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
			lock (_syncObj)
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

			dspClientsConnected.Text = _server.Clients.Count.ToString();

			_timer.Start();
		}

		private void SocketServer_OnServerEvent(object sender, ServerEventArgs e)
		{
			switch (e.Type)
			{
				case ServerEventTypes.OnDisconnect:
					_status = e.Message + e.Client.LocalEndPoint.ToString();
					break;
				case ServerEventTypes.OnError:
					_status = e.Message + e.Client.LocalEndPoint.ToString();
					break;
				case ServerEventTypes.OnListenerStart:
					_status = e.Message;
					break;
				case ServerEventTypes.OnListenerStop:
					_status = e.Message;
					break;
				case ServerEventTypes.OnListenerAccept:
					_status = e.Message + e.Client.LocalEndPoint.ToString();
					break;
				case ServerEventTypes.OnRawReceive:
					//_status = e.Message + e.Client.LocalEndPoint.ToString();
					break;
				case ServerEventTypes.OnReceive:
					lock (_syncObj)
					{
						_messages.Add("OnReceive " + e.Message, _dtHP.UtcNow);
						_server.Send(e.Message);
					}
					break;
				case ServerEventTypes.OnSend:
					_status = e.Message + e.Client.LocalEndPoint.ToString();
					break;
			}
		}

		private void frmMain_FormClosing(object sender, FormClosingEventArgs e)
		{
			_server.Stop();
		}
	}
}