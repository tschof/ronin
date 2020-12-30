namespace ROC
{
	partial class frmWatchListDisplay
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
			this.grpAnimation = new System.Windows.Forms.GroupBox();
			this.cboNotificationType = new System.Windows.Forms.ComboBox();
			this.itemColorDown = new ROC.itemBaseColor();
			this.itemColorUp = new ROC.itemBaseColor();
			this.panelControl = new PanelEx.VistaPanel();
			this.cmdApplyToAll = new ButtonEx.VistaButton();
			this.cmdApply = new ButtonEx.VistaButton();
			this.cmdCancel = new ButtonEx.VistaButton();
			this.grpColumn = new System.Windows.Forms.GroupBox();
			this.itemColumnName = new ROC.itemBaseColor();
			this.itemColumnVwap = new ROC.itemBaseColor();
			this.itemColumnClose = new ROC.itemBaseColor();
			this.itemColumnOpen = new ROC.itemBaseColor();
			this.itemColumnLow = new ROC.itemBaseColor();
			this.itemColumnHigh = new ROC.itemBaseColor();
			this.itemColumnVolume = new ROC.itemBaseColor();
			this.itemColumnPctChange = new ROC.itemBaseColor();
			this.itemColumnNetChange = new ROC.itemBaseColor();
			this.itemColumnTradePartID = new ROC.itemBaseColor();
			this.itemColumnTradeVolume = new ROC.itemBaseColor();
			this.itemColumnTick = new ROC.itemBaseColor();
			this.itemColumnLastTraded = new ROC.itemBaseColor();
			this.itemColumnAskLow = new ROC.itemBaseColor();
			this.itemColumnAskHigh = new ROC.itemBaseColor();
			this.itemColumnAskPartID = new ROC.itemBaseColor();
			this.itemColumnAskSize = new ROC.itemBaseColor();
			this.itemColumnAskPrice = new ROC.itemBaseColor();
			this.itemColumnBidLow = new ROC.itemBaseColor();
			this.itemColumnBidHigh = new ROC.itemBaseColor();
			this.itemColumnBidPartID = new ROC.itemBaseColor();
			this.itemColumnBidSize = new ROC.itemBaseColor();
			this.itemColumnBidPrice = new ROC.itemBaseColor();
			this.itemColumnDetail = new ROC.itemBaseColor();
			this.itemColumnSymbol = new ROC.itemBaseColor();
			this.cmdDefaultColor = new ButtonEx.VistaButton();
			this.grpRowType = new System.Windows.Forms.GroupBox();
			this.cboRowType = new System.Windows.Forms.ComboBox();
			this.grpGrid = new System.Windows.Forms.GroupBox();
			this.itemFontText = new ROC.itemBaseFont();
			this.itemColorGridlines = new ROC.itemBaseColor();
			this.itemColorSelectedBackground = new ROC.itemBaseColor();
			this.itemColorBackground = new ROC.itemBaseColor();
			this.itemColorText = new ROC.itemBaseColor();
			this.grpColumnExt = new System.Windows.Forms.GroupBox();
			this.itemColumnSecType = new ROC.itemBaseColor();
			this.itemColumnDisplayFactor = new ROC.itemBaseColor();
			this.itemColumnTickSize = new ROC.itemBaseColor();
			this.itemColumnClientRecivedTime = new ROC.itemBaseColor();
			this.itemColumnServerSentTime = new ROC.itemBaseColor();
			this.itemColumnOnMsgTime = new ROC.itemBaseColor();
			this.itemColumnDecodeTime = new ROC.itemBaseColor();
			this.itemColumnLineTime = new ROC.itemBaseColor();
			this.grpAnimation.SuspendLayout();
			this.panelControl.SuspendLayout();
			this.grpColumn.SuspendLayout();
			this.grpRowType.SuspendLayout();
			this.grpGrid.SuspendLayout();
			this.grpColumnExt.SuspendLayout();
			this.SuspendLayout();
			// 
			// grpAnimation
			// 
			this.grpAnimation.Controls.Add(this.cboNotificationType);
			this.grpAnimation.Controls.Add(this.itemColorDown);
			this.grpAnimation.Controls.Add(this.itemColorUp);
			this.grpAnimation.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.grpAnimation.ForeColor = System.Drawing.Color.White;
			this.grpAnimation.Location = new System.Drawing.Point(139, 289);
			this.grpAnimation.Name = "grpAnimation";
			this.grpAnimation.Size = new System.Drawing.Size(124, 99);
			this.grpAnimation.TabIndex = 43;
			this.grpAnimation.TabStop = false;
			this.grpAnimation.Text = "Animation";
			// 
			// cboNotificationType
			// 
			this.cboNotificationType.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.cboNotificationType.FormattingEnabled = true;
			this.cboNotificationType.Items.AddRange(new object[] {
            "None",
            "Border",
            "BackGround",
            "ForeGround"});
			this.cboNotificationType.Location = new System.Drawing.Point(3, 75);
			this.cboNotificationType.Name = "cboNotificationType";
			this.cboNotificationType.Size = new System.Drawing.Size(118, 21);
			this.cboNotificationType.TabIndex = 47;
			// 
			// itemColorDown
			// 
			this.itemColorDown.BackColor = System.Drawing.Color.Black;
			this.itemColorDown.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemColorDown.ForeColor = System.Drawing.Color.White;
			this.itemColorDown.ItemColor = System.Drawing.Color.Black;
			this.itemColorDown.ItemName = "Down -";
			this.itemColorDown.Location = new System.Drawing.Point(3, 40);
			this.itemColorDown.Name = "itemColorDown";
			this.itemColorDown.Padding = new System.Windows.Forms.Padding(2);
			this.itemColorDown.Size = new System.Drawing.Size(118, 24);
			this.itemColorDown.TabIndex = 5;
			// 
			// itemColorUp
			// 
			this.itemColorUp.BackColor = System.Drawing.Color.Black;
			this.itemColorUp.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemColorUp.ForeColor = System.Drawing.Color.White;
			this.itemColorUp.ItemColor = System.Drawing.Color.Black;
			this.itemColorUp.ItemName = "Up +";
			this.itemColorUp.Location = new System.Drawing.Point(3, 16);
			this.itemColorUp.Name = "itemColorUp";
			this.itemColorUp.Padding = new System.Windows.Forms.Padding(2);
			this.itemColorUp.Size = new System.Drawing.Size(118, 24);
			this.itemColorUp.TabIndex = 46;
			// 
			// panelControl
			// 
			this.panelControl.BackColor = System.Drawing.Color.Black;
			this.panelControl.Controls.Add(this.cmdApplyToAll);
			this.panelControl.Controls.Add(this.cmdApply);
			this.panelControl.Controls.Add(this.cmdCancel);
			this.panelControl.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.panelControl.Location = new System.Drawing.Point(0, 667);
			this.panelControl.Name = "panelControl";
			this.panelControl.Padding = new System.Windows.Forms.Padding(5);
			this.panelControl.Size = new System.Drawing.Size(266, 31);
			this.panelControl.TabIndex = 42;
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
			// grpColumn
			// 
			this.grpColumn.Controls.Add(this.itemColumnName);
			this.grpColumn.Controls.Add(this.itemColumnVwap);
			this.grpColumn.Controls.Add(this.itemColumnClose);
			this.grpColumn.Controls.Add(this.itemColumnOpen);
			this.grpColumn.Controls.Add(this.itemColumnLow);
			this.grpColumn.Controls.Add(this.itemColumnHigh);
			this.grpColumn.Controls.Add(this.itemColumnVolume);
			this.grpColumn.Controls.Add(this.itemColumnPctChange);
			this.grpColumn.Controls.Add(this.itemColumnNetChange);
			this.grpColumn.Controls.Add(this.itemColumnTradePartID);
			this.grpColumn.Controls.Add(this.itemColumnTradeVolume);
			this.grpColumn.Controls.Add(this.itemColumnTick);
			this.grpColumn.Controls.Add(this.itemColumnLastTraded);
			this.grpColumn.Controls.Add(this.itemColumnAskLow);
			this.grpColumn.Controls.Add(this.itemColumnAskHigh);
			this.grpColumn.Controls.Add(this.itemColumnAskPartID);
			this.grpColumn.Controls.Add(this.itemColumnAskSize);
			this.grpColumn.Controls.Add(this.itemColumnAskPrice);
			this.grpColumn.Controls.Add(this.itemColumnBidLow);
			this.grpColumn.Controls.Add(this.itemColumnBidHigh);
			this.grpColumn.Controls.Add(this.itemColumnBidPartID);
			this.grpColumn.Controls.Add(this.itemColumnBidSize);
			this.grpColumn.Controls.Add(this.itemColumnBidPrice);
			this.grpColumn.Controls.Add(this.itemColumnDetail);
			this.grpColumn.Controls.Add(this.itemColumnSymbol);
			this.grpColumn.Controls.Add(this.cmdDefaultColor);
			this.grpColumn.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.grpColumn.ForeColor = System.Drawing.Color.White;
			this.grpColumn.Location = new System.Drawing.Point(5, 12);
			this.grpColumn.Name = "grpColumn";
			this.grpColumn.Size = new System.Drawing.Size(133, 649);
			this.grpColumn.TabIndex = 41;
			this.grpColumn.TabStop = false;
			this.grpColumn.Text = "Columns";
			// 
			// itemColumnName
			// 
			this.itemColumnName.BackColor = System.Drawing.Color.Black;
			this.itemColumnName.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemColumnName.ForeColor = System.Drawing.Color.White;
			this.itemColumnName.ItemColor = System.Drawing.Color.Black;
			this.itemColumnName.ItemName = "Name";
			this.itemColumnName.Location = new System.Drawing.Point(3, 616);
			this.itemColumnName.Name = "itemColumnName";
			this.itemColumnName.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnName.Size = new System.Drawing.Size(127, 24);
			this.itemColumnName.TabIndex = 69;
			// 
			// itemColumnVwap
			// 
			this.itemColumnVwap.BackColor = System.Drawing.Color.Black;
			this.itemColumnVwap.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemColumnVwap.ForeColor = System.Drawing.Color.White;
			this.itemColumnVwap.ItemColor = System.Drawing.Color.Black;
			this.itemColumnVwap.ItemName = "Vwap";
			this.itemColumnVwap.Location = new System.Drawing.Point(3, 592);
			this.itemColumnVwap.Name = "itemColumnVwap";
			this.itemColumnVwap.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnVwap.Size = new System.Drawing.Size(127, 24);
			this.itemColumnVwap.TabIndex = 68;
			// 
			// itemColumnClose
			// 
			this.itemColumnClose.BackColor = System.Drawing.Color.Black;
			this.itemColumnClose.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemColumnClose.ForeColor = System.Drawing.Color.White;
			this.itemColumnClose.ItemColor = System.Drawing.Color.Black;
			this.itemColumnClose.ItemName = "Close";
			this.itemColumnClose.Location = new System.Drawing.Point(3, 568);
			this.itemColumnClose.Name = "itemColumnClose";
			this.itemColumnClose.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnClose.Size = new System.Drawing.Size(127, 24);
			this.itemColumnClose.TabIndex = 67;
			// 
			// itemColumnOpen
			// 
			this.itemColumnOpen.BackColor = System.Drawing.Color.Black;
			this.itemColumnOpen.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemColumnOpen.ForeColor = System.Drawing.Color.White;
			this.itemColumnOpen.ItemColor = System.Drawing.Color.Black;
			this.itemColumnOpen.ItemName = "Open";
			this.itemColumnOpen.Location = new System.Drawing.Point(3, 544);
			this.itemColumnOpen.Name = "itemColumnOpen";
			this.itemColumnOpen.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnOpen.Size = new System.Drawing.Size(127, 24);
			this.itemColumnOpen.TabIndex = 66;
			// 
			// itemColumnLow
			// 
			this.itemColumnLow.BackColor = System.Drawing.Color.Black;
			this.itemColumnLow.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemColumnLow.ForeColor = System.Drawing.Color.White;
			this.itemColumnLow.ItemColor = System.Drawing.Color.Black;
			this.itemColumnLow.ItemName = "Low";
			this.itemColumnLow.Location = new System.Drawing.Point(3, 520);
			this.itemColumnLow.Name = "itemColumnLow";
			this.itemColumnLow.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnLow.Size = new System.Drawing.Size(127, 24);
			this.itemColumnLow.TabIndex = 65;
			// 
			// itemColumnHigh
			// 
			this.itemColumnHigh.BackColor = System.Drawing.Color.Black;
			this.itemColumnHigh.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemColumnHigh.ForeColor = System.Drawing.Color.White;
			this.itemColumnHigh.ItemColor = System.Drawing.Color.Black;
			this.itemColumnHigh.ItemName = "High";
			this.itemColumnHigh.Location = new System.Drawing.Point(3, 496);
			this.itemColumnHigh.Name = "itemColumnHigh";
			this.itemColumnHigh.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnHigh.Size = new System.Drawing.Size(127, 24);
			this.itemColumnHigh.TabIndex = 31;
			// 
			// itemColumnVolume
			// 
			this.itemColumnVolume.BackColor = System.Drawing.Color.Black;
			this.itemColumnVolume.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemColumnVolume.ForeColor = System.Drawing.Color.White;
			this.itemColumnVolume.ItemColor = System.Drawing.Color.Black;
			this.itemColumnVolume.ItemName = "Volume";
			this.itemColumnVolume.Location = new System.Drawing.Point(3, 472);
			this.itemColumnVolume.Name = "itemColumnVolume";
			this.itemColumnVolume.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnVolume.Size = new System.Drawing.Size(127, 24);
			this.itemColumnVolume.TabIndex = 30;
			// 
			// itemColumnPctChange
			// 
			this.itemColumnPctChange.BackColor = System.Drawing.Color.Black;
			this.itemColumnPctChange.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemColumnPctChange.ForeColor = System.Drawing.Color.White;
			this.itemColumnPctChange.ItemColor = System.Drawing.Color.Black;
			this.itemColumnPctChange.ItemName = "Pct Change";
			this.itemColumnPctChange.Location = new System.Drawing.Point(3, 448);
			this.itemColumnPctChange.Name = "itemColumnPctChange";
			this.itemColumnPctChange.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnPctChange.Size = new System.Drawing.Size(127, 24);
			this.itemColumnPctChange.TabIndex = 29;
			// 
			// itemColumnNetChange
			// 
			this.itemColumnNetChange.BackColor = System.Drawing.Color.Black;
			this.itemColumnNetChange.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemColumnNetChange.ForeColor = System.Drawing.Color.White;
			this.itemColumnNetChange.ItemColor = System.Drawing.Color.Black;
			this.itemColumnNetChange.ItemName = "Net Change";
			this.itemColumnNetChange.Location = new System.Drawing.Point(3, 424);
			this.itemColumnNetChange.Name = "itemColumnNetChange";
			this.itemColumnNetChange.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnNetChange.Size = new System.Drawing.Size(127, 24);
			this.itemColumnNetChange.TabIndex = 28;
			// 
			// itemColumnTradePartID
			// 
			this.itemColumnTradePartID.BackColor = System.Drawing.Color.Black;
			this.itemColumnTradePartID.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemColumnTradePartID.ForeColor = System.Drawing.Color.White;
			this.itemColumnTradePartID.ItemColor = System.Drawing.Color.Black;
			this.itemColumnTradePartID.ItemName = "Traded PID";
			this.itemColumnTradePartID.Location = new System.Drawing.Point(3, 400);
			this.itemColumnTradePartID.Name = "itemColumnTradePartID";
			this.itemColumnTradePartID.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnTradePartID.Size = new System.Drawing.Size(127, 24);
			this.itemColumnTradePartID.TabIndex = 64;
			// 
			// itemColumnTradeVolume
			// 
			this.itemColumnTradeVolume.BackColor = System.Drawing.Color.Black;
			this.itemColumnTradeVolume.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemColumnTradeVolume.ForeColor = System.Drawing.Color.White;
			this.itemColumnTradeVolume.ItemColor = System.Drawing.Color.Black;
			this.itemColumnTradeVolume.ItemName = "Traded Vol";
			this.itemColumnTradeVolume.Location = new System.Drawing.Point(3, 376);
			this.itemColumnTradeVolume.Name = "itemColumnTradeVolume";
			this.itemColumnTradeVolume.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnTradeVolume.Size = new System.Drawing.Size(127, 24);
			this.itemColumnTradeVolume.TabIndex = 63;
			// 
			// itemColumnTick
			// 
			this.itemColumnTick.BackColor = System.Drawing.Color.Black;
			this.itemColumnTick.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemColumnTick.ForeColor = System.Drawing.Color.White;
			this.itemColumnTick.ItemColor = System.Drawing.Color.Black;
			this.itemColumnTick.ItemName = "Tick";
			this.itemColumnTick.Location = new System.Drawing.Point(3, 352);
			this.itemColumnTick.Name = "itemColumnTick";
			this.itemColumnTick.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnTick.Size = new System.Drawing.Size(127, 24);
			this.itemColumnTick.TabIndex = 70;
			// 
			// itemColumnLastTraded
			// 
			this.itemColumnLastTraded.BackColor = System.Drawing.Color.Black;
			this.itemColumnLastTraded.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemColumnLastTraded.ForeColor = System.Drawing.Color.White;
			this.itemColumnLastTraded.ItemColor = System.Drawing.Color.Black;
			this.itemColumnLastTraded.ItemName = "Last Traded";
			this.itemColumnLastTraded.Location = new System.Drawing.Point(3, 328);
			this.itemColumnLastTraded.Name = "itemColumnLastTraded";
			this.itemColumnLastTraded.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnLastTraded.Size = new System.Drawing.Size(127, 24);
			this.itemColumnLastTraded.TabIndex = 50;
			// 
			// itemColumnAskLow
			// 
			this.itemColumnAskLow.BackColor = System.Drawing.Color.Black;
			this.itemColumnAskLow.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemColumnAskLow.ForeColor = System.Drawing.Color.White;
			this.itemColumnAskLow.ItemColor = System.Drawing.Color.Black;
			this.itemColumnAskLow.ItemName = "Ask Low";
			this.itemColumnAskLow.Location = new System.Drawing.Point(3, 304);
			this.itemColumnAskLow.Name = "itemColumnAskLow";
			this.itemColumnAskLow.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnAskLow.Size = new System.Drawing.Size(127, 24);
			this.itemColumnAskLow.TabIndex = 61;
			// 
			// itemColumnAskHigh
			// 
			this.itemColumnAskHigh.BackColor = System.Drawing.Color.Black;
			this.itemColumnAskHigh.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemColumnAskHigh.ForeColor = System.Drawing.Color.White;
			this.itemColumnAskHigh.ItemColor = System.Drawing.Color.Black;
			this.itemColumnAskHigh.ItemName = "Ask High";
			this.itemColumnAskHigh.Location = new System.Drawing.Point(3, 280);
			this.itemColumnAskHigh.Name = "itemColumnAskHigh";
			this.itemColumnAskHigh.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnAskHigh.Size = new System.Drawing.Size(127, 24);
			this.itemColumnAskHigh.TabIndex = 60;
			// 
			// itemColumnAskPartID
			// 
			this.itemColumnAskPartID.BackColor = System.Drawing.Color.Black;
			this.itemColumnAskPartID.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemColumnAskPartID.ForeColor = System.Drawing.Color.White;
			this.itemColumnAskPartID.ItemColor = System.Drawing.Color.Black;
			this.itemColumnAskPartID.ItemName = "Ask PID";
			this.itemColumnAskPartID.Location = new System.Drawing.Point(3, 256);
			this.itemColumnAskPartID.Name = "itemColumnAskPartID";
			this.itemColumnAskPartID.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnAskPartID.Size = new System.Drawing.Size(127, 24);
			this.itemColumnAskPartID.TabIndex = 59;
			// 
			// itemColumnAskSize
			// 
			this.itemColumnAskSize.BackColor = System.Drawing.Color.Black;
			this.itemColumnAskSize.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemColumnAskSize.ForeColor = System.Drawing.Color.White;
			this.itemColumnAskSize.ItemColor = System.Drawing.Color.Black;
			this.itemColumnAskSize.ItemName = "Ask Size";
			this.itemColumnAskSize.Location = new System.Drawing.Point(3, 232);
			this.itemColumnAskSize.Name = "itemColumnAskSize";
			this.itemColumnAskSize.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnAskSize.Size = new System.Drawing.Size(127, 24);
			this.itemColumnAskSize.TabIndex = 27;
			// 
			// itemColumnAskPrice
			// 
			this.itemColumnAskPrice.BackColor = System.Drawing.Color.Black;
			this.itemColumnAskPrice.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemColumnAskPrice.ForeColor = System.Drawing.Color.White;
			this.itemColumnAskPrice.ItemColor = System.Drawing.Color.Black;
			this.itemColumnAskPrice.ItemName = "AskPrice";
			this.itemColumnAskPrice.Location = new System.Drawing.Point(3, 208);
			this.itemColumnAskPrice.Name = "itemColumnAskPrice";
			this.itemColumnAskPrice.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnAskPrice.Size = new System.Drawing.Size(127, 24);
			this.itemColumnAskPrice.TabIndex = 26;
			// 
			// itemColumnBidLow
			// 
			this.itemColumnBidLow.BackColor = System.Drawing.Color.Black;
			this.itemColumnBidLow.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemColumnBidLow.ForeColor = System.Drawing.Color.White;
			this.itemColumnBidLow.ItemColor = System.Drawing.Color.Black;
			this.itemColumnBidLow.ItemName = "Bid Low";
			this.itemColumnBidLow.Location = new System.Drawing.Point(3, 184);
			this.itemColumnBidLow.Name = "itemColumnBidLow";
			this.itemColumnBidLow.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnBidLow.Size = new System.Drawing.Size(127, 24);
			this.itemColumnBidLow.TabIndex = 58;
			// 
			// itemColumnBidHigh
			// 
			this.itemColumnBidHigh.BackColor = System.Drawing.Color.Black;
			this.itemColumnBidHigh.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemColumnBidHigh.ForeColor = System.Drawing.Color.White;
			this.itemColumnBidHigh.ItemColor = System.Drawing.Color.Black;
			this.itemColumnBidHigh.ItemName = "Bid High";
			this.itemColumnBidHigh.Location = new System.Drawing.Point(3, 160);
			this.itemColumnBidHigh.Name = "itemColumnBidHigh";
			this.itemColumnBidHigh.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnBidHigh.Size = new System.Drawing.Size(127, 24);
			this.itemColumnBidHigh.TabIndex = 57;
			// 
			// itemColumnBidPartID
			// 
			this.itemColumnBidPartID.BackColor = System.Drawing.Color.Black;
			this.itemColumnBidPartID.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemColumnBidPartID.ForeColor = System.Drawing.Color.White;
			this.itemColumnBidPartID.ItemColor = System.Drawing.Color.Black;
			this.itemColumnBidPartID.ItemName = "Bid PID";
			this.itemColumnBidPartID.Location = new System.Drawing.Point(3, 136);
			this.itemColumnBidPartID.Name = "itemColumnBidPartID";
			this.itemColumnBidPartID.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnBidPartID.Size = new System.Drawing.Size(127, 24);
			this.itemColumnBidPartID.TabIndex = 56;
			// 
			// itemColumnBidSize
			// 
			this.itemColumnBidSize.BackColor = System.Drawing.Color.Black;
			this.itemColumnBidSize.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemColumnBidSize.ForeColor = System.Drawing.Color.White;
			this.itemColumnBidSize.ItemColor = System.Drawing.Color.Black;
			this.itemColumnBidSize.ItemName = "Bid Size";
			this.itemColumnBidSize.Location = new System.Drawing.Point(3, 112);
			this.itemColumnBidSize.Name = "itemColumnBidSize";
			this.itemColumnBidSize.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnBidSize.Size = new System.Drawing.Size(127, 24);
			this.itemColumnBidSize.TabIndex = 25;
			// 
			// itemColumnBidPrice
			// 
			this.itemColumnBidPrice.BackColor = System.Drawing.Color.Black;
			this.itemColumnBidPrice.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemColumnBidPrice.ForeColor = System.Drawing.Color.White;
			this.itemColumnBidPrice.ItemColor = System.Drawing.Color.Black;
			this.itemColumnBidPrice.ItemName = "Bid Price";
			this.itemColumnBidPrice.Location = new System.Drawing.Point(3, 88);
			this.itemColumnBidPrice.Name = "itemColumnBidPrice";
			this.itemColumnBidPrice.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnBidPrice.Size = new System.Drawing.Size(127, 24);
			this.itemColumnBidPrice.TabIndex = 24;
			// 
			// itemColumnDetail
			// 
			this.itemColumnDetail.BackColor = System.Drawing.Color.Black;
			this.itemColumnDetail.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemColumnDetail.ForeColor = System.Drawing.Color.White;
			this.itemColumnDetail.ItemColor = System.Drawing.Color.Black;
			this.itemColumnDetail.ItemName = "Detail";
			this.itemColumnDetail.Location = new System.Drawing.Point(3, 64);
			this.itemColumnDetail.Name = "itemColumnDetail";
			this.itemColumnDetail.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnDetail.Size = new System.Drawing.Size(127, 24);
			this.itemColumnDetail.TabIndex = 23;
			// 
			// itemColumnSymbol
			// 
			this.itemColumnSymbol.BackColor = System.Drawing.Color.Black;
			this.itemColumnSymbol.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemColumnSymbol.ForeColor = System.Drawing.Color.White;
			this.itemColumnSymbol.ItemColor = System.Drawing.Color.Black;
			this.itemColumnSymbol.ItemName = "Symbol";
			this.itemColumnSymbol.Location = new System.Drawing.Point(3, 40);
			this.itemColumnSymbol.Name = "itemColumnSymbol";
			this.itemColumnSymbol.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnSymbol.Size = new System.Drawing.Size(127, 24);
			this.itemColumnSymbol.TabIndex = 62;
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
			this.grpRowType.TabIndex = 40;
			this.grpRowType.TabStop = false;
			this.grpRowType.Text = "Row Type";
			// 
			// cboRowType
			// 
			this.cboRowType.Dock = System.Windows.Forms.DockStyle.Top;
			this.cboRowType.FormattingEnabled = true;
			this.cboRowType.Items.AddRange(new object[] {
            "None",
            "Performence"});
			this.cboRowType.Location = new System.Drawing.Point(6, 19);
			this.cboRowType.Name = "cboRowType";
			this.cboRowType.Size = new System.Drawing.Size(112, 21);
			this.cboRowType.TabIndex = 0;
			this.cboRowType.SelectedIndexChanged += new System.EventHandler(this.cboRowType_SelectedIndexChanged);
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
			this.grpGrid.TabIndex = 39;
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
			// grpColumnExt
			// 
			this.grpColumnExt.Controls.Add(this.itemColumnSecType);
			this.grpColumnExt.Controls.Add(this.itemColumnDisplayFactor);
			this.grpColumnExt.Controls.Add(this.itemColumnTickSize);
			this.grpColumnExt.Controls.Add(this.itemColumnClientRecivedTime);
			this.grpColumnExt.Controls.Add(this.itemColumnServerSentTime);
			this.grpColumnExt.Controls.Add(this.itemColumnOnMsgTime);
			this.grpColumnExt.Controls.Add(this.itemColumnDecodeTime);
			this.grpColumnExt.Controls.Add(this.itemColumnLineTime);
			this.grpColumnExt.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.grpColumnExt.ForeColor = System.Drawing.Color.White;
			this.grpColumnExt.Location = new System.Drawing.Point(139, 394);
			this.grpColumnExt.Name = "grpColumnExt";
			this.grpColumnExt.Padding = new System.Windows.Forms.Padding(6);
			this.grpColumnExt.Size = new System.Drawing.Size(124, 267);
			this.grpColumnExt.TabIndex = 44;
			this.grpColumnExt.TabStop = false;
			this.grpColumnExt.Text = "Ext Columns";
			// 
			// itemColumnSecType
			// 
			this.itemColumnSecType.BackColor = System.Drawing.Color.Black;
			this.itemColumnSecType.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemColumnSecType.ForeColor = System.Drawing.Color.White;
			this.itemColumnSecType.ItemColor = System.Drawing.Color.Black;
			this.itemColumnSecType.ItemName = "Sec Type";
			this.itemColumnSecType.Location = new System.Drawing.Point(6, 187);
			this.itemColumnSecType.Name = "itemColumnSecType";
			this.itemColumnSecType.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnSecType.Size = new System.Drawing.Size(112, 24);
			this.itemColumnSecType.TabIndex = 77;
			// 
			// itemColumnDisplayFactor
			// 
			this.itemColumnDisplayFactor.BackColor = System.Drawing.Color.Black;
			this.itemColumnDisplayFactor.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemColumnDisplayFactor.ForeColor = System.Drawing.Color.White;
			this.itemColumnDisplayFactor.ItemColor = System.Drawing.Color.Black;
			this.itemColumnDisplayFactor.ItemName = "Factor";
			this.itemColumnDisplayFactor.Location = new System.Drawing.Point(6, 163);
			this.itemColumnDisplayFactor.Name = "itemColumnDisplayFactor";
			this.itemColumnDisplayFactor.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnDisplayFactor.Size = new System.Drawing.Size(112, 24);
			this.itemColumnDisplayFactor.TabIndex = 76;
			// 
			// itemColumnTickSize
			// 
			this.itemColumnTickSize.BackColor = System.Drawing.Color.Black;
			this.itemColumnTickSize.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemColumnTickSize.ForeColor = System.Drawing.Color.White;
			this.itemColumnTickSize.ItemColor = System.Drawing.Color.Black;
			this.itemColumnTickSize.ItemName = "TickSize";
			this.itemColumnTickSize.Location = new System.Drawing.Point(6, 139);
			this.itemColumnTickSize.Name = "itemColumnTickSize";
			this.itemColumnTickSize.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnTickSize.Size = new System.Drawing.Size(112, 24);
			this.itemColumnTickSize.TabIndex = 75;
			// 
			// itemColumnClientRecivedTime
			// 
			this.itemColumnClientRecivedTime.BackColor = System.Drawing.Color.Black;
			this.itemColumnClientRecivedTime.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemColumnClientRecivedTime.ForeColor = System.Drawing.Color.White;
			this.itemColumnClientRecivedTime.ItemColor = System.Drawing.Color.Black;
			this.itemColumnClientRecivedTime.ItemName = "Recived";
			this.itemColumnClientRecivedTime.Location = new System.Drawing.Point(6, 115);
			this.itemColumnClientRecivedTime.Name = "itemColumnClientRecivedTime";
			this.itemColumnClientRecivedTime.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnClientRecivedTime.Size = new System.Drawing.Size(112, 24);
			this.itemColumnClientRecivedTime.TabIndex = 74;
			// 
			// itemColumnServerSentTime
			// 
			this.itemColumnServerSentTime.BackColor = System.Drawing.Color.Black;
			this.itemColumnServerSentTime.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemColumnServerSentTime.ForeColor = System.Drawing.Color.White;
			this.itemColumnServerSentTime.ItemColor = System.Drawing.Color.Black;
			this.itemColumnServerSentTime.ItemName = "Sent";
			this.itemColumnServerSentTime.Location = new System.Drawing.Point(6, 91);
			this.itemColumnServerSentTime.Name = "itemColumnServerSentTime";
			this.itemColumnServerSentTime.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnServerSentTime.Size = new System.Drawing.Size(112, 24);
			this.itemColumnServerSentTime.TabIndex = 73;
			// 
			// itemColumnOnMsgTime
			// 
			this.itemColumnOnMsgTime.BackColor = System.Drawing.Color.Black;
			this.itemColumnOnMsgTime.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemColumnOnMsgTime.ForeColor = System.Drawing.Color.White;
			this.itemColumnOnMsgTime.ItemColor = System.Drawing.Color.Black;
			this.itemColumnOnMsgTime.ItemName = "OnMsg";
			this.itemColumnOnMsgTime.Location = new System.Drawing.Point(6, 67);
			this.itemColumnOnMsgTime.Name = "itemColumnOnMsgTime";
			this.itemColumnOnMsgTime.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnOnMsgTime.Size = new System.Drawing.Size(112, 24);
			this.itemColumnOnMsgTime.TabIndex = 72;
			// 
			// itemColumnDecodeTime
			// 
			this.itemColumnDecodeTime.BackColor = System.Drawing.Color.Black;
			this.itemColumnDecodeTime.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemColumnDecodeTime.ForeColor = System.Drawing.Color.White;
			this.itemColumnDecodeTime.ItemColor = System.Drawing.Color.Black;
			this.itemColumnDecodeTime.ItemName = "Decode";
			this.itemColumnDecodeTime.Location = new System.Drawing.Point(6, 43);
			this.itemColumnDecodeTime.Name = "itemColumnDecodeTime";
			this.itemColumnDecodeTime.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnDecodeTime.Size = new System.Drawing.Size(112, 24);
			this.itemColumnDecodeTime.TabIndex = 71;
			// 
			// itemColumnLineTime
			// 
			this.itemColumnLineTime.BackColor = System.Drawing.Color.Black;
			this.itemColumnLineTime.Dock = System.Windows.Forms.DockStyle.Top;
			this.itemColumnLineTime.ForeColor = System.Drawing.Color.White;
			this.itemColumnLineTime.ItemColor = System.Drawing.Color.Black;
			this.itemColumnLineTime.ItemName = "Line";
			this.itemColumnLineTime.Location = new System.Drawing.Point(6, 19);
			this.itemColumnLineTime.Name = "itemColumnLineTime";
			this.itemColumnLineTime.Padding = new System.Windows.Forms.Padding(2);
			this.itemColumnLineTime.Size = new System.Drawing.Size(112, 24);
			this.itemColumnLineTime.TabIndex = 70;
			// 
			// frmWatchListDisplay
			// 
			this.AnimationType = FormEx.VistaWindowBase.AnimationTypes.Slide;
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.BackColor = System.Drawing.SystemColors.ControlDarkDark;
			this.ClientSize = new System.Drawing.Size(266, 698);
			this.Controls.Add(this.grpColumnExt);
			this.Controls.Add(this.grpAnimation);
			this.Controls.Add(this.panelControl);
			this.Controls.Add(this.grpColumn);
			this.Controls.Add(this.grpRowType);
			this.Controls.Add(this.grpGrid);
			this.ForeColor = System.Drawing.Color.White;
			this.Name = "frmWatchListDisplay";
			this.ShowInTaskbar = false;
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.Text = "frmWatchListDisplay";
			this.grpAnimation.ResumeLayout(false);
			this.panelControl.ResumeLayout(false);
			this.grpColumn.ResumeLayout(false);
			this.grpRowType.ResumeLayout(false);
			this.grpGrid.ResumeLayout(false);
			this.grpColumnExt.ResumeLayout(false);
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.GroupBox grpAnimation;
		private System.Windows.Forms.ComboBox cboNotificationType;
		public itemBaseColor itemColorDown;
		public itemBaseColor itemColorUp;
		private PanelEx.VistaPanel panelControl;
		private ButtonEx.VistaButton cmdApplyToAll;
		private ButtonEx.VistaButton cmdApply;
		private ButtonEx.VistaButton cmdCancel;
		private System.Windows.Forms.GroupBox grpColumn;
		public itemBaseColor itemColumnHigh;
		public itemBaseColor itemColumnVolume;
		public itemBaseColor itemColumnPctChange;
		public itemBaseColor itemColumnNetChange;
		public itemBaseColor itemColumnLastTraded;
		public itemBaseColor itemColumnAskSize;
		public itemBaseColor itemColumnAskPrice;
		public itemBaseColor itemColumnBidSize;
		public itemBaseColor itemColumnBidPrice;
		public itemBaseColor itemColumnDetail;
		private ButtonEx.VistaButton cmdDefaultColor;
		private System.Windows.Forms.GroupBox grpRowType;
		private System.Windows.Forms.ComboBox cboRowType;
		private System.Windows.Forms.GroupBox grpGrid;
		public itemBaseFont itemFontText;
		public itemBaseColor itemColorGridlines;
		public itemBaseColor itemColorSelectedBackground;
		public itemBaseColor itemColorBackground;
		public itemBaseColor itemColorText;
		public itemBaseColor itemColumnAskLow;
		public itemBaseColor itemColumnAskHigh;
		public itemBaseColor itemColumnAskPartID;
		public itemBaseColor itemColumnBidLow;
		public itemBaseColor itemColumnBidHigh;
		public itemBaseColor itemColumnBidPartID;
		public itemBaseColor itemColumnSymbol;
		public itemBaseColor itemColumnTradePartID;
		public itemBaseColor itemColumnTradeVolume;
		public itemBaseColor itemColumnOpen;
		public itemBaseColor itemColumnLow;
		public itemBaseColor itemColumnName;
		public itemBaseColor itemColumnVwap;
		public itemBaseColor itemColumnClose;
		public itemBaseColor itemColumnTick;
		private System.Windows.Forms.GroupBox grpColumnExt;
		public itemBaseColor itemColumnClientRecivedTime;
		public itemBaseColor itemColumnServerSentTime;
		public itemBaseColor itemColumnOnMsgTime;
		public itemBaseColor itemColumnDecodeTime;
		public itemBaseColor itemColumnLineTime;
		public itemBaseColor itemColumnSecType;
		public itemBaseColor itemColumnDisplayFactor;
		public itemBaseColor itemColumnTickSize;
	}
}