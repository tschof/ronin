namespace ROC
{
	partial class frmLogin
	{
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		/// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
		protected override void Dispose(bool disposing)
		{
			if (disposing && (components != null))
			{
				components.Dispose();
			}
			base.Dispose(disposing);
		}

		#region Windows Form Designer generated code

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(frmLogin));
			this.CaptionBar = new FormEx.VistaWindowCaptionBar();
			this.panelManagerLogin = new PanelEx.PanelManager();
			this.managedPanelLogin = new PanelEx.ManagedPanel();
			this.picProcessing = new System.Windows.Forms.PictureBox();
			this.picLogo = new System.Windows.Forms.PictureBox();
			this.vistaPanelLogin = new PanelEx.VistaPanel();
			this.txtPassword = new System.Windows.Forms.TextBox();
			this.dspPassword = new LabelEx.LabelBase();
			this.txtUserName = new System.Windows.Forms.TextBox();
			this.dspUserName = new LabelEx.LabelBase();
			this.lblMDSBackup = new LabelEx.LabelBase();
			this.lblMDS = new LabelEx.LabelBase();
			this.dspMDS = new LabelEx.LabelBase();
			this.lblRDS = new LabelEx.LabelBase();
			this.dspRDS = new LabelEx.LabelBase();
			this.lblROM = new LabelEx.LabelBase();
			this.dspROM = new LabelEx.LabelBase();
			this.cmdCancel = new ButtonEx.VistaButton();
			this.cmdLogin = new ButtonEx.VistaButton();
			this.managedPanelPassword = new PanelEx.ManagedPanel();
			this.vistaPanelPassowrd = new PanelEx.VistaPanel();
			this.cmdPasswordCancel = new ButtonEx.VistaButton();
			this.cmdPasswordApply = new ButtonEx.VistaButton();
			this.txtPasswordConfirmNewPassword = new System.Windows.Forms.TextBox();
			this.dspPasswordConfirmNewPassword = new LabelEx.LabelBase();
			this.txtPasswordNewPassword = new System.Windows.Forms.TextBox();
			this.dspPasswordNewPassword = new LabelEx.LabelBase();
			this.txtPasswordOldPassword = new System.Windows.Forms.TextBox();
			this.dspPasswordOldPassword = new LabelEx.LabelBase();
			this.txtPasswordUserName = new System.Windows.Forms.TextBox();
			this.dspPasswordUserName = new LabelEx.LabelBase();
			this.managedPanelSetting = new PanelEx.ManagedPanel();
			this.pictureBoxLogo = new System.Windows.Forms.PictureBox();
			this.vistaPanelSetting = new PanelEx.VistaPanel();
			this.chkPlayAllStatusSounds = new System.Windows.Forms.CheckBox();
			this.chkUseMarketData = new System.Windows.Forms.CheckBox();
			this.chkDelayUIProcess = new System.Windows.Forms.CheckBox();
			this.chkLargeScreenSize = new System.Windows.Forms.CheckBox();
			this.chkAutoFocusOnControls = new System.Windows.Forms.CheckBox();
			this.lblDisplayChoices = new LabelEx.LabelVista();
			this.chkDoNotLoadOPRAParticipant = new System.Windows.Forms.CheckBox();
			this.chkShowOnlySelectedExchange = new System.Windows.Forms.CheckBox();
			this.lblOptionChoices = new LabelEx.LabelVista();
			this.chkShowInternalStatus = new System.Windows.Forms.CheckBox();
			this.chkConfimOnCancelAll = new System.Windows.Forms.CheckBox();
			this.chkSkipGTCandGTD = new System.Windows.Forms.CheckBox();
			this.chkCancelOnDisconnect = new System.Windows.Forms.CheckBox();
			this.lblOrderChoices = new LabelEx.LabelVista();
			this.chkUseROMDatabase = new System.Windows.Forms.CheckBox();
			this.chkUseTpos = new System.Windows.Forms.CheckBox();
			this.lblROCChoices = new LabelEx.LabelVista();
			this.vistaPanelCommandStrip = new PanelEx.VistaPanel();
			this.cmdHome = new ButtonEx.VistaButton();
			this.cmdPassword = new ButtonEx.VistaButton();
			this.cmdSetting = new ButtonEx.VistaButton();
			this.lblLoginStatus = new LabelEx.LabelVista();
			this.lblROMStatus = new LabelEx.LabelBase();
			this.panelManagerLogin.SuspendLayout();
			this.managedPanelLogin.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.picProcessing)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.picLogo)).BeginInit();
			this.vistaPanelLogin.SuspendLayout();
			this.managedPanelPassword.SuspendLayout();
			this.vistaPanelPassowrd.SuspendLayout();
			this.managedPanelSetting.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.pictureBoxLogo)).BeginInit();
			this.vistaPanelSetting.SuspendLayout();
			this.vistaPanelCommandStrip.SuspendLayout();
			this.SuspendLayout();
			// 
			// CaptionBar
			// 
			this.CaptionBar.BackColor = System.Drawing.Color.Transparent;
			this.CaptionBar.Dock = System.Windows.Forms.DockStyle.Top;
			this.CaptionBar.IconIndex = 1;
			this.CaptionBar.Location = new System.Drawing.Point(0, 0);
			this.CaptionBar.Name = "CaptionBar";
			this.CaptionBar.Size = new System.Drawing.Size(484, 26);
			this.CaptionBar.TabIndex = 0;
			this.CaptionBar.TabStop = false;
			// 
			// panelManagerLogin
			// 
			this.panelManagerLogin.Controls.Add(this.managedPanelLogin);
			this.panelManagerLogin.Controls.Add(this.managedPanelPassword);
			this.panelManagerLogin.Controls.Add(this.managedPanelSetting);
			this.panelManagerLogin.Dock = System.Windows.Forms.DockStyle.Fill;
			this.panelManagerLogin.Location = new System.Drawing.Point(0, 51);
			this.panelManagerLogin.Name = "panelManagerLogin";
			this.panelManagerLogin.SelectedIndex = 0;
			this.panelManagerLogin.SelectedPanel = this.managedPanelLogin;
			this.panelManagerLogin.Size = new System.Drawing.Size(484, 234);
			this.panelManagerLogin.TabIndex = 1;
			this.panelManagerLogin.TabStop = false;
			// 
			// managedPanelLogin
			// 
			this.managedPanelLogin.Controls.Add(this.picProcessing);
			this.managedPanelLogin.Controls.Add(this.picLogo);
			this.managedPanelLogin.Controls.Add(this.vistaPanelLogin);
			this.managedPanelLogin.Location = new System.Drawing.Point(0, 0);
			this.managedPanelLogin.Name = "managedPanelLogin";
			this.managedPanelLogin.Size = new System.Drawing.Size(484, 234);
			// 
			// picProcessing
			// 
			this.picProcessing.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.picProcessing.Image = global::ROC.Properties.Resources.PROCESS_BLUEANDWHITE;
			this.picProcessing.Location = new System.Drawing.Point(0, 217);
			this.picProcessing.Name = "picProcessing";
			this.picProcessing.Size = new System.Drawing.Size(324, 17);
			this.picProcessing.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
			this.picProcessing.TabIndex = 3;
			this.picProcessing.TabStop = false;
			this.picProcessing.Visible = false;
			// 
			// picLogo
			// 
			this.picLogo.BackColor = System.Drawing.SystemColors.Control;
			this.picLogo.Dock = System.Windows.Forms.DockStyle.Fill;
			this.picLogo.Image = global::ROC.Properties.Resources.LOGO;
			this.picLogo.Location = new System.Drawing.Point(0, 0);
			this.picLogo.Name = "picLogo";
			this.picLogo.Size = new System.Drawing.Size(324, 234);
			this.picLogo.SizeMode = System.Windows.Forms.PictureBoxSizeMode.CenterImage;
			this.picLogo.TabIndex = 2;
			this.picLogo.TabStop = false;
			// 
			// vistaPanelLogin
			// 
			this.vistaPanelLogin.BackColor = System.Drawing.Color.Black;
			this.vistaPanelLogin.Controls.Add(this.txtPassword);
			this.vistaPanelLogin.Controls.Add(this.dspPassword);
			this.vistaPanelLogin.Controls.Add(this.txtUserName);
			this.vistaPanelLogin.Controls.Add(this.dspUserName);
			this.vistaPanelLogin.Controls.Add(this.lblMDSBackup);
			this.vistaPanelLogin.Controls.Add(this.lblMDS);
			this.vistaPanelLogin.Controls.Add(this.dspMDS);
			this.vistaPanelLogin.Controls.Add(this.lblRDS);
			this.vistaPanelLogin.Controls.Add(this.dspRDS);
			this.vistaPanelLogin.Controls.Add(this.lblROM);
			this.vistaPanelLogin.Controls.Add(this.dspROM);
			this.vistaPanelLogin.Controls.Add(this.cmdCancel);
			this.vistaPanelLogin.Controls.Add(this.cmdLogin);
			this.vistaPanelLogin.Dock = System.Windows.Forms.DockStyle.Right;
			this.vistaPanelLogin.ForeColor = System.Drawing.Color.Silver;
			this.vistaPanelLogin.Location = new System.Drawing.Point(324, 0);
			this.vistaPanelLogin.Name = "vistaPanelLogin";
			this.vistaPanelLogin.Padding = new System.Windows.Forms.Padding(5);
			this.vistaPanelLogin.Size = new System.Drawing.Size(160, 234);
			this.vistaPanelLogin.TabIndex = 1;
			// 
			// txtPassword
			// 
			this.txtPassword.BackColor = System.Drawing.SystemColors.ControlDarkDark;
			this.txtPassword.Dock = System.Windows.Forms.DockStyle.Top;
			this.txtPassword.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.txtPassword.ForeColor = System.Drawing.Color.White;
			this.txtPassword.Location = new System.Drawing.Point(5, 166);
			this.txtPassword.Name = "txtPassword";
			this.txtPassword.PasswordChar = '*';
			this.txtPassword.Size = new System.Drawing.Size(150, 26);
			this.txtPassword.TabIndex = 1;
			this.txtPassword.TextChanged += new System.EventHandler(this.txtPassword_TextChanged);
			this.txtPassword.KeyDown += new System.Windows.Forms.KeyEventHandler(this.txtPassword_KeyDown);
			// 
			// dspPassword
			// 
			this.dspPassword.AutoColor = false;
			this.dspPassword.AutoSize = true;
			this.dspPassword.DefaultColor = System.Drawing.Color.White;
			this.dspPassword.DefaultText = "Password :";
			this.dspPassword.DisplayFactor = 1;
			this.dspPassword.Dock = System.Windows.Forms.DockStyle.Top;
			this.dspPassword.DownColor = System.Drawing.Color.Red;
			this.dspPassword.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspPassword.ForeColor = System.Drawing.Color.Silver;
			this.dspPassword.Is64Th = false;
			this.dspPassword.IsCurrency = false;
			this.dspPassword.IsFraction = false;
			this.dspPassword.IsPercent = false;
			this.dspPassword.Location = new System.Drawing.Point(5, 151);
			this.dspPassword.MaxDecimal = 7;
			this.dspPassword.Name = "dspPassword";
			this.dspPassword.Padding = new System.Windows.Forms.Padding(1);
			this.dspPassword.Size = new System.Drawing.Size(71, 15);
			this.dspPassword.TabIndex = 19;
			this.dspPassword.Text = "Password :";
			this.dspPassword.TickSize = 0.01;
			this.dspPassword.UpColor = System.Drawing.Color.LimeGreen;
			this.dspPassword.Value = "";
			// 
			// txtUserName
			// 
			this.txtUserName.BackColor = System.Drawing.SystemColors.ControlDarkDark;
			this.txtUserName.Dock = System.Windows.Forms.DockStyle.Top;
			this.txtUserName.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.txtUserName.ForeColor = System.Drawing.Color.White;
			this.txtUserName.Location = new System.Drawing.Point(5, 125);
			this.txtUserName.Name = "txtUserName";
			this.txtUserName.Size = new System.Drawing.Size(150, 26);
			this.txtUserName.TabIndex = 0;
			this.txtUserName.TextChanged += new System.EventHandler(this.txtUserName_TextChanged);
			// 
			// dspUserName
			// 
			this.dspUserName.AutoColor = false;
			this.dspUserName.AutoSize = true;
			this.dspUserName.DefaultColor = System.Drawing.Color.White;
			this.dspUserName.DefaultText = "User Name :";
			this.dspUserName.DisplayFactor = 1;
			this.dspUserName.Dock = System.Windows.Forms.DockStyle.Top;
			this.dspUserName.DownColor = System.Drawing.Color.Red;
			this.dspUserName.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspUserName.ForeColor = System.Drawing.Color.Silver;
			this.dspUserName.Is64Th = false;
			this.dspUserName.IsCurrency = false;
			this.dspUserName.IsFraction = false;
			this.dspUserName.IsPercent = false;
			this.dspUserName.Location = new System.Drawing.Point(5, 110);
			this.dspUserName.MaxDecimal = 7;
			this.dspUserName.Name = "dspUserName";
			this.dspUserName.Padding = new System.Windows.Forms.Padding(1);
			this.dspUserName.Size = new System.Drawing.Size(79, 15);
			this.dspUserName.TabIndex = 18;
			this.dspUserName.Text = "User Name :";
			this.dspUserName.TickSize = 0.01;
			this.dspUserName.UpColor = System.Drawing.Color.LimeGreen;
			this.dspUserName.Value = "";
			// 
			// lblMDSBackup
			// 
			this.lblMDSBackup.AutoColor = false;
			this.lblMDSBackup.AutoEllipsis = true;
			this.lblMDSBackup.AutoSize = true;
			this.lblMDSBackup.DefaultColor = System.Drawing.Color.White;
			this.lblMDSBackup.DefaultText = "xxx.xxx.xxx.xxx";
			this.lblMDSBackup.DisplayFactor = 1;
			this.lblMDSBackup.Dock = System.Windows.Forms.DockStyle.Top;
			this.lblMDSBackup.DownColor = System.Drawing.Color.Red;
			this.lblMDSBackup.ForeColor = System.Drawing.Color.Olive;
			this.lblMDSBackup.Is64Th = false;
			this.lblMDSBackup.IsCurrency = false;
			this.lblMDSBackup.IsFraction = false;
			this.lblMDSBackup.IsPercent = false;
			this.lblMDSBackup.Location = new System.Drawing.Point(5, 95);
			this.lblMDSBackup.MaxDecimal = 7;
			this.lblMDSBackup.MaximumSize = new System.Drawing.Size(150, 15);
			this.lblMDSBackup.Name = "lblMDSBackup";
			this.lblMDSBackup.Padding = new System.Windows.Forms.Padding(5, 1, 1, 1);
			this.lblMDSBackup.Size = new System.Drawing.Size(82, 15);
			this.lblMDSBackup.TabIndex = 21;
			this.lblMDSBackup.Text = "xxx.xxx.xxx.xxx";
			this.lblMDSBackup.TickSize = 0.01;
			this.lblMDSBackup.UpColor = System.Drawing.Color.LimeGreen;
			this.lblMDSBackup.Value = "";
			// 
			// lblMDS
			// 
			this.lblMDS.AutoColor = false;
			this.lblMDS.AutoEllipsis = true;
			this.lblMDS.AutoSize = true;
			this.lblMDS.DefaultColor = System.Drawing.Color.White;
			this.lblMDS.DefaultText = "xxx.xxx.xxx.xxx";
			this.lblMDS.DisplayFactor = 1;
			this.lblMDS.Dock = System.Windows.Forms.DockStyle.Top;
			this.lblMDS.DownColor = System.Drawing.Color.Red;
			this.lblMDS.ForeColor = System.Drawing.SystemColors.GradientActiveCaption;
			this.lblMDS.Is64Th = false;
			this.lblMDS.IsCurrency = false;
			this.lblMDS.IsFraction = false;
			this.lblMDS.IsPercent = false;
			this.lblMDS.Location = new System.Drawing.Point(5, 80);
			this.lblMDS.MaxDecimal = 7;
			this.lblMDS.MaximumSize = new System.Drawing.Size(150, 15);
			this.lblMDS.Name = "lblMDS";
			this.lblMDS.Padding = new System.Windows.Forms.Padding(5, 1, 1, 1);
			this.lblMDS.Size = new System.Drawing.Size(82, 15);
			this.lblMDS.TabIndex = 17;
			this.lblMDS.Text = "xxx.xxx.xxx.xxx";
			this.lblMDS.TickSize = 0.01;
			this.lblMDS.UpColor = System.Drawing.Color.LimeGreen;
			this.lblMDS.Value = "";
			// 
			// dspMDS
			// 
			this.dspMDS.AutoColor = false;
			this.dspMDS.AutoSize = true;
			this.dspMDS.DefaultColor = System.Drawing.Color.White;
			this.dspMDS.DefaultText = "MDS :";
			this.dspMDS.DisplayFactor = 1;
			this.dspMDS.Dock = System.Windows.Forms.DockStyle.Top;
			this.dspMDS.DownColor = System.Drawing.Color.Red;
			this.dspMDS.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspMDS.ForeColor = System.Drawing.Color.Silver;
			this.dspMDS.Is64Th = false;
			this.dspMDS.IsCurrency = false;
			this.dspMDS.IsFraction = false;
			this.dspMDS.IsPercent = false;
			this.dspMDS.Location = new System.Drawing.Point(5, 65);
			this.dspMDS.MaxDecimal = 7;
			this.dspMDS.Name = "dspMDS";
			this.dspMDS.Padding = new System.Windows.Forms.Padding(1);
			this.dspMDS.Size = new System.Drawing.Size(44, 15);
			this.dspMDS.TabIndex = 16;
			this.dspMDS.Text = "MDS :";
			this.dspMDS.TickSize = 0.01;
			this.dspMDS.UpColor = System.Drawing.Color.LimeGreen;
			this.dspMDS.Value = "";
			// 
			// lblRDS
			// 
			this.lblRDS.AutoColor = false;
			this.lblRDS.AutoEllipsis = true;
			this.lblRDS.AutoSize = true;
			this.lblRDS.DefaultColor = System.Drawing.Color.White;
			this.lblRDS.DefaultText = "xxx.xxx.xxx.xxx";
			this.lblRDS.DisplayFactor = 1;
			this.lblRDS.Dock = System.Windows.Forms.DockStyle.Top;
			this.lblRDS.DownColor = System.Drawing.Color.Red;
			this.lblRDS.ForeColor = System.Drawing.SystemColors.GradientActiveCaption;
			this.lblRDS.Is64Th = false;
			this.lblRDS.IsCurrency = false;
			this.lblRDS.IsFraction = false;
			this.lblRDS.IsPercent = false;
			this.lblRDS.Location = new System.Drawing.Point(5, 50);
			this.lblRDS.MaxDecimal = 7;
			this.lblRDS.MaximumSize = new System.Drawing.Size(150, 15);
			this.lblRDS.Name = "lblRDS";
			this.lblRDS.Padding = new System.Windows.Forms.Padding(5, 1, 1, 1);
			this.lblRDS.Size = new System.Drawing.Size(82, 15);
			this.lblRDS.TabIndex = 15;
			this.lblRDS.Text = "xxx.xxx.xxx.xxx";
			this.lblRDS.TickSize = 0.01;
			this.lblRDS.UpColor = System.Drawing.Color.LimeGreen;
			this.lblRDS.Value = "";
			// 
			// dspRDS
			// 
			this.dspRDS.AutoColor = false;
			this.dspRDS.AutoSize = true;
			this.dspRDS.DefaultColor = System.Drawing.Color.White;
			this.dspRDS.DefaultText = "RDS :";
			this.dspRDS.DisplayFactor = 1;
			this.dspRDS.Dock = System.Windows.Forms.DockStyle.Top;
			this.dspRDS.DownColor = System.Drawing.Color.Red;
			this.dspRDS.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspRDS.ForeColor = System.Drawing.Color.Silver;
			this.dspRDS.Is64Th = false;
			this.dspRDS.IsCurrency = false;
			this.dspRDS.IsFraction = false;
			this.dspRDS.IsPercent = false;
			this.dspRDS.Location = new System.Drawing.Point(5, 35);
			this.dspRDS.MaxDecimal = 7;
			this.dspRDS.Name = "dspRDS";
			this.dspRDS.Padding = new System.Windows.Forms.Padding(1);
			this.dspRDS.Size = new System.Drawing.Size(43, 15);
			this.dspRDS.TabIndex = 14;
			this.dspRDS.Text = "RDS :";
			this.dspRDS.TickSize = 0.01;
			this.dspRDS.UpColor = System.Drawing.Color.LimeGreen;
			this.dspRDS.Value = "";
			// 
			// lblROM
			// 
			this.lblROM.AutoColor = false;
			this.lblROM.AutoEllipsis = true;
			this.lblROM.AutoSize = true;
			this.lblROM.BackColor = System.Drawing.Color.Transparent;
			this.lblROM.DefaultColor = System.Drawing.Color.White;
			this.lblROM.DefaultText = "xxx.xxx.xxx.xxx";
			this.lblROM.DisplayFactor = 1;
			this.lblROM.Dock = System.Windows.Forms.DockStyle.Top;
			this.lblROM.DownColor = System.Drawing.Color.Red;
			this.lblROM.ForeColor = System.Drawing.SystemColors.GradientActiveCaption;
			this.lblROM.Is64Th = false;
			this.lblROM.IsCurrency = false;
			this.lblROM.IsFraction = false;
			this.lblROM.IsPercent = false;
			this.lblROM.Location = new System.Drawing.Point(5, 20);
			this.lblROM.MaxDecimal = 7;
			this.lblROM.MaximumSize = new System.Drawing.Size(150, 15);
			this.lblROM.Name = "lblROM";
			this.lblROM.Padding = new System.Windows.Forms.Padding(5, 1, 1, 1);
			this.lblROM.Size = new System.Drawing.Size(82, 15);
			this.lblROM.TabIndex = 13;
			this.lblROM.Text = "xxx.xxx.xxx.xxx";
			this.lblROM.TickSize = 0.01;
			this.lblROM.UpColor = System.Drawing.Color.LimeGreen;
			this.lblROM.Value = "";
			// 
			// dspROM
			// 
			this.dspROM.AutoColor = false;
			this.dspROM.AutoSize = true;
			this.dspROM.BackColor = System.Drawing.Color.Transparent;
			this.dspROM.DefaultColor = System.Drawing.Color.White;
			this.dspROM.DefaultText = "ROM :";
			this.dspROM.DisplayFactor = 1;
			this.dspROM.Dock = System.Windows.Forms.DockStyle.Top;
			this.dspROM.DownColor = System.Drawing.Color.Red;
			this.dspROM.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspROM.ForeColor = System.Drawing.Color.Silver;
			this.dspROM.Is64Th = false;
			this.dspROM.IsCurrency = false;
			this.dspROM.IsFraction = false;
			this.dspROM.IsPercent = false;
			this.dspROM.Location = new System.Drawing.Point(5, 5);
			this.dspROM.MaxDecimal = 7;
			this.dspROM.Name = "dspROM";
			this.dspROM.Padding = new System.Windows.Forms.Padding(1);
			this.dspROM.Size = new System.Drawing.Size(45, 15);
			this.dspROM.TabIndex = 12;
			this.dspROM.Text = "ROM :";
			this.dspROM.TickSize = 0.01;
			this.dspROM.UpColor = System.Drawing.Color.LimeGreen;
			this.dspROM.Value = "";
			// 
			// cmdCancel
			// 
			this.cmdCancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
			this.cmdCancel.BackColor = System.Drawing.Color.Black;
			this.cmdCancel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdCancel.ForeColor = System.Drawing.Color.Red;
			this.cmdCancel.Location = new System.Drawing.Point(100, 203);
			this.cmdCancel.Name = "cmdCancel";
			this.cmdCancel.Size = new System.Drawing.Size(55, 23);
			this.cmdCancel.TabIndex = 3;
			this.cmdCancel.Text = "Cancel";
			this.cmdCancel.Click += new System.EventHandler(this.cmdCancel_Click);
			// 
			// cmdLogin
			// 
			this.cmdLogin.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
			this.cmdLogin.AutoSize = true;
			this.cmdLogin.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdLogin.BackColor = System.Drawing.Color.Black;
			this.cmdLogin.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdLogin.ForeColor = System.Drawing.Color.LimeGreen;
			this.cmdLogin.Location = new System.Drawing.Point(5, 203);
			this.cmdLogin.Name = "cmdLogin";
			this.cmdLogin.Size = new System.Drawing.Size(48, 23);
			this.cmdLogin.TabIndex = 2;
			this.cmdLogin.Text = "Login";
			this.cmdLogin.Click += new System.EventHandler(this.cmdLogin_Click);
			// 
			// managedPanelPassword
			// 
			this.managedPanelPassword.Controls.Add(this.vistaPanelPassowrd);
			this.managedPanelPassword.Location = new System.Drawing.Point(0, 0);
			this.managedPanelPassword.Name = "managedPanelPassword";
			this.managedPanelPassword.Size = new System.Drawing.Size(0, 0);
			// 
			// vistaPanelPassowrd
			// 
			this.vistaPanelPassowrd.BackColor = System.Drawing.Color.Black;
			this.vistaPanelPassowrd.Controls.Add(this.cmdPasswordCancel);
			this.vistaPanelPassowrd.Controls.Add(this.cmdPasswordApply);
			this.vistaPanelPassowrd.Controls.Add(this.txtPasswordConfirmNewPassword);
			this.vistaPanelPassowrd.Controls.Add(this.dspPasswordConfirmNewPassword);
			this.vistaPanelPassowrd.Controls.Add(this.txtPasswordNewPassword);
			this.vistaPanelPassowrd.Controls.Add(this.dspPasswordNewPassword);
			this.vistaPanelPassowrd.Controls.Add(this.txtPasswordOldPassword);
			this.vistaPanelPassowrd.Controls.Add(this.dspPasswordOldPassword);
			this.vistaPanelPassowrd.Controls.Add(this.txtPasswordUserName);
			this.vistaPanelPassowrd.Controls.Add(this.dspPasswordUserName);
			this.vistaPanelPassowrd.Dock = System.Windows.Forms.DockStyle.Right;
			this.vistaPanelPassowrd.Location = new System.Drawing.Point(-160, 0);
			this.vistaPanelPassowrd.Name = "vistaPanelPassowrd";
			this.vistaPanelPassowrd.Padding = new System.Windows.Forms.Padding(5);
			this.vistaPanelPassowrd.Size = new System.Drawing.Size(160, 0);
			this.vistaPanelPassowrd.TabIndex = 2;
			// 
			// cmdPasswordCancel
			// 
			this.cmdPasswordCancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
			this.cmdPasswordCancel.BackColor = System.Drawing.Color.Black;
			this.cmdPasswordCancel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdPasswordCancel.ForeColor = System.Drawing.Color.Red;
			this.cmdPasswordCancel.Location = new System.Drawing.Point(100, -31);
			this.cmdPasswordCancel.Name = "cmdPasswordCancel";
			this.cmdPasswordCancel.Size = new System.Drawing.Size(55, 23);
			this.cmdPasswordCancel.TabIndex = 5;
			this.cmdPasswordCancel.Text = "Cancel";
			this.cmdPasswordCancel.Click += new System.EventHandler(this.cmdPasswordCancel_Click);
			// 
			// cmdPasswordApply
			// 
			this.cmdPasswordApply.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
			this.cmdPasswordApply.AutoSize = true;
			this.cmdPasswordApply.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdPasswordApply.BackColor = System.Drawing.Color.Black;
			this.cmdPasswordApply.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdPasswordApply.ForeColor = System.Drawing.Color.LimeGreen;
			this.cmdPasswordApply.Location = new System.Drawing.Point(5, -31);
			this.cmdPasswordApply.Name = "cmdPasswordApply";
			this.cmdPasswordApply.Size = new System.Drawing.Size(48, 23);
			this.cmdPasswordApply.TabIndex = 4;
			this.cmdPasswordApply.Text = "Apply";
			this.cmdPasswordApply.Click += new System.EventHandler(this.cmdPasswordApply_Click);
			// 
			// txtPasswordConfirmNewPassword
			// 
			this.txtPasswordConfirmNewPassword.BackColor = System.Drawing.SystemColors.ControlDarkDark;
			this.txtPasswordConfirmNewPassword.Dock = System.Windows.Forms.DockStyle.Top;
			this.txtPasswordConfirmNewPassword.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.txtPasswordConfirmNewPassword.ForeColor = System.Drawing.Color.White;
			this.txtPasswordConfirmNewPassword.Location = new System.Drawing.Point(5, 143);
			this.txtPasswordConfirmNewPassword.Name = "txtPasswordConfirmNewPassword";
			this.txtPasswordConfirmNewPassword.PasswordChar = '*';
			this.txtPasswordConfirmNewPassword.Size = new System.Drawing.Size(150, 26);
			this.txtPasswordConfirmNewPassword.TabIndex = 3;
			this.txtPasswordConfirmNewPassword.UseSystemPasswordChar = true;
			this.txtPasswordConfirmNewPassword.KeyUp += new System.Windows.Forms.KeyEventHandler(this.txtPasswordConfirmNewPassword_KeyUp);
			// 
			// dspPasswordConfirmNewPassword
			// 
			this.dspPasswordConfirmNewPassword.AutoColor = false;
			this.dspPasswordConfirmNewPassword.AutoSize = true;
			this.dspPasswordConfirmNewPassword.DefaultColor = System.Drawing.Color.White;
			this.dspPasswordConfirmNewPassword.DefaultText = "User Name :";
			this.dspPasswordConfirmNewPassword.DisplayFactor = 1;
			this.dspPasswordConfirmNewPassword.Dock = System.Windows.Forms.DockStyle.Top;
			this.dspPasswordConfirmNewPassword.DownColor = System.Drawing.Color.Red;
			this.dspPasswordConfirmNewPassword.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspPasswordConfirmNewPassword.ForeColor = System.Drawing.Color.Silver;
			this.dspPasswordConfirmNewPassword.Is64Th = false;
			this.dspPasswordConfirmNewPassword.IsCurrency = false;
			this.dspPasswordConfirmNewPassword.IsFraction = false;
			this.dspPasswordConfirmNewPassword.IsPercent = false;
			this.dspPasswordConfirmNewPassword.Location = new System.Drawing.Point(5, 128);
			this.dspPasswordConfirmNewPassword.MaxDecimal = 7;
			this.dspPasswordConfirmNewPassword.Name = "dspPasswordConfirmNewPassword";
			this.dspPasswordConfirmNewPassword.Padding = new System.Windows.Forms.Padding(1);
			this.dspPasswordConfirmNewPassword.Size = new System.Drawing.Size(142, 15);
			this.dspPasswordConfirmNewPassword.TabIndex = 26;
			this.dspPasswordConfirmNewPassword.Text = "Confirm New Password:";
			this.dspPasswordConfirmNewPassword.TickSize = 0.01;
			this.dspPasswordConfirmNewPassword.UpColor = System.Drawing.Color.LimeGreen;
			this.dspPasswordConfirmNewPassword.Value = "";
			// 
			// txtPasswordNewPassword
			// 
			this.txtPasswordNewPassword.BackColor = System.Drawing.SystemColors.ControlDarkDark;
			this.txtPasswordNewPassword.Dock = System.Windows.Forms.DockStyle.Top;
			this.txtPasswordNewPassword.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.txtPasswordNewPassword.ForeColor = System.Drawing.Color.White;
			this.txtPasswordNewPassword.Location = new System.Drawing.Point(5, 102);
			this.txtPasswordNewPassword.Name = "txtPasswordNewPassword";
			this.txtPasswordNewPassword.PasswordChar = '*';
			this.txtPasswordNewPassword.Size = new System.Drawing.Size(150, 26);
			this.txtPasswordNewPassword.TabIndex = 2;
			this.txtPasswordNewPassword.UseSystemPasswordChar = true;
			this.txtPasswordNewPassword.KeyUp += new System.Windows.Forms.KeyEventHandler(this.txtPasswordNewPassword_KeyUp);
			// 
			// dspPasswordNewPassword
			// 
			this.dspPasswordNewPassword.AutoColor = false;
			this.dspPasswordNewPassword.AutoSize = true;
			this.dspPasswordNewPassword.DefaultColor = System.Drawing.Color.White;
			this.dspPasswordNewPassword.DefaultText = "User Name :";
			this.dspPasswordNewPassword.DisplayFactor = 1;
			this.dspPasswordNewPassword.Dock = System.Windows.Forms.DockStyle.Top;
			this.dspPasswordNewPassword.DownColor = System.Drawing.Color.Red;
			this.dspPasswordNewPassword.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspPasswordNewPassword.ForeColor = System.Drawing.Color.Silver;
			this.dspPasswordNewPassword.Is64Th = false;
			this.dspPasswordNewPassword.IsCurrency = false;
			this.dspPasswordNewPassword.IsFraction = false;
			this.dspPasswordNewPassword.IsPercent = false;
			this.dspPasswordNewPassword.Location = new System.Drawing.Point(5, 87);
			this.dspPasswordNewPassword.MaxDecimal = 7;
			this.dspPasswordNewPassword.Name = "dspPasswordNewPassword";
			this.dspPasswordNewPassword.Padding = new System.Windows.Forms.Padding(1);
			this.dspPasswordNewPassword.Size = new System.Drawing.Size(96, 15);
			this.dspPasswordNewPassword.TabIndex = 24;
			this.dspPasswordNewPassword.Text = "New Password:";
			this.dspPasswordNewPassword.TickSize = 0.01;
			this.dspPasswordNewPassword.UpColor = System.Drawing.Color.LimeGreen;
			this.dspPasswordNewPassword.Value = "";
			// 
			// txtPasswordOldPassword
			// 
			this.txtPasswordOldPassword.BackColor = System.Drawing.SystemColors.ControlDarkDark;
			this.txtPasswordOldPassword.Dock = System.Windows.Forms.DockStyle.Top;
			this.txtPasswordOldPassword.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.txtPasswordOldPassword.ForeColor = System.Drawing.Color.White;
			this.txtPasswordOldPassword.Location = new System.Drawing.Point(5, 61);
			this.txtPasswordOldPassword.Name = "txtPasswordOldPassword";
			this.txtPasswordOldPassword.PasswordChar = '*';
			this.txtPasswordOldPassword.Size = new System.Drawing.Size(150, 26);
			this.txtPasswordOldPassword.TabIndex = 1;
			this.txtPasswordOldPassword.UseSystemPasswordChar = true;
			this.txtPasswordOldPassword.KeyUp += new System.Windows.Forms.KeyEventHandler(this.txtPasswordOldPassword_KeyUp);
			// 
			// dspPasswordOldPassword
			// 
			this.dspPasswordOldPassword.AutoColor = false;
			this.dspPasswordOldPassword.AutoSize = true;
			this.dspPasswordOldPassword.DefaultColor = System.Drawing.Color.White;
			this.dspPasswordOldPassword.DefaultText = "User Name :";
			this.dspPasswordOldPassword.DisplayFactor = 1;
			this.dspPasswordOldPassword.Dock = System.Windows.Forms.DockStyle.Top;
			this.dspPasswordOldPassword.DownColor = System.Drawing.Color.Red;
			this.dspPasswordOldPassword.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspPasswordOldPassword.ForeColor = System.Drawing.Color.Silver;
			this.dspPasswordOldPassword.Is64Th = false;
			this.dspPasswordOldPassword.IsCurrency = false;
			this.dspPasswordOldPassword.IsFraction = false;
			this.dspPasswordOldPassword.IsPercent = false;
			this.dspPasswordOldPassword.Location = new System.Drawing.Point(5, 46);
			this.dspPasswordOldPassword.MaxDecimal = 7;
			this.dspPasswordOldPassword.Name = "dspPasswordOldPassword";
			this.dspPasswordOldPassword.Padding = new System.Windows.Forms.Padding(1);
			this.dspPasswordOldPassword.Size = new System.Drawing.Size(90, 15);
			this.dspPasswordOldPassword.TabIndex = 22;
			this.dspPasswordOldPassword.Text = "Old Password:";
			this.dspPasswordOldPassword.TickSize = 0.01;
			this.dspPasswordOldPassword.UpColor = System.Drawing.Color.LimeGreen;
			this.dspPasswordOldPassword.Value = "";
			// 
			// txtPasswordUserName
			// 
			this.txtPasswordUserName.BackColor = System.Drawing.SystemColors.ControlDarkDark;
			this.txtPasswordUserName.CharacterCasing = System.Windows.Forms.CharacterCasing.Upper;
			this.txtPasswordUserName.Dock = System.Windows.Forms.DockStyle.Top;
			this.txtPasswordUserName.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.txtPasswordUserName.ForeColor = System.Drawing.Color.White;
			this.txtPasswordUserName.Location = new System.Drawing.Point(5, 20);
			this.txtPasswordUserName.Name = "txtPasswordUserName";
			this.txtPasswordUserName.Size = new System.Drawing.Size(150, 26);
			this.txtPasswordUserName.TabIndex = 0;
			this.txtPasswordUserName.KeyUp += new System.Windows.Forms.KeyEventHandler(this.txtPasswordUserName_KeyUp);
			// 
			// dspPasswordUserName
			// 
			this.dspPasswordUserName.AutoColor = false;
			this.dspPasswordUserName.AutoSize = true;
			this.dspPasswordUserName.DefaultColor = System.Drawing.Color.White;
			this.dspPasswordUserName.DefaultText = "User Name :";
			this.dspPasswordUserName.DisplayFactor = 1;
			this.dspPasswordUserName.Dock = System.Windows.Forms.DockStyle.Top;
			this.dspPasswordUserName.DownColor = System.Drawing.Color.Red;
			this.dspPasswordUserName.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspPasswordUserName.ForeColor = System.Drawing.Color.Silver;
			this.dspPasswordUserName.Is64Th = false;
			this.dspPasswordUserName.IsCurrency = false;
			this.dspPasswordUserName.IsFraction = false;
			this.dspPasswordUserName.IsPercent = false;
			this.dspPasswordUserName.Location = new System.Drawing.Point(5, 5);
			this.dspPasswordUserName.MaxDecimal = 7;
			this.dspPasswordUserName.Name = "dspPasswordUserName";
			this.dspPasswordUserName.Padding = new System.Windows.Forms.Padding(1);
			this.dspPasswordUserName.Size = new System.Drawing.Size(75, 15);
			this.dspPasswordUserName.TabIndex = 20;
			this.dspPasswordUserName.Text = "User Name:";
			this.dspPasswordUserName.TickSize = 0.01;
			this.dspPasswordUserName.UpColor = System.Drawing.Color.LimeGreen;
			this.dspPasswordUserName.Value = "";
			// 
			// managedPanelSetting
			// 
			this.managedPanelSetting.Controls.Add(this.pictureBoxLogo);
			this.managedPanelSetting.Controls.Add(this.vistaPanelSetting);
			this.managedPanelSetting.Location = new System.Drawing.Point(0, 0);
			this.managedPanelSetting.Name = "managedPanelSetting";
			this.managedPanelSetting.Size = new System.Drawing.Size(484, 234);
			// 
			// pictureBoxLogo
			// 
			this.pictureBoxLogo.BackColor = System.Drawing.SystemColors.Control;
			this.pictureBoxLogo.Dock = System.Windows.Forms.DockStyle.Fill;
			this.pictureBoxLogo.Image = global::ROC.Properties.Resources.LOGO;
			this.pictureBoxLogo.Location = new System.Drawing.Point(0, 0);
			this.pictureBoxLogo.Name = "pictureBoxLogo";
			this.pictureBoxLogo.Size = new System.Drawing.Size(324, 234);
			this.pictureBoxLogo.SizeMode = System.Windows.Forms.PictureBoxSizeMode.CenterImage;
			this.pictureBoxLogo.TabIndex = 3;
			this.pictureBoxLogo.TabStop = false;
			// 
			// vistaPanelSetting
			// 
			this.vistaPanelSetting.BackColor = System.Drawing.Color.Black;
			this.vistaPanelSetting.Controls.Add(this.chkPlayAllStatusSounds);
			this.vistaPanelSetting.Controls.Add(this.chkUseMarketData);
			this.vistaPanelSetting.Controls.Add(this.chkDelayUIProcess);
			this.vistaPanelSetting.Controls.Add(this.chkLargeScreenSize);
			this.vistaPanelSetting.Controls.Add(this.chkAutoFocusOnControls);
			this.vistaPanelSetting.Controls.Add(this.lblDisplayChoices);
			this.vistaPanelSetting.Controls.Add(this.chkDoNotLoadOPRAParticipant);
			this.vistaPanelSetting.Controls.Add(this.chkShowOnlySelectedExchange);
			this.vistaPanelSetting.Controls.Add(this.lblOptionChoices);
			this.vistaPanelSetting.Controls.Add(this.chkShowInternalStatus);
			this.vistaPanelSetting.Controls.Add(this.chkConfimOnCancelAll);
			this.vistaPanelSetting.Controls.Add(this.chkSkipGTCandGTD);
			this.vistaPanelSetting.Controls.Add(this.chkCancelOnDisconnect);
			this.vistaPanelSetting.Controls.Add(this.lblOrderChoices);
			this.vistaPanelSetting.Controls.Add(this.chkUseROMDatabase);
			this.vistaPanelSetting.Controls.Add(this.chkUseTpos);
			this.vistaPanelSetting.Controls.Add(this.lblROCChoices);
			this.vistaPanelSetting.Dock = System.Windows.Forms.DockStyle.Right;
			this.vistaPanelSetting.Location = new System.Drawing.Point(324, 0);
			this.vistaPanelSetting.Name = "vistaPanelSetting";
			this.vistaPanelSetting.Padding = new System.Windows.Forms.Padding(5, 5, 2, 2);
			this.vistaPanelSetting.Size = new System.Drawing.Size(160, 234);
			this.vistaPanelSetting.TabIndex = 2;
			// 
			// chkPlayAllStatusSounds
			// 
			this.chkPlayAllStatusSounds.AutoSize = true;
			this.chkPlayAllStatusSounds.Dock = System.Windows.Forms.DockStyle.Top;
			this.chkPlayAllStatusSounds.Location = new System.Drawing.Point(5, 285);
			this.chkPlayAllStatusSounds.Name = "chkPlayAllStatusSounds";
			this.chkPlayAllStatusSounds.Size = new System.Drawing.Size(153, 17);
			this.chkPlayAllStatusSounds.TabIndex = 5;
			this.chkPlayAllStatusSounds.Text = "Play All Status Sound";
			this.chkPlayAllStatusSounds.UseVisualStyleBackColor = true;
			this.chkPlayAllStatusSounds.Visible = false;
			this.chkPlayAllStatusSounds.CheckedChanged += new System.EventHandler(this.chkPlayAllStatusSounds_CheckedChanged);
			// 
			// chkUseMarketData
			// 
			this.chkUseMarketData.AutoSize = true;
			this.chkUseMarketData.Dock = System.Windows.Forms.DockStyle.Top;
			this.chkUseMarketData.Location = new System.Drawing.Point(5, 268);
			this.chkUseMarketData.Name = "chkUseMarketData";
			this.chkUseMarketData.Size = new System.Drawing.Size(153, 17);
			this.chkUseMarketData.TabIndex = 0;
			this.chkUseMarketData.Text = "Use Market Data";
			this.chkUseMarketData.UseVisualStyleBackColor = true;
			this.chkUseMarketData.Visible = false;
			this.chkUseMarketData.CheckedChanged += new System.EventHandler(this.chkUseMarketData_CheckedChanged);
			// 
			// chkDelayUIProcess
			// 
			this.chkDelayUIProcess.AutoSize = true;
			this.chkDelayUIProcess.Dock = System.Windows.Forms.DockStyle.Top;
			this.chkDelayUIProcess.Location = new System.Drawing.Point(5, 251);
			this.chkDelayUIProcess.Name = "chkDelayUIProcess";
			this.chkDelayUIProcess.Size = new System.Drawing.Size(153, 17);
			this.chkDelayUIProcess.TabIndex = 9;
			this.chkDelayUIProcess.Text = "Delay UI Process";
			this.chkDelayUIProcess.UseVisualStyleBackColor = true;
			this.chkDelayUIProcess.Visible = false;
			this.chkDelayUIProcess.CheckedChanged += new System.EventHandler(this.chkDelayUIProcess_CheckedChanged);
			// 
			// chkLargeScreenSize
			// 
			this.chkLargeScreenSize.AutoSize = true;
			this.chkLargeScreenSize.Dock = System.Windows.Forms.DockStyle.Top;
			this.chkLargeScreenSize.Location = new System.Drawing.Point(5, 234);
			this.chkLargeScreenSize.Name = "chkLargeScreenSize";
			this.chkLargeScreenSize.Size = new System.Drawing.Size(153, 17);
			this.chkLargeScreenSize.TabIndex = 8;
			this.chkLargeScreenSize.Text = "Large Screen Size";
			this.chkLargeScreenSize.UseVisualStyleBackColor = true;
			this.chkLargeScreenSize.Visible = false;
			this.chkLargeScreenSize.CheckedChanged += new System.EventHandler(this.chkLargeScreenSize_CheckedChanged);
			// 
			// chkAutoFocusOnControls
			// 
			this.chkAutoFocusOnControls.AutoSize = true;
			this.chkAutoFocusOnControls.Dock = System.Windows.Forms.DockStyle.Top;
			this.chkAutoFocusOnControls.Location = new System.Drawing.Point(5, 217);
			this.chkAutoFocusOnControls.Name = "chkAutoFocusOnControls";
			this.chkAutoFocusOnControls.Size = new System.Drawing.Size(153, 17);
			this.chkAutoFocusOnControls.TabIndex = 0;
			this.chkAutoFocusOnControls.Text = "Auto Focus On Controls";
			this.chkAutoFocusOnControls.UseVisualStyleBackColor = true;
			this.chkAutoFocusOnControls.CheckedChanged += new System.EventHandler(this.chkAutoFocusOnControls_CheckedChanged);
			// 
			// lblDisplayChoices
			// 
			this.lblDisplayChoices.AutoColor = false;
			this.lblDisplayChoices.AutoSize = true;
			this.lblDisplayChoices.DefaultColor = System.Drawing.Color.White;
			this.lblDisplayChoices.DefaultText = "";
			this.lblDisplayChoices.DisplayFactor = 1;
			this.lblDisplayChoices.Dock = System.Windows.Forms.DockStyle.Top;
			this.lblDisplayChoices.DownColor = System.Drawing.Color.Red;
			this.lblDisplayChoices.Is64Th = false;
			this.lblDisplayChoices.IsCurrency = false;
			this.lblDisplayChoices.IsFraction = false;
			this.lblDisplayChoices.IsPercent = false;
			this.lblDisplayChoices.Location = new System.Drawing.Point(5, 198);
			this.lblDisplayChoices.MaxDecimal = 7;
			this.lblDisplayChoices.Name = "lblDisplayChoices";
			this.lblDisplayChoices.Padding = new System.Windows.Forms.Padding(3);
			this.lblDisplayChoices.Size = new System.Drawing.Size(88, 19);
			this.lblDisplayChoices.TabIndex = 0;
			this.lblDisplayChoices.Text = "Display Choices";
			this.lblDisplayChoices.TickSize = 0.01;
			this.lblDisplayChoices.UpColor = System.Drawing.Color.LimeGreen;
			this.lblDisplayChoices.Value = "";
			// 
			// chkDoNotLoadOPRAParticipant
			// 
			this.chkDoNotLoadOPRAParticipant.AutoSize = true;
			this.chkDoNotLoadOPRAParticipant.Dock = System.Windows.Forms.DockStyle.Top;
			this.chkDoNotLoadOPRAParticipant.Location = new System.Drawing.Point(5, 181);
			this.chkDoNotLoadOPRAParticipant.Name = "chkDoNotLoadOPRAParticipant";
			this.chkDoNotLoadOPRAParticipant.Size = new System.Drawing.Size(153, 17);
			this.chkDoNotLoadOPRAParticipant.TabIndex = 11;
			this.chkDoNotLoadOPRAParticipant.Text = "Do Not Load OPRA Part.";
			this.chkDoNotLoadOPRAParticipant.UseVisualStyleBackColor = true;
			this.chkDoNotLoadOPRAParticipant.CheckedChanged += new System.EventHandler(this.chkDoNotLoadOPRAParticipant_CheckedChanged);
			// 
			// chkShowOnlySelectedExchange
			// 
			this.chkShowOnlySelectedExchange.AutoSize = true;
			this.chkShowOnlySelectedExchange.Dock = System.Windows.Forms.DockStyle.Top;
			this.chkShowOnlySelectedExchange.Location = new System.Drawing.Point(5, 164);
			this.chkShowOnlySelectedExchange.Name = "chkShowOnlySelectedExchange";
			this.chkShowOnlySelectedExchange.Size = new System.Drawing.Size(153, 17);
			this.chkShowOnlySelectedExchange.TabIndex = 7;
			this.chkShowOnlySelectedExchange.Text = "Show Only Selected Exch.";
			this.chkShowOnlySelectedExchange.UseVisualStyleBackColor = true;
			this.chkShowOnlySelectedExchange.CheckedChanged += new System.EventHandler(this.chkShowOnlySelectedExchange_CheckedChanged);
			// 
			// lblOptionChoices
			// 
			this.lblOptionChoices.AutoColor = false;
			this.lblOptionChoices.AutoSize = true;
			this.lblOptionChoices.DefaultColor = System.Drawing.Color.White;
			this.lblOptionChoices.DefaultText = "";
			this.lblOptionChoices.DisplayFactor = 1;
			this.lblOptionChoices.Dock = System.Windows.Forms.DockStyle.Top;
			this.lblOptionChoices.DownColor = System.Drawing.Color.Red;
			this.lblOptionChoices.Is64Th = false;
			this.lblOptionChoices.IsCurrency = false;
			this.lblOptionChoices.IsFraction = false;
			this.lblOptionChoices.IsPercent = false;
			this.lblOptionChoices.Location = new System.Drawing.Point(5, 145);
			this.lblOptionChoices.MaxDecimal = 7;
			this.lblOptionChoices.Name = "lblOptionChoices";
			this.lblOptionChoices.Padding = new System.Windows.Forms.Padding(3);
			this.lblOptionChoices.Size = new System.Drawing.Size(85, 19);
			this.lblOptionChoices.TabIndex = 0;
			this.lblOptionChoices.Text = "Option Choices";
			this.lblOptionChoices.TickSize = 0.01;
			this.lblOptionChoices.UpColor = System.Drawing.Color.LimeGreen;
			this.lblOptionChoices.Value = "";
			// 
			// chkShowInternalStatus
			// 
			this.chkShowInternalStatus.AutoSize = true;
			this.chkShowInternalStatus.Dock = System.Windows.Forms.DockStyle.Top;
			this.chkShowInternalStatus.Location = new System.Drawing.Point(5, 128);
			this.chkShowInternalStatus.Name = "chkShowInternalStatus";
			this.chkShowInternalStatus.Size = new System.Drawing.Size(153, 17);
			this.chkShowInternalStatus.TabIndex = 6;
			this.chkShowInternalStatus.Text = "Show Internal Status";
			this.chkShowInternalStatus.UseVisualStyleBackColor = true;
			this.chkShowInternalStatus.CheckedChanged += new System.EventHandler(this.chkShowInternalStatus_CheckedChanged);
			// 
			// chkConfimOnCancelAll
			// 
			this.chkConfimOnCancelAll.AutoSize = true;
			this.chkConfimOnCancelAll.Dock = System.Windows.Forms.DockStyle.Top;
			this.chkConfimOnCancelAll.Location = new System.Drawing.Point(5, 111);
			this.chkConfimOnCancelAll.Name = "chkConfimOnCancelAll";
			this.chkConfimOnCancelAll.Size = new System.Drawing.Size(153, 17);
			this.chkConfimOnCancelAll.TabIndex = 4;
			this.chkConfimOnCancelAll.Text = "Confirm On Cancel All";
			this.chkConfimOnCancelAll.UseVisualStyleBackColor = true;
			this.chkConfimOnCancelAll.CheckedChanged += new System.EventHandler(this.chkConfimOnCancelAll_CheckedChanged);
			// 
			// chkSkipGTCandGTD
			// 
			this.chkSkipGTCandGTD.AutoSize = true;
			this.chkSkipGTCandGTD.BackColor = System.Drawing.Color.Transparent;
			this.chkSkipGTCandGTD.Dock = System.Windows.Forms.DockStyle.Top;
			this.chkSkipGTCandGTD.Location = new System.Drawing.Point(5, 94);
			this.chkSkipGTCandGTD.Name = "chkSkipGTCandGTD";
			this.chkSkipGTCandGTD.Padding = new System.Windows.Forms.Padding(18, 0, 0, 0);
			this.chkSkipGTCandGTD.Size = new System.Drawing.Size(153, 17);
			this.chkSkipGTCandGTD.TabIndex = 10;
			this.chkSkipGTCandGTD.Text = "Skip GTC and GTD";
			this.chkSkipGTCandGTD.UseVisualStyleBackColor = false;
			this.chkSkipGTCandGTD.CheckedChanged += new System.EventHandler(this.chkSkipGTCandGTD_CheckedChanged);
			// 
			// chkCancelOnDisconnect
			// 
			this.chkCancelOnDisconnect.AutoSize = true;
			this.chkCancelOnDisconnect.Dock = System.Windows.Forms.DockStyle.Top;
			this.chkCancelOnDisconnect.Location = new System.Drawing.Point(5, 77);
			this.chkCancelOnDisconnect.Name = "chkCancelOnDisconnect";
			this.chkCancelOnDisconnect.Size = new System.Drawing.Size(153, 17);
			this.chkCancelOnDisconnect.TabIndex = 3;
			this.chkCancelOnDisconnect.Text = "Cancel All On Disconnect";
			this.chkCancelOnDisconnect.UseVisualStyleBackColor = true;
			this.chkCancelOnDisconnect.CheckedChanged += new System.EventHandler(this.chkCancelOnDisconnect_CheckedChanged);
			// 
			// lblOrderChoices
			// 
			this.lblOrderChoices.AutoColor = false;
			this.lblOrderChoices.AutoSize = true;
			this.lblOrderChoices.DefaultColor = System.Drawing.Color.White;
			this.lblOrderChoices.DefaultText = "";
			this.lblOrderChoices.DisplayFactor = 1;
			this.lblOrderChoices.Dock = System.Windows.Forms.DockStyle.Top;
			this.lblOrderChoices.DownColor = System.Drawing.Color.Red;
			this.lblOrderChoices.Is64Th = false;
			this.lblOrderChoices.IsCurrency = false;
			this.lblOrderChoices.IsFraction = false;
			this.lblOrderChoices.IsPercent = false;
			this.lblOrderChoices.Location = new System.Drawing.Point(5, 58);
			this.lblOrderChoices.MaxDecimal = 7;
			this.lblOrderChoices.Name = "lblOrderChoices";
			this.lblOrderChoices.Padding = new System.Windows.Forms.Padding(3);
			this.lblOrderChoices.Size = new System.Drawing.Size(80, 19);
			this.lblOrderChoices.TabIndex = 0;
			this.lblOrderChoices.Text = "Order Choices";
			this.lblOrderChoices.TickSize = 0.01;
			this.lblOrderChoices.UpColor = System.Drawing.Color.LimeGreen;
			this.lblOrderChoices.Value = "";
			// 
			// chkUseROMDatabase
			// 
			this.chkUseROMDatabase.AutoSize = true;
			this.chkUseROMDatabase.Dock = System.Windows.Forms.DockStyle.Top;
			this.chkUseROMDatabase.Location = new System.Drawing.Point(5, 41);
			this.chkUseROMDatabase.Name = "chkUseROMDatabase";
			this.chkUseROMDatabase.Size = new System.Drawing.Size(153, 17);
			this.chkUseROMDatabase.TabIndex = 2;
			this.chkUseROMDatabase.Text = "Use Database Orders";
			this.chkUseROMDatabase.UseVisualStyleBackColor = true;
			this.chkUseROMDatabase.CheckedChanged += new System.EventHandler(this.chkUseROMDatabase_CheckedChanged);
			// 
			// chkUseTpos
			// 
			this.chkUseTpos.AutoSize = true;
			this.chkUseTpos.Dock = System.Windows.Forms.DockStyle.Top;
			this.chkUseTpos.Location = new System.Drawing.Point(5, 24);
			this.chkUseTpos.Name = "chkUseTpos";
			this.chkUseTpos.Size = new System.Drawing.Size(153, 17);
			this.chkUseTpos.TabIndex = 1;
			this.chkUseTpos.Text = "Use Positions Trades";
			this.chkUseTpos.UseVisualStyleBackColor = true;
			this.chkUseTpos.CheckedChanged += new System.EventHandler(this.chkUseTpos_CheckedChanged);
			// 
			// lblROCChoices
			// 
			this.lblROCChoices.AutoColor = false;
			this.lblROCChoices.AutoSize = true;
			this.lblROCChoices.DefaultColor = System.Drawing.Color.White;
			this.lblROCChoices.DefaultText = "";
			this.lblROCChoices.DisplayFactor = 1;
			this.lblROCChoices.Dock = System.Windows.Forms.DockStyle.Top;
			this.lblROCChoices.DownColor = System.Drawing.Color.Red;
			this.lblROCChoices.Is64Th = false;
			this.lblROCChoices.IsCurrency = false;
			this.lblROCChoices.IsFraction = false;
			this.lblROCChoices.IsPercent = false;
			this.lblROCChoices.Location = new System.Drawing.Point(5, 5);
			this.lblROCChoices.MaxDecimal = 7;
			this.lblROCChoices.Name = "lblROCChoices";
			this.lblROCChoices.Padding = new System.Windows.Forms.Padding(3);
			this.lblROCChoices.Size = new System.Drawing.Size(77, 19);
			this.lblROCChoices.TabIndex = 0;
			this.lblROCChoices.Text = "ROC Choices";
			this.lblROCChoices.TickSize = 0.01;
			this.lblROCChoices.UpColor = System.Drawing.Color.LimeGreen;
			this.lblROCChoices.Value = "";
			// 
			// vistaPanelCommandStrip
			// 
			this.vistaPanelCommandStrip.BackColor = System.Drawing.Color.Black;
			this.vistaPanelCommandStrip.Controls.Add(this.cmdHome);
			this.vistaPanelCommandStrip.Controls.Add(this.cmdPassword);
			this.vistaPanelCommandStrip.Controls.Add(this.cmdSetting);
			this.vistaPanelCommandStrip.Controls.Add(this.lblLoginStatus);
			this.vistaPanelCommandStrip.Dock = System.Windows.Forms.DockStyle.Top;
			this.vistaPanelCommandStrip.Location = new System.Drawing.Point(0, 26);
			this.vistaPanelCommandStrip.Name = "vistaPanelCommandStrip";
			this.vistaPanelCommandStrip.Padding = new System.Windows.Forms.Padding(2);
			this.vistaPanelCommandStrip.Size = new System.Drawing.Size(484, 25);
			this.vistaPanelCommandStrip.TabIndex = 2;
			// 
			// cmdHome
			// 
			this.cmdHome.AutoSize = true;
			this.cmdHome.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdHome.BackColor = System.Drawing.Color.Black;
			this.cmdHome.Dock = System.Windows.Forms.DockStyle.Right;
			this.cmdHome.Location = new System.Drawing.Point(324, 2);
			this.cmdHome.Name = "cmdHome";
			this.cmdHome.Size = new System.Drawing.Size(45, 21);
			this.cmdHome.TabIndex = 0;
			this.cmdHome.TabStop = false;
			this.cmdHome.Text = "Home";
			this.cmdHome.Click += new System.EventHandler(this.cmdHome_Click);
			// 
			// cmdPassword
			// 
			this.cmdPassword.AutoSize = true;
			this.cmdPassword.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdPassword.BackColor = System.Drawing.Color.Black;
			this.cmdPassword.Dock = System.Windows.Forms.DockStyle.Right;
			this.cmdPassword.Location = new System.Drawing.Point(369, 2);
			this.cmdPassword.Name = "cmdPassword";
			this.cmdPassword.Size = new System.Drawing.Size(63, 21);
			this.cmdPassword.TabIndex = 1;
			this.cmdPassword.TabStop = false;
			this.cmdPassword.Text = "Password";
			this.cmdPassword.Click += new System.EventHandler(this.cmdPassword_Click);
			// 
			// cmdSetting
			// 
			this.cmdSetting.AutoSize = true;
			this.cmdSetting.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdSetting.BackColor = System.Drawing.Color.Black;
			this.cmdSetting.Dock = System.Windows.Forms.DockStyle.Right;
			this.cmdSetting.Location = new System.Drawing.Point(432, 2);
			this.cmdSetting.Name = "cmdSetting";
			this.cmdSetting.Size = new System.Drawing.Size(50, 21);
			this.cmdSetting.TabIndex = 2;
			this.cmdSetting.TabStop = false;
			this.cmdSetting.Text = "Setting";
			this.cmdSetting.Click += new System.EventHandler(this.cmdSetting_Click);
			// 
			// lblLoginStatus
			// 
			this.lblLoginStatus.ApplyShine = true;
			this.lblLoginStatus.AutoColor = false;
			this.lblLoginStatus.AutoEllipsis = true;
			this.lblLoginStatus.DefaultColor = System.Drawing.Color.White;
			this.lblLoginStatus.DefaultText = "";
			this.lblLoginStatus.DisplayFactor = 1;
			this.lblLoginStatus.Dock = System.Windows.Forms.DockStyle.Fill;
			this.lblLoginStatus.DownColor = System.Drawing.Color.Red;
			this.lblLoginStatus.Is64Th = false;
			this.lblLoginStatus.IsCurrency = false;
			this.lblLoginStatus.IsFraction = false;
			this.lblLoginStatus.IsPercent = false;
			this.lblLoginStatus.Location = new System.Drawing.Point(2, 2);
			this.lblLoginStatus.MaxDecimal = 7;
			this.lblLoginStatus.Name = "lblLoginStatus";
			this.lblLoginStatus.Padding = new System.Windows.Forms.Padding(0, 3, 0, 3);
			this.lblLoginStatus.Size = new System.Drawing.Size(480, 21);
			this.lblLoginStatus.TabIndex = 101;
			this.lblLoginStatus.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			this.lblLoginStatus.TickSize = 0.01;
			this.lblLoginStatus.UpColor = System.Drawing.Color.LimeGreen;
			this.lblLoginStatus.Value = "";
			// 
			// lblROMStatus
			// 
			this.lblROMStatus.AutoColor = false;
			this.lblROMStatus.AutoEllipsis = true;
			this.lblROMStatus.BackColor = System.Drawing.SystemColors.ControlDarkDark;
			this.lblROMStatus.DefaultColor = System.Drawing.Color.White;
			this.lblROMStatus.DefaultText = "ROM Not Connected";
			this.lblROMStatus.DisplayFactor = 1;
			this.lblROMStatus.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.lblROMStatus.DownColor = System.Drawing.Color.Red;
			this.lblROMStatus.ForeColor = System.Drawing.Color.WhiteSmoke;
			this.lblROMStatus.Is64Th = false;
			this.lblROMStatus.IsCurrency = false;
			this.lblROMStatus.IsFraction = false;
			this.lblROMStatus.IsPercent = false;
			this.lblROMStatus.Location = new System.Drawing.Point(0, 285);
			this.lblROMStatus.MaxDecimal = 7;
			this.lblROMStatus.Name = "lblROMStatus";
			this.lblROMStatus.Size = new System.Drawing.Size(484, 13);
			this.lblROMStatus.TabIndex = 100;
			this.lblROMStatus.Text = "ROM Not Connected";
			this.lblROMStatus.TickSize = 0.01;
			this.lblROMStatus.UpColor = System.Drawing.Color.LimeGreen;
			this.lblROMStatus.Value = "";
			// 
			// frmLogin
			// 
			this.AnimationType = FormEx.VistaWindowBase.AnimationTypes.Slide;
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.BackColor = System.Drawing.SystemColors.ControlDarkDark;
			this.Caption = "Login";
			this.ClientSize = new System.Drawing.Size(484, 298);
			this.Controls.Add(this.panelManagerLogin);
			this.Controls.Add(this.vistaPanelCommandStrip);
			this.Controls.Add(this.CaptionBar);
			this.Controls.Add(this.lblROMStatus);
			this.ForeColor = System.Drawing.Color.White;
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.Name = "frmLogin";
			this.OnTop = true;
			this.ROCWindowType = FormEx.VistaWindowBase.ROCWindowTypes.Login;
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.Text = "ROC Login";
			this.TopMost = true;
			this.panelManagerLogin.ResumeLayout(false);
			this.managedPanelLogin.ResumeLayout(false);
			this.managedPanelLogin.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.picProcessing)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.picLogo)).EndInit();
			this.vistaPanelLogin.ResumeLayout(false);
			this.vistaPanelLogin.PerformLayout();
			this.managedPanelPassword.ResumeLayout(false);
			this.vistaPanelPassowrd.ResumeLayout(false);
			this.vistaPanelPassowrd.PerformLayout();
			this.managedPanelSetting.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.pictureBoxLogo)).EndInit();
			this.vistaPanelSetting.ResumeLayout(false);
			this.vistaPanelSetting.PerformLayout();
			this.vistaPanelCommandStrip.ResumeLayout(false);
			this.vistaPanelCommandStrip.PerformLayout();
			this.ResumeLayout(false);

		}

		#endregion

		private FormEx.VistaWindowCaptionBar CaptionBar;
		private PanelEx.PanelManager panelManagerLogin;
		private PanelEx.ManagedPanel managedPanelLogin;
		private PanelEx.ManagedPanel managedPanelPassword;
		private PanelEx.ManagedPanel managedPanelSetting;
		private PanelEx.VistaPanel vistaPanelCommandStrip;
		private ButtonEx.VistaButton cmdSetting;
		private ButtonEx.VistaButton cmdPassword;
		private ButtonEx.VistaButton cmdHome;
		private PanelEx.VistaPanel vistaPanelLogin;
		private PanelEx.VistaPanel vistaPanelPassowrd;
		private PanelEx.VistaPanel vistaPanelSetting;
		private ButtonEx.VistaButton cmdCancel;
		private ButtonEx.VistaButton cmdLogin;
		private System.Windows.Forms.TextBox txtUserName;
		private System.Windows.Forms.TextBox txtPassword;
		private LabelEx.LabelBase dspPassword;
		private LabelEx.LabelBase dspUserName;
		private LabelEx.LabelBase lblMDS;
		private LabelEx.LabelBase dspMDS;
		private LabelEx.LabelBase lblRDS;
		private LabelEx.LabelBase dspRDS;
		private LabelEx.LabelBase lblROM;
		private LabelEx.LabelBase dspROM;
		private System.Windows.Forms.PictureBox picLogo;
		private LabelEx.LabelVista lblLoginStatus;
		private LabelEx.LabelBase lblROMStatus;
		private System.Windows.Forms.PictureBox picProcessing;
		private System.Windows.Forms.CheckBox chkUseROMDatabase;
		private System.Windows.Forms.CheckBox chkUseTpos;
		private System.Windows.Forms.CheckBox chkUseMarketData;
		private System.Windows.Forms.PictureBox pictureBoxLogo;
		private System.Windows.Forms.CheckBox chkShowOnlySelectedExchange;
		private LabelEx.LabelVista lblROCChoices;
		private LabelEx.LabelVista lblOptionChoices;
		private System.Windows.Forms.CheckBox chkConfimOnCancelAll;
		private System.Windows.Forms.CheckBox chkPlayAllStatusSounds;
		private LabelEx.LabelVista lblOrderChoices;
		private System.Windows.Forms.TextBox txtPasswordUserName;
		private LabelEx.LabelBase dspPasswordUserName;
		private System.Windows.Forms.TextBox txtPasswordConfirmNewPassword;
		private LabelEx.LabelBase dspPasswordConfirmNewPassword;
		private System.Windows.Forms.TextBox txtPasswordNewPassword;
		private LabelEx.LabelBase dspPasswordNewPassword;
		private System.Windows.Forms.TextBox txtPasswordOldPassword;
		private LabelEx.LabelBase dspPasswordOldPassword;
		private ButtonEx.VistaButton cmdPasswordCancel;
		private ButtonEx.VistaButton cmdPasswordApply;
		private System.Windows.Forms.CheckBox chkCancelOnDisconnect;
		private LabelEx.LabelVista lblDisplayChoices;
		private System.Windows.Forms.CheckBox chkLargeScreenSize;
		private System.Windows.Forms.CheckBox chkShowInternalStatus;
		private System.Windows.Forms.CheckBox chkDelayUIProcess;
		private System.Windows.Forms.CheckBox chkSkipGTCandGTD;
		private System.Windows.Forms.CheckBox chkDoNotLoadOPRAParticipant;
		private System.Windows.Forms.CheckBox chkAutoFocusOnControls;
		private LabelEx.LabelBase lblMDSBackup;
	}
}