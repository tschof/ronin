namespace ROC
{
	partial class menuTradesFilter
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
			this.lblFilterOptions = new System.Windows.Forms.Label();
			this.lblFilterFutures = new System.Windows.Forms.Label();
			this.lblFilterStocks = new System.Windows.Forms.Label();
			this.SuspendLayout();
			// 
			// lblFilterOptions
			// 
			this.lblFilterOptions.Dock = System.Windows.Forms.DockStyle.Fill;
			this.lblFilterOptions.ForeColor = System.Drawing.Color.White;
			this.lblFilterOptions.Location = new System.Drawing.Point(92, 0);
			this.lblFilterOptions.Name = "lblFilterOptions";
			this.lblFilterOptions.Size = new System.Drawing.Size(52, 24);
			this.lblFilterOptions.TabIndex = 6;
			this.lblFilterOptions.Text = "Options";
			this.lblFilterOptions.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			this.lblFilterOptions.Click += new System.EventHandler(this.lblFilterOptions_Click);
			// 
			// lblFilterFutures
			// 
			this.lblFilterFutures.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblFilterFutures.ForeColor = System.Drawing.Color.White;
			this.lblFilterFutures.Location = new System.Drawing.Point(45, 0);
			this.lblFilterFutures.Name = "lblFilterFutures";
			this.lblFilterFutures.Size = new System.Drawing.Size(47, 24);
			this.lblFilterFutures.TabIndex = 5;
			this.lblFilterFutures.Text = "Futures";
			this.lblFilterFutures.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			this.lblFilterFutures.Click += new System.EventHandler(this.lblFilterFutures_Click);
			// 
			// lblFilterStocks
			// 
			this.lblFilterStocks.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblFilterStocks.ForeColor = System.Drawing.Color.White;
			this.lblFilterStocks.Location = new System.Drawing.Point(0, 0);
			this.lblFilterStocks.Name = "lblFilterStocks";
			this.lblFilterStocks.Size = new System.Drawing.Size(45, 24);
			this.lblFilterStocks.TabIndex = 4;
			this.lblFilterStocks.Text = "Stocks";
			this.lblFilterStocks.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			this.lblFilterStocks.Click += new System.EventHandler(this.lblFilterStocks_Click);
			// 
			// menuTradesFilter
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.BackColor = System.Drawing.Color.Black;
			this.Controls.Add(this.lblFilterOptions);
			this.Controls.Add(this.lblFilterFutures);
			this.Controls.Add(this.lblFilterStocks);
			this.ForeColor = System.Drawing.Color.White;
			this.Name = "menuTradesFilter";
			this.Size = new System.Drawing.Size(144, 24);
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.Label lblFilterOptions;
		private System.Windows.Forms.Label lblFilterFutures;
		private System.Windows.Forms.Label lblFilterStocks;
	}
}
