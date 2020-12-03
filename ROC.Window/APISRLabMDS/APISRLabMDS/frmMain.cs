using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace APISRLabMDS
{
	public partial class frmMain : Form
	{
		public frmMain()
		{
			InitializeComponent();
		}

		private void cmdCheckEnvironments_Click(object sender, EventArgs e)
		{
			GLOBAL.HSRLab.Status = HelperSRLab.StatusTypes.NotConnected;
			GLOBAL.HSRLab.CheckEnvironments();
		}
	}
}