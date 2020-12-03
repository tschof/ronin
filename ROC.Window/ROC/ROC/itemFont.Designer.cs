namespace ROC
{
	partial class itemFont
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
			this.lblFont = new System.Windows.Forms.Label();
			this.lblName = new System.Windows.Forms.Label();
			this.SuspendLayout();
			// 
			// lblFont
			// 
			this.lblFont.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.lblFont.Dock = System.Windows.Forms.DockStyle.Fill;
			this.lblFont.Location = new System.Drawing.Point(3, 3);
			this.lblFont.Name = "lblFont";
			this.lblFont.Size = new System.Drawing.Size(140, 62);
			this.lblFont.TabIndex = 0;
			this.lblFont.Text = "Aa Bb Cc Dd";
			this.lblFont.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			this.lblFont.Click += new System.EventHandler(this.lblFont_Click);
			// 
			// lblName
			// 
			this.lblName.AutoEllipsis = true;
			this.lblName.BackColor = System.Drawing.Color.White;
			this.lblName.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.lblName.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.lblName.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblName.ForeColor = System.Drawing.Color.SteelBlue;
			this.lblName.Location = new System.Drawing.Point(3, 65);
			this.lblName.Name = "lblName";
			this.lblName.Size = new System.Drawing.Size(140, 19);
			this.lblName.TabIndex = 1;
			this.lblName.Text = "Name";
			this.lblName.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			// 
			// itemFont
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.Controls.Add(this.lblFont);
			this.Controls.Add(this.lblName);
			this.Name = "itemFont";
			this.Padding = new System.Windows.Forms.Padding(3);
			this.Size = new System.Drawing.Size(146, 87);
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.Label lblFont;
		private System.Windows.Forms.Label lblName;
	}
}
