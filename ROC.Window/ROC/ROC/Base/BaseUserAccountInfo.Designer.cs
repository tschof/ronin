namespace ROC
{
	partial class BaseUserAccountInfo
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
			this.panelManagerMain = new PanelEx.PanelManager();
			this.panelUserInfo = new PanelEx.ManagedPanel();
			this.cmdSettings = new ButtonEx.VistaButton();
			this.cmdExchange = new ButtonEx.VistaButton();
			this.lblExchange = new LabelEx.LabelVista();
			this.cmdAccount = new ButtonEx.VistaButton();
			this.lblAccount = new LabelEx.LabelVista();
			this.cmdTrader = new ButtonEx.VistaButton();
			this.lblTrader = new LabelEx.LabelVista();
			this.panelManagerMain.SuspendLayout();
			this.panelUserInfo.SuspendLayout();
			this.SuspendLayout();
			// 
			// panelManagerMain
			// 
			this.panelManagerMain.Controls.Add(this.panelUserInfo);
			this.panelManagerMain.Dock = System.Windows.Forms.DockStyle.Fill;
			this.panelManagerMain.Location = new System.Drawing.Point(1, 1);
			this.panelManagerMain.Name = "panelManagerMain";
			this.panelManagerMain.SelectedIndex = 0;
			this.panelManagerMain.SelectedPanel = this.panelUserInfo;
			this.panelManagerMain.Size = new System.Drawing.Size(428, 23);
			this.panelManagerMain.TabIndex = 0;
			this.panelManagerMain.TabStop = false;
			// 
			// panelUserInfo
			// 
			this.panelUserInfo.BackColor = System.Drawing.SystemColors.ControlDarkDark;
			this.panelUserInfo.Controls.Add(this.cmdSettings);
			this.panelUserInfo.Controls.Add(this.cmdExchange);
			this.panelUserInfo.Controls.Add(this.lblExchange);
			this.panelUserInfo.Controls.Add(this.cmdAccount);
			this.panelUserInfo.Controls.Add(this.lblAccount);
			this.panelUserInfo.Controls.Add(this.cmdTrader);
			this.panelUserInfo.Controls.Add(this.lblTrader);
			this.panelUserInfo.Location = new System.Drawing.Point(0, 0);
			this.panelUserInfo.Name = "panelUserInfo";
			this.panelUserInfo.Size = new System.Drawing.Size(428, 23);
			this.panelUserInfo.Text = "panel";
			// 
			// cmdSettings
			// 
			this.cmdSettings.BackColor = System.Drawing.Color.Black;
			this.cmdSettings.ButtonType = ButtonEx.VistaButton.VistaButtonTypes.Split;
			this.cmdSettings.Dock = System.Windows.Forms.DockStyle.Left;
			this.cmdSettings.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdSettings.Location = new System.Drawing.Point(353, 0);
			this.cmdSettings.MinimumSize = new System.Drawing.Size(75, 0);
			this.cmdSettings.Name = "cmdSettings";
			this.cmdSettings.Size = new System.Drawing.Size(75, 23);
			this.cmdSettings.TabIndex = 6;
			this.cmdSettings.Text = "Settings";
			this.cmdSettings.UserSplitSize = 20;
			// 
			// cmdExchange
			// 
			this.cmdExchange.AutoSize = true;
			this.cmdExchange.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdExchange.BackColor = System.Drawing.Color.Black;
			this.cmdExchange.ButtonType = ButtonEx.VistaButton.VistaButtonTypes.Split;
			this.cmdExchange.Dock = System.Windows.Forms.DockStyle.Left;
			this.cmdExchange.Location = new System.Drawing.Point(298, 0);
			this.cmdExchange.MinimumSize = new System.Drawing.Size(55, 0);
			this.cmdExchange.Name = "cmdExchange";
			this.cmdExchange.Size = new System.Drawing.Size(55, 23);
			this.cmdExchange.TabIndex = 5;
			this.cmdExchange.Text = "N/A";
			this.cmdExchange.UserSplitSize = 20;
			// 
			// lblExchange
			// 
			this.lblExchange.AutoColor = false;
			this.lblExchange.DefaultColor = System.Drawing.Color.White;
			this.lblExchange.DefaultText = "";
			this.lblExchange.DisplayFactor = 1;
			this.lblExchange.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblExchange.DownColor = System.Drawing.Color.Red;
			this.lblExchange.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblExchange.IsCurrency = false;
			this.lblExchange.IsFraction = false;
			this.lblExchange.IsPercent = false;
			this.lblExchange.Location = new System.Drawing.Point(226, 0);
			this.lblExchange.MaxDecimal = 7;
			this.lblExchange.Name = "lblExchange";
			this.lblExchange.Padding = new System.Windows.Forms.Padding(2);
			this.lblExchange.Size = new System.Drawing.Size(72, 23);
			this.lblExchange.TabIndex = 4;
			this.lblExchange.Text = "Exchange:";
			this.lblExchange.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			this.lblExchange.TickSize = 0.01;
			this.lblExchange.UpColor = System.Drawing.Color.LimeGreen;
			this.lblExchange.Value = "";
			// 
			// cmdAccount
			// 
			this.cmdAccount.AutoSize = true;
			this.cmdAccount.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdAccount.BackColor = System.Drawing.Color.Black;
			this.cmdAccount.ButtonType = ButtonEx.VistaButton.VistaButtonTypes.Split;
			this.cmdAccount.Dock = System.Windows.Forms.DockStyle.Left;
			this.cmdAccount.Location = new System.Drawing.Point(171, 0);
			this.cmdAccount.MinimumSize = new System.Drawing.Size(55, 0);
			this.cmdAccount.Name = "cmdAccount";
			this.cmdAccount.Size = new System.Drawing.Size(55, 23);
			this.cmdAccount.TabIndex = 3;
			this.cmdAccount.Text = "N/A";
			this.cmdAccount.UserSplitSize = 20;
			// 
			// lblAccount
			// 
			this.lblAccount.AutoColor = false;
			this.lblAccount.DefaultColor = System.Drawing.Color.White;
			this.lblAccount.DefaultText = "";
			this.lblAccount.DisplayFactor = 1;
			this.lblAccount.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblAccount.DownColor = System.Drawing.Color.Red;
			this.lblAccount.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblAccount.IsCurrency = false;
			this.lblAccount.IsFraction = false;
			this.lblAccount.IsPercent = false;
			this.lblAccount.Location = new System.Drawing.Point(107, 0);
			this.lblAccount.MaxDecimal = 7;
			this.lblAccount.Name = "lblAccount";
			this.lblAccount.Padding = new System.Windows.Forms.Padding(2);
			this.lblAccount.Size = new System.Drawing.Size(64, 23);
			this.lblAccount.TabIndex = 2;
			this.lblAccount.Text = "Account:";
			this.lblAccount.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			this.lblAccount.TickSize = 0.01;
			this.lblAccount.UpColor = System.Drawing.Color.LimeGreen;
			this.lblAccount.Value = "";
			// 
			// cmdTrader
			// 
			this.cmdTrader.AutoSize = true;
			this.cmdTrader.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdTrader.BackColor = System.Drawing.Color.Black;
			this.cmdTrader.ButtonType = ButtonEx.VistaButton.VistaButtonTypes.Split;
			this.cmdTrader.Dock = System.Windows.Forms.DockStyle.Left;
			this.cmdTrader.Location = new System.Drawing.Point(52, 0);
			this.cmdTrader.MinimumSize = new System.Drawing.Size(55, 0);
			this.cmdTrader.Name = "cmdTrader";
			this.cmdTrader.Size = new System.Drawing.Size(55, 23);
			this.cmdTrader.TabIndex = 1;
			this.cmdTrader.Text = "N/A";
			this.cmdTrader.UserSplitSize = 20;
			// 
			// lblTrader
			// 
			this.lblTrader.AutoColor = false;
			this.lblTrader.DefaultColor = System.Drawing.Color.White;
			this.lblTrader.DefaultText = "";
			this.lblTrader.DisplayFactor = 1;
			this.lblTrader.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblTrader.DownColor = System.Drawing.Color.Red;
			this.lblTrader.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblTrader.IsCurrency = false;
			this.lblTrader.IsFraction = false;
			this.lblTrader.IsPercent = false;
			this.lblTrader.Location = new System.Drawing.Point(0, 0);
			this.lblTrader.MaxDecimal = 7;
			this.lblTrader.Name = "lblTrader";
			this.lblTrader.Padding = new System.Windows.Forms.Padding(2);
			this.lblTrader.Size = new System.Drawing.Size(52, 23);
			this.lblTrader.TabIndex = 0;
			this.lblTrader.Text = "Trader:";
			this.lblTrader.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			this.lblTrader.TickSize = 0.01;
			this.lblTrader.UpColor = System.Drawing.Color.LimeGreen;
			this.lblTrader.Value = "";
			// 
			// BaseUserAccountInfo
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.Controls.Add(this.panelManagerMain);
			this.MinimumSize = new System.Drawing.Size(430, 25);
			this.Name = "BaseUserAccountInfo";
			this.Padding = new System.Windows.Forms.Padding(1);
			this.Size = new System.Drawing.Size(430, 25);
			this.Load += new System.EventHandler(this.BaseUserAccountInfo_Load);
			this.panelManagerMain.ResumeLayout(false);
			this.panelUserInfo.ResumeLayout(false);
			this.panelUserInfo.PerformLayout();
			this.ResumeLayout(false);

		}

		#endregion

		private PanelEx.PanelManager panelManagerMain;
		private PanelEx.ManagedPanel panelUserInfo;
		private LabelEx.LabelVista lblTrader;
		private ButtonEx.VistaButton cmdTrader;
		private ButtonEx.VistaButton cmdSettings;
		private ButtonEx.VistaButton cmdExchange;
		private LabelEx.LabelVista lblExchange;
		private ButtonEx.VistaButton cmdAccount;
		private LabelEx.LabelVista lblAccount;

	}
}
