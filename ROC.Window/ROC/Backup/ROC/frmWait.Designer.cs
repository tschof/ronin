namespace ROC
{
	partial class frmWait
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
			this.vistaPanel1 = new PanelEx.VistaPanel();
			this.lblCaption = new System.Windows.Forms.Label();
			this.pictureBoxWait = new System.Windows.Forms.PictureBox();
			this.vistaPanel1.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.pictureBoxWait)).BeginInit();
			this.SuspendLayout();
			// 
			// vistaPanel1
			// 
			this.vistaPanel1.BackColor = System.Drawing.Color.Black;
			this.vistaPanel1.Controls.Add(this.lblCaption);
			this.vistaPanel1.Controls.Add(this.pictureBoxWait);
			this.vistaPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.vistaPanel1.Location = new System.Drawing.Point(0, 0);
			this.vistaPanel1.Name = "vistaPanel1";
			this.vistaPanel1.Padding = new System.Windows.Forms.Padding(5);
			this.vistaPanel1.Size = new System.Drawing.Size(265, 74);
			this.vistaPanel1.TabIndex = 1;
			// 
			// lblCaption
			// 
			this.lblCaption.AutoEllipsis = true;
			this.lblCaption.BackColor = System.Drawing.Color.Transparent;
			this.lblCaption.Dock = System.Windows.Forms.DockStyle.Fill;
			this.lblCaption.Location = new System.Drawing.Point(5, 5);
			this.lblCaption.Name = "lblCaption";
			this.lblCaption.Size = new System.Drawing.Size(255, 47);
			this.lblCaption.TabIndex = 2;
			this.lblCaption.Text = "Caption";
			// 
			// pictureBoxWait
			// 
			this.pictureBoxWait.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.pictureBoxWait.Image = global::ROC.Properties.Resources.PROCESS_BLUEANDWHITE;
			this.pictureBoxWait.Location = new System.Drawing.Point(5, 52);
			this.pictureBoxWait.Name = "pictureBoxWait";
			this.pictureBoxWait.Size = new System.Drawing.Size(255, 17);
			this.pictureBoxWait.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
			this.pictureBoxWait.TabIndex = 3;
			this.pictureBoxWait.TabStop = false;
			// 
			// frmWait
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(265, 74);
			this.Controls.Add(this.vistaPanel1);
			this.Name = "frmWait";
			this.ROCWindowType = FormEx.VistaWindowBase.ROCWindowTypes.Wait;
			this.ShowInTaskbar = false;
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.Text = "frmWait";
			this.TopMost = true;
			this.vistaPanel1.ResumeLayout(false);
			this.vistaPanel1.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.pictureBoxWait)).EndInit();
			this.ResumeLayout(false);

		}

		#endregion

		private PanelEx.VistaPanel vistaPanel1;
		private System.Windows.Forms.Label lblCaption;
		private System.Windows.Forms.PictureBox pictureBoxWait;
	}
}