namespace SocketEx_TestClient
{
	partial class frmMainClient
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
			this.cmdConnect = new System.Windows.Forms.Button();
			this.cmdDisconnect = new System.Windows.Forms.Button();
			this.cmdSend = new System.Windows.Forms.Button();
			this.txtSend = new System.Windows.Forms.TextBox();
			this.txtStatus = new System.Windows.Forms.TextBox();
			this.cmdAutoSend = new System.Windows.Forms.Button();
			this.cmdStopAutoSend = new System.Windows.Forms.Button();
			this.rtbMessages = new System.Windows.Forms.RichTextBox();
			this.SuspendLayout();
			// 
			// cmdConnect
			// 
			this.cmdConnect.Location = new System.Drawing.Point(12, 12);
			this.cmdConnect.Name = "cmdConnect";
			this.cmdConnect.Size = new System.Drawing.Size(75, 23);
			this.cmdConnect.TabIndex = 0;
			this.cmdConnect.Text = "Connect";
			this.cmdConnect.UseVisualStyleBackColor = true;
			this.cmdConnect.Click += new System.EventHandler(this.cmdConnect_Click);
			// 
			// cmdDisconnect
			// 
			this.cmdDisconnect.Location = new System.Drawing.Point(93, 12);
			this.cmdDisconnect.Name = "cmdDisconnect";
			this.cmdDisconnect.Size = new System.Drawing.Size(75, 23);
			this.cmdDisconnect.TabIndex = 1;
			this.cmdDisconnect.Text = "Disconnect";
			this.cmdDisconnect.UseVisualStyleBackColor = true;
			this.cmdDisconnect.Click += new System.EventHandler(this.cmdDisconnect_Click);
			// 
			// cmdSend
			// 
			this.cmdSend.Location = new System.Drawing.Point(13, 240);
			this.cmdSend.Name = "cmdSend";
			this.cmdSend.Size = new System.Drawing.Size(75, 20);
			this.cmdSend.TabIndex = 2;
			this.cmdSend.Text = "Send";
			this.cmdSend.UseVisualStyleBackColor = true;
			this.cmdSend.Click += new System.EventHandler(this.cmdSend_Click);
			// 
			// txtSend
			// 
			this.txtSend.Location = new System.Drawing.Point(94, 240);
			this.txtSend.Name = "txtSend";
			this.txtSend.Size = new System.Drawing.Size(290, 20);
			this.txtSend.TabIndex = 3;
			// 
			// txtStatus
			// 
			this.txtStatus.Location = new System.Drawing.Point(12, 214);
			this.txtStatus.Name = "txtStatus";
			this.txtStatus.Size = new System.Drawing.Size(372, 20);
			this.txtStatus.TabIndex = 5;
			// 
			// cmdAutoSend
			// 
			this.cmdAutoSend.Location = new System.Drawing.Point(228, 12);
			this.cmdAutoSend.Name = "cmdAutoSend";
			this.cmdAutoSend.Size = new System.Drawing.Size(75, 23);
			this.cmdAutoSend.TabIndex = 6;
			this.cmdAutoSend.Text = "Auto Send";
			this.cmdAutoSend.UseVisualStyleBackColor = true;
			this.cmdAutoSend.Click += new System.EventHandler(this.cmdAutoSend_Click);
			// 
			// cmdStopAutoSend
			// 
			this.cmdStopAutoSend.Location = new System.Drawing.Point(309, 12);
			this.cmdStopAutoSend.Name = "cmdStopAutoSend";
			this.cmdStopAutoSend.Size = new System.Drawing.Size(75, 23);
			this.cmdStopAutoSend.TabIndex = 7;
			this.cmdStopAutoSend.Text = "Stop";
			this.cmdStopAutoSend.UseVisualStyleBackColor = true;
			this.cmdStopAutoSend.Click += new System.EventHandler(this.cmdStopAutoSend_Click);
			// 
			// rtbMessages
			// 
			this.rtbMessages.Location = new System.Drawing.Point(12, 41);
			this.rtbMessages.Name = "rtbMessages";
			this.rtbMessages.Size = new System.Drawing.Size(372, 163);
			this.rtbMessages.TabIndex = 8;
			this.rtbMessages.Text = "";
			// 
			// frmMainClient
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(395, 270);
			this.Controls.Add(this.rtbMessages);
			this.Controls.Add(this.cmdStopAutoSend);
			this.Controls.Add(this.cmdAutoSend);
			this.Controls.Add(this.txtStatus);
			this.Controls.Add(this.txtSend);
			this.Controls.Add(this.cmdSend);
			this.Controls.Add(this.cmdDisconnect);
			this.Controls.Add(this.cmdConnect);
			this.Name = "frmMainClient";
			this.Text = "Client";
			this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.frmMain_FormClosing);
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.Button cmdConnect;
		private System.Windows.Forms.Button cmdDisconnect;
		private System.Windows.Forms.Button cmdSend;
		private System.Windows.Forms.TextBox txtSend;
		private System.Windows.Forms.TextBox txtStatus;
		private System.Windows.Forms.Button cmdAutoSend;
		private System.Windows.Forms.Button cmdStopAutoSend;
		private System.Windows.Forms.RichTextBox rtbMessages;
	}
}

