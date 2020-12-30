namespace ROC
{
	partial class menuBaseOrderModification
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
			this.panelCommands = new System.Windows.Forms.Panel();
			this.panelCommandsInside = new System.Windows.Forms.Panel();
			this.cmdCancel = new ButtonEx.VistaButton();
			this.cmdCancelAll = new ButtonEx.VistaButton();
			this.cmdReplace = new ButtonEx.VistaButton();
			this.panelDetails = new System.Windows.Forms.Panel();
			this.panelShow = new System.Windows.Forms.Panel();
			this.dspShowIncrement = new LabelEx.LabelBase();
			this.numShow = new System.Windows.Forms.NumericUpDown();
			this.chkShow = new System.Windows.Forms.CheckBox();
			this.panelOrderTypes = new System.Windows.Forms.Panel();
			this.cboOrder = new System.Windows.Forms.ComboBox();
			this.lblOrderType = new System.Windows.Forms.Label();
			this.panelDuration = new System.Windows.Forms.Panel();
			this.cboDuration = new System.Windows.Forms.ComboBox();
			this.lblDuration = new System.Windows.Forms.Label();
			this.panelPegPrice = new System.Windows.Forms.Panel();
			this.dspPegPriceTickSize = new LabelEx.LabelBase();
			this.lblPegPriceX = new LabelEx.LabelBase();
			this.dspPegPriceIncrement = new LabelEx.LabelBase();
			this.numPegPrice = new System.Windows.Forms.NumericUpDown();
			this.lblPegPrice = new System.Windows.Forms.Label();
			this.panelStopPrice = new System.Windows.Forms.Panel();
			this.dspStopPriceTickSize = new LabelEx.LabelBase();
			this.lblStopPriceX = new LabelEx.LabelBase();
			this.dspStopPriceIncrement = new LabelEx.LabelBase();
			this.numStopPrice = new System.Windows.Forms.NumericUpDown();
			this.lblStopPrice = new System.Windows.Forms.Label();
			this.panelLimitPrice = new System.Windows.Forms.Panel();
			this.dspLimitPriceTickSize = new LabelEx.LabelBase();
			this.lblLimitPriceX = new LabelEx.LabelBase();
			this.dspLimitPriceIncrement = new LabelEx.LabelBase();
			this.numLimitPrice = new System.Windows.Forms.NumericUpDown();
			this.lblLimitPrice = new System.Windows.Forms.Label();
			this.panelShares = new System.Windows.Forms.Panel();
			this.dspQuantityIncrement = new LabelEx.LabelBase();
			this.numQuantity = new System.Windows.Forms.NumericUpDown();
			this.lblQuantity = new System.Windows.Forms.Label();
			this.panelTop = new System.Windows.Forms.Panel();
			this.dspMessage = new System.Windows.Forms.Label();
			this.cmdClose = new System.Windows.Forms.PictureBox();
			this.panelCommands.SuspendLayout();
			this.panelCommandsInside.SuspendLayout();
			this.panelDetails.SuspendLayout();
			this.panelShow.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.numShow)).BeginInit();
			this.panelOrderTypes.SuspendLayout();
			this.panelDuration.SuspendLayout();
			this.panelPegPrice.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.numPegPrice)).BeginInit();
			this.panelStopPrice.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.numStopPrice)).BeginInit();
			this.panelLimitPrice.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.numLimitPrice)).BeginInit();
			this.panelShares.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.numQuantity)).BeginInit();
			this.panelTop.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.cmdClose)).BeginInit();
			this.SuspendLayout();
			// 
			// panelCommands
			// 
			this.panelCommands.Controls.Add(this.panelCommandsInside);
			this.panelCommands.Dock = System.Windows.Forms.DockStyle.Left;
			this.panelCommands.Location = new System.Drawing.Point(0, 0);
			this.panelCommands.Name = "panelCommands";
			this.panelCommands.Size = new System.Drawing.Size(82, 77);
			this.panelCommands.TabIndex = 2;
			// 
			// panelCommandsInside
			// 
			this.panelCommandsInside.BackColor = System.Drawing.SystemColors.ControlDarkDark;
			this.panelCommandsInside.Controls.Add(this.cmdCancel);
			this.panelCommandsInside.Controls.Add(this.cmdCancelAll);
			this.panelCommandsInside.Controls.Add(this.cmdReplace);
			this.panelCommandsInside.Dock = System.Windows.Forms.DockStyle.Top;
			this.panelCommandsInside.Location = new System.Drawing.Point(0, 0);
			this.panelCommandsInside.Name = "panelCommandsInside";
			this.panelCommandsInside.Padding = new System.Windows.Forms.Padding(3);
			this.panelCommandsInside.Size = new System.Drawing.Size(82, 76);
			this.panelCommandsInside.TabIndex = 5;
			// 
			// cmdCancel
			// 
			this.cmdCancel.AutoSize = true;
			this.cmdCancel.BackColor = System.Drawing.SystemColors.ControlDarkDark;
			this.cmdCancel.Dock = System.Windows.Forms.DockStyle.Top;
			this.cmdCancel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdCancel.ForeColor = System.Drawing.Color.Red;
			this.cmdCancel.Location = new System.Drawing.Point(3, 49);
			this.cmdCancel.Name = "cmdCancel";
			this.cmdCancel.Size = new System.Drawing.Size(76, 23);
			this.cmdCancel.TabIndex = 1;
			this.cmdCancel.Text = "Cancel";
			this.cmdCancel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			this.cmdCancel.Click += new System.EventHandler(this.cmdCancel_Click);
			// 
			// cmdCancelAll
			// 
			this.cmdCancelAll.AutoSize = true;
			this.cmdCancelAll.BackColor = System.Drawing.SystemColors.ControlDarkDark;
			this.cmdCancelAll.Dock = System.Windows.Forms.DockStyle.Top;
			this.cmdCancelAll.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdCancelAll.ForeColor = System.Drawing.Color.Red;
			this.cmdCancelAll.Location = new System.Drawing.Point(3, 26);
			this.cmdCancelAll.Name = "cmdCancelAll";
			this.cmdCancelAll.Size = new System.Drawing.Size(76, 23);
			this.cmdCancelAll.TabIndex = 4;
			this.cmdCancelAll.Text = "Cancel All";
			this.cmdCancelAll.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			this.cmdCancelAll.Click += new System.EventHandler(this.cmdCancelAll_Click);
			// 
			// cmdReplace
			// 
			this.cmdReplace.AutoSize = true;
			this.cmdReplace.BackColor = System.Drawing.SystemColors.ControlDarkDark;
			this.cmdReplace.Dock = System.Windows.Forms.DockStyle.Top;
			this.cmdReplace.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdReplace.ForeColor = System.Drawing.Color.LimeGreen;
			this.cmdReplace.Location = new System.Drawing.Point(3, 3);
			this.cmdReplace.Name = "cmdReplace";
			this.cmdReplace.Size = new System.Drawing.Size(76, 23);
			this.cmdReplace.TabIndex = 2;
			this.cmdReplace.Text = "Replace";
			this.cmdReplace.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			this.cmdReplace.Click += new System.EventHandler(this.cmdReplace_Click);
			// 
			// panelDetails
			// 
			this.panelDetails.BackColor = System.Drawing.SystemColors.ControlDarkDark;
			this.panelDetails.Controls.Add(this.panelShow);
			this.panelDetails.Controls.Add(this.panelOrderTypes);
			this.panelDetails.Controls.Add(this.panelDuration);
			this.panelDetails.Controls.Add(this.panelPegPrice);
			this.panelDetails.Controls.Add(this.panelStopPrice);
			this.panelDetails.Controls.Add(this.panelLimitPrice);
			this.panelDetails.Controls.Add(this.panelShares);
			this.panelDetails.Dock = System.Windows.Forms.DockStyle.Top;
			this.panelDetails.Location = new System.Drawing.Point(82, 0);
			this.panelDetails.Name = "panelDetails";
			this.panelDetails.Padding = new System.Windows.Forms.Padding(1);
			this.panelDetails.Size = new System.Drawing.Size(362, 58);
			this.panelDetails.TabIndex = 3;
			// 
			// panelShow
			// 
			this.panelShow.Controls.Add(this.dspShowIncrement);
			this.panelShow.Controls.Add(this.numShow);
			this.panelShow.Controls.Add(this.chkShow);
			this.panelShow.Dock = System.Windows.Forms.DockStyle.Left;
			this.panelShow.Enabled = false;
			this.panelShow.Location = new System.Drawing.Point(443, 1);
			this.panelShow.MinimumSize = new System.Drawing.Size(70, 47);
			this.panelShow.Name = "panelShow";
			this.panelShow.Padding = new System.Windows.Forms.Padding(2);
			this.panelShow.Size = new System.Drawing.Size(70, 56);
			this.panelShow.TabIndex = 6;
			// 
			// dspShowIncrement
			// 
			this.dspShowIncrement.AutoColor = false;
			this.dspShowIncrement.AutoSize = true;
			this.dspShowIncrement.BackColor = System.Drawing.SystemColors.ControlText;
			this.dspShowIncrement.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.dspShowIncrement.DefaultColor = System.Drawing.Color.White;
			this.dspShowIncrement.DefaultText = "100";
			this.dspShowIncrement.DisplayFactor = 1;
			this.dspShowIncrement.Dock = System.Windows.Forms.DockStyle.Right;
			this.dspShowIncrement.DownColor = System.Drawing.Color.Red;
			this.dspShowIncrement.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspShowIncrement.ForeColor = System.Drawing.Color.Gainsboro;
			this.dspShowIncrement.Is64Th = false;
			this.dspShowIncrement.IsCurrency = false;
			this.dspShowIncrement.IsFraction = false;
			this.dspShowIncrement.IsPercent = false;
			this.dspShowIncrement.Location = new System.Drawing.Point(43, 40);
			this.dspShowIncrement.MaxDecimal = 0;
			this.dspShowIncrement.Name = "dspShowIncrement";
			this.dspShowIncrement.Size = new System.Drawing.Size(25, 14);
			this.dspShowIncrement.TabIndex = 8;
			this.dspShowIncrement.Text = "100";
			this.dspShowIncrement.TickSize = 0.01;
			this.dspShowIncrement.UpColor = System.Drawing.Color.LimeGreen;
			this.dspShowIncrement.Value = "";
			// 
			// numShow
			// 
			this.numShow.BackColor = System.Drawing.Color.White;
			this.numShow.Dock = System.Windows.Forms.DockStyle.Top;
			this.numShow.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.numShow.ForeColor = System.Drawing.Color.Black;
			this.numShow.Increment = new decimal(new int[] {
            10,
            0,
            0,
            0});
			this.numShow.Location = new System.Drawing.Point(2, 19);
			this.numShow.Maximum = new decimal(new int[] {
            1215752191,
            23,
            0,
            0});
			this.numShow.Name = "numShow";
			this.numShow.Size = new System.Drawing.Size(66, 21);
			this.numShow.TabIndex = 7;
			this.numShow.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
			// 
			// chkShow
			// 
			this.chkShow.AutoSize = true;
			this.chkShow.BackColor = System.Drawing.SystemColors.ControlDarkDark;
			this.chkShow.Dock = System.Windows.Forms.DockStyle.Top;
			this.chkShow.Location = new System.Drawing.Point(2, 2);
			this.chkShow.Name = "chkShow";
			this.chkShow.RightToLeft = System.Windows.Forms.RightToLeft.Yes;
			this.chkShow.Size = new System.Drawing.Size(66, 17);
			this.chkShow.TabIndex = 6;
			this.chkShow.Text = "Show";
			this.chkShow.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.chkShow.UseVisualStyleBackColor = false;
			// 
			// panelOrderTypes
			// 
			this.panelOrderTypes.Controls.Add(this.cboOrder);
			this.panelOrderTypes.Controls.Add(this.lblOrderType);
			this.panelOrderTypes.Dock = System.Windows.Forms.DockStyle.Left;
			this.panelOrderTypes.Enabled = false;
			this.panelOrderTypes.Location = new System.Drawing.Point(361, 1);
			this.panelOrderTypes.MinimumSize = new System.Drawing.Size(82, 55);
			this.panelOrderTypes.Name = "panelOrderTypes";
			this.panelOrderTypes.Padding = new System.Windows.Forms.Padding(2, 4, 2, 2);
			this.panelOrderTypes.Size = new System.Drawing.Size(82, 56);
			this.panelOrderTypes.TabIndex = 3;
			// 
			// cboOrder
			// 
			this.cboOrder.BackColor = System.Drawing.Color.White;
			this.cboOrder.Dock = System.Windows.Forms.DockStyle.Top;
			this.cboOrder.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.cboOrder.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cboOrder.ForeColor = System.Drawing.Color.Black;
			this.cboOrder.FormattingEnabled = true;
			this.cboOrder.Location = new System.Drawing.Point(2, 19);
			this.cboOrder.MaxDropDownItems = 3;
			this.cboOrder.MinimumSize = new System.Drawing.Size(72, 0);
			this.cboOrder.Name = "cboOrder";
			this.cboOrder.Size = new System.Drawing.Size(78, 21);
			this.cboOrder.TabIndex = 2;
			// 
			// lblOrderType
			// 
			this.lblOrderType.AutoSize = true;
			this.lblOrderType.Dock = System.Windows.Forms.DockStyle.Top;
			this.lblOrderType.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblOrderType.Location = new System.Drawing.Point(2, 4);
			this.lblOrderType.Name = "lblOrderType";
			this.lblOrderType.Padding = new System.Windows.Forms.Padding(0, 0, 0, 2);
			this.lblOrderType.Size = new System.Drawing.Size(33, 15);
			this.lblOrderType.TabIndex = 0;
			this.lblOrderType.Text = "Order";
			// 
			// panelDuration
			// 
			this.panelDuration.BackColor = System.Drawing.SystemColors.ControlDarkDark;
			this.panelDuration.Controls.Add(this.cboDuration);
			this.panelDuration.Controls.Add(this.lblDuration);
			this.panelDuration.Dock = System.Windows.Forms.DockStyle.Left;
			this.panelDuration.Location = new System.Drawing.Point(306, 1);
			this.panelDuration.MinimumSize = new System.Drawing.Size(53, 55);
			this.panelDuration.Name = "panelDuration";
			this.panelDuration.Padding = new System.Windows.Forms.Padding(2, 4, 2, 2);
			this.panelDuration.Size = new System.Drawing.Size(55, 56);
			this.panelDuration.TabIndex = 4;
			// 
			// cboDuration
			// 
			this.cboDuration.BackColor = System.Drawing.Color.White;
			this.cboDuration.Dock = System.Windows.Forms.DockStyle.Top;
			this.cboDuration.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.cboDuration.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cboDuration.ForeColor = System.Drawing.Color.Black;
			this.cboDuration.FormattingEnabled = true;
			this.cboDuration.Location = new System.Drawing.Point(2, 19);
			this.cboDuration.MaxDropDownItems = 3;
			this.cboDuration.MinimumSize = new System.Drawing.Size(50, 0);
			this.cboDuration.Name = "cboDuration";
			this.cboDuration.Size = new System.Drawing.Size(51, 21);
			this.cboDuration.TabIndex = 5;
			// 
			// lblDuration
			// 
			this.lblDuration.AutoSize = true;
			this.lblDuration.Dock = System.Windows.Forms.DockStyle.Top;
			this.lblDuration.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblDuration.Location = new System.Drawing.Point(2, 4);
			this.lblDuration.Name = "lblDuration";
			this.lblDuration.Padding = new System.Windows.Forms.Padding(0, 0, 0, 2);
			this.lblDuration.Size = new System.Drawing.Size(47, 15);
			this.lblDuration.TabIndex = 0;
			this.lblDuration.Text = "Duration";
			// 
			// panelPegPrice
			// 
			this.panelPegPrice.BackColor = System.Drawing.SystemColors.ControlDarkDark;
			this.panelPegPrice.Controls.Add(this.dspPegPriceTickSize);
			this.panelPegPrice.Controls.Add(this.lblPegPriceX);
			this.panelPegPrice.Controls.Add(this.dspPegPriceIncrement);
			this.panelPegPrice.Controls.Add(this.numPegPrice);
			this.panelPegPrice.Controls.Add(this.lblPegPrice);
			this.panelPegPrice.Dock = System.Windows.Forms.DockStyle.Left;
			this.panelPegPrice.Enabled = false;
			this.panelPegPrice.Location = new System.Drawing.Point(227, 1);
			this.panelPegPrice.MinimumSize = new System.Drawing.Size(79, 55);
			this.panelPegPrice.Name = "panelPegPrice";
			this.panelPegPrice.Padding = new System.Windows.Forms.Padding(2, 4, 2, 2);
			this.panelPegPrice.Size = new System.Drawing.Size(79, 56);
			this.panelPegPrice.TabIndex = 7;
			// 
			// dspPegPriceTickSize
			// 
			this.dspPegPriceTickSize.AutoColor = false;
			this.dspPegPriceTickSize.AutoSize = true;
			this.dspPegPriceTickSize.BackColor = System.Drawing.Color.Transparent;
			this.dspPegPriceTickSize.DefaultColor = System.Drawing.Color.White;
			this.dspPegPriceTickSize.DefaultText = "0.01";
			this.dspPegPriceTickSize.DisplayFactor = 1;
			this.dspPegPriceTickSize.Dock = System.Windows.Forms.DockStyle.Right;
			this.dspPegPriceTickSize.DownColor = System.Drawing.Color.Red;
			this.dspPegPriceTickSize.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspPegPriceTickSize.ForeColor = System.Drawing.Color.Gainsboro;
			this.dspPegPriceTickSize.Is64Th = false;
			this.dspPegPriceTickSize.IsCurrency = false;
			this.dspPegPriceTickSize.IsFraction = true;
			this.dspPegPriceTickSize.IsPercent = false;
			this.dspPegPriceTickSize.Location = new System.Drawing.Point(26, 40);
			this.dspPegPriceTickSize.MaxDecimal = 7;
			this.dspPegPriceTickSize.Name = "dspPegPriceTickSize";
			this.dspPegPriceTickSize.Size = new System.Drawing.Size(27, 12);
			this.dspPegPriceTickSize.TabIndex = 9;
			this.dspPegPriceTickSize.Text = "0.01";
			this.dspPegPriceTickSize.TickSize = 0.01;
			this.dspPegPriceTickSize.UpColor = System.Drawing.Color.LimeGreen;
			this.dspPegPriceTickSize.Value = "";
			// 
			// lblPegPriceX
			// 
			this.lblPegPriceX.AutoColor = false;
			this.lblPegPriceX.AutoSize = true;
			this.lblPegPriceX.BackColor = System.Drawing.Color.Transparent;
			this.lblPegPriceX.DefaultColor = System.Drawing.Color.White;
			this.lblPegPriceX.DefaultText = "0.01";
			this.lblPegPriceX.DisplayFactor = 1;
			this.lblPegPriceX.Dock = System.Windows.Forms.DockStyle.Right;
			this.lblPegPriceX.DownColor = System.Drawing.Color.Red;
			this.lblPegPriceX.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblPegPriceX.ForeColor = System.Drawing.Color.Gainsboro;
			this.lblPegPriceX.Is64Th = false;
			this.lblPegPriceX.IsCurrency = false;
			this.lblPegPriceX.IsFraction = false;
			this.lblPegPriceX.IsPercent = false;
			this.lblPegPriceX.Location = new System.Drawing.Point(53, 40);
			this.lblPegPriceX.MaxDecimal = 7;
			this.lblPegPriceX.Name = "lblPegPriceX";
			this.lblPegPriceX.Size = new System.Drawing.Size(11, 12);
			this.lblPegPriceX.TabIndex = 10;
			this.lblPegPriceX.Text = "x";
			this.lblPegPriceX.TickSize = 0.01;
			this.lblPegPriceX.UpColor = System.Drawing.Color.LimeGreen;
			this.lblPegPriceX.Value = "";
			// 
			// dspPegPriceIncrement
			// 
			this.dspPegPriceIncrement.AutoColor = false;
			this.dspPegPriceIncrement.AutoSize = true;
			this.dspPegPriceIncrement.BackColor = System.Drawing.SystemColors.ControlText;
			this.dspPegPriceIncrement.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.dspPegPriceIncrement.DefaultColor = System.Drawing.Color.White;
			this.dspPegPriceIncrement.DefaultText = "100";
			this.dspPegPriceIncrement.DisplayFactor = 1;
			this.dspPegPriceIncrement.Dock = System.Windows.Forms.DockStyle.Right;
			this.dspPegPriceIncrement.DownColor = System.Drawing.Color.Red;
			this.dspPegPriceIncrement.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspPegPriceIncrement.ForeColor = System.Drawing.Color.Gainsboro;
			this.dspPegPriceIncrement.Is64Th = false;
			this.dspPegPriceIncrement.IsCurrency = false;
			this.dspPegPriceIncrement.IsFraction = false;
			this.dspPegPriceIncrement.IsPercent = false;
			this.dspPegPriceIncrement.Location = new System.Drawing.Point(64, 40);
			this.dspPegPriceIncrement.MaxDecimal = 0;
			this.dspPegPriceIncrement.Name = "dspPegPriceIncrement";
			this.dspPegPriceIncrement.Size = new System.Drawing.Size(13, 14);
			this.dspPegPriceIncrement.TabIndex = 6;
			this.dspPegPriceIncrement.Text = "1";
			this.dspPegPriceIncrement.TickSize = 0.01;
			this.dspPegPriceIncrement.UpColor = System.Drawing.Color.LimeGreen;
			this.dspPegPriceIncrement.Value = "";
			// 
			// numPegPrice
			// 
			this.numPegPrice.BackColor = System.Drawing.Color.White;
			this.numPegPrice.DecimalPlaces = 2;
			this.numPegPrice.Dock = System.Windows.Forms.DockStyle.Top;
			this.numPegPrice.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.numPegPrice.ForeColor = System.Drawing.Color.Black;
			this.numPegPrice.Increment = new decimal(new int[] {
            10,
            0,
            0,
            0});
			this.numPegPrice.Location = new System.Drawing.Point(2, 19);
			this.numPegPrice.Maximum = new decimal(new int[] {
            1215752191,
            23,
            0,
            0});
			this.numPegPrice.Minimum = new decimal(new int[] {
            1215752191,
            23,
            0,
            -2147483648});
			this.numPegPrice.Name = "numPegPrice";
			this.numPegPrice.Size = new System.Drawing.Size(75, 21);
			this.numPegPrice.TabIndex = 4;
			this.numPegPrice.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
			// 
			// lblPegPrice
			// 
			this.lblPegPrice.AutoSize = true;
			this.lblPegPrice.Dock = System.Windows.Forms.DockStyle.Top;
			this.lblPegPrice.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblPegPrice.Location = new System.Drawing.Point(2, 4);
			this.lblPegPrice.Name = "lblPegPrice";
			this.lblPegPrice.Padding = new System.Windows.Forms.Padding(0, 0, 0, 2);
			this.lblPegPrice.Size = new System.Drawing.Size(53, 15);
			this.lblPegPrice.TabIndex = 0;
			this.lblPegPrice.Text = "Peg Price";
			// 
			// panelStopPrice
			// 
			this.panelStopPrice.BackColor = System.Drawing.SystemColors.ControlDarkDark;
			this.panelStopPrice.Controls.Add(this.dspStopPriceTickSize);
			this.panelStopPrice.Controls.Add(this.lblStopPriceX);
			this.panelStopPrice.Controls.Add(this.dspStopPriceIncrement);
			this.panelStopPrice.Controls.Add(this.numStopPrice);
			this.panelStopPrice.Controls.Add(this.lblStopPrice);
			this.panelStopPrice.Dock = System.Windows.Forms.DockStyle.Left;
			this.panelStopPrice.Enabled = false;
			this.panelStopPrice.Location = new System.Drawing.Point(148, 1);
			this.panelStopPrice.MinimumSize = new System.Drawing.Size(79, 55);
			this.panelStopPrice.Name = "panelStopPrice";
			this.panelStopPrice.Padding = new System.Windows.Forms.Padding(2, 4, 2, 2);
			this.panelStopPrice.Size = new System.Drawing.Size(79, 56);
			this.panelStopPrice.TabIndex = 5;
			// 
			// dspStopPriceTickSize
			// 
			this.dspStopPriceTickSize.AutoColor = false;
			this.dspStopPriceTickSize.AutoSize = true;
			this.dspStopPriceTickSize.BackColor = System.Drawing.Color.Transparent;
			this.dspStopPriceTickSize.DefaultColor = System.Drawing.Color.White;
			this.dspStopPriceTickSize.DefaultText = "0.01";
			this.dspStopPriceTickSize.DisplayFactor = 1;
			this.dspStopPriceTickSize.Dock = System.Windows.Forms.DockStyle.Right;
			this.dspStopPriceTickSize.DownColor = System.Drawing.Color.Red;
			this.dspStopPriceTickSize.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspStopPriceTickSize.ForeColor = System.Drawing.Color.Gainsboro;
			this.dspStopPriceTickSize.Is64Th = false;
			this.dspStopPriceTickSize.IsCurrency = false;
			this.dspStopPriceTickSize.IsFraction = true;
			this.dspStopPriceTickSize.IsPercent = false;
			this.dspStopPriceTickSize.Location = new System.Drawing.Point(26, 40);
			this.dspStopPriceTickSize.MaxDecimal = 7;
			this.dspStopPriceTickSize.Name = "dspStopPriceTickSize";
			this.dspStopPriceTickSize.Size = new System.Drawing.Size(27, 12);
			this.dspStopPriceTickSize.TabIndex = 9;
			this.dspStopPriceTickSize.Text = "0.01";
			this.dspStopPriceTickSize.TickSize = 0.01;
			this.dspStopPriceTickSize.UpColor = System.Drawing.Color.LimeGreen;
			this.dspStopPriceTickSize.Value = "";
			// 
			// lblStopPriceX
			// 
			this.lblStopPriceX.AutoColor = false;
			this.lblStopPriceX.AutoSize = true;
			this.lblStopPriceX.BackColor = System.Drawing.Color.Transparent;
			this.lblStopPriceX.DefaultColor = System.Drawing.Color.White;
			this.lblStopPriceX.DefaultText = "0.01";
			this.lblStopPriceX.DisplayFactor = 1;
			this.lblStopPriceX.Dock = System.Windows.Forms.DockStyle.Right;
			this.lblStopPriceX.DownColor = System.Drawing.Color.Red;
			this.lblStopPriceX.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblStopPriceX.ForeColor = System.Drawing.Color.Gainsboro;
			this.lblStopPriceX.Is64Th = false;
			this.lblStopPriceX.IsCurrency = false;
			this.lblStopPriceX.IsFraction = false;
			this.lblStopPriceX.IsPercent = false;
			this.lblStopPriceX.Location = new System.Drawing.Point(53, 40);
			this.lblStopPriceX.MaxDecimal = 7;
			this.lblStopPriceX.Name = "lblStopPriceX";
			this.lblStopPriceX.Size = new System.Drawing.Size(11, 12);
			this.lblStopPriceX.TabIndex = 10;
			this.lblStopPriceX.Text = "x";
			this.lblStopPriceX.TickSize = 0.01;
			this.lblStopPriceX.UpColor = System.Drawing.Color.LimeGreen;
			this.lblStopPriceX.Value = "";
			// 
			// dspStopPriceIncrement
			// 
			this.dspStopPriceIncrement.AutoColor = false;
			this.dspStopPriceIncrement.AutoSize = true;
			this.dspStopPriceIncrement.BackColor = System.Drawing.SystemColors.ControlText;
			this.dspStopPriceIncrement.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.dspStopPriceIncrement.DefaultColor = System.Drawing.Color.White;
			this.dspStopPriceIncrement.DefaultText = "100";
			this.dspStopPriceIncrement.DisplayFactor = 1;
			this.dspStopPriceIncrement.Dock = System.Windows.Forms.DockStyle.Right;
			this.dspStopPriceIncrement.DownColor = System.Drawing.Color.Red;
			this.dspStopPriceIncrement.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspStopPriceIncrement.ForeColor = System.Drawing.Color.Gainsboro;
			this.dspStopPriceIncrement.Is64Th = false;
			this.dspStopPriceIncrement.IsCurrency = false;
			this.dspStopPriceIncrement.IsFraction = false;
			this.dspStopPriceIncrement.IsPercent = false;
			this.dspStopPriceIncrement.Location = new System.Drawing.Point(64, 40);
			this.dspStopPriceIncrement.MaxDecimal = 0;
			this.dspStopPriceIncrement.Name = "dspStopPriceIncrement";
			this.dspStopPriceIncrement.Size = new System.Drawing.Size(13, 14);
			this.dspStopPriceIncrement.TabIndex = 6;
			this.dspStopPriceIncrement.Text = "1";
			this.dspStopPriceIncrement.TickSize = 0.01;
			this.dspStopPriceIncrement.UpColor = System.Drawing.Color.LimeGreen;
			this.dspStopPriceIncrement.Value = "";
			// 
			// numStopPrice
			// 
			this.numStopPrice.BackColor = System.Drawing.Color.White;
			this.numStopPrice.DecimalPlaces = 2;
			this.numStopPrice.Dock = System.Windows.Forms.DockStyle.Top;
			this.numStopPrice.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.numStopPrice.ForeColor = System.Drawing.Color.Black;
			this.numStopPrice.Increment = new decimal(new int[] {
            10,
            0,
            0,
            0});
			this.numStopPrice.Location = new System.Drawing.Point(2, 19);
			this.numStopPrice.Maximum = new decimal(new int[] {
            1215752191,
            23,
            0,
            0});
			this.numStopPrice.Name = "numStopPrice";
			this.numStopPrice.Size = new System.Drawing.Size(75, 21);
			this.numStopPrice.TabIndex = 4;
			this.numStopPrice.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
			// 
			// lblStopPrice
			// 
			this.lblStopPrice.AutoSize = true;
			this.lblStopPrice.Dock = System.Windows.Forms.DockStyle.Top;
			this.lblStopPrice.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblStopPrice.Location = new System.Drawing.Point(2, 4);
			this.lblStopPrice.Name = "lblStopPrice";
			this.lblStopPrice.Padding = new System.Windows.Forms.Padding(0, 0, 0, 2);
			this.lblStopPrice.Size = new System.Drawing.Size(56, 15);
			this.lblStopPrice.TabIndex = 0;
			this.lblStopPrice.Text = "Stop Price";
			// 
			// panelLimitPrice
			// 
			this.panelLimitPrice.BackColor = System.Drawing.Color.CornflowerBlue;
			this.panelLimitPrice.Controls.Add(this.dspLimitPriceTickSize);
			this.panelLimitPrice.Controls.Add(this.lblLimitPriceX);
			this.panelLimitPrice.Controls.Add(this.dspLimitPriceIncrement);
			this.panelLimitPrice.Controls.Add(this.numLimitPrice);
			this.panelLimitPrice.Controls.Add(this.lblLimitPrice);
			this.panelLimitPrice.Dock = System.Windows.Forms.DockStyle.Left;
			this.panelLimitPrice.Location = new System.Drawing.Point(69, 1);
			this.panelLimitPrice.MinimumSize = new System.Drawing.Size(79, 55);
			this.panelLimitPrice.Name = "panelLimitPrice";
			this.panelLimitPrice.Padding = new System.Windows.Forms.Padding(2, 4, 2, 2);
			this.panelLimitPrice.Size = new System.Drawing.Size(79, 56);
			this.panelLimitPrice.TabIndex = 2;
			// 
			// dspLimitPriceTickSize
			// 
			this.dspLimitPriceTickSize.AutoColor = false;
			this.dspLimitPriceTickSize.AutoSize = true;
			this.dspLimitPriceTickSize.BackColor = System.Drawing.Color.Transparent;
			this.dspLimitPriceTickSize.DefaultColor = System.Drawing.Color.White;
			this.dspLimitPriceTickSize.DefaultText = "0.01";
			this.dspLimitPriceTickSize.DisplayFactor = 1;
			this.dspLimitPriceTickSize.Dock = System.Windows.Forms.DockStyle.Right;
			this.dspLimitPriceTickSize.DownColor = System.Drawing.Color.Red;
			this.dspLimitPriceTickSize.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspLimitPriceTickSize.ForeColor = System.Drawing.Color.Gainsboro;
			this.dspLimitPriceTickSize.Is64Th = false;
			this.dspLimitPriceTickSize.IsCurrency = false;
			this.dspLimitPriceTickSize.IsFraction = true;
			this.dspLimitPriceTickSize.IsPercent = false;
			this.dspLimitPriceTickSize.Location = new System.Drawing.Point(26, 40);
			this.dspLimitPriceTickSize.MaxDecimal = 7;
			this.dspLimitPriceTickSize.Name = "dspLimitPriceTickSize";
			this.dspLimitPriceTickSize.Size = new System.Drawing.Size(27, 12);
			this.dspLimitPriceTickSize.TabIndex = 7;
			this.dspLimitPriceTickSize.Text = "0.01";
			this.dspLimitPriceTickSize.TickSize = 0.01;
			this.dspLimitPriceTickSize.UpColor = System.Drawing.Color.LimeGreen;
			this.dspLimitPriceTickSize.Value = "";
			// 
			// lblLimitPriceX
			// 
			this.lblLimitPriceX.AutoColor = false;
			this.lblLimitPriceX.AutoSize = true;
			this.lblLimitPriceX.BackColor = System.Drawing.Color.Transparent;
			this.lblLimitPriceX.DefaultColor = System.Drawing.Color.White;
			this.lblLimitPriceX.DefaultText = "0.01";
			this.lblLimitPriceX.DisplayFactor = 1;
			this.lblLimitPriceX.Dock = System.Windows.Forms.DockStyle.Right;
			this.lblLimitPriceX.DownColor = System.Drawing.Color.Red;
			this.lblLimitPriceX.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblLimitPriceX.ForeColor = System.Drawing.Color.Gainsboro;
			this.lblLimitPriceX.Is64Th = false;
			this.lblLimitPriceX.IsCurrency = false;
			this.lblLimitPriceX.IsFraction = false;
			this.lblLimitPriceX.IsPercent = false;
			this.lblLimitPriceX.Location = new System.Drawing.Point(53, 40);
			this.lblLimitPriceX.MaxDecimal = 7;
			this.lblLimitPriceX.Name = "lblLimitPriceX";
			this.lblLimitPriceX.Size = new System.Drawing.Size(11, 12);
			this.lblLimitPriceX.TabIndex = 8;
			this.lblLimitPriceX.Text = "x";
			this.lblLimitPriceX.TickSize = 0.01;
			this.lblLimitPriceX.UpColor = System.Drawing.Color.LimeGreen;
			this.lblLimitPriceX.Value = "";
			// 
			// dspLimitPriceIncrement
			// 
			this.dspLimitPriceIncrement.AutoColor = false;
			this.dspLimitPriceIncrement.AutoSize = true;
			this.dspLimitPriceIncrement.BackColor = System.Drawing.SystemColors.ControlText;
			this.dspLimitPriceIncrement.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.dspLimitPriceIncrement.DefaultColor = System.Drawing.Color.White;
			this.dspLimitPriceIncrement.DefaultText = "100";
			this.dspLimitPriceIncrement.DisplayFactor = 1;
			this.dspLimitPriceIncrement.Dock = System.Windows.Forms.DockStyle.Right;
			this.dspLimitPriceIncrement.DownColor = System.Drawing.Color.Red;
			this.dspLimitPriceIncrement.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspLimitPriceIncrement.ForeColor = System.Drawing.Color.Gainsboro;
			this.dspLimitPriceIncrement.Is64Th = false;
			this.dspLimitPriceIncrement.IsCurrency = false;
			this.dspLimitPriceIncrement.IsFraction = false;
			this.dspLimitPriceIncrement.IsPercent = false;
			this.dspLimitPriceIncrement.Location = new System.Drawing.Point(64, 40);
			this.dspLimitPriceIncrement.MaxDecimal = 0;
			this.dspLimitPriceIncrement.Name = "dspLimitPriceIncrement";
			this.dspLimitPriceIncrement.Size = new System.Drawing.Size(13, 14);
			this.dspLimitPriceIncrement.TabIndex = 6;
			this.dspLimitPriceIncrement.Text = "1";
			this.dspLimitPriceIncrement.TickSize = 0.01;
			this.dspLimitPriceIncrement.UpColor = System.Drawing.Color.LimeGreen;
			this.dspLimitPriceIncrement.Value = "";
			// 
			// numLimitPrice
			// 
			this.numLimitPrice.BackColor = System.Drawing.Color.White;
			this.numLimitPrice.DecimalPlaces = 2;
			this.numLimitPrice.Dock = System.Windows.Forms.DockStyle.Top;
			this.numLimitPrice.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.numLimitPrice.ForeColor = System.Drawing.Color.Black;
			this.numLimitPrice.Increment = new decimal(new int[] {
            10,
            0,
            0,
            0});
			this.numLimitPrice.Location = new System.Drawing.Point(2, 19);
			this.numLimitPrice.Maximum = new decimal(new int[] {
            1215752191,
            23,
            0,
            0});
			this.numLimitPrice.Name = "numLimitPrice";
			this.numLimitPrice.Size = new System.Drawing.Size(75, 21);
			this.numLimitPrice.TabIndex = 3;
			this.numLimitPrice.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
			// 
			// lblLimitPrice
			// 
			this.lblLimitPrice.AutoSize = true;
			this.lblLimitPrice.Dock = System.Windows.Forms.DockStyle.Top;
			this.lblLimitPrice.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblLimitPrice.Location = new System.Drawing.Point(2, 4);
			this.lblLimitPrice.Name = "lblLimitPrice";
			this.lblLimitPrice.Padding = new System.Windows.Forms.Padding(0, 0, 0, 2);
			this.lblLimitPrice.Size = new System.Drawing.Size(55, 15);
			this.lblLimitPrice.TabIndex = 0;
			this.lblLimitPrice.Text = "Limit Price";
			// 
			// panelShares
			// 
			this.panelShares.BackColor = System.Drawing.SystemColors.ControlDarkDark;
			this.panelShares.Controls.Add(this.dspQuantityIncrement);
			this.panelShares.Controls.Add(this.numQuantity);
			this.panelShares.Controls.Add(this.lblQuantity);
			this.panelShares.Dock = System.Windows.Forms.DockStyle.Left;
			this.panelShares.Location = new System.Drawing.Point(1, 1);
			this.panelShares.MinimumSize = new System.Drawing.Size(68, 55);
			this.panelShares.Name = "panelShares";
			this.panelShares.Padding = new System.Windows.Forms.Padding(2, 4, 2, 2);
			this.panelShares.Size = new System.Drawing.Size(68, 56);
			this.panelShares.TabIndex = 1;
			// 
			// dspQuantityIncrement
			// 
			this.dspQuantityIncrement.AutoColor = false;
			this.dspQuantityIncrement.AutoSize = true;
			this.dspQuantityIncrement.BackColor = System.Drawing.SystemColors.ControlText;
			this.dspQuantityIncrement.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.dspQuantityIncrement.DefaultColor = System.Drawing.Color.White;
			this.dspQuantityIncrement.DefaultText = "100";
			this.dspQuantityIncrement.DisplayFactor = 1;
			this.dspQuantityIncrement.Dock = System.Windows.Forms.DockStyle.Right;
			this.dspQuantityIncrement.DownColor = System.Drawing.Color.Red;
			this.dspQuantityIncrement.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspQuantityIncrement.ForeColor = System.Drawing.Color.Gainsboro;
			this.dspQuantityIncrement.Is64Th = false;
			this.dspQuantityIncrement.IsCurrency = false;
			this.dspQuantityIncrement.IsFraction = false;
			this.dspQuantityIncrement.IsPercent = false;
			this.dspQuantityIncrement.Location = new System.Drawing.Point(41, 40);
			this.dspQuantityIncrement.MaxDecimal = 0;
			this.dspQuantityIncrement.Name = "dspQuantityIncrement";
			this.dspQuantityIncrement.Size = new System.Drawing.Size(25, 14);
			this.dspQuantityIncrement.TabIndex = 5;
			this.dspQuantityIncrement.Text = "100";
			this.dspQuantityIncrement.TickSize = 0.01;
			this.dspQuantityIncrement.UpColor = System.Drawing.Color.LimeGreen;
			this.dspQuantityIncrement.Value = "";
			// 
			// numQuantity
			// 
			this.numQuantity.BackColor = System.Drawing.Color.White;
			this.numQuantity.Dock = System.Windows.Forms.DockStyle.Top;
			this.numQuantity.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.numQuantity.ForeColor = System.Drawing.Color.Black;
			this.numQuantity.Increment = new decimal(new int[] {
            100,
            0,
            0,
            0});
			this.numQuantity.Location = new System.Drawing.Point(2, 19);
			this.numQuantity.Maximum = new decimal(new int[] {
            1215752191,
            23,
            0,
            0});
			this.numQuantity.Name = "numQuantity";
			this.numQuantity.Size = new System.Drawing.Size(64, 21);
			this.numQuantity.TabIndex = 1;
			this.numQuantity.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
			// 
			// lblQuantity
			// 
			this.lblQuantity.AutoSize = true;
			this.lblQuantity.Dock = System.Windows.Forms.DockStyle.Top;
			this.lblQuantity.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblQuantity.Location = new System.Drawing.Point(2, 4);
			this.lblQuantity.Name = "lblQuantity";
			this.lblQuantity.Padding = new System.Windows.Forms.Padding(0, 0, 0, 2);
			this.lblQuantity.Size = new System.Drawing.Size(23, 15);
			this.lblQuantity.TabIndex = 0;
			this.lblQuantity.Text = "Qty";
			// 
			// panelTop
			// 
			this.panelTop.Controls.Add(this.dspMessage);
			this.panelTop.Controls.Add(this.cmdClose);
			this.panelTop.Dock = System.Windows.Forms.DockStyle.Fill;
			this.panelTop.Location = new System.Drawing.Point(82, 58);
			this.panelTop.Name = "panelTop";
			this.panelTop.Size = new System.Drawing.Size(362, 19);
			this.panelTop.TabIndex = 4;
			// 
			// dspMessage
			// 
			this.dspMessage.AutoEllipsis = true;
			this.dspMessage.BackColor = System.Drawing.SystemColors.ControlDarkDark;
			this.dspMessage.Location = new System.Drawing.Point(1, 1);
			this.dspMessage.Name = "dspMessage";
			this.dspMessage.Size = new System.Drawing.Size(343, 17);
			this.dspMessage.TabIndex = 2;
			this.dspMessage.Text = "Message";
			this.dspMessage.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			// 
			// cmdClose
			// 
			this.cmdClose.BackColor = System.Drawing.SystemColors.ControlDarkDark;
			this.cmdClose.Image = global::ROC.Properties.Resources.Close;
			this.cmdClose.Location = new System.Drawing.Point(344, 1);
			this.cmdClose.Name = "cmdClose";
			this.cmdClose.Size = new System.Drawing.Size(17, 17);
			this.cmdClose.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
			this.cmdClose.TabIndex = 1;
			this.cmdClose.TabStop = false;
			this.cmdClose.Click += new System.EventHandler(this.cmdClose_Click);
			// 
			// menuBaseOrderModification
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.BackColor = System.Drawing.Color.Transparent;
			this.Controls.Add(this.panelTop);
			this.Controls.Add(this.panelDetails);
			this.Controls.Add(this.panelCommands);
			this.ForeColor = System.Drawing.Color.Gainsboro;
			this.MinimumSize = new System.Drawing.Size(364, 77);
			this.Name = "menuBaseOrderModification";
			this.Size = new System.Drawing.Size(444, 77);
			this.panelCommands.ResumeLayout(false);
			this.panelCommandsInside.ResumeLayout(false);
			this.panelCommandsInside.PerformLayout();
			this.panelDetails.ResumeLayout(false);
			this.panelShow.ResumeLayout(false);
			this.panelShow.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.numShow)).EndInit();
			this.panelOrderTypes.ResumeLayout(false);
			this.panelOrderTypes.PerformLayout();
			this.panelDuration.ResumeLayout(false);
			this.panelDuration.PerformLayout();
			this.panelPegPrice.ResumeLayout(false);
			this.panelPegPrice.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.numPegPrice)).EndInit();
			this.panelStopPrice.ResumeLayout(false);
			this.panelStopPrice.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.numStopPrice)).EndInit();
			this.panelLimitPrice.ResumeLayout(false);
			this.panelLimitPrice.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.numLimitPrice)).EndInit();
			this.panelShares.ResumeLayout(false);
			this.panelShares.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.numQuantity)).EndInit();
			this.panelTop.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.cmdClose)).EndInit();
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.Panel panelCommands;
		private ButtonEx.VistaButton cmdReplace;
		private ButtonEx.VistaButton cmdCancel;
		private System.Windows.Forms.Panel panelDetails;
		private System.Windows.Forms.Label lblQuantity;
		private System.Windows.Forms.Panel panelDuration;
		private System.Windows.Forms.Label lblDuration;
		private LabelEx.LabelBase lblStopPriceX;
		private System.Windows.Forms.Label lblStopPrice;
		private LabelEx.LabelBase lblLimitPriceX;
		private System.Windows.Forms.Label lblLimitPrice;
		private System.Windows.Forms.Panel panelOrderTypes;
		private System.Windows.Forms.Label lblOrderType;
		private ButtonEx.VistaButton cmdCancelAll;
		private System.Windows.Forms.Panel panelTop;
		private System.Windows.Forms.PictureBox cmdClose;
		protected internal System.Windows.Forms.Panel panelShares;
		protected internal LabelEx.LabelBase dspQuantityIncrement;
		protected internal System.Windows.Forms.NumericUpDown numQuantity;
		protected internal System.Windows.Forms.ComboBox cboDuration;
		protected internal System.Windows.Forms.Panel panelStopPrice;
		protected internal LabelEx.LabelBase dspStopPriceTickSize;
		protected internal LabelEx.LabelBase dspStopPriceIncrement;
		protected internal System.Windows.Forms.NumericUpDown numStopPrice;
		protected internal System.Windows.Forms.Panel panelLimitPrice;
		protected internal LabelEx.LabelBase dspLimitPriceTickSize;
		protected internal LabelEx.LabelBase dspLimitPriceIncrement;
		protected internal System.Windows.Forms.NumericUpDown numLimitPrice;
		protected internal System.Windows.Forms.ComboBox cboOrder;
		protected internal System.Windows.Forms.Panel panelShow;
		protected internal LabelEx.LabelBase dspShowIncrement;
		protected internal System.Windows.Forms.NumericUpDown numShow;
		protected internal System.Windows.Forms.CheckBox chkShow;
		protected internal System.Windows.Forms.Label dspMessage;
		protected internal System.Windows.Forms.Panel panelPegPrice;
		protected internal LabelEx.LabelBase dspPegPriceTickSize;
		private LabelEx.LabelBase lblPegPriceX;
		protected internal LabelEx.LabelBase dspPegPriceIncrement;
		protected internal System.Windows.Forms.NumericUpDown numPegPrice;
		private System.Windows.Forms.Label lblPegPrice;
		private System.Windows.Forms.Panel panelCommandsInside;
	}
}
