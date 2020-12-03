namespace ROC
{
	partial class FutureMatrixObjects
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
			this.splitSpreadObj = new System.Windows.Forms.SplitContainer();
			this.panelBidSize = new System.Windows.Forms.Panel();
			this.dspBidSize = new LabelEx.LabelBase();
			this.panelBidPrice = new System.Windows.Forms.Panel();
			this.dspBidPrice = new LabelEx.LabelBase();
			this.panelAskSize = new System.Windows.Forms.Panel();
			this.dspAskSize = new LabelEx.LabelBase();
			this.panelAskPrice = new System.Windows.Forms.Panel();
			this.dspAskPrice = new LabelEx.LabelBase();
			this.dspSymbol = new LabelEx.LabelBase();
			this.splitSpreadObj.Panel1.SuspendLayout();
			this.splitSpreadObj.Panel2.SuspendLayout();
			this.splitSpreadObj.SuspendLayout();
			this.panelBidSize.SuspendLayout();
			this.panelBidPrice.SuspendLayout();
			this.panelAskSize.SuspendLayout();
			this.panelAskPrice.SuspendLayout();
			this.SuspendLayout();
			// 
			// splitSpreadObj
			// 
			this.splitSpreadObj.BackColor = System.Drawing.Color.Black;
			this.splitSpreadObj.Dock = System.Windows.Forms.DockStyle.Top;
			this.splitSpreadObj.Location = new System.Drawing.Point(1, 16);
			this.splitSpreadObj.Name = "splitSpreadObj";
			// 
			// splitSpreadObj.Panel1
			// 
			this.splitSpreadObj.Panel1.Controls.Add(this.panelBidSize);
			this.splitSpreadObj.Panel1.Controls.Add(this.panelBidPrice);
			// 
			// splitSpreadObj.Panel2
			// 
			this.splitSpreadObj.Panel2.Controls.Add(this.panelAskSize);
			this.splitSpreadObj.Panel2.Controls.Add(this.panelAskPrice);
			this.splitSpreadObj.Size = new System.Drawing.Size(122, 34);
			this.splitSpreadObj.SplitterDistance = 60;
			this.splitSpreadObj.SplitterWidth = 2;
			this.splitSpreadObj.TabIndex = 7;
			// 
			// panelBidSize
			// 
			this.panelBidSize.BackColor = System.Drawing.SystemColors.ControlDarkDark;
			this.panelBidSize.Controls.Add(this.dspBidSize);
			this.panelBidSize.Dock = System.Windows.Forms.DockStyle.Top;
			this.panelBidSize.Location = new System.Drawing.Point(0, 17);
			this.panelBidSize.MinimumSize = new System.Drawing.Size(0, 17);
			this.panelBidSize.Name = "panelBidSize";
			this.panelBidSize.Size = new System.Drawing.Size(60, 17);
			this.panelBidSize.TabIndex = 0;
			this.panelBidSize.MouseLeave += new System.EventHandler(this.dspBidPrice_MouseLeave);
			this.panelBidSize.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.dspSymbol_MouseDoubleClick);
			this.panelBidSize.MouseEnter += new System.EventHandler(this.dspBidPrice_MouseEnter);
			// 
			// dspBidSize
			// 
			this.dspBidSize.AutoColor = false;
			this.dspBidSize.AutoSize = true;
			this.dspBidSize.DefaultColor = System.Drawing.Color.White;
			this.dspBidSize.DefaultText = "0";
			this.dspBidSize.DisplayFactor = 1;
			this.dspBidSize.Dock = System.Windows.Forms.DockStyle.Right;
			this.dspBidSize.DownColor = System.Drawing.Color.Red;
			this.dspBidSize.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspBidSize.ForeColor = System.Drawing.Color.Lime;
			this.dspBidSize.Is64Th = false;
			this.dspBidSize.IsCurrency = false;
			this.dspBidSize.IsFraction = false;
			this.dspBidSize.IsPercent = false;
			this.dspBidSize.Location = new System.Drawing.Point(47, 0);
			this.dspBidSize.MaxDecimal = 7;
			this.dspBidSize.Name = "dspBidSize";
			this.dspBidSize.Padding = new System.Windows.Forms.Padding(0, 1, 0, 0);
			this.dspBidSize.Size = new System.Drawing.Size(13, 14);
			this.dspBidSize.TabIndex = 2;
			this.dspBidSize.Text = "0";
			this.dspBidSize.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.dspBidSize.TickSize = 0.01;
			this.dspBidSize.UpColor = System.Drawing.Color.LimeGreen;
			this.dspBidSize.Value = "";
			this.dspBidSize.MouseLeave += new System.EventHandler(this.dspBidPrice_MouseLeave);
			this.dspBidSize.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.dspSymbol_MouseDoubleClick);
			this.dspBidSize.MouseEnter += new System.EventHandler(this.dspBidPrice_MouseEnter);
			// 
			// panelBidPrice
			// 
			this.panelBidPrice.BackColor = System.Drawing.Color.DarkSlateGray;
			this.panelBidPrice.Controls.Add(this.dspBidPrice);
			this.panelBidPrice.Dock = System.Windows.Forms.DockStyle.Top;
			this.panelBidPrice.Location = new System.Drawing.Point(0, 0);
			this.panelBidPrice.MinimumSize = new System.Drawing.Size(0, 17);
			this.panelBidPrice.Name = "panelBidPrice";
			this.panelBidPrice.Size = new System.Drawing.Size(60, 17);
			this.panelBidPrice.TabIndex = 3;
			this.panelBidPrice.MouseLeave += new System.EventHandler(this.dspBidPrice_MouseLeave);
			this.panelBidPrice.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.dspSymbol_MouseDoubleClick);
			this.panelBidPrice.MouseEnter += new System.EventHandler(this.dspBidPrice_MouseEnter);
			// 
			// dspBidPrice
			// 
			this.dspBidPrice.AutoColor = false;
			this.dspBidPrice.AutoSize = true;
			this.dspBidPrice.DefaultColor = System.Drawing.Color.White;
			this.dspBidPrice.DefaultText = "0.00";
			this.dspBidPrice.DisplayFactor = 1;
			this.dspBidPrice.Dock = System.Windows.Forms.DockStyle.Right;
			this.dspBidPrice.DownColor = System.Drawing.Color.Red;
			this.dspBidPrice.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspBidPrice.ForeColor = System.Drawing.Color.White;
			this.dspBidPrice.Is64Th = false;
			this.dspBidPrice.IsCurrency = false;
			this.dspBidPrice.IsFraction = true;
			this.dspBidPrice.IsPercent = false;
			this.dspBidPrice.Location = new System.Drawing.Point(29, 0);
			this.dspBidPrice.MaxDecimal = 2;
			this.dspBidPrice.Name = "dspBidPrice";
			this.dspBidPrice.Padding = new System.Windows.Forms.Padding(0, 1, 0, 0);
			this.dspBidPrice.Size = new System.Drawing.Size(31, 14);
			this.dspBidPrice.TabIndex = 2;
			this.dspBidPrice.Text = "0.00";
			this.dspBidPrice.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			this.dspBidPrice.TickSize = 0.01;
			this.dspBidPrice.UpColor = System.Drawing.Color.LimeGreen;
			this.dspBidPrice.Value = "";
			this.dspBidPrice.MouseLeave += new System.EventHandler(this.dspBidPrice_MouseLeave);
			this.dspBidPrice.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.dspSymbol_MouseDoubleClick);
			this.dspBidPrice.MouseEnter += new System.EventHandler(this.dspBidPrice_MouseEnter);
			// 
			// panelAskSize
			// 
			this.panelAskSize.BackColor = System.Drawing.SystemColors.ControlDarkDark;
			this.panelAskSize.Controls.Add(this.dspAskSize);
			this.panelAskSize.Dock = System.Windows.Forms.DockStyle.Top;
			this.panelAskSize.Location = new System.Drawing.Point(0, 17);
			this.panelAskSize.MinimumSize = new System.Drawing.Size(0, 17);
			this.panelAskSize.Name = "panelAskSize";
			this.panelAskSize.Size = new System.Drawing.Size(60, 17);
			this.panelAskSize.TabIndex = 2;
			this.panelAskSize.MouseLeave += new System.EventHandler(this.dspBidPrice_MouseLeave);
			this.panelAskSize.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.dspSymbol_MouseDoubleClick);
			this.panelAskSize.MouseEnter += new System.EventHandler(this.dspBidPrice_MouseEnter);
			// 
			// dspAskSize
			// 
			this.dspAskSize.AutoColor = false;
			this.dspAskSize.AutoSize = true;
			this.dspAskSize.DefaultColor = System.Drawing.Color.White;
			this.dspAskSize.DefaultText = "0";
			this.dspAskSize.DisplayFactor = 1;
			this.dspAskSize.Dock = System.Windows.Forms.DockStyle.Right;
			this.dspAskSize.DownColor = System.Drawing.Color.Red;
			this.dspAskSize.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspAskSize.ForeColor = System.Drawing.Color.Red;
			this.dspAskSize.Is64Th = false;
			this.dspAskSize.IsCurrency = false;
			this.dspAskSize.IsFraction = false;
			this.dspAskSize.IsPercent = false;
			this.dspAskSize.Location = new System.Drawing.Point(47, 0);
			this.dspAskSize.MaxDecimal = 7;
			this.dspAskSize.Name = "dspAskSize";
			this.dspAskSize.Padding = new System.Windows.Forms.Padding(0, 1, 0, 0);
			this.dspAskSize.Size = new System.Drawing.Size(13, 14);
			this.dspAskSize.TabIndex = 2;
			this.dspAskSize.Text = "0";
			this.dspAskSize.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.dspAskSize.TickSize = 0.01;
			this.dspAskSize.UpColor = System.Drawing.Color.LimeGreen;
			this.dspAskSize.Value = "";
			this.dspAskSize.MouseLeave += new System.EventHandler(this.dspBidPrice_MouseLeave);
			this.dspAskSize.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.dspSymbol_MouseDoubleClick);
			this.dspAskSize.MouseEnter += new System.EventHandler(this.dspBidPrice_MouseEnter);
			// 
			// panelAskPrice
			// 
			this.panelAskPrice.BackColor = System.Drawing.Color.Brown;
			this.panelAskPrice.Controls.Add(this.dspAskPrice);
			this.panelAskPrice.Dock = System.Windows.Forms.DockStyle.Top;
			this.panelAskPrice.Location = new System.Drawing.Point(0, 0);
			this.panelAskPrice.MinimumSize = new System.Drawing.Size(0, 17);
			this.panelAskPrice.Name = "panelAskPrice";
			this.panelAskPrice.Size = new System.Drawing.Size(60, 17);
			this.panelAskPrice.TabIndex = 4;
			this.panelAskPrice.MouseLeave += new System.EventHandler(this.dspBidPrice_MouseLeave);
			this.panelAskPrice.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.dspSymbol_MouseDoubleClick);
			this.panelAskPrice.MouseEnter += new System.EventHandler(this.dspBidPrice_MouseEnter);
			// 
			// dspAskPrice
			// 
			this.dspAskPrice.AutoColor = false;
			this.dspAskPrice.AutoSize = true;
			this.dspAskPrice.DefaultColor = System.Drawing.Color.White;
			this.dspAskPrice.DefaultText = "0.00";
			this.dspAskPrice.DisplayFactor = 1;
			this.dspAskPrice.Dock = System.Windows.Forms.DockStyle.Right;
			this.dspAskPrice.DownColor = System.Drawing.Color.Red;
			this.dspAskPrice.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspAskPrice.ForeColor = System.Drawing.Color.White;
			this.dspAskPrice.Is64Th = false;
			this.dspAskPrice.IsCurrency = false;
			this.dspAskPrice.IsFraction = true;
			this.dspAskPrice.IsPercent = false;
			this.dspAskPrice.Location = new System.Drawing.Point(29, 0);
			this.dspAskPrice.MaxDecimal = 2;
			this.dspAskPrice.Name = "dspAskPrice";
			this.dspAskPrice.Padding = new System.Windows.Forms.Padding(0, 1, 0, 0);
			this.dspAskPrice.Size = new System.Drawing.Size(31, 14);
			this.dspAskPrice.TabIndex = 2;
			this.dspAskPrice.Text = "0.00";
			this.dspAskPrice.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.dspAskPrice.TickSize = 0.01;
			this.dspAskPrice.UpColor = System.Drawing.Color.LimeGreen;
			this.dspAskPrice.Value = "";
			this.dspAskPrice.MouseLeave += new System.EventHandler(this.dspBidPrice_MouseLeave);
			this.dspAskPrice.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.dspSymbol_MouseDoubleClick);
			this.dspAskPrice.MouseEnter += new System.EventHandler(this.dspBidPrice_MouseEnter);
			// 
			// dspSymbol
			// 
			this.dspSymbol.AutoColor = false;
			this.dspSymbol.BackColor = System.Drawing.Color.Gainsboro;
			this.dspSymbol.DefaultColor = System.Drawing.Color.White;
			this.dspSymbol.DefaultText = "";
			this.dspSymbol.DisplayFactor = 1;
			this.dspSymbol.Dock = System.Windows.Forms.DockStyle.Top;
			this.dspSymbol.DownColor = System.Drawing.Color.Red;
			this.dspSymbol.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspSymbol.ForeColor = System.Drawing.Color.Black;
			this.dspSymbol.Is64Th = false;
			this.dspSymbol.IsCurrency = false;
			this.dspSymbol.IsFraction = false;
			this.dspSymbol.IsPercent = false;
			this.dspSymbol.Location = new System.Drawing.Point(1, 1);
			this.dspSymbol.MaxDecimal = 7;
			this.dspSymbol.Name = "dspSymbol";
			this.dspSymbol.Size = new System.Drawing.Size(122, 15);
			this.dspSymbol.TabIndex = 8;
			this.dspSymbol.Text = "Symbol";
			this.dspSymbol.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.dspSymbol.TickSize = 0.01;
			this.dspSymbol.UpColor = System.Drawing.Color.LimeGreen;
			this.dspSymbol.Value = "";
			this.dspSymbol.MouseLeave += new System.EventHandler(this.dspBidPrice_MouseLeave);
			this.dspSymbol.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.dspSymbol_MouseDoubleClick);
			this.dspSymbol.MouseEnter += new System.EventHandler(this.dspBidPrice_MouseEnter);
			// 
			// FutureMatrixObjects
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.BackColor = System.Drawing.Color.Black;
			this.Controls.Add(this.splitSpreadObj);
			this.Controls.Add(this.dspSymbol);
			this.Name = "FutureMatrixObjects";
			this.Padding = new System.Windows.Forms.Padding(1);
			this.Size = new System.Drawing.Size(124, 51);
			this.splitSpreadObj.Panel1.ResumeLayout(false);
			this.splitSpreadObj.Panel2.ResumeLayout(false);
			this.splitSpreadObj.ResumeLayout(false);
			this.panelBidSize.ResumeLayout(false);
			this.panelBidSize.PerformLayout();
			this.panelBidPrice.ResumeLayout(false);
			this.panelBidPrice.PerformLayout();
			this.panelAskSize.ResumeLayout(false);
			this.panelAskSize.PerformLayout();
			this.panelAskPrice.ResumeLayout(false);
			this.panelAskPrice.PerformLayout();
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.SplitContainer splitSpreadObj;
		private System.Windows.Forms.Panel panelBidSize;
		private LabelEx.LabelBase dspBidSize;
		private System.Windows.Forms.Panel panelBidPrice;
		private LabelEx.LabelBase dspBidPrice;
		private System.Windows.Forms.Panel panelAskSize;
		private LabelEx.LabelBase dspAskSize;
		private System.Windows.Forms.Panel panelAskPrice;
		private LabelEx.LabelBase dspAskPrice;
		private LabelEx.LabelBase dspSymbol;



	}
}
