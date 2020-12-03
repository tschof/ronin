namespace ROC
{
	partial class frmStockTicket
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
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(frmStockTicket));
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
			this.panelUserSelection = new System.Windows.Forms.Panel();
			this.toolStripTraderInfo = new System.Windows.Forms.ToolStrip();
			this.lblTradeFor = new System.Windows.Forms.ToolStripLabel();
			this.cmdTradeFor = new System.Windows.Forms.ToolStripDropDownButton();
			this.lblLocalAccountAcrn = new System.Windows.Forms.ToolStripLabel();
			this.cmdAccount = new System.Windows.Forms.ToolStripDropDownButton();
			this.cmdLevel = new System.Windows.Forms.ToolStripDropDownButton();
			this.itemLevel1 = new System.Windows.Forms.ToolStripMenuItem();
			this.itemLevel2 = new System.Windows.Forms.ToolStripMenuItem();
			this.cmdSettings = new System.Windows.Forms.ToolStripDropDownButton();
			this.itemSaveAsDefault = new System.Windows.Forms.ToolStripMenuItem();
			this.itemResetToSystemDefault = new System.Windows.Forms.ToolStripMenuItem();
			this.panelCommands = new System.Windows.Forms.Panel();
			this.cmdShort = new ButtonEx.VistaButton();
			this.cmdSell = new ButtonEx.VistaButton();
			this.cmdBuy = new ButtonEx.VistaButton();
			this.panelTicker = new System.Windows.Forms.Panel();
			this.panelBottom = new System.Windows.Forms.Panel();
			this.dspClosePrice = new LabelEx.LabelBase();
			this.lblClosePrice = new System.Windows.Forms.Label();
			this.dspHighPrice = new LabelEx.LabelBase();
			this.lblHighPrice = new System.Windows.Forms.Label();
			this.dspLowPrice = new LabelEx.LabelBase();
			this.lblLowPrice = new System.Windows.Forms.Label();
			this.dspTradeVolume = new LabelEx.LabelBase();
			this.lblTradeVolume = new System.Windows.Forms.Label();
			this.lblSecurityStatus = new LabelEx.LabelVista();
			this.lblMarketDataError = new LabelEx.LabelVista();
			this.panelMiddle = new System.Windows.Forms.Panel();
			this.dspVolume = new LabelEx.LabelBase();
			this.lblVolume = new System.Windows.Forms.Label();
			this.dspPctChange = new LabelEx.LabelBase();
			this.lblPctChange = new System.Windows.Forms.Label();
			this.dspNetChange = new LabelEx.LabelBase();
			this.lblNetChange = new System.Windows.Forms.Label();
			this.panelUpper = new System.Windows.Forms.Panel();
			this.dspAskSize = new LabelEx.LabelBase();
			this.lblAskSize = new System.Windows.Forms.Label();
			this.dspAskPrice = new LabelEx.LabelBase();
			this.lblName = new System.Windows.Forms.Label();
			this.dspBidSize = new LabelEx.LabelBase();
			this.lblBidSize = new System.Windows.Forms.Label();
			this.dspBidPrice = new LabelEx.LabelBase();
			this.lblBidPrice = new System.Windows.Forms.Label();
			this.panelSymbolTrade = new System.Windows.Forms.Panel();
			this.dspTradedPrice = new LabelEx.LabelBase();
			this.picTradeTick = new System.Windows.Forms.PictureBox();
			this.cboSymbolDetails = new System.Windows.Forms.ComboBox();
			this.panelOrderInfo = new System.Windows.Forms.Panel();
			this.panelOrderInfoMsg = new System.Windows.Forms.Panel();
			this.dspOrderMsg = new System.Windows.Forms.RichTextBox();
			this.panelAlgo = new System.Windows.Forms.Panel();
			this.panelAlgoMVP = new System.Windows.Forms.Panel();
			this.numMVP = new NumericUpDownEx.NumericUpDownBase();
			this.chkMVP = new System.Windows.Forms.CheckBox();
			this.panelEndTime = new System.Windows.Forms.Panel();
			this.dtpEndTime = new System.Windows.Forms.DateTimePicker();
			this.chkEndTime = new System.Windows.Forms.CheckBox();
			this.panelStartTime = new System.Windows.Forms.Panel();
			this.dtpStartTime = new System.Windows.Forms.DateTimePicker();
			this.chkStartTime = new System.Windows.Forms.CheckBox();
			this.panelAlgoExchange = new System.Windows.Forms.Panel();
			this.cboAlgoExchange = new System.Windows.Forms.ComboBox();
			this.chkAlgoExchange = new System.Windows.Forms.CheckBox();
			this.panelAlgoType = new System.Windows.Forms.Panel();
			this.cboAlgoType = new System.Windows.Forms.ComboBox();
			this.lblAlgoType = new System.Windows.Forms.Label();
			this.panelOrderInfoLower = new System.Windows.Forms.Panel();
			this.panelMinShares = new System.Windows.Forms.Panel();
			this.dspMinQuantityIncrement = new LabelEx.LabelBase();
			this.numMinQuantity = new NumericUpDownEx.NumericUpDownBase();
			this.chkMinQty = new System.Windows.Forms.CheckBox();
			this.panelInstructions = new System.Windows.Forms.Panel();
			this.cboInstructions = new System.Windows.Forms.ComboBox();
			this.chkInstructions = new System.Windows.Forms.CheckBox();
			this.panelShortLender = new System.Windows.Forms.Panel();
			this.cboShortLenders = new System.Windows.Forms.ComboBox();
			this.lblShortLender = new System.Windows.Forms.Label();
			this.panelExchange = new System.Windows.Forms.Panel();
			this.cboExchange = new System.Windows.Forms.ComboBox();
			this.lblExchange = new System.Windows.Forms.Label();
			this.panelShow = new System.Windows.Forms.Panel();
			this.dspShowIncrement = new LabelEx.LabelBase();
			this.numShow = new NumericUpDownEx.NumericUpDownBase();
			this.chkShow = new System.Windows.Forms.CheckBox();
			this.panelOrderInfoUpper = new System.Windows.Forms.Panel();
			this.panelDuration = new System.Windows.Forms.Panel();
			this.cboDuration = new System.Windows.Forms.ComboBox();
			this.lblDuration = new System.Windows.Forms.Label();
			this.panelPegPrice = new System.Windows.Forms.Panel();
			this.dspPegPrice = new System.Windows.Forms.TextBox();
			this.dspPegPriceTickSize = new LabelEx.LabelBase();
			this.lblPegPriceX = new LabelEx.LabelBase();
			this.dspPegPriceIncrement = new LabelEx.LabelBase();
			this.numPegPrice = new NumericUpDownEx.NumericUpDownBase();
			this.lblPegPrice = new System.Windows.Forms.Label();
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
			this.imageListTradeTick = new System.Windows.Forms.ImageList(this.components);
			this.panelLevel2 = new System.Windows.Forms.Panel();
			this.splitContainerLevel2 = new System.Windows.Forms.SplitContainer();
			this.rocLevel2ListBid = new DataGridViewEx.ROCLevel2List();
			this.rocLevel2ListAsk = new DataGridViewEx.ROCLevel2List();
			this.CaptionBar = new FormEx.VistaWindowCaptionBar();
			this.panelCommandsEx = new System.Windows.Forms.Panel();
			this.panelUserSelection.SuspendLayout();
			this.toolStripTraderInfo.SuspendLayout();
			this.panelCommands.SuspendLayout();
			this.panelTicker.SuspendLayout();
			this.panelBottom.SuspendLayout();
			this.panelMiddle.SuspendLayout();
			this.panelUpper.SuspendLayout();
			this.panelSymbolTrade.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.picTradeTick)).BeginInit();
			this.panelOrderInfo.SuspendLayout();
			this.panelOrderInfoMsg.SuspendLayout();
			this.panelAlgo.SuspendLayout();
			this.panelAlgoMVP.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.numMVP)).BeginInit();
			this.panelEndTime.SuspendLayout();
			this.panelStartTime.SuspendLayout();
			this.panelAlgoExchange.SuspendLayout();
			this.panelAlgoType.SuspendLayout();
			this.panelOrderInfoLower.SuspendLayout();
			this.panelMinShares.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.numMinQuantity)).BeginInit();
			this.panelInstructions.SuspendLayout();
			this.panelShortLender.SuspendLayout();
			this.panelExchange.SuspendLayout();
			this.panelShow.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.numShow)).BeginInit();
			this.panelOrderInfoUpper.SuspendLayout();
			this.panelDuration.SuspendLayout();
			this.panelPegPrice.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.numPegPrice)).BeginInit();
			this.panelStopPrice.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.numStopPrice)).BeginInit();
			this.panelLimitPrice.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.numLimitPrice)).BeginInit();
			this.panelOrderTypes.SuspendLayout();
			this.panelShares.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.numQuantity)).BeginInit();
			this.panelLevel2.SuspendLayout();
			this.splitContainerLevel2.Panel1.SuspendLayout();
			this.splitContainerLevel2.Panel2.SuspendLayout();
			this.splitContainerLevel2.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.rocLevel2ListBid)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.rocLevel2ListAsk)).BeginInit();
			this.SuspendLayout();
			// 
			// panelUserSelection
			// 
			this.panelUserSelection.BackColor = System.Drawing.Color.LightSteelBlue;
			this.panelUserSelection.Controls.Add(this.toolStripTraderInfo);
			this.panelUserSelection.Dock = System.Windows.Forms.DockStyle.Top;
			this.panelUserSelection.Font = new System.Drawing.Font("Tahoma", 6F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.panelUserSelection.Location = new System.Drawing.Point(0, 26);
			this.panelUserSelection.Name = "panelUserSelection";
			this.panelUserSelection.Padding = new System.Windows.Forms.Padding(0, 0, 0, 2);
			this.panelUserSelection.Size = new System.Drawing.Size(440, 26);
			this.panelUserSelection.TabIndex = 0;
			// 
			// toolStripTraderInfo
			// 
			this.toolStripTraderInfo.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
			this.toolStripTraderInfo.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.lblTradeFor,
            this.cmdTradeFor,
            this.lblLocalAccountAcrn,
            this.cmdAccount,
            this.cmdLevel,
            this.cmdSettings});
			this.toolStripTraderInfo.Location = new System.Drawing.Point(0, 0);
			this.toolStripTraderInfo.Name = "toolStripTraderInfo";
			this.toolStripTraderInfo.ShowItemToolTips = false;
			this.toolStripTraderInfo.Size = new System.Drawing.Size(440, 25);
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
			// cmdLevel
			// 
			this.cmdLevel.AutoToolTip = false;
			this.cmdLevel.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
			this.cmdLevel.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.itemLevel1,
            this.itemLevel2});
			this.cmdLevel.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdLevel.Image = ((System.Drawing.Image)(resources.GetObject("cmdLevel.Image")));
			this.cmdLevel.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.cmdLevel.Name = "cmdLevel";
			this.cmdLevel.Size = new System.Drawing.Size(60, 22);
			this.cmdLevel.Text = "Level 1";
			// 
			// itemLevel1
			// 
			this.itemLevel1.Name = "itemLevel1";
			this.itemLevel1.Size = new System.Drawing.Size(125, 22);
			this.itemLevel1.Text = "Level 1";
			this.itemLevel1.Click += new System.EventHandler(this.Level_Changed);
			// 
			// itemLevel2
			// 
			this.itemLevel2.Name = "itemLevel2";
			this.itemLevel2.Size = new System.Drawing.Size(125, 22);
			this.itemLevel2.Text = "Level 2";
			this.itemLevel2.Click += new System.EventHandler(this.Level_Changed);
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
			// panelCommands
			// 
			this.panelCommands.BackColor = System.Drawing.Color.LightSteelBlue;
			this.panelCommands.Controls.Add(this.cmdShort);
			this.panelCommands.Controls.Add(this.cmdSell);
			this.panelCommands.Controls.Add(this.cmdBuy);
			this.panelCommands.Dock = System.Windows.Forms.DockStyle.Top;
			this.panelCommands.Location = new System.Drawing.Point(0, 272);
			this.panelCommands.MinimumSize = new System.Drawing.Size(308, 24);
			this.panelCommands.Name = "panelCommands";
			this.panelCommands.Padding = new System.Windows.Forms.Padding(2);
			this.panelCommands.Size = new System.Drawing.Size(440, 24);
			this.panelCommands.TabIndex = 0;
			// 
			// cmdShort
			// 
			this.cmdShort.BackColor = System.Drawing.Color.Orchid;
			this.cmdShort.Dock = System.Windows.Forms.DockStyle.Fill;
			this.cmdShort.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdShort.ForeColor = System.Drawing.Color.Black;
			this.cmdShort.Location = new System.Drawing.Point(102, 2);
			this.cmdShort.MinimumSize = new System.Drawing.Size(100, 20);
			this.cmdShort.Name = "cmdShort";
			this.cmdShort.Size = new System.Drawing.Size(236, 20);
			this.cmdShort.TabIndex = 0;
			this.cmdShort.TabStop = false;
			this.cmdShort.Text = "SHORT";
			this.cmdShort.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
			this.cmdShort.UserSplitSize = 20;
			this.cmdShort.Click += new System.EventHandler(this.cmdShort_Click);
			// 
			// cmdSell
			// 
			this.cmdSell.BackColor = System.Drawing.Color.Red;
			this.cmdSell.Dock = System.Windows.Forms.DockStyle.Right;
			this.cmdSell.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdSell.ForeColor = System.Drawing.Color.Black;
			this.cmdSell.Location = new System.Drawing.Point(338, 2);
			this.cmdSell.MinimumSize = new System.Drawing.Size(100, 20);
			this.cmdSell.Name = "cmdSell";
			this.cmdSell.Size = new System.Drawing.Size(100, 20);
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
			this.cmdBuy.MinimumSize = new System.Drawing.Size(100, 20);
			this.cmdBuy.Name = "cmdBuy";
			this.cmdBuy.Size = new System.Drawing.Size(100, 20);
			this.cmdBuy.TabIndex = 0;
			this.cmdBuy.TabStop = false;
			this.cmdBuy.Text = "BUY";
			this.cmdBuy.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
			this.cmdBuy.UserSplitSize = 20;
			this.cmdBuy.Click += new System.EventHandler(this.cmdBuy_Click);
			// 
			// panelTicker
			// 
			this.panelTicker.BackColor = System.Drawing.Color.Black;
			this.panelTicker.Controls.Add(this.panelBottom);
			this.panelTicker.Controls.Add(this.panelMiddle);
			this.panelTicker.Controls.Add(this.panelUpper);
			this.panelTicker.Controls.Add(this.panelSymbolTrade);
			this.panelTicker.Dock = System.Windows.Forms.DockStyle.Top;
			this.panelTicker.ForeColor = System.Drawing.Color.Gainsboro;
			this.panelTicker.Location = new System.Drawing.Point(0, 52);
			this.panelTicker.Name = "panelTicker";
			this.panelTicker.Size = new System.Drawing.Size(440, 45);
			this.panelTicker.TabIndex = 0;
			// 
			// panelBottom
			// 
			this.panelBottom.AutoSize = true;
			this.panelBottom.Controls.Add(this.dspClosePrice);
			this.panelBottom.Controls.Add(this.lblClosePrice);
			this.panelBottom.Controls.Add(this.dspHighPrice);
			this.panelBottom.Controls.Add(this.lblHighPrice);
			this.panelBottom.Controls.Add(this.dspLowPrice);
			this.panelBottom.Controls.Add(this.lblLowPrice);
			this.panelBottom.Controls.Add(this.dspTradeVolume);
			this.panelBottom.Controls.Add(this.lblTradeVolume);
			this.panelBottom.Controls.Add(this.lblSecurityStatus);
			this.panelBottom.Controls.Add(this.lblMarketDataError);
			this.panelBottom.Dock = System.Windows.Forms.DockStyle.Top;
			this.panelBottom.Location = new System.Drawing.Point(91, 30);
			this.panelBottom.MinimumSize = new System.Drawing.Size(217, 15);
			this.panelBottom.Name = "panelBottom";
			this.panelBottom.Size = new System.Drawing.Size(349, 15);
			this.panelBottom.TabIndex = 0;
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
			this.dspClosePrice.IsFraction = false;
			this.dspClosePrice.IsPercent = false;
			this.dspClosePrice.Location = new System.Drawing.Point(202, 0);
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
			this.dspClosePrice.MouseDown += new System.Windows.Forms.MouseEventHandler(this.Price_MouseDown);
			this.dspClosePrice.MouseUp += new System.Windows.Forms.MouseEventHandler(this.Price_MouseUp);
			// 
			// lblClosePrice
			// 
			this.lblClosePrice.AutoSize = true;
			this.lblClosePrice.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblClosePrice.ForeColor = System.Drawing.Color.Khaki;
			this.lblClosePrice.Location = new System.Drawing.Point(185, 0);
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
			this.dspHighPrice.IsFraction = false;
			this.dspHighPrice.IsPercent = false;
			this.dspHighPrice.Location = new System.Drawing.Point(154, 0);
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
			this.dspHighPrice.MouseDown += new System.Windows.Forms.MouseEventHandler(this.Price_MouseDown);
			this.dspHighPrice.MouseUp += new System.Windows.Forms.MouseEventHandler(this.Price_MouseUp);
			// 
			// lblHighPrice
			// 
			this.lblHighPrice.AutoSize = true;
			this.lblHighPrice.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblHighPrice.ForeColor = System.Drawing.Color.Khaki;
			this.lblHighPrice.Location = new System.Drawing.Point(139, 0);
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
			this.dspLowPrice.IsFraction = false;
			this.dspLowPrice.IsPercent = false;
			this.dspLowPrice.Location = new System.Drawing.Point(108, 0);
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
			this.dspLowPrice.MouseDown += new System.Windows.Forms.MouseEventHandler(this.Price_MouseDown);
			this.dspLowPrice.MouseUp += new System.Windows.Forms.MouseEventHandler(this.Price_MouseUp);
			// 
			// lblLowPrice
			// 
			this.lblLowPrice.AutoSize = true;
			this.lblLowPrice.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblLowPrice.ForeColor = System.Drawing.Color.Khaki;
			this.lblLowPrice.Location = new System.Drawing.Point(93, 0);
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
			this.dspTradeVolume.Location = new System.Drawing.Point(79, 0);
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
			this.lblTradeVolume.Location = new System.Drawing.Point(57, 0);
			this.lblTradeVolume.MaximumSize = new System.Drawing.Size(22, 14);
			this.lblTradeVolume.MinimumSize = new System.Drawing.Size(22, 14);
			this.lblTradeVolume.Name = "lblTradeVolume";
			this.lblTradeVolume.Padding = new System.Windows.Forms.Padding(0, 1, 0, 0);
			this.lblTradeVolume.Size = new System.Drawing.Size(22, 14);
			this.lblTradeVolume.TabIndex = 0;
			this.lblTradeVolume.Text = "tv";
			this.lblTradeVolume.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
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
			this.lblSecurityStatus.TabIndex = 1;
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
			// panelMiddle
			// 
			this.panelMiddle.AutoSize = true;
			this.panelMiddle.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.panelMiddle.Controls.Add(this.dspVolume);
			this.panelMiddle.Controls.Add(this.lblVolume);
			this.panelMiddle.Controls.Add(this.dspPctChange);
			this.panelMiddle.Controls.Add(this.lblPctChange);
			this.panelMiddle.Controls.Add(this.dspNetChange);
			this.panelMiddle.Controls.Add(this.lblNetChange);
			this.panelMiddle.Dock = System.Windows.Forms.DockStyle.Top;
			this.panelMiddle.Location = new System.Drawing.Point(91, 15);
			this.panelMiddle.MinimumSize = new System.Drawing.Size(217, 15);
			this.panelMiddle.Name = "panelMiddle";
			this.panelMiddle.Size = new System.Drawing.Size(349, 15);
			this.panelMiddle.TabIndex = 0;
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
			this.dspVolume.Location = new System.Drawing.Point(141, 0);
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
			this.lblVolume.Location = new System.Drawing.Point(119, 0);
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
			this.dspPctChange.Location = new System.Drawing.Point(75, 0);
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
			this.lblPctChange.Location = new System.Drawing.Point(53, 0);
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
			this.dspNetChange.IsFraction = false;
			this.dspNetChange.IsPercent = false;
			this.dspNetChange.Location = new System.Drawing.Point(22, 0);
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
			this.lblNetChange.Location = new System.Drawing.Point(0, 0);
			this.lblNetChange.MaximumSize = new System.Drawing.Size(22, 14);
			this.lblNetChange.MinimumSize = new System.Drawing.Size(22, 14);
			this.lblNetChange.Name = "lblNetChange";
			this.lblNetChange.Padding = new System.Windows.Forms.Padding(0, 1, 0, 0);
			this.lblNetChange.Size = new System.Drawing.Size(22, 14);
			this.lblNetChange.TabIndex = 0;
			this.lblNetChange.Text = "net";
			this.lblNetChange.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// panelUpper
			// 
			this.panelUpper.AutoSize = true;
			this.panelUpper.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.panelUpper.Controls.Add(this.dspAskSize);
			this.panelUpper.Controls.Add(this.lblAskSize);
			this.panelUpper.Controls.Add(this.dspAskPrice);
			this.panelUpper.Controls.Add(this.lblName);
			this.panelUpper.Controls.Add(this.dspBidSize);
			this.panelUpper.Controls.Add(this.lblBidSize);
			this.panelUpper.Controls.Add(this.dspBidPrice);
			this.panelUpper.Controls.Add(this.lblBidPrice);
			this.panelUpper.Dock = System.Windows.Forms.DockStyle.Top;
			this.panelUpper.Location = new System.Drawing.Point(91, 0);
			this.panelUpper.MinimumSize = new System.Drawing.Size(217, 15);
			this.panelUpper.Name = "panelUpper";
			this.panelUpper.Size = new System.Drawing.Size(349, 15);
			this.panelUpper.TabIndex = 0;
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
			this.dspAskPrice.IsFraction = false;
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
			this.dspAskPrice.MouseDown += new System.Windows.Forms.MouseEventHandler(this.Price_MouseDown);
			this.dspAskPrice.MouseUp += new System.Windows.Forms.MouseEventHandler(this.Price_MouseUp);
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
			this.dspBidPrice.IsFraction = false;
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
			this.dspBidPrice.MouseDown += new System.Windows.Forms.MouseEventHandler(this.Price_MouseDown);
			this.dspBidPrice.MouseUp += new System.Windows.Forms.MouseEventHandler(this.Price_MouseUp);
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
			this.panelSymbolTrade.Controls.Add(this.dspTradedPrice);
			this.panelSymbolTrade.Controls.Add(this.picTradeTick);
			this.panelSymbolTrade.Controls.Add(this.cboSymbolDetails);
			this.panelSymbolTrade.Dock = System.Windows.Forms.DockStyle.Left;
			this.panelSymbolTrade.Location = new System.Drawing.Point(0, 0);
			this.panelSymbolTrade.Name = "panelSymbolTrade";
			this.panelSymbolTrade.Size = new System.Drawing.Size(91, 45);
			this.panelSymbolTrade.TabIndex = 0;
			// 
			// dspTradedPrice
			// 
			this.dspTradedPrice.AutoColor = false;
			this.dspTradedPrice.AutoSize = true;
			this.dspTradedPrice.DefaultColor = System.Drawing.Color.White;
			this.dspTradedPrice.DefaultText = "0.00";
			this.dspTradedPrice.DisplayFactor = 1;
			this.dspTradedPrice.Dock = System.Windows.Forms.DockStyle.Right;
			this.dspTradedPrice.DownColor = System.Drawing.Color.Red;
			this.dspTradedPrice.Font = new System.Drawing.Font("Tahoma", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspTradedPrice.Is64Th = false;
			this.dspTradedPrice.IsCurrency = false;
			this.dspTradedPrice.IsFraction = false;
			this.dspTradedPrice.IsPercent = false;
			this.dspTradedPrice.Location = new System.Drawing.Point(32, 22);
			this.dspTradedPrice.MaxDecimal = 7;
			this.dspTradedPrice.Name = "dspTradedPrice";
			this.dspTradedPrice.Padding = new System.Windows.Forms.Padding(0, 3, 0, 0);
			this.dspTradedPrice.Size = new System.Drawing.Size(36, 19);
			this.dspTradedPrice.TabIndex = 0;
			this.dspTradedPrice.Text = "0.00";
			this.dspTradedPrice.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.dspTradedPrice.TickSize = 0.01;
			this.dspTradedPrice.UpColor = System.Drawing.Color.LimeGreen;
			this.dspTradedPrice.Value = "";
			this.dspTradedPrice.MouseDown += new System.Windows.Forms.MouseEventHandler(this.Price_MouseDown);
			this.dspTradedPrice.MouseUp += new System.Windows.Forms.MouseEventHandler(this.Price_MouseUp);
			// 
			// picTradeTick
			// 
			this.picTradeTick.Dock = System.Windows.Forms.DockStyle.Right;
			this.picTradeTick.Location = new System.Drawing.Point(68, 22);
			this.picTradeTick.MaximumSize = new System.Drawing.Size(23, 23);
			this.picTradeTick.MinimumSize = new System.Drawing.Size(23, 23);
			this.picTradeTick.Name = "picTradeTick";
			this.picTradeTick.Padding = new System.Windows.Forms.Padding(1);
			this.picTradeTick.Size = new System.Drawing.Size(23, 23);
			this.picTradeTick.TabIndex = 0;
			this.picTradeTick.TabStop = false;
			// 
			// cboSymbolDetails
			// 
			this.cboSymbolDetails.BackColor = System.Drawing.Color.Black;
			this.cboSymbolDetails.Dock = System.Windows.Forms.DockStyle.Top;
			this.cboSymbolDetails.Font = new System.Drawing.Font("Tahoma", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cboSymbolDetails.ForeColor = System.Drawing.Color.Gold;
			this.cboSymbolDetails.FormattingEnabled = true;
			this.cboSymbolDetails.Location = new System.Drawing.Point(0, 0);
			this.cboSymbolDetails.Name = "cboSymbolDetails";
			this.cboSymbolDetails.Size = new System.Drawing.Size(91, 22);
			this.cboSymbolDetails.TabIndex = 0;
			this.cboSymbolDetails.TabStop = false;
			this.cboSymbolDetails.KeyUp += new System.Windows.Forms.KeyEventHandler(this.cboSymbolDetails_KeyUp);
			this.cboSymbolDetails.KeyDown += new System.Windows.Forms.KeyEventHandler(this.cboSymbolDetails_KeyDown);
			// 
			// panelOrderInfo
			// 
			this.panelOrderInfo.BackColor = System.Drawing.Color.LightSteelBlue;
			this.panelOrderInfo.Controls.Add(this.panelOrderInfoMsg);
			this.panelOrderInfo.Controls.Add(this.panelAlgo);
			this.panelOrderInfo.Controls.Add(this.panelOrderInfoLower);
			this.panelOrderInfo.Controls.Add(this.panelOrderInfoUpper);
			this.panelOrderInfo.Dock = System.Windows.Forms.DockStyle.Top;
			this.panelOrderInfo.Location = new System.Drawing.Point(0, 97);
			this.panelOrderInfo.MinimumSize = new System.Drawing.Size(308, 135);
			this.panelOrderInfo.Name = "panelOrderInfo";
			this.panelOrderInfo.Size = new System.Drawing.Size(440, 175);
			this.panelOrderInfo.TabIndex = 0;
			// 
			// panelOrderInfoMsg
			// 
			this.panelOrderInfoMsg.BackColor = System.Drawing.Color.LightSteelBlue;
			this.panelOrderInfoMsg.Controls.Add(this.dspOrderMsg);
			this.panelOrderInfoMsg.Dock = System.Windows.Forms.DockStyle.Top;
			this.panelOrderInfoMsg.Font = new System.Drawing.Font("Tahoma", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.panelOrderInfoMsg.Location = new System.Drawing.Point(0, 150);
			this.panelOrderInfoMsg.Name = "panelOrderInfoMsg";
			this.panelOrderInfoMsg.Padding = new System.Windows.Forms.Padding(4);
			this.panelOrderInfoMsg.Size = new System.Drawing.Size(440, 25);
			this.panelOrderInfoMsg.TabIndex = 0;
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
			this.dspOrderMsg.Size = new System.Drawing.Size(432, 17);
			this.dspOrderMsg.TabIndex = 0;
			this.dspOrderMsg.TabStop = false;
			this.dspOrderMsg.Text = "";
			// 
			// panelAlgo
			// 
			this.panelAlgo.Controls.Add(this.panelAlgoMVP);
			this.panelAlgo.Controls.Add(this.panelEndTime);
			this.panelAlgo.Controls.Add(this.panelStartTime);
			this.panelAlgo.Controls.Add(this.panelAlgoExchange);
			this.panelAlgo.Controls.Add(this.panelAlgoType);
			this.panelAlgo.Dock = System.Windows.Forms.DockStyle.Top;
			this.panelAlgo.Location = new System.Drawing.Point(0, 110);
			this.panelAlgo.MinimumSize = new System.Drawing.Size(308, 40);
			this.panelAlgo.Name = "panelAlgo";
			this.panelAlgo.Size = new System.Drawing.Size(440, 40);
			this.panelAlgo.TabIndex = 0;
			this.panelAlgo.Visible = false;
			this.panelAlgo.VisibleChanged += new System.EventHandler(this.AlgoType_SelectedIndexChanged);
			// 
			// panelAlgoMVP
			// 
			this.panelAlgoMVP.Controls.Add(this.numMVP);
			this.panelAlgoMVP.Controls.Add(this.chkMVP);
			this.panelAlgoMVP.Dock = System.Windows.Forms.DockStyle.Left;
			this.panelAlgoMVP.Location = new System.Drawing.Point(328, 0);
			this.panelAlgoMVP.MinimumSize = new System.Drawing.Size(57, 40);
			this.panelAlgoMVP.Name = "panelAlgoMVP";
			this.panelAlgoMVP.Padding = new System.Windows.Forms.Padding(2);
			this.panelAlgoMVP.Size = new System.Drawing.Size(57, 40);
			this.panelAlgoMVP.TabIndex = 0;
			// 
			// numMVP
			// 
			this.numMVP.BackColor = System.Drawing.Color.White;
			this.numMVP.Dock = System.Windows.Forms.DockStyle.Top;
			this.numMVP.Enabled = false;
			this.numMVP.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.numMVP.ForeColor = System.Drawing.Color.Black;
			this.numMVP.Location = new System.Drawing.Point(2, 19);
			this.numMVP.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
			this.numMVP.Name = "numMVP";
			this.numMVP.Size = new System.Drawing.Size(53, 21);
			this.numMVP.TabIndex = 0;
			this.numMVP.TabStop = false;
			this.numMVP.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
			this.numMVP.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
			// 
			// chkMVP
			// 
			this.chkMVP.AutoSize = true;
			this.chkMVP.BackColor = System.Drawing.Color.LightSteelBlue;
			this.chkMVP.Dock = System.Windows.Forms.DockStyle.Top;
			this.chkMVP.Location = new System.Drawing.Point(2, 2);
			this.chkMVP.Name = "chkMVP";
			this.chkMVP.RightToLeft = System.Windows.Forms.RightToLeft.Yes;
			this.chkMVP.Size = new System.Drawing.Size(53, 17);
			this.chkMVP.TabIndex = 0;
			this.chkMVP.TabStop = false;
			this.chkMVP.Text = "MVP";
			this.chkMVP.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.chkMVP.UseVisualStyleBackColor = false;
			this.chkMVP.CheckedChanged += new System.EventHandler(this.chkMVP_CheckedChanged);
			// 
			// panelEndTime
			// 
			this.panelEndTime.Controls.Add(this.dtpEndTime);
			this.panelEndTime.Controls.Add(this.chkEndTime);
			this.panelEndTime.Dock = System.Windows.Forms.DockStyle.Left;
			this.panelEndTime.Location = new System.Drawing.Point(241, 0);
			this.panelEndTime.MinimumSize = new System.Drawing.Size(87, 40);
			this.panelEndTime.Name = "panelEndTime";
			this.panelEndTime.Padding = new System.Windows.Forms.Padding(2);
			this.panelEndTime.Size = new System.Drawing.Size(87, 40);
			this.panelEndTime.TabIndex = 0;
			// 
			// dtpEndTime
			// 
			this.dtpEndTime.CustomFormat = "   HH:mm";
			this.dtpEndTime.Dock = System.Windows.Forms.DockStyle.Top;
			this.dtpEndTime.Enabled = false;
			this.dtpEndTime.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dtpEndTime.Format = System.Windows.Forms.DateTimePickerFormat.Custom;
			this.dtpEndTime.Location = new System.Drawing.Point(2, 19);
			this.dtpEndTime.Name = "dtpEndTime";
			this.dtpEndTime.ShowUpDown = true;
			this.dtpEndTime.Size = new System.Drawing.Size(83, 20);
			this.dtpEndTime.TabIndex = 0;
			// 
			// chkEndTime
			// 
			this.chkEndTime.AutoSize = true;
			this.chkEndTime.BackColor = System.Drawing.Color.LightSteelBlue;
			this.chkEndTime.Dock = System.Windows.Forms.DockStyle.Top;
			this.chkEndTime.Location = new System.Drawing.Point(2, 2);
			this.chkEndTime.Name = "chkEndTime";
			this.chkEndTime.RightToLeft = System.Windows.Forms.RightToLeft.Yes;
			this.chkEndTime.Size = new System.Drawing.Size(83, 17);
			this.chkEndTime.TabIndex = 0;
			this.chkEndTime.TabStop = false;
			this.chkEndTime.Text = "End Time";
			this.chkEndTime.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.chkEndTime.UseVisualStyleBackColor = false;
			this.chkEndTime.CheckedChanged += new System.EventHandler(this.chkEndTime_CheckedChanged);
			// 
			// panelStartTime
			// 
			this.panelStartTime.Controls.Add(this.dtpStartTime);
			this.panelStartTime.Controls.Add(this.chkStartTime);
			this.panelStartTime.Dock = System.Windows.Forms.DockStyle.Left;
			this.panelStartTime.Location = new System.Drawing.Point(150, 0);
			this.panelStartTime.MinimumSize = new System.Drawing.Size(87, 40);
			this.panelStartTime.Name = "panelStartTime";
			this.panelStartTime.Padding = new System.Windows.Forms.Padding(2);
			this.panelStartTime.Size = new System.Drawing.Size(91, 40);
			this.panelStartTime.TabIndex = 0;
			// 
			// dtpStartTime
			// 
			this.dtpStartTime.CustomFormat = "   HH:mm";
			this.dtpStartTime.Dock = System.Windows.Forms.DockStyle.Top;
			this.dtpStartTime.Enabled = false;
			this.dtpStartTime.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dtpStartTime.Format = System.Windows.Forms.DateTimePickerFormat.Custom;
			this.dtpStartTime.Location = new System.Drawing.Point(2, 19);
			this.dtpStartTime.Name = "dtpStartTime";
			this.dtpStartTime.ShowUpDown = true;
			this.dtpStartTime.Size = new System.Drawing.Size(87, 20);
			this.dtpStartTime.TabIndex = 0;
			// 
			// chkStartTime
			// 
			this.chkStartTime.AutoSize = true;
			this.chkStartTime.BackColor = System.Drawing.Color.LightSteelBlue;
			this.chkStartTime.Dock = System.Windows.Forms.DockStyle.Top;
			this.chkStartTime.Location = new System.Drawing.Point(2, 2);
			this.chkStartTime.Name = "chkStartTime";
			this.chkStartTime.RightToLeft = System.Windows.Forms.RightToLeft.Yes;
			this.chkStartTime.Size = new System.Drawing.Size(87, 17);
			this.chkStartTime.TabIndex = 0;
			this.chkStartTime.TabStop = false;
			this.chkStartTime.Text = "Start Time";
			this.chkStartTime.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.chkStartTime.UseVisualStyleBackColor = false;
			this.chkStartTime.CheckedChanged += new System.EventHandler(this.chkStartTime_CheckedChanged);
			// 
			// panelAlgoExchange
			// 
			this.panelAlgoExchange.Controls.Add(this.cboAlgoExchange);
			this.panelAlgoExchange.Controls.Add(this.chkAlgoExchange);
			this.panelAlgoExchange.Dock = System.Windows.Forms.DockStyle.Left;
			this.panelAlgoExchange.Location = new System.Drawing.Point(63, 0);
			this.panelAlgoExchange.MinimumSize = new System.Drawing.Size(72, 40);
			this.panelAlgoExchange.Name = "panelAlgoExchange";
			this.panelAlgoExchange.Padding = new System.Windows.Forms.Padding(2);
			this.panelAlgoExchange.Size = new System.Drawing.Size(87, 40);
			this.panelAlgoExchange.TabIndex = 0;
			this.panelAlgoExchange.Visible = false;
			// 
			// cboAlgoExchange
			// 
			this.cboAlgoExchange.BackColor = System.Drawing.Color.WhiteSmoke;
			this.cboAlgoExchange.Dock = System.Windows.Forms.DockStyle.Fill;
			this.cboAlgoExchange.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.cboAlgoExchange.Enabled = false;
			this.cboAlgoExchange.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cboAlgoExchange.ForeColor = System.Drawing.Color.Black;
			this.cboAlgoExchange.FormattingEnabled = true;
			this.cboAlgoExchange.Location = new System.Drawing.Point(2, 19);
			this.cboAlgoExchange.Name = "cboAlgoExchange";
			this.cboAlgoExchange.Size = new System.Drawing.Size(83, 21);
			this.cboAlgoExchange.TabIndex = 0;
			this.cboAlgoExchange.TabStop = false;
			// 
			// chkAlgoExchange
			// 
			this.chkAlgoExchange.AutoSize = true;
			this.chkAlgoExchange.BackColor = System.Drawing.Color.LightSteelBlue;
			this.chkAlgoExchange.Dock = System.Windows.Forms.DockStyle.Top;
			this.chkAlgoExchange.Location = new System.Drawing.Point(2, 2);
			this.chkAlgoExchange.Name = "chkAlgoExchange";
			this.chkAlgoExchange.RightToLeft = System.Windows.Forms.RightToLeft.Yes;
			this.chkAlgoExchange.Size = new System.Drawing.Size(83, 17);
			this.chkAlgoExchange.TabIndex = 0;
			this.chkAlgoExchange.TabStop = false;
			this.chkAlgoExchange.Text = "Algo Exch.";
			this.chkAlgoExchange.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.chkAlgoExchange.UseVisualStyleBackColor = false;
			this.chkAlgoExchange.CheckedChanged += new System.EventHandler(this.chkAlgoExchange_CheckedChanged);
			// 
			// panelAlgoType
			// 
			this.panelAlgoType.Controls.Add(this.cboAlgoType);
			this.panelAlgoType.Controls.Add(this.lblAlgoType);
			this.panelAlgoType.Dock = System.Windows.Forms.DockStyle.Left;
			this.panelAlgoType.Location = new System.Drawing.Point(0, 0);
			this.panelAlgoType.MinimumSize = new System.Drawing.Size(63, 40);
			this.panelAlgoType.Name = "panelAlgoType";
			this.panelAlgoType.Padding = new System.Windows.Forms.Padding(2, 4, 2, 2);
			this.panelAlgoType.Size = new System.Drawing.Size(63, 40);
			this.panelAlgoType.TabIndex = 0;
			this.panelAlgoType.Visible = false;
			// 
			// cboAlgoType
			// 
			this.cboAlgoType.BackColor = System.Drawing.Color.WhiteSmoke;
			this.cboAlgoType.Dock = System.Windows.Forms.DockStyle.Fill;
			this.cboAlgoType.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.cboAlgoType.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cboAlgoType.ForeColor = System.Drawing.Color.Black;
			this.cboAlgoType.FormattingEnabled = true;
			this.cboAlgoType.Location = new System.Drawing.Point(2, 19);
			this.cboAlgoType.Name = "cboAlgoType";
			this.cboAlgoType.Size = new System.Drawing.Size(59, 21);
			this.cboAlgoType.TabIndex = 0;
			this.cboAlgoType.TabStop = false;
			this.cboAlgoType.SelectedIndexChanged += new System.EventHandler(this.AlgoType_SelectedIndexChanged);
			// 
			// lblAlgoType
			// 
			this.lblAlgoType.AutoSize = true;
			this.lblAlgoType.Dock = System.Windows.Forms.DockStyle.Top;
			this.lblAlgoType.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblAlgoType.Location = new System.Drawing.Point(2, 4);
			this.lblAlgoType.Name = "lblAlgoType";
			this.lblAlgoType.Padding = new System.Windows.Forms.Padding(0, 0, 0, 2);
			this.lblAlgoType.Size = new System.Drawing.Size(28, 15);
			this.lblAlgoType.TabIndex = 0;
			this.lblAlgoType.Text = "Algo";
			// 
			// panelOrderInfoLower
			// 
			this.panelOrderInfoLower.Controls.Add(this.panelMinShares);
			this.panelOrderInfoLower.Controls.Add(this.panelInstructions);
			this.panelOrderInfoLower.Controls.Add(this.panelShortLender);
			this.panelOrderInfoLower.Controls.Add(this.panelExchange);
			this.panelOrderInfoLower.Controls.Add(this.panelShow);
			this.panelOrderInfoLower.Dock = System.Windows.Forms.DockStyle.Top;
			this.panelOrderInfoLower.Location = new System.Drawing.Point(0, 55);
			this.panelOrderInfoLower.MinimumSize = new System.Drawing.Size(308, 55);
			this.panelOrderInfoLower.Name = "panelOrderInfoLower";
			this.panelOrderInfoLower.Size = new System.Drawing.Size(440, 55);
			this.panelOrderInfoLower.TabIndex = 0;
			// 
			// panelMinShares
			// 
			this.panelMinShares.Controls.Add(this.dspMinQuantityIncrement);
			this.panelMinShares.Controls.Add(this.numMinQuantity);
			this.panelMinShares.Controls.Add(this.chkMinQty);
			this.panelMinShares.Dock = System.Windows.Forms.DockStyle.Left;
			this.panelMinShares.Location = new System.Drawing.Point(298, 0);
			this.panelMinShares.MinimumSize = new System.Drawing.Size(68, 55);
			this.panelMinShares.Name = "panelMinShares";
			this.panelMinShares.Padding = new System.Windows.Forms.Padding(2);
			this.panelMinShares.Size = new System.Drawing.Size(68, 55);
			this.panelMinShares.TabIndex = 0;
			// 
			// dspMinQuantityIncrement
			// 
			this.dspMinQuantityIncrement.AutoColor = false;
			this.dspMinQuantityIncrement.AutoSize = true;
			this.dspMinQuantityIncrement.BackColor = System.Drawing.SystemColors.ControlText;
			this.dspMinQuantityIncrement.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.dspMinQuantityIncrement.DefaultColor = System.Drawing.Color.White;
			this.dspMinQuantityIncrement.DefaultText = "100";
			this.dspMinQuantityIncrement.DisplayFactor = 1;
			this.dspMinQuantityIncrement.Dock = System.Windows.Forms.DockStyle.Right;
			this.dspMinQuantityIncrement.DownColor = System.Drawing.Color.Red;
			this.dspMinQuantityIncrement.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspMinQuantityIncrement.ForeColor = System.Drawing.Color.Gainsboro;
			this.dspMinQuantityIncrement.Is64Th = false;
			this.dspMinQuantityIncrement.IsCurrency = false;
			this.dspMinQuantityIncrement.IsFraction = false;
			this.dspMinQuantityIncrement.IsPercent = false;
			this.dspMinQuantityIncrement.Location = new System.Drawing.Point(41, 40);
			this.dspMinQuantityIncrement.MaxDecimal = 0;
			this.dspMinQuantityIncrement.Name = "dspMinQuantityIncrement";
			this.dspMinQuantityIncrement.Size = new System.Drawing.Size(25, 14);
			this.dspMinQuantityIncrement.TabIndex = 0;
			this.dspMinQuantityIncrement.Text = "100";
			this.dspMinQuantityIncrement.TickSize = 0.01;
			this.dspMinQuantityIncrement.UpColor = System.Drawing.Color.LimeGreen;
			this.dspMinQuantityIncrement.Value = "";
			this.dspMinQuantityIncrement.MouseClick += new System.Windows.Forms.MouseEventHandler(this.dspMinQuantityIncrement_MouseClick);
			// 
			// numMinQuantity
			// 
			this.numMinQuantity.BackColor = System.Drawing.Color.White;
			this.numMinQuantity.Dock = System.Windows.Forms.DockStyle.Top;
			this.numMinQuantity.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.numMinQuantity.ForeColor = System.Drawing.Color.Black;
			this.numMinQuantity.Increment = new decimal(new int[] {
            100,
            0,
            0,
            0});
			this.numMinQuantity.Location = new System.Drawing.Point(2, 19);
			this.numMinQuantity.Maximum = new decimal(new int[] {
            1215752191,
            23,
            0,
            0});
			this.numMinQuantity.Name = "numMinQuantity";
			this.numMinQuantity.Size = new System.Drawing.Size(64, 21);
			this.numMinQuantity.TabIndex = 0;
			this.numMinQuantity.TabStop = false;
			this.numMinQuantity.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
			this.numMinQuantity.ValueChanged += new System.EventHandler(this.Ticket_ValueChanged);
			// 
			// chkMinQty
			// 
			this.chkMinQty.AutoSize = true;
			this.chkMinQty.Dock = System.Windows.Forms.DockStyle.Top;
			this.chkMinQty.Location = new System.Drawing.Point(2, 2);
			this.chkMinQty.Name = "chkMinQty";
			this.chkMinQty.RightToLeft = System.Windows.Forms.RightToLeft.Yes;
			this.chkMinQty.Size = new System.Drawing.Size(64, 17);
			this.chkMinQty.TabIndex = 0;
			this.chkMinQty.TabStop = false;
			this.chkMinQty.Text = "Min Qty";
			this.chkMinQty.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.chkMinQty.UseVisualStyleBackColor = false;
			this.chkMinQty.CheckedChanged += new System.EventHandler(this.Ticket_ValueChanged);
			// 
			// panelInstructions
			// 
			this.panelInstructions.Controls.Add(this.cboInstructions);
			this.panelInstructions.Controls.Add(this.chkInstructions);
			this.panelInstructions.Dock = System.Windows.Forms.DockStyle.Left;
			this.panelInstructions.Location = new System.Drawing.Point(219, 0);
			this.panelInstructions.MinimumSize = new System.Drawing.Size(79, 55);
			this.panelInstructions.Name = "panelInstructions";
			this.panelInstructions.Padding = new System.Windows.Forms.Padding(2);
			this.panelInstructions.Size = new System.Drawing.Size(79, 55);
			this.panelInstructions.TabIndex = 0;
			// 
			// cboInstructions
			// 
			this.cboInstructions.BackColor = System.Drawing.Color.White;
			this.cboInstructions.Dock = System.Windows.Forms.DockStyle.Fill;
			this.cboInstructions.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cboInstructions.ForeColor = System.Drawing.Color.Black;
			this.cboInstructions.FormattingEnabled = true;
			this.cboInstructions.Location = new System.Drawing.Point(2, 19);
			this.cboInstructions.Name = "cboInstructions";
			this.cboInstructions.Size = new System.Drawing.Size(75, 21);
			this.cboInstructions.TabIndex = 0;
			this.cboInstructions.TabStop = false;
			this.cboInstructions.SelectedIndexChanged += new System.EventHandler(this.cboInstructions_SelectedIndexChanged);
			this.cboInstructions.KeyUp += new System.Windows.Forms.KeyEventHandler(this.cboInstructions_KeyUp);
			this.cboInstructions.KeyDown += new System.Windows.Forms.KeyEventHandler(this.cboInstructions_KeyDown);
			this.cboInstructions.TextChanged += new System.EventHandler(this.Ticket_ValueChanged);
			// 
			// chkInstructions
			// 
			this.chkInstructions.AutoSize = true;
			this.chkInstructions.BackColor = System.Drawing.Color.LightSteelBlue;
			this.chkInstructions.Dock = System.Windows.Forms.DockStyle.Top;
			this.chkInstructions.Location = new System.Drawing.Point(2, 2);
			this.chkInstructions.Name = "chkInstructions";
			this.chkInstructions.RightToLeft = System.Windows.Forms.RightToLeft.Yes;
			this.chkInstructions.Size = new System.Drawing.Size(75, 17);
			this.chkInstructions.TabIndex = 0;
			this.chkInstructions.TabStop = false;
			this.chkInstructions.Text = "Instruction";
			this.chkInstructions.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.chkInstructions.UseVisualStyleBackColor = false;
			this.chkInstructions.CheckedChanged += new System.EventHandler(this.Ticket_ValueChanged);
			// 
			// panelShortLender
			// 
			this.panelShortLender.Controls.Add(this.cboShortLenders);
			this.panelShortLender.Controls.Add(this.lblShortLender);
			this.panelShortLender.Dock = System.Windows.Forms.DockStyle.Left;
			this.panelShortLender.Location = new System.Drawing.Point(140, 0);
			this.panelShortLender.MinimumSize = new System.Drawing.Size(79, 55);
			this.panelShortLender.Name = "panelShortLender";
			this.panelShortLender.Padding = new System.Windows.Forms.Padding(2, 3, 2, 2);
			this.panelShortLender.Size = new System.Drawing.Size(79, 55);
			this.panelShortLender.TabIndex = 0;
			// 
			// cboShortLenders
			// 
			this.cboShortLenders.BackColor = System.Drawing.Color.White;
			this.cboShortLenders.Dock = System.Windows.Forms.DockStyle.Fill;
			this.cboShortLenders.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cboShortLenders.ForeColor = System.Drawing.Color.Black;
			this.cboShortLenders.FormattingEnabled = true;
			this.cboShortLenders.Location = new System.Drawing.Point(2, 18);
			this.cboShortLenders.Name = "cboShortLenders";
			this.cboShortLenders.Size = new System.Drawing.Size(75, 21);
			this.cboShortLenders.TabIndex = 0;
			this.cboShortLenders.TabStop = false;
			this.cboShortLenders.KeyUp += new System.Windows.Forms.KeyEventHandler(this.cboShortLenders_KeyUp);
			this.cboShortLenders.KeyDown += new System.Windows.Forms.KeyEventHandler(this.cboShortLenders_KeyDown);
			this.cboShortLenders.TextChanged += new System.EventHandler(this.Ticket_ValueChanged);
			// 
			// lblShortLender
			// 
			this.lblShortLender.AutoSize = true;
			this.lblShortLender.Dock = System.Windows.Forms.DockStyle.Top;
			this.lblShortLender.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblShortLender.Location = new System.Drawing.Point(2, 3);
			this.lblShortLender.Name = "lblShortLender";
			this.lblShortLender.Padding = new System.Windows.Forms.Padding(0, 0, 0, 2);
			this.lblShortLender.Size = new System.Drawing.Size(73, 15);
			this.lblShortLender.TabIndex = 0;
			this.lblShortLender.Text = "Short Lenders";
			// 
			// panelExchange
			// 
			this.panelExchange.Controls.Add(this.cboExchange);
			this.panelExchange.Controls.Add(this.lblExchange);
			this.panelExchange.Dock = System.Windows.Forms.DockStyle.Left;
			this.panelExchange.Location = new System.Drawing.Point(68, 0);
			this.panelExchange.MinimumSize = new System.Drawing.Size(72, 55);
			this.panelExchange.Name = "panelExchange";
			this.panelExchange.Padding = new System.Windows.Forms.Padding(2, 3, 2, 2);
			this.panelExchange.Size = new System.Drawing.Size(72, 55);
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
			this.cboExchange.Size = new System.Drawing.Size(68, 21);
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
			// panelShow
			// 
			this.panelShow.Controls.Add(this.dspShowIncrement);
			this.panelShow.Controls.Add(this.numShow);
			this.panelShow.Controls.Add(this.chkShow);
			this.panelShow.Dock = System.Windows.Forms.DockStyle.Left;
			this.panelShow.Location = new System.Drawing.Point(0, 0);
			this.panelShow.MinimumSize = new System.Drawing.Size(68, 55);
			this.panelShow.Name = "panelShow";
			this.panelShow.Padding = new System.Windows.Forms.Padding(2);
			this.panelShow.Size = new System.Drawing.Size(68, 55);
			this.panelShow.TabIndex = 0;
			// 
			// dspShowIncrement
			// 
			this.dspShowIncrement.AutoColor = false;
			this.dspShowIncrement.AutoSize = true;
			this.dspShowIncrement.BackColor = System.Drawing.SystemColors.ControlText;
			this.dspShowIncrement.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.dspShowIncrement.DefaultColor = System.Drawing.Color.White;
			this.dspShowIncrement.DefaultText = "100";
			this.dspShowIncrement.DisplayFactor = 1;
			this.dspShowIncrement.Dock = System.Windows.Forms.DockStyle.Right;
			this.dspShowIncrement.DownColor = System.Drawing.Color.Red;
			this.dspShowIncrement.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspShowIncrement.ForeColor = System.Drawing.Color.Gainsboro;
			this.dspShowIncrement.Is64Th = false;
			this.dspShowIncrement.IsCurrency = false;
			this.dspShowIncrement.IsFraction = false;
			this.dspShowIncrement.IsPercent = false;
			this.dspShowIncrement.Location = new System.Drawing.Point(41, 40);
			this.dspShowIncrement.MaxDecimal = 0;
			this.dspShowIncrement.Name = "dspShowIncrement";
			this.dspShowIncrement.Size = new System.Drawing.Size(25, 14);
			this.dspShowIncrement.TabIndex = 0;
			this.dspShowIncrement.Text = "100";
			this.dspShowIncrement.TickSize = 0.01;
			this.dspShowIncrement.UpColor = System.Drawing.Color.LimeGreen;
			this.dspShowIncrement.Value = "";
			this.dspShowIncrement.MouseClick += new System.Windows.Forms.MouseEventHandler(this.dspShowIncrement_MouseClick);
			// 
			// numShow
			// 
			this.numShow.BackColor = System.Drawing.Color.White;
			this.numShow.Dock = System.Windows.Forms.DockStyle.Top;
			this.numShow.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.numShow.ForeColor = System.Drawing.Color.Black;
			this.numShow.Increment = new decimal(new int[] {
            10,
            0,
            0,
            0});
			this.numShow.Location = new System.Drawing.Point(2, 19);
			this.numShow.Maximum = new decimal(new int[] {
            1215752191,
            23,
            0,
            0});
			this.numShow.Name = "numShow";
			this.numShow.Size = new System.Drawing.Size(64, 21);
			this.numShow.TabIndex = 0;
			this.numShow.TabStop = false;
			this.numShow.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
			this.numShow.ValueChanged += new System.EventHandler(this.Ticket_ValueChanged);
			this.numShow.KeyUp += new System.Windows.Forms.KeyEventHandler(this.Control_KeyUp);
			this.numShow.KeyDown += new System.Windows.Forms.KeyEventHandler(this.Control_KeyUp);
			// 
			// chkShow
			// 
			this.chkShow.AutoSize = true;
			this.chkShow.BackColor = System.Drawing.Color.LightSteelBlue;
			this.chkShow.Dock = System.Windows.Forms.DockStyle.Top;
			this.chkShow.Location = new System.Drawing.Point(2, 2);
			this.chkShow.Name = "chkShow";
			this.chkShow.RightToLeft = System.Windows.Forms.RightToLeft.Yes;
			this.chkShow.Size = new System.Drawing.Size(64, 17);
			this.chkShow.TabIndex = 0;
			this.chkShow.TabStop = false;
			this.chkShow.Text = "Show";
			this.chkShow.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.chkShow.UseVisualStyleBackColor = false;
			this.chkShow.CheckedChanged += new System.EventHandler(this.Ticket_ValueChanged);
			// 
			// panelOrderInfoUpper
			// 
			this.panelOrderInfoUpper.Controls.Add(this.panelDuration);
			this.panelOrderInfoUpper.Controls.Add(this.panelPegPrice);
			this.panelOrderInfoUpper.Controls.Add(this.panelStopPrice);
			this.panelOrderInfoUpper.Controls.Add(this.panelLimitPrice);
			this.panelOrderInfoUpper.Controls.Add(this.panelOrderTypes);
			this.panelOrderInfoUpper.Controls.Add(this.panelShares);
			this.panelOrderInfoUpper.Dock = System.Windows.Forms.DockStyle.Top;
			this.panelOrderInfoUpper.Location = new System.Drawing.Point(0, 0);
			this.panelOrderInfoUpper.MinimumSize = new System.Drawing.Size(308, 55);
			this.panelOrderInfoUpper.Name = "panelOrderInfoUpper";
			this.panelOrderInfoUpper.Size = new System.Drawing.Size(440, 55);
			this.panelOrderInfoUpper.TabIndex = 0;
			// 
			// panelDuration
			// 
			this.panelDuration.Controls.Add(this.cboDuration);
			this.panelDuration.Controls.Add(this.lblDuration);
			this.panelDuration.Dock = System.Windows.Forms.DockStyle.Left;
			this.panelDuration.Location = new System.Drawing.Point(387, 0);
			this.panelDuration.MinimumSize = new System.Drawing.Size(53, 55);
			this.panelDuration.Name = "panelDuration";
			this.panelDuration.Padding = new System.Windows.Forms.Padding(2);
			this.panelDuration.Size = new System.Drawing.Size(53, 55);
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
			this.cboDuration.Size = new System.Drawing.Size(50, 21);
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
			// panelPegPrice
			// 
			this.panelPegPrice.Controls.Add(this.dspPegPrice);
			this.panelPegPrice.Controls.Add(this.dspPegPriceTickSize);
			this.panelPegPrice.Controls.Add(this.lblPegPriceX);
			this.panelPegPrice.Controls.Add(this.dspPegPriceIncrement);
			this.panelPegPrice.Controls.Add(this.numPegPrice);
			this.panelPegPrice.Controls.Add(this.lblPegPrice);
			this.panelPegPrice.Dock = System.Windows.Forms.DockStyle.Left;
			this.panelPegPrice.Location = new System.Drawing.Point(308, 0);
			this.panelPegPrice.MinimumSize = new System.Drawing.Size(79, 55);
			this.panelPegPrice.Name = "panelPegPrice";
			this.panelPegPrice.Padding = new System.Windows.Forms.Padding(2);
			this.panelPegPrice.Size = new System.Drawing.Size(79, 55);
			this.panelPegPrice.TabIndex = 0;
			this.panelPegPrice.Visible = false;
			// 
			// dspPegPrice
			// 
			this.dspPegPrice.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
						| System.Windows.Forms.AnchorStyles.Left)
						| System.Windows.Forms.AnchorStyles.Right)));
			this.dspPegPrice.BackColor = System.Drawing.Color.White;
			this.dspPegPrice.Location = new System.Drawing.Point(3, 17);
			this.dspPegPrice.Name = "dspPegPrice";
			this.dspPegPrice.Size = new System.Drawing.Size(56, 20);
			this.dspPegPrice.TabIndex = 2;
			this.dspPegPrice.TabStop = false;
			this.dspPegPrice.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
			this.dspPegPrice.WordWrap = false;
			this.dspPegPrice.MouseLeave += new System.EventHandler(this.dspPegPrice_Leave);
			this.dspPegPrice.KeyDown += new System.Windows.Forms.KeyEventHandler(this.dspPegPrice_KeyDown);
			this.dspPegPrice.KeyUp += new System.Windows.Forms.KeyEventHandler(this.Control_KeyUp);
			// 
			// dspPegPriceTickSize
			// 
			this.dspPegPriceTickSize.AutoColor = false;
			this.dspPegPriceTickSize.AutoSize = true;
			this.dspPegPriceTickSize.BackColor = System.Drawing.Color.Transparent;
			this.dspPegPriceTickSize.DefaultColor = System.Drawing.Color.White;
			this.dspPegPriceTickSize.DefaultText = "0.01";
			this.dspPegPriceTickSize.DisplayFactor = 1;
			this.dspPegPriceTickSize.Dock = System.Windows.Forms.DockStyle.Right;
			this.dspPegPriceTickSize.DownColor = System.Drawing.Color.Red;
			this.dspPegPriceTickSize.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspPegPriceTickSize.ForeColor = System.Drawing.Color.Gainsboro;
			this.dspPegPriceTickSize.Is64Th = false;
			this.dspPegPriceTickSize.IsCurrency = false;
			this.dspPegPriceTickSize.IsFraction = false;
			this.dspPegPriceTickSize.IsPercent = false;
			this.dspPegPriceTickSize.Location = new System.Drawing.Point(26, 38);
			this.dspPegPriceTickSize.MaxDecimal = 7;
			this.dspPegPriceTickSize.Name = "dspPegPriceTickSize";
			this.dspPegPriceTickSize.Size = new System.Drawing.Size(27, 12);
			this.dspPegPriceTickSize.TabIndex = 0;
			this.dspPegPriceTickSize.Text = "0.01";
			this.dspPegPriceTickSize.TickSize = 0.01;
			this.dspPegPriceTickSize.UpColor = System.Drawing.Color.LimeGreen;
			this.dspPegPriceTickSize.Value = "";
			// 
			// lblPegPriceX
			// 
			this.lblPegPriceX.AutoColor = false;
			this.lblPegPriceX.AutoSize = true;
			this.lblPegPriceX.BackColor = System.Drawing.Color.Transparent;
			this.lblPegPriceX.DefaultColor = System.Drawing.Color.White;
			this.lblPegPriceX.DefaultText = "0.01";
			this.lblPegPriceX.DisplayFactor = 1;
			this.lblPegPriceX.Dock = System.Windows.Forms.DockStyle.Right;
			this.lblPegPriceX.DownColor = System.Drawing.Color.Red;
			this.lblPegPriceX.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblPegPriceX.ForeColor = System.Drawing.Color.Gainsboro;
			this.lblPegPriceX.Is64Th = false;
			this.lblPegPriceX.IsCurrency = false;
			this.lblPegPriceX.IsFraction = false;
			this.lblPegPriceX.IsPercent = false;
			this.lblPegPriceX.Location = new System.Drawing.Point(53, 38);
			this.lblPegPriceX.MaxDecimal = 7;
			this.lblPegPriceX.Name = "lblPegPriceX";
			this.lblPegPriceX.Size = new System.Drawing.Size(11, 12);
			this.lblPegPriceX.TabIndex = 0;
			this.lblPegPriceX.Text = "x";
			this.lblPegPriceX.TickSize = 0.01;
			this.lblPegPriceX.UpColor = System.Drawing.Color.LimeGreen;
			this.lblPegPriceX.Value = "";
			// 
			// dspPegPriceIncrement
			// 
			this.dspPegPriceIncrement.AutoColor = false;
			this.dspPegPriceIncrement.AutoSize = true;
			this.dspPegPriceIncrement.BackColor = System.Drawing.SystemColors.ControlText;
			this.dspPegPriceIncrement.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.dspPegPriceIncrement.DefaultColor = System.Drawing.Color.White;
			this.dspPegPriceIncrement.DefaultText = "100";
			this.dspPegPriceIncrement.DisplayFactor = 1;
			this.dspPegPriceIncrement.Dock = System.Windows.Forms.DockStyle.Right;
			this.dspPegPriceIncrement.DownColor = System.Drawing.Color.Red;
			this.dspPegPriceIncrement.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspPegPriceIncrement.ForeColor = System.Drawing.Color.Gainsboro;
			this.dspPegPriceIncrement.Is64Th = false;
			this.dspPegPriceIncrement.IsCurrency = false;
			this.dspPegPriceIncrement.IsFraction = false;
			this.dspPegPriceIncrement.IsPercent = false;
			this.dspPegPriceIncrement.Location = new System.Drawing.Point(64, 38);
			this.dspPegPriceIncrement.MaxDecimal = 0;
			this.dspPegPriceIncrement.Name = "dspPegPriceIncrement";
			this.dspPegPriceIncrement.Size = new System.Drawing.Size(13, 14);
			this.dspPegPriceIncrement.TabIndex = 0;
			this.dspPegPriceIncrement.Text = "1";
			this.dspPegPriceIncrement.TickSize = 0.01;
			this.dspPegPriceIncrement.UpColor = System.Drawing.Color.LimeGreen;
			this.dspPegPriceIncrement.Value = "";
			this.dspPegPriceIncrement.MouseClick += new System.Windows.Forms.MouseEventHandler(this.dspPegPriceIncrement_MouseClick);
			// 
			// numPegPrice
			// 
			this.numPegPrice.BackColor = System.Drawing.Color.White;
			this.numPegPrice.DecimalPlaces = 2;
			this.numPegPrice.Dock = System.Windows.Forms.DockStyle.Top;
			this.numPegPrice.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.numPegPrice.ForeColor = System.Drawing.Color.Black;
			this.numPegPrice.Increment = new decimal(new int[] {
            10,
            0,
            0,
            0});
			this.numPegPrice.Location = new System.Drawing.Point(2, 17);
			this.numPegPrice.Maximum = new decimal(new int[] {
            1215752191,
            23,
            0,
            0});
			this.numPegPrice.Minimum = new decimal(new int[] {
            1215752191,
            23,
            0,
            -2147483648});
			this.numPegPrice.Name = "numPegPrice";
			this.numPegPrice.Size = new System.Drawing.Size(75, 21);
			this.numPegPrice.TabIndex = 0;
			this.numPegPrice.TabStop = false;
			this.numPegPrice.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
			this.numPegPrice.ValueChanged += new System.EventHandler(this.Ticket_ValueChanged);
			this.numPegPrice.KeyUp += new System.Windows.Forms.KeyEventHandler(this.Control_KeyUp);
			// 
			// lblPegPrice
			// 
			this.lblPegPrice.AutoSize = true;
			this.lblPegPrice.Dock = System.Windows.Forms.DockStyle.Top;
			this.lblPegPrice.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblPegPrice.Location = new System.Drawing.Point(2, 2);
			this.lblPegPrice.Name = "lblPegPrice";
			this.lblPegPrice.Padding = new System.Windows.Forms.Padding(0, 0, 0, 2);
			this.lblPegPrice.Size = new System.Drawing.Size(53, 15);
			this.lblPegPrice.TabIndex = 0;
			this.lblPegPrice.Text = "Peg Price";
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
			this.panelStopPrice.Location = new System.Drawing.Point(229, 0);
			this.panelStopPrice.MinimumSize = new System.Drawing.Size(79, 55);
			this.panelStopPrice.Name = "panelStopPrice";
			this.panelStopPrice.Padding = new System.Windows.Forms.Padding(2);
			this.panelStopPrice.Size = new System.Drawing.Size(79, 55);
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
			this.dspStopPrice.Size = new System.Drawing.Size(56, 20);
			this.dspStopPrice.TabIndex = 1;
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
			this.dspStopPriceTickSize.IsFraction = false;
			this.dspStopPriceTickSize.IsPercent = false;
			this.dspStopPriceTickSize.Location = new System.Drawing.Point(26, 38);
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
			this.lblStopPriceX.Location = new System.Drawing.Point(53, 38);
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
			this.dspStopPriceIncrement.Location = new System.Drawing.Point(64, 38);
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
			this.numStopPrice.Size = new System.Drawing.Size(75, 21);
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
			this.panelLimitPrice.Location = new System.Drawing.Point(150, 0);
			this.panelLimitPrice.MinimumSize = new System.Drawing.Size(79, 55);
			this.panelLimitPrice.Name = "panelLimitPrice";
			this.panelLimitPrice.Padding = new System.Windows.Forms.Padding(2);
			this.panelLimitPrice.Size = new System.Drawing.Size(79, 55);
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
			this.dspLimitPrice.Size = new System.Drawing.Size(58, 20);
			this.dspLimitPrice.TabIndex = 1;
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
			this.dspLimitPriceTickSize.IsFraction = false;
			this.dspLimitPriceTickSize.IsPercent = false;
			this.dspLimitPriceTickSize.Location = new System.Drawing.Point(26, 38);
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
			this.lblLimitPriceX.Location = new System.Drawing.Point(53, 38);
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
			this.dspLimitPriceIncrement.Location = new System.Drawing.Point(64, 38);
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
			this.numLimitPrice.Size = new System.Drawing.Size(75, 21);
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
			this.panelOrderTypes.Location = new System.Drawing.Point(68, 0);
			this.panelOrderTypes.MinimumSize = new System.Drawing.Size(82, 55);
			this.panelOrderTypes.Name = "panelOrderTypes";
			this.panelOrderTypes.Padding = new System.Windows.Forms.Padding(2);
			this.panelOrderTypes.Size = new System.Drawing.Size(82, 55);
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
			this.cboOrder.MinimumSize = new System.Drawing.Size(72, 0);
			this.cboOrder.Name = "cboOrder";
			this.cboOrder.Size = new System.Drawing.Size(78, 21);
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
			this.panelShares.MinimumSize = new System.Drawing.Size(68, 55);
			this.panelShares.Name = "panelShares";
			this.panelShares.Padding = new System.Windows.Forms.Padding(2);
			this.panelShares.Size = new System.Drawing.Size(68, 55);
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
			this.dspQuantityIncrement.Location = new System.Drawing.Point(41, 38);
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
			this.numQuantity.Size = new System.Drawing.Size(64, 21);
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
			// imageListTradeTick
			// 
			this.imageListTradeTick.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageListTradeTick.ImageStream")));
			this.imageListTradeTick.TransparentColor = System.Drawing.Color.Transparent;
			this.imageListTradeTick.Images.SetKeyName(0, "UpGreen.png");
			this.imageListTradeTick.Images.SetKeyName(1, "Blank.png");
			this.imageListTradeTick.Images.SetKeyName(2, "DownRed.png");
			// 
			// panelLevel2
			// 
			this.panelLevel2.BackColor = System.Drawing.Color.LightSteelBlue;
			this.panelLevel2.Controls.Add(this.splitContainerLevel2);
			this.panelLevel2.Dock = System.Windows.Forms.DockStyle.Fill;
			this.panelLevel2.Location = new System.Drawing.Point(0, 320);
			this.panelLevel2.Name = "panelLevel2";
			this.panelLevel2.Padding = new System.Windows.Forms.Padding(2);
			this.panelLevel2.Size = new System.Drawing.Size(440, 92);
			this.panelLevel2.TabIndex = 0;
			// 
			// splitContainerLevel2
			// 
			this.splitContainerLevel2.Dock = System.Windows.Forms.DockStyle.Fill;
			this.splitContainerLevel2.Location = new System.Drawing.Point(2, 2);
			this.splitContainerLevel2.Name = "splitContainerLevel2";
			// 
			// splitContainerLevel2.Panel1
			// 
			this.splitContainerLevel2.Panel1.Controls.Add(this.rocLevel2ListBid);
			// 
			// splitContainerLevel2.Panel2
			// 
			this.splitContainerLevel2.Panel2.Controls.Add(this.rocLevel2ListAsk);
			this.splitContainerLevel2.Size = new System.Drawing.Size(436, 88);
			this.splitContainerLevel2.SplitterDistance = 208;
			this.splitContainerLevel2.SplitterWidth = 3;
			this.splitContainerLevel2.TabIndex = 0;
			this.splitContainerLevel2.TabStop = false;
			// 
			// rocLevel2ListBid
			// 
			this.rocLevel2ListBid.AskColor = System.Drawing.Color.Red;
			this.rocLevel2ListBid.BidColor = System.Drawing.Color.LimeGreen;
			this.rocLevel2ListBid.BlueDefault = 228;
			this.rocLevel2ListBid.ColorShiftInterval = 18;
			this.rocLevel2ListBid.ColorShiftIntervalMax = 7;
			this.rocLevel2ListBid.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
			new BindingListEx.ROCBindingList<string>().Add("");
			this.rocLevel2ListBid.DisplayFactors = ((System.Collections.Generic.Dictionary<string, double>)(resources.GetObject("rocLevel2ListBid.DisplayFactors")));
			this.rocLevel2ListBid.Dock = System.Windows.Forms.DockStyle.Fill;
			this.rocLevel2ListBid.GreenDefault = 243;
			this.rocLevel2ListBid.GridKeys = ((System.Collections.Generic.List<string>)(resources.GetObject("rocLevel2ListBid.GridKeys")));
			this.rocLevel2ListBid.HeaderBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(233)))), ((int)(((byte)(243)))), ((int)(((byte)(228)))));
			this.rocLevel2ListBid.Importing = false;
			this.rocLevel2ListBid.LastFilter = "";
			this.rocLevel2ListBid.LastSort = "";
			this.rocLevel2ListBid.Location = new System.Drawing.Point(0, 0);
			this.rocLevel2ListBid.Name = "rocLevel2ListBid";
			this.rocLevel2ListBid.PairDGV = null;
			this.rocLevel2ListBid.PriceL2 = 0;
			this.rocLevel2ListBid.PriceL2Selected = false;
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
			this.rocLevel2ListBid.Printer = dgvBasePrinter1;
			this.rocLevel2ListBid.Processing = false;
			this.rocLevel2ListBid.RedDefault = 233;
			this.rocLevel2ListBid.RefreshAggragation = true;
			this.rocLevel2ListBid.RefreshSharedRows = false;
			this.rocLevel2ListBid.ReplaceOrderNow = false;
			this.rocLevel2ListBid.RocGridTable = null;
			this.rocLevel2ListBid.SelectedForeColor = System.Drawing.Color.LightBlue;
			this.rocLevel2ListBid.ShouldScrollToLastRow = false;
			this.rocLevel2ListBid.ShowOrderID = "";
			this.rocLevel2ListBid.ShowOrderMenu = false;
			this.rocLevel2ListBid.ShowTip = "";
			this.rocLevel2ListBid.ShowToolTip = false;
			this.rocLevel2ListBid.Size = new System.Drawing.Size(208, 88);
			this.rocLevel2ListBid.SizeL2 = ((long)(0));
			this.rocLevel2ListBid.SizeL2Selected = false;
			new BindingListEx.ROCBindingList<string>().Add("");
			this.rocLevel2ListBid.TabIndex = 0;
			this.rocLevel2ListBid.TabStop = false;
			this.rocLevel2ListBid.TickSizes = ((System.Collections.Generic.Dictionary<string, double>)(resources.GetObject("rocLevel2ListBid.TickSizes")));
			this.rocLevel2ListBid.WorkingOrderID = "";
			// 
			// rocLevel2ListAsk
			// 
			this.rocLevel2ListAsk.AskColor = System.Drawing.Color.Red;
			this.rocLevel2ListAsk.BidColor = System.Drawing.Color.LimeGreen;
			this.rocLevel2ListAsk.BlueDefault = 228;
			this.rocLevel2ListAsk.ColorShiftInterval = 18;
			this.rocLevel2ListAsk.ColorShiftIntervalMax = 7;
			this.rocLevel2ListAsk.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
			new BindingListEx.ROCBindingList<string>().Add("");
			this.rocLevel2ListAsk.DisplayFactors = ((System.Collections.Generic.Dictionary<string, double>)(resources.GetObject("rocLevel2ListAsk.DisplayFactors")));
			this.rocLevel2ListAsk.Dock = System.Windows.Forms.DockStyle.Fill;
			this.rocLevel2ListAsk.GreenDefault = 243;
			this.rocLevel2ListAsk.GridKeys = ((System.Collections.Generic.List<string>)(resources.GetObject("rocLevel2ListAsk.GridKeys")));
			this.rocLevel2ListAsk.HeaderBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(233)))), ((int)(((byte)(243)))), ((int)(((byte)(228)))));
			this.rocLevel2ListAsk.Importing = false;
			this.rocLevel2ListAsk.LastFilter = "";
			this.rocLevel2ListAsk.LastSort = "";
			this.rocLevel2ListAsk.Location = new System.Drawing.Point(0, 0);
			this.rocLevel2ListAsk.Name = "rocLevel2ListAsk";
			this.rocLevel2ListAsk.PairDGV = null;
			this.rocLevel2ListAsk.PriceL2 = 0;
			this.rocLevel2ListAsk.PriceL2Selected = false;
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
			this.rocLevel2ListAsk.Printer = dgvBasePrinter2;
			this.rocLevel2ListAsk.Processing = false;
			this.rocLevel2ListAsk.RedDefault = 233;
			this.rocLevel2ListAsk.RefreshAggragation = true;
			this.rocLevel2ListAsk.RefreshSharedRows = false;
			this.rocLevel2ListAsk.ReplaceOrderNow = false;
			this.rocLevel2ListAsk.RocGridTable = null;
			this.rocLevel2ListAsk.SelectedForeColor = System.Drawing.Color.LightBlue;
			this.rocLevel2ListAsk.ShouldScrollToLastRow = false;
			this.rocLevel2ListAsk.ShowOrderID = "";
			this.rocLevel2ListAsk.ShowOrderMenu = false;
			this.rocLevel2ListAsk.ShowTip = "";
			this.rocLevel2ListAsk.ShowToolTip = false;
			this.rocLevel2ListAsk.Size = new System.Drawing.Size(225, 88);
			this.rocLevel2ListAsk.SizeL2 = ((long)(0));
			this.rocLevel2ListAsk.SizeL2Selected = false;
			new BindingListEx.ROCBindingList<string>().Add("");
			this.rocLevel2ListAsk.TabIndex = 0;
			this.rocLevel2ListAsk.TabStop = false;
			this.rocLevel2ListAsk.TickSizes = ((System.Collections.Generic.Dictionary<string, double>)(resources.GetObject("rocLevel2ListAsk.TickSizes")));
			this.rocLevel2ListAsk.WorkingOrderID = "";
			// 
			// CaptionBar
			// 
			this.CaptionBar.BackColor = System.Drawing.Color.Transparent;
			this.CaptionBar.Dock = System.Windows.Forms.DockStyle.Top;
			this.CaptionBar.Location = new System.Drawing.Point(0, 0);
			this.CaptionBar.Name = "CaptionBar";
			this.CaptionBar.ShowFit = true;
			this.CaptionBar.ShowShowAll = true;
			this.CaptionBar.Size = new System.Drawing.Size(440, 26);
			this.CaptionBar.TabIndex = 0;
			this.CaptionBar.TabStop = false;
			// 
			// panelCommandsEx
			// 
			this.panelCommandsEx.BackColor = System.Drawing.Color.LightSteelBlue;
			this.panelCommandsEx.Dock = System.Windows.Forms.DockStyle.Top;
			this.panelCommandsEx.Location = new System.Drawing.Point(0, 296);
			this.panelCommandsEx.MinimumSize = new System.Drawing.Size(261, 24);
			this.panelCommandsEx.Name = "panelCommandsEx";
			this.panelCommandsEx.Padding = new System.Windows.Forms.Padding(2);
			this.panelCommandsEx.Size = new System.Drawing.Size(440, 24);
			this.panelCommandsEx.TabIndex = 0;
			this.panelCommandsEx.Visible = false;
			// 
			// frmStockTicket
			// 
			this.Animate = false;
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.BackColor = System.Drawing.SystemColors.Control;
			this.Caption = "STOCK TICKET";
			this.ClientSize = new System.Drawing.Size(440, 412);
			this.Controls.Add(this.panelLevel2);
			this.Controls.Add(this.panelCommandsEx);
			this.Controls.Add(this.panelCommands);
			this.Controls.Add(this.panelOrderInfo);
			this.Controls.Add(this.panelTicker);
			this.Controls.Add(this.panelUserSelection);
			this.Controls.Add(this.CaptionBar);
			this.MinimumSize = new System.Drawing.Size(316, 420);
			this.Name = "frmStockTicket";
			this.ShowInTaskbar = false;
			this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
			this.panelUserSelection.ResumeLayout(false);
			this.panelUserSelection.PerformLayout();
			this.toolStripTraderInfo.ResumeLayout(false);
			this.toolStripTraderInfo.PerformLayout();
			this.panelCommands.ResumeLayout(false);
			this.panelTicker.ResumeLayout(false);
			this.panelTicker.PerformLayout();
			this.panelBottom.ResumeLayout(false);
			this.panelBottom.PerformLayout();
			this.panelMiddle.ResumeLayout(false);
			this.panelMiddle.PerformLayout();
			this.panelUpper.ResumeLayout(false);
			this.panelUpper.PerformLayout();
			this.panelSymbolTrade.ResumeLayout(false);
			this.panelSymbolTrade.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.picTradeTick)).EndInit();
			this.panelOrderInfo.ResumeLayout(false);
			this.panelOrderInfoMsg.ResumeLayout(false);
			this.panelAlgo.ResumeLayout(false);
			this.panelAlgoMVP.ResumeLayout(false);
			this.panelAlgoMVP.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.numMVP)).EndInit();
			this.panelEndTime.ResumeLayout(false);
			this.panelEndTime.PerformLayout();
			this.panelStartTime.ResumeLayout(false);
			this.panelStartTime.PerformLayout();
			this.panelAlgoExchange.ResumeLayout(false);
			this.panelAlgoExchange.PerformLayout();
			this.panelAlgoType.ResumeLayout(false);
			this.panelAlgoType.PerformLayout();
			this.panelOrderInfoLower.ResumeLayout(false);
			this.panelMinShares.ResumeLayout(false);
			this.panelMinShares.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.numMinQuantity)).EndInit();
			this.panelInstructions.ResumeLayout(false);
			this.panelInstructions.PerformLayout();
			this.panelShortLender.ResumeLayout(false);
			this.panelShortLender.PerformLayout();
			this.panelExchange.ResumeLayout(false);
			this.panelExchange.PerformLayout();
			this.panelShow.ResumeLayout(false);
			this.panelShow.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.numShow)).EndInit();
			this.panelOrderInfoUpper.ResumeLayout(false);
			this.panelDuration.ResumeLayout(false);
			this.panelDuration.PerformLayout();
			this.panelPegPrice.ResumeLayout(false);
			this.panelPegPrice.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.numPegPrice)).EndInit();
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
			this.panelLevel2.ResumeLayout(false);
			this.splitContainerLevel2.Panel1.ResumeLayout(false);
			this.splitContainerLevel2.Panel2.ResumeLayout(false);
			this.splitContainerLevel2.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.rocLevel2ListBid)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.rocLevel2ListAsk)).EndInit();
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.Panel panelUserSelection;
		private System.Windows.Forms.Panel panelCommands;
		private ButtonEx.VistaButton cmdSell;
		private ButtonEx.VistaButton cmdShort;
		private ButtonEx.VistaButton cmdBuy;
		private System.Windows.Forms.Panel panelTicker;
		private System.Windows.Forms.Panel panelOrderInfo;
		private System.Windows.Forms.Panel panelSymbolTrade;
		private LabelEx.LabelBase dspTradedPrice;
		private System.Windows.Forms.PictureBox picTradeTick;
		private System.Windows.Forms.Panel panelUpper;
		private System.Windows.Forms.Panel panelBottom;
		private LabelEx.LabelBase dspClosePrice;
		private System.Windows.Forms.Label lblClosePrice;
		private LabelEx.LabelBase dspHighPrice;
		private System.Windows.Forms.Label lblHighPrice;
		private LabelEx.LabelBase dspLowPrice;
		private System.Windows.Forms.Label lblLowPrice;
		private System.Windows.Forms.Panel panelOrderInfoUpper;
		private System.Windows.Forms.Panel panelDuration;
		private System.Windows.Forms.ComboBox cboDuration;
		private System.Windows.Forms.Label lblDuration;
		private System.Windows.Forms.Panel panelStopPrice;
		public NumericUpDownEx.NumericUpDownBase numStopPrice;
		private System.Windows.Forms.Panel panelLimitPrice;
		public NumericUpDownEx.NumericUpDownBase numLimitPrice;
		private System.Windows.Forms.Panel panelOrderTypes;
		private System.Windows.Forms.ComboBox cboOrder;
		private System.Windows.Forms.Label lblOrderType;
		private System.Windows.Forms.Panel panelShares;
		public NumericUpDownEx.NumericUpDownBase numQuantity;
		private System.Windows.Forms.Panel panelOrderInfoLower;
		private System.Windows.Forms.Panel panelInstructions;
		private System.Windows.Forms.Panel panelShortLender;
		private System.Windows.Forms.Label lblShortLender;
		private System.Windows.Forms.Panel panelExchange;
		private System.Windows.Forms.Panel panelShow;
		public NumericUpDownEx.NumericUpDownBase numShow;
		private System.Windows.Forms.CheckBox chkShow;
		private System.Windows.Forms.Panel panelOrderInfoMsg;
		private System.Windows.Forms.CheckBox chkInstructions;
		private System.Windows.Forms.Label lblStopPrice;
		private System.Windows.Forms.Label lblLimitPrice;
		private System.Windows.Forms.Label lblQuantity;
		private System.Windows.Forms.ToolStrip toolStripTraderInfo;
		private System.Windows.Forms.ToolStripLabel lblTradeFor;
		private System.Windows.Forms.ToolStripLabel lblLocalAccountAcrn;
		private System.Windows.Forms.ToolStripDropDownButton cmdLevel;
		private System.Windows.Forms.ToolStripMenuItem itemLevel1;
		private System.Windows.Forms.ToolStripMenuItem itemLevel2;
		private System.Windows.Forms.ImageList imageListTradeTick;
		private System.Windows.Forms.ToolStripDropDownButton cmdSettings;
		private System.Windows.Forms.ToolStripMenuItem itemSaveAsDefault;
		private LabelEx.LabelBase dspQuantityIncrement;
		private LabelEx.LabelBase dspShowIncrement;
		private LabelEx.LabelBase dspStopPriceIncrement;
		private LabelEx.LabelBase dspLimitPriceIncrement;
		private LabelEx.LabelBase dspStopPriceTickSize;
		private LabelEx.LabelBase lblStopPriceX;
		private LabelEx.LabelBase dspLimitPriceTickSize;
		private LabelEx.LabelBase lblLimitPriceX;
		private LabelEx.LabelBase dspAskPrice;
		private System.Windows.Forms.Label lblName;
		private LabelEx.LabelBase dspBidPrice;
		private System.Windows.Forms.Label lblBidPrice;
		private System.Windows.Forms.Panel panelMiddle;
		private LabelEx.LabelBase dspPctChange;
		private System.Windows.Forms.Label lblPctChange;
		private LabelEx.LabelBase dspNetChange;
		private System.Windows.Forms.Label lblNetChange;
		private LabelEx.LabelBase dspAskSize;
		private System.Windows.Forms.Label lblAskSize;
		private LabelEx.LabelBase dspBidSize;
		private System.Windows.Forms.Label lblBidSize;
		private System.Windows.Forms.Panel panelLevel2;
		private FormEx.VistaWindowCaptionBar CaptionBar;
		private System.Windows.Forms.SplitContainer splitContainerLevel2;
		private DataGridViewEx.ROCLevel2List rocLevel2ListBid;
		private DataGridViewEx.ROCLevel2List rocLevel2ListAsk;
		protected internal System.Windows.Forms.ComboBox cboExchange;
		protected internal System.Windows.Forms.ToolStripDropDownButton cmdTradeFor;
		protected internal System.Windows.Forms.ToolStripDropDownButton cmdAccount;
		protected internal System.Windows.Forms.Label lblExchange;
		protected internal System.Windows.Forms.ComboBox cboSymbolDetails;
		protected internal System.Windows.Forms.ComboBox cboInstructions;
		protected internal System.Windows.Forms.ComboBox cboShortLenders;
		private System.Windows.Forms.ToolStripMenuItem itemResetToSystemDefault;
		private LabelEx.LabelBase dspVolume;
		private System.Windows.Forms.Label lblVolume;
		private LabelEx.LabelBase dspTradeVolume;
		private System.Windows.Forms.Label lblTradeVolume;
		private LabelEx.LabelVista lblMarketDataError;
		private System.Windows.Forms.RichTextBox dspOrderMsg;
		private System.Windows.Forms.Panel panelEndTime;
		private System.Windows.Forms.Panel panelStartTime;
		internal System.Windows.Forms.DateTimePicker dtpStartTime;
		internal System.Windows.Forms.DateTimePicker dtpEndTime;
		private System.Windows.Forms.Panel panelAlgoType;
		protected internal System.Windows.Forms.ComboBox cboAlgoType;
		protected internal System.Windows.Forms.Label lblAlgoType;
		private System.Windows.Forms.CheckBox chkEndTime;
		private System.Windows.Forms.CheckBox chkStartTime;
		private System.Windows.Forms.Panel panelAlgo;
		private System.Windows.Forms.Panel panelAlgoExchange;
		protected internal System.Windows.Forms.ComboBox cboAlgoExchange;
		private System.Windows.Forms.CheckBox chkAlgoExchange;
		private System.Windows.Forms.Panel panelPegPrice;
		private LabelEx.LabelBase dspPegPriceTickSize;
		private LabelEx.LabelBase lblPegPriceX;
		private LabelEx.LabelBase dspPegPriceIncrement;
		public NumericUpDownEx.NumericUpDownBase numPegPrice;
		private System.Windows.Forms.Label lblPegPrice;
		protected internal System.Windows.Forms.Panel panelCommandsEx;
		private System.Windows.Forms.Panel panelAlgoMVP;
		public NumericUpDownEx.NumericUpDownBase numMVP;
		private System.Windows.Forms.CheckBox chkMVP;
		private System.Windows.Forms.TextBox dspLimitPrice;
		private System.Windows.Forms.TextBox dspStopPrice;
		private System.Windows.Forms.TextBox dspPegPrice;
		private LabelEx.LabelVista lblSecurityStatus;
		private System.Windows.Forms.Panel panelMinShares;
		private LabelEx.LabelBase dspMinQuantityIncrement;
		public NumericUpDownEx.NumericUpDownBase numMinQuantity;
		private System.Windows.Forms.CheckBox chkMinQty;
	}
}