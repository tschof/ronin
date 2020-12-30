namespace ROC
{
	partial class frmClock
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
			this.CaptionBar = new FormEx.VistaWindowCaptionBar();
			this.digitalClock = new ClockEx.DigitalClockCtrl();
			this.SuspendLayout();
			// 
			// CaptionBar
			// 
			this.CaptionBar.BackColor = System.Drawing.Color.Transparent;
			this.CaptionBar.ConfirmBeforeClose = true;
			this.CaptionBar.Dock = System.Windows.Forms.DockStyle.Top;
			this.CaptionBar.Location = new System.Drawing.Point(0, 0);
			this.CaptionBar.Name = "CaptionBar";
			this.CaptionBar.ShowIcon = false;
			this.CaptionBar.ShowMaximize = false;
			this.CaptionBar.ShowMinimize = false;
			this.CaptionBar.ShowShowAll = true;
			this.CaptionBar.Size = new System.Drawing.Size(354, 26);
			this.CaptionBar.TabIndex = 1;
			// 
			// digitalClock
			// 
			this.digitalClock.BackColor = System.Drawing.Color.Black;
			this.digitalClock.ClockDisplayFormat = ClockEx.ClockFormat.TwelveHourFormat;
			this.digitalClock.DigitsColor = System.Drawing.Color.Red;
			this.digitalClock.Dock = System.Windows.Forms.DockStyle.Fill;
			this.digitalClock.Location = new System.Drawing.Point(0, 26);
			this.digitalClock.Name = "digitalClock";
			this.digitalClock.Size = new System.Drawing.Size(354, 71);
			this.digitalClock.TabIndex = 0;
			this.digitalClock.TabStop = false;
			this.digitalClock.TimeZoneDisplayFormat = ClockEx.TimeZoneFormat.Local;
			this.digitalClock.MouseClick += new System.Windows.Forms.MouseEventHandler(this.digitalClock_MouseClick);
			// 
			// frmClock
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.Caption = "Clock";
			this.ClientSize = new System.Drawing.Size(354, 97);
			this.Controls.Add(this.digitalClock);
			this.Controls.Add(this.CaptionBar);
			this.MaximumSize = new System.Drawing.Size(362, 105);
			this.MinimumSize = new System.Drawing.Size(181, 55);
			this.Name = "frmClock";
			this.ShowInTaskbar = false;
			this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
			this.ResumeLayout(false);

		}

		#endregion

		private FormEx.VistaWindowCaptionBar CaptionBar;
		private ClockEx.DigitalClockCtrl digitalClock;
	}
}