namespace ROC
{
	partial class frmEMail
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
			this.panelEmail = new PanelEx.VistaPanel();
			this.cmdSendWithoutLog = new ButtonEx.VistaButton();
			this.txtWindowPassword = new System.Windows.Forms.TextBox();
			this.dspWindowPassword = new LabelEx.LabelBase();
			this.txtWindowUserName = new System.Windows.Forms.TextBox();
			this.dspWindowUserName = new LabelEx.LabelBase();
			this.lblEMailNotice = new LabelEx.LabelBase();
			this.rtbBody = new System.Windows.Forms.RichTextBox();
			this.dspBody = new LabelEx.LabelBase();
			this.txtTitle = new System.Windows.Forms.TextBox();
			this.dspTitle = new LabelEx.LabelBase();
			this.txtFrom = new System.Windows.Forms.TextBox();
			this.dspFrom = new LabelEx.LabelBase();
			this.cmdCancel = new ButtonEx.VistaButton();
			this.cmdSend = new ButtonEx.VistaButton();
			this.lblEMailServerPort = new LabelEx.LabelBase();
			this.txtEMailServerAddress = new System.Windows.Forms.TextBox();
			this.dspEMailServerAddress = new LabelEx.LabelBase();
			this.panelEmail.SuspendLayout();
			this.SuspendLayout();
			// 
			// panelEmail
			// 
			this.panelEmail.BackColor = System.Drawing.Color.Black;
			this.panelEmail.Controls.Add(this.cmdSendWithoutLog);
			this.panelEmail.Controls.Add(this.txtWindowPassword);
			this.panelEmail.Controls.Add(this.dspWindowPassword);
			this.panelEmail.Controls.Add(this.txtWindowUserName);
			this.panelEmail.Controls.Add(this.dspWindowUserName);
			this.panelEmail.Controls.Add(this.lblEMailNotice);
			this.panelEmail.Controls.Add(this.rtbBody);
			this.panelEmail.Controls.Add(this.dspBody);
			this.panelEmail.Controls.Add(this.txtTitle);
			this.panelEmail.Controls.Add(this.dspTitle);
			this.panelEmail.Controls.Add(this.txtFrom);
			this.panelEmail.Controls.Add(this.dspFrom);
			this.panelEmail.Controls.Add(this.cmdCancel);
			this.panelEmail.Controls.Add(this.cmdSend);
			this.panelEmail.Controls.Add(this.lblEMailServerPort);
			this.panelEmail.Controls.Add(this.txtEMailServerAddress);
			this.panelEmail.Controls.Add(this.dspEMailServerAddress);
			this.panelEmail.Dock = System.Windows.Forms.DockStyle.Fill;
			this.panelEmail.Location = new System.Drawing.Point(0, 0);
			this.panelEmail.Name = "panelEmail";
			this.panelEmail.Padding = new System.Windows.Forms.Padding(5);
			this.panelEmail.Size = new System.Drawing.Size(355, 470);
			this.panelEmail.TabIndex = 0;
			// 
			// cmdSendWithoutLog
			// 
			this.cmdSendWithoutLog.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
			this.cmdSendWithoutLog.AutoSize = true;
			this.cmdSendWithoutLog.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdSendWithoutLog.BackColor = System.Drawing.Color.Black;
			this.cmdSendWithoutLog.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdSendWithoutLog.ForeColor = System.Drawing.Color.GreenYellow;
			this.cmdSendWithoutLog.Location = new System.Drawing.Point(179, 439);
			this.cmdSendWithoutLog.Name = "cmdSendWithoutLog";
			this.cmdSendWithoutLog.Size = new System.Drawing.Size(119, 23);
			this.cmdSendWithoutLog.TabIndex = 2;
			this.cmdSendWithoutLog.TabStop = false;
			this.cmdSendWithoutLog.Text = "Send Without Log";
			this.cmdSendWithoutLog.Click += new System.EventHandler(this.cmdSendWithoutLog_Click);
			// 
			// txtWindowPassword
			// 
			this.txtWindowPassword.BackColor = System.Drawing.SystemColors.ControlDarkDark;
			this.txtWindowPassword.Dock = System.Windows.Forms.DockStyle.Top;
			this.txtWindowPassword.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.txtWindowPassword.ForeColor = System.Drawing.Color.White;
			this.txtWindowPassword.Location = new System.Drawing.Point(5, 407);
			this.txtWindowPassword.Name = "txtWindowPassword";
			this.txtWindowPassword.PasswordChar = '*';
			this.txtWindowPassword.Size = new System.Drawing.Size(345, 26);
			this.txtWindowPassword.TabIndex = 1;
			this.txtWindowPassword.TextChanged += new System.EventHandler(this.txtWindowPassword_TextChanged);
			this.txtWindowPassword.KeyDown += new System.Windows.Forms.KeyEventHandler(this.txtWindowPassword_KeyDown);
			// 
			// dspWindowPassword
			// 
			this.dspWindowPassword.AutoColor = false;
			this.dspWindowPassword.AutoSize = true;
			this.dspWindowPassword.DefaultColor = System.Drawing.Color.White;
			this.dspWindowPassword.DefaultText = "Window Password :";
			this.dspWindowPassword.DisplayFactor = 1;
			this.dspWindowPassword.Dock = System.Windows.Forms.DockStyle.Top;
			this.dspWindowPassword.DownColor = System.Drawing.Color.Red;
			this.dspWindowPassword.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspWindowPassword.ForeColor = System.Drawing.Color.Silver;
			this.dspWindowPassword.IsCurrency = false;
			this.dspWindowPassword.IsFraction = false;
			this.dspWindowPassword.IsPercent = false;
			this.dspWindowPassword.Location = new System.Drawing.Point(5, 392);
			this.dspWindowPassword.MaxDecimal = 7;
			this.dspWindowPassword.Name = "dspWindowPassword";
			this.dspWindowPassword.Padding = new System.Windows.Forms.Padding(1);
			this.dspWindowPassword.Size = new System.Drawing.Size(120, 15);
			this.dspWindowPassword.TabIndex = 0;
			this.dspWindowPassword.Text = "Window Password :";
			this.dspWindowPassword.TickSize = 0.01;
			this.dspWindowPassword.UpColor = System.Drawing.Color.LimeGreen;
			this.dspWindowPassword.Value = "";
			// 
			// txtWindowUserName
			// 
			this.txtWindowUserName.BackColor = System.Drawing.SystemColors.ControlDarkDark;
			this.txtWindowUserName.Dock = System.Windows.Forms.DockStyle.Top;
			this.txtWindowUserName.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.txtWindowUserName.ForeColor = System.Drawing.Color.White;
			this.txtWindowUserName.Location = new System.Drawing.Point(5, 366);
			this.txtWindowUserName.Name = "txtWindowUserName";
			this.txtWindowUserName.Size = new System.Drawing.Size(345, 26);
			this.txtWindowUserName.TabIndex = 0;
			this.txtWindowUserName.TextChanged += new System.EventHandler(this.txtWindowUserName_TextChanged);
			// 
			// dspWindowUserName
			// 
			this.dspWindowUserName.AutoColor = false;
			this.dspWindowUserName.AutoSize = true;
			this.dspWindowUserName.DefaultColor = System.Drawing.Color.White;
			this.dspWindowUserName.DefaultText = "Window User Name :";
			this.dspWindowUserName.DisplayFactor = 1;
			this.dspWindowUserName.Dock = System.Windows.Forms.DockStyle.Top;
			this.dspWindowUserName.DownColor = System.Drawing.Color.Red;
			this.dspWindowUserName.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspWindowUserName.ForeColor = System.Drawing.Color.Silver;
			this.dspWindowUserName.IsCurrency = false;
			this.dspWindowUserName.IsFraction = false;
			this.dspWindowUserName.IsPercent = false;
			this.dspWindowUserName.Location = new System.Drawing.Point(5, 351);
			this.dspWindowUserName.MaxDecimal = 7;
			this.dspWindowUserName.Name = "dspWindowUserName";
			this.dspWindowUserName.Padding = new System.Windows.Forms.Padding(1);
			this.dspWindowUserName.Size = new System.Drawing.Size(128, 15);
			this.dspWindowUserName.TabIndex = 0;
			this.dspWindowUserName.Text = "Window User Name :";
			this.dspWindowUserName.TickSize = 0.01;
			this.dspWindowUserName.UpColor = System.Drawing.Color.LimeGreen;
			this.dspWindowUserName.Value = "";
			// 
			// lblEMailNotice
			// 
			this.lblEMailNotice.AutoColor = false;
			this.lblEMailNotice.AutoEllipsis = true;
			this.lblEMailNotice.BackColor = System.Drawing.Color.Transparent;
			this.lblEMailNotice.DefaultColor = System.Drawing.Color.White;
			this.lblEMailNotice.DefaultText = "Port: 25";
			this.lblEMailNotice.DisplayFactor = 1;
			this.lblEMailNotice.Dock = System.Windows.Forms.DockStyle.Top;
			this.lblEMailNotice.DownColor = System.Drawing.Color.Red;
			this.lblEMailNotice.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblEMailNotice.ForeColor = System.Drawing.Color.Gold;
			this.lblEMailNotice.IsCurrency = false;
			this.lblEMailNotice.IsFraction = false;
			this.lblEMailNotice.IsPercent = false;
			this.lblEMailNotice.Location = new System.Drawing.Point(5, 313);
			this.lblEMailNotice.MaxDecimal = 7;
			this.lblEMailNotice.Name = "lblEMailNotice";
			this.lblEMailNotice.Padding = new System.Windows.Forms.Padding(5);
			this.lblEMailNotice.Size = new System.Drawing.Size(345, 38);
			this.lblEMailNotice.TabIndex = 0;
			this.lblEMailNotice.Text = "For Users Non Ronin Network Please Change Your Email Server and Type in your Wind" +
				"ow user name and Password.";
			this.lblEMailNotice.TickSize = 0.01;
			this.lblEMailNotice.UpColor = System.Drawing.Color.LimeGreen;
			this.lblEMailNotice.Value = "";
			// 
			// rtbBody
			// 
			this.rtbBody.BackColor = System.Drawing.SystemColors.ControlDarkDark;
			this.rtbBody.Dock = System.Windows.Forms.DockStyle.Top;
			this.rtbBody.ForeColor = System.Drawing.Color.White;
			this.rtbBody.Location = new System.Drawing.Point(5, 143);
			this.rtbBody.Name = "rtbBody";
			this.rtbBody.Size = new System.Drawing.Size(345, 170);
			this.rtbBody.TabIndex = 0;
			this.rtbBody.TabStop = false;
			this.rtbBody.Text = "";
			// 
			// dspBody
			// 
			this.dspBody.AutoColor = false;
			this.dspBody.AutoSize = true;
			this.dspBody.DefaultColor = System.Drawing.Color.White;
			this.dspBody.DefaultText = "User Name :";
			this.dspBody.DisplayFactor = 1;
			this.dspBody.Dock = System.Windows.Forms.DockStyle.Top;
			this.dspBody.DownColor = System.Drawing.Color.Red;
			this.dspBody.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspBody.ForeColor = System.Drawing.Color.Silver;
			this.dspBody.IsCurrency = false;
			this.dspBody.IsFraction = false;
			this.dspBody.IsPercent = false;
			this.dspBody.Location = new System.Drawing.Point(5, 128);
			this.dspBody.MaxDecimal = 7;
			this.dspBody.Name = "dspBody";
			this.dspBody.Padding = new System.Windows.Forms.Padding(1);
			this.dspBody.Size = new System.Drawing.Size(45, 15);
			this.dspBody.TabIndex = 0;
			this.dspBody.Text = "Body :";
			this.dspBody.TickSize = 0.01;
			this.dspBody.UpColor = System.Drawing.Color.LimeGreen;
			this.dspBody.Value = "";
			// 
			// txtTitle
			// 
			this.txtTitle.BackColor = System.Drawing.SystemColors.ControlDarkDark;
			this.txtTitle.Dock = System.Windows.Forms.DockStyle.Top;
			this.txtTitle.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.txtTitle.ForeColor = System.Drawing.Color.White;
			this.txtTitle.Location = new System.Drawing.Point(5, 102);
			this.txtTitle.Name = "txtTitle";
			this.txtTitle.Size = new System.Drawing.Size(345, 26);
			this.txtTitle.TabIndex = 0;
			this.txtTitle.TabStop = false;
			// 
			// dspTitle
			// 
			this.dspTitle.AutoColor = false;
			this.dspTitle.AutoSize = true;
			this.dspTitle.DefaultColor = System.Drawing.Color.White;
			this.dspTitle.DefaultText = "User Name :";
			this.dspTitle.DisplayFactor = 1;
			this.dspTitle.Dock = System.Windows.Forms.DockStyle.Top;
			this.dspTitle.DownColor = System.Drawing.Color.Red;
			this.dspTitle.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspTitle.ForeColor = System.Drawing.Color.Silver;
			this.dspTitle.IsCurrency = false;
			this.dspTitle.IsFraction = false;
			this.dspTitle.IsPercent = false;
			this.dspTitle.Location = new System.Drawing.Point(5, 87);
			this.dspTitle.MaxDecimal = 7;
			this.dspTitle.Name = "dspTitle";
			this.dspTitle.Padding = new System.Windows.Forms.Padding(1);
			this.dspTitle.Size = new System.Drawing.Size(42, 15);
			this.dspTitle.TabIndex = 0;
			this.dspTitle.Text = "Title :";
			this.dspTitle.TickSize = 0.01;
			this.dspTitle.UpColor = System.Drawing.Color.LimeGreen;
			this.dspTitle.Value = "";
			// 
			// txtFrom
			// 
			this.txtFrom.BackColor = System.Drawing.SystemColors.ControlDarkDark;
			this.txtFrom.Dock = System.Windows.Forms.DockStyle.Top;
			this.txtFrom.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.txtFrom.ForeColor = System.Drawing.Color.White;
			this.txtFrom.Location = new System.Drawing.Point(5, 61);
			this.txtFrom.Name = "txtFrom";
			this.txtFrom.Size = new System.Drawing.Size(345, 26);
			this.txtFrom.TabIndex = 0;
			this.txtFrom.TabStop = false;
			// 
			// dspFrom
			// 
			this.dspFrom.AutoColor = false;
			this.dspFrom.AutoSize = true;
			this.dspFrom.DefaultColor = System.Drawing.Color.White;
			this.dspFrom.DefaultText = "From :";
			this.dspFrom.DisplayFactor = 1;
			this.dspFrom.Dock = System.Windows.Forms.DockStyle.Top;
			this.dspFrom.DownColor = System.Drawing.Color.Red;
			this.dspFrom.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspFrom.ForeColor = System.Drawing.Color.Silver;
			this.dspFrom.IsCurrency = false;
			this.dspFrom.IsFraction = false;
			this.dspFrom.IsPercent = false;
			this.dspFrom.Location = new System.Drawing.Point(5, 46);
			this.dspFrom.MaxDecimal = 7;
			this.dspFrom.Name = "dspFrom";
			this.dspFrom.Padding = new System.Windows.Forms.Padding(1);
			this.dspFrom.Size = new System.Drawing.Size(44, 15);
			this.dspFrom.TabIndex = 0;
			this.dspFrom.Text = "From :";
			this.dspFrom.TickSize = 0.01;
			this.dspFrom.UpColor = System.Drawing.Color.LimeGreen;
			this.dspFrom.Value = "";
			// 
			// cmdCancel
			// 
			this.cmdCancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
			this.cmdCancel.BackColor = System.Drawing.Color.Black;
			this.cmdCancel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdCancel.ForeColor = System.Drawing.Color.Red;
			this.cmdCancel.Location = new System.Drawing.Point(5, 439);
			this.cmdCancel.Name = "cmdCancel";
			this.cmdCancel.Size = new System.Drawing.Size(55, 23);
			this.cmdCancel.TabIndex = 0;
			this.cmdCancel.TabStop = false;
			this.cmdCancel.Text = "Cancel";
			this.cmdCancel.Click += new System.EventHandler(this.cmdCancel_Click);
			// 
			// cmdSend
			// 
			this.cmdSend.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
			this.cmdSend.AutoSize = true;
			this.cmdSend.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdSend.BackColor = System.Drawing.Color.Black;
			this.cmdSend.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdSend.ForeColor = System.Drawing.Color.LimeGreen;
			this.cmdSend.Location = new System.Drawing.Point(304, 439);
			this.cmdSend.Name = "cmdSend";
			this.cmdSend.Size = new System.Drawing.Size(46, 23);
			this.cmdSend.TabIndex = 0;
			this.cmdSend.TabStop = false;
			this.cmdSend.Text = "Send";
			this.cmdSend.Click += new System.EventHandler(this.cmdSend_Click);
			// 
			// lblEMailServerPort
			// 
			this.lblEMailServerPort.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.lblEMailServerPort.AutoColor = false;
			this.lblEMailServerPort.AutoEllipsis = true;
			this.lblEMailServerPort.AutoSize = true;
			this.lblEMailServerPort.BackColor = System.Drawing.Color.Transparent;
			this.lblEMailServerPort.DefaultColor = System.Drawing.Color.White;
			this.lblEMailServerPort.DefaultText = "Port: 25";
			this.lblEMailServerPort.DisplayFactor = 1;
			this.lblEMailServerPort.DownColor = System.Drawing.Color.Red;
			this.lblEMailServerPort.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblEMailServerPort.ForeColor = System.Drawing.SystemColors.GradientActiveCaption;
			this.lblEMailServerPort.IsCurrency = false;
			this.lblEMailServerPort.IsFraction = false;
			this.lblEMailServerPort.IsPercent = false;
			this.lblEMailServerPort.Location = new System.Drawing.Point(307, 4);
			this.lblEMailServerPort.MaxDecimal = 7;
			this.lblEMailServerPort.MaximumSize = new System.Drawing.Size(150, 15);
			this.lblEMailServerPort.Name = "lblEMailServerPort";
			this.lblEMailServerPort.Padding = new System.Windows.Forms.Padding(5, 1, 1, 1);
			this.lblEMailServerPort.Size = new System.Drawing.Size(43, 14);
			this.lblEMailServerPort.TabIndex = 0;
			this.lblEMailServerPort.Text = "Port: 25";
			this.lblEMailServerPort.TickSize = 0.01;
			this.lblEMailServerPort.UpColor = System.Drawing.Color.LimeGreen;
			this.lblEMailServerPort.Value = "";
			// 
			// txtEMailServerAddress
			// 
			this.txtEMailServerAddress.BackColor = System.Drawing.SystemColors.ControlDarkDark;
			this.txtEMailServerAddress.Dock = System.Windows.Forms.DockStyle.Top;
			this.txtEMailServerAddress.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.txtEMailServerAddress.ForeColor = System.Drawing.Color.White;
			this.txtEMailServerAddress.Location = new System.Drawing.Point(5, 20);
			this.txtEMailServerAddress.Name = "txtEMailServerAddress";
			this.txtEMailServerAddress.Size = new System.Drawing.Size(345, 26);
			this.txtEMailServerAddress.TabIndex = 0;
			this.txtEMailServerAddress.TabStop = false;
			// 
			// dspEMailServerAddress
			// 
			this.dspEMailServerAddress.AutoColor = false;
			this.dspEMailServerAddress.AutoSize = true;
			this.dspEMailServerAddress.BackColor = System.Drawing.Color.Transparent;
			this.dspEMailServerAddress.DefaultColor = System.Drawing.Color.White;
			this.dspEMailServerAddress.DefaultText = "EMail Server :";
			this.dspEMailServerAddress.DisplayFactor = 1;
			this.dspEMailServerAddress.Dock = System.Windows.Forms.DockStyle.Top;
			this.dspEMailServerAddress.DownColor = System.Drawing.Color.Red;
			this.dspEMailServerAddress.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspEMailServerAddress.ForeColor = System.Drawing.Color.Silver;
			this.dspEMailServerAddress.IsCurrency = false;
			this.dspEMailServerAddress.IsFraction = false;
			this.dspEMailServerAddress.IsPercent = false;
			this.dspEMailServerAddress.Location = new System.Drawing.Point(5, 5);
			this.dspEMailServerAddress.MaxDecimal = 7;
			this.dspEMailServerAddress.Name = "dspEMailServerAddress";
			this.dspEMailServerAddress.Padding = new System.Windows.Forms.Padding(1);
			this.dspEMailServerAddress.Size = new System.Drawing.Size(89, 15);
			this.dspEMailServerAddress.TabIndex = 0;
			this.dspEMailServerAddress.Text = "EMail Server :";
			this.dspEMailServerAddress.TickSize = 0.01;
			this.dspEMailServerAddress.UpColor = System.Drawing.Color.LimeGreen;
			this.dspEMailServerAddress.Value = "";
			// 
			// frmEMail
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(355, 470);
			this.Controls.Add(this.panelEmail);
			this.Name = "frmEMail";
			this.OnTop = true;
			this.ShowInTaskbar = false;
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.Text = "frmEMail";
			this.TopMost = true;
			this.panelEmail.ResumeLayout(false);
			this.panelEmail.PerformLayout();
			this.ResumeLayout(false);

		}

		#endregion

		private PanelEx.VistaPanel panelEmail;
		private LabelEx.LabelBase dspEMailServerAddress;
		private System.Windows.Forms.TextBox txtEMailServerAddress;
		private System.Windows.Forms.TextBox txtWindowPassword;
		private LabelEx.LabelBase dspWindowPassword;
		private System.Windows.Forms.TextBox txtWindowUserName;
		private LabelEx.LabelBase dspWindowUserName;
		private LabelEx.LabelBase lblEMailServerPort;
		private ButtonEx.VistaButton cmdSend;
		private ButtonEx.VistaButton cmdCancel;
		private LabelEx.LabelBase lblEMailNotice;
		private System.Windows.Forms.TextBox txtFrom;
		private LabelEx.LabelBase dspFrom;
		private System.Windows.Forms.RichTextBox rtbBody;
		private LabelEx.LabelBase dspBody;
		private System.Windows.Forms.TextBox txtTitle;
		private LabelEx.LabelBase dspTitle;
		private ButtonEx.VistaButton cmdSendWithoutLog;
	}
}