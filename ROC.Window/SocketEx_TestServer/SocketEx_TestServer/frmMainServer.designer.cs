namespace SocketEx_TestServer
{
	partial class frmMainServer
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
			this.cmdStart = new System.Windows.Forms.Button();
			this.cmdStop = new System.Windows.Forms.Button();
			this.lblClientsConnected = new System.Windows.Forms.Label();
			this.dspClientsConnected = new System.Windows.Forms.Label();
			this.txtStatus = new System.Windows.Forms.TextBox();
			this.rtbMessages = new System.Windows.Forms.RichTextBox();
			this.SuspendLayout();
			// 
			// cmdStart
			// 
			this.cmdStart.Location = new System.Drawing.Point(12, 12);
			this.cmdStart.Name = "cmdStart";
			this.cmdStart.Size = new System.Drawing.Size(75, 23);
			this.cmdStart.TabIndex = 0;
			this.cmdStart.Text = "Start";
			this.cmdStart.UseVisualStyleBackColor = true;
			this.cmdStart.Click += new System.EventHandler(this.cmdStart_Click);
			// 
			// cmdStop
			// 
			this.cmdStop.Enabled = false;
			this.cmdStop.Location = new System.Drawing.Point(93, 12);
			this.cmdStop.Name = "cmdStop";
			this.cmdStop.Size = new System.Drawing.Size(75, 23);
			this.cmdStop.TabIndex = 1;
			this.cmdStop.Text = "Stop";
			this.cmdStop.UseVisualStyleBackColor = true;
			this.cmdStop.Click += new System.EventHandler(this.cmdStop_Click);
			// 
			// lblClientsConnected
			// 
			this.lblClientsConnected.AutoSize = true;
			this.lblClientsConnected.Location = new System.Drawing.Point(9, 238);
			this.lblClientsConnected.Name = "lblClientsConnected";
			this.lblClientsConnected.Size = new System.Drawing.Size(99, 13);
			this.lblClientsConnected.TabIndex = 2;
			this.lblClientsConnected.Text = "Clients Connected: ";
			// 
			// dspClientsConnected
			// 
			this.dspClientsConnected.AutoSize = true;
			this.dspClientsConnected.Location = new System.Drawing.Point(114, 238);
			this.dspClientsConnected.Name = "dspClientsConnected";
			this.dspClientsConnected.Size = new System.Drawing.Size(13, 13);
			this.dspClientsConnected.TabIndex = 3;
			this.dspClientsConnected.Text = "0";
			// 
			// txtStatus
			// 
			this.txtStatus.Location = new System.Drawing.Point(12, 210);
			this.txtStatus.Name = "txtStatus";
			this.txtStatus.Size = new System.Drawing.Size(372, 20);
			this.txtStatus.TabIndex = 5;
			// 
			// rtbMessages
			// 
			this.rtbMessages.Location = new System.Drawing.Point(12, 41);
			this.rtbMessages.Name = "rtbMessages";
			this.rtbMessages.Size = new System.Drawing.Size(372, 163);
			this.rtbMessages.TabIndex = 6;
			this.rtbMessages.Text = "";
			// 
			// frmMain
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(396, 260);
			this.Controls.Add(this.rtbMessages);
			this.Controls.Add(this.txtStatus);
			this.Controls.Add(this.dspClientsConnected);
			this.Controls.Add(this.lblClientsConnected);
			this.Controls.Add(this.cmdStop);
			this.Controls.Add(this.cmdStart);
			this.Name = "frmMain";
			this.Text = "Server";
			this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.frmMain_FormClosing);
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.Button cmdStart;
		private System.Windows.Forms.Button cmdStop;
		private System.Windows.Forms.Label lblClientsConnected;
		private System.Windows.Forms.Label dspClientsConnected;
		private System.Windows.Forms.TextBox txtStatus;
		private System.Windows.Forms.RichTextBox rtbMessages;
	}
}

