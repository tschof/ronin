using System;
using System.Drawing;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Windows.Forms;

using FormEx;
using LabelEx;
using SerializationEx;
using ContextMenuEx;
using ButtonEx;
using RDSEx;
using MarketDataEx;
using System.Diagnostics;
using System.Runtime.InteropServices;

namespace ROC
{
	[System.ComponentModel.DesignerCategory("Form")]
	public partial class frmMain : VistaWindowBorderless
	{
		private sealed class CaptionStateIcons
		{
			public const int NORMAL = 0;
			public const int DISABLED = 1;
			public const int PRESSED = 2;
			public const int HOT = 3;
		}

		#region - Properties -

		public bool InSystemTray
		{
			get
			{
				return notifyIconROC.Visible;
			}
			set
			{
				notifyIconROC.Visible = value;
			}
		}

		private Rectangle _lastRectangle = new Rectangle();
		public Rectangle FormRectangle
		{
			get
			{
				if (_lastRectangle.Height != Size.Height || 
					_lastRectangle.Width != Size.Width ||
					_lastRectangle.X != Location.X ||
					_lastRectangle.Y != Location.Y)
				{
					return _lastRectangle = new Rectangle(Location, Size);
				}

				return _lastRectangle;
			}
		}

		public bool OkToMinimize
		{
			get
			{
				if (Top == 0 && 
					_menuFileActions == null &&
					_menuROC == null &&
					_menuStandardTickets == null &&
					_menuCustomTickets == null &&
					cmdWindows.ContextMenuStrip == null && 
					Height == MaximumSize.Height && 
					!FormRectangle.Contains(Cursor.Position))
				{
					return true;
				}
				return false;
			}
		}

		#endregion

		#region - Constructor -

		public frmMain()
		{
			InitializeComponent();
			
			Initialize();
		}

		private void Initialize()
		{
			if (Configuration.ROM.Default.IP != "ROMAPI")
			{
				lblCaption.BackColor = Color.LimeGreen;
				lblCaptionEnd.BackColor = Color.LimeGreen;
				lblCaptionEnd.Text = GLOBAL.HROM.UserName;
			}

			Caption = Configuration.User.Default.InstanceName + ": " + GLOBAL.HROM.UserName;
			Text = Caption;

			notifyIconROC.Text = Configuration.User.Default.InstanceName;
			notifyIconROC.BalloonTipTitle = String.Concat(new object[] { Configuration.User.Default.InstanceName, " UI Minimized" });
			notifyIconROC.BalloonTipText = String.Concat(new object[] { "Double Click to Reopen ", Configuration.User.Default.InstanceName, " UI" });

			DockWindow.RegisterExternalReferenceForm(this);

			//GLOBAL.HProcess.StartProcess();

			UITimerTicked += new EventHandler(Form_UITimerTicked);
			StartTimer(Configuration.User.Default.UIUpdateRate);

			StartMemoryTimer();
		}

		#endregion

		#region - Memory Flash Timer -

		private Timer _memTimer;

		private void StartMemoryTimer()
		{
			if (_memTimer == null)
			{
				_memTimer = new Timer();
				_memTimer.Interval = Configuration.ROC.Default.MemoryFlushInterval * 1000 * 60;
				_memTimer.Tick += new EventHandler(MemTimer_Tick);
				_memTimer.Enabled = true;
			}
		}

		private void MemTimer_Tick(object sender, EventArgs e)
		{
			HelperMemory.Flush();
		}

		private void StopMemoryTimer()
		{
			if (_memTimer != null)
			{
				_memTimer.Enabled = false;
				_memTimer.Tick -= new EventHandler(MemTimer_Tick);
			}
			_memTimer = null;
		}

		#endregion

		#region - Wait Window -

		private void ShowWaitWindow()
		{
			ShowWaitWindow("Loading...");
		}
		private void ShowWaitWindow(string msg)
		{
			if (GLOBAL.WaitWindow.Message != msg)
			{
				GLOBAL.WaitWindow.Message = msg;
			}

			if (!GLOBAL.WaitWindow.Visible)
			{
				GLOBAL.WaitWindow.Show();
				GLOBAL.WaitWindow.BringToFront();
			}
		}

		private void HideWaitWindow()
		{
			GLOBAL.WaitWindow.Hide();
		}

		#endregion

		#region - Form UI Timer Event -

		private HelperROM.StatusTypes _lastROMStatus = HelperROM.StatusTypes.None;
		private Dictionary<string, HelperMDS.StatusTypes> _lastMDSStatuss = new Dictionary<string, HelperMDS.StatusTypes>();

