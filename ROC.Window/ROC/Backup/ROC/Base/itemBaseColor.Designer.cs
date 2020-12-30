namespace ROC
{
	partial class itemBaseColor
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
			this.lblColor = new System.Windows.Forms.Label();
			this.SuspendLayout();
			// 
			// lblName
			// 
			this.lblName.AutoEllipsis = true;
			this.lblName.BackColor = System.Drawing.Color.White;
			this.lblName.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.lblName.Dock = System.Windows.Forms.DockStyle.Fill;
			this.lblName.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblName.ForeColor = System.Drawing.Color.SteelBlue;
			this.lblName.Location = new System.Drawing.Point(34, 2);
			this.lblName.Name = "lblName";
			this.lblName.Size = new System.Drawing.Size(140, 19);
			this.lblName.TabIndex = 0;
			this.lblName.Text = "Name";
			this.lblName.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			// 
			// lblColor
			// 
			this.lblColor.BackColor = System.Drawing.Color.Black;
			this.lblColor.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.lblColor.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblColor.Location = new System.Drawing.Point(2, 2);
			this.lblColor.Name = "lblColor";
			this.lblColor.Size = new System.Drawing.Size(32, 19);
			this.lblColor.TabIndex = 1;
			this.lblColor.Click += new System.EventHandler(this.lblColor_Click);
			// 
			// itemColor
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.BackColor = System.Drawing.Color.Transparent;
			this.Controls.Add(this.lblName);
			this.Controls.Add(this.lblColor);
			this.ForeColor = System.Drawing.Color.White;
			this.Name = "itemColor";
			this.Padding = new System.Windows.Forms.Padding(2);
			this.Size = new System.Drawing.Size(176, 23);
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.Label lblName;
		private System.Windows.Forms.Label lblColor;
	}
}
