namespace ROC
{
	partial class itemBasePriceSizePair
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
			this.lblName = new System.Windows.Forms.Label();
			this.panel1 = new System.Windows.Forms.Panel();
			this.dspSize = new LabelEx.LabelBase();
			this.dspPrice = new LabelEx.LabelBase();
			this.panel1.SuspendLayout();
			this.SuspendLayout();
			// 
			// lblName
			// 
			this.lblName.AutoSize = true;
			this.lblName.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblName.ForeColor = System.Drawing.Color.Khaki;
			this.lblName.Location = new System.Drawing.Point(0, 0);
			this.lblName.MaximumSize = new System.Drawing.Size(24, 14);
			this.lblName.MinimumSize = new System.Drawing.Size(24, 14);
			this.lblName.Name = "lblName";
			this.lblName.Padding = new System.Windows.Forms.Padding(0, 1, 0, 0);
			this.lblName.Size = new System.Drawing.Size(24, 14);
			this.lblName.TabIndex = 11;
			this.lblName.Text = "xxx";
			this.lblName.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// panel1
			// 
			this.panel1.Controls.Add(this.dspSize);
			this.panel1.Controls.Add(this.dspPrice);
			this.panel1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.panel1.Location = new System.Drawing.Point(24, 0);
			this.panel1.Name = "panel1";
			this.panel1.Size = new System.Drawing.Size(31, 26);
			this.panel1.TabIndex = 12;
			// 
			// dspSize
			// 
			this.dspSize.AutoColor = false;
			this.dspSize.AutoSize = true;
			this.dspSize.DefaultColor = System.Drawing.Color.White;
			this.dspSize.DefaultText = "0";
			this.dspSize.DisplayFactor = 1;
			this.dspSize.Dock = System.Windows.Forms.DockStyle.Top;
			this.dspSize.DownColor = System.Drawing.Color.Red;
			this.dspSize.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspSize.IsCurrency = false;
			this.dspSize.IsFraction = false;
			this.dspSize.Location = new System.Drawing.Point(0, 13);
			this.dspSize.MaxDecimal = 7;
			this.dspSize.Name = "dspSize";
			this.dspSize.Size = new System.Drawing.Size(13, 13);
			this.dspSize.TabIndex = 13;
			this.dspSize.Text = "0";
			this.dspSize.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.dspSize.TickSize = 0.01;
			this.dspSize.UpColor = System.Drawing.Color.LimeGreen;
			this.dspSize.Value = "";
			// 
			// dspPrice
			// 
			this.dspPrice.AutoColor = false;
			this.dspPrice.AutoSize = true;
			this.dspPrice.DefaultColor = System.Drawing.Color.White;
			this.dspPrice.DefaultText = "0.00";
			this.dspPrice.DisplayFactor = 1;
			this.dspPrice.Dock = System.Windows.Forms.DockStyle.Top;
			this.dspPrice.DownColor = System.Drawing.Color.Red;
			this.dspPrice.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspPrice.IsCurrency = false;
			this.dspPrice.IsFraction = false;
			this.dspPrice.Location = new System.Drawing.Point(0, 0);
			this.dspPrice.MaxDecimal = 7;
			this.dspPrice.Name = "dspPrice";
			this.dspPrice.Size = new System.Drawing.Size(31, 13);
			this.dspPrice.TabIndex = 12;
			this.dspPrice.Text = "0.00";
			this.dspPrice.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.dspPrice.TickSize = 0.01;
			this.dspPrice.UpColor = System.Drawing.Color.LimeGreen;
			this.dspPrice.Value = "";
			// 
			// itemBasePriceSizePair
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.BackColor = System.Drawing.Color.Black;
			this.Controls.Add(this.panel1);
			this.Controls.Add(this.lblName);
			this.ForeColor = System.Drawing.Color.Gainsboro;
			this.Name = "itemBasePriceSizePair";
			this.Size = new System.Drawing.Size(55, 26);
			this.panel1.ResumeLayout(false);
			this.panel1.PerformLayout();
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.Label lblName;
		private System.Windows.Forms.Panel panel1;
		private LabelEx.LabelBase dspSize;
		private LabelEx.LabelBase dspPrice;

	}
}