		private void Form_UITimerTicked(object sender, EventArgs e)
		{
			#region - Application Stop Time -

			AutoCancelTimeCheck();

			StopTimeCheck();

			#endregion

			#region - ROM -

			GLOBAL.HROM.LogStatusMessages();
			GLOBAL.HROM.LogRomMessages();

			if (GLOBAL.HROM.EndOfQueuedMsg)
			{
				HideWaitWindow();

				if (GLOBAL.ROCLoginCompleted)
				{
					if (_lastROMStatus != GLOBAL.HROM.Status)
					{
						_lastROMStatus = GLOBAL.HROM.Status;
						if (_lastROMStatus == HelperROM.StatusTypes.LoggedIn && !GLOBAL.ByPassRomLogin)
						{
							GLOBAL.HROM.StartHartbeat();
						}
					}

					if (GLOBAL.HROM.Status == HelperROM.StatusTypes.LoggedIn && !GLOBAL.ByPassRomLogin)
					{
						//GLOBAL.HROM.HartbeatTimeOutCheck();
					}
				}
			}
			else
			{
				if (GLOBAL.HROM.Status == HelperROM.StatusTypes.LoggedIn)
				{
					ShowWaitWindow(String.Concat(new object[] { "Processing ROM Messages... ", GLOBAL.HROM.EndOfQueuedMsgCount.ToString() }));
				}
			}

			if (GLOBAL.HROM.AlertList.Count > 0)
			{
				GLOBAL.HROM.ShowAlerts();
			}

			#endregion

			#region - MDS -

			foreach (HelperMDS mds in GLOBAL.HMDSs)
			{
				if (GLOBAL.ROCLoginCompleted)
				{
					if (_lastMDSStatuss.ContainsKey(mds.ServerIP))
					{
						if (_lastMDSStatuss[mds.ServerIP] != mds.Status)
						{
							_lastMDSStatuss[mds.ServerIP] = mds.Status;
						}
					}
					else
					{
						_lastMDSStatuss.Add(mds.ServerIP, mds.Status);
					}

					if (_lastMDSStatuss[mds.ServerIP] == HelperMDS.StatusTypes.Started)
					{
						mds.StartHartbeat();
					}
				}

				if (GLOBAL.MDSsConnected)
				{
					//mds.HartbeatTimeOutCheck();
				}

				mds.LogStatusMessages();

				if (mds.AlertList.Count > 0)
				{
					mds.ShowAlerts();
				}
			}

			#endregion

			#region - RDS -

			GLOBAL.HRDS.LogStatusMessages();

			if (GLOBAL.HRDS.AlertList.Count > 0)
			{
				GLOBAL.HRDS.ShowAlerts();
			}

			#endregion

			#region - ROC -

			GLOBAL.HROC.LogStatusMessages();

			if (GLOBAL.HROC.AlertList.Count > 0)
			{
				GLOBAL.HROC.ShowAlerts();
			}

			#endregion

			#region - From Visiblity -

			if (InSystemTray)
			{
				if (!notifyIconROC.Visible)
				{
					notifyIconROC.Visible = true;
				}
				if (Visible)
				{
					Visible = false;
				}
			}
			else
			{
				if (!Visible)
				{
					Visible = true;
				}
				if (notifyIconROC.Visible)
				{
					if (notifyIconROC.ContextMenuStrip != null)
					{
						notifyIconROC.ContextMenuStrip.Visible = false;
						notifyIconROC.ContextMenuStrip = null;
					}
					notifyIconROC.Visible = false;
				}

				if (!TopLevel)
				{
					Show();
					BringToFront();
				}
			}

			#endregion

			#region - ROC Connection Status -

			SetROCState();

			#endregion

			#region - From Auto Hide Check -

			FormCollapse();

			#endregion
		}

		private void SetROCState()
		{
			if (_menuROC != null)
			{
				_menuROC.SetStatus();
			}

			bool romInError = false;
			bool rdsInError = false;
			bool mdsInError = false;

			if (GLOBAL.HROM.Status != HelperROM.StatusTypes.LoggedIn)
			{
				romInError = true;
			}

			if (GLOBAL.HRDS.Status == HelperRDS.StatusTypes.GotError ||
				GLOBAL.HRDS.Status == HelperRDS.StatusTypes.Failed)
			{
				rdsInError = true;
			}

			if (Configuration.User.Default.UseMarketData && !GLOBAL.MDSsConnected)
			{
				mdsInError = true;
			}

			if (romInError || rdsInError || mdsInError)
			{
				if (cmdROC.BackColor != Color.DarkRed)
				{
					cmdROC.BackColor = Color.DarkRed;
				}
			}
			else
			{
				if (cmdROC.BackColor != Color.Black)
				{
					cmdROC.BackColor = Color.Black;
				}
			}
		}

		private void Form_MouseMove(object sender, MouseEventArgs e)
		{
			FormExpand();
		}

		private void FormExpand()
		{
			Height = MaximumSize.Height;
			BackColor = Color.FromKnownColor(KnownColor.Control);
			Padding = new Padding(0);
		}

