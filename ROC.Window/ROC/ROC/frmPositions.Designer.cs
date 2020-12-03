namespace FontDialogEx
{
	partial class frmPositions
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
			this.CaptionBar = new FormEx.VistaWindowCaptionBar();
			this.rocPositions = new DataGridViewEx.ROCPositionList();
			((System.ComponentModel.ISupportInitialize)(this.rocPositions)).BeginInit();
			this.SuspendLayout();
			// 
			// CaptionBar
			// 
			this.CaptionBar.BackColor = System.Drawing.Color.Transparent;
			this.CaptionBar.Dock = System.Windows.Forms.DockStyle.Top;
			this.CaptionBar.EnableCaptionEdit = true;
			this.CaptionBar.Location = new System.Drawing.Point(0, 0);
			this.CaptionBar.Name = "CaptionBar";
			this.CaptionBar.ShowFit = true;
			this.CaptionBar.Size = new System.Drawing.Size(292, 26);
			this.CaptionBar.TabIndex = 0;
			// 
			// rocPositions
			// 
			this.rocPositions.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
			this.rocPositions.Dock = System.Windows.Forms.DockStyle.Fill;
			this.rocPositions.Location = new System.Drawing.Point(0, 26);
			this.rocPositions.Name = "rocPositions";
			this.rocPositions.Size = new System.Drawing.Size(292, 266);
			this.rocPositions.TabIndex = 1;
			// 
			// frmPositions
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.Caption = "Positions";
			this.ClientSize = new System.Drawing.Size(292, 292);
			this.Controls.Add(this.rocPositions);
			this.Controls.Add(this.CaptionBar);
			this.Name = "frmPositions";
			this.ShowInTaskbar = false;
			this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
			((System.ComponentModel.ISupportInitialize)(this.rocPositions)).EndInit();
			this.ResumeLayout(false);

		}

		#endregion

		private FormEx.VistaWindowCaptionBar CaptionBar;
		private DataGridViewEx.ROCPositionList rocPositions;
	}
}