namespace ROC
{
	partial class frmTrades
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
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(frmTrades));
			DataGridViewEx.DGVBasePrinter dgvBasePrinter1 = new DataGridViewEx.DGVBasePrinter();
			System.Drawing.StringFormat stringFormat1 = new System.Drawing.StringFormat();
			System.Drawing.StringFormat stringFormat2 = new System.Drawing.StringFormat();
			System.Drawing.StringFormat stringFormat3 = new System.Drawing.StringFormat();
			System.Drawing.StringFormat stringFormat4 = new System.Drawing.StringFormat();
			this.rocTradesList = new DataGridViewEx.ROCTradeList();
			this.panelStatus = new PanelEx.VistaPanel();
			this.lblNetValue = new LabelEx.LabelVista();
			this.lblNetQty = new LabelEx.LabelVista();
			this.dspNetValue = new LabelEx.LabelVista();
			this.lblSellValue = new LabelEx.LabelVista();
			this.lblSellQty = new LabelEx.LabelVista();
			this.dspSellValue = new LabelEx.LabelVista();
			this.lblShortValue = new LabelEx.LabelVista();
			this.lblShortQty = new LabelEx.LabelVista();
			this.dspShortValue = new LabelEx.LabelVista();
			this.lblBuyValue = new LabelEx.LabelVista();
			this.lblBuyQty = new LabelEx.LabelVista();
			this.dspBuyValue = new LabelEx.LabelVista();
			this.dspVAL = new LabelEx.LabelVista();
			this.dspQTY = new LabelEx.LabelVista();
			this.lblTradesQty = new LabelEx.LabelVista();
			this.dspTradesQty = new LabelEx.LabelVista();
			this.CaptionBar = new FormEx.VistaWindowCaptionBar();
			this.panelPositionsCommands = new PanelEx.VistaPanel();
			this.txtSymbolFilter = new System.Windows.Forms.TextBox();
			this.lblSymbols = new LabelEx.LabelVista();
			this.cmdSecurities = new ButtonEx.VistaButton();
			this.cmdSources = new ButtonEx.VistaButton();
			this.menuProcessing = new ROC.menuBaseProcessing();
			this.panelPositionFilters = new PanelEx.VistaPanel();
			this.cmdAccounts = new ButtonEx.VistaButton();
			this.cmdShowAll = new ButtonEx.VistaButton();
			((System.ComponentModel.ISupportInitialize)(this.rocTradesList)).BeginInit();
			this.panelStatus.SuspendLayout();
			this.panelPositionsCommands.SuspendLayout();
			this.panelPositionFilters.SuspendLayout();
			this.SuspendLayout();
			// 
			// rocTradesList
			// 
			this.rocTradesList.BlueDefault = 228;
			this.rocTradesList.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
			new BindingListEx.ROCBindingList<string>().Add("");
			this.rocTradesList.DisplayFactors = ((System.Collections.Generic.Dictionary<string, double>)(resources.GetObject("rocTradesList.DisplayFactors")));
			this.rocTradesList.Dock = System.Windows.Forms.DockStyle.Fill;
			this.rocTradesList.GreenDefault = 243;
			this.rocTradesList.GridKeys = ((System.Collections.Generic.List<string>)(resources.GetObject("rocTradesList.GridKeys")));
			this.rocTradesList.HeaderBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(233)))), ((int)(((byte)(243)))), ((int)(((byte)(228)))));
			this.rocTradesList.Importing = false;
			this.rocTradesList.LastFilter = "";
			this.rocTradesList.LastSort = "[OmTime] asc";
			this.rocTradesList.Location = new System.Drawing.Point(0, 74);
			this.rocTradesList.Name = "rocTradesList";
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
			dgvBasePrinter1.PrintPreviewZoom = 0.5;
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
			this.rocTradesList.Printer = dgvBasePrinter1;
			this.rocTradesList.Processing = false;
			this.rocTradesList.RedDefault = 233;
			this.rocTradesList.RefreshAggragation = true;
			this.rocTradesList.RefreshSharedRows = false;
			this.rocTradesList.ShouldScrollToLastRow = false;
			this.rocTradesList.ShowActionMenu = false;
			this.rocTradesList.ShowColumnMenu = false;
			this.rocTradesList.Size = new System.Drawing.Size(642, 151);
			new BindingListEx.ROCBindingList<string>().Add("");
			this.rocTradesList.TabIndex = 7;
			this.rocTradesList.TickSizes = ((System.Collections.Generic.Dictionary<string, double>)(resources.GetObject("rocTradesList.TickSizes")));
			// 
			// panelStatus
			// 
			this.panelStatus.BackColor = System.Drawing.Color.Black;
			this.panelStatus.Controls.Add(this.lblNetValue);
			this.panelStatus.Controls.Add(this.lblNetQty);
			this.panelStatus.Controls.Add(this.dspNetValue);
			this.panelStatus.Controls.Add(this.lblSellValue);
			this.panelStatus.Controls.Add(this.lblSellQty);
			this.panelStatus.Controls.Add(this.dspSellValue);
			this.panelStatus.Controls.Add(this.lblShortValue);
			this.panelStatus.Controls.Add(this.lblShortQty);
			this.panelStatus.Controls.Add(this.dspShortValue);
			this.panelStatus.Controls.Add(this.lblBuyValue);
			this.panelStatus.Controls.Add(this.lblBuyQty);
			this.panelStatus.Controls.Add(this.dspBuyValue);
			this.panelStatus.Controls.Add(this.dspVAL);
			this.panelStatus.Controls.Add(this.dspQTY);
			this.panelStatus.Controls.Add(this.lblTradesQty);
			this.panelStatus.Controls.Add(this.dspTradesQty);
			this.panelStatus.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.panelStatus.Location = new System.Drawing.Point(0, 225);
			this.panelStatus.Name = "panelStatus";
			this.panelStatus.Size = new System.Drawing.Size(642, 17);
			this.panelStatus.TabIndex = 9;
			// 
			// lblNetValue
			// 
			this.lblNetValue.AutoColor = true;
			this.lblNetValue.AutoSize = true;
			this.lblNetValue.DefaultColor = System.Drawing.Color.White;
			this.lblNetValue.DefaultText = "0";
			this.lblNetValue.DisplayFactor = 1;
			this.lblNetValue.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblNetValue.DownColor = System.Drawing.Color.Red;
			this.lblNetValue.Is64Th = false;
			this.lblNetValue.IsCurrency = true;
			this.lblNetValue.IsFraction = false;
			this.lblNetValue.IsPercent = false;
			this.lblNetValue.Location = new System.Drawing.Point(396, 0);
			this.lblNetValue.MaxDecimal = 0;
			this.lblNetValue.Name = "lblNetValue";
			this.lblNetValue.Padding = new System.Windows.Forms.Padding(2);
			this.lblNetValue.Size = new System.Drawing.Size(17, 17);
			this.lblNetValue.TabIndex = 9;
			this.lblNetValue.Text = "0";
			this.lblNetValue.TickSize = 0.01;
			this.lblNetValue.UpColor = System.Drawing.Color.LimeGreen;
			this.lblNetValue.Value = "";
			// 
			// lblNetQty
			// 
			this.lblNetQty.AutoColor = true;
			this.lblNetQty.AutoSize = true;
			this.lblNetQty.DefaultColor = System.Drawing.Color.White;
			this.lblNetQty.DefaultText = "0";
			this.lblNetQty.DisplayFactor = 1;
			this.lblNetQty.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblNetQty.DownColor = System.Drawing.Color.Red;
			this.lblNetQty.Is64Th = false;
			this.lblNetQty.IsCurrency = false;
			this.lblNetQty.IsFraction = false;
			this.lblNetQty.IsPercent = false;
			this.lblNetQty.Location = new System.Drawing.Point(379, 0);
			this.lblNetQty.MaxDecimal = 0;
			this.lblNetQty.Name = "lblNetQty";
			this.lblNetQty.Padding = new System.Windows.Forms.Padding(2);
			this.lblNetQty.Size = new System.Drawing.Size(17, 17);
			this.lblNetQty.TabIndex = 15;
			this.lblNetQty.Text = "0";
			this.lblNetQty.TickSize = 0.01;
			this.lblNetQty.UpColor = System.Drawing.Color.LimeGreen;
			this.lblNetQty.Value = "";
			// 
			// dspNetValue
			// 
			this.dspNetValue.AutoColor = false;
			this.dspNetValue.AutoSize = true;
			this.dspNetValue.DefaultColor = System.Drawing.Color.White;
			this.dspNetValue.DefaultText = "NET";
			this.dspNetValue.DisplayFactor = 1;
			this.dspNetValue.Dock = System.Windows.Forms.DockStyle.Left;
			this.dspNetValue.DownColor = System.Drawing.Color.Red;
			this.dspNetValue.Is64Th = false;
			this.dspNetValue.IsCurrency = false;
			this.dspNetValue.IsFraction = false;
			this.dspNetValue.IsPercent = false;
			this.dspNetValue.Location = new System.Drawing.Point(346, 0);
			this.dspNetValue.MaxDecimal = 7;
			this.dspNetValue.Name = "dspNetValue";
			this.dspNetValue.Padding = new System.Windows.Forms.Padding(2);
			this.dspNetValue.Size = new System.Drawing.Size(33, 17);
			this.dspNetValue.TabIndex = 8;
			this.dspNetValue.Text = "NET";
			this.dspNetValue.TickSize = 0.01;
			this.dspNetValue.UpColor = System.Drawing.Color.LimeGreen;
			this.dspNetValue.Value = "";
			// 
			// lblSellValue
			// 
			this.lblSellValue.AutoColor = false;
			this.lblSellValue.AutoSize = true;
			this.lblSellValue.DefaultColor = System.Drawing.Color.White;
			this.lblSellValue.DefaultText = "0";
			this.lblSellValue.DisplayFactor = 1;
			this.lblSellValue.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblSellValue.DownColor = System.Drawing.Color.Red;
			this.lblSellValue.ForeColor = System.Drawing.Color.Red;
			this.lblSellValue.Is64Th = false;
			this.lblSellValue.IsCurrency = true;
			this.lblSellValue.IsFraction = false;
			this.lblSellValue.IsPercent = false;
			this.lblSellValue.Location = new System.Drawing.Point(329, 0);
			this.lblSellValue.MaxDecimal = 0;
			this.lblSellValue.Name = "lblSellValue";
			this.lblSellValue.Padding = new System.Windows.Forms.Padding(2);
			this.lblSellValue.Size = new System.Drawing.Size(17, 17);
			this.lblSellValue.TabIndex = 6;
			this.lblSellValue.Text = "0";
			this.lblSellValue.TickSize = 0.01;
			this.lblSellValue.UpColor = System.Drawing.Color.LimeGreen;
			this.lblSellValue.Value = "";
			// 
			// lblSellQty
			// 
			this.lblSellQty.AutoColor = false;
			this.lblSellQty.AutoSize = true;
			this.lblSellQty.DefaultColor = System.Drawing.Color.White;
			this.lblSellQty.DefaultText = "0";
			this.lblSellQty.DisplayFactor = 1;
			this.lblSellQty.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblSellQty.DownColor = System.Drawing.Color.Red;
			this.lblSellQty.ForeColor = System.Drawing.Color.Red;
			this.lblSellQty.Is64Th = false;
			this.lblSellQty.IsCurrency = false;
			this.lblSellQty.IsFraction = false;
			this.lblSellQty.IsPercent = false;
			this.lblSellQty.Location = new System.Drawing.Point(312, 0);
			this.lblSellQty.MaxDecimal = 0;
			this.lblSellQty.Name = "lblSellQty";
			this.lblSellQty.Padding = new System.Windows.Forms.Padding(2);
			this.lblSellQty.Size = new System.Drawing.Size(17, 17);
			this.lblSellQty.TabIndex = 13;
			this.lblSellQty.Text = "0";
			this.lblSellQty.TickSize = 0.01;
			this.lblSellQty.UpColor = System.Drawing.Color.LimeGreen;
			this.lblSellQty.Value = "";
			// 
			// dspSellValue
			// 
			this.dspSellValue.AutoColor = false;
			this.dspSellValue.AutoSize = true;
			this.dspSellValue.DefaultColor = System.Drawing.Color.White;
			this.dspSellValue.DefaultText = "SELL";
			this.dspSellValue.DisplayFactor = 1;
			this.dspSellValue.Dock = System.Windows.Forms.DockStyle.Left;
			this.dspSellValue.DownColor = System.Drawing.Color.Red;
			this.dspSellValue.ForeColor = System.Drawing.Color.Red;
			this.dspSellValue.Is64Th = false;
			this.dspSellValue.IsCurrency = false;
			this.dspSellValue.IsFraction = false;
			this.dspSellValue.IsPercent = false;
			this.dspSellValue.Location = new System.Drawing.Point(275, 0);
			this.dspSellValue.MaxDecimal = 7;
			this.dspSellValue.Name = "dspSellValue";
			this.dspSellValue.Padding = new System.Windows.Forms.Padding(2);
			this.dspSellValue.Size = new System.Drawing.Size(37, 17);
			this.dspSellValue.TabIndex = 5;
			this.dspSellValue.Text = "SELL";
			this.dspSellValue.TickSize = 0.01;
			this.dspSellValue.UpColor = System.Drawing.Color.LimeGreen;
			this.dspSellValue.Value = "";
			// 
			// lblShortValue
			// 
			this.lblShortValue.AutoColor = false;
			this.lblShortValue.AutoSize = true;
			this.lblShortValue.DefaultColor = System.Drawing.Color.White;
			this.lblShortValue.DefaultText = "0";
			this.lblShortValue.DisplayFactor = 1;
			this.lblShortValue.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblShortValue.DownColor = System.Drawing.Color.Red;
			this.lblShortValue.ForeColor = System.Drawing.Color.Magenta;
			this.lblShortValue.Is64Th = false;
			this.lblShortValue.IsCurrency = true;
			this.lblShortValue.IsFraction = false;
			this.lblShortValue.IsPercent = false;
			this.lblShortValue.Location = new System.Drawing.Point(258, 0);
			this.lblShortValue.MaxDecimal = 0;
			this.lblShortValue.Name = "lblShortValue";
			this.lblShortValue.Padding = new System.Windows.Forms.Padding(2);
			this.lblShortValue.Size = new System.Drawing.Size(17, 17);
			this.lblShortValue.TabIndex = 11;
			this.lblShortValue.Text = "0";
			this.lblShortValue.TickSize = 0.01;
			this.lblShortValue.UpColor = System.Drawing.Color.LimeGreen;
			this.lblShortValue.Value = "";
			// 
			// lblShortQty
			// 
			this.lblShortQty.AutoColor = false;
			this.lblShortQty.AutoSize = true;
			this.lblShortQty.DefaultColor = System.Drawing.Color.White;
			this.lblShortQty.DefaultText = "0";
			this.lblShortQty.DisplayFactor = 1;
			this.lblShortQty.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblShortQty.DownColor = System.Drawing.Color.Red;
			this.lblShortQty.ForeColor = System.Drawing.Color.Magenta;
			this.lblShortQty.Is64Th = false;
			this.lblShortQty.IsCurrency = false;
			this.lblShortQty.IsFraction = false;
			this.lblShortQty.IsPercent = false;
			this.lblShortQty.Location = new System.Drawing.Point(241, 0);
			this.lblShortQty.MaxDecimal = 0;
			this.lblShortQty.Name = "lblShortQty";
			this.lblShortQty.Padding = new System.Windows.Forms.Padding(2);
			this.lblShortQty.Size = new System.Drawing.Size(17, 17);
			this.lblShortQty.TabIndex = 14;
			this.lblShortQty.Text = "0";
			this.lblShortQty.TickSize = 0.01;
			this.lblShortQty.UpColor = System.Drawing.Color.LimeGreen;
			this.lblShortQty.Value = "";
			// 
			// dspShortValue
			// 
			this.dspShortValue.AutoColor = false;
			this.dspShortValue.AutoSize = true;
			this.dspShortValue.DefaultColor = System.Drawing.Color.White;
			this.dspShortValue.DefaultText = "SHORT";
			this.dspShortValue.DisplayFactor = 1;
			this.dspShortValue.Dock = System.Windows.Forms.DockStyle.Left;
			this.dspShortValue.DownColor = System.Drawing.Color.Red;
			this.dspShortValue.ForeColor = System.Drawing.Color.Magenta;
			this.dspShortValue.Is64Th = false;
			this.dspShortValue.IsCurrency = false;
			this.dspShortValue.IsFraction = false;
			this.dspShortValue.IsPercent = false;
			this.dspShortValue.Location = new System.Drawing.Point(192, 0);
			this.dspShortValue.MaxDecimal = 7;
			this.dspShortValue.Name = "dspShortValue";
			this.dspShortValue.Padding = new System.Windows.Forms.Padding(2);
			this.dspShortValue.Size = new System.Drawing.Size(49, 17);
			this.dspShortValue.TabIndex = 10;
			this.dspShortValue.Text = "SHORT";
			this.dspShortValue.TickSize = 0.01;
			this.dspShortValue.UpColor = System.Drawing.Color.LimeGreen;
			this.dspShortValue.Value = "";
			// 
			// lblBuyValue
			// 
			this.lblBuyValue.AutoColor = false;
			this.lblBuyValue.AutoSize = true;
			this.lblBuyValue.DefaultColor = System.Drawing.Color.White;
			this.lblBuyValue.DefaultText = "0";
			this.lblBuyValue.DisplayFactor = 1;
			this.lblBuyValue.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblBuyValue.DownColor = System.Drawing.Color.Red;
			this.lblBuyValue.ForeColor = System.Drawing.Color.LimeGreen;
			this.lblBuyValue.Is64Th = false;
			this.lblBuyValue.IsCurrency = true;
			this.lblBuyValue.IsFraction = false;
			this.lblBuyValue.IsPercent = false;
			this.lblBuyValue.Location = new System.Drawing.Point(175, 0);
			this.lblBuyValue.MaxDecimal = 0;
			this.lblBuyValue.Name = "lblBuyValue";
			this.lblBuyValue.Padding = new System.Windows.Forms.Padding(2);
			this.lblBuyValue.Size = new System.Drawing.Size(17, 17);
			this.lblBuyValue.TabIndex = 3;
			this.lblBuyValue.Text = "0";
			this.lblBuyValue.TickSize = 0.01;
			this.lblBuyValue.UpColor = System.Drawing.Color.LimeGreen;
			this.lblBuyValue.Value = "";
			// 
			// lblBuyQty
			// 
			this.lblBuyQty.AutoColor = false;
			this.lblBuyQty.AutoSize = true;
			this.lblBuyQty.DefaultColor = System.Drawing.Color.White;
			this.lblBuyQty.DefaultText = "0";
			this.lblBuyQty.DisplayFactor = 1;
			this.lblBuyQty.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblBuyQty.DownColor = System.Drawing.Color.Red;
			this.lblBuyQty.ForeColor = System.Drawing.Color.LimeGreen;
			this.lblBuyQty.Is64Th = false;
			this.lblBuyQty.IsCurrency = false;
			this.lblBuyQty.IsFraction = false;
			this.lblBuyQty.IsPercent = false;
			this.lblBuyQty.Location = new System.Drawing.Point(158, 0);
			this.lblBuyQty.MaxDecimal = 0;
			this.lblBuyQty.Name = "lblBuyQty";
			this.lblBuyQty.Padding = new System.Windows.Forms.Padding(2);
			this.lblBuyQty.Size = new System.Drawing.Size(17, 17);
			this.lblBuyQty.TabIndex = 12;
			this.lblBuyQty.Text = "0";
			this.lblBuyQty.TickSize = 0.01;
			this.lblBuyQty.UpColor = System.Drawing.Color.LimeGreen;
			this.lblBuyQty.Value = "";
			// 
			// dspBuyValue
			// 
			this.dspBuyValue.AutoColor = false;
			this.dspBuyValue.AutoSize = true;
			this.dspBuyValue.DefaultColor = System.Drawing.Color.White;
			this.dspBuyValue.DefaultText = "BUY";
			this.dspBuyValue.DisplayFactor = 1;
			this.dspBuyValue.Dock = System.Windows.Forms.DockStyle.Left;
			this.dspBuyValue.DownColor = System.Drawing.Color.Red;
			this.dspBuyValue.ForeColor = System.Drawing.Color.LimeGreen;
			this.dspBuyValue.Is64Th = false;
			this.dspBuyValue.IsCurrency = false;
			this.dspBuyValue.IsFraction = false;
			this.dspBuyValue.IsPercent = false;
			this.dspBuyValue.Location = new System.Drawing.Point(125, 0);
			this.dspBuyValue.MaxDecimal = 7;
			this.dspBuyValue.MinimumSize = new System.Drawing.Size(17, 17);
			this.dspBuyValue.Name = "dspBuyValue";
			this.dspBuyValue.Padding = new System.Windows.Forms.Padding(2);
			this.dspBuyValue.Size = new System.Drawing.Size(33, 17);
			this.dspBuyValue.TabIndex = 2;
			this.dspBuyValue.Text = "BUY";
			this.dspBuyValue.TickSize = 0.01;
			this.dspBuyValue.UpColor = System.Drawing.Color.LimeGreen;
			this.dspBuyValue.Value = "";
			// 
			// dspVAL
			// 
			this.dspVAL.AutoColor = false;
			this.dspVAL.AutoSize = true;
			this.dspVAL.DefaultColor = System.Drawing.Color.White;
			this.dspVAL.DefaultText = "Buy";
			this.dspVAL.DisplayFactor = 1;
			this.dspVAL.Dock = System.Windows.Forms.DockStyle.Left;
			this.dspVAL.DownColor = System.Drawing.Color.Red;
			this.dspVAL.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspVAL.ForeColor = System.Drawing.Color.DarkGray;
			this.dspVAL.Is64Th = false;
			this.dspVAL.IsCurrency = false;
			this.dspVAL.IsFraction = false;
			this.dspVAL.IsPercent = false;
			this.dspVAL.Location = new System.Drawing.Point(94, 0);
			this.dspVAL.MaxDecimal = 7;
			this.dspVAL.Name = "dspVAL";
			this.dspVAL.Padding = new System.Windows.Forms.Padding(2);
			this.dspVAL.Size = new System.Drawing.Size(31, 17);
			this.dspVAL.TabIndex = 16;
			this.dspVAL.Text = "VAL";
			this.dspVAL.TickSize = 0.01;
			this.dspVAL.UpColor = System.Drawing.Color.LimeGreen;
			this.dspVAL.Value = "";
			// 
			// dspQTY
			// 
			this.dspQTY.AutoColor = false;
			this.dspQTY.AutoSize = true;
			this.dspQTY.DefaultColor = System.Drawing.Color.White;
			this.dspQTY.DefaultText = "Buy";
			this.dspQTY.DisplayFactor = 1;
			this.dspQTY.Dock = System.Windows.Forms.DockStyle.Left;
			this.dspQTY.DownColor = System.Drawing.Color.Red;
			this.dspQTY.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspQTY.ForeColor = System.Drawing.Color.DarkGray;
			this.dspQTY.Is64Th = false;
			this.dspQTY.IsCurrency = false;
			this.dspQTY.IsFraction = false;
			this.dspQTY.IsPercent = false;
			this.dspQTY.Location = new System.Drawing.Point(61, 0);
			this.dspQTY.MaxDecimal = 7;
			this.dspQTY.Name = "dspQTY";
			this.dspQTY.Padding = new System.Windows.Forms.Padding(2);
			this.dspQTY.Size = new System.Drawing.Size(33, 17);
			this.dspQTY.TabIndex = 17;
			this.dspQTY.Text = "QTY";
			this.dspQTY.TickSize = 0.01;
			this.dspQTY.UpColor = System.Drawing.Color.LimeGreen;
			this.dspQTY.Value = "";
			// 
			// lblTradesQty
			// 
			this.lblTradesQty.AutoColor = false;
			this.lblTradesQty.AutoSize = true;
			this.lblTradesQty.DefaultColor = System.Drawing.Color.White;
			this.lblTradesQty.DefaultText = "0";
			this.lblTradesQty.DisplayFactor = 1;
			this.lblTradesQty.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblTradesQty.DownColor = System.Drawing.Color.Red;
			this.lblTradesQty.Is64Th = false;
			this.lblTradesQty.IsCurrency = false;
			this.lblTradesQty.IsFraction = false;
			this.lblTradesQty.IsPercent = false;
			this.lblTradesQty.Location = new System.Drawing.Point(44, 0);
			this.lblTradesQty.MaxDecimal = 0;
			this.lblTradesQty.MinimumSize = new System.Drawing.Size(17, 17);
			this.lblTradesQty.Name = "lblTradesQty";
			this.lblTradesQty.Padding = new System.Windows.Forms.Padding(2);
			this.lblTradesQty.Size = new System.Drawing.Size(17, 17);
			this.lblTradesQty.TabIndex = 1;
			this.lblTradesQty.Text = "0";
			this.lblTradesQty.TickSize = 0.01;
			this.lblTradesQty.UpColor = System.Drawing.Color.LimeGreen;
			this.lblTradesQty.Value = "";
			// 
			// dspTradesQty
			// 
			this.dspTradesQty.ApplyShine = true;
			this.dspTradesQty.AutoColor = false;
			this.dspTradesQty.AutoSize = true;
			this.dspTradesQty.DefaultColor = System.Drawing.Color.White;
			this.dspTradesQty.DefaultText = "Trades";
			this.dspTradesQty.DisplayFactor = 1;
			this.dspTradesQty.Dock = System.Windows.Forms.DockStyle.Left;
			this.dspTradesQty.DownColor = System.Drawing.Color.Red;
			this.dspTradesQty.Is64Th = false;
			this.dspTradesQty.IsCurrency = false;
			this.dspTradesQty.IsFraction = false;
			this.dspTradesQty.IsPercent = false;
			this.dspTradesQty.Location = new System.Drawing.Point(0, 0);
			this.dspTradesQty.MaxDecimal = 7;
			this.dspTradesQty.MinimumSize = new System.Drawing.Size(17, 17);
			this.dspTradesQty.Name = "dspTradesQty";
			this.dspTradesQty.Padding = new System.Windows.Forms.Padding(2);
			this.dspTradesQty.Size = new System.Drawing.Size(44, 17);
			this.dspTradesQty.TabIndex = 0;
			this.dspTradesQty.Text = "Trades";
			this.dspTradesQty.TickSize = 0.01;
			this.dspTradesQty.UpColor = System.Drawing.Color.LimeGreen;
			this.dspTradesQty.Value = "";
			// 
			// CaptionBar
			// 
			this.CaptionBar.BackColor = System.Drawing.Color.Transparent;
			this.CaptionBar.Dock = System.Windows.Forms.DockStyle.Top;
			this.CaptionBar.EnableCaptionEdit = true;
			this.CaptionBar.IconIndex = 8;
			this.CaptionBar.Location = new System.Drawing.Point(0, 0);
			this.CaptionBar.Name = "CaptionBar";
			this.CaptionBar.ShowDisplay = true;
			this.CaptionBar.ShowFit = true;
			this.CaptionBar.ShowRefresh = true;
			this.CaptionBar.ShowShowAll = true;
			this.CaptionBar.Size = new System.Drawing.Size(642, 26);
			this.CaptionBar.TabIndex = 0;
			// 
			// panelPositionsCommands
			// 
			this.panelPositionsCommands.BackColor = System.Drawing.Color.Black;
			this.panelPositionsCommands.Controls.Add(this.txtSymbolFilter);
			this.panelPositionsCommands.Controls.Add(this.lblSymbols);
			this.panelPositionsCommands.Controls.Add(this.cmdSecurities);
			this.panelPositionsCommands.Controls.Add(this.cmdSources);
			this.panelPositionsCommands.Dock = System.Windows.Forms.DockStyle.Top;
			this.panelPositionsCommands.Location = new System.Drawing.Point(0, 26);
			this.panelPositionsCommands.Name = "panelPositionsCommands";
			this.panelPositionsCommands.Size = new System.Drawing.Size(642, 24);
			this.panelPositionsCommands.TabIndex = 11;
			// 
			// txtSymbolFilter
			// 
			this.txtSymbolFilter.BackColor = System.Drawing.SystemColors.ControlDarkDark;
			this.txtSymbolFilter.Dock = System.Windows.Forms.DockStyle.Fill;
			this.txtSymbolFilter.Font = new System.Drawing.Font("Microsoft Sans Serif", 11.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.txtSymbolFilter.ForeColor = System.Drawing.Color.GreenYellow;
			this.txtSymbolFilter.Location = new System.Drawing.Point(226, 0);
			this.txtSymbolFilter.Name = "txtSymbolFilter";
			this.txtSymbolFilter.Size = new System.Drawing.Size(416, 24);
			this.txtSymbolFilter.TabIndex = 7;
			this.txtSymbolFilter.Visible = false;
			this.txtSymbolFilter.KeyDown += new System.Windows.Forms.KeyEventHandler(this.txtSymbolFilter_KeyDown);
			// 
			// lblSymbols
			// 
			this.lblSymbols.AutoColor = false;
			this.lblSymbols.AutoSize = true;
			this.lblSymbols.BackColor = System.Drawing.Color.ForestGreen;
			this.lblSymbols.DefaultColor = System.Drawing.Color.White;
			this.lblSymbols.DefaultText = "";
			this.lblSymbols.DisplayFactor = 1;
			this.lblSymbols.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblSymbols.DownColor = System.Drawing.Color.Red;
			this.lblSymbols.Is64Th = false;
			this.lblSymbols.IsCurrency = false;
			this.lblSymbols.IsFraction = false;
			this.lblSymbols.IsPercent = false;
			this.lblSymbols.Location = new System.Drawing.Point(157, 0);
			this.lblSymbols.MaxDecimal = 7;
			this.lblSymbols.MinimumSize = new System.Drawing.Size(69, 24);
			this.lblSymbols.Name = "lblSymbols";
			this.lblSymbols.Padding = new System.Windows.Forms.Padding(2, 0, 0, 0);
			this.lblSymbols.Size = new System.Drawing.Size(69, 24);
			this.lblSymbols.TabIndex = 20;
			this.lblSymbols.Text = "Show Only :";
			this.lblSymbols.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			this.lblSymbols.TickSize = 0.01;
			this.lblSymbols.UpColor = System.Drawing.Color.LimeGreen;
			this.lblSymbols.Value = "";
			// 
			// cmdSecurities
			// 
			this.cmdSecurities.AutoSize = true;
			this.cmdSecurities.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdSecurities.BackColor = System.Drawing.Color.Black;
			this.cmdSecurities.Dock = System.Windows.Forms.DockStyle.Left;
			this.cmdSecurities.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdSecurities.Image = global::ROC.Properties.Resources.Type_Filter;
			this.cmdSecurities.ImageAlign = System.Drawing.ContentAlignment.TopLeft;
			this.cmdSecurities.Location = new System.Drawing.Point(75, 0);
			this.cmdSecurities.MinimumSize = new System.Drawing.Size(26, 18);
			this.cmdSecurities.Name = "cmdSecurities";
			this.cmdSecurities.ROCButtonType = ButtonEx.VistaButton.ROCButtonTypes.FilterIn;
			this.cmdSecurities.Size = new System.Drawing.Size(82, 24);
			this.cmdSecurities.TabIndex = 18;
			this.cmdSecurities.TabStop = false;
			this.cmdSecurities.Text = "Securities:";
			this.cmdSecurities.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
			this.cmdSecurities.UserSplitSize = 20;
			this.cmdSecurities.MouseSplitClick += new System.EventHandler(this.cmdSecurities_Click);
			this.cmdSecurities.MouseSplitDown += new System.EventHandler(this.cmdSecurities_Click);
			this.cmdSecurities.Click += new System.EventHandler(this.cmdSecurities_Click);
			// 
			// cmdSources
			// 
			this.cmdSources.AutoSize = true;
			this.cmdSources.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdSources.BackColor = System.Drawing.Color.Black;
			this.cmdSources.Dock = System.Windows.Forms.DockStyle.Left;
			this.cmdSources.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdSources.Image = global::ROC.Properties.Resources.Filter2;
			this.cmdSources.ImageAlign = System.Drawing.ContentAlignment.TopLeft;
			this.cmdSources.Location = new System.Drawing.Point(0, 0);
			this.cmdSources.MinimumSize = new System.Drawing.Size(26, 18);
			this.cmdSources.Name = "cmdSources";
			this.cmdSources.ROCButtonType = ButtonEx.VistaButton.ROCButtonTypes.FilterIn;
			this.cmdSources.Size = new System.Drawing.Size(75, 24);
			this.cmdSources.TabIndex = 17;
			this.cmdSources.TabStop = false;
			this.cmdSources.Text = "Sources:";
			this.cmdSources.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
			this.cmdSources.UserSplitSize = 20;
			this.cmdSources.MouseSplitEnter += new System.EventHandler(this.cmdSources_Click);
			this.cmdSources.MouseSplitClick += new System.EventHandler(this.cmdSources_Click);
			this.cmdSources.Click += new System.EventHandler(this.cmdSources_Click);
			// 
			// menuProcessing
			// 
			this.menuProcessing.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
			this.menuProcessing.BackColor = System.Drawing.Color.Transparent;
			this.menuProcessing.Caption = "000";
			this.menuProcessing.Location = new System.Drawing.Point(21, 148);
			this.menuProcessing.MaximumSize = new System.Drawing.Size(51, 51);
			this.menuProcessing.MinimumSize = new System.Drawing.Size(51, 51);
			this.menuProcessing.Name = "menuProcessing";
			this.menuProcessing.Size = new System.Drawing.Size(51, 51);
			this.menuProcessing.TabIndex = 10;
			this.menuProcessing.TabStop = false;
			this.menuProcessing.Visible = false;
			// 
			// panelPositionFilters
			// 
			this.panelPositionFilters.BackColor = System.Drawing.Color.Black;
			this.panelPositionFilters.Controls.Add(this.cmdAccounts);
			this.panelPositionFilters.Controls.Add(this.cmdShowAll);
			this.panelPositionFilters.Dock = System.Windows.Forms.DockStyle.Top;
			this.panelPositionFilters.Location = new System.Drawing.Point(0, 50);
			this.panelPositionFilters.Name = "panelPositionFilters";
			this.panelPositionFilters.Size = new System.Drawing.Size(642, 24);
			this.panelPositionFilters.TabIndex = 13;
			// 
			// cmdAccounts
			// 
			this.cmdAccounts.AutoSize = true;
			this.cmdAccounts.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdAccounts.BackColor = System.Drawing.Color.Black;
			this.cmdAccounts.Dock = System.Windows.Forms.DockStyle.Left;
			this.cmdAccounts.Image = global::ROC.Properties.Resources.Account;
			this.cmdAccounts.Location = new System.Drawing.Point(58, 0);
			this.cmdAccounts.Name = "cmdAccounts";
			this.cmdAccounts.ROCButtonType = ButtonEx.VistaButton.ROCButtonTypes.FilterOut;
			this.cmdAccounts.Size = new System.Drawing.Size(81, 24);
			this.cmdAccounts.TabIndex = 9;
			this.cmdAccounts.TabStop = false;
			this.cmdAccounts.Text = "Accounts:";
			this.cmdAccounts.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
			this.cmdAccounts.UserSplitSize = 20;
			this.cmdAccounts.MouseSplitClick += new System.EventHandler(this.cmdAccounts_Click);
			this.cmdAccounts.MouseSplitDown += new System.EventHandler(this.cmdAccounts_Click);
			this.cmdAccounts.Click += new System.EventHandler(this.cmdAccounts_Click);
			// 
			// cmdShowAll
			// 
			this.cmdShowAll.AutoSize = true;
			this.cmdShowAll.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdShowAll.BackColor = System.Drawing.Color.DarkGreen;
			this.cmdShowAll.Dock = System.Windows.Forms.DockStyle.Left;
			this.cmdShowAll.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdShowAll.ImageAlign = System.Drawing.ContentAlignment.TopLeft;
			this.cmdShowAll.Location = new System.Drawing.Point(0, 0);
			this.cmdShowAll.MinimumSize = new System.Drawing.Size(26, 18);
			this.cmdShowAll.Name = "cmdShowAll";
			this.cmdShowAll.ROCButtonType = ButtonEx.VistaButton.ROCButtonTypes.FilterIn;
			this.cmdShowAll.Size = new System.Drawing.Size(58, 24);
			this.cmdShowAll.TabIndex = 11;
			this.cmdShowAll.TabStop = false;
			this.cmdShowAll.Text = "Show All";
			this.cmdShowAll.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.cmdShowAll.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
			this.cmdShowAll.UserSplitSize = 20;
			this.cmdShowAll.Click += new System.EventHandler(this.cmdShowAll_Click);
			// 
			// frmTrades
			// 
			this.Animate = false;
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.Caption = "Trades";
			this.ClientSize = new System.Drawing.Size(642, 242);
			this.Controls.Add(this.menuProcessing);
			this.Controls.Add(this.rocTradesList);
			this.Controls.Add(this.panelPositionFilters);
			this.Controls.Add(this.panelPositionsCommands);
			this.Controls.Add(this.panelStatus);
			this.Controls.Add(this.CaptionBar);
			this.MinimumSize = new System.Drawing.Size(400, 250);
			this.Name = "frmTrades";
			this.ROCWindowType = FormEx.VistaWindowBase.ROCWindowTypes.Trades;
			this.ShowInTaskbar = false;
			this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
			((System.ComponentModel.ISupportInitialize)(this.rocTradesList)).EndInit();
			this.panelStatus.ResumeLayout(false);
			this.panelStatus.PerformLayout();
			this.panelPositionsCommands.ResumeLayout(false);
			this.panelPositionsCommands.PerformLayout();
			this.panelPositionFilters.ResumeLayout(false);
			this.panelPositionFilters.PerformLayout();
			this.ResumeLayout(false);

		}

		#endregion

		private DataGridViewEx.ROCTradeList rocTradesList;
		private PanelEx.VistaPanel panelStatus;
		private LabelEx.LabelVista lblNetValue;
		private LabelEx.LabelVista dspNetValue;
		private LabelEx.LabelVista lblSellValue;
		private LabelEx.LabelVista dspSellValue;
		private LabelEx.LabelVista lblBuyValue;
		private LabelEx.LabelVista dspBuyValue;
		private LabelEx.LabelVista lblTradesQty;
		private LabelEx.LabelVista dspTradesQty;
		private FormEx.VistaWindowCaptionBar CaptionBar;
		private menuBaseProcessing menuProcessing;
		private LabelEx.LabelVista lblShortValue;
		private LabelEx.LabelVista dspShortValue;
		private LabelEx.LabelVista lblSellQty;
		private LabelEx.LabelVista lblBuyQty;
		private LabelEx.LabelVista lblShortQty;
		private LabelEx.LabelVista lblNetQty;
		private PanelEx.VistaPanel panelPositionsCommands;
		private System.Windows.Forms.TextBox txtSymbolFilter;
		private PanelEx.VistaPanel panelPositionFilters;
		private ButtonEx.VistaButton cmdAccounts;
		private ButtonEx.VistaButton cmdSecurities;
		private ButtonEx.VistaButton cmdSources;
		private ButtonEx.VistaButton cmdShowAll;
		private LabelEx.LabelVista lblSymbols;
		private LabelEx.LabelVista dspVAL;
		private LabelEx.LabelVista dspQTY;
	}
}