		private void FormCollapse()
		{
			if (OkToMinimize)
			{
				Height = MinimumSize.Height;
				BackColor = Color.FromKnownColor(KnownColor.DarkOliveGreen);
				Padding = new Padding(0, 3, 0, 0);
			}
		}

		#endregion

		#region - Stop Timer -

		private HelperAutomationTimer _autoStopTimer;

		private void StopTimeCheck()
		{
			if (Configuration.User.Default.UseStopTime)
			{
				if (_autoStopTimer == null)
				{
					_autoStopTimer = new HelperAutomationTimer(AutomationType.AutoStop);
					_autoStopTimer.PropertyChanged += new PropertyChangedEventHandler(AutoTimer_PropertyChanged);
				}
				_autoStopTimer.CheckAutoTimer(GLOBAL.StopTime);
			}
		}

		#region - Not Used -

		//private void StopTimeCheck1()
		//{
		//    if (Configuration.User.Default.UseStopTime)
		//    {
		//        TimeSpan ts = DateTime.Now.Subtract(GLOBAL.StopTime);
		//        if (ts.TotalSeconds >= 0 && ts.TotalSeconds <= 60)
		//        {
		//            // Only do stop when its with in 60 sec of the stop time
		//            AutoExitApplication();
		//        }
		//    }
		//}

		#endregion

		#endregion

		#region - Auto Cancel Timer -

		private HelperAutomationTimer _stockAutoCancelTimer = null;
		private HelperAutomationTimer _futureAutoCancelTimer = null;
		private HelperAutomationTimer _optionAutoCancelTimer = null;

		private void AutoCancelTimeCheck()
		{
			if (Configuration.User.Default.UseStockAutoCancel)
			{
				if (_stockAutoCancelTimer == null)
				{
					_stockAutoCancelTimer = new HelperAutomationTimer(AutomationType.AutoCancelStock);
					_stockAutoCancelTimer.PropertyChanged += new PropertyChangedEventHandler(AutoTimer_PropertyChanged);
				}
				_stockAutoCancelTimer.CheckAutoTimer(GLOBAL.StockAutoCancelTime);
			}
			if (Configuration.User.Default.UseFutureAutoCancel)
			{
				if (_futureAutoCancelTimer == null)
				{
					_futureAutoCancelTimer = new HelperAutomationTimer(AutomationType.AutoCancelFuture);
					_futureAutoCancelTimer.PropertyChanged += new PropertyChangedEventHandler(AutoTimer_PropertyChanged);
				}
				_futureAutoCancelTimer.CheckAutoTimer(GLOBAL.FutureAutoCancelTime);
			}
			if (Configuration.User.Default.UseOptionAutoCancel)
			{
				if (_optionAutoCancelTimer == null)
				{
					_optionAutoCancelTimer = new HelperAutomationTimer(AutomationType.AutoCancelOption);
					_optionAutoCancelTimer.PropertyChanged += new PropertyChangedEventHandler(AutoTimer_PropertyChanged);
				}
				_optionAutoCancelTimer.CheckAutoTimer(GLOBAL.OptionAutoCancelTime);
			}
		}

		#region - Not Used -

		//private bool _autoStockCancelSent = false;
		//private bool _autoFutureCancelSent = false;
		//private bool _autoOptionCancelSent = false;
		//private int _autoCancelWindow = 60;
		//private void AutoCancelTimeCheck1()
		//{
		//    try
		//    {
		//        #region - Stock -

		//        if (Configuration.User.Default.UseStockAutoCancel)
		//        {
		//            TimeSpan ts = DateTime.Now.Subtract(GLOBAL.StockAutoCancelTime);
		//            if (ts.TotalSeconds >= 0 && ts.TotalSeconds <= _autoCancelWindow)
		//            {
		//                if (!_autoStockCancelSent)
		//                {
		//                    _autoStockCancelSent = true;
		//                    // Only do cancel when its with in 15 sec of the cancel time
		//                    GLOBAL.HROM.CancelAllOpenOrders(CSVEx.CSVFieldIDs.SecutrityTypes.Equity);
		//                }
		//            }

		//            if (_autoStockCancelSent && (ts.TotalSeconds < 0 || ts.TotalSeconds > _autoCancelWindow))
		//            {
		//                // reset for the next auto cancel check
		//                _autoStockCancelSent = false;
		//            }
		//        }

		//        #endregion

		//        #region - Future -

		//        if (Configuration.User.Default.UseFutureAutoCancel)
		//        {
		//            TimeSpan ts = DateTime.Now.Subtract(GLOBAL.FutureAutoCancelTime);
		//            if (ts.TotalSeconds >= 0 && ts.TotalSeconds <= _autoCancelWindow)
		//            {
		//                if (!_autoFutureCancelSent)
		//                {
		//                    _autoFutureCancelSent = true;
		//                    // Only do cancel when its with in 15 sec of the cancel time
		//                    GLOBAL.HROM.CancelAllOpenOrders(CSVEx.CSVFieldIDs.SecutrityTypes.Future);
		//                }
		//            }

