namespace APISRLabMDS
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
			this.grpSRLab = new System.Windows.Forms.GroupBox();
			this.cmdSRLabMDS = new System.Windows.Forms.Button();
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
			this.cmdSRLabDisconnect = new System.Windows.Forms.Button();
			this.cmdSRLabConnect = new System.Windows.Forms.Button();
			this.cmdCheckEnvironments = new System.Windows.Forms.Button();
			this.tabPageSymbols = new System.Windows.Forms.TabPage();
			this.txtSymbolToGet = new System.Windows.Forms.TextBox();
			this.cmdRefreshSymbol = new System.Windows.Forms.Button();
			this.tabPageUser = new System.Windows.Forms.TabPage();
			this.cmdRefreshUser = new System.Windows.Forms.Button();
			this.tabPageAbout = new System.Windows.Forms.TabPage();
			this.rtbAbout = new System.Windows.Forms.RichTextBox();
			this.picLogo = new System.Windows.Forms.PictureBox();
			this.tabControlMain.SuspendLayout();
			this.tabPageMain.SuspendLayout();
			this.grpServer.SuspendLayout();
			this.statusStripServer.SuspendLayout();
			this.grpSRLab.SuspendLayout();
			this.statusStripLBM.SuspendLayout();
			this.tabPageSymbols.SuspendLayout();
			this.tabPageUser.SuspendLayout();
			this.tabPageAbout.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.picLogo)).BeginInit();
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
			this.tabControlMain.Size = new System.Drawing.Size(575, 324);
			this.tabControlMain.TabIndex = 4;
			// 
			// tabPageMain
			// 
			this.tabPageMain.Controls.Add(this.grpServer);
			this.tabPageMain.Controls.Add(this.grpSRLab);
			this.tabPageMain.Location = new System.Drawing.Point(4, 22);
			this.tabPageMain.Name = "tabPageMain";
			this.tabPageMain.Padding = new System.Windows.Forms.Padding(3);
			this.tabPageMain.Size = new System.Drawing.Size(567, 298);
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
			this.grpServer.Location = new System.Drawing.Point(3, 161);
			this.grpServer.Name = "grpServer";
			this.grpServer.Size = new System.Drawing.Size(561, 134);
			this.grpServer.TabIndex = 4;
			this.grpServer.TabStop = false;
			this.grpServer.Text = "Server";
			// 
			// lblMDSSentMessage
			// 
			this.lblMDSSentMessage.AutoEllipsis = true;
			this.lblMDSSentMessage.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.lblMDSSentMessage.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.lblMDSSentMessage.ForeColor = System.Drawing.Color.SeaGreen;
			this.lblMDSSentMessage.Location = new System.Drawing.Point(3, 61);
			this.lblMDSSentMessage.MinimumSize = new System.Drawing.Size(0, 13);
			this.lblMDSSentMessage.Name = "lblMDSSentMessage";
			this.lblMDSSentMessage.Padding = new System.Windows.Forms.Padding(2);
			this.lblMDSSentMessage.Size = new System.Drawing.Size(555, 24);
			this.lblMDSSentMessage.TabIndex = 6;
			this.lblMDSSentMessage.Text = "Sent:";
			this.lblMDSSentMessage.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			// 
			// lblMDSRecivedMessage
			// 
			this.lblMDSRecivedMessage.AutoEllipsis = true;
			this.lblMDSRecivedMessage.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.lblMDSRecivedMessage.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.lblMDSRecivedMessage.ForeColor = System.Drawing.Color.Blue;
			this.lblMDSRecivedMessage.Location = new System.Drawing.Point(3, 85);
			this.lblMDSRecivedMessage.MinimumSize = new System.Drawing.Size(0, 13);
			this.lblMDSRecivedMessage.Name = "lblMDSRecivedMessage";
			this.lblMDSRecivedMessage.Size = new System.Drawing.Size(555, 24);
			this.lblMDSRecivedMessage.TabIndex = 5;
			this.lblMDSRecivedMessage.Text = "Recived:";
			this.lblMDSRecivedMessage.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			// 
			// statusStripServer
			// 
			this.statusStripServer.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.dspMDS,
            this.lblMDSStatus,
            this.lblMDSStatusMessage});
			this.statusStripServer.Location = new System.Drawing.Point(3, 109);
			this.statusStripServer.Name = "statusStripServer";
			this.statusStripServer.Size = new System.Drawing.Size(555, 22);
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
			this.cmdServerStop.Size = new System.Drawing.Size(555, 23);
			this.cmdServerStop.TabIndex = 3;
			this.cmdServerStop.Text = "Stop";
			this.cmdServerStop.UseVisualStyleBackColor = true;
			// 
			// cmdServerStart
			// 
			this.cmdServerStart.Dock = System.Windows.Forms.DockStyle.Top;
			this.cmdServerStart.Location = new System.Drawing.Point(3, 16);
			this.cmdServerStart.Name = "cmdServerStart";
			this.cmdServerStart.Size = new System.Drawing.Size(555, 23);
			this.cmdServerStart.TabIndex = 2;
			this.cmdServerStart.Text = "Start";
			this.cmdServerStart.UseVisualStyleBackColor = true;
			// 
			// grpSRLab
			// 
			this.grpSRLab.Controls.Add(this.cmdSRLabMDS);
			this.grpSRLab.Controls.Add(this.cmdForceGC);
			this.grpSRLab.Controls.Add(this.statusStripLBM);
			this.grpSRLab.Controls.Add(this.cmdSRLabDisconnect);
			this.grpSRLab.Controls.Add(this.cmdSRLabConnect);
			this.grpSRLab.Controls.Add(this.cmdCheckEnvironments);
			this.grpSRLab.Dock = System.Windows.Forms.DockStyle.Top;
			this.grpSRLab.Location = new System.Drawing.Point(3, 3);
			this.grpSRLab.Name = "grpSRLab";
			this.grpSRLab.Size = new System.Drawing.Size(561, 158);
			this.grpSRLab.TabIndex = 2;
			this.grpSRLab.TabStop = false;
			this.grpSRLab.Text = "SRLab";
			// 
			// cmdSRLabMDS
			// 
			this.cmdSRLabMDS.Dock = System.Windows.Forms.DockStyle.Top;
			this.cmdSRLabMDS.Location = new System.Drawing.Point(3, 108);
			this.cmdSRLabMDS.Name = "cmdSRLabMDS";
			this.cmdSRLabMDS.Size = new System.Drawing.Size(555, 23);
			this.cmdSRLabMDS.TabIndex = 3;
			this.cmdSRLabMDS.Text = "Test";
			this.cmdSRLabMDS.UseVisualStyleBackColor = true;
			// 
			// cmdForceGC
			// 
			this.cmdForceGC.Dock = System.Windows.Forms.DockStyle.Top;
			this.cmdForceGC.Location = new System.Drawing.Point(3, 85);
			this.cmdForceGC.Name = "cmdForceGC";
			this.cmdForceGC.Size = new System.Drawing.Size(555, 23);
			this.cmdForceGC.TabIndex = 5;
			this.cmdForceGC.Text = "Force GC";
			this.cmdForceGC.UseVisualStyleBackColor = true;
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
			this.statusStripLBM.Location = new System.Drawing.Point(3, 133);
			this.statusStripLBM.Name = "statusStripLBM";
			this.statusStripLBM.Size = new System.Drawing.Size(555, 22);
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
			// cmdSRLabDisconnect
			// 
			this.cmdSRLabDisconnect.Dock = System.Windows.Forms.DockStyle.Top;
			this.cmdSRLabDisconnect.Location = new System.Drawing.Point(3, 62);
			this.cmdSRLabDisconnect.Name = "cmdSRLabDisconnect";
			this.cmdSRLabDisconnect.Size = new System.Drawing.Size(555, 23);
			this.cmdSRLabDisconnect.TabIndex = 3;
			this.cmdSRLabDisconnect.Text = "Disconnect";
			this.cmdSRLabDisconnect.UseVisualStyleBackColor = true;
			// 
			// cmdSRLabConnect
			// 
			this.cmdSRLabConnect.Dock = System.Windows.Forms.DockStyle.Top;
			this.cmdSRLabConnect.Location = new System.Drawing.Point(3, 39);
			this.cmdSRLabConnect.Name = "cmdSRLabConnect";
			this.cmdSRLabConnect.Size = new System.Drawing.Size(555, 23);
			this.cmdSRLabConnect.TabIndex = 2;
			this.cmdSRLabConnect.Text = "Connect";
			this.cmdSRLabConnect.UseVisualStyleBackColor = true;
			// 
			// cmdCheckEnvironments
			// 
			this.cmdCheckEnvironments.Dock = System.Windows.Forms.DockStyle.Top;
			this.cmdCheckEnvironments.Location = new System.Drawing.Point(3, 16);
			this.cmdCheckEnvironments.Name = "cmdCheckEnvironments";
			this.cmdCheckEnvironments.Size = new System.Drawing.Size(555, 23);
			this.cmdCheckEnvironments.TabIndex = 6;
			this.cmdCheckEnvironments.Text = "Check Environments";
			this.cmdCheckEnvironments.UseVisualStyleBackColor = true;
			this.cmdCheckEnvironments.Click += new System.EventHandler(this.cmdCheckEnvironments_Click);
			// 
			// tabPageSymbols
			// 
			this.tabPageSymbols.Controls.Add(this.txtSymbolToGet);
			this.tabPageSymbols.Controls.Add(this.cmdRefreshSymbol);
			this.tabPageSymbols.Location = new System.Drawing.Point(4, 22);
			this.tabPageSymbols.Name = "tabPageSymbols";
			this.tabPageSymbols.Padding = new System.Windows.Forms.Padding(3);
			this.tabPageSymbols.Size = new System.Drawing.Size(611, 494);
			this.tabPageSymbols.TabIndex = 1;
			this.tabPageSymbols.Text = "Symbols";
			this.tabPageSymbols.UseVisualStyleBackColor = true;
			// 
			// txtSymbolToGet
			// 
			this.txtSymbolToGet.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.txtSymbolToGet.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.txtSymbolToGet.Location = new System.Drawing.Point(3, 445);
			this.txtSymbolToGet.Name = "txtSymbolToGet";
			this.txtSymbolToGet.Size = new System.Drawing.Size(605, 20);
			this.txtSymbolToGet.TabIndex = 3;
			// 
			// cmdRefreshSymbol
			// 
			this.cmdRefreshSymbol.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.cmdRefreshSymbol.Location = new System.Drawing.Point(3, 465);
			this.cmdRefreshSymbol.Name = "cmdRefreshSymbol";
			this.cmdRefreshSymbol.Size = new System.Drawing.Size(605, 26);
			this.cmdRefreshSymbol.TabIndex = 1;
			this.cmdRefreshSymbol.Text = "Refresh Symbol";
			this.cmdRefreshSymbol.UseVisualStyleBackColor = true;
			// 
			// tabPageUser
			// 
			this.tabPageUser.Controls.Add(this.cmdRefreshUser);
			this.tabPageUser.Location = new System.Drawing.Point(4, 22);
			this.tabPageUser.Name = "tabPageUser";
			this.tabPageUser.Padding = new System.Windows.Forms.Padding(3);
			this.tabPageUser.Size = new System.Drawing.Size(611, 494);
			this.tabPageUser.TabIndex = 2;
			this.tabPageUser.Text = "User";
			this.tabPageUser.UseVisualStyleBackColor = true;
			// 
			// cmdRefreshUser
			// 
			this.cmdRefreshUser.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.cmdRefreshUser.Location = new System.Drawing.Point(3, 465);
			this.cmdRefreshUser.Name = "cmdRefreshUser";
			this.cmdRefreshUser.Size = new System.Drawing.Size(605, 26);
			this.cmdRefreshUser.TabIndex = 2;
			this.cmdRefreshUser.Text = "Refresh User";
			this.cmdRefreshUser.UseVisualStyleBackColor = true;
			// 
			// tabPageAbout
			// 
			this.tabPageAbout.Controls.Add(this.rtbAbout);
			this.tabPageAbout.Controls.Add(this.picLogo);
			this.tabPageAbout.Location = new System.Drawing.Point(4, 22);
			this.tabPageAbout.Name = "tabPageAbout";
			this.tabPageAbout.Padding = new System.Windows.Forms.Padding(3);
			this.tabPageAbout.Size = new System.Drawing.Size(611, 494);
			this.tabPageAbout.TabIndex = 4;
			this.tabPageAbout.Text = "About";
			this.tabPageAbout.UseVisualStyleBackColor = true;
			// 
			// rtbAbout
			// 
			this.rtbAbout.BackColor = System.Drawing.SystemColors.ActiveCaptionText;
			this.rtbAbout.BorderStyle = System.Windows.Forms.BorderStyle.None;
			this.rtbAbout.Dock = System.Windows.Forms.DockStyle.Fill;
			this.rtbAbout.Font = new System.Drawing.Font("Courier New", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.rtbAbout.Location = new System.Drawing.Point(308, 3);
			this.rtbAbout.Name = "rtbAbout";
			this.rtbAbout.ReadOnly = true;
			this.rtbAbout.Size = new System.Drawing.Size(300, 488);
			this.rtbAbout.TabIndex = 0;
			this.rtbAbout.Text = "";
			this.rtbAbout.WordWrap = false;
			// 
			// picLogo
			// 
			this.picLogo.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Center;
			this.picLogo.Dock = System.Windows.Forms.DockStyle.Left;
			this.picLogo.Location = new System.Drawing.Point(3, 3);
			this.picLogo.MaximumSize = new System.Drawing.Size(321, 0);
			this.picLogo.Name = "picLogo";
			this.picLogo.Size = new System.Drawing.Size(305, 488);
			this.picLogo.TabIndex = 2;
			this.picLogo.TabStop = false;
			// 
			// frmMain
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(575, 324);
			this.Controls.Add(this.tabControlMain);
			this.Name = "frmMain";
			this.Text = "Form1";
			this.tabControlMain.ResumeLayout(false);
			this.tabPageMain.ResumeLayout(false);
			this.grpServer.ResumeLayout(false);
			this.grpServer.PerformLayout();
			this.statusStripServer.ResumeLayout(false);
			this.statusStripServer.PerformLayout();
			this.grpSRLab.ResumeLayout(false);
			this.grpSRLab.PerformLayout();
			this.statusStripLBM.ResumeLayout(false);
			this.statusStripLBM.PerformLayout();
			this.tabPageSymbols.ResumeLayout(false);
			this.tabPageSymbols.PerformLayout();
			this.tabPageUser.ResumeLayout(false);
			this.tabPageAbout.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.picLogo)).EndInit();
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.TabControl tabControlMain;
		private System.Windows.Forms.TabPage tabPageMain;
		private System.Windows.Forms.GroupBox grpServer;
		private System.Windows.Forms.Label lblMDSSentMessage;
		private System.Windows.Forms.Label lblMDSRecivedMessage;
		private System.Windows.Forms.StatusStrip statusStripServer;
		private System.Windows.Forms.ToolStripStatusLabel dspMDS;
		private System.Windows.Forms.ToolStripStatusLabel lblMDSStatus;
		private System.Windows.Forms.ToolStripStatusLabel lblMDSStatusMessage;
		private System.Windows.Forms.Button cmdServerStop;
		private System.Windows.Forms.Button cmdServerStart;
		private System.Windows.Forms.GroupBox grpSRLab;
		private System.Windows.Forms.Button cmdSRLabMDS;
		private System.Windows.Forms.Button cmdForceGC;
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
		private System.Windows.Forms.Button cmdSRLabDisconnect;
		private System.Windows.Forms.Button cmdSRLabConnect;
		private System.Windows.Forms.Button cmdCheckEnvironments;
		private System.Windows.Forms.TabPage tabPageSymbols;
		private System.Windows.Forms.TextBox txtSymbolToGet;
		private System.Windows.Forms.Button cmdRefreshSymbol;
		private System.Windows.Forms.TabPage tabPageUser;
		private System.Windows.Forms.Button cmdRefreshUser;
		private System.Windows.Forms.TabPage tabPageAbout;
		private System.Windows.Forms.RichTextBox rtbAbout;
		private System.Windows.Forms.PictureBox picLogo;
	}
}

