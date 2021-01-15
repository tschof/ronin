namespace ROC
{
	partial class frmOptionTicket
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
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(frmOptionTicket));
			DataGridViewEx.DGVBasePrinter dgvBasePrinter1 = new DataGridViewEx.DGVBasePrinter();
			System.Drawing.StringFormat stringFormat1 = new System.Drawing.StringFormat();
			System.Drawing.StringFormat stringFormat2 = new System.Drawing.StringFormat();
			System.Drawing.StringFormat stringFormat3 = new System.Drawing.StringFormat();
			System.Drawing.StringFormat stringFormat4 = new System.Drawing.StringFormat();
			this.CaptionBar = new FormEx.VistaWindowCaptionBar();
			this.lblTradeFor = new System.Windows.Forms.ToolStripLabel();
			this.cmdTradeFor = new System.Windows.Forms.ToolStripDropDownButton();
			this.lblLocalAccountAcrn = new System.Windows.Forms.ToolStripLabel();
			this.cmdAccount = new System.Windows.Forms.ToolStripDropDownButton();
			this.cmdSettings = new System.Windows.Forms.ToolStripDropDownButton();
			this.itemSaveAsDefault = new System.Windows.Forms.ToolStripMenuItem();
			this.itemResetToSystemDefault = new System.Windows.Forms.ToolStripMenuItem();
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
			this.cboSymbolDetails = new System.Windows.Forms.ComboBox();
			this.panelOrderInfo = new System.Windows.Forms.Panel();
			this.panelOrderInfoMsg = new System.Windows.Forms.Panel();
			this.lblDoNotTranslate = new LabelEx.LabelVista();
			this.dspOrderMsg = new System.Windows.Forms.RichTextBox();
			this.panelOrderInfoUpper = new System.Windows.Forms.Panel();
			this.panelStrike = new System.Windows.Forms.Panel();
			this.cmdCenterOnStrike = new ButtonEx.VistaButton();
			this.cboStrike = new System.Windows.Forms.ComboBox();
			this.lblStrike = new System.Windows.Forms.Label();
			this.panelExpiration = new System.Windows.Forms.Panel();
			this.chkMonthlyOnly = new System.Windows.Forms.CheckBox();
			this.dspExpiration = new System.Windows.Forms.TextBox();
			this.cboExpiration = new System.Windows.Forms.ComboBox();
			this.lblExpiration = new System.Windows.Forms.Label();
			this.panelExchange = new System.Windows.Forms.Panel();
			this.cboExchange = new System.Windows.Forms.ComboBox();
			this.lblExchange = new System.Windows.Forms.Label();
			this.panelCallPut = new System.Windows.Forms.Panel();
			this.cboCallPut = new System.Windows.Forms.ComboBox();
			this.lblCallPut = new System.Windows.Forms.Label();
			this.panelDuration = new System.Windows.Forms.Panel();
			this.cboDuration = new System.Windows.Forms.ComboBox();
			this.lblDuration = new System.Windows.Forms.Label();
			this.panelStopPrice = new System.Windows.Forms.Panel();
			this.dspStopPrice = new System.Windows.Forms.TextBox();
			this.dspStopPriceTickSize = new LabelEx.LabelBase();
			this.lblStopPriceX = new LabelEx.LabelBase();
			this.dspStopPriceIncrement = new LabelEx.LabelBase();
			this.numStopPrice = new NumericUpDownEx.NumericUpDownBase();
			this.lblStopPrice = new System.Windows.Forms.Label();
			this.panelLimitPrice = new System.Windows.Forms.Panel();
			this.dspLimitPrice = new System.Windows.Forms.TextBox();
			this.dspLimitPriceTickSize = new LabelEx.LabelBase();
			this.lblLimitPriceX = new LabelEx.LabelBase();
			this.dspLimitPriceIncrement = new LabelEx.LabelBase();
			this.numLimitPrice = new NumericUpDownEx.NumericUpDownBase();
			this.lblLimitPrice = new System.Windows.Forms.Label();
			this.panelOrderTypes = new System.Windows.Forms.Panel();
			this.cboOrder = new System.Windows.Forms.ComboBox();
			this.lblOrderType = new System.Windows.Forms.Label();
			this.panelShares = new System.Windows.Forms.Panel();
			this.dspQuantityIncrement = new LabelEx.LabelBase();
			this.numQuantity = new NumericUpDownEx.NumericUpDownBase();
			this.lblQuantity = new System.Windows.Forms.Label();
			this.panelCommands = new System.Windows.Forms.Panel();
			this.cmdSellClose = new ButtonEx.VistaButton();
			this.cmdBuyClose = new ButtonEx.VistaButton();
			this.cmdSellOpen = new ButtonEx.VistaButton();
			this.cmdBuyOpen = new ButtonEx.VistaButton();
			this.panelManagerOption = new PanelEx.PanelManager();
			this.managedPanelStock = new PanelEx.ManagedPanel();
			this.panelExchanges = new System.Windows.Forms.Panel();
			this.panelExchangeSelection = new System.Windows.Forms.Panel();
			this.lblC2 = new LabelEx.LabelBase();
			this.lblBATS = new LabelEx.LabelBase();
			this.lblNSDQ = new LabelEx.LabelBase();
			this.lblPCX = new LabelEx.LabelBase();
			this.lblAMEX = new LabelEx.LabelBase();
			this.lblCBOE = new LabelEx.LabelBase();
			this.lblISE = new LabelEx.LabelBase();
			this.lblBOX = new LabelEx.LabelBase();
			this.lblPHLX = new LabelEx.LabelBase();
			this.cboExpirationToShow = new System.Windows.Forms.ComboBox();
			this.lblExpirationToShow = new System.Windows.Forms.PictureBox();
			this.cboStrikesToShow = new System.Windows.Forms.ComboBox();
			this.lblPUTS = new LabelEx.LabelVista();
			this.lblStrikeToShow = new System.Windows.Forms.PictureBox();
			this.lblCalls = new LabelEx.LabelVista();
			this.managedPanelFuture = new PanelEx.ManagedPanel();
			this.rocOptionList = new DataGridViewEx.ROCOptionList();
			this.imageListTradeTick = new System.Windows.Forms.ImageList(this.components);
			this.menuProcessing = new ROC.menuBaseProcessing();
			this.panelCommandsEx = new System.Windows.Forms.Panel();
			this.panelGrid = new System.Windows.Forms.Panel();
			this.toolStripTraderInfo = new System.Windows.Forms.ToolStrip();
			this.panelTicker.SuspendLayout();
			this.panelMiddle.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.picTradeTick)).BeginInit();
			this.panelUpper.SuspendLayout();
			this.panelSymbolTrade.SuspendLayout();
			this.panelOrderInfo.SuspendLayout();
			this.panelOrderInfoMsg.SuspendLayout();
			this.panelOrderInfoUpper.SuspendLayout();
			this.panelStrike.SuspendLayout();
			this.panelExpiration.SuspendLayout();
			this.panelExchange.SuspendLayout();
			this.panelCallPut.SuspendLayout();
			this.panelDuration.SuspendLayout();
			this.panelStopPrice.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.numStopPrice)).BeginInit();
			this.panelLimitPrice.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.numLimitPrice)).BeginInit();
			this.panelOrderTypes.SuspendLayout();
			this.panelShares.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.numQuantity)).BeginInit();
			this.panelCommands.SuspendLayout();
			this.panelManagerOption.SuspendLayout();
			this.managedPanelStock.SuspendLayout();
			this.panelExchanges.SuspendLayout();
			this.panelExchangeSelection.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.lblExpirationToShow)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.lblStrikeToShow)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.rocOptionList)).BeginInit();
			this.panelGrid.SuspendLayout();
			this.toolStripTraderInfo.SuspendLayout();
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
			this.CaptionBar.Size = new System.Drawing.Size(608, 26);
			this.CaptionBar.TabIndex = 0;
			this.CaptionBar.TabStop = false;
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
			// cmdSettings
			// 
			this.cmdSettings.AutoToolTip = false;
			this.cmdSettings.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
			this.cmdSettings.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.itemSaveAsDefault,
            this.itemResetToSystemDefault});
			this.cmdSettings.Image = ((System.Drawing.Image)(resources.GetObject("cmdSettings.Image")));
			this.cmdSettings.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.cmdSettings.Name = "cmdSettings";
			this.cmdSettings.Size = new System.Drawing.Size(59, 22);
			this.cmdSettings.Text = "Settings";
			// 
			// itemSaveAsDefault
			// 
			this.itemSaveAsDefault.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.itemSaveAsDefault.Name = "itemSaveAsDefault";
			this.itemSaveAsDefault.Size = new System.Drawing.Size(225, 22);
			this.itemSaveAsDefault.Text = "Save As Default";
			this.itemSaveAsDefault.Click += new System.EventHandler(this.itemSaveAsDefault_Click);
			// 
			// itemResetToSystemDefault
			// 
			this.itemResetToSystemDefault.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.itemResetToSystemDefault.Name = "itemResetToSystemDefault";
			this.itemResetToSystemDefault.Size = new System.Drawing.Size(225, 22);
			this.itemResetToSystemDefault.Text = "Reset To System Default";
			this.itemResetToSystemDefault.Click += new System.EventHandler(this.itemResetToSystemDefault_Click);
			// 
			// panelTicker
			// 
			this.panelTicker.BackColor = System.Drawing.Color.Black;
			this.panelTicker.Controls.Add(this.panelMiddle);
			this.panelTicker.Controls.Add(this.panelUpper);
			this.panelTicker.Controls.Add(this.panelSymbolTrade);
			this.panelTicker.Dock = System.Windows.Forms.DockStyle.Top;
			this.panelTicker.ForeColor = System.Drawing.Color.Gainsboro;
			this.panelTicker.Location = new System.Drawing.Point(0, 51);
			this.panelTicker.Name = "panelTicker";
			this.panelTicker.Size = new System.Drawing.Size(608, 30);
			this.panelTicker.TabIndex = 0;
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
			this.panelMiddle.Location = new System.Drawing.Point(100, 15);
			this.panelMiddle.MinimumSize = new System.Drawing.Size(270, 15);
			this.panelMiddle.Name = "panelMiddle";
			this.panelMiddle.Size = new System.Drawing.Size(508, 15);
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
			this.panelUpper.Location = new System.Drawing.Point(100, 0);
			this.panelUpper.MinimumSize = new System.Drawing.Size(270, 15);
			this.panelUpper.Name = "panelUpper";
			this.panelUpper.Size = new System.Drawing.Size(508, 15);
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
			this.panelSymbolTrade.Controls.Add(this.cboSymbolDetails);
			this.panelSymbolTrade.Dock = System.Windows.Forms.DockStyle.Left;
			this.panelSymbolTrade.Location = new System.Drawing.Point(0, 0);
			this.panelSymbolTrade.Name = "panelSymbolTrade";
			this.panelSymbolTrade.Padding = new System.Windows.Forms.Padding(4, 4, 4, 0);
			this.panelSymbolTrade.Size = new System.Drawing.Size(100, 30);
			this.panelSymbolTrade.TabIndex = 0;
			// 
			// cboSymbolDetails
			// 
			this.cboSymbolDetails.BackColor = System.Drawing.Color.Black;
			this.cboSymbolDetails.Dock = System.Windows.Forms.DockStyle.Top;
			this.cboSymbolDetails.Font = new System.Drawing.Font("Tahoma", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cboSymbolDetails.ForeColor = System.Drawing.Color.Gold;
			this.cboSymbolDetails.FormattingEnabled = true;
			this.cboSymbolDetails.Location = new System.Drawing.Point(4, 4);
			this.cboSymbolDetails.Name = "cboSymbolDetails";
			this.cboSymbolDetails.Size = new System.Drawing.Size(92, 22);
			this.cboSymbolDetails.TabIndex = 0;
			this.cboSymbolDetails.TabStop = false;
			this.cboSymbolDetails.KeyUp += new System.Windows.Forms.KeyEventHandler(this.cboSymbolDetails_KeyUp);
			this.cboSymbolDetails.KeyDown += new System.Windows.Forms.KeyEventHandler(this.cboSymbolDetails_KeyDown);
			// 
			// panelOrderInfo
			// 
			this.panelOrderInfo.BackColor = System.Drawing.Color.LightSteelBlue;
			this.panelOrderInfo.Controls.Add(this.panelOrderInfoMsg);
			this.panelOrderInfo.Controls.Add(this.panelOrderInfoUpper);
			this.panelOrderInfo.Dock = System.Windows.Forms.DockStyle.Top;
			this.panelOrderInfo.Location = new System.Drawing.Point(0, 81);
			this.panelOrderInfo.Name = "panelOrderInfo";
			this.panelOrderInfo.Size = new System.Drawing.Size(608, 80);
			this.panelOrderInfo.TabIndex = 0;
			// 
			// panelOrderInfoMsg
			// 
			this.panelOrderInfoMsg.BackColor = System.Drawing.Color.LightSteelBlue;
			this.panelOrderInfoMsg.Controls.Add(this.lblDoNotTranslate);
			this.panelOrderInfoMsg.Controls.Add(this.dspOrderMsg);
			this.panelOrderInfoMsg.Dock = System.Windows.Forms.DockStyle.Fill;
			this.panelOrderInfoMsg.Font = new System.Drawing.Font("Tahoma", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.panelOrderInfoMsg.Location = new System.Drawing.Point(0, 55);
			this.panelOrderInfoMsg.Name = "panelOrderInfoMsg";
			this.panelOrderInfoMsg.Padding = new System.Windows.Forms.Padding(4);
			this.panelOrderInfoMsg.Size = new System.Drawing.Size(608, 25);
			this.panelOrderInfoMsg.TabIndex = 0;
			// 
			// lblDoNotTranslate
			// 
			this.lblDoNotTranslate.AutoColor = false;
			this.lblDoNotTranslate.BlinkBackColor = System.Drawing.Color.Black;
			this.lblDoNotTranslate.BlinkForeColorColor = System.Drawing.Color.Red;
			this.lblDoNotTranslate.BlinkInnerBorderColor = System.Drawing.Color.Black;
			this.lblDoNotTranslate.BlinkOuterBorderColor = System.Drawing.Color.Black;
			this.lblDoNotTranslate.DefaultColor = System.Drawing.Color.White;
			this.lblDoNotTranslate.DefaultText = "Please use CME electronic symbol";
			this.lblDoNotTranslate.DisplayFactor = 1;
			this.lblDoNotTranslate.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblDoNotTranslate.DownColor = System.Drawing.Color.Red;
			this.lblDoNotTranslate.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblDoNotTranslate.Is64Th = false;
			this.lblDoNotTranslate.IsCurrency = false;
			this.lblDoNotTranslate.IsFraction = false;
			this.lblDoNotTranslate.IsPercent = false;
			this.lblDoNotTranslate.Location = new System.Drawing.Point(4, 4);
			this.lblDoNotTranslate.MaxDecimal = 7;
			this.lblDoNotTranslate.Name = "lblDoNotTranslate";
			this.lblDoNotTranslate.OuterBorderColor = System.Drawing.Color.Black;
			this.lblDoNotTranslate.Size = new System.Drawing.Size(208, 17);
			this.lblDoNotTranslate.TabIndex = 1;
			this.lblDoNotTranslate.Text = "Please use CME electronic symbol";
			this.lblDoNotTranslate.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			this.lblDoNotTranslate.TickSize = 0.01;
			this.lblDoNotTranslate.UpColor = System.Drawing.Color.LimeGreen;
			this.lblDoNotTranslate.Value = "";
			this.lblDoNotTranslate.Visible = false;
			// 
			// dspOrderMsg
			// 
			this.dspOrderMsg.BackColor = System.Drawing.Color.White;
			this.dspOrderMsg.BorderStyle = System.Windows.Forms.BorderStyle.None;
			this.dspOrderMsg.Dock = System.Windows.Forms.DockStyle.Fill;
			this.dspOrderMsg.ForeColor = System.Drawing.Color.Black;
			this.dspOrderMsg.Location = new System.Drawing.Point(4, 4);
			this.dspOrderMsg.Multiline = false;
			this.dspOrderMsg.Name = "dspOrderMsg";
			this.dspOrderMsg.ReadOnly = true;
			this.dspOrderMsg.ScrollBars = System.Windows.Forms.RichTextBoxScrollBars.None;
			this.dspOrderMsg.Size = new System.Drawing.Size(600, 17);
			this.dspOrderMsg.TabIndex = 0;
			this.dspOrderMsg.TabStop = false;
			this.dspOrderMsg.Text = "";
			// 
			// panelOrderInfoUpper
			// 
			this.panelOrderInfoUpper.Controls.Add(this.panelStrike);
			this.panelOrderInfoUpper.Controls.Add(this.panelExpiration);
			this.panelOrderInfoUpper.Controls.Add(this.panelExchange);
			this.panelOrderInfoUpper.Controls.Add(this.panelCallPut);
			this.panelOrderInfoUpper.Controls.Add(this.panelDuration);
			this.panelOrderInfoUpper.Controls.Add(this.panelStopPrice);
			this.panelOrderInfoUpper.Controls.Add(this.panelLimitPrice);
			this.panelOrderInfoUpper.Controls.Add(this.panelOrderTypes);
			this.panelOrderInfoUpper.Controls.Add(this.panelShares);
			this.panelOrderInfoUpper.Dock = System.Windows.Forms.DockStyle.Top;
			this.panelOrderInfoUpper.Location = new System.Drawing.Point(0, 0);
			this.panelOrderInfoUpper.Name = "panelOrderInfoUpper";
			this.panelOrderInfoUpper.Size = new System.Drawing.Size(608, 55);
			this.panelOrderInfoUpper.TabIndex = 0;
			// 
			// panelStrike
			// 
			this.panelStrike.Controls.Add(this.cmdCenterOnStrike);
			this.panelStrike.Controls.Add(this.cboStrike);
			this.panelStrike.Controls.Add(this.lblStrike);
			this.panelStrike.Dock = System.Windows.Forms.DockStyle.Left;
			this.panelStrike.Location = new System.Drawing.Point(483, 0);
			this.panelStrike.MinimumSize = new System.Drawing.Size(66, 55);
			this.panelStrike.Name = "panelStrike";
			this.panelStrike.Padding = new System.Windows.Forms.Padding(2);
			this.panelStrike.Size = new System.Drawing.Size(66, 55);
			this.panelStrike.TabIndex = 0;
			// 
			// cmdCenterOnStrike
			// 
			this.cmdCenterOnStrike.AutoSize = true;
			this.cmdCenterOnStrike.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdCenterOnStrike.BackColor = System.Drawing.Color.Silver;
			this.cmdCenterOnStrike.BackgroundImage = global::ROC.Properties.Resources.ToCenter;
			this.cmdCenterOnStrike.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Zoom;
			this.cmdCenterOnStrike.Dock = System.Windows.Forms.DockStyle.Right;
			this.cmdCenterOnStrike.Font = new System.Drawing.Font("Microsoft Sans Serif", 6F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdCenterOnStrike.ForeColor = System.Drawing.Color.DarkBlue;
			this.cmdCenterOnStrike.ImageIndex = 3;
			this.cmdCenterOnStrike.Location = new System.Drawing.Point(48, 38);
			this.cmdCenterOnStrike.MaximumSize = new System.Drawing.Size(16, 16);
			this.cmdCenterOnStrike.MinimumSize = new System.Drawing.Size(16, 16);
			this.cmdCenterOnStrike.Name = "cmdCenterOnStrike";
			this.cmdCenterOnStrike.Size = new System.Drawing.Size(16, 16);
			this.cmdCenterOnStrike.TabIndex = 0;
			this.cmdCenterOnStrike.TabStop = false;
			this.cmdCenterOnStrike.Click += new System.EventHandler(this.cmdCenterOnStrike_Click);
			// 
			// cboStrike
			// 
			this.cboStrike.BackColor = System.Drawing.Color.White;
			this.cboStrike.Dock = System.Windows.Forms.DockStyle.Top;
			this.cboStrike.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cboStrike.ForeColor = System.Drawing.Color.Black;
			this.cboStrike.FormattingEnabled = true;
			this.cboStrike.Location = new System.Drawing.Point(2, 17);
			this.cboStrike.MinimumSize = new System.Drawing.Size(50, 0);
			this.cboStrike.Name = "cboStrike";
			this.cboStrike.Size = new System.Drawing.Size(62, 21);
			this.cboStrike.TabIndex = 0;
			this.cboStrike.TabStop = false;
			this.cboStrike.SelectedIndexChanged += new System.EventHandler(this.Ticket_ValueChanged);
			// 
			// lblStrike
			// 
			this.lblStrike.AutoSize = true;
			this.lblStrike.Dock = System.Windows.Forms.DockStyle.Top;
			this.lblStrike.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblStrike.Location = new System.Drawing.Point(2, 2);
			this.lblStrike.Name = "lblStrike";
			this.lblStrike.Padding = new System.Windows.Forms.Padding(0, 0, 0, 2);
			this.lblStrike.Size = new System.Drawing.Size(34, 15);
			this.lblStrike.TabIndex = 0;
			this.lblStrike.Text = "Strike";
			// 
			// panelExpiration
			// 
			this.panelExpiration.Controls.Add(this.chkMonthlyOnly);
			this.panelExpiration.Controls.Add(this.dspExpiration);
			this.panelExpiration.Controls.Add(this.cboExpiration);
			this.panelExpiration.Controls.Add(this.lblExpiration);
			this.panelExpiration.Dock = System.Windows.Forms.DockStyle.Left;
			this.panelExpiration.Location = new System.Drawing.Point(417, 0);
			this.panelExpiration.MinimumSize = new System.Drawing.Size(66, 55);
			this.panelExpiration.Name = "panelExpiration";
			this.panelExpiration.Padding = new System.Windows.Forms.Padding(2);
			this.panelExpiration.Size = new System.Drawing.Size(66, 55);
			this.panelExpiration.TabIndex = 0;
			// 
			// chkMonthlyOnly
			// 
			this.chkMonthlyOnly.AutoSize = true;
			this.chkMonthlyOnly.Checked = true;
			this.chkMonthlyOnly.CheckState = System.Windows.Forms.CheckState.Checked;
			this.chkMonthlyOnly.Dock = System.Windows.Forms.DockStyle.Top;
			this.chkMonthlyOnly.Location = new System.Drawing.Point(2, 38);
			this.chkMonthlyOnly.Name = "chkMonthlyOnly";
			this.chkMonthlyOnly.Size = new System.Drawing.Size(62, 17);
			this.chkMonthlyOnly.TabIndex = 0;
			this.chkMonthlyOnly.TabStop = false;
			this.chkMonthlyOnly.Text = "Month Only";
			this.chkMonthlyOnly.UseVisualStyleBackColor = true;
			this.chkMonthlyOnly.CheckedChanged += new System.EventHandler(this.chkMonthlyOnly_CheckedChanged);
			// 
			// dspExpiration
			// 
			this.dspExpiration.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
						| System.Windows.Forms.AnchorStyles.Left)
						| System.Windows.Forms.AnchorStyles.Right)));
			this.dspExpiration.BackColor = System.Drawing.Color.White;
			this.dspExpiration.Location = new System.Drawing.Point(2, 17);
			this.dspExpiration.Name = "dspExpiration";
			this.dspExpiration.ReadOnly = true;
			this.dspExpiration.Size = new System.Drawing.Size(44, 20);
			this.dspExpiration.TabIndex = 0;
			this.dspExpiration.TabStop = false;
			this.dspExpiration.WordWrap = false;
			// 
			// cboExpiration
			// 
			this.cboExpiration.BackColor = System.Drawing.Color.White;
			this.cboExpiration.Dock = System.Windows.Forms.DockStyle.Top;
			this.cboExpiration.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cboExpiration.ForeColor = System.Drawing.Color.Black;
			this.cboExpiration.FormattingEnabled = true;
			this.cboExpiration.Location = new System.Drawing.Point(2, 17);
			this.cboExpiration.MinimumSize = new System.Drawing.Size(50, 0);
			this.cboExpiration.Name = "cboExpiration";
			this.cboExpiration.Size = new System.Drawing.Size(62, 21);
			this.cboExpiration.TabIndex = 0;
			this.cboExpiration.TabStop = false;
			this.cboExpiration.MouseClick += new System.Windows.Forms.MouseEventHandler(this.cboExpiration_MouseClick);
			this.cboExpiration.SelectedIndexChanged += new System.EventHandler(this.Ticket_ValueChanged);
			// 
			// lblExpiration
			// 
			this.lblExpiration.AutoEllipsis = true;
			this.lblExpiration.Dock = System.Windows.Forms.DockStyle.Top;
			this.lblExpiration.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblExpiration.Location = new System.Drawing.Point(2, 2);
			this.lblExpiration.Name = "lblExpiration";
			this.lblExpiration.Padding = new System.Windows.Forms.Padding(0, 0, 0, 2);
			this.lblExpiration.Size = new System.Drawing.Size(62, 15);
			this.lblExpiration.TabIndex = 0;
			this.lblExpiration.Text = "Expiration Date";
			// 
			// panelExchange
			// 
			this.panelExchange.Controls.Add(this.cboExchange);
			this.panelExchange.Controls.Add(this.lblExchange);
			this.panelExchange.Dock = System.Windows.Forms.DockStyle.Left;
			this.panelExchange.Location = new System.Drawing.Point(357, 0);
			this.panelExchange.MinimumSize = new System.Drawing.Size(60, 55);
			this.panelExchange.Name = "panelExchange";
			this.panelExchange.Padding = new System.Windows.Forms.Padding(2);
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
			this.cboExchange.Location = new System.Drawing.Point(2, 17);
			this.cboExchange.Name = "cboExchange";
			this.cboExchange.Size = new System.Drawing.Size(56, 21);
			this.cboExchange.TabIndex = 0;
			this.cboExchange.TabStop = false;
			this.cboExchange.SelectedIndexChanged += new System.EventHandler(this.Ticket_ValueChanged);
			// 
			// lblExchange
			// 
			this.lblExchange.AutoSize = true;
			this.lblExchange.Dock = System.Windows.Forms.DockStyle.Top;
			this.lblExchange.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblExchange.Location = new System.Drawing.Point(2, 2);
			this.lblExchange.Name = "lblExchange";
			this.lblExchange.Padding = new System.Windows.Forms.Padding(0, 0, 0, 2);
			this.lblExchange.Size = new System.Drawing.Size(55, 15);
			this.lblExchange.TabIndex = 0;
			this.lblExchange.Text = "Exchange";
			// 
			// panelCallPut
			// 
			this.panelCallPut.Controls.Add(this.cboCallPut);
			this.panelCallPut.Controls.Add(this.lblCallPut);
			this.panelCallPut.Dock = System.Windows.Forms.DockStyle.Left;
			this.panelCallPut.Location = new System.Drawing.Point(301, 0);
			this.panelCallPut.MinimumSize = new System.Drawing.Size(56, 55);
			this.panelCallPut.Name = "panelCallPut";
			this.panelCallPut.Padding = new System.Windows.Forms.Padding(2);
			this.panelCallPut.Size = new System.Drawing.Size(56, 55);
			this.panelCallPut.TabIndex = 0;
			// 
			// cboCallPut
			// 
			this.cboCallPut.BackColor = System.Drawing.Color.White;
			this.cboCallPut.Dock = System.Windows.Forms.DockStyle.Top;
			this.cboCallPut.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cboCallPut.ForeColor = System.Drawing.Color.Black;
			this.cboCallPut.FormattingEnabled = true;
			this.cboCallPut.Location = new System.Drawing.Point(2, 17);
			this.cboCallPut.MinimumSize = new System.Drawing.Size(50, 0);
			this.cboCallPut.Name = "cboCallPut";
			this.cboCallPut.Size = new System.Drawing.Size(52, 21);
			this.cboCallPut.TabIndex = 0;
			this.cboCallPut.TabStop = false;
			this.cboCallPut.SelectedIndexChanged += new System.EventHandler(this.Ticket_ValueChanged);
			// 
			// lblCallPut
			// 
			this.lblCallPut.AutoSize = true;
			this.lblCallPut.Dock = System.Windows.Forms.DockStyle.Top;
			this.lblCallPut.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblCallPut.Location = new System.Drawing.Point(2, 2);
			this.lblCallPut.Name = "lblCallPut";
			this.lblCallPut.Padding = new System.Windows.Forms.Padding(0, 0, 0, 2);
			this.lblCallPut.Size = new System.Drawing.Size(45, 15);
			this.lblCallPut.TabIndex = 0;
			this.lblCallPut.Text = "Call/Put";
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
			this.panelStopPrice.Controls.Add(this.dspStopPrice);
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
			this.panelStopPrice.Visible = false;
			this.panelStopPrice.Click += new System.EventHandler(this.PriceSelection_Click);
			// 
			// dspStopPrice
			// 
			this.dspStopPrice.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
						| System.Windows.Forms.AnchorStyles.Left)
						| System.Windows.Forms.AnchorStyles.Right)));
			this.dspStopPrice.BackColor = System.Drawing.Color.White;
			this.dspStopPrice.Location = new System.Drawing.Point(2, 17);
			this.dspStopPrice.Name = "dspStopPrice";
			this.dspStopPrice.Size = new System.Drawing.Size(43, 20);
			this.dspStopPrice.TabIndex = 0;
			this.dspStopPrice.TabStop = false;
			this.dspStopPrice.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
			this.dspStopPrice.WordWrap = false;
			this.dspStopPrice.MouseLeave += new System.EventHandler(this.dspStopPrice_Leave);
			this.dspStopPrice.KeyDown += new System.Windows.Forms.KeyEventHandler(this.dspStopPrice_KeyDown);
			this.dspStopPrice.KeyUp += new System.Windows.Forms.KeyEventHandler(this.Control_KeyUp);
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
			this.dspStopPriceTickSize.Click += new System.EventHandler(this.PriceSelection_Click);
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
			this.lblStopPriceX.Click += new System.EventHandler(this.PriceSelection_Click);
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
			this.numStopPrice.Name = "numStopPrice";
			this.numStopPrice.Size = new System.Drawing.Size(61, 21);
			this.numStopPrice.TabIndex = 0;
			this.numStopPrice.TabStop = false;
			this.numStopPrice.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
			this.numStopPrice.ValueChanged += new System.EventHandler(this.Ticket_ValueChanged);
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
			this.lblStopPrice.Click += new System.EventHandler(this.PriceSelection_Click);
			// 
			// panelLimitPrice
			// 
			this.panelLimitPrice.BackColor = System.Drawing.Color.CornflowerBlue;
			this.panelLimitPrice.Controls.Add(this.dspLimitPrice);
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
			this.panelLimitPrice.Click += new System.EventHandler(this.PriceSelection_Click);
			// 
			// dspLimitPrice
			// 
			this.dspLimitPrice.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
						| System.Windows.Forms.AnchorStyles.Left)
						| System.Windows.Forms.AnchorStyles.Right)));
			this.dspLimitPrice.BackColor = System.Drawing.Color.White;
			this.dspLimitPrice.Location = new System.Drawing.Point(2, 17);
			this.dspLimitPrice.Name = "dspLimitPrice";
			this.dspLimitPrice.Size = new System.Drawing.Size(43, 20);
			this.dspLimitPrice.TabIndex = 0;
			this.dspLimitPrice.TabStop = false;
			this.dspLimitPrice.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
			this.dspLimitPrice.WordWrap = false;
			this.dspLimitPrice.MouseLeave += new System.EventHandler(this.dspLimitPrice_Leave);
			this.dspLimitPrice.KeyDown += new System.Windows.Forms.KeyEventHandler(this.dspLimitPrice_KeyDown);
			this.dspLimitPrice.KeyUp += new System.Windows.Forms.KeyEventHandler(this.Control_KeyUp);
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
			this.dspLimitPriceTickSize.Click += new System.EventHandler(this.PriceSelection_Click);
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
			this.lblLimitPriceX.Click += new System.EventHandler(this.PriceSelection_Click);
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
			this.lblLimitPrice.Click += new System.EventHandler(this.PriceSelection_Click);
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
			this.dspQuantityIncrement.Location = new System.Drawing.Point(26, 38);
			this.dspQuantityIncrement.MaxDecimal = 0;
			this.dspQuantityIncrement.Name = "dspQuantityIncrement";
			this.dspQuantityIncrement.Size = new System.Drawing.Size(25, 14);
			this.dspQuantityIncrement.TabIndex = 0;
			this.dspQuantityIncrement.Text = "100";
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
			this.panelCommands.Controls.Add(this.cmdSellClose);
			this.panelCommands.Controls.Add(this.cmdBuyClose);
			this.panelCommands.Controls.Add(this.cmdSellOpen);
			this.panelCommands.Controls.Add(this.cmdBuyOpen);
			this.panelCommands.Dock = System.Windows.Forms.DockStyle.Top;
			this.panelCommands.Location = new System.Drawing.Point(0, 161);
			this.panelCommands.MinimumSize = new System.Drawing.Size(261, 24);
			this.panelCommands.Name = "panelCommands";
			this.panelCommands.Padding = new System.Windows.Forms.Padding(2);
			this.panelCommands.Size = new System.Drawing.Size(608, 24);
			this.panelCommands.TabIndex = 0;
			// 
			// cmdSellClose
			// 
			this.cmdSellClose.BackColor = System.Drawing.Color.Orchid;
			this.cmdSellClose.Dock = System.Windows.Forms.DockStyle.Right;
			this.cmdSellClose.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdSellClose.ForeColor = System.Drawing.Color.Black;
			this.cmdSellClose.Location = new System.Drawing.Point(410, 2);
			this.cmdSellClose.MinimumSize = new System.Drawing.Size(98, 20);
			this.cmdSellClose.Name = "cmdSellClose";
			this.cmdSellClose.Size = new System.Drawing.Size(98, 20);
			this.cmdSellClose.TabIndex = 0;
			this.cmdSellClose.TabStop = false;
			this.cmdSellClose.Text = "SELL CLOSE";
			this.cmdSellClose.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
			this.cmdSellClose.UserSplitSize = 20;
			this.cmdSellClose.Click += new System.EventHandler(this.cmdSellClose_Click);
			// 
			// cmdBuyClose
			// 
			this.cmdBuyClose.BackColor = System.Drawing.Color.CornflowerBlue;
			this.cmdBuyClose.Dock = System.Windows.Forms.DockStyle.Left;
			this.cmdBuyClose.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdBuyClose.ForeColor = System.Drawing.Color.Black;
			this.cmdBuyClose.Location = new System.Drawing.Point(100, 2);
			this.cmdBuyClose.MinimumSize = new System.Drawing.Size(98, 20);
			this.cmdBuyClose.Name = "cmdBuyClose";
			this.cmdBuyClose.Size = new System.Drawing.Size(98, 20);
			this.cmdBuyClose.TabIndex = 0;
			this.cmdBuyClose.TabStop = false;
			this.cmdBuyClose.Text = "BUY CLOSE";
			this.cmdBuyClose.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
			this.cmdBuyClose.UserSplitSize = 20;
			this.cmdBuyClose.Click += new System.EventHandler(this.cmdBuyClose_Click);
			// 
			// cmdSellOpen
			// 
			this.cmdSellOpen.BackColor = System.Drawing.Color.Red;
			this.cmdSellOpen.Dock = System.Windows.Forms.DockStyle.Right;
			this.cmdSellOpen.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdSellOpen.ForeColor = System.Drawing.Color.Black;
			this.cmdSellOpen.Location = new System.Drawing.Point(508, 2);
			this.cmdSellOpen.MinimumSize = new System.Drawing.Size(98, 20);
			this.cmdSellOpen.Name = "cmdSellOpen";
			this.cmdSellOpen.Size = new System.Drawing.Size(98, 20);
			this.cmdSellOpen.TabIndex = 0;
			this.cmdSellOpen.TabStop = false;
			this.cmdSellOpen.Text = "SELL OPEN";
			this.cmdSellOpen.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
			this.cmdSellOpen.UserSplitSize = 20;
			this.cmdSellOpen.Click += new System.EventHandler(this.cmdSellOpen_Click);
			// 
			// cmdBuyOpen
			// 
			this.cmdBuyOpen.BackColor = System.Drawing.Color.LimeGreen;
			this.cmdBuyOpen.Dock = System.Windows.Forms.DockStyle.Left;
			this.cmdBuyOpen.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdBuyOpen.ForeColor = System.Drawing.Color.Black;
			this.cmdBuyOpen.Location = new System.Drawing.Point(2, 2);
			this.cmdBuyOpen.MinimumSize = new System.Drawing.Size(98, 20);
			this.cmdBuyOpen.Name = "cmdBuyOpen";
			this.cmdBuyOpen.Size = new System.Drawing.Size(98, 20);
			this.cmdBuyOpen.TabIndex = 0;
			this.cmdBuyOpen.TabStop = false;
			this.cmdBuyOpen.Text = "BUY OPEN";
			this.cmdBuyOpen.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
			this.cmdBuyOpen.UserSplitSize = 20;
			this.cmdBuyOpen.Click += new System.EventHandler(this.cmdBuyOpen_Click);
			// 
			// panelManagerOption
			// 
			this.panelManagerOption.Controls.Add(this.managedPanelStock);
			this.panelManagerOption.Controls.Add(this.managedPanelFuture);
			this.panelManagerOption.Dock = System.Windows.Forms.DockStyle.Top;
			this.panelManagerOption.Location = new System.Drawing.Point(0, 209);
			this.panelManagerOption.MinimumSize = new System.Drawing.Size(261, 24);
			this.panelManagerOption.Name = "panelManagerOption";
			this.panelManagerOption.Padding = new System.Windows.Forms.Padding(2);
			this.panelManagerOption.SelectedIndex = 0;
			this.panelManagerOption.SelectedPanel = this.managedPanelStock;
			this.panelManagerOption.Size = new System.Drawing.Size(608, 24);
			this.panelManagerOption.TabIndex = 0;
			this.panelManagerOption.TabStop = false;
			// 
			// managedPanelStock
			// 
			this.managedPanelStock.BackColor = System.Drawing.Color.LightSteelBlue;
			this.managedPanelStock.Controls.Add(this.panelExchanges);
			this.managedPanelStock.Controls.Add(this.cboExpirationToShow);
			this.managedPanelStock.Controls.Add(this.lblExpirationToShow);
			this.managedPanelStock.Controls.Add(this.cboStrikesToShow);
			this.managedPanelStock.Controls.Add(this.lblPUTS);
			this.managedPanelStock.Controls.Add(this.lblStrikeToShow);
			this.managedPanelStock.Controls.Add(this.lblCalls);
			this.managedPanelStock.Location = new System.Drawing.Point(0, 0);
			this.managedPanelStock.Name = "managedPanelStock";
			this.managedPanelStock.Padding = new System.Windows.Forms.Padding(2);
			this.managedPanelStock.Size = new System.Drawing.Size(608, 24);
			this.managedPanelStock.Text = "managedPanelOption";
			// 
			// panelExchanges
			// 
			this.panelExchanges.AutoSize = true;
			this.panelExchanges.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.panelExchanges.BackColor = System.Drawing.Color.LightSteelBlue;
			this.panelExchanges.Controls.Add(this.panelExchangeSelection);
			this.panelExchanges.Dock = System.Windows.Forms.DockStyle.Fill;
			this.panelExchanges.ForeColor = System.Drawing.Color.White;
			this.panelExchanges.Location = new System.Drawing.Point(200, 2);
			this.panelExchanges.MinimumSize = new System.Drawing.Size(280, 20);
			this.panelExchanges.Name = "panelExchanges";
			this.panelExchanges.Padding = new System.Windows.Forms.Padding(35, 3, 0, 0);
			this.panelExchanges.Size = new System.Drawing.Size(358, 20);
			this.panelExchanges.TabIndex = 0;
			// 
			// panelExchangeSelection
			// 
			this.panelExchangeSelection.AutoSize = true;
			this.panelExchangeSelection.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.panelExchangeSelection.BackColor = System.Drawing.Color.LightSteelBlue;
			this.panelExchangeSelection.Controls.Add(this.lblC2);
			this.panelExchangeSelection.Controls.Add(this.lblBATS);
			this.panelExchangeSelection.Controls.Add(this.lblNSDQ);
			this.panelExchangeSelection.Controls.Add(this.lblPCX);
			this.panelExchangeSelection.Controls.Add(this.lblAMEX);
			this.panelExchangeSelection.Controls.Add(this.lblCBOE);
			this.panelExchangeSelection.Controls.Add(this.lblISE);
			this.panelExchangeSelection.Controls.Add(this.lblBOX);
			this.panelExchangeSelection.Controls.Add(this.lblPHLX);
			this.panelExchangeSelection.Dock = System.Windows.Forms.DockStyle.Left;
			this.panelExchangeSelection.ForeColor = System.Drawing.Color.White;
			this.panelExchangeSelection.Location = new System.Drawing.Point(35, 3);
			this.panelExchangeSelection.MaximumSize = new System.Drawing.Size(281, 14);
			this.panelExchangeSelection.MinimumSize = new System.Drawing.Size(264, 14);
			this.panelExchangeSelection.Name = "panelExchangeSelection";
			this.panelExchangeSelection.Size = new System.Drawing.Size(281, 14);
			this.panelExchangeSelection.TabIndex = 0;
			// 
			// lblC2
			// 
			this.lblC2.AutoColor = false;
			this.lblC2.AutoSize = true;
			this.lblC2.BackColor = System.Drawing.Color.Black;
			this.lblC2.DefaultColor = System.Drawing.Color.White;
			this.lblC2.DefaultText = "";
			this.lblC2.DisplayFactor = 1;
			this.lblC2.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblC2.DownColor = System.Drawing.Color.Red;
			this.lblC2.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblC2.ForeColor = System.Drawing.Color.DarkKhaki;
			this.lblC2.Is64Th = false;
			this.lblC2.IsCurrency = false;
			this.lblC2.IsFraction = false;
			this.lblC2.IsPercent = false;
			this.lblC2.Location = new System.Drawing.Point(262, 0);
			this.lblC2.MaxDecimal = 7;
			this.lblC2.Name = "lblC2";
			this.lblC2.Size = new System.Drawing.Size(20, 13);
			this.lblC2.TabIndex = 2;
			this.lblC2.Text = "C2";
			this.lblC2.TickSize = 0.01;
			this.lblC2.UpColor = System.Drawing.Color.LimeGreen;
			this.lblC2.Value = "";
			this.lblC2.Click += new System.EventHandler(this.ExchangeFilter_Click);
			// 
			// lblBATS
			// 
			this.lblBATS.AutoColor = false;
			this.lblBATS.AutoSize = true;
			this.lblBATS.BackColor = System.Drawing.Color.Black;
			this.lblBATS.DefaultColor = System.Drawing.Color.White;
			this.lblBATS.DefaultText = "";
			this.lblBATS.DisplayFactor = 1;
			this.lblBATS.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblBATS.DownColor = System.Drawing.Color.Red;
			this.lblBATS.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblBATS.ForeColor = System.Drawing.Color.Violet;
			this.lblBATS.Is64Th = false;
			this.lblBATS.IsCurrency = false;
			this.lblBATS.IsFraction = false;
			this.lblBATS.IsPercent = false;
			this.lblBATS.Location = new System.Drawing.Point(227, 0);
			this.lblBATS.MaxDecimal = 7;
			this.lblBATS.Name = "lblBATS";
			this.lblBATS.Size = new System.Drawing.Size(35, 13);
			this.lblBATS.TabIndex = 1;
			this.lblBATS.Text = "BATS";
			this.lblBATS.TickSize = 0.01;
			this.lblBATS.UpColor = System.Drawing.Color.LimeGreen;
			this.lblBATS.Value = "";
			this.lblBATS.Click += new System.EventHandler(this.ExchangeFilter_Click);
			// 
			// lblNSDQ
			// 
			this.lblNSDQ.AutoColor = false;
			this.lblNSDQ.AutoSize = true;
			this.lblNSDQ.BackColor = System.Drawing.Color.Black;
			this.lblNSDQ.DefaultColor = System.Drawing.Color.White;
			this.lblNSDQ.DefaultText = "";
			this.lblNSDQ.DisplayFactor = 1;
			this.lblNSDQ.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblNSDQ.DownColor = System.Drawing.Color.Red;
			this.lblNSDQ.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblNSDQ.Is64Th = false;
			this.lblNSDQ.IsCurrency = false;
			this.lblNSDQ.IsFraction = false;
			this.lblNSDQ.IsPercent = false;
			this.lblNSDQ.Location = new System.Drawing.Point(189, 0);
			this.lblNSDQ.MaxDecimal = 7;
			this.lblNSDQ.Name = "lblNSDQ";
			this.lblNSDQ.Size = new System.Drawing.Size(38, 13);
			this.lblNSDQ.TabIndex = 0;
			this.lblNSDQ.Text = "NSDQ";
			this.lblNSDQ.TickSize = 0.01;
			this.lblNSDQ.UpColor = System.Drawing.Color.LimeGreen;
			this.lblNSDQ.Value = "";
			this.lblNSDQ.Click += new System.EventHandler(this.ExchangeFilter_Click);
			// 
			// lblPCX
			// 
			this.lblPCX.AutoColor = false;
			this.lblPCX.AutoSize = true;
			this.lblPCX.BackColor = System.Drawing.Color.Black;
			this.lblPCX.DefaultColor = System.Drawing.Color.White;
			this.lblPCX.DefaultText = "";
			this.lblPCX.DisplayFactor = 1;
			this.lblPCX.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblPCX.DownColor = System.Drawing.Color.Red;
			this.lblPCX.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblPCX.ForeColor = System.Drawing.Color.Orange;
			this.lblPCX.Is64Th = false;
			this.lblPCX.IsCurrency = false;
			this.lblPCX.IsFraction = false;
			this.lblPCX.IsPercent = false;
			this.lblPCX.Location = new System.Drawing.Point(161, 0);
			this.lblPCX.MaxDecimal = 7;
			this.lblPCX.Name = "lblPCX";
			this.lblPCX.Size = new System.Drawing.Size(28, 13);
			this.lblPCX.TabIndex = 0;
			this.lblPCX.Text = "PCX";
			this.lblPCX.TickSize = 0.01;
			this.lblPCX.UpColor = System.Drawing.Color.LimeGreen;
			this.lblPCX.Value = "";
			this.lblPCX.Click += new System.EventHandler(this.ExchangeFilter_Click);
			// 
			// lblAMEX
			// 
			this.lblAMEX.AutoColor = false;
			this.lblAMEX.AutoSize = true;
			this.lblAMEX.BackColor = System.Drawing.Color.Black;
			this.lblAMEX.DefaultColor = System.Drawing.Color.White;
			this.lblAMEX.DefaultText = "";
			this.lblAMEX.DisplayFactor = 1;
			this.lblAMEX.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblAMEX.DownColor = System.Drawing.Color.Red;
			this.lblAMEX.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblAMEX.ForeColor = System.Drawing.Color.Yellow;
			this.lblAMEX.Is64Th = false;
			this.lblAMEX.IsCurrency = false;
			this.lblAMEX.IsFraction = false;
			this.lblAMEX.IsPercent = false;
			this.lblAMEX.Location = new System.Drawing.Point(124, 0);
			this.lblAMEX.MaxDecimal = 7;
			this.lblAMEX.Name = "lblAMEX";
			this.lblAMEX.Size = new System.Drawing.Size(37, 13);
			this.lblAMEX.TabIndex = 0;
			this.lblAMEX.Text = "AMEX";
			this.lblAMEX.TickSize = 0.01;
			this.lblAMEX.UpColor = System.Drawing.Color.LimeGreen;
			this.lblAMEX.Value = "";
			this.lblAMEX.Click += new System.EventHandler(this.ExchangeFilter_Click);
			// 
			// lblCBOE
			// 
			this.lblCBOE.AutoColor = false;
			this.lblCBOE.AutoSize = true;
			this.lblCBOE.BackColor = System.Drawing.Color.Black;
			this.lblCBOE.DefaultColor = System.Drawing.Color.White;
			this.lblCBOE.DefaultText = "";
			this.lblCBOE.DisplayFactor = 1;
			this.lblCBOE.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblCBOE.DownColor = System.Drawing.Color.Red;
			this.lblCBOE.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblCBOE.ForeColor = System.Drawing.Color.GreenYellow;
			this.lblCBOE.Is64Th = false;
			this.lblCBOE.IsCurrency = false;
			this.lblCBOE.IsFraction = false;
			this.lblCBOE.IsPercent = false;
			this.lblCBOE.Location = new System.Drawing.Point(88, 0);
			this.lblCBOE.MaxDecimal = 7;
			this.lblCBOE.Name = "lblCBOE";
			this.lblCBOE.Size = new System.Drawing.Size(36, 13);
			this.lblCBOE.TabIndex = 0;
			this.lblCBOE.Text = "CBOE";
			this.lblCBOE.TickSize = 0.01;
			this.lblCBOE.UpColor = System.Drawing.Color.LimeGreen;
			this.lblCBOE.Value = "";
			this.lblCBOE.Click += new System.EventHandler(this.ExchangeFilter_Click);
			// 
			// lblISE
			// 
			this.lblISE.AutoColor = false;
			this.lblISE.AutoSize = true;
			this.lblISE.BackColor = System.Drawing.Color.Black;
			this.lblISE.DefaultColor = System.Drawing.Color.White;
			this.lblISE.DefaultText = "";
			this.lblISE.DisplayFactor = 1;
			this.lblISE.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblISE.DownColor = System.Drawing.Color.Red;
			this.lblISE.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblISE.ForeColor = System.Drawing.Color.DeepSkyBlue;
			this.lblISE.Is64Th = false;
			this.lblISE.IsCurrency = false;
			this.lblISE.IsFraction = false;
			this.lblISE.IsPercent = false;
			this.lblISE.Location = new System.Drawing.Point(64, 0);
			this.lblISE.MaxDecimal = 7;
			this.lblISE.Name = "lblISE";
			this.lblISE.Size = new System.Drawing.Size(24, 13);
			this.lblISE.TabIndex = 0;
			this.lblISE.Text = "ISE";
			this.lblISE.TickSize = 0.01;
			this.lblISE.UpColor = System.Drawing.Color.LimeGreen;
			this.lblISE.Value = "";
			this.lblISE.Click += new System.EventHandler(this.ExchangeFilter_Click);
			// 
			// lblBOX
			// 
			this.lblBOX.AutoColor = false;
			this.lblBOX.AutoSize = true;
			this.lblBOX.BackColor = System.Drawing.Color.Black;
			this.lblBOX.DefaultColor = System.Drawing.Color.White;
			this.lblBOX.DefaultText = "";
			this.lblBOX.DisplayFactor = 1;
			this.lblBOX.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblBOX.DownColor = System.Drawing.Color.Red;
			this.lblBOX.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblBOX.ForeColor = System.Drawing.Color.OrangeRed;
			this.lblBOX.Is64Th = false;
			this.lblBOX.IsCurrency = false;
			this.lblBOX.IsFraction = false;
			this.lblBOX.IsPercent = false;
			this.lblBOX.Location = new System.Drawing.Point(35, 0);
			this.lblBOX.MaxDecimal = 7;
			this.lblBOX.Name = "lblBOX";
			this.lblBOX.Size = new System.Drawing.Size(29, 13);
			this.lblBOX.TabIndex = 0;
			this.lblBOX.Text = "BOX";
			this.lblBOX.TickSize = 0.01;
			this.lblBOX.UpColor = System.Drawing.Color.LimeGreen;
			this.lblBOX.Value = "";
			this.lblBOX.Click += new System.EventHandler(this.ExchangeFilter_Click);
			// 
			// lblPHLX
			// 
			this.lblPHLX.AutoColor = false;
			this.lblPHLX.AutoSize = true;
			this.lblPHLX.BackColor = System.Drawing.Color.Black;
			this.lblPHLX.DefaultColor = System.Drawing.Color.White;
			this.lblPHLX.DefaultText = "";
			this.lblPHLX.DisplayFactor = 1;
			this.lblPHLX.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblPHLX.DownColor = System.Drawing.Color.Red;
			this.lblPHLX.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblPHLX.ForeColor = System.Drawing.Color.MediumPurple;
			this.lblPHLX.Is64Th = false;
			this.lblPHLX.IsCurrency = false;
			this.lblPHLX.IsFraction = false;
			this.lblPHLX.IsPercent = false;
			this.lblPHLX.Location = new System.Drawing.Point(0, 0);
			this.lblPHLX.MaxDecimal = 7;
			this.lblPHLX.Name = "lblPHLX";
			this.lblPHLX.Size = new System.Drawing.Size(35, 13);
			this.lblPHLX.TabIndex = 0;
			this.lblPHLX.Text = "PHLX";
			this.lblPHLX.TickSize = 0.01;
			this.lblPHLX.UpColor = System.Drawing.Color.LimeGreen;
			this.lblPHLX.Value = "";
			this.lblPHLX.Click += new System.EventHandler(this.ExchangeFilter_Click);
			// 
			// cboExpirationToShow
			// 
			this.cboExpirationToShow.Dock = System.Windows.Forms.DockStyle.Left;
			this.cboExpirationToShow.DropDownHeight = 136;
			this.cboExpirationToShow.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cboExpirationToShow.FormattingEnabled = true;
			this.cboExpirationToShow.IntegralHeight = false;
			this.cboExpirationToShow.Items.AddRange(new object[] {
            "1",
            "2",
            "3",
            "4",
            "5",
            "6",
            "7",
            "8",
            "9",
            "ALL"});
			this.cboExpirationToShow.Location = new System.Drawing.Point(155, 2);
			this.cboExpirationToShow.Name = "cboExpirationToShow";
			this.cboExpirationToShow.Size = new System.Drawing.Size(45, 21);
			this.cboExpirationToShow.TabIndex = 0;
			this.cboExpirationToShow.TabStop = false;
			this.cboExpirationToShow.Text = "1";
			this.cboExpirationToShow.SelectedIndexChanged += new System.EventHandler(this.OptionRange_SelectedIndexChanged);
			// 
			// lblExpirationToShow
			// 
			this.lblExpirationToShow.BackgroundImage = global::ROC.Properties.Resources.Expiration;
			this.lblExpirationToShow.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Zoom;
			this.lblExpirationToShow.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblExpirationToShow.Location = new System.Drawing.Point(125, 2);
			this.lblExpirationToShow.Name = "lblExpirationToShow";
			this.lblExpirationToShow.Size = new System.Drawing.Size(30, 20);
			this.lblExpirationToShow.TabIndex = 4;
			this.lblExpirationToShow.TabStop = false;
			// 
			// cboStrikesToShow
			// 
			this.cboStrikesToShow.Dock = System.Windows.Forms.DockStyle.Left;
			this.cboStrikesToShow.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cboStrikesToShow.FormattingEnabled = true;
			this.cboStrikesToShow.Items.AddRange(new object[] {
            "5",
            "10",
            "15",
            "20",
            "25",
            "30",
            "ALL"});
			this.cboStrikesToShow.Location = new System.Drawing.Point(80, 2);
			this.cboStrikesToShow.Name = "cboStrikesToShow";
			this.cboStrikesToShow.Size = new System.Drawing.Size(45, 21);
			this.cboStrikesToShow.TabIndex = 0;
			this.cboStrikesToShow.TabStop = false;
			this.cboStrikesToShow.Text = "20";
			this.cboStrikesToShow.SelectedIndexChanged += new System.EventHandler(this.OptionRange_SelectedIndexChanged);
			// 
			// lblPUTS
			// 
			this.lblPUTS.AutoColor = false;
			this.lblPUTS.BackColor = System.Drawing.Color.Blue;
			this.lblPUTS.DefaultColor = System.Drawing.Color.White;
			this.lblPUTS.DefaultText = "PUTS";
			this.lblPUTS.DisplayFactor = 1;
			this.lblPUTS.Dock = System.Windows.Forms.DockStyle.Right;
			this.lblPUTS.DownColor = System.Drawing.Color.Red;
			this.lblPUTS.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblPUTS.ForeColor = System.Drawing.Color.Gold;
			this.lblPUTS.Is64Th = false;
			this.lblPUTS.IsCurrency = false;
			this.lblPUTS.IsFraction = false;
			this.lblPUTS.IsPercent = false;
			this.lblPUTS.Location = new System.Drawing.Point(558, 2);
			this.lblPUTS.MaxDecimal = 0;
			this.lblPUTS.MinimumSize = new System.Drawing.Size(48, 20);
			this.lblPUTS.Name = "lblPUTS";
			this.lblPUTS.Size = new System.Drawing.Size(48, 20);
			this.lblPUTS.TabIndex = 0;
			this.lblPUTS.Text = "PUTS";
			this.lblPUTS.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			this.lblPUTS.TickSize = 0.01;
			this.lblPUTS.UpColor = System.Drawing.Color.LimeGreen;
			this.lblPUTS.Value = "";
			// 
			// lblStrikeToShow
			// 
			this.lblStrikeToShow.BackgroundImage = global::ROC.Properties.Resources.Strike;
			this.lblStrikeToShow.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Zoom;
			this.lblStrikeToShow.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblStrikeToShow.Location = new System.Drawing.Point(50, 2);
			this.lblStrikeToShow.Name = "lblStrikeToShow";
			this.lblStrikeToShow.Size = new System.Drawing.Size(30, 20);
			this.lblStrikeToShow.TabIndex = 3;
			this.lblStrikeToShow.TabStop = false;
			// 
			// lblCalls
			// 
			this.lblCalls.AutoColor = false;
			this.lblCalls.BackColor = System.Drawing.Color.Blue;
			this.lblCalls.DefaultColor = System.Drawing.Color.White;
			this.lblCalls.DefaultText = "CALLS";
			this.lblCalls.DisplayFactor = 1;
			this.lblCalls.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblCalls.DownColor = System.Drawing.Color.Red;
			this.lblCalls.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblCalls.ForeColor = System.Drawing.Color.Gold;
			this.lblCalls.Is64Th = false;
			this.lblCalls.IsCurrency = false;
			this.lblCalls.IsFraction = false;
			this.lblCalls.IsPercent = false;
			this.lblCalls.Location = new System.Drawing.Point(2, 2);
			this.lblCalls.MaxDecimal = 0;
			this.lblCalls.MinimumSize = new System.Drawing.Size(48, 20);
			this.lblCalls.Name = "lblCalls";
			this.lblCalls.Size = new System.Drawing.Size(48, 20);
			this.lblCalls.TabIndex = 0;
			this.lblCalls.Text = "CALLS";
			this.lblCalls.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			this.lblCalls.TickSize = 0.01;
			this.lblCalls.UpColor = System.Drawing.Color.LimeGreen;
			this.lblCalls.Value = "";
			// 
			// managedPanelFuture
			// 
			this.managedPanelFuture.BackColor = System.Drawing.Color.LightSteelBlue;
			this.managedPanelFuture.Location = new System.Drawing.Point(0, 0);
			this.managedPanelFuture.Name = "managedPanelFuture";
			this.managedPanelFuture.Size = new System.Drawing.Size(0, 0);
			this.managedPanelFuture.Text = "managedPanelSSF";
			// 
			// rocOptionList
			// 
			this.rocOptionList.AskColor = System.Drawing.Color.Red;
			this.rocOptionList.AtMoney = 0;
			this.rocOptionList.BidColor = System.Drawing.Color.LimeGreen;
			this.rocOptionList.BlueDefault = 228;
			this.rocOptionList.CallKeys = ((System.Collections.Generic.List<string>)(resources.GetObject("rocOptionList.CallKeys")));
			this.rocOptionList.ColorShiftInterval = 18;
			this.rocOptionList.ColorShiftIntervalMax = 10;
			this.rocOptionList.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
			this.rocOptionList.CommonFactor = 1;
			this.rocOptionList.CurrentExpDate = new System.DateTime(((long)(0)));
			this.rocOptionList.CurrentStrike = 0;
			this.rocOptionList.DefaultAtMoneyColor = System.Drawing.Color.FromArgb(((int)(((byte)(233)))), ((int)(((byte)(243)))), ((int)(((byte)(228)))));
			new BindingListEx.ROCBindingList<string>().Add("");
			this.rocOptionList.DisplayFactors = ((System.Collections.Generic.Dictionary<string, double>)(resources.GetObject("rocOptionList.DisplayFactors")));
			this.rocOptionList.Dock = System.Windows.Forms.DockStyle.Fill;
			this.rocOptionList.ExchangeColors = ((System.Collections.Generic.Dictionary<string, System.Drawing.Color>)(resources.GetObject("rocOptionList.ExchangeColors")));
			this.rocOptionList.ExpirationRange = 1;
			this.rocOptionList.FilterOutAll = false;
			this.rocOptionList.FilterOutAllWithoutFilter = false;
			this.rocOptionList.GreenDefault = 243;
			this.rocOptionList.GridKeys = ((System.Collections.Generic.List<string>)(resources.GetObject("rocOptionList.GridKeys")));
			this.rocOptionList.HeaderBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(233)))), ((int)(((byte)(243)))), ((int)(((byte)(228)))));
			this.rocOptionList.Importing = false;
			this.rocOptionList.LastFilter = "";
			this.rocOptionList.LastSort = "Expiration, Strike";
			this.rocOptionList.Location = new System.Drawing.Point(2, 2);
			this.rocOptionList.MDCallSymbol = "";
			this.rocOptionList.MDCallSymbolOPRA = "";
			this.rocOptionList.MDPutSymbol = "";
			this.rocOptionList.MDPutSymbolOPRA = "";
			this.rocOptionList.MDSource = "";
			this.rocOptionList.Name = "rocOptionList";
			this.rocOptionList.OptionCallPut = "C";
			this.rocOptionList.OptionCallSymbol = "";
			this.rocOptionList.OptionExchange = "";
			this.rocOptionList.OptionExpiration = new System.DateTime(2009, 8, 12, 13, 23, 59, 365);
			this.rocOptionList.OptionLoaded = false;
			this.rocOptionList.OptionPrice = 0;
			this.rocOptionList.OptionPriceSelected = false;
			this.rocOptionList.OptionPutSymbol = "";
			this.rocOptionList.OptionStrike = 0;
			this.rocOptionList.OptionSymbols = ((System.Collections.Generic.List<string>)(resources.GetObject("rocOptionList.OptionSymbols")));
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
			this.rocOptionList.Printer = dgvBasePrinter1;
			this.rocOptionList.Processing = false;
			this.rocOptionList.PutKeys = ((System.Collections.Generic.List<string>)(resources.GetObject("rocOptionList.PutKeys")));
			this.rocOptionList.RedDefault = 233;
			this.rocOptionList.RefreshAggragation = true;
			this.rocOptionList.RefreshNBBO = false;
			this.rocOptionList.RefreshSharedRows = false;
			this.rocOptionList.SecType = "";
			this.rocOptionList.SelectedForeColor = System.Drawing.Color.DarkBlue;
			this.rocOptionList.ShouldScrollToLastRow = false;
			this.rocOptionList.Show64Th = false;
			this.rocOptionList.ShowColumnMenu = false;
			this.rocOptionList.ShowOptionSymbolColumn = false;
			this.rocOptionList.Size = new System.Drawing.Size(604, 209);
			this.rocOptionList.StrikeRange = 20;
			this.rocOptionList.Subscribe = false;
			new BindingListEx.ROCBindingList<string>().Add("");
			this.rocOptionList.TabIndex = 0;
			this.rocOptionList.TabStop = false;
			this.rocOptionList.TickSizes = ((System.Collections.Generic.Dictionary<string, double>)(resources.GetObject("rocOptionList.TickSizes")));
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
			// menuProcessing
			// 
			this.menuProcessing.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
			this.menuProcessing.BackColor = System.Drawing.Color.Transparent;
			this.menuProcessing.Caption = "000";
			this.menuProcessing.Location = new System.Drawing.Point(26, 136);
			this.menuProcessing.MaximumSize = new System.Drawing.Size(51, 51);
			this.menuProcessing.MinimumSize = new System.Drawing.Size(51, 51);
			this.menuProcessing.Name = "menuProcessing";
			this.menuProcessing.Size = new System.Drawing.Size(51, 51);
			this.menuProcessing.TabIndex = 0;
			this.menuProcessing.TabStop = false;
			this.menuProcessing.Visible = false;
			// 
			// panelCommandsEx
			// 
			this.panelCommandsEx.BackColor = System.Drawing.Color.LightSteelBlue;
			this.panelCommandsEx.Dock = System.Windows.Forms.DockStyle.Top;
			this.panelCommandsEx.Location = new System.Drawing.Point(0, 185);
			this.panelCommandsEx.MinimumSize = new System.Drawing.Size(261, 24);
			this.panelCommandsEx.Name = "panelCommandsEx";
			this.panelCommandsEx.Padding = new System.Windows.Forms.Padding(2);
			this.panelCommandsEx.Size = new System.Drawing.Size(608, 24);
			this.panelCommandsEx.TabIndex = 0;
			this.panelCommandsEx.Visible = false;
			// 
			// panelGrid
			// 
			this.panelGrid.BackColor = System.Drawing.Color.LightSteelBlue;
			this.panelGrid.Controls.Add(this.menuProcessing);
			this.panelGrid.Controls.Add(this.rocOptionList);
			this.panelGrid.Dock = System.Windows.Forms.DockStyle.Fill;
			this.panelGrid.Location = new System.Drawing.Point(0, 233);
			this.panelGrid.MinimumSize = new System.Drawing.Size(261, 24);
			this.panelGrid.Name = "panelGrid";
			this.panelGrid.Padding = new System.Windows.Forms.Padding(2);
			this.panelGrid.Size = new System.Drawing.Size(608, 213);
			this.panelGrid.TabIndex = 0;
			// 
			// toolStripTraderInfo
			// 
			this.toolStripTraderInfo.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
			this.toolStripTraderInfo.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.lblTradeFor,
            this.cmdTradeFor,
            this.lblLocalAccountAcrn,
            this.cmdAccount,
            this.cmdSettings});
			this.toolStripTraderInfo.Location = new System.Drawing.Point(0, 26);
			this.toolStripTraderInfo.Name = "toolStripTraderInfo";
			this.toolStripTraderInfo.ShowItemToolTips = false;
			this.toolStripTraderInfo.Size = new System.Drawing.Size(608, 25);
			this.toolStripTraderInfo.TabIndex = 0;
			this.toolStripTraderInfo.Text = "UserInfo";
			// 
			// frmOptionTicket
			// 
			this.Animate = false;
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.Caption = "OPTION TICKET";
			this.ClientSize = new System.Drawing.Size(608, 446);
			this.Controls.Add(this.panelGrid);
			this.Controls.Add(this.panelManagerOption);
			this.Controls.Add(this.panelCommandsEx);
			this.Controls.Add(this.panelCommands);
			this.Controls.Add(this.panelOrderInfo);
			this.Controls.Add(this.panelTicker);
			this.Controls.Add(this.toolStripTraderInfo);
			this.Controls.Add(this.CaptionBar);
			this.MinimumSize = new System.Drawing.Size(558, 454);
			this.Name = "frmOptionTicket";
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
			this.panelOrderInfo.ResumeLayout(false);
			this.panelOrderInfoMsg.ResumeLayout(false);
			this.panelOrderInfoUpper.ResumeLayout(false);
			this.panelStrike.ResumeLayout(false);
			this.panelStrike.PerformLayout();
			this.panelExpiration.ResumeLayout(false);
			this.panelExpiration.PerformLayout();
			this.panelExchange.ResumeLayout(false);
			this.panelExchange.PerformLayout();
			this.panelCallPut.ResumeLayout(false);
			this.panelCallPut.PerformLayout();
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
			this.panelManagerOption.ResumeLayout(false);
			this.managedPanelStock.ResumeLayout(false);
			this.managedPanelStock.PerformLayout();
			this.panelExchanges.ResumeLayout(false);
			this.panelExchanges.PerformLayout();
			this.panelExchangeSelection.ResumeLayout(false);
			this.panelExchangeSelection.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.lblExpirationToShow)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.lblStrikeToShow)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.rocOptionList)).EndInit();
			this.panelGrid.ResumeLayout(false);
			this.toolStripTraderInfo.ResumeLayout(false);
			this.toolStripTraderInfo.PerformLayout();
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private FormEx.VistaWindowCaptionBar CaptionBar;
		private System.Windows.Forms.ToolStripLabel lblTradeFor;
		protected internal System.Windows.Forms.ToolStripDropDownButton cmdTradeFor;
		private System.Windows.Forms.ToolStripLabel lblLocalAccountAcrn;
		protected internal System.Windows.Forms.ToolStripDropDownButton cmdAccount;
		private System.Windows.Forms.ToolStripDropDownButton cmdSettings;
		private System.Windows.Forms.ToolStripMenuItem itemSaveAsDefault;
		private System.Windows.Forms.ToolStripMenuItem itemResetToSystemDefault;
		private System.Windows.Forms.Panel panelTicker;
		private System.Windows.Forms.Panel panelMiddle;
		private System.Windows.Forms.Panel panelUpper;
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
		private LabelEx.LabelBase dspVolume;
		private System.Windows.Forms.Label lblVolume;
		private LabelEx.LabelBase dspPctChange;
		private System.Windows.Forms.Label lblPctChange;
		private LabelEx.LabelBase dspNetChange;
		private System.Windows.Forms.Label lblNetChange;
		private System.Windows.Forms.Panel panelOrderInfo;
		private System.Windows.Forms.Panel panelOrderInfoMsg;
		private System.Windows.Forms.Panel panelOrderInfoUpper;
		private System.Windows.Forms.Panel panelStrike;
		private System.Windows.Forms.ComboBox cboStrike;
		private System.Windows.Forms.Label lblStrike;
		private System.Windows.Forms.Panel panelExpiration;
		private System.Windows.Forms.ComboBox cboExpiration;
		private System.Windows.Forms.Label lblExpiration;
		private System.Windows.Forms.Panel panelExchange;
		protected internal System.Windows.Forms.ComboBox cboExchange;
		protected internal System.Windows.Forms.Label lblExchange;
		private System.Windows.Forms.Panel panelCallPut;
		private System.Windows.Forms.ComboBox cboCallPut;
		private System.Windows.Forms.Label lblCallPut;
		private System.Windows.Forms.Panel panelDuration;
		private System.Windows.Forms.Label lblDuration;
		private System.Windows.Forms.Panel panelStopPrice;
		private LabelEx.LabelBase dspStopPriceTickSize;
		private LabelEx.LabelBase lblStopPriceX;
		private LabelEx.LabelBase dspStopPriceIncrement;
		public NumericUpDownEx.NumericUpDownBase numStopPrice;
		private System.Windows.Forms.Label lblStopPrice;
		private System.Windows.Forms.Panel panelLimitPrice;
		private LabelEx.LabelBase dspLimitPriceTickSize;
		private LabelEx.LabelBase lblLimitPriceX;
		private LabelEx.LabelBase dspLimitPriceIncrement;
		public NumericUpDownEx.NumericUpDownBase numLimitPrice;
		private System.Windows.Forms.Label lblLimitPrice;
		private System.Windows.Forms.Panel panelOrderTypes;
		private System.Windows.Forms.ComboBox cboOrder;
		private System.Windows.Forms.Label lblOrderType;
		private System.Windows.Forms.Panel panelShares;
		private LabelEx.LabelBase dspQuantityIncrement;
		public NumericUpDownEx.NumericUpDownBase numQuantity;
		private System.Windows.Forms.Label lblQuantity;
		private System.Windows.Forms.Panel panelCommands;
		private ButtonEx.VistaButton cmdSellClose;
		private ButtonEx.VistaButton cmdBuyClose;
		private ButtonEx.VistaButton cmdSellOpen;
		private ButtonEx.VistaButton cmdBuyOpen;
		private PanelEx.PanelManager panelManagerOption;
		private PanelEx.ManagedPanel managedPanelStock;
		private LabelEx.LabelVista lblPUTS;
		private LabelEx.LabelVista lblCalls;
		private PanelEx.ManagedPanel managedPanelFuture;
		private System.Windows.Forms.Panel panelExchanges;
		private System.Windows.Forms.Panel panelExchangeSelection;
		private LabelEx.LabelBase lblNSDQ;
		private LabelEx.LabelBase lblPCX;
		private LabelEx.LabelBase lblAMEX;
		private LabelEx.LabelBase lblCBOE;
		private LabelEx.LabelBase lblISE;
		private LabelEx.LabelBase lblBOX;
		private LabelEx.LabelBase lblPHLX;
		private DataGridViewEx.ROCOptionList rocOptionList;
		private System.Windows.Forms.ImageList imageListTradeTick;
		private System.Windows.Forms.ComboBox cboDuration;
		private menuBaseProcessing menuProcessing;
		private LabelEx.LabelVista lblMarketDataError;
		private System.Windows.Forms.RichTextBox dspOrderMsg;
		private ButtonEx.VistaButton cmdCenterOnStrike;
		protected internal System.Windows.Forms.Panel panelCommandsEx;
		private System.Windows.Forms.TextBox dspExpiration;
		private System.Windows.Forms.TextBox dspStopPrice;
		private System.Windows.Forms.TextBox dspLimitPrice;
		protected internal System.Windows.Forms.Panel panelGrid;
		private System.Windows.Forms.ComboBox cboStrikesToShow;
		private System.Windows.Forms.ComboBox cboExpirationToShow;
		private System.Windows.Forms.PictureBox lblStrikeToShow;
		private System.Windows.Forms.PictureBox lblExpirationToShow;
		private System.Windows.Forms.ToolStrip toolStripTraderInfo;
		private LabelEx.LabelBase lblBATS;
		private LabelEx.LabelVista lblSecurityStatus;
		private System.Windows.Forms.CheckBox chkMonthlyOnly;
		private LabelEx.LabelVista lblDoNotTranslate;
		private LabelEx.LabelBase lblC2;
	}
}