namespace Test
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
			this.numericUpDownVista1 = new NumericUpDownEx.NumericUpDownVista();
			this.SuspendLayout();
			// 
			// numericUpDownVista1
			// 
			this.numericUpDownVista1.BackColor = System.Drawing.SystemColors.Control;
			this.numericUpDownVista1.Location = new System.Drawing.Point(12, 12);
			this.numericUpDownVista1.MinimumSize = new System.Drawing.Size(19, 19);
			this.numericUpDownVista1.Name = "numericUpDownVista1";
			this.numericUpDownVista1.Padding = new System.Windows.Forms.Padding(0, 0, 0, 3);
			this.numericUpDownVista1.Size = new System.Drawing.Size(44, 19);
			this.numericUpDownVista1.TabIndex = 0;
			// 
			// Form1
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(292, 273);
			this.Controls.Add(this.numericUpDownVista1);
			this.Name = "Form1";
			this.Text = "Form1";
			this.ResumeLayout(false);

		}

		#endregion

		private NumericUpDownEx.NumericUpDownVista numericUpDownVista1;


	}
}

