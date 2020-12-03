using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;

using DateTimeEx;
using MarketDataEx;
using SRLabEx;

namespace SRLabMDS
{
	public partial class frmMain : Form
	{
		#region - Local Variable -

		private DateTimeHP _dtHP = new DateTimeHP();

		private object _syncObj = new object();

		private Timer _uiTimer;

		private DateTime _stopTime;

		private char[] _keySpliter = new char[] { ',' };

		#endregion

		#region - Constructor -

		public frmMain()
		{
			InitializeComponent();

			Initialize();
		}

		private void Initialize()
		{
			Text = Configuration.User.Default.InstanceName.ToUpper();
			if (Text.Contains("TEST") || Text.Contains("DEV"))
			{
				BackColor = Color.Red;
				tabPageMain.ForeColor = Color.Red;
			}
			else if (Text.Contains("BETA"))
			{
				BackColor = Color.Yellow;
				tabPageMain.ForeColor = Color.Yellow;
			}

			notifyIconMDS.Text = Configuration.User.Default.InstanceName;
			notifyIconMDS.BalloonTipTitle = String.Concat(new object[] { Configuration.User.Default.InstanceName, " UI Minimized" });
			notifyIconMDS.BalloonTipText = String.Concat(new object[] { "Double Click to Reopen ", Configuration.User.Default.InstanceName, " UI" });

			mdsWatchList.Initialize();
			mdsWatchList.PropertyChanged += new PropertyChangedEventHandler(mdsWatchList_PropertyChanged);

			mdsUserWatchList.Initialize();

			_stopTime = Convert.ToDateTime(Configuration.User.Default.StopTime);

			StartUITimer();
		}

		#endregion

		#region - Closing Event -

		protected override void OnClosing(CancelEventArgs e)
		{
			StopUITimer();

			GLOBAL.HSRLab.Status = HelperSRLab.StatusTypes.DisConnecting;
			GLOBAL.HSRLab.Stop();

			GLOBAL.HMDS.Status = HelperMDS.StatusTypes.Stopping;
			GLOBAL.HMDS.Disconnect();

			base.OnClosing(e);
		}

		#endregion

		#region - Form Event -

		private void notifyIconMDS_MouseDoubleClick(object sender, MouseEventArgs e)
		{
			Show();
			WindowState = FormWindowState.Normal;
		}

		private void frmMain_Resize(object sender, EventArgs e)
		{
			switch (WindowState)
			{
				case FormWindowState.Minimized:
					notifyIconMDS.Visible = true;
					notifyIconMDS.ShowBalloonTip(Configuration.User.Default.NotificationDuration);
					Hide();
					break;
				default:
					notifyIconMDS.Visible = false;
					break;
			}
		}

		#endregion

		#region - UI Timer -

		private void StartUITimer()
		{
			if (_uiTimer == null)
			{
				_uiTimer = new Timer();
				_uiTimer.Interval = Configuration.User.Default.UIUpdateRate;
				_uiTimer.Tick += new EventHandler(UITick);
				_uiTimer.Enabled = true;
			}
		}

		private void StopUITimer()
		{
			if (_uiTimer != null)
			{
				_uiTimer.Stop();
				_uiTimer.Tick -= new EventHandler(UITick);
			}
			_uiTimer = null;
		}