		//            if (_autoFutureCancelSent && (ts.TotalSeconds < 0 || ts.TotalSeconds > _autoCancelWindow))
		//            {
		//                // reset for the next auto cancel check
		//                _autoFutureCancelSent = false;
		//            }
		//        }

		//        #endregion

		//        #region - Option -

		//        if (Configuration.User.Default.UseOptionAutoCancel)
		//        {
		//            TimeSpan ts = DateTime.Now.Subtract(GLOBAL.OptionAutoCancelTime);
		//            if (ts.TotalSeconds >= 0 && ts.TotalSeconds <= _autoCancelWindow)
		//            {
		//                if (!_autoOptionCancelSent)
		//                {
		//                    _autoOptionCancelSent = true;
		//                    // Only do cancel when its with in 15 sec of the cancel time
		//                    GLOBAL.HROM.CancelAllOpenOrders(CSVEx.CSVFieldIDs.SecutrityTypes.Option);
		//                }
		//            }

		//            if (_autoOptionCancelSent && (ts.TotalSeconds < 0 || ts.TotalSeconds > _autoCancelWindow))
		//            {
		//                // reset for the next auto cancel check
		//                _autoOptionCancelSent = false;
		//            }
		//        }

		//        #endregion
		//    }
		//    catch (Exception ex)
		//    {
		//        GLOBAL.HROC.AddToStatusLogs(ex.Message);
		//        GLOBAL.HROC.AddToStatusLogs(ex.StackTrace);
		//    }
		//}

		#endregion

		#endregion

		#region - Automation Events -

		private void AutoTimer_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			switch (e.PropertyName)
			{
				case AutomationType.AutoCancelStock:
					GLOBAL.HROM.CancelAllOpenOrders(CSVEx.CSVFieldIDs.SecutrityTypes.Equity);
					break;
				case AutomationType.AutoCancelFuture:
					GLOBAL.HROM.CancelAllOpenOrders(CSVEx.CSVFieldIDs.SecutrityTypes.Future);
					break;
				case AutomationType.AutoCancelOption:
					GLOBAL.HROM.CancelAllOpenOrders(CSVEx.CSVFieldIDs.SecutrityTypes.Option);
					break;
				case AutomationType.AutoStop:
					AutoExitApplication();
					break;
			}
		}

		#endregion

		#region - Closing Event -

		protected override void OnClosing(CancelEventArgs e)
		{
			StopTimer();

			DockWindow.UnregisterExternalReferenceForm(this);			
			base.OnClosing(e);
		}

		#endregion

		#region - Form Button Event - 

		private void Button_MouseHover(object sender, EventArgs e)
		{
			Button_Action(sender, CaptionStateIcons.HOT);

			ToolBarActive_MouseEnter(null, EventArgs.Empty);
		}

		private void Button_MouseDown(object sender, MouseEventArgs e)
		{
			Button_Action(sender, CaptionStateIcons.PRESSED);
		}

		private void Button_MouseUp(object sender, MouseEventArgs e)
		{
			Button_Action(sender, CaptionStateIcons.HOT);
		}

		private void Button_MouseEnter(object sender, EventArgs e)
		{
			Button_Action(sender, CaptionStateIcons.HOT);
		}

		private void Button_MouseLeave(object sender, EventArgs e)
		{
			Button_Action(sender, CaptionStateIcons.NORMAL);
		}

		private void Button_MouseMove(object sender, MouseEventArgs e)
		{
			Button_Action(sender, CaptionStateIcons.HOT);
		}

		private void Button_Action(object sender, int state)
		{
			GLOBAL.Interrupt = true;

			VistaButton button = (VistaButton)sender;
			if (button.ImageList != null && button.ImageList.Images.Count > state)
			{
				button.ImageIndex = state;
			}

			switch (state)
			{
				case CaptionStateIcons.PRESSED:
					switch (button.Name)
					{
						case "cmdClose":
							StopMemoryTimer();
							ExitApplication();
							break;
						case "cmdMinimize":
							InSystemTray = true;
							HelperMemory.Flush();
							break;
						default:
							break;
					}
					break;
				default:
					break;
			}
		}

		#endregion

		#region - Notification Tray Event -

		// Fixed the issue when the form shows up in the task bar when ContextMenuStip is shown
		[DllImport("User32.dll", ExactSpelling = true, CharSet = CharSet.Auto)]
		public static extern bool SetForegroundWindow(HandleRef hWnd);

