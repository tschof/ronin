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
	public partial class frmAbout : VistaWindowBorderless
	{
		public frmAbout()
		{
			InitializeComponent();

			Initialize();
		}

		private void Initialize()
		{
			lblROM.Text = Configuration.ROM.Default.IP + " : " + Configuration.ROM.Default.Port;
			lblROM.SetToolTip(lblROM.Text);

			lblRDS.Text = GLOBAL.HRDS.Address;
			lblRDS.SetToolTip(lblRDS.Text);

			lblMDS.Text = Configuration.MDS.Default.IP + " : " + Configuration.MDS.Default.Port;
			lblMDS.SetToolTip(lblMDS.Text);

			lblMDSBackup.Text = Configuration.MDS.Default.IPBackup + " : " + Configuration.MDS.Default.PortBackup;
			lblMDSBackup.SetToolTip(lblMDSBackup.Text);

			lblROCVersion.Text = "v" + Application.ProductVersion;

			FormClosing += new FormClosingEventHandler(frmAbout_FormClosing);
		}

		private void frmAbout_FormClosing(object sender, EventArgs e)
		{
			GLOBAL.HWindows.CloseWindow(this);
		}
	}
}