		private void UITick(object sender, EventArgs e)
		{
			try
			{
				StopUITimer();

				ShowSRLabStatus();
				ShowServerStatus();

				// SRLAB
				GLOBAL.HSRLab.LogStatusMessages();
				GLOBAL.HSRLab.LogSRLabMessages();
				// MDS
				GLOBAL.HMDS.LogStatusMessages();
				GLOBAL.HMDS.LogMDSMessages();
				// TCP
				GLOBAL.HMDS.LogRecivedMsgs();
				GLOBAL.HMDS.LogSentMsgs();

				if (GLOBAL.AutoHide)
				{
					GLOBAL.AutoHide = false;
					WindowState = FormWindowState.Minimized;
				}

				if (GLOBAL.AutoStartLBM && GLOBAL.HSRLab.Status == HelperSRLab.StatusTypes.NotConnected)
				{
					cmdCheckEnvironments_Click(null, EventArgs.Empty);
				}

				if (GLOBAL.AutoStartLBM && GLOBAL.HSRLab.Status == HelperSRLab.StatusTypes.EnvironmentVariableChecked)
				{
					GLOBAL.AutoStartLBM = false;
				}

				if (GLOBAL.AutoLoad && GLOBAL.HSRLab.Status == HelperSRLab.StatusTypes.Connected)
				{
					GLOBAL.AutoLoad = false;
				}

				if (Configuration.User.Default.UseStopTime && DateTime.Now >= _stopTime && DateTime.Now < _stopTime.AddMinutes(1))
				{
					Close();
				}
				else
				{
					StartUITimer();
				}
			}
			catch (Exception ex)
			{
				GLOBAL.AddToExecption(ex);
			}
		}

		#endregion

		#region - SRLab -

		private long _lastDataMsgCount = 0;
		private double _lastPeakDataMsgRate = 0;

		private void ShowSRLabStatus()
		{
			ProcessSRLabStatus();

			ProcessSRLabDataMsgRate();
		}

		private void ProcessSRLabStatus()
		{
			if (GLOBAL.HSRLab.StatusChanged)
			{
				GLOBAL.HSRLab.StatusChanged = false;
				switch (GLOBAL.HSRLab.Status)
				{
					case HelperSRLab.StatusTypes.Connected:
						cmdSRLabConnect.Enabled = false;
						cmdSRLabDisconnect.Enabled = true;

						lblLBMStatus.ForeColor = Color.Green;
						lblLBMStatus.Text = "Connected";
						break;
					case HelperSRLab.StatusTypes.Connecting:
						cmdSRLabConnect.Enabled = false;
						cmdSRLabDisconnect.Enabled = false;

						lblLBMStatus.ForeColor = Color.Yellow;
						lblLBMStatus.Text = "Connecting...";
						break;
					case HelperSRLab.StatusTypes.DisConnecting:
						cmdSRLabConnect.Enabled = false;
						cmdSRLabDisconnect.Enabled = false;

						lblLBMStatus.ForeColor = Color.Yellow;
						lblLBMStatus.Text = "Disconnecting...";
						break;
					case HelperSRLab.StatusTypes.NotConnected:
					default:
						cmdSRLabConnect.Enabled = true;
						cmdSRLabDisconnect.Enabled = false;

						lblLBMStatus.ForeColor = Color.Red;
						lblLBMStatus.Text = "Not Connected";
						break;
				}
			}

			if (GLOBAL.HSRLab.StatusDsp != "" && GLOBAL.HSRLab.StatusDsp != lblLBMStatusMessage.Text)
			{
				lblLBMStatusMessage.Text = GLOBAL.HSRLab.StatusDsp;
			}
		}

		private void ProcessSRLabDataMsgRate()
		{
			if (GLOBAL.HSRLab.TotalDataMsgCountChanged)
			{
				GLOBAL.HSRLab.TotalDataMsgCountChanged = false;

				int ts = ((TimeSpan)DateTime.Now.Subtract(GLOBAL.HSRLab.FirstMsgTime)).Milliseconds;
				if (ts > 0 && GLOBAL.HSRLab.TotoalDataMsgCount > _lastDataMsgCount)
				{
					double rate = (GLOBAL.HSRLab.TotoalDataMsgCount - _lastDataMsgCount) / ts;
					_lastDataMsgCount = GLOBAL.HSRLab.TotoalDataMsgCount;
					if (rate > _lastPeakDataMsgRate)
					{
						lblLBMPeakMsgRate.Text = rate.ToString("F0");
						_lastPeakDataMsgRate = rate;
					}
					lblLBMLastMsgRate.Text = rate.ToString("F0");
				}

				lblLBMTotalMsgCount.Text = GLOBAL.HSRLab.TotoalDataMsgCount.ToString();
			}
		}

		private void cmdSRLabConnect_Click(object sender, EventArgs e)
		{
			_lastDataMsgCount = 0;

			GLOBAL.HSRLab.Status = HelperSRLab.StatusTypes.Connecting;
			GLOBAL.HSRLab.Start();
		}

