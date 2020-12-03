namespace FontDialogEx
{
	partial class frmOrdersDisplay
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
			this.panelControl = new PanelEx.VistaPanel();
			this.cmdApplyToAll = new ButtonEx.VistaButton();
			this.cmdApply = new ButtonEx.VistaButton();
			this.cmdCancel = new ButtonEx.VistaButton();
			this.grpColumn = new System.Windows.Forms.GroupBox();
			this.itemColumnAccount = new FontDialogEx.itemColor();
			this.itemColumnTime = new FontDialogEx.itemColor();
			this.itemColumnExchange = new FontDialogEx.itemColor();
			this.itemColumnDuration = new FontDialogEx.itemColor();
			this.itemColumnType = new FontDialogEx.itemColor();
			this.itemColumnAvgPrice = new FontDialogEx.itemColor();
			this.itemColumnFilledQty = new FontDialogEx.itemColor();
			this.itemColumnPrice = new FontDialogEx.itemColor();
			this.itemColumnLeaveQty = new FontDialogEx.itemColor();
			this.itemColumnQty = new FontDialogEx.itemColor();
			this.itemColumnSymbol = new FontDialogEx.itemColor();
			this.itemColumnOrderID = new FontDialogEx.itemColor();
			this.grpRowType = new System.Windows.Forms.GroupBox();
			this.cboRowType = new System.Windows.Forms.ComboBox();
			this.grpSide = new System.Windows.Forms.GroupBox();
			this.itemSideSellShort = new FontDialogEx.itemColor();
			this.itemSideBuy = new FontDialogEx.itemColor();
			this.itemSideSell = new FontDialogEx.itemColor();
			this.grpStatus = new System.Windows.Forms.GroupBox();
			this.itemStatusOther = new FontDialogEx.itemColor();
			this.itemStatusReplacePending = new FontDialogEx.itemColor();
			this.itemStatusReplaceRejected = new FontDialogEx.itemColor();
			this.itemStatusBusted = new FontDialogEx.itemColor();
			this.itemStatusCorrected = new FontDialogEx.itemColor();
			this.itemStatusCancelRejected = new FontDialogEx.itemColor();
			this.itemStatusExpired = new FontDialogEx.itemColor();
			this.itemStatusCalculated = new FontDialogEx.itemColor();
			this.itemStatusStopped = new FontDialogEx.itemColor();
			this.itemStatusPendingCancel = new FontDialogEx.itemColor();
			this.itemStatusDoneForDay = new FontDialogEx.itemColor();
			this.itemStatusRejected = new FontDialogEx.itemColor();
			this.itemStatusCancelled = new FontDialogEx.itemColor();
			this.itemStatusReplaced = new FontDialogEx.itemColor();
			this.itemStatusFilled = new FontDialogEx.itemColor();
			this.itemStatusPartialFilled = new FontDialogEx.itemColor();
			this.itemStatusOpen = new FontDialogEx.itemColor();
			this.grpGrid = new System.Windows.Forms.GroupBox();
			this.itemColorGridlines = new FontDialogEx.itemColor();
			this.itemColorSelectedBackground = new FontDialogEx.itemColor();
			this.itemColorBackground = new FontDialogEx.itemColor();
			this.cmdDefaultColor = new ButtonEx.VistaButton();
			this.itemFontText = new FontDialogEx.itemFont();
			this.itemColorText = new FontDialogEx.itemColor();
			this.panelControl.SuspendLayout();
			this.grpColumn.SuspendLayout();
			this.grpRowType.SuspendLayout();
			this.grpSide.SuspendLayout();
			this.grpStatus.SuspendLayout();
			this.grpGrid.SuspendLayout();
			this.SuspendLayout();
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
			this.panelControl.Size = new System.Drawing.Size(434, 31);
			this.panelControl.TabIndex = 20;
			// 
			// cmdApplyToAll
			// 
			this.cmdApplyToAll.BackColor = System.Drawing.Color.Black;
			this.cmdApplyToAll.Location = new System.Drawing.Point(144, 5);
			this.cmdApplyToAll.Name = "cmdApplyToAll";
			this.cmdApplyToAll.Size = new System.Drawing.Size(161, 21);
			this.cmdApplyToAll.TabIndex = 12;
			this.cmdApplyToAll.Text = "Apply To All";
			this.cmdApplyToAll.Click += new System.EventHandler(this.cmdApplyToAll_Click);
			// 
			// cmdApply
			// 
			this.cmdApply.BackColor = System.Drawing.Color.Black;
			this.cmdApply.Location = new System.Drawing.Point(308, 5);
			this.cmdApply.Name = "cmdApply";
			this.cmdApply.Size = new System.Drawing.Size(121, 21);
			this.cmdApply.TabIndex = 13;
			this.cmdApply.Text = "Apply";
			this.cmdApply.Click += new System.EventHandler(this.cmdApply_Click);
			// 
			// cmdCancel
			// 
			this.cmdCancel.BackColor = System.Drawing.Color.Black;
			this.cmdCancel.Location = new System.Drawing.Point(5, 5);
			this.cmdCancel.Name = "cmdCancel";
			this.cmdCancel.Size = new System.Drawing.Size(133, 21);
			this.cmdCancel.TabIndex = 11;
			this.cmdCancel.Text = "Cancel";
			this.cmdCancel.Click += new System.EventHandler(this.cmdCancel_Click);
			// 
			// grpColumn
			// 
			this.grpColumn.Controls.Add(this.itemColumnAccount);
			this.grpColumn.Controls.Add(this.itemColumnTime);
			this.grpColumn.Controls.Add(this.itemColumnExchange);
			this.grpColumn.Controls.Add(this.itemColumnDuration);
			this.grpColumn.Controls.Add(this.itemColumnType);
			this.grpColumn.Controls.Add(this.itemColumnAvgPrice);
			this.grpColumn.Controls.Add(this.itemColumnFilledQty);
			this.grpColumn.Controls.Add(this.itemColumnPrice);
			this.grpColumn.Controls.Add(this.itemColumnLeaveQty);
			this.grpColumn.Controls.Add(this.itemColumnQty);
			this.grpColumn.Controls.Add(this.itemColumnSymbol);
			this.grpColumn.Controls.Add(this.itemColumnOrderID);
			this.grpColumn.Controls.Add(this.cmdDefaultColor);
			this.grpColumn.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.grpColumn.ForeColor = System.Drawing.Color.White;
			this.grpColumn.Location = new System.Drawing.Point(5, 12);
			this.grpColumn.Name = "grpColumn";
			this.grpColumn.Size = new System.Drawing.Size(133, 430);
			this.grpColumn.TabIndex = 30;
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
			this.itemColumnAccount.Location = new System.Drawing.Point(3, 304);
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
			this.itemColumnTime.Location = new System.Drawing.Point(3, 280);
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
			this.itemColumnExchange.Location = new System.Drawing.Point(3, 256);
			this.itemColumnExchange.Name = "itemColumnExchange";
			this.itemColumnExchange.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnExchange.Size = new System.Drawing.Size(127, 24);
			this.itemColumnExchange.TabIndex = 29;
			// 
			// itemColumnDuration
			// 
			this.itemColumnDuration.BackColor = System.Drawing.Color.Black;
			this.itemColumnDuration.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemColumnDuration.ForeColor = System.Drawing.Color.White;
			this.itemColumnDuration.ItemColor = System.Drawing.Color.Black;
			this.itemColumnDuration.ItemName = "Duration";
			this.itemColumnDuration.Location = new System.Drawing.Point(3, 232);
			this.itemColumnDuration.Name = "itemColumnDuration";
			this.itemColumnDuration.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnDuration.Size = new System.Drawing.Size(127, 24);
			this.itemColumnDuration.TabIndex = 28;
			// 
			// itemColumnType
			// 
			this.itemColumnType.BackColor = System.Drawing.Color.Black;
			this.itemColumnType.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemColumnType.ForeColor = System.Drawing.Color.White;
			this.itemColumnType.ItemColor = System.Drawing.Color.Black;
			this.itemColumnType.ItemName = "Order Type";
			this.itemColumnType.Location = new System.Drawing.Point(3, 208);
			this.itemColumnType.Name = "itemColumnType";
			this.itemColumnType.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnType.Size = new System.Drawing.Size(127, 24);
			this.itemColumnType.TabIndex = 27;
			// 
			// itemColumnAvgPrice
			// 
			this.itemColumnAvgPrice.BackColor = System.Drawing.Color.Black;
			this.itemColumnAvgPrice.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemColumnAvgPrice.ForeColor = System.Drawing.Color.White;
			this.itemColumnAvgPrice.ItemColor = System.Drawing.Color.Black;
			this.itemColumnAvgPrice.ItemName = "AvgPrice";
			this.itemColumnAvgPrice.Location = new System.Drawing.Point(3, 184);
			this.itemColumnAvgPrice.Name = "itemColumnAvgPrice";
			this.itemColumnAvgPrice.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnAvgPrice.Size = new System.Drawing.Size(127, 24);
			this.itemColumnAvgPrice.TabIndex = 26;
			// 
			// itemColumnFilledQty
			// 
			this.itemColumnFilledQty.BackColor = System.Drawing.Color.Black;
			this.itemColumnFilledQty.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemColumnFilledQty.ForeColor = System.Drawing.Color.White;
			this.itemColumnFilledQty.ItemColor = System.Drawing.Color.Black;
			this.itemColumnFilledQty.ItemName = "Filled Qty";
			this.itemColumnFilledQty.Location = new System.Drawing.Point(3, 160);
			this.itemColumnFilledQty.Name = "itemColumnFilledQty";
			this.itemColumnFilledQty.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnFilledQty.Size = new System.Drawing.Size(127, 24);
			this.itemColumnFilledQty.TabIndex = 25;
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
			// itemColumnLeaveQty
			// 
			this.itemColumnLeaveQty.BackColor = System.Drawing.Color.Black;
			this.itemColumnLeaveQty.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemColumnLeaveQty.ForeColor = System.Drawing.Color.White;
			this.itemColumnLeaveQty.ItemColor = System.Drawing.Color.Black;
			this.itemColumnLeaveQty.ItemName = "Leave Qty";
			this.itemColumnLeaveQty.Location = new System.Drawing.Point(3, 112);
			this.itemColumnLeaveQty.Name = "itemColumnLeaveQty";
			this.itemColumnLeaveQty.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnLeaveQty.Size = new System.Drawing.Size(127, 24);
			this.itemColumnLeaveQty.TabIndex = 23;
			// 
			// itemColumnQty
			// 
			this.itemColumnQty.BackColor = System.Drawing.Color.Black;
			this.itemColumnQty.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemColumnQty.ForeColor = System.Drawing.Color.White;
			this.itemColumnQty.ItemColor = System.Drawing.Color.Black;
			this.itemColumnQty.ItemName = "Qty";
			this.itemColumnQty.Location = new System.Drawing.Point(3, 88);
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
			// itemColumnOrderID
			// 
			this.itemColumnOrderID.BackColor = System.Drawing.Color.Black;
			this.itemColumnOrderID.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemColumnOrderID.ForeColor = System.Drawing.Color.White;
			this.itemColumnOrderID.ItemColor = System.Drawing.Color.Black;
			this.itemColumnOrderID.ItemName = "Order ID";
			this.itemColumnOrderID.Location = new System.Drawing.Point(3, 40);
			this.itemColumnOrderID.Name = "itemColumnOrderID";
			this.itemColumnOrderID.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnOrderID.Size = new System.Drawing.Size(127, 24);
			this.itemColumnOrderID.TabIndex = 20;
			// 
			// grpRowType
			// 
			this.grpRowType.Controls.Add(this.cboRowType);
			this.grpRowType.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.grpRowType.ForeColor = System.Drawing.Color.White;
			this.grpRowType.Location = new System.Drawing.Point(141, 12);
			this.grpRowType.Name = "grpRowType";
			this.grpRowType.Padding = new System.Windows.Forms.Padding(6);
			this.grpRowType.Size = new System.Drawing.Size(124, 43);
			this.grpRowType.TabIndex = 28;
			this.grpRowType.TabStop = false;
			this.grpRowType.Text = "Row Type";
			// 
			// cboRowType
			// 
			this.cboRowType.Dock = System.Windows.Forms.DockStyle.Top;
			this.cboRowType.FormattingEnabled = true;
			this.cboRowType.Items.AddRange(new object[] {
            "None",
            "Status",
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
			this.grpSide.Location = new System.Drawing.Point(141, 289);
			this.grpSide.Name = "grpSide";
			this.grpSide.Size = new System.Drawing.Size(124, 152);
			this.grpSide.TabIndex = 29;
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
			// grpStatus
			// 
			this.grpStatus.Controls.Add(this.itemStatusOther);
			this.grpStatus.Controls.Add(this.itemStatusReplacePending);
			this.grpStatus.Controls.Add(this.itemStatusReplaceRejected);
			this.grpStatus.Controls.Add(this.itemStatusBusted);
			this.grpStatus.Controls.Add(this.itemStatusCorrected);
			this.grpStatus.Controls.Add(this.itemStatusCancelRejected);
			this.grpStatus.Controls.Add(this.itemStatusExpired);
			this.grpStatus.Controls.Add(this.itemStatusCalculated);
			this.grpStatus.Controls.Add(this.itemStatusStopped);
			this.grpStatus.Controls.Add(this.itemStatusPendingCancel);
			this.grpStatus.Controls.Add(this.itemStatusDoneForDay);
			this.grpStatus.Controls.Add(this.itemStatusRejected);
			this.grpStatus.Controls.Add(this.itemStatusCancelled);
			this.grpStatus.Controls.Add(this.itemStatusReplaced);
			this.grpStatus.Controls.Add(this.itemStatusFilled);
			this.grpStatus.Controls.Add(this.itemStatusPartialFilled);
			this.grpStatus.Controls.Add(this.itemStatusOpen);
			this.grpStatus.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.grpStatus.ForeColor = System.Drawing.Color.White;
			this.grpStatus.Location = new System.Drawing.Point(268, 12);
			this.grpStatus.Name = "grpStatus";
			this.grpStatus.Size = new System.Drawing.Size(161, 430);
			this.grpStatus.TabIndex = 27;
			this.grpStatus.TabStop = false;
			this.grpStatus.Text = "Status";
			// 
			// itemStatusOther
			// 
			this.itemStatusOther.BackColor = System.Drawing.Color.Black;
			this.itemStatusOther.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemStatusOther.ForeColor = System.Drawing.Color.White;
			this.itemStatusOther.ItemColor = System.Drawing.Color.Black;
			this.itemStatusOther.ItemName = "Other";
			this.itemStatusOther.Location = new System.Drawing.Point(3, 400);
			this.itemStatusOther.Name = "itemStatusOther";
			this.itemStatusOther.Padding = new System.Windows.Forms.Padding(2);
			this.itemStatusOther.Size = new System.Drawing.Size(155, 24);
			this.itemStatusOther.TabIndex = 21;
			// 
			// itemStatusReplacePending
			// 
			this.itemStatusReplacePending.BackColor = System.Drawing.Color.Black;
			this.itemStatusReplacePending.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemStatusReplacePending.ForeColor = System.Drawing.Color.White;
			this.itemStatusReplacePending.ItemColor = System.Drawing.Color.Black;
			this.itemStatusReplacePending.ItemName = "Replace Pending";
			this.itemStatusReplacePending.Location = new System.Drawing.Point(3, 376);
			this.itemStatusReplacePending.Name = "itemStatusReplacePending";
			this.itemStatusReplacePending.Padding = new System.Windows.Forms.Padding(2);
			this.itemStatusReplacePending.Size = new System.Drawing.Size(155, 24);
			this.itemStatusReplacePending.TabIndex = 20;
			// 
			// itemStatusReplaceRejected
			// 
			this.itemStatusReplaceRejected.BackColor = System.Drawing.Color.Black;
			this.itemStatusReplaceRejected.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemStatusReplaceRejected.ForeColor = System.Drawing.Color.White;
			this.itemStatusReplaceRejected.ItemColor = System.Drawing.Color.Black;
			this.itemStatusReplaceRejected.ItemName = "Replace Rejected";
			this.itemStatusReplaceRejected.Location = new System.Drawing.Point(3, 352);
			this.itemStatusReplaceRejected.Name = "itemStatusReplaceRejected";
			this.itemStatusReplaceRejected.Padding = new System.Windows.Forms.Padding(2);
			this.itemStatusReplaceRejected.Size = new System.Drawing.Size(155, 24);
			this.itemStatusReplaceRejected.TabIndex = 19;
			// 
			// itemStatusBusted
			// 
			this.itemStatusBusted.BackColor = System.Drawing.Color.Black;
			this.itemStatusBusted.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemStatusBusted.ForeColor = System.Drawing.Color.White;
			this.itemStatusBusted.ItemColor = System.Drawing.Color.Black;
			this.itemStatusBusted.ItemName = "Busted";
			this.itemStatusBusted.Location = new System.Drawing.Point(3, 328);
			this.itemStatusBusted.Name = "itemStatusBusted";
			this.itemStatusBusted.Padding = new System.Windows.Forms.Padding(2);
			this.itemStatusBusted.Size = new System.Drawing.Size(155, 24);
			this.itemStatusBusted.TabIndex = 18;
			// 
			// itemStatusCorrected
			// 
			this.itemStatusCorrected.BackColor = System.Drawing.Color.Black;
			this.itemStatusCorrected.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemStatusCorrected.ForeColor = System.Drawing.Color.White;
			this.itemStatusCorrected.ItemColor = System.Drawing.Color.Black;
			this.itemStatusCorrected.ItemName = "Corrected";
			this.itemStatusCorrected.Location = new System.Drawing.Point(3, 304);
			this.itemStatusCorrected.Name = "itemStatusCorrected";
			this.itemStatusCorrected.Padding = new System.Windows.Forms.Padding(2);
			this.itemStatusCorrected.Size = new System.Drawing.Size(155, 24);
			this.itemStatusCorrected.TabIndex = 17;
			// 
			// itemStatusCancelRejected
			// 
			this.itemStatusCancelRejected.BackColor = System.Drawing.Color.Black;
			this.itemStatusCancelRejected.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemStatusCancelRejected.ForeColor = System.Drawing.Color.White;
			this.itemStatusCancelRejected.ItemColor = System.Drawing.Color.Black;
			this.itemStatusCancelRejected.ItemName = "Cancel Rejected";
			this.itemStatusCancelRejected.Location = new System.Drawing.Point(3, 280);
			this.itemStatusCancelRejected.Name = "itemStatusCancelRejected";
			this.itemStatusCancelRejected.Padding = new System.Windows.Forms.Padding(2);
			this.itemStatusCancelRejected.Size = new System.Drawing.Size(155, 24);
			this.itemStatusCancelRejected.TabIndex = 16;
			// 
			// itemStatusExpired
			// 
			this.itemStatusExpired.BackColor = System.Drawing.Color.Black;
			this.itemStatusExpired.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemStatusExpired.ForeColor = System.Drawing.Color.White;
			this.itemStatusExpired.ItemColor = System.Drawing.Color.Black;
			this.itemStatusExpired.ItemName = "Expired";
			this.itemStatusExpired.Location = new System.Drawing.Point(3, 256);
			this.itemStatusExpired.Name = "itemStatusExpired";
			this.itemStatusExpired.Padding = new System.Windows.Forms.Padding(2);
			this.itemStatusExpired.Size = new System.Drawing.Size(155, 24);
			this.itemStatusExpired.TabIndex = 14;
			// 
			// itemStatusCalculated
			// 
			this.itemStatusCalculated.BackColor = System.Drawing.Color.Black;
			this.itemStatusCalculated.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemStatusCalculated.ForeColor = System.Drawing.Color.White;
			this.itemStatusCalculated.ItemColor = System.Drawing.Color.Black;
			this.itemStatusCalculated.ItemName = "Calculated";
			this.itemStatusCalculated.Location = new System.Drawing.Point(3, 232);
			this.itemStatusCalculated.Name = "itemStatusCalculated";
			this.itemStatusCalculated.Padding = new System.Windows.Forms.Padding(2);
			this.itemStatusCalculated.Size = new System.Drawing.Size(155, 24);
			this.itemStatusCalculated.TabIndex = 13;
			// 
			// itemStatusStopped
			// 
			this.itemStatusStopped.BackColor = System.Drawing.Color.Black;
			this.itemStatusStopped.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemStatusStopped.ForeColor = System.Drawing.Color.White;
			this.itemStatusStopped.ItemColor = System.Drawing.Color.Black;
			this.itemStatusStopped.ItemName = "Stopped";
			this.itemStatusStopped.Location = new System.Drawing.Point(3, 208);
			this.itemStatusStopped.Name = "itemStatusStopped";
			this.itemStatusStopped.Padding = new System.Windows.Forms.Padding(2);
			this.itemStatusStopped.Size = new System.Drawing.Size(155, 24);
			this.itemStatusStopped.TabIndex = 11;
			// 
			// itemStatusPendingCancel
			// 
			this.itemStatusPendingCancel.BackColor = System.Drawing.Color.Black;
			this.itemStatusPendingCancel.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemStatusPendingCancel.ForeColor = System.Drawing.Color.White;
			this.itemStatusPendingCancel.ItemColor = System.Drawing.Color.Black;
			this.itemStatusPendingCancel.ItemName = "Pending Cancel";
			this.itemStatusPendingCancel.Location = new System.Drawing.Point(3, 184);
			this.itemStatusPendingCancel.Name = "itemStatusPendingCancel";
			this.itemStatusPendingCancel.Padding = new System.Windows.Forms.Padding(2);
			this.itemStatusPendingCancel.Size = new System.Drawing.Size(155, 24);
			this.itemStatusPendingCancel.TabIndex = 10;
			// 
			// itemStatusDoneForDay
			// 
			this.itemStatusDoneForDay.BackColor = System.Drawing.Color.Black;
			this.itemStatusDoneForDay.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemStatusDoneForDay.ForeColor = System.Drawing.Color.White;
			this.itemStatusDoneForDay.ItemColor = System.Drawing.Color.Black;
			this.itemStatusDoneForDay.ItemName = "Done For Day";
			this.itemStatusDoneForDay.Location = new System.Drawing.Point(3, 160);
			this.itemStatusDoneForDay.Name = "itemStatusDoneForDay";
			this.itemStatusDoneForDay.Padding = new System.Windows.Forms.Padding(2);
			this.itemStatusDoneForDay.Size = new System.Drawing.Size(155, 24);
			this.itemStatusDoneForDay.TabIndex = 7;
			// 
			// itemStatusRejected
			// 
			this.itemStatusRejected.BackColor = System.Drawing.Color.Black;
			this.itemStatusRejected.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemStatusRejected.ForeColor = System.Drawing.Color.White;
			this.itemStatusRejected.ItemColor = System.Drawing.Color.Black;
			this.itemStatusRejected.ItemName = "Rejected";
			this.itemStatusRejected.Location = new System.Drawing.Point(3, 136);
			this.itemStatusRejected.Name = "itemStatusRejected";
			this.itemStatusRejected.Padding = new System.Windows.Forms.Padding(2);
			this.itemStatusRejected.Size = new System.Drawing.Size(155, 24);
			this.itemStatusRejected.TabIndex = 12;
			// 
			// itemStatusCancelled
			// 
			this.itemStatusCancelled.BackColor = System.Drawing.Color.Black;
			this.itemStatusCancelled.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemStatusCancelled.ForeColor = System.Drawing.Color.White;
			this.itemStatusCancelled.ItemColor = System.Drawing.Color.Black;
			this.itemStatusCancelled.ItemName = "Cancelled";
			this.itemStatusCancelled.Location = new System.Drawing.Point(3, 112);
			this.itemStatusCancelled.Name = "itemStatusCancelled";
			this.itemStatusCancelled.Padding = new System.Windows.Forms.Padding(2);
			this.itemStatusCancelled.Size = new System.Drawing.Size(155, 24);
			this.itemStatusCancelled.TabIndex = 8;
			// 
			// itemStatusReplaced
			// 
			this.itemStatusReplaced.BackColor = System.Drawing.Color.Black;
			this.itemStatusReplaced.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemStatusReplaced.ForeColor = System.Drawing.Color.White;
			this.itemStatusReplaced.ItemColor = System.Drawing.Color.Black;
			this.itemStatusReplaced.ItemName = "Replaced";
			this.itemStatusReplaced.Location = new System.Drawing.Point(3, 88);
			this.itemStatusReplaced.Name = "itemStatusReplaced";
			this.itemStatusReplaced.Padding = new System.Windows.Forms.Padding(2);
			this.itemStatusReplaced.Size = new System.Drawing.Size(155, 24);
			this.itemStatusReplaced.TabIndex = 9;
			// 
			// itemStatusFilled
			// 
			this.itemStatusFilled.BackColor = System.Drawing.Color.Black;
			this.itemStatusFilled.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemStatusFilled.ForeColor = System.Drawing.Color.White;
			this.itemStatusFilled.ItemColor = System.Drawing.Color.Black;
			this.itemStatusFilled.ItemName = "Filled";
			this.itemStatusFilled.Location = new System.Drawing.Point(3, 64);
			this.itemStatusFilled.Name = "itemStatusFilled";
			this.itemStatusFilled.Padding = new System.Windows.Forms.Padding(2);
			this.itemStatusFilled.Size = new System.Drawing.Size(155, 24);
			this.itemStatusFilled.TabIndex = 6;
			// 
			// itemStatusPartialFilled
			// 
			this.itemStatusPartialFilled.BackColor = System.Drawing.Color.Black;
			this.itemStatusPartialFilled.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemStatusPartialFilled.ForeColor = System.Drawing.Color.White;
			this.itemStatusPartialFilled.ItemColor = System.Drawing.Color.Black;
			this.itemStatusPartialFilled.ItemName = "Partial Filled";
			this.itemStatusPartialFilled.Location = new System.Drawing.Point(3, 40);
			this.itemStatusPartialFilled.Name = "itemStatusPartialFilled";
			this.itemStatusPartialFilled.Padding = new System.Windows.Forms.Padding(2);
			this.itemStatusPartialFilled.Size = new System.Drawing.Size(155, 24);
			this.itemStatusPartialFilled.TabIndex = 5;
			// 
			// itemStatusOpen
			// 
			this.itemStatusOpen.BackColor = System.Drawing.Color.Black;
			this.itemStatusOpen.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemStatusOpen.ForeColor = System.Drawing.Color.White;
			this.itemStatusOpen.ItemColor = System.Drawing.Color.Black;
			this.itemStatusOpen.ItemName = "Open";
			this.itemStatusOpen.Location = new System.Drawing.Point(3, 16);
			this.itemStatusOpen.Name = "itemStatusOpen";
			this.itemStatusOpen.Padding = new System.Windows.Forms.Padding(2);
			this.itemStatusOpen.Size = new System.Drawing.Size(155, 24);
			this.itemStatusOpen.TabIndex = 15;
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
			this.grpGrid.Location = new System.Drawing.Point(141, 61);
			this.grpGrid.Name = "grpGrid";
			this.grpGrid.Size = new System.Drawing.Size(124, 222);
			this.grpGrid.TabIndex = 26;
			this.grpGrid.TabStop = false;
			this.grpGrid.Text = "Grid";
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
			// frmOrdersDisplay
			// 
			this.AnimationType = FormEx.VistaWindowBase.AnimationTypes.Slide;
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.BackColor = System.Drawing.SystemColors.ControlDarkDark;
			this.Caption = "Orders Display";
			this.ClientSize = new System.Drawing.Size(434, 478);
			this.Controls.Add(this.grpColumn);
			this.Controls.Add(this.grpRowType);
			this.Controls.Add(this.grpSide);
			this.Controls.Add(this.grpStatus);
			this.Controls.Add(this.grpGrid);
			this.Controls.Add(this.panelControl);
			this.ForeColor = System.Drawing.Color.White;
			this.Name = "frmOrdersDisplay";
			this.ShowInTaskbar = false;
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.panelControl.ResumeLayout(false);
			this.grpColumn.ResumeLayout(false);
			this.grpRowType.ResumeLayout(false);
			this.grpSide.ResumeLayout(false);
			this.grpStatus.ResumeLayout(false);
			this.grpGrid.ResumeLayout(false);
			this.ResumeLayout(false);

		}

		#endregion

		private PanelEx.VistaPanel panelControl;
		private ButtonEx.VistaButton cmdApplyToAll;
		private ButtonEx.VistaButton cmdApply;
		private ButtonEx.VistaButton cmdCancel;
		private System.Windows.Forms.GroupBox grpColumn;
		public itemColor itemColumnAccount;
		public itemColor itemColumnTime;
		public itemColor itemColumnExchange;
		public itemColor itemColumnDuration;
		public itemColor itemColumnType;
		public itemColor itemColumnAvgPrice;
		public itemColor itemColumnFilledQty;
		public itemColor itemColumnPrice;
		public itemColor itemColumnLeaveQty;
		public itemColor itemColumnQty;
		public itemColor itemColumnSymbol;
		public itemColor itemColumnOrderID;
		private System.Windows.Forms.GroupBox grpRowType;
		private System.Windows.Forms.ComboBox cboRowType;
		private System.Windows.Forms.GroupBox grpSide;
		public itemColor itemSideSellShort;
		public itemColor itemSideBuy;
		public itemColor itemSideSell;
		private System.Windows.Forms.GroupBox grpStatus;
		public itemColor itemStatusOther;
		public itemColor itemStatusReplacePending;
		public itemColor itemStatusReplaceRejected;
		public itemColor itemStatusBusted;
		public itemColor itemStatusCorrected;
		public itemColor itemStatusCancelRejected;
		public itemColor itemStatusExpired;
		public itemColor itemStatusCalculated;
		public itemColor itemStatusStopped;
		public itemColor itemStatusPendingCancel;
		public itemColor itemStatusDoneForDay;
		public itemColor itemStatusRejected;
		public itemColor itemStatusCancelled;
		public itemColor itemStatusReplaced;
		public itemColor itemStatusFilled;
		public itemColor itemStatusPartialFilled;
		public itemColor itemStatusOpen;
		private System.Windows.Forms.GroupBox grpGrid;
		public itemColor itemColorGridlines;
		public itemColor itemColorSelectedBackground;
		public itemColor itemColorBackground;
		private ButtonEx.VistaButton cmdDefaultColor;
		public itemFont itemFontText;
		public itemColor itemColorText;



	}
}
