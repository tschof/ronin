using System;

using FormEx;
using System.Windows.Forms;
using System.Diagnostics;

namespace ROC
{
	[System.ComponentModel.DesignerCategory("Form")]
	public partial class frmLogin : VistaWindowBorderless
	{
		private enum LoginPanels
		{
			Home,
			Password,
			Setting,
		}

		private Timer _loginTimer;
		private bool _loginClicked = false;

		public frmLogin()
		{
			InitializeComponent();

			Initialize();
		}

		private void Initialize()
		{
			cmdLogin.Enabled = false;

			lblROM.Text = Configuration.ROM.Default.IP + " : " + Configuration.ROM.Default.Port;
			lblROM.SetToolTip(lblROM.Text);

			if (GLOBAL.HRDS.Address.ToUpper().Contains("RDS_DEV"))
			{
				lblRDS.Text = "RDS_DEV";
			}
			else if (GLOBAL.HRDS.Address.ToUpper().Contains("RDS_BETA"))
			{
				lblRDS.Text = "RDS_BETA";
			}
			else if (GLOBAL.HRDS.Address.ToUpper().Contains("RDS_PROD"))
			{
				lblRDS.Text = "RDS_PROD";
			}
			else
			{
				lblRDS.Text = GLOBAL.HRDS.Address;
			}
			lblRDS.SetToolTip(lblRDS.Text);

			lblMDS.Text = Configuration.MDS.Default.IP + " : " + Configuration.MDS.Default.Port;
			lblMDS.SetToolTip(lblMDS.Text);

			lblMDSBackup.Text = Configuration.MDS.Default.IPBackup + " : " + Configuration.MDS.Default.PortBackup;
			lblMDSBackup.SetToolTip(lblMDSBackup.Text);

			chkUseMarketData.Checked = Configuration.User.Default.UseMarketData;
			chkUseTpos.Checked = Configuration.User.Default.UseTPOS;
			chkUseROMDatabase.Checked = Configuration.User.Default.UseROMDatabase;

			// Option Choices
			chkShowOnlySelectedExchange.Checked = Configuration.User.Default.ShowOnlySelectedExchange;
			chkDoNotLoadOPRAParticipant.Checked = Configuration.User.Default.DoNotLoadOPRAParticipant;

			// Order Choices
			chkCancelOnDisconnect.Checked = Configuration.ROM.Default.CancelOnDisconnect;
			chkConfimOnCancelAll.Checked = Configuration.User.Default.ConfirmOnCancellAll;
			chkPlayAllStatusSounds.Checked = Configuration.User.Default.PlayAllStatusSound;
			chkShowInternalStatus.Checked = Configuration.User.Default.ShowInternalStatus;

			// Disply Choices
			chkLargeScreenSize.Checked = Configuration.ROC.Default.LargeScreenSize;
			chkDelayUIProcess.Checked = Configuration.User.Default.DelayUIProcess;
			ShowPanel(LoginPanels.Home);

			//if (Configuration.User.Default.LastUserName != "")
			//{
			//    txtUserName.Text = Configuration.User.Default.LastUserName;
			//    txtPassword.Select();
			//}

			UITimerTicked += new EventHandler(Form_UITimerTicked);
			StartTimer(Configuration.User.Default.UIUpdateRate);
		}

		#region - Form Event -

		private void Form_UITimerTicked(object sender, EventArgs e)
		{
			if (!TopLevel)
			{
				Show();
				BringToFront();
			}
		}

		#endregion

		#region - Login Timer -

		private void StartLoginTimer()
		{
			if (_loginTimer == null)
			{
				_loginTimer = new Timer();
				_loginTimer.Interval = 100;
				_loginTimer.Tick += new EventHandler(LogginTimer_Tick);
				_loginTimer.Enabled = true;
			}

			txtUserName.Enabled = false;
			txtPassword.Enabled = false;
			cmdLogin.Enabled = false;
			cmdCancel.Enabled = false;

			picProcessing.Visible = true;

			lblLoginStatus.Text = "Login Started...";
			lblROMStatus.Text = "";
		}

		private void StopLoginTimer()
		{
			if (_loginTimer != null)
			{
				_loginTimer.Stop();
				_loginTimer.Tick -= new EventHandler(LogginTimer_Tick);
			}
			_loginTimer = null;

			txtUserName.Enabled = true;
			txtPassword.Enabled = true;
			LoginEnable();
			cmdCancel.Enabled = true;

			picProcessing.Visible = false;

			if (GLOBAL.HRDS.Status != HelperRDS.StatusTypes.Done)
			{
				GLOBAL.OrderManagers = new HelperROM();
				GLOBAL.HRDS = new HelperRDS();
				GLOBAL.MarketDataProviders = new System.Collections.Generic.List<HelperMDS>();
			}
		}