		private void cmdSRLabDisconnect_Click(object sender, EventArgs e)
		{
			GLOBAL.HSRLab.Status = HelperSRLab.StatusTypes.DisConnecting;
			GLOBAL.HSRLab.Stop();
		}

		private void cmdForceGC_Click(object sender, EventArgs e)
		{
			GLOBAL.HSRLab.ForceGC();
		}

		private void cmdCheckEnvironments_Click(object sender, EventArgs e)
		{
			GLOBAL.HSRLab.Status = HelperSRLab.StatusTypes.EnvironmentVariableChecking;
			GLOBAL.HSRLab.CheckEnvironments();
		}

		#endregion

		#region - Server -

		private void cmdStart_Click(object sender, EventArgs e)
		{
			GLOBAL.HMDS.Connect();
		}

		private void cmdStop_Click(object sender, EventArgs e)
		{
			GLOBAL.HMDS.Disconnect();
		}

		private void ShowServerStatus()
		{
			ProcessServerStatus();
		}

		private void ProcessServerStatus()
		{
			if (GLOBAL.HMDS.StatusChanged)
			{
				GLOBAL.HMDS.StatusChanged = false;
				switch (GLOBAL.HMDS.Status)
				{
					case HelperMDS.StatusTypes.Stopped:
						cmdServerStop.Enabled = false;
						cmdServerStart.Enabled = true;

						lblMDSStatus.ForeColor = Color.Red;
						lblMDSStatus.Text = "Stopped";
						break;
					case HelperMDS.StatusTypes.Stopping:
						cmdServerStop.Enabled = false;
						cmdServerStart.Enabled = false;

						lblMDSStatus.ForeColor = Color.Yellow;
						lblMDSStatus.Text = "Stopping...";
						break;
					case HelperMDS.StatusTypes.Starting:
						cmdServerStop.Enabled = false;
						cmdServerStart.Enabled = false;

						lblMDSStatus.ForeColor = Color.Yellow;
						lblMDSStatus.Text = "Starting...";
						break;
					case HelperMDS.StatusTypes.Started:
						cmdServerStop.Enabled = true;
						cmdServerStart.Enabled = false;

						lblMDSStatus.ForeColor = Color.Green;
						lblMDSStatus.Text = "Started";
						break;
				}
			}

			if (GLOBAL.HMDS.StatusDsp != "" && GLOBAL.HMDS.StatusDsp != lblMDSStatusMessage.Text)
			{
				lblMDSStatusMessage.Text = GLOBAL.HMDS.StatusDsp;
			}

			if (GLOBAL.HMDS.LastReceivedMsg != "" && GLOBAL.HMDS.LastReceivedMsg != lblMDSRecivedMessage.Text)
			{
				lblMDSRecivedMessage.Text = GLOBAL.HMDS.LastReceivedMsg;
			}

			if (GLOBAL.HMDS.LastSentMsg != "" && GLOBAL.HMDS.LastSentMsg != lblMDSSentMessage.Text)
			{
				lblMDSSentMessage.Text = GLOBAL.HMDS.LastSentMsg;
			}
		}

		#endregion

		#region - Symbol List -

		public bool IsProcessingSymbol
		{
			get
			{
				return mdsWatchList.Processing;
			}
			set
			{
				mdsWatchList.Processing = value;
			}
		}

		private void cmdRefreshSymbol_Click(object sender, EventArgs e)
		{
			if (!IsProcessingSymbol)
			{
				System.Threading.ThreadPool.QueueUserWorkItem(LoadSymbolTable);
			}
		}

		private void txtSymbolToGet_KeyDown(object sender, KeyEventArgs e)
		{
			switch (e.KeyData)
			{
				case Keys.Enter:
					cmdRefreshSymbol_Click(null, EventArgs.Empty);
					break;
			}
		}

