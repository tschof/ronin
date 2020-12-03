namespace ROC
{
	partial class frmSounds
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
			System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle4 = new System.Windows.Forms.DataGridViewCellStyle();
			System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle5 = new System.Windows.Forms.DataGridViewCellStyle();
			System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle6 = new System.Windows.Forms.DataGridViewCellStyle();
			this.CaptionBar = new FormEx.VistaWindowCaptionBar();
			this.panelBackground = new PanelEx.VistaPanel();
			this.cmdApply = new ButtonEx.VistaButton();
			this.cmdCancel = new ButtonEx.VistaButton();
			this.DGVSounds = new System.Windows.Forms.DataGridView();
			this.Type = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.Path = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.Browse = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.Test = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.Enable = new System.Windows.Forms.DataGridViewCheckBoxColumn();
			this.panelBackground.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.DGVSounds)).BeginInit();
			this.SuspendLayout();
			// 
			// CaptionBar
			// 
			this.CaptionBar.BackColor = System.Drawing.Color.Transparent;
			this.CaptionBar.Dock = System.Windows.Forms.DockStyle.Top;
			this.CaptionBar.Location = new System.Drawing.Point(0, 0);
			this.CaptionBar.Name = "CaptionBar";
			this.CaptionBar.Size = new System.Drawing.Size(535, 26);
			this.CaptionBar.TabIndex = 1;
			this.CaptionBar.TabStop = false;
			// 
			// panelBackground
			// 
			this.panelBackground.BackColor = System.Drawing.Color.Black;
			this.panelBackground.Controls.Add(this.cmdApply);
			this.panelBackground.Controls.Add(this.cmdCancel);
			this.panelBackground.Controls.Add(this.DGVSounds);
			this.panelBackground.Dock = System.Windows.Forms.DockStyle.Fill;
			this.panelBackground.Location = new System.Drawing.Point(0, 26);
			this.panelBackground.Name = "panelBackground";
			this.panelBackground.Padding = new System.Windows.Forms.Padding(2);
			this.panelBackground.Size = new System.Drawing.Size(535, 219);
			this.panelBackground.TabIndex = 2;
			// 
			// cmdApply
			// 
			this.cmdApply.BackColor = System.Drawing.Color.LimeGreen;
			this.cmdApply.Dock = System.Windows.Forms.DockStyle.Right;
			this.cmdApply.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdApply.ForeColor = System.Drawing.Color.Black;
			this.cmdApply.Location = new System.Drawing.Point(383, 196);
			this.cmdApply.Name = "cmdApply";
			this.cmdApply.Size = new System.Drawing.Size(75, 21);
			this.cmdApply.TabIndex = 1;
			this.cmdApply.Text = "Apply";
			this.cmdApply.Click += new System.EventHandler(this.cmdApply_Click);
			// 
			// cmdCancel
			// 
			this.cmdCancel.BackColor = System.Drawing.Color.Red;
			this.cmdCancel.Dock = System.Windows.Forms.DockStyle.Right;
			this.cmdCancel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdCancel.ForeColor = System.Drawing.Color.Black;
			this.cmdCancel.Location = new System.Drawing.Point(458, 196);
			this.cmdCancel.Name = "cmdCancel";
			this.cmdCancel.Size = new System.Drawing.Size(75, 21);
			this.cmdCancel.TabIndex = 2;
			this.cmdCancel.Text = "Cancel";
			this.cmdCancel.Click += new System.EventHandler(this.cmdCancel_Click);
			// 
			// DGVSounds
			// 
			this.DGVSounds.AllowUserToAddRows = false;
			this.DGVSounds.AllowUserToDeleteRows = false;
			this.DGVSounds.AllowUserToResizeRows = false;
			this.DGVSounds.BorderStyle = System.Windows.Forms.BorderStyle.None;
			this.DGVSounds.ColumnHeadersBorderStyle = System.Windows.Forms.DataGridViewHeaderBorderStyle.Single;
			dataGridViewCellStyle1.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
			dataGridViewCellStyle1.BackColor = System.Drawing.SystemColors.Control;
			dataGridViewCellStyle1.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			dataGridViewCellStyle1.ForeColor = System.Drawing.SystemColors.WindowText;
			dataGridViewCellStyle1.Padding = new System.Windows.Forms.Padding(2);
			dataGridViewCellStyle1.SelectionBackColor = System.Drawing.SystemColors.Highlight;
			dataGridViewCellStyle1.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
			dataGridViewCellStyle1.WrapMode = System.Windows.Forms.DataGridViewTriState.False;
			this.DGVSounds.ColumnHeadersDefaultCellStyle = dataGridViewCellStyle1;
			this.DGVSounds.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
			this.DGVSounds.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.Type,
            this.Path,
            this.Browse,
            this.Test,
            this.Enable});
			this.DGVSounds.Dock = System.Windows.Forms.DockStyle.Top;
			this.DGVSounds.Location = new System.Drawing.Point(2, 2);
			this.DGVSounds.MultiSelect = false;
			this.DGVSounds.Name = "DGVSounds";
			this.DGVSounds.RowHeadersVisible = false;
			this.DGVSounds.ScrollBars = System.Windows.Forms.ScrollBars.None;
			this.DGVSounds.Size = new System.Drawing.Size(531, 194);
			this.DGVSounds.TabIndex = 0;
			// 
			// Type
			// 
			this.Type.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
			dataGridViewCellStyle2.BackColor = System.Drawing.Color.Black;
			dataGridViewCellStyle2.ForeColor = System.Drawing.Color.Khaki;
			this.Type.DefaultCellStyle = dataGridViewCellStyle2;
			this.Type.FillWeight = 40.32626F;
			this.Type.HeaderText = "Type";
			this.Type.Name = "Type";
			this.Type.ReadOnly = true;
			this.Type.Resizable = System.Windows.Forms.DataGridViewTriState.False;
			this.Type.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable;
			// 
			// Path
			// 
			this.Path.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
			dataGridViewCellStyle3.BackColor = System.Drawing.Color.Black;
			this.Path.DefaultCellStyle = dataGridViewCellStyle3;
			this.Path.FillWeight = 116.8638F;
			this.Path.HeaderText = "Path";
			this.Path.Name = "Path";
			this.Path.Resizable = System.Windows.Forms.DataGridViewTriState.False;
			this.Path.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable;
			// 
			// Browse
			// 
			this.Browse.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
			dataGridViewCellStyle4.BackColor = System.Drawing.SystemColors.ButtonFace;
			dataGridViewCellStyle4.ForeColor = System.Drawing.Color.Black;
			this.Browse.DefaultCellStyle = dataGridViewCellStyle4;
			this.Browse.FillWeight = 23.37277F;
			this.Browse.HeaderText = "";
			this.Browse.Name = "Browse";
			this.Browse.ReadOnly = true;
			this.Browse.Resizable = System.Windows.Forms.DataGridViewTriState.False;
			this.Browse.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable;
			// 
			// Test
			// 
			this.Test.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
			dataGridViewCellStyle5.BackColor = System.Drawing.SystemColors.ButtonFace;
			dataGridViewCellStyle5.ForeColor = System.Drawing.Color.Black;
			this.Test.DefaultCellStyle = dataGridViewCellStyle5;
			this.Test.FillWeight = 23.37277F;
			this.Test.HeaderText = "";
			this.Test.Name = "Test";
			this.Test.ReadOnly = true;
			this.Test.Resizable = System.Windows.Forms.DataGridViewTriState.False;
			this.Test.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable;
			// 
			// Enable
			// 
			dataGridViewCellStyle6.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleCenter;
			dataGridViewCellStyle6.BackColor = System.Drawing.SystemColors.ButtonFace;
			dataGridViewCellStyle6.NullValue = false;
			this.Enable.DefaultCellStyle = dataGridViewCellStyle6;
			this.Enable.HeaderText = "E";
			this.Enable.Name = "Enable";
			this.Enable.Width = 20;
			// 
			// frmSounds
			// 
			this.AnimationType = FormEx.VistaWindowBase.AnimationTypes.Slide;
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.Caption = "Sounds";
			this.ClientSize = new System.Drawing.Size(535, 245);
			this.Controls.Add(this.panelBackground);
			this.Controls.Add(this.CaptionBar);
			this.MaximumSize = new System.Drawing.Size(543, 253);
			this.MinimumSize = new System.Drawing.Size(543, 253);
			this.Name = "frmSounds";
			this.ShowInTaskbar = false;
			this.TopMost = true;
			this.panelBackground.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.DGVSounds)).EndInit();
			this.ResumeLayout(false);

		}

		#endregion

		private FormEx.VistaWindowCaptionBar CaptionBar;
		private PanelEx.VistaPanel panelBackground;
		private System.Windows.Forms.DataGridView DGVSounds;
		private ButtonEx.VistaButton cmdCancel;
		private ButtonEx.VistaButton cmdApply;
		private System.Windows.Forms.DataGridViewTextBoxColumn Type;
		private System.Windows.Forms.DataGridViewTextBoxColumn Path;
		private System.Windows.Forms.DataGridViewTextBoxColumn Browse;
		private System.Windows.Forms.DataGridViewTextBoxColumn Test;
		private System.Windows.Forms.DataGridViewCheckBoxColumn Enable;
	}
}