namespace ROC
{
	partial class frmBatchTicket
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
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(frmBatchTicket));
			DataGridViewEx.DGVBasePrinter dgvBasePrinter1 = new DataGridViewEx.DGVBasePrinter();
			System.Drawing.StringFormat stringFormat1 = new System.Drawing.StringFormat();
			System.Drawing.StringFormat stringFormat2 = new System.Drawing.StringFormat();
			System.Drawing.StringFormat stringFormat3 = new System.Drawing.StringFormat();
			System.Drawing.StringFormat stringFormat4 = new System.Drawing.StringFormat();
			this.CaptionBar = new FormEx.VistaWindowCaptionBar();
			this.panelTradeCommands = new PanelEx.VistaPanel();
			this.cmdFile = new ButtonEx.VistaButton();
			this.panelStatus = new PanelEx.VistaPanel();
			this.lblTotalValue = new LabelEx.LabelVista();
			this.dspTotalValue = new LabelEx.LabelVista();
			this.lblShortValue = new LabelEx.LabelVista();
			this.dspShortValue = new LabelEx.LabelVista();
			this.lblSellValue = new LabelEx.LabelVista();
			this.dspSellValue = new LabelEx.LabelVista();
			this.lblBuyValue = new LabelEx.LabelVista();
			this.dspBuyValue = new LabelEx.LabelVista();
			this.lblTradesQty = new LabelEx.LabelVista();
			this.dspTradesQty = new LabelEx.LabelVista();
			this.rocBatchList = new DataGridViewEx.ROCBatchList();
			this.panelTradeCommands.SuspendLayout();
			this.panelStatus.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.rocBatchList)).BeginInit();
			this.SuspendLayout();
			// 
			// CaptionBar
			// 
			this.CaptionBar.BackColor = System.Drawing.Color.Transparent;
			this.CaptionBar.Dock = System.Windows.Forms.DockStyle.Top;
			this.CaptionBar.Location = new System.Drawing.Point(0, 0);
			this.CaptionBar.Name = "CaptionBar";
			this.CaptionBar.ShowFit = true;
			this.CaptionBar.Size = new System.Drawing.Size(492, 26);
			this.CaptionBar.TabIndex = 0;
			// 
			// panelTradeCommands
			// 
			this.panelTradeCommands.BackColor = System.Drawing.Color.Black;
			this.panelTradeCommands.Controls.Add(this.cmdFile);
			this.panelTradeCommands.Dock = System.Windows.Forms.DockStyle.Top;
			this.panelTradeCommands.Location = new System.Drawing.Point(0, 26);
			this.panelTradeCommands.Name = "panelTradeCommands";
			this.panelTradeCommands.Size = new System.Drawing.Size(492, 24);
			this.panelTradeCommands.TabIndex = 7;
			// 
			// cmdFile
			// 
			this.cmdFile.BackColor = System.Drawing.Color.Black;
			this.cmdFile.ButtonType = ButtonEx.VistaButton.VistaButtonTypes.Split;
			this.cmdFile.Dock = System.Windows.Forms.DockStyle.Left;
			this.cmdFile.Image = global::ROC.Properties.Resources.File;
			this.cmdFile.Location = new System.Drawing.Point(0, 0);
			this.cmdFile.Name = "cmdFile";
			this.cmdFile.Size = new System.Drawing.Size(72, 24);
			this.cmdFile.TabIndex = 6;
			this.cmdFile.TabStop = false;
			this.cmdFile.Text = "File";
			this.cmdFile.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
			this.cmdFile.UserSplitSize = 20;
			// 
			// panelStatus
			// 
			this.panelStatus.BackColor = System.Drawing.Color.Black;
			this.panelStatus.Controls.Add(this.lblTotalValue);
			this.panelStatus.Controls.Add(this.dspTotalValue);
			this.panelStatus.Controls.Add(this.lblShortValue);
			this.panelStatus.Controls.Add(this.dspShortValue);
			this.panelStatus.Controls.Add(this.lblSellValue);
			this.panelStatus.Controls.Add(this.dspSellValue);
			this.panelStatus.Controls.Add(this.lblBuyValue);
			this.panelStatus.Controls.Add(this.dspBuyValue);
			this.panelStatus.Controls.Add(this.lblTradesQty);
			this.panelStatus.Controls.Add(this.dspTradesQty);
			this.panelStatus.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.panelStatus.Location = new System.Drawing.Point(0, 225);
			this.panelStatus.Name = "panelStatus";
			this.panelStatus.Size = new System.Drawing.Size(492, 17);
			this.panelStatus.TabIndex = 10;
			// 
			// lblTotalValue
			// 
			this.lblTotalValue.AutoSize = true;
			this.lblTotalValue.DefaultText = "0";
			this.lblTotalValue.DisplayFactor = 1;
			this.lblTotalValue.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblTotalValue.Location = new System.Drawing.Point(240, 0);
			this.lblTotalValue.Name = "lblTotalValue";
			this.lblTotalValue.Padding = new System.Windows.Forms.Padding(2);
			this.lblTotalValue.Size = new System.Drawing.Size(17, 17);
			this.lblTotalValue.TabIndex = 9;
			this.lblTotalValue.Text = "0";
			this.lblTotalValue.TickSize = 0.01;
			this.lblTotalValue.Value = "";
			// 
			// dspTotalValue
			// 
			this.dspTotalValue.ApplyShine = true;
			this.dspTotalValue.AutoSize = true;
			this.dspTotalValue.DefaultText = "Total";
			this.dspTotalValue.DisplayFactor = 1;
			this.dspTotalValue.Dock = System.Windows.Forms.DockStyle.Left;
			this.dspTotalValue.Location = new System.Drawing.Point(205, 0);
			this.dspTotalValue.Name = "dspTotalValue";
			this.dspTotalValue.Padding = new System.Windows.Forms.Padding(2);
			this.dspTotalValue.Size = new System.Drawing.Size(35, 17);
			this.dspTotalValue.TabIndex = 8;
			this.dspTotalValue.Text = "Total";
			this.dspTotalValue.TickSize = 0.01;
			this.dspTotalValue.Value = "";
			// 
			// lblShortValue
			// 
			this.lblShortValue.AutoSize = true;
			this.lblShortValue.DefaultText = "0";
			this.lblShortValue.DisplayFactor = 1;
			this.lblShortValue.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblShortValue.ForeColor = System.Drawing.Color.Orange;
			this.lblShortValue.Location = new System.Drawing.Point(188, 0);
			this.lblShortValue.Name = "lblShortValue";
			this.lblShortValue.Padding = new System.Windows.Forms.Padding(2);
			this.lblShortValue.Size = new System.Drawing.Size(17, 17);
			this.lblShortValue.TabIndex = 11;
			this.lblShortValue.Text = "0";
			this.lblShortValue.TickSize = 0.01;
			this.lblShortValue.Value = "";
			// 
			// dspShortValue
			// 
			this.dspShortValue.ApplyShine = true;
			this.dspShortValue.AutoSize = true;
			this.dspShortValue.DefaultText = "Short";
			this.dspShortValue.DisplayFactor = 1;
			this.dspShortValue.Dock = System.Windows.Forms.DockStyle.Left;
			this.dspShortValue.Location = new System.Drawing.Point(152, 0);
			this.dspShortValue.Name = "dspShortValue";
			this.dspShortValue.Padding = new System.Windows.Forms.Padding(2);
			this.dspShortValue.Size = new System.Drawing.Size(36, 17);
			this.dspShortValue.TabIndex = 10;
			this.dspShortValue.Text = "Short";
			this.dspShortValue.TickSize = 0.01;
			this.dspShortValue.Value = "";
			// 
			// lblSellValue
			// 
			this.lblSellValue.AutoSize = true;
			this.lblSellValue.DefaultText = "0";
			this.lblSellValue.DisplayFactor = 1;
			this.lblSellValue.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblSellValue.ForeColor = System.Drawing.Color.LimeGreen;
			this.lblSellValue.Location = new System.Drawing.Point(135, 0);
			this.lblSellValue.Name = "lblSellValue";
			this.lblSellValue.Padding = new System.Windows.Forms.Padding(2);
			this.lblSellValue.Size = new System.Drawing.Size(17, 17);
			this.lblSellValue.TabIndex = 6;
			this.lblSellValue.Text = "0";
			this.lblSellValue.TickSize = 0.01;
			this.lblSellValue.Value = "";
			// 
			// dspSellValue
			// 
			this.dspSellValue.ApplyShine = true;
			this.dspSellValue.AutoSize = true;
			this.dspSellValue.DefaultText = "Sell";
			this.dspSellValue.DisplayFactor = 1;
			this.dspSellValue.Dock = System.Windows.Forms.DockStyle.Left;
			this.dspSellValue.Location = new System.Drawing.Point(107, 0);
			this.dspSellValue.Name = "dspSellValue";
			this.dspSellValue.Padding = new System.Windows.Forms.Padding(2);
			this.dspSellValue.Size = new System.Drawing.Size(28, 17);
			this.dspSellValue.TabIndex = 5;
			this.dspSellValue.Text = "Sell";
			this.dspSellValue.TickSize = 0.01;
			this.dspSellValue.Value = "";
			// 
			// lblBuyValue
			// 
			this.lblBuyValue.AutoSize = true;
			this.lblBuyValue.DefaultText = "0";
			this.lblBuyValue.DisplayFactor = 1;
			this.lblBuyValue.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblBuyValue.ForeColor = System.Drawing.Color.Red;
			this.lblBuyValue.Location = new System.Drawing.Point(90, 0);
			this.lblBuyValue.Name = "lblBuyValue";
			this.lblBuyValue.Padding = new System.Windows.Forms.Padding(2);
			this.lblBuyValue.Size = new System.Drawing.Size(17, 17);
			this.lblBuyValue.TabIndex = 3;
			this.lblBuyValue.Text = "0";
			this.lblBuyValue.TickSize = 0.01;
			this.lblBuyValue.Value = "";
			// 
			// dspBuyValue
			// 
			this.dspBuyValue.ApplyShine = true;
			this.dspBuyValue.AutoSize = true;
			this.dspBuyValue.DefaultText = "Buy";
			this.dspBuyValue.DisplayFactor = 1;
			this.dspBuyValue.Dock = System.Windows.Forms.DockStyle.Left;
			this.dspBuyValue.Location = new System.Drawing.Point(61, 0);
			this.dspBuyValue.Name = "dspBuyValue";
			this.dspBuyValue.Padding = new System.Windows.Forms.Padding(2);
			this.dspBuyValue.Size = new System.Drawing.Size(29, 17);
			this.dspBuyValue.TabIndex = 2;
			this.dspBuyValue.Text = "Buy";
			this.dspBuyValue.TickSize = 0.01;
			this.dspBuyValue.Value = "";
			// 
			// lblTradesQty
			// 
			this.lblTradesQty.AutoSize = true;
			this.lblTradesQty.DefaultText = "0";
			this.lblTradesQty.DisplayFactor = 1;
			this.lblTradesQty.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblTradesQty.Location = new System.Drawing.Point(44, 0);
			this.lblTradesQty.Name = "lblTradesQty";
			this.lblTradesQty.Padding = new System.Windows.Forms.Padding(2);
			this.lblTradesQty.Size = new System.Drawing.Size(17, 17);
			this.lblTradesQty.TabIndex = 1;
			this.lblTradesQty.Text = "0";
			this.lblTradesQty.TickSize = 0.01;
			this.lblTradesQty.Value = "";
			// 
			// dspTradesQty
			// 
			this.dspTradesQty.ApplyShine = true;
			this.dspTradesQty.AutoSize = true;
			this.dspTradesQty.DefaultText = "Trades";
			this.dspTradesQty.DisplayFactor = 1;
			this.dspTradesQty.Dock = System.Windows.Forms.DockStyle.Left;
			this.dspTradesQty.Location = new System.Drawing.Point(0, 0);
			this.dspTradesQty.Name = "dspTradesQty";
			this.dspTradesQty.Padding = new System.Windows.Forms.Padding(2);
			this.dspTradesQty.Size = new System.Drawing.Size(44, 17);
			this.dspTradesQty.TabIndex = 0;
			this.dspTradesQty.Text = "Trades";
			this.dspTradesQty.TickSize = 0.01;
			this.dspTradesQty.Value = "";
			// 
			// rocBatchList
			// 
			this.rocBatchList.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
			new BindingListEx.ROCBindingList<string>().Add("");
			this.rocBatchList.Dock = System.Windows.Forms.DockStyle.Fill;
			this.rocBatchList.GridKeys = ((System.Collections.Generic.List<string>)(resources.GetObject("rocBatchList.GridKeys")));
			this.rocBatchList.LastSort = "";
			this.rocBatchList.Location = new System.Drawing.Point(0, 50);
			this.rocBatchList.Name = "rocBatchList";
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
			this.rocBatchList.Printer = dgvBasePrinter1;
			this.rocBatchList.Processing = false;
			this.rocBatchList.RefreshAggragation = true;
			this.rocBatchList.RocGridTable = null;
			this.rocBatchList.ShouldScrollToLastRow = false;
			this.rocBatchList.Size = new System.Drawing.Size(492, 175);
			new BindingListEx.ROCBindingList<string>().Add("");
			this.rocBatchList.TabIndex = 11;
			// 
			// frmBatchTicket
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.Caption = "Batch Ticket";
			this.ClientSize = new System.Drawing.Size(492, 242);
			this.Controls.Add(this.rocBatchList);
			this.Controls.Add(this.panelStatus);
			this.Controls.Add(this.panelTradeCommands);
			this.Controls.Add(this.CaptionBar);
			this.MinimumSize = new System.Drawing.Size(500, 250);
			this.Name = "frmBatchTicket";
			this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
			this.panelTradeCommands.ResumeLayout(false);
			this.panelStatus.ResumeLayout(false);
			this.panelStatus.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.rocBatchList)).EndInit();
			this.ResumeLayout(false);

		}

		#endregion

		private FormEx.VistaWindowCaptionBar CaptionBar;
		private PanelEx.VistaPanel panelTradeCommands;
		private ButtonEx.VistaButton cmdFile;
		private PanelEx.VistaPanel panelStatus;
		private LabelEx.LabelVista lblTotalValue;
		private LabelEx.LabelVista dspTotalValue;
		private LabelEx.LabelVista lblShortValue;
		private LabelEx.LabelVista dspShortValue;
		private LabelEx.LabelVista lblSellValue;
		private LabelEx.LabelVista dspSellValue;
		private LabelEx.LabelVista lblBuyValue;
		private LabelEx.LabelVista dspBuyValue;
		private LabelEx.LabelVista lblTradesQty;
		private LabelEx.LabelVista dspTradesQty;
		private DataGridViewEx.ROCBatchList rocBatchList;
	}
}