		private DataView _issueSymbolSearchView;
		private DataView issueSymbolSearchView
		{
			get
			{
				if (_issueSymbolSearchView == null)
				{
					_issueSymbolSearchView = new DataView(mdsWatchList.MdsGridTable);
					_issueSymbolSearchView.Sort = "IssueSymbol";
				}
				return _issueSymbolSearchView;
			}
		}

		private DataView _issueSymbolSourceSearchView;
		private DataView issueSymbolSourceSearchView
		{
			get
			{
				if (_issueSymbolSourceSearchView == null)
				{
					_issueSymbolSourceSearchView = new DataView(mdsWatchList.MdsGridTable);
					_issueSymbolSourceSearchView.Sort = "IssueSymbol, Source";
				}
				return _issueSymbolSourceSearchView;
			}
		}

		private delegate void LoadSymbolTableDelegate(object input);
		private void LoadSymbolTable(object input)
		{
			if (InvokeRequired)
			{
				BeginInvoke(new LoadSymbolTableDelegate(LoadSymbolTable), new object[] { input });
				return;
			}

			try
			{
				if (GLOBAL.HSRLab.Subscriptions.Count > 0)
				{
					txtSymbolToGet.Text = txtSymbolToGet.Text.ToUpper();

					Dictionary<string, SubscriptionFields> locSubs = HelperSymbolList.GetSubscriptions(txtSymbolToGet.Text);
					List<string> locCreatedSubs = HelperSymbolList.GetCreatedSubscriptions(txtSymbolToGet.Text);
					Dictionary<string, MDServerToClient> locDatas = HelperSymbolList.GetCurrentMarketData(txtSymbolToGet.Text);

					lock (mdsWatchList)
					{
						//mdsWatchList.MdsGridTable.BeginLoadData();
						//mdsWatchList.SuspendLayout();

						mdsWatchList.MdsGridTable.Clear();

						DataRowView[] rows;

						#region - Normal -

						rows = new DataRowView[0];
						foreach (SubscriptionFields locSub in locSubs.Values)
						{
							rows = issueSymbolSearchView.FindRows(locSub.Symbol);

							if (rows.Length == 0)
							{
								mdsWatchList.MdsGridTable.Rows.Add(AddUpdateSymbolRow(mdsWatchList.MdsGridTable.NewRow(), locSub));
							}
							else
							{
								foreach (DataRowView row in rows)
								{
									AddUpdateSymbolRow(row.Row, locSub);
								}
							}
						}

						string[] keys = new string[0];
						string symbol = "";
						string source = "";

						rows = new DataRowView[0];
						foreach (string mdKey in locCreatedSubs)
						{
							keys = mdKey.Split(_keySpliter);

							if (keys.Length == 2)
							{
								symbol = keys[0];
								source = keys[1];

								rows = issueSymbolSourceSearchView.FindRows(new object[] { symbol, source });

								if (rows.Length == 0)
								{
									mdsWatchList.MdsGridTable.Rows.Add(AddUpdateSymbolRow(mdsWatchList.MdsGridTable.NewRow(), symbol));
								}
								else
								{
									foreach (DataRowView row in rows)
									{
										AddUpdateSymbolRow(row.Row, symbol);
									}
								}
							}
						}

						#endregion

						rows = new DataRowView[0];
						foreach (string mdKey in locDatas.Keys)
						{
							keys = mdKey.Split(_keySpliter);

							if (keys.Length == 2)
							{
								symbol = keys[0];
								source = keys[1];

								rows = issueSymbolSourceSearchView.FindRows(new object[] { symbol, source });

								if (rows.Length == 0)
								{
									//mdsWatchList.MdsGridTable.Rows.Add(AddUpdateSymbolRow(mdsWatchList.MdsGridTable.NewRow(), locDatas[mdKey]));
								}
								else
								{
									foreach (DataRowView row in rows)
									{
										AddUpdateSymbolRow(row.Row, locDatas[mdKey]);
									}
								}
							}
						}

						//mdsWatchList.MdsGridTable.EndLoadData();
						//mdsWatchList.ResumeLayout();

						SetColumnMode(mdsWatchList, DataGridViewAutoSizeColumnMode.DisplayedCells);
						SetColumnMode(mdsWatchList, DataGridViewAutoSizeColumnMode.None);
					}
				}
			}
			catch (Exception ex)
			{
				GLOBAL.AddToExecption(ex);
			}
		}

