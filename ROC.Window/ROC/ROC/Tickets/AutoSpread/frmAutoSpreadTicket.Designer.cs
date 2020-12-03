namespace ROC
{
	partial class frmAutoSpreadTicket
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
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(frmAutoSpreadTicket));
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
			this.CaptionBar = new FormEx.VistaWindowCaptionBar();
			this.toolStripTraderInfo = new System.Windows.Forms.ToolStrip();
			this.lblTradeFor = new System.Windows.Forms.ToolStripLabel();
			this.cmdTradeFor = new System.Windows.Forms.ToolStripDropDownButton();
			this.lblLocalAccountAcrn = new System.Windows.Forms.ToolStripLabel();
			this.cmdAccount = new System.Windows.Forms.ToolStripDropDownButton();
			this.panelUpper = new System.Windows.Forms.Panel();
			this.panelMISC = new System.Windows.Forms.Panel();
			this.panelPriceBottom = new System.Windows.Forms.Panel();
			this.dspTradedPrice = new LabelEx.LabelBase();
			this.lblTradedPrice = new System.Windows.Forms.Label();
			this.lblMarketDataError = new LabelEx.LabelVista();
			this.panelPriceTop = new System.Windows.Forms.Panel();
			this.dspAskPrice = new LabelEx.LabelBase();
			this.lblAskPrice = new System.Windows.Forms.Label();
			this.dspBidPrice = new LabelEx.LabelBase();
			this.lblBidPrice = new System.Windows.Forms.Label();
			this.panelLegs = new System.Windows.Forms.Panel();
			this.lblLegs = new LabelEx.LabelBase();
			this.dspLegs = new System.Windows.Forms.Label();
			this.panelTickSize = new System.Windows.Forms.Panel();
			this.numTickSize = new System.Windows.Forms.NumericUpDown();
			this.dspTickSize = new System.Windows.Forms.Label();
			this.panelTicketToggle = new System.Windows.Forms.Panel();
			this.cmdSelectSettingGrid = new LabelEx.LabelVista();
			this.cmdSelectLadder = new LabelEx.LabelVista();
			this.panelOrderInfo = new System.Windows.Forms.Panel();
			this.panelLegInfo = new System.Windows.Forms.Panel();
			this.dspSelectedLegPrice = new System.Windows.Forms.RichTextBox();
			this.panelOrderInfoMsg = new System.Windows.Forms.Panel();
			this.dspOrderMsg = new System.Windows.Forms.RichTextBox();
			this.panelOrderInfoUpper = new System.Windows.Forms.Panel();
			this.panelExchange = new System.Windows.Forms.Panel();
			this.cboExchange = new System.Windows.Forms.ComboBox();
			this.lblExchange = new System.Windows.Forms.Label();
			this.panelDuration = new System.Windows.Forms.Panel();
			this.cboDuration = new System.Windows.Forms.ComboBox();
			this.lblDuration = new System.Windows.Forms.Label();
			this.panelStopPrice = new System.Windows.Forms.Panel();
			this.dspStopPriceTickSize = new LabelEx.LabelBase();
			this.lblStopPriceX = new LabelEx.LabelBase();
			this.dspStopPriceIncrement = new LabelEx.LabelBase();
			this.numStopPrice = new System.Windows.Forms.NumericUpDown();
			this.lblStopPrice = new System.Windows.Forms.Label();
			this.panelLimitPrice = new System.Windows.Forms.Panel();
			this.dspLimitPriceTickSize = new LabelEx.LabelBase();
			this.lblLimitPriceX = new LabelEx.LabelBase();
			this.dspLimitPriceIncrement = new LabelEx.LabelBase();
			this.numLimitPrice = new System.Windows.Forms.NumericUpDown();
			this.lblLimitPrice = new System.Windows.Forms.Label();
			this.panelOrderTypes = new System.Windows.Forms.Panel();
			this.cboOrder = new System.Windows.Forms.ComboBox();
			this.lblOrderType = new System.Windows.Forms.Label();
			this.panelShares = new System.Windows.Forms.Panel();
			this.dspQuantityIncrement = new LabelEx.LabelBase();
			this.numQuantity = new System.Windows.Forms.NumericUpDown();
			this.lblQuantity = new System.Windows.Forms.Label();
			this.panelCommands = new System.Windows.Forms.Panel();
			this.cmdSell = new ButtonEx.VistaButton();
			this.cmdBuy = new ButtonEx.VistaButton();
			this.panelManagerQuick = new PanelEx.PanelManager();
			this.managedPanelBook = new PanelEx.ManagedPanel();
			this.dspNetOpen = new LabelEx.LabelVista();
			this.dspSellOpenQty = new LabelEx.LabelVista();
			this.dspBuyOpenQty = new LabelEx.LabelVista();
			this.cmdCenterLow = new ButtonEx.VistaButton();
			this.cmdCenterTrade = new ButtonEx.VistaButton();
			this.cmdCancelAllSell = new ButtonEx.VistaButton();
			this.cmdCancelAll = new ButtonEx.VistaButton();
			this.cmdCancelAllBuy = new ButtonEx.VistaButton();
			this.cmdCenterHigh = new ButtonEx.VistaButton();
			this.splitContainer = new System.Windows.Forms.SplitContainer();
			this.rocAutoSpreadList = new DataGridViewEx.ROCAutoSpreadList();
			this.rocAutoSpreadListSettings = new DataGridViewEx.ROCAutoSpreadListSettings();
			this.toolStripTraderInfo.SuspendLayout();
			this.panelUpper.SuspendLayout();
			this.panelMISC.SuspendLayout();
			this.panelPriceBottom.SuspendLayout();
			this.panelPriceTop.SuspendLayout();
			this.panelLegs.SuspendLayout();
			this.panelTickSize.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.numTickSize)).BeginInit();
			this.panelTicketToggle.SuspendLayout();
			this.panelOrderInfo.SuspendLayout();
			this.panelLegInfo.SuspendLayout();
			this.panelOrderInfoMsg.SuspendLayout();
			this.panelOrderInfoUpper.SuspendLayout();
			this.panelExchange.SuspendLayout();
			this.panelDuration.SuspendLayout();
			this.panelStopPrice.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.numStopPrice)).BeginInit();
			this.panelLimitPrice.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.numLimitPrice)).BeginInit();
			this.panelOrderTypes.SuspendLayout();
			this.panelShares.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.numQuantity)).BeginInit();
			this.panelCommands.SuspendLayout();
			this.panelManagerQuick.SuspendLayout();
			this.managedPanelBook.SuspendLayout();
			this.splitContainer.Panel1.SuspendLayout();
			this.splitContainer.Panel2.SuspendLayout();
			this.splitContainer.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.rocAutoSpreadList)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.rocAutoSpreadListSettings)).BeginInit();
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
			this.CaptionBar.Size = new System.Drawing.Size(362, 26);
			this.CaptionBar.TabIndex = 0;
			this.CaptionBar.TabStop = false;
			// 
			// toolStripTraderInfo
			// 
			this.toolStripTraderInfo.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
			this.toolStripTraderInfo.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.lblTradeFor,
            this.cmdTradeFor,
            this.lblLocalAccountAcrn,
            this.cmdAccount});
			this.toolStripTraderInfo.Location = new System.Drawing.Point(0, 26);
			this.toolStripTraderInfo.Name = "toolStripTraderInfo";
			this.toolStripTraderInfo.ShowItemToolTips = false;
			this.toolStripTraderInfo.Size = new System.Drawing.Size(362, 25);
			this.toolStripTraderInfo.TabIndex = 0;
			this.toolStripTraderInfo.Text = "UserInfo";
			// 
			// lblTradeFor
			// 
			this.lblTradeFor.ForeColor = System.Drawing.Color.DimGray;
			this.lblTradeFor.Name = "lblTradeFor";
			this.lblTradeFor.Size = new System.Drawing.Size(43, 22);
			this.lblTradeFor.Text = "Trader:";
			// 
			// cmdTradeFor
			// 
			this.cmdTradeFor.AutoToolTip = false;
			this.cmdTradeFor.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
			this.cmdTradeFor.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdTradeFor.Image = ((System.Drawing.Image)(resources.GetObject("cmdTradeFor.Image")));
			this.cmdTradeFor.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.cmdTradeFor.Name = "cmdTradeFor";
			this.cmdTradeFor.Size = new System.Drawing.Size(41, 22);
			this.cmdTradeFor.Text = "N/A";
			// 
			// lblLocalAccountAcrn
			// 
			this.lblLocalAccountAcrn.ForeColor = System.Drawing.Color.DimGray;
			this.lblLocalAccountAcrn.Name = "lblLocalAccountAcrn";
			this.lblLocalAccountAcrn.Size = new System.Drawing.Size(50, 22);
			this.lblLocalAccountAcrn.Text = "Account:";
			// 
			// cmdAccount
			// 
			this.cmdAccount.AutoToolTip = false;
			this.cmdAccount.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
			this.cmdAccount.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdAccount.Image = ((System.Drawing.Image)(resources.GetObject("cmdAccount.Image")));
			this.cmdAccount.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.cmdAccount.Name = "cmdAccount";
			this.cmdAccount.Size = new System.Drawing.Size(41, 22);
			this.cmdAccount.Text = "N/A";
			// 
			// panelUpper
			// 
			this.panelUpper.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.panelUpper.BackColor = System.Drawing.Color.Black;
			this.panelUpper.Controls.Add(this.panelMISC);
			this.panelUpper.Controls.Add(this.panelLegs);
			this.panelUpper.Controls.Add(this.panelTickSize);
			this.panelUpper.Controls.Add(this.panelTicketToggle);
			this.panelUpper.Dock = System.Windows.Forms.DockStyle.Top;
			this.panelUpper.ForeColor = System.Drawing.Color.Khaki;
			this.panelUpper.Location = new System.Drawing.Point(0, 51);
			this.panelUpper.Name = "panelUpper";
			this.panelUpper.Padding = new System.Windows.Forms.Padding(1);
			this.panelUpper.Size = new System.Drawing.Size(362, 38);
			this.panelUpper.TabIndex = 0;
			// 
			// panelMISC
			// 
			this.panelMISC.Controls.Add(this.panelPriceBottom);
			this.panelMISC.Controls.Add(this.panelPriceTop);
			this.panelMISC.Dock = System.Windows.Forms.DockStyle.Left;
			this.panelMISC.Location = new System.Drawing.Point(127, 1);
			this.panelMISC.MinimumSize = new System.Drawing.Size(53, 0);
			this.panelMISC.Name = "panelMISC";
			this.panelMISC.Padding = new System.Windows.Forms.Padding(2);
			this.panelMISC.Size = new System.Drawing.Size(190, 36);
			this.panelMISC.TabIndex = 0;
			// 
			// panelPriceBottom
			// 
			this.panelPriceBottom.AutoSize = true;
			this.panelPriceBottom.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.panelPriceBottom.Controls.Add(this.dspTradedPrice);
			this.panelPriceBottom.Controls.Add(this.lblTradedPrice);
			this.panelPriceBottom.Controls.Add(this.lblMarketDataError);
			this.panelPriceBottom.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.panelPriceBottom.ForeColor = System.Drawing.Color.White;
			this.panelPriceBottom.Location = new System.Drawing.Point(2, 19);
			this.panelPriceBottom.MinimumSize = new System.Drawing.Size(176, 15);
			this.panelPriceBottom.Name = "panelPriceBottom";
			this.panelPriceBottom.Size = new System.Drawing.Size(186, 15);
			this.panelPriceBottom.TabIndex = 0;
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
			this.dspTradedPrice.Location = new System.Drawing.Point(52, 0);
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
			// lblTradedPrice
			// 
			this.lblTradedPrice.AutoSize = true;
			this.lblTradedPrice.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblTradedPrice.ForeColor = System.Drawing.Color.Khaki;
			this.lblTradedPrice.Location = new System.Drawing.Point(26, 0);
			this.lblTradedPrice.MaximumSize = new System.Drawing.Size(26, 14);
			this.lblTradedPrice.MinimumSize = new System.Drawing.Size(26, 14);
			this.lblTradedPrice.Name = "lblTradedPrice";
			this.lblTradedPrice.Padding = new System.Windows.Forms.Padding(0, 1, 0, 0);
			this.lblTradedPrice.Size = new System.Drawing.Size(26, 14);
			this.lblTradedPrice.TabIndex = 0;
			this.lblTradedPrice.Text = "last";
			this.lblTradedPrice.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
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
			// panelPriceTop
			// 
			this.panelPriceTop.AutoSize = true;
			this.panelPriceTop.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.panelPriceTop.Controls.Add(this.dspAskPrice);
			this.panelPriceTop.Controls.Add(this.lblAskPrice);
			this.panelPriceTop.Controls.Add(this.dspBidPrice);
			this.panelPriceTop.Controls.Add(this.lblBidPrice);
			this.panelPriceTop.Dock = System.Windows.Forms.DockStyle.Top;
			this.panelPriceTop.ForeColor = System.Drawing.Color.White;
			this.panelPriceTop.Location = new System.Drawing.Point(2, 2);
			this.panelPriceTop.MinimumSize = new System.Drawing.Size(176, 15);
			this.panelPriceTop.Name = "panelPriceTop";
			this.panelPriceTop.Size = new System.Drawing.Size(186, 15);
			this.panelPriceTop.TabIndex = 0;
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
			this.dspAskPrice.Location = new System.Drawing.Point(77, 0);
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
			// lblAskPrice
			// 
			this.lblAskPrice.AutoSize = true;
			this.lblAskPrice.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblAskPrice.ForeColor = System.Drawing.Color.Khaki;
			this.lblAskPrice.Location = new System.Drawing.Point(53, 0);
			this.lblAskPrice.MaximumSize = new System.Drawing.Size(24, 14);
			this.lblAskPrice.MinimumSize = new System.Drawing.Size(24, 14);
			this.lblAskPrice.Name = "lblAskPrice";
			this.lblAskPrice.Padding = new System.Windows.Forms.Padding(0, 1, 0, 0);
			this.lblAskPrice.Size = new System.Drawing.Size(24, 14);
			this.lblAskPrice.TabIndex = 0;
			this.lblAskPrice.Text = "ask";
			this.lblAskPrice.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
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
			// panelLegs
			// 
			this.panelLegs.Controls.Add(this.lblLegs);
			this.panelLegs.Controls.Add(this.dspLegs);
			this.panelLegs.Dock = System.Windows.Forms.DockStyle.Fill;
			this.panelLegs.Location = new System.Drawing.Point(127, 1);
			this.panelLegs.MinimumSize = new System.Drawing.Size(43, 0);
			this.panelLegs.Name = "panelLegs";
			this.panelLegs.Padding = new System.Windows.Forms.Padding(2);
			this.panelLegs.Size = new System.Drawing.Size(234, 36);
			this.panelLegs.TabIndex = 0;
			// 
			// lblLegs
			// 
			this.lblLegs.AutoColor = false;
			this.lblLegs.DefaultColor = System.Drawing.Color.White;
			this.lblLegs.DefaultText = "0";
			this.lblLegs.DisplayFactor = 1;
			this.lblLegs.Dock = System.Windows.Forms.DockStyle.Fill;
			this.lblLegs.DownColor = System.Drawing.Color.Red;
			this.lblLegs.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblLegs.ForeColor = System.Drawing.Color.Lavender;
			this.lblLegs.Is64Th = false;
			this.lblLegs.IsCurrency = false;
			this.lblLegs.IsFraction = false;
			this.lblLegs.IsPercent = false;
			this.lblLegs.Location = new System.Drawing.Point(2, 17);
			this.lblLegs.MaxDecimal = 0;
			this.lblLegs.Name = "lblLegs";
			this.lblLegs.Size = new System.Drawing.Size(230, 17);
			this.lblLegs.TabIndex = 0;
			this.lblLegs.Text = "0";
			this.lblLegs.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			this.lblLegs.TickSize = 0.01;
			this.lblLegs.UpColor = System.Drawing.Color.LimeGreen;
			this.lblLegs.Value = "";
			// 
			// dspLegs
			// 
			this.dspLegs.AutoSize = true;
			this.dspLegs.Dock = System.Windows.Forms.DockStyle.Top;
			this.dspLegs.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspLegs.Location = new System.Drawing.Point(2, 2);
			this.dspLegs.Name = "dspLegs";
			this.dspLegs.Padding = new System.Windows.Forms.Padding(0, 0, 0, 2);
			this.dspLegs.Size = new System.Drawing.Size(30, 15);
			this.dspLegs.TabIndex = 0;
			this.dspLegs.Text = "Legs";
			// 
			// panelTickSize
			// 
			this.panelTickSize.Controls.Add(this.numTickSize);
			this.panelTickSize.Controls.Add(this.dspTickSize);
			this.panelTickSize.Dock = System.Windows.Forms.DockStyle.Left;
			this.panelTickSize.Location = new System.Drawing.Point(54, 1);
			this.panelTickSize.MinimumSize = new System.Drawing.Size(53, 0);
			this.panelTickSize.Name = "panelTickSize";
			this.panelTickSize.Padding = new System.Windows.Forms.Padding(2);
			this.panelTickSize.Size = new System.Drawing.Size(73, 36);
			this.panelTickSize.TabIndex = 0;
			// 
			// numTickSize
			// 
			this.numTickSize.BackColor = System.Drawing.Color.Black;
			this.numTickSize.BorderStyle = System.Windows.Forms.BorderStyle.None;
			this.numTickSize.DecimalPlaces = 2;
			this.numTickSize.Dock = System.Windows.Forms.DockStyle.Fill;
			this.numTickSize.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.numTickSize.ForeColor = System.Drawing.Color.Lavender;
			this.numTickSize.Increment = new decimal(new int[] {
            1,
            0,
            0,
            131072});
			this.numTickSize.Location = new System.Drawing.Point(2, 17);
			this.numTickSize.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            131072});
			this.numTickSize.Name = "numTickSize";
			this.numTickSize.Size = new System.Drawing.Size(69, 18);
			this.numTickSize.TabIndex = 0;
			this.numTickSize.TabStop = false;
			this.numTickSize.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
			this.numTickSize.Value = new decimal(new int[] {
            1,
            0,
            0,
            131072});
			this.numTickSize.ValueChanged += new System.EventHandler(this.TickSize_ValueChanged);
			this.numTickSize.KeyUp += new System.Windows.Forms.KeyEventHandler(this.Control_KeyUp);
			// 
			// dspTickSize
			// 
			this.dspTickSize.AutoSize = true;
			this.dspTickSize.Dock = System.Windows.Forms.DockStyle.Top;
			this.dspTickSize.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspTickSize.Location = new System.Drawing.Point(2, 2);
			this.dspTickSize.Name = "dspTickSize";
			this.dspTickSize.Padding = new System.Windows.Forms.Padding(0, 0, 0, 2);
			this.dspTickSize.Size = new System.Drawing.Size(51, 15);
			this.dspTickSize.TabIndex = 0;
			this.dspTickSize.Text = "Tick Size";
			// 
			// panelTicketToggle
			// 
			this.panelTicketToggle.Controls.Add(this.cmdSelectSettingGrid);
			this.panelTicketToggle.Controls.Add(this.cmdSelectLadder);
			this.panelTicketToggle.Dock = System.Windows.Forms.DockStyle.Left;
			this.panelTicketToggle.Location = new System.Drawing.Point(1, 1);
			this.panelTicketToggle.MinimumSize = new System.Drawing.Size(53, 0);
			this.panelTicketToggle.Name = "panelTicketToggle";
			this.panelTicketToggle.Padding = new System.Windows.Forms.Padding(2);
			this.panelTicketToggle.Size = new System.Drawing.Size(53, 36);
			this.panelTicketToggle.TabIndex = 0;
			// 
			// cmdSelectSettingGrid
			// 
			this.cmdSelectSettingGrid.Animate = true;
			this.cmdSelectSettingGrid.ApplyShine = true;
			this.cmdSelectSettingGrid.AutoColor = false;
			this.cmdSelectSettingGrid.BackColor = System.Drawing.Color.LimeGreen;
			this.cmdSelectSettingGrid.DefaultColor = System.Drawing.Color.White;
			this.cmdSelectSettingGrid.DefaultText = "";
			this.cmdSelectSettingGrid.DisplayFactor = 1;
			this.cmdSelectSettingGrid.Dock = System.Windows.Forms.DockStyle.Top;
			this.cmdSelectSettingGrid.DownColor = System.Drawing.Color.Red;
			this.cmdSelectSettingGrid.InnerBorderColor = System.Drawing.Color.White;
			this.cmdSelectSettingGrid.Is64Th = false;
			this.cmdSelectSettingGrid.IsCurrency = false;
			this.cmdSelectSettingGrid.IsFraction = false;
			this.cmdSelectSettingGrid.IsPercent = false;
			this.cmdSelectSettingGrid.Location = new System.Drawing.Point(2, 19);
			this.cmdSelectSettingGrid.MaxDecimal = 7;
			this.cmdSelectSettingGrid.Name = "cmdSelectSettingGrid";
			this.cmdSelectSettingGrid.OuterBorderColor = System.Drawing.Color.Transparent;
			this.cmdSelectSettingGrid.Size = new System.Drawing.Size(49, 17);
			this.cmdSelectSettingGrid.TabIndex = 0;
			this.cmdSelectSettingGrid.Text = "Setting";
			this.cmdSelectSettingGrid.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			this.cmdSelectSettingGrid.TickSize = 0.01;
			this.cmdSelectSettingGrid.UpColor = System.Drawing.Color.LimeGreen;
			this.cmdSelectSettingGrid.Value = "";
			this.cmdSelectSettingGrid.Click += new System.EventHandler(this.ToggleGrid_Click);
			// 
			// cmdSelectLadder
			// 
			this.cmdSelectLadder.Animate = true;
			this.cmdSelectLadder.ApplyShine = true;
			this.cmdSelectLadder.AutoColor = false;
			this.cmdSelectLadder.BackColor = System.Drawing.Color.Gray;
			this.cmdSelectLadder.DefaultColor = System.Drawing.Color.White;
			this.cmdSelectLadder.DefaultText = "";
			this.cmdSelectLadder.DisplayFactor = 1;
			this.cmdSelectLadder.Dock = System.Windows.Forms.DockStyle.Top;
			this.cmdSelectLadder.DownColor = System.Drawing.Color.Red;
			this.cmdSelectLadder.Enabled = false;
			this.cmdSelectLadder.InnerBorderColor = System.Drawing.Color.White;
			this.cmdSelectLadder.Is64Th = false;
			this.cmdSelectLadder.IsCurrency = false;
			this.cmdSelectLadder.IsFraction = false;
			this.cmdSelectLadder.IsPercent = false;
			this.cmdSelectLadder.Location = new System.Drawing.Point(2, 2);
			this.cmdSelectLadder.MaxDecimal = 7;
			this.cmdSelectLadder.Name = "cmdSelectLadder";
			this.cmdSelectLadder.OuterBorderColor = System.Drawing.Color.Transparent;
			this.cmdSelectLadder.Size = new System.Drawing.Size(49, 17);
			this.cmdSelectLadder.TabIndex = 0;
			this.cmdSelectLadder.Text = "Ladder";
			this.cmdSelectLadder.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			this.cmdSelectLadder.TickSize = 0.01;
			this.cmdSelectLadder.UpColor = System.Drawing.Color.LimeGreen;
			this.cmdSelectLadder.Value = "";
			this.cmdSelectLadder.Click += new System.EventHandler(this.ToggleGrid_Click);
			// 
			// panelOrderInfo
			// 
			this.panelOrderInfo.BackColor = System.Drawing.Color.LightSteelBlue;
			this.panelOrderInfo.Controls.Add(this.panelLegInfo);
			this.panelOrderInfo.Controls.Add(this.panelOrderInfoMsg);
			this.panelOrderInfo.Controls.Add(this.panelOrderInfoUpper);
			this.panelOrderInfo.Dock = System.Windows.Forms.DockStyle.Top;
			this.panelOrderInfo.Location = new System.Drawing.Point(0, 89);
			this.panelOrderInfo.Name = "panelOrderInfo";
			this.panelOrderInfo.Size = new System.Drawing.Size(362, 180);
			this.panelOrderInfo.TabIndex = 0;
			// 
			// panelLegInfo
			// 
			this.panelLegInfo.BackColor = System.Drawing.Color.LightSteelBlue;
			this.panelLegInfo.Controls.Add(this.dspSelectedLegPrice);
			this.panelLegInfo.Dock = System.Windows.Forms.DockStyle.Fill;
			this.panelLegInfo.Font = new System.Drawing.Font("Tahoma", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.panelLegInfo.Location = new System.Drawing.Point(0, 80);
			this.panelLegInfo.Name = "panelLegInfo";
			this.panelLegInfo.Padding = new System.Windows.Forms.Padding(4);
			this.panelLegInfo.Size = new System.Drawing.Size(362, 100);
			this.panelLegInfo.TabIndex = 0;
			// 
			// dspSelectedLegPrice
			// 
			this.dspSelectedLegPrice.BackColor = System.Drawing.SystemColors.ActiveCaptionText;
			this.dspSelectedLegPrice.BorderStyle = System.Windows.Forms.BorderStyle.None;
			this.dspSelectedLegPrice.Dock = System.Windows.Forms.DockStyle.Fill;
			this.dspSelectedLegPrice.Font = new System.Drawing.Font("Courier New", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspSelectedLegPrice.Location = new System.Drawing.Point(4, 4);
			this.dspSelectedLegPrice.Name = "dspSelectedLegPrice";
			this.dspSelectedLegPrice.ReadOnly = true;
			this.dspSelectedLegPrice.ScrollBars = System.Windows.Forms.RichTextBoxScrollBars.Vertical;
			this.dspSelectedLegPrice.Size = new System.Drawing.Size(354, 92);
			this.dspSelectedLegPrice.TabIndex = 0;
			this.dspSelectedLegPrice.TabStop = false;
			this.dspSelectedLegPrice.Text = "";
			this.dspSelectedLegPrice.WordWrap = false;
			// 
			// panelOrderInfoMsg
			// 
			this.panelOrderInfoMsg.BackColor = System.Drawing.Color.LightSteelBlue;
			this.panelOrderInfoMsg.Controls.Add(this.dspOrderMsg);
			this.panelOrderInfoMsg.Dock = System.Windows.Forms.DockStyle.Top;
			this.panelOrderInfoMsg.Font = new System.Drawing.Font("Tahoma", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.panelOrderInfoMsg.Location = new System.Drawing.Point(0, 55);
			this.panelOrderInfoMsg.MaximumSize = new System.Drawing.Size(0, 25);
			this.panelOrderInfoMsg.Name = "panelOrderInfoMsg";
			this.panelOrderInfoMsg.Padding = new System.Windows.Forms.Padding(4);
			this.panelOrderInfoMsg.Size = new System.Drawing.Size(362, 25);
			this.panelOrderInfoMsg.TabIndex = 0;
			// 
			// dspOrderMsg
			// 
			this.dspOrderMsg.BackColor = System.Drawing.Color.White;
			this.dspOrderMsg.BorderStyle = System.Windows.Forms.BorderStyle.None;
			this.dspOrderMsg.Dock = System.Windows.Forms.DockStyle.Fill;
			this.dspOrderMsg.ForeColor = System.Drawing.Color.Black;
			this.dspOrderMsg.Location = new System.Drawing.Point(4, 4);
			this.dspOrderMsg.MaximumSize = new System.Drawing.Size(0, 17);
			this.dspOrderMsg.Multiline = false;
			this.dspOrderMsg.Name = "dspOrderMsg";
			this.dspOrderMsg.ReadOnly = true;
			this.dspOrderMsg.ScrollBars = System.Windows.Forms.RichTextBoxScrollBars.None;
			this.dspOrderMsg.Size = new System.Drawing.Size(354, 17);
			this.dspOrderMsg.TabIndex = 0;
			this.dspOrderMsg.TabStop = false;
			this.dspOrderMsg.Text = "";
			// 
			// panelOrderInfoUpper
			// 
			this.panelOrderInfoUpper.BackColor = System.Drawing.Color.LightSteelBlue;
			this.panelOrderInfoUpper.Controls.Add(this.panelExchange);
			this.panelOrderInfoUpper.Controls.Add(this.panelDuration);
			this.panelOrderInfoUpper.Controls.Add(this.panelStopPrice);
			this.panelOrderInfoUpper.Controls.Add(this.panelLimitPrice);
			this.panelOrderInfoUpper.Controls.Add(this.panelOrderTypes);
			this.panelOrderInfoUpper.Controls.Add(this.panelShares);
			this.panelOrderInfoUpper.Dock = System.Windows.Forms.DockStyle.Top;
			this.panelOrderInfoUpper.Location = new System.Drawing.Point(0, 0);
			this.panelOrderInfoUpper.Name = "panelOrderInfoUpper";
			this.panelOrderInfoUpper.Size = new System.Drawing.Size(362, 55);
			this.panelOrderInfoUpper.TabIndex = 0;
			// 
			// panelExchange
			// 
			this.panelExchange.Controls.Add(this.cboExchange);
			this.panelExchange.Controls.Add(this.lblExchange);
			this.panelExchange.Dock = System.Windows.Forms.DockStyle.Left;
			this.panelExchange.Location = new System.Drawing.Point(301, 0);
			this.panelExchange.MinimumSize = new System.Drawing.Size(60, 55);
			this.panelExchange.Name = "panelExchange";
			this.panelExchange.Padding = new System.Windows.Forms.Padding(2, 3, 2, 2);
			this.panelExchange.Size = new System.Drawing.Size(60, 55);
			this.panelExchange.TabIndex = 0;
			// 
			// cboExchange
			// 
			this.cboExchange.BackColor = System.Drawing.Color.WhiteSmoke;
			this.cboExchange.Dock = System.Windows.Forms.DockStyle.Fill;
			this.cboExchange.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.cboExchange.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cboExchange.ForeColor = System.Drawing.Color.Black;
			this.cboExchange.FormattingEnabled = true;
			this.cboExchange.Location = new System.Drawing.Point(2, 18);
			this.cboExchange.Name = "cboExchange";
			this.cboExchange.Size = new System.Drawing.Size(56, 21);
			this.cboExchange.TabIndex = 0;
			this.cboExchange.TabStop = false;
			this.cboExchange.SelectedIndexChanged += new System.EventHandler(this.Ticket_ValueChanged);
			this.cboExchange.KeyUp += new System.Windows.Forms.KeyEventHandler(this.Control_KeyUp);
			// 
			// lblExchange
			// 
			this.lblExchange.AutoSize = true;
			this.lblExchange.Dock = System.Windows.Forms.DockStyle.Top;
			this.lblExchange.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblExchange.Location = new System.Drawing.Point(2, 3);
			this.lblExchange.Name = "lblExchange";
			this.lblExchange.Padding = new System.Windows.Forms.Padding(0, 0, 0, 2);
			this.lblExchange.Size = new System.Drawing.Size(55, 15);
			this.lblExchange.TabIndex = 0;
			this.lblExchange.Text = "Exchange";
			// 
			// panelDuration
			// 
			this.panelDuration.Controls.Add(this.cboDuration);
			this.panelDuration.Controls.Add(this.lblDuration);
			this.panelDuration.Dock = System.Windows.Forms.DockStyle.Left;
			this.panelDuration.Location = new System.Drawing.Point(245, 0);
			this.panelDuration.MinimumSize = new System.Drawing.Size(56, 55);
			this.panelDuration.Name = "panelDuration";
			this.panelDuration.Padding = new System.Windows.Forms.Padding(2);
			this.panelDuration.Size = new System.Drawing.Size(56, 55);
			this.panelDuration.TabIndex = 0;
			// 
			// cboDuration
			// 
			this.cboDuration.BackColor = System.Drawing.Color.White;
			this.cboDuration.Dock = System.Windows.Forms.DockStyle.Top;
			this.cboDuration.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.cboDuration.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cboDuration.ForeColor = System.Drawing.Color.Black;
			this.cboDuration.FormattingEnabled = true;
			this.cboDuration.Location = new System.Drawing.Point(2, 17);
			this.cboDuration.MinimumSize = new System.Drawing.Size(50, 0);
			this.cboDuration.Name = "cboDuration";
			this.cboDuration.Size = new System.Drawing.Size(52, 21);
			this.cboDuration.TabIndex = 0;
			this.cboDuration.TabStop = false;
			this.cboDuration.SelectedIndexChanged += new System.EventHandler(this.Ticket_ValueChanged);
			this.cboDuration.KeyUp += new System.Windows.Forms.KeyEventHandler(this.Control_KeyUp);
			// 
			// lblDuration
			// 
			this.lblDuration.AutoSize = true;
			this.lblDuration.Dock = System.Windows.Forms.DockStyle.Top;
			this.lblDuration.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblDuration.Location = new System.Drawing.Point(2, 2);
			this.lblDuration.Name = "lblDuration";
			this.lblDuration.Padding = new System.Windows.Forms.Padding(0, 0, 0, 2);
			this.lblDuration.Size = new System.Drawing.Size(47, 15);
			this.lblDuration.TabIndex = 0;
			this.lblDuration.Text = "Duration";
			// 
			// panelStopPrice
			// 
			this.panelStopPrice.Controls.Add(this.dspStopPriceTickSize);
			this.panelStopPrice.Controls.Add(this.lblStopPriceX);
			this.panelStopPrice.Controls.Add(this.dspStopPriceIncrement);
			this.panelStopPrice.Controls.Add(this.numStopPrice);
			this.panelStopPrice.Controls.Add(this.lblStopPrice);
			this.panelStopPrice.Dock = System.Windows.Forms.DockStyle.Left;
			this.panelStopPrice.Location = new System.Drawing.Point(180, 0);
			this.panelStopPrice.MinimumSize = new System.Drawing.Size(65, 55);
			this.panelStopPrice.Name = "panelStopPrice";
			this.panelStopPrice.Padding = new System.Windows.Forms.Padding(2);
			this.panelStopPrice.Size = new System.Drawing.Size(65, 55);
			this.panelStopPrice.TabIndex = 0;
			// 
			// dspStopPriceTickSize
			// 
			this.dspStopPriceTickSize.AutoColor = false;
			this.dspStopPriceTickSize.AutoSize = true;
			this.dspStopPriceTickSize.BackColor = System.Drawing.Color.Transparent;
			this.dspStopPriceTickSize.DefaultColor = System.Drawing.Color.White;
			this.dspStopPriceTickSize.DefaultText = "0.01";
			this.dspStopPriceTickSize.DisplayFactor = 1;
			this.dspStopPriceTickSize.Dock = System.Windows.Forms.DockStyle.Right;
			this.dspStopPriceTickSize.DownColor = System.Drawing.Color.Red;
			this.dspStopPriceTickSize.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspStopPriceTickSize.ForeColor = System.Drawing.Color.Gainsboro;
			this.dspStopPriceTickSize.Is64Th = false;
			this.dspStopPriceTickSize.IsCurrency = false;
			this.dspStopPriceTickSize.IsFraction = true;
			this.dspStopPriceTickSize.IsPercent = false;
			this.dspStopPriceTickSize.Location = new System.Drawing.Point(12, 38);
			this.dspStopPriceTickSize.MaxDecimal = 7;
			this.dspStopPriceTickSize.Name = "dspStopPriceTickSize";
			this.dspStopPriceTickSize.Size = new System.Drawing.Size(27, 12);
			this.dspStopPriceTickSize.TabIndex = 0;
			this.dspStopPriceTickSize.Text = "0.01";
			this.dspStopPriceTickSize.TickSize = 0.01;
			this.dspStopPriceTickSize.UpColor = System.Drawing.Color.LimeGreen;
			this.dspStopPriceTickSize.Value = "";
			// 
			// lblStopPriceX
			// 
			this.lblStopPriceX.AutoColor = false;
			this.lblStopPriceX.AutoSize = true;
			this.lblStopPriceX.BackColor = System.Drawing.Color.Transparent;
			this.lblStopPriceX.DefaultColor = System.Drawing.Color.White;
			this.lblStopPriceX.DefaultText = "0.01";
			this.lblStopPriceX.DisplayFactor = 1;
			this.lblStopPriceX.Dock = System.Windows.Forms.DockStyle.Right;
			this.lblStopPriceX.DownColor = System.Drawing.Color.Red;
			this.lblStopPriceX.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblStopPriceX.ForeColor = System.Drawing.Color.Gainsboro;
			this.lblStopPriceX.Is64Th = false;
			this.lblStopPriceX.IsCurrency = false;
			this.lblStopPriceX.IsFraction = false;
			this.lblStopPriceX.IsPercent = false;
			this.lblStopPriceX.Location = new System.Drawing.Point(39, 38);
			this.lblStopPriceX.MaxDecimal = 7;
			this.lblStopPriceX.Name = "lblStopPriceX";
			this.lblStopPriceX.Size = new System.Drawing.Size(11, 12);
			this.lblStopPriceX.TabIndex = 0;
			this.lblStopPriceX.Text = "x";
			this.lblStopPriceX.TickSize = 0.01;
			this.lblStopPriceX.UpColor = System.Drawing.Color.LimeGreen;
			this.lblStopPriceX.Value = "";
			// 
			// dspStopPriceIncrement
			// 
			this.dspStopPriceIncrement.AutoColor = false;
			this.dspStopPriceIncrement.AutoSize = true;
			this.dspStopPriceIncrement.BackColor = System.Drawing.SystemColors.ControlText;
			this.dspStopPriceIncrement.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.dspStopPriceIncrement.DefaultColor = System.Drawing.Color.White;
			this.dspStopPriceIncrement.DefaultText = "100";
			this.dspStopPriceIncrement.DisplayFactor = 1;
			this.dspStopPriceIncrement.Dock = System.Windows.Forms.DockStyle.Right;
			this.dspStopPriceIncrement.DownColor = System.Drawing.Color.Red;
			this.dspStopPriceIncrement.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspStopPriceIncrement.ForeColor = System.Drawing.Color.Gainsboro;
			this.dspStopPriceIncrement.Is64Th = false;
			this.dspStopPriceIncrement.IsCurrency = false;
			this.dspStopPriceIncrement.IsFraction = false;
			this.dspStopPriceIncrement.IsPercent = false;
			this.dspStopPriceIncrement.Location = new System.Drawing.Point(50, 38);
			this.dspStopPriceIncrement.MaxDecimal = 0;
			this.dspStopPriceIncrement.Name = "dspStopPriceIncrement";
			this.dspStopPriceIncrement.Size = new System.Drawing.Size(13, 14);
			this.dspStopPriceIncrement.TabIndex = 0;
			this.dspStopPriceIncrement.Text = "1";
			this.dspStopPriceIncrement.TickSize = 0.01;
			this.dspStopPriceIncrement.UpColor = System.Drawing.Color.LimeGreen;
			this.dspStopPriceIncrement.Value = "";
			this.dspStopPriceIncrement.MouseClick += new System.Windows.Forms.MouseEventHandler(this.dspStopPriceIncrement_MouseClick);
			// 
			// numStopPrice
			// 
			this.numStopPrice.BackColor = System.Drawing.Color.White;
			this.numStopPrice.DecimalPlaces = 2;
			this.numStopPrice.Dock = System.Windows.Forms.DockStyle.Top;
			this.numStopPrice.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.numStopPrice.ForeColor = System.Drawing.Color.Black;
			this.numStopPrice.Increment = new decimal(new int[] {
            10,
            0,
            0,
            0});
			this.numStopPrice.Location = new System.Drawing.Point(2, 17);
			this.numStopPrice.Maximum = new decimal(new int[] {
            1215752191,
            23,
            0,
            0});
			this.numStopPrice.Minimum = new decimal(new int[] {
            1215752191,
            23,
            0,
            -2147483648});
			this.numStopPrice.Name = "numStopPrice";
			this.numStopPrice.Size = new System.Drawing.Size(61, 21);
			this.numStopPrice.TabIndex = 0;
			this.numStopPrice.TabStop = false;
			this.numStopPrice.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
			this.numStopPrice.ValueChanged += new System.EventHandler(this.Ticket_ValueChanged);
			this.numStopPrice.KeyUp += new System.Windows.Forms.KeyEventHandler(this.Control_KeyUp);
			// 
			// lblStopPrice
			// 
			this.lblStopPrice.AutoSize = true;
			this.lblStopPrice.Dock = System.Windows.Forms.DockStyle.Top;
			this.lblStopPrice.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblStopPrice.Location = new System.Drawing.Point(2, 2);
			this.lblStopPrice.Name = "lblStopPrice";
			this.lblStopPrice.Padding = new System.Windows.Forms.Padding(0, 0, 0, 2);
			this.lblStopPrice.Size = new System.Drawing.Size(56, 15);
			this.lblStopPrice.TabIndex = 0;
			this.lblStopPrice.Text = "Stop Price";
			// 
			// panelLimitPrice
			// 
			this.panelLimitPrice.BackColor = System.Drawing.Color.CornflowerBlue;
			this.panelLimitPrice.Controls.Add(this.dspLimitPriceTickSize);
			this.panelLimitPrice.Controls.Add(this.lblLimitPriceX);
			this.panelLimitPrice.Controls.Add(this.dspLimitPriceIncrement);
			this.panelLimitPrice.Controls.Add(this.numLimitPrice);
			this.panelLimitPrice.Controls.Add(this.lblLimitPrice);
			this.panelLimitPrice.Dock = System.Windows.Forms.DockStyle.Left;
			this.panelLimitPrice.Location = new System.Drawing.Point(115, 0);
			this.panelLimitPrice.MinimumSize = new System.Drawing.Size(65, 55);
			this.panelLimitPrice.Name = "panelLimitPrice";
			this.panelLimitPrice.Padding = new System.Windows.Forms.Padding(2);
			this.panelLimitPrice.Size = new System.Drawing.Size(65, 55);
			this.panelLimitPrice.TabIndex = 0;
			// 
			// dspLimitPriceTickSize
			// 
			this.dspLimitPriceTickSize.AutoColor = false;
			this.dspLimitPriceTickSize.AutoSize = true;
			this.dspLimitPriceTickSize.BackColor = System.Drawing.Color.Transparent;
			this.dspLimitPriceTickSize.DefaultColor = System.Drawing.Color.White;
			this.dspLimitPriceTickSize.DefaultText = "0.01";
			this.dspLimitPriceTickSize.DisplayFactor = 1;
			this.dspLimitPriceTickSize.Dock = System.Windows.Forms.DockStyle.Right;
			this.dspLimitPriceTickSize.DownColor = System.Drawing.Color.Red;
			this.dspLimitPriceTickSize.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspLimitPriceTickSize.ForeColor = System.Drawing.Color.Gainsboro;
			this.dspLimitPriceTickSize.Is64Th = false;
			this.dspLimitPriceTickSize.IsCurrency = false;
			this.dspLimitPriceTickSize.IsFraction = true;
			this.dspLimitPriceTickSize.IsPercent = false;
			this.dspLimitPriceTickSize.Location = new System.Drawing.Point(12, 38);
			this.dspLimitPriceTickSize.MaxDecimal = 7;
			this.dspLimitPriceTickSize.Name = "dspLimitPriceTickSize";
			this.dspLimitPriceTickSize.Size = new System.Drawing.Size(27, 12);
			this.dspLimitPriceTickSize.TabIndex = 0;
			this.dspLimitPriceTickSize.Text = "0.01";
			this.dspLimitPriceTickSize.TickSize = 0.01;
			this.dspLimitPriceTickSize.UpColor = System.Drawing.Color.LimeGreen;
			this.dspLimitPriceTickSize.Value = "";
			// 
			// lblLimitPriceX
			// 
			this.lblLimitPriceX.AutoColor = false;
			this.lblLimitPriceX.AutoSize = true;
			this.lblLimitPriceX.BackColor = System.Drawing.Color.Transparent;
			this.lblLimitPriceX.DefaultColor = System.Drawing.Color.White;
			this.lblLimitPriceX.DefaultText = "0.01";
			this.lblLimitPriceX.DisplayFactor = 1;
			this.lblLimitPriceX.Dock = System.Windows.Forms.DockStyle.Right;
			this.lblLimitPriceX.DownColor = System.Drawing.Color.Red;
			this.lblLimitPriceX.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblLimitPriceX.ForeColor = System.Drawing.Color.Gainsboro;
			this.lblLimitPriceX.Is64Th = false;
			this.lblLimitPriceX.IsCurrency = false;
			this.lblLimitPriceX.IsFraction = false;
			this.lblLimitPriceX.IsPercent = false;
			this.lblLimitPriceX.Location = new System.Drawing.Point(39, 38);
			this.lblLimitPriceX.MaxDecimal = 7;
			this.lblLimitPriceX.Name = "lblLimitPriceX";
			this.lblLimitPriceX.Size = new System.Drawing.Size(11, 12);
			this.lblLimitPriceX.TabIndex = 0;
			this.lblLimitPriceX.Text = "x";
			this.lblLimitPriceX.TickSize = 0.01;
			this.lblLimitPriceX.UpColor = System.Drawing.Color.LimeGreen;
			this.lblLimitPriceX.Value = "";
			// 
			// dspLimitPriceIncrement
			// 
			this.dspLimitPriceIncrement.AutoColor = false;
			this.dspLimitPriceIncrement.AutoSize = true;
			this.dspLimitPriceIncrement.BackColor = System.Drawing.SystemColors.ControlText;
			this.dspLimitPriceIncrement.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.dspLimitPriceIncrement.DefaultColor = System.Drawing.Color.White;
			this.dspLimitPriceIncrement.DefaultText = "100";
			this.dspLimitPriceIncrement.DisplayFactor = 1;
			this.dspLimitPriceIncrement.Dock = System.Windows.Forms.DockStyle.Right;
			this.dspLimitPriceIncrement.DownColor = System.Drawing.Color.Red;
			this.dspLimitPriceIncrement.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspLimitPriceIncrement.ForeColor = System.Drawing.Color.Gainsboro;
			this.dspLimitPriceIncrement.Is64Th = false;
			this.dspLimitPriceIncrement.IsCurrency = false;
			this.dspLimitPriceIncrement.IsFraction = false;
			this.dspLimitPriceIncrement.IsPercent = false;
			this.dspLimitPriceIncrement.Location = new System.Drawing.Point(50, 38);
			this.dspLimitPriceIncrement.MaxDecimal = 0;
			this.dspLimitPriceIncrement.Name = "dspLimitPriceIncrement";
			this.dspLimitPriceIncrement.Size = new System.Drawing.Size(13, 14);
			this.dspLimitPriceIncrement.TabIndex = 0;
			this.dspLimitPriceIncrement.Text = "1";
			this.dspLimitPriceIncrement.TickSize = 0.01;
			this.dspLimitPriceIncrement.UpColor = System.Drawing.Color.LimeGreen;
			this.dspLimitPriceIncrement.Value = "";
			this.dspLimitPriceIncrement.MouseClick += new System.Windows.Forms.MouseEventHandler(this.dspLimitPriceIncrement_MouseClick);
			// 
			// numLimitPrice
			// 
			this.numLimitPrice.BackColor = System.Drawing.Color.White;
			this.numLimitPrice.DecimalPlaces = 2;
			this.numLimitPrice.Dock = System.Windows.Forms.DockStyle.Top;
			this.numLimitPrice.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.numLimitPrice.ForeColor = System.Drawing.Color.Black;
			this.numLimitPrice.Increment = new decimal(new int[] {
            10,
            0,
            0,
            0});
			this.numLimitPrice.Location = new System.Drawing.Point(2, 17);
			this.numLimitPrice.Maximum = new decimal(new int[] {
            1215752191,
            23,
            0,
            0});
			this.numLimitPrice.Minimum = new decimal(new int[] {
            1215752191,
            23,
            0,
            -2147483648});
			this.numLimitPrice.Name = "numLimitPrice";
			this.numLimitPrice.Size = new System.Drawing.Size(61, 21);
			this.numLimitPrice.TabIndex = 0;
			this.numLimitPrice.TabStop = false;
			this.numLimitPrice.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
			this.numLimitPrice.ValueChanged += new System.EventHandler(this.Ticket_ValueChanged);
			this.numLimitPrice.KeyUp += new System.Windows.Forms.KeyEventHandler(this.Control_KeyUp);
			// 
			// lblLimitPrice
			// 
			this.lblLimitPrice.AutoSize = true;
			this.lblLimitPrice.Dock = System.Windows.Forms.DockStyle.Top;
			this.lblLimitPrice.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblLimitPrice.Location = new System.Drawing.Point(2, 2);
			this.lblLimitPrice.Name = "lblLimitPrice";
			this.lblLimitPrice.Padding = new System.Windows.Forms.Padding(0, 0, 0, 2);
			this.lblLimitPrice.Size = new System.Drawing.Size(55, 15);
			this.lblLimitPrice.TabIndex = 0;
			this.lblLimitPrice.Text = "Limit Price";
			// 
			// panelOrderTypes
			// 
			this.panelOrderTypes.Controls.Add(this.cboOrder);
			this.panelOrderTypes.Controls.Add(this.lblOrderType);
			this.panelOrderTypes.Dock = System.Windows.Forms.DockStyle.Left;
			this.panelOrderTypes.Location = new System.Drawing.Point(53, 0);
			this.panelOrderTypes.MinimumSize = new System.Drawing.Size(62, 55);
			this.panelOrderTypes.Name = "panelOrderTypes";
			this.panelOrderTypes.Padding = new System.Windows.Forms.Padding(2);
			this.panelOrderTypes.Size = new System.Drawing.Size(62, 55);
			this.panelOrderTypes.TabIndex = 0;
			// 
			// cboOrder
			// 
			this.cboOrder.BackColor = System.Drawing.Color.White;
			this.cboOrder.Dock = System.Windows.Forms.DockStyle.Top;
			this.cboOrder.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.cboOrder.DropDownWidth = 106;
			this.cboOrder.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cboOrder.ForeColor = System.Drawing.Color.Black;
			this.cboOrder.FormattingEnabled = true;
			this.cboOrder.Location = new System.Drawing.Point(2, 17);
			this.cboOrder.Name = "cboOrder";
			this.cboOrder.Size = new System.Drawing.Size(58, 21);
			this.cboOrder.TabIndex = 0;
			this.cboOrder.TabStop = false;
			this.cboOrder.SelectedIndexChanged += new System.EventHandler(this.Ticket_ValueChanged);
			this.cboOrder.KeyUp += new System.Windows.Forms.KeyEventHandler(this.Control_KeyUp);
			// 
			// lblOrderType
			// 
			this.lblOrderType.AutoSize = true;
			this.lblOrderType.Dock = System.Windows.Forms.DockStyle.Top;
			this.lblOrderType.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblOrderType.Location = new System.Drawing.Point(2, 2);
			this.lblOrderType.Name = "lblOrderType";
			this.lblOrderType.Padding = new System.Windows.Forms.Padding(0, 0, 0, 2);
			this.lblOrderType.Size = new System.Drawing.Size(33, 15);
			this.lblOrderType.TabIndex = 0;
			this.lblOrderType.Text = "Order";
			// 
			// panelShares
			// 
			this.panelShares.Controls.Add(this.dspQuantityIncrement);
			this.panelShares.Controls.Add(this.numQuantity);
			this.panelShares.Controls.Add(this.lblQuantity);
			this.panelShares.Dock = System.Windows.Forms.DockStyle.Left;
			this.panelShares.Location = new System.Drawing.Point(0, 0);
			this.panelShares.MinimumSize = new System.Drawing.Size(53, 55);
			this.panelShares.Name = "panelShares";
			this.panelShares.Padding = new System.Windows.Forms.Padding(2);
			this.panelShares.Size = new System.Drawing.Size(53, 55);
			this.panelShares.TabIndex = 0;
			// 
			// dspQuantityIncrement
			// 
			this.dspQuantityIncrement.AutoColor = false;
			this.dspQuantityIncrement.AutoSize = true;
			this.dspQuantityIncrement.BackColor = System.Drawing.SystemColors.ControlText;
			this.dspQuantityIncrement.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.dspQuantityIncrement.DefaultColor = System.Drawing.Color.White;
			this.dspQuantityIncrement.DefaultText = "100";
			this.dspQuantityIncrement.DisplayFactor = 1;
			this.dspQuantityIncrement.Dock = System.Windows.Forms.DockStyle.Right;
			this.dspQuantityIncrement.DownColor = System.Drawing.Color.Red;
			this.dspQuantityIncrement.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspQuantityIncrement.ForeColor = System.Drawing.Color.Gainsboro;
			this.dspQuantityIncrement.Is64Th = false;
			this.dspQuantityIncrement.IsCurrency = false;
			this.dspQuantityIncrement.IsFraction = false;
			this.dspQuantityIncrement.IsPercent = false;
			this.dspQuantityIncrement.Location = new System.Drawing.Point(38, 38);
			this.dspQuantityIncrement.MaxDecimal = 0;
			this.dspQuantityIncrement.Name = "dspQuantityIncrement";
			this.dspQuantityIncrement.Size = new System.Drawing.Size(13, 14);
			this.dspQuantityIncrement.TabIndex = 0;
			this.dspQuantityIncrement.Text = "1";
			this.dspQuantityIncrement.TickSize = 0.01;
			this.dspQuantityIncrement.UpColor = System.Drawing.Color.LimeGreen;
			this.dspQuantityIncrement.Value = "";
			this.dspQuantityIncrement.MouseClick += new System.Windows.Forms.MouseEventHandler(this.dspQuantityIncrement_MouseClick);
			// 
			// numQuantity
			// 
			this.numQuantity.BackColor = System.Drawing.Color.White;
			this.numQuantity.Dock = System.Windows.Forms.DockStyle.Top;
			this.numQuantity.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.numQuantity.ForeColor = System.Drawing.Color.Black;
			this.numQuantity.Increment = new decimal(new int[] {
            100,
            0,
            0,
            0});
			this.numQuantity.Location = new System.Drawing.Point(2, 17);
			this.numQuantity.Maximum = new decimal(new int[] {
            1215752191,
            23,
            0,
            0});
			this.numQuantity.Name = "numQuantity";
			this.numQuantity.Size = new System.Drawing.Size(49, 21);
			this.numQuantity.TabIndex = 0;
			this.numQuantity.TabStop = false;
			this.numQuantity.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
			this.numQuantity.ValueChanged += new System.EventHandler(this.Ticket_ValueChanged);
			this.numQuantity.KeyUp += new System.Windows.Forms.KeyEventHandler(this.Control_KeyUp);
			// 
			// lblQuantity
			// 
			this.lblQuantity.AutoSize = true;
			this.lblQuantity.Dock = System.Windows.Forms.DockStyle.Top;
			this.lblQuantity.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblQuantity.Location = new System.Drawing.Point(2, 2);
			this.lblQuantity.Name = "lblQuantity";
			this.lblQuantity.Padding = new System.Windows.Forms.Padding(0, 0, 0, 2);
			this.lblQuantity.Size = new System.Drawing.Size(23, 15);
			this.lblQuantity.TabIndex = 0;
			this.lblQuantity.Text = "Qty";
			// 
			// panelCommands
			// 
			this.panelCommands.BackColor = System.Drawing.Color.LightSteelBlue;
			this.panelCommands.Controls.Add(this.cmdSell);
			this.panelCommands.Controls.Add(this.cmdBuy);
			this.panelCommands.Dock = System.Windows.Forms.DockStyle.Top;
			this.panelCommands.Location = new System.Drawing.Point(0, 269);
			this.panelCommands.MinimumSize = new System.Drawing.Size(241, 24);
			this.panelCommands.Name = "panelCommands";
			this.panelCommands.Padding = new System.Windows.Forms.Padding(2);
			this.panelCommands.Size = new System.Drawing.Size(362, 24);
			this.panelCommands.TabIndex = 0;
			// 
			// cmdSell
			// 
			this.cmdSell.BackColor = System.Drawing.Color.Red;
			this.cmdSell.Dock = System.Windows.Forms.DockStyle.Right;
			this.cmdSell.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdSell.ForeColor = System.Drawing.Color.Black;
			this.cmdSell.Location = new System.Drawing.Point(242, 2);
			this.cmdSell.MinimumSize = new System.Drawing.Size(78, 20);
			this.cmdSell.Name = "cmdSell";
			this.cmdSell.Size = new System.Drawing.Size(118, 20);
			this.cmdSell.TabIndex = 0;
			this.cmdSell.TabStop = false;
			this.cmdSell.Text = "SELL";
			this.cmdSell.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
			this.cmdSell.UserSplitSize = 20;
			this.cmdSell.Click += new System.EventHandler(this.cmdSell_Click);
			// 
			// cmdBuy
			// 
			this.cmdBuy.BackColor = System.Drawing.Color.LimeGreen;
			this.cmdBuy.Dock = System.Windows.Forms.DockStyle.Left;
			this.cmdBuy.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdBuy.ForeColor = System.Drawing.Color.Black;
			this.cmdBuy.Location = new System.Drawing.Point(2, 2);
			this.cmdBuy.MinimumSize = new System.Drawing.Size(78, 20);
			this.cmdBuy.Name = "cmdBuy";
			this.cmdBuy.Size = new System.Drawing.Size(118, 20);
			this.cmdBuy.TabIndex = 0;
			this.cmdBuy.TabStop = false;
			this.cmdBuy.Text = "BUY";
			this.cmdBuy.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
			this.cmdBuy.UserSplitSize = 20;
			this.cmdBuy.Click += new System.EventHandler(this.cmdBuy_Click);
			// 
			// panelManagerQuick
			// 
			this.panelManagerQuick.Controls.Add(this.managedPanelBook);
			this.panelManagerQuick.Dock = System.Windows.Forms.DockStyle.Top;
			this.panelManagerQuick.Location = new System.Drawing.Point(0, 293);
			this.panelManagerQuick.MinimumSize = new System.Drawing.Size(241, 24);
			this.panelManagerQuick.Name = "panelManagerQuick";
			this.panelManagerQuick.Padding = new System.Windows.Forms.Padding(2);
			this.panelManagerQuick.SelectedIndex = 0;
			this.panelManagerQuick.SelectedPanel = this.managedPanelBook;
			this.panelManagerQuick.Size = new System.Drawing.Size(362, 24);
			this.panelManagerQuick.TabIndex = 0;
			this.panelManagerQuick.TabStop = false;
			// 
			// managedPanelBook
			// 
			this.managedPanelBook.BackColor = System.Drawing.Color.LightSteelBlue;
			this.managedPanelBook.Controls.Add(this.dspNetOpen);
			this.managedPanelBook.Controls.Add(this.dspSellOpenQty);
			this.managedPanelBook.Controls.Add(this.dspBuyOpenQty);
			this.managedPanelBook.Controls.Add(this.cmdCenterLow);
			this.managedPanelBook.Controls.Add(this.cmdCenterTrade);
			this.managedPanelBook.Controls.Add(this.cmdCancelAllSell);
			this.managedPanelBook.Controls.Add(this.cmdCancelAll);
			this.managedPanelBook.Controls.Add(this.cmdCancelAllBuy);
			this.managedPanelBook.Controls.Add(this.cmdCenterHigh);
			this.managedPanelBook.Location = new System.Drawing.Point(0, 0);
			this.managedPanelBook.Name = "managedPanelBook";
			this.managedPanelBook.Padding = new System.Windows.Forms.Padding(2);
			this.managedPanelBook.Size = new System.Drawing.Size(362, 24);
			this.managedPanelBook.Text = "managedPanelBook";
			// 
			// dspNetOpen
			// 
			this.dspNetOpen.AutoColor = true;
			this.dspNetOpen.DefaultColor = System.Drawing.Color.White;
			this.dspNetOpen.DefaultText = "0";
			this.dspNetOpen.DisplayFactor = 1;
			this.dspNetOpen.Dock = System.Windows.Forms.DockStyle.Fill;
			this.dspNetOpen.DownColor = System.Drawing.Color.Red;
			this.dspNetOpen.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspNetOpen.Is64Th = false;
			this.dspNetOpen.IsCurrency = false;
			this.dspNetOpen.IsFraction = false;
			this.dspNetOpen.IsPercent = false;
			this.dspNetOpen.Location = new System.Drawing.Point(120, 2);
			this.dspNetOpen.MaxDecimal = 0;
			this.dspNetOpen.MinimumSize = new System.Drawing.Size(31, 20);
			this.dspNetOpen.Name = "dspNetOpen";
			this.dspNetOpen.Size = new System.Drawing.Size(122, 20);
			this.dspNetOpen.TabIndex = 0;
			this.dspNetOpen.Text = "0";
			this.dspNetOpen.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			this.dspNetOpen.TickSize = 0.01;
			this.dspNetOpen.UpColor = System.Drawing.Color.LimeGreen;
			this.dspNetOpen.Value = "";
			// 
			// dspSellOpenQty
			// 
			this.dspSellOpenQty.AutoColor = false;
			this.dspSellOpenQty.BackColor = System.Drawing.Color.Red;
			this.dspSellOpenQty.DefaultColor = System.Drawing.Color.White;
			this.dspSellOpenQty.DefaultText = "0";
			this.dspSellOpenQty.DisplayFactor = 1;
			this.dspSellOpenQty.Dock = System.Windows.Forms.DockStyle.Right;
			this.dspSellOpenQty.DownColor = System.Drawing.Color.Red;
			this.dspSellOpenQty.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspSellOpenQty.Is64Th = false;
			this.dspSellOpenQty.IsCurrency = false;
			this.dspSellOpenQty.IsFraction = false;
			this.dspSellOpenQty.IsPercent = false;
			this.dspSellOpenQty.Location = new System.Drawing.Point(242, 2);
			this.dspSellOpenQty.MaxDecimal = 0;
			this.dspSellOpenQty.MinimumSize = new System.Drawing.Size(38, 20);
			this.dspSellOpenQty.Name = "dspSellOpenQty";
			this.dspSellOpenQty.Size = new System.Drawing.Size(58, 20);
			this.dspSellOpenQty.TabIndex = 0;
			this.dspSellOpenQty.Text = "0";
			this.dspSellOpenQty.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			this.dspSellOpenQty.TickSize = 0.01;
			this.dspSellOpenQty.UpColor = System.Drawing.Color.LimeGreen;
			this.dspSellOpenQty.Value = "";
			// 
			// dspBuyOpenQty
			// 
			this.dspBuyOpenQty.AutoColor = false;
			this.dspBuyOpenQty.BackColor = System.Drawing.Color.LimeGreen;
			this.dspBuyOpenQty.DefaultColor = System.Drawing.Color.White;
			this.dspBuyOpenQty.DefaultText = "0";
			this.dspBuyOpenQty.DisplayFactor = 1;
			this.dspBuyOpenQty.Dock = System.Windows.Forms.DockStyle.Left;
			this.dspBuyOpenQty.DownColor = System.Drawing.Color.Red;
			this.dspBuyOpenQty.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspBuyOpenQty.Is64Th = false;
			this.dspBuyOpenQty.IsCurrency = false;
			this.dspBuyOpenQty.IsFraction = false;
			this.dspBuyOpenQty.IsPercent = false;
			this.dspBuyOpenQty.Location = new System.Drawing.Point(62, 2);
			this.dspBuyOpenQty.MaxDecimal = 0;
			this.dspBuyOpenQty.MinimumSize = new System.Drawing.Size(38, 20);
			this.dspBuyOpenQty.Name = "dspBuyOpenQty";
			this.dspBuyOpenQty.Size = new System.Drawing.Size(58, 20);
			this.dspBuyOpenQty.TabIndex = 0;
			this.dspBuyOpenQty.Text = "0";
			this.dspBuyOpenQty.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			this.dspBuyOpenQty.TickSize = 0.01;
			this.dspBuyOpenQty.UpColor = System.Drawing.Color.LimeGreen;
			this.dspBuyOpenQty.Value = "";
			// 
			// cmdCenterLow
			// 
			this.cmdCenterLow.AutoSize = true;
			this.cmdCenterLow.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdCenterLow.BackColor = System.Drawing.Color.Red;
			this.cmdCenterLow.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("cmdCenterLow.BackgroundImage")));
			this.cmdCenterLow.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Zoom;
			this.cmdCenterLow.Dock = System.Windows.Forms.DockStyle.Right;
			this.cmdCenterLow.Font = new System.Drawing.Font("Microsoft Sans Serif", 6F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdCenterLow.ForeColor = System.Drawing.Color.DarkBlue;
			this.cmdCenterLow.ImageIndex = 3;
			this.cmdCenterLow.Location = new System.Drawing.Point(300, 2);
			this.cmdCenterLow.MaximumSize = new System.Drawing.Size(20, 20);
			this.cmdCenterLow.MinimumSize = new System.Drawing.Size(20, 20);
			this.cmdCenterLow.Name = "cmdCenterLow";
			this.cmdCenterLow.Size = new System.Drawing.Size(20, 20);
			this.cmdCenterLow.TabIndex = 0;
			this.cmdCenterLow.TabStop = false;
			this.cmdCenterLow.Click += new System.EventHandler(this.cmdCenterLow_Click);
			// 
			// cmdCenterTrade
			// 
			this.cmdCenterTrade.AutoSize = true;
			this.cmdCenterTrade.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdCenterTrade.BackColor = System.Drawing.Color.Silver;
			this.cmdCenterTrade.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("cmdCenterTrade.BackgroundImage")));
			this.cmdCenterTrade.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Zoom;
			this.cmdCenterTrade.Dock = System.Windows.Forms.DockStyle.Right;
			this.cmdCenterTrade.Font = new System.Drawing.Font("Microsoft Sans Serif", 6F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdCenterTrade.ForeColor = System.Drawing.Color.DarkBlue;
			this.cmdCenterTrade.ImageIndex = 3;
			this.cmdCenterTrade.Location = new System.Drawing.Point(320, 2);
			this.cmdCenterTrade.MaximumSize = new System.Drawing.Size(20, 20);
			this.cmdCenterTrade.MinimumSize = new System.Drawing.Size(20, 20);
			this.cmdCenterTrade.Name = "cmdCenterTrade";
			this.cmdCenterTrade.Size = new System.Drawing.Size(20, 20);
			this.cmdCenterTrade.TabIndex = 0;
			this.cmdCenterTrade.TabStop = false;
			this.cmdCenterTrade.Click += new System.EventHandler(this.cmdCenterTrade_Click);
			// 
			// cmdCancelAllSell
			// 
			this.cmdCancelAllSell.AutoSize = true;
			this.cmdCancelAllSell.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdCancelAllSell.BackColor = System.Drawing.Color.Red;
			this.cmdCancelAllSell.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("cmdCancelAllSell.BackgroundImage")));
			this.cmdCancelAllSell.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Zoom;
			this.cmdCancelAllSell.Dock = System.Windows.Forms.DockStyle.Left;
			this.cmdCancelAllSell.Font = new System.Drawing.Font("Microsoft Sans Serif", 6F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdCancelAllSell.ForeColor = System.Drawing.Color.DarkBlue;
			this.cmdCancelAllSell.ImageIndex = 3;
			this.cmdCancelAllSell.InnerBorderColor = System.Drawing.Color.Blue;
			this.cmdCancelAllSell.Location = new System.Drawing.Point(42, 2);
			this.cmdCancelAllSell.MaximumSize = new System.Drawing.Size(20, 20);
			this.cmdCancelAllSell.MinimumSize = new System.Drawing.Size(20, 20);
			this.cmdCancelAllSell.Name = "cmdCancelAllSell";
			this.cmdCancelAllSell.Size = new System.Drawing.Size(20, 20);
			this.cmdCancelAllSell.TabIndex = 0;
			this.cmdCancelAllSell.TabStop = false;
			this.cmdCancelAllSell.Click += new System.EventHandler(this.cmdCancelAllSell_Click);
			// 
			// cmdCancelAll
			// 
			this.cmdCancelAll.AutoSize = true;
			this.cmdCancelAll.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdCancelAll.BackColor = System.Drawing.Color.Silver;
			this.cmdCancelAll.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("cmdCancelAll.BackgroundImage")));
			this.cmdCancelAll.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Zoom;
			this.cmdCancelAll.Dock = System.Windows.Forms.DockStyle.Left;
			this.cmdCancelAll.Font = new System.Drawing.Font("Microsoft Sans Serif", 6F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdCancelAll.ForeColor = System.Drawing.Color.DarkBlue;
			this.cmdCancelAll.ImageIndex = 3;
			this.cmdCancelAll.Location = new System.Drawing.Point(22, 2);
			this.cmdCancelAll.MaximumSize = new System.Drawing.Size(20, 20);
			this.cmdCancelAll.MinimumSize = new System.Drawing.Size(20, 20);
			this.cmdCancelAll.Name = "cmdCancelAll";
			this.cmdCancelAll.Size = new System.Drawing.Size(20, 20);
			this.cmdCancelAll.TabIndex = 0;
			this.cmdCancelAll.TabStop = false;
			this.cmdCancelAll.Click += new System.EventHandler(this.cmdCancelAll_Click);
			// 
			// cmdCancelAllBuy
			// 
			this.cmdCancelAllBuy.AutoSize = true;
			this.cmdCancelAllBuy.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdCancelAllBuy.BackColor = System.Drawing.Color.LimeGreen;
			this.cmdCancelAllBuy.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("cmdCancelAllBuy.BackgroundImage")));
			this.cmdCancelAllBuy.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Zoom;
			this.cmdCancelAllBuy.Dock = System.Windows.Forms.DockStyle.Left;
			this.cmdCancelAllBuy.Font = new System.Drawing.Font("Microsoft Sans Serif", 6F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdCancelAllBuy.ForeColor = System.Drawing.Color.DarkBlue;
			this.cmdCancelAllBuy.ImageIndex = 3;
			this.cmdCancelAllBuy.Location = new System.Drawing.Point(2, 2);
			this.cmdCancelAllBuy.MaximumSize = new System.Drawing.Size(20, 20);
			this.cmdCancelAllBuy.MinimumSize = new System.Drawing.Size(20, 20);
			this.cmdCancelAllBuy.Name = "cmdCancelAllBuy";
			this.cmdCancelAllBuy.Size = new System.Drawing.Size(20, 20);
			this.cmdCancelAllBuy.TabIndex = 0;
			this.cmdCancelAllBuy.TabStop = false;
			this.cmdCancelAllBuy.Click += new System.EventHandler(this.cmdCancelAllBuy_Click);
			// 
			// cmdCenterHigh
			// 
			this.cmdCenterHigh.AutoSize = true;
			this.cmdCenterHigh.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdCenterHigh.BackColor = System.Drawing.Color.LimeGreen;
			this.cmdCenterHigh.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("cmdCenterHigh.BackgroundImage")));
			this.cmdCenterHigh.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Zoom;
			this.cmdCenterHigh.Dock = System.Windows.Forms.DockStyle.Right;
			this.cmdCenterHigh.Font = new System.Drawing.Font("Microsoft Sans Serif", 6F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdCenterHigh.ForeColor = System.Drawing.Color.DarkBlue;
			this.cmdCenterHigh.ImageIndex = 3;
			this.cmdCenterHigh.Location = new System.Drawing.Point(340, 2);
			this.cmdCenterHigh.MaximumSize = new System.Drawing.Size(20, 20);
			this.cmdCenterHigh.MinimumSize = new System.Drawing.Size(20, 20);
			this.cmdCenterHigh.Name = "cmdCenterHigh";
			this.cmdCenterHigh.Size = new System.Drawing.Size(20, 20);
			this.cmdCenterHigh.TabIndex = 0;
			this.cmdCenterHigh.TabStop = false;
			this.cmdCenterHigh.Click += new System.EventHandler(this.cmdCenterHigh_Click);
			// 
			// splitContainer
			// 
			this.splitContainer.Dock = System.Windows.Forms.DockStyle.Fill;
			this.splitContainer.Location = new System.Drawing.Point(0, 317);
			this.splitContainer.Name = "splitContainer";
			this.splitContainer.Orientation = System.Windows.Forms.Orientation.Horizontal;
			// 
			// splitContainer.Panel1
			// 
			this.splitContainer.Panel1.Controls.Add(this.rocAutoSpreadList);
			// 
			// splitContainer.Panel2
			// 
			this.splitContainer.Panel2.Controls.Add(this.rocAutoSpreadListSettings);
			this.splitContainer.Size = new System.Drawing.Size(362, 175);
			this.splitContainer.SplitterDistance = 122;
			this.splitContainer.TabIndex = 0;
			this.splitContainer.TabStop = false;
			// 
			// rocAutoSpreadList
			// 
			this.rocAutoSpreadList.askBookPrices = ((System.Collections.Generic.List<double>)(resources.GetObject("rocAutoSpreadList.askBookPrices")));
			this.rocAutoSpreadList.AskColor = System.Drawing.Color.Red;
			this.rocAutoSpreadList.AskPrice = 0;
			this.rocAutoSpreadList.AutoCenterOnTraded = false;
			this.rocAutoSpreadList.AutoCenterRange = ((long)(3));
			this.rocAutoSpreadList.bidBookPrices = ((System.Collections.Generic.List<double>)(resources.GetObject("rocAutoSpreadList.bidBookPrices")));
			this.rocAutoSpreadList.BidColor = System.Drawing.Color.LimeGreen;
			this.rocAutoSpreadList.BidPrice = 0;
			this.rocAutoSpreadList.BlueDefault = 228;
			this.rocAutoSpreadList.BuyCancel = false;
			this.rocAutoSpreadList.BuyReplace = false;
			this.rocAutoSpreadList.BuySelected = false;
			this.rocAutoSpreadList.BuyStopSelected = false;
			this.rocAutoSpreadList.ClearBuyStopLimit = false;
			this.rocAutoSpreadList.ClearSellStopLimit = false;
			this.rocAutoSpreadList.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
			new BindingListEx.ROCBindingList<string>().Add("");
			this.rocAutoSpreadList.DisplayFactors = ((System.Collections.Generic.Dictionary<string, double>)(resources.GetObject("rocAutoSpreadList.DisplayFactors")));
			this.rocAutoSpreadList.Dock = System.Windows.Forms.DockStyle.Fill;
			this.rocAutoSpreadList.EndReplacePrice = 0;
			this.rocAutoSpreadList.GreenDefault = 243;
			this.rocAutoSpreadList.GridKeys = ((System.Collections.Generic.List<string>)(resources.GetObject("rocAutoSpreadList.GridKeys")));
			this.rocAutoSpreadList.HasFlagChange = false;
			this.rocAutoSpreadList.HeaderBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(233)))), ((int)(((byte)(243)))), ((int)(((byte)(228)))));
			this.rocAutoSpreadList.HighPrice = 0;
			this.rocAutoSpreadList.Importing = false;
			this.rocAutoSpreadList.LastFilter = "";
			this.rocAutoSpreadList.LastSort = "Price DESC";
			this.rocAutoSpreadList.Location = new System.Drawing.Point(0, 0);
			this.rocAutoSpreadList.LowPrice = 0;
			this.rocAutoSpreadList.Name = "rocAutoSpreadList";
			this.rocAutoSpreadList.PriceBook = 0;
			this.rocAutoSpreadList.PriceSelected = false;
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
			this.rocAutoSpreadList.Printer = dgvBasePrinter1;
			this.rocAutoSpreadList.Processing = false;
			this.rocAutoSpreadList.Qty = ((long)(0));
			this.rocAutoSpreadList.ReadyForTickTable = false;
			this.rocAutoSpreadList.RedDefault = 233;
			this.rocAutoSpreadList.RefreshAggragation = true;
			this.rocAutoSpreadList.RefreshLadder = false;
			this.rocAutoSpreadList.RefreshSharedRows = false;
			this.rocAutoSpreadList.SelectedForeColor = System.Drawing.Color.LightBlue;
			this.rocAutoSpreadList.SelectedStopLimitBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(34)))), ((int)(((byte)(34)))), ((int)(((byte)(34)))));
			this.rocAutoSpreadList.SellCancel = false;
			this.rocAutoSpreadList.SellReplace = false;
			this.rocAutoSpreadList.SellSelected = false;
			this.rocAutoSpreadList.SellStopSelected = false;
			this.rocAutoSpreadList.ShouldScrollToLastRow = false;
			this.rocAutoSpreadList.ShowBuyStopLimit = false;
			this.rocAutoSpreadList.ShowColumnMenu = false;
			this.rocAutoSpreadList.ShowQtyMenu = false;
			this.rocAutoSpreadList.ShowSellStopLimit = false;
			this.rocAutoSpreadList.Size = new System.Drawing.Size(362, 122);
			this.rocAutoSpreadList.StartReplacePrice = 0;
			this.rocAutoSpreadList.StopPriceBook = 0;
			new BindingListEx.ROCBindingList<string>().Add("");
			this.rocAutoSpreadList.TabIndex = 0;
			this.rocAutoSpreadList.TabStop = false;
			this.rocAutoSpreadList.TickSize = 0;
			this.rocAutoSpreadList.TickSizes = ((System.Collections.Generic.Dictionary<string, double>)(resources.GetObject("rocAutoSpreadList.TickSizes")));
			this.rocAutoSpreadList.TradePrice = 0;
			this.rocAutoSpreadList.TradeTick = "";
			// 
			// rocAutoSpreadListSettings
			// 
			this.rocAutoSpreadListSettings.BlueDefault = 228;
			this.rocAutoSpreadListSettings.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
			new BindingListEx.ROCBindingList<string>().Add("");
			this.rocAutoSpreadListSettings.DisplayFactors = ((System.Collections.Generic.Dictionary<string, double>)(resources.GetObject("rocAutoSpreadListSettings.DisplayFactors")));
			this.rocAutoSpreadListSettings.Dock = System.Windows.Forms.DockStyle.Fill;
			this.rocAutoSpreadListSettings.GreenDefault = 243;
			this.rocAutoSpreadListSettings.GridKeys = ((System.Collections.Generic.List<string>)(resources.GetObject("rocAutoSpreadListSettings.GridKeys")));
			this.rocAutoSpreadListSettings.HeaderBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(233)))), ((int)(((byte)(243)))), ((int)(((byte)(228)))));
			this.rocAutoSpreadListSettings.Importing = false;
			this.rocAutoSpreadListSettings.LastFilter = "";
			this.rocAutoSpreadListSettings.LastSort = "";
			this.rocAutoSpreadListSettings.Location = new System.Drawing.Point(0, 0);
			this.rocAutoSpreadListSettings.Name = "rocAutoSpreadListSettings";
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
			this.rocAutoSpreadListSettings.Printer = dgvBasePrinter2;
			this.rocAutoSpreadListSettings.Processing = false;
			this.rocAutoSpreadListSettings.RedDefault = 233;
			this.rocAutoSpreadListSettings.RefreshAggragation = true;
			this.rocAutoSpreadListSettings.RefreshRation = false;
			this.rocAutoSpreadListSettings.RefreshSharedRows = false;
			this.rocAutoSpreadListSettings.ShouldScrollToLastRow = false;
			this.rocAutoSpreadListSettings.ShowAccountMenu = false;
			this.rocAutoSpreadListSettings.ShowColumnMenu = false;
			this.rocAutoSpreadListSettings.ShowExchangeMenu = false;
			this.rocAutoSpreadListSettings.ShowTraderMenu = false;
			this.rocAutoSpreadListSettings.Size = new System.Drawing.Size(362, 49);
			this.rocAutoSpreadListSettings.SubscribeSymbolDetail = "";
			new BindingListEx.ROCBindingList<string>().Add("");
			this.rocAutoSpreadListSettings.TabIndex = 0;
			this.rocAutoSpreadListSettings.TabStop = false;
			this.rocAutoSpreadListSettings.TickSizes = ((System.Collections.Generic.Dictionary<string, double>)(resources.GetObject("rocAutoSpreadListSettings.TickSizes")));
			// 
			// frmAutoSpreadTicket
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.Caption = "AUTO SPREAD TICKET";
			this.ClientSize = new System.Drawing.Size(362, 492);
			this.Controls.Add(this.splitContainer);
			this.Controls.Add(this.panelManagerQuick);
			this.Controls.Add(this.panelCommands);
			this.Controls.Add(this.panelOrderInfo);
			this.Controls.Add(this.panelUpper);
			this.Controls.Add(this.toolStripTraderInfo);
			this.Controls.Add(this.CaptionBar);
			this.MaximumSize = new System.Drawing.Size(569, 2048);
			this.MinimumSize = new System.Drawing.Size(304, 500);
			this.Name = "frmAutoSpreadTicket";
			this.ShowInTaskbar = false;
			this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
			this.toolStripTraderInfo.ResumeLayout(false);
			this.toolStripTraderInfo.PerformLayout();
			this.panelUpper.ResumeLayout(false);
			this.panelMISC.ResumeLayout(false);
			this.panelMISC.PerformLayout();
			this.panelPriceBottom.ResumeLayout(false);
			this.panelPriceBottom.PerformLayout();
			this.panelPriceTop.ResumeLayout(false);
			this.panelPriceTop.PerformLayout();
			this.panelLegs.ResumeLayout(false);
			this.panelLegs.PerformLayout();
			this.panelTickSize.ResumeLayout(false);
			this.panelTickSize.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.numTickSize)).EndInit();
			this.panelTicketToggle.ResumeLayout(false);
			this.panelOrderInfo.ResumeLayout(false);
			this.panelLegInfo.ResumeLayout(false);
			this.panelOrderInfoMsg.ResumeLayout(false);
			this.panelOrderInfoUpper.ResumeLayout(false);
			this.panelExchange.ResumeLayout(false);
			this.panelExchange.PerformLayout();
			this.panelDuration.ResumeLayout(false);
			this.panelDuration.PerformLayout();
			this.panelStopPrice.ResumeLayout(false);
			this.panelStopPrice.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.numStopPrice)).EndInit();
			this.panelLimitPrice.ResumeLayout(false);
			this.panelLimitPrice.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.numLimitPrice)).EndInit();
			this.panelOrderTypes.ResumeLayout(false);
			this.panelOrderTypes.PerformLayout();
			this.panelShares.ResumeLayout(false);
			this.panelShares.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.numQuantity)).EndInit();
			this.panelCommands.ResumeLayout(false);
			this.panelManagerQuick.ResumeLayout(false);
			this.managedPanelBook.ResumeLayout(false);
			this.managedPanelBook.PerformLayout();
			this.splitContainer.Panel1.ResumeLayout(false);
			this.splitContainer.Panel2.ResumeLayout(false);
			this.splitContainer.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.rocAutoSpreadList)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.rocAutoSpreadListSettings)).EndInit();
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private FormEx.VistaWindowCaptionBar CaptionBar;
		private System.Windows.Forms.ToolStrip toolStripTraderInfo;
		private System.Windows.Forms.ToolStripLabel lblTradeFor;
		protected internal System.Windows.Forms.ToolStripDropDownButton cmdTradeFor;
		private System.Windows.Forms.ToolStripLabel lblLocalAccountAcrn;
		protected internal System.Windows.Forms.ToolStripDropDownButton cmdAccount;
		private System.Windows.Forms.Panel panelUpper;
		private System.Windows.Forms.Panel panelLegs;
		private System.Windows.Forms.Label dspLegs;
		private System.Windows.Forms.Panel panelOrderInfo;
		private System.Windows.Forms.Panel panelOrderInfoMsg;
		private System.Windows.Forms.RichTextBox dspOrderMsg;
		private System.Windows.Forms.Panel panelOrderInfoUpper;
		private System.Windows.Forms.Panel panelExchange;
		protected internal System.Windows.Forms.ComboBox cboExchange;
		protected internal System.Windows.Forms.Label lblExchange;
		private System.Windows.Forms.Panel panelDuration;
		private System.Windows.Forms.ComboBox cboDuration;
		private System.Windows.Forms.Label lblDuration;
		private System.Windows.Forms.Panel panelOrderTypes;
		private System.Windows.Forms.ComboBox cboOrder;
		private System.Windows.Forms.Label lblOrderType;
		private System.Windows.Forms.Panel panelShares;
		private LabelEx.LabelBase dspQuantityIncrement;
		public System.Windows.Forms.NumericUpDown numQuantity;
		private System.Windows.Forms.Label lblQuantity;
		private System.Windows.Forms.Panel panelCommands;
		private ButtonEx.VistaButton cmdSell;
		private ButtonEx.VistaButton cmdBuy;
		private PanelEx.PanelManager panelManagerQuick;
		private PanelEx.ManagedPanel managedPanelBook;
		private LabelEx.LabelVista dspNetOpen;
		private LabelEx.LabelVista dspSellOpenQty;
		private LabelEx.LabelVista dspBuyOpenQty;
		private ButtonEx.VistaButton cmdCenterLow;
		private ButtonEx.VistaButton cmdCenterTrade;
		private ButtonEx.VistaButton cmdCenterHigh;
		private ButtonEx.VistaButton cmdCancelAllSell;
		private ButtonEx.VistaButton cmdCancelAll;
		private ButtonEx.VistaButton cmdCancelAllBuy;
		private LabelEx.LabelBase lblLegs;
		private System.Windows.Forms.Panel panelTickSize;
		private System.Windows.Forms.Label dspTickSize;
		private System.Windows.Forms.SplitContainer splitContainer;
		private DataGridViewEx.ROCAutoSpreadList rocAutoSpreadList;
		private DataGridViewEx.ROCAutoSpreadListSettings rocAutoSpreadListSettings;
		private System.Windows.Forms.NumericUpDown numTickSize;
		private System.Windows.Forms.Panel panelTicketToggle;
		private System.Windows.Forms.Panel panelStopPrice;
		private LabelEx.LabelBase dspStopPriceTickSize;
		private LabelEx.LabelBase lblStopPriceX;
		private LabelEx.LabelBase dspStopPriceIncrement;
		public System.Windows.Forms.NumericUpDown numStopPrice;
		private System.Windows.Forms.Label lblStopPrice;
		private System.Windows.Forms.Panel panelLimitPrice;
		private LabelEx.LabelBase dspLimitPriceTickSize;
		private LabelEx.LabelBase lblLimitPriceX;
		private LabelEx.LabelBase dspLimitPriceIncrement;
		public System.Windows.Forms.NumericUpDown numLimitPrice;
		private System.Windows.Forms.Label lblLimitPrice;
		private System.Windows.Forms.Panel panelLegInfo;
		private System.Windows.Forms.RichTextBox dspSelectedLegPrice;
		private LabelEx.LabelVista cmdSelectSettingGrid;
		private LabelEx.LabelVista cmdSelectLadder;
		private System.Windows.Forms.Panel panelMISC;
		private System.Windows.Forms.Panel panelPriceBottom;
		private LabelEx.LabelBase dspTradedPrice;
		private System.Windows.Forms.Label lblTradedPrice;
		private LabelEx.LabelVista lblMarketDataError;
		private System.Windows.Forms.Panel panelPriceTop;
		private LabelEx.LabelBase dspAskPrice;
		private System.Windows.Forms.Label lblAskPrice;
		private LabelEx.LabelBase dspBidPrice;
		private System.Windows.Forms.Label lblBidPrice;
	}
}