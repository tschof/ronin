namespace ROC
{
	partial class frmQuickTicketDisplay
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
			this.grpColumnFore = new System.Windows.Forms.GroupBox();
			this.cmdDefaultForeColor = new ButtonEx.VistaButton();
			this.itemColumnForeBuyOpenQty = new ROC.itemBaseColor();
			this.itemColumnForeBidQty = new ROC.itemBaseColor();
			this.itemColumnForeBidSize = new ROC.itemBaseColor();
			this.itemColumnForePrice = new ROC.itemBaseColor();
			this.itemColumnForeAskSize = new ROC.itemBaseColor();
			this.itemColumnForeAskQty = new ROC.itemBaseColor();
			this.itemColumnForeSellOpenQty = new ROC.itemBaseColor();
			this.itemColumnForeVolume = new ROC.itemBaseColor();
			this.grpGrid = new System.Windows.Forms.GroupBox();
			this.itemFontText = new ROC.itemBaseFont();
			this.itemColorGridlines = new ROC.itemBaseColor();
			this.cmdDefaultGridColor = new ButtonEx.VistaButton();
			this.panelControl = new PanelEx.VistaPanel();
			this.cmdApplyToAll = new ButtonEx.VistaButton();
			this.cmdApply = new ButtonEx.VistaButton();
			this.cmdCancel = new ButtonEx.VistaButton();
			this.grpColumnBack = new System.Windows.Forms.GroupBox();
			this.cmdDefaultBackColor = new ButtonEx.VistaButton();
			this.itemColumnBackBuyOpenQty = new ROC.itemBaseColor();
			this.itemColumnBackBidQty = new ROC.itemBaseColor();
			this.itemColumnBackBidSize = new ROC.itemBaseColor();
			this.itemColumnBackPrice = new ROC.itemBaseColor();
			this.itemColumnBackAskSize = new ROC.itemBaseColor();
			this.itemColumnBackAskQty = new ROC.itemBaseColor();
			this.itemColumnBackSellOpenQty = new ROC.itemBaseColor();
			this.itemColumnBackVolume = new ROC.itemBaseColor();
			this.grpTicket = new System.Windows.Forms.GroupBox();
			this.itemColorSelectedStopLimitBackColor = new ROC.itemBaseColor();
			this.lblTicketOpacity = new System.Windows.Forms.Label();
			this.itemOpacity = new System.Windows.Forms.TrackBar();
			this.itemColorTicketBackColor = new ROC.itemBaseColor();
			this.cmdDefaultTicketColor = new ButtonEx.VistaButton();
			this.grpTicker = new System.Windows.Forms.GroupBox();
			this.chkShowPrevClosePrice = new System.Windows.Forms.CheckBox();
			this.chkShowHighPrice = new System.Windows.Forms.CheckBox();
			this.chkShowLowPrice = new System.Windows.Forms.CheckBox();
			this.chkShowTradedVolume = new System.Windows.Forms.CheckBox();
			this.chkShowTotalVolume = new System.Windows.Forms.CheckBox();
			this.chkShowPctChange = new System.Windows.Forms.CheckBox();
			this.chkShowNetChange = new System.Windows.Forms.CheckBox();
			this.chkShowAskSize = new System.Windows.Forms.CheckBox();
			this.chkShowAskPrice = new System.Windows.Forms.CheckBox();
			this.chkShowBidSize = new System.Windows.Forms.CheckBox();
			this.chkShowBidPrice = new System.Windows.Forms.CheckBox();
			this.grpOrderInfo = new System.Windows.Forms.GroupBox();
			this.chkShowCommand = new System.Windows.Forms.CheckBox();
			this.chkShowOrderInfo = new System.Windows.Forms.CheckBox();
			this.chkShowExchange = new System.Windows.Forms.CheckBox();
			this.chkShowDuration = new System.Windows.Forms.CheckBox();
			this.chkShowStopPrice = new System.Windows.Forms.CheckBox();
			this.chkShowLimitPrice = new System.Windows.Forms.CheckBox();
			this.chkShowOrderType = new System.Windows.Forms.CheckBox();
			this.chkShowQty = new System.Windows.Forms.CheckBox();
			this.grpLadder = new System.Windows.Forms.GroupBox();
			this.dspAutoCenterRange = new LabelEx.LabelBase();
			this.numAutoCenterRange = new System.Windows.Forms.NumericUpDown();
			this.chkAutoCenter = new System.Windows.Forms.CheckBox();
			this.chkShowSelectedAccountOnly = new System.Windows.Forms.CheckBox();
			this.grpColumnFore.SuspendLayout();
			this.grpGrid.SuspendLayout();
			this.panelControl.SuspendLayout();
			this.grpColumnBack.SuspendLayout();
			this.grpTicket.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.itemOpacity)).BeginInit();
			this.grpTicker.SuspendLayout();
			this.grpOrderInfo.SuspendLayout();
			this.grpLadder.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.numAutoCenterRange)).BeginInit();
			this.SuspendLayout();
			// 
			// grpColumnFore
			// 
			this.grpColumnFore.Controls.Add(this.cmdDefaultForeColor);
			this.grpColumnFore.Controls.Add(this.itemColumnForeBuyOpenQty);
			this.grpColumnFore.Controls.Add(this.itemColumnForeBidQty);
			this.grpColumnFore.Controls.Add(this.itemColumnForeBidSize);
			this.grpColumnFore.Controls.Add(this.itemColumnForePrice);
			this.grpColumnFore.Controls.Add(this.itemColumnForeAskSize);
			this.grpColumnFore.Controls.Add(this.itemColumnForeAskQty);
			this.grpColumnFore.Controls.Add(this.itemColumnForeSellOpenQty);
			this.grpColumnFore.Controls.Add(this.itemColumnForeVolume);
			this.grpColumnFore.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.grpColumnFore.ForeColor = System.Drawing.Color.White;
			this.grpColumnFore.Location = new System.Drawing.Point(5, 12);
			this.grpColumnFore.Name = "grpColumnFore";
			this.grpColumnFore.Size = new System.Drawing.Size(133, 249);
			this.grpColumnFore.TabIndex = 37;
			this.grpColumnFore.TabStop = false;
			this.grpColumnFore.Text = "Columns Foreground";
			// 
			// cmdDefaultForeColor
			// 
			this.cmdDefaultForeColor.BackColor = System.Drawing.Color.Black;
			this.cmdDefaultForeColor.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.cmdDefaultForeColor.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdDefaultForeColor.Location = new System.Drawing.Point(3, 30);
			this.cmdDefaultForeColor.Name = "cmdDefaultForeColor";
			this.cmdDefaultForeColor.Size = new System.Drawing.Size(127, 24);
			this.cmdDefaultForeColor.TabIndex = 42;
			this.cmdDefaultForeColor.Text = "Default Color...";
			this.cmdDefaultForeColor.Click += new System.EventHandler(this.cmdDefaultForeColor_Click);
			// 
			// itemColumnForeBuyOpenQty
			// 
			this.itemColumnForeBuyOpenQty.BackColor = System.Drawing.Color.Black;
			this.itemColumnForeBuyOpenQty.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.itemColumnForeBuyOpenQty.ForeColor = System.Drawing.Color.White;
			this.itemColumnForeBuyOpenQty.ItemColor = System.Drawing.Color.Black;
			this.itemColumnForeBuyOpenQty.ItemName = "BuyOpen Qty";
			this.itemColumnForeBuyOpenQty.Location = new System.Drawing.Point(3, 54);
			this.itemColumnForeBuyOpenQty.Name = "itemColumnForeBuyOpenQty";
			this.itemColumnForeBuyOpenQty.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnForeBuyOpenQty.Size = new System.Drawing.Size(127, 24);
			this.itemColumnForeBuyOpenQty.TabIndex = 23;
			// 
			// itemColumnForeBidQty
			// 
			this.itemColumnForeBidQty.BackColor = System.Drawing.Color.Black;
			this.itemColumnForeBidQty.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.itemColumnForeBidQty.Enabled = false;
			this.itemColumnForeBidQty.ForeColor = System.Drawing.Color.White;
			this.itemColumnForeBidQty.ItemColor = System.Drawing.Color.Black;
			this.itemColumnForeBidQty.ItemName = "Bid Qty";
			this.itemColumnForeBidQty.Location = new System.Drawing.Point(3, 78);
			this.itemColumnForeBidQty.Name = "itemColumnForeBidQty";
			this.itemColumnForeBidQty.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnForeBidQty.Size = new System.Drawing.Size(127, 24);
			this.itemColumnForeBidQty.TabIndex = 24;
			this.itemColumnForeBidQty.Visible = false;
			// 
			// itemColumnForeBidSize
			// 
			this.itemColumnForeBidSize.BackColor = System.Drawing.Color.Black;
			this.itemColumnForeBidSize.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.itemColumnForeBidSize.ForeColor = System.Drawing.Color.White;
			this.itemColumnForeBidSize.ItemColor = System.Drawing.Color.Black;
			this.itemColumnForeBidSize.ItemName = "Bid Size";
			this.itemColumnForeBidSize.Location = new System.Drawing.Point(3, 102);
			this.itemColumnForeBidSize.Name = "itemColumnForeBidSize";
			this.itemColumnForeBidSize.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnForeBidSize.Size = new System.Drawing.Size(127, 24);
			this.itemColumnForeBidSize.TabIndex = 25;
			// 
			// itemColumnForePrice
			// 
			this.itemColumnForePrice.BackColor = System.Drawing.Color.Black;
			this.itemColumnForePrice.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.itemColumnForePrice.Enabled = false;
			this.itemColumnForePrice.ForeColor = System.Drawing.Color.White;
			this.itemColumnForePrice.ItemColor = System.Drawing.Color.Black;
			this.itemColumnForePrice.ItemName = "Price";
			this.itemColumnForePrice.Location = new System.Drawing.Point(3, 126);
			this.itemColumnForePrice.Name = "itemColumnForePrice";
			this.itemColumnForePrice.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnForePrice.Size = new System.Drawing.Size(127, 24);
			this.itemColumnForePrice.TabIndex = 26;
			this.itemColumnForePrice.Visible = false;
			// 
			// itemColumnForeAskSize
			// 
			this.itemColumnForeAskSize.BackColor = System.Drawing.Color.Black;
			this.itemColumnForeAskSize.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.itemColumnForeAskSize.ForeColor = System.Drawing.Color.White;
			this.itemColumnForeAskSize.ItemColor = System.Drawing.Color.Black;
			this.itemColumnForeAskSize.ItemName = "Ask Size";
			this.itemColumnForeAskSize.Location = new System.Drawing.Point(3, 150);
			this.itemColumnForeAskSize.Name = "itemColumnForeAskSize";
			this.itemColumnForeAskSize.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnForeAskSize.Size = new System.Drawing.Size(127, 24);
			this.itemColumnForeAskSize.TabIndex = 27;
			// 
			// itemColumnForeAskQty
			// 
			this.itemColumnForeAskQty.BackColor = System.Drawing.Color.Black;
			this.itemColumnForeAskQty.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.itemColumnForeAskQty.Enabled = false;
			this.itemColumnForeAskQty.ForeColor = System.Drawing.Color.White;
			this.itemColumnForeAskQty.ItemColor = System.Drawing.Color.Black;
			this.itemColumnForeAskQty.ItemName = "Ask Qty";
			this.itemColumnForeAskQty.Location = new System.Drawing.Point(3, 174);
			this.itemColumnForeAskQty.Name = "itemColumnForeAskQty";
			this.itemColumnForeAskQty.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnForeAskQty.Size = new System.Drawing.Size(127, 24);
			this.itemColumnForeAskQty.TabIndex = 50;
			this.itemColumnForeAskQty.Visible = false;
			// 
			// itemColumnForeSellOpenQty
			// 
			this.itemColumnForeSellOpenQty.BackColor = System.Drawing.Color.Black;
			this.itemColumnForeSellOpenQty.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.itemColumnForeSellOpenQty.ForeColor = System.Drawing.Color.White;
			this.itemColumnForeSellOpenQty.ItemColor = System.Drawing.Color.Black;
			this.itemColumnForeSellOpenQty.ItemName = "SellOpen Qty";
			this.itemColumnForeSellOpenQty.Location = new System.Drawing.Point(3, 198);
			this.itemColumnForeSellOpenQty.Name = "itemColumnForeSellOpenQty";
			this.itemColumnForeSellOpenQty.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnForeSellOpenQty.Size = new System.Drawing.Size(127, 24);
			this.itemColumnForeSellOpenQty.TabIndex = 28;
			// 
			// itemColumnForeVolume
			// 
			this.itemColumnForeVolume.BackColor = System.Drawing.Color.Black;
			this.itemColumnForeVolume.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.itemColumnForeVolume.Enabled = false;
			this.itemColumnForeVolume.ForeColor = System.Drawing.Color.White;
			this.itemColumnForeVolume.ItemColor = System.Drawing.Color.Black;
			this.itemColumnForeVolume.ItemName = "Volume";
			this.itemColumnForeVolume.Location = new System.Drawing.Point(3, 222);
			this.itemColumnForeVolume.Name = "itemColumnForeVolume";
			this.itemColumnForeVolume.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnForeVolume.Size = new System.Drawing.Size(127, 24);
			this.itemColumnForeVolume.TabIndex = 30;
			this.itemColumnForeVolume.Visible = false;
			// 
			// grpGrid
			// 
			this.grpGrid.Controls.Add(this.itemFontText);
			this.grpGrid.Controls.Add(this.itemColorGridlines);
			this.grpGrid.Controls.Add(this.cmdDefaultGridColor);
			this.grpGrid.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.grpGrid.ForeColor = System.Drawing.Color.White;
			this.grpGrid.Location = new System.Drawing.Point(5, 267);
			this.grpGrid.Name = "grpGrid";
			this.grpGrid.Size = new System.Drawing.Size(133, 129);
			this.grpGrid.TabIndex = 38;
			this.grpGrid.TabStop = false;
			this.grpGrid.Text = "Grid";
			// 
			// itemFontText
			// 
			this.itemFontText.Dock = System.Windows.Forms.DockStyle.Fill;
			this.itemFontText.ItemFont = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.itemFontText.ItemName = "Font";
			this.itemFontText.Location = new System.Drawing.Point(3, 64);
			this.itemFontText.Name = "itemFontText";
			this.itemFontText.Padding = new System.Windows.Forms.Padding(3);
			this.itemFontText.Size = new System.Drawing.Size(127, 62);
			this.itemFontText.TabIndex = 45;
			// 
			// itemColorGridlines
			// 
			this.itemColorGridlines.BackColor = System.Drawing.Color.Black;
			this.itemColorGridlines.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemColorGridlines.ForeColor = System.Drawing.Color.White;
			this.itemColorGridlines.ItemColor = System.Drawing.Color.Black;
			this.itemColorGridlines.ItemName = "Gridlines";
			this.itemColorGridlines.Location = new System.Drawing.Point(3, 40);
			this.itemColorGridlines.Name = "itemColorGridlines";
			this.itemColorGridlines.Padding = new System.Windows.Forms.Padding(2);
			this.itemColorGridlines.Size = new System.Drawing.Size(127, 24);
			this.itemColorGridlines.TabIndex = 7;
			// 
			// cmdDefaultGridColor
			// 
			this.cmdDefaultGridColor.BackColor = System.Drawing.Color.Black;
			this.cmdDefaultGridColor.Dock = System.Windows.Forms.DockStyle.Top;
			this.cmdDefaultGridColor.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdDefaultGridColor.Location = new System.Drawing.Point(3, 16);
			this.cmdDefaultGridColor.Name = "cmdDefaultGridColor";
			this.cmdDefaultGridColor.Size = new System.Drawing.Size(127, 24);
			this.cmdDefaultGridColor.TabIndex = 46;
			this.cmdDefaultGridColor.Text = "Default Color...";
			this.cmdDefaultGridColor.Click += new System.EventHandler(this.cmdDefaultGridColor_Click);
			// 
			// panelControl
			// 
			this.panelControl.BackColor = System.Drawing.Color.Black;
			this.panelControl.Controls.Add(this.cmdApplyToAll);
			this.panelControl.Controls.Add(this.cmdApply);
			this.panelControl.Controls.Add(this.cmdCancel);
			this.panelControl.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.panelControl.Location = new System.Drawing.Point(0, 707);
			this.panelControl.Name = "panelControl";
			this.panelControl.Padding = new System.Windows.Forms.Padding(5);
			this.panelControl.Size = new System.Drawing.Size(282, 31);
			this.panelControl.TabIndex = 40;
			// 
			// cmdApplyToAll
			// 
			this.cmdApplyToAll.BackColor = System.Drawing.Color.Black;
			this.cmdApplyToAll.Location = new System.Drawing.Point(87, 5);
			this.cmdApplyToAll.Name = "cmdApplyToAll";
			this.cmdApplyToAll.Size = new System.Drawing.Size(102, 21);
			this.cmdApplyToAll.TabIndex = 12;
			this.cmdApplyToAll.Text = "Apply To All";
			this.cmdApplyToAll.Click += new System.EventHandler(this.cmdApplyToAll_Click);
			// 
			// cmdApply
			// 
			this.cmdApply.BackColor = System.Drawing.Color.Black;
			this.cmdApply.Location = new System.Drawing.Point(195, 5);
			this.cmdApply.Name = "cmdApply";
			this.cmdApply.Size = new System.Drawing.Size(82, 21);
			this.cmdApply.TabIndex = 13;
			this.cmdApply.Text = "Apply";
			this.cmdApply.Click += new System.EventHandler(this.cmdApply_Click);
			// 
			// cmdCancel
			// 
			this.cmdCancel.BackColor = System.Drawing.Color.Black;
			this.cmdCancel.Location = new System.Drawing.Point(5, 5);
			this.cmdCancel.Name = "cmdCancel";
			this.cmdCancel.Size = new System.Drawing.Size(76, 21);
			this.cmdCancel.TabIndex = 11;
			this.cmdCancel.Text = "Cancel";
			this.cmdCancel.Click += new System.EventHandler(this.cmdCancel_Click);
			// 
			// grpColumnBack
			// 
			this.grpColumnBack.Controls.Add(this.cmdDefaultBackColor);
			this.grpColumnBack.Controls.Add(this.itemColumnBackBuyOpenQty);
			this.grpColumnBack.Controls.Add(this.itemColumnBackBidQty);
			this.grpColumnBack.Controls.Add(this.itemColumnBackBidSize);
			this.grpColumnBack.Controls.Add(this.itemColumnBackPrice);
			this.grpColumnBack.Controls.Add(this.itemColumnBackAskSize);
			this.grpColumnBack.Controls.Add(this.itemColumnBackAskQty);
			this.grpColumnBack.Controls.Add(this.itemColumnBackSellOpenQty);
			this.grpColumnBack.Controls.Add(this.itemColumnBackVolume);
			this.grpColumnBack.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.grpColumnBack.ForeColor = System.Drawing.Color.White;
			this.grpColumnBack.Location = new System.Drawing.Point(144, 12);
			this.grpColumnBack.Name = "grpColumnBack";
			this.grpColumnBack.Size = new System.Drawing.Size(133, 249);
			this.grpColumnBack.TabIndex = 41;
			this.grpColumnBack.TabStop = false;
			this.grpColumnBack.Text = "Columns Background";
			// 
			// cmdDefaultBackColor
			// 
			this.cmdDefaultBackColor.BackColor = System.Drawing.Color.Black;
			this.cmdDefaultBackColor.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.cmdDefaultBackColor.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdDefaultBackColor.Location = new System.Drawing.Point(3, 30);
			this.cmdDefaultBackColor.Name = "cmdDefaultBackColor";
			this.cmdDefaultBackColor.Size = new System.Drawing.Size(127, 24);
			this.cmdDefaultBackColor.TabIndex = 42;
			this.cmdDefaultBackColor.Text = "Default Color...";
			this.cmdDefaultBackColor.Click += new System.EventHandler(this.cmdDefaultBackColor_Click);
			// 
			// itemColumnBackBuyOpenQty
			// 
			this.itemColumnBackBuyOpenQty.BackColor = System.Drawing.Color.Black;
			this.itemColumnBackBuyOpenQty.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.itemColumnBackBuyOpenQty.ForeColor = System.Drawing.Color.White;
			this.itemColumnBackBuyOpenQty.ItemColor = System.Drawing.Color.Black;
			this.itemColumnBackBuyOpenQty.ItemName = "BuyOpen Qty";
			this.itemColumnBackBuyOpenQty.Location = new System.Drawing.Point(3, 54);
			this.itemColumnBackBuyOpenQty.Name = "itemColumnBackBuyOpenQty";
			this.itemColumnBackBuyOpenQty.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnBackBuyOpenQty.Size = new System.Drawing.Size(127, 24);
			this.itemColumnBackBuyOpenQty.TabIndex = 23;
			// 
			// itemColumnBackBidQty
			// 
			this.itemColumnBackBidQty.BackColor = System.Drawing.Color.Black;
			this.itemColumnBackBidQty.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.itemColumnBackBidQty.ForeColor = System.Drawing.Color.White;
			this.itemColumnBackBidQty.ItemColor = System.Drawing.Color.Black;
			this.itemColumnBackBidQty.ItemName = "Bid Qty";
			this.itemColumnBackBidQty.Location = new System.Drawing.Point(3, 78);
			this.itemColumnBackBidQty.Name = "itemColumnBackBidQty";
			this.itemColumnBackBidQty.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnBackBidQty.Size = new System.Drawing.Size(127, 24);
			this.itemColumnBackBidQty.TabIndex = 24;
			// 
			// itemColumnBackBidSize
			// 
			this.itemColumnBackBidSize.BackColor = System.Drawing.Color.Black;
			this.itemColumnBackBidSize.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.itemColumnBackBidSize.Enabled = false;
			this.itemColumnBackBidSize.ForeColor = System.Drawing.Color.White;
			this.itemColumnBackBidSize.ItemColor = System.Drawing.Color.Black;
			this.itemColumnBackBidSize.ItemName = "Bid Size";
			this.itemColumnBackBidSize.Location = new System.Drawing.Point(3, 102);
			this.itemColumnBackBidSize.Name = "itemColumnBackBidSize";
			this.itemColumnBackBidSize.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnBackBidSize.Size = new System.Drawing.Size(127, 24);
			this.itemColumnBackBidSize.TabIndex = 25;
			this.itemColumnBackBidSize.Visible = false;
			// 
			// itemColumnBackPrice
			// 
			this.itemColumnBackPrice.BackColor = System.Drawing.Color.Black;
			this.itemColumnBackPrice.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.itemColumnBackPrice.ForeColor = System.Drawing.Color.White;
			this.itemColumnBackPrice.ItemColor = System.Drawing.Color.Black;
			this.itemColumnBackPrice.ItemName = "Price";
			this.itemColumnBackPrice.Location = new System.Drawing.Point(3, 126);
			this.itemColumnBackPrice.Name = "itemColumnBackPrice";
			this.itemColumnBackPrice.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnBackPrice.Size = new System.Drawing.Size(127, 24);
			this.itemColumnBackPrice.TabIndex = 26;
			// 
			// itemColumnBackAskSize
			// 
			this.itemColumnBackAskSize.BackColor = System.Drawing.Color.Black;
			this.itemColumnBackAskSize.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.itemColumnBackAskSize.Enabled = false;
			this.itemColumnBackAskSize.ForeColor = System.Drawing.Color.White;
			this.itemColumnBackAskSize.ItemColor = System.Drawing.Color.Black;
			this.itemColumnBackAskSize.ItemName = "Ask Size";
			this.itemColumnBackAskSize.Location = new System.Drawing.Point(3, 150);
			this.itemColumnBackAskSize.Name = "itemColumnBackAskSize";
			this.itemColumnBackAskSize.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnBackAskSize.Size = new System.Drawing.Size(127, 24);
			this.itemColumnBackAskSize.TabIndex = 27;
			this.itemColumnBackAskSize.Visible = false;
			// 
			// itemColumnBackAskQty
			// 
			this.itemColumnBackAskQty.BackColor = System.Drawing.Color.Black;
			this.itemColumnBackAskQty.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.itemColumnBackAskQty.ForeColor = System.Drawing.Color.White;
			this.itemColumnBackAskQty.ItemColor = System.Drawing.Color.Black;
			this.itemColumnBackAskQty.ItemName = "Ask Qty";
			this.itemColumnBackAskQty.Location = new System.Drawing.Point(3, 174);
			this.itemColumnBackAskQty.Name = "itemColumnBackAskQty";
			this.itemColumnBackAskQty.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnBackAskQty.Size = new System.Drawing.Size(127, 24);
			this.itemColumnBackAskQty.TabIndex = 50;
			// 
			// itemColumnBackSellOpenQty
			// 
			this.itemColumnBackSellOpenQty.BackColor = System.Drawing.Color.Black;
			this.itemColumnBackSellOpenQty.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.itemColumnBackSellOpenQty.ForeColor = System.Drawing.Color.White;
			this.itemColumnBackSellOpenQty.ItemColor = System.Drawing.Color.Black;
			this.itemColumnBackSellOpenQty.ItemName = "SellOpen Qty";
			this.itemColumnBackSellOpenQty.Location = new System.Drawing.Point(3, 198);
			this.itemColumnBackSellOpenQty.Name = "itemColumnBackSellOpenQty";
			this.itemColumnBackSellOpenQty.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnBackSellOpenQty.Size = new System.Drawing.Size(127, 24);
			this.itemColumnBackSellOpenQty.TabIndex = 28;
			// 
			// itemColumnBackVolume
			// 
			this.itemColumnBackVolume.BackColor = System.Drawing.Color.Black;
			this.itemColumnBackVolume.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.itemColumnBackVolume.Enabled = false;
			this.itemColumnBackVolume.ForeColor = System.Drawing.Color.White;
			this.itemColumnBackVolume.ItemColor = System.Drawing.Color.Black;
			this.itemColumnBackVolume.ItemName = "Volume";
			this.itemColumnBackVolume.Location = new System.Drawing.Point(3, 222);
			this.itemColumnBackVolume.Name = "itemColumnBackVolume";
			this.itemColumnBackVolume.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnBackVolume.Size = new System.Drawing.Size(127, 24);
			this.itemColumnBackVolume.TabIndex = 30;
			this.itemColumnBackVolume.Visible = false;
			// 
			// grpTicket
			// 
			this.grpTicket.Controls.Add(this.itemColorSelectedStopLimitBackColor);
			this.grpTicket.Controls.Add(this.lblTicketOpacity);
			this.grpTicket.Controls.Add(this.itemOpacity);
			this.grpTicket.Controls.Add(this.itemColorTicketBackColor);
			this.grpTicket.Controls.Add(this.cmdDefaultTicketColor);
			this.grpTicket.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.grpTicket.ForeColor = System.Drawing.Color.White;
			this.grpTicket.Location = new System.Drawing.Point(144, 267);
			this.grpTicket.Name = "grpTicket";
			this.grpTicket.Size = new System.Drawing.Size(133, 162);
			this.grpTicket.TabIndex = 42;
			this.grpTicket.TabStop = false;
			this.grpTicket.Text = "Ticket";
			// 
			// itemColorSelectedStopLimitBackColor
			// 
			this.itemColorSelectedStopLimitBackColor.BackColor = System.Drawing.Color.Black;
			this.itemColorSelectedStopLimitBackColor.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.itemColorSelectedStopLimitBackColor.ForeColor = System.Drawing.Color.White;
			this.itemColorSelectedStopLimitBackColor.ItemColor = System.Drawing.Color.Black;
			this.itemColorSelectedStopLimitBackColor.ItemName = "StopLimit";
			this.itemColorSelectedStopLimitBackColor.Location = new System.Drawing.Point(3, 135);
			this.itemColorSelectedStopLimitBackColor.Name = "itemColorSelectedStopLimitBackColor";
			this.itemColorSelectedStopLimitBackColor.Padding = new System.Windows.Forms.Padding(2);
			this.itemColorSelectedStopLimitBackColor.Size = new System.Drawing.Size(127, 24);
			this.itemColorSelectedStopLimitBackColor.TabIndex = 52;
			// 
			// lblTicketOpacity
			// 
			this.lblTicketOpacity.Dock = System.Windows.Forms.DockStyle.Top;
			this.lblTicketOpacity.Location = new System.Drawing.Point(3, 106);
			this.lblTicketOpacity.Name = "lblTicketOpacity";
			this.lblTicketOpacity.Size = new System.Drawing.Size(127, 20);
			this.lblTicketOpacity.TabIndex = 47;
			this.lblTicketOpacity.Text = "Opacity";
			this.lblTicketOpacity.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			// 
			// itemOpacity
			// 
			this.itemOpacity.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemOpacity.Location = new System.Drawing.Point(3, 64);
			this.itemOpacity.Maximum = 100;
			this.itemOpacity.Name = "itemOpacity";
			this.itemOpacity.Size = new System.Drawing.Size(127, 42);
			this.itemOpacity.TabIndex = 0;
			this.itemOpacity.TabStop = false;
			this.itemOpacity.TickFrequency = 10;
			this.itemOpacity.Value = 100;
			// 
			// itemColorTicketBackColor
			// 
			this.itemColorTicketBackColor.BackColor = System.Drawing.Color.Black;
			this.itemColorTicketBackColor.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemColorTicketBackColor.ForeColor = System.Drawing.Color.White;
			this.itemColorTicketBackColor.ItemColor = System.Drawing.Color.Black;
			this.itemColorTicketBackColor.ItemName = "Ticket";
			this.itemColorTicketBackColor.Location = new System.Drawing.Point(3, 40);
			this.itemColorTicketBackColor.Name = "itemColorTicketBackColor";
			this.itemColorTicketBackColor.Padding = new System.Windows.Forms.Padding(2);
			this.itemColorTicketBackColor.Size = new System.Drawing.Size(127, 24);
			this.itemColorTicketBackColor.TabIndex = 46;
			// 
			// cmdDefaultTicketColor
			// 
			this.cmdDefaultTicketColor.BackColor = System.Drawing.Color.Black;
			this.cmdDefaultTicketColor.Dock = System.Windows.Forms.DockStyle.Top;
			this.cmdDefaultTicketColor.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdDefaultTicketColor.Location = new System.Drawing.Point(3, 16);
			this.cmdDefaultTicketColor.Name = "cmdDefaultTicketColor";
			this.cmdDefaultTicketColor.Size = new System.Drawing.Size(127, 24);
			this.cmdDefaultTicketColor.TabIndex = 48;
			this.cmdDefaultTicketColor.Text = "Default Color...";
			this.cmdDefaultTicketColor.Click += new System.EventHandler(this.cmdDefaultTicketColor_Click);
			// 
			// grpTicker
			// 
			this.grpTicker.Controls.Add(this.chkShowPrevClosePrice);
			this.grpTicker.Controls.Add(this.chkShowHighPrice);
			this.grpTicker.Controls.Add(this.chkShowLowPrice);
			this.grpTicker.Controls.Add(this.chkShowTradedVolume);
			this.grpTicker.Controls.Add(this.chkShowTotalVolume);
			this.grpTicker.Controls.Add(this.chkShowPctChange);
			this.grpTicker.Controls.Add(this.chkShowNetChange);
			this.grpTicker.Controls.Add(this.chkShowAskSize);
			this.grpTicker.Controls.Add(this.chkShowAskPrice);
			this.grpTicker.Controls.Add(this.chkShowBidSize);
			this.grpTicker.Controls.Add(this.chkShowBidPrice);
			this.grpTicker.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.grpTicker.ForeColor = System.Drawing.Color.White;
			this.grpTicker.Location = new System.Drawing.Point(5, 402);
			this.grpTicker.Name = "grpTicker";
			this.grpTicker.Padding = new System.Windows.Forms.Padding(10, 5, 10, 3);
			this.grpTicker.Size = new System.Drawing.Size(133, 216);
			this.grpTicker.TabIndex = 43;
			this.grpTicker.TabStop = false;
			this.grpTicker.Text = "Ticker Show";
			// 
			// chkShowPrevClosePrice
			// 
			this.chkShowPrevClosePrice.AutoSize = true;
			this.chkShowPrevClosePrice.Dock = System.Windows.Forms.DockStyle.Top;
			this.chkShowPrevClosePrice.Location = new System.Drawing.Point(10, 188);
			this.chkShowPrevClosePrice.Name = "chkShowPrevClosePrice";
			this.chkShowPrevClosePrice.Size = new System.Drawing.Size(113, 17);
			this.chkShowPrevClosePrice.TabIndex = 10;
			this.chkShowPrevClosePrice.TabStop = false;
			this.chkShowPrevClosePrice.Text = "Close Price";
			this.chkShowPrevClosePrice.UseVisualStyleBackColor = true;
			// 
			// chkShowHighPrice
			// 
			this.chkShowHighPrice.AutoSize = true;
			this.chkShowHighPrice.Dock = System.Windows.Forms.DockStyle.Top;
			this.chkShowHighPrice.Location = new System.Drawing.Point(10, 171);
			this.chkShowHighPrice.Name = "chkShowHighPrice";
			this.chkShowHighPrice.Size = new System.Drawing.Size(113, 17);
			this.chkShowHighPrice.TabIndex = 9;
			this.chkShowHighPrice.TabStop = false;
			this.chkShowHighPrice.Text = "High Price";
			this.chkShowHighPrice.UseVisualStyleBackColor = true;
			// 
			// chkShowLowPrice
			// 
			this.chkShowLowPrice.AutoSize = true;
			this.chkShowLowPrice.Dock = System.Windows.Forms.DockStyle.Top;
			this.chkShowLowPrice.Location = new System.Drawing.Point(10, 154);
			this.chkShowLowPrice.Name = "chkShowLowPrice";
			this.chkShowLowPrice.Size = new System.Drawing.Size(113, 17);
			this.chkShowLowPrice.TabIndex = 8;
			this.chkShowLowPrice.TabStop = false;
			this.chkShowLowPrice.Text = "Low Price";
			this.chkShowLowPrice.UseVisualStyleBackColor = true;
			// 
			// chkShowTradedVolume
			// 
			this.chkShowTradedVolume.AutoSize = true;
			this.chkShowTradedVolume.Dock = System.Windows.Forms.DockStyle.Top;
			this.chkShowTradedVolume.Location = new System.Drawing.Point(10, 137);
			this.chkShowTradedVolume.Name = "chkShowTradedVolume";
			this.chkShowTradedVolume.Size = new System.Drawing.Size(113, 17);
			this.chkShowTradedVolume.TabIndex = 7;
			this.chkShowTradedVolume.TabStop = false;
			this.chkShowTradedVolume.Text = "Traded Volume";
			this.chkShowTradedVolume.UseVisualStyleBackColor = true;
			// 
			// chkShowTotalVolume
			// 
			this.chkShowTotalVolume.AutoSize = true;
			this.chkShowTotalVolume.Dock = System.Windows.Forms.DockStyle.Top;
			this.chkShowTotalVolume.Location = new System.Drawing.Point(10, 120);
			this.chkShowTotalVolume.Name = "chkShowTotalVolume";
			this.chkShowTotalVolume.Size = new System.Drawing.Size(113, 17);
			this.chkShowTotalVolume.TabIndex = 6;
			this.chkShowTotalVolume.TabStop = false;
			this.chkShowTotalVolume.Text = "Total Volume";
			this.chkShowTotalVolume.UseVisualStyleBackColor = true;
			// 
			// chkShowPctChange
			// 
			this.chkShowPctChange.AutoSize = true;
			this.chkShowPctChange.Dock = System.Windows.Forms.DockStyle.Top;
			this.chkShowPctChange.Location = new System.Drawing.Point(10, 103);
			this.chkShowPctChange.Name = "chkShowPctChange";
			this.chkShowPctChange.Size = new System.Drawing.Size(113, 17);
			this.chkShowPctChange.TabIndex = 5;
			this.chkShowPctChange.TabStop = false;
			this.chkShowPctChange.Text = "Pct Change";
			this.chkShowPctChange.UseVisualStyleBackColor = true;
			// 
			// chkShowNetChange
			// 
			this.chkShowNetChange.AutoSize = true;
			this.chkShowNetChange.Dock = System.Windows.Forms.DockStyle.Top;
			this.chkShowNetChange.Location = new System.Drawing.Point(10, 86);
			this.chkShowNetChange.Name = "chkShowNetChange";
			this.chkShowNetChange.Size = new System.Drawing.Size(113, 17);
			this.chkShowNetChange.TabIndex = 4;
			this.chkShowNetChange.TabStop = false;
			this.chkShowNetChange.Text = "Net Change";
			this.chkShowNetChange.UseVisualStyleBackColor = true;
			// 
			// chkShowAskSize
			// 
			this.chkShowAskSize.AutoSize = true;
			this.chkShowAskSize.Dock = System.Windows.Forms.DockStyle.Top;
			this.chkShowAskSize.Location = new System.Drawing.Point(10, 69);
			this.chkShowAskSize.Name = "chkShowAskSize";
			this.chkShowAskSize.Size = new System.Drawing.Size(113, 17);
			this.chkShowAskSize.TabIndex = 3;
			this.chkShowAskSize.TabStop = false;
			this.chkShowAskSize.Text = "Ask Size";
			this.chkShowAskSize.UseVisualStyleBackColor = true;
			// 
			// chkShowAskPrice
			// 
			this.chkShowAskPrice.AutoSize = true;
			this.chkShowAskPrice.Dock = System.Windows.Forms.DockStyle.Top;
			this.chkShowAskPrice.Location = new System.Drawing.Point(10, 52);
			this.chkShowAskPrice.Name = "chkShowAskPrice";
			this.chkShowAskPrice.Size = new System.Drawing.Size(113, 17);
			this.chkShowAskPrice.TabIndex = 2;
			this.chkShowAskPrice.TabStop = false;
			this.chkShowAskPrice.Text = "Ask Price";
			this.chkShowAskPrice.UseVisualStyleBackColor = true;
			// 
			// chkShowBidSize
			// 
			this.chkShowBidSize.AutoSize = true;
			this.chkShowBidSize.Dock = System.Windows.Forms.DockStyle.Top;
			this.chkShowBidSize.Location = new System.Drawing.Point(10, 35);
			this.chkShowBidSize.Name = "chkShowBidSize";
			this.chkShowBidSize.Size = new System.Drawing.Size(113, 17);
			this.chkShowBidSize.TabIndex = 1;
			this.chkShowBidSize.TabStop = false;
			this.chkShowBidSize.Text = "Bid Size";
			this.chkShowBidSize.UseVisualStyleBackColor = true;
			// 
			// chkShowBidPrice
			// 
			this.chkShowBidPrice.AutoSize = true;
			this.chkShowBidPrice.Dock = System.Windows.Forms.DockStyle.Top;
			this.chkShowBidPrice.Location = new System.Drawing.Point(10, 18);
			this.chkShowBidPrice.Name = "chkShowBidPrice";
			this.chkShowBidPrice.Size = new System.Drawing.Size(113, 17);
			this.chkShowBidPrice.TabIndex = 0;
			this.chkShowBidPrice.TabStop = false;
			this.chkShowBidPrice.Text = "Bid Price";
			this.chkShowBidPrice.UseVisualStyleBackColor = true;
			// 
			// grpOrderInfo
			// 
			this.grpOrderInfo.Controls.Add(this.chkShowCommand);
			this.grpOrderInfo.Controls.Add(this.chkShowOrderInfo);
			this.grpOrderInfo.Controls.Add(this.chkShowExchange);
			this.grpOrderInfo.Controls.Add(this.chkShowDuration);
			this.grpOrderInfo.Controls.Add(this.chkShowStopPrice);
			this.grpOrderInfo.Controls.Add(this.chkShowLimitPrice);
			this.grpOrderInfo.Controls.Add(this.chkShowOrderType);
			this.grpOrderInfo.Controls.Add(this.chkShowQty);
			this.grpOrderInfo.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.grpOrderInfo.ForeColor = System.Drawing.Color.White;
			this.grpOrderInfo.Location = new System.Drawing.Point(144, 435);
			this.grpOrderInfo.Name = "grpOrderInfo";
			this.grpOrderInfo.Padding = new System.Windows.Forms.Padding(10, 5, 10, 3);
			this.grpOrderInfo.Size = new System.Drawing.Size(133, 183);
			this.grpOrderInfo.TabIndex = 44;
			this.grpOrderInfo.TabStop = false;
			this.grpOrderInfo.Text = "Order Info Show";
			// 
			// chkShowCommand
			// 
			this.chkShowCommand.AutoSize = true;
			this.chkShowCommand.Dock = System.Windows.Forms.DockStyle.Top;
			this.chkShowCommand.Location = new System.Drawing.Point(10, 137);
			this.chkShowCommand.Name = "chkShowCommand";
			this.chkShowCommand.Size = new System.Drawing.Size(113, 17);
			this.chkShowCommand.TabIndex = 7;
			this.chkShowCommand.Text = "Command";
			this.chkShowCommand.UseVisualStyleBackColor = true;
			// 
			// chkShowOrderInfo
			// 
			this.chkShowOrderInfo.AutoSize = true;
			this.chkShowOrderInfo.Dock = System.Windows.Forms.DockStyle.Top;
			this.chkShowOrderInfo.Enabled = false;
			this.chkShowOrderInfo.Location = new System.Drawing.Point(10, 120);
			this.chkShowOrderInfo.Name = "chkShowOrderInfo";
			this.chkShowOrderInfo.Size = new System.Drawing.Size(113, 17);
			this.chkShowOrderInfo.TabIndex = 0;
			this.chkShowOrderInfo.TabStop = false;
			this.chkShowOrderInfo.Text = "Order Info";
			this.chkShowOrderInfo.UseVisualStyleBackColor = true;
			this.chkShowOrderInfo.Visible = false;
			// 
			// chkShowExchange
			// 
			this.chkShowExchange.AutoSize = true;
			this.chkShowExchange.Dock = System.Windows.Forms.DockStyle.Top;
			this.chkShowExchange.Location = new System.Drawing.Point(10, 103);
			this.chkShowExchange.Name = "chkShowExchange";
			this.chkShowExchange.Size = new System.Drawing.Size(113, 17);
			this.chkShowExchange.TabIndex = 5;
			this.chkShowExchange.Text = "Exchange";
			this.chkShowExchange.UseVisualStyleBackColor = true;
			// 
			// chkShowDuration
			// 
			this.chkShowDuration.AutoSize = true;
			this.chkShowDuration.Dock = System.Windows.Forms.DockStyle.Top;
			this.chkShowDuration.Location = new System.Drawing.Point(10, 86);
			this.chkShowDuration.Name = "chkShowDuration";
			this.chkShowDuration.Size = new System.Drawing.Size(113, 17);
			this.chkShowDuration.TabIndex = 4;
			this.chkShowDuration.Text = "Duration";
			this.chkShowDuration.UseVisualStyleBackColor = true;
			// 
			// chkShowStopPrice
			// 
			this.chkShowStopPrice.AutoSize = true;
			this.chkShowStopPrice.Dock = System.Windows.Forms.DockStyle.Top;
			this.chkShowStopPrice.Location = new System.Drawing.Point(10, 69);
			this.chkShowStopPrice.Name = "chkShowStopPrice";
			this.chkShowStopPrice.Size = new System.Drawing.Size(113, 17);
			this.chkShowStopPrice.TabIndex = 3;
			this.chkShowStopPrice.Text = "Stop Price";
			this.chkShowStopPrice.UseVisualStyleBackColor = true;
			// 
			// chkShowLimitPrice
			// 
			this.chkShowLimitPrice.AutoSize = true;
			this.chkShowLimitPrice.Dock = System.Windows.Forms.DockStyle.Top;
			this.chkShowLimitPrice.Location = new System.Drawing.Point(10, 52);
			this.chkShowLimitPrice.Name = "chkShowLimitPrice";
			this.chkShowLimitPrice.Size = new System.Drawing.Size(113, 17);
			this.chkShowLimitPrice.TabIndex = 2;
			this.chkShowLimitPrice.Text = "Limit Price";
			this.chkShowLimitPrice.UseVisualStyleBackColor = true;
			// 
			// chkShowOrderType
			// 
			this.chkShowOrderType.AutoSize = true;
			this.chkShowOrderType.Dock = System.Windows.Forms.DockStyle.Top;
			this.chkShowOrderType.Location = new System.Drawing.Point(10, 35);
			this.chkShowOrderType.Name = "chkShowOrderType";
			this.chkShowOrderType.Size = new System.Drawing.Size(113, 17);
			this.chkShowOrderType.TabIndex = 1;
			this.chkShowOrderType.Text = "Order Type";
			this.chkShowOrderType.UseVisualStyleBackColor = true;
			// 
			// chkShowQty
			// 
			this.chkShowQty.AutoSize = true;
			this.chkShowQty.Dock = System.Windows.Forms.DockStyle.Top;
			this.chkShowQty.Location = new System.Drawing.Point(10, 18);
			this.chkShowQty.Name = "chkShowQty";
			this.chkShowQty.Size = new System.Drawing.Size(113, 17);
			this.chkShowQty.TabIndex = 0;
			this.chkShowQty.Text = "Qty";
			this.chkShowQty.UseVisualStyleBackColor = true;
			// 
			// grpLadder
			// 
			this.grpLadder.Controls.Add(this.dspAutoCenterRange);
			this.grpLadder.Controls.Add(this.numAutoCenterRange);
			this.grpLadder.Controls.Add(this.chkAutoCenter);
			this.grpLadder.Controls.Add(this.chkShowSelectedAccountOnly);
			this.grpLadder.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.grpLadder.ForeColor = System.Drawing.Color.White;
			this.grpLadder.Location = new System.Drawing.Point(5, 624);
			this.grpLadder.Name = "grpLadder";
			this.grpLadder.Padding = new System.Windows.Forms.Padding(10, 5, 10, 3);
			this.grpLadder.Size = new System.Drawing.Size(272, 77);
			this.grpLadder.TabIndex = 45;
			this.grpLadder.TabStop = false;
			this.grpLadder.Text = "Ladder";
			// 
			// dspAutoCenterRange
			// 
			this.dspAutoCenterRange.AutoColor = false;
			this.dspAutoCenterRange.DefaultColor = System.Drawing.Color.White;
			this.dspAutoCenterRange.DefaultText = "";
			this.dspAutoCenterRange.DisplayFactor = 1;
			this.dspAutoCenterRange.DownColor = System.Drawing.Color.Red;
			this.dspAutoCenterRange.Is64Th = false;
			this.dspAutoCenterRange.IsCurrency = false;
			this.dspAutoCenterRange.IsFraction = false;
			this.dspAutoCenterRange.IsPercent = false;
			this.dspAutoCenterRange.Location = new System.Drawing.Point(64, 51);
			this.dspAutoCenterRange.MaxDecimal = 7;
			this.dspAutoCenterRange.Name = "dspAutoCenterRange";
			this.dspAutoCenterRange.Size = new System.Drawing.Size(167, 20);
			this.dspAutoCenterRange.TabIndex = 0;
			this.dspAutoCenterRange.Text = "Auto Center Range (Min. 3)";
			this.dspAutoCenterRange.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			this.dspAutoCenterRange.TickSize = 0.01;
			this.dspAutoCenterRange.UpColor = System.Drawing.Color.LimeGreen;
			this.dspAutoCenterRange.Value = "";
			// 
			// numAutoCenterRange
			// 
			this.numAutoCenterRange.Location = new System.Drawing.Point(10, 51);
			this.numAutoCenterRange.Minimum = new decimal(new int[] {
            3,
            0,
            0,
            0});
			this.numAutoCenterRange.Name = "numAutoCenterRange";
			this.numAutoCenterRange.Size = new System.Drawing.Size(51, 20);
			this.numAutoCenterRange.TabIndex = 0;
			this.numAutoCenterRange.TabStop = false;
			this.numAutoCenterRange.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
			this.numAutoCenterRange.Value = new decimal(new int[] {
            10,
            0,
            0,
            0});
			// 
			// chkAutoCenter
			// 
			this.chkAutoCenter.AutoSize = true;
			this.chkAutoCenter.Dock = System.Windows.Forms.DockStyle.Top;
			this.chkAutoCenter.Location = new System.Drawing.Point(10, 35);
			this.chkAutoCenter.Name = "chkAutoCenter";
			this.chkAutoCenter.Size = new System.Drawing.Size(252, 17);
			this.chkAutoCenter.TabIndex = 9;
			this.chkAutoCenter.Text = "Auto Center ( Apply To All = Default )";
			this.chkAutoCenter.UseVisualStyleBackColor = true;
			// 
			// chkShowSelectedAccountOnly
			// 
			this.chkShowSelectedAccountOnly.AutoSize = true;
			this.chkShowSelectedAccountOnly.Dock = System.Windows.Forms.DockStyle.Top;
			this.chkShowSelectedAccountOnly.Location = new System.Drawing.Point(10, 18);
			this.chkShowSelectedAccountOnly.Name = "chkShowSelectedAccountOnly";
			this.chkShowSelectedAccountOnly.Size = new System.Drawing.Size(252, 17);
			this.chkShowSelectedAccountOnly.TabIndex = 8;
			this.chkShowSelectedAccountOnly.Text = "Show Selected Account Only";
			this.chkShowSelectedAccountOnly.UseVisualStyleBackColor = true;
			// 
			// frmQuickTicketDisplay
			// 
			this.AnimationType = FormEx.VistaWindowBase.AnimationTypes.Slide;
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.BackColor = System.Drawing.SystemColors.ControlDarkDark;
			this.ClientSize = new System.Drawing.Size(282, 738);
			this.Controls.Add(this.grpLadder);
			this.Controls.Add(this.grpOrderInfo);
			this.Controls.Add(this.grpTicker);
			this.Controls.Add(this.grpTicket);
			this.Controls.Add(this.grpColumnBack);
			this.Controls.Add(this.panelControl);
			this.Controls.Add(this.grpGrid);
			this.Controls.Add(this.grpColumnFore);
			this.Name = "frmQuickTicketDisplay";
			this.ShowInTaskbar = false;
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.Text = "frmQuickTicketDisplay";
			this.grpColumnFore.ResumeLayout(false);
			this.grpGrid.ResumeLayout(false);
			this.panelControl.ResumeLayout(false);
			this.grpColumnBack.ResumeLayout(false);
			this.grpTicket.ResumeLayout(false);
			this.grpTicket.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.itemOpacity)).EndInit();
			this.grpTicker.ResumeLayout(false);
			this.grpTicker.PerformLayout();
			this.grpOrderInfo.ResumeLayout(false);
			this.grpOrderInfo.PerformLayout();
			this.grpLadder.ResumeLayout(false);
			this.grpLadder.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.numAutoCenterRange)).EndInit();
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.GroupBox grpColumnFore;
		public itemBaseColor itemColumnForeVolume;
		public itemBaseColor itemColumnForeSellOpenQty;
		public itemBaseColor itemColumnForeAskQty;
		public itemBaseColor itemColumnForeAskSize;
		public itemBaseColor itemColumnForePrice;
		public itemBaseColor itemColumnForeBidSize;
		public itemBaseColor itemColumnForeBidQty;
		public itemBaseColor itemColumnForeBuyOpenQty;
		private ButtonEx.VistaButton cmdDefaultForeColor;
		private System.Windows.Forms.GroupBox grpGrid;
		public itemBaseFont itemFontText;
		public itemBaseColor itemColorGridlines;
		private PanelEx.VistaPanel panelControl;
		private ButtonEx.VistaButton cmdApplyToAll;
		private ButtonEx.VistaButton cmdApply;
		private ButtonEx.VistaButton cmdCancel;
		private System.Windows.Forms.GroupBox grpColumnBack;
		private ButtonEx.VistaButton cmdDefaultBackColor;
		public itemBaseColor itemColumnBackBuyOpenQty;
		public itemBaseColor itemColumnBackBidQty;
		public itemBaseColor itemColumnBackBidSize;
		public itemBaseColor itemColumnBackPrice;
		public itemBaseColor itemColumnBackAskSize;
		public itemBaseColor itemColumnBackAskQty;
		public itemBaseColor itemColumnBackSellOpenQty;
		public itemBaseColor itemColumnBackVolume;
		private System.Windows.Forms.GroupBox grpTicket;
		public itemBaseColor itemColorTicketBackColor;
		private System.Windows.Forms.TrackBar itemOpacity;
		private System.Windows.Forms.Label lblTicketOpacity;
		private System.Windows.Forms.GroupBox grpTicker;
		private System.Windows.Forms.CheckBox chkShowPrevClosePrice;
		private System.Windows.Forms.CheckBox chkShowHighPrice;
		private System.Windows.Forms.CheckBox chkShowLowPrice;
		private System.Windows.Forms.CheckBox chkShowTradedVolume;
		private System.Windows.Forms.CheckBox chkShowTotalVolume;
		private System.Windows.Forms.CheckBox chkShowPctChange;
		private System.Windows.Forms.CheckBox chkShowNetChange;
		private System.Windows.Forms.CheckBox chkShowAskSize;
		private System.Windows.Forms.CheckBox chkShowAskPrice;
		private System.Windows.Forms.CheckBox chkShowBidSize;
		private System.Windows.Forms.CheckBox chkShowBidPrice;
		private System.Windows.Forms.GroupBox grpOrderInfo;
		private System.Windows.Forms.CheckBox chkShowCommand;
		private System.Windows.Forms.CheckBox chkShowOrderInfo;
		private System.Windows.Forms.CheckBox chkShowExchange;
		private System.Windows.Forms.CheckBox chkShowDuration;
		private System.Windows.Forms.CheckBox chkShowStopPrice;
		private System.Windows.Forms.CheckBox chkShowLimitPrice;
		private System.Windows.Forms.CheckBox chkShowOrderType;
		private System.Windows.Forms.CheckBox chkShowQty;
		private ButtonEx.VistaButton cmdDefaultGridColor;
		private ButtonEx.VistaButton cmdDefaultTicketColor;
		private System.Windows.Forms.GroupBox grpLadder;
		private System.Windows.Forms.CheckBox chkAutoCenter;
		private System.Windows.Forms.CheckBox chkShowSelectedAccountOnly;
		public itemBaseColor itemColorSelectedStopLimitBackColor;
		private LabelEx.LabelBase dspAutoCenterRange;
		private System.Windows.Forms.NumericUpDown numAutoCenterRange;
	}
}