		private DataRow AddUpdateSymbolRow(DataRow row, SubscriptionFields locSub)
		{
			row["Symbol"] = locSub.Symbol;
			row["IssueSymbol"] = locSub.Symbol;
			row["Source"] = locSub.Source;

			if (locSub.Type == "BOOK")
			{
				row["IsBook"] = 1;
			}
			else
			{
				row["IsBook"] = 0;
			}

			return row;
		}

		private DataRow AddUpdateSymbolRow(DataRow row, string symbol)
		{
			row["Symbol"] = symbol;
			row["Status"] = "Created";

			return row;
		}

		private DataRow AddUpdateSymbolRow(DataRow row, MDServerToClient data)
		{
			row["Symbol"] = data.uSymbol;
			row["IssueSymbol"] = data.IssueSymbol;

			row["BidPartID"] = data.BidPartId;

			if (data.BidPrice != null)
			{
				row["BidPrice"] = (double)data.BidPrice;
			}
			if (data.BidSize != null)
			{
				row["BidSize"] = (long)data.BidSize;
			}

			row["AskPartID"] = data.AskPartId;

			if (data.AskPrice != null)
			{
				row["AskPrice"] = (double)data.AskPrice;
			}
			if (data.AskSize != null)
			{
				row["AskSize"] = (long)data.AskSize;
			}

			row["TradePartID"] = data.TradePartId;
			if (data.TradePrice != null)
			{
				row["TradePrice"] = (double)data.TradePrice;
			}
			if (data.TradeVolume != null)
			{
				row["TradeVolume"] = (long)data.TradeVolume;
			}

			row["LineTime"] = data.LineTime;
			if (data.uDecodedTime != null)
			{
				row["DecodeTime"] = (double)data.uDecodedTime;
			}

			return row;
		}

		private void SetColumnMode(DataGridView grid, DataGridViewAutoSizeColumnMode mode)
		{
			int oldColWidth = 0;
			foreach (DataGridViewColumn col in grid.Columns)
			{
				switch (mode)
				{
					case DataGridViewAutoSizeColumnMode.NotSet:
					case DataGridViewAutoSizeColumnMode.None:
						oldColWidth = col.Width;
						col.AutoSizeMode = mode;
						col.Width = oldColWidth;
						break;
					default:
						col.AutoSizeMode = mode;
						break;
				}
			}
		}

		#endregion

		#region - Symbol List Event -

		private void mdsWatchList_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			switch (e.PropertyName)
			{
				case "ResubscribeIssueSymbol":
					if (mdsWatchList.ResubscribeBook == 1)
					{
						GLOBAL.HSRLab.Subscribe(new MDClientToServer("S", mdsWatchList.ResubscribeBookSymbol, mdsWatchList.ResubscribeSource, "BOOK", CSVEx.CSVFieldIDs.SecutrityTypes.Future, true));
					}
					else
					{
						GLOBAL.HSRLab.Subscribe(new MDClientToServer("S", mdsWatchList.ResubscribeIssueSymbol, mdsWatchList.ResubscribeSource, true));
					}
					break;
			}
		}

		#endregion

		#region - User List -

		public bool IsProcessingUser
		{
			get
			{
				return mdsUserWatchList.Processing;
			}
			set
			{
				mdsUserWatchList.Processing = value;
			}
		}

		private DataView _usernameSearchView;
		private DataView usernameSearchView
		{
			get
			{
				if (_usernameSearchView == null)
				{
					_usernameSearchView = new DataView(mdsUserWatchList.MdsGridTable);
					_usernameSearchView.Sort = "ClientID";
				}
				return _usernameSearchView;
			}
		}

		private void cmdRefreshUser_Click(object sender, EventArgs e)
		{
			if (!IsProcessingUser)
			{
				IsProcessingUser = true;
				System.Threading.ThreadPool.QueueUserWorkItem(LoadUserTable);
			}
		}

