namespace ROC
{
	partial class frmOrders
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
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(frmOrders));
			DataGridViewEx.DGVBasePrinter dgvBasePrinter1 = new DataGridViewEx.DGVBasePrinter();
			System.Drawing.StringFormat stringFormat1 = new System.Drawing.StringFormat();
			System.Drawing.StringFormat stringFormat2 = new System.Drawing.StringFormat();
			System.Drawing.StringFormat stringFormat3 = new System.Drawing.StringFormat();
			System.Drawing.StringFormat stringFormat4 = new System.Drawing.StringFormat();
			DataGridViewEx.ROCOrderList.RowSelected rowSelected1 = new DataGridViewEx.ROCOrderList.RowSelected();
			this.CaptionBar = new FormEx.VistaWindowCaptionBar();
			this.rocOrdersList = new DataGridViewEx.ROCOrderList();
			this.panelStatus = new PanelEx.VistaPanel();
			this.lblNet = new LabelEx.LabelVista();
			this.dspNet = new LabelEx.LabelVista();
			this.lblSellFilled = new LabelEx.LabelVista();
			this.lblSellOpened = new LabelEx.LabelVista();
			this.dspSell = new LabelEx.LabelVista();
			this.lblShortFilled = new LabelEx.LabelVista();
			this.lblShortOpened = new LabelEx.LabelVista();
			this.labelShort = new LabelEx.LabelVista();
			this.lblBuyFilled = new LabelEx.LabelVista();
			this.lblBuyOpened = new LabelEx.LabelVista();
			this.dspBuy = new LabelEx.LabelVista();
			this.dspEXC = new LabelEx.LabelVista();
			this.dspWRK = new LabelEx.LabelVista();
			this.lblOrdersQty = new LabelEx.LabelVista();
			this.dspOrdersQty = new LabelEx.LabelVista();
			this.panelOrderCommands = new PanelEx.VistaPanel();
			this.txtSymbolFilter = new System.Windows.Forms.TextBox();
			this.lblSymbols = new LabelEx.LabelVista();
			this.cmdTIF = new ButtonEx.VistaButton();
			this.cmdOrderType = new ButtonEx.VistaButton();
			this.cmdSecurities = new ButtonEx.VistaButton();
			this.cmdStatus = new ButtonEx.VistaButton();
			this.panelPositionFilters = new PanelEx.VistaPanel();
			this.cmdAccounts = new ButtonEx.VistaButton();
			this.cmdShowAll = new ButtonEx.VistaButton();
			this.menuProcessing = new ROC.menuBaseProcessing();
			this.cmdCancelAll = new ButtonEx.VistaButton();
			((System.ComponentModel.ISupportInitialize)(this.rocOrdersList)).BeginInit();
			this.panelStatus.SuspendLayout();
			this.panelOrderCommands.SuspendLayout();
			this.panelPositionFilters.SuspendLayout();
			this.SuspendLayout();
			// 
			// CaptionBar
			// 
			this.CaptionBar.BackColor = System.Drawing.Color.Transparent;
			this.CaptionBar.Dock = System.Windows.Forms.DockStyle.Top;
			this.CaptionBar.EnableCaptionEdit = true;
			this.CaptionBar.IconIndex = 7;
			this.CaptionBar.Location = new System.Drawing.Point(0, 0);
			this.CaptionBar.Name = "CaptionBar";
			this.CaptionBar.ShowDisplay = true;
			this.CaptionBar.ShowFit = true;
			this.CaptionBar.ShowRefresh = true;
			this.CaptionBar.ShowShowAll = true;
			this.CaptionBar.Size = new System.Drawing.Size(642, 26);
			this.CaptionBar.TabIndex = 0;
			// 
			// rocOrdersList
			// 
			this.rocOrdersList.AllowUserToOrderColumns = true;
			this.rocOrdersList.BlueDefault = 228;
			this.rocOrdersList.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
			new BindingListEx.ROCBindingList<string>().Add("");
			this.rocOrdersList.DisplayFactors = ((System.Collections.Generic.Dictionary<string, double>)(resources.GetObject("rocOrdersList.DisplayFactors")));
			this.rocOrdersList.Dock = System.Windows.Forms.DockStyle.Fill;
			this.rocOrdersList.GreenDefault = 243;
			this.rocOrdersList.GridKeys = ((System.Collections.Generic.List<string>)(resources.GetObject("rocOrdersList.GridKeys")));
			this.rocOrdersList.HeaderBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(233)))), ((int)(((byte)(243)))), ((int)(((byte)(228)))));
			this.rocOrdersList.Importing = false;
			this.rocOrdersList.LastFilter = "";
			this.rocOrdersList.LastSort = "[OmTime] asc";
			this.rocOrdersList.Location = new System.Drawing.Point(0, 74);
			this.rocOrdersList.Name = "rocOrdersList";
			this.rocOrdersList.OrderStatuses = ((System.Collections.Generic.Dictionary<string, long>)(resources.GetObject("rocOrdersList.OrderStatuses")));
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
			this.rocOrdersList.Printer = dgvBasePrinter1;
			this.rocOrdersList.Processing = false;
			this.rocOrdersList.RedDefault = 233;
			this.rocOrdersList.RefreshAggragation = true;
			this.rocOrdersList.RefreshSharedRows = false;
			this.rocOrdersList.ShouldScrollToLastRow = false;
			this.rocOrdersList.ShowActionMenu = false;
			this.rocOrdersList.ShowColumnMenu = false;
			this.rocOrdersList.ShowMenu = false;
			this.rocOrdersList.ShowMenuSymbol = rowSelected1;
			this.rocOrdersList.Size = new System.Drawing.Size(642, 151);
			new BindingListEx.ROCBindingList<string>().Add("");
			this.rocOrdersList.TabIndex = 7;
			this.rocOrdersList.TickSizes = ((System.Collections.Generic.Dictionary<string, double>)(resources.GetObject("rocOrdersList.TickSizes")));
			// 
			// panelStatus
			// 
			this.panelStatus.BackColor = System.Drawing.Color.Black;
			this.panelStatus.Controls.Add(this.lblNet);
			this.panelStatus.Controls.Add(this.dspNet);
			this.panelStatus.Controls.Add(this.lblSellFilled);
			this.panelStatus.Controls.Add(this.lblSellOpened);
			this.panelStatus.Controls.Add(this.dspSell);
			this.panelStatus.Controls.Add(this.lblShortFilled);
			this.panelStatus.Controls.Add(this.lblShortOpened);
			this.panelStatus.Controls.Add(this.labelShort);
			this.panelStatus.Controls.Add(this.lblBuyFilled);
			this.panelStatus.Controls.Add(this.lblBuyOpened);
			this.panelStatus.Controls.Add(this.dspBuy);
			this.panelStatus.Controls.Add(this.dspEXC);
			this.panelStatus.Controls.Add(this.dspWRK);
			this.panelStatus.Controls.Add(this.lblOrdersQty);
			this.panelStatus.Controls.Add(this.dspOrdersQty);
			this.panelStatus.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.panelStatus.Location = new System.Drawing.Point(0, 225);
			this.panelStatus.Name = "panelStatus";
			this.panelStatus.Size = new System.Drawing.Size(642, 17);
			this.panelStatus.TabIndex = 8;
			// 
			// lblNet
			// 
			this.lblNet.AutoColor = true;
			this.lblNet.AutoSize = true;
			this.lblNet.DefaultColor = System.Drawing.Color.White;
			this.lblNet.DefaultText = "0";
			this.lblNet.DisplayFactor = 1;
			this.lblNet.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblNet.DownColor = System.Drawing.Color.Red;
			this.lblNet.Is64Th = false;
			this.lblNet.IsCurrency = false;
			this.lblNet.IsFraction = false;
			this.lblNet.IsPercent = false;
			this.lblNet.Location = new System.Drawing.Point(382, 0);
			this.lblNet.MaxDecimal = 0;
			this.lblNet.Name = "lblNet";
			this.lblNet.Padding = new System.Windows.Forms.Padding(2);
			this.lblNet.Size = new System.Drawing.Size(17, 17);
			this.lblNet.TabIndex = 19;
			this.lblNet.Text = "0";
			this.lblNet.TickSize = 0.01;
			this.lblNet.UpColor = System.Drawing.Color.LimeGreen;
			this.lblNet.Value = "";
			// 
			// dspNet
			// 
			this.dspNet.AutoColor = false;
			this.dspNet.AutoSize = true;
			this.dspNet.DefaultColor = System.Drawing.Color.White;
			this.dspNet.DefaultText = "NET";
			this.dspNet.DisplayFactor = 1;
			this.dspNet.Dock = System.Windows.Forms.DockStyle.Left;
			this.dspNet.DownColor = System.Drawing.Color.Red;
			this.dspNet.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspNet.Is64Th = false;
			this.dspNet.IsCurrency = false;
			this.dspNet.IsFraction = false;
			this.dspNet.IsPercent = false;
			this.dspNet.Location = new System.Drawing.Point(349, 0);
			this.dspNet.MaxDecimal = 7;
			this.dspNet.Name = "dspNet";
			this.dspNet.Padding = new System.Windows.Forms.Padding(2);
			this.dspNet.Size = new System.Drawing.Size(33, 17);
			this.dspNet.TabIndex = 18;
			this.dspNet.Text = "NET";
			this.dspNet.TickSize = 0.01;
			this.dspNet.UpColor = System.Drawing.Color.LimeGreen;
			this.dspNet.Value = "";
			// 
			// lblSellFilled
			// 
			this.lblSellFilled.AutoColor = false;
			this.lblSellFilled.AutoSize = true;
			this.lblSellFilled.DefaultColor = System.Drawing.Color.White;
			this.lblSellFilled.DefaultText = "0";
			this.lblSellFilled.DisplayFactor = 1;
			this.lblSellFilled.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblSellFilled.DownColor = System.Drawing.Color.Red;
			this.lblSellFilled.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblSellFilled.ForeColor = System.Drawing.Color.Red;
			this.lblSellFilled.Is64Th = false;
			this.lblSellFilled.IsCurrency = false;
			this.lblSellFilled.IsFraction = false;
			this.lblSellFilled.IsPercent = false;
			this.lblSellFilled.Location = new System.Drawing.Point(332, 0);
			this.lblSellFilled.MaxDecimal = 0;
			this.lblSellFilled.Name = "lblSellFilled";
			this.lblSellFilled.Padding = new System.Windows.Forms.Padding(2);
			this.lblSellFilled.Size = new System.Drawing.Size(17, 17);
			this.lblSellFilled.TabIndex = 6;
			this.lblSellFilled.Text = "0";
			this.lblSellFilled.TickSize = 0.01;
			this.lblSellFilled.UpColor = System.Drawing.Color.LimeGreen;
			this.lblSellFilled.Value = "";
			// 
			// lblSellOpened
			// 
			this.lblSellOpened.AutoColor = false;
			this.lblSellOpened.AutoSize = true;
			this.lblSellOpened.DefaultColor = System.Drawing.Color.White;
			this.lblSellOpened.DefaultText = "0";
			this.lblSellOpened.DisplayFactor = 1;
			this.lblSellOpened.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblSellOpened.DownColor = System.Drawing.Color.Red;
			this.lblSellOpened.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblSellOpened.ForeColor = System.Drawing.Color.Red;
			this.lblSellOpened.Is64Th = false;
			this.lblSellOpened.IsCurrency = false;
			this.lblSellOpened.IsFraction = false;
			this.lblSellOpened.IsPercent = false;
			this.lblSellOpened.Location = new System.Drawing.Point(315, 0);
			this.lblSellOpened.MaxDecimal = 0;
			this.lblSellOpened.Name = "lblSellOpened";
			this.lblSellOpened.Padding = new System.Windows.Forms.Padding(2);
			this.lblSellOpened.Size = new System.Drawing.Size(17, 17);
			this.lblSellOpened.TabIndex = 7;
			this.lblSellOpened.Text = "0";
			this.lblSellOpened.TickSize = 0.01;
			this.lblSellOpened.UpColor = System.Drawing.Color.LimeGreen;
			this.lblSellOpened.Value = "";
			// 
			// dspSell
			// 
			this.dspSell.AutoColor = false;
			this.dspSell.AutoSize = true;
			this.dspSell.DefaultColor = System.Drawing.Color.White;
			this.dspSell.DefaultText = "SELL";
			this.dspSell.DisplayFactor = 1;
			this.dspSell.Dock = System.Windows.Forms.DockStyle.Left;
			this.dspSell.DownColor = System.Drawing.Color.Red;
			this.dspSell.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspSell.ForeColor = System.Drawing.Color.Red;
			this.dspSell.Is64Th = false;
			this.dspSell.IsCurrency = false;
			this.dspSell.IsFraction = false;
			this.dspSell.IsPercent = false;
			this.dspSell.Location = new System.Drawing.Point(278, 0);
			this.dspSell.MaxDecimal = 7;
			this.dspSell.Name = "dspSell";
			this.dspSell.Padding = new System.Windows.Forms.Padding(2);
			this.dspSell.Size = new System.Drawing.Size(37, 17);
			this.dspSell.TabIndex = 17;
			this.dspSell.Text = "SELL";
			this.dspSell.TickSize = 0.01;
			this.dspSell.UpColor = System.Drawing.Color.LimeGreen;
			this.dspSell.Value = "";
			// 
			// lblShortFilled
			// 
			this.lblShortFilled.AutoColor = false;
			this.lblShortFilled.AutoSize = true;
			this.lblShortFilled.DefaultColor = System.Drawing.Color.White;
			this.lblShortFilled.DefaultText = "0";
			this.lblShortFilled.DisplayFactor = 1;
			this.lblShortFilled.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblShortFilled.DownColor = System.Drawing.Color.Red;
			this.lblShortFilled.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblShortFilled.ForeColor = System.Drawing.Color.Magenta;
			this.lblShortFilled.Is64Th = false;
			this.lblShortFilled.IsCurrency = false;
			this.lblShortFilled.IsFraction = false;
			this.lblShortFilled.IsPercent = false;
			this.lblShortFilled.Location = new System.Drawing.Point(261, 0);
			this.lblShortFilled.MaxDecimal = 0;
			this.lblShortFilled.Name = "lblShortFilled";
			this.lblShortFilled.Padding = new System.Windows.Forms.Padding(2);
			this.lblShortFilled.Size = new System.Drawing.Size(17, 17);
			this.lblShortFilled.TabIndex = 9;
			this.lblShortFilled.Text = "0";
			this.lblShortFilled.TickSize = 0.01;
			this.lblShortFilled.UpColor = System.Drawing.Color.LimeGreen;
			this.lblShortFilled.Value = "";
			// 
			// lblShortOpened
			// 
			this.lblShortOpened.AutoColor = false;
			this.lblShortOpened.AutoSize = true;
			this.lblShortOpened.DefaultColor = System.Drawing.Color.White;
			this.lblShortOpened.DefaultText = "0";
			this.lblShortOpened.DisplayFactor = 1;
			this.lblShortOpened.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblShortOpened.DownColor = System.Drawing.Color.Red;
			this.lblShortOpened.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblShortOpened.ForeColor = System.Drawing.Color.Magenta;
			this.lblShortOpened.Is64Th = false;
			this.lblShortOpened.IsCurrency = false;
			this.lblShortOpened.IsFraction = false;
			this.lblShortOpened.IsPercent = false;
			this.lblShortOpened.Location = new System.Drawing.Point(244, 0);
			this.lblShortOpened.MaxDecimal = 0;
			this.lblShortOpened.Name = "lblShortOpened";
			this.lblShortOpened.Padding = new System.Windows.Forms.Padding(2);
			this.lblShortOpened.Size = new System.Drawing.Size(17, 17);
			this.lblShortOpened.TabIndex = 10;
			this.lblShortOpened.Text = "0";
			this.lblShortOpened.TickSize = 0.01;
			this.lblShortOpened.UpColor = System.Drawing.Color.LimeGreen;
			this.lblShortOpened.Value = "";
			// 
			// labelShort
			// 
			this.labelShort.AutoColor = false;
			this.labelShort.AutoSize = true;
			this.labelShort.DefaultColor = System.Drawing.Color.White;
			this.labelShort.DefaultText = "SHORT";
			this.labelShort.DisplayFactor = 1;
			this.labelShort.Dock = System.Windows.Forms.DockStyle.Left;
			this.labelShort.DownColor = System.Drawing.Color.Red;
			this.labelShort.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.labelShort.ForeColor = System.Drawing.Color.Magenta;
			this.labelShort.Is64Th = false;
			this.labelShort.IsCurrency = false;
			this.labelShort.IsFraction = false;
			this.labelShort.IsPercent = false;
			this.labelShort.Location = new System.Drawing.Point(195, 0);
			this.labelShort.MaxDecimal = 7;
			this.labelShort.Name = "labelShort";
			this.labelShort.Padding = new System.Windows.Forms.Padding(2);
			this.labelShort.Size = new System.Drawing.Size(49, 17);
			this.labelShort.TabIndex = 16;
			this.labelShort.Text = "SHORT";
			this.labelShort.TickSize = 0.01;
			this.labelShort.UpColor = System.Drawing.Color.LimeGreen;
			this.labelShort.Value = "";
			// 
			// lblBuyFilled
			// 
			this.lblBuyFilled.AutoColor = false;
			this.lblBuyFilled.AutoSize = true;
			this.lblBuyFilled.DefaultColor = System.Drawing.Color.White;
			this.lblBuyFilled.DefaultText = "0";
			this.lblBuyFilled.DisplayFactor = 1;
			this.lblBuyFilled.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblBuyFilled.DownColor = System.Drawing.Color.Red;
			this.lblBuyFilled.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblBuyFilled.ForeColor = System.Drawing.Color.LimeGreen;
			this.lblBuyFilled.Is64Th = false;
			this.lblBuyFilled.IsCurrency = false;
			this.lblBuyFilled.IsFraction = false;
			this.lblBuyFilled.IsPercent = false;
			this.lblBuyFilled.Location = new System.Drawing.Point(178, 0);
			this.lblBuyFilled.MaxDecimal = 0;
			this.lblBuyFilled.Name = "lblBuyFilled";
			this.lblBuyFilled.Padding = new System.Windows.Forms.Padding(2);
			this.lblBuyFilled.Size = new System.Drawing.Size(17, 17);
			this.lblBuyFilled.TabIndex = 3;
			this.lblBuyFilled.Text = "0";
			this.lblBuyFilled.TickSize = 0.01;
			this.lblBuyFilled.UpColor = System.Drawing.Color.LimeGreen;
			this.lblBuyFilled.Value = "";
			// 
			// lblBuyOpened
			// 
			this.lblBuyOpened.AutoColor = false;
			this.lblBuyOpened.AutoSize = true;
			this.lblBuyOpened.DefaultColor = System.Drawing.Color.White;
			this.lblBuyOpened.DefaultText = "0";
			this.lblBuyOpened.DisplayFactor = 1;
			this.lblBuyOpened.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblBuyOpened.DownColor = System.Drawing.Color.Red;
			this.lblBuyOpened.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblBuyOpened.ForeColor = System.Drawing.Color.LimeGreen;
			this.lblBuyOpened.Is64Th = false;
			this.lblBuyOpened.IsCurrency = false;
			this.lblBuyOpened.IsFraction = false;
			this.lblBuyOpened.IsPercent = false;
			this.lblBuyOpened.Location = new System.Drawing.Point(161, 0);
			this.lblBuyOpened.MaxDecimal = 0;
			this.lblBuyOpened.Name = "lblBuyOpened";
			this.lblBuyOpened.Padding = new System.Windows.Forms.Padding(2);
			this.lblBuyOpened.Size = new System.Drawing.Size(17, 17);
			this.lblBuyOpened.TabIndex = 4;
			this.lblBuyOpened.Text = "0";
			this.lblBuyOpened.TickSize = 0.01;
			this.lblBuyOpened.UpColor = System.Drawing.Color.LimeGreen;
			this.lblBuyOpened.Value = "";
			// 
			// dspBuy
			// 
			this.dspBuy.AutoColor = false;
			this.dspBuy.AutoSize = true;
			this.dspBuy.DefaultColor = System.Drawing.Color.White;
			this.dspBuy.DefaultText = "BUY";
			this.dspBuy.DisplayFactor = 1;
			this.dspBuy.Dock = System.Windows.Forms.DockStyle.Left;
			this.dspBuy.DownColor = System.Drawing.Color.Red;
			this.dspBuy.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspBuy.ForeColor = System.Drawing.Color.LimeGreen;
			this.dspBuy.Is64Th = false;
			this.dspBuy.IsCurrency = false;
			this.dspBuy.IsFraction = false;
			this.dspBuy.IsPercent = false;
			this.dspBuy.Location = new System.Drawing.Point(128, 0);
			this.dspBuy.MaxDecimal = 7;
			this.dspBuy.Name = "dspBuy";
			this.dspBuy.Padding = new System.Windows.Forms.Padding(2);
			this.dspBuy.Size = new System.Drawing.Size(33, 17);
			this.dspBuy.TabIndex = 15;
			this.dspBuy.Text = "BUY";
			this.dspBuy.TickSize = 0.01;
			this.dspBuy.UpColor = System.Drawing.Color.LimeGreen;
			this.dspBuy.Value = "";
			// 
			// dspEXC
			// 
			this.dspEXC.AutoColor = false;
			this.dspEXC.AutoSize = true;
			this.dspEXC.DefaultColor = System.Drawing.Color.White;
			this.dspEXC.DefaultText = "Buy";
			this.dspEXC.DisplayFactor = 1;
			this.dspEXC.Dock = System.Windows.Forms.DockStyle.Left;
			this.dspEXC.DownColor = System.Drawing.Color.Red;
			this.dspEXC.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspEXC.ForeColor = System.Drawing.Color.DarkGray;
			this.dspEXC.Is64Th = false;
			this.dspEXC.IsCurrency = false;
			this.dspEXC.IsFraction = false;
			this.dspEXC.IsPercent = false;
			this.dspEXC.Location = new System.Drawing.Point(96, 0);
			this.dspEXC.MaxDecimal = 7;
			this.dspEXC.Name = "dspEXC";
			this.dspEXC.Padding = new System.Windows.Forms.Padding(2);
			this.dspEXC.Size = new System.Drawing.Size(32, 17);
			this.dspEXC.TabIndex = 2;
			this.dspEXC.Text = "EXC";
			this.dspEXC.TickSize = 0.01;
			this.dspEXC.UpColor = System.Drawing.Color.LimeGreen;
			this.dspEXC.Value = "";
			// 
			// dspWRK
			// 
			this.dspWRK.AutoColor = false;
			this.dspWRK.AutoSize = true;
			this.dspWRK.DefaultColor = System.Drawing.Color.White;
			this.dspWRK.DefaultText = "Buy";
			this.dspWRK.DisplayFactor = 1;
			this.dspWRK.Dock = System.Windows.Forms.DockStyle.Left;
			this.dspWRK.DownColor = System.Drawing.Color.Red;
			this.dspWRK.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspWRK.ForeColor = System.Drawing.Color.DarkGray;
			this.dspWRK.Is64Th = false;
			this.dspWRK.IsCurrency = false;
			this.dspWRK.IsFraction = false;
			this.dspWRK.IsPercent = false;
			this.dspWRK.Location = new System.Drawing.Point(59, 0);
			this.dspWRK.MaxDecimal = 7;
			this.dspWRK.Name = "dspWRK";
			this.dspWRK.Padding = new System.Windows.Forms.Padding(2);
			this.dspWRK.Size = new System.Drawing.Size(37, 17);
			this.dspWRK.TabIndex = 14;
			this.dspWRK.Text = "WRK";
			this.dspWRK.TickSize = 0.01;
			this.dspWRK.UpColor = System.Drawing.Color.LimeGreen;
			this.dspWRK.Value = "";
			// 
			// lblOrdersQty
			// 
			this.lblOrdersQty.AutoColor = false;
			this.lblOrdersQty.AutoSize = true;
			this.lblOrdersQty.DefaultColor = System.Drawing.Color.White;
			this.lblOrdersQty.DefaultText = "0";
			this.lblOrdersQty.DisplayFactor = 1;
			this.lblOrdersQty.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblOrdersQty.DownColor = System.Drawing.Color.Red;
			this.lblOrdersQty.Is64Th = false;
			this.lblOrdersQty.IsCurrency = false;
			this.lblOrdersQty.IsFraction = false;
			this.lblOrdersQty.IsPercent = false;
			this.lblOrdersQty.Location = new System.Drawing.Point(42, 0);
			this.lblOrdersQty.MaxDecimal = 0;
			this.lblOrdersQty.Name = "lblOrdersQty";
			this.lblOrdersQty.Padding = new System.Windows.Forms.Padding(2);
			this.lblOrdersQty.Size = new System.Drawing.Size(17, 17);
			this.lblOrdersQty.TabIndex = 1;
			this.lblOrdersQty.Text = "0";
			this.lblOrdersQty.TickSize = 0.01;
			this.lblOrdersQty.UpColor = System.Drawing.Color.LimeGreen;
			this.lblOrdersQty.Value = "";
			// 
			// dspOrdersQty
			// 
			this.dspOrdersQty.ApplyShine = true;
			this.dspOrdersQty.AutoColor = false;
			this.dspOrdersQty.AutoSize = true;
			this.dspOrdersQty.DefaultColor = System.Drawing.Color.White;
			this.dspOrdersQty.DefaultText = "Orders";
			this.dspOrdersQty.DisplayFactor = 1;
			this.dspOrdersQty.Dock = System.Windows.Forms.DockStyle.Left;
			this.dspOrdersQty.DownColor = System.Drawing.Color.Red;
			this.dspOrdersQty.Is64Th = false;
			this.dspOrdersQty.IsCurrency = false;
			this.dspOrdersQty.IsFraction = false;
			this.dspOrdersQty.IsPercent = false;
			this.dspOrdersQty.Location = new System.Drawing.Point(0, 0);
			this.dspOrdersQty.MaxDecimal = 7;
			this.dspOrdersQty.Name = "dspOrdersQty";
			this.dspOrdersQty.Padding = new System.Windows.Forms.Padding(2);
			this.dspOrdersQty.Size = new System.Drawing.Size(42, 17);
			this.dspOrdersQty.TabIndex = 0;
			this.dspOrdersQty.Text = "Orders";
			this.dspOrdersQty.TickSize = 0.01;
			this.dspOrdersQty.UpColor = System.Drawing.Color.LimeGreen;
			this.dspOrdersQty.Value = "";
			// 
			// panelOrderCommands
			// 
			this.panelOrderCommands.BackColor = System.Drawing.Color.Black;
			this.panelOrderCommands.Controls.Add(this.txtSymbolFilter);
			this.panelOrderCommands.Controls.Add(this.lblSymbols);
			this.panelOrderCommands.Controls.Add(this.cmdTIF);
			this.panelOrderCommands.Controls.Add(this.cmdOrderType);
			this.panelOrderCommands.Controls.Add(this.cmdSecurities);
			this.panelOrderCommands.Controls.Add(this.cmdStatus);
			this.panelOrderCommands.Dock = System.Windows.Forms.DockStyle.Top;
			this.panelOrderCommands.Location = new System.Drawing.Point(0, 26);
			this.panelOrderCommands.Name = "panelOrderCommands";
			this.panelOrderCommands.Size = new System.Drawing.Size(642, 24);
			this.panelOrderCommands.TabIndex = 12;
			// 
			// txtSymbolFilter
			// 
			this.txtSymbolFilter.BackColor = System.Drawing.SystemColors.ControlDarkDark;
			this.txtSymbolFilter.Dock = System.Windows.Forms.DockStyle.Fill;
			this.txtSymbolFilter.Font = new System.Drawing.Font("Microsoft Sans Serif", 11.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.txtSymbolFilter.ForeColor = System.Drawing.Color.GreenYellow;
			this.txtSymbolFilter.Location = new System.Drawing.Point(355, 0);
			this.txtSymbolFilter.Name = "txtSymbolFilter";
			this.txtSymbolFilter.Size = new System.Drawing.Size(287, 24);
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
			this.lblSymbols.Location = new System.Drawing.Point(286, 0);
			this.lblSymbols.MaxDecimal = 7;
			this.lblSymbols.MinimumSize = new System.Drawing.Size(69, 24);
			this.lblSymbols.Name = "lblSymbols";
			this.lblSymbols.Padding = new System.Windows.Forms.Padding(2, 0, 0, 0);
			this.lblSymbols.Size = new System.Drawing.Size(69, 24);
			this.lblSymbols.TabIndex = 19;
			this.lblSymbols.Text = "Show Only :";
			this.lblSymbols.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			this.lblSymbols.TickSize = 0.01;
			this.lblSymbols.UpColor = System.Drawing.Color.LimeGreen;
			this.lblSymbols.Value = "";
			// 
			// cmdTIF
			// 
			this.cmdTIF.AutoSize = true;
			this.cmdTIF.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdTIF.BackColor = System.Drawing.Color.DarkRed;
			this.cmdTIF.Dock = System.Windows.Forms.DockStyle.Left;
			this.cmdTIF.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdTIF.Image = global::ROC.Properties.Resources.File;
			this.cmdTIF.ImageAlign = System.Drawing.ContentAlignment.TopLeft;
			this.cmdTIF.Location = new System.Drawing.Point(234, 0);
			this.cmdTIF.MinimumSize = new System.Drawing.Size(26, 18);
			this.cmdTIF.Name = "cmdTIF";
			this.cmdTIF.ROCButtonType = ButtonEx.VistaButton.ROCButtonTypes.FilterOut;
			this.cmdTIF.Size = new System.Drawing.Size(52, 24);
			this.cmdTIF.TabIndex = 22;
			this.cmdTIF.TabStop = false;
			this.cmdTIF.Text = "TIF:";
			this.cmdTIF.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.cmdTIF.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
			this.cmdTIF.UserSplitSize = 20;
			this.cmdTIF.MouseSplitClick += new System.EventHandler(this.cmdTIF_Click);
			this.cmdTIF.MouseSplitDown += new System.EventHandler(this.cmdTIF_Click);
			this.cmdTIF.Click += new System.EventHandler(this.cmdTIF_Click);
			// 
			// cmdOrderType
			// 
			this.cmdOrderType.AutoSize = true;
			this.cmdOrderType.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdOrderType.BackColor = System.Drawing.Color.DarkRed;
			this.cmdOrderType.Dock = System.Windows.Forms.DockStyle.Left;
			this.cmdOrderType.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdOrderType.Image = global::ROC.Properties.Resources.File;
			this.cmdOrderType.ImageAlign = System.Drawing.ContentAlignment.TopLeft;
			this.cmdOrderType.Location = new System.Drawing.Point(148, 0);
			this.cmdOrderType.MinimumSize = new System.Drawing.Size(26, 18);
			this.cmdOrderType.Name = "cmdOrderType";
			this.cmdOrderType.ROCButtonType = ButtonEx.VistaButton.ROCButtonTypes.FilterOut;
			this.cmdOrderType.Size = new System.Drawing.Size(86, 24);
			this.cmdOrderType.TabIndex = 21;
			this.cmdOrderType.TabStop = false;
			this.cmdOrderType.Text = "OrderType:";
			this.cmdOrderType.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.cmdOrderType.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
			this.cmdOrderType.UserSplitSize = 20;
			this.cmdOrderType.MouseSplitClick += new System.EventHandler(this.cmdOrderType_Click);
			this.cmdOrderType.MouseSplitDown += new System.EventHandler(this.cmdOrderType_Click);
			this.cmdOrderType.Click += new System.EventHandler(this.cmdOrderType_Click);
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
			this.cmdSecurities.Location = new System.Drawing.Point(66, 0);
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
			// cmdStatus
			// 
			this.cmdStatus.AutoSize = true;
			this.cmdStatus.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdStatus.BackColor = System.Drawing.Color.Black;
			this.cmdStatus.Dock = System.Windows.Forms.DockStyle.Left;
			this.cmdStatus.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdStatus.Image = global::ROC.Properties.Resources.S;
			this.cmdStatus.ImageAlign = System.Drawing.ContentAlignment.TopLeft;
			this.cmdStatus.Location = new System.Drawing.Point(0, 0);
			this.cmdStatus.MinimumSize = new System.Drawing.Size(26, 18);
			this.cmdStatus.Name = "cmdStatus";
			this.cmdStatus.ROCButtonType = ButtonEx.VistaButton.ROCButtonTypes.FilterIn;
			this.cmdStatus.Size = new System.Drawing.Size(66, 24);
			this.cmdStatus.TabIndex = 17;
			this.cmdStatus.TabStop = false;
			this.cmdStatus.Text = "Status:";
			this.cmdStatus.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
			this.cmdStatus.UserSplitSize = 20;
			this.cmdStatus.MouseSplitClick += new System.EventHandler(this.cmdStatus_Click);
			this.cmdStatus.MouseSplitDown += new System.EventHandler(this.cmdStatus_Click);
			this.cmdStatus.Click += new System.EventHandler(this.cmdStatus_Click);
			// 
			// panelPositionFilters
			// 
			this.panelPositionFilters.BackColor = System.Drawing.Color.Black;
			this.panelPositionFilters.Controls.Add(this.cmdCancelAll);
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
			this.cmdAccounts.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdAccounts.Image = global::ROC.Properties.Resources.Account;
			this.cmdAccounts.ImageAlign = System.Drawing.ContentAlignment.TopLeft;
			this.cmdAccounts.Location = new System.Drawing.Point(58, 0);
			this.cmdAccounts.MinimumSize = new System.Drawing.Size(26, 18);
			this.cmdAccounts.Name = "cmdAccounts";
			this.cmdAccounts.ROCButtonType = ButtonEx.VistaButton.ROCButtonTypes.FilterIn;
			this.cmdAccounts.Size = new System.Drawing.Size(81, 24);
			this.cmdAccounts.TabIndex = 8;
			this.cmdAccounts.TabStop = false;
			this.cmdAccounts.Text = "Accounts:";
			this.cmdAccounts.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
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
			this.cmdShowAll.TabIndex = 9;
			this.cmdShowAll.TabStop = false;
			this.cmdShowAll.Text = "Show All";
			this.cmdShowAll.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.cmdShowAll.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
			this.cmdShowAll.UserSplitSize = 20;
			this.cmdShowAll.Click += new System.EventHandler(this.cmdShowAll_Click);
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
			this.menuProcessing.TabIndex = 4;
			this.menuProcessing.TabStop = false;
			this.menuProcessing.Visible = false;
			// 
			// cmdCancelAll
			// 
			this.cmdCancelAll.AutoSize = true;
			this.cmdCancelAll.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdCancelAll.BackColor = System.Drawing.Color.DarkRed;
			this.cmdCancelAll.Dock = System.Windows.Forms.DockStyle.Right;
			this.cmdCancelAll.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdCancelAll.ImageAlign = System.Drawing.ContentAlignment.TopLeft;
			this.cmdCancelAll.Location = new System.Drawing.Point(578, 0);
			this.cmdCancelAll.MinimumSize = new System.Drawing.Size(26, 18);
			this.cmdCancelAll.Name = "cmdCancelAll";
			this.cmdCancelAll.ROCButtonType = ButtonEx.VistaButton.ROCButtonTypes.FilterIn;
			this.cmdCancelAll.Size = new System.Drawing.Size(64, 24);
			this.cmdCancelAll.TabIndex = 21;
			this.cmdCancelAll.TabStop = false;
			this.cmdCancelAll.Text = "Cancel All";
			this.cmdCancelAll.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.cmdCancelAll.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
			this.cmdCancelAll.UserSplitSize = 20;
			this.cmdCancelAll.Click += new System.EventHandler(this.cmdCancelAll_Click);
			// 
			// frmOrders
			// 
			this.Animate = false;
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.BackColor = System.Drawing.SystemColors.Control;
			this.Caption = "Orders";
			this.ClientSize = new System.Drawing.Size(642, 242);
			this.Controls.Add(this.menuProcessing);
			this.Controls.Add(this.rocOrdersList);
			this.Controls.Add(this.panelPositionFilters);
			this.Controls.Add(this.panelOrderCommands);
			this.Controls.Add(this.panelStatus);
			this.Controls.Add(this.CaptionBar);
			this.ForeColor = System.Drawing.SystemColors.ControlText;
			this.MinimumSize = new System.Drawing.Size(400, 250);
			this.Name = "frmOrders";
			this.ROCWindowType = FormEx.VistaWindowBase.ROCWindowTypes.Orders;
			this.ShowIcon = false;
			this.ShowInTaskbar = false;
			this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
			((System.ComponentModel.ISupportInitialize)(this.rocOrdersList)).EndInit();
			this.panelStatus.ResumeLayout(false);
			this.panelStatus.PerformLayout();
			this.panelOrderCommands.ResumeLayout(false);
			this.panelOrderCommands.PerformLayout();
			this.panelPositionFilters.ResumeLayout(false);
			this.panelPositionFilters.PerformLayout();
			this.ResumeLayout(false);

		}

		#endregion

		private FormEx.VistaWindowCaptionBar CaptionBar;
		private menuBaseProcessing menuProcessing;
		private DataGridViewEx.ROCOrderList rocOrdersList;
		private PanelEx.VistaPanel panelStatus;
		private LabelEx.LabelVista dspOrdersQty;
		private LabelEx.LabelVista lblOrdersQty;
		private LabelEx.LabelVista lblBuyFilled;
		private LabelEx.LabelVista dspEXC;
		private LabelEx.LabelVista lblBuyOpened;
		private LabelEx.LabelVista lblShortOpened;
		private LabelEx.LabelVista lblShortFilled;
		private LabelEx.LabelVista lblSellOpened;
		private LabelEx.LabelVista lblSellFilled;
		private PanelEx.VistaPanel panelOrderCommands;
		private System.Windows.Forms.TextBox txtSymbolFilter;
		private LabelEx.LabelVista dspWRK;
		private LabelEx.LabelVista labelShort;
		private LabelEx.LabelVista dspBuy;
		private LabelEx.LabelVista dspSell;
		private LabelEx.LabelVista lblNet;
		private LabelEx.LabelVista dspNet;
		private PanelEx.VistaPanel panelPositionFilters;
		private ButtonEx.VistaButton cmdAccounts;
		private LabelEx.LabelVista lblSymbols;
		private ButtonEx.VistaButton cmdSecurities;
		private ButtonEx.VistaButton cmdStatus;
		private ButtonEx.VistaButton cmdShowAll;
		private ButtonEx.VistaButton cmdOrderType;
		private ButtonEx.VistaButton cmdTIF;
		private ButtonEx.VistaButton cmdCancelAll;
	}
}