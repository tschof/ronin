namespace ROC
{
	partial class itemBaseTicker
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
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(itemBaseTicker));
			this.dspClosePrice = new LabelEx.LabelBase();
			this.panelManagerTicker = new PanelEx.PanelManager();
			this.managedPanelMain = new PanelEx.ManagedPanel();
			this.panelBottom = new System.Windows.Forms.Panel();
			this.lblClosePrice = new System.Windows.Forms.Label();
			this.dspHighPrice = new LabelEx.LabelBase();
			this.lblHighPrice = new System.Windows.Forms.Label();
			this.dspLowPrice = new LabelEx.LabelBase();
			this.lblLowPrice = new System.Windows.Forms.Label();
			this.dspVolume = new LabelEx.LabelBase();
			this.lblVolume = new System.Windows.Forms.Label();
			this.panelUpper = new System.Windows.Forms.Panel();
			this.dspPctChange = new LabelEx.LabelBase();
			this.lblPctChange = new System.Windows.Forms.Label();
			this.dspNetChange = new LabelEx.LabelBase();
			this.lblNetChange = new System.Windows.Forms.Label();
			this.AskPriceSizePair = new ROC.itemBasePriceSizePair();
			this.BidPriceSizePair = new ROC.itemBasePriceSizePair();
			this.SymbolTradePair = new ROC.itemBaseSymbolTradePair();
			this.managedPanelHide = new PanelEx.ManagedPanel();
			this.panelManagerTicker.SuspendLayout();
			this.managedPanelMain.SuspendLayout();
			this.panelBottom.SuspendLayout();
			this.panelUpper.SuspendLayout();
			this.SuspendLayout();
			// 
			// dspClosePrice
			// 
			this.dspClosePrice.AutoColor = false;
			this.dspClosePrice.AutoSize = true;
			this.dspClosePrice.DefaultColor = System.Drawing.Color.White;
			this.dspClosePrice.DefaultText = "0.00";
			this.dspClosePrice.DisplayFactor = 1;
			this.dspClosePrice.Dock = System.Windows.Forms.DockStyle.Left;
			this.dspClosePrice.DownColor = System.Drawing.Color.Red;
			this.dspClosePrice.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspClosePrice.IsCurrency = false;
			this.dspClosePrice.IsFraction = false;
			this.dspClosePrice.Location = new System.Drawing.Point(189, 0);
			this.dspClosePrice.MaxDecimal = 7;
			this.dspClosePrice.Name = "dspClosePrice";
			this.dspClosePrice.Padding = new System.Windows.Forms.Padding(0, 1, 0, 0);
			this.dspClosePrice.Size = new System.Drawing.Size(31, 14);
			this.dspClosePrice.TabIndex = 22;
			this.dspClosePrice.Text = "0.00";
			this.dspClosePrice.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.dspClosePrice.TickSize = 0.01;
			this.dspClosePrice.UpColor = System.Drawing.Color.LimeGreen;
			this.dspClosePrice.Value = "";
			// 
			// panelManagerTicker
			// 
			this.panelManagerTicker.Controls.Add(this.managedPanelMain);
			this.panelManagerTicker.Controls.Add(this.managedPanelHide);
			this.panelManagerTicker.Dock = System.Windows.Forms.DockStyle.Fill;
			this.panelManagerTicker.Location = new System.Drawing.Point(0, 0);
			this.panelManagerTicker.Name = "panelManagerTicker";
			this.panelManagerTicker.SelectedIndex = 0;
			this.panelManagerTicker.SelectedPanel = this.managedPanelMain;
			this.panelManagerTicker.Size = new System.Drawing.Size(380, 45);
			this.panelManagerTicker.TabIndex = 1;
			this.panelManagerTicker.TabStop = false;
			// 
			// managedPanelMain
			// 
			this.managedPanelMain.BackColor = System.Drawing.Color.Black;
			this.managedPanelMain.Controls.Add(this.panelBottom);
			this.managedPanelMain.Controls.Add(this.panelUpper);
			this.managedPanelMain.Controls.Add(this.SymbolTradePair);
			this.managedPanelMain.ForeColor = System.Drawing.Color.Gainsboro;
			this.managedPanelMain.Location = new System.Drawing.Point(0, 0);
			this.managedPanelMain.Name = "managedPanelMain";
			this.managedPanelMain.Size = new System.Drawing.Size(380, 45);
			this.managedPanelMain.Text = "managedPanelMain";
			// 
			// panelBottom
			// 
			this.panelBottom.Controls.Add(this.dspClosePrice);
			this.panelBottom.Controls.Add(this.lblClosePrice);
			this.panelBottom.Controls.Add(this.dspHighPrice);
			this.panelBottom.Controls.Add(this.lblHighPrice);
			this.panelBottom.Controls.Add(this.dspLowPrice);
			this.panelBottom.Controls.Add(this.lblLowPrice);
			this.panelBottom.Controls.Add(this.dspVolume);
			this.panelBottom.Controls.Add(this.lblVolume);
			this.panelBottom.Dock = System.Windows.Forms.DockStyle.Top;
			this.panelBottom.Location = new System.Drawing.Point(76, 27);
			this.panelBottom.Name = "panelBottom";
			this.panelBottom.Size = new System.Drawing.Size(304, 18);
			this.panelBottom.TabIndex = 5;
			// 
			// lblClosePrice
			// 
			this.lblClosePrice.AutoSize = true;
			this.lblClosePrice.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblClosePrice.ForeColor = System.Drawing.Color.Khaki;
			this.lblClosePrice.Location = new System.Drawing.Point(165, 0);
			this.lblClosePrice.MaximumSize = new System.Drawing.Size(24, 14);
			this.lblClosePrice.MinimumSize = new System.Drawing.Size(24, 14);
			this.lblClosePrice.Name = "lblClosePrice";
			this.lblClosePrice.Padding = new System.Windows.Forms.Padding(0, 1, 0, 0);
			this.lblClosePrice.Size = new System.Drawing.Size(24, 14);
			this.lblClosePrice.TabIndex = 21;
			this.lblClosePrice.Text = "pr";
			this.lblClosePrice.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// dspHighPrice
			// 
			this.dspHighPrice.AutoColor = false;
			this.dspHighPrice.AutoSize = true;
			this.dspHighPrice.DefaultColor = System.Drawing.Color.White;
			this.dspHighPrice.DefaultText = "0.00";
			this.dspHighPrice.DisplayFactor = 1;
			this.dspHighPrice.Dock = System.Windows.Forms.DockStyle.Left;
			this.dspHighPrice.DownColor = System.Drawing.Color.Red;
			this.dspHighPrice.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspHighPrice.IsCurrency = false;
			this.dspHighPrice.IsFraction = false;
			this.dspHighPrice.Location = new System.Drawing.Point(134, 0);
			this.dspHighPrice.MaxDecimal = 7;
			this.dspHighPrice.Name = "dspHighPrice";
			this.dspHighPrice.Padding = new System.Windows.Forms.Padding(0, 1, 0, 0);
			this.dspHighPrice.Size = new System.Drawing.Size(31, 14);
			this.dspHighPrice.TabIndex = 20;
			this.dspHighPrice.Text = "0.00";
			this.dspHighPrice.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.dspHighPrice.TickSize = 0.01;
			this.dspHighPrice.UpColor = System.Drawing.Color.LimeGreen;
			this.dspHighPrice.Value = "";
			// 
			// lblHighPrice
			// 
			this.lblHighPrice.AutoSize = true;
			this.lblHighPrice.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblHighPrice.ForeColor = System.Drawing.Color.Khaki;
			this.lblHighPrice.Location = new System.Drawing.Point(110, 0);
			this.lblHighPrice.MaximumSize = new System.Drawing.Size(24, 14);
			this.lblHighPrice.MinimumSize = new System.Drawing.Size(24, 14);
			this.lblHighPrice.Name = "lblHighPrice";
			this.lblHighPrice.Padding = new System.Windows.Forms.Padding(0, 1, 0, 0);
			this.lblHighPrice.Size = new System.Drawing.Size(24, 14);
			this.lblHighPrice.TabIndex = 19;
			this.lblHighPrice.Text = "hi";
			this.lblHighPrice.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// dspLowPrice
			// 
			this.dspLowPrice.AutoColor = false;
			this.dspLowPrice.AutoSize = true;
			this.dspLowPrice.DefaultColor = System.Drawing.Color.White;
			this.dspLowPrice.DefaultText = "0.00";
			this.dspLowPrice.DisplayFactor = 1;
			this.dspLowPrice.Dock = System.Windows.Forms.DockStyle.Left;
			this.dspLowPrice.DownColor = System.Drawing.Color.Red;
			this.dspLowPrice.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspLowPrice.IsCurrency = false;
			this.dspLowPrice.IsFraction = false;
			this.dspLowPrice.Location = new System.Drawing.Point(79, 0);
			this.dspLowPrice.MaxDecimal = 7;
			this.dspLowPrice.Name = "dspLowPrice";
			this.dspLowPrice.Padding = new System.Windows.Forms.Padding(0, 1, 0, 0);
			this.dspLowPrice.Size = new System.Drawing.Size(31, 14);
			this.dspLowPrice.TabIndex = 18;
			this.dspLowPrice.Text = "0.00";
			this.dspLowPrice.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.dspLowPrice.TickSize = 0.01;
			this.dspLowPrice.UpColor = System.Drawing.Color.LimeGreen;
			this.dspLowPrice.Value = "";
			// 
			// lblLowPrice
			// 
			this.lblLowPrice.AutoSize = true;
			this.lblLowPrice.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblLowPrice.ForeColor = System.Drawing.Color.Khaki;
			this.lblLowPrice.Location = new System.Drawing.Point(55, 0);
			this.lblLowPrice.MaximumSize = new System.Drawing.Size(24, 14);
			this.lblLowPrice.MinimumSize = new System.Drawing.Size(24, 14);
			this.lblLowPrice.Name = "lblLowPrice";
			this.lblLowPrice.Padding = new System.Windows.Forms.Padding(0, 1, 0, 0);
			this.lblLowPrice.Size = new System.Drawing.Size(24, 14);
			this.lblLowPrice.TabIndex = 17;
			this.lblLowPrice.Text = "lo";
			this.lblLowPrice.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// dspVolume
			// 
			this.dspVolume.AutoColor = false;
			this.dspVolume.AutoSize = true;
			this.dspVolume.DefaultColor = System.Drawing.Color.White;
			this.dspVolume.DefaultText = "0.00";
			this.dspVolume.DisplayFactor = 1;
			this.dspVolume.Dock = System.Windows.Forms.DockStyle.Left;
			this.dspVolume.DownColor = System.Drawing.Color.Red;
			this.dspVolume.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspVolume.IsCurrency = false;
			this.dspVolume.IsFraction = false;
			this.dspVolume.Location = new System.Drawing.Point(24, 0);
			this.dspVolume.MaxDecimal = 7;
			this.dspVolume.Name = "dspVolume";
			this.dspVolume.Padding = new System.Windows.Forms.Padding(0, 1, 0, 0);
			this.dspVolume.Size = new System.Drawing.Size(31, 14);
			this.dspVolume.TabIndex = 16;
			this.dspVolume.Text = "0.00";
			this.dspVolume.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.dspVolume.TickSize = 0.01;
			this.dspVolume.UpColor = System.Drawing.Color.LimeGreen;
			this.dspVolume.Value = "";
			// 
			// lblVolume
			// 
			this.lblVolume.AutoSize = true;
			this.lblVolume.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblVolume.ForeColor = System.Drawing.Color.Khaki;
			this.lblVolume.Location = new System.Drawing.Point(0, 0);
			this.lblVolume.MaximumSize = new System.Drawing.Size(24, 14);
			this.lblVolume.MinimumSize = new System.Drawing.Size(24, 14);
			this.lblVolume.Name = "lblVolume";
			this.lblVolume.Padding = new System.Windows.Forms.Padding(0, 1, 0, 0);
			this.lblVolume.Size = new System.Drawing.Size(24, 14);
			this.lblVolume.TabIndex = 10;
			this.lblVolume.Text = "vol";
			this.lblVolume.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// panelUpper
			// 
			this.panelUpper.Controls.Add(this.dspPctChange);
			this.panelUpper.Controls.Add(this.lblPctChange);
			this.panelUpper.Controls.Add(this.dspNetChange);
			this.panelUpper.Controls.Add(this.lblNetChange);
			this.panelUpper.Controls.Add(this.AskPriceSizePair);
			this.panelUpper.Controls.Add(this.BidPriceSizePair);
			this.panelUpper.Dock = System.Windows.Forms.DockStyle.Top;
			this.panelUpper.Location = new System.Drawing.Point(76, 0);
			this.panelUpper.Name = "panelUpper";
			this.panelUpper.Size = new System.Drawing.Size(304, 27);
			this.panelUpper.TabIndex = 3;
			// 
			// dspPctChange
			// 
			this.dspPctChange.AutoColor = false;
			this.dspPctChange.AutoSize = true;
			this.dspPctChange.DefaultColor = System.Drawing.Color.White;
			this.dspPctChange.DefaultText = "0.00%";
			this.dspPctChange.DisplayFactor = 1;
			this.dspPctChange.Dock = System.Windows.Forms.DockStyle.Left;
			this.dspPctChange.DownColor = System.Drawing.Color.Red;
			this.dspPctChange.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspPctChange.IsCurrency = false;
			this.dspPctChange.IsFraction = false;
			this.dspPctChange.Location = new System.Drawing.Point(197, 0);
			this.dspPctChange.MaxDecimal = 7;
			this.dspPctChange.Name = "dspPctChange";
			this.dspPctChange.Padding = new System.Windows.Forms.Padding(0, 1, 0, 0);
			this.dspPctChange.Size = new System.Drawing.Size(44, 14);
			this.dspPctChange.TabIndex = 17;
			this.dspPctChange.Text = "0.00%";
			this.dspPctChange.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.dspPctChange.TickSize = 0.01;
			this.dspPctChange.UpColor = System.Drawing.Color.LimeGreen;
			this.dspPctChange.Value = "";
			// 
			// lblPctChange
			// 
			this.lblPctChange.AutoSize = true;
			this.lblPctChange.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblPctChange.ForeColor = System.Drawing.Color.Khaki;
			this.lblPctChange.Location = new System.Drawing.Point(173, 0);
			this.lblPctChange.MaximumSize = new System.Drawing.Size(24, 14);
			this.lblPctChange.MinimumSize = new System.Drawing.Size(24, 14);
			this.lblPctChange.Name = "lblPctChange";
			this.lblPctChange.Padding = new System.Windows.Forms.Padding(0, 1, 0, 0);
			this.lblPctChange.Size = new System.Drawing.Size(24, 14);
			this.lblPctChange.TabIndex = 16;
			this.lblPctChange.Text = "pct";
			this.lblPctChange.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// dspNetChange
			// 
			this.dspNetChange.AutoColor = false;
			this.dspNetChange.AutoSize = true;
			this.dspNetChange.DefaultColor = System.Drawing.Color.White;
			this.dspNetChange.DefaultText = "0.00";
			this.dspNetChange.DisplayFactor = 1;
			this.dspNetChange.Dock = System.Windows.Forms.DockStyle.Left;
			this.dspNetChange.DownColor = System.Drawing.Color.Red;
			this.dspNetChange.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspNetChange.IsCurrency = false;
			this.dspNetChange.IsFraction = false;
			this.dspNetChange.Location = new System.Drawing.Point(142, 0);
			this.dspNetChange.MaxDecimal = 7;
			this.dspNetChange.Name = "dspNetChange";
			this.dspNetChange.Padding = new System.Windows.Forms.Padding(0, 1, 0, 0);
			this.dspNetChange.Size = new System.Drawing.Size(31, 14);
			this.dspNetChange.TabIndex = 15;
			this.dspNetChange.Text = "0.00";
			this.dspNetChange.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.dspNetChange.TickSize = 0.01;
			this.dspNetChange.UpColor = System.Drawing.Color.LimeGreen;
			this.dspNetChange.Value = "";
			// 
			// lblNetChange
			// 
			this.lblNetChange.AutoSize = true;
			this.lblNetChange.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblNetChange.ForeColor = System.Drawing.Color.Khaki;
			this.lblNetChange.Location = new System.Drawing.Point(118, 0);
			this.lblNetChange.MaximumSize = new System.Drawing.Size(24, 14);
			this.lblNetChange.MinimumSize = new System.Drawing.Size(24, 14);
			this.lblNetChange.Name = "lblNetChange";
			this.lblNetChange.Padding = new System.Windows.Forms.Padding(0, 1, 0, 0);
			this.lblNetChange.Size = new System.Drawing.Size(24, 14);
			this.lblNetChange.TabIndex = 14;
			this.lblNetChange.Text = "net";
			this.lblNetChange.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// AskPriceSizePair
			// 
			this.AskPriceSizePair.BackColor = System.Drawing.Color.Black;
			this.AskPriceSizePair.Dock = System.Windows.Forms.DockStyle.Left;
			this.AskPriceSizePair.ForeColor = System.Drawing.Color.Gainsboro;
			this.AskPriceSizePair.Location = new System.Drawing.Point(60, 0);
			this.AskPriceSizePair.Name = "AskPriceSizePair";
			this.AskPriceSizePair.PZPName = "ask";
			this.AskPriceSizePair.Size = new System.Drawing.Size(58, 27);
			this.AskPriceSizePair.TabIndex = 18;
			// 
			// BidPriceSizePair
			// 
			this.BidPriceSizePair.BackColor = System.Drawing.Color.Black;
			this.BidPriceSizePair.Dock = System.Windows.Forms.DockStyle.Left;
			this.BidPriceSizePair.ForeColor = System.Drawing.Color.Gainsboro;
			this.BidPriceSizePair.Location = new System.Drawing.Point(0, 0);
			this.BidPriceSizePair.Name = "BidPriceSizePair";
			this.BidPriceSizePair.PZPName = "bid";
			this.BidPriceSizePair.Size = new System.Drawing.Size(60, 27);
			this.BidPriceSizePair.TabIndex = 19;
			// 
			// SymbolTradePair
			// 
			this.SymbolTradePair.BackColor = System.Drawing.Color.Black;
			this.SymbolTradePair.Dock = System.Windows.Forms.DockStyle.Left;
			this.SymbolTradePair.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.SymbolTradePair.ForeColor = System.Drawing.Color.Gainsboro;
			this.SymbolTradePair.Location = new System.Drawing.Point(0, 0);
			this.SymbolTradePair.Name = "SymbolTradePair";
			this.SymbolTradePair.Size = new System.Drawing.Size(76, 45);
			this.SymbolTradePair.SymbolToAdd = "";
			this.SymbolTradePair.SymbolToRemove = "";
			this.SymbolTradePair.SymbolToSubscribe = "";
			this.SymbolTradePair.TabIndex = 6;
			// 
			// managedPanelHide
			// 
			this.managedPanelHide.Location = new System.Drawing.Point(0, 0);
			this.managedPanelHide.Name = "managedPanelHide";
			this.managedPanelHide.Size = new System.Drawing.Size(0, 0);
			this.managedPanelHide.Text = "managedPanel2";
			// 
			// itemBaseTicker
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.Controls.Add(this.panelManagerTicker);
			this.Name = "itemBaseTicker";
			this.Size = new System.Drawing.Size(380, 45);
			this.panelManagerTicker.ResumeLayout(false);
			this.managedPanelMain.ResumeLayout(false);
			this.panelBottom.ResumeLayout(false);
			this.panelBottom.PerformLayout();
			this.panelUpper.ResumeLayout(false);
			this.panelUpper.PerformLayout();
			this.ResumeLayout(false);

		}

		#endregion

		private LabelEx.LabelBase dspClosePrice;
		private PanelEx.PanelManager panelManagerTicker;
		private PanelEx.ManagedPanel managedPanelMain;
		private System.Windows.Forms.Panel panelBottom;
		private System.Windows.Forms.Label lblClosePrice;
		private LabelEx.LabelBase dspHighPrice;
		private System.Windows.Forms.Label lblHighPrice;
		private LabelEx.LabelBase dspLowPrice;
		private System.Windows.Forms.Label lblLowPrice;
		private LabelEx.LabelBase dspVolume;
		private System.Windows.Forms.Label lblVolume;
		private System.Windows.Forms.Panel panelUpper;
		private LabelEx.LabelBase dspPctChange;
		private System.Windows.Forms.Label lblPctChange;
		private LabelEx.LabelBase dspNetChange;
		private System.Windows.Forms.Label lblNetChange;
		private itemBasePriceSizePair AskPriceSizePair;
		private itemBasePriceSizePair BidPriceSizePair;
		private itemBaseSymbolTradePair SymbolTradePair;
		private PanelEx.ManagedPanel managedPanelHide;
	}
}
