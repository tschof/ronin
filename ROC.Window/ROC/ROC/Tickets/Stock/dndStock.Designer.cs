namespace ROC
{
	partial class dndStock
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
			this.lblPrice = new LabelEx.LabelVista();
			this.dspPrice = new LabelEx.LabelVista();
			this.lblSize = new LabelEx.LabelVista();
			this.dspSize = new LabelEx.LabelVista();
			this.SuspendLayout();
			// 
			// lblPrice
			// 
			this.lblPrice.ApplyShine = true;
			this.lblPrice.AutoColor = false;
			this.lblPrice.DefaultColor = System.Drawing.Color.White;
			this.lblPrice.DefaultText = "";
			this.lblPrice.DisplayFactor = 1;
			this.lblPrice.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblPrice.DownColor = System.Drawing.Color.Red;
			this.lblPrice.Is64Th = false;
			this.lblPrice.IsCurrency = false;
			this.lblPrice.IsFraction = false;
			this.lblPrice.IsPercent = false;
			this.lblPrice.Location = new System.Drawing.Point(0, 0);
			this.lblPrice.MaxDecimal = 7;
			this.lblPrice.Name = "lblPrice";
			this.lblPrice.Size = new System.Drawing.Size(38, 19);
			this.lblPrice.TabIndex = 0;
			this.lblPrice.Text = "Price:";
			this.lblPrice.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			this.lblPrice.TickSize = 0.01;
			this.lblPrice.UpColor = System.Drawing.Color.LimeGreen;
			this.lblPrice.Value = "";
			// 
			// dspPrice
			// 
			this.dspPrice.AutoColor = false;
			this.dspPrice.DefaultColor = System.Drawing.Color.White;
			this.dspPrice.DefaultText = "0";
			this.dspPrice.DisplayFactor = 1;
			this.dspPrice.Dock = System.Windows.Forms.DockStyle.Left;
			this.dspPrice.DownColor = System.Drawing.Color.Red;
			this.dspPrice.Is64Th = false;
			this.dspPrice.IsCurrency = true;
			this.dspPrice.IsFraction = false;
			this.dspPrice.IsPercent = false;
			this.dspPrice.Location = new System.Drawing.Point(38, 0);
			this.dspPrice.MaxDecimal = 2;
			this.dspPrice.Name = "dspPrice";
			this.dspPrice.Size = new System.Drawing.Size(57, 19);
			this.dspPrice.TabIndex = 1;
			this.dspPrice.Text = "0.00";
			this.dspPrice.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.dspPrice.TickSize = 0.01;
			this.dspPrice.UpColor = System.Drawing.Color.LimeGreen;
			this.dspPrice.Value = "";
			// 
			// lblSize
			// 
			this.lblSize.ApplyShine = true;
			this.lblSize.AutoColor = false;
			this.lblSize.DefaultColor = System.Drawing.Color.White;
			this.lblSize.DefaultText = "";
			this.lblSize.DisplayFactor = 1;
			this.lblSize.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblSize.DownColor = System.Drawing.Color.Red;
			this.lblSize.Is64Th = false;
			this.lblSize.IsCurrency = false;
			this.lblSize.IsFraction = false;
			this.lblSize.IsPercent = false;
			this.lblSize.Location = new System.Drawing.Point(95, 0);
			this.lblSize.MaxDecimal = 7;
			this.lblSize.Name = "lblSize";
			this.lblSize.Size = new System.Drawing.Size(38, 19);
			this.lblSize.TabIndex = 2;
			this.lblSize.Text = "Size:";
			this.lblSize.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			this.lblSize.TickSize = 0.01;
			this.lblSize.UpColor = System.Drawing.Color.LimeGreen;
			this.lblSize.Value = "";
			// 
			// dspSize
			// 
			this.dspSize.AutoColor = false;
			this.dspSize.DefaultColor = System.Drawing.Color.White;
			this.dspSize.DefaultText = "0";
			this.dspSize.DisplayFactor = 1;
			this.dspSize.Dock = System.Windows.Forms.DockStyle.Left;
			this.dspSize.DownColor = System.Drawing.Color.Red;
			this.dspSize.Is64Th = false;
			this.dspSize.IsCurrency = false;
			this.dspSize.IsFraction = false;
			this.dspSize.IsPercent = false;
			this.dspSize.Location = new System.Drawing.Point(133, 0);
			this.dspSize.MaxDecimal = 0;
			this.dspSize.Name = "dspSize";
			this.dspSize.Size = new System.Drawing.Size(57, 19);
			this.dspSize.TabIndex = 3;
			this.dspSize.Text = "0";
			this.dspSize.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.dspSize.TickSize = 0.01;
			this.dspSize.UpColor = System.Drawing.Color.LimeGreen;
			this.dspSize.Value = "";
			// 
			// dndStock
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.BackColor = System.Drawing.Color.Black;
			this.ClientSize = new System.Drawing.Size(191, 19);
			this.ControlBox = false;
			this.Controls.Add(this.dspSize);
			this.Controls.Add(this.lblSize);
			this.Controls.Add(this.dspPrice);
			this.Controls.Add(this.lblPrice);
			this.DoubleBuffered = true;
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
			this.MaximizeBox = false;
			this.MaximumSize = new System.Drawing.Size(191, 19);
			this.MinimizeBox = false;
			this.MinimumSize = new System.Drawing.Size(191, 19);
			this.Name = "dndStock";
			this.Opacity = 0.5;
			this.ShowIcon = false;
			this.ShowInTaskbar = false;
			this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
			this.TopMost = true;
			this.TransparencyKey = System.Drawing.Color.Black;
			this.ResumeLayout(false);

		}

		#endregion

		private LabelEx.LabelVista lblPrice;
		private LabelEx.LabelVista lblSize;
		private LabelEx.LabelVista dspPrice;
		private LabelEx.LabelVista dspSize;

	}
}