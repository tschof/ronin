namespace ROC
{
	partial class frmAbout
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
			this.picLogo = new System.Windows.Forms.PictureBox();
			this.panelAbout = new PanelEx.VistaPanel();
			this.lblMDSBackup = new LabelEx.LabelBase();
			this.lblMDS = new LabelEx.LabelBase();
			this.dspMDS = new LabelEx.LabelBase();
			this.lblRDS = new LabelEx.LabelBase();
			this.dspRDS = new LabelEx.LabelBase();
			this.lblROM = new LabelEx.LabelBase();
			this.dspROM = new LabelEx.LabelBase();
			this.CaptionBar = new FormEx.VistaWindowCaptionBar();
			this.dspROCVersion = new System.Windows.Forms.Label();
			this.lblROCVersion = new System.Windows.Forms.Label();
			((System.ComponentModel.ISupportInitialize)(this.picLogo)).BeginInit();
			this.panelAbout.SuspendLayout();
			this.SuspendLayout();
			// 
			// picLogo
			// 
			this.picLogo.BackColor = System.Drawing.SystemColors.Control;
			this.picLogo.Dock = System.Windows.Forms.DockStyle.Top;
			this.picLogo.Image = global::ROC.Properties.Resources.LOGO;
			this.picLogo.Location = new System.Drawing.Point(0, 26);
			this.picLogo.Name = "picLogo";
			this.picLogo.Size = new System.Drawing.Size(300, 124);
			this.picLogo.SizeMode = System.Windows.Forms.PictureBoxSizeMode.CenterImage;
			this.picLogo.TabIndex = 4;
			this.picLogo.TabStop = false;
			// 
			// panelAbout
			// 
			this.panelAbout.BackColor = System.Drawing.Color.Black;
			this.panelAbout.Controls.Add(this.lblMDSBackup);
			this.panelAbout.Controls.Add(this.lblMDS);
			this.panelAbout.Controls.Add(this.dspMDS);
			this.panelAbout.Controls.Add(this.lblRDS);
			this.panelAbout.Controls.Add(this.dspRDS);
			this.panelAbout.Controls.Add(this.lblROM);
			this.panelAbout.Controls.Add(this.dspROM);
			this.panelAbout.Dock = System.Windows.Forms.DockStyle.Fill;
			this.panelAbout.Location = new System.Drawing.Point(0, 150);
			this.panelAbout.Name = "panelAbout";
			this.panelAbout.Padding = new System.Windows.Forms.Padding(2);
			this.panelAbout.Size = new System.Drawing.Size(300, 110);
			this.panelAbout.TabIndex = 24;
			// 
			// lblMDSBackup
			// 
			this.lblMDSBackup.AutoColor = false;
			this.lblMDSBackup.AutoEllipsis = true;
			this.lblMDSBackup.AutoSize = true;
			this.lblMDSBackup.DefaultColor = System.Drawing.Color.White;
			this.lblMDSBackup.DefaultText = "xxx.xxx.xxx.xxx";
			this.lblMDSBackup.DisplayFactor = 1;
			this.lblMDSBackup.Dock = System.Windows.Forms.DockStyle.Top;
			this.lblMDSBackup.DownColor = System.Drawing.Color.Red;
			this.lblMDSBackup.ForeColor = System.Drawing.Color.Olive;
			this.lblMDSBackup.Is64Th = false;
			this.lblMDSBackup.IsCurrency = false;
			this.lblMDSBackup.IsFraction = false;
			this.lblMDSBackup.IsPercent = false;
			this.lblMDSBackup.Location = new System.Drawing.Point(2, 92);
			this.lblMDSBackup.MaxDecimal = 7;
			this.lblMDSBackup.MaximumSize = new System.Drawing.Size(300, 15);
			this.lblMDSBackup.Name = "lblMDSBackup";
			this.lblMDSBackup.Padding = new System.Windows.Forms.Padding(5, 1, 1, 1);
			this.lblMDSBackup.Size = new System.Drawing.Size(82, 15);
			this.lblMDSBackup.TabIndex = 30;
			this.lblMDSBackup.Text = "xxx.xxx.xxx.xxx";
			this.lblMDSBackup.TickSize = 0.01;
			this.lblMDSBackup.UpColor = System.Drawing.Color.LimeGreen;
			this.lblMDSBackup.Value = "";
			// 
			// lblMDS
			// 
			this.lblMDS.AutoColor = false;
			this.lblMDS.AutoEllipsis = true;
			this.lblMDS.AutoSize = true;
			this.lblMDS.DefaultColor = System.Drawing.Color.White;
			this.lblMDS.DefaultText = "xxx.xxx.xxx.xxx";
			this.lblMDS.DisplayFactor = 1;
			this.lblMDS.Dock = System.Windows.Forms.DockStyle.Top;
			this.lblMDS.DownColor = System.Drawing.Color.Red;
			this.lblMDS.ForeColor = System.Drawing.SystemColors.GradientActiveCaption;
			this.lblMDS.Is64Th = false;
			this.lblMDS.IsCurrency = false;
			this.lblMDS.IsFraction = false;
			this.lblMDS.IsPercent = false;
			this.lblMDS.Location = new System.Drawing.Point(2, 77);
			this.lblMDS.MaxDecimal = 7;
			this.lblMDS.MaximumSize = new System.Drawing.Size(300, 15);
			this.lblMDS.Name = "lblMDS";
			this.lblMDS.Padding = new System.Windows.Forms.Padding(5, 1, 1, 1);
			this.lblMDS.Size = new System.Drawing.Size(82, 15);
			this.lblMDS.TabIndex = 29;
			this.lblMDS.Text = "xxx.xxx.xxx.xxx";
			this.lblMDS.TickSize = 0.01;
			this.lblMDS.UpColor = System.Drawing.Color.LimeGreen;
			this.lblMDS.Value = "";
			// 
			// dspMDS
			// 
			this.dspMDS.AutoColor = false;
			this.dspMDS.AutoSize = true;
			this.dspMDS.DefaultColor = System.Drawing.Color.White;
			this.dspMDS.DefaultText = "MDS :";
			this.dspMDS.DisplayFactor = 1;
			this.dspMDS.Dock = System.Windows.Forms.DockStyle.Top;
			this.dspMDS.DownColor = System.Drawing.Color.Red;
			this.dspMDS.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspMDS.ForeColor = System.Drawing.Color.Silver;
			this.dspMDS.Is64Th = false;
			this.dspMDS.IsCurrency = false;
			this.dspMDS.IsFraction = false;
			this.dspMDS.IsPercent = false;
			this.dspMDS.Location = new System.Drawing.Point(2, 62);
			this.dspMDS.MaxDecimal = 7;
			this.dspMDS.Name = "dspMDS";
			this.dspMDS.Padding = new System.Windows.Forms.Padding(1);
			this.dspMDS.Size = new System.Drawing.Size(44, 15);
			this.dspMDS.TabIndex = 28;
			this.dspMDS.Text = "MDS :";
			this.dspMDS.TickSize = 0.01;
			this.dspMDS.UpColor = System.Drawing.Color.LimeGreen;
			this.dspMDS.Value = "";
			// 
			// lblRDS
			// 
			this.lblRDS.AutoColor = false;
			this.lblRDS.AutoEllipsis = true;
			this.lblRDS.AutoSize = true;
			this.lblRDS.DefaultColor = System.Drawing.Color.White;
			this.lblRDS.DefaultText = "xxx.xxx.xxx.xxx";
			this.lblRDS.DisplayFactor = 1;
			this.lblRDS.Dock = System.Windows.Forms.DockStyle.Top;
			this.lblRDS.DownColor = System.Drawing.Color.Red;
			this.lblRDS.ForeColor = System.Drawing.SystemColors.GradientActiveCaption;
			this.lblRDS.Is64Th = false;
			this.lblRDS.IsCurrency = false;
			this.lblRDS.IsFraction = false;
			this.lblRDS.IsPercent = false;
			this.lblRDS.Location = new System.Drawing.Point(2, 47);
			this.lblRDS.MaxDecimal = 7;
			this.lblRDS.MaximumSize = new System.Drawing.Size(300, 15);
			this.lblRDS.Name = "lblRDS";
			this.lblRDS.Padding = new System.Windows.Forms.Padding(5, 1, 1, 1);
			this.lblRDS.Size = new System.Drawing.Size(82, 15);
			this.lblRDS.TabIndex = 27;
			this.lblRDS.Text = "xxx.xxx.xxx.xxx";
			this.lblRDS.TickSize = 0.01;
			this.lblRDS.UpColor = System.Drawing.Color.LimeGreen;
			this.lblRDS.Value = "";
			// 
			// dspRDS
			// 
			this.dspRDS.AutoColor = false;
			this.dspRDS.AutoSize = true;
			this.dspRDS.DefaultColor = System.Drawing.Color.White;
			this.dspRDS.DefaultText = "RDS :";
			this.dspRDS.DisplayFactor = 1;
			this.dspRDS.Dock = System.Windows.Forms.DockStyle.Top;
			this.dspRDS.DownColor = System.Drawing.Color.Red;
			this.dspRDS.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspRDS.ForeColor = System.Drawing.Color.Silver;
			this.dspRDS.Is64Th = false;
			this.dspRDS.IsCurrency = false;
			this.dspRDS.IsFraction = false;
			this.dspRDS.IsPercent = false;
			this.dspRDS.Location = new System.Drawing.Point(2, 32);
			this.dspRDS.MaxDecimal = 7;
			this.dspRDS.Name = "dspRDS";
			this.dspRDS.Padding = new System.Windows.Forms.Padding(1);
			this.dspRDS.Size = new System.Drawing.Size(43, 15);
			this.dspRDS.TabIndex = 26;
			this.dspRDS.Text = "RDS :";
			this.dspRDS.TickSize = 0.01;
			this.dspRDS.UpColor = System.Drawing.Color.LimeGreen;
			this.dspRDS.Value = "";
			// 
			// lblROM
			// 
			this.lblROM.AutoColor = false;
			this.lblROM.AutoEllipsis = true;
			this.lblROM.AutoSize = true;
			this.lblROM.BackColor = System.Drawing.Color.Transparent;
			this.lblROM.DefaultColor = System.Drawing.Color.White;
			this.lblROM.DefaultText = "xxx.xxx.xxx.xxx";
			this.lblROM.DisplayFactor = 1;
			this.lblROM.Dock = System.Windows.Forms.DockStyle.Top;
			this.lblROM.DownColor = System.Drawing.Color.Red;
			this.lblROM.ForeColor = System.Drawing.SystemColors.GradientActiveCaption;
			this.lblROM.Is64Th = false;
			this.lblROM.IsCurrency = false;
			this.lblROM.IsFraction = false;
			this.lblROM.IsPercent = false;
			this.lblROM.Location = new System.Drawing.Point(2, 17);
			this.lblROM.MaxDecimal = 7;
			this.lblROM.MaximumSize = new System.Drawing.Size(300, 15);
			this.lblROM.Name = "lblROM";
			this.lblROM.Padding = new System.Windows.Forms.Padding(5, 1, 1, 1);
			this.lblROM.Size = new System.Drawing.Size(82, 15);
			this.lblROM.TabIndex = 25;
			this.lblROM.Text = "xxx.xxx.xxx.xxx";
			this.lblROM.TickSize = 0.01;
			this.lblROM.UpColor = System.Drawing.Color.LimeGreen;
			this.lblROM.Value = "";
			// 
			// dspROM
			// 
			this.dspROM.AutoColor = false;
			this.dspROM.AutoSize = true;
			this.dspROM.BackColor = System.Drawing.Color.Transparent;
			this.dspROM.DefaultColor = System.Drawing.Color.White;
			this.dspROM.DefaultText = "ROM :";
			this.dspROM.DisplayFactor = 1;
			this.dspROM.Dock = System.Windows.Forms.DockStyle.Top;
			this.dspROM.DownColor = System.Drawing.Color.Red;
			this.dspROM.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspROM.ForeColor = System.Drawing.Color.Silver;
			this.dspROM.Is64Th = false;
			this.dspROM.IsCurrency = false;
			this.dspROM.IsFraction = false;
			this.dspROM.IsPercent = false;
			this.dspROM.Location = new System.Drawing.Point(2, 2);
			this.dspROM.MaxDecimal = 7;
			this.dspROM.Name = "dspROM";
			this.dspROM.Padding = new System.Windows.Forms.Padding(1);
			this.dspROM.Size = new System.Drawing.Size(45, 15);
			this.dspROM.TabIndex = 24;
			this.dspROM.Text = "ROM :";
			this.dspROM.TickSize = 0.01;
			this.dspROM.UpColor = System.Drawing.Color.LimeGreen;
			this.dspROM.Value = "";
			// 
			// CaptionBar
			// 
			this.CaptionBar.BackColor = System.Drawing.Color.Transparent;
			this.CaptionBar.Dock = System.Windows.Forms.DockStyle.Top;
			this.CaptionBar.Location = new System.Drawing.Point(0, 0);
			this.CaptionBar.Name = "CaptionBar";
			this.CaptionBar.Size = new System.Drawing.Size(300, 26);
			this.CaptionBar.TabIndex = 25;
			this.CaptionBar.TabStop = false;
			// 
			// dspROCVersion
			// 
			this.dspROCVersion.AutoSize = true;
			this.dspROCVersion.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspROCVersion.Location = new System.Drawing.Point(143, 55);
			this.dspROCVersion.Name = "dspROCVersion";
			this.dspROCVersion.Size = new System.Drawing.Size(0, 13);
			this.dspROCVersion.TabIndex = 26;
			// 
			// lblROCVersion
			// 
			this.lblROCVersion.AutoSize = true;
			this.lblROCVersion.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblROCVersion.Location = new System.Drawing.Point(237, 29);
			this.lblROCVersion.Name = "lblROCVersion";
			this.lblROCVersion.Size = new System.Drawing.Size(51, 13);
			this.lblROCVersion.TabIndex = 27;
			this.lblROCVersion.Text = "xx.xx.xx";
			// 
			// frmAbout
			// 
			this.AnimationType = FormEx.VistaWindowBase.AnimationTypes.Slide;
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.Caption = "About";
			this.ClientSize = new System.Drawing.Size(300, 260);
			this.Controls.Add(this.lblROCVersion);
			this.Controls.Add(this.dspROCVersion);
			this.Controls.Add(this.panelAbout);
			this.Controls.Add(this.picLogo);
			this.Controls.Add(this.CaptionBar);
			this.Name = "frmAbout";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.Text = "frmAbout";
			((System.ComponentModel.ISupportInitialize)(this.picLogo)).EndInit();
			this.panelAbout.ResumeLayout(false);
			this.panelAbout.PerformLayout();
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.PictureBox picLogo;
		private PanelEx.VistaPanel panelAbout;
		private LabelEx.LabelBase lblMDS;
		private LabelEx.LabelBase dspMDS;
		private LabelEx.LabelBase lblRDS;
		private LabelEx.LabelBase dspRDS;
		private LabelEx.LabelBase lblROM;
		private LabelEx.LabelBase dspROM;
		private FormEx.VistaWindowCaptionBar CaptionBar;
		private System.Windows.Forms.Label dspROCVersion;
		private System.Windows.Forms.Label lblROCVersion;
		private LabelEx.LabelBase lblMDSBackup;
	}
}