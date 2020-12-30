namespace ROC
{
	partial class itemBaseDateTimePicker
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
			this.dateTimePicker = new System.Windows.Forms.DateTimePicker();
			this.cmdOk = new ButtonEx.VistaButton();
			this.cmdExit = new ButtonEx.VistaButton();
			this.SuspendLayout();
			// 
			// dateTimePicker
			// 
			this.dateTimePicker.CustomFormat = "HH:mm:ss";
			this.dateTimePicker.Dock = System.Windows.Forms.DockStyle.Left;
			this.dateTimePicker.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dateTimePicker.Format = System.Windows.Forms.DateTimePickerFormat.Custom;
			this.dateTimePicker.Location = new System.Drawing.Point(0, 0);
			this.dateTimePicker.Name = "dateTimePicker";
			this.dateTimePicker.ShowUpDown = true;
			this.dateTimePicker.Size = new System.Drawing.Size(84, 22);
			this.dateTimePicker.TabIndex = 0;
			// 
			// cmdOk
			// 
			this.cmdOk.BackColor = System.Drawing.Color.Black;
			this.cmdOk.Dock = System.Windows.Forms.DockStyle.Right;
			this.cmdOk.Location = new System.Drawing.Point(86, 0);
			this.cmdOk.Name = "cmdOk";
			this.cmdOk.Size = new System.Drawing.Size(29, 22);
			this.cmdOk.TabIndex = 1;
			this.cmdOk.Text = "Ok";
			this.cmdOk.Click += new System.EventHandler(this.cmdOk_Click);
			// 
			// cmdExit
			// 
			this.cmdExit.BackColor = System.Drawing.Color.Black;
			this.cmdExit.Dock = System.Windows.Forms.DockStyle.Right;
			this.cmdExit.Location = new System.Drawing.Point(115, 0);
			this.cmdExit.Name = "cmdExit";
			this.cmdExit.Size = new System.Drawing.Size(37, 22);
			this.cmdExit.TabIndex = 2;
			this.cmdExit.Text = "Exit";
			this.cmdExit.Click += new System.EventHandler(this.cmdExit_Click);
			// 
			// itemBaseDateTimePicker
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.Controls.Add(this.cmdOk);
			this.Controls.Add(this.cmdExit);
			this.Controls.Add(this.dateTimePicker);
			this.Name = "itemBaseDateTimePicker";
			this.Size = new System.Drawing.Size(152, 22);
			this.ResumeLayout(false);

		}

		#endregion

		private ButtonEx.VistaButton cmdOk;
		private ButtonEx.VistaButton cmdExit;
		internal System.Windows.Forms.DateTimePicker dateTimePicker;
	}
}
