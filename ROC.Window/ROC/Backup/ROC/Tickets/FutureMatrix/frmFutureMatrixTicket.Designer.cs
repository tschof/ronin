namespace ROC
{
	partial class frmFutureMatrixTicket
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
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(frmFutureMatrixTicket));
			this.CaptionBar = new FormEx.VistaWindowCaptionBar();
			this.panelTicker = new System.Windows.Forms.Panel();
			this.panelMiddle = new System.Windows.Forms.Panel();
			this.dspClosePrice = new LabelEx.LabelBase();
			this.lblClosePrice = new System.Windows.Forms.Label();
			this.dspHighPrice = new LabelEx.LabelBase();
			this.lblHighPrice = new System.Windows.Forms.Label();
			this.dspLowPrice = new LabelEx.LabelBase();
			this.lblLowPrice = new System.Windows.Forms.Label();
			this.dspTradeVolume = new LabelEx.LabelBase();
			this.lblTradeVolume = new System.Windows.Forms.Label();
			this.picTradeTick = new System.Windows.Forms.PictureBox();
			this.dspTradedPrice = new LabelEx.LabelBase();
			this.lblTradePrice = new System.Windows.Forms.Label();
			this.lblSecurityStatus = new LabelEx.LabelVista();
			this.lblMarketDataError = new LabelEx.LabelVista();
			this.panelUpper = new System.Windows.Forms.Panel();
			this.dspVolume = new LabelEx.LabelBase();
			this.lblVolume = new System.Windows.Forms.Label();
			this.dspPctChange = new LabelEx.LabelBase();
			this.lblPctChange = new System.Windows.Forms.Label();
			this.dspNetChange = new LabelEx.LabelBase();
			this.lblNetChange = new System.Windows.Forms.Label();
			this.dspAskSize = new LabelEx.LabelBase();
			this.lblAskSize = new System.Windows.Forms.Label();
			this.dspAskPrice = new LabelEx.LabelBase();
			this.lblName = new System.Windows.Forms.Label();
			this.dspBidSize = new LabelEx.LabelBase();
			this.lblBidSize = new System.Windows.Forms.Label();
			this.dspBidPrice = new LabelEx.LabelBase();
			this.lblBidPrice = new System.Windows.Forms.Label();
			this.panelSymbolTrade = new System.Windows.Forms.Panel();
			this.cboMatrixInterval = new System.Windows.Forms.ComboBox();
			this.cboMatrixRange = new System.Windows.Forms.ComboBox();
			this.cboSymbolDetails = new System.Windows.Forms.ComboBox();
			this.imageListTradeTick = new System.Windows.Forms.ImageList(this.components);
			this.panelGrid = new PanelEx.VistaPanel();
			this.menuProcessing = new ROC.menuBaseProcessing();
			this.panelTicker.SuspendLayout();
			this.panelMiddle.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.picTradeTick)).BeginInit();
			this.panelUpper.SuspendLayout();
			this.panelSymbolTrade.SuspendLayout();
			this.panelGrid.SuspendLayout();
			this.SuspendLayout();
			// 
			// CaptionBar
			// 
			this.CaptionBar.BackColor = System.Drawing.Color.Transparent;
			this.CaptionBar.Dock = System.Windows.Forms.DockStyle.Top;
			this.CaptionBar.Location = new System.Drawing.Point(0, 0);
			this.CaptionBar.Name = "CaptionBar";
			this.CaptionBar.ShowFit = true;
			this.CaptionBar.ShowShowAll = true;
			this.CaptionBar.Size = new System.Drawing.Size(748, 26);
			this.CaptionBar.TabIndex = 1;
			this.CaptionBar.TabStop = false;
			// 
			// panelTicker
			// 
			this.panelTicker.BackColor = System.Drawing.Color.Black;
			this.panelTicker.Controls.Add(this.panelMiddle);
			this.panelTicker.Controls.Add(this.panelUpper);
			this.panelTicker.Controls.Add(this.panelSymbolTrade);
			this.panelTicker.Dock = System.Windows.Forms.DockStyle.Top;
			this.panelTicker.ForeColor = System.Drawing.Color.Gainsboro;
			this.panelTicker.Location = new System.Drawing.Point(0, 26);
			this.panelTicker.Name = "panelTicker";
			this.panelTicker.Size = new System.Drawing.Size(748, 30);
			this.panelTicker.TabIndex = 2;
			// 
			// panelMiddle
			// 
			this.panelMiddle.AutoSize = true;
			this.panelMiddle.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.panelMiddle.Controls.Add(this.dspClosePrice);
			this.panelMiddle.Controls.Add(this.lblClosePrice);
			this.panelMiddle.Controls.Add(this.dspHighPrice);
			this.panelMiddle.Controls.Add(this.lblHighPrice);
			this.panelMiddle.Controls.Add(this.dspLowPrice);
			this.panelMiddle.Controls.Add(this.lblLowPrice);
			this.panelMiddle.Controls.Add(this.dspTradeVolume);
			this.panelMiddle.Controls.Add(this.lblTradeVolume);
			this.panelMiddle.Controls.Add(this.picTradeTick);
			this.panelMiddle.Controls.Add(this.dspTradedPrice);
			this.panelMiddle.Controls.Add(this.lblTradePrice);
			this.panelMiddle.Controls.Add(this.lblSecurityStatus);
			this.panelMiddle.Controls.Add(this.lblMarketDataError);
			this.panelMiddle.Dock = System.Windows.Forms.DockStyle.Top;
			this.panelMiddle.Location = new System.Drawing.Point(227, 15);
			this.panelMiddle.MinimumSize = new System.Drawing.Size(270, 15);
			this.panelMiddle.Name = "panelMiddle";
			this.panelMiddle.Size = new System.Drawing.Size(521, 15);
			this.panelMiddle.TabIndex = 0;
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
			this.dspClosePrice.Is64Th = false;
			this.dspClosePrice.IsCurrency = false;
			this.dspClosePrice.IsFraction = true;
			this.dspClosePrice.IsPercent = false;
			this.dspClosePrice.Location = new System.Drawing.Point(266, 0);
			this.dspClosePrice.MaxDecimal = 2;
			this.dspClosePrice.Name = "dspClosePrice";
			this.dspClosePrice.Padding = new System.Windows.Forms.Padding(0, 1, 0, 0);
			this.dspClosePrice.Size = new System.Drawing.Size(31, 14);
			this.dspClosePrice.TabIndex = 0;
			this.dspClosePrice.Text = "0.00";
			this.dspClosePrice.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.dspClosePrice.TickSize = 0.01;
			this.dspClosePrice.UpColor = System.Drawing.Color.LimeGreen;
			this.dspClosePrice.Value = "";
			// 
			// lblClosePrice
			// 
			this.lblClosePrice.AutoSize = true;
			this.lblClosePrice.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblClosePrice.ForeColor = System.Drawing.Color.Khaki;
			this.lblClosePrice.Location = new System.Drawing.Point(249, 0);
			this.lblClosePrice.MaximumSize = new System.Drawing.Size(17, 14);
			this.lblClosePrice.MinimumSize = new System.Drawing.Size(17, 14);
			this.lblClosePrice.Name = "lblClosePrice";
			this.lblClosePrice.Padding = new System.Windows.Forms.Padding(0, 1, 0, 0);
			this.lblClosePrice.Size = new System.Drawing.Size(17, 14);
			this.lblClosePrice.TabIndex = 0;
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
			this.dspHighPrice.Is64Th = false;
			this.dspHighPrice.IsCurrency = false;
			this.dspHighPrice.IsFraction = true;
			this.dspHighPrice.IsPercent = false;
			this.dspHighPrice.Location = new System.Drawing.Point(218, 0);
			this.dspHighPrice.MaxDecimal = 2;
			this.dspHighPrice.Name = "dspHighPrice";
			this.dspHighPrice.Padding = new System.Windows.Forms.Padding(0, 1, 0, 0);
			this.dspHighPrice.Size = new System.Drawing.Size(31, 14);
			this.dspHighPrice.TabIndex = 0;
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
			this.lblHighPrice.Location = new System.Drawing.Point(203, 0);
			this.lblHighPrice.MaximumSize = new System.Drawing.Size(15, 14);
			this.lblHighPrice.MinimumSize = new System.Drawing.Size(15, 14);
			this.lblHighPrice.Name = "lblHighPrice";
			this.lblHighPrice.Padding = new System.Windows.Forms.Padding(0, 1, 0, 0);
			this.lblHighPrice.Size = new System.Drawing.Size(15, 14);
			this.lblHighPrice.TabIndex = 0;
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
			this.dspLowPrice.Is64Th = false;
			this.dspLowPrice.IsCurrency = false;
			this.dspLowPrice.IsFraction = true;
			this.dspLowPrice.IsPercent = false;
			this.dspLowPrice.Location = new System.Drawing.Point(172, 0);
			this.dspLowPrice.MaxDecimal = 2;
			this.dspLowPrice.Name = "dspLowPrice";
			this.dspLowPrice.Padding = new System.Windows.Forms.Padding(0, 1, 0, 0);
			this.dspLowPrice.Size = new System.Drawing.Size(31, 14);
			this.dspLowPrice.TabIndex = 0;
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
			this.lblLowPrice.Location = new System.Drawing.Point(157, 0);
			this.lblLowPrice.MaximumSize = new System.Drawing.Size(15, 14);
			this.lblLowPrice.MinimumSize = new System.Drawing.Size(15, 14);
			this.lblLowPrice.Name = "lblLowPrice";
			this.lblLowPrice.Padding = new System.Windows.Forms.Padding(0, 1, 0, 0);
			this.lblLowPrice.Size = new System.Drawing.Size(15, 14);
			this.lblLowPrice.TabIndex = 0;
			this.lblLowPrice.Text = "lo";
			this.lblLowPrice.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// dspTradeVolume
			// 
			this.dspTradeVolume.AutoColor = false;
			this.dspTradeVolume.AutoSize = true;
			this.dspTradeVolume.DefaultColor = System.Drawing.Color.White;
			this.dspTradeVolume.DefaultText = "0";
			this.dspTradeVolume.DisplayFactor = 1;
			this.dspTradeVolume.Dock = System.Windows.Forms.DockStyle.Left;
			this.dspTradeVolume.DownColor = System.Drawing.Color.Red;
			this.dspTradeVolume.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspTradeVolume.Is64Th = false;
			this.dspTradeVolume.IsCurrency = false;
			this.dspTradeVolume.IsFraction = false;
			this.dspTradeVolume.IsPercent = false;
			this.dspTradeVolume.Location = new System.Drawing.Point(143, 0);
			this.dspTradeVolume.MaxDecimal = 0;
			this.dspTradeVolume.Name = "dspTradeVolume";
			this.dspTradeVolume.Padding = new System.Windows.Forms.Padding(0, 1, 0, 0);
			this.dspTradeVolume.Size = new System.Drawing.Size(14, 14);
			this.dspTradeVolume.TabIndex = 0;
			this.dspTradeVolume.Text = "0";
			this.dspTradeVolume.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.dspTradeVolume.TickSize = 0.01;
			this.dspTradeVolume.UpColor = System.Drawing.Color.LimeGreen;
			this.dspTradeVolume.Value = "";
			// 
			// lblTradeVolume
			// 
			this.lblTradeVolume.AutoSize = true;
			this.lblTradeVolume.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblTradeVolume.ForeColor = System.Drawing.Color.Khaki;
			this.lblTradeVolume.Location = new System.Drawing.Point(126, 0);
			this.lblTradeVolume.MaximumSize = new System.Drawing.Size(17, 14);
			this.lblTradeVolume.MinimumSize = new System.Drawing.Size(17, 14);
			this.lblTradeVolume.Name = "lblTradeVolume";
			this.lblTradeVolume.Padding = new System.Windows.Forms.Padding(0, 1, 0, 0);
			this.lblTradeVolume.Size = new System.Drawing.Size(17, 14);
			this.lblTradeVolume.TabIndex = 0;
			this.lblTradeVolume.Text = "sz";
			this.lblTradeVolume.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// picTradeTick
			// 
			this.picTradeTick.Dock = System.Windows.Forms.DockStyle.Left;
			this.picTradeTick.Location = new System.Drawing.Point(112, 0);
			this.picTradeTick.MaximumSize = new System.Drawing.Size(14, 14);
			this.picTradeTick.MinimumSize = new System.Drawing.Size(14, 14);
			this.picTradeTick.Name = "picTradeTick";
			this.picTradeTick.Padding = new System.Windows.Forms.Padding(1);
			this.picTradeTick.Size = new System.Drawing.Size(14, 14);
			this.picTradeTick.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
			this.picTradeTick.TabIndex = 0;
			this.picTradeTick.TabStop = false;
			// 
			// dspTradedPrice
			// 
			this.dspTradedPrice.AutoColor = false;
			this.dspTradedPrice.AutoSize = true;
			this.dspTradedPrice.DefaultColor = System.Drawing.Color.White;
			this.dspTradedPrice.DefaultText = "0.00";
			this.dspTradedPrice.DisplayFactor = 1;
			this.dspTradedPrice.Dock = System.Windows.Forms.DockStyle.Left;
			this.dspTradedPrice.DownColor = System.Drawing.Color.Red;
			this.dspTradedPrice.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspTradedPrice.Is64Th = false;
			this.dspTradedPrice.IsCurrency = false;
			this.dspTradedPrice.IsFraction = true;
			this.dspTradedPrice.IsPercent = false;
			this.dspTradedPrice.Location = new System.Drawing.Point(81, 0);
			this.dspTradedPrice.MaxDecimal = 2;
			this.dspTradedPrice.Name = "dspTradedPrice";
			this.dspTradedPrice.Size = new System.Drawing.Size(31, 13);
			this.dspTradedPrice.TabIndex = 0;
			this.dspTradedPrice.Text = "0.00";
			this.dspTradedPrice.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.dspTradedPrice.TickSize = 0.01;
			this.dspTradedPrice.UpColor = System.Drawing.Color.LimeGreen;
			this.dspTradedPrice.Value = "";
			// 
			// lblTradePrice
			// 
			this.lblTradePrice.AutoSize = true;
			this.lblTradePrice.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblTradePrice.ForeColor = System.Drawing.Color.Khaki;
			this.lblTradePrice.Location = new System.Drawing.Point(57, 0);
			this.lblTradePrice.MaximumSize = new System.Drawing.Size(24, 14);
			this.lblTradePrice.MinimumSize = new System.Drawing.Size(24, 14);
			this.lblTradePrice.Name = "lblTradePrice";
			this.lblTradePrice.Padding = new System.Windows.Forms.Padding(0, 1, 0, 0);
			this.lblTradePrice.Size = new System.Drawing.Size(24, 14);
			this.lblTradePrice.TabIndex = 0;
			this.lblTradePrice.Text = "last";
			this.lblTradePrice.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// lblSecurityStatus
			// 
			this.lblSecurityStatus.AutoColor = false;
			this.lblSecurityStatus.AutoSize = true;
			this.lblSecurityStatus.BlinkBackColor = System.Drawing.Color.Black;
			this.lblSecurityStatus.BlinkForeColorColor = System.Drawing.Color.Gold;
			this.lblSecurityStatus.BlinkInnerBorderColor = System.Drawing.Color.Black;
			this.lblSecurityStatus.BlinkOuterBorderColor = System.Drawing.Color.Black;
			this.lblSecurityStatus.DefaultColor = System.Drawing.Color.White;
			this.lblSecurityStatus.DefaultText = "No Market Data";
			this.lblSecurityStatus.DisplayFactor = 1;
			this.lblSecurityStatus.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblSecurityStatus.DownColor = System.Drawing.Color.Red;
			this.lblSecurityStatus.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblSecurityStatus.Is64Th = false;
			this.lblSecurityStatus.IsCurrency = false;
			this.lblSecurityStatus.IsFraction = false;
			this.lblSecurityStatus.IsPercent = false;
			this.lblSecurityStatus.Location = new System.Drawing.Point(26, 0);
			this.lblSecurityStatus.MaxDecimal = 7;
			this.lblSecurityStatus.Name = "lblSecurityStatus";
			this.lblSecurityStatus.OuterBorderColor = System.Drawing.Color.Black;
			this.lblSecurityStatus.Size = new System.Drawing.Size(31, 13);
			this.lblSecurityStatus.TabIndex = 2;
			this.lblSecurityStatus.Text = "STA";
			this.lblSecurityStatus.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			this.lblSecurityStatus.TickSize = 0.01;
			this.lblSecurityStatus.UpColor = System.Drawing.Color.LimeGreen;
			this.lblSecurityStatus.Value = "";
			this.lblSecurityStatus.Visible = false;
			// 
			// lblMarketDataError
			// 
			this.lblMarketDataError.AutoColor = false;
			this.lblMarketDataError.AutoSize = true;
			this.lblMarketDataError.BlinkBackColor = System.Drawing.Color.Black;
			this.lblMarketDataError.BlinkForeColorColor = System.Drawing.Color.Red;
			this.lblMarketDataError.BlinkInnerBorderColor = System.Drawing.Color.Black;
			this.lblMarketDataError.BlinkOuterBorderColor = System.Drawing.Color.Black;
			this.lblMarketDataError.DefaultColor = System.Drawing.Color.White;
			this.lblMarketDataError.DefaultText = "No Market Data";
			this.lblMarketDataError.DisplayFactor = 1;
			this.lblMarketDataError.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblMarketDataError.DownColor = System.Drawing.Color.Red;
			this.lblMarketDataError.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblMarketDataError.Is64Th = false;
			this.lblMarketDataError.IsCurrency = false;
			this.lblMarketDataError.IsFraction = false;
			this.lblMarketDataError.IsPercent = false;
			this.lblMarketDataError.Location = new System.Drawing.Point(0, 0);
			this.lblMarketDataError.MaxDecimal = 7;
			this.lblMarketDataError.Name = "lblMarketDataError";
			this.lblMarketDataError.OuterBorderColor = System.Drawing.Color.Black;
			this.lblMarketDataError.Size = new System.Drawing.Size(26, 13);
			this.lblMarketDataError.TabIndex = 0;
			this.lblMarketDataError.Text = "MD";
			this.lblMarketDataError.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			this.lblMarketDataError.TickSize = 0.01;
			this.lblMarketDataError.UpColor = System.Drawing.Color.LimeGreen;
			this.lblMarketDataError.Value = "";
			this.lblMarketDataError.Visible = false;
			// 
			// panelUpper
			// 
			this.panelUpper.AutoSize = true;
			this.panelUpper.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.panelUpper.Controls.Add(this.dspVolume);
			this.panelUpper.Controls.Add(this.lblVolume);
			this.panelUpper.Controls.Add(this.dspPctChange);
			this.panelUpper.Controls.Add(this.lblPctChange);
			this.panelUpper.Controls.Add(this.dspNetChange);
			this.panelUpper.Controls.Add(this.lblNetChange);
			this.panelUpper.Controls.Add(this.dspAskSize);
			this.panelUpper.Controls.Add(this.lblAskSize);
			this.panelUpper.Controls.Add(this.dspAskPrice);
			this.panelUpper.Controls.Add(this.lblName);
			this.panelUpper.Controls.Add(this.dspBidSize);
			this.panelUpper.Controls.Add(this.lblBidSize);
			this.panelUpper.Controls.Add(this.dspBidPrice);
			this.panelUpper.Controls.Add(this.lblBidPrice);
			this.panelUpper.Dock = System.Windows.Forms.DockStyle.Top;
			this.panelUpper.Location = new System.Drawing.Point(227, 0);
			this.panelUpper.MinimumSize = new System.Drawing.Size(270, 15);
			this.panelUpper.Name = "panelUpper";
			this.panelUpper.Size = new System.Drawing.Size(521, 15);
			this.panelUpper.TabIndex = 0;
			// 
			// dspVolume
			// 
			this.dspVolume.AutoColor = false;
			this.dspVolume.AutoSize = true;
			this.dspVolume.DefaultColor = System.Drawing.Color.White;
			this.dspVolume.DefaultText = "0";
			this.dspVolume.DisplayFactor = 1;
			this.dspVolume.Dock = System.Windows.Forms.DockStyle.Left;
			this.dspVolume.DownColor = System.Drawing.Color.Red;
			this.dspVolume.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspVolume.Is64Th = false;
			this.dspVolume.IsCurrency = false;
			this.dspVolume.IsFraction = false;
			this.dspVolume.IsPercent = false;
			this.dspVolume.Location = new System.Drawing.Point(309, 0);
			this.dspVolume.MaxDecimal = 0;
			this.dspVolume.Name = "dspVolume";
			this.dspVolume.Padding = new System.Windows.Forms.Padding(0, 1, 0, 0);
			this.dspVolume.Size = new System.Drawing.Size(14, 14);
			this.dspVolume.TabIndex = 0;
			this.dspVolume.Text = "0";
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
			this.lblVolume.Location = new System.Drawing.Point(287, 0);
			this.lblVolume.MaximumSize = new System.Drawing.Size(22, 14);
			this.lblVolume.MinimumSize = new System.Drawing.Size(22, 14);
			this.lblVolume.Name = "lblVolume";
			this.lblVolume.Padding = new System.Windows.Forms.Padding(0, 1, 0, 0);
			this.lblVolume.Size = new System.Drawing.Size(22, 14);
			this.lblVolume.TabIndex = 0;
			this.lblVolume.Text = "vol";
			this.lblVolume.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// dspPctChange
			// 
			this.dspPctChange.AutoColor = true;
			this.dspPctChange.AutoSize = true;
			this.dspPctChange.DefaultColor = System.Drawing.Color.White;
			this.dspPctChange.DefaultText = "0.00%";
			this.dspPctChange.DisplayFactor = 1;
			this.dspPctChange.Dock = System.Windows.Forms.DockStyle.Left;
			this.dspPctChange.DownColor = System.Drawing.Color.Red;
			this.dspPctChange.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspPctChange.Is64Th = false;
			this.dspPctChange.IsCurrency = false;
			this.dspPctChange.IsFraction = false;
			this.dspPctChange.IsPercent = true;
			this.dspPctChange.Location = new System.Drawing.Point(243, 0);
			this.dspPctChange.MaxDecimal = 2;
			this.dspPctChange.Name = "dspPctChange";
			this.dspPctChange.Padding = new System.Windows.Forms.Padding(0, 1, 0, 0);
			this.dspPctChange.Size = new System.Drawing.Size(44, 14);
			this.dspPctChange.TabIndex = 0;
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
			this.lblPctChange.Location = new System.Drawing.Point(221, 0);
			this.lblPctChange.MaximumSize = new System.Drawing.Size(22, 14);
			this.lblPctChange.MinimumSize = new System.Drawing.Size(22, 14);
			this.lblPctChange.Name = "lblPctChange";
			this.lblPctChange.Padding = new System.Windows.Forms.Padding(0, 1, 0, 0);
			this.lblPctChange.Size = new System.Drawing.Size(22, 14);
			this.lblPctChange.TabIndex = 0;
			this.lblPctChange.Text = "pct";
			this.lblPctChange.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// dspNetChange
			// 
			this.dspNetChange.AutoColor = true;
			this.dspNetChange.AutoSize = true;
			this.dspNetChange.DefaultColor = System.Drawing.Color.White;
			this.dspNetChange.DefaultText = "0.00";
			this.dspNetChange.DisplayFactor = 1;
			this.dspNetChange.Dock = System.Windows.Forms.DockStyle.Left;
			this.dspNetChange.DownColor = System.Drawing.Color.Red;
			this.dspNetChange.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspNetChange.Is64Th = false;
			this.dspNetChange.IsCurrency = false;
			this.dspNetChange.IsFraction = true;
			this.dspNetChange.IsPercent = false;
			this.dspNetChange.Location = new System.Drawing.Point(190, 0);
			this.dspNetChange.MaxDecimal = 2;
			this.dspNetChange.Name = "dspNetChange";
			this.dspNetChange.Padding = new System.Windows.Forms.Padding(0, 1, 0, 0);
			this.dspNetChange.Size = new System.Drawing.Size(31, 14);
			this.dspNetChange.TabIndex = 0;
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
			this.lblNetChange.Location = new System.Drawing.Point(168, 0);
			this.lblNetChange.MaximumSize = new System.Drawing.Size(22, 14);
			this.lblNetChange.MinimumSize = new System.Drawing.Size(22, 14);
			this.lblNetChange.Name = "lblNetChange";
			this.lblNetChange.Padding = new System.Windows.Forms.Padding(0, 1, 0, 0);
			this.lblNetChange.Size = new System.Drawing.Size(22, 14);
			this.lblNetChange.TabIndex = 0;
			this.lblNetChange.Text = "net";
			this.lblNetChange.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// dspAskSize
			// 
			this.dspAskSize.AutoColor = false;
			this.dspAskSize.AutoSize = true;
			this.dspAskSize.DefaultColor = System.Drawing.Color.White;
			this.dspAskSize.DefaultText = "0";
			this.dspAskSize.DisplayFactor = 1;
			this.dspAskSize.Dock = System.Windows.Forms.DockStyle.Left;
			this.dspAskSize.DownColor = System.Drawing.Color.Red;
			this.dspAskSize.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspAskSize.Is64Th = false;
			this.dspAskSize.IsCurrency = false;
			this.dspAskSize.IsFraction = false;
			this.dspAskSize.IsPercent = false;
			this.dspAskSize.Location = new System.Drawing.Point(155, 0);
			this.dspAskSize.MaxDecimal = 7;
			this.dspAskSize.Name = "dspAskSize";
			this.dspAskSize.Size = new System.Drawing.Size(13, 13);
			this.dspAskSize.TabIndex = 0;
			this.dspAskSize.Text = "0";
			this.dspAskSize.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.dspAskSize.TickSize = 0.01;
			this.dspAskSize.UpColor = System.Drawing.Color.LimeGreen;
			this.dspAskSize.Value = "";
			// 
			// lblAskSize
			// 
			this.lblAskSize.AutoSize = true;
			this.lblAskSize.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblAskSize.ForeColor = System.Drawing.Color.Khaki;
			this.lblAskSize.Location = new System.Drawing.Point(138, 0);
			this.lblAskSize.MaximumSize = new System.Drawing.Size(17, 14);
			this.lblAskSize.MinimumSize = new System.Drawing.Size(17, 14);
			this.lblAskSize.Name = "lblAskSize";
			this.lblAskSize.Padding = new System.Windows.Forms.Padding(0, 1, 0, 0);
			this.lblAskSize.Size = new System.Drawing.Size(17, 14);
			this.lblAskSize.TabIndex = 0;
			this.lblAskSize.Text = "sz";
			this.lblAskSize.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// dspAskPrice
			// 
			this.dspAskPrice.AutoColor = false;
			this.dspAskPrice.AutoSize = true;
			this.dspAskPrice.DefaultColor = System.Drawing.Color.White;
			this.dspAskPrice.DefaultText = "0.00";
			this.dspAskPrice.DisplayFactor = 1;
			this.dspAskPrice.Dock = System.Windows.Forms.DockStyle.Left;
			this.dspAskPrice.DownColor = System.Drawing.Color.Red;
			this.dspAskPrice.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspAskPrice.Is64Th = false;
			this.dspAskPrice.IsCurrency = false;
			this.dspAskPrice.IsFraction = true;
			this.dspAskPrice.IsPercent = false;
			this.dspAskPrice.Location = new System.Drawing.Point(107, 0);
			this.dspAskPrice.MaxDecimal = 2;
			this.dspAskPrice.Name = "dspAskPrice";
			this.dspAskPrice.Size = new System.Drawing.Size(31, 13);
			this.dspAskPrice.TabIndex = 0;
			this.dspAskPrice.Text = "0.00";
			this.dspAskPrice.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.dspAskPrice.TickSize = 0.01;
			this.dspAskPrice.UpColor = System.Drawing.Color.LimeGreen;
			this.dspAskPrice.Value = "";
			// 
			// lblName
			// 
			this.lblName.AutoSize = true;
			this.lblName.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblName.ForeColor = System.Drawing.Color.Khaki;
			this.lblName.Location = new System.Drawing.Point(83, 0);
			this.lblName.MaximumSize = new System.Drawing.Size(24, 14);
			this.lblName.MinimumSize = new System.Drawing.Size(24, 14);
			this.lblName.Name = "lblName";
			this.lblName.Padding = new System.Windows.Forms.Padding(0, 1, 0, 0);
			this.lblName.Size = new System.Drawing.Size(24, 14);
			this.lblName.TabIndex = 0;
			this.lblName.Text = "ask";
			this.lblName.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// dspBidSize
			// 
			this.dspBidSize.AutoColor = false;
			this.dspBidSize.AutoSize = true;
			this.dspBidSize.DefaultColor = System.Drawing.Color.White;
			this.dspBidSize.DefaultText = "0";
			this.dspBidSize.DisplayFactor = 1;
			this.dspBidSize.Dock = System.Windows.Forms.DockStyle.Left;
			this.dspBidSize.DownColor = System.Drawing.Color.Red;
			this.dspBidSize.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspBidSize.Is64Th = false;
			this.dspBidSize.IsCurrency = false;
			this.dspBidSize.IsFraction = false;
			this.dspBidSize.IsPercent = false;
			this.dspBidSize.Location = new System.Drawing.Point(70, 0);
			this.dspBidSize.MaxDecimal = 7;
			this.dspBidSize.Name = "dspBidSize";
			this.dspBidSize.Size = new System.Drawing.Size(13, 13);
			this.dspBidSize.TabIndex = 0;
			this.dspBidSize.Text = "0";
			this.dspBidSize.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.dspBidSize.TickSize = 0.01;
			this.dspBidSize.UpColor = System.Drawing.Color.LimeGreen;
			this.dspBidSize.Value = "";
			// 
			// lblBidSize
			// 
			this.lblBidSize.AutoSize = true;
			this.lblBidSize.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblBidSize.ForeColor = System.Drawing.Color.Khaki;
			this.lblBidSize.Location = new System.Drawing.Point(53, 0);
			this.lblBidSize.MaximumSize = new System.Drawing.Size(17, 14);
			this.lblBidSize.MinimumSize = new System.Drawing.Size(17, 14);
			this.lblBidSize.Name = "lblBidSize";
			this.lblBidSize.Padding = new System.Windows.Forms.Padding(0, 1, 0, 0);
			this.lblBidSize.Size = new System.Drawing.Size(17, 14);
			this.lblBidSize.TabIndex = 0;
			this.lblBidSize.Text = "sz";
			this.lblBidSize.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// dspBidPrice
			// 
			this.dspBidPrice.AutoColor = false;
			this.dspBidPrice.AutoSize = true;
			this.dspBidPrice.DefaultColor = System.Drawing.Color.White;
			this.dspBidPrice.DefaultText = "0.00";
			this.dspBidPrice.DisplayFactor = 1;
			this.dspBidPrice.Dock = System.Windows.Forms.DockStyle.Left;
			this.dspBidPrice.DownColor = System.Drawing.Color.Red;
			this.dspBidPrice.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspBidPrice.Is64Th = false;
			this.dspBidPrice.IsCurrency = false;
			this.dspBidPrice.IsFraction = true;
			this.dspBidPrice.IsPercent = false;
			this.dspBidPrice.Location = new System.Drawing.Point(22, 0);
			this.dspBidPrice.MaxDecimal = 2;
			this.dspBidPrice.Name = "dspBidPrice";
			this.dspBidPrice.Size = new System.Drawing.Size(31, 13);
			this.dspBidPrice.TabIndex = 0;
			this.dspBidPrice.Text = "0.00";
			this.dspBidPrice.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.dspBidPrice.TickSize = 0.01;
			this.dspBidPrice.UpColor = System.Drawing.Color.LimeGreen;
			this.dspBidPrice.Value = "";
			// 
			// lblBidPrice
			// 
			this.lblBidPrice.AutoSize = true;
			this.lblBidPrice.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblBidPrice.ForeColor = System.Drawing.Color.Khaki;
			this.lblBidPrice.Location = new System.Drawing.Point(0, 0);
			this.lblBidPrice.MaximumSize = new System.Drawing.Size(22, 14);
			this.lblBidPrice.MinimumSize = new System.Drawing.Size(22, 14);
			this.lblBidPrice.Name = "lblBidPrice";
			this.lblBidPrice.Padding = new System.Windows.Forms.Padding(0, 1, 0, 0);
			this.lblBidPrice.Size = new System.Drawing.Size(22, 14);
			this.lblBidPrice.TabIndex = 0;
			this.lblBidPrice.Text = "bid";
			this.lblBidPrice.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// panelSymbolTrade
			// 
			this.panelSymbolTrade.BackColor = System.Drawing.Color.LightSteelBlue;
			this.panelSymbolTrade.Controls.Add(this.cboMatrixInterval);
			this.panelSymbolTrade.Controls.Add(this.cboMatrixRange);
			this.panelSymbolTrade.Controls.Add(this.cboSymbolDetails);
			this.panelSymbolTrade.Dock = System.Windows.Forms.DockStyle.Left;
			this.panelSymbolTrade.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.panelSymbolTrade.Location = new System.Drawing.Point(0, 0);
			this.panelSymbolTrade.Name = "panelSymbolTrade";
			this.panelSymbolTrade.Padding = new System.Windows.Forms.Padding(4, 4, 4, 0);
			this.panelSymbolTrade.Size = new System.Drawing.Size(227, 30);
			this.panelSymbolTrade.TabIndex = 0;
			// 
			// cboMatrixInterval
			// 
			this.cboMatrixInterval.BackColor = System.Drawing.Color.White;
			this.cboMatrixInterval.Font = new System.Drawing.Font("Tahoma", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cboMatrixInterval.ForeColor = System.Drawing.Color.Black;
			this.cboMatrixInterval.FormattingEnabled = true;
			this.cboMatrixInterval.Items.AddRange(new object[] {
            "Monthly",
            "Quarterly"});
			this.cboMatrixInterval.Location = new System.Drawing.Point(99, 4);
			this.cboMatrixInterval.Name = "cboMatrixInterval";
			this.cboMatrixInterval.Size = new System.Drawing.Size(84, 22);
			this.cboMatrixInterval.TabIndex = 1;
			this.cboMatrixInterval.TabStop = false;
			this.cboMatrixInterval.Text = "Quarterly";
			this.cboMatrixInterval.SelectedIndexChanged += new System.EventHandler(this.cboMatrix_SelectedIndexChanged);
			// 
			// cboMatrixRange
			// 
			this.cboMatrixRange.BackColor = System.Drawing.Color.White;
			this.cboMatrixRange.Dock = System.Windows.Forms.DockStyle.Right;
			this.cboMatrixRange.Font = new System.Drawing.Font("Tahoma", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cboMatrixRange.ForeColor = System.Drawing.Color.Black;
			this.cboMatrixRange.FormattingEnabled = true;
			this.cboMatrixRange.Items.AddRange(new object[] {
            "6",
            "7",
            "8",
            "9",
            "10",
            "11",
            "12"});
			this.cboMatrixRange.Location = new System.Drawing.Point(183, 4);
			this.cboMatrixRange.Name = "cboMatrixRange";
			this.cboMatrixRange.Size = new System.Drawing.Size(40, 22);
			this.cboMatrixRange.TabIndex = 0;
			this.cboMatrixRange.TabStop = false;
			this.cboMatrixRange.Text = "9";
			this.cboMatrixRange.SelectedIndexChanged += new System.EventHandler(this.cboMatrix_SelectedIndexChanged);
			// 
			// cboSymbolDetails
			// 
			this.cboSymbolDetails.BackColor = System.Drawing.Color.Black;
			this.cboSymbolDetails.Dock = System.Windows.Forms.DockStyle.Left;
			this.cboSymbolDetails.Font = new System.Drawing.Font("Tahoma", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cboSymbolDetails.ForeColor = System.Drawing.Color.Gold;
			this.cboSymbolDetails.FormattingEnabled = true;
			this.cboSymbolDetails.Location = new System.Drawing.Point(4, 4);
			this.cboSymbolDetails.Name = "cboSymbolDetails";
			this.cboSymbolDetails.Size = new System.Drawing.Size(92, 22);
			this.cboSymbolDetails.TabIndex = 0;
			this.cboSymbolDetails.KeyUp += new System.Windows.Forms.KeyEventHandler(this.cboSymbolDetails_KeyUp);
			this.cboSymbolDetails.KeyDown += new System.Windows.Forms.KeyEventHandler(this.cboSymbolDetails_KeyDown);
			// 
			// imageListTradeTick
			// 
			this.imageListTradeTick.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageListTradeTick.ImageStream")));
			this.imageListTradeTick.TransparentColor = System.Drawing.Color.Transparent;
			this.imageListTradeTick.Images.SetKeyName(0, "UpGreen.png");
			this.imageListTradeTick.Images.SetKeyName(1, "Blank.png");
			this.imageListTradeTick.Images.SetKeyName(2, "DownRed.png");
			this.imageListTradeTick.Images.SetKeyName(3, "Cancel.png");
			// 
			// panelGrid
			// 
			this.panelGrid.BackColor = System.Drawing.Color.Black;
			this.panelGrid.Controls.Add(this.menuProcessing);
			this.panelGrid.Dock = System.Windows.Forms.DockStyle.Fill;
			this.panelGrid.Location = new System.Drawing.Point(0, 56);
			this.panelGrid.Name = "panelGrid";
			this.panelGrid.Size = new System.Drawing.Size(748, 186);
			this.panelGrid.TabIndex = 3;
			// 
			// menuProcessing
			// 
			this.menuProcessing.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
			this.menuProcessing.BackColor = System.Drawing.Color.Transparent;
			this.menuProcessing.Caption = "000";
			this.menuProcessing.Location = new System.Drawing.Point(12, 123);
			this.menuProcessing.MaximumSize = new System.Drawing.Size(51, 51);
			this.menuProcessing.MinimumSize = new System.Drawing.Size(51, 51);
			this.menuProcessing.Name = "menuProcessing";
			this.menuProcessing.Size = new System.Drawing.Size(51, 51);
			this.menuProcessing.TabIndex = 1;
			this.menuProcessing.TabStop = false;
			this.menuProcessing.Visible = false;
			// 
			// frmFutureMatrixTicket
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.BackColor = System.Drawing.SystemColors.Control;
			this.Caption = "Future Matrix Ticket";
			this.ClientSize = new System.Drawing.Size(748, 242);
			this.Controls.Add(this.panelGrid);
			this.Controls.Add(this.panelTicker);
			this.Controls.Add(this.CaptionBar);
			this.MaximumSize = new System.Drawing.Size(2000, 1600);
			this.MinimumSize = new System.Drawing.Size(350, 250);
			this.Name = "frmFutureMatrixTicket";
			this.Opacity = 0.5;
			this.ShowInTaskbar = false;
			this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
			this.panelTicker.ResumeLayout(false);
			this.panelTicker.PerformLayout();
			this.panelMiddle.ResumeLayout(false);
			this.panelMiddle.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.picTradeTick)).EndInit();
			this.panelUpper.ResumeLayout(false);
			this.panelUpper.PerformLayout();
			this.panelSymbolTrade.ResumeLayout(false);
			this.panelGrid.ResumeLayout(false);
			this.ResumeLayout(false);

		}

		#endregion

		private FormEx.VistaWindowCaptionBar CaptionBar;
		private System.Windows.Forms.Panel panelTicker;
		private System.Windows.Forms.Panel panelMiddle;
		private LabelEx.LabelBase dspClosePrice;
		private System.Windows.Forms.Label lblClosePrice;
		private LabelEx.LabelBase dspHighPrice;
		private System.Windows.Forms.Label lblHighPrice;
		private LabelEx.LabelBase dspLowPrice;
		private System.Windows.Forms.Label lblLowPrice;
		private LabelEx.LabelBase dspTradeVolume;
		private System.Windows.Forms.Label lblTradeVolume;
		private System.Windows.Forms.PictureBox picTradeTick;
		private LabelEx.LabelBase dspTradedPrice;
		private System.Windows.Forms.Label lblTradePrice;
		private LabelEx.LabelVista lblSecurityStatus;
		private LabelEx.LabelVista lblMarketDataError;
		private System.Windows.Forms.Panel panelUpper;
		private LabelEx.LabelBase dspVolume;
		private System.Windows.Forms.Label lblVolume;
		private LabelEx.LabelBase dspPctChange;
		private System.Windows.Forms.Label lblPctChange;
		private LabelEx.LabelBase dspNetChange;
		private System.Windows.Forms.Label lblNetChange;
		private LabelEx.LabelBase dspAskSize;
		private System.Windows.Forms.Label lblAskSize;
		private LabelEx.LabelBase dspAskPrice;
		private System.Windows.Forms.Label lblName;
		private LabelEx.LabelBase dspBidSize;
		private System.Windows.Forms.Label lblBidSize;
		private LabelEx.LabelBase dspBidPrice;
		private System.Windows.Forms.Label lblBidPrice;
		private System.Windows.Forms.Panel panelSymbolTrade;
		protected internal System.Windows.Forms.ComboBox cboSymbolDetails;
		private System.Windows.Forms.ImageList imageListTradeTick;
		protected internal System.Windows.Forms.ComboBox cboMatrixRange;
		protected internal System.Windows.Forms.ComboBox cboMatrixInterval;
		private PanelEx.VistaPanel panelGrid;
		private menuBaseProcessing menuProcessing;
	}
}