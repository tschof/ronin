namespace ROC
{
	partial class frmSettings
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
			System.Windows.Forms.Label lblAdminTools;
			System.Windows.Forms.Label lblResubscribeSourceFilter;
			System.Windows.Forms.Label lblResubscribeRate;
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(frmSettings));
			DataGridViewEx.DGVBasePrinter dgvBasePrinter1 = new DataGridViewEx.DGVBasePrinter();
			System.Drawing.StringFormat stringFormat1 = new System.Drawing.StringFormat();
			System.Drawing.StringFormat stringFormat2 = new System.Drawing.StringFormat();
			System.Drawing.StringFormat stringFormat3 = new System.Drawing.StringFormat();
			System.Drawing.StringFormat stringFormat4 = new System.Drawing.StringFormat();
			System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle1 = new System.Windows.Forms.DataGridViewCellStyle();
			System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle2 = new System.Windows.Forms.DataGridViewCellStyle();
			System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle3 = new System.Windows.Forms.DataGridViewCellStyle();
			System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle4 = new System.Windows.Forms.DataGridViewCellStyle();
			System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle5 = new System.Windows.Forms.DataGridViewCellStyle();
			System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle6 = new System.Windows.Forms.DataGridViewCellStyle();
			System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle7 = new System.Windows.Forms.DataGridViewCellStyle();
			System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle8 = new System.Windows.Forms.DataGridViewCellStyle();
			System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle9 = new System.Windows.Forms.DataGridViewCellStyle();
			DataGridViewEx.DGVBasePrinter dgvBasePrinter2 = new DataGridViewEx.DGVBasePrinter();
			System.Drawing.StringFormat stringFormat5 = new System.Drawing.StringFormat();
			System.Drawing.StringFormat stringFormat6 = new System.Drawing.StringFormat();
			System.Drawing.StringFormat stringFormat7 = new System.Drawing.StringFormat();
			System.Drawing.StringFormat stringFormat8 = new System.Drawing.StringFormat();
			System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle10 = new System.Windows.Forms.DataGridViewCellStyle();
			System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle11 = new System.Windows.Forms.DataGridViewCellStyle();
			System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle12 = new System.Windows.Forms.DataGridViewCellStyle();
			System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle13 = new System.Windows.Forms.DataGridViewCellStyle();
			System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle14 = new System.Windows.Forms.DataGridViewCellStyle();
			this.CaptionBar = new FormEx.VistaWindowCaptionBar();
			this.panelSelections = new PanelEx.VistaPanel();
			this.cmdProcess = new ButtonEx.VistaButton();
			this.cmdQuickQty = new ButtonEx.VistaButton();
			this.cmdOrders = new ButtonEx.VistaButton();
			this.cmdSounds = new ButtonEx.VistaButton();
			this.cmdWindows = new ButtonEx.VistaButton();
			this.cmdSymbols = new ButtonEx.VistaButton();
			this.panelManagerSettings = new PanelEx.PanelManager();
			this.managedPanelSymbolSettings = new PanelEx.ManagedPanel();
			this.rocSymbolSettingList = new DataGridViewEx.ROCSymbolSettingList();
			this.panelSymbolSettings = new PanelEx.VistaPanel();
			this.cmdOptionSymbols = new ButtonEx.VistaButton();
			this.cmdFutureSymbols = new ButtonEx.VistaButton();
			this.cmdStockSymbols = new ButtonEx.VistaButton();
			this.lblSymbolSettings = new LabelEx.LabelRotatable();
			this.managedPaneWindowSettings = new PanelEx.ManagedPanel();
			this.DGVAutoFocusSettings = new System.Windows.Forms.DataGridView();
			this.Window = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.Enable = new System.Windows.Forms.DataGridViewCheckBoxColumn();
			this.lblAutoFocus = new LabelEx.LabelRotatable();
			this.managedPanelSoundSettings = new PanelEx.ManagedPanel();
			this.DGVSounds = new System.Windows.Forms.DataGridView();
			this.Type = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.Path = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.Browse = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.Test = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.SoundEnable = new System.Windows.Forms.DataGridViewCheckBoxColumn();
			this.labelSoundSettings = new LabelEx.LabelRotatable();
			this.managedPanelOrderSettings = new PanelEx.ManagedPanel();
			this.panelArgPriceResolution = new System.Windows.Forms.Panel();
			this.cboAvgPriceResolution = new System.Windows.Forms.ComboBox();
			this.lblAvePriceResolution = new System.Windows.Forms.Label();
			this.panelAutoFocusLast = new System.Windows.Forms.Panel();
			this.chkOrderAutoFocusLast = new System.Windows.Forms.CheckBox();
			this.lblAutoFocusLast = new System.Windows.Forms.Label();
			this.panelSkipGTDandGTDonAuto = new System.Windows.Forms.Panel();
			this.chkSkipGTCandGTDonAuto = new System.Windows.Forms.CheckBox();
			this.lblSkipGTCandGTDonAuto = new System.Windows.Forms.Label();
			this.panelOptionAutoCancel = new System.Windows.Forms.Panel();
			this.chkOptionAutoCancel = new System.Windows.Forms.CheckBox();
			this.dtpOptionAutoCancel = new System.Windows.Forms.DateTimePicker();
			this.lblOptionAutoCancel = new System.Windows.Forms.Label();
			this.panelFutureAutoCancel = new System.Windows.Forms.Panel();
			this.chkFutureAutoCancel = new System.Windows.Forms.CheckBox();
			this.dtpFutureAutoCancel = new System.Windows.Forms.DateTimePicker();
			this.lblFutureAutoCancel = new System.Windows.Forms.Label();
			this.panelStockAutoCancel = new System.Windows.Forms.Panel();
			this.chkStockAutoCancel = new System.Windows.Forms.CheckBox();
			this.dtpStockAutoCancel = new System.Windows.Forms.DateTimePicker();
			this.lblStockAutoCancel = new System.Windows.Forms.Label();
			this.labelOrderSettings = new LabelEx.LabelRotatable();
			this.managedPanelQuickButtonSettings = new PanelEx.ManagedPanel();
			this.rocQuickButtonList = new DataGridViewEx.ROCQuickButtonList();
			this.panelQuickButtonSelection = new PanelEx.VistaPanel();
			this.cmdOptionTicket = new ButtonEx.VistaButton();
			this.cmdFutureTicket = new ButtonEx.VistaButton();
			this.cmdStockTicket = new ButtonEx.VistaButton();
			this.labelQuickButtonSettings = new LabelEx.LabelRotatable();
			this.managedPanelProcessSettings = new PanelEx.ManagedPanel();
			this.panel1 = new System.Windows.Forms.Panel();
			this.dspMDSBackup = new System.Windows.Forms.Label();
			this.lblMDSBackup = new System.Windows.Forms.Label();
			this.chkMDSBackup = new System.Windows.Forms.CheckBox();
			this.panel2 = new System.Windows.Forms.Panel();
			this.dspMDS = new System.Windows.Forms.Label();
			this.lblMDS = new System.Windows.Forms.Label();
			this.chkMDS = new System.Windows.Forms.CheckBox();
			this.panelAdmin = new System.Windows.Forms.Panel();
			this.cboResubscribeRate = new System.Windows.Forms.ComboBox();
			this.panelResubscribe = new System.Windows.Forms.Panel();
			this.cmdResubscrbe = new ButtonEx.VistaButton();
			this.lblResubscribeStatus = new LabelEx.LabelBase();
			this.txtSourceFilter = new System.Windows.Forms.TextBox();
			this.panelUIUpdateRate = new System.Windows.Forms.Panel();
			this.panelTrackBarUI = new System.Windows.Forms.Panel();
			this.lbl250ms = new System.Windows.Forms.Label();
			this.lbl200ms = new System.Windows.Forms.Label();
			this.lbl150ms = new System.Windows.Forms.Label();
			this.lbl100ms = new System.Windows.Forms.Label();
			this.lbl50ms = new System.Windows.Forms.Label();
			this.lbl0ms = new System.Windows.Forms.Label();
			this.trackBarUIUpdateRate = new System.Windows.Forms.TrackBar();
			this.lblUIUpdateRate = new System.Windows.Forms.Label();
			this.chkUserUIUpdateRate = new System.Windows.Forms.CheckBox();
			this.panelCSVThreadPriority = new System.Windows.Forms.Panel();
			this.lblCSVNotice = new System.Windows.Forms.Label();
			this.cboCSVThreadPriority = new System.Windows.Forms.ComboBox();
			this.lblCSVThreadPriority = new System.Windows.Forms.Label();
			this.panelMDThreadPriority = new System.Windows.Forms.Panel();
			this.lblMDNotice = new System.Windows.Forms.Label();
			this.cboMDThreadPriority = new System.Windows.Forms.ComboBox();
			this.lblMDThreadPriority = new System.Windows.Forms.Label();
			this.lblProcessSettings = new LabelEx.LabelRotatable();
			this.panelCMD = new PanelEx.VistaPanel();
			this.cmdApply = new ButtonEx.VistaButton();
			this.cmdCancelSymbols = new ButtonEx.VistaButton();
			this.dataGridViewTextBoxColumn1 = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.dataGridViewTextBoxColumn2 = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.dataGridViewTextBoxColumn3 = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.dataGridViewTextBoxColumn4 = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.dataGridViewTextBoxColumn5 = new System.Windows.Forms.DataGridViewTextBoxColumn();
			lblAdminTools = new System.Windows.Forms.Label();
			lblResubscribeSourceFilter = new System.Windows.Forms.Label();
			lblResubscribeRate = new System.Windows.Forms.Label();
			this.panelSelections.SuspendLayout();
			this.panelManagerSettings.SuspendLayout();
			this.managedPanelSymbolSettings.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.rocSymbolSettingList)).BeginInit();
			this.panelSymbolSettings.SuspendLayout();
			this.managedPaneWindowSettings.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.DGVAutoFocusSettings)).BeginInit();
			this.managedPanelSoundSettings.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.DGVSounds)).BeginInit();
			this.managedPanelOrderSettings.SuspendLayout();
			this.panelArgPriceResolution.SuspendLayout();
			this.panelAutoFocusLast.SuspendLayout();
			this.panelSkipGTDandGTDonAuto.SuspendLayout();
			this.panelOptionAutoCancel.SuspendLayout();
			this.panelFutureAutoCancel.SuspendLayout();
			this.panelStockAutoCancel.SuspendLayout();
			this.managedPanelQuickButtonSettings.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.rocQuickButtonList)).BeginInit();
			this.panelQuickButtonSelection.SuspendLayout();
			this.managedPanelProcessSettings.SuspendLayout();
			this.panel1.SuspendLayout();
			this.panel2.SuspendLayout();
			this.panelAdmin.SuspendLayout();
			this.panelResubscribe.SuspendLayout();
			this.panelUIUpdateRate.SuspendLayout();
			this.panelTrackBarUI.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.trackBarUIUpdateRate)).BeginInit();
			this.panelCSVThreadPriority.SuspendLayout();
			this.panelMDThreadPriority.SuspendLayout();
			this.panelCMD.SuspendLayout();
			this.SuspendLayout();
			// 
			// lblAdminTools
			// 
			lblAdminTools.ForeColor = System.Drawing.Color.White;
			lblAdminTools.Location = new System.Drawing.Point(6, 6);
			lblAdminTools.Name = "lblAdminTools";
			lblAdminTools.Size = new System.Drawing.Size(125, 20);
			lblAdminTools.TabIndex = 15;
			lblAdminTools.Text = "Re-Subscribe To MDS";
			lblAdminTools.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// lblResubscribeSourceFilter
			// 
			lblResubscribeSourceFilter.Dock = System.Windows.Forms.DockStyle.Left;
			lblResubscribeSourceFilter.ForeColor = System.Drawing.Color.White;
			lblResubscribeSourceFilter.Location = new System.Drawing.Point(2, 2);
			lblResubscribeSourceFilter.Name = "lblResubscribeSourceFilter";
			lblResubscribeSourceFilter.Size = new System.Drawing.Size(79, 20);
			lblResubscribeSourceFilter.TabIndex = 16;
			lblResubscribeSourceFilter.Text = "Source Filter : ";
			lblResubscribeSourceFilter.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// lblResubscribeRate
			// 
			lblResubscribeRate.Dock = System.Windows.Forms.DockStyle.Left;
			lblResubscribeRate.ForeColor = System.Drawing.Color.White;
			lblResubscribeRate.Location = new System.Drawing.Point(0, 24);
			lblResubscribeRate.Name = "lblResubscribeRate";
			lblResubscribeRate.Size = new System.Drawing.Size(107, 24);
			lblResubscribeRate.TabIndex = 18;
			lblResubscribeRate.Text = "Resubscribe Rate : ";
			lblResubscribeRate.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// CaptionBar
			// 
			this.CaptionBar.BackColor = System.Drawing.Color.Transparent;
			this.CaptionBar.Dock = System.Windows.Forms.DockStyle.Top;
			this.CaptionBar.Location = new System.Drawing.Point(0, 0);
			this.CaptionBar.Name = "CaptionBar";
			this.CaptionBar.ShowFit = true;
			this.CaptionBar.Size = new System.Drawing.Size(404, 26);
			this.CaptionBar.TabIndex = 3;
			this.CaptionBar.TabStop = false;
			// 
			// panelSelections
			// 
			this.panelSelections.BackColor = System.Drawing.Color.Black;
			this.panelSelections.Controls.Add(this.cmdProcess);
			this.panelSelections.Controls.Add(this.cmdQuickQty);
			this.panelSelections.Controls.Add(this.cmdOrders);
			this.panelSelections.Controls.Add(this.cmdSounds);
			this.panelSelections.Controls.Add(this.cmdWindows);
			this.panelSelections.Controls.Add(this.cmdSymbols);
			this.panelSelections.Dock = System.Windows.Forms.DockStyle.Top;
			this.panelSelections.Location = new System.Drawing.Point(0, 26);
			this.panelSelections.Name = "panelSelections";
			this.panelSelections.Size = new System.Drawing.Size(404, 24);
			this.panelSelections.TabIndex = 4;
			// 
			// cmdProcess
			// 
			this.cmdProcess.AutoSize = true;
			this.cmdProcess.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdProcess.BackColor = System.Drawing.Color.Black;
			this.cmdProcess.Dock = System.Windows.Forms.DockStyle.Left;
			this.cmdProcess.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdProcess.Location = new System.Drawing.Point(341, 0);
			this.cmdProcess.Name = "cmdProcess";
			this.cmdProcess.Size = new System.Drawing.Size(62, 24);
			this.cmdProcess.TabIndex = 8;
			this.cmdProcess.Text = "Process";
			this.cmdProcess.Click += new System.EventHandler(this.cmdProcess_Click);
			// 
			// cmdQuickQty
			// 
			this.cmdQuickQty.AutoSize = true;
			this.cmdQuickQty.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdQuickQty.BackColor = System.Drawing.Color.Black;
			this.cmdQuickQty.Dock = System.Windows.Forms.DockStyle.Left;
			this.cmdQuickQty.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdQuickQty.Location = new System.Drawing.Point(244, 0);
			this.cmdQuickQty.Name = "cmdQuickQty";
			this.cmdQuickQty.Size = new System.Drawing.Size(97, 24);
			this.cmdQuickQty.TabIndex = 6;
			this.cmdQuickQty.Text = "Quick Buttons";
			this.cmdQuickQty.Click += new System.EventHandler(this.cmdQuickButton_Click);
			// 
			// cmdOrders
			// 
			this.cmdOrders.AutoSize = true;
			this.cmdOrders.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdOrders.BackColor = System.Drawing.Color.Black;
			this.cmdOrders.Dock = System.Windows.Forms.DockStyle.Left;
			this.cmdOrders.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdOrders.Location = new System.Drawing.Point(190, 0);
			this.cmdOrders.Name = "cmdOrders";
			this.cmdOrders.Size = new System.Drawing.Size(54, 24);
			this.cmdOrders.TabIndex = 7;
			this.cmdOrders.Text = "Orders";
			this.cmdOrders.Click += new System.EventHandler(this.cmdOrders_Click);
			// 
			// cmdSounds
			// 
			this.cmdSounds.AutoSize = true;
			this.cmdSounds.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdSounds.BackColor = System.Drawing.Color.Black;
			this.cmdSounds.Dock = System.Windows.Forms.DockStyle.Left;
			this.cmdSounds.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdSounds.Location = new System.Drawing.Point(131, 0);
			this.cmdSounds.Name = "cmdSounds";
			this.cmdSounds.Size = new System.Drawing.Size(59, 24);
			this.cmdSounds.TabIndex = 4;
			this.cmdSounds.Text = "Sounds";
			this.cmdSounds.Click += new System.EventHandler(this.cmdSounds_Click);
			// 
			// cmdWindows
			// 
			this.cmdWindows.AutoSize = true;
			this.cmdWindows.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdWindows.BackColor = System.Drawing.Color.Black;
			this.cmdWindows.Dock = System.Windows.Forms.DockStyle.Left;
			this.cmdWindows.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdWindows.Location = new System.Drawing.Point(63, 0);
			this.cmdWindows.Name = "cmdWindows";
			this.cmdWindows.Size = new System.Drawing.Size(68, 24);
			this.cmdWindows.TabIndex = 3;
			this.cmdWindows.Text = "Windows";
			this.cmdWindows.Click += new System.EventHandler(this.cmdWindows_Click);
			// 
			// cmdSymbols
			// 
			this.cmdSymbols.AutoSize = true;
			this.cmdSymbols.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdSymbols.BackColor = System.Drawing.Color.Black;
			this.cmdSymbols.Dock = System.Windows.Forms.DockStyle.Left;
			this.cmdSymbols.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdSymbols.Location = new System.Drawing.Point(0, 0);
			this.cmdSymbols.Name = "cmdSymbols";
			this.cmdSymbols.Size = new System.Drawing.Size(63, 24);
			this.cmdSymbols.TabIndex = 5;
			this.cmdSymbols.Text = "Symbols";
			this.cmdSymbols.Click += new System.EventHandler(this.cmdSymbols_Click);
			// 
			// panelManagerSettings
			// 
			this.panelManagerSettings.Controls.Add(this.managedPanelSymbolSettings);
			this.panelManagerSettings.Controls.Add(this.managedPaneWindowSettings);
			this.panelManagerSettings.Controls.Add(this.managedPanelSoundSettings);
			this.panelManagerSettings.Controls.Add(this.managedPanelOrderSettings);
			this.panelManagerSettings.Controls.Add(this.managedPanelQuickButtonSettings);
			this.panelManagerSettings.Controls.Add(this.managedPanelProcessSettings);
			this.panelManagerSettings.Dock = System.Windows.Forms.DockStyle.Fill;
			this.panelManagerSettings.Location = new System.Drawing.Point(0, 50);
			this.panelManagerSettings.Name = "panelManagerSettings";
			this.panelManagerSettings.SelectedIndex = 0;
			this.panelManagerSettings.SelectedPanel = this.managedPanelSymbolSettings;
			this.panelManagerSettings.Size = new System.Drawing.Size(404, 353);
			this.panelManagerSettings.TabIndex = 5;
			this.panelManagerSettings.TabStop = false;
			// 
			// managedPanelSymbolSettings
			// 
			this.managedPanelSymbolSettings.BackColor = System.Drawing.SystemColors.Control;
			this.managedPanelSymbolSettings.Controls.Add(this.rocSymbolSettingList);
			this.managedPanelSymbolSettings.Controls.Add(this.panelSymbolSettings);
			this.managedPanelSymbolSettings.Controls.Add(this.lblSymbolSettings);
			this.managedPanelSymbolSettings.Location = new System.Drawing.Point(0, 0);
			this.managedPanelSymbolSettings.Name = "managedPanelSymbolSettings";
			this.managedPanelSymbolSettings.Padding = new System.Windows.Forms.Padding(2);
			this.managedPanelSymbolSettings.Size = new System.Drawing.Size(404, 353);
			this.managedPanelSymbolSettings.Text = "managedPanelSymbolSymbolSettings";
			// 
			// rocSymbolSettingList
			// 
			this.rocSymbolSettingList.BlueDefault = 228;
			this.rocSymbolSettingList.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
			this.rocSymbolSettingList.CurrentSecType = "E";
			new BindingListEx.ROCBindingList<string>().Add("");
			this.rocSymbolSettingList.DisplayFactors = ((System.Collections.Generic.Dictionary<string, double>)(resources.GetObject("rocSymbolSettingList.DisplayFactors")));
			this.rocSymbolSettingList.Dock = System.Windows.Forms.DockStyle.Fill;
			this.rocSymbolSettingList.GreenDefault = 243;
			this.rocSymbolSettingList.GridKeys = ((System.Collections.Generic.List<string>)(resources.GetObject("rocSymbolSettingList.GridKeys")));
			this.rocSymbolSettingList.HeaderBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(233)))), ((int)(((byte)(243)))), ((int)(((byte)(228)))));
			this.rocSymbolSettingList.Importing = false;
			this.rocSymbolSettingList.LastFilter = "";
			this.rocSymbolSettingList.LastSort = "";
			this.rocSymbolSettingList.Location = new System.Drawing.Point(26, 26);
			this.rocSymbolSettingList.Name = "rocSymbolSettingList";
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
			this.rocSymbolSettingList.Printer = dgvBasePrinter1;
			this.rocSymbolSettingList.Processing = false;
			this.rocSymbolSettingList.RedDefault = 233;
			this.rocSymbolSettingList.RefreshAggragation = true;
			this.rocSymbolSettingList.RefreshSharedRows = false;
			this.rocSymbolSettingList.ShouldScrollToLastRow = false;
			this.rocSymbolSettingList.Size = new System.Drawing.Size(376, 325);
			new BindingListEx.ROCBindingList<string>().Add("");
			this.rocSymbolSettingList.TabIndex = 5;
			this.rocSymbolSettingList.TickSizes = ((System.Collections.Generic.Dictionary<string, double>)(resources.GetObject("rocSymbolSettingList.TickSizes")));
			// 
			// panelSymbolSettings
			// 
			this.panelSymbolSettings.BackColor = System.Drawing.Color.Black;
			this.panelSymbolSettings.Controls.Add(this.cmdOptionSymbols);
			this.panelSymbolSettings.Controls.Add(this.cmdFutureSymbols);
			this.panelSymbolSettings.Controls.Add(this.cmdStockSymbols);
			this.panelSymbolSettings.Dock = System.Windows.Forms.DockStyle.Top;
			this.panelSymbolSettings.Location = new System.Drawing.Point(26, 2);
			this.panelSymbolSettings.Name = "panelSymbolSettings";
			this.panelSymbolSettings.Size = new System.Drawing.Size(376, 24);
			this.panelSymbolSettings.TabIndex = 10;
			// 
			// cmdOptionSymbols
			// 
			this.cmdOptionSymbols.AutoSize = true;
			this.cmdOptionSymbols.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdOptionSymbols.BackColor = System.Drawing.Color.Khaki;
			this.cmdOptionSymbols.Dock = System.Windows.Forms.DockStyle.Left;
			this.cmdOptionSymbols.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdOptionSymbols.ForeColor = System.Drawing.Color.Black;
			this.cmdOptionSymbols.Location = new System.Drawing.Point(203, 0);
			this.cmdOptionSymbols.Name = "cmdOptionSymbols";
			this.cmdOptionSymbols.Size = new System.Drawing.Size(104, 24);
			this.cmdOptionSymbols.TabIndex = 12;
			this.cmdOptionSymbols.Text = "Option Symbols";
			this.cmdOptionSymbols.Click += new System.EventHandler(this.cmdOptionSymbols_Click);
			// 
			// cmdFutureSymbols
			// 
			this.cmdFutureSymbols.AutoSize = true;
			this.cmdFutureSymbols.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdFutureSymbols.BackColor = System.Drawing.Color.Khaki;
			this.cmdFutureSymbols.Dock = System.Windows.Forms.DockStyle.Left;
			this.cmdFutureSymbols.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdFutureSymbols.ForeColor = System.Drawing.Color.Black;
			this.cmdFutureSymbols.Location = new System.Drawing.Point(100, 0);
			this.cmdFutureSymbols.Name = "cmdFutureSymbols";
			this.cmdFutureSymbols.Size = new System.Drawing.Size(103, 24);
			this.cmdFutureSymbols.TabIndex = 11;
			this.cmdFutureSymbols.Text = "Future Symbols";
			this.cmdFutureSymbols.Click += new System.EventHandler(this.cmdFutureSymbols_Click);
			// 
			// cmdStockSymbols
			// 
			this.cmdStockSymbols.AutoSize = true;
			this.cmdStockSymbols.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdStockSymbols.BackColor = System.Drawing.Color.Khaki;
			this.cmdStockSymbols.Dock = System.Windows.Forms.DockStyle.Left;
			this.cmdStockSymbols.Enabled = false;
			this.cmdStockSymbols.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdStockSymbols.ForeColor = System.Drawing.Color.Black;
			this.cmdStockSymbols.Location = new System.Drawing.Point(0, 0);
			this.cmdStockSymbols.Name = "cmdStockSymbols";
			this.cmdStockSymbols.Size = new System.Drawing.Size(100, 24);
			this.cmdStockSymbols.TabIndex = 10;
			this.cmdStockSymbols.Text = "Stock Symbols";
			this.cmdStockSymbols.Click += new System.EventHandler(this.cmdStockSymbols_Click);
			// 
			// lblSymbolSettings
			// 
			this.lblSymbolSettings.AutoColor = false;
			this.lblSymbolSettings.BackColor = System.Drawing.SystemColors.ActiveCaption;
			this.lblSymbolSettings.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.lblSymbolSettings.DefaultColor = System.Drawing.Color.White;
			this.lblSymbolSettings.DefaultText = "";
			this.lblSymbolSettings.DisplayFactor = 1;
			this.lblSymbolSettings.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblSymbolSettings.DownColor = System.Drawing.Color.Red;
			this.lblSymbolSettings.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblSymbolSettings.ForeColor = System.Drawing.SystemColors.ActiveCaptionText;
			this.lblSymbolSettings.Is64Th = false;
			this.lblSymbolSettings.IsCurrency = false;
			this.lblSymbolSettings.IsFraction = false;
			this.lblSymbolSettings.IsPercent = false;
			this.lblSymbolSettings.Location = new System.Drawing.Point(2, 2);
			this.lblSymbolSettings.MaxDecimal = 7;
			this.lblSymbolSettings.Name = "lblSymbolSettings";
			this.lblSymbolSettings.RotationAngle = 270;
			this.lblSymbolSettings.Size = new System.Drawing.Size(24, 349);
			this.lblSymbolSettings.TabIndex = 2;
			this.lblSymbolSettings.Text = "Symbol Settings";
			this.lblSymbolSettings.TextDirection = LabelEx.LabelRotatable.Direction.Clockwise;
			this.lblSymbolSettings.TextOrientation = LabelEx.LabelRotatable.Orientation.Rotate;
			this.lblSymbolSettings.TickSize = 0.01;
			this.lblSymbolSettings.UpColor = System.Drawing.Color.LimeGreen;
			this.lblSymbolSettings.Value = "";
			// 
			// managedPaneWindowSettings
			// 
			this.managedPaneWindowSettings.Controls.Add(this.DGVAutoFocusSettings);
			this.managedPaneWindowSettings.Controls.Add(this.lblAutoFocus);
			this.managedPaneWindowSettings.Location = new System.Drawing.Point(0, 0);
			this.managedPaneWindowSettings.Name = "managedPaneWindowSettings";
			this.managedPaneWindowSettings.Size = new System.Drawing.Size(0, 0);
			this.managedPaneWindowSettings.Text = "managedPanelWindowSettings";
			// 
			// DGVAutoFocusSettings
			// 
			this.DGVAutoFocusSettings.AllowUserToAddRows = false;
			this.DGVAutoFocusSettings.AllowUserToDeleteRows = false;
			this.DGVAutoFocusSettings.AllowUserToResizeRows = false;
			this.DGVAutoFocusSettings.ColumnHeadersBorderStyle = System.Windows.Forms.DataGridViewHeaderBorderStyle.Single;
			dataGridViewCellStyle1.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
			dataGridViewCellStyle1.BackColor = System.Drawing.SystemColors.Control;
			dataGridViewCellStyle1.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			dataGridViewCellStyle1.ForeColor = System.Drawing.Color.Black;
			dataGridViewCellStyle1.Padding = new System.Windows.Forms.Padding(2);
			dataGridViewCellStyle1.SelectionBackColor = System.Drawing.SystemColors.Highlight;
			dataGridViewCellStyle1.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
			dataGridViewCellStyle1.WrapMode = System.Windows.Forms.DataGridViewTriState.False;
			this.DGVAutoFocusSettings.ColumnHeadersDefaultCellStyle = dataGridViewCellStyle1;
			this.DGVAutoFocusSettings.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
			this.DGVAutoFocusSettings.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.Window,
            this.Enable});
			this.DGVAutoFocusSettings.Dock = System.Windows.Forms.DockStyle.Fill;
			this.DGVAutoFocusSettings.GridColor = System.Drawing.Color.Black;
			this.DGVAutoFocusSettings.Location = new System.Drawing.Point(26, 0);
			this.DGVAutoFocusSettings.MinimumSize = new System.Drawing.Size(216, 218);
			this.DGVAutoFocusSettings.MultiSelect = false;
			this.DGVAutoFocusSettings.Name = "DGVAutoFocusSettings";
			this.DGVAutoFocusSettings.RowHeadersVisible = false;
			this.DGVAutoFocusSettings.Size = new System.Drawing.Size(216, 218);
			this.DGVAutoFocusSettings.TabIndex = 5;
			// 
			// Window
			// 
			this.Window.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
			dataGridViewCellStyle2.BackColor = System.Drawing.Color.Black;
			dataGridViewCellStyle2.ForeColor = System.Drawing.Color.Khaki;
			this.Window.DefaultCellStyle = dataGridViewCellStyle2;
			this.Window.FillWeight = 40.32626F;
			this.Window.HeaderText = "Window";
			this.Window.Name = "Window";
			this.Window.ReadOnly = true;
			this.Window.Resizable = System.Windows.Forms.DataGridViewTriState.False;
			this.Window.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable;
			// 
			// Enable
			// 
			dataGridViewCellStyle3.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleCenter;
			dataGridViewCellStyle3.BackColor = System.Drawing.SystemColors.ButtonFace;
			dataGridViewCellStyle3.NullValue = false;
			this.Enable.DefaultCellStyle = dataGridViewCellStyle3;
			this.Enable.HeaderText = "";
			this.Enable.Name = "Enable";
			this.Enable.Width = 20;
			// 
			// lblAutoFocus
			// 
			this.lblAutoFocus.AutoColor = false;
			this.lblAutoFocus.BackColor = System.Drawing.SystemColors.ActiveCaption;
			this.lblAutoFocus.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.lblAutoFocus.DefaultColor = System.Drawing.Color.White;
			this.lblAutoFocus.DefaultText = "";
			this.lblAutoFocus.DisplayFactor = 1;
			this.lblAutoFocus.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblAutoFocus.DownColor = System.Drawing.Color.Red;
			this.lblAutoFocus.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblAutoFocus.ForeColor = System.Drawing.SystemColors.ActiveCaptionText;
			this.lblAutoFocus.Is64Th = false;
			this.lblAutoFocus.IsCurrency = false;
			this.lblAutoFocus.IsFraction = false;
			this.lblAutoFocus.IsPercent = false;
			this.lblAutoFocus.Location = new System.Drawing.Point(0, 0);
			this.lblAutoFocus.MaxDecimal = 7;
			this.lblAutoFocus.Name = "lblAutoFocus";
			this.lblAutoFocus.RotationAngle = 270;
			this.lblAutoFocus.Size = new System.Drawing.Size(26, 0);
			this.lblAutoFocus.TabIndex = 6;
			this.lblAutoFocus.Text = "Window Settings";
			this.lblAutoFocus.TextDirection = LabelEx.LabelRotatable.Direction.Clockwise;
			this.lblAutoFocus.TextOrientation = LabelEx.LabelRotatable.Orientation.Rotate;
			this.lblAutoFocus.TickSize = 0.01;
			this.lblAutoFocus.UpColor = System.Drawing.Color.LimeGreen;
			this.lblAutoFocus.Value = "";
			// 
			// managedPanelSoundSettings
			// 
			this.managedPanelSoundSettings.Controls.Add(this.DGVSounds);
			this.managedPanelSoundSettings.Controls.Add(this.labelSoundSettings);
			this.managedPanelSoundSettings.Location = new System.Drawing.Point(0, 0);
			this.managedPanelSoundSettings.Name = "managedPanelSoundSettings";
			this.managedPanelSoundSettings.Size = new System.Drawing.Size(0, 0);
			this.managedPanelSoundSettings.Text = "managedSoundSettings";
			// 
			// DGVSounds
			// 
			this.DGVSounds.AllowUserToAddRows = false;
			this.DGVSounds.AllowUserToDeleteRows = false;
			this.DGVSounds.AllowUserToResizeRows = false;
			this.DGVSounds.BorderStyle = System.Windows.Forms.BorderStyle.None;
			this.DGVSounds.ColumnHeadersBorderStyle = System.Windows.Forms.DataGridViewHeaderBorderStyle.Single;
			dataGridViewCellStyle4.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
			dataGridViewCellStyle4.BackColor = System.Drawing.SystemColors.Control;
			dataGridViewCellStyle4.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			dataGridViewCellStyle4.ForeColor = System.Drawing.SystemColors.WindowText;
			dataGridViewCellStyle4.Padding = new System.Windows.Forms.Padding(2);
			dataGridViewCellStyle4.SelectionBackColor = System.Drawing.SystemColors.Highlight;
			dataGridViewCellStyle4.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
			dataGridViewCellStyle4.WrapMode = System.Windows.Forms.DataGridViewTriState.False;
			this.DGVSounds.ColumnHeadersDefaultCellStyle = dataGridViewCellStyle4;
			this.DGVSounds.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
			this.DGVSounds.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.Type,
            this.Path,
            this.Browse,
            this.Test,
            this.SoundEnable});
			this.DGVSounds.Dock = System.Windows.Forms.DockStyle.Fill;
			this.DGVSounds.Location = new System.Drawing.Point(24, 0);
			this.DGVSounds.MultiSelect = false;
			this.DGVSounds.Name = "DGVSounds";
			this.DGVSounds.RowHeadersVisible = false;
			this.DGVSounds.Size = new System.Drawing.Size(0, 0);
			this.DGVSounds.TabIndex = 4;
			// 
			// Type
			// 
			dataGridViewCellStyle5.BackColor = System.Drawing.Color.Black;
			dataGridViewCellStyle5.ForeColor = System.Drawing.Color.Khaki;
			this.Type.DefaultCellStyle = dataGridViewCellStyle5;
			this.Type.FillWeight = 40.32626F;
			this.Type.HeaderText = "Type";
			this.Type.Name = "Type";
			this.Type.ReadOnly = true;
			this.Type.Resizable = System.Windows.Forms.DataGridViewTriState.True;
			this.Type.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable;
			this.Type.Width = 39;
			// 
			// Path
			// 
			dataGridViewCellStyle6.BackColor = System.Drawing.Color.Black;
			dataGridViewCellStyle6.ForeColor = System.Drawing.Color.White;
			this.Path.DefaultCellStyle = dataGridViewCellStyle6;
			this.Path.FillWeight = 116.8638F;
			this.Path.HeaderText = "Path";
			this.Path.MinimumWidth = 100;
			this.Path.Name = "Path";
			this.Path.Resizable = System.Windows.Forms.DataGridViewTriState.True;
			this.Path.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable;
			this.Path.Width = 113;
			// 
			// Browse
			// 
			dataGridViewCellStyle7.BackColor = System.Drawing.SystemColors.ButtonFace;
			dataGridViewCellStyle7.ForeColor = System.Drawing.Color.Black;
			this.Browse.DefaultCellStyle = dataGridViewCellStyle7;
			this.Browse.FillWeight = 23.37277F;
			this.Browse.HeaderText = "";
			this.Browse.Name = "Browse";
			this.Browse.ReadOnly = true;
			this.Browse.Resizable = System.Windows.Forms.DataGridViewTriState.True;
			this.Browse.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable;
			this.Browse.Width = 22;
			// 
			// Test
			// 
			dataGridViewCellStyle8.BackColor = System.Drawing.SystemColors.ButtonFace;
			dataGridViewCellStyle8.ForeColor = System.Drawing.Color.Black;
			this.Test.DefaultCellStyle = dataGridViewCellStyle8;
			this.Test.FillWeight = 23.37277F;
			this.Test.HeaderText = "";
			this.Test.Name = "Test";
			this.Test.ReadOnly = true;
			this.Test.Resizable = System.Windows.Forms.DataGridViewTriState.True;
			this.Test.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable;
			this.Test.Width = 23;
			// 
			// SoundEnable
			// 
			dataGridViewCellStyle9.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleCenter;
			dataGridViewCellStyle9.BackColor = System.Drawing.SystemColors.ButtonFace;
			dataGridViewCellStyle9.NullValue = false;
			this.SoundEnable.DefaultCellStyle = dataGridViewCellStyle9;
			this.SoundEnable.HeaderText = "";
			this.SoundEnable.Name = "SoundEnable";
			this.SoundEnable.Width = 20;
			// 
			// labelSoundSettings
			// 
			this.labelSoundSettings.AutoColor = false;
			this.labelSoundSettings.BackColor = System.Drawing.SystemColors.ActiveCaption;
			this.labelSoundSettings.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.labelSoundSettings.DefaultColor = System.Drawing.Color.White;
			this.labelSoundSettings.DefaultText = "";
			this.labelSoundSettings.DisplayFactor = 1;
			this.labelSoundSettings.Dock = System.Windows.Forms.DockStyle.Left;
			this.labelSoundSettings.DownColor = System.Drawing.Color.Red;
			this.labelSoundSettings.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.labelSoundSettings.ForeColor = System.Drawing.SystemColors.ActiveCaptionText;
			this.labelSoundSettings.Is64Th = false;
			this.labelSoundSettings.IsCurrency = false;
			this.labelSoundSettings.IsFraction = false;
			this.labelSoundSettings.IsPercent = false;
			this.labelSoundSettings.Location = new System.Drawing.Point(0, 0);
			this.labelSoundSettings.MaxDecimal = 7;
			this.labelSoundSettings.Name = "labelSoundSettings";
			this.labelSoundSettings.RotationAngle = 270;
			this.labelSoundSettings.Size = new System.Drawing.Size(24, 0);
			this.labelSoundSettings.TabIndex = 3;
			this.labelSoundSettings.Text = "Sound Settings";
			this.labelSoundSettings.TextDirection = LabelEx.LabelRotatable.Direction.Clockwise;
			this.labelSoundSettings.TextOrientation = LabelEx.LabelRotatable.Orientation.Rotate;
			this.labelSoundSettings.TickSize = 0.01;
			this.labelSoundSettings.UpColor = System.Drawing.Color.LimeGreen;
			this.labelSoundSettings.Value = "";
			// 
			// managedPanelOrderSettings
			// 
			this.managedPanelOrderSettings.BackColor = System.Drawing.SystemColors.ControlDarkDark;
			this.managedPanelOrderSettings.Controls.Add(this.panelArgPriceResolution);
			this.managedPanelOrderSettings.Controls.Add(this.panelAutoFocusLast);
			this.managedPanelOrderSettings.Controls.Add(this.panelSkipGTDandGTDonAuto);
			this.managedPanelOrderSettings.Controls.Add(this.panelOptionAutoCancel);
			this.managedPanelOrderSettings.Controls.Add(this.panelFutureAutoCancel);
			this.managedPanelOrderSettings.Controls.Add(this.panelStockAutoCancel);
			this.managedPanelOrderSettings.Controls.Add(this.labelOrderSettings);
			this.managedPanelOrderSettings.Location = new System.Drawing.Point(0, 0);
			this.managedPanelOrderSettings.Name = "managedPanelOrderSettings";
			this.managedPanelOrderSettings.Padding = new System.Windows.Forms.Padding(2);
			this.managedPanelOrderSettings.Size = new System.Drawing.Size(0, 0);
			this.managedPanelOrderSettings.Text = "managedPanelOrderSettings";
			// 
			// panelArgPriceResolution
			// 
			this.panelArgPriceResolution.BackColor = System.Drawing.SystemColors.ControlDark;
			this.panelArgPriceResolution.Controls.Add(this.cboAvgPriceResolution);
			this.panelArgPriceResolution.Controls.Add(this.lblAvePriceResolution);
			this.panelArgPriceResolution.Dock = System.Windows.Forms.DockStyle.Top;
			this.panelArgPriceResolution.Location = new System.Drawing.Point(26, 122);
			this.panelArgPriceResolution.Name = "panelArgPriceResolution";
			this.panelArgPriceResolution.Padding = new System.Windows.Forms.Padding(2);
			this.panelArgPriceResolution.Size = new System.Drawing.Size(0, 24);
			this.panelArgPriceResolution.TabIndex = 10;
			// 
			// cboAvgPriceResolution
			// 
			this.cboAvgPriceResolution.Dock = System.Windows.Forms.DockStyle.Left;
			this.cboAvgPriceResolution.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cboAvgPriceResolution.FormattingEnabled = true;
			this.cboAvgPriceResolution.Items.AddRange(new object[] {
            "0",
            "1",
            "2",
            "3",
            "4",
            "5",
            "6",
            "7"});
			this.cboAvgPriceResolution.Location = new System.Drawing.Point(147, 2);
			this.cboAvgPriceResolution.Name = "cboAvgPriceResolution";
			this.cboAvgPriceResolution.Size = new System.Drawing.Size(40, 21);
			this.cboAvgPriceResolution.TabIndex = 8;
			// 
			// lblAvePriceResolution
			// 
			this.lblAvePriceResolution.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblAvePriceResolution.ForeColor = System.Drawing.Color.White;
			this.lblAvePriceResolution.Location = new System.Drawing.Point(2, 2);
			this.lblAvePriceResolution.Name = "lblAvePriceResolution";
			this.lblAvePriceResolution.Size = new System.Drawing.Size(145, 20);
			this.lblAvePriceResolution.TabIndex = 7;
			this.lblAvePriceResolution.Text = "Average Price Resolution :";
			this.lblAvePriceResolution.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// panelAutoFocusLast
			// 
			this.panelAutoFocusLast.Controls.Add(this.chkOrderAutoFocusLast);
			this.panelAutoFocusLast.Controls.Add(this.lblAutoFocusLast);
			this.panelAutoFocusLast.Dock = System.Windows.Forms.DockStyle.Top;
			this.panelAutoFocusLast.Location = new System.Drawing.Point(26, 98);
			this.panelAutoFocusLast.Name = "panelAutoFocusLast";
			this.panelAutoFocusLast.Padding = new System.Windows.Forms.Padding(2);
			this.panelAutoFocusLast.Size = new System.Drawing.Size(0, 24);
			this.panelAutoFocusLast.TabIndex = 9;
			// 
			// chkOrderAutoFocusLast
			// 
			this.chkOrderAutoFocusLast.Dock = System.Windows.Forms.DockStyle.Right;
			this.chkOrderAutoFocusLast.ForeColor = System.Drawing.Color.White;
			this.chkOrderAutoFocusLast.Location = new System.Drawing.Point(-75, 2);
			this.chkOrderAutoFocusLast.Name = "chkOrderAutoFocusLast";
			this.chkOrderAutoFocusLast.Size = new System.Drawing.Size(73, 20);
			this.chkOrderAutoFocusLast.TabIndex = 9;
			this.chkOrderAutoFocusLast.Text = "Enable";
			this.chkOrderAutoFocusLast.UseVisualStyleBackColor = true;
			// 
			// lblAutoFocusLast
			// 
			this.lblAutoFocusLast.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblAutoFocusLast.ForeColor = System.Drawing.Color.White;
			this.lblAutoFocusLast.Location = new System.Drawing.Point(2, 2);
			this.lblAutoFocusLast.Name = "lblAutoFocusLast";
			this.lblAutoFocusLast.Size = new System.Drawing.Size(145, 20);
			this.lblAutoFocusLast.TabIndex = 7;
			this.lblAutoFocusLast.Text = "Auto Focus Last :";
			this.lblAutoFocusLast.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// panelSkipGTDandGTDonAuto
			// 
			this.panelSkipGTDandGTDonAuto.BackColor = System.Drawing.SystemColors.ControlDark;
			this.panelSkipGTDandGTDonAuto.Controls.Add(this.chkSkipGTCandGTDonAuto);
			this.panelSkipGTDandGTDonAuto.Controls.Add(this.lblSkipGTCandGTDonAuto);
			this.panelSkipGTDandGTDonAuto.Dock = System.Windows.Forms.DockStyle.Top;
			this.panelSkipGTDandGTDonAuto.Location = new System.Drawing.Point(26, 74);
			this.panelSkipGTDandGTDonAuto.Name = "panelSkipGTDandGTDonAuto";
			this.panelSkipGTDandGTDonAuto.Padding = new System.Windows.Forms.Padding(2);
			this.panelSkipGTDandGTDonAuto.Size = new System.Drawing.Size(0, 24);
			this.panelSkipGTDandGTDonAuto.TabIndex = 13;
			// 
			// chkSkipGTCandGTDonAuto
			// 
			this.chkSkipGTCandGTDonAuto.Dock = System.Windows.Forms.DockStyle.Right;
			this.chkSkipGTCandGTDonAuto.ForeColor = System.Drawing.Color.White;
			this.chkSkipGTCandGTDonAuto.Location = new System.Drawing.Point(-75, 2);
			this.chkSkipGTCandGTDonAuto.Name = "chkSkipGTCandGTDonAuto";
			this.chkSkipGTCandGTDonAuto.Size = new System.Drawing.Size(73, 20);
			this.chkSkipGTCandGTDonAuto.TabIndex = 9;
			this.chkSkipGTCandGTDonAuto.Text = "Enable";
			this.chkSkipGTCandGTDonAuto.UseVisualStyleBackColor = true;
			// 
			// lblSkipGTCandGTDonAuto
			// 
			this.lblSkipGTCandGTDonAuto.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblSkipGTCandGTDonAuto.ForeColor = System.Drawing.Color.White;
			this.lblSkipGTCandGTDonAuto.Location = new System.Drawing.Point(2, 2);
			this.lblSkipGTCandGTDonAuto.Name = "lblSkipGTCandGTDonAuto";
			this.lblSkipGTCandGTDonAuto.Size = new System.Drawing.Size(145, 20);
			this.lblSkipGTCandGTDonAuto.TabIndex = 7;
			this.lblSkipGTCandGTDonAuto.Text = "Skip GTC and GTD on Auto:";
			this.lblSkipGTCandGTDonAuto.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// panelOptionAutoCancel
			// 
			this.panelOptionAutoCancel.Controls.Add(this.chkOptionAutoCancel);
			this.panelOptionAutoCancel.Controls.Add(this.dtpOptionAutoCancel);
			this.panelOptionAutoCancel.Controls.Add(this.lblOptionAutoCancel);
			this.panelOptionAutoCancel.Dock = System.Windows.Forms.DockStyle.Top;
			this.panelOptionAutoCancel.Location = new System.Drawing.Point(26, 50);
			this.panelOptionAutoCancel.Name = "panelOptionAutoCancel";
			this.panelOptionAutoCancel.Padding = new System.Windows.Forms.Padding(2);
			this.panelOptionAutoCancel.Size = new System.Drawing.Size(0, 24);
			this.panelOptionAutoCancel.TabIndex = 12;
			// 
			// chkOptionAutoCancel
			// 
			this.chkOptionAutoCancel.Dock = System.Windows.Forms.DockStyle.Right;
			this.chkOptionAutoCancel.ForeColor = System.Drawing.Color.White;
			this.chkOptionAutoCancel.Location = new System.Drawing.Point(-75, 2);
			this.chkOptionAutoCancel.Name = "chkOptionAutoCancel";
			this.chkOptionAutoCancel.Size = new System.Drawing.Size(73, 20);
			this.chkOptionAutoCancel.TabIndex = 9;
			this.chkOptionAutoCancel.Text = "Enable";
			this.chkOptionAutoCancel.UseVisualStyleBackColor = true;
			// 
			// dtpOptionAutoCancel
			// 
			this.dtpOptionAutoCancel.CustomFormat = "HH:mm:ss";
			this.dtpOptionAutoCancel.Dock = System.Windows.Forms.DockStyle.Left;
			this.dtpOptionAutoCancel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dtpOptionAutoCancel.Format = System.Windows.Forms.DateTimePickerFormat.Custom;
			this.dtpOptionAutoCancel.Location = new System.Drawing.Point(147, 2);
			this.dtpOptionAutoCancel.Name = "dtpOptionAutoCancel";
			this.dtpOptionAutoCancel.ShowUpDown = true;
			this.dtpOptionAutoCancel.Size = new System.Drawing.Size(79, 20);
			this.dtpOptionAutoCancel.TabIndex = 0;
			this.dtpOptionAutoCancel.Value = new System.DateTime(1753, 1, 1, 1, 0, 0, 0);
			// 
			// lblOptionAutoCancel
			// 
			this.lblOptionAutoCancel.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblOptionAutoCancel.ForeColor = System.Drawing.Color.White;
			this.lblOptionAutoCancel.Location = new System.Drawing.Point(2, 2);
			this.lblOptionAutoCancel.Name = "lblOptionAutoCancel";
			this.lblOptionAutoCancel.Size = new System.Drawing.Size(145, 20);
			this.lblOptionAutoCancel.TabIndex = 7;
			this.lblOptionAutoCancel.Text = "Option Auto Cancel Time :";
			this.lblOptionAutoCancel.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// panelFutureAutoCancel
			// 
			this.panelFutureAutoCancel.BackColor = System.Drawing.SystemColors.ControlDark;
			this.panelFutureAutoCancel.Controls.Add(this.chkFutureAutoCancel);
			this.panelFutureAutoCancel.Controls.Add(this.dtpFutureAutoCancel);
			this.panelFutureAutoCancel.Controls.Add(this.lblFutureAutoCancel);
			this.panelFutureAutoCancel.Dock = System.Windows.Forms.DockStyle.Top;
			this.panelFutureAutoCancel.Location = new System.Drawing.Point(26, 26);
			this.panelFutureAutoCancel.Name = "panelFutureAutoCancel";
			this.panelFutureAutoCancel.Padding = new System.Windows.Forms.Padding(2);
			this.panelFutureAutoCancel.Size = new System.Drawing.Size(0, 24);
			this.panelFutureAutoCancel.TabIndex = 11;
			// 
			// chkFutureAutoCancel
			// 
			this.chkFutureAutoCancel.Dock = System.Windows.Forms.DockStyle.Right;
			this.chkFutureAutoCancel.ForeColor = System.Drawing.Color.White;
			this.chkFutureAutoCancel.Location = new System.Drawing.Point(-75, 2);
			this.chkFutureAutoCancel.Name = "chkFutureAutoCancel";
			this.chkFutureAutoCancel.Size = new System.Drawing.Size(73, 20);
			this.chkFutureAutoCancel.TabIndex = 9;
			this.chkFutureAutoCancel.Text = "Enable";
			this.chkFutureAutoCancel.UseVisualStyleBackColor = true;
			// 
			// dtpFutureAutoCancel
			// 
			this.dtpFutureAutoCancel.CustomFormat = "HH:mm:ss";
			this.dtpFutureAutoCancel.Dock = System.Windows.Forms.DockStyle.Left;
			this.dtpFutureAutoCancel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dtpFutureAutoCancel.Format = System.Windows.Forms.DateTimePickerFormat.Custom;
			this.dtpFutureAutoCancel.Location = new System.Drawing.Point(147, 2);
			this.dtpFutureAutoCancel.Name = "dtpFutureAutoCancel";
			this.dtpFutureAutoCancel.ShowUpDown = true;
			this.dtpFutureAutoCancel.Size = new System.Drawing.Size(79, 20);
			this.dtpFutureAutoCancel.TabIndex = 0;
			this.dtpFutureAutoCancel.Value = new System.DateTime(1753, 1, 1, 1, 0, 0, 0);
			// 
			// lblFutureAutoCancel
			// 
			this.lblFutureAutoCancel.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblFutureAutoCancel.ForeColor = System.Drawing.Color.White;
			this.lblFutureAutoCancel.Location = new System.Drawing.Point(2, 2);
			this.lblFutureAutoCancel.Name = "lblFutureAutoCancel";
			this.lblFutureAutoCancel.Size = new System.Drawing.Size(145, 20);
			this.lblFutureAutoCancel.TabIndex = 7;
			this.lblFutureAutoCancel.Text = "Future Auto Cancel Time :";
			this.lblFutureAutoCancel.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// panelStockAutoCancel
			// 
			this.panelStockAutoCancel.Controls.Add(this.chkStockAutoCancel);
			this.panelStockAutoCancel.Controls.Add(this.dtpStockAutoCancel);
			this.panelStockAutoCancel.Controls.Add(this.lblStockAutoCancel);
			this.panelStockAutoCancel.Dock = System.Windows.Forms.DockStyle.Top;
			this.panelStockAutoCancel.Location = new System.Drawing.Point(26, 2);
			this.panelStockAutoCancel.Name = "panelStockAutoCancel";
			this.panelStockAutoCancel.Padding = new System.Windows.Forms.Padding(2);
			this.panelStockAutoCancel.Size = new System.Drawing.Size(0, 24);
			this.panelStockAutoCancel.TabIndex = 8;
			// 
			// chkStockAutoCancel
			// 
			this.chkStockAutoCancel.Dock = System.Windows.Forms.DockStyle.Right;
			this.chkStockAutoCancel.ForeColor = System.Drawing.Color.White;
			this.chkStockAutoCancel.Location = new System.Drawing.Point(-75, 2);
			this.chkStockAutoCancel.Name = "chkStockAutoCancel";
			this.chkStockAutoCancel.Size = new System.Drawing.Size(73, 20);
			this.chkStockAutoCancel.TabIndex = 9;
			this.chkStockAutoCancel.Text = "Enable";
			this.chkStockAutoCancel.UseVisualStyleBackColor = true;
			// 
			// dtpStockAutoCancel
			// 
			this.dtpStockAutoCancel.CustomFormat = "HH:mm:ss";
			this.dtpStockAutoCancel.Dock = System.Windows.Forms.DockStyle.Left;
			this.dtpStockAutoCancel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dtpStockAutoCancel.Format = System.Windows.Forms.DateTimePickerFormat.Custom;
			this.dtpStockAutoCancel.Location = new System.Drawing.Point(147, 2);
			this.dtpStockAutoCancel.Name = "dtpStockAutoCancel";
			this.dtpStockAutoCancel.ShowUpDown = true;
			this.dtpStockAutoCancel.Size = new System.Drawing.Size(79, 20);
			this.dtpStockAutoCancel.TabIndex = 0;
			this.dtpStockAutoCancel.Value = new System.DateTime(1753, 1, 1, 1, 0, 0, 0);
			// 
			// lblStockAutoCancel
			// 
			this.lblStockAutoCancel.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblStockAutoCancel.ForeColor = System.Drawing.Color.White;
			this.lblStockAutoCancel.Location = new System.Drawing.Point(2, 2);
			this.lblStockAutoCancel.Name = "lblStockAutoCancel";
			this.lblStockAutoCancel.Size = new System.Drawing.Size(145, 20);
			this.lblStockAutoCancel.TabIndex = 7;
			this.lblStockAutoCancel.Text = "Stock Auto Cancel Time :";
			this.lblStockAutoCancel.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// labelOrderSettings
			// 
			this.labelOrderSettings.AutoColor = false;
			this.labelOrderSettings.BackColor = System.Drawing.SystemColors.ActiveCaption;
			this.labelOrderSettings.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.labelOrderSettings.DefaultColor = System.Drawing.Color.White;
			this.labelOrderSettings.DefaultText = "";
			this.labelOrderSettings.DisplayFactor = 1;
			this.labelOrderSettings.Dock = System.Windows.Forms.DockStyle.Left;
			this.labelOrderSettings.DownColor = System.Drawing.Color.Red;
			this.labelOrderSettings.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.labelOrderSettings.ForeColor = System.Drawing.SystemColors.ActiveCaptionText;
			this.labelOrderSettings.Is64Th = false;
			this.labelOrderSettings.IsCurrency = false;
			this.labelOrderSettings.IsFraction = false;
			this.labelOrderSettings.IsPercent = false;
			this.labelOrderSettings.Location = new System.Drawing.Point(2, 2);
			this.labelOrderSettings.MaxDecimal = 7;
			this.labelOrderSettings.Name = "labelOrderSettings";
			this.labelOrderSettings.RotationAngle = 270;
			this.labelOrderSettings.Size = new System.Drawing.Size(24, 0);
			this.labelOrderSettings.TabIndex = 5;
			this.labelOrderSettings.Text = "Orders Settings";
			this.labelOrderSettings.TextDirection = LabelEx.LabelRotatable.Direction.Clockwise;
			this.labelOrderSettings.TextOrientation = LabelEx.LabelRotatable.Orientation.Rotate;
			this.labelOrderSettings.TickSize = 0.01;
			this.labelOrderSettings.UpColor = System.Drawing.Color.LimeGreen;
			this.labelOrderSettings.Value = "";
			// 
			// managedPanelQuickButtonSettings
			// 
			this.managedPanelQuickButtonSettings.Controls.Add(this.rocQuickButtonList);
			this.managedPanelQuickButtonSettings.Controls.Add(this.panelQuickButtonSelection);
			this.managedPanelQuickButtonSettings.Controls.Add(this.labelQuickButtonSettings);
			this.managedPanelQuickButtonSettings.Location = new System.Drawing.Point(0, 0);
			this.managedPanelQuickButtonSettings.Name = "managedPanelQuickButtonSettings";
			this.managedPanelQuickButtonSettings.Size = new System.Drawing.Size(0, 0);
			this.managedPanelQuickButtonSettings.Text = "managedPanelQuickButtonSettings";
			// 
			// rocQuickButtonList
			// 
			this.rocQuickButtonList.BlueDefault = 228;
			this.rocQuickButtonList.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
			this.rocQuickButtonList.CurrentSecType = "E";
			new BindingListEx.ROCBindingList<string>().Add("");
			this.rocQuickButtonList.DisplayFactors = ((System.Collections.Generic.Dictionary<string, double>)(resources.GetObject("rocQuickButtonList.DisplayFactors")));
			this.rocQuickButtonList.Dock = System.Windows.Forms.DockStyle.Fill;
			this.rocQuickButtonList.GreenDefault = 243;
			this.rocQuickButtonList.GridKeys = ((System.Collections.Generic.List<string>)(resources.GetObject("rocQuickButtonList.GridKeys")));
			this.rocQuickButtonList.HeaderBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(233)))), ((int)(((byte)(243)))), ((int)(((byte)(228)))));
			this.rocQuickButtonList.Importing = false;
			this.rocQuickButtonList.LastFilter = "";
			this.rocQuickButtonList.LastSort = "";
			this.rocQuickButtonList.Location = new System.Drawing.Point(24, 24);
			this.rocQuickButtonList.Name = "rocQuickButtonList";
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
			this.rocQuickButtonList.Printer = dgvBasePrinter2;
			this.rocQuickButtonList.Processing = false;
			this.rocQuickButtonList.RedDefault = 233;
			this.rocQuickButtonList.RefreshAggragation = true;
			this.rocQuickButtonList.RefreshSharedRows = false;
			this.rocQuickButtonList.ShouldScrollToLastRow = false;
			this.rocQuickButtonList.Size = new System.Drawing.Size(0, 0);
			new BindingListEx.ROCBindingList<string>().Add("");
			this.rocQuickButtonList.TabIndex = 12;
			this.rocQuickButtonList.TickSizes = ((System.Collections.Generic.Dictionary<string, double>)(resources.GetObject("rocQuickButtonList.TickSizes")));
			// 
			// panelQuickButtonSelection
			// 
			this.panelQuickButtonSelection.BackColor = System.Drawing.Color.Black;
			this.panelQuickButtonSelection.Controls.Add(this.cmdOptionTicket);
			this.panelQuickButtonSelection.Controls.Add(this.cmdFutureTicket);
			this.panelQuickButtonSelection.Controls.Add(this.cmdStockTicket);
			this.panelQuickButtonSelection.Dock = System.Windows.Forms.DockStyle.Top;
			this.panelQuickButtonSelection.Location = new System.Drawing.Point(24, 0);
			this.panelQuickButtonSelection.Name = "panelQuickButtonSelection";
			this.panelQuickButtonSelection.Size = new System.Drawing.Size(0, 24);
			this.panelQuickButtonSelection.TabIndex = 11;
			// 
			// cmdOptionTicket
			// 
			this.cmdOptionTicket.AutoSize = true;
			this.cmdOptionTicket.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdOptionTicket.BackColor = System.Drawing.Color.Goldenrod;
			this.cmdOptionTicket.Dock = System.Windows.Forms.DockStyle.Left;
			this.cmdOptionTicket.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdOptionTicket.ForeColor = System.Drawing.Color.Black;
			this.cmdOptionTicket.Location = new System.Drawing.Point(183, 0);
			this.cmdOptionTicket.Name = "cmdOptionTicket";
			this.cmdOptionTicket.Size = new System.Drawing.Size(94, 24);
			this.cmdOptionTicket.TabIndex = 12;
			this.cmdOptionTicket.Text = "Option Ticket";
			this.cmdOptionTicket.Click += new System.EventHandler(this.cmdOptionTicket_Click);
			// 
			// cmdFutureTicket
			// 
			this.cmdFutureTicket.AutoSize = true;
			this.cmdFutureTicket.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdFutureTicket.BackColor = System.Drawing.Color.Goldenrod;
			this.cmdFutureTicket.Dock = System.Windows.Forms.DockStyle.Left;
			this.cmdFutureTicket.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdFutureTicket.ForeColor = System.Drawing.Color.Black;
			this.cmdFutureTicket.Location = new System.Drawing.Point(90, 0);
			this.cmdFutureTicket.Name = "cmdFutureTicket";
			this.cmdFutureTicket.Size = new System.Drawing.Size(93, 24);
			this.cmdFutureTicket.TabIndex = 11;
			this.cmdFutureTicket.Text = "Future Ticket";
			this.cmdFutureTicket.Click += new System.EventHandler(this.cmdFutureTicket_Click);
			// 
			// cmdStockTicket
			// 
			this.cmdStockTicket.AutoSize = true;
			this.cmdStockTicket.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdStockTicket.BackColor = System.Drawing.Color.Goldenrod;
			this.cmdStockTicket.Dock = System.Windows.Forms.DockStyle.Left;
			this.cmdStockTicket.Enabled = false;
			this.cmdStockTicket.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdStockTicket.ForeColor = System.Drawing.Color.Black;
			this.cmdStockTicket.Location = new System.Drawing.Point(0, 0);
			this.cmdStockTicket.Name = "cmdStockTicket";
			this.cmdStockTicket.Size = new System.Drawing.Size(90, 24);
			this.cmdStockTicket.TabIndex = 10;
			this.cmdStockTicket.Text = "Stock Ticket";
			this.cmdStockTicket.Click += new System.EventHandler(this.cmdStockTicket_Click);
			// 
			// labelQuickButtonSettings
			// 
			this.labelQuickButtonSettings.AutoColor = false;
			this.labelQuickButtonSettings.BackColor = System.Drawing.SystemColors.ActiveCaption;
			this.labelQuickButtonSettings.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.labelQuickButtonSettings.DefaultColor = System.Drawing.Color.White;
			this.labelQuickButtonSettings.DefaultText = "";
			this.labelQuickButtonSettings.DisplayFactor = 1;
			this.labelQuickButtonSettings.Dock = System.Windows.Forms.DockStyle.Left;
			this.labelQuickButtonSettings.DownColor = System.Drawing.Color.Red;
			this.labelQuickButtonSettings.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.labelQuickButtonSettings.ForeColor = System.Drawing.SystemColors.ActiveCaptionText;
			this.labelQuickButtonSettings.Is64Th = false;
			this.labelQuickButtonSettings.IsCurrency = false;
			this.labelQuickButtonSettings.IsFraction = false;
			this.labelQuickButtonSettings.IsPercent = false;
			this.labelQuickButtonSettings.Location = new System.Drawing.Point(0, 0);
			this.labelQuickButtonSettings.MaxDecimal = 7;
			this.labelQuickButtonSettings.Name = "labelQuickButtonSettings";
			this.labelQuickButtonSettings.RotationAngle = 270;
			this.labelQuickButtonSettings.Size = new System.Drawing.Size(24, 0);
			this.labelQuickButtonSettings.TabIndex = 4;
			this.labelQuickButtonSettings.Text = "Quick Button Settings";
			this.labelQuickButtonSettings.TextDirection = LabelEx.LabelRotatable.Direction.Clockwise;
			this.labelQuickButtonSettings.TextOrientation = LabelEx.LabelRotatable.Orientation.Rotate;
			this.labelQuickButtonSettings.TickSize = 0.01;
			this.labelQuickButtonSettings.UpColor = System.Drawing.Color.LimeGreen;
			this.labelQuickButtonSettings.Value = "";
			// 
			// managedPanelProcessSettings
			// 
			this.managedPanelProcessSettings.BackColor = System.Drawing.SystemColors.ControlDarkDark;
			this.managedPanelProcessSettings.Controls.Add(this.panel1);
			this.managedPanelProcessSettings.Controls.Add(this.panel2);
			this.managedPanelProcessSettings.Controls.Add(this.panelAdmin);
			this.managedPanelProcessSettings.Controls.Add(this.panelUIUpdateRate);
			this.managedPanelProcessSettings.Controls.Add(this.panelCSVThreadPriority);
			this.managedPanelProcessSettings.Controls.Add(this.panelMDThreadPriority);
			this.managedPanelProcessSettings.Controls.Add(this.lblProcessSettings);
			this.managedPanelProcessSettings.Location = new System.Drawing.Point(0, 0);
			this.managedPanelProcessSettings.Name = "managedPanelProcessSettings";
			this.managedPanelProcessSettings.Size = new System.Drawing.Size(404, 353);
			this.managedPanelProcessSettings.Text = "managedPanelProcessSettings";
			// 
			// panel1
			// 
			this.panel1.Controls.Add(this.dspMDSBackup);
			this.panel1.Controls.Add(this.lblMDSBackup);
			this.panel1.Controls.Add(this.chkMDSBackup);
			this.panel1.Dock = System.Windows.Forms.DockStyle.Top;
			this.panel1.Location = new System.Drawing.Point(24, 144);
			this.panel1.Name = "panel1";
			this.panel1.Padding = new System.Windows.Forms.Padding(2);
			this.panel1.Size = new System.Drawing.Size(380, 48);
			this.panel1.TabIndex = 15;
			// 
			// dspMDSBackup
			// 
			this.dspMDSBackup.Dock = System.Windows.Forms.DockStyle.Left;
			this.dspMDSBackup.ForeColor = System.Drawing.Color.Olive;
			this.dspMDSBackup.Location = new System.Drawing.Point(150, 2);
			this.dspMDSBackup.Name = "dspMDSBackup";
			this.dspMDSBackup.Size = new System.Drawing.Size(99, 44);
			this.dspMDSBackup.TabIndex = 9;
			this.dspMDSBackup.Text = "xxx.xxx.xxx.xxx";
			this.dspMDSBackup.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			// 
			// lblMDSBackup
			// 
			this.lblMDSBackup.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblMDSBackup.ForeColor = System.Drawing.Color.White;
			this.lblMDSBackup.Location = new System.Drawing.Point(51, 2);
			this.lblMDSBackup.Name = "lblMDSBackup";
			this.lblMDSBackup.Size = new System.Drawing.Size(99, 44);
			this.lblMDSBackup.TabIndex = 8;
			this.lblMDSBackup.Text = "MDS Bakcup : ";
			this.lblMDSBackup.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// chkMDSBackup
			// 
			this.chkMDSBackup.CheckAlign = System.Drawing.ContentAlignment.TopCenter;
			this.chkMDSBackup.Dock = System.Windows.Forms.DockStyle.Left;
			this.chkMDSBackup.Enabled = false;
			this.chkMDSBackup.ForeColor = System.Drawing.Color.White;
			this.chkMDSBackup.Location = new System.Drawing.Point(2, 2);
			this.chkMDSBackup.Name = "chkMDSBackup";
			this.chkMDSBackup.Padding = new System.Windows.Forms.Padding(0, 6, 0, 3);
			this.chkMDSBackup.Size = new System.Drawing.Size(49, 44);
			this.chkMDSBackup.TabIndex = 7;
			this.chkMDSBackup.Text = "Enable";
			this.chkMDSBackup.TextAlign = System.Drawing.ContentAlignment.BottomCenter;
			this.chkMDSBackup.UseVisualStyleBackColor = true;
			// 
			// panel2
			// 
			this.panel2.BackColor = System.Drawing.SystemColors.ButtonShadow;
			this.panel2.Controls.Add(this.dspMDS);
			this.panel2.Controls.Add(this.lblMDS);
			this.panel2.Controls.Add(this.chkMDS);
			this.panel2.Dock = System.Windows.Forms.DockStyle.Top;
			this.panel2.Location = new System.Drawing.Point(24, 96);
			this.panel2.Name = "panel2";
			this.panel2.Padding = new System.Windows.Forms.Padding(2);
			this.panel2.Size = new System.Drawing.Size(380, 48);
			this.panel2.TabIndex = 16;
			// 
			// dspMDS
			// 
			this.dspMDS.Dock = System.Windows.Forms.DockStyle.Left;
			this.dspMDS.ForeColor = System.Drawing.SystemColors.GradientActiveCaption;
			this.dspMDS.Location = new System.Drawing.Point(150, 2);
			this.dspMDS.Name = "dspMDS";
			this.dspMDS.Size = new System.Drawing.Size(99, 44);
			this.dspMDS.TabIndex = 9;
			this.dspMDS.Text = "xxx.xxx.xxx.xxx";
			this.dspMDS.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			// 
			// lblMDS
			// 
			this.lblMDS.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblMDS.ForeColor = System.Drawing.Color.White;
			this.lblMDS.Location = new System.Drawing.Point(51, 2);
			this.lblMDS.Name = "lblMDS";
			this.lblMDS.Size = new System.Drawing.Size(99, 44);
			this.lblMDS.TabIndex = 8;
			this.lblMDS.Text = "MDS : ";
			this.lblMDS.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// chkMDS
			// 
			this.chkMDS.CheckAlign = System.Drawing.ContentAlignment.TopCenter;
			this.chkMDS.Dock = System.Windows.Forms.DockStyle.Left;
			this.chkMDS.Enabled = false;
			this.chkMDS.ForeColor = System.Drawing.Color.White;
			this.chkMDS.Location = new System.Drawing.Point(2, 2);
			this.chkMDS.Name = "chkMDS";
			this.chkMDS.Padding = new System.Windows.Forms.Padding(0, 6, 0, 3);
			this.chkMDS.Size = new System.Drawing.Size(49, 44);
			this.chkMDS.TabIndex = 7;
			this.chkMDS.Text = "Enable";
			this.chkMDS.TextAlign = System.Drawing.ContentAlignment.BottomCenter;
			this.chkMDS.UseVisualStyleBackColor = true;
			// 
			// panelAdmin
			// 
			this.panelAdmin.BackColor = System.Drawing.Color.Sienna;
			this.panelAdmin.Controls.Add(this.cboResubscribeRate);
			this.panelAdmin.Controls.Add(lblResubscribeRate);
			this.panelAdmin.Controls.Add(this.panelResubscribe);
			this.panelAdmin.Controls.Add(lblAdminTools);
			this.panelAdmin.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.panelAdmin.Location = new System.Drawing.Point(24, 305);
			this.panelAdmin.Name = "panelAdmin";
			this.panelAdmin.Size = new System.Drawing.Size(380, 48);
			this.panelAdmin.TabIndex = 14;
			this.panelAdmin.Visible = false;
			// 
			// cboResubscribeRate
			// 
			this.cboResubscribeRate.Dock = System.Windows.Forms.DockStyle.Left;
			this.cboResubscribeRate.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cboResubscribeRate.FormattingEnabled = true;
			this.cboResubscribeRate.Items.AddRange(new object[] {
            "25",
            "50",
            "100",
            "500",
            "1000",
            "5000"});
			this.cboResubscribeRate.Location = new System.Drawing.Point(107, 24);
			this.cboResubscribeRate.Name = "cboResubscribeRate";
			this.cboResubscribeRate.Size = new System.Drawing.Size(43, 21);
			this.cboResubscribeRate.TabIndex = 19;
			this.cboResubscribeRate.Text = "50";
			this.cboResubscribeRate.SelectedIndexChanged += new System.EventHandler(this.cboResubscribeRate_SelectedIndexChanged);
			// 
			// panelResubscribe
			// 
			this.panelResubscribe.Controls.Add(this.cmdResubscrbe);
			this.panelResubscribe.Controls.Add(this.lblResubscribeStatus);
			this.panelResubscribe.Controls.Add(this.txtSourceFilter);
			this.panelResubscribe.Controls.Add(lblResubscribeSourceFilter);
			this.panelResubscribe.Dock = System.Windows.Forms.DockStyle.Top;
			this.panelResubscribe.Location = new System.Drawing.Point(0, 0);
			this.panelResubscribe.Name = "panelResubscribe";
			this.panelResubscribe.Padding = new System.Windows.Forms.Padding(2);
			this.panelResubscribe.Size = new System.Drawing.Size(380, 24);
			this.panelResubscribe.TabIndex = 16;
			// 
			// cmdResubscrbe
			// 
			this.cmdResubscrbe.BackColor = System.Drawing.Color.Black;
			this.cmdResubscrbe.Dock = System.Windows.Forms.DockStyle.Right;
			this.cmdResubscrbe.Location = new System.Drawing.Point(290, 2);
			this.cmdResubscrbe.Name = "cmdResubscrbe";
			this.cmdResubscrbe.Size = new System.Drawing.Size(88, 20);
			this.cmdResubscrbe.TabIndex = 12;
			this.cmdResubscrbe.Text = "Re-Subscribe";
			this.cmdResubscrbe.Click += new System.EventHandler(this.cmdResubscrib_Click);
			// 
			// lblResubscribeStatus
			// 
			this.lblResubscribeStatus.AutoColor = false;
			this.lblResubscribeStatus.DefaultColor = System.Drawing.Color.White;
			this.lblResubscribeStatus.DefaultText = "";
			this.lblResubscribeStatus.DisplayFactor = 1;
			this.lblResubscribeStatus.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblResubscribeStatus.DownColor = System.Drawing.Color.Red;
			this.lblResubscribeStatus.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblResubscribeStatus.ForeColor = System.Drawing.Color.White;
			this.lblResubscribeStatus.Is64Th = false;
			this.lblResubscribeStatus.IsCurrency = false;
			this.lblResubscribeStatus.IsFraction = false;
			this.lblResubscribeStatus.IsPercent = false;
			this.lblResubscribeStatus.Location = new System.Drawing.Point(171, 2);
			this.lblResubscribeStatus.MaxDecimal = 7;
			this.lblResubscribeStatus.Name = "lblResubscribeStatus";
			this.lblResubscribeStatus.Size = new System.Drawing.Size(128, 20);
			this.lblResubscribeStatus.TabIndex = 13;
			this.lblResubscribeStatus.Text = "000/000/000";
			this.lblResubscribeStatus.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			this.lblResubscribeStatus.TickSize = 0.01;
			this.lblResubscribeStatus.UpColor = System.Drawing.Color.LimeGreen;
			this.lblResubscribeStatus.Value = "";
			// 
			// txtSourceFilter
			// 
			this.txtSourceFilter.Dock = System.Windows.Forms.DockStyle.Left;
			this.txtSourceFilter.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.txtSourceFilter.Location = new System.Drawing.Point(81, 2);
			this.txtSourceFilter.Name = "txtSourceFilter";
			this.txtSourceFilter.Size = new System.Drawing.Size(90, 20);
			this.txtSourceFilter.TabIndex = 14;
			// 
			// panelUIUpdateRate
			// 
			this.panelUIUpdateRate.Controls.Add(this.panelTrackBarUI);
			this.panelUIUpdateRate.Controls.Add(this.lblUIUpdateRate);
			this.panelUIUpdateRate.Controls.Add(this.chkUserUIUpdateRate);
			this.panelUIUpdateRate.Dock = System.Windows.Forms.DockStyle.Top;
			this.panelUIUpdateRate.Location = new System.Drawing.Point(24, 48);
			this.panelUIUpdateRate.Name = "panelUIUpdateRate";
			this.panelUIUpdateRate.Padding = new System.Windows.Forms.Padding(2);
			this.panelUIUpdateRate.Size = new System.Drawing.Size(380, 48);
			this.panelUIUpdateRate.TabIndex = 11;
			// 
			// panelTrackBarUI
			// 
			this.panelTrackBarUI.Controls.Add(this.lbl250ms);
			this.panelTrackBarUI.Controls.Add(this.lbl200ms);
			this.panelTrackBarUI.Controls.Add(this.lbl150ms);
			this.panelTrackBarUI.Controls.Add(this.lbl100ms);
			this.panelTrackBarUI.Controls.Add(this.lbl50ms);
			this.panelTrackBarUI.Controls.Add(this.lbl0ms);
			this.panelTrackBarUI.Controls.Add(this.trackBarUIUpdateRate);
			this.panelTrackBarUI.Dock = System.Windows.Forms.DockStyle.Fill;
			this.panelTrackBarUI.Location = new System.Drawing.Point(150, 2);
			this.panelTrackBarUI.Name = "panelTrackBarUI";
			this.panelTrackBarUI.Size = new System.Drawing.Size(228, 44);
			this.panelTrackBarUI.TabIndex = 8;
			// 
			// lbl250ms
			// 
			this.lbl250ms.AutoSize = true;
			this.lbl250ms.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lbl250ms.ForeColor = System.Drawing.Color.White;
			this.lbl250ms.Location = new System.Drawing.Point(198, 32);
			this.lbl250ms.Name = "lbl250ms";
			this.lbl250ms.Size = new System.Drawing.Size(25, 12);
			this.lbl250ms.TabIndex = 13;
			this.lbl250ms.Text = "1000";
			// 
			// lbl200ms
			// 
			this.lbl200ms.AutoSize = true;
			this.lbl200ms.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lbl200ms.ForeColor = System.Drawing.Color.White;
			this.lbl200ms.Location = new System.Drawing.Point(163, 32);
			this.lbl200ms.Name = "lbl200ms";
			this.lbl200ms.Size = new System.Drawing.Size(20, 12);
			this.lbl200ms.TabIndex = 12;
			this.lbl200ms.Text = "800";
			// 
			// lbl150ms
			// 
			this.lbl150ms.AutoSize = true;
			this.lbl150ms.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lbl150ms.ForeColor = System.Drawing.Color.White;
			this.lbl150ms.Location = new System.Drawing.Point(124, 32);
			this.lbl150ms.Name = "lbl150ms";
			this.lbl150ms.Size = new System.Drawing.Size(20, 12);
			this.lbl150ms.TabIndex = 11;
			this.lbl150ms.Text = "600";
			// 
			// lbl100ms
			// 
			this.lbl100ms.AutoSize = true;
			this.lbl100ms.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lbl100ms.ForeColor = System.Drawing.Color.White;
			this.lbl100ms.Location = new System.Drawing.Point(83, 32);
			this.lbl100ms.Name = "lbl100ms";
			this.lbl100ms.Size = new System.Drawing.Size(20, 12);
			this.lbl100ms.TabIndex = 10;
			this.lbl100ms.Text = "400";
			// 
			// lbl50ms
			// 
			this.lbl50ms.AutoSize = true;
			this.lbl50ms.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lbl50ms.ForeColor = System.Drawing.Color.White;
			this.lbl50ms.Location = new System.Drawing.Point(43, 32);
			this.lbl50ms.Name = "lbl50ms";
			this.lbl50ms.Size = new System.Drawing.Size(20, 12);
			this.lbl50ms.TabIndex = 9;
			this.lbl50ms.Text = "200";
			// 
			// lbl0ms
			// 
			this.lbl0ms.AutoSize = true;
			this.lbl0ms.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lbl0ms.ForeColor = System.Drawing.Color.White;
			this.lbl0ms.Location = new System.Drawing.Point(6, 32);
			this.lbl0ms.Name = "lbl0ms";
			this.lbl0ms.Size = new System.Drawing.Size(10, 12);
			this.lbl0ms.TabIndex = 8;
			this.lbl0ms.Text = "1";
			// 
			// trackBarUIUpdateRate
			// 
			this.trackBarUIUpdateRate.Dock = System.Windows.Forms.DockStyle.Left;
			this.trackBarUIUpdateRate.LargeChange = 100;
			this.trackBarUIUpdateRate.Location = new System.Drawing.Point(0, 0);
			this.trackBarUIUpdateRate.Maximum = 1000;
			this.trackBarUIUpdateRate.Minimum = 1;
			this.trackBarUIUpdateRate.Name = "trackBarUIUpdateRate";
			this.trackBarUIUpdateRate.Size = new System.Drawing.Size(225, 44);
			this.trackBarUIUpdateRate.SmallChange = 100;
			this.trackBarUIUpdateRate.TabIndex = 7;
			this.trackBarUIUpdateRate.TabStop = false;
			this.trackBarUIUpdateRate.TickFrequency = 100;
			this.trackBarUIUpdateRate.TickStyle = System.Windows.Forms.TickStyle.Both;
			this.trackBarUIUpdateRate.Value = 100;
			// 
			// lblUIUpdateRate
			// 
			this.lblUIUpdateRate.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblUIUpdateRate.ForeColor = System.Drawing.Color.White;
			this.lblUIUpdateRate.Location = new System.Drawing.Point(51, 2);
			this.lblUIUpdateRate.Name = "lblUIUpdateRate";
			this.lblUIUpdateRate.Size = new System.Drawing.Size(99, 44);
			this.lblUIUpdateRate.TabIndex = 0;
			this.lblUIUpdateRate.Text = "UI Update Rate : ";
			this.lblUIUpdateRate.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// chkUserUIUpdateRate
			// 
			this.chkUserUIUpdateRate.CheckAlign = System.Drawing.ContentAlignment.TopCenter;
			this.chkUserUIUpdateRate.Dock = System.Windows.Forms.DockStyle.Left;
			this.chkUserUIUpdateRate.ForeColor = System.Drawing.Color.White;
			this.chkUserUIUpdateRate.Location = new System.Drawing.Point(2, 2);
			this.chkUserUIUpdateRate.Name = "chkUserUIUpdateRate";
			this.chkUserUIUpdateRate.Padding = new System.Windows.Forms.Padding(0, 6, 0, 3);
			this.chkUserUIUpdateRate.Size = new System.Drawing.Size(49, 44);
			this.chkUserUIUpdateRate.TabIndex = 7;
			this.chkUserUIUpdateRate.Text = "Enable";
			this.chkUserUIUpdateRate.TextAlign = System.Drawing.ContentAlignment.BottomCenter;
			this.chkUserUIUpdateRate.UseVisualStyleBackColor = true;
			this.chkUserUIUpdateRate.CheckedChanged += new System.EventHandler(this.chkUserUIUpdateRate_CheckedChanged);
			// 
			// panelCSVThreadPriority
			// 
			this.panelCSVThreadPriority.BackColor = System.Drawing.SystemColors.ButtonShadow;
			this.panelCSVThreadPriority.Controls.Add(this.lblCSVNotice);
			this.panelCSVThreadPriority.Controls.Add(this.cboCSVThreadPriority);
			this.panelCSVThreadPriority.Controls.Add(this.lblCSVThreadPriority);
			this.panelCSVThreadPriority.Dock = System.Windows.Forms.DockStyle.Top;
			this.panelCSVThreadPriority.Location = new System.Drawing.Point(24, 24);
			this.panelCSVThreadPriority.Name = "panelCSVThreadPriority";
			this.panelCSVThreadPriority.Padding = new System.Windows.Forms.Padding(2);
			this.panelCSVThreadPriority.Size = new System.Drawing.Size(380, 24);
			this.panelCSVThreadPriority.TabIndex = 10;
			// 
			// lblCSVNotice
			// 
			this.lblCSVNotice.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblCSVNotice.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblCSVNotice.ForeColor = System.Drawing.Color.DarkRed;
			this.lblCSVNotice.Location = new System.Drawing.Point(240, 2);
			this.lblCSVNotice.Name = "lblCSVNotice";
			this.lblCSVNotice.Size = new System.Drawing.Size(80, 20);
			this.lblCSVNotice.TabIndex = 1;
			this.lblCSVNotice.Text = "Restart Required";
			this.lblCSVNotice.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// cboCSVThreadPriority
			// 
			this.cboCSVThreadPriority.Dock = System.Windows.Forms.DockStyle.Left;
			this.cboCSVThreadPriority.FormattingEnabled = true;
			this.cboCSVThreadPriority.Items.AddRange(new object[] {
            "0 Lowest",
            "1 BelowNormal",
            "2 Normal",
            "3 AboveNormal",
            "4 Highest"});
			this.cboCSVThreadPriority.Location = new System.Drawing.Point(150, 2);
			this.cboCSVThreadPriority.Name = "cboCSVThreadPriority";
			this.cboCSVThreadPriority.Size = new System.Drawing.Size(90, 21);
			this.cboCSVThreadPriority.TabIndex = 0;
			this.cboCSVThreadPriority.TabStop = false;
			// 
			// lblCSVThreadPriority
			// 
			this.lblCSVThreadPriority.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblCSVThreadPriority.ForeColor = System.Drawing.Color.White;
			this.lblCSVThreadPriority.Location = new System.Drawing.Point(2, 2);
			this.lblCSVThreadPriority.Name = "lblCSVThreadPriority";
			this.lblCSVThreadPriority.Size = new System.Drawing.Size(148, 20);
			this.lblCSVThreadPriority.TabIndex = 0;
			this.lblCSVThreadPriority.Text = "ROM Order Thread Priority : ";
			this.lblCSVThreadPriority.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// panelMDThreadPriority
			// 
			this.panelMDThreadPriority.Controls.Add(this.lblMDNotice);
			this.panelMDThreadPriority.Controls.Add(this.cboMDThreadPriority);
			this.panelMDThreadPriority.Controls.Add(this.lblMDThreadPriority);
			this.panelMDThreadPriority.Dock = System.Windows.Forms.DockStyle.Top;
			this.panelMDThreadPriority.Location = new System.Drawing.Point(24, 0);
			this.panelMDThreadPriority.Name = "panelMDThreadPriority";
			this.panelMDThreadPriority.Padding = new System.Windows.Forms.Padding(2);
			this.panelMDThreadPriority.Size = new System.Drawing.Size(380, 24);
			this.panelMDThreadPriority.TabIndex = 9;
			// 
			// lblMDNotice
			// 
			this.lblMDNotice.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblMDNotice.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblMDNotice.ForeColor = System.Drawing.Color.DarkRed;
			this.lblMDNotice.Location = new System.Drawing.Point(240, 2);
			this.lblMDNotice.Name = "lblMDNotice";
			this.lblMDNotice.Size = new System.Drawing.Size(80, 20);
			this.lblMDNotice.TabIndex = 0;
			this.lblMDNotice.Text = "Restart Required";
			this.lblMDNotice.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// cboMDThreadPriority
			// 
			this.cboMDThreadPriority.Dock = System.Windows.Forms.DockStyle.Left;
			this.cboMDThreadPriority.FormattingEnabled = true;
			this.cboMDThreadPriority.Items.AddRange(new object[] {
            "0 Lowest",
            "1 BelowNormal",
            "2 Normal",
            "3 AboveNormal",
            "4 Highest"});
			this.cboMDThreadPriority.Location = new System.Drawing.Point(150, 2);
			this.cboMDThreadPriority.Name = "cboMDThreadPriority";
			this.cboMDThreadPriority.Size = new System.Drawing.Size(90, 21);
			this.cboMDThreadPriority.TabIndex = 0;
			this.cboMDThreadPriority.TabStop = false;
			// 
			// lblMDThreadPriority
			// 
			this.lblMDThreadPriority.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblMDThreadPriority.ForeColor = System.Drawing.Color.White;
			this.lblMDThreadPriority.Location = new System.Drawing.Point(2, 2);
			this.lblMDThreadPriority.Name = "lblMDThreadPriority";
			this.lblMDThreadPriority.Size = new System.Drawing.Size(148, 20);
			this.lblMDThreadPriority.TabIndex = 0;
			this.lblMDThreadPriority.Text = "Market Data Thread Priority : ";
			this.lblMDThreadPriority.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// lblProcessSettings
			// 
			this.lblProcessSettings.AutoColor = false;
			this.lblProcessSettings.BackColor = System.Drawing.SystemColors.ActiveCaption;
			this.lblProcessSettings.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.lblProcessSettings.DefaultColor = System.Drawing.Color.White;
			this.lblProcessSettings.DefaultText = "";
			this.lblProcessSettings.DisplayFactor = 1;
			this.lblProcessSettings.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblProcessSettings.DownColor = System.Drawing.Color.Red;
			this.lblProcessSettings.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblProcessSettings.ForeColor = System.Drawing.SystemColors.ActiveCaptionText;
			this.lblProcessSettings.Is64Th = false;
			this.lblProcessSettings.IsCurrency = false;
			this.lblProcessSettings.IsFraction = false;
			this.lblProcessSettings.IsPercent = false;
			this.lblProcessSettings.Location = new System.Drawing.Point(0, 0);
			this.lblProcessSettings.MaxDecimal = 7;
			this.lblProcessSettings.Name = "lblProcessSettings";
			this.lblProcessSettings.RotationAngle = 270;
			this.lblProcessSettings.Size = new System.Drawing.Size(24, 353);
			this.lblProcessSettings.TabIndex = 6;
			this.lblProcessSettings.Text = "Process Settings";
			this.lblProcessSettings.TextDirection = LabelEx.LabelRotatable.Direction.Clockwise;
			this.lblProcessSettings.TextOrientation = LabelEx.LabelRotatable.Orientation.Rotate;
			this.lblProcessSettings.TickSize = 0.01;
			this.lblProcessSettings.UpColor = System.Drawing.Color.LimeGreen;
			this.lblProcessSettings.Value = "";
			// 
			// panelCMD
			// 
			this.panelCMD.BackColor = System.Drawing.Color.Black;
			this.panelCMD.Controls.Add(this.cmdApply);
			this.panelCMD.Controls.Add(this.cmdCancelSymbols);
			this.panelCMD.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.panelCMD.Location = new System.Drawing.Point(0, 403);
			this.panelCMD.Name = "panelCMD";
			this.panelCMD.Size = new System.Drawing.Size(404, 24);
			this.panelCMD.TabIndex = 17;
			// 
			// cmdApply
			// 
			this.cmdApply.BackColor = System.Drawing.Color.LimeGreen;
			this.cmdApply.Dock = System.Windows.Forms.DockStyle.Right;
			this.cmdApply.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdApply.ForeColor = System.Drawing.Color.Black;
			this.cmdApply.Location = new System.Drawing.Point(254, 0);
			this.cmdApply.Name = "cmdApply";
			this.cmdApply.Size = new System.Drawing.Size(75, 24);
			this.cmdApply.TabIndex = 17;
			this.cmdApply.Text = "Apply";
			this.cmdApply.Click += new System.EventHandler(this.cmdApply_Click);
			// 
			// cmdCancelSymbols
			// 
			this.cmdCancelSymbols.BackColor = System.Drawing.Color.Red;
			this.cmdCancelSymbols.Dock = System.Windows.Forms.DockStyle.Right;
			this.cmdCancelSymbols.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdCancelSymbols.ForeColor = System.Drawing.Color.Black;
			this.cmdCancelSymbols.Location = new System.Drawing.Point(329, 0);
			this.cmdCancelSymbols.Name = "cmdCancelSymbols";
			this.cmdCancelSymbols.Size = new System.Drawing.Size(75, 24);
			this.cmdCancelSymbols.TabIndex = 18;
			this.cmdCancelSymbols.Text = "Cancel";
			this.cmdCancelSymbols.Click += new System.EventHandler(this.cmdCancel_Click);
			// 
			// dataGridViewTextBoxColumn1
			// 
			this.dataGridViewTextBoxColumn1.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
			dataGridViewCellStyle10.BackColor = System.Drawing.Color.Black;
			dataGridViewCellStyle10.ForeColor = System.Drawing.Color.Khaki;
			this.dataGridViewTextBoxColumn1.DefaultCellStyle = dataGridViewCellStyle10;
			this.dataGridViewTextBoxColumn1.FillWeight = 40.32626F;
			this.dataGridViewTextBoxColumn1.HeaderText = "Window";
			this.dataGridViewTextBoxColumn1.Name = "dataGridViewTextBoxColumn1";
			this.dataGridViewTextBoxColumn1.ReadOnly = true;
			this.dataGridViewTextBoxColumn1.Resizable = System.Windows.Forms.DataGridViewTriState.False;
			this.dataGridViewTextBoxColumn1.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable;
			// 
			// dataGridViewTextBoxColumn2
			// 
			dataGridViewCellStyle11.BackColor = System.Drawing.Color.Black;
			dataGridViewCellStyle11.ForeColor = System.Drawing.Color.Khaki;
			this.dataGridViewTextBoxColumn2.DefaultCellStyle = dataGridViewCellStyle11;
			this.dataGridViewTextBoxColumn2.FillWeight = 40.32626F;
			this.dataGridViewTextBoxColumn2.HeaderText = "Type";
			this.dataGridViewTextBoxColumn2.Name = "dataGridViewTextBoxColumn2";
			this.dataGridViewTextBoxColumn2.ReadOnly = true;
			this.dataGridViewTextBoxColumn2.Resizable = System.Windows.Forms.DataGridViewTriState.True;
			this.dataGridViewTextBoxColumn2.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable;
			this.dataGridViewTextBoxColumn2.Width = 39;
			// 
			// dataGridViewTextBoxColumn3
			// 
			dataGridViewCellStyle12.BackColor = System.Drawing.Color.Black;
			dataGridViewCellStyle12.ForeColor = System.Drawing.Color.White;
			this.dataGridViewTextBoxColumn3.DefaultCellStyle = dataGridViewCellStyle12;
			this.dataGridViewTextBoxColumn3.FillWeight = 116.8638F;
			this.dataGridViewTextBoxColumn3.HeaderText = "Path";
			this.dataGridViewTextBoxColumn3.MinimumWidth = 100;
			this.dataGridViewTextBoxColumn3.Name = "dataGridViewTextBoxColumn3";
			this.dataGridViewTextBoxColumn3.Resizable = System.Windows.Forms.DataGridViewTriState.True;
			this.dataGridViewTextBoxColumn3.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable;
			this.dataGridViewTextBoxColumn3.Width = 113;
			// 
			// dataGridViewTextBoxColumn4
			// 
			dataGridViewCellStyle13.BackColor = System.Drawing.SystemColors.ButtonFace;
			dataGridViewCellStyle13.ForeColor = System.Drawing.Color.Black;
			this.dataGridViewTextBoxColumn4.DefaultCellStyle = dataGridViewCellStyle13;
			this.dataGridViewTextBoxColumn4.FillWeight = 23.37277F;
			this.dataGridViewTextBoxColumn4.HeaderText = "";
			this.dataGridViewTextBoxColumn4.Name = "dataGridViewTextBoxColumn4";
			this.dataGridViewTextBoxColumn4.ReadOnly = true;
			this.dataGridViewTextBoxColumn4.Resizable = System.Windows.Forms.DataGridViewTriState.True;
			this.dataGridViewTextBoxColumn4.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable;
			this.dataGridViewTextBoxColumn4.Width = 22;
			// 
			// dataGridViewTextBoxColumn5
			// 
			dataGridViewCellStyle14.BackColor = System.Drawing.SystemColors.ButtonFace;
			dataGridViewCellStyle14.ForeColor = System.Drawing.Color.Black;
			this.dataGridViewTextBoxColumn5.DefaultCellStyle = dataGridViewCellStyle14;
			this.dataGridViewTextBoxColumn5.FillWeight = 23.37277F;
			this.dataGridViewTextBoxColumn5.HeaderText = "";
			this.dataGridViewTextBoxColumn5.Name = "dataGridViewTextBoxColumn5";
			this.dataGridViewTextBoxColumn5.ReadOnly = true;
			this.dataGridViewTextBoxColumn5.Resizable = System.Windows.Forms.DataGridViewTriState.True;
			this.dataGridViewTextBoxColumn5.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable;
			this.dataGridViewTextBoxColumn5.Width = 23;
			// 
			// frmSettings
			// 
			this.AnimationType = FormEx.VistaWindowBase.AnimationTypes.Slide;
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.Caption = "Settings";
			this.ClientSize = new System.Drawing.Size(404, 427);
			this.Controls.Add(this.panelManagerSettings);
			this.Controls.Add(this.panelSelections);
			this.Controls.Add(this.CaptionBar);
			this.Controls.Add(this.panelCMD);
			this.MinimumSize = new System.Drawing.Size(412, 387);
			this.Name = "frmSettings";
			this.OnTop = true;
			this.ShowInTaskbar = false;
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.TopMost = true;
			this.panelSelections.ResumeLayout(false);
			this.panelSelections.PerformLayout();
			this.panelManagerSettings.ResumeLayout(false);
			this.managedPanelSymbolSettings.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.rocSymbolSettingList)).EndInit();
			this.panelSymbolSettings.ResumeLayout(false);
			this.panelSymbolSettings.PerformLayout();
			this.managedPaneWindowSettings.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.DGVAutoFocusSettings)).EndInit();
			this.managedPanelSoundSettings.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.DGVSounds)).EndInit();
			this.managedPanelOrderSettings.ResumeLayout(false);
			this.panelArgPriceResolution.ResumeLayout(false);
			this.panelAutoFocusLast.ResumeLayout(false);
			this.panelSkipGTDandGTDonAuto.ResumeLayout(false);
			this.panelOptionAutoCancel.ResumeLayout(false);
			this.panelFutureAutoCancel.ResumeLayout(false);
			this.panelStockAutoCancel.ResumeLayout(false);
			this.managedPanelQuickButtonSettings.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.rocQuickButtonList)).EndInit();
			this.panelQuickButtonSelection.ResumeLayout(false);
			this.panelQuickButtonSelection.PerformLayout();
			this.managedPanelProcessSettings.ResumeLayout(false);
			this.panel1.ResumeLayout(false);
			this.panel2.ResumeLayout(false);
			this.panelAdmin.ResumeLayout(false);
			this.panelResubscribe.ResumeLayout(false);
			this.panelResubscribe.PerformLayout();
			this.panelUIUpdateRate.ResumeLayout(false);
			this.panelTrackBarUI.ResumeLayout(false);
			this.panelTrackBarUI.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.trackBarUIUpdateRate)).EndInit();
			this.panelCSVThreadPriority.ResumeLayout(false);
			this.panelMDThreadPriority.ResumeLayout(false);
			this.panelCMD.ResumeLayout(false);
			this.ResumeLayout(false);

}

		#endregion

		private FormEx.VistaWindowCaptionBar CaptionBar;
		private PanelEx.VistaPanel panelSelections;
		private PanelEx.PanelManager panelManagerSettings;
		private PanelEx.ManagedPanel managedPanelSymbolSettings;
		private LabelEx.LabelRotatable lblSymbolSettings;
		private DataGridViewEx.ROCSymbolSettingList rocSymbolSettingList;
		private ButtonEx.VistaButton cmdWindows;
		private PanelEx.ManagedPanel managedPaneWindowSettings;
		private PanelEx.ManagedPanel managedPanelSoundSettings;
		private ButtonEx.VistaButton cmdSounds;
		private PanelEx.VistaPanel panelSymbolSettings;
		private ButtonEx.VistaButton cmdOptionSymbols;
		private ButtonEx.VistaButton cmdFutureSymbols;
		private ButtonEx.VistaButton cmdStockSymbols;
		private PanelEx.VistaPanel panelCMD;
		private ButtonEx.VistaButton cmdApply;
		private ButtonEx.VistaButton cmdCancelSymbols;
		private ButtonEx.VistaButton cmdSymbols;
		private System.Windows.Forms.DataGridView DGVAutoFocusSettings;
		private System.Windows.Forms.DataGridViewTextBoxColumn Window;
		private System.Windows.Forms.DataGridViewCheckBoxColumn Enable;
		private LabelEx.LabelRotatable lblAutoFocus;
		private LabelEx.LabelRotatable labelSoundSettings;
		private System.Windows.Forms.DataGridView DGVSounds;
		private System.Windows.Forms.DataGridViewTextBoxColumn Type;
		private System.Windows.Forms.DataGridViewTextBoxColumn Path;
		private System.Windows.Forms.DataGridViewTextBoxColumn Browse;
		private System.Windows.Forms.DataGridViewTextBoxColumn Test;
		private System.Windows.Forms.DataGridViewCheckBoxColumn SoundEnable;
		private PanelEx.ManagedPanel managedPanelQuickButtonSettings;
		private LabelEx.LabelRotatable labelQuickButtonSettings;
		private ButtonEx.VistaButton cmdQuickQty;
		private PanelEx.VistaPanel panelQuickButtonSelection;
		private ButtonEx.VistaButton cmdOptionTicket;
		private ButtonEx.VistaButton cmdFutureTicket;
		private ButtonEx.VistaButton cmdStockTicket;
		private DataGridViewEx.ROCQuickButtonList rocQuickButtonList;
		private ButtonEx.VistaButton cmdOrders;
		private PanelEx.ManagedPanel managedPanelOrderSettings;
		private LabelEx.LabelRotatable labelOrderSettings;
		private System.Windows.Forms.Panel panelStockAutoCancel;
		private System.Windows.Forms.DateTimePicker dtpStockAutoCancel;
		private System.Windows.Forms.CheckBox chkStockAutoCancel;
		private System.Windows.Forms.Label lblStockAutoCancel;
		private ButtonEx.VistaButton cmdProcess;
		private PanelEx.ManagedPanel managedPanelProcessSettings;
		private System.Windows.Forms.Panel panelMDThreadPriority;
		private System.Windows.Forms.Label lblMDThreadPriority;
		private LabelEx.LabelRotatable lblProcessSettings;
		private System.Windows.Forms.Panel panelCSVThreadPriority;
		private System.Windows.Forms.Label lblCSVThreadPriority;
		private System.Windows.Forms.Panel panelUIUpdateRate;
		private System.Windows.Forms.Label lblUIUpdateRate;
		private System.Windows.Forms.ComboBox cboCSVThreadPriority;
		private System.Windows.Forms.ComboBox cboMDThreadPriority;
		private System.Windows.Forms.Label lblCSVNotice;
		private System.Windows.Forms.Label lblMDNotice;
		private System.Windows.Forms.CheckBox chkUserUIUpdateRate;
		private System.Windows.Forms.Panel panelTrackBarUI;
		private System.Windows.Forms.Label lbl250ms;
		private System.Windows.Forms.Label lbl200ms;
		private System.Windows.Forms.Label lbl150ms;
		private System.Windows.Forms.Label lbl100ms;
		private System.Windows.Forms.Label lbl50ms;
		private System.Windows.Forms.Label lbl0ms;
		private System.Windows.Forms.TrackBar trackBarUIUpdateRate;
		private System.Windows.Forms.Panel panelAutoFocusLast;
		private System.Windows.Forms.CheckBox chkOrderAutoFocusLast;
		private System.Windows.Forms.Label lblAutoFocusLast;
		private System.Windows.Forms.Panel panelArgPriceResolution;
		private System.Windows.Forms.ComboBox cboAvgPriceResolution;
		private System.Windows.Forms.Label lblAvePriceResolution;
		private ButtonEx.VistaButton cmdResubscrbe;
		private LabelEx.LabelBase lblResubscribeStatus;
		private System.Windows.Forms.Panel panelAdmin;
		private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn1;
		private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn2;
		private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn3;
		private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn4;
		private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn5;
		private System.Windows.Forms.TextBox txtSourceFilter;
		private System.Windows.Forms.Panel panelResubscribe;
		private System.Windows.Forms.ComboBox cboResubscribeRate;
		private System.Windows.Forms.Panel panelSkipGTDandGTDonAuto;
		private System.Windows.Forms.CheckBox chkSkipGTCandGTDonAuto;
		private System.Windows.Forms.Label lblSkipGTCandGTDonAuto;
		private System.Windows.Forms.Panel panelOptionAutoCancel;
		private System.Windows.Forms.CheckBox chkOptionAutoCancel;
		private System.Windows.Forms.DateTimePicker dtpOptionAutoCancel;
		private System.Windows.Forms.Label lblOptionAutoCancel;
		private System.Windows.Forms.Panel panelFutureAutoCancel;
		private System.Windows.Forms.CheckBox chkFutureAutoCancel;
		private System.Windows.Forms.DateTimePicker dtpFutureAutoCancel;
		private System.Windows.Forms.Label lblFutureAutoCancel;
		private System.Windows.Forms.Panel panel1;
		private System.Windows.Forms.Label dspMDSBackup;
		private System.Windows.Forms.Label lblMDSBackup;
		private System.Windows.Forms.CheckBox chkMDSBackup;
		private System.Windows.Forms.Panel panel2;
		private System.Windows.Forms.Label dspMDS;
		private System.Windows.Forms.Label lblMDS;
		private System.Windows.Forms.CheckBox chkMDS;
	}
}