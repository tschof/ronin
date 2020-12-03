namespace NumericUpDownEx
{
	partial class NumericUpDownVista
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
			this.numBase = new System.Windows.Forms.NumericUpDown();
			this.dspBase = new System.Windows.Forms.TextBox();
			this.cmdUp = new ButtonEx.VistaButton();
			this.cmdDown = new ButtonEx.VistaButton();
			((System.ComponentModel.ISupportInitialize)(this.numBase)).BeginInit();
			this.SuspendLayout();
			// 
			// numBase
			// 
			this.numBase.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.numBase.Location = new System.Drawing.Point(0, 0);
			this.numBase.Name = "numBase";
			this.numBase.Size = new System.Drawing.Size(150, 20);
			this.numBase.TabIndex = 2;
			this.numBase.Visible = false;
			// 
			// dspBase
			// 
			this.dspBase.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspBase.Location = new System.Drawing.Point(0, 0);
			this.dspBase.Name = "dspBase";
			this.dspBase.Size = new System.Drawing.Size(132, 20);
			this.dspBase.TabIndex = 3;
			// 
			// cmdUp
			// 
			this.cmdUp.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.cmdUp.BackColor = System.Drawing.Color.Black;
			this.cmdUp.Location = new System.Drawing.Point(129, 0);
			this.cmdUp.Name = "cmdUp";
			this.cmdUp.Size = new System.Drawing.Size(18, 10);
			this.cmdUp.TabIndex = 4;
			// 
			// cmdDown
			// 
			this.cmdDown.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.cmdDown.BackColor = System.Drawing.Color.Black;
			this.cmdDown.Location = new System.Drawing.Point(129, 10);
			this.cmdDown.Name = "cmdDown";
			this.cmdDown.Size = new System.Drawing.Size(18, 10);
			this.cmdDown.TabIndex = 5;
			// 
			// NumericUpDownVista
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.BackColor = System.Drawing.SystemColors.Window;
			this.Controls.Add(this.cmdDown);
			this.Controls.Add(this.cmdUp);
			this.Controls.Add(this.dspBase);
			this.Controls.Add(this.numBase);
			this.DoubleBuffered = true;
			this.MinimumSize = new System.Drawing.Size(19, 19);
			this.Name = "NumericUpDownVista";
			this.Padding = new System.Windows.Forms.Padding(0, 0, 0, 3);
			this.Size = new System.Drawing.Size(148, 20);
			((System.ComponentModel.ISupportInitialize)(this.numBase)).EndInit();
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.TextBox dspBase;
		private System.Windows.Forms.NumericUpDown numBase;
		private ButtonEx.VistaButton cmdUp;
		private ButtonEx.VistaButton cmdDown;
	}
}