		private void LogginTimer_Tick(object sender, EventArgs e)
		{
			try
			{
				switch (GLOBAL.OrderManagers.Status)
				{
					case HelperROM.StatusTypes.LoggedIn:
						#region - RDS Logging In -
						if (GLOBAL.HRDS.StatusDsp != "")
						{
							lblLoginStatus.Text = GLOBAL.HRDS.StatusDsp;
							lblLoginStatus.SetToolTip(lblLoginStatus.Text);
						}

						switch (GLOBAL.HRDS.Status)
						{
							case HelperRDS.StatusTypes.ConnectingToMDS:
								GLOBAL.HRDS.ConnectingToMDS();
								break;
							case HelperRDS.StatusTypes.ConnectedToMDS:
								GLOBAL.HRDS.LoginDone();
								break;
							case HelperRDS.StatusTypes.ConnectionToMDSFailed:
								//GLOBAL.HROM.Disconnect();
								//StopLoginTimer();

								GLOBAL.HRDS.LoginDone();
								break;
							case HelperRDS.StatusTypes.Done:
								StopLoginTimer();

								// Save the User Name after a succesful login
								Configuration.User.Default.LastUserName = GLOBAL.OrderManagers.UserName;
								Configuration.User.Default.Save();

								Close();
								break;
							case HelperRDS.StatusTypes.Failed:
								GLOBAL.OrderManagers.Disconnect();
								StopLoginTimer();
								break;
							default:
								#region - RDS Initializing -

								switch (GLOBAL.HRDS.Status)
								{
									case HelperRDS.StatusTypes.None:
										GLOBAL.HRDS.GetVersion();
										break;
									case HelperRDS.StatusTypes.GettingVersion:
										break;
									case HelperRDS.StatusTypes.GotVersion:
										GLOBAL.HRDS.GetUserProfile(GLOBAL.OrderManagers.UserName, GLOBAL.OrderManagers.Password);
										break;
									case HelperRDS.StatusTypes.GettingUserprofiles:
										break;
									case HelperRDS.StatusTypes.GotUserprofiles:
										GLOBAL.HRDS.GetUserMarketDataSource();
										break;
									case HelperRDS.StatusTypes.GettingUserMarketDataSources:
										break;
									case HelperRDS.StatusTypes.GotUserMarketDataSources:
										GLOBAL.HRDS.GetUserOrders();
										break;
									case HelperRDS.StatusTypes.GettingUserOrders:
										break;
									case HelperRDS.StatusTypes.GotUserOrders:
										GLOBAL.HRDS.GetUserExecutions();
										break;
									case HelperRDS.StatusTypes.GettingUserExecutions:
										break;
									case HelperRDS.StatusTypes.GotUserExecutions:
										GLOBAL.HRDS.GetTPOSPositions();
										break;
									case HelperRDS.StatusTypes.GettingTPOSPositions:
										break;
									case HelperRDS.StatusTypes.GotTPOSPositions:
										GLOBAL.HRDS.GetTPOSTrades(false);
										break;
									case HelperRDS.StatusTypes.GettingTPOSTrades:
										break;
									case HelperRDS.StatusTypes.GotTPOSTrades:
										GLOBAL.HRDS.ConnectToMDS();
										break;
									case HelperRDS.StatusTypes.GotError:
										if (GLOBAL.ByPassRdsLogin || !Configuration.User.Default.UseTPOS)
										{
											GLOBAL.HRDS.ConnectToMDS();
										}
										else
										{
											GLOBAL.HRDS.LoginFailed();
											GLOBAL.OrderManagers.Disconnect();
											StopLoginTimer();
										}
										break;
								}

								#endregion
								break;
						}
						#endregion
						break;
					default:
						#region - ROM Logging In -

						if (GLOBAL.OrderManagers.StatusDsp != "")
						{
							lblROMStatus.Text = GLOBAL.OrderManagers.StatusDsp;
							lblROMStatus.SetToolTip(lblROMStatus.Text);
						}

						if (GLOBAL.OrderManagers.RomMsgDsp != "")
						{
							lblLoginStatus.Text = GLOBAL.OrderManagers.RomMsgDsp;
							lblLoginStatus.SetToolTip(lblLoginStatus.Text);
						}

						switch (GLOBAL.OrderManagers.Status)
						{
							case HelperROM.StatusTypes.Stopped:
								if (_loginClicked)
								{
									_loginClicked = false;
									if (GLOBAL.ByPassRomLogin)
										ByPassROMLogin();
									else
										GLOBAL.OrderManagers.Connect();
								}
								else
								{
									GLOBAL.OrderManagers.Disconnect();
									StopLoginTimer();
								}
								break;
							case HelperROM.StatusTypes.Stopping:
								break;
							case HelperROM.StatusTypes.Started:
								GLOBAL.UserUIProfile.LoadAllOrderInfo(txtUserName.Text);
								GLOBAL.OrderManagers.Loggin(txtUserName.Text, txtPassword.Text);
								break;
							case HelperROM.StatusTypes.Starting:
								break;
							case HelperROM.StatusTypes.LoggingIn:
								break;
							case HelperROM.StatusTypes.LogInFailed:
								//GLOBAL.HROM.Disconnect();
								StopLoginTimer();
								break;
							case HelperROM.StatusTypes.Error:
								if (GLOBAL.ByPassRomLogin)
								{
									ByPassROMLogin();
								}
								else
								{
									GLOBAL.OrderManagers.Disconnect();
									StopLoginTimer();
								}
								break;
						}

						#endregion
						break;
				}

				if (GLOBAL.OrderManagers.AlertList.Count > 0)
				{
					GLOBAL.OrderManagers.ShowAlerts();
				}

				if (GLOBAL.HRDS.AlertList.Count > 0)
				{
					GLOBAL.HRDS.ShowAlerts();
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		private void ByPassROMLogin()
		{
			GLOBAL.OrderManagers.UserName = txtUserName.Text;
			GLOBAL.OrderManagers.Password = txtPassword.Text;

			// When the ROM server is mssing
			GLOBAL.OrderManagers.Status = HelperROM.StatusTypes.LoggedIn;
			GLOBAL.OrderManagers.EndOfQueuedMsg = true;
		}

		#endregion

		private void txtUserName_TextChanged(object sender, EventArgs e)
		{
			LoginEnable();
		}

		private void txtPassword_TextChanged(object sender, EventArgs e)
		{
			LoginEnable();
		}

		private void LoginEnable()
		{
			if (txtUserName.Text == "" || txtPassword.Text == "")
			{
				cmdLogin.Enabled = false;
			}
			else
			{
				cmdLogin.Enabled = true;
			}
		}

		private void txtPassword_KeyDown(object sender, KeyEventArgs e)
		{
			switch (e.KeyCode)
			{
				case Keys.Enter:
					cmdLogin_Click(null, EventArgs.Empty);
					break;
			}
		}

		private void cmdLogin_Click(object sender, EventArgs e)
		{
			_loginClicked = true;
			StartLoginTimer();
		}

		private void cmdCancel_Click(object sender, EventArgs e)
		{
			Close();
		}

		private void cmdHome_Click(object sender, EventArgs e)
		{
			ShowPanel(LoginPanels.Home);
		}

		private void cmdPassword_Click(object sender, EventArgs e)
		{
			ShowPanel(LoginPanels.Password);
		}

		private void cmdSetting_Click(object sender, EventArgs e)
		{
			ShowPanel(LoginPanels.Setting);
		}

		private void ShowPanel(LoginPanels panel)
		{
			switch (panel)
			{
				case LoginPanels.Home:
					cmdHome.Enabled = false;
					cmdPassword.Enabled = true;
					cmdSetting.Enabled = true;

					panelManagerLogin.SelectedPanel = managedPanelLogin;

					ActiveControl = txtUserName;
					break;
				case LoginPanels.Password:
					cmdHome.Enabled = true;
					cmdPassword.Enabled = false;
					cmdSetting.Enabled = true;

					panelManagerLogin.SelectedPanel = managedPanelPassword;
					break;
				case LoginPanels.Setting:
					cmdHome.Enabled = true;
					cmdPassword.Enabled = true;
					cmdSetting.Enabled = false;

					panelManagerLogin.SelectedPanel = managedPanelSetting;
					break;
			}
		}

		#region - Settings -

		private void chkUseMarketData_CheckedChanged(object sender, EventArgs e)
		{
			Configuration.User.Default.UseMarketData = chkUseMarketData.Checked;
			Configuration.User.Default.Save();
		}

		private void chkUseTpos_CheckedChanged(object sender, EventArgs e)
		{
			Configuration.User.Default.UseTPOS = chkUseTpos.Checked;
			Configuration.User.Default.Save();
		}

		private void chkUseROMDatabase_CheckedChanged(object sender, EventArgs e)
		{
			Configuration.User.Default.UseROMDatabase = chkUseROMDatabase.Checked;
			Configuration.User.Default.Save();
		}

		private void chkShowOnlySelectedExchange_CheckedChanged(object sender, EventArgs e)
		{
			Configuration.User.Default.ShowOnlySelectedExchange = chkShowOnlySelectedExchange.Checked;
			Configuration.User.Default.Save();
		}

		private void chkDoNotLoadOPRAParticipant_CheckedChanged(object sender, EventArgs e)
		{
			Configuration.User.Default.DoNotLoadOPRAParticipant = chkDoNotLoadOPRAParticipant.Checked;
			Configuration.User.Default.Save();
		}

		private void chkConfimOnCancelAll_CheckedChanged(object sender, EventArgs e)
		{
			Configuration.User.Default.ConfirmOnCancellAll = chkConfimOnCancelAll.Checked;
			Configuration.User.Default.Save();
		}

		private void chkPlayAllStatusSounds_CheckedChanged(object sender, EventArgs e)
		{
			Configuration.User.Default.PlayAllStatusSound = chkPlayAllStatusSounds.Checked;
			Configuration.User.Default.Save();
		}

		private void chkShowInternalStatus_CheckedChanged(object sender, EventArgs e)
		{
			Configuration.User.Default.ShowInternalStatus = chkShowInternalStatus.Checked;
			Configuration.User.Default.Save();
		}

		private void chkDelayUIProcess_CheckedChanged(object sender, EventArgs e)
		{
			Configuration.User.Default.DelayUIProcess = chkDelayUIProcess.Checked;
			Configuration.User.Default.Save();
		}

		private void chkCancelOnDisconnect_CheckedChanged(object sender, EventArgs e)
		{
			Configuration.ROM.Default.CancelOnDisconnect = chkCancelOnDisconnect.Checked;
			Configuration.ROM.Default.Save();
		}

		private void chkSkipGTCandGTD_CheckedChanged(object sender, EventArgs e)
		{
			Configuration.ROM.Default.SkipGTCandGTD = chkSkipGTCandGTD.Checked;
			Configuration.ROM.Default.Save();
		}

		private void chkLargeScreenSize_CheckedChanged(object sender, EventArgs e)
		{
			Configuration.ROC.Default.LargeScreenSize = chkLargeScreenSize.Checked;
			Configuration.ROC.Default.Save();
		}

		private void chkAutoFocusOnControls_CheckedChanged(object sender, EventArgs e)
		{
			Configuration.User.Default.AutoFocusOnControls = chkAutoFocusOnControls.Checked;
			Configuration.User.Default.Save();
		}

		#endregion

		#region - Change Password -

		private void cmdPasswordApply_Click(object sender, EventArgs e)
		{
			if (VarifyNewPassword())
			{
				GLOBAL.HRDS.UpdateUserPassword(txtPasswordUserName.Text, txtPasswordOldPassword.Text, txtPasswordNewPassword.Text);

				lblROMStatus.Text = "New Password : (Updated)";
			}
		}

		private bool VarifyNewPassword()
		{
			if (txtPasswordUserName.Text == "" || 
				txtPasswordOldPassword.Text == "" || 
				txtPasswordNewPassword.Text == "" ||
				txtPasswordConfirmNewPassword.Text == "")
			{
				lblROMStatus.Text = "New Password : (Missing Infomation)";
				return false;
			}
			else if (txtPasswordOldPassword.Text == txtPasswordNewPassword.Text)
			{
				lblROMStatus.Text = "New Password : (Same As Old?)";
				return false;
			}
			else if (txtPasswordNewPassword.Text != txtPasswordConfirmNewPassword.Text)
			{
				lblROMStatus.Text = "New Password : (MissMatch)";
				return false;
			}

			lblROMStatus.Text = "New Password : (Updating...)";
			return true;
		}

		private void cmdPasswordCancel_Click(object sender, EventArgs e)
		{
			txtPasswordUserName.Text = "";
			txtPasswordOldPassword.Text = "";
			txtPasswordNewPassword.Text = "";
			txtPasswordConfirmNewPassword.Text = "";

			ShowPanel(LoginPanels.Home);
		}

		#endregion

		private void txtPasswordUserName_KeyUp(object sender, KeyEventArgs e)
		{
			switch (e.KeyCode)
			{
				case Keys.Enter:
					txtPasswordOldPassword.Select();
					break;
			}
		}

		private void txtPasswordOldPassword_KeyUp(object sender, KeyEventArgs e)
		{
			switch (e.KeyCode)
			{
				case Keys.Enter:
					txtPasswordNewPassword.Select();
					break;
			}
		}

		private void txtPasswordNewPassword_KeyUp(object sender, KeyEventArgs e)
		{
			switch (e.KeyCode)
			{
				case Keys.Enter:
					txtPasswordConfirmNewPassword.Select();
					break;
			}
		}

		private void txtPasswordConfirmNewPassword_KeyUp(object sender, KeyEventArgs e)
		{
			switch (e.KeyCode)
			{
				case Keys.Enter:
					cmdPasswordApply.Select();
					break;
			}
		}
	}
}