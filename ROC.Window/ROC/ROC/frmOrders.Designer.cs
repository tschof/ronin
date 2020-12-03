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
			this.CaptionBar = new FormEx.VistaWindowCaptionBar();
			this.rocOrdersList = new DataGridViewEx.ROCOrderList();
			((System.ComponentModel.ISupportInitialize)(this.rocOrdersList)).BeginInit();
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
			this.CaptionBar.ShowRefresh = true;
			this.CaptionBar.Size = new System.Drawing.Size(292, 26);
			this.CaptionBar.TabIndex = 0;
			// 
			// rocOrdersList
			// 
			this.rocOrdersList.AllowUserToOrderColumns = true;
			this.rocOrdersList.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
			this.rocOrdersList.Dock = System.Windows.Forms.DockStyle.Fill;
			this.rocOrdersList.Location = new System.Drawing.Point(0, 26);
			this.rocOrdersList.Name = "rocOrdersList";
			this.rocOrdersList.Size = new System.Drawing.Size(292, 266);
			this.rocOrdersList.TabIndex = 1;
			// 
			// frmOrders
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.Caption = "Orders";
			this.ClientSize = new System.Drawing.Size(292, 292);
			this.Controls.Add(this.rocOrdersList);
			this.Controls.Add(this.CaptionBar);
			this.Name = "frmOrders";
			this.ShowInTaskbar = false;
			this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
			((System.ComponentModel.ISupportInitialize)(this.rocOrdersList)).EndInit();
			this.ResumeLayout(false);

		}

		#endregion

		private FormEx.VistaWindowCaptionBar CaptionBar;
		private DataGridViewEx.ROCOrderList rocOrdersList;
	}
}