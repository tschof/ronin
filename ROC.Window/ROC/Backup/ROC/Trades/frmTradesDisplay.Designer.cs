namespace ROC
{
	partial class frmTradesDisplay
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
			this.grpColumn = new System.Windows.Forms.GroupBox();
			this.itemColumnAccount = new ROC.itemBaseColor();
			this.itemColumnTime = new ROC.itemBaseColor();
			this.itemColumnExchange = new ROC.itemBaseColor();
			this.itemColumnValue = new ROC.itemBaseColor();
			this.itemColumnPrice = new ROC.itemBaseColor();
			this.itemColumnQty = new ROC.itemBaseColor();
			this.itemColumnSymbol = new ROC.itemBaseColor();
			this.itemColumnExecID = new ROC.itemBaseColor();
			this.cmdDefaultColor = new ButtonEx.VistaButton();
			this.grpRowType = new System.Windows.Forms.GroupBox();
			this.cboRowType = new System.Windows.Forms.ComboBox();
			this.grpSide = new System.Windows.Forms.GroupBox();
			this.itemSideSellShort = new ROC.itemBaseColor();
			this.itemSideBuy = new ROC.itemBaseColor();
			this.itemSideSell = new ROC.itemBaseColor();
			this.grpGrid = new System.Windows.Forms.GroupBox();
			this.itemFontText = new ROC.itemBaseFont();
			this.itemColorGridlines = new ROC.itemBaseColor();
			this.itemColorSelectedBackground = new ROC.itemBaseColor();
			this.itemColorBackground = new ROC.itemBaseColor();
			this.itemColorText = new ROC.itemBaseColor();
			this.panelControl = new PanelEx.VistaPanel();
			this.cmdApplyToAll = new ButtonEx.VistaButton();
			this.cmdApply = new ButtonEx.VistaButton();
			this.cmdCancel = new ButtonEx.VistaButton();
			this.itemColumnDisplay = new ROC.itemBaseColor();
			this.grpColumn.SuspendLayout();
			this.grpRowType.SuspendLayout();
			this.grpSide.SuspendLayout();
			this.grpGrid.SuspendLayout();
			this.panelControl.SuspendLayout();
			this.SuspendLayout();
			// 
			// grpColumn
			// 
			this.grpColumn.Controls.Add(this.itemColumnAccount);
			this.grpColumn.Controls.Add(this.itemColumnTime);
			this.grpColumn.Controls.Add(this.itemColumnExchange);
			this.grpColumn.Controls.Add(this.itemColumnValue);
			this.grpColumn.Controls.Add(this.itemColumnPrice);
			this.grpColumn.Controls.Add(this.itemColumnQty);
			this.grpColumn.Controls.Add(this.itemColumnDisplay);
			this.grpColumn.Controls.Add(this.itemColumnSymbol);
			this.grpColumn.Controls.Add(this.itemColumnExecID);
			this.grpColumn.Controls.Add(this.cmdDefaultColor);
			this.grpColumn.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.grpColumn.ForeColor = System.Drawing.Color.White;
			this.grpColumn.Location = new System.Drawing.Point(3, 12);
			this.grpColumn.Name = "grpColumn";
			this.grpColumn.Size = new System.Drawing.Size(133, 430);
			this.grpColumn.TabIndex = 34;
			this.grpColumn.TabStop = false;
			this.grpColumn.Text = "Columns";
			// 
			// itemColumnAccount
			// 
			this.itemColumnAccount.BackColor = System.Drawing.Color.Black;
			this.itemColumnAccount.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemColumnAccount.ForeColor = System.Drawing.Color.White;
			this.itemColumnAccount.ItemColor = System.Drawing.Color.Black;
			this.itemColumnAccount.ItemName = "Account";
			this.itemColumnAccount.Location = new System.Drawing.Point(3, 232);
			this.itemColumnAccount.Name = "itemColumnAccount";
			this.itemColumnAccount.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnAccount.Size = new System.Drawing.Size(127, 24);
			this.itemColumnAccount.TabIndex = 31;
			// 
			// itemColumnTime
			// 
			this.itemColumnTime.BackColor = System.Drawing.Color.Black;
			this.itemColumnTime.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemColumnTime.ForeColor = System.Drawing.Color.White;
			this.itemColumnTime.ItemColor = System.Drawing.Color.Black;
			this.itemColumnTime.ItemName = "Time";
			this.itemColumnTime.Location = new System.Drawing.Point(3, 208);
			this.itemColumnTime.Name = "itemColumnTime";
			this.itemColumnTime.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnTime.Size = new System.Drawing.Size(127, 24);
			this.itemColumnTime.TabIndex = 30;
			// 
			// itemColumnExchange
			// 
			this.itemColumnExchange.BackColor = System.Drawing.Color.Black;
			this.itemColumnExchange.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemColumnExchange.ForeColor = System.Drawing.Color.White;
			this.itemColumnExchange.ItemColor = System.Drawing.Color.Black;
			this.itemColumnExchange.ItemName = "Exchange";
			this.itemColumnExchange.Location = new System.Drawing.Point(3, 184);
			this.itemColumnExchange.Name = "itemColumnExchange";
			this.itemColumnExchange.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnExchange.Size = new System.Drawing.Size(127, 24);
			this.itemColumnExchange.TabIndex = 29;
			// 
			// itemColumnValue
			// 
			this.itemColumnValue.BackColor = System.Drawing.Color.Black;
			this.itemColumnValue.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemColumnValue.ForeColor = System.Drawing.Color.White;
			this.itemColumnValue.ItemColor = System.Drawing.Color.Black;
			this.itemColumnValue.ItemName = "Value";
			this.itemColumnValue.Location = new System.Drawing.Point(3, 160);
			this.itemColumnValue.Name = "itemColumnValue";
			this.itemColumnValue.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnValue.Size = new System.Drawing.Size(127, 24);
			this.itemColumnValue.TabIndex = 26;
			// 
			// itemColumnPrice
			// 
			this.itemColumnPrice.BackColor = System.Drawing.Color.Black;
			this.itemColumnPrice.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemColumnPrice.ForeColor = System.Drawing.Color.White;
			this.itemColumnPrice.ItemColor = System.Drawing.Color.Black;
			this.itemColumnPrice.ItemName = "Price";
			this.itemColumnPrice.Location = new System.Drawing.Point(3, 136);
			this.itemColumnPrice.Name = "itemColumnPrice";
			this.itemColumnPrice.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnPrice.Size = new System.Drawing.Size(127, 24);
			this.itemColumnPrice.TabIndex = 24;
			// 
			// itemColumnQty
			// 
			this.itemColumnQty.BackColor = System.Drawing.Color.Black;
			this.itemColumnQty.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemColumnQty.ForeColor = System.Drawing.Color.White;
			this.itemColumnQty.ItemColor = System.Drawing.Color.Black;
			this.itemColumnQty.ItemName = "Qty";
			this.itemColumnQty.Location = new System.Drawing.Point(3, 112);
			this.itemColumnQty.Name = "itemColumnQty";
			this.itemColumnQty.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnQty.Size = new System.Drawing.Size(127, 24);
			this.itemColumnQty.TabIndex = 22;
			// 
			// itemColumnSymbol
			// 
			this.itemColumnSymbol.BackColor = System.Drawing.Color.Black;
			this.itemColumnSymbol.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemColumnSymbol.ForeColor = System.Drawing.Color.White;
			this.itemColumnSymbol.ItemColor = System.Drawing.Color.Black;
			this.itemColumnSymbol.ItemName = "Symbol";
			this.itemColumnSymbol.Location = new System.Drawing.Point(3, 64);
			this.itemColumnSymbol.Name = "itemColumnSymbol";
			this.itemColumnSymbol.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnSymbol.Size = new System.Drawing.Size(127, 24);
			this.itemColumnSymbol.TabIndex = 21;
			// 
			// itemColumnExecID
			// 
			this.itemColumnExecID.BackColor = System.Drawing.Color.Black;
			this.itemColumnExecID.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemColumnExecID.ForeColor = System.Drawing.Color.White;
			this.itemColumnExecID.ItemColor = System.Drawing.Color.Black;
			this.itemColumnExecID.ItemName = "Exec ID";
			this.itemColumnExecID.Location = new System.Drawing.Point(3, 40);
			this.itemColumnExecID.Name = "itemColumnExecID";
			this.itemColumnExecID.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnExecID.Size = new System.Drawing.Size(127, 24);
			this.itemColumnExecID.TabIndex = 20;
			// 
			// cmdDefaultColor
			// 
			this.cmdDefaultColor.BackColor = System.Drawing.Color.Black;
			this.cmdDefaultColor.Dock = System.Windows.Forms.DockStyle.Top;
			this.cmdDefaultColor.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdDefaultColor.Location = new System.Drawing.Point(3, 16);
			this.cmdDefaultColor.Name = "cmdDefaultColor";
			this.cmdDefaultColor.Size = new System.Drawing.Size(127, 24);
			this.cmdDefaultColor.TabIndex = 42;
			this.cmdDefaultColor.Text = "Default Color...";
			this.cmdDefaultColor.Click += new System.EventHandler(this.cmdDefaultColor_Click);
			// 
			// grpRowType
			// 
			this.grpRowType.Controls.Add(this.cboRowType);
			this.grpRowType.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.grpRowType.ForeColor = System.Drawing.Color.White;
			this.grpRowType.Location = new System.Drawing.Point(139, 12);
			this.grpRowType.Name = "grpRowType";
			this.grpRowType.Padding = new System.Windows.Forms.Padding(6);
			this.grpRowType.Size = new System.Drawing.Size(124, 43);
			this.grpRowType.TabIndex = 32;
			this.grpRowType.TabStop = false;
			this.grpRowType.Text = "Row Type";
			// 
			// cboRowType
			// 
			this.cboRowType.Dock = System.Windows.Forms.DockStyle.Top;
			this.cboRowType.FormattingEnabled = true;
			this.cboRowType.Items.AddRange(new object[] {
            "None",
            "Side"});
			this.cboRowType.Location = new System.Drawing.Point(6, 19);
			this.cboRowType.Name = "cboRowType";
			this.cboRowType.Size = new System.Drawing.Size(112, 21);
			this.cboRowType.TabIndex = 0;
			this.cboRowType.SelectedIndexChanged += new System.EventHandler(this.cboRowType_SelectedIndexChanged);
			// 
			// grpSide
			// 
			this.grpSide.Controls.Add(this.itemSideSellShort);
			this.grpSide.Controls.Add(this.itemSideBuy);
			this.grpSide.Controls.Add(this.itemSideSell);
			this.grpSide.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.grpSide.ForeColor = System.Drawing.Color.White;
			this.grpSide.Location = new System.Drawing.Point(139, 289);
			this.grpSide.Name = "grpSide";
			this.grpSide.Size = new System.Drawing.Size(124, 152);
			this.grpSide.TabIndex = 33;
			this.grpSide.TabStop = false;
			this.grpSide.Text = "Side";
			// 
			// itemSideSellShort
			// 
			this.itemSideSellShort.BackColor = System.Drawing.Color.Black;
			this.itemSideSellShort.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemSideSellShort.ForeColor = System.Drawing.Color.White;
			this.itemSideSellShort.ItemColor = System.Drawing.Color.Black;
			this.itemSideSellShort.ItemName = "Sell Short";
			this.itemSideSellShort.Location = new System.Drawing.Point(3, 64);
			this.itemSideSellShort.Name = "itemSideSellShort";
			this.itemSideSellShort.Padding = new System.Windows.Forms.Padding(2);
			this.itemSideSellShort.Size = new System.Drawing.Size(118, 24);
			this.itemSideSellShort.TabIndex = 8;
			// 
			// itemSideBuy
			// 
			this.itemSideBuy.BackColor = System.Drawing.Color.Black;
			this.itemSideBuy.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemSideBuy.ForeColor = System.Drawing.Color.White;
			this.itemSideBuy.ItemColor = System.Drawing.Color.Black;
			this.itemSideBuy.ItemName = "Buy";
			this.itemSideBuy.Location = new System.Drawing.Point(3, 40);
			this.itemSideBuy.Name = "itemSideBuy";
			this.itemSideBuy.Padding = new System.Windows.Forms.Padding(2);
			this.itemSideBuy.Size = new System.Drawing.Size(118, 24);
			this.itemSideBuy.TabIndex = 7;
			// 
			// itemSideSell
			// 
			this.itemSideSell.BackColor = System.Drawing.Color.Black;
			this.itemSideSell.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemSideSell.ForeColor = System.Drawing.Color.White;
			this.itemSideSell.ItemColor = System.Drawing.Color.Black;
			this.itemSideSell.ItemName = "Sell";
			this.itemSideSell.Location = new System.Drawing.Point(3, 16);
			this.itemSideSell.Name = "itemSideSell";
			this.itemSideSell.Padding = new System.Windows.Forms.Padding(2);
			this.itemSideSell.Size = new System.Drawing.Size(118, 24);
			this.itemSideSell.TabIndex = 6;
			// 
			// grpGrid
			// 
			this.grpGrid.Controls.Add(this.itemFontText);
			this.grpGrid.Controls.Add(this.itemColorGridlines);
			this.grpGrid.Controls.Add(this.itemColorSelectedBackground);
			this.grpGrid.Controls.Add(this.itemColorBackground);
			this.grpGrid.Controls.Add(this.itemColorText);
			this.grpGrid.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.grpGrid.ForeColor = System.Drawing.Color.White;
			this.grpGrid.Location = new System.Drawing.Point(139, 61);
			this.grpGrid.Name = "grpGrid";
			this.grpGrid.Size = new System.Drawing.Size(124, 222);
			this.grpGrid.TabIndex = 31;
			this.grpGrid.TabStop = false;
			this.grpGrid.Text = "Grid";
			// 
			// itemFontText
			// 
			this.itemFontText.Dock = System.Windows.Forms.DockStyle.Fill;
			this.itemFontText.ItemFont = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.itemFontText.ItemName = "Font";
			this.itemFontText.Location = new System.Drawing.Point(3, 112);
			this.itemFontText.Name = "itemFontText";
			this.itemFontText.Padding = new System.Windows.Forms.Padding(3);
			this.itemFontText.Size = new System.Drawing.Size(118, 107);
			this.itemFontText.TabIndex = 45;
			// 
			// itemColorGridlines
			// 
			this.itemColorGridlines.BackColor = System.Drawing.Color.Black;
			this.itemColorGridlines.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemColorGridlines.ForeColor = System.Drawing.Color.White;
			this.itemColorGridlines.ItemColor = System.Drawing.Color.Black;
			this.itemColorGridlines.ItemName = "Gridlines";
			this.itemColorGridlines.Location = new System.Drawing.Point(3, 88);
			this.itemColorGridlines.Name = "itemColorGridlines";
			this.itemColorGridlines.Padding = new System.Windows.Forms.Padding(2);
			this.itemColorGridlines.Size = new System.Drawing.Size(118, 24);
			this.itemColorGridlines.TabIndex = 7;
			// 
			// itemColorSelectedBackground
			// 
			this.itemColorSelectedBackground.BackColor = System.Drawing.Color.Black;
			this.itemColorSelectedBackground.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemColorSelectedBackground.ForeColor = System.Drawing.Color.White;
			this.itemColorSelectedBackground.ItemColor = System.Drawing.Color.Black;
			this.itemColorSelectedBackground.ItemName = "Selected";
			this.itemColorSelectedBackground.Location = new System.Drawing.Point(3, 64);
			this.itemColorSelectedBackground.Name = "itemColorSelectedBackground";
			this.itemColorSelectedBackground.Padding = new System.Windows.Forms.Padding(2);
			this.itemColorSelectedBackground.Size = new System.Drawing.Size(118, 24);
			this.itemColorSelectedBackground.TabIndex = 6;
			// 
			// itemColorBackground
			// 
			this.itemColorBackground.BackColor = System.Drawing.Color.Black;
			this.itemColorBackground.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemColorBackground.ForeColor = System.Drawing.Color.White;
			this.itemColorBackground.ItemColor = System.Drawing.Color.Black;
			this.itemColorBackground.ItemName = "Background";
			this.itemColorBackground.Location = new System.Drawing.Point(3, 40);
			this.itemColorBackground.Name = "itemColorBackground";
			this.itemColorBackground.Padding = new System.Windows.Forms.Padding(2);
			this.itemColorBackground.Size = new System.Drawing.Size(118, 24);
			this.itemColorBackground.TabIndex = 5;
			// 
			// itemColorText
			// 
			this.itemColorText.BackColor = System.Drawing.Color.Black;
			this.itemColorText.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemColorText.ForeColor = System.Drawing.Color.White;
			this.itemColorText.ItemColor = System.Drawing.Color.Black;
			this.itemColorText.ItemName = "Text";
			this.itemColorText.Location = new System.Drawing.Point(3, 16);
			this.itemColorText.Name = "itemColorText";
			this.itemColorText.Padding = new System.Windows.Forms.Padding(2);
			this.itemColorText.Size = new System.Drawing.Size(118, 24);
			this.itemColorText.TabIndex = 46;
			// 
			// panelControl
			// 
			this.panelControl.BackColor = System.Drawing.Color.Black;
			this.panelControl.Controls.Add(this.cmdApplyToAll);
			this.panelControl.Controls.Add(this.cmdApply);
			this.panelControl.Controls.Add(this.cmdCancel);
			this.panelControl.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.panelControl.Location = new System.Drawing.Point(0, 447);
			this.panelControl.Name = "panelControl";
			this.panelControl.Padding = new System.Windows.Forms.Padding(5);
			this.panelControl.Size = new System.Drawing.Size(268, 31);
			this.panelControl.TabIndex = 35;
			// 
			// cmdApplyToAll
			// 
			this.cmdApplyToAll.BackColor = System.Drawing.Color.Black;
			this.cmdApplyToAll.Location = new System.Drawing.Point(87, 5);
			this.cmdApplyToAll.Name = "cmdApplyToAll";
			this.cmdApplyToAll.Size = new System.Drawing.Size(88, 21);
			this.cmdApplyToAll.TabIndex = 12;
			this.cmdApplyToAll.Text = "Apply To All";
			this.cmdApplyToAll.Click += new System.EventHandler(this.cmdApplyToAll_Click);
			// 
			// cmdApply
			// 
			this.cmdApply.BackColor = System.Drawing.Color.Black;
			this.cmdApply.Location = new System.Drawing.Point(181, 5);
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
			// itemColumnDisplay
			// 
			this.itemColumnDisplay.BackColor = System.Drawing.Color.Black;
			this.itemColumnDisplay.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemColumnDisplay.ForeColor = System.Drawing.Color.White;
			this.itemColumnDisplay.ItemColor = System.Drawing.Color.Black;
			this.itemColumnDisplay.ItemName = "Display";
			this.itemColumnDisplay.Location = new System.Drawing.Point(3, 88);
			this.itemColumnDisplay.Name = "itemColumnDisplay";
			this.itemColumnDisplay.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnDisplay.Size = new System.Drawing.Size(127, 24);
			this.itemColumnDisplay.TabIndex = 43;
			// 
			// frmTradesDisplay
			// 
			this.AnimationType = FormEx.VistaWindowBase.AnimationTypes.Slide;
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.BackColor = System.Drawing.SystemColors.ControlDarkDark;
			this.ClientSize = new System.Drawing.Size(268, 478);
			this.Controls.Add(this.panelControl);
			this.Controls.Add(this.grpColumn);
			this.Controls.Add(this.grpRowType);
			this.Controls.Add(this.grpSide);
			this.Controls.Add(this.grpGrid);
			this.ForeColor = System.Drawing.Color.White;
			this.Name = "frmTradesDisplay";
			this.ShowInTaskbar = false;
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.Text = "frmTradesDisplay";
			this.grpColumn.ResumeLayout(false);
			this.grpRowType.ResumeLayout(false);
			this.grpSide.ResumeLayout(false);
			this.grpGrid.ResumeLayout(false);
			this.panelControl.ResumeLayout(false);
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.GroupBox grpColumn;
		public itemBaseColor itemColumnAccount;
		public itemBaseColor itemColumnTime;
		public itemBaseColor itemColumnExchange;
		public itemBaseColor itemColumnValue;
		public itemBaseColor itemColumnPrice;
		public itemBaseColor itemColumnQty;
		public itemBaseColor itemColumnSymbol;
		public itemBaseColor itemColumnExecID;
		private ButtonEx.VistaButton cmdDefaultColor;
		private System.Windows.Forms.GroupBox grpRowType;
		private System.Windows.Forms.ComboBox cboRowType;
		private System.Windows.Forms.GroupBox grpSide;
		public itemBaseColor itemSideSellShort;
		public itemBaseColor itemSideBuy;
		public itemBaseColor itemSideSell;
		private System.Windows.Forms.GroupBox grpGrid;
		public itemBaseFont itemFontText;
		public itemBaseColor itemColorGridlines;
		public itemBaseColor itemColorSelectedBackground;
		public itemBaseColor itemColorBackground;
		public itemBaseColor itemColorText;
		private PanelEx.VistaPanel panelControl;
		private ButtonEx.VistaButton cmdApplyToAll;
		private ButtonEx.VistaButton cmdApply;
		private ButtonEx.VistaButton cmdCancel;
		public itemBaseColor itemColumnDisplay;
	}
}