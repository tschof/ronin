namespace ROC
{
	partial class frmPlotList
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
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(frmPlotList));
			DataGridViewEx.DGVBasePrinter dgvBasePrinter1 = new DataGridViewEx.DGVBasePrinter();
			System.Drawing.StringFormat stringFormat1 = new System.Drawing.StringFormat();
			System.Drawing.StringFormat stringFormat2 = new System.Drawing.StringFormat();
			System.Drawing.StringFormat stringFormat3 = new System.Drawing.StringFormat();
			System.Drawing.StringFormat stringFormat4 = new System.Drawing.StringFormat();
			this.CaptionBar = new FormEx.VistaWindowCaptionBar();
			this.splitPlot = new System.Windows.Forms.SplitContainer();
			this.splitGraph = new System.Windows.Forms.SplitContainer();
			this.zedTradedGraph = new PlotEx.ZedGraphControl();
			this.zedTradedVolumesGraph = new PlotEx.ZedGraphControl();
			this.lblMarketDataError = new LabelEx.LabelVista();
			this.menuProcessing = new ROC.menuBaseProcessing();
			this.rocPlotList = new DataGridViewEx.ROCWatchList();
			this.splitPlot.Panel1.SuspendLayout();
			this.splitPlot.Panel2.SuspendLayout();
			this.splitPlot.SuspendLayout();
			this.splitGraph.Panel1.SuspendLayout();
			this.splitGraph.Panel2.SuspendLayout();
			this.splitGraph.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.rocPlotList)).BeginInit();
			this.SuspendLayout();
			// 
			// CaptionBar
			// 
			this.CaptionBar.BackColor = System.Drawing.Color.Transparent;
			this.CaptionBar.Dock = System.Windows.Forms.DockStyle.Top;
			this.CaptionBar.Location = new System.Drawing.Point(0, 0);
			this.CaptionBar.Name = "CaptionBar";
			this.CaptionBar.ShowDisplay = true;
			this.CaptionBar.ShowFit = true;
			this.CaptionBar.ShowRefresh = true;
			this.CaptionBar.ShowShowAll = true;
			this.CaptionBar.Size = new System.Drawing.Size(642, 26);
			this.CaptionBar.TabIndex = 1;
			// 
			// splitPlot
			// 
			this.splitPlot.Dock = System.Windows.Forms.DockStyle.Fill;
			this.splitPlot.Location = new System.Drawing.Point(0, 26);
			this.splitPlot.Name = "splitPlot";
			this.splitPlot.Orientation = System.Windows.Forms.Orientation.Horizontal;
			// 
			// splitPlot.Panel1
			// 
			this.splitPlot.Panel1.Controls.Add(this.splitGraph);
			// 
			// splitPlot.Panel2
			// 
			this.splitPlot.Panel2.Controls.Add(this.lblMarketDataError);
			this.splitPlot.Panel2.Controls.Add(this.menuProcessing);
			this.splitPlot.Panel2.Controls.Add(this.rocPlotList);
			this.splitPlot.Size = new System.Drawing.Size(642, 616);
			this.splitPlot.SplitterDistance = 508;
			this.splitPlot.TabIndex = 2;
			// 
			// splitGraph
			// 
			this.splitGraph.Dock = System.Windows.Forms.DockStyle.Fill;
			this.splitGraph.Location = new System.Drawing.Point(0, 0);
			this.splitGraph.Name = "splitGraph";
			this.splitGraph.Orientation = System.Windows.Forms.Orientation.Horizontal;
			// 
			// splitGraph.Panel1
			// 
			this.splitGraph.Panel1.Controls.Add(this.zedTradedGraph);
			// 
			// splitGraph.Panel2
			// 
			this.splitGraph.Panel2.Controls.Add(this.zedTradedVolumesGraph);
			this.splitGraph.Size = new System.Drawing.Size(642, 508);
			this.splitGraph.SplitterDistance = 356;
			this.splitGraph.TabIndex = 0;
			// 
			// zedTradedGraph
			// 
			this.zedTradedGraph.Dock = System.Windows.Forms.DockStyle.Fill;
			this.zedTradedGraph.Location = new System.Drawing.Point(0, 0);
			this.zedTradedGraph.Name = "zedTradedGraph";
			this.zedTradedGraph.ScrollGrace = 0;
			this.zedTradedGraph.ScrollMaxX = 0;
			this.zedTradedGraph.ScrollMaxY = 0;
			this.zedTradedGraph.ScrollMaxY2 = 0;
			this.zedTradedGraph.ScrollMinX = 0;
			this.zedTradedGraph.ScrollMinY = 0;
			this.zedTradedGraph.ScrollMinY2 = 0;
			this.zedTradedGraph.Size = new System.Drawing.Size(642, 356);
			this.zedTradedGraph.TabIndex = 5;
			// 
			// zedTradedVolumesGraph
			// 
			this.zedTradedVolumesGraph.Dock = System.Windows.Forms.DockStyle.Fill;
			this.zedTradedVolumesGraph.Font = new System.Drawing.Font("Microsoft Sans Serif", 6F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.zedTradedVolumesGraph.Location = new System.Drawing.Point(0, 0);
			this.zedTradedVolumesGraph.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
			this.zedTradedVolumesGraph.Name = "zedTradedVolumesGraph";
			this.zedTradedVolumesGraph.ScrollGrace = 0;
			this.zedTradedVolumesGraph.ScrollMaxX = 0;
			this.zedTradedVolumesGraph.ScrollMaxY = 0;
			this.zedTradedVolumesGraph.ScrollMaxY2 = 0;
			this.zedTradedVolumesGraph.ScrollMinX = 0;
			this.zedTradedVolumesGraph.ScrollMinY = 0;
			this.zedTradedVolumesGraph.ScrollMinY2 = 0;
			this.zedTradedVolumesGraph.Size = new System.Drawing.Size(642, 148);
			this.zedTradedVolumesGraph.TabIndex = 4;
			// 
			// lblMarketDataError
			// 
			this.lblMarketDataError.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.lblMarketDataError.AutoColor = false;
			this.lblMarketDataError.AutoSize = true;
			this.lblMarketDataError.DefaultColor = System.Drawing.Color.White;
			this.lblMarketDataError.DefaultText = "No Market Data";
			this.lblMarketDataError.DisplayFactor = 1;
			this.lblMarketDataError.DownColor = System.Drawing.Color.Red;
			this.lblMarketDataError.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblMarketDataError.Is64Th = false;
			this.lblMarketDataError.IsCurrency = false;
			this.lblMarketDataError.IsFraction = false;
			this.lblMarketDataError.IsPercent = false;
			this.lblMarketDataError.Location = new System.Drawing.Point(518, 35);
			this.lblMarketDataError.MaxDecimal = 7;
			this.lblMarketDataError.Name = "lblMarketDataError";
			this.lblMarketDataError.Padding = new System.Windows.Forms.Padding(2);
			this.lblMarketDataError.Size = new System.Drawing.Size(95, 17);
			this.lblMarketDataError.TabIndex = 16;
			this.lblMarketDataError.Text = "Market Data Error";
			this.lblMarketDataError.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			this.lblMarketDataError.TickSize = 0.01;
			this.lblMarketDataError.UpColor = System.Drawing.Color.LimeGreen;
			this.lblMarketDataError.Value = "";
			this.lblMarketDataError.Visible = false;
			// 
			// menuProcessing
			// 
			this.menuProcessing.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
			this.menuProcessing.BackColor = System.Drawing.Color.Transparent;
			this.menuProcessing.Caption = "000";
			this.menuProcessing.Location = new System.Drawing.Point(12, 41);
			this.menuProcessing.MaximumSize = new System.Drawing.Size(51, 51);
			this.menuProcessing.MinimumSize = new System.Drawing.Size(51, 51);
			this.menuProcessing.Name = "menuProcessing";
			this.menuProcessing.Size = new System.Drawing.Size(51, 51);
			this.menuProcessing.TabIndex = 15;
			this.menuProcessing.TabStop = false;
			this.menuProcessing.Visible = false;
			// 
			// rocPlotList
			// 
			this.rocPlotList.BlueDefault = 228;
			this.rocPlotList.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
			new BindingListEx.ROCBindingList<string>().Add("");
			this.rocPlotList.DisplayFactors = ((System.Collections.Generic.Dictionary<string, double>)(resources.GetObject("rocPlotList.DisplayFactors")));
			this.rocPlotList.Dock = System.Windows.Forms.DockStyle.Fill;
			this.rocPlotList.GreenDefault = 243;
			this.rocPlotList.GridKeys = ((System.Collections.Generic.List<string>)(resources.GetObject("rocPlotList.GridKeys")));
			this.rocPlotList.HeaderBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(233)))), ((int)(((byte)(243)))), ((int)(((byte)(228)))));
			this.rocPlotList.Importing = false;
			this.rocPlotList.LastFilter = "";
			this.rocPlotList.LastSort = "";
			this.rocPlotList.LastSortColumnDirection = System.Windows.Forms.SortOrder.None;
			this.rocPlotList.LastSortColumnName = "";
			this.rocPlotList.Location = new System.Drawing.Point(0, 0);
			this.rocPlotList.Name = "rocPlotList";
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
			this.rocPlotList.Printer = dgvBasePrinter1;
			this.rocPlotList.Processing = false;
			this.rocPlotList.RedDefault = 233;
			this.rocPlotList.RefreshAggragation = true;
			this.rocPlotList.RefreshSharedRows = false;
			this.rocPlotList.ShouldScrollToLastRow = false;
			this.rocPlotList.ShowActionMenu = false;
			this.rocPlotList.ShowColumnMenu = false;
			this.rocPlotList.ShowMenu = false;
			this.rocPlotList.Size = new System.Drawing.Size(642, 104);
			this.rocPlotList.SubscribeSymbolDetail = "";
			new BindingListEx.ROCBindingList<string>().Add("");
			this.rocPlotList.TabIndex = 0;
			this.rocPlotList.TickSizes = ((System.Collections.Generic.Dictionary<string, double>)(resources.GetObject("rocPlotList.TickSizes")));
			// 
			// frmPlotList
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.Caption = "Plot List";
			this.ClientSize = new System.Drawing.Size(642, 642);
			this.Controls.Add(this.splitPlot);
			this.Controls.Add(this.CaptionBar);
			this.MinimumSize = new System.Drawing.Size(300, 600);
			this.Name = "frmPlotList";
			this.ShowInTaskbar = false;
			this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
			this.splitPlot.Panel1.ResumeLayout(false);
			this.splitPlot.Panel2.ResumeLayout(false);
			this.splitPlot.Panel2.PerformLayout();
			this.splitPlot.ResumeLayout(false);
			this.splitGraph.Panel1.ResumeLayout(false);
			this.splitGraph.Panel2.ResumeLayout(false);
			this.splitGraph.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.rocPlotList)).EndInit();
			this.ResumeLayout(false);

		}

		#endregion

		private FormEx.VistaWindowCaptionBar CaptionBar;
		private System.Windows.Forms.SplitContainer splitPlot;
		private DataGridViewEx.ROCWatchList rocPlotList;
		private menuBaseProcessing menuProcessing;
		private System.Windows.Forms.SplitContainer splitGraph;
		private PlotEx.ZedGraphControl zedTradedVolumesGraph;
		private PlotEx.ZedGraphControl zedTradedGraph;
		private LabelEx.LabelVista lblMarketDataError;
	}
}