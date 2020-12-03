namespace ROC
{
	partial class itemAutoSpreadSettings
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
			this.vistaPanel1 = new PanelEx.VistaPanel();
			this.panelTickSize = new System.Windows.Forms.Panel();
			this.dspTickSize = new LabelEx.LabelBase();
			this.lblTickSize = new System.Windows.Forms.Label();
			this.panelContractSize = new System.Windows.Forms.Panel();
			this.dspContractSize = new LabelEx.LabelBase();
			this.lblContractSize = new System.Windows.Forms.Label();
			this.panelMultiplier = new System.Windows.Forms.Panel();
			this.numMultiplier = new System.Windows.Forms.NumericUpDown();
			this.lblMultiplier = new System.Windows.Forms.Label();
			this.panelSymbol = new System.Windows.Forms.Panel();
			this.txtSymbol = new System.Windows.Forms.TextBox();
			this.lblSymbol = new System.Windows.Forms.Label();
			this.vistaPanel1.SuspendLayout();
			this.panelTickSize.SuspendLayout();
			this.panelContractSize.SuspendLayout();
			this.panelMultiplier.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.numMultiplier)).BeginInit();
			this.panelSymbol.SuspendLayout();
			this.SuspendLayout();
			// 
			// vistaPanel1
			// 
			this.vistaPanel1.BackColor = System.Drawing.Color.Black;
			this.vistaPanel1.Controls.Add(this.panelTickSize);
			this.vistaPanel1.Controls.Add(this.panelContractSize);
			this.vistaPanel1.Controls.Add(this.panelMultiplier);
			this.vistaPanel1.Controls.Add(this.panelSymbol);
			this.vistaPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.vistaPanel1.Location = new System.Drawing.Point(2, 2);
			this.vistaPanel1.Name = "vistaPanel1";
			this.vistaPanel1.Size = new System.Drawing.Size(297, 42);
			this.vistaPanel1.TabIndex = 0;
			// 
			// panelTickSize
			// 
			this.panelTickSize.Controls.Add(this.dspTickSize);
			this.panelTickSize.Controls.Add(this.lblTickSize);
			this.panelTickSize.Dock = System.Windows.Forms.DockStyle.Left;
			this.panelTickSize.Location = new System.Drawing.Point(232, 0);
			this.panelTickSize.MinimumSize = new System.Drawing.Size(53, 42);
			this.panelTickSize.Name = "panelTickSize";
			this.panelTickSize.Padding = new System.Windows.Forms.Padding(2);
			this.panelTickSize.Size = new System.Drawing.Size(63, 42);
			this.panelTickSize.TabIndex = 4;
			// 
			// dspTickSize
			// 
			this.dspTickSize.AutoColor = false;
			this.dspTickSize.DefaultColor = System.Drawing.Color.White;
			this.dspTickSize.DefaultText = "0.00";
			this.dspTickSize.DisplayFactor = 1;
			this.dspTickSize.Dock = System.Windows.Forms.DockStyle.Fill;
			this.dspTickSize.DownColor = System.Drawing.Color.Red;
			this.dspTickSize.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspTickSize.ForeColor = System.Drawing.Color.Lavender;
			this.dspTickSize.IsCurrency = false;
			this.dspTickSize.IsFraction = false;
			this.dspTickSize.IsPercent = false;
			this.dspTickSize.Location = new System.Drawing.Point(2, 17);
			this.dspTickSize.MaxDecimal = 7;
			this.dspTickSize.Name = "dspTickSize";
			this.dspTickSize.Size = new System.Drawing.Size(59, 23);
			this.dspTickSize.TabIndex = 2;
			this.dspTickSize.Text = "0.00";
			this.dspTickSize.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			this.dspTickSize.TickSize = 0.01;
			this.dspTickSize.UpColor = System.Drawing.Color.LimeGreen;
			this.dspTickSize.Value = "";
			// 
			// lblTickSize
			// 
			this.lblTickSize.AutoSize = true;
			this.lblTickSize.Dock = System.Windows.Forms.DockStyle.Top;
			this.lblTickSize.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblTickSize.Location = new System.Drawing.Point(2, 2);
			this.lblTickSize.Name = "lblTickSize";
			this.lblTickSize.Padding = new System.Windows.Forms.Padding(0, 0, 0, 2);
			this.lblTickSize.Size = new System.Drawing.Size(51, 15);
			this.lblTickSize.TabIndex = 0;
			this.lblTickSize.Text = "Tick Size";
			// 
			// panelContractSize
			// 
			this.panelContractSize.Controls.Add(this.dspContractSize);
			this.panelContractSize.Controls.Add(this.lblContractSize);
			this.panelContractSize.Dock = System.Windows.Forms.DockStyle.Left;
			this.panelContractSize.Location = new System.Drawing.Point(155, 0);
			this.panelContractSize.MinimumSize = new System.Drawing.Size(53, 42);
			this.panelContractSize.Name = "panelContractSize";
			this.panelContractSize.Padding = new System.Windows.Forms.Padding(2);
			this.panelContractSize.Size = new System.Drawing.Size(77, 42);
			this.panelContractSize.TabIndex = 3;
			// 
			// dspContractSize
			// 
			this.dspContractSize.AutoColor = false;
			this.dspContractSize.DefaultColor = System.Drawing.Color.White;
			this.dspContractSize.DefaultText = "0.00";
			this.dspContractSize.DisplayFactor = 1;
			this.dspContractSize.Dock = System.Windows.Forms.DockStyle.Fill;
			this.dspContractSize.DownColor = System.Drawing.Color.Red;
			this.dspContractSize.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspContractSize.ForeColor = System.Drawing.Color.Lavender;
			this.dspContractSize.IsCurrency = false;
			this.dspContractSize.IsFraction = false;
			this.dspContractSize.IsPercent = false;
			this.dspContractSize.Location = new System.Drawing.Point(2, 17);
			this.dspContractSize.MaxDecimal = 7;
			this.dspContractSize.Name = "dspContractSize";
			this.dspContractSize.Size = new System.Drawing.Size(73, 23);
			this.dspContractSize.TabIndex = 2;
			this.dspContractSize.Text = "0.00";
			this.dspContractSize.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			this.dspContractSize.TickSize = 0.01;
			this.dspContractSize.UpColor = System.Drawing.Color.LimeGreen;
			this.dspContractSize.Value = "";
			// 
			// lblContractSize
			// 
			this.lblContractSize.AutoSize = true;
			this.lblContractSize.Dock = System.Windows.Forms.DockStyle.Top;
			this.lblContractSize.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblContractSize.Location = new System.Drawing.Point(2, 2);
			this.lblContractSize.Name = "lblContractSize";
			this.lblContractSize.Padding = new System.Windows.Forms.Padding(0, 0, 0, 2);
			this.lblContractSize.Size = new System.Drawing.Size(70, 15);
			this.lblContractSize.TabIndex = 0;
			this.lblContractSize.Text = "Contract Size";
			// 
			// panelMultiplier
			// 
			this.panelMultiplier.Controls.Add(this.numMultiplier);
			this.panelMultiplier.Controls.Add(this.lblMultiplier);
			this.panelMultiplier.Dock = System.Windows.Forms.DockStyle.Left;
			this.panelMultiplier.Location = new System.Drawing.Point(92, 0);
			this.panelMultiplier.MinimumSize = new System.Drawing.Size(53, 42);
			this.panelMultiplier.Name = "panelMultiplier";
			this.panelMultiplier.Padding = new System.Windows.Forms.Padding(2);
			this.panelMultiplier.Size = new System.Drawing.Size(63, 42);
			this.panelMultiplier.TabIndex = 2;
			// 
			// numMultiplier
			// 
			this.numMultiplier.BackColor = System.Drawing.Color.White;
			this.numMultiplier.Dock = System.Windows.Forms.DockStyle.Top;
			this.numMultiplier.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.numMultiplier.ForeColor = System.Drawing.Color.Black;
			this.numMultiplier.Location = new System.Drawing.Point(2, 17);
			this.numMultiplier.Maximum = new decimal(new int[] {
            99,
            0,
            0,
            0});
			this.numMultiplier.Minimum = new decimal(new int[] {
            99,
            0,
            0,
            -2147483648});
			this.numMultiplier.Name = "numMultiplier";
			this.numMultiplier.Size = new System.Drawing.Size(59, 21);
			this.numMultiplier.TabIndex = 1;
			this.numMultiplier.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
			// 
			// lblMultiplier
			// 
			this.lblMultiplier.AutoSize = true;
			this.lblMultiplier.Dock = System.Windows.Forms.DockStyle.Top;
			this.lblMultiplier.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblMultiplier.Location = new System.Drawing.Point(2, 2);
			this.lblMultiplier.Name = "lblMultiplier";
			this.lblMultiplier.Padding = new System.Windows.Forms.Padding(0, 0, 0, 2);
			this.lblMultiplier.Size = new System.Drawing.Size(48, 15);
			this.lblMultiplier.TabIndex = 0;
			this.lblMultiplier.Text = "Multiplier";
			// 
			// panelSymbol
			// 
			this.panelSymbol.Controls.Add(this.txtSymbol);
			this.panelSymbol.Controls.Add(this.lblSymbol);
			this.panelSymbol.Dock = System.Windows.Forms.DockStyle.Left;
			this.panelSymbol.Location = new System.Drawing.Point(0, 0);
			this.panelSymbol.MinimumSize = new System.Drawing.Size(53, 42);
			this.panelSymbol.Name = "panelSymbol";
			this.panelSymbol.Padding = new System.Windows.Forms.Padding(2);
			this.panelSymbol.Size = new System.Drawing.Size(92, 42);
			this.panelSymbol.TabIndex = 1;
			// 
			// txtSymbol
			// 
			this.txtSymbol.BackColor = System.Drawing.Color.Black;
			this.txtSymbol.CharacterCasing = System.Windows.Forms.CharacterCasing.Upper;
			this.txtSymbol.Dock = System.Windows.Forms.DockStyle.Top;
			this.txtSymbol.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.txtSymbol.ForeColor = System.Drawing.Color.Gold;
			this.txtSymbol.Location = new System.Drawing.Point(2, 17);
			this.txtSymbol.Name = "txtSymbol";
			this.txtSymbol.Size = new System.Drawing.Size(88, 21);
			this.txtSymbol.TabIndex = 1;
			// 
			// lblSymbol
			// 
			this.lblSymbol.AutoSize = true;
			this.lblSymbol.Dock = System.Windows.Forms.DockStyle.Top;
			this.lblSymbol.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblSymbol.Location = new System.Drawing.Point(2, 2);
			this.lblSymbol.Name = "lblSymbol";
			this.lblSymbol.Padding = new System.Windows.Forms.Padding(0, 0, 0, 2);
			this.lblSymbol.Size = new System.Drawing.Size(41, 15);
			this.lblSymbol.TabIndex = 0;
			this.lblSymbol.Text = "Symbol";
			// 
			// itemAutoSpreadSettings
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.BackColor = System.Drawing.Color.Transparent;
			this.Controls.Add(this.vistaPanel1);
			this.Name = "itemAutoSpreadSettings";
			this.Padding = new System.Windows.Forms.Padding(2);
			this.Size = new System.Drawing.Size(301, 46);
			this.vistaPanel1.ResumeLayout(false);
			this.panelTickSize.ResumeLayout(false);
			this.panelTickSize.PerformLayout();
			this.panelContractSize.ResumeLayout(false);
			this.panelContractSize.PerformLayout();
			this.panelMultiplier.ResumeLayout(false);
			this.panelMultiplier.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.numMultiplier)).EndInit();
			this.panelSymbol.ResumeLayout(false);
			this.panelSymbol.PerformLayout();
			this.ResumeLayout(false);

		}

		#endregion

		private PanelEx.VistaPanel vistaPanel1;
		private System.Windows.Forms.Panel panelMultiplier;
		private System.Windows.Forms.Label lblMultiplier;
		private System.Windows.Forms.Panel panelSymbol;
		private System.Windows.Forms.Label lblSymbol;
		private System.Windows.Forms.Panel panelTickSize;
		private System.Windows.Forms.Label lblTickSize;
		private System.Windows.Forms.Panel panelContractSize;
		private System.Windows.Forms.Label lblContractSize;
		protected internal System.Windows.Forms.NumericUpDown numMultiplier;
		protected internal LabelEx.LabelBase dspTickSize;
		protected internal LabelEx.LabelBase dspContractSize;
		protected internal System.Windows.Forms.TextBox txtSymbol;
	}
}