		private delegate void LoadUserTableDelegate(object input);
		private void LoadUserTable(object input)
		{
			if (InvokeRequired)
			{
				BeginInvoke(new LoadUserTableDelegate(LoadUserTable), new object[] { input });
				return;
			}

			try
			{
				lock (mdsUserWatchList.MdsGridTable)
				{
					mdsUserWatchList.MdsGridTable.Rows.Clear();
				}

				if (GLOBAL.HMDS.ClientToUserMap.Count > 0)
				{
					Dictionary<IntPtr, MDClientToServer> locClientToUserMap = HelperUserList.GetClientToUserMap();

					lock (mdsUserWatchList.MdsGridTable)
					{
						//mdsUserWatchList.MdsGridTable.BeginLoadData();
						//mdsUserWatchList.SuspendLayout();

						//mdsUserWatchList.MdsGridTable.Rows.Clear();

						DataRowView[] rows;

						rows = new DataRowView[0];
						foreach (IntPtr locClient in locClientToUserMap.Keys)
						{
							rows = usernameSearchView.FindRows(locClient.ToString());
							string ip = HelperUserList.GetClientIP(locClient);

							if (rows.Length == 0)
							{
								mdsUserWatchList.MdsGridTable.Rows.Add(AddUpdateUserRow(mdsUserWatchList.MdsGridTable.NewRow(), locClientToUserMap[locClient], locClient, ip));
							}
							else
							{
								foreach (DataRowView row in rows)
								{
									AddUpdateUserRow(row.Row, locClientToUserMap[locClient], locClient, ip);
								}
							}
						}

						//mdsUserWatchList.MdsGridTable.EndLoadData();
						//mdsUserWatchList.ResumeLayout();

						SetColumnMode(mdsUserWatchList, DataGridViewAutoSizeColumnMode.DisplayedCells);
						SetColumnMode(mdsUserWatchList, DataGridViewAutoSizeColumnMode.None);
					}
				}

				IsProcessingUser = false;
			}
			catch (Exception ex)
			{
				GLOBAL.AddToExecption(ex);
			}
		}

		private DataRow AddUpdateUserRow(DataRow row, MDClientToServer data, IntPtr id, string ip)
		{
			row["ClientID"] = id.ToString();
			row["ClientIP"] = ip;
			row["Username"] = data.UserName;
			row["Password"] = data.Password;

			if (GLOBAL.HMDS.ClientHartbeatMap.ContainsKey(id))
			{
				row["Datetime"] = GLOBAL.HMDS.ClientHartbeatMap[id].ToString("hh:mm:ss");
			}
			else
			{
				row["Datetime"] = "";
			}

			if (GLOBAL.HMDS.ClientToSymbolMap.ContainsKey(id))
			{
				row["SymbolCount"] = GLOBAL.HMDS.ClientToSymbolMap[id].Count;
			}
			else
			{
				row["SymbolCount"] = 0;
			}

			return row;
		}

		#endregion

		#region - Load Subscription From XML -

