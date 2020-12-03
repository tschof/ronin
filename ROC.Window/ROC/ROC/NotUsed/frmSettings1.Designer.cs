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
			System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle1 = new System.Windows.Forms.DataGridViewCellStyle();
			System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle2 = new System.Windows.Forms.DataGridViewCellStyle();
			System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle3 = new System.Windows.Forms.DataGridViewCellStyle();
			this.CaptionBar = new FormEx.VistaWindowCaptionBar();
			this.panelSelections = new PanelEx.VistaPanel();
			this.cmdGeneral = new ButtonEx.VistaButton();
			this.panelManagerSettings = new PanelEx.PanelManager();
			this.managedPanelGeneral = new PanelEx.ManagedPanel();
			this.cmdApply = new ButtonEx.VistaButton();
			this.cmdCancel = new ButtonEx.VistaButton();
			this.DGVAutoFocusSettings = new System.Windows.Forms.DataGridView();
			this.Window = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.Enable = new System.Windows.Forms.DataGridViewCheckBoxColumn();
			this.lblAutoFocus = new LabelEx.LabelRotatable();
			this.panelSelections.SuspendLayout();
			this.panelManagerSettings.SuspendLayout();
			this.managedPanelGeneral.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.DGVAutoFocusSettings)).BeginInit();
			this.SuspendLayout();
			// 
			// CaptionBar
			// 
			this.CaptionBar.BackColor = System.Drawing.Color.Transparent;
			this.CaptionBar.Dock = System.Windows.Forms.DockStyle.Top;
			this.CaptionBar.Location = new System.Drawing.Point(0, 0);
			this.CaptionBar.Name = "CaptionBar";
			this.CaptionBar.Size = new System.Drawing.Size(172, 26);
			this.CaptionBar.TabIndex = 2;
			this.CaptionBar.TabStop = false;
			// 
			// panelSelections
			// 
			this.panelSelections.BackColor = System.Drawing.Color.Black;
			this.panelSelections.Controls.Add(this.cmdGeneral);
			this.panelSelections.Dock = System.Windows.Forms.DockStyle.Top;
			this.panelSelections.Location = new System.Drawing.Point(0, 26);
			this.panelSelections.Name = "panelSelections";
			this.panelSelections.Size = new System.Drawing.Size(172, 24);
			this.panelSelections.TabIndex = 3;
			// 
			// cmdGeneral
			// 
			this.cmdGeneral.BackColor = System.Drawing.Color.Black;
			this.cmdGeneral.Dock = System.Windows.Forms.DockStyle.Left;
			this.cmdGeneral.Location = new System.Drawing.Point(0, 0);
			this.cmdGeneral.Name = "cmdGeneral";
			this.cmdGeneral.Size = new System.Drawing.Size(54, 24);
			this.cmdGeneral.TabIndex = 0;
			this.cmdGeneral.Text = "General";
			// 
			// panelManagerSettings
			// 
			this.panelManagerSettings.Controls.Add(this.managedPanelGeneral);
			this.panelManagerSettings.Dock = System.Windows.Forms.DockStyle.Fill;
			this.panelManagerSettings.Location = new System.Drawing.Point(0, 50);
			this.panelManagerSettings.Name = "panelManagerSettings";
			this.panelManagerSettings.SelectedIndex = 0;
			this.panelManagerSettings.SelectedPanel = this.managedPanelGeneral;
			this.panelManagerSettings.Size = new System.Drawing.Size(172, 297);
			this.panelManagerSettings.TabIndex = 4;
			this.panelManagerSettings.TabStop = false;
			// 
			// managedPanelGeneral
			// 
			this.managedPanelGeneral.Controls.Add(this.cmdApply);
			this.managedPanelGeneral.Controls.Add(this.cmdCancel);
			this.managedPanelGeneral.Controls.Add(this.DGVAutoFocusSettings);
			this.managedPanelGeneral.Controls.Add(this.lblAutoFocus);
			this.managedPanelGeneral.Location = new System.Drawing.Point(0, 0);
			this.managedPanelGeneral.Name = "managedPanelGeneral";
			this.managedPanelGeneral.Size = new System.Drawing.Size(172, 297);
			this.managedPanelGeneral.Text = "managedPanelGeneral";
			// 
			// cmdApply
			// 
			this.cmdApply.BackColor = System.Drawing.Color.LimeGreen;
			this.cmdApply.Dock = System.Windows.Forms.DockStyle.Right;
			this.cmdApply.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdApply.ForeColor = System.Drawing.Color.Black;
			this.cmdApply.Location = new System.Drawing.Point(22, 276);
			this.cmdApply.Name = "cmdApply";
			this.cmdApply.Size = new System.Drawing.Size(75, 21);
			this.cmdApply.TabIndex = 3;
			this.cmdApply.Text = "Apply";
			this.cmdApply.Click += new System.EventHandler(this.cmdApply_Click);
			// 
			// cmdCancel
			// 
			this.cmdCancel.BackColor = System.Drawing.Color.Red;
			this.cmdCancel.Dock = System.Windows.Forms.DockStyle.Right;
			this.cmdCancel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdCancel.ForeColor = System.Drawing.Color.Black;
			this.cmdCancel.Location = new System.Drawing.Point(97, 276);
			this.cmdCancel.Name = "cmdCancel";
			this.cmdCancel.Size = new System.Drawing.Size(75, 21);
			this.cmdCancel.TabIndex = 4;
			this.cmdCancel.Text = "Cancel";
			this.cmdCancel.Click += new System.EventHandler(this.cmdCancel_Click);
			// 
			// DGVAutoFocusSettings
			// 
			this.DGVAutoFocusSettings.AllowUserToAddRows = false;
			this.DGVAutoFocusSettings.AllowUserToDeleteRows = false;
			this.DGVAutoFocusSettings.AllowUserToResizeRows = false;
			this.DGVAutoFocusSettings.BorderStyle = System.Windows.Forms.BorderStyle.None;
			this.DGVAutoFocusSettings.ColumnHeadersBorderStyle = System.Windows.Forms.DataGridViewHeaderBorderStyle.Single;
			dataGridViewCellStyle1.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
			dataGridViewCellStyle1.BackColor = System.Drawing.SystemColors.ActiveCaption;
			dataGridViewCellStyle1.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			dataGridViewCellStyle1.ForeColor = System.Drawing.SystemColors.ActiveCaptionText;
			dataGridViewCellStyle1.Padding = new System.Windows.Forms.Padding(2);
			dataGridViewCellStyle1.SelectionBackColor = System.Drawing.SystemColors.Highlight;
			dataGridViewCellStyle1.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
			dataGridViewCellStyle1.WrapMode = System.Windows.Forms.DataGridViewTriState.False;
			this.DGVAutoFocusSettings.ColumnHeadersDefaultCellStyle = dataGridViewCellStyle1;
			this.DGVAutoFocusSettings.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
			this.DGVAutoFocusSettings.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.Window,
            this.Enable});
			this.DGVAutoFocusSettings.Dock = System.Windows.Forms.DockStyle.Top;
			this.DGVAutoFocusSettings.Location = new System.Drawing.Point(21, 0);
			this.DGVAutoFocusSettings.MultiSelect = false;
			this.DGVAutoFocusSettings.Name = "DGVAutoFocusSettings";
			this.DGVAutoFocusSettings.RowHeadersVisible = false;
			this.DGVAutoFocusSettings.ScrollBars = System.Windows.Forms.ScrollBars.None;
			this.DGVAutoFocusSettings.Size = new System.Drawing.Size(151, 276);
			this.DGVAutoFocusSettings.TabIndex = 1;
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
			this.lblAutoFocus.DefaultColor = System.Drawing.Color.White;
			this.lblAutoFocus.DefaultText = "";
			this.lblAutoFocus.DisplayFactor = 1;
			this.lblAutoFocus.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblAutoFocus.DownColor = System.Drawing.Color.Red;
			this.lblAutoFocus.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblAutoFocus.ForeColor = System.Drawing.SystemColors.ActiveCaptionText;
			this.lblAutoFocus.IsCurrency = false;
			this.lblAutoFocus.IsFraction = false;
			this.lblAutoFocus.IsPercent = false;
			this.lblAutoFocus.Location = new System.Drawing.Point(0, 0);
			this.lblAutoFocus.MaxDecimal = 7;
			this.lblAutoFocus.Name = "lblAutoFocus";
			this.lblAutoFocus.RotationAngle = 270;
			this.lblAutoFocus.Size = new System.Drawing.Size(21, 297);
			this.lblAutoFocus.TabIndex = 2;
			this.lblAutoFocus.Text = "Auto Focus Settings";
			this.lblAutoFocus.TextDirection = LabelEx.LabelRotatable.Direction.Clockwise;
			this.lblAutoFocus.TextOrientation = LabelEx.LabelRotatable.Orientation.Rotate;
			this.lblAutoFocus.TickSize = 0.01;
			this.lblAutoFocus.UpColor = System.Drawing.Color.LimeGreen;
			this.lblAutoFocus.Value = "";
			// 
			// frmSettings
			// 
			this.AnimationType = FormEx.VistaWindowBase.AnimationTypes.Slide;
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.Caption = "Settings";
			this.ClientSize = new System.Drawing.Size(172, 347);
			this.Controls.Add(this.panelManagerSettings);
			this.Controls.Add(this.panelSelections);
			this.Controls.Add(this.CaptionBar);
			this.MaximumSize = new System.Drawing.Size(180, 355);
			this.MinimumSize = new System.Drawing.Size(180, 355);
			this.Name = "frmSettings";
			this.ShowInTaskbar = false;
			this.TopMost = true;
			this.panelSelections.ResumeLayout(false);
			this.panelManagerSettings.ResumeLayout(false);
			this.managedPanelGeneral.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.DGVAutoFocusSettings)).EndInit();
			this.ResumeLayout(false);

		}

		#endregion

		private FormEx.VistaWindowCaptionBar CaptionBar;
		private PanelEx.VistaPanel panelSelections;
		private ButtonEx.VistaButton cmdGeneral;
		private PanelEx.PanelManager panelManagerSettings;
		private PanelEx.ManagedPanel managedPanelGeneral;
		private System.Windows.Forms.DataGridView DGVAutoFocusSettings;
		private LabelEx.LabelRotatable lblAutoFocus;
		private ButtonEx.VistaButton cmdApply;
		private ButtonEx.VistaButton cmdCancel;
		private System.Windows.Forms.DataGridViewTextBoxColumn Window;
		private System.Windows.Forms.DataGridViewCheckBoxColumn Enable;
	}
}