		private void notifyIconROC_ShowMenu(object sender, MouseEventArgs e)
		{
			try
			{
				if (notifyIconROC.ContextMenuStrip == null)
				{
					notifyIconROC.ContextMenuStrip = GLOBAL.HWindows.NotificationMenu;
					Point p = Cursor.Position;
					p.Offset(-25, -15);
					notifyIconROC.ContextMenuStrip.Closed += new ToolStripDropDownClosedEventHandler(notifyMenu_Closed);
					notifyIconROC.ContextMenuStrip.Show(p, ToolStripDropDownDirection.AboveRight);
				}
				else
				{
					try
					{
						((VistaContextMenu)notifyIconROC.ContextMenuStrip).ShowAgain();
						SetForegroundWindow(new HandleRef(this, this.Handle));
					}
					catch (Exception ex)
					{
						// TODO There is a pommision issue
						GLOBAL.HROC.AddToStatusLogs(ex.Message);
						GLOBAL.HROC.AddToStatusLogs(ex.StackTrace);
					}
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		private void notifyIconROC_MouseDoubleClick(object sender, MouseEventArgs e)
		{
			InSystemTray = false;
		}

		private void notifyMenu_Closed(object sender, ToolStripDropDownClosedEventArgs e)
		{
			if (notifyIconROC.ContextMenuStrip != null)
			{
				notifyIconROC.ContextMenuStrip.Closed -= new ToolStripDropDownClosedEventHandler(notifyMenu_Closed);
				notifyIconROC.ContextMenuStrip = null;
			}
		}

		#endregion

		#region - ToolBar Event -

		private void ToolBarActive_MouseEnter(object sender, EventArgs e)
		{
			Activate();
		}

		private void ToolBarMove_MouseDown(object sender, MouseEventArgs e)
		{
			base.MoveByCaptionLable(sender, e);
		}

		#endregion
		
		#region - ROC Menu -

		private menuMain _menuROC;
		private PropertyContainer _menuROCContainer;
		private void cmdROC_Click(object sender, EventArgs e)
		{
			if (_menuROC == null)
			{
				_menuROC = new menuMain();
				_menuROC.PropertyChanged += new PropertyChangedEventHandler(menuROC_PropertyChanged);
			}

			if (_menuROCContainer == null)
			{
				_menuROCContainer = new PropertyContainer(_menuROC);
				_menuROCContainer.Closed += new ToolStripDropDownClosedEventHandler(menuROCContainer_Closed);
			}

			_menuROCContainer.Show((VistaButton)sender);
		}

		private void menuROC_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			_menuROCContainer.Close();
			switch (e.PropertyName)
			{
				case "ROM":
					if (MessageBox.Show("Reconnect To ROM?", "Confirmation", MessageBoxButtons.YesNo) == DialogResult.Yes)
					{
						GLOBAL.HROM.Reconnect();
					}
					break;
				case "RDS":
					if (MessageBox.Show("Reconnect To RDS With Last Saved Profile?", "Confirmation", MessageBoxButtons.YesNo) == DialogResult.Yes)
					{
						GLOBAL.UserUIProfile.Dispose();
						GLOBAL.UserUIProfile.ReLoad();

						// Reset the RDS Status
						GLOBAL.HRDS.Status = HelperRDS.StatusTypes.Done;
					}
					break;
				case "MDS":
					if (MessageBox.Show("Reconnect To Market Data?", "Confirmation", MessageBoxButtons.YesNo) == DialogResult.Yes)
					{
						GLOBAL.MDSsReconnect();
					}
					break;
				case "UserProfile":
					GLOBAL.HWindows.OpenWindow(new frmUserProfile());
					break;
				case "IM":
					break;
				case "Alerts":
					GLOBAL.HWindows.OpenWindow(new frmAlert());
					break;
				case "Clock":
					GLOBAL.HWindows.OpenWindow(new frmClock());
					break;
				case "Settings":
					GLOBAL.HWindows.OpenWindow(new frmSettings());
					break;
				case "ZeroAll":
					GLOBAL.HWindows.ZeroAllWindowPosition();
					break;
				case "About":
					GLOBAL.HWindows.OpenWindow(new frmAbout());
					break;
			}
		}

		private void menuROCContainer_Closed(object sender, ToolStripDropDownClosedEventArgs e)
		{
			_menuROCContainer.Closed -= new ToolStripDropDownClosedEventHandler(menuROCContainer_Closed);
			_menuROCContainer = null;
			_menuROC.PropertyChanged -= new PropertyChangedEventHandler(menuROC_PropertyChanged);
			_menuROC = null;
		}

		#endregion

		#region - File Menu -

		private menuBaseAction _menuFileActions;
		private PropertyContainer _menuFileActionsContainer;
		private void cmdFiles_Click(object sender, EventArgs e)
		{
			if (_menuFileActions == null)
			{
				Dictionary<string, string> items = new Dictionary<string, string>();
				items.Add("Save", "Save");
				items.Add("SaveAs", "SaveAs");
				items.Add("Load", "Load");
				items.Add("LoadFrom", "LoadFrom");
				items.Add("EmailLog", "EmailLog");

				_menuFileActions = new menuBaseAction(items, true);
				_menuFileActions.PropertyChanged += new PropertyChangedEventHandler(menuFileActions_PropertyChanged);
			}

			if (_menuFileActionsContainer == null)
			{
				_menuFileActionsContainer = new PropertyContainer(_menuFileActions);
				_menuFileActionsContainer.Closed += new ToolStripDropDownClosedEventHandler(menuFileActionsContainer_Closed);
			}

			_menuFileActionsContainer.Show((VistaButton)sender);
		}

		private void menuFileActions_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			_menuFileActionsContainer.Close();
			switch (e.PropertyName)
			{
				case "Save":
					GLOBAL.UserUIProfile.Save(false);
					break;
				case "SaveAs":
					SaveAs();
					break;
				case "Load":
					if (MessageBox.Show("Override Current Layout?", "Confirmation", MessageBoxButtons.YesNo) == DialogResult.Yes)
					{
						GLOBAL.UserUIProfile.ReLoad();
					}
					break;
				case "LoadFrom":
					LoadFrom();
					break;
				case "EmailLog":
					GLOBAL.HWindows.OpenWindow(new frmEMail());
					break;
			}
		}

		private void menuFileActionsContainer_Closed(object sender, ToolStripDropDownClosedEventArgs e)
		{
			_menuFileActionsContainer.Closed -= new ToolStripDropDownClosedEventHandler(menuFileActionsContainer_Closed);
			_menuFileActionsContainer = null;
			_menuFileActions.PropertyChanged -= new PropertyChangedEventHandler(menuFileActions_PropertyChanged);
			_menuFileActions = null;
		}

		private void SaveAs()
		{
			SaveFileDialog sfd = new SaveFileDialog();
			sfd.DefaultExt = "profile";
			sfd.Filter = "Profile File (*.profile)|*.profile";
			sfd.AddExtension = true;
			sfd.RestoreDirectory = true;
			sfd.Title = "Save Current Profile As";
			sfd.InitialDirectory = Configuration.Path.Default.ProfilePath;

			if (sfd.ShowDialog() == DialogResult.OK)
			{
				GLOBAL.UserUIProfile.Save(System.IO.Path.GetDirectoryName(sfd.FileName) + @"\", System.IO.Path.GetFileName(sfd.FileName), false);
			}
		}

		private void LoadFrom()
		{
			OpenFileDialog ofd = new OpenFileDialog();
			ofd.DefaultExt = "profile";
			ofd.Filter = "Profile File (*.profile)|*.profile";
			ofd.AddExtension = true;
			ofd.RestoreDirectory = true;
			ofd.Title = "Open Profile From";
			ofd.InitialDirectory = Configuration.Path.Default.ProfilePath;

			if (ofd.ShowDialog() == DialogResult.OK)
			{
				GLOBAL.UserUIProfile.ReLoad(System.IO.Path.GetDirectoryName(ofd.FileName) + @"\", System.IO.Path.GetFileName(ofd.FileName));
			}
		}

		#endregion

		#region - Lists Button Click -

		private void cmdWatchList_Click(object sender, EventArgs e)
		{
			lock (GLOBAL.HWindows.WatchListWindows)
			{
				GLOBAL.HWindows.OpenWindow(new frmWatchList(), true);
			}
		}

		private void cmdPlotList_Click(object sender, EventArgs e)
		{
			lock (GLOBAL.HWindows.PlotListWindows)
			{
				GLOBAL.HWindows.OpenWindow(new frmPlotList(), true);
			}
		}

		#endregion

		#region - Standard Tickets Menu -

		private menuBaseAction _menuStandardTickets;
		private PropertyContainer _menuStandardTicketsContainer;
		private void cmdStandardTickets_Click(object sender, EventArgs e)
		{
			if (_menuStandardTickets == null)
			{
				Dictionary<string, string> items = new Dictionary<string, string>();
				items.Add("StockTicket", "Stock Ticket");
				items.Add("OptionTicket", "Option Ticket");
				items.Add("FutureTicket", "Future Ticket");

				_menuStandardTickets = new menuBaseAction(items, true);
				_menuStandardTickets.PropertyChanged += new PropertyChangedEventHandler(menuStandardTickets_PropertyChanged);
			}

			if (_menuStandardTicketsContainer == null)
			{
				_menuStandardTicketsContainer = new PropertyContainer(_menuStandardTickets);
				_menuStandardTicketsContainer.Closed += new ToolStripDropDownClosedEventHandler(menuStandardTicketsContainer_Closed);
			}

			_menuStandardTicketsContainer.Show((VistaButton)sender);
		}

		private void menuStandardTickets_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			_menuStandardTicketsContainer.Close();
			switch (e.PropertyName)
			{
				case "StockTicket":
					lock (GLOBAL.HWindows.StockTicketWindows)
					{
						GLOBAL.HWindows.OpenWindow(new frmStockTicket(), true);
					}
					break;
				case "OptionTicket":
					lock (GLOBAL.HWindows.OptionTicketWindows)
					{
						GLOBAL.HWindows.OpenWindow(new frmOptionTicket(), true);
					}
					break;
				case "FutureTicket":
					lock (GLOBAL.HWindows.FutureTicketWindows)
					{
						GLOBAL.HWindows.OpenWindow(new frmFutureTicket(), true);
					}
					break;
			}
		}

		private void menuStandardTicketsContainer_Closed(object sender, ToolStripDropDownClosedEventArgs e)
		{
			_menuStandardTicketsContainer.Closed -= new ToolStripDropDownClosedEventHandler(menuStandardTicketsContainer_Closed);
			_menuStandardTicketsContainer = null;
			_menuStandardTickets.PropertyChanged -= new PropertyChangedEventHandler(menuStandardTickets_PropertyChanged);
			_menuStandardTickets = null;
		}

		#endregion

		#region - Custom Tickets Menu -

		private menuBaseAction _menuCustomTickets;
		private PropertyContainer _menuCustomTicketsContainer;
		private void cmdCustomTickets_Click(object sender, EventArgs e)
		{
			if (_menuCustomTickets == null)
			{
				Dictionary<string, string> items = new Dictionary<string, string>();
				items.Add("QuickTradeTicket", "Quick Trade Ticket");
				items.Add("BatchTicket", "Batch Ticket");
				items.Add("MDBatchTicket", "MD Batch Ticket");
				items.Add("AutoSpreadTicket", "Auto Spread Ticket");
				items.Add("FutureMatrixTicket", "Future Matrix Ticket");

				_menuCustomTickets = new menuBaseAction(items, true);
				_menuCustomTickets.PropertyChanged += new PropertyChangedEventHandler(menuCustomTickets_PropertyChanged);
			}

			if (_menuCustomTicketsContainer == null)
			{
				_menuCustomTicketsContainer = new PropertyContainer(_menuCustomTickets);
				_menuCustomTicketsContainer.Closed += new ToolStripDropDownClosedEventHandler(menuCustomTicketsContainer_Closed);
			}

			_menuCustomTicketsContainer.Show((VistaButton)sender);
		}

		private void menuCustomTickets_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			_menuCustomTicketsContainer.Close();
			switch (e.PropertyName)
			{
				case "QuickTradeTicket":
					lock (GLOBAL.HWindows.QuickTicketWindows)
					{
						GLOBAL.HWindows.OpenWindow(new frmQuickTicket(), true);
					}
					break;
				case "BatchTicket":
					lock (GLOBAL.HWindows.BatchTicketWindows)
					{
						GLOBAL.HWindows.OpenWindow(new frmBatchTicket(), true);
					}
					break;
				case "MDBatchTicket":
					lock (GLOBAL.HWindows.BatchMarketTicketWindows)
					{
						GLOBAL.HWindows.OpenWindow(new frmBatchMarketTicket(), true);
					}
					break;
				case "AutoSpreadTicket":
					lock (GLOBAL.HWindows.AutoSpreadTicketWindows)
					{
						GLOBAL.HWindows.OpenWindow(new frmAutoSpreadTicket(), true);
					}
					break;
				case "FutureMatrixTicket":
					lock (GLOBAL.HWindows.FutureMatrixTicketWindows)
					{
						GLOBAL.HWindows.OpenWindow(new frmFutureMatrixTicket(), true);
					}
					break;
			}
		}

		private void menuCustomTicketsContainer_Closed(object sender, ToolStripDropDownClosedEventArgs e)
		{
			_menuCustomTicketsContainer.Closed -= new ToolStripDropDownClosedEventHandler(menuCustomTicketsContainer_Closed);
			_menuCustomTicketsContainer = null;
			_menuCustomTickets.PropertyChanged -= new PropertyChangedEventHandler(menuCustomTickets_PropertyChanged);
			_menuCustomTickets = null;
		}

		#endregion

		#region - Order Trade Position Click -

		private void cmdOrders_Click(object sender, EventArgs e)
		{
			lock (GLOBAL.HWindows.OrderWindows)
			{
				GLOBAL.HWindows.OpenWindow(new frmOrders(), true);
			}
		}

		private void cmdTrades_Click(object sender, EventArgs e)
		{
			lock (GLOBAL.HWindows.TradeWindows)
			{
				GLOBAL.HWindows.OpenWindow(new frmTrades(), true);
			}
		}

		private void cmdPositions_Click(object sender, EventArgs e)
		{
			lock (GLOBAL.HWindows.PositionWindows)
			{
				GLOBAL.HWindows.OpenWindow(new frmPositions(), true);
			}
		}

		#endregion

		#region - Windows Menu -

		private void cmdWindows_Click(object sender, EventArgs e)
		{
			cmdWindows.ContextMenuStrip = GLOBAL.HWindows.WindowsMenu;
			cmdWindows.ContextMenuStrip.Show(cmdWindows, new Point(0, cmdWindows.Height));

			cmdWindows.ContextMenuStrip.Closed += new ToolStripDropDownClosedEventHandler(ContextMenuStrip_Closed);
		}

		private void ContextMenuStrip_Closed(object sender, ToolStripDropDownClosedEventArgs e)
		{
			cmdWindows.ContextMenuStrip = null;
		}

		#endregion

		#region - Main Support Functions -

		public void AutoExitApplication()
		{
			// Stop all Process
			GLOBAL.HProcess.StopProcess();

			if (!Configuration.ROM.Default.CancelOnDisconnect)
			{
				// Check for open orders
				OpenOrderExitCheck(true);
			}

			// Save user profile
			GLOBAL.UserUIProfile.Save(true);

			// Wait a bit for the last bit of ROM Messages
			System.Threading.Thread.Sleep(1000);
			Application.DoEvents();

			// Disconnect From ROM
			GLOBAL.HROM.Disconnect();

			// Save the Orders and Trades Locally
			GLOBAL.UserUIProfile.SaveAllOrderInfo();

			// Disconnection From MDS
			GLOBAL.MDSsDisconnect();

			Close();
		}

		public void ExitApplication()
		{
			if (MessageBox.Show("Exit ROC?", "Confirmation", MessageBoxButtons.YesNo) == DialogResult.Yes)
			{
				GLOBAL.ShuttingDown = true;

				// Stop all Process
				GLOBAL.HProcess.StopProcess();

				// Check for open orders
				OpenOrderExitCheck(false);

				// Save the user profile
				GLOBAL.UserUIProfile.Save(false);

				// Wait a bit for the last bit of ROM Messages
				System.Threading.Thread.Sleep(1000);
				Application.DoEvents();

				// Disconnect From ROM
				GLOBAL.HROM.Disconnect();

				// Save the Orders and Trades Locally
				GLOBAL.UserUIProfile.SaveAllOrderInfo();

				// Disconnection From MDS
				GLOBAL.MDSsDisconnect();

				Close();
			}
		}

		private void OpenOrderExitCheck(bool isAuto)
		{
			if (!Configuration.ROM.Default.CancelOnDisconnect)
			{
				//DataRow[] rows = GLOBAL.HOrders.Table.Select(String.Format("Status = {0} Or Status = {1} Or Status = {2} Or Status = {3}", CSVEx.CSVFieldIDs.StatusCodes.Filled, CSVEx.CSVFieldIDs.StatusCodes.FilledAndCancelled, CSVEx.CSVFieldIDs.StatusCodes.Rejected, CSVEx.CSVFieldIDs.StatusCodes.Canceled));
				//if (GLOBAL.HOrders.Table.Rows.Count != rows.Length)
				if (GLOBAL.HROM.HasOpenOrders())
				{
					if (isAuto)
					{
						GLOBAL.HROM.CancelAllOpenOrders("");
					}
					else
					{
						if (MessageBox.Show("Cancel All Open Orders?", "Confirmation", MessageBoxButtons.YesNo) == DialogResult.Yes)
						{
							//GLOBAL.HROM.CancelAllOrder(false, "");
							GLOBAL.HROM.CancelAllOpenOrders("");
						}
					}
				}
			}
		}

		#endregion

		#region - Main Process -

		private bool _processingMarketData = false;
		public bool ProcessingMarketData
		{
			get
			{
				return _processingMarketData;
			}
			set
			{
				_processingMarketData = value;
			}
		}

		private delegate void ProcessDelegate();
		public void ProcessMarketData()
		{
			if (InvokeRequired)
			{
				BeginInvoke(new ProcessDelegate(ProcessMarketData));
				return;
			}

			try
			{
				GLOBAL.HProcess.ProcessAllMarketData();
				ProcessingMarketData = false;
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
				ProcessingMarketData = false;
			}
		}

		private bool _processingCSV = false;
		public bool ProcessingCSV
		{
			get
			{
				return _processingCSV;
			}
			set
			{
				_processingCSV = value;
			}
		}

		private delegate void ProcessCSVDelegate();
		public void ProcessCSV()
		{
			if (InvokeRequired)
			{
				BeginInvoke(new ProcessCSVDelegate(ProcessCSV));
				return;
			}

			try
			{
				GLOBAL.HProcess.ProcessAllCSV();
				ProcessingCSV = false;
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
				ProcessingCSV = false;
			}
		}

		#endregion
	}
}