using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using FormEx;

namespace ROC
{
	[System.ComponentModel.DesignerCategory("Form")]
	public partial class frmEMail : VistaWindowBorderless
	{
		public frmEMail()
		{
			InitializeComponent();

			Initialize();
		}

		private void Initialize()
		{
			txtEMailServerAddress.Text = Configuration.EMail.Default.SMTPServer;
			txtFrom.Text = Configuration.EMail.Default.MailFrom;
			txtTitle.Text = Configuration.EMail.Default.MailSubject;
			txtWindowUserName.Text = Configuration.EMail.Default.SMTPUsername;

			rtbBody.Text = Configuration.EMail.Default.MailBody;

			UITimerTicked += new EventHandler(Form_UITimerTicked);
			StartTimer(Configuration.User.Default.UIUpdateRate);

			FormClosing += new FormClosingEventHandler(Form_FormClosing);
		}

		#region - Form Event -

		private void Form_UITimerTicked(object sender, EventArgs e)
		{
			if (!TopLevel)
			{
				Show();
				BringToFront();
			}
		}

		private void Form_FormClosing(object sender, EventArgs e)
		{
			GLOBAL.HWindows.CloseWindow(this);
		}

		#endregion

		#region - Ticket Event -

		private void cmdCancel_Click(object sender, EventArgs e)
		{
			Close();
		}

		private void txtWindowPassword_KeyDown(object sender, KeyEventArgs e)
		{
			switch (e.KeyCode)
			{
				case Keys.Enter:
					cmdSend_Click(null, EventArgs.Empty);
					break;
			}
		}

		private void cmdSend_Click(object sender, EventArgs e)
		{
			Save();
			GLOBAL.HMail.EmailLogFiles(
				txtFrom.Text, 
				txtTitle.Text, 
				rtbBody.Text, 
				txtEMailServerAddress.Text, 
				txtWindowUserName.Text, 
				txtWindowPassword.Text,
				true);
			Close();
		}

		private void cmdSendWithoutLog_Click(object sender, EventArgs e)
		{
			Save();
			GLOBAL.HMail.EmailLogFiles(
				txtFrom.Text,
				txtTitle.Text,
				rtbBody.Text,
				txtEMailServerAddress.Text,
				txtWindowUserName.Text,
				txtWindowPassword.Text,
				false);
			Close();
		}

		private void txtWindowUserName_TextChanged(object sender, EventArgs e)
		{
			//SendEnable();
		}

		private void txtWindowPassword_TextChanged(object sender, EventArgs e)
		{
			//SendEnable();
		}

		#endregion

		private void Save()
		{
			if (Configuration.EMail.Default.SMTPServer != txtEMailServerAddress.Text)
			{
				if (MessageBox.Show("Change EMail Server?", "Confirmation", MessageBoxButtons.YesNo) == DialogResult.Yes)
				{
					Configuration.EMail.Default.SMTPServer = txtEMailServerAddress.Text;
				}
			}
			if (!txtFrom.Text.Contains("@"))
			{
				txtFrom.Text = "";
			}
			Configuration.EMail.Default.MailFrom = txtFrom.Text;
			Configuration.EMail.Default.MailSubject = txtTitle.Text;
			Configuration.EMail.Default.SMTPUsername = txtWindowUserName.Text;
			Configuration.EMail.Default.Save();
		}

		private void SendEnable()
		{
			if (txtWindowUserName.Text == "" || txtWindowPassword.Text == "")
			{
				cmdSend.Enabled = false;
				cmdSendWithoutLog.Enabled = false;
			}
			else
			{
				cmdSend.Enabled = true;
				cmdSendWithoutLog.Enabled = true;
			}
		}
	}
}