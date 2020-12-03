namespace ROC
{
	partial class menuBaseProcessing
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
			this.panelBack = new PanelEx.VistaPanel();
			this.picProcessing = new System.Windows.Forms.PictureBox();
			this.lblProcessing = new LabelEx.LabelVista();
			this.panelBack.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.picProcessing)).BeginInit();
			this.SuspendLayout();
			// 
			// panelBack
			// 
			this.panelBack.ApplyShine = true;
			this.panelBack.BackColor = System.Drawing.Color.OliveDrab;
			this.panelBack.Controls.Add(this.picProcessing);
			this.panelBack.Controls.Add(this.lblProcessing);
			this.panelBack.Dock = System.Windows.Forms.DockStyle.Fill;
			this.panelBack.Location = new System.Drawing.Point(0, 0);
			this.panelBack.Name = "panelBack";
			this.panelBack.Padding = new System.Windows.Forms.Padding(2);
			this.panelBack.Size = new System.Drawing.Size(51, 51);
			this.panelBack.TabIndex = 0;
			// 
			// picProcessing
			// 
			this.picProcessing.Dock = System.Windows.Forms.DockStyle.Fill;
			this.picProcessing.Image = global::ROC.Properties.Resources.PROCESS_PROCESSING;
			this.picProcessing.Location = new System.Drawing.Point(2, 2);
			this.picProcessing.Name = "picProcessing";
			this.picProcessing.Size = new System.Drawing.Size(47, 34);
			this.picProcessing.SizeMode = System.Windows.Forms.PictureBoxSizeMode.CenterImage;
			this.picProcessing.TabIndex = 4;
			this.picProcessing.TabStop = false;
			// 
			// lblProcessing
			// 
			this.lblProcessing.DefaultText = "000";
			this.lblProcessing.DisplayFactor = 1;
			this.lblProcessing.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.lblProcessing.Location = new System.Drawing.Point(2, 36);
			this.lblProcessing.Name = "lblProcessing";
			this.lblProcessing.Size = new System.Drawing.Size(47, 13);
			this.lblProcessing.TabIndex = 5;
			this.lblProcessing.Text = "000";
			this.lblProcessing.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			this.lblProcessing.TickSize = 0.01;
			this.lblProcessing.Value = "";
			// 
			// menuProcessing
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.BackColor = System.Drawing.Color.Transparent;
			this.Controls.Add(this.panelBack);
			this.MaximumSize = new System.Drawing.Size(51, 51);
			this.MinimumSize = new System.Drawing.Size(51, 51);
			this.Name = "menuProcessing";
			this.Size = new System.Drawing.Size(51, 51);
			this.panelBack.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.picProcessing)).EndInit();
			this.ResumeLayout(false);

		}

		#endregion

		private PanelEx.VistaPanel panelBack;
		private System.Windows.Forms.PictureBox picProcessing;
		private LabelEx.LabelVista lblProcessing;

	}
}
