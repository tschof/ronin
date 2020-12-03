namespace DGVTest
{
	partial class Form1
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
			this.dgvEx1 = new GridEx.DGVEx();
			((System.ComponentModel.ISupportInitialize)(this.dgvEx1)).BeginInit();
			this.SuspendLayout();
			// 
			// dgvEx1
			// 
			this.dgvEx1.AllowUserToOrderColumns = true;
			this.dgvEx1.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
			this.dgvEx1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.dgvEx1.Location = new System.Drawing.Point(0, 0);
			this.dgvEx1.Name = "dgvEx1";
			this.dgvEx1.Size = new System.Drawing.Size(653, 273);
			this.dgvEx1.TabIndex = 0;
			// 
			// Form1
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(653, 273);
			this.Controls.Add(this.dgvEx1);
			this.Name = "Form1";
			this.Text = "Form1";
			((System.ComponentModel.ISupportInitialize)(this.dgvEx1)).EndInit();
			this.ResumeLayout(false);

		}

		#endregion

		private GridEx.DGVEx dgvEx1;
	}
}

