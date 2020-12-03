namespace MDS
{
	partial class frmMain
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
			this.components = new System.ComponentModel.Container();
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(frmMain));
			DataGridViewEx.DGVBasePrinter dgvBasePrinter1 = new DataGridViewEx.DGVBasePrinter();
			System.Drawing.StringFormat stringFormat1 = new System.Drawing.StringFormat();
			System.Drawing.StringFormat stringFormat2 = new System.Drawing.StringFormat();
			System.Drawing.StringFormat stringFormat3 = new System.Drawing.StringFormat();
			System.Drawing.StringFormat stringFormat4 = new System.Drawing.StringFormat();
			DataGridViewEx.DGVBasePrinter dgvBasePrinter2 = new DataGridViewEx.DGVBasePrinter();
			System.Drawing.StringFormat stringFormat5 = new System.Drawing.StringFormat();
			System.Drawing.StringFormat stringFormat6 = new System.Drawing.StringFormat();
			System.Drawing.StringFormat stringFormat7 = new System.Drawing.StringFormat();
			System.Drawing.StringFormat stringFormat8 = new System.Drawing.StringFormat();
			this.tabControlMain = new System.Windows.Forms.TabControl();
			this.tabPageMain = new System.Windows.Forms.TabPage();
			this.grpServer = new System.Windows.Forms.GroupBox();
			this.lblMDSSentMessage = new System.Windows.Forms.Label();
			this.lblMDSRecivedMessage = new System.Windows.Forms.Label();
			this.statusStripServer = new System.Windows.Forms.StatusStrip();
			this.dspMDS = new System.Windows.Forms.ToolStripStatusLabel();
			this.lblMDSStatus = new System.Windows.Forms.ToolStripStatusLabel();
			this.lblMDSStatusMessage = new System.Windows.Forms.ToolStripStatusLabel();
			this.cmdServerStop = new System.Windows.Forms.Button();
			this.cmdServerStart = new System.Windows.Forms.Button();
			this.grpWombat = new System.Windows.Forms.GroupBox();
			this.cmdLBMTest = new System.Windows.Forms.Button();
			this.cmdForceGC = new System.Windows.Forms.Button();
			this.statusStripLBM = new System.Windows.Forms.StatusStrip();
			this.dspLBM = new System.Windows.Forms.ToolStripStatusLabel();
			this.dspLBMTotalMsgCount = new System.Windows.Forms.ToolStripStatusLabel();
			this.lblLBMTotalMsgCount = new System.Windows.Forms.ToolStripStatusLabel();
			this.dspLBMPeakMsgRate = new System.Windows.Forms.ToolStripStatusLabel();
			this.lblLBMPeakMsgRate = new System.Windows.Forms.ToolStripStatusLabel();
			this.dspLBMLastMsgRate = new System.Windows.Forms.ToolStripStatusLabel();
			this.lblLBMLastMsgRate = new System.Windows.Forms.ToolStripStatusLabel();
			this.lblLBMStatus = new System.Windows.Forms.ToolStripStatusLabel();
			this.lblLBMStatusMessage = new System.Windows.Forms.ToolStripStatusLabel();
			this.cmdWombatDisconnect = new System.Windows.Forms.Button();
			this.cmdWombatConnect = new System.Windows.Forms.Button();
			this.tabPageSymbols = new System.Windows.Forms.TabPage();
			this.mdsWatchList = new DataGridViewEx.MDSWatchList();
			this.txtSymbolToGet = new System.Windows.Forms.TextBox();
			this.cmdExportSymbols = new System.Windows.Forms.Button();
			this.cmdRefreshSymbols = new System.Windows.Forms.Button();
			this.tabPageUser = new System.Windows.Forms.TabPage();
			this.mdsUserWatchList = new DataGridViewEx.MDSUserWatchList();
			this.cmdRefreshUser = new System.Windows.Forms.Button();
			this.tabPageAbout = new System.Windows.Forms.TabPage();
			this.rtbAbout = new System.Windows.Forms.RichTextBox();
			this.pictureBoxLogo = new System.Windows.Forms.PictureBox();
			this.notifyIconMDS = new System.Windows.Forms.NotifyIcon(this.components);
			this.tabControlMain.SuspendLayout();
			this.tabPageMain.SuspendLayout();
			this.grpServer.SuspendLayout();
			this.statusStripServer.SuspendLayout();
			this.grpWombat.SuspendLayout();
			this.statusStripLBM.SuspendLayout();
			this.tabPageSymbols.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.mdsWatchList)).BeginInit();
			this.tabPageUser.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.mdsUserWatchList)).BeginInit();
			this.tabPageAbout.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.pictureBoxLogo)).BeginInit();
			this.SuspendLayout();
			// 
			// tabControlMain
			// 
			this.tabControlMain.Controls.Add(this.tabPageMain);
			this.tabControlMain.Controls.Add(this.tabPageSymbols);
			this.tabControlMain.Controls.Add(this.tabPageUser);
			this.tabControlMain.Controls.Add(this.tabPageAbout);
			this.tabControlMain.Dock = System.Windows.Forms.DockStyle.Fill;
			this.tabControlMain.Location = new System.Drawing.Point(0, 0);
			this.tabControlMain.Name = "tabControlMain";
			this.tabControlMain.SelectedIndex = 0;
			this.tabControlMain.Size = new System.Drawing.Size(635, 390);
			this.tabControlMain.TabIndex = 2;
			this.tabControlMain.SelectedIndexChanged += new System.EventHandler(this.tabControlMain_SelectedIndexChanged);
			// 
			// tabPageMain
			// 
			this.tabPageMain.Controls.Add(this.grpServer);
			this.tabPageMain.Controls.Add(this.grpWombat);
			this.tabPageMain.Location = new System.Drawing.Point(4, 22);
			this.tabPageMain.Name = "tabPageMain";
			this.tabPageMain.Padding = new System.Windows.Forms.Padding(3);
			this.tabPageMain.Size = new System.Drawing.Size(627, 364);
			this.tabPageMain.TabIndex = 0;
			this.tabPageMain.Text = "Main";
			this.tabPageMain.UseVisualStyleBackColor = true;
			// 
			// grpServer
			// 
			this.grpServer.Controls.Add(this.lblMDSSentMessage);
			this.grpServer.Controls.Add(this.lblMDSRecivedMessage);
			this.grpServer.Controls.Add(this.statusStripServer);
			this.grpServer.Controls.Add(this.cmdServerStop);
			this.grpServer.Controls.Add(this.cmdServerStart);
			this.grpServer.Dock = System.Windows.Forms.DockStyle.Fill;
			this.grpServer.Location = new System.Drawing.Point(3, 156);
			this.grpServer.Name = "grpServer";
			this.grpServer.Size = new System.Drawing.Size(621, 205);
			this.grpServer.TabIndex = 4;
			this.grpServer.TabStop = false;
			this.grpServer.Text = "Server";
			// 
			// lblMDSSentMessage
			// 
			this.lblMDSSentMessage.AutoEllipsis = true;
			this.lblMDSSentMessage.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.lblMDSSentMessage.ForeColor = System.Drawing.Color.SeaGreen;
			this.lblMDSSentMessage.Location = new System.Drawing.Point(3, 154);
			this.lblMDSSentMessage.MinimumSize = new System.Drawing.Size(0, 13);
			this.lblMDSSentMessage.Name = "lblMDSSentMessage";
			this.lblMDSSentMessage.Size = new System.Drawing.Size(615, 13);
			this.lblMDSSentMessage.TabIndex = 6;
			this.lblMDSSentMessage.Text = "Sent:";
			// 
			// lblMDSRecivedMessage
			// 
			this.lblMDSRecivedMessage.AutoEllipsis = true;
			this.lblMDSRecivedMessage.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.lblMDSRecivedMessage.ForeColor = System.Drawing.Color.Blue;
			this.lblMDSRecivedMessage.Location = new System.Drawing.Point(3, 167);
			this.lblMDSRecivedMessage.MinimumSize = new System.Drawing.Size(0, 13);
			this.lblMDSRecivedMessage.Name = "lblMDSRecivedMessage";
			this.lblMDSRecivedMessage.Size = new System.Drawing.Size(615, 13);
			this.lblMDSRecivedMessage.TabIndex = 5;
			this.lblMDSRecivedMessage.Text = "Recived:";
			// 
			// statusStripServer
			// 
			this.statusStripServer.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.dspMDS,
            this.lblMDSStatus,
            this.lblMDSStatusMessage});
			this.statusStripServer.Location = new System.Drawing.Point(3, 180);
			this.statusStripServer.Name = "statusStripServer";
			this.statusStripServer.Size = new System.Drawing.Size(615, 22);
			this.statusStripServer.SizingGrip = false;
			this.statusStripServer.Stretch = false;
			this.statusStripServer.TabIndex = 4;
			this.statusStripServer.Text = "Server";
			// 
			// dspMDS
			// 
			this.dspMDS.Name = "dspMDS";
			this.dspMDS.Size = new System.Drawing.Size(28, 17);
			this.dspMDS.Text = "MDS";
			// 
			// lblMDSStatus
			// 
			this.lblMDSStatus.ForeColor = System.Drawing.Color.Red;
			this.lblMDSStatus.Name = "lblMDSStatus";
			this.lblMDSStatus.Size = new System.Drawing.Size(47, 17);
			this.lblMDSStatus.Text = "Stopped";
			// 
			// lblMDSStatusMessage
			// 
			this.lblMDSStatusMessage.ForeColor = System.Drawing.Color.WhiteSmoke;
			this.lblMDSStatusMessage.Name = "lblMDSStatusMessage";
			this.lblMDSStatusMessage.Size = new System.Drawing.Size(83, 17);
			this.lblMDSStatusMessage.Text = "Status Message";
			// 
			// cmdServerStop
			// 
			this.cmdServerStop.Dock = System.Windows.Forms.DockStyle.Top;
			this.cmdServerStop.Enabled = false;
			this.cmdServerStop.Location = new System.Drawing.Point(3, 39);
			this.cmdServerStop.Name = "cmdServerStop";
			this.cmdServerStop.Size = new System.Drawing.Size(615, 23);
			this.cmdServerStop.TabIndex = 3;
			this.cmdServerStop.Text = "Stop";
			this.cmdServerStop.UseVisualStyleBackColor = true;
			this.cmdServerStop.Click += new System.EventHandler(this.cmdStop_Click);
			// 
			// cmdServerStart
			// 
			this.cmdServerStart.Dock = System.Windows.Forms.DockStyle.Top;
			this.cmdServerStart.Location = new System.Drawing.Point(3, 16);
			this.cmdServerStart.Name = "cmdServerStart";
			this.cmdServerStart.Size = new System.Drawing.Size(615, 23);
			this.cmdServerStart.TabIndex = 2;
			this.cmdServerStart.Text = "Start";
			this.cmdServerStart.UseVisualStyleBackColor = true;
			this.cmdServerStart.Click += new System.EventHandler(this.cmdStart_Click);
			// 
			// grpWombat
			// 
			this.grpWombat.Controls.Add(this.cmdLBMTest);
			this.grpWombat.Controls.Add(this.cmdForceGC);
			this.grpWombat.Controls.Add(this.statusStripLBM);
			this.grpWombat.Controls.Add(this.cmdWombatDisconnect);
			this.grpWombat.Controls.Add(this.cmdWombatConnect);
			this.grpWombat.Dock = System.Windows.Forms.DockStyle.Top;
			this.grpWombat.Location = new System.Drawing.Point(3, 3);
			this.grpWombat.Name = "grpWombat";
			this.grpWombat.Size = new System.Drawing.Size(621, 153);
			this.grpWombat.TabIndex = 2;
			this.grpWombat.TabStop = false;
			this.grpWombat.Text = "Wombat";
			// 
			// cmdLBMTest
			// 
			this.cmdLBMTest.Dock = System.Windows.Forms.DockStyle.Top;
			this.cmdLBMTest.Location = new System.Drawing.Point(3, 85);
			this.cmdLBMTest.Name = "cmdLBMTest";
			this.cmdLBMTest.Size = new System.Drawing.Size(615, 23);
			this.cmdLBMTest.TabIndex = 3;
			this.cmdLBMTest.Text = "Test";
			this.cmdLBMTest.UseVisualStyleBackColor = true;
			this.cmdLBMTest.Click += new System.EventHandler(this.cmdLBMTest_Click);
			// 
			// cmdForceGC
			// 
			this.cmdForceGC.Dock = System.Windows.Forms.DockStyle.Top;
			this.cmdForceGC.Location = new System.Drawing.Point(3, 62);
			this.cmdForceGC.Name = "cmdForceGC";
			this.cmdForceGC.Size = new System.Drawing.Size(615, 23);
			this.cmdForceGC.TabIndex = 5;
			this.cmdForceGC.Text = "Force GC";
			this.cmdForceGC.UseVisualStyleBackColor = true;
			this.cmdForceGC.Click += new System.EventHandler(this.cmdForceGC_Click);
			// 
			// statusStripLBM
			// 
			this.statusStripLBM.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.dspLBM,
            this.dspLBMTotalMsgCount,
            this.lblLBMTotalMsgCount,
            this.dspLBMPeakMsgRate,
            this.lblLBMPeakMsgRate,
            this.dspLBMLastMsgRate,
            this.lblLBMLastMsgRate,
            this.lblLBMStatus,
            this.lblLBMStatusMessage});
			this.statusStripLBM.Location = new System.Drawing.Point(3, 128);
			this.statusStripLBM.Name = "statusStripLBM";
			this.statusStripLBM.Size = new System.Drawing.Size(615, 22);
			this.statusStripLBM.SizingGrip = false;
			this.statusStripLBM.Stretch = false;
			this.statusStripLBM.TabIndex = 4;
			this.statusStripLBM.Text = "LBM";
			// 
			// dspLBM
			// 
			this.dspLBM.ForeColor = System.Drawing.Color.RoyalBlue;
			this.dspLBM.Name = "dspLBM";
			this.dspLBM.Size = new System.Drawing.Size(26, 17);
			this.dspLBM.Text = "LBM";
			// 
			// dspLBMTotalMsgCount
			// 
			this.dspLBMTotalMsgCount.ForeColor = System.Drawing.Color.Goldenrod;
			this.dspLBMTotalMsgCount.Name = "dspLBMTotalMsgCount";
			this.dspLBMTotalMsgCount.Size = new System.Drawing.Size(31, 17);
			this.dspLBMTotalMsgCount.Text = "Total";
			// 
			// lblLBMTotalMsgCount
			// 
			this.lblLBMTotalMsgCount.ForeColor = System.Drawing.Color.Goldenrod;
			this.lblLBMTotalMsgCount.Name = "lblLBMTotalMsgCount";
			this.lblLBMTotalMsgCount.Size = new System.Drawing.Size(13, 17);
			this.lblLBMTotalMsgCount.Text = "0";
			this.lblLBMTotalMsgCount.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// dspLBMPeakMsgRate
			// 
			this.dspLBMPeakMsgRate.ForeColor = System.Drawing.Color.Green;
			this.dspLBMPeakMsgRate.Name = "dspLBMPeakMsgRate";
			this.dspLBMPeakMsgRate.Size = new System.Drawing.Size(58, 17);
			this.dspLBMPeakMsgRate.Text = "Peak m/ms";
			// 
			// lblLBMPeakMsgRate
			// 
			this.lblLBMPeakMsgRate.ForeColor = System.Drawing.Color.Green;
			this.lblLBMPeakMsgRate.Name = "lblLBMPeakMsgRate";
			this.lblLBMPeakMsgRate.Size = new System.Drawing.Size(13, 17);
			this.lblLBMPeakMsgRate.Text = "0";
			this.lblLBMPeakMsgRate.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// dspLBMLastMsgRate
			// 
			this.dspLBMLastMsgRate.Name = "dspLBMLastMsgRate";
			this.dspLBMLastMsgRate.Size = new System.Drawing.Size(55, 17);
			this.dspLBMLastMsgRate.Text = "Last m/ms";
			// 
			// lblLBMLastMsgRate
			// 
			this.lblLBMLastMsgRate.Name = "lblLBMLastMsgRate";
			this.lblLBMLastMsgRate.Size = new System.Drawing.Size(13, 17);
			this.lblLBMLastMsgRate.Text = "0";
			this.lblLBMLastMsgRate.ToolTipText = "Last 100ms Rate";
			// 
			// lblLBMStatus
			// 
			this.lblLBMStatus.ForeColor = System.Drawing.Color.Red;
			this.lblLBMStatus.Name = "lblLBMStatus";
			this.lblLBMStatus.Size = new System.Drawing.Size(79, 17);
			this.lblLBMStatus.Text = "Not Connected";
			// 
			// lblLBMStatusMessage
			// 
			this.lblLBMStatusMessage.ForeColor = System.Drawing.Color.WhiteSmoke;
			this.lblLBMStatusMessage.Name = "lblLBMStatusMessage";
			this.lblLBMStatusMessage.Size = new System.Drawing.Size(83, 17);
			this.lblLBMStatusMessage.Text = "Status Message";
			// 
			// cmdWombatDisconnect
			// 
			this.cmdWombatDisconnect.Dock = System.Windows.Forms.DockStyle.Top;
			this.cmdWombatDisconnect.Location = new System.Drawing.Point(3, 39);
			this.cmdWombatDisconnect.Name = "cmdWombatDisconnect";
			this.cmdWombatDisconnect.Size = new System.Drawing.Size(615, 23);
			this.cmdWombatDisconnect.TabIndex = 3;
			this.cmdWombatDisconnect.Text = "Disconnect";
			this.cmdWombatDisconnect.UseVisualStyleBackColor = true;
			this.cmdWombatDisconnect.Click += new System.EventHandler(this.cmdWombatDisconnect_Click);
			// 
			// cmdWombatConnect
			// 
			this.cmdWombatConnect.Dock = System.Windows.Forms.DockStyle.Top;
			this.cmdWombatConnect.Location = new System.Drawing.Point(3, 16);
			this.cmdWombatConnect.Name = "cmdWombatConnect";
			this.cmdWombatConnect.Size = new System.Drawing.Size(615, 23);
			this.cmdWombatConnect.TabIndex = 2;
			this.cmdWombatConnect.Text = "Connect";
			this.cmdWombatConnect.UseVisualStyleBackColor = true;
			this.cmdWombatConnect.Click += new System.EventHandler(this.cmdWombatConnect_Click);
			// 
			// tabPageSymbols
			// 
			this.tabPageSymbols.Controls.Add(this.mdsWatchList);
			this.tabPageSymbols.Controls.Add(this.txtSymbolToGet);
			this.tabPageSymbols.Controls.Add(this.cmdExportSymbols);
			this.tabPageSymbols.Controls.Add(this.cmdRefreshSymbols);
			this.tabPageSymbols.Location = new System.Drawing.Point(4, 22);
			this.tabPageSymbols.Name = "tabPageSymbols";
			this.tabPageSymbols.Padding = new System.Windows.Forms.Padding(3);
			this.tabPageSymbols.Size = new System.Drawing.Size(627, 364);
			this.tabPageSymbols.TabIndex = 1;
			this.tabPageSymbols.Text = "Symbols";
			this.tabPageSymbols.UseVisualStyleBackColor = true;
			// 
			// mdsWatchList
			// 
			this.mdsWatchList.BlueDefault = 228;
			this.mdsWatchList.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
			new BindingListEx.ROCBindingList<string>().Add("");
			this.mdsWatchList.DisplayFactors = ((System.Collections.Generic.Dictionary<string, double>)(resources.GetObject("mdsWatchList.DisplayFactors")));
			this.mdsWatchList.Dock = System.Windows.Forms.DockStyle.Fill;
			this.mdsWatchList.GreenDefault = 243;
			this.mdsWatchList.GridKeys = ((System.Collections.Generic.List<string>)(resources.GetObject("mdsWatchList.GridKeys")));
			this.mdsWatchList.HeaderBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(233)))), ((int)(((byte)(243)))), ((int)(((byte)(228)))));
			this.mdsWatchList.Importing = false;
			this.mdsWatchList.LastFilter = "";
			this.mdsWatchList.LastSort = "Symbol";
			this.mdsWatchList.Location = new System.Drawing.Point(3, 3);
			this.mdsWatchList.Name = "mdsWatchList";
			dgvBasePrinter1.BlackWhite = true;
			dgvBasePrinter1.CellAlignment = System.Drawing.StringAlignment.Near;
			dgvBasePrinter1.CellFormatFlags = ((System.Drawing.StringFormatFlags)((System.Drawing.StringFormatFlags.LineLimit | System.Drawing.StringFormatFlags.NoClip)));
			dgvBasePrinter1.DocName = null;
			dgvBasePrinter1.Footer = null;
			dgvBasePrinter1.FooterAlignment = System.Drawing.StringAlignment.Center;
			dgvBasePrinter1.FooterColor = System.Drawing.Color.Black;
			dgvBasePrinter1.FooterFont = new System.Drawing.Font("Tahoma", 10F, System.Drawing.FontStyle.Bold);
			stringFormat1.Alignment = System.Drawing.StringAlignment.Center;
			stringFormat1.FormatFlags = ((System.Drawing.StringFormatFlags)(((System.Drawing.StringFormatFlags.NoWrap | System.Drawing.StringFormatFlags.LineLimit)
						| System.Drawing.StringFormatFlags.NoClip)));
			stringFormat1.HotkeyPrefix = System.Drawing.Text.HotkeyPrefix.None;
			stringFormat1.LineAlignment = System.Drawing.StringAlignment.Center;
			stringFormat1.Trimming = System.Drawing.StringTrimming.Word;
			dgvBasePrinter1.FooterFormat = stringFormat1;
			dgvBasePrinter1.FooterFormatFlags = ((System.Drawing.StringFormatFlags)(((System.Drawing.StringFormatFlags.NoWrap | System.Drawing.StringFormatFlags.LineLimit)
						| System.Drawing.StringFormatFlags.NoClip)));
			dgvBasePrinter1.FooterSpacing = 0F;
			dgvBasePrinter1.HeaderCellAlignment = System.Drawing.StringAlignment.Near;
			dgvBasePrinter1.HeaderCellFormatFlags = ((System.Drawing.StringFormatFlags)((System.Drawing.StringFormatFlags.LineLimit | System.Drawing.StringFormatFlags.NoClip)));
			dgvBasePrinter1.Owner = null;
			dgvBasePrinter1.PageNumberAlignment = System.Drawing.StringAlignment.Center;
			dgvBasePrinter1.PageNumberColor = System.Drawing.Color.Black;
			dgvBasePrinter1.PageNumberFont = new System.Drawing.Font("Tahoma", 8F);
			stringFormat2.Alignment = System.Drawing.StringAlignment.Center;
			stringFormat2.FormatFlags = ((System.Drawing.StringFormatFlags)(((System.Drawing.StringFormatFlags.NoWrap | System.Drawing.StringFormatFlags.LineLimit)
						| System.Drawing.StringFormatFlags.NoClip)));
			stringFormat2.HotkeyPrefix = System.Drawing.Text.HotkeyPrefix.None;
			stringFormat2.LineAlignment = System.Drawing.StringAlignment.Center;
			stringFormat2.Trimming = System.Drawing.StringTrimming.Word;
			dgvBasePrinter1.PageNumberFormat = stringFormat2;
			dgvBasePrinter1.PageNumberFormatFlags = ((System.Drawing.StringFormatFlags)(((System.Drawing.StringFormatFlags.NoWrap | System.Drawing.StringFormatFlags.LineLimit)
						| System.Drawing.StringFormatFlags.NoClip)));
			dgvBasePrinter1.PageNumberInHeader = false;
			dgvBasePrinter1.PageNumberOnSeparateLine = true;
			dgvBasePrinter1.PageNumbers = true;
			dgvBasePrinter1.PageSeparator = " of ";
			dgvBasePrinter1.PageText = "Page ";
			dgvBasePrinter1.PartText = " - Part ";
			dgvBasePrinter1.PorportionalColumns = false;
			dgvBasePrinter1.PreviewDialogIcon = null;
			dgvBasePrinter1.PrinterName = null;
			dgvBasePrinter1.PrintMargins = new System.Drawing.Printing.Margins(60, 60, 40, 40);
			dgvBasePrinter1.PrintPreviewZoom = 1;
			dgvBasePrinter1.ShowTotalPageNumber = true;
			dgvBasePrinter1.SubTitle = null;
			dgvBasePrinter1.SubTitleAlignment = System.Drawing.StringAlignment.Center;
			dgvBasePrinter1.SubTitleColor = System.Drawing.Color.Black;
			dgvBasePrinter1.SubTitleFont = new System.Drawing.Font("Tahoma", 12F, System.Drawing.FontStyle.Bold);
			stringFormat3.Alignment = System.Drawing.StringAlignment.Center;
			stringFormat3.FormatFlags = ((System.Drawing.StringFormatFlags)(((System.Drawing.StringFormatFlags.NoWrap | System.Drawing.StringFormatFlags.LineLimit)
						| System.Drawing.StringFormatFlags.NoClip)));
			stringFormat3.HotkeyPrefix = System.Drawing.Text.HotkeyPrefix.None;
			stringFormat3.LineAlignment = System.Drawing.StringAlignment.Center;
			stringFormat3.Trimming = System.Drawing.StringTrimming.Word;
			dgvBasePrinter1.SubTitleFormat = stringFormat3;
			dgvBasePrinter1.SubTitleFormatFlags = ((System.Drawing.StringFormatFlags)(((System.Drawing.StringFormatFlags.NoWrap | System.Drawing.StringFormatFlags.LineLimit)
						| System.Drawing.StringFormatFlags.NoClip)));
			dgvBasePrinter1.TableAlignment = DataGridViewEx.DGVBasePrinter.Alignment.NotSet;
			dgvBasePrinter1.Title = null;
			dgvBasePrinter1.TitleAlignment = System.Drawing.StringAlignment.Center;
			dgvBasePrinter1.TitleColor = System.Drawing.Color.Black;
			dgvBasePrinter1.TitleFont = new System.Drawing.Font("Tahoma", 18F, System.Drawing.FontStyle.Bold);
			stringFormat4.Alignment = System.Drawing.StringAlignment.Center;
			stringFormat4.FormatFlags = ((System.Drawing.StringFormatFlags)(((System.Drawing.StringFormatFlags.NoWrap | System.Drawing.StringFormatFlags.LineLimit)
						| System.Drawing.StringFormatFlags.NoClip)));
			stringFormat4.HotkeyPrefix = System.Drawing.Text.HotkeyPrefix.None;
			stringFormat4.LineAlignment = System.Drawing.StringAlignment.Center;
			stringFormat4.Trimming = System.Drawing.StringTrimming.Word;
			dgvBasePrinter1.TitleFormat = stringFormat4;
			dgvBasePrinter1.TitleFormatFlags = ((System.Drawing.StringFormatFlags)(((System.Drawing.StringFormatFlags.NoWrap | System.Drawing.StringFormatFlags.LineLimit)
						| System.Drawing.StringFormatFlags.NoClip)));
			this.mdsWatchList.Printer = dgvBasePrinter1;
			this.mdsWatchList.Processing = false;
			this.mdsWatchList.RedDefault = 233;
			this.mdsWatchList.RefreshAggragation = true;
			this.mdsWatchList.RefreshSharedRows = false;
			this.mdsWatchList.ResubscribeBook = ((long)(0));
			this.mdsWatchList.ResubscribeBookSymbol = "";
			this.mdsWatchList.ResubscribeIssueSymbol = "";
			this.mdsWatchList.ResubscribeSource = "";
			this.mdsWatchList.ShouldScrollToLastRow = false;
			this.mdsWatchList.Size = new System.Drawing.Size(621, 286);
			new BindingListEx.ROCBindingList<string>().Add("");
			this.mdsWatchList.TabIndex = 2;
			this.mdsWatchList.TickSizes = ((System.Collections.Generic.Dictionary<string, double>)(resources.GetObject("mdsWatchList.TickSizes")));
			// 
			// txtSymbolToGet
			// 
			this.txtSymbolToGet.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.txtSymbolToGet.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.txtSymbolToGet.Location = new System.Drawing.Point(3, 289);
			this.txtSymbolToGet.Name = "txtSymbolToGet";
			this.txtSymbolToGet.Size = new System.Drawing.Size(621, 20);
			this.txtSymbolToGet.TabIndex = 3;
			this.txtSymbolToGet.KeyDown += new System.Windows.Forms.KeyEventHandler(this.txtSymbolToGet_KeyDown);
			// 
			// cmdExportSymbols
			// 
			this.cmdExportSymbols.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.cmdExportSymbols.Location = new System.Drawing.Point(3, 309);
			this.cmdExportSymbols.Name = "cmdExportSymbols";
			this.cmdExportSymbols.Size = new System.Drawing.Size(621, 26);
			this.cmdExportSymbols.TabIndex = 4;
			this.cmdExportSymbols.Text = "Export Symbols";
			this.cmdExportSymbols.UseVisualStyleBackColor = true;
			this.cmdExportSymbols.Click += new System.EventHandler(this.cmdExportSymbols_Click);
			// 
			// cmdRefreshSymbols
			// 
			this.cmdRefreshSymbols.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.cmdRefreshSymbols.Location = new System.Drawing.Point(3, 335);
			this.cmdRefreshSymbols.Name = "cmdRefreshSymbols";
			this.cmdRefreshSymbols.Size = new System.Drawing.Size(621, 26);
			this.cmdRefreshSymbols.TabIndex = 1;
			this.cmdRefreshSymbols.Text = "Refresh Symbols";
			this.cmdRefreshSymbols.UseVisualStyleBackColor = true;
			this.cmdRefreshSymbols.Click += new System.EventHandler(this.cmdRefreshSymbols_Click);
			// 
			// tabPageUser
			// 
			this.tabPageUser.Controls.Add(this.mdsUserWatchList);
			this.tabPageUser.Controls.Add(this.cmdRefreshUser);
			this.tabPageUser.Location = new System.Drawing.Point(4, 22);
			this.tabPageUser.Name = "tabPageUser";
			this.tabPageUser.Padding = new System.Windows.Forms.Padding(3);
			this.tabPageUser.Size = new System.Drawing.Size(627, 364);
			this.tabPageUser.TabIndex = 2;
			this.tabPageUser.Text = "User";
			this.tabPageUser.UseVisualStyleBackColor = true;
			// 
			// mdsUserWatchList
			// 
			this.mdsUserWatchList.BlueDefault = 228;
			this.mdsUserWatchList.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
			new BindingListEx.ROCBindingList<string>().Add("");
			this.mdsUserWatchList.DisplayFactors = ((System.Collections.Generic.Dictionary<string, double>)(resources.GetObject("mdsUserWatchList.DisplayFactors")));
			this.mdsUserWatchList.Dock = System.Windows.Forms.DockStyle.Fill;
			this.mdsUserWatchList.GreenDefault = 243;
			this.mdsUserWatchList.GridKeys = ((System.Collections.Generic.List<string>)(resources.GetObject("mdsUserWatchList.GridKeys")));
			this.mdsUserWatchList.HeaderBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(233)))), ((int)(((byte)(243)))), ((int)(((byte)(228)))));
			this.mdsUserWatchList.Importing = false;
			this.mdsUserWatchList.LastFilter = "";
			this.mdsUserWatchList.LastSort = "Username";
			this.mdsUserWatchList.Location = new System.Drawing.Point(3, 3);
			this.mdsUserWatchList.Name = "mdsUserWatchList";
			dgvBasePrinter2.BlackWhite = true;
			dgvBasePrinter2.CellAlignment = System.Drawing.StringAlignment.Near;
			dgvBasePrinter2.CellFormatFlags = ((System.Drawing.StringFormatFlags)((System.Drawing.StringFormatFlags.LineLimit | System.Drawing.StringFormatFlags.NoClip)));
			dgvBasePrinter2.DocName = null;
			dgvBasePrinter2.Footer = null;
			dgvBasePrinter2.FooterAlignment = System.Drawing.StringAlignment.Center;
			dgvBasePrinter2.FooterColor = System.Drawing.Color.Black;
			dgvBasePrinter2.FooterFont = new System.Drawing.Font("Tahoma", 10F, System.Drawing.FontStyle.Bold);
			stringFormat5.Alignment = System.Drawing.StringAlignment.Center;
			stringFormat5.FormatFlags = ((System.Drawing.StringFormatFlags)(((System.Drawing.StringFormatFlags.NoWrap | System.Drawing.StringFormatFlags.LineLimit)
						| System.Drawing.StringFormatFlags.NoClip)));
			stringFormat5.HotkeyPrefix = System.Drawing.Text.HotkeyPrefix.None;
			stringFormat5.LineAlignment = System.Drawing.StringAlignment.Center;
			stringFormat5.Trimming = System.Drawing.StringTrimming.Word;
			dgvBasePrinter2.FooterFormat = stringFormat5;
			dgvBasePrinter2.FooterFormatFlags = ((System.Drawing.StringFormatFlags)(((System.Drawing.StringFormatFlags.NoWrap | System.Drawing.StringFormatFlags.LineLimit)
						| System.Drawing.StringFormatFlags.NoClip)));
			dgvBasePrinter2.FooterSpacing = 0F;
			dgvBasePrinter2.HeaderCellAlignment = System.Drawing.StringAlignment.Near;
			dgvBasePrinter2.HeaderCellFormatFlags = ((System.Drawing.StringFormatFlags)((System.Drawing.StringFormatFlags.LineLimit | System.Drawing.StringFormatFlags.NoClip)));
			dgvBasePrinter2.Owner = null;
			dgvBasePrinter2.PageNumberAlignment = System.Drawing.StringAlignment.Center;
			dgvBasePrinter2.PageNumberColor = System.Drawing.Color.Black;
			dgvBasePrinter2.PageNumberFont = new System.Drawing.Font("Tahoma", 8F);
			stringFormat6.Alignment = System.Drawing.StringAlignment.Center;
			stringFormat6.FormatFlags = ((System.Drawing.StringFormatFlags)(((System.Drawing.StringFormatFlags.NoWrap | System.Drawing.StringFormatFlags.LineLimit)
						| System.Drawing.StringFormatFlags.NoClip)));
			stringFormat6.HotkeyPrefix = System.Drawing.Text.HotkeyPrefix.None;
			stringFormat6.LineAlignment = System.Drawing.StringAlignment.Center;
			stringFormat6.Trimming = System.Drawing.StringTrimming.Word;
			dgvBasePrinter2.PageNumberFormat = stringFormat6;
			dgvBasePrinter2.PageNumberFormatFlags = ((System.Drawing.StringFormatFlags)(((System.Drawing.StringFormatFlags.NoWrap | System.Drawing.StringFormatFlags.LineLimit)
						| System.Drawing.StringFormatFlags.NoClip)));
			dgvBasePrinter2.PageNumberInHeader = false;
			dgvBasePrinter2.PageNumberOnSeparateLine = true;
			dgvBasePrinter2.PageNumbers = true;
			dgvBasePrinter2.PageSeparator = " of ";
			dgvBasePrinter2.PageText = "Page ";
			dgvBasePrinter2.PartText = " - Part ";
			dgvBasePrinter2.PorportionalColumns = false;
			dgvBasePrinter2.PreviewDialogIcon = null;
			dgvBasePrinter2.PrinterName = null;
			dgvBasePrinter2.PrintMargins = new System.Drawing.Printing.Margins(60, 60, 40, 40);
			dgvBasePrinter2.PrintPreviewZoom = 1;
			dgvBasePrinter2.ShowTotalPageNumber = true;
			dgvBasePrinter2.SubTitle = null;
			dgvBasePrinter2.SubTitleAlignment = System.Drawing.StringAlignment.Center;
			dgvBasePrinter2.SubTitleColor = System.Drawing.Color.Black;
			dgvBasePrinter2.SubTitleFont = new System.Drawing.Font("Tahoma", 12F, System.Drawing.FontStyle.Bold);
			stringFormat7.Alignment = System.Drawing.StringAlignment.Center;
			stringFormat7.FormatFlags = ((System.Drawing.StringFormatFlags)(((System.Drawing.StringFormatFlags.NoWrap | System.Drawing.StringFormatFlags.LineLimit)
						| System.Drawing.StringFormatFlags.NoClip)));
			stringFormat7.HotkeyPrefix = System.Drawing.Text.HotkeyPrefix.None;
			stringFormat7.LineAlignment = System.Drawing.StringAlignment.Center;
			stringFormat7.Trimming = System.Drawing.StringTrimming.Word;
			dgvBasePrinter2.SubTitleFormat = stringFormat7;
			dgvBasePrinter2.SubTitleFormatFlags = ((System.Drawing.StringFormatFlags)(((System.Drawing.StringFormatFlags.NoWrap | System.Drawing.StringFormatFlags.LineLimit)
						| System.Drawing.StringFormatFlags.NoClip)));
			dgvBasePrinter2.TableAlignment = DataGridViewEx.DGVBasePrinter.Alignment.NotSet;
			dgvBasePrinter2.Title = null;
			dgvBasePrinter2.TitleAlignment = System.Drawing.StringAlignment.Center;
			dgvBasePrinter2.TitleColor = System.Drawing.Color.Black;
			dgvBasePrinter2.TitleFont = new System.Drawing.Font("Tahoma", 18F, System.Drawing.FontStyle.Bold);
			stringFormat8.Alignment = System.Drawing.StringAlignment.Center;
			stringFormat8.FormatFlags = ((System.Drawing.StringFormatFlags)(((System.Drawing.StringFormatFlags.NoWrap | System.Drawing.StringFormatFlags.LineLimit)
						| System.Drawing.StringFormatFlags.NoClip)));
			stringFormat8.HotkeyPrefix = System.Drawing.Text.HotkeyPrefix.None;
			stringFormat8.LineAlignment = System.Drawing.StringAlignment.Center;
			stringFormat8.Trimming = System.Drawing.StringTrimming.Word;
			dgvBasePrinter2.TitleFormat = stringFormat8;
			dgvBasePrinter2.TitleFormatFlags = ((System.Drawing.StringFormatFlags)(((System.Drawing.StringFormatFlags.NoWrap | System.Drawing.StringFormatFlags.LineLimit)
						| System.Drawing.StringFormatFlags.NoClip)));
			this.mdsUserWatchList.Printer = dgvBasePrinter2;
			this.mdsUserWatchList.Processing = false;
			this.mdsUserWatchList.RedDefault = 233;
			this.mdsUserWatchList.RefreshAggragation = true;
			this.mdsUserWatchList.RefreshSharedRows = false;
			this.mdsUserWatchList.ShouldScrollToLastRow = false;
			this.mdsUserWatchList.Size = new System.Drawing.Size(621, 332);
			new BindingListEx.ROCBindingList<string>().Add("");
			this.mdsUserWatchList.TabIndex = 3;
			this.mdsUserWatchList.TickSizes = ((System.Collections.Generic.Dictionary<string, double>)(resources.GetObject("mdsUserWatchList.TickSizes")));
			// 
			// cmdRefreshUser
			// 
			this.cmdRefreshUser.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.cmdRefreshUser.Location = new System.Drawing.Point(3, 335);
			this.cmdRefreshUser.Name = "cmdRefreshUser";
			this.cmdRefreshUser.Size = new System.Drawing.Size(621, 26);
			this.cmdRefreshUser.TabIndex = 2;
			this.cmdRefreshUser.Text = "Refresh User";
			this.cmdRefreshUser.UseVisualStyleBackColor = true;
			this.cmdRefreshUser.Click += new System.EventHandler(this.cmdRefreshUser_Click);
			// 
			// tabPageAbout
			// 
			this.tabPageAbout.Controls.Add(this.rtbAbout);
			this.tabPageAbout.Controls.Add(this.pictureBoxLogo);
			this.tabPageAbout.Location = new System.Drawing.Point(4, 22);
			this.tabPageAbout.Name = "tabPageAbout";
			this.tabPageAbout.Padding = new System.Windows.Forms.Padding(3);
			this.tabPageAbout.Size = new System.Drawing.Size(627, 364);
			this.tabPageAbout.TabIndex = 4;
			this.tabPageAbout.Text = "About";
			this.tabPageAbout.UseVisualStyleBackColor = true;
			// 
			// rtbAbout
			// 
			this.rtbAbout.BackColor = System.Drawing.SystemColors.ActiveCaptionText;
			this.rtbAbout.BorderStyle = System.Windows.Forms.BorderStyle.None;
			this.rtbAbout.Dock = System.Windows.Forms.DockStyle.Fill;
			this.rtbAbout.Location = new System.Drawing.Point(324, 3);
			this.rtbAbout.Name = "rtbAbout";
			this.rtbAbout.ReadOnly = true;
			this.rtbAbout.Size = new System.Drawing.Size(300, 358);
			this.rtbAbout.TabIndex = 0;
			this.rtbAbout.Text = "";
			this.rtbAbout.WordWrap = false;
			// 
			// pictureBoxLogo
			// 
			this.pictureBoxLogo.BackgroundImage = global::MDS.Properties.Resources.web_rgb300;
			this.pictureBoxLogo.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Center;
			this.pictureBoxLogo.Dock = System.Windows.Forms.DockStyle.Left;
			this.pictureBoxLogo.Location = new System.Drawing.Point(3, 3);
			this.pictureBoxLogo.MaximumSize = new System.Drawing.Size(321, 358);
			this.pictureBoxLogo.Name = "pictureBoxLogo";
			this.pictureBoxLogo.Size = new System.Drawing.Size(321, 358);
			this.pictureBoxLogo.TabIndex = 1;
			this.pictureBoxLogo.TabStop = false;
			// 
			// notifyIconMDS
			// 
			this.notifyIconMDS.Icon = ((System.Drawing.Icon)(resources.GetObject("notifyIconMDS.Icon")));
			this.notifyIconMDS.Text = "MDS";
			this.notifyIconMDS.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.notifyIconMDS_MouseDoubleClick);
			// 
			// frmMain
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(635, 390);
			this.Controls.Add(this.tabControlMain);
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.Name = "frmMain";
			this.Text = "Market Data Server";
			this.Resize += new System.EventHandler(this.frmMain_Resize);
			this.tabControlMain.ResumeLayout(false);
			this.tabPageMain.ResumeLayout(false);
			this.grpServer.ResumeLayout(false);
			this.grpServer.PerformLayout();
			this.statusStripServer.ResumeLayout(false);
			this.statusStripServer.PerformLayout();
			this.grpWombat.ResumeLayout(false);
			this.grpWombat.PerformLayout();
			this.statusStripLBM.ResumeLayout(false);
			this.statusStripLBM.PerformLayout();
			this.tabPageSymbols.ResumeLayout(false);
			this.tabPageSymbols.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.mdsWatchList)).EndInit();
			this.tabPageUser.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.mdsUserWatchList)).EndInit();
			this.tabPageAbout.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.pictureBoxLogo)).EndInit();
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.TabControl tabControlMain;
		private System.Windows.Forms.TabPage tabPageMain;
		private System.Windows.Forms.TabPage tabPageSymbols;
		private System.Windows.Forms.GroupBox grpWombat;
		private System.Windows.Forms.Button cmdWombatDisconnect;
		private System.Windows.Forms.Button cmdWombatConnect;
		private System.Windows.Forms.Button cmdLBMTest;
		private System.Windows.Forms.GroupBox grpServer;
		private System.Windows.Forms.Button cmdServerStop;
		private System.Windows.Forms.Button cmdServerStart;
		private System.Windows.Forms.StatusStrip statusStripLBM;
		private System.Windows.Forms.ToolStripStatusLabel dspLBM;
		private System.Windows.Forms.ToolStripStatusLabel dspLBMTotalMsgCount;
		private System.Windows.Forms.ToolStripStatusLabel lblLBMTotalMsgCount;
		private System.Windows.Forms.ToolStripStatusLabel dspLBMPeakMsgRate;
		private System.Windows.Forms.ToolStripStatusLabel lblLBMPeakMsgRate;
		private System.Windows.Forms.ToolStripStatusLabel dspLBMLastMsgRate;
		private System.Windows.Forms.ToolStripStatusLabel lblLBMLastMsgRate;
		private System.Windows.Forms.ToolStripStatusLabel lblLBMStatus;
		private System.Windows.Forms.ToolStripStatusLabel lblLBMStatusMessage;
		private System.Windows.Forms.StatusStrip statusStripServer;
		private System.Windows.Forms.ToolStripStatusLabel dspMDS;
		private System.Windows.Forms.ToolStripStatusLabel lblMDSStatus;
		private System.Windows.Forms.ToolStripStatusLabel lblMDSStatusMessage;
		private System.Windows.Forms.Button cmdForceGC;
		private System.Windows.Forms.NotifyIcon notifyIconMDS;
		private System.Windows.Forms.Label lblMDSSentMessage;
		private System.Windows.Forms.Label lblMDSRecivedMessage;
		private System.Windows.Forms.Button cmdRefreshSymbols;
		private System.Windows.Forms.TabPage tabPageUser;
		private DataGridViewEx.MDSWatchList mdsWatchList;
		private System.Windows.Forms.Button cmdRefreshUser;
		private DataGridViewEx.MDSUserWatchList mdsUserWatchList;
		private System.Windows.Forms.TextBox txtSymbolToGet;
		private System.Windows.Forms.TabPage tabPageAbout;
		private System.Windows.Forms.RichTextBox rtbAbout;
		private System.Windows.Forms.PictureBox pictureBoxLogo;
		private System.Windows.Forms.Button cmdExportSymbols;
	}
}

