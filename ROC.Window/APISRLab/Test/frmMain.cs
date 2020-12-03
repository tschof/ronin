using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace Test
{
	public partial class frmMain : Form
	{
		public frmMain()
		{
			InitializeComponent();
		}

		protected override void OnLoad(EventArgs e)
		{
			base.OnLoad(e);
			dgvTestView.DataSource = bindingSource1;
			bindingSource1.DataSource = GLOBAL.SRLab.Datas;
		}

		protected override void OnClosing(CancelEventArgs e)
		{
			GLOBAL.SRLab.SRLabShutdown();
			base.OnClosing(e);
		}

		private void timer1_Tick(object sender, EventArgs e)
		{
		}

		private void button1_Click(object sender, EventArgs e)
		{
			GLOBAL.SRLab.Setup();
		}
	}
}