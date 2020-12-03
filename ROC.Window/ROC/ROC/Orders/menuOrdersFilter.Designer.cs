namespace ROC
{
	partial class menuOrdersFilter
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
			this.lblFilterStocks = new System.Windows.Forms.Label();
			this.lblFilterFutures = new System.Windows.Forms.Label();
			this.lblFilterOptions = new System.Windows.Forms.Label();
			this.SuspendLayout();
			// 
			// lblFilterStocks
			// 
			this.lblFilterStocks.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblFilterStocks.ForeColor = System.Drawing.Color.White;
			this.lblFilterStocks.Location = new System.Drawing.Point(0, 0);
			this.lblFilterStocks.Name = "lblFilterStocks";
			this.lblFilterStocks.Size = new System.Drawing.Size(45, 24);
			this.lblFilterStocks.TabIndex = 1;
			this.lblFilterStocks.Text = "Stocks";
			this.lblFilterStocks.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			this.lblFilterStocks.Click += new System.EventHandler(this.lblFilterStocks_Click);
			// 
			// lblFilterFutures
			// 
			this.lblFilterFutures.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblFilterFutures.ForeColor = System.Drawing.Color.White;
			this.lblFilterFutures.Location = new System.Drawing.Point(45, 0);
			this.lblFilterFutures.Name = "lblFilterFutures";
			this.lblFilterFutures.Size = new System.Drawing.Size(47, 24);
			this.lblFilterFutures.TabIndex = 2;
			this.lblFilterFutures.Text = "Futures";
			this.lblFilterFutures.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			this.lblFilterFutures.Click += new System.EventHandler(this.lblFilterFutures_Click);
			// 
			// lblFilterOptions
			// 
			this.lblFilterOptions.Dock = System.Windows.Forms.DockStyle.Fill;
			this.lblFilterOptions.ForeColor = System.Drawing.Color.White;
			this.lblFilterOptions.Location = new System.Drawing.Point(92, 0);
			this.lblFilterOptions.Name = "lblFilterOptions";
			this.lblFilterOptions.Size = new System.Drawing.Size(52, 24);
			this.lblFilterOptions.TabIndex = 3;
			this.lblFilterOptions.Text = "Options";
			this.lblFilterOptions.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			this.lblFilterOptions.Click += new System.EventHandler(this.lblFilterOptions_Click);
			// 
			// menuOrdersFilter
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.BackColor = System.Drawing.Color.Black;
			this.Controls.Add(this.lblFilterOptions);
			this.Controls.Add(this.lblFilterFutures);
			this.Controls.Add(this.lblFilterStocks);
			this.ForeColor = System.Drawing.Color.White;
			this.Name = "menuOrdersFilter";
			this.Size = new System.Drawing.Size(144, 24);
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.Label lblFilterStocks;
		private System.Windows.Forms.Label lblFilterFutures;
		private System.Windows.Forms.Label lblFilterOptions;

	}
}
