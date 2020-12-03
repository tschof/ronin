namespace ROC
{
	partial class menuMain
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

		#region Component Designer generated code

		/// <summary> 
		/// Required method for Designer support - do not modify 
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.cmdAbout = new ButtonEx.VistaButton();
			this.cmdAlerts = new ButtonEx.VistaButton();
			this.cmdIM = new ButtonEx.VistaButton();
			this.cmdUserProfile = new ButtonEx.VistaButton();
			this.cmdMDS = new ButtonEx.VistaButton();
			this.cmdRDS = new ButtonEx.VistaButton();
			this.cmdROM = new ButtonEx.VistaButton();
			this.vistaPanelStatus = new PanelEx.VistaPanel();
			this.dspROCMenu = new LabelEx.LabelRotatable();
			this.lblMDSStatus = new LabelEx.LabelVista();
			this.lblRDSStatus = new LabelEx.LabelVista();
			this.lblROMStatus = new LabelEx.LabelVista();
			this.cmdZeroAllWindows = new ButtonEx.VistaButton();
			this.cmdSettings = new ButtonEx.VistaButton();
			this.cmdClock = new ButtonEx.VistaButton();
			this.vistaPanelStatus.SuspendLayout();
			this.SuspendLayout();
			// 
			// cmdAbout
			// 
			this.cmdAbout.AutoSize = true;
			this.cmdAbout.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdAbout.BackColor = System.Drawing.Color.Black;
			this.cmdAbout.Dock = System.Windows.Forms.DockStyle.Top;
			this.cmdAbout.Location = new System.Drawing.Point(23, 208);
			this.cmdAbout.Name = "cmdAbout";
			this.cmdAbout.Size = new System.Drawing.Size(56, 23);
			this.cmdAbout.TabIndex = 7;
			this.cmdAbout.Text = "About";
			this.cmdAbout.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			this.cmdAbout.Click += new System.EventHandler(this.cmdAbout_Click);
			// 
			// cmdAlerts
			// 
			this.cmdAlerts.AutoSize = true;
			this.cmdAlerts.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdAlerts.BackColor = System.Drawing.Color.Black;
			this.cmdAlerts.Dock = System.Windows.Forms.DockStyle.Top;
			this.cmdAlerts.Location = new System.Drawing.Point(23, 116);
			this.cmdAlerts.Name = "cmdAlerts";
			this.cmdAlerts.Size = new System.Drawing.Size(56, 23);
			this.cmdAlerts.TabIndex = 6;
			this.cmdAlerts.Text = "Alerts";
			this.cmdAlerts.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			this.cmdAlerts.Click += new System.EventHandler(this.cmdAlerts_Click);
			// 
			// cmdIM
			// 
			this.cmdIM.AutoSize = true;
			this.cmdIM.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdIM.BackColor = System.Drawing.Color.Black;
			this.cmdIM.Dock = System.Windows.Forms.DockStyle.Top;
			this.cmdIM.Location = new System.Drawing.Point(23, 93);
			this.cmdIM.Name = "cmdIM";
			this.cmdIM.Size = new System.Drawing.Size(56, 23);
			this.cmdIM.TabIndex = 5;
			this.cmdIM.Text = "IM";
			this.cmdIM.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			this.cmdIM.Click += new System.EventHandler(this.cmdIM_Click);
			// 
			// cmdUserProfile
			// 
			this.cmdUserProfile.AutoSize = true;
			this.cmdUserProfile.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdUserProfile.BackColor = System.Drawing.Color.Black;
			this.cmdUserProfile.Dock = System.Windows.Forms.DockStyle.Top;
			this.cmdUserProfile.Location = new System.Drawing.Point(23, 70);
			this.cmdUserProfile.Name = "cmdUserProfile";
			this.cmdUserProfile.Size = new System.Drawing.Size(56, 23);
			this.cmdUserProfile.TabIndex = 4;
			this.cmdUserProfile.Text = "Profiles";
			this.cmdUserProfile.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			this.cmdUserProfile.Click += new System.EventHandler(this.cmdUserProfile_Click);
			// 
			// cmdMDS
			// 
			this.cmdMDS.AutoSize = true;
			this.cmdMDS.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdMDS.BackColor = System.Drawing.Color.Black;
			this.cmdMDS.Dock = System.Windows.Forms.DockStyle.Top;
			this.cmdMDS.Enabled = false;
			this.cmdMDS.Location = new System.Drawing.Point(23, 47);
			this.cmdMDS.Name = "cmdMDS";
			this.cmdMDS.Size = new System.Drawing.Size(56, 23);
			this.cmdMDS.TabIndex = 3;
			this.cmdMDS.Text = "MDS";
			this.cmdMDS.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			this.cmdMDS.Click += new System.EventHandler(this.cmdMDS_Click);
			// 
			// cmdRDS
			// 
			this.cmdRDS.AutoSize = true;
			this.cmdRDS.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdRDS.BackColor = System.Drawing.Color.Black;
			this.cmdRDS.Dock = System.Windows.Forms.DockStyle.Top;
			this.cmdRDS.Location = new System.Drawing.Point(23, 24);
			this.cmdRDS.Name = "cmdRDS";
			this.cmdRDS.Size = new System.Drawing.Size(56, 23);
			this.cmdRDS.TabIndex = 2;
			this.cmdRDS.Text = "RDS";
			this.cmdRDS.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			this.cmdRDS.Click += new System.EventHandler(this.cmdRDS_Click);
			// 
			// cmdROM
			// 
			this.cmdROM.AutoSize = true;
			this.cmdROM.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdROM.BackColor = System.Drawing.Color.Black;
			this.cmdROM.Dock = System.Windows.Forms.DockStyle.Top;
			this.cmdROM.Location = new System.Drawing.Point(23, 1);
			this.cmdROM.Name = "cmdROM";
			this.cmdROM.Size = new System.Drawing.Size(56, 23);
			this.cmdROM.TabIndex = 1;
			this.cmdROM.Text = "ROM";
			this.cmdROM.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			this.cmdROM.Click += new System.EventHandler(this.cmdROM_Click);
			// 
			// vistaPanelStatus
			// 
			this.vistaPanelStatus.BackColor = System.Drawing.Color.Black;
			this.vistaPanelStatus.Controls.Add(this.dspROCMenu);
			this.vistaPanelStatus.Controls.Add(this.lblMDSStatus);
			this.vistaPanelStatus.Controls.Add(this.lblRDSStatus);
			this.vistaPanelStatus.Controls.Add(this.lblROMStatus);
			this.vistaPanelStatus.Dock = System.Windows.Forms.DockStyle.Left;
			this.vistaPanelStatus.Location = new System.Drawing.Point(1, 1);
			this.vistaPanelStatus.Name = "vistaPanelStatus";
			this.vistaPanelStatus.Padding = new System.Windows.Forms.Padding(5, 0, 5, 5);
			this.vistaPanelStatus.Size = new System.Drawing.Size(22, 231);
			this.vistaPanelStatus.TabIndex = 0;
			// 
			// dspROCMenu
			// 
			this.dspROCMenu.AutoColor = false;
			this.dspROCMenu.DefaultColor = System.Drawing.Color.White;
			this.dspROCMenu.DefaultText = "";
			this.dspROCMenu.DisplayFactor = 1;
			this.dspROCMenu.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.dspROCMenu.DownColor = System.Drawing.Color.Red;
			this.dspROCMenu.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspROCMenu.Is64Th = false;
			this.dspROCMenu.IsCurrency = false;
			this.dspROCMenu.IsFraction = false;
			this.dspROCMenu.IsPercent = false;
			this.dspROCMenu.Location = new System.Drawing.Point(5, 161);
			this.dspROCMenu.MaxDecimal = 7;
			this.dspROCMenu.Name = "dspROCMenu";
			this.dspROCMenu.RotationAngle = 270;
			this.dspROCMenu.Size = new System.Drawing.Size(12, 65);
			this.dspROCMenu.TabIndex = 3;
			this.dspROCMenu.Text = "ROC Menu";
			this.dspROCMenu.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			this.dspROCMenu.TextDirection = LabelEx.LabelRotatable.Direction.Clockwise;
			this.dspROCMenu.TextOrientation = LabelEx.LabelRotatable.Orientation.Rotate;
			this.dspROCMenu.TickSize = 0.01;
			this.dspROCMenu.UpColor = System.Drawing.Color.LimeGreen;
			this.dspROCMenu.Value = "";
			// 
			// lblMDSStatus
			// 
			this.lblMDSStatus.AutoColor = false;
			this.lblMDSStatus.DefaultColor = System.Drawing.Color.White;
			this.lblMDSStatus.DefaultText = "";
			this.lblMDSStatus.DisplayFactor = 1;
			this.lblMDSStatus.DownColor = System.Drawing.Color.Red;
			this.lblMDSStatus.Is64Th = false;
			this.lblMDSStatus.IsCurrency = false;
			this.lblMDSStatus.IsFraction = false;
			this.lblMDSStatus.IsPercent = false;
			this.lblMDSStatus.Location = new System.Drawing.Point(4, 48);
			this.lblMDSStatus.MaxDecimal = 7;
			this.lblMDSStatus.Name = "lblMDSStatus";
			this.lblMDSStatus.Size = new System.Drawing.Size(14, 18);
			this.lblMDSStatus.TabIndex = 2;
			this.lblMDSStatus.TickSize = 0.01;
			this.lblMDSStatus.UpColor = System.Drawing.Color.LimeGreen;
			this.lblMDSStatus.Value = "";
			// 
			// lblRDSStatus
			// 
			this.lblRDSStatus.AutoColor = false;
			this.lblRDSStatus.DefaultColor = System.Drawing.Color.White;
			this.lblRDSStatus.DefaultText = "";
			this.lblRDSStatus.DisplayFactor = 1;
			this.lblRDSStatus.DownColor = System.Drawing.Color.Red;
			this.lblRDSStatus.Is64Th = false;
			this.lblRDSStatus.IsCurrency = false;
			this.lblRDSStatus.IsFraction = false;
			this.lblRDSStatus.IsPercent = false;
			this.lblRDSStatus.Location = new System.Drawing.Point(4, 26);
			this.lblRDSStatus.MaxDecimal = 7;
			this.lblRDSStatus.Name = "lblRDSStatus";
			this.lblRDSStatus.Size = new System.Drawing.Size(14, 18);
			this.lblRDSStatus.TabIndex = 1;
			this.lblRDSStatus.TickSize = 0.01;
			this.lblRDSStatus.UpColor = System.Drawing.Color.LimeGreen;
			this.lblRDSStatus.Value = "";
			// 
			// lblROMStatus
			// 
			this.lblROMStatus.AutoColor = false;
			this.lblROMStatus.DefaultColor = System.Drawing.Color.White;
			this.lblROMStatus.DefaultText = "";
			this.lblROMStatus.DisplayFactor = 1;
			this.lblROMStatus.DownColor = System.Drawing.Color.Red;
			this.lblROMStatus.Is64Th = false;
			this.lblROMStatus.IsCurrency = false;
			this.lblROMStatus.IsFraction = false;
			this.lblROMStatus.IsPercent = false;
			this.lblROMStatus.Location = new System.Drawing.Point(4, 5);
			this.lblROMStatus.MaxDecimal = 7;
			this.lblROMStatus.Name = "lblROMStatus";
			this.lblROMStatus.Size = new System.Drawing.Size(14, 18);
			this.lblROMStatus.TabIndex = 0;
			this.lblROMStatus.TickSize = 0.01;
			this.lblROMStatus.UpColor = System.Drawing.Color.LimeGreen;
			this.lblROMStatus.Value = "";
			// 
			// cmdZeroAllWindows
			// 
			this.cmdZeroAllWindows.AutoSize = true;
			this.cmdZeroAllWindows.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdZeroAllWindows.BackColor = System.Drawing.Color.Black;
			this.cmdZeroAllWindows.Dock = System.Windows.Forms.DockStyle.Top;
			this.cmdZeroAllWindows.Location = new System.Drawing.Point(23, 185);
			this.cmdZeroAllWindows.Name = "cmdZeroAllWindows";
			this.cmdZeroAllWindows.Size = new System.Drawing.Size(56, 23);
			this.cmdZeroAllWindows.TabIndex = 9;
			this.cmdZeroAllWindows.Text = "Zero All";
			this.cmdZeroAllWindows.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			this.cmdZeroAllWindows.Click += new System.EventHandler(this.cmdZeroAllWindows_Click);
			// 
			// cmdSettings
			// 
			this.cmdSettings.AutoSize = true;
			this.cmdSettings.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdSettings.BackColor = System.Drawing.Color.Black;
			this.cmdSettings.Dock = System.Windows.Forms.DockStyle.Top;
			this.cmdSettings.Location = new System.Drawing.Point(23, 162);
			this.cmdSettings.Name = "cmdSettings";
			this.cmdSettings.Size = new System.Drawing.Size(56, 23);
			this.cmdSettings.TabIndex = 11;
			this.cmdSettings.Text = "Settings";
			this.cmdSettings.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			this.cmdSettings.Click += new System.EventHandler(this.cmdSettings_Click);
			// 
			// cmdClock
			// 
			this.cmdClock.AutoSize = true;
			this.cmdClock.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdClock.BackColor = System.Drawing.Color.Black;
			this.cmdClock.Dock = System.Windows.Forms.DockStyle.Top;
			this.cmdClock.Location = new System.Drawing.Point(23, 139);
			this.cmdClock.Name = "cmdClock";
			this.cmdClock.Size = new System.Drawing.Size(56, 23);
			this.cmdClock.TabIndex = 12;
			this.cmdClock.Text = "Clock";
			this.cmdClock.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			this.cmdClock.Click += new System.EventHandler(this.cmdClock_Click);
			// 
			// menuMain
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.BackColor = System.Drawing.SystemColors.Control;
			this.Controls.Add(this.cmdAbout);
			this.Controls.Add(this.cmdZeroAllWindows);
			this.Controls.Add(this.cmdSettings);
			this.Controls.Add(this.cmdClock);
			this.Controls.Add(this.cmdAlerts);
			this.Controls.Add(this.cmdIM);
			this.Controls.Add(this.cmdUserProfile);
			this.Controls.Add(this.cmdMDS);
			this.Controls.Add(this.cmdRDS);
			this.Controls.Add(this.cmdROM);
			this.Controls.Add(this.vistaPanelStatus);
			this.Name = "menuMain";
			this.Padding = new System.Windows.Forms.Padding(1);
			this.Size = new System.Drawing.Size(80, 233);
			this.vistaPanelStatus.ResumeLayout(false);
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private PanelEx.VistaPanel vistaPanelStatus;
		private LabelEx.LabelVista lblROMStatus;
		private ButtonEx.VistaButton cmdROM;
		private ButtonEx.VistaButton cmdRDS;
		private ButtonEx.VistaButton cmdMDS;
		private LabelEx.LabelVista lblMDSStatus;
		private LabelEx.LabelVista lblRDSStatus;
		private ButtonEx.VistaButton cmdUserProfile;
		private ButtonEx.VistaButton cmdIM;
		private ButtonEx.VistaButton cmdAlerts;
		private ButtonEx.VistaButton cmdAbout;
		private LabelEx.LabelRotatable dspROCMenu;
		private ButtonEx.VistaButton cmdZeroAllWindows;
		private ButtonEx.VistaButton cmdSettings;
		private ButtonEx.VistaButton cmdClock;

	}
}
