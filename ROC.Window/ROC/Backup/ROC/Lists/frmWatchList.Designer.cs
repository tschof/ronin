namespace ROC
{
	partial class frmWatchList
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
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(frmWatchList));
			DataGridViewEx.DGVBasePrinter dgvBasePrinter1 = new DataGridViewEx.DGVBasePrinter();
			System.Drawing.StringFormat stringFormat1 = new System.Drawing.StringFormat();
			System.Drawing.StringFormat stringFormat2 = new System.Drawing.StringFormat();
			System.Drawing.StringFormat stringFormat3 = new System.Drawing.StringFormat();
			System.Drawing.StringFormat stringFormat4 = new System.Drawing.StringFormat();
			this.CaptionBar = new FormEx.VistaWindowCaptionBar();
			this.menuProcessing = new ROC.menuBaseProcessing();
			this.rocWatchList = new DataGridViewEx.ROCWatchList();
			this.lblMarketDataError = new LabelEx.LabelVista();
			((System.ComponentModel.ISupportInitialize)(this.rocWatchList)).BeginInit();
			this.SuspendLayout();
			// 
			// CaptionBar
			// 
			this.CaptionBar.BackColor = System.Drawing.Color.Transparent;
			this.CaptionBar.ConfirmBeforeClose = true;
			this.CaptionBar.Dock = System.Windows.Forms.DockStyle.Top;
			this.CaptionBar.EnableCaptionEdit = true;
			this.CaptionBar.Location = new System.Drawing.Point(0, 0);
			this.CaptionBar.Name = "CaptionBar";
			this.CaptionBar.ShowDisplay = true;
			this.CaptionBar.ShowFit = true;
			this.CaptionBar.ShowRefresh = true;
			this.CaptionBar.ShowShowAll = true;
			this.CaptionBar.Size = new System.Drawing.Size(642, 26);
			this.CaptionBar.TabIndex = 0;
			// 
			// menuProcessing
			// 
			this.menuProcessing.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
			this.menuProcessing.BackColor = System.Drawing.Color.Transparent;
			this.menuProcessing.Caption = "000";
			this.menuProcessing.Location = new System.Drawing.Point(12, 166);
			this.menuProcessing.MaximumSize = new System.Drawing.Size(51, 51);
			this.menuProcessing.MinimumSize = new System.Drawing.Size(51, 51);
			this.menuProcessing.Name = "menuProcessing";
			this.menuProcessing.Size = new System.Drawing.Size(51, 51);
			this.menuProcessing.TabIndex = 14;
			this.menuProcessing.TabStop = false;
			this.menuProcessing.Visible = false;
			// 
			// rocWatchList
			// 
			this.rocWatchList.BlueDefault = 228;
			this.rocWatchList.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
			new BindingListEx.ROCBindingList<string>().Add("");
			this.rocWatchList.DisplayFactors = ((System.Collections.Generic.Dictionary<string, double>)(resources.GetObject("rocWatchList.DisplayFactors")));
			this.rocWatchList.Dock = System.Windows.Forms.DockStyle.Fill;
			this.rocWatchList.GreenDefault = 243;
			this.rocWatchList.GridKeys = ((System.Collections.Generic.List<string>)(resources.GetObject("rocWatchList.GridKeys")));
			this.rocWatchList.HeaderBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(233)))), ((int)(((byte)(243)))), ((int)(((byte)(228)))));
			this.rocWatchList.Importing = false;
			this.rocWatchList.LastFilter = "";
			this.rocWatchList.LastSort = "";
			this.rocWatchList.LastSortColumnDirection = System.Windows.Forms.SortOrder.None;
			this.rocWatchList.LastSortColumnName = "";
			this.rocWatchList.Location = new System.Drawing.Point(0, 26);
			this.rocWatchList.Name = "rocWatchList";
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
			this.rocWatchList.Printer = dgvBasePrinter1;
			this.rocWatchList.Processing = false;
			this.rocWatchList.RedDefault = 233;
			this.rocWatchList.RefreshAggragation = true;
			this.rocWatchList.RefreshSharedRows = false;
			this.rocWatchList.ShouldScrollToLastRow = false;
			this.rocWatchList.ShowActionMenu = false;
			this.rocWatchList.ShowColumnMenu = false;
			this.rocWatchList.ShowMenu = false;
			this.rocWatchList.Size = new System.Drawing.Size(642, 216);
			this.rocWatchList.SubscribeSymbolDetail = "";
			new BindingListEx.ROCBindingList<string>().Add("");
			this.rocWatchList.TabIndex = 15;
			this.rocWatchList.TickSizes = ((System.Collections.Generic.Dictionary<string, double>)(resources.GetObject("rocWatchList.TickSizes")));
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
			this.lblMarketDataError.Location = new System.Drawing.Point(518, 60);
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
			// frmWatchList
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.Caption = "Watch List";
			this.ClientSize = new System.Drawing.Size(642, 242);
			this.Controls.Add(this.lblMarketDataError);
			this.Controls.Add(this.menuProcessing);
			this.Controls.Add(this.rocWatchList);
			this.Controls.Add(this.CaptionBar);
			this.MinimumSize = new System.Drawing.Size(300, 250);
			this.Name = "frmWatchList";
			this.ROCWindowType = FormEx.VistaWindowBase.ROCWindowTypes.WatchList;
			this.ShowInTaskbar = false;
			this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
			((System.ComponentModel.ISupportInitialize)(this.rocWatchList)).EndInit();
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private FormEx.VistaWindowCaptionBar CaptionBar;
		private menuBaseProcessing menuProcessing;
		private DataGridViewEx.ROCWatchList rocWatchList;
		private LabelEx.LabelVista lblMarketDataError;
	}
}