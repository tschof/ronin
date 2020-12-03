namespace AdminTool
{
	partial class frmMain
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
			this.tabMain = new System.Windows.Forms.TabControl();
			this.tabPageRDS = new System.Windows.Forms.TabPage();
			this.tabPageReleases = new System.Windows.Forms.TabPage();
			this.tabPageMDS = new System.Windows.Forms.TabPage();
			this.dgvMDSStatus = new System.Windows.Forms.DataGridView();
			this.tabMain.SuspendLayout();
			this.tabPageMDS.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.dgvMDSStatus)).BeginInit();
			this.SuspendLayout();
			// 
			// tabMain
			// 
			this.tabMain.Controls.Add(this.tabPageMDS);
			this.tabMain.Controls.Add(this.tabPageRDS);
			this.tabMain.Controls.Add(this.tabPageReleases);
			this.tabMain.Dock = System.Windows.Forms.DockStyle.Fill;
			this.tabMain.Location = new System.Drawing.Point(0, 0);
			this.tabMain.Name = "tabMain";
			this.tabMain.SelectedIndex = 0;
			this.tabMain.Size = new System.Drawing.Size(292, 273);
			this.tabMain.TabIndex = 0;
			// 
			// tabPageRDS
			// 
			this.tabPageRDS.Location = new System.Drawing.Point(4, 22);
			this.tabPageRDS.Name = "tabPageRDS";
			this.tabPageRDS.Padding = new System.Windows.Forms.Padding(3);
			this.tabPageRDS.Size = new System.Drawing.Size(284, 247);
			this.tabPageRDS.TabIndex = 0;
			this.tabPageRDS.Text = "RDS";
			this.tabPageRDS.UseVisualStyleBackColor = true;
			// 
			// tabPageReleases
			// 
			this.tabPageReleases.Location = new System.Drawing.Point(4, 22);
			this.tabPageReleases.Name = "tabPageReleases";
			this.tabPageReleases.Padding = new System.Windows.Forms.Padding(3);
			this.tabPageReleases.Size = new System.Drawing.Size(284, 247);
			this.tabPageReleases.TabIndex = 1;
			this.tabPageReleases.Text = "Releases";
			this.tabPageReleases.UseVisualStyleBackColor = true;
			// 
			// tabPageMDS
			// 
			this.tabPageMDS.Controls.Add(this.dgvMDSStatus);
			this.tabPageMDS.Location = new System.Drawing.Point(4, 22);
			this.tabPageMDS.Name = "tabPageMDS";
			this.tabPageMDS.Size = new System.Drawing.Size(284, 247);
			this.tabPageMDS.TabIndex = 2;
			this.tabPageMDS.Text = "MDS";
			this.tabPageMDS.UseVisualStyleBackColor = true;
			// 
			// dgvMDSStatus
			// 
			this.dgvMDSStatus.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
			this.dgvMDSStatus.Dock = System.Windows.Forms.DockStyle.Fill;
			this.dgvMDSStatus.Location = new System.Drawing.Point(0, 0);
			this.dgvMDSStatus.Name = "dgvMDSStatus";
			this.dgvMDSStatus.Size = new System.Drawing.Size(284, 247);
			this.dgvMDSStatus.TabIndex = 0;
			// 
			// frmMain
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(292, 273);
			this.Controls.Add(this.tabMain);
			this.Name = "frmMain";
			this.Text = "Admin Tool";
			this.tabMain.ResumeLayout(false);
			this.tabPageMDS.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.dgvMDSStatus)).EndInit();
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.TabControl tabMain;
		private System.Windows.Forms.TabPage tabPageMDS;
		private System.Windows.Forms.TabPage tabPageRDS;
		private System.Windows.Forms.TabPage tabPageReleases;
		private System.Windows.Forms.DataGridView dgvMDSStatus;
	}
}