		private void cmdSRLabMDS_Click(object sender, EventArgs e)
		{
			//GLOBAL.HSRLab.Subscribe(new MDClientToServer("S", "F", "CTA", false));
			//GLOBAL.HSRLab.Subscribe(new MDClientToServer("S", "F.A", "CTA", false));
			//GLOBAL.HSRLab.Subscribe(new MDClientToServer("S", "F.B", "CTA", false));
			//GLOBAL.HSRLab.Subscribe(new MDClientToServer("S", "F.C", "CTA", false));
			//GLOBAL.HSRLab.Subscribe(new MDClientToServer("S", "F.D", "CTA", false));
			//GLOBAL.HSRLab.Subscribe(new MDClientToServer("S", "F.I", "CTA", false));
			//GLOBAL.HSRLab.Subscribe(new MDClientToServer("S", "F.M", "CTA", false));
			//GLOBAL.HSRLab.Subscribe(new MDClientToServer("S", "F.N", "CTA", false));
			//GLOBAL.HSRLab.Subscribe(new MDClientToServer("S", "F.P", "CTA", false));
			//GLOBAL.HSRLab.Subscribe(new MDClientToServer("S", "F.T", "CTA", false));
			//GLOBAL.HSRLab.Subscribe(new MDClientToServer("S", "F.W", "CTA", false));
			//GLOBAL.HSRLab.Subscribe(new MDClientToServer("S", "F.X", "CTA", false));
			//GLOBAL.HSRLab.Subscribe(new MDClientToServer("S", "F.Z", "CTA", false));
			//GLOBAL.HSRLab.Subscribe(new MDClientToServer("S", "INTC", "NASDAQ", false));
			//GLOBAL.HSRLab.Subscribe(new MDClientToServer("S", "ZBM0", "CME-CBOT", "", false));
			//GLOBAL.HSRLab.Subscribe(new MDClientToServer("S", "ESM0", "CME", "", false));
			//GLOBAL.HSRLab.Subscribe(new MDClientToServer("S", "RTAH0", "ICE", "", false));
			//GLOBAL.HSRLab.Subscribe(new MDClientToServer("S", "YMH0", "CME", "", false));
			//GLOBAL.HSRLab.Subscribe(new MDClientToServer("S", "bESH0", "CME", "BOOK", "F", false));

			//GLOBAL.HSRLab.Subscribe(new MDClientToServer("S", "INTC.A", "NASDAQ", false));
			//GLOBAL.HSRLab.Subscribe(new MDClientToServer("S", "INTC.B", "NASDAQ", false));
			//GLOBAL.HSRLab.Subscribe(new MDClientToServer("S", "IBM.T", "CTA", false));
			//GLOBAL.HSRLab.Subscribe(new MDClientToServer("S", "ESH0", "CME", "", false));
			//GLOBAL.HSRLab.Subscribe(new MDClientToServer("S", "bESU9", "CME", "BOOK", "F", false));
			//GLOBAL.HSRLab.Subscribe(new MDClientToServer("S", "b6JU9", "CME", "BOOK", "F", false));
			//GLOBAL.HSRLab.Subscribe(new MDClientToServer("S", "6JU9", "CME", "", "F", false));
			//GLOBAL.HSRLab.Subscribe(new MDClientToServer("S", "SPX.IDX", "OPRANBBO", "", "I", false));
			//GLOBAL.HSRLab.Subscribe(new MDClientToServer("S", "VIX.IDX", "OPRANBBO", "", "I", false));

			HelperSubscriptionCache.GetSubscriptionCache();
			foreach (DataRow row in HelperSubscriptionCache.CacheTable.Rows)
			{
				SubscribeByDataRow(row);
			}
		}

		private void SubscribeByDataRow(DataRow row)
		{
			try
			{
				switch (row["MsgType"].ToString().ToUpper())
				{
					case "S":
						switch (row["Reload"].ToString().ToUpper())
						{
							case "TRUE":
							case "T":
							case "1":
								GLOBAL.HSRLab.Subscribe(
									new MDClientToServer("S",
									row["Symbol"].ToString(),
									row["Source"].ToString().ToUpper(),
									row["SubscriptionType"].ToString().ToUpper(),
									row["SecType"].ToString().ToUpper(), true));
								break;
							default:
								GLOBAL.HSRLab.Subscribe(
									new MDClientToServer("S",
									row["Symbol"].ToString(),
									row["Source"].ToString().ToUpper(),
									row["SubscriptionType"].ToString().ToUpper(),
									row["SecType"].ToString().ToUpper(), false));
								break;
						}

						//Debug.Print(row["Symbol"].ToString());
						break;
				}
			}
			catch (Exception ex)
			{
				GLOBAL.AddToExecption(ex);
			}
		}

		#endregion

		#region - About -

		private void tabControlMain_SelectedIndexChanged(object sender, EventArgs e)
		{
			switch (tabControlMain.SelectedTab.Name)
			{
				case "tabPageAbout":
					rtbAbout.Text = "";
					rtbAbout.SelectedText = HelperAbout.GetVersion();
					rtbAbout.SelectedText = HelperAbout.GetSRLabSetting();
					rtbAbout.SelectedText = HelperAbout.GetMDSSetting();
					break;
			}
		}

		#endregion
	}
}