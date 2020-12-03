using System;
using System.Collections.Generic;
using System.Text;
using EMailEx;

namespace APISRLabMDS
{
	public class HelperEMail
	{
		EMail _mail = new EMail();

		public void EmailLBMException(string msg)
		{
			try
			{
				_mail.MailFrom = Configuration.MDS.Default.IP + "@ronin-capital.com";

				_mail.SendAsync = false;
				_mail.MailTo = Configuration.EMail.Default.MailTo;
				_mail.MailSubject = Configuration.EMail.Default.MailSubject;
				_mail.MailBody = msg;

				_mail.SMTPServer = Configuration.EMail.Default.SMTPServer;
				_mail.SMTPUsername = Configuration.EMail.Default.SMTPUsername;
				_mail.SMTPPassword = Configuration.EMail.Default.SMTPPassword;
				_mail.Send();
			}
			catch (Exception ex)
			{
				GLOBAL.AddToExecption(ex);
			}
		}
	}
}
