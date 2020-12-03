namespace ROC
{
	partial class itemBaseSymbolTradePair
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
			this.components = new System.ComponentModel.Container();
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(itemBaseSymbolTradePair));
			this.cboSymbol = new System.Windows.Forms.ComboBox();
			this.dspPrice = new LabelEx.LabelBase();
			this.picTradeTick = new System.Windows.Forms.PictureBox();
			this.imageListTradeTick = new System.Windows.Forms.ImageList(this.components);
			((System.ComponentModel.ISupportInitialize)(this.picTradeTick)).BeginInit();
			this.SuspendLayout();
			// 
			// cboSymbol
			// 
			this.cboSymbol.BackColor = System.Drawing.Color.Black;
			this.cboSymbol.Dock = System.Windows.Forms.DockStyle.Top;
			this.cboSymbol.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.cboSymbol.Font = new System.Drawing.Font("Tahoma", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cboSymbol.ForeColor = System.Drawing.Color.Gold;
			this.cboSymbol.FormattingEnabled = true;
			this.cboSymbol.Location = new System.Drawing.Point(0, 0);
			this.cboSymbol.Name = "cboSymbol";
			this.cboSymbol.Size = new System.Drawing.Size(91, 22);
			this.cboSymbol.TabIndex = 3;
			this.cboSymbol.SelectedIndexChanged += new System.EventHandler(this.cboSymbol_SelectedIndexChanged);
			this.cboSymbol.KeyUp += new System.Windows.Forms.KeyEventHandler(this.cboSymbol_KeyUp);
			this.cboSymbol.KeyDown += new System.Windows.Forms.KeyEventHandler(this.cboSymbol_KeyDown);
			// 
			// dspPrice
			// 
			this.dspPrice.AutoColor = false;
			this.dspPrice.AutoSize = true;
			this.dspPrice.DefaultColor = System.Drawing.Color.White;
			this.dspPrice.DefaultText = "0.00";
			this.dspPrice.DisplayFactor = 1;
			this.dspPrice.Dock = System.Windows.Forms.DockStyle.Right;
			this.dspPrice.DownColor = System.Drawing.Color.Red;
			this.dspPrice.Font = new System.Drawing.Font("Tahoma", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspPrice.IsCurrency = false;
			this.dspPrice.IsFraction = false;
			this.dspPrice.Location = new System.Drawing.Point(32, 22);
			this.dspPrice.MaxDecimal = 7;
			this.dspPrice.Name = "dspPrice";
			this.dspPrice.Padding = new System.Windows.Forms.Padding(0, 3, 0, 0);
			this.dspPrice.Size = new System.Drawing.Size(36, 19);
			this.dspPrice.TabIndex = 13;
			this.dspPrice.Text = "0.00";
			this.dspPrice.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.dspPrice.TickSize = 0.01;
			this.dspPrice.UpColor = System.Drawing.Color.LimeGreen;
			this.dspPrice.Value = "";
			// 
			// picTradeTick
			// 
			this.picTradeTick.Dock = System.Windows.Forms.DockStyle.Right;
			this.picTradeTick.Location = new System.Drawing.Point(68, 22);
			this.picTradeTick.MaximumSize = new System.Drawing.Size(23, 23);
			this.picTradeTick.MinimumSize = new System.Drawing.Size(23, 23);
			this.picTradeTick.Name = "picTradeTick";
			this.picTradeTick.Padding = new System.Windows.Forms.Padding(1);
			this.picTradeTick.Size = new System.Drawing.Size(23, 23);
			this.picTradeTick.TabIndex = 14;
			this.picTradeTick.TabStop = false;
			// 
			// imageListTradeTick
			// 
			this.imageListTradeTick.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageListTradeTick.ImageStream")));
			this.imageListTradeTick.TransparentColor = System.Drawing.Color.Transparent;
			this.imageListTradeTick.Images.SetKeyName(0, "UpGreen.png");
			this.imageListTradeTick.Images.SetKeyName(1, "Blank.png");
			this.imageListTradeTick.Images.SetKeyName(2, "DownRed.png");
			// 
			// itemBaseSymbolTradePair
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.BackColor = System.Drawing.Color.Black;
			this.Controls.Add(this.dspPrice);
			this.Controls.Add(this.picTradeTick);
			this.Controls.Add(this.cboSymbol);
			this.ForeColor = System.Drawing.Color.Gainsboro;
			this.Name = "itemBaseSymbolTradePair";
			this.Size = new System.Drawing.Size(91, 45);
			((System.ComponentModel.ISupportInitialize)(this.picTradeTick)).EndInit();
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.ComboBox cboSymbol;
		private LabelEx.LabelBase dspPrice;
		private System.Windows.Forms.PictureBox picTradeTick;
		private System.Windows.Forms.ImageList imageListTradeTick;
